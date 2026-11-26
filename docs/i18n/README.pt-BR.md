# RE:FRAME — Persona 3 Reload

[English](../../README.md) · [Français](README.fr-FR.md) · [Italiano](README.it-IT.md) · [Deutsch](README.de-DE.md) · [Español (España)](README.es-ES.md) · [日本語](README.ja-JP.md) · [한국어](README.ko-KR.md) · [Polski](README.pl-PL.md) · **[Português (Brasil)](README.pt-BR.md)** · [Русский](README.ru-RU.md) · [简体中文](README.zh-CN.md) · [繁體中文](README.zh-TW.md) · [Türkçe](README.tr-TR.md) · [Українська](README.uk-UA.md)

<!-- source-readme: d635567e66ae8b9728ea8b204baf8079bd1c8590 -->

![Versão 1.1.0](https://img.shields.io/badge/release-1.1.0-2469ad) ![Windows x64](https://img.shields.io/badge/platform-Windows%20x64-2469ad) ![DirectX 12](https://img.shields.io/badge/renderer-DirectX%2012-107c10) [![Licença MIT](https://img.shields.io/badge/license-MIT-blue)](../../LICENSE)

Suavize os serrilhados e reduza a cintilação em **Persona 3 Reload** sem ajustar a escala de renderização para 200%. DLAA e o AA nativo do FSR ou do XeSS suavizam os contornos na resolução da sua tela. Para ganhar fluidez, experimente um modo de upscaling.

**[Versões publicadas do RE:FRAME](https://github.com/Aporial/reframe-p3r/releases/latest)**

![Menu de gráficos de Persona 3 Reload com as opções do RE:FRAME](../images/settings.png)

*As opções do RE:FRAME ficam no próprio menu de gráficos do jogo.*

## Instalação

1. Feche o jogo e baixe **REFRAME-P3R-v1.1.0.zip** em [GitHub Releases](https://github.com/Aporial/reframe-p3r/releases/latest).
2. No Steam, clique com o botão direito em **Persona 3 Reload** e abra **Propriedades → Arquivos instalados → Procurar**.
3. Extraia o ZIP na pasta que já contém `P3R`. O arquivo coloca `d3d12.dll` ao lado de `P3R.exe` em `P3R\Binaries\Win64`.
4. Inicie o jogo pelo Steam normalmente.
5. Abra as configurações de gráficos e selecione **Antisserrilhamento** logo abaixo da escala de renderização (**Rendering Scale**). Escolha um modo de qualidade, se disponível, e confirme as alterações.

**Já existe um `d3d12.dll` em `P3R\Binaries\Win64`?** Ele pode pertencer a outro mod. Descubra qual mod o utiliza e siga as instruções de remoção dele antes de extrair o ZIP.

O ZIP inclui o mod e as bibliotecas gráficas necessárias. A configuração é feita no menu do jogo, sem ReShade, gerenciador de mods, sobreposição, inicializador separado ou edição manual de arquivos de configuração.

### Atualização e desinstalação

- **Atualização** · Feche o jogo, execute o desinstalador da versão anterior e extraia o novo ZIP. Para manter os ajustes de uma versão de desenvolvimento, preserve `P3RGraphicsEnhanced\Settings.ini` até o RE:FRAME gravar em `ReFrame\Settings.ini`.
- **Desinstalação** · Feche o jogo e execute `Remove-ReFrame.ps1` em `P3R\Binaries\Win64\ReFrame`. O script verifica os hashes e preserva as configurações e os logs gerados. Use `-WhatIf` para conferir o que seria removido. [Guia de instalação e remoção](INSTALLATION.pt-BR.md)

[Instalação](#instala%C3%A7%C3%A3o) · [Por que não usar escala de renderização de 200%?](#por-que-n%C3%A3o-usar-escala-de-renderiza%C3%A7%C3%A3o-de-200) · [Qual modo usar?](#qual-modo-usar) · [Recursos](#recursos) · [Placas de vídeo compatíveis](#placas-de-v%C3%ADdeo-compat%C3%ADveis) · [Configurações e modos avançados](#configura%C3%A7%C3%B5es-e-modos-avan%C3%A7ados) · [Compatibilidade e limitações](#compatibilidade-e-limita%C3%A7%C3%B5es) · [Solução de problemas](#solu%C3%A7%C3%A3o-de-problemas) · [Compilação a partir do código-fonte](#compila%C3%A7%C3%A3o-a-partir-do-c%C3%B3digo-fonte) · [Licença](#licen%C3%A7a)

## Por que não usar escala de renderização de 200%?

Em 1920×1080, serrilhados e cintilação em detalhes finos podem chamar a atenção. Aumentar a escala de renderização pode suavizar a imagem, mas exige mais da placa de vídeo.

| Escala de renderização | Imagem renderizada em uma tela 1080p |
| --- | --- |
| **100%** | 1920×1080, cerca de 2,1 milhões de pixels por quadro |
| **200%** | 3840×2160, cerca de 8,3 milhões de pixels por quadro |

**Em 200%, a largura e a altura dobram. O jogo calcula quatro vezes mais pixels.** O AA nativo usa informações de quadros consecutivos para suavizar os contornos em 100%. Ele também tem um custo de processamento e o resultado em relação à escala de 200% varia com a cena.

O upscaling parte de uma imagem com menos pixels do que a tela exibe. Pode aumentar os FPS quando o limite é a placa de vídeo.

## Qual modo usar?

- **TAA original** · O antisserrilhamento do próprio jogo, com controle manual da escala de renderização.
- **NVIDIA DLAA** · Contornos mais suaves em 100%. Em uma RTX compatível, comece por aqui se a qualidade da imagem for a prioridade.
- **AMD FSR AA nativo** · Antisserrilhamento da AMD em 100% em hardware compatível.
- **Intel XeSS AA nativo** · Antisserrilhamento da Intel em 100% em hardware compatível.
- **DLSS / FSR / XeSS Qualidade** · A primeira opção para experimentar upscaling. O jogo renderiza em uma resolução menor e reconstrói a imagem na resolução de saída escolhida.
- **Equilibrado / Desempenho** · Renderizam abaixo da resolução de **Qualidade** para aliviar ainda mais a GPU. Os detalhes finos podem ficar menos nítidos.

| Objetivo | Opção recomendada |
| --- | --- |
| Priorizar a qualidade de imagem em RTX | NVIDIA DLAA |
| AA na resolução da tela sem RTX  |  AA nativo do FSR ou do XeSS, quando disponível |
| Mais FPS  |  DLSS, FSR ou XeSS no modo **Qualidade** |
| Desempenho máximo  |  **Desempenho**, ou **Equilibrado** para um meio-termo |

A disponibilidade e os resultados dependem da GPU, do driver e da cena. O menu exibe as opções que o sistema informa estarem disponíveis.

## Recursos

- Antisserrilhamento em resolução nativa e upscaling com NVIDIA DLAA/DLSS, AMD FSR e Intel XeSS.
- Ajustes gráficos no próprio jogo com escala de renderização automática nos modos dos SDKs.
- Detecção automática de 13 idiomas da interface.
- Instalação por ZIP com script de remoção. Inicialização pelo Steam com menus e controles do jogo.

## Requisitos

O RE:FRAME requer uma cópia de Persona 3 Reload comprada legalmente no Steam, Windows 10 ou 11 x64, DirectX 12 e um driver gráfico adequado. A versão 1.1.0 é destinada à **build 22672075 do Steam** (ID do aplicativo **2161700**).

### Placas de vídeo compatíveis

| Tecnologia | Placas compatíveis |
| --- | --- |
| **NVIDIA DLAA / DLSS** | Placas GeForce RTX compatíveis com um driver aceito. [NVIDIA DLSS](https://www.nvidia.com/en-us/geforce/technologies/dlss/) |
| **AMD FSR** | O SDK pode oferecer FSR 4.1.2 em modelos Radeon compatíveis das séries RX 6000, 7000 e 9000. O FSR 3.1.6 está disponível em outras GPUs que atendem aos requisitos do SDK. [SDK do AMD FSR](https://gpuopen.com/amd-fsr-sdk/) |
| **Intel XeSS** | GPUs Intel Iris Xe e Arc compatíveis, além de modelos compatíveis de outros fabricantes com Shader Model 6.4 e aceleração DP4a. [Requisitos do Intel XeSS](https://github.com/intel/xess/blob/v3.0.2/doc/xess_sr_developer_guide_english.md#requirements) |

O XeSS exige Microsoft Visual C++ Redistributable x64 **14.40.33810 ou posterior**. A Intel indica as builds 19043 do Windows 10 e 22000 do Windows 11 como mínimas para esse SDK. [Instalação das bibliotecas do XeSS](https://github.com/intel/xess/blob/v3.0.2/doc/xess_sr_developer_guide_english.md#deployment) · [Microsoft Visual C++ Redistributable](https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist)

## Configurações e modos avançados

Selecione uma tecnologia em **Antisserrilhamento** e escolha um **Modo de qualidade** disponível. A opção **Modelo DLSS** aparece com NVIDIA DLSS. Com **TAA original**, a escala de renderização pode ser editada manualmente. Se uma tecnologia controlar a escala, o ajuste mostra o valor alvo do SDK. Use a caixa de confirmação do jogo para aplicar ou cancelar. Você pode voltar ao **TAA original** e aplicar uma nova escala manual na mesma confirmação.

**Nitidez** ajusta os detalhes após a reconstrução com DLAA, DLSS, FSR ou XeSS e antes de o jogo desenhar o HUD. Vai de 0% (desativada por padrão) a 100% em passos de 5%. Aplique o ajuste para ver a diferença sem reiniciar. O valor é compartilhado entre tecnologias e modos de qualidade. O TAA original oculta o controle.

### Modos disponíveis

| Modo | NVIDIA DLSS | AMD FSR | Intel XeSS |
| --- | --- | --- | --- |
| AA nativo (100%) | DLAA | AA nativo | AA nativo |
| Qualidade ultra+ | — | — | Quando disponível |
| Qualidade ultra | — | — | Quando disponível |
| Qualidade | ✓ | ✓ | ✓ |
| Equilibrado | ✓ | ✓ | ✓ |
| Desempenho | ✓ | ✓ | ✓ |

Os três SDKs oferecem até 14 modos no total. O **TAA original** também está disponível.

### Modelos DLSS

| Modelo | Modelo solicitado |
| --- | --- |
| **Auto (recomendado)** | K para DLAA, **Qualidade** e **Equilibrado**. M para **Desempenho**. |
| **DLSS 4 — Padrão (K)** | O modelo Transformer do DLSS 4. |
| **DLSS 4.5 — Otimizado (M)** | O modelo DLSS 4.5 que a NVIDIA recomenda para **Desempenho**. |
| **DLSS 4.5 — Detalhe (L)** | O modelo DLSS 4.5 que a NVIDIA recomenda para Ultra desempenho em 4K. O modelo é escolhido separadamente do modo de qualidade. |

M e L podem consumir mais tempo de GPU nas RTX das séries 20 e 30. [Guia da NVIDIA sobre modelos DLSS](https://www.nvidia.com/en-us/geforce/news/dlss-4-5-super-resolution-available-now/)

### Idiomas da interface

Inglês, francês, italiano, alemão, espanhol (Espanha), japonês, coreano, polonês, português (Brasil), russo, chinês simplificado, chinês tradicional e turco. O RE:FRAME acompanha o idioma do jogo e usa inglês como alternativa. [Persona 3 Reload no Steam](https://store.steampowered.com/app/2161700/Persona_3_Reload/)

## Compatibilidade e limitações

- **Atualizações do jogo** · Não foi verificado o suporte a builds do Steam diferentes da 22672075, outras lojas, ambientes fora do Windows ou vários proxies D3D12 ao mesmo tempo.
- **Modo alternativo** · O jogo usa **TAA original** em etapas de renderização não reconhecidas e em vistas secundárias, como reflexos em superfícies planas.
- **Testes** · A nitidez foi verificada no jogo a 1080p, no modo Desempenho de DLSS, FSR e XeSS, em uma RTX 5060. Os testes offline cobriram os 14 modos de SDK disponíveis nessa GPU. FSR 4 em Radeon e XeSS otimizado para Intel Arc ainda precisam de testes nesses dispositivos. [Testes e compatibilidade](../COMPATIBILITY.md)

## Solução de problemas

- **As opções do RE:FRAME não aparecem?** Confira a build do Steam e certifique-se de que `d3d12.dll` esteja ao lado de `P3R.exe`.
- **A tecnologia que você quer não aparece?** Confira a placa de vídeo, o driver e as bibliotecas em `P3R\Binaries\Win64\ReFrame`. Para XeSS, confira também o Microsoft Visual C++ Redistributable.
- **A escala de renderização está bloqueada?** É o comportamento esperado com DLSS, FSR ou XeSS. Selecione **TAA original** para alterá-la manualmente.

Para mais ajuda, consulte [Solução de problemas](../TROUBLESHOOTING.md). O mod registra a resolução real de renderização e eventuais retornos ao modo alternativo em `P3R\Binaries\Win64\ReFrame\ReFrame.log`.

## Compilação a partir do código-fonte

Abra o **Prompt de Comando das Ferramentas Nativas x64 para Visual Studio 2022**, com as ferramentas de C++ e o Windows SDK instalados, e execute os comandos abaixo.

```powershell
powershell -NoProfile -File scripts\Fetch-Dependencies.ps1
powershell -NoProfile -File build.ps1
powershell -NoProfile -File scripts\New-Release.ps1
```

[Guia de compilação](../BUILDING.md) · [Arquitetura](../ARCHITECTURE.md) · [Histórico de alterações](../../CHANGELOG.md) · [Como contribuir](../../CONTRIBUTING.md)

## Licença

Copyright © 2026 Aporia. A licença MIT cobre o código-fonte original do RE:FRAME. As bibliotecas dos SDKs, o carregador NVIDIA NGX, MinHook/HDE e os demais componentes de terceiros mantêm seus próprios termos e avisos. O jogo e seus recursos continuam sujeitos aos termos de seus titulares. [Licença MIT](../../LICENSE) · [Avisos de terceiros](../../THIRD_PARTY_NOTICES.md) · [Licenças dos fornecedores](../../licenses/)

## Aviso legal

O RE:FRAME é um projeto independente e não oficial feito por fãs. Não tem vínculo, aprovação ou patrocínio de ATLUS, SEGA, NVIDIA, AMD ou Intel. Todas as marcas pertencem a seus respectivos titulares.

O RE:FRAME requer uma cópia de Persona 3 Reload comprada legalmente. Não distribui arquivos do jogo, não contorna o DRM e não desbloqueia DLCs. A captura de tela documenta a interface e mostra elementos do jogo. [Contrato de licença de Persona 3 Reload no Steam](https://store.steampowered.com/eula/2161700_eula_1)
