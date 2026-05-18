import tkinter as tk
from tkinter import ttk, simpledialog
import os, glob, csv, sqlite3, time, socket, threading, random
from datetime import datetime
from tkintermapview import TkinterMapView
from geopy.geocoders import ArcGIS

# Import der Engines
from Propagation_Engine import PropagationEngine
from Storm_Simulation import StormEngine

# Import der RX-Antennencharakteristik
from tkinter import filedialog
from Antenna_Engine import AntennaEngine

# --- LOKALISIERUNGSDATEN (Inkl. Gewitter-Erweiterungen) ---
LANGUAGES = {
    "DE": {
        "title": "Ausbreitungssimulator für HF-Modulator Steuerung (UDP 8888)",
        "date": "Datum (TT.MM):", "time": "Uhrzeit:", "sens": "Schwelle (dBµV/m):",
        "sun": "Sonne (RX):", "pot_head": "Potential (Nacht)", "cur_head": "AKTUELLER Empfang",
        "list_hdr": "dB | Hz | Name | Modus | ms", "fast": "⏩ Fast-Forward",
        "fast_stop": "■ Stop Fast", "real": "▶ Echtzeit Start", "real_stop": "■ Echtzeit Stop",
        "rec_marker": "🏠 Empfänger", "rec_menu": "Empfänger hierher",
        "mod_menu": "Modulator", "mod_off": "Alle AUS (0.0)", "mod_max": "Alle MAX (1.0)", "mod_custom": "Gain Wert setzen...",
        "sf_title": "⛈ Gewitter-Zelle", "sf_place": "⛈ Gewitter setzen", "sf_click": "👉 Klick in Karte...",
        "sf_on": "QRN Start", "sf_off": "QRN Stop", "sf_dir": "Richtung (°):", "sf_spd": "Speed (km/h):",
        "sf_amp": "Basis-Power:", "sf_rate": "Häufigkeit:",
        "rx_ant_load" : "📡 Antenne laden...", "rx_ant_type" : "Rundstrahler (Omni)", "rx_ant_dir" : "Ausrichtung RX-Antenne:"
    },
    "EN": {
        "title": "HF Modulator Propagation Simulator Control (UDP 8888)",
        "date": "Date (DD.MM):", "time": "Time:", "sens": "Threshold (dBµV/m):",
        "sun": "Sun (RX):", "pot_head": "Potential (Night)", "cur_head": "CURRENT Reception",
        "list_hdr": "dB | Hz | Name | Mode | ms", "fast": "⏩ Fast-Forward",
        "fast_stop": "■ Stop Fast", "real": "▶ Start Realtime", "real_stop": "■ Stop Realtime",
        "rec_marker": "🏠 Receiver", "rec_menu": "Receiver here",
        "mod_menu": "Modulator", "mod_off": "All OFF (0.0)", "mod_max": "All MAX (1.0)", "mod_custom": "Set Gain Value...",
        "sf_title": "⛈ Storm Cell", "sf_place": "⛈ Place Storm", "sf_click": "👉 Click Map...",
        "sf_on": "QRN Start", "sf_off": "QRN Stop", "sf_dir": "Dir (°):", "sf_spd": "Speed (km/h):",
        "sf_amp": "Base Power:", "sf_rate": "Frequency:",
        "rx_ant_load" : "📡 Load RX Antenna ...", "rx_ant_type" : "Omnidirectional (Omni)", "rx_ant_dir" : "RX Antenna Alignment:"
    },
    "FR": {
        "title": "Simulateur de propagation HF (UDP 8888)",
        "date": "Date (JJ.MM):", "time": "Heure:", "sens": "Seuil (dBµV/m):",
        "sun": "Soleil (RX):", "pot_head": "Potentiel (Nuit)", "cur_head": "Réception ACTUELLE",
        "list_hdr": "dB | Hz | Nom | Mode | ms", "fast": "⏩ Avance rapide",
        "fast_stop": "■ Arrêter", "real": "▶ Temps réel", "real_stop": "■ Arrêter",
        "rec_marker": "🏠 Récepteur", "rec_menu": "Récepteur ici",
        "mod_menu": "Modulateur", "mod_off": "Tous OFF (0.0)", "mod_max": "Tous MAX (1.0)", "mod_custom": "Régler le Gain...",
        "sf_title": "⛈ Orage", "sf_place": "⛈ Placer l'orage", "sf_click": "👉 Cliquer sur carte",
        "sf_on": "Démarrer QRN", "sf_off": "Arrêter QRN", "sf_dir": "Dir (°):", "sf_spd": "Vitesse (km/h):",
        "sf_amp": "Puissance:", "sf_rate": "Fréquence:",
        "rx_ant_load" : "📡 Charger l'antenne RX...", "rx_ant_type" : "Omnidirectionnelle (Omni)", "rx_ant_dir" : "Alignement de l'antenne RX :"

    },
    "IT": {
        "title": "Simulatore di propagazione HF (UDP 8888)",
        "date": "Data (GG.MM):", "time": "Ora:", "sens": "Soglia (dBµV/m):",
        "sun": "Sole (RX):", "pot_head": "Potenziale (Notte)", "cur_head": "Ricezione ATTUALE",
        "list_hdr": "dB | Hz | Nome | Modo | ms", "fast": "⏩ Avanti veloce",
        "fast_stop": "■ Ferma", "real": "▶ Tempo reale", "real_stop": "■ Ferma",
        "rec_marker": "🏠 Ricevitore", "rec_menu": "Ricevitore qui",
        "mod_menu": "Modulatore", "mod_off": "Tutto OFF (0.0)", "mod_max": "Tutto MAX (1.0)", "mod_custom": "Imposta Gain...",
        "sf_title": "⛈ Temporale", "sf_place": "⛈ Piazza temporale", "sf_click": "👉 Clicca sulla mappa",
        "sf_on": "Avvia QRN", "sf_off": "Ferma QRN", "sf_dir": "Dir (°):", "sf_spd": "Velocità:",
        "sf_amp": "Potenza:", "sf_rate": "Frequenza:",
        "rx_ant_load" : "📡 Carica antenna RX...", "rx_ant_type" : "Omnidirezionale (Omni)", "rx_ant_dir" : "Allineamento antenna RX :"

    },
    "JP": {
        "title": "HFモジュレーター伝搬シミュレーター (UDP 8888)",
        "date": "日付 (日.月):", "time": "時刻:", "sens": "しきい値 (dBµV/m):",
        "sun": "太陽 (RX):", "pot_head": "ポテンシャル (夜間)", "cur_head": "現在の受信状態",
        "list_hdr": "dB | Hz | 放送局名 | モード | ms", "fast": "⏩ 早送り",
        "fast_stop": "■ 停止", "real": "▶ リアルタイム開始", "real_stop": "■ 停止",
        "rec_marker": "🏠 受信機", "rec_menu": "受信機をここに設置",
        "mod_menu": "変調器", "mod_off": "全オフ (0.0)", "mod_max": "全最大 (1.0)", "mod_custom": "ゲイン値を設定...",
        "sf_title": "⛈ 雷雨セル", "sf_place": "⛈ 雷雨を配置", "sf_click": "👉 地図をクリック...",
        "sf_on": "QRN 開始", "sf_off": "QRN 停止", "sf_dir": "方向 (°):", "sf_spd": "速度 (km/h):",
        "sf_amp": "基本電力:", "sf_rate": "頻度:",
        "rx_ant_load" : "📡 RXアンテナを読み込む...", "rx_ant_type" : "無指向性 (オムニ)", "rx_ant_dir" : "RXアンテナ方向:"

    }
}

