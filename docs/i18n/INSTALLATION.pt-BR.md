# RE:FRAME — Persona 3 Reload 1.1.4

Por Aporia. Para uma cópia de Persona 3 Reload adquirida legalmente pelo Steam, ID do aplicativo **2161700**, build **22672075**. Requer Windows x64 e DirectX 12.

## Novidades

O mapa da cidade usa a escala de renderização do jogo para que o cenário 3D preencha toda a área do mapa. Ao fechar o mapa, o jogo volta ao modo de upscaling selecionado.

A **Nitidez** usa FidelityFX CAS para ajustar os detalhes após a reconstrução por DLAA, DLSS, FSR ou XeSS, antes de desenhar o HUD. Vai de 0% (desativada por padrão) a 100% em passos de 5%. Aplique a mudança no menu de gráficos. Ela entra em vigor sem reiniciar, e o mesmo valor vale para todas as tecnologias e modos de qualidade. O TAA original oculta o controle.

## Instalar ou atualizar

1. Feche o jogo. No Steam, abra **Propriedades → Arquivos instalados → Explorar**.
2. Se `P3R\Binaries\Win64` já tiver um `d3d12.dll`, siga as instruções de remoção do mod correspondente. Para atualizar o RE:FRAME, execute primeiro o `Remove-ReFrame.ps1` da versão anterior e, em seguida, extraia o novo ZIP.
3. Compare o SHA-256 do ZIP com o arquivo `.sha256` fornecido. Extraia **REFRAME-P3R-v1.1.4.zip** na pasta que contém `P3R`. Inicie pelo Steam e selecione a tecnologia, o modo de qualidade e a nitidez no menu de gráficos.

Mantenha `ReFrame\Settings.ini` para preservar suas configurações. Se estiver atualizando uma versão de desenvolvimento antiga, mantenha `P3RGraphicsEnhanced\Settings.ini` até o RE:FRAME salvar na nova pasta. O XeSS requer Microsoft Visual C++ Redistributable x64 14.40.33810 ou posterior.

## Desinstalar

Feche o jogo e execute `Remove-ReFrame.ps1` em `P3R\Binaries\Win64\ReFrame`. Use `-WhatIf` para visualizar o que será removido. O script confere o manifesto e os hashes dos arquivos e preserva as configurações e os registros.

## Verificar a nitidez

Aplique 0% e depois 100% na mesma cena, com a mesma tecnologia, modo de qualidade e posição da câmera. Saia do menu após cada alteração. Compare cabelos e texturas finas. Depois mova a câmera e confira contornos, HUD e legendas. Confira se Cancelar descarta a alteração e se Aplicar salva o valor após reiniciar.

O registro `P3R\Binaries\Win64\ReFrame\ReFrame.log` informa a tecnologia ativa e os quadros processados pelo CAS. Uma amostra estável mostra `240 frames (0 CAS)` a 0% e `240 frames (240 CAS)` quando o filtro processa todos os quadros.

A nitidez foi verificada no jogo a 1080p, no modo Desempenho de DLSS, FSR e XeSS, em uma RTX 5060. Os testes offline cobriram os 14 modos de SDK disponíveis nessa GPU. FSR 4 em Radeon e XeSS otimizado para Intel Arc ainda precisam de testes nesses dispositivos.

## Licença

RE:FRAME é um projeto independente feito por fãs. O arquivo inclui a licença MIT do RE:FRAME e os avisos separados do FidelityFX CAS, dos runtimes dos fabricantes e do MinHook em `Licenses`.
