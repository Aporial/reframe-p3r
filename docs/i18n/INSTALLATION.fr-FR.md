# RE:FRAME — Persona 3 Reload 1.1.5

Par Aporia. Pour une copie de Persona 3 Reload acquise légalement sur Steam, ID **2161700**, build **22672075**. Windows x64 et DirectX 12 requis.

## Nouveautés

La carte de la ville peut utiliser le mode de mise à l’échelle DLSS, FSR ou XeSS sélectionné. Si RE:FRAME ne peut pas corriger le décor 3D, la carte reprend l’échelle du jeu jusqu’à sa fermeture.

La **Netteté** utilise FidelityFX CAS pour accentuer les détails après la reconstruction DLAA, DLSS, FSR ou XeSS, avant le dessin du HUD. Le réglage va de 0 % (désactivé par défaut) à 100 % par pas de 5 %. Appliquez le changement dans le menu Graphismes. Il prend effet sans redémarrer et la valeur est commune aux technologies et modes de qualité. Le TAA d’origine masque le réglage.

## Installer ou mettre à jour

1. Fermez le jeu. Dans Steam, ouvrez **Propriétés → Fichiers installés → Parcourir**.
2. Si `P3R\Binaries\Win64` contient déjà un `d3d12.dll`, suivez la procédure de désinstallation du mod qui l’a installé. Pour mettre à jour RE:FRAME, exécutez d’abord `Remove-ReFrame.ps1` de la version précédente, puis extrayez le nouveau ZIP.
3. Comparez le SHA-256 du ZIP avec le fichier `.sha256` fourni. Extrayez **REFRAME-P3R-v1.1.5.zip** dans le dossier qui contient `P3R`. Lancez le jeu dans Steam et choisissez la technologie, le mode de qualité et la netteté dans le menu Graphismes.

Conservez `ReFrame\Settings.ini` pour retrouver vos réglages. Après une version de développement plus ancienne, conservez `P3RGraphicsEnhanced\Settings.ini` jusqu’à ce que RE:FRAME écrive dans le nouveau dossier. XeSS nécessite le redistribuable Microsoft Visual C++ x64 14.40.33810 ou ultérieur.

## Désinstaller

Fermez le jeu et exécutez `Remove-ReFrame.ps1` depuis `P3R\Binaries\Win64\ReFrame`. L’option `-WhatIf` affiche un aperçu. Le script vérifie le manifeste et les empreintes des fichiers, puis conserve les réglages et les journaux.

## Vérifier la netteté

Appliquez 0 % puis 100 % dans la même scène, avec les mêmes technologie, mode de qualité et position de caméra. Quittez le menu après chaque changement. Comparez les cheveux et les textures fines, puis tournez la caméra et vérifiez les contours, le HUD et les sous-titres. Annuler doit écarter un changement et Appliquer doit le conserver après redémarrage.

Le journal `P3R\Binaries\Win64\ReFrame\ReFrame.log` indique la technologie active et le nombre d’images traitées par CAS. Sur 240 images, attendez-vous à `240 frames (0 CAS)` à 0 % et à `240 frames (240 CAS)` si le filtre s’applique à toutes les images.

La netteté a été vérifiée en jeu à 1080p en mode Performances avec DLSS, FSR et XeSS sur une RTX 5060. Les 14 modes SDK disponibles sur cette carte ont été testés hors jeu. FSR 4 sur Radeon et la version de XeSS optimisée pour Intel Arc restent à vérifier sur ces cartes.

## Licence

RE:FRAME est un projet indépendant réalisé par des fans. L’archive inclut la licence MIT du projet et les notices de FidelityFX CAS, des bibliothèques des fabricants et de MinHook dans `Licenses`.