class RadioMapApp:
    def __init__(self, root):
        self.root = root
        self.cur_lang = "DE"
        self.LANGUAGES = LANGUAGES
        
        # Engines initialisieren
        self.prop_engine = PropagationEngine(self)
        self.storm_engine = StormEngine(self)
        self.antenna_engine = AntennaEngine(self)

        # UDP Setup für externen Modulator (Gain & Sferics Split)
        self.udp_ip = "127.0.0.1"; self.udp_port_gain = 8888; self.udp_port_sferics = 8889
        self.sock_gain = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock_sferics = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

        # GPS Empfang / new RX Location over UDP 8886
        self.gps_udp_port = 8886
        threading.Thread(target=self.udp_gps_listener, daemon=True).start()

        self.is_realtime_running = False; self.is_fast_running = False; self.last_pc_minute = -1

        # 1. Datenbank & Geocoder Initialisierung
        self.db_path = "geocache_radio.db"
        self.conn = sqlite3.connect(self.db_path); self.cursor = self.conn.cursor()
        self.cursor.execute('CREATE TABLE IF NOT EXISTS locations (key TEXT PRIMARY KEY, lat REAL, lon REAL)')
        self.conn.commit()
        self.geolocator = ArcGIS(timeout=10)

        # 2. Karten-Widget Setup
        self.map_widget = TkinterMapView(self.root, width=1200, height=900)
        self.map_widget.pack(fill="both", expand=True)
        # --- FIXIERTER BLITZ-BUTTON UNTEN LINKS IN MAP_WND---
        self.btn_map_sferics = tk.Button(
            self.map_widget, 
            text="⚡", 
            font=("Arial", 20, "bold"),
            bg="#f0f0f0",
            fg="orange",
            width=2,
            height=1,
            relief="raised",
            command=self.toggle_storm_panel # Öffnet das neue Kindfenster
        )
        # Positionierung: 20 Pixel vom linken Rand, 20 Pixel vom unteren Rand
        self.btn_map_sferics.place(relx=0, rely=1.0, x=20, y=-20, anchor="sw")

        self.receiver_coords = (52.52, 13.40); self.receiver_marker = None
        self.map_widget.set_position(52.52, 13.40); self.map_widget.set_zoom(5)
        self.map_widget.add_left_click_map_command(self.storm_engine.place_storm_callback)
        
        self.setup_menu()
        self.open_simulation_window()
        self.load_all_from_folder("tx_sites")
        self.change_language("DE")

    def udp_gps_listener(self):
        """Lauscht auf Port 8886 auf Koordinaten vom Navi."""
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.bind((self.udp_ip, self.gps_udp_port))
        
        while True:
            try:
                data, _ = sock.recvfrom(1024)
                # Format: "lat,lon" (z.B. "50.5200,11.4000")
                coords_str = data.decode('utf-8').strip().split(',')
                if len(coords_str) == 2:
                    lat = float(coords_str[0])
                    lon = float(coords_str[1])
                    
                    # Update an den Haupt-Thread übergeben
                    self.root.after_idle(self.set_receiver, (lat, lon))
            except:
                pass


    def setup_menu(self):
        self.menubar = tk.Menu(self.root)

        # 1. Sprachen-Menü
        lang_menu = tk.Menu(self.menubar, tearoff=0)
        for c, n in [("DE","Deutsch"),("EN","English"),("FR","Français"),("IT","Italiano"),("JP","日本語")]:
            lang_menu.add_command(label=n, command=lambda code=c: self.change_language(code))
        self.menubar.add_cascade(label="Language", menu=lang_menu)

        # 2. Modulator-Menü
        self.mod_menu = tk.Menu(self.menubar, tearoff=0)
        self.menubar.add_cascade(menu=self.mod_menu)
        self.root.config(menu=self.menubar)

    def update_sferics_button_color(self):
        if hasattr(self, 'storm_engine') and self.storm_engine.storm_active:
            self.btn_map_sferics.config(bg="yellow", fg="red")
        else:
            self.btn_map_sferics.config(bg="#f0f0f0", fg="orange")

    def change_language(self, code):
        self.cur_lang = code
        T = LANGUAGES[code]

        # UI-Updates im Simulationsfenster
        self.root.title(T["title"]); self.sim_win.title(T["title"])
        self.lbl_date_desc.config(text=T["date"]); self.lbl_time_desc.config(text=T["time"])
        self.lbl_sens_desc.config(text=T["sens"]); self.lbl_pot_title.config(text=T["pot_head"])
        self.lbl_cur_title.config(text=f"{T['cur_head']} ({T['list_hdr']})")

        # Gewitter-UI Updates
        self.sf_frame.config(text=T["sf_title"]); self.lbl_sf_dir.config(text=T["sf_dir"])
        self.lbl_sf_spd.config(text=T["sf_spd"]); self.lbl_sf_amp.config(text=T["sf_amp"])
        self.lbl_sf_rate.config(text=T["sf_rate"])

        if hasattr(self, 'storm_win'):
            self.storm_win.title(T["sf_title"])

        if not self.storm_engine.storm_placement_mode:
            self.btn_place_storm.config(text=T["sf_place"])

        self.btn_sferics.config(text=T["sf_off"] if self.storm_engine.storm_active else T["sf_on"])

        # Antennencharakteristik
        self.btn_load_ant.config(text=T["rx_ant_load"])
        self.lbl_ant_dir.config(text=T["rx_ant_dir"])

        # Prüfen: Ist der Clear-Button deaktiviert? Dann ist KEINE Antenne geladen.
        if str(self.btn_clear_ant["state"]) == "disabled":
            self.lbl_ant_status.config(text=T["rx_ant_type"], fg="grey")
        else:
            # Falls doch, behalten wir zwingend den blau geschriebenen Dateinamen!
            if hasattr(self, 'antenna_engine') and hasattr(self.antenna_engine, 'filename'):
                self.lbl_ant_status.config(text=self.antenna_engine.filename, fg="blue")
        # Button-Texte
        self.btn_fast.config(text=T["fast_stop"] if self.is_fast_running else T["fast"])
        self.btn_realtime.config(text=T["real_stop"] if self.is_realtime_running else T["real"])

        # --- FIX FÜR RECHTSKLICK-MENÜ ---
        self.map_widget.right_click_menu_commands = [] 
        self.map_widget.add_right_click_menu_command(label=T["rec_menu"], command=self.set_receiver, pass_coords=True)

        # --- MODULATOR MENÜ AKTUALISIEREN ---
        self.mod_menu.delete(0, tk.END)
        self.mod_menu.add_command(label=T["mod_off"], command=lambda: self.set_global_gain_value(0.0))
        self.mod_menu.add_command(label=T["mod_max"], command=lambda: self.set_global_gain_value(1.0))
        self.mod_menu.add_command(label=T["mod_custom"], command=self.set_global_gain_dialog)
        # Setzt den Titel des 2. Menü-Eintrags
        self.menubar.entryconfig(2, label=T["mod_menu"])
        # Marker-Text anpassen        
        if self.receiver_marker: self.receiver_marker.set_text(T["rec_marker"])

        self.run_simulation()

    # --- WRAPPER FÜR ENGINES ---
    def run_simulation(self, send_udp=False): self.prop_engine.run_simulation(send_udp)
    def toggle_sferics(self): self.storm_engine.toggle_sferics()
    def apply_storm_profile(self, event=None): self.storm_engine.apply_storm_profile(event)

    def set_receiver(self, coords):
        self.receiver_coords = coords
        #if self.receiver_marker: self.receiver_marker.delete() 
        if self.receiver_marker is None:
            self.receiver_marker = self.map_widget.set_marker(
                *coords, 
                text=self.LANGUAGES[self.cur_lang]["rec_marker"], 
                marker_color_outside="blue"
            )
        self.receiver_marker.set_position(*coords)
        self.map_widget.set_position(*coords)
        self.run_simulation()

    def open_simulation_window(self):
        self.sim_win = tk.Toplevel(self.root);
        self.sim_win.protocol("WM_DELETE_WINDOW", lambda: None)
        self.sim_win.geometry("1050x800")

        ctrl = tk.Frame(self.sim_win, pady=10); ctrl.pack(fill="x")

        # UI Elemente für Lokalisation als Instanzvariablen
        self.lbl_date_desc = tk.Label(ctrl);
        self.lbl_date_desc.grid(row=0, column=0)

        self.ent_date = tk.Entry(ctrl, width=10); 
        self.ent_date.insert(0, "15.10.");
        self.ent_date.grid(row=0, column=1)

        self.lbl_time_desc = tk.Label(ctrl); self.lbl_time_desc.grid(row=0, column=2)
        self.lbl_time_val = tk.Label(ctrl, text="12:00", font=("Arial", 10, "bold"), width=6);
        self.lbl_time_val.grid(row=0, column=3)

        self.time_slider = tk.Scale(ctrl, from_=0, to=23.99, resolution=0.01, orient="horizontal", length=200, showvalue=0, command=self.update_time_label)
        self.time_slider.set(12.0); self.time_slider.grid(row=0, column=4);
        self.time_slider.bind("<ButtonRelease-1>", lambda e: self.run_simulation())

        # Realtime Button
        self.btn_realtime = tk.Button(ctrl, command=self.toggle_realtime, width=14);
        self.btn_realtime.grid(row=0, column=5)
 
        # Fast-Forward Button
        self.btn_fast = tk.Button(ctrl, command=self.toggle_fastforward, width=14);
        self.btn_fast.grid(row=0, column=6)

        self.lbl_sens_desc = tk.Label(ctrl); 
        self.lbl_sens_desc.grid(row=1, column=2)

        self.sens_slider = tk.Scale(ctrl, from_=10, to=90, orient="horizontal", length=200);
        self.sens_slider.set(44);
        self.sens_slider.grid(row=1, column=4);
        self.sens_slider.bind("<ButtonRelease-1>", lambda e: self.run_simulation())

        self.lbl_sun = tk.Label(ctrl, font=("Arial", 10)); self.lbl_sun.grid(row=1, column=0, columnspan=2)

        # --- ANTENNEN STEUERUNG (Row 2) ---
        # Wir trennen Spalte 0 und 1 für den Lade- und Löschbutton auf
        self.btn_load_ant = tk.Button(ctrl, text="📡 Antenne laden...", command=self.load_antenna_dialog, width=25)
        self.btn_load_ant.grid(row=2, column=0, pady=5, padx=10, sticky="ew")

        self.btn_clear_ant = tk.Button(ctrl, text="❌", command=self.clear_antenna, state="disabled", width=3, fg="red")
        self.btn_clear_ant.grid(row=2, column=1, pady=5, padx=2)

        self.lbl_ant_status = tk.Label(ctrl, text="Rundstrahler (Omni)", fg="grey")
        self.lbl_ant_status.grid(row=2, column=2, columnspan=2, sticky="w", padx=5)

        self.lbl_ant_dir = tk.Label(ctrl, text="Ausrichtung RX-Antenne:")
        self.lbl_ant_dir.grid(row=2, column=4, sticky="e")

        self.sl_ant_dir = tk.Scale(ctrl, from_=0, to=359, orient="horizontal", length=200, state="disabled", command=self.update_antenna_heading)
        self.sl_ant_dir.set(0)
        self.sl_ant_dir.grid(row=2, column=5, columnspan=2)

        # --- EIGENES KIND-FENSTER FÜR STORM PANEL ---
        #self.btn_toggle_storm = tk.Button(self.sim_win, text="⛈ Sferics Fenster öffnen", command=self.toggle_storm_panel, bg="#d5dbdb")
        #self.btn_toggle_storm.pack(fill="x", padx=10, pady=2)

        # Neues Fenster erstellen, aber sofort verstecken
        self.storm_win = tk.Toplevel(self.root)
        self.storm_win.geometry("600x180")
        self.storm_win.resizable(False, False)
        # Wenn der User auf das 'X' drückt, wird das Fenster nur versteckt, nicht zerstört!
        self.storm_win.protocol("WM_DELETE_WINDOW", self.toggle_storm_panel)
        self.storm_win.withdraw() 

        # sf_frame nutzt jetzt storm_win als Parent anstatt sim_win
        self.sf_frame = tk.LabelFrame(self.storm_win, pady=5, padx=10)
        self.sf_frame.pack(fill="both", expand=True, padx=10, pady=10)

        r1 = tk.Frame(self.sf_frame); r1.pack(fill="x")
        self.btn_place_storm = tk.Button(r1, command=self.storm_engine.enable_storm_placement, width=18); self.btn_place_storm.pack(side="left")
        self.btn_sferics = tk.Button(r1, command=self.toggle_sferics, width=12, state="disabled"); self.btn_sferics.pack(side="left")
        tk.Label(r1, text="ITU-Profil:").pack(side="left")
        self.storm_profile = ttk.Combobox(r1, values=["Manual", "ITU Weak", "ITU Medium", "ITU Strong"], state="readonly", width=10); 
        self.storm_profile.current(0);
        self.storm_profile.bind("<<ComboboxSelected>>", self.apply_storm_profile); self.storm_profile.pack(side="left")

        # Richtung/Speed
        r2 = tk.Frame(self.sf_frame); r2.pack(fill="x")
        self.lbl_sf_dir = tk.Label(r2); self.lbl_sf_dir.pack(side="left")
        self.sl_sf_dir = tk.Scale(r2, from_=0, to=360, orient="horizontal", length=150); self.sl_sf_dir.set(90); self.sl_sf_dir.pack(side="left")
        self.lbl_sf_spd = tk.Label(r2); self.lbl_sf_spd.pack(side="left")
        self.sl_sf_spd = tk.Scale(r2, from_=0, to=100, orient="horizontal", length=150); self.sl_sf_spd.set(50); self.sl_sf_spd.pack(side="left")

        # Power/Rate
        r3 = tk.Frame(self.sf_frame); r3.pack(fill="x")
        self.lbl_sf_amp = tk.Label(r3); self.lbl_sf_amp.pack(side="left")
        self.sl_sf_amp = tk.Scale(r3, from_=50, to=5000, orient="horizontal", length=250); self.sl_sf_amp.set(2500); self.sl_sf_amp.pack(side="left")
        self.lbl_sf_rate = tk.Label(r3); self.lbl_sf_rate.pack(side="left")
        self.sl_sf_rate = tk.Scale(r3, from_=1, to=20, orient="horizontal", length=250); self.sl_sf_rate.set(6); self.sl_sf_rate.pack(side="left")

        # --- LISTEN-BEREICH ---
        lf = tk.Frame(self.sim_win); lf.pack(expand=True, fill="both", padx=10)
        self.lbl_pot_title = tk.Label(lf); self.lbl_pot_title.grid(row=0, column=0)
        self.lbl_cur_title = tk.Label(lf); self.lbl_cur_title.grid(row=0, column=1)

        fs = ("MS Gothic", 9) if os.name == 'nt' else ("Courier", 9)
        self.list_max = tk.Listbox(lf, width=45, font=fs); self.list_max.grid(row=1, column=0, sticky="nsew")
        self.list_now = tk.Listbox(lf, width=80, font=fs); self.list_now.grid(row=1, column=1, sticky="nsew")
        lf.rowconfigure(1, weight=1); lf.columnconfigure(1, weight=2)

    def toggle_realtime(self):
        self.is_realtime_running = not self.is_realtime_running
        self.change_language(self.cur_lang)

        # WICHTIG: Den Anker auf -1 setzen, damit er beim 
        # nächsten Check sofort die aktuelle Minute als Basis nimmt.
        if self.is_realtime_running: self.last_pc_minute = -1; self.realtime_tick()
        self.run_simulation(True)

    def realtime_tick(self):
        if self.is_realtime_running and self.sim_win.winfo_exists():
            now = datetime.now()
  
            # PRÜFUNG: Hat die PC-Uhr gerade auf eine neue Minute gewechselt?
            if now.minute != self.last_pc_minute:
                if self.last_pc_minute != -1:
                    # Exakt eine Minute (1/60 Stunde) addieren
                    nv = (float(self.time_slider.get()) + (1/60)) % 24

                    # Slider und Label aktualisieren
                    self.time_slider.set(nv); 
                    self.update_time_label(nv);

                    # Simulation neu berechnen
                    self.run_simulation(True)

                     # GEWITTER-LEBENSDAUER REDUZIEREN
                    if self.storm_engine.storm_active:
                        self.storm_engine.storm_life_minutes -= 1.0
                        #print("sferice_active_for: \n",self.storm_engine.storm_life_minutes)
                        if self.storm_engine.storm_life_minutes <= 0: self.toggle_sferics() # Schaltet QRN aus und löscht Icons/Pfeile
                        #print("⛈ Gewitter hat sich aufgelöst.")

            # Wir prüfen häufig (alle 0.5s), damit wir den Moment 
            # des Umspringens präzise (max. 0.5s Verzögerung) erwischen.
                self.last_pc_minute = now.minute
            self.root.after(500, self.realtime_tick)

    def toggle_fastforward(self):
        self.is_fast_running = not self.is_fast_running
        self.change_language(self.cur_lang);
        self.fast_tick()

    def fast_tick(self):
        if self.is_fast_running and self.sim_win.winfo_exists():
            nv = (float(self.time_slider.get()) + (1/60)) % 24
            self.time_slider.set(nv);
            self.update_time_label(nv);
            self.run_simulation(True)
            self.root.after(1000, self.fast_tick)

    def update_time_label(self, val):
        v = float(val); self.lbl_time_val.config(text=f"{int(v):02d}:{int((v*60)%60):02d}")

    def toggle_storm_panel(self):
        """Öffnet oder versteckt das Gewitter-Kindfenster."""
        # Sicherheitscheck, falls das Fenster noch nicht existiert
        if not hasattr(self, 'storm_win'):
            return

        # Prüfen, ob das Fenster aktuell versteckt ist
        if self.storm_win.state() == "withdrawn":
            self.storm_win.deiconify()  # Fenster einblenden
            self.storm_win.lift()       # In den Vordergrund holen
            
            # Button auf der Karte optisch ändern, wenn das Fenster offen ist:
            self.btn_map_sferics.config(relief="sunken", bg="#d5dbdb")
        else:
            self.storm_win.withdraw()   # Fenster verstecken
            
            # Button wieder in normalen Zustand versetzen
            self.btn_map_sferics.config(relief="raised", bg="#f0f0f0")
            
            # WICHTIG: Falls ein Gewitter läuft, soll die Farbe erhalten bleiben
            self.update_sferics_button_color()

    def load_antenna_dialog(self):
        filepath = filedialog.askopenfilename(
            title="Antennendatei wählen",
            filetypes=(("Antenna Files", "*.msi *.ant"),),
            parent=self.sim_win
        )
        if filepath:
            if self.antenna_engine.load_file(filepath):
                self.lbl_ant_status.config(text=f"{self.antenna_engine.filename}", fg="blue")
                self.sl_ant_dir.config(state="normal")
                self.btn_clear_ant.config(state="normal") # <-- Aktiviert den X-Button
                # RECHNERISCH + VISUELL AKTUALISIEREN
                self.antenna_engine.update_visuals() 
                self.run_simulation(True)

    def update_antenna_heading(self, val):
        if hasattr(self, 'antenna_engine'):
            self.antenna_engine.antenna_heading = float(val)
            # send_udp=True (bzw. True) sorgt dafür, dass bei JEDEM 
            # Zucken des Sliders die Daten berechnet und direkt per UDP verschickt werden!
            # RECHNERISCH + VISUELL LIVE IN DER MAP ROTIEREN BEIM SLIDEN
            self.antenna_engine.update_visuals()
            self.run_simulation(True)

    def clear_antenna(self):
        """Verwirft das Diagramm, setzt die UI zurück und funkt den Omni-Status an den Modulator."""
        if hasattr(self, 'antenna_engine'):
            # 1. Engine zurücksetzen
            self.antenna_engine.reset()
            
            # 2. UI-Elemente zurücksetzen und sperren
            current_omni_text = self.LANGUAGES[self.cur_lang]["rx_ant_type"]
            self.lbl_ant_status.config(text=current_omni_text, fg="grey")
            self.sl_ant_dir.set(0)
            self.sl_ant_dir.config(state="disabled")
            self.btn_clear_ant.config(state="disabled") # <-- Deaktiviert sich selbst
            
            # 3. SOFORTIGE NEUBERECHNUNG UND LIVE-VERSAND ÜBER UDP
            self.run_simulation(True)

    def set_global_gain_dialog(self):
        v = simpledialog.askfloat("Gain", "0.0 - 1.0:", minvalue=0.0, maxvalue=1.0, parent=self.root)
        if v is not None: self.set_global_gain_value(v)

    def set_global_gain_value(self, val):
        for f in range(100000, 1701000, 1000):
            try: self.sock_gain.sendto(f"{f}:{round(float(val), 6)}".encode(), (self.udp_ip, self.udp_port_gain))
            except: pass

    def load_all_from_folder(self, folder):
        if not os.path.exists(folder): os.makedirs(folder); return
        for file in glob.glob(os.path.join(folder, "*.csv")):
            with open(file, mode='r', encoding='utf-8-sig') as f:
                for row in csv.reader(f):
                    if len(row) >= 5:
                        stadt, land, freq, kw, prog = [item.strip() for item in row[:5]]
                        cache_key = f"{stadt},{land}".upper()
                        self.cursor.execute("SELECT lat, lon FROM locations WHERE key=?", (cache_key,))
                        res = self.cursor.fetchone()
                        if not res:
                            try:
                                loc = self.geolocator.geocode(f"{stadt}, {land}")
                                if loc: 
                                    res = (loc.latitude, loc.longitude)
                                    self.cursor.execute("INSERT INTO locations VALUES (?,?,?)", (cache_key, *res))
                                    self.conn.commit()
                            except: continue
                        if res:
                            m = self.map_widget.set_marker(*res, text=f"{prog}\n{freq} kHz")
                            m.data = {"freq": freq, "kw": kw, "prog": prog}
