# RE:FRAME — Persona 3 Reload

[English](../../README.md) · [Français](README.fr-FR.md) · **[Italiano](README.it-IT.md)** · [Deutsch](README.de-DE.md) · [Español (España)](README.es-ES.md) · [日本語](README.ja-JP.md) · [한국어](README.ko-KR.md) · [Polski](README.pl-PL.md) · [Português (Brasil)](README.pt-BR.md) · [Русский](README.ru-RU.md) · [简体中文](README.zh-CN.md) · [繁體中文](README.zh-TW.md) · [Türkçe](README.tr-TR.md) · [Українська](README.uk-UA.md)

<!-- source-readme: d635567e66ae8b9728ea8b204baf8079bd1c8590 -->

![Versione 1.1.0](https://img.shields.io/badge/release-1.1.0-2469ad) ![Windows x64](https://img.shields.io/badge/platform-Windows%20x64-2469ad) ![DirectX 12](https://img.shields.io/badge/renderer-DirectX%2012-107c10) [![Licenza MIT](https://img.shields.io/badge/license-MIT-blue)](../../LICENSE)

Riduci le scalettature e lo sfarfallio in **Persona 3 Reload** senza portare la scala di rendering al 200%. DLAA e l’AA nativo di FSR o XeSS migliorano i contorni alla risoluzione del tuo schermo. Per più fluidità, prova l’upscaling.

**[Versioni pubblicate di RE:FRAME](https://github.com/Aporial/reframe-p3r/releases/latest)**

![Menu grafico di Persona 3 Reload con le opzioni RE:FRAME](../images/settings.png)

*Le opzioni di RE:FRAME compaiono nel menu grafico del gioco.*

## Installazione

1. Chiudi il gioco e scarica **REFRAME-P3R-v1.1.0.zip** da [GitHub Releases](https://github.com/Aporial/reframe-p3r/releases/latest).
2. Su Steam, fai clic con il tasto destro su **Persona 3 Reload** e scegli **Proprietà → File installati → Sfoglia**.
3. Estrai lo ZIP nella cartella che contiene già `P3R`. L’archivio colloca `d3d12.dll` accanto a `P3R.exe` in `P3R\Binaries\Win64`.
4. Avvia il gioco normalmente da Steam.
5. Apri le impostazioni grafiche e seleziona **Antialiasing**, subito sotto la scala di rendering (**Rendering Scale**). Scegli una modalità di qualità, se disponibile, e conferma le modifiche.

**In `P3R\Binaries\Win64` c’è già un file `d3d12.dll`?** Potrebbe appartenere a un’altra mod. Individua quale mod lo usa e segui le sue istruzioni di disinstallazione prima di estrarre lo ZIP.

Lo ZIP include la mod e le librerie grafiche necessarie. Configuri tutto dal menu del gioco, senza ReShade, gestori di mod, overlay, launcher separati o modifiche manuali ai file di configurazione.

### Aggiornamento e disinstallazione

- **Aggiornare** · Chiudi il gioco, disinstalla la versione precedente con il suo script ed estrai il nuovo ZIP. Per mantenere le impostazioni di una versione di sviluppo, conserva `P3RGraphicsEnhanced\Settings.ini` finché RE:FRAME non salva in `ReFrame\Settings.ini`.
- **Disinstallare** · Chiudi il gioco ed esegui `Remove-ReFrame.ps1` da `P3R\Binaries\Win64\ReFrame`. Lo script verifica gli hash dei file e conserva impostazioni e log generati. Aggiungi `-WhatIf` per un’anteprima delle operazioni. [Guida all’installazione e alla rimozione](INSTALLATION.it-IT.md)

[Installazione](#installazione) · [Perché non usare la scala di rendering al 200%?](#perch%C3%A9-non-usare-la-scala-di-rendering-al-200) · [Quale modalità scegliere?](#quale-modalit%C3%A0-scegliere) · [Funzionalità](#funzionalit%C3%A0) · [Schede video compatibili](#schede-video-compatibili) · [Impostazioni avanzate e modalità](#impostazioni-avanzate-e-modalit%C3%A0) · [Compatibilità e limiti](#compatibilit%C3%A0-e-limiti) · [Risoluzione dei problemi](#risoluzione-dei-problemi) · [Compilazione dal codice sorgente](#compilazione-dal-codice-sorgente) · [Licenza](#licenza)

## Perché non usare la scala di rendering al 200%?

A 1920×1080 è facile notare bordi a scaletta e sfarfallio sui dettagli sottili. Aumentare la scala di rendering può attenuarli, ma comporta più lavoro per la scheda video.

| Scala di rendering | Immagine renderizzata su uno schermo 1080p |
| --- | --- |
| **100%** | 1920×1080, circa 2,1 milioni di pixel per fotogramma |
| **200%** | 3840×2160, circa 8,3 milioni di pixel per fotogramma |

**Al 200% raddoppiano entrambe le dimensioni e i pixel da calcolare quadruplicano.** L’AA nativo leviga i bordi al 100% combinando informazioni di fotogrammi consecutivi. Richiede a sua volta risorse e il confronto con il 200% dipende dalla scena.

L’upscaling parte da un’immagine con meno pixel di quelli mostrati sullo schermo. Può aumentare gli FPS quando il limite è la scheda video.

## Quale modalità scegliere?

- **TAA originale** · L’antialiasing integrato nel gioco, con scala di rendering regolabile a mano.
- **NVIDIA DLAA** · Contorni più lisci al 100%. Su una RTX compatibile, prova questo modo se dai priorità alla qualità dell’immagine.
- **AMD FSR AA nativo** · Antialiasing AMD al 100% su hardware compatibile.
- **Intel XeSS AA nativo** · Antialiasing Intel al 100% su hardware compatibile.
- **DLSS / FSR / XeSS Qualità** · Il punto di partenza per l’upscaling. Il gioco renderizza a una risoluzione inferiore e ricostruisce l’immagine alla risoluzione di uscita selezionata.
- **Bilanciato / Prestazioni** · Usano una risoluzione inferiore rispetto a **Qualità** per ridurre ancora il carico sulla GPU. I dettagli fini possono apparire meno nitidi.

| Obiettivo | Scelta consigliata |
| --- | --- |
| Privilegiare la qualità su RTX | NVIDIA DLAA |
| Antialiasing alla risoluzione dello schermo senza RTX  |  AA nativo di FSR o XeSS, se disponibile |
| Aumentare gli FPS  |  DLSS, FSR o XeSS in modalità **Qualità** |
| Massime prestazioni  |  **Prestazioni**, oppure **Bilanciato** per un compromesso |

Disponibilità e risultati dipendono da scheda video, driver e scena. Il menu mostra le opzioni che il sistema segnala come utilizzabili.

## Funzionalità

- Antialiasing alla risoluzione nativa e upscaling con NVIDIA DLAA/DLSS, AMD FSR e Intel XeSS.
- Controlli grafici nel menu del gioco, con gestione automatica della scala di rendering per le modalità degli SDK.
- Rilevamento automatico di 13 lingue dell’interfaccia.
- Installazione da ZIP con script di rimozione. Avvio da Steam con il menu e i comandi del gioco.

## Requisiti

RE:FRAME richiede una copia di Persona 3 Reload acquistata legalmente su Steam, Windows 10 o 11 x64, DirectX 12 e un driver grafico adatto. La versione 1.1.0 è pensata per la **build Steam 22672075** (ID app **2161700**).

### Schede video compatibili

| Tecnologia | Schede compatibili |
| --- | --- |
| **NVIDIA DLAA / DLSS** | Schede GeForce RTX compatibili con un driver supportato. [NVIDIA DLSS](https://www.nvidia.com/en-us/geforce/technologies/dlss/) |
| **AMD FSR** | L’SDK può offrire FSR 4.1.2 sulle Radeon compatibili delle serie RX 6000, 7000 e 9000. FSR 3.1.6 è disponibile su altre schede che soddisfano i requisiti dell’SDK. [SDK AMD FSR](https://gpuopen.com/amd-fsr-sdk/) |
| **Intel XeSS** | Hardware Intel Iris Xe e Arc compatibile e schede compatibili di altri produttori con Shader Model 6.4 e accelerazione DP4a. [Requisiti di Intel XeSS](https://github.com/intel/xess/blob/v3.0.2/doc/xess_sr_developer_guide_english.md#requirements) |

XeSS richiede Microsoft Visual C++ Redistributable x64 **14.40.33810 o successivo**. Intel indica come minimi per questo SDK la build 19043 di Windows 10 e la build 22000 di Windows 11. [Distribuzione di Intel XeSS](https://github.com/intel/xess/blob/v3.0.2/doc/xess_sr_developer_guide_english.md#deployment) · [Microsoft Visual C++ Redistributable](https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist)

## Impostazioni avanzate e modalità

Scegli una tecnologia in **Antialiasing**, poi una **Modalità qualità** disponibile. **Modello DLSS** compare con NVIDIA DLSS. Con **TAA originale** puoi regolare la scala manualmente. Quando la tecnologia la gestisce, il valore mostrato è quello indicato dall’SDK. Conferma o annulla con la normale finestra del gioco. Puoi scegliere una nuova scala manuale quando torni a **TAA originale**, con un’unica conferma.

**Nitidezza** regola il dettaglio dopo la ricostruzione DLAA, DLSS, FSR o XeSS e prima che il gioco disegni l’HUD. Va da 0% (disattivata per impostazione predefinita) a 100% a passi del 5%. Applica la modifica per vederla senza riavviare. Il valore resta memorizzato tra tecnologie e modalità di qualità. Il TAA originale nasconde il controllo.

### Modalità disponibili

| Modalità | NVIDIA DLSS | AMD FSR | Intel XeSS |
| --- | --- | --- | --- |
| AA nativo (100%) | DLAA | AA nativo | AA nativo |
| Qualità ultra+ | — | — | Se disponibile |
| Qualità ultra | — | — | Se disponibile |
| Qualità | ✓ | ✓ | ✓ |
| Bilanciato | ✓ | ✓ | ✓ |
| Prestazioni | ✓ | ✓ | ✓ |

I tre SDK offrono fino a 14 modalità in totale. È disponibile anche **TAA originale**.

### Modelli DLSS

| Modello | Modello richiesto |
| --- | --- |
| **Auto (consigliato)** | K per DLAA, **Qualità** e **Bilanciato**. M per **Prestazioni**. |
| **DLSS 4 — Standard (K)** | Il modello Transformer di DLSS 4. |
| **DLSS 4.5 — Ottimizzato (M)** | Il modello DLSS 4.5 che NVIDIA consiglia per **Prestazioni**. |
| **DLSS 4.5 — Dettaglio (L)** | Il modello DLSS 4.5 consigliato da NVIDIA per Ultra prestazioni in 4K. La scelta del modello è separata dalla modalità di qualità. |

I modelli M e L possono richiedere più tempo di elaborazione sulle RTX delle serie 20 e 30. [Guida NVIDIA ai modelli DLSS](https://www.nvidia.com/en-us/geforce/news/dlss-4-5-super-resolution-available-now/)

### Lingue dell’interfaccia

Inglese, francese, italiano, tedesco, spagnolo (Spagna), giapponese, coreano, polacco, portoghese (Brasile), russo, cinese semplificato, cinese tradizionale e turco. RE:FRAME usa la lingua del gioco e, se non è supportata, passa all’inglese. [Persona 3 Reload su Steam](https://store.steampowered.com/app/2161700/Persona_3_Reload/)

## Compatibilità e limiti

- **Aggiornamenti del gioco** · Il supporto per build Steam diverse dalla 22672075, altri negozi, ambienti diversi da Windows e più proxy D3D12 insieme non è stato verificato.
- **Ripiego** · Il gioco usa **TAA originale** per le fasi di rendering non riconosciute e le viste secondarie come i riflessi planari.
- **Test** · La nitidezza è stata verificata in gioco a 1080p nelle modalità Prestazioni di DLSS, FSR e XeSS su una RTX 5060. I test indipendenti hanno coperto tutte le 14 modalità SDK disponibili su quella GPU. Restano da provare FSR 4 su Radeon e XeSS ottimizzato per Intel su Arc. [Test e compatibilità](../COMPATIBILITY.md)

## Risoluzione dei problemi

- **Le opzioni RE:FRAME non compaiono?** Controlla la build Steam e assicurati che `d3d12.dll` si trovi accanto a `P3R.exe`.
- **Non trovi la tecnologia che cerchi?** Verifica la scheda video, il driver e le librerie in `P3R\Binaries\Win64\ReFrame`. Per XeSS controlla anche Microsoft Visual C++ Redistributable.
- **La scala di rendering è bloccata?** È normale con DLSS, FSR o XeSS. Seleziona **TAA originale** per regolarla a mano.

Per ulteriore assistenza, consulta [Risoluzione dei problemi](../TROUBLESHOOTING.md). La mod registra in `P3R\Binaries\Win64\ReFrame\ReFrame.log` la risoluzione effettiva e gli eventuali ripieghi.

## Compilazione dal codice sorgente

Apri il **prompt dei comandi degli strumenti nativi x64 per Visual Studio 2022** con gli strumenti C++ e Windows SDK installati, poi esegui questi comandi.

```powershell
powershell -NoProfile -File scripts\Fetch-Dependencies.ps1
powershell -NoProfile -File build.ps1
powershell -NoProfile -File scripts\New-Release.ps1
```

[Guida alla compilazione](../BUILDING.md) · [Architettura](../ARCHITECTURE.md) · [Registro delle modifiche](../../CHANGELOG.md) · [Come contribuire](../../CONTRIBUTING.md)

## Licenza

Copyright © 2026 Aporia. La licenza MIT copre il codice sorgente originale di RE:FRAME. Le librerie degli SDK, il loader NVIDIA NGX, MinHook/HDE e gli altri componenti di terze parti mantengono le proprie condizioni e note legali. Il gioco e le sue risorse restano soggetti alle condizioni dei rispettivi titolari. [Licenza MIT](../../LICENSE) · [Note sulle terze parti](../../THIRD_PARTY_NOTICES.md) · [Licenze dei fornitori](../../licenses/)

## Dichiarazione di non affiliazione

RE:FRAME è un progetto indipendente e non ufficiale realizzato da fan. Non è affiliato ad ATLUS, SEGA, NVIDIA, AMD o Intel, né approvato o sponsorizzato da queste società. Tutti i marchi appartengono ai rispettivi titolari.

RE:FRAME richiede una copia di Persona 3 Reload acquistata legalmente. Non distribuisce file del gioco, non aggira il DRM e non sblocca DLC. La schermata documenta l’interfaccia e mostra elementi del gioco. [Contratto di licenza Steam di Persona 3 Reload](https://store.steampowered.com/eula/2161700_eula_1)
