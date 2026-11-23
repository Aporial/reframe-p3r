# RE:FRAME — Persona 3 Reload

[English](../../README.md) · [Français](README.fr-FR.md) · [Italiano](README.it-IT.md) · [Deutsch](README.de-DE.md) · **[Español (España)](README.es-ES.md)** · [日本語](README.ja-JP.md) · [한국어](README.ko-KR.md) · [Polski](README.pl-PL.md) · [Português (Brasil)](README.pt-BR.md) · [Русский](README.ru-RU.md) · [简体中文](README.zh-CN.md) · [繁體中文](README.zh-TW.md) · [Türkçe](README.tr-TR.md) · [Українська](README.uk-UA.md)

<!-- source-readme: 834e57d57f2b49f6b6c8540a9c22bb0042aa92a7 -->

![Versión 1.0.0](https://img.shields.io/badge/release-1.0.0-2469ad) ![Windows x64](https://img.shields.io/badge/platform-Windows%20x64-2469ad) ![DirectX 12](https://img.shields.io/badge/renderer-DirectX%2012-107c10) [![Licencia MIT](https://img.shields.io/badge/license-MIT-blue)](../../LICENSE)

Suaviza los bordes dentados y reduce el parpadeo en **Persona 3 Reload** sin subir la escala de renderizado al 200 %. DLAA y el AA nativo de FSR o XeSS mejoran los contornos a la resolución de tu pantalla. Para ganar fluidez, prueba uno de los modos de escalado.

**[Descargar RE:FRAME v1.0.0](https://github.com/Aporial/reframe-p3r/releases/latest)**

![Menú de gráficos de Persona 3 Reload con los ajustes de RE:FRAME](../images/settings.png)

*Las opciones de RE:FRAME aparecen en el menú de gráficos del juego.*

## Instalación

1. Cierra el juego y descarga **REFRAME-P3R-v1.0.0.zip** desde [Versiones de GitHub](https://github.com/Aporial/reframe-p3r/releases/latest).
2. En Steam, haz clic derecho en **Persona 3 Reload** y selecciona **Propiedades → Archivos instalados → Explorar**.
3. Extrae el ZIP en la carpeta que ya contiene `P3R`. El archivo coloca `d3d12.dll` junto a `P3R.exe` en `P3R\Binaries\Win64`.
4. Inicia el juego desde Steam como de costumbre.
5. Abre los ajustes gráficos y elige **Suavizado de bordes** justo debajo de la escala de renderizado (**Rendering Scale**). Selecciona un modo de calidad si está disponible y confirma los cambios.

**¿Ya hay un `d3d12.dll` en `P3R\Binaries\Win64`?** Podría pertenecer a otro mod. Averigua cuál lo usa y sigue sus instrucciones de desinstalación antes de extraer el ZIP.

El ZIP incluye el mod y sus bibliotecas gráficas. Puedes configurarlo desde el propio juego, sin ReShade, gestores de mods, superposiciones, lanzadores externos ni cambios manuales en los archivos de configuración.

### Actualizar y desinstalar

- **Actualizar** · Cierra el juego, ejecuta el desinstalador de la versión anterior y extrae el nuevo ZIP. Si quieres conservar los ajustes de una versión de desarrollo, mantén `P3RGraphicsEnhanced\Settings.ini` hasta que RE:FRAME guarde en `ReFrame\Settings.ini`.
- **Desinstalar** · Cierra el juego y ejecuta `Remove-ReFrame.ps1` desde `P3R\Binaries\Win64\ReFrame`. El script comprueba los hashes de los archivos y conserva los ajustes y registros generados. Añade `-WhatIf` para ver qué haría. [Guía de instalación y desinstalación](../INSTALLATION.md)

[Instalación](#instalaci%C3%B3n) · [¿Por qué no renderizar al 200%?](#por-qu%C3%A9-no-renderizar-al-200) · [¿Qué modo debería usar?](#qu%C3%A9-modo-deber%C3%ADa-usar) · [Funciones](#funciones) · [Tarjetas gráficas compatibles](#tarjetas-gr%C3%A1ficas-compatibles) · [Ajustes y modos avanzados](#ajustes-y-modos-avanzados) · [Compatibilidad y limitaciones](#compatibilidad-y-limitaciones) · [Solución de problemas](#soluci%C3%B3n-de-problemas) · [Compilar desde el código fuente](#compilar-desde-el-c%C3%B3digo-fuente) · [Licencia](#licencia)

## ¿Por qué no renderizar al 200%?

A 1920×1080 pueden notarse los bordes dentados y el parpadeo de los detalles finos. Aumentar la escala de renderizado puede suavizarlos, pero da más trabajo a la tarjeta gráfica.

| Escala de renderizado | Imagen que se calcula en una pantalla 1080p |
| --- | --- |
| **100%** | 1920×1080, unos 2,1 millones de píxeles por fotograma |
| **200%** | 3840×2160, unos 8,3 millones de píxeles por fotograma |

**Al 200 % se duplican el ancho y el alto, de modo que el juego calcula cuatro veces más píxeles.** El AA nativo suaviza los bordes al 100 % combinando varios fotogramas consecutivos. También tiene un coste de procesamiento y el resultado frente al 200 % depende de la escena.

El escalado parte de una imagen con menos píxeles que la imagen mostrada. Puede aumentar los FPS cuando la tarjeta gráfica es el factor limitante.

## ¿Qué modo debería usar?

- **TAA original** · El suavizado de bordes del juego, con ajuste manual de la escala de renderizado.
- **NVIDIA DLAA** · Bordes más suaves al 100 %. Es un buen punto de partida en una RTX compatible si lo que más te importa es la calidad de imagen.
- **AMD FSR AA nativo** · Suavizado de AMD al 100 % en equipos compatibles.
- **Intel XeSS AA nativo** · Suavizado de Intel al 100 % en equipos compatibles.
- **DLSS / FSR / XeSS Calidad** · Empieza por aquí si vas a usar escalado. El juego renderiza a una resolución inferior y reconstruye la imagen a la resolución de salida elegida.
- **Equilibrado / Rendimiento** · Usan una resolución inferior a **Calidad** para reducir más la carga gráfica. Los detalles finos pueden verse menos nítidos.

| Objetivo | Opción recomendada |
| --- | --- |
| Priorizar la calidad de imagen en RTX | NVIDIA DLAA |
| AA a la resolución de pantalla sin RTX  |  AA nativo de FSR o XeSS, según disponibilidad |
| Más FPS  |  DLSS, FSR o XeSS en modo **Calidad** |
| Máximo rendimiento  |  **Rendimiento**, o **Equilibrado** para un término medio |

La disponibilidad y los resultados dependen de la GPU, el controlador y la escena. El menú muestra las opciones que tu sistema indica como disponibles.

## Funciones

- Suavizado a resolución nativa y escalado mediante NVIDIA DLAA/DLSS, AMD FSR e Intel XeSS.
- Ajustes en el menú del juego con gestión automática de la escala de renderizado en los modos de los SDK.
- Detección automática de 13 idiomas de interfaz.
- Instalación por ZIP con script de desinstalación. Inicio desde Steam con el menú y los controles del juego.

## Requisitos

RE:FRAME requiere una copia de Persona 3 Reload adquirida legalmente en Steam, Windows 10 u 11 x64, DirectX 12 y un controlador gráfico adecuado. La versión 1.0.0 está destinada a la **compilación de Steam 22672075** (ID de aplicación **2161700**).

### Tarjetas gráficas compatibles

| Tecnología | Tarjetas compatibles |
| --- | --- |
| **NVIDIA DLAA / DLSS** | Tarjetas GeForce RTX compatibles con un controlador admitido. [NVIDIA DLSS](https://www.nvidia.com/en-us/geforce/technologies/dlss/) |
| **AMD FSR** | El SDK puede ofrecer FSR 4.1.2 en tarjetas Radeon compatibles de las series RX 6000, 7000 y 9000. FSR 3.1.6 está disponible en otras tarjetas que cumplan los requisitos del SDK. [SDK de AMD FSR](https://gpuopen.com/amd-fsr-sdk/) |
| **Intel XeSS** | Intel Iris Xe y Arc compatibles, además de tarjetas de otros fabricantes compatibles con Shader Model 6.4 y aceleración DP4a. [Requisitos de Intel XeSS](https://github.com/intel/xess/blob/v3.0.2/doc/xess_sr_developer_guide_english.md#requirements) |

XeSS requiere Microsoft Visual C++ Redistributable x64 **14.40.33810 o posterior**. Intel indica como mínimos para este SDK las compilaciones 19043 de Windows 10 y 22000 de Windows 11. [Instalación de las bibliotecas de XeSS](https://github.com/intel/xess/blob/v3.0.2/doc/xess_sr_developer_guide_english.md#deployment) · [Microsoft Visual C++ Redistributable](https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist)

## Ajustes y modos avanzados

Elige una opción en **Suavizado de bordes** y luego un **Modo de calidad** disponible. **Modelo DLSS** aparece al usar NVIDIA DLSS. Con **TAA original** puedes ajustar la escala a mano. Si la gestiona una tecnología, el ajuste muestra el objetivo del SDK. Confirma o cancela en el cuadro de diálogo del juego. Puedes volver a **TAA original** y fijar una escala manual nueva con la misma confirmación.

### Modos disponibles

| Modo | NVIDIA DLSS | AMD FSR | Intel XeSS |
| --- | --- | --- | --- |
| AA nativo (100%) | DLAA | AA nativo | AA nativo |
| Calidad ultra+ | — | — | Si está disponible |
| Calidad ultra | — | — | Si está disponible |
| Calidad | ✓ | ✓ | ✓ |
| Equilibrado | ✓ | ✓ | ✓ |
| Rendimiento | ✓ | ✓ | ✓ |

Los tres SDK ofrecen hasta 14 modos en total. También puedes elegir **TAA original**.

### Modelos DLSS

| Modelo | Modelo que solicita |
| --- | --- |
| **Auto (recomendado)** | K para DLAA, **Calidad** y **Equilibrado**. M para **Rendimiento**. |
| **DLSS 4 — Estándar (K)** | El modelo Transformer de DLSS 4. |
| **DLSS 4.5 — Optimizado (M)** | El modelo DLSS 4.5 que NVIDIA recomienda para **Rendimiento**. |
| **DLSS 4.5 — Detalle (L)** | El modelo DLSS 4.5 que NVIDIA recomienda para Rendimiento ultra en 4K. La elección del modelo es independiente del modo de calidad. |

M y L pueden requerir más tiempo de GPU en las RTX de las series 20 y 30. [Guía de NVIDIA sobre modelos DLSS](https://www.nvidia.com/en-us/geforce/news/dlss-4-5-super-resolution-available-now/)

### Idiomas de la interfaz

Inglés, francés, italiano, alemán, español (España), japonés, coreano, polaco, portugués (Brasil), ruso, chino simplificado, chino tradicional y turco. RE:FRAME sigue el idioma del juego y recurre al inglés si el idioma no es compatible. [Persona 3 Reload en Steam](https://store.steampowered.com/app/2161700/Persona_3_Reload/)

## Compatibilidad y limitaciones

- **Actualizaciones del juego** · No se han verificado otras compilaciones de Steam distintas de la 22672075, otras tiendas, entornos ajenos a Windows ni la combinación de varios proxies de D3D12.
- **Modo de respaldo** · El juego utiliza **TAA original** en pasos de renderizado que no reconoce y en vistas secundarias, como los reflejos planos.
- **Pruebas** · La versión 1.0.0 superó la compilación y las comprobaciones estáticas. En versiones de desarrollo anteriores se probaron DLAA, DLSS, FSR 3.1.6 y XeSS en su variante compatible con varias marcas en una RTX 5060. Quedan pendientes las pruebas en juego de la 1.0.0 y las pruebas específicas de FSR 4 en Radeon y XeSS optimizado para Intel en Arc. El rendimiento depende del equipo. [Pruebas y compatibilidad](../COMPATIBILITY.md)

## Solución de problemas

- **¿No aparecen las opciones de RE:FRAME?** Revisa la compilación de Steam y comprueba que `d3d12.dll` esté junto a `P3R.exe`.
- **¿No sale la tecnología que quieres?** Comprueba la GPU, el controlador y las bibliotecas en `P3R\Binaries\Win64\ReFrame`. Para XeSS, revisa también Microsoft Visual C++ Redistributable.
- **¿No puedes cambiar la escala de renderizado?** Es normal con DLSS, FSR o XeSS. Selecciona **TAA original** para ajustarla a mano.

Si necesitas ayuda, consulta [Solución de problemas](../TROUBLESHOOTING.md). El mod registra la resolución real de renderizado y si ha vuelto al modo de respaldo en `P3R\Binaries\Win64\ReFrame\ReFrame.log`.

## Compilar desde el código fuente

Abre el **símbolo del sistema de herramientas nativas x64 para Visual Studio 2022** con las herramientas de C++ y Windows SDK instaladas y ejecuta estos comandos.

```powershell
powershell -NoProfile -File scripts\Fetch-Dependencies.ps1
powershell -NoProfile -File build.ps1
powershell -NoProfile -File scripts\New-Release.ps1
```

[Guía de compilación](../BUILDING.md) · [Arquitectura](../ARCHITECTURE.md) · [Historial de cambios](../../CHANGELOG.md) · [Cómo contribuir](../../CONTRIBUTING.md)

## Licencia

Copyright © 2026 Aporia. La licencia MIT cubre el código fuente original de RE:FRAME. Las bibliotecas de los SDK, el cargador NVIDIA NGX, MinHook/HDE y los demás componentes de terceros conservan sus propias condiciones y avisos. El juego y sus recursos siguen sujetos a las condiciones de sus titulares. [Licencia MIT](../../LICENSE) · [Avisos de terceros](../../THIRD_PARTY_NOTICES.md) · [Licencias de los proveedores](../../licenses/)

## Aviso legal

RE:FRAME es un proyecto independiente y no oficial creado por aficionados. No está afiliado a ATLUS, SEGA, NVIDIA, AMD ni Intel y no cuenta con su aprobación ni patrocinio. Las marcas pertenecen a sus respectivos titulares.

RE:FRAME requiere una copia de Persona 3 Reload adquirida legalmente. No distribuye archivos del juego, elude el DRM ni desbloquea DLC. La captura documenta la interfaz y muestra elementos del juego. [Acuerdo de licencia de Persona 3 Reload en Steam](https://store.steampowered.com/eula/2161700_eula_1)
