import os, time, math, random, threading
from PIL import Image, ImageTk

SFERICS_LIFETIME = 120.0 # Die Zelle lebt 2 Stunden Simulationszeit

class StormEngine:
    def __init__(self, app):
        self.app = app
        self.storm_active = False
        self.storm_placement_mode = False
        self.storm_pos = [50.0, 10.0]
        self.storm_life_minutes = SFERICS_LIFETIME
        self.storm_polygon = None
        self.storm_icons = []
        self.storm_vector = []
        self.storm_last_ui_update = 0

        self.storm_image = None

        # Absoluten Pfad zur Datei ermitteln
        script_dir = os.path.dirname(os.path.abspath(__file__))
        image_path = os.path.join(script_dir, "cloud.png")
        try:
            if os.path.exists(image_path):
                pil_img = Image.open(image_path).convert("RGBA")
                # Falls das Bild zu groß/klein ist, hier anpassen:
                pil_img = pil_img.resize((64, 64), Image.Resampling.LANCZOS)
                self.storm_image = ImageTk.PhotoImage(pil_img)
                print(f"Gewitter-Bild erfolgreich geladen: {image_path}")
            else:
                print(f"FEHLER: Datei nicht gefunden unter {image_path}")
                self.storm_image = None
        except Exception as e:
            print(f"Fehler beim Laden des PNGs: {e}")
            self.storm_image = None


    def enable_storm_placement(self):
        self.storm_placement_mode = True
        self.app.btn_place_storm.config(text=self.app.LANGUAGES[self.app.cur_lang]["sf_click"], bg="yellow")

    def place_storm_callback(self, coords):
        if self.storm_placement_mode:
            self.storm_pos = [coords[0], coords[1]]
            self.storm_placement_mode = False
            self.app.btn_place_storm.config(text=self.app.LANGUAGES[self.app.cur_lang]["sf_place"], bg="#f0f0f0")
            self.app.btn_sferics.config(state="normal")
            if not self.storm_active: self.toggle_sferics()

    def apply_storm_profile(self, event=None):
        profile = self.app.storm_profile.get()
        if profile == "ITU Weak":
            self.app.sl_sf_amp.set(800); self.app.sl_sf_rate.set(2)
        elif profile == "ITU Medium":
            self.app.sl_sf_amp.set(2200); self.app.sl_sf_rate.set(7)
        elif profile == "ITU Strong":
            self.app.sl_sf_amp.set(4500); self.app.sl_sf_rate.set(15)

    def toggle_sferics(self):
        self.storm_active = not self.storm_active
        self.app.change_language(self.app.cur_lang)
        if self.storm_active:
            self.storm_icons = []
            self.storm_last_ui_update = 0.0
            self.storm_life_minutes = SFERICS_LIFETIME
            threading.Thread(target=self.run_engine, daemon=True).start()
        else:
            self.cleanup_graphics()

    def cleanup_graphics(self):
        if self.storm_polygon: self.storm_polygon.delete(); self.storm_polygon = None
        for icon in self.storm_icons: icon.delete()
        self.storm_icons = []
        for line in self.storm_vector: line.delete()
        self.storm_vector = []

    def run_engine(self):
        last_time = time.time()
        while self.storm_active:
            current_time = time.time()
            # Die echte vergangene Zeit (meist ~0.1s wegen sleep)
            real_dt = current_time - last_time
            last_time = current_time
             
            # --- DER FAST-FORWARD FIX ---
            # Wenn Fast-Forward aktiv ist, multiplizieren wir die Zeit mal 60
            dt = real_dt * (60.0 if self.app.is_fast_running else 1.0)
        
            # 1. Bewegung (dt ist nun entweder 0.1s oder 6.0s effektiv)
            speed_deg_s = (self.app.sl_sf_spd.get() / 3600.0) / 111.0
            rad = math.radians(self.app.sl_sf_dir.get())

            self.storm_pos[0] += math.cos(rad) * speed_deg_s * dt
            self.storm_pos[1] += math.sin(rad) * speed_deg_s * dt

            # 2. Grafik-Update (bleibt bei realen 0.5s, sonst flimmert es)        
            if current_time - self.storm_last_ui_update > 0.5:
                self.storm_last_ui_update = current_time
                self.app.root.after(0, self.draw_storm_circle)
        
            # 3. UDP-Sferics (Häufigkeit bei Fast-Forward ebenfalls anpassen?)
            # Optional: Blitze bei Fast-Forward seltener senden, da sie sonst alles fluten
            rate_limit = 0.05 if not self.app.is_fast_running else 0.01

            dist_km = math.sqrt(((self.storm_pos[0]-self.app.receiver_coords[0])*111.0)**2 + 
                               ((self.storm_pos[1]-self.app.receiver_coords[1])*85.0)**2)
        
            if random.random() < (self.app.sl_sf_rate.get() * rate_limit):
                attenuation = 1.0 / (1.0 + (max(0, dist_km - 20) / 80.0)**2)
                amp = int(self.app.sl_sf_amp.get() * attenuation * random.uniform(0.7, 1.3))
                if amp > 10:
                    try: 
                        self.app.sock_sferics.sendto(f"{amp}:{random.uniform(5,45):.1f}".encode(), 
                                                   (self.app.udp_ip, self.app.udp_port_sferics))
                    except: pass
            time.sleep(0.1)

    def draw_storm_circle(self):
        """
        Zeichnet die Gewitterzone. Nutzt das PNG als Favorit, 
        Emojis dienen als Fallback.
        """
        if not self.storm_active: return

        # 1. Geometrie & Zoom abfragen
        current_zoom = self.app.map_widget.zoom
        cos_lat = math.cos(math.radians(self.storm_pos[0]))
        aspect_korrektur = 1.0 / cos_lat
        r_deg = 0.7

        pts = [(self.storm_pos[0] + math.cos(math.radians(i)) * r_deg, 
                self.storm_pos[1] + math.sin(math.radians(i)) * r_deg * aspect_korrektur) 
               for i in range(0, 360, 10)]

        if self.storm_polygon: self.storm_polygon.delete()
        self.storm_polygon = self.app.map_widget.set_polygon(
            pts, fill_color="", outline_color="#ff3333", border_width=3
        )

        # --- Live-Daten Text generieren ---
        spd = self.app.sl_sf_spd.get()
        hdg = self.app.sl_sf_dir.get()
        # Lebenszeit runden und verhindern, dass negative Werte aufblinken
        life = int(max(0, self.storm_life_minutes)) 
        info_text = f"HDG: {hdg}° | SPD: {spd} km/h | TTL: {life} min"

        # 2. Darstellung (PNG vs. Emoji-Fallback)
        inner_r = r_deg * 0.6
        
        if self.storm_image:
            # Wir rechnen die halbe Bildhöhe (z.B. 32px) in Breitengrade um.
            # Die magische Zahl 170.1022 ist eine Annäherung für die Mercator-Projektion.
            image_h = 64  # Nutze die Höhe aus deinem resize() in __init__
            lat_offset = (image_h / 2.0) * (170.0 / (pow(2, current_zoom) * 256.0))
            
            # Wir setzen den Marker um den Offset nach SÜDEN (-), 
            # damit die Mitte des Bildes auf dem Zentrum liegt.
            draw_pos = (self.storm_pos[0] - 3*lat_offset, self.storm_pos[1])

            if len(self.storm_icons) != 1:
                for icon in self.storm_icons: icon.delete()
                self.storm_icons = []

            if not self.storm_icons:
                icon = self.app.map_widget.set_marker(
                    draw_pos[0], draw_pos[1], 
                    text=info_text, image=self.storm_image, # Text hier einfügen
                    font=("Arial", 10, "bold"), text_color="black",
                    marker_color_circle="", marker_color_outside=""
                )
                self.storm_icons.append(icon)
            else:
                self.storm_icons[0].set_position(draw_pos[0], draw_pos[1])
                self.storm_icons[0].set_text(info_text) # Text dynamisch aktualisieren!
        
        else:
            # --- MODUS B: EMOJI-FALLBACK ---
            symbols = ["⚡", "☁️", "⛈️", "⚡", "☁️"]
            offsets = [(0,0), (0.4, 0.4), (-0.4, -0.4), (0.3, -0.5), (-0.3, 0.5)]
            
            # Falls vorher das PNG da war (Länge == 1), löschen für Neuaufbau
            if len(self.storm_icons) == 1:
                self.storm_icons[0].delete()
                self.storm_icons = []

            if not self.storm_icons:
                for i, (o_lat, o_lon) in enumerate(offsets):
                    # Den Infotext nur an das mittlere (erste) Emoji hängen
                    marker_text = symbols[i % len(symbols)]
                    if i == 0: marker_text += f"\n{info_text}"

                    icon = self.app.map_widget.set_marker(
                        self.storm_pos[0] + o_lat * inner_r, 
                        self.storm_pos[1] + o_lon * inner_r * aspect_korrektur,
                        text=marker_text, font=("Arial", 12, "bold"), 
                        marker_color_circle="#ffcc00", marker_color_outside="#ffcc00"
                    )
                    self.storm_icons.append(icon)
            else:
                for i, (o_lat, o_lon) in enumerate(offsets):
                    self.storm_icons[i].set_position(
                        self.storm_pos[0] + o_lat * inner_r, 
                        self.storm_pos[1] + o_lon * inner_r * aspect_korrektur
                    )
                    # Beim Aktualisieren der Position auch den Text erneuern
                    if i == 0:
                        self.storm_icons[i].set_text(symbols[0] + f"\n{info_text}")

        # 3. Zugrichtung (Bleibt gleich)
        for line in self.storm_vector: line.delete()
        self.storm_vector = []
        speed = self.app.sl_sf_spd.get()
        if speed > 0:
            rad_dir = math.radians(self.app.sl_sf_dir.get())
            arrow_l = (speed / 100.0) * r_deg 
            e_lat = self.storm_pos[0] + math.cos(rad_dir) * arrow_l
            e_lon = self.storm_pos[1] + math.sin(rad_dir) * arrow_l * aspect_korrektur
            self.storm_vector.append(self.app.map_widget.set_path(
                [(self.storm_pos[0], self.storm_pos[1]), (e_lat, e_lon)], 
                color="#0055ff", width=4
            ))
