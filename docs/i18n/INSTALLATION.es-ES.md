# RE:FRAME — Persona 3 Reload 1.1.4

Por Aporia. Para una copia de Persona 3 Reload adquirida legalmente en Steam, ID de aplicación **2161700**, compilación **22672075**. Requiere Windows x64 y DirectX 12.

## Novedades

El mapa de la ciudad usa la escala de renderizado del juego para que el fondo 3D ocupe toda el área del mapa. Al cerrarlo, se vuelve a aplicar el modo de reescalado elegido.

**Nitidez** usa FidelityFX CAS para ajustar el detalle tras la reconstrucción con DLAA, DLSS, FSR o XeSS y antes de dibujar el HUD. Abarca del 0 % (desactivado por defecto) al 100 % en pasos del 5 %. Aplica el ajuste en el menú de gráficos. Se aplica sin reiniciar y el valor se conserva entre tecnologías y modos de calidad. El TAA original oculta el control.

## Instalar o actualizar

1. Cierra el juego. En Steam, abre **Propiedades → Archivos instalados → Explorar**.
2. Si ya hay un `d3d12.dll` en `P3R\Binaries\Win64`, sigue las instrucciones de retirada del mod correspondiente. Para actualizar RE:FRAME, ejecuta primero `Remove-ReFrame.ps1` de la versión anterior y luego extrae el nuevo ZIP.
3. Compara el SHA-256 del ZIP con el archivo adjunto `.sha256`. Extrae **REFRAME-P3R-v1.1.4.zip** en la carpeta que contiene `P3R`. Inicia el juego desde Steam y elige la tecnología, el modo de calidad y la nitidez en el menú de gráficos.

Conserva `ReFrame\Settings.ini` para mantener los ajustes. Si vienes de una versión de desarrollo anterior, conserva `P3RGraphicsEnhanced\Settings.ini` hasta que RE:FRAME guarde en la carpeta nueva. XeSS requiere Microsoft Visual C++ Redistributable x64 14.40.33810 o posterior.

## Desinstalar

Cierra el juego y ejecuta `Remove-ReFrame.ps1` desde `P3R\Binaries\Win64\ReFrame`. Usa `-WhatIf` para previsualizar las eliminaciones. El script verifica el manifiesto y las sumas de los archivos y conserva los ajustes y los registros.

## Comprobar la nitidez

Aplica primero el 0 % y luego el 100 % en la misma escena, con la misma tecnología, modo de calidad y posición de cámara. Sal del menú tras cada cambio. Compara el pelo y las texturas finas. Mueve la cámara y revisa los bordes, el HUD y los subtítulos. Comprueba que Cancelar descarta el cambio y Aplicar lo conserva al reiniciar.

El registro `P3R\Binaries\Win64\ReFrame\ReFrame.log` indica la tecnología activa y los fotogramas con CAS. Con el 0 % debe indicar `240 frames (0 CAS)`. Si el filtro se aplica a toda la muestra, indica `240 frames (240 CAS)`.

Se comprobó la nitidez en el juego a 1080p con DLSS, FSR y XeSS en modo Rendimiento en una RTX 5060. Las pruebas independientes cubrieron los 14 modos de SDK disponibles en esa GPU. Aún faltan pruebas específicas de FSR 4 en Radeon y de XeSS optimizado para Intel en Arc.

## Licencia

RE:FRAME es un proyecto independiente de aficionados. La carpeta `Licenses` incluye la licencia MIT de RE:FRAME y los avisos correspondientes a FidelityFX CAS, las bibliotecas de los fabricantes y MinHook.
