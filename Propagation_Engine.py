import math
import tkinter as tk

class PropagationEngine:
    def __init__(self, app):
        self.app = app
        self.c = 299.792  # Lichtgeschwindigkeit in km/ms
        self.last_active_stations = set()

    def get_sun_alt(self, lat, lon, n_day, time_h):
        decl = 23.45 * math.sin(math.radians(360 / 365 * (n_day - 81)))
        hour_angle = (time_h - 12) * 15 + lon
        return math.degrees(math.asin(math.sin(math.radians(lat)) * math.sin(math.radians(decl)) +
                                     math.cos(math.radians(lat)) * math.cos(math.radians(decl)) * math.cos(math.radians(hour_angle))))

    def run_simulation(self, send_udp=False):
        """
        WELLENAUSBREITUNGS-MODELL (MF/LF):
        
        1. BODENWELLE (Groundwave) nach ITU-R P.368-10:
           Berechnung der Feldstärke über Grund (σ = 5mS/m, Mittelwert).
           Basis: 107 dBµV/m @ 1km/1kW. Dämpfung erfolgt frequenzabhängig (f/180).
        
        2. RAUMWELLE (Skywave) nach ITU-R P.1147-2:
           Reflexion an der Ionosphäre (E-Schicht @100km bis F-Schicht @250km).
           Pfadverlust basierend auf der tatsächlichen Wegstrecke (Slant Range).
           
        3. D-SCHICHT ABSORPTION (Appleton-Hartree Vereinfachung):
           Berücksichtigt die solare Absorption in der D-Schicht.
           Dämpfungsfaktor steigt linear mit dem Sonnenstand (Elevation) 
           und quadratisch mit sinkender Frequenz (1/f²).
           
        4. LAUFZEIT-DIFFERENZIERUNG:
           Berechnung der Gruppenlaufzeit (Group Delay) für Boden- und Raumwelle,
           wichtig für die Simulation von Nah-Schwund (Fading/Phasing).
        """
        if not hasattr(self.app, 'list_max') or not self.app.list_max.winfo_exists(): return
        
        try:
            from datetime import datetime
            # --- Zeit- & Datums-Parsing ---
            date_raw = self.app.ent_date.get().strip().rstrip('.')
            if len(date_raw.split('.')) == 2: date_raw += ".1975" # Standardjahr für n_day Kalkulation
            dt = datetime.strptime(date_raw, "%d.%m.%Y")
            n_day, time_h = dt.timetuple().tm_yday, float(self.app.time_slider.get())
            
            self.app.list_max.delete(0, tk.END)
            self.app.list_now.delete(0, tk.END)
            
            # --- RX Standort & Sonnenstand --- Chapman-Schicht-Modell (vereinfacht).
            u_lat, u_lon = self.app.receiver_coords
            alt_rx = self.get_sun_alt(u_lat, u_lon, n_day, time_h)
            self.app.lbl_sun.config(text=f"{self.app.LANGUAGES[self.app.cur_lang]['sun']} {alt_rx:.2f}°", 
                               fg="blue" if alt_rx < -0.83 else "orange")

            # --- DYNAMISCHE REFLEXIONSHÖHE (Virtual Height h_ion) ---
            # Modelliert den Übergang von E-Schicht (Tag/Sommer) zu F-Schicht (Nacht/Winter)
            seasonal_factor = (math.cos(math.radians(360 / 365 * (n_day - 172))) + 1) / 2 # 0=Sommer, 1=Winter
            night_factor = 1.0 if alt_rx < -12 else max(0, -alt_rx / 12) if alt_rx < 0 else 0
            h_ion = 100 + (seasonal_factor * 80) + (night_factor * 70) 

        except Exception as e: 
            print(f"Sim Error: {e}")
            return

        res_list = []; sens = self.app.sens_slider.get()
        
        for marker in self.app.map_widget.canvas_marker_list:
            d = getattr(marker, 'data', None)
            if isinstance(d, dict) and "freq" in d:
                m_lat, m_lon = marker.position
                
                # --- DISTANZBERECHNUNG ---
                # Vereinfachte Projektion für lokale Radien (km)
                dist = math.sqrt((u_lat - m_lat)**2 + (math.cos(math.radians(u_lat)) * (u_lon - m_lon))**2) * 111.32
                f, p = float(d["freq"]), float(d["kw"])
                
                # --- 1. BODENWELLE ---
                # Basis: 107 dBuV/m @ 1km/1kW. Dämpfung: Freiraum + f-abhängige Bodenabsorption
                fs_g = 107 + 10*math.log10(p) - (20*math.log10(dist+1) + (f/180)*(dist/25))
                
                # --- 2. RAUMWELLE ---
                # Berechnung des tatsächlichen Signalwegs über die Ionosphäre (Dreiecksgeometrie)
                path_rw = 2 * math.sqrt((dist/2)**2 + h_ion**2)
                sky_base = 95 + 10*math.log10(p) - (20*math.log10(path_rw) + 15) if dist > 50 else -100
                
                # D-SCHICHT ABSORPTION
                # Bestimmt durch den höchsten Sonnenstand auf dem Pfad (RX vs TX)
                eff_alt = max(alt_rx, self.get_sun_alt(m_lat, m_lon, n_day, time_h))
                sun_f = max(0, (eff_alt + 12) / 24) if eff_alt > -12 else 0

                # Absorption steigt quadratisch mit sinkender Frequenz (1/f^2)
                sky_loss = sun_f * 85 * ((1000 / f) ** 2)
                cur_sky = sky_base - sky_loss

                # --- ANTENNEN-RICHTCHARAKTERISTIK ---
                # Berechnet die Dämpfung basierend auf Azimut und Antennendrehung
                ant_loss = self.app.antenna_engine.get_attenuation(u_lat, u_lon, m_lat, m_lon)
                # Die berechnete Dämpfung (ist negativ oder 0) wird addiert (= abgezogen)
                fs_g += ant_loss
                sky_base += ant_loss
                cur_sky += ant_loss
                
                # --- LAUFZEITEN (Delays) ---
                t_bw = dist / (self.c * 0.997) # Bodenwelle (leicht verzögert durch Erdnähe)
                t_rw = path_rw / self.c        # Raumwelle (direkter Pfad durch Luft/Vakuum)
                
                res_list.append({
                    "p": d["prog"], "f": f, 
                    "pot": max(fs_g, sky_base), 
                    "now": max(fs_g, cur_sky), 
                    "fs_g": fs_g, "fs_s": cur_sky, 
                    "t_bw": t_bw, "t_rw": t_rw
                })

        # SORTIERUNG & ANZEIGE
        # 1. Potential-Liste (Was wäre nachts möglich?)
        res_list.sort(key=lambda x: x["pot"], reverse=True)
        for r in res_list: 
            self.app.list_max.insert(tk.END, f"{r['pot']:4.1f} | {int(r['f']):4} kHz | {r['p'][:25]}")
        
        # 2. Aktuelle Empfangsliste (Was ist JETZT hörbar?)
        res_list.sort(key=lambda x: x["now"], reverse=True)

        # Set für diesen Durchlauf vorbereiten
        now_active_stations = set()

        for r in res_list:
            if r['now'] > sens:
                # Modus-Erkennung: Welcher Pfad dominiert oder gibt es Interferenz (Fading)?
                m = "GrW+SkW" if abs(r['fs_g'] - r['fs_s']) < 10 else ("SkW   " if r['fs_s'] > r['fs_g'] else "GrW   ")
                # Anzeige der Delays je nach aktivem Pfad
                t_disp = f"{r['t_bw']:.2f}/{r['t_rw']:.2f}" if "GrW+SkW" in m else (f"{r['t_rw']:.2f}" if "SkW" in m else f"{r['t_bw']:.2f}")

                self.app.list_now.insert(tk.END, f"{r['now']:4.1f} | {int(r['f']):4}k | {r['p'][:15]} | {m} | {t_disp}ms")
                
                # --- UDP-VERSAND AN MODULATOR ---
                if send_udp:
                    freq_hz = int(r['f'] * 1000)
                    now_active_stations.add(freq_hz)  # Im aktuellen Durchlauf als aktiv registrieren
                    
                    # Dynamik: 40 dB Fenster oberhalb der eingestellten Schwelle
                    gain_norm = max(0.0, min(1.0, (r['now'] - sens) / 40.0))
                    # Format: Frequenz in Hz : Linearer Gain (0.0-1.0)
                    msg = f"{freq_hz}:{round(gain_norm, 6)}"
                    try: 
                        self.app.sock_gain.sendto(msg.encode(), (self.app.udp_ip, self.app.udp_port_gain))
                    except: 
                        pass

        # --- AUFRÄUMEN --- EFFIZIENTER GEDÄCHTNIS-RESET (ERSETZT DAS GANZE BAND ZU NULLEN) ---
        if send_udp:
            # Welche Frequenzen waren LETZTES MAL aktiv, sind es JETZT aber nicht mehr?
            droped_stations = self.last_active_stations - now_active_stations

            # NUR für diese stumm gewordenen Sender schicken wir exakt EINMAL 0.0!
            for f_hz in droped_stations:
                try: 
                    self.app.sock_gain.sendto(f"{f_hz}:0.0".encode(), (self.app.udp_ip, self.app.udp_port_gain))
                except: 
                    pass

            # Das Gedächtnis für den nächsten Schritt aktualisieren
            self.last_active_stations = now_active_stations


        # Aktualisiert das optische Antennen-Polygon, falls eine Richtcharakteristik aktiv ist
        if hasattr(self.app, 'antenna_engine'):
            self.app.antenna_engine.update_visuals()

