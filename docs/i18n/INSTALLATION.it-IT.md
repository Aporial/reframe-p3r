# RE:FRAME — Persona 3 Reload 1.1.5

Di Aporia. Per una copia di Persona 3 Reload acquistata legalmente su Steam, ID app **2161700**, build **22672075**. Richiede Windows x64 e DirectX 12.

## Novità

La mappa della città può usare la modalità di upscaling DLSS, FSR o XeSS selezionata. Se RE:FRAME non riesce a correggere lo sfondo 3D, la mappa torna alla scala del gioco fino alla chiusura.

**Nitidezza** usa FidelityFX CAS per regolare i dettagli dopo la ricostruzione con DLAA, DLSS, FSR o XeSS e prima del disegno dell’HUD. Va da 0% (disattivata per impostazione predefinita) a 100% in passi del 5%. Applica la modifica nel menu Grafica. Ha effetto senza riavvio e il valore è condiviso tra tecnologie e modalità di qualità. Il TAA originale nasconde il controllo.

## Installazione o aggiornamento

1. Chiudi il gioco. In Steam, apri **Proprietà → File installati → Sfoglia**.
2. Se `P3R\Binaries\Win64` contiene già `d3d12.dll`, segui le istruzioni di rimozione della mod che lo usa. Per aggiornare RE:FRAME, esegui prima `Remove-ReFrame.ps1` della versione precedente e poi estrai il nuovo ZIP.
3. Confronta lo SHA-256 dello ZIP con il file `.sha256` fornito. Estrai **REFRAME-P3R-v1.1.5.zip** nella cartella che contiene `P3R`. Avvia da Steam e imposta tecnologia, modalità qualità e nitidezza nel menu Grafica.

Conserva `ReFrame\Settings.ini` per mantenere le impostazioni. Con una versione di sviluppo precedente, conserva `P3RGraphicsEnhanced\Settings.ini` fino a quando RE:FRAME salva nella nuova cartella. XeSS richiede Microsoft Visual C++ Redistributable x64 14.40.33810 o successivo.

## Disinstallazione

Chiudi il gioco ed esegui `Remove-ReFrame.ps1` da `P3R\Binaries\Win64\ReFrame`. Usa `-WhatIf` per vedere un’anteprima. Lo script controlla il manifesto e le impronte dei file e mantiene impostazioni e registri.

## Verifica della nitidezza

Applica 0% e poi 100% nella stessa scena, con la stessa tecnologia, modalità qualità e posizione della telecamera. Esci dal menu dopo ogni cambio. Confronta capelli e texture fini. Muovi poi la telecamera e verifica contorni, HUD e sottotitoli. Annulla deve scartare una modifica in sospeso e Applica deve conservarla dopo il riavvio.

Il registro `P3R\Binaries\Win64\ReFrame\ReFrame.log` riporta la tecnologia attiva e i fotogrammi trattati da CAS. Un campione stabile mostra `240 frames (0 CAS)` a 0% e `240 frames (240 CAS)` se il filtro lavora su tutti i fotogrammi.

La nitidezza è stata verificata in gioco a 1080p nelle modalità Prestazioni di DLSS, FSR e XeSS su una RTX 5060. I test indipendenti hanno coperto tutte le 14 modalità SDK disponibili su quella GPU. Restano da provare FSR 4 su Radeon e XeSS ottimizzato per Intel su Arc.

## Licenza

RE:FRAME è un progetto indipendente realizzato da appassionati. L’archivio contiene in `Licenses` la licenza MIT di RE:FRAME e le note separate per FidelityFX CAS, le librerie dei produttori e MinHook.
