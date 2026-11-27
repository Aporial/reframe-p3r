# RE:FRAME — Persona 3 Reload

[English](../../README.md) · [Français](README.fr-FR.md) · [Italiano](README.it-IT.md) · [Deutsch](README.de-DE.md) · [Español (España)](README.es-ES.md) · [日本語](README.ja-JP.md) · **[한국어](README.ko-KR.md)** · [Polski](README.pl-PL.md) · [Português (Brasil)](README.pt-BR.md) · [Русский](README.ru-RU.md) · [简体中文](README.zh-CN.md) · [繁體中文](README.zh-TW.md) · [Türkçe](README.tr-TR.md) · [Українська](README.uk-UA.md)

<!-- source-readme: 8deef3923bb5be1cd0937c169e899628281472fb -->

![버전 1.1.4](https://img.shields.io/badge/release-1.1.4-2469ad) ![Windows x64](https://img.shields.io/badge/platform-Windows%20x64-2469ad) ![DirectX 12](https://img.shields.io/badge/renderer-DirectX%2012-107c10) [![MIT 라이선스](https://img.shields.io/badge/license-MIT-blue)](../../LICENSE)

**Persona 3 Reload**의 렌더링 배율을 200%로 높이지 않고도 계단 현상과 깜빡임을 줄일 수 있습니다. DLAA와 FSR·XeSS의 네이티브 AA는 화면 해상도에서 가장자리를 부드럽게 처리합니다. 프레임 속도를 높이고 싶다면 업스케일링 모드를 선택하세요.

**[공개된 RE:FRAME 릴리스](https://github.com/Aporial/reframe-p3r/releases/latest)**

![RE:FRAME 설정이 표시된 Persona 3 Reload의 그래픽 메뉴](../images/settings.png)

*RE:FRAME 설정은 게임 자체의 그래픽 메뉴에 표시됩니다.*

## 설치

1. 게임을 종료한 뒤 [GitHub Releases](https://github.com/Aporial/reframe-p3r/releases/latest)에서 **REFRAME-P3R-v1.1.4.zip**을 다운로드하세요.
2. Steam에서 **Persona 3 Reload**를 마우스 오른쪽 버튼으로 클릭하고 **속성 → 설치된 파일 → 찾아보기**를 선택하세요.
3. `P3R` 폴더가 이미 있는 상위 폴더에 ZIP을 푸세요. `P3R\Binaries\Win64` 내의 `P3R.exe` 옆에 `d3d12.dll`이 배치됩니다.
4. 평소처럼 Steam에서 게임을 실행하세요.
5. 그래픽 설정을 열고 렌더링 배율(**Rendering Scale**) 바로 아래의 **안티앨리어싱 방식**을 선택하세요. 품질 모드가 표시되면 선택하고 변경 사항을 확정하세요.

**`P3R\Binaries\Win64`에 `d3d12.dll`이 이미 있나요?** 다른 모드에서 사용하는 파일일 수 있습니다. 어떤 모드의 파일인지 확인하고, 해당 모드의 제거 절차를 따른 뒤 압축을 푸세요.

ZIP에 모드와 필요한 그래픽 라이브러리가 들어 있습니다. 게임 메뉴에서 설정할 수 있어 ReShade, 외부 모드 로더, 오버레이, 별도 런처, 구성 파일 수동 편집이 필요하지 않습니다.

### 업데이트와 제거

- **업데이트** · 게임을 종료하고 이전 버전의 제거 스크립트를 실행한 다음 새 ZIP을 푸세요. 개발 버전의 설정을 유지하려면 RE:FRAME이 `ReFrame\Settings.ini`에 저장할 때까지 `P3RGraphicsEnhanced\Settings.ini`를 남겨 두세요.
- **제거** · 게임을 종료하고 `P3R\Binaries\Win64\ReFrame`에서 `Remove-ReFrame.ps1`을 실행하세요. 스크립트는 파일 해시를 확인하고 생성된 설정과 로그를 보존합니다. `-WhatIf`을 추가하면 변경 내용을 미리 볼 수 있습니다. [설치 및 제거 안내](INSTALLATION.ko-KR.md)

[설치](#%EC%84%A4%EC%B9%98) · [렌더링 배율을 200%로 올리지 않아도 되는 이유](#%EB%A0%8C%EB%8D%94%EB%A7%81-%EB%B0%B0%EC%9C%A8%EC%9D%84-200%EB%A1%9C-%EC%98%AC%EB%A6%AC%EC%A7%80-%EC%95%8A%EC%95%84%EB%8F%84-%EB%90%98%EB%8A%94-%EC%9D%B4%EC%9C%A0) · [어떤 모드를 선택할까요?](#%EC%96%B4%EB%96%A4-%EB%AA%A8%EB%93%9C%EB%A5%BC-%EC%84%A0%ED%83%9D%ED%95%A0%EA%B9%8C%EC%9A%94) · [주요 기능](#%EC%A3%BC%EC%9A%94-%EA%B8%B0%EB%8A%A5) · [지원 그래픽 카드](#%EC%A7%80%EC%9B%90-%EA%B7%B8%EB%9E%98%ED%94%BD-%EC%B9%B4%EB%93%9C) · [고급 설정과 모드](#%EA%B3%A0%EA%B8%89-%EC%84%A4%EC%A0%95%EA%B3%BC-%EB%AA%A8%EB%93%9C) · [호환성과 제한 사항](#%ED%98%B8%ED%99%98%EC%84%B1%EA%B3%BC-%EC%A0%9C%ED%95%9C-%EC%82%AC%ED%95%AD) · [문제 해결](#%EB%AC%B8%EC%A0%9C-%ED%95%B4%EA%B2%B0) · [소스에서 빌드](#%EC%86%8C%EC%8A%A4%EC%97%90%EC%84%9C-%EB%B9%8C%EB%93%9C) · [라이선스](#%EB%9D%BC%EC%9D%B4%EC%84%A0%EC%8A%A4)

## 렌더링 배율을 200%로 올리지 않아도 되는 이유

1920×1080에서는 톱니 모양의 가장자리와 세밀한 부분의 깜빡임이 눈에 띌 수 있습니다. 렌더링 배율을 높이면 완화될 수 있지만 그래픽 카드의 부담도 커집니다.

| 렌더링 배율 | 1080p 화면에서 게임이 렌더링하는 해상도 |
| --- | --- |
| **100%** | 1920×1080, 프레임당 약 210만 픽셀 |
| **200%** | 3840×2160, 프레임당 약 830만 픽셀 |

**200%에서는 가로와 세로가 모두 두 배가 되어 네 배의 픽셀을 계산합니다.** 네이티브 AA는 연속된 프레임의 정보를 활용해 100% 배율에서 가장자리를 부드럽게 만듭니다. 이 방식도 GPU 자원을 사용하며, 200% 렌더링과 비교한 결과는 장면마다 다릅니다.

업스케일링은 화면에 출력하는 것보다 적은 픽셀을 렌더링합니다. GPU가 성능을 제한할 때 FPS가 높아질 수 있습니다.

## 어떤 모드를 선택할까요?

- **기본 TAA** · 게임에 내장된 안티앨리어싱입니다. 렌더링 배율을 직접 조절할 수 있습니다.
- **NVIDIA DLAA** · 100% 배율에서 가장자리를 부드럽게 처리합니다. 호환되는 RTX를 쓰고 화질이 우선이라면 이 모드부터 사용해 보세요.
- **AMD FSR 네이티브 AA** · 지원되는 하드웨어에서 100% 배율로 사용하는 AMD 안티앨리어싱입니다.
- **Intel XeSS 네이티브 AA** · 지원되는 하드웨어에서 100% 배율로 사용하는 Intel 안티앨리어싱입니다.
- **DLSS / FSR / XeSS 품질** · 업스케일링을 처음 적용하기에 좋은 선택입니다. 낮은 해상도로 렌더링한 뒤 선택한 출력 해상도로 영상을 재구성합니다.
- **균형 / 성능** · **품질**보다 낮은 해상도로 렌더링해 GPU 부하를 더 낮춥니다. 미세한 부분이 덜 선명하게 보일 수 있습니다.

| 목표 | 권장 옵션 |
| --- | --- |
| RTX에서 화질 우선 | NVIDIA DLAA |
| RTX 없이 화면 해상도에서 안티앨리어싱  |  사용 가능한 경우 FSR 또는 XeSS의 **네이티브 AA** |
| 더 높은 FPS  |  DLSS, FSR 또는 XeSS의 **품질** |
| 성능 최우선  |  **성능**, 또는 화질과의 절충을 원하면 **균형** |

사용할 수 있는 옵션과 결과는 GPU, 드라이버, 장면에 따라 달라집니다. 메뉴에는 시스템이 지원한다고 보고한 옵션이 표시됩니다.

## 주요 기능

- NVIDIA DLAA/DLSS, AMD FSR, Intel XeSS를 통한 네이티브 해상도 안티앨리어싱과 업스케일링.
- 게임 내 그래픽 설정과 SDK 모드의 자동 렌더링 배율 조절.
- 인터페이스 언어 13종 자동 감지.
- ZIP 설치 및 제거 스크립트 제공. Steam에서 실행하며 게임의 메뉴와 조작 방식을 그대로 사용합니다.

## 시스템 요구 사항

RE:FRAME을 사용하려면 Steam에서 정식 구매한 Persona 3 Reload, Windows 10 또는 11 x64, DirectX 12와 호환되는 그래픽 드라이버가 필요합니다. 1.1.4의 대상은 **Steam 빌드 22672075**(앱 ID **2161700**)입니다.

### 지원 그래픽 카드

| 기술 | 지원 GPU |
| --- | --- |
| **NVIDIA DLAA / DLSS** | 지원되는 드라이버를 사용하는 호환 GeForce RTX GPU. [NVIDIA DLSS](https://www.nvidia.com/en-us/geforce/technologies/dlss/) |
| **AMD FSR** | 지원되는 Radeon RX 6000, 7000, 9000 시리즈에서는 SDK가 FSR 4.1.2를 제공할 수 있습니다. SDK 요구 사항을 충족하는 다른 GPU는 FSR 3.1.6을 사용할 수 있습니다. [AMD FSR SDK](https://gpuopen.com/amd-fsr-sdk/) |
| **Intel XeSS** | 호환되는 Intel Iris Xe 및 Arc GPU와 Shader Model 6.4 및 DP4a 가속을 지원하는 타사 호환 GPU. [Intel XeSS 요구 사항](https://github.com/intel/xess/blob/v3.0.2/doc/xess_sr_developer_guide_english.md#requirements) |

XeSS는 Microsoft Visual C++ x64 재배포 가능 패키지 **14.40.33810 이상**이 필요합니다. Intel이 제시하는 이 SDK의 최소 요구 버전은 Windows 10 빌드 19043과 Windows 11 빌드 22000입니다. [Intel XeSS 설치 요구 사항](https://github.com/intel/xess/blob/v3.0.2/doc/xess_sr_developer_guide_english.md#deployment) · [Microsoft Visual C++ 재배포 가능 패키지](https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist)

## 고급 설정과 모드

**안티앨리어싱 방식**에서 방식을 고른 뒤 지원되는 **품질 모드**를 선택하세요. NVIDIA DLSS를 사용하면 **DLSS 모델** 항목이 표시됩니다. **기본 TAA**에서는 렌더링 배율을 직접 조절할 수 있으며, SDK가 관리할 때는 해당 SDK의 목표 배율을 표시합니다. 게임의 확인 창에서 변경을 확정하거나 취소할 수 있습니다. **기본 TAA**로 돌아가면서 새 수동 배율을 정하고 한 번에 적용할 수도 있습니다.

**선명도**는 DLAA, DLSS, FSR 또는 XeSS로 재구성한 뒤 HUD를 그리기 전에 화면의 세부 표현을 조절합니다. 기본값은 0%(꺼짐)이며 5% 단위로 100%까지 설정할 수 있습니다. 적용하면 게임을 다시 시작할 필요가 없습니다. 기술이나 품질 모드를 바꿔도 값은 유지됩니다. 기본 TAA에서는 항목이 숨겨집니다.

### 사용 가능한 모드

| 모드 | NVIDIA DLSS | AMD FSR | Intel XeSS |
| --- | --- | --- | --- |
| 네이티브 AA (100%) | DLAA | 네이티브 AA | 네이티브 AA |
| 울트라 품질+ | — | — | 사용 가능한 경우 |
| 울트라 품질 | — | — | 사용 가능한 경우 |
| 품질 | ✓ | ✓ | ✓ |
| 균형 | ✓ | ✓ | ✓ |
| 성능 | ✓ | ✓ | ✓ |

세 SDK를 합치면 최대 14개 모드를 지원합니다. **기본 TAA**도 선택할 수 있습니다.

### DLSS 모델

| 모델 | 요청하는 모델 |
| --- | --- |
| **자동 (권장)** | DLAA, **품질**, **균형**에는 K, **성능**에는 M을 사용합니다. |
| **DLSS 4 — 표준 (K)** | DLSS 4의 트랜스포머 모델입니다. |
| **DLSS 4.5 — 최적화 (M)** | NVIDIA가 **성능**에 권장하는 DLSS 4.5 모델입니다. |
| **DLSS 4.5 — 디테일 (L)** | NVIDIA가 4K 울트라 성능에 권장하는 DLSS 4.5 모델입니다. 품질 모드와 모델은 별도로 선택합니다. |

RTX 20 및 30 시리즈에서는 M과 L의 GPU 처리 시간이 더 길어질 수 있습니다. [NVIDIA의 DLSS 모델 안내](https://www.nvidia.com/en-us/geforce/news/dlss-4-5-super-resolution-available-now/)

### 인터페이스 언어

영어, 프랑스어, 이탈리아어, 독일어, 스페인어(스페인), 일본어, 한국어, 폴란드어, 포르투갈어(브라질), 러시아어, 중국어 간체, 중국어 번체, 튀르키예어. RE:FRAME은 게임의 언어를 따르며 지원되지 않는 언어에서는 영어를 사용합니다. [Steam의 Persona 3 Reload](https://store.steampowered.com/app/2161700/Persona_3_Reload/)

## 호환성과 제한 사항

- **게임 업데이트** · Steam 빌드 22672075 이외의 빌드, 다른 스토어판, Windows 이외의 실행 환경, 여러 D3D12 프록시의 동시 사용은 확인하지 않았습니다.
- **기본 방식으로 복귀** · 게임은 인식되지 않는 렌더링 단계나 평면 반사와 같은 보조 화면에 **기본 TAA**를 사용합니다.
- **테스트** · RTX 5060에서 DLSS, FSR, XeSS의 성능 모드로 1080p 게임 내 선명도를 확인했습니다. 같은 GPU에서 사용 가능한 SDK 모드 14개를 오프라인으로도 확인했습니다. Radeon의 FSR 4와 Intel Arc에 최적화된 XeSS는 해당 하드웨어에서 별도로 확인해야 합니다. [테스트 범위 및 호환성](../COMPATIBILITY.md)

## 문제 해결

- **RE:FRAME 설정이 안 보이나요?** Steam 빌드를 확인하고 `d3d12.dll`이 `P3R.exe` 옆에 있는지 확인하세요.
- **원하는 방식이 안 보이나요?** GPU 지원 여부, 드라이버, `P3R\Binaries\Win64\ReFrame` 안의 라이브러리를 확인하세요. XeSS의 경우 Visual C++ 재배포 가능 패키지도 확인하세요.
- **렌더링 배율을 변경할 수 없나요?** DLSS, FSR 또는 XeSS를 사용하는 동안에는 정상입니다. 직접 조절하려면 **기본 TAA**를 선택하세요.

자세한 내용은 [문제 해결](../TROUBLESHOOTING.md)을 참고하세요. 모드는 실제 렌더링 해상도와 기본 모드로 돌아간 내역을 `P3R\Binaries\Win64\ReFrame\ReFrame.log`에 기록합니다.

## 소스에서 빌드

C++ 워크로드와 Windows SDK가 설치된 **Visual Studio 2022용 x64 네이티브 도구 명령 프롬프트**에서 다음 명령을 실행하세요.

```powershell
powershell -NoProfile -File scripts\Fetch-Dependencies.ps1
powershell -NoProfile -File build.ps1
powershell -NoProfile -File scripts\New-Release.ps1
```

[빌드 안내](../BUILDING.md) · [아키텍처](../ARCHITECTURE.md) · [변경 내역](../../CHANGELOG.md) · [기여 안내](../../CONTRIBUTING.md)

## 라이선스

Copyright © 2026 Aporia. MIT 라이선스는 RE:FRAME의 독자적인 소스 코드에 적용됩니다. 함께 제공되는 SDK 런타임, NVIDIA NGX 로더, MinHook/HDE 및 기타 제3자 구성 요소에는 각자의 조건과 고지가 적용됩니다. 게임과 리소스는 권리자의 조건을 따릅니다. [MIT 라이선스](../../LICENSE) · [제3자 고지](../../THIRD_PARTY_NOTICES.md) · [공급업체 라이선스](../../licenses/)

## 면책 고지

RE:FRAME은 팬이 만든 독립적인 비공식 프로젝트입니다. ATLUS, SEGA, NVIDIA, AMD 또는 Intel과 제휴하지 않았으며, 해당 회사의 보증이나 후원을 받지 않습니다. 모든 상표는 각 권리자에게 속합니다.

RE:FRAME을 사용하려면 Persona 3 Reload를 정식으로 구매해야 합니다. 모드는 게임 파일을 배포하거나 DRM을 우회하거나 DLC를 잠금 해제하지 않습니다. 스크린샷은 인터페이스와 게임 리소스를 보여 줍니다. [Persona 3 Reload Steam 최종 사용자 라이선스 계약](https://store.steampowered.com/eula/2161700_eula_1)
