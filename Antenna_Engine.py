import math
import os

class AntennaEngine:
    def __init__(self, app):
        self.app = app
        self.horizontal_pattern = {}
        self.antenna_heading = 0.0  # Ausrichtung in Grad (0 = Nord)
        self.is_active = False
        self.filename = ""
        self.visual_polygon = None  # Speichert das aktuelle Karten-Overlay

    def load_file(self, filepath):
        """Liest MSI- oder ANT-Dateien ein und extrahiert das horizontale Diagramm."""
        self.horizontal_pattern.clear()
        try:
            with open(filepath, "r", encoding="utf-8") as f:
                in_horizontal = False
                for line in f:
                    line = line.strip().upper()
                    if not line or line.startswith("#"): 
                        continue
                    
                    if "HORIZONTAL" in line:
                        in_horizontal = True
                        continue
                    if "VERTICAL" in line:
                        in_horizontal = False
                        continue
                    
                    parts = line.split()
                    if len(parts) >= 2:
                        try:
                            angle = int(float(parts[0]))
                            val = float(parts[1])
                            if val > 0: 
                                val = -val
                            self.horizontal_pattern[angle % 360] = val
                        except ValueError:
                            pass
            
            self.is_active = len(self.horizontal_pattern) > 0
            if self.is_active:
                self.filename = os.path.basename(filepath)
                print(f"Antenne geladen: {self.filename} ({len(self.horizontal_pattern)} Punkte)")
            return True
        except Exception as e:
            print(f"Fehler beim Laden der Antenne: {e}")
            self.is_active = False
            return False

    def get_bearing(self, lat1, lon1, lat2, lon2):
        """Berechnet den geografischen Peilwinkel (Azimut) von RX (1) zu TX (2)."""
        lat1_rad, lat2_rad = math.radians(lat1), math.radians(lat2)
        d_lon_rad = math.radians(lon2 - lon1)

        y = math.sin(d_lon_rad) * math.cos(lat2_rad)
        x = math.cos(lat1_rad) * math.sin(lat2_rad) - math.sin(lat1_rad) * math.cos(lat2_rad) * math.cos(d_lon_rad)
        
        return (math.degrees(math.atan2(y, x)) + 360) % 360

    def get_attenuation(self, rx_lat, rx_lon, tx_lat, tx_lon):
        """Gibt die winkelabhängige Dämpfung in dB zurück."""
        if not self.is_active:
            return 0.0

        tx_bearing = self.get_bearing(rx_lat, rx_lon, tx_lat, tx_lon)
        rel_angle = int(round(tx_bearing - self.antenna_heading)) % 360
        return self.horizontal_pattern.get(rel_angle, 0.0)

    def update_visuals(self):
        """Zeichnet das Antennendiagramm live als Vektor-Polygon um den Empfänger."""
        # Altes Diagramm von der Karte löschen, falls vorhanden
        if self.visual_polygon:
            try:
                self.visual_polygon.delete()
            except:
                pass
            self.visual_polygon = None

        # FIX: Prüfe auf das korrekte Tuple aus der UI_Components
        if not self.is_active or not hasattr(self.app, 'receiver_coords') or self.app.receiver_coords is None:
            return

        # Koordinaten sauber entpacken
        rx_lat, rx_lon = self.app.receiver_coords

        points = []
        max_radius_deg = 0.5  # Skalierungsgröße des Diagramms auf der Karte (ca. 50km Radius)
        aspect_correction = math.cos(math.radians(rx_lat))

        # Wir tasten das Diagramm in Schritten ab, um eine feine Kurve zu zeichnen
        for angle in range(0, 360, 2):
            # Geografischer Winkel im Raum inkl. der Rotation des Sliders
            world_angle_rad = math.radians((angle + self.antenna_heading) % 360)

            # Dämpfung für diesen relativen Winkel holen
            attenuation = self.horizontal_pattern.get(angle, 0.0)

            # Umrechnung von dB in eine optische Skalierung (Dynamikbereich im Plot: 30 dB)
            clamped_att = max(-30.0, attenuation)
            factor = (clamped_att + 30.0) / 30.0  # 0 dB = 1.0 (Maximum), -30 dB = 0.0 (Minimum)
            
            # Ein kleiner Basis-Offset, damit tiefe Nullstellen im Zentrum nicht kollabieren
            factor = 0.03 + 0.97 * factor 
            r = max_radius_deg * factor

            # Umrechnung in GPS-Offsets (0° Kompass = Nord = nach oben auf Y-Achse)
            p_lat = rx_lat + r * math.cos(world_angle_rad)
            p_lon = rx_lon + (r * math.sin(world_angle_rad) / (aspect_correction if aspect_correction > 0 else 1))
            points.append((p_lat, p_lon))

        # Als elegantes, durchsichtiges Drahtgitter-Polygon auf der Map platzieren
        try:
            self.visual_polygon = self.app.map_widget.set_polygon(
                points,
                fill_color="",           # Keine Füllung (transparent), wie bei Storm-Zelle!
                outline_color="#ff5500",  # Knalliges Signal-Orange für die Richtcharakteristik
                border_width=3,
                name="AntennaPattern"
            )
        except Exception as e:
            print(f"Fehler beim Zeichnen des Antennen-Polygons: {e}")

    def reset(self):
        """Setzt die Antenne zurück und löscht das visuelle Overlay."""
        if self.visual_polygon:
            try: self.visual_polygon.delete()
            except: pass
            self.visual_polygon = None
        self.horizontal_pattern.clear()
        self.antenna_heading = 0.0
        self.is_active = False
        self.filename = ""
        print("Antennendiagramm verworfen. Zurück im Rundstrahlbetrieb.")