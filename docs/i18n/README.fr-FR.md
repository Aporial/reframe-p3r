# RE:FRAME — Persona 3 Reload

[English](../../README.md) · **[Français](README.fr-FR.md)** · [Italiano](README.it-IT.md) · [Deutsch](README.de-DE.md) · [Español (España)](README.es-ES.md) · [日本語](README.ja-JP.md) · [한국어](README.ko-KR.md) · [Polski](README.pl-PL.md) · [Português (Brasil)](README.pt-BR.md) · [Русский](README.ru-RU.md) · [简体中文](README.zh-CN.md) · [繁體中文](README.zh-TW.md) · [Türkçe](README.tr-TR.md) · [Українська](README.uk-UA.md)

<!-- source-readme: 8deef3923bb5be1cd0937c169e899628281472fb -->

![Version 1.1.4](https://img.shields.io/badge/release-1.1.4-2469ad) ![Windows x64](https://img.shields.io/badge/platform-Windows%20x64-2469ad) ![DirectX 12](https://img.shields.io/badge/renderer-DirectX%2012-107c10) [![Licence MIT](https://img.shields.io/badge/license-MIT-blue)](../../LICENSE)

Réduisez les contours crénelés et le scintillement dans **Persona 3 Reload** sans régler l’échelle de rendu sur 200 %. DLAA et l’AA natif de FSR ou de XeSS lissent les contours à la résolution de votre écran. Pour gagner en performances, choisissez un mode de mise à l’échelle.

**[Versions publiées de RE:FRAME](https://github.com/Aporial/reframe-p3r/releases/latest)**

![Menu graphique de Persona 3 Reload avec les réglages de RE:FRAME](../images/settings.png)

*Les options de RE:FRAME s’affichent dans le menu graphique du jeu.*

## Installation

1. Fermez le jeu et téléchargez **REFRAME-P3R-v1.1.4.zip** sur [GitHub Releases](https://github.com/Aporial/reframe-p3r/releases/latest).
2. Dans Steam, faites un clic droit sur **Persona 3 Reload**, puis choisissez **Propriétés → Fichiers installés → Parcourir**.
3. Extrayez le ZIP dans le dossier qui contient déjà `P3R`. L’archive place `d3d12.dll` à côté de `P3R.exe` dans `P3R\Binaries\Win64`.
4. Lancez le jeu depuis Steam comme d’habitude.
5. Ouvrez les paramètres graphiques et sélectionnez **Anticrénelage** juste sous l’échelle de rendu (**Rendering Scale**). Choisissez un mode de qualité s’il est proposé, puis confirmez vos modifications.

**Un fichier `d3d12.dll` existe déjà dans `P3R\Binaries\Win64` ?** Il peut appartenir à un autre mod. Identifiez-le et suivez ses instructions de désinstallation avant d’extraire le ZIP.

Le ZIP contient le mod et ses bibliothèques graphiques. Tout se règle dans le menu du jeu, sans ReShade, gestionnaire de mods, surcouche, lanceur séparé ni modification manuelle de la configuration.

### Mise à jour et désinstallation

- **Mise à jour** · Fermez le jeu, désinstallez la version précédente avec son script, puis extrayez le nouveau ZIP. Pour conserver des réglages d’une version de développement, gardez `P3RGraphicsEnhanced\Settings.ini` jusqu’à ce que RE:FRAME enregistre dans `ReFrame\Settings.ini`.
- **Désinstallation** · Fermez le jeu et lancez `Remove-ReFrame.ps1` depuis `P3R\Binaries\Win64\ReFrame`. Le script vérifie les empreintes des fichiers et conserve les réglages et journaux générés. Ajoutez `-WhatIf` pour prévisualiser les opérations. [Guide d’installation et de désinstallation](INSTALLATION.fr-FR.md)

[Installation](#installation) · [Pourquoi ne pas rendre le jeu à 200% ?](#pourquoi-ne-pas-rendre-le-jeu-%C3%A0-200-) · [Quel mode choisir ?](#quel-mode-choisir-) · [Fonctionnalités](#fonctionnalit%C3%A9s) · [Cartes graphiques compatibles](#cartes-graphiques-compatibles) · [Réglages avancés et modes](#r%C3%A9glages-avanc%C3%A9s-et-modes) · [Compatibilité et limites](#compatibilit%C3%A9-et-limites) · [Dépannage](#d%C3%A9pannage) · [Compilation à partir des sources](#compilation-%C3%A0-partir-des-sources) · [Licence](#licence)

## Pourquoi ne pas rendre le jeu à 200% ?

En 1920×1080, les contours en escalier et le scintillement des détails fins peuvent se remarquer. Augmenter l’échelle de rendu peut les atténuer, mais demande plus de travail à la carte graphique.

| Échelle de rendu | Image calculée sur un écran 1080p |
| --- | --- |
| **100%** | 1920×1080, environ 2,1 millions de pixels par image |
| **200%** | 3840×2160, environ 8,3 millions de pixels par image |

**À 200 %, la largeur et la hauteur doublent et le jeu calcule quatre fois plus de pixels.** L’AA natif lisse les contours à 100 % à partir de plusieurs images consécutives. Il a aussi un coût de calcul et le résultat par rapport au rendu à 200 % dépend de la scène.

La mise à l’échelle part d’une image qui contient moins de pixels que l’image affichée. Elle peut augmenter le nombre d’images par seconde lorsque la carte graphique limite les performances.

## Quel mode choisir ?

- **TAA d’origine** · Anticrénelage du jeu avec réglage manuel de l’échelle de rendu.
- **NVIDIA DLAA** · Des contours plus lisses à 100 %. Sur une RTX compatible, commencez par ce mode si la qualité d’image est votre priorité.
- **AMD FSR AA natif** · Anticrénelage d’AMD à 100 % sur le matériel compatible.
- **Intel XeSS AA natif** · Anticrénelage d’Intel à 100 % sur le matériel compatible.
- **DLSS / FSR / XeSS Qualité** · Un bon point de départ pour la mise à l’échelle. Le jeu calcule l’image à une résolution inférieure, puis la reconstruit à la résolution de sortie choisie.
- **Équilibré / Performances** · Le jeu utilise une résolution inférieure à **Qualité** pour alléger davantage la charge graphique. Les détails fins peuvent paraître moins nets.

| Objectif | Choix conseillé |
| --- | --- |
| Privilégier la qualité sur RTX | NVIDIA DLAA |
| Lisser à la résolution de l’écran sans RTX  |  AA natif de FSR ou de XeSS, selon disponibilité |
| Gagner en fluidité  |  DLSS, FSR ou XeSS en mode **Qualité** |
| Privilégier les performances  |  **Performances**, ou **Équilibré** pour un compromis |

Les options et les résultats dépendent de la carte graphique, du pilote et de la scène. Le menu affiche les choix signalés comme disponibles par votre système.

## Fonctionnalités

- Anticrénelage à la résolution de l’écran et mise à l’échelle avec NVIDIA DLAA/DLSS, AMD FSR et Intel XeSS.
- Réglages dans le menu du jeu et échelle de rendu gérée automatiquement pour les modes des SDK.
- Détection automatique parmi 13 langues d’interface.
- Installation depuis un ZIP, avec désinstallation incluse. Lancement depuis Steam et utilisation du menu et des commandes du jeu.

## Configuration requise

RE:FRAME nécessite une copie de Persona 3 Reload achetée légalement sur Steam, Windows 10 ou 11 x64, DirectX 12 et un pilote graphique adapté. La version 1.1.4 cible le **build Steam 22672075** (ID de l’application **2161700**).

### Cartes graphiques compatibles

| Technologie | Cartes compatibles |
| --- | --- |
| **NVIDIA DLAA / DLSS** | Cartes GeForce RTX compatibles avec un pilote pris en charge. [NVIDIA DLSS](https://www.nvidia.com/en-us/geforce/technologies/dlss/) |
| **AMD FSR** | Le SDK peut proposer FSR 4.1.2 sur les Radeon compatibles des séries RX 6000, 7000 et 9000. FSR 3.1.6 est disponible sur d’autres cartes qui répondent aux exigences du SDK. [SDK AMD FSR](https://gpuopen.com/amd-fsr-sdk/) |
| **Intel XeSS** | Intel Iris Xe et Arc compatibles, ainsi que les cartes compatibles d’autres fabricants prenant en charge Shader Model 6.4 et l’accélération DP4a. [Configuration requise pour Intel XeSS](https://github.com/intel/xess/blob/v3.0.2/doc/xess_sr_developer_guide_english.md#requirements) |

XeSS nécessite Microsoft Visual C++ Redistributable x64 **14.40.33810 ou ultérieur**. Intel indique les versions minimales 19043 pour Windows 10 et 22000 pour Windows 11 pour ce SDK. [Installation des bibliothèques XeSS](https://github.com/intel/xess/blob/v3.0.2/doc/xess_sr_developer_guide_english.md#deployment) · [Microsoft Visual C++ Redistributable](https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist)

## Réglages avancés et modes

Choisissez **Anticrénelage**, puis un **Mode de qualité** disponible. La ligne **Modèle DLSS** s’affiche avec NVIDIA DLSS. Avec **TAA d’origine**, réglez l’échelle de rendu manuellement. Si une technologie la gère, le réglage affiche la cible du SDK. Confirmez ou annulez dans la boîte de dialogue habituelle du jeu. Vous pouvez revenir à **TAA d’origine** et choisir une autre échelle manuelle lors de la même validation.

**Netteté** accentue les détails après la reconstruction DLAA, DLSS, FSR ou XeSS et avant l’affichage du HUD. Le réglage va de 0 % (désactivé par défaut) à 100 % par pas de 5 %. Appliquez le réglage pour voir le résultat sans redémarrer. La valeur est conservée entre les technologies et les modes de qualité. Le TAA d’origine masque le réglage.

### Modes disponibles

| Mode | NVIDIA DLSS | AMD FSR | Intel XeSS |
| --- | --- | --- | --- |
| AA natif (100%) | DLAA | AA natif | AA natif |
| Ultra qualité + | — | — | Selon disponibilité |
| Ultra qualité | — | — | Selon disponibilité |
| Qualité | ✓ | ✓ | ✓ |
| Équilibré | ✓ | ✓ | ✓ |
| Performances | ✓ | ✓ | ✓ |

Les trois SDK proposent jusqu’à 14 modes au total. Le **TAA d’origine** est aussi disponible.

### Modèles DLSS

| Modèle | Modèle demandé |
| --- | --- |
| **Auto (recommandé)** | K pour DLAA, **Qualité** et **Équilibré**. M pour **Performances**. |
| **DLSS 4 — Standard (K)** | Le modèle Transformer de DLSS 4. |
| **DLSS 4.5 — Optimisé (M)** | Le modèle DLSS 4.5 recommandé par NVIDIA pour **Performances**. |
| **DLSS 4.5 — Détail (L)** | Le modèle DLSS 4.5 recommandé par NVIDIA pour Ultra performances en 4K. Le modèle se choisit séparément du mode de qualité. |

M et L peuvent demander plus de temps de calcul aux cartes RTX des séries 20 et 30. [Guide NVIDIA des modèles DLSS](https://www.nvidia.com/en-us/geforce/news/dlss-4-5-super-resolution-available-now/)

### Langues de l’interface

Anglais, français, italien, allemand, espagnol (Espagne), japonais, coréen, polonais, portugais (Brésil), russe, chinois simplifié, chinois traditionnel et turc. RE:FRAME suit la langue du jeu et utilise l’anglais si elle n’est pas prise en charge. [Persona 3 Reload sur Steam](https://store.steampowered.com/app/2161700/Persona_3_Reload/)

## Compatibilité et limites

- **Mises à jour du jeu** · Les builds Steam autres que 22672075, les autres boutiques, les environnements hors Windows et l’utilisation de plusieurs proxys D3D12 ensemble restent à vérifier.
- **Solution de repli** · Le jeu utilise **TAA d’origine** pour les étapes de rendu non reconnues et les vues secondaires comme les reflets sur des surfaces planes.
- **Tests** · La netteté a été vérifiée en jeu à 1080p en mode Performances avec DLSS, FSR et XeSS sur une RTX 5060. Les 14 modes SDK disponibles sur cette carte ont été testés hors jeu. FSR 4 sur Radeon et la version de XeSS optimisée pour Intel Arc restent à vérifier sur ces cartes. [Tests et compatibilité](../COMPATIBILITY.md)

## Dépannage

- **Les options de RE:FRAME sont absentes ?** Vérifiez le build Steam et la présence de `d3d12.dll` à côté de `P3R.exe`.
- **La technologie recherchée n’est pas proposée ?** Vérifiez la carte graphique, le pilote et les bibliothèques dans `P3R\Binaries\Win64\ReFrame`. Pour XeSS, vérifiez aussi Microsoft Visual C++ Redistributable.
- **L’échelle de rendu est bloquée ?** C’est normal avec DLSS, FSR ou XeSS. Sélectionnez **TAA d’origine** pour l’ajuster à la main.

Consultez la page [Dépannage](../TROUBLESHOOTING.md) pour obtenir de l’aide. Le mod enregistre la résolution réelle et les solutions de repli utilisées dans `P3R\Binaries\Win64\ReFrame\ReFrame.log`.

## Compilation à partir des sources

Ouvrez l’**invite de commandes des outils natifs x64 pour Visual Studio 2022** avec les outils C++ et le Windows SDK installés, puis exécutez ces commandes.

```powershell
powershell -NoProfile -File scripts\Fetch-Dependencies.ps1
powershell -NoProfile -File build.ps1
powershell -NoProfile -File scripts\New-Release.ps1
```

[Guide de compilation](../BUILDING.md) · [Architecture](../ARCHITECTURE.md) · [Journal des modifications](../../CHANGELOG.md) · [Contribution](../../CONTRIBUTING.md)

## Licence

Copyright © 2026 Aporia. La licence MIT couvre le code source original de RE:FRAME. Les bibliothèques des SDK, le chargeur NVIDIA NGX, MinHook/HDE et les autres composants tiers conservent leurs propres conditions et notices. Le jeu et ses ressources restent soumis aux conditions de leurs propriétaires. [Licence MIT](../../LICENSE) · [Notices des composants tiers](../../THIRD_PARTY_NOTICES.md) · [Licences des fournisseurs](../../licenses/)

## Avis de non-affiliation

RE:FRAME est un projet indépendant et non officiel créé par des fans. Il n’est ni affilié à ATLUS, SEGA, NVIDIA, AMD ou Intel, ni approuvé ou parrainé par ces sociétés. Les marques appartiennent à leurs propriétaires respectifs.

RE:FRAME nécessite une copie de Persona 3 Reload achetée légalement. Il ne distribue pas de fichiers du jeu, ne contourne pas les DRM et ne déverrouille aucun DLC. La capture montre l’interface et des éléments du jeu. [Contrat de licence Steam de Persona 3 Reload](https://store.steampowered.com/eula/2161700_eula_1)
