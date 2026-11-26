# RE:FRAME — Persona 3 Reload 1.1.0 · Aktualizacja ostrości obrazu

Autor to Aporia. Wymagana jest legalnie kupiona na Steam kopia Persona 3 Reload, ID aplikacji **2161700**, kompilacja **22672075**, a także Windows x64 i DirectX 12.

## Co nowego

**Ostrość** używa FidelityFX CAS, aby zmieniać szczegółowość po rekonstrukcji DLAA, DLSS, FSR lub XeSS i przed rysowaniem HUD-u. Zakres wynosi od 0% (domyślnie wyłączona) do 100% co 5%. Zastosuj zmianę w menu grafiki. Działa bez ponownego uruchamiania, a wartość jest wspólna dla technologii i trybów jakości. Oryginalne TAA ukrywa ten element.

## Instalacja lub aktualizacja

1. Zamknij grę. W Steam wybierz **Właściwości → Zainstalowane pliki → Przeglądaj**.
2. Jeśli w `P3R\Binaries\Win64` jest już `d3d12.dll`, postępuj zgodnie z instrukcją usuwania moda korzystającego z tego pliku. Aktualizując wersję testową RE:FRAME 1.1.0, zrób kopię zapasową plików i rozpakuj wersję docelową na tę instalację.
3. Porównaj SHA-256 archiwum z plikiem `.sha256`. Rozpakuj **REFRAME-P3R-v1.1.0.zip** w folderze zawierającym `P3R`. Uruchom grę przez Steam i wybierz technologię, tryb jakości oraz ostrość w menu grafiki.

Zachowaj `ReFrame\Settings.ini`, aby zachować ustawienia. Przy aktualizacji ze starszej wersji rozwojowej zachowaj `P3RGraphicsEnhanced\Settings.ini`, aż RE:FRAME zapisze ustawienia w nowym folderze. XeSS wymaga pakietu Microsoft Visual C++ Redistributable x64 14.40.33810 lub nowszego.

## Odinstalowanie

Zamknij grę i uruchom `Remove-ReFrame.ps1` z `P3R\Binaries\Win64\ReFrame`. `-WhatIf` pozwala zobaczyć listę plików do usunięcia. Skrypt sprawdza manifest i sumy kontrolne plików oraz zachowuje ustawienia i dzienniki.

## Sprawdzanie ostrości

Zastosuj 0%, a potem 100% w tej samej scenie, z tą samą technologią, trybem jakości i położeniem kamery. Za każdym razem zamknij menu. Porównaj włosy i drobne tekstury. Porusz kamerą i sprawdź krawędzie, HUD oraz napisy. Anulowanie powinno odrzucić zmianę, a zastosowanie zachować ją po ponownym uruchomieniu.

Dziennik `P3R\Binaries\Win64\ReFrame\ReFrame.log` pokazuje aktywną technologię i liczbę klatek z zastosowanym CAS. Stabilny pomiar 240 klatek pokaże `240 frames (0 CAS)` przy 0% oraz `240 frames (240 CAS)` przy pełnym pokryciu.

Ostrość sprawdzono w grze w 1080p na RTX 5060 w trybie Wydajność dla DLSS, FSR i XeSS. Testy offline objęły wszystkie 14 dostępnych trybów SDK na tej karcie. FSR 4 na Radeonach i XeSS zoptymalizowany dla Intel Arc nadal wymagają testów na tych kartach.

## Licencja

RE:FRAME jest niezależnym projektem fanowskim. Archiwum zawiera w `Licenses` licencję MIT projektu oraz odrębne informacje licencyjne FidelityFX CAS, bibliotek producentów i MinHook.
