# RE:FRAME — Persona 3 Reload

[English](../../README.md) · [Français](README.fr-FR.md) · [Italiano](README.it-IT.md) · **[Deutsch](README.de-DE.md)** · [Español (España)](README.es-ES.md) · [日本語](README.ja-JP.md) · [한국어](README.ko-KR.md) · [Polski](README.pl-PL.md) · [Português (Brasil)](README.pt-BR.md) · [Русский](README.ru-RU.md) · [简体中文](README.zh-CN.md) · [繁體中文](README.zh-TW.md) · [Türkçe](README.tr-TR.md) · [Українська](README.uk-UA.md)

<!-- source-readme: 834e57d57f2b49f6b6c8540a9c22bb0042aa92a7 -->

![Version 1.0.0](https://img.shields.io/badge/release-1.0.0-2469ad) ![Windows x64](https://img.shields.io/badge/platform-Windows%20x64-2469ad) ![DirectX 12](https://img.shields.io/badge/renderer-DirectX%2012-107c10) [![MIT-Lizenz](https://img.shields.io/badge/license-MIT-blue)](../../LICENSE)

Weniger Treppenkanten und Flimmern in **Persona 3 Reload**, ohne die Renderauflösung auf 200 % zu stellen. DLAA und natives AA mit FSR oder XeSS glätten Kanten in der Auflösung deines Bildschirms. Wenn du mehr Leistung möchtest, wähle einen Modus mit Hochskalierung.

**[RE:FRAME v1.0.0 herunterladen](https://github.com/Aporial/reframe-p3r/releases/latest)**

![Grafikmenü von Persona 3 Reload mit den Einstellungen von RE:FRAME](../images/settings.png)

*Die Optionen von RE:FRAME stehen direkt im Grafikmenü des Spiels.*

## Installation

1. Beende das Spiel und lade **REFRAME-P3R-v1.0.0.zip** über [Releases auf GitHub](https://github.com/Aporial/reframe-p3r/releases/latest) herunter.
2. Klicke in Steam mit der rechten Maustaste auf **Persona 3 Reload** und wähle **Eigenschaften → Installierte Dateien → Durchsuchen**.
3. Entpacke die ZIP-Datei in den Ordner, der bereits `P3R` enthält. Das Archiv legt `d3d12.dll` in `P3R\Binaries\Win64` direkt neben `P3R.exe` ab.
4. Starte das Spiel wie gewohnt über Steam.
5. Öffne die Grafikeinstellungen und wähle **Kantenglättung** direkt unter der Renderauflösung (**Rendering Scale**). Wähle einen Qualitätsmodus, falls verfügbar, und bestätige deine Änderungen.

**In `P3R\Binaries\Win64` liegt bereits eine `d3d12.dll`?** Sie kann zu einer anderen Mod gehören. Prüfe, welche Mod sie verwendet, und folge deren Anleitung zur Deinstallation, bevor du das ZIP entpackst.

Das ZIP enthält die Mod und die benötigten Grafikbibliotheken. Du stellst alles direkt im Spiel ein. ReShade, Mod-Loader, Overlays, separate Launcher und manuelle Änderungen an Konfigurationsdateien sind dafür überflüssig.

### Aktualisieren und deinstallieren

- **Aktualisieren** · Beende das Spiel, führe das Deinstallationsskript der bisherigen Version aus und entpacke das neue ZIP. Wenn du Einstellungen aus einer Entwicklungsversion übernehmen möchtest, behalte `P3RGraphicsEnhanced\Settings.ini`, bis RE:FRAME in `ReFrame\Settings.ini` speichert.
- **Deinstallieren** · Beende das Spiel und führe `Remove-ReFrame.ps1` in `P3R\Binaries\Win64\ReFrame` aus. Das Skript prüft Datei-Hashes und behält die erzeugten Einstellungen und Protokolle. Mit `-WhatIf` siehst du vorab, welche Dateien es entfernen würde. [Installations- und Deinstallationsanleitung](../INSTALLATION.md)

[Installation](#installation) · [Warum nicht mit 200% rendern?](#warum-nicht-mit-200-rendern) · [Welchen Modus sollte ich wählen?](#welchen-modus-sollte-ich-w%C3%A4hlen) · [Funktionen](#funktionen) · [Unterstützte Grafikkarten](#unterst%C3%BCtzte-grafikkarten) · [Erweiterte Einstellungen und Modi](#erweiterte-einstellungen-und-modi) · [Kompatibilität und Einschränkungen](#kompatibilit%C3%A4t-und-einschr%C3%A4nkungen) · [Fehlerbehebung](#fehlerbehebung) · [Aus dem Quellcode bauen](#aus-dem-quellcode-bauen) · [Lizenz](#lizenz)

## Warum nicht mit 200% rendern?

Bei 1920×1080 können Treppenkanten und flimmernde feine Details auffallen. Eine höhere Renderauflösung kann das Bild glätten, verlangt der Grafikkarte aber mehr ab.

| Renderauflösung | Berechnetes Bild auf einem 1080p-Bildschirm |
| --- | --- |
| **100%** | 1920×1080, rund 2,1 Millionen Pixel pro Bild |
| **200%** | 3840×2160, rund 8,3 Millionen Pixel pro Bild |

**Bei 200 % verdoppeln sich Breite und Höhe. Das Spiel berechnet viermal so viele Pixel.** Natives AA nutzt aufeinanderfolgende Bilder, um Kanten bei 100 % zu glätten. Auch das kostet GPU-Zeit. Wie es im Vergleich zu 200 % aussieht, hängt von der Szene ab.

Bei der Hochskalierung berechnet das Spiel weniger Pixel, als der Bildschirm anzeigt. Wenn die Grafikkarte der Engpass ist, kann das die Bildrate erhöhen.

## Welchen Modus sollte ich wählen?

- **Original-TAA** · Die Kantenglättung des Spiels mit manuell einstellbarer Renderauflösung.
- **NVIDIA DLAA** · Glattere Kanten bei 100 %. Auf einer kompatiblen RTX-Karte ist das die erste Wahl, wenn dir Bildqualität am wichtigsten ist.
- **AMD FSR Natives AA** · Kantenglättung von AMD bei 100 % auf unterstützter Hardware.
- **Intel XeSS Natives AA** · Kantenglättung von Intel bei 100 % auf unterstützter Hardware.
- **DLSS / FSR / XeSS Qualität** · Ein guter Einstieg in die Hochskalierung. Das Spiel rendert in einer niedrigeren Auflösung und rekonstruiert das Bild in der gewählten Ausgabeauflösung.
- **Ausgewogen / Leistung** · Diese Modi rendern niedriger als **Qualität**, um die GPU weiter zu entlasten. Feine Details können weicher wirken.

| Ziel | Empfohlene Option |
| --- | --- |
| Bildqualität auf RTX priorisieren | NVIDIA DLAA |
| Kantenglättung in Bildschirmauflösung ohne RTX  |  Natives AA von FSR oder XeSS, sofern verfügbar |
| Mehr FPS  |  DLSS, FSR oder XeSS im Modus **Qualität** |
| Maximale Leistung  |  **Leistung**, oder **Ausgewogen** als Mittelweg |

Verfügbarkeit und Ergebnisse hängen von Grafikkarte, Treiber und Szene ab. Das Menü zeigt die Optionen an, die dein System als verfügbar meldet.

## Funktionen

- Kantenglättung in nativer Auflösung und Hochskalierung mit NVIDIA DLAA/DLSS, AMD FSR und Intel XeSS.
- Einstellungen im Grafikmenü des Spiels mit automatischer Renderauflösung für die SDK-Modi.
- Automatische Spracherkennung für 13 Menüsprachen.
- Installation per ZIP mit Deinstallationsskript. Start über Steam mit Menü und Steuerung des Spiels.

## Systemanforderungen

RE:FRAME benötigt eine legal erworbene Steam-Kopie von Persona 3 Reload, Windows 10 oder 11 x64, DirectX 12 und einen geeigneten Grafiktreiber. Version 1.0.0 ist für **Steam-Build 22672075** (App-ID **2161700**) vorgesehen.

### Unterstützte Grafikkarten

| Technik | Unterstützte Grafikkarten |
| --- | --- |
| **NVIDIA DLAA / DLSS** | Kompatible GeForce-RTX-Grafikkarten mit einem unterstützten Treiber. [NVIDIA DLSS](https://www.nvidia.com/en-us/geforce/technologies/dlss/) |
| **AMD FSR** | Das SDK kann FSR 4.1.2 auf unterstützten Radeon-Karten der Serien RX 6000, 7000 und 9000 anbieten. FSR 3.1.6 ist auf anderen Grafikkarten verfügbar, die die SDK-Anforderungen erfüllen. [AMD FSR SDK](https://gpuopen.com/amd-fsr-sdk/) |
| **Intel XeSS** | Kompatible Intel-Iris-Xe- und Arc-Grafik sowie unterstützte GPUs anderer Hersteller mit Shader Model 6.4 und DP4a-Beschleunigung. [Anforderungen für Intel XeSS](https://github.com/intel/xess/blob/v3.0.2/doc/xess_sr_developer_guide_english.md#requirements) |

XeSS benötigt Microsoft Visual C++ Redistributable x64 **14.40.33810 oder neuer**. Intel nennt für dieses SDK mindestens Build 19043 bei Windows 10 und Build 22000 bei Windows 11. [XeSS-Laufzeit einrichten](https://github.com/intel/xess/blob/v3.0.2/doc/xess_sr_developer_guide_english.md#deployment) · [Microsoft Visual C++ Redistributable](https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist)

## Erweiterte Einstellungen und Modi

Wähle unter **Kantenglättung** eine Technik und anschließend einen verfügbaren **Qualitätsmodus**. **DLSS-Modell** erscheint bei NVIDIA DLSS. Mit **Original-TAA** kannst du die Renderauflösung selbst einstellen. Wenn ein SDK sie steuert, zeigt die Einstellung dessen Zielwert. Bestätige oder verwirf Änderungen über den normalen Dialog des Spiels. Beim Wechsel zu **Original-TAA** kannst du einen neuen manuellen Wert mit derselben Bestätigung übernehmen.

### Verfügbare Modi

| Modus | NVIDIA DLSS | AMD FSR | Intel XeSS |
| --- | --- | --- | --- |
| Natives AA (100%) | DLAA | Natives AA | Natives AA |
| Ultraqualität+ | — | — | Falls verfügbar |
| Ultraqualität | — | — | Falls verfügbar |
| Qualität | ✓ | ✓ | ✓ |
| Ausgewogen | ✓ | ✓ | ✓ |
| Leistung | ✓ | ✓ | ✓ |

Die drei SDKs bieten insgesamt bis zu 14 Modi. **Original-TAA** steht zusätzlich zur Verfügung.

### DLSS-Modelle

| Modell | Angefordertes Modell |
| --- | --- |
| **Auto (empfohlen)** | K für DLAA, **Qualität** und **Ausgewogen**. M für **Leistung**. |
| **DLSS 4 — Standard (K)** | Das Transformer-Modell von DLSS 4. |
| **DLSS 4.5 — Optimiert (M)** | Das von NVIDIA für **Leistung** empfohlene Modell von DLSS 4.5. |
| **DLSS 4.5 — Details (L)** | Das von NVIDIA für Ultra-Leistung in 4K empfohlene Modell von DLSS 4.5. Die Modellauswahl ist unabhängig vom Qualitätsmodus. |

M und L können auf RTX-Karten der Serien 20 und 30 mehr GPU-Zeit beanspruchen. [NVIDIAs Leitfaden zu DLSS-Modellen](https://www.nvidia.com/en-us/geforce/news/dlss-4-5-super-resolution-available-now/)

### Menüsprachen

Englisch, Französisch, Italienisch, Deutsch, Spanisch (Spanien), Japanisch, Koreanisch, Polnisch, Portugiesisch (Brasilien), Russisch, vereinfachtes Chinesisch, traditionelles Chinesisch und Türkisch. RE:FRAME folgt der Spielsprache und wechselt bei Bedarf auf Englisch. [Persona 3 Reload auf Steam](https://store.steampowered.com/app/2161700/Persona_3_Reload/)

## Kompatibilität und Einschränkungen

- **Spielupdates** · Andere Steam-Builds als 22672075, andere Stores, Laufzeitumgebungen außerhalb von Windows und mehrere D3D12-Proxys gleichzeitig sind nicht verifiziert.
- **Rückfall auf TAA** · Bei unbekannten Renderdurchgängen und Nebenansichten wie planaren Spiegelungen verwendet das Spiel **Original-TAA**.
- **Tests** · Version 1.0.0 wurde erfolgreich gebaut und statisch geprüft. Mit früheren Entwicklungsversionen wurden DLAA, DLSS, FSR 3.1.6 und die herstellerübergreifende XeSS-Variante auf einer RTX 5060 getestet. Spieltests für 1.0.0 sowie separate Hardwaretests für FSR 4 auf Radeon und die Intel-optimierte XeSS-Variante auf Arc stehen aus. Die Leistung hängt vom System ab. [Testumfang und Kompatibilität](../COMPATIBILITY.md)

## Fehlerbehebung

- **Die RE:FRAME-Optionen fehlen?** Prüfe den Steam-Build und ob `d3d12.dll` neben `P3R.exe` liegt.
- **Deine bevorzugte Technik fehlt?** Prüfe Grafikkarte, Treiber und die Laufzeitdateien in `P3R\Binaries\Win64\ReFrame`. Kontrolliere für XeSS auch das Visual C++ Redistributable.
- **Die Renderauflösung ist gesperrt?** Das ist bei DLSS, FSR und XeSS normal. Wähle **Original-TAA**, um sie manuell zu ändern.

Weitere Hilfe findest du unter [Fehlerbehebung](../TROUBLESHOOTING.md). Die Mod protokolliert die tatsächliche Renderauflösung und eventuelle Rückfälle in `P3R\Binaries\Win64\ReFrame\ReFrame.log`.

## Aus dem Quellcode bauen

Öffne die **x64 Native Tools Command Prompt für Visual Studio 2022** mit installierter C++-Workload und Windows SDK und führe die folgenden Befehle aus.

```powershell
powershell -NoProfile -File scripts\Fetch-Dependencies.ps1
powershell -NoProfile -File build.ps1
powershell -NoProfile -File scripts\New-Release.ps1
```

[Build-Anleitung](../BUILDING.md) · [Architektur](../ARCHITECTURE.md) · [Änderungsprotokoll](../../CHANGELOG.md) · [Mitwirken](../../CONTRIBUTING.md)

## Lizenz

Copyright © 2026 Aporia. Die MIT-Lizenz gilt für den eigenen Quellcode von RE:FRAME. Für mitgelieferte SDK-Laufzeitbibliotheken, den NVIDIA-NGX-Loader, MinHook/HDE und andere Drittkomponenten gelten ihre jeweiligen Bedingungen und Hinweise. Für das Spiel und seine Inhalte gelten die Bedingungen der jeweiligen Rechteinhaber. [MIT-Lizenz](../../LICENSE) · [Hinweise zu Drittkomponenten](../../THIRD_PARTY_NOTICES.md) · [Herstellerlizenzen](../../licenses/)

## Rechtliche Hinweise

RE:FRAME ist ein unabhängiges, inoffizielles Fanprojekt. Es ist nicht mit ATLUS, SEGA, NVIDIA, AMD oder Intel verbunden und wird von diesen Firmen weder empfohlen noch gesponsert. Alle Marken gehören ihren jeweiligen Inhabern.

RE:FRAME setzt eine legal erworbene Kopie von Persona 3 Reload voraus. Es verbreitet keine Spieldateien, umgeht keinen Kopierschutz (DRM) und schaltet keine DLCs frei. Der Screenshot zeigt die Benutzeroberfläche und Inhalte des Spiels. [Steam-Endnutzerlizenz für Persona 3 Reload](https://store.steampowered.com/eula/2161700_eula_1)
