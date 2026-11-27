# RE:FRAME — Persona 3 Reload 1.1.5

Von Aporia. Für eine legal erworbene Steam-Kopie von Persona 3 Reload, App-ID **2161700**, Build **22672075**. Erfordert Windows x64 und DirectX 12.

## Neu in dieser Version

Die Stadtkarte kann den gewählten Upscaling-Modus von DLSS, FSR oder XeSS nutzen. Kann RE:FRAME den 3D-Hintergrund nicht korrigieren, verwendet die Karte bis zum Schließen den Rendermaßstab des Spiels.

**Schärfe** nutzt FidelityFX CAS, um die Bilddetails nach der Rekonstruktion mit DLAA, DLSS, FSR oder XeSS und vor dem Zeichnen des HUDs anzupassen. Der Regler reicht in 5-%-Schritten von 0 % (standardmäßig aus) bis 100 %. Änderungen werden im Grafikmenü übernommen und wirken sofort. Der Wert gilt für alle Verfahren und Qualitätsmodi. Original-TAA blendet den Regler aus.

## Installieren oder aktualisieren

1. Beende das Spiel. Öffne in Steam **Eigenschaften → Installierte Dateien → Durchsuchen**.
2. Falls `P3R\Binaries\Win64` bereits eine `d3d12.dll` enthält, befolge die Deinstallationsanleitung des zugehörigen Mods. Führe vor dem Aktualisieren von RE:FRAME das Skript `Remove-ReFrame.ps1` der bisherigen Version aus und entpacke dann die neue ZIP-Datei.
3. Vergleiche die SHA-256-Prüfsumme des ZIPs mit der Datei `.sha256`. Entpacke **REFRAME-P3R-v1.1.5.zip** in den Ordner, der `P3R` enthält. Starte das Spiel über Steam und wähle Verfahren, Qualitätsmodus und Schärfe.

Bewahre `ReFrame\Settings.ini` auf, um deine Einstellungen zu behalten. Bei älteren Entwicklungsversionen behältst du `P3RGraphicsEnhanced\Settings.ini`, bis RE:FRAME in den neuen Ordner schreibt. XeSS benötigt Microsoft Visual C++ Redistributable x64 14.40.33810 oder neuer.

## Deinstallieren

Beende das Spiel und führe `Remove-ReFrame.ps1` in `P3R\Binaries\Win64\ReFrame` aus. `-WhatIf` zeigt eine Vorschau. Das Skript prüft Manifest und Prüfsummen und lässt Einstellungen und Protokolle bestehen.

## Schärfe prüfen

Übernimm nacheinander 0 % und 100 % in derselben Szene mit identischem Verfahren, Qualitätsmodus und Blickwinkel. Schließe jeweils das Menü. Vergleiche Haare und feine Texturen. Prüfe dann beim Schwenken Kanten, HUD und Untertitel. Abbrechen soll ausstehende Änderungen verwerfen, Übernehmen den Wert über einen Neustart hinweg speichern.

In `P3R\Binaries\Win64\ReFrame\ReFrame.log` stehen das aktive Verfahren und die Anzahl der CAS-Durchläufe. Bei 0 % zeigt eine Stichprobe `240 frames (0 CAS)`, bei durchgehend aktivem Filter `240 frames (240 CAS)`.

Die Schärfe wurde im Spiel bei 1080p in den Leistungsmodi von DLSS, FSR und XeSS auf einer RTX 5060 geprüft. Die Offline-Tests deckten alle 14 verfügbaren SDK-Modi auf dieser GPU ab. Eigene Hardwaretests für FSR 4 auf Radeon und das für Intel Arc optimierte XeSS stehen aus.

## Lizenz

RE:FRAME ist ein unabhängiges Fanprojekt. Unter `Licenses` enthält das Archiv die MIT-Lizenz von RE:FRAME und die separaten Hinweise zu FidelityFX CAS, den Herstellerlaufzeiten und MinHook.
