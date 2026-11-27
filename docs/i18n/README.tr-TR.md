# RE:FRAME — Persona 3 Reload

[English](../../README.md) · [Français](README.fr-FR.md) · [Italiano](README.it-IT.md) · [Deutsch](README.de-DE.md) · [Español (España)](README.es-ES.md) · [日本語](README.ja-JP.md) · [한국어](README.ko-KR.md) · [Polski](README.pl-PL.md) · [Português (Brasil)](README.pt-BR.md) · [Русский](README.ru-RU.md) · [简体中文](README.zh-CN.md) · [繁體中文](README.zh-TW.md) · **[Türkçe](README.tr-TR.md)** · [Українська](README.uk-UA.md)

<!-- source-readme: 8deef3923bb5be1cd0937c169e899628281472fb -->

![Sürüm 1.1.4](https://img.shields.io/badge/release-1.1.4-2469ad) ![Windows x64](https://img.shields.io/badge/platform-Windows%20x64-2469ad) ![DirectX 12](https://img.shields.io/badge/renderer-DirectX%2012-107c10) [![Projenin kendi kaynak kodu için MIT Lisansı](https://img.shields.io/badge/license-MIT-blue)](../../LICENSE)

**Persona 3 Reload**'da işleme ölçeğini %200'e çıkarmadan tırtıklı kenarları ve görüntüdeki titreşimi azaltın. DLAA, FSR Yerel AA ve XeSS Yerel AA ekranınızın çözünürlüğünde daha temiz bir görüntü sağlar. Daha yüksek kare hızı için bir çözünürlük yükseltme modu seçebilirsiniz.

**[Yayımlanan RE:FRAME sürümleri](https://github.com/Aporial/reframe-p3r/releases/latest)**

![Persona 3 Reload grafik menüsündeki RE:FRAME ayarları](../images/settings.png)

*RE:FRAME seçenekleri Persona 3 Reload'un kendi grafik menüsünde yer alır.*

## Kurulum

1. Oyunu kapatın ve [GitHub Releases](https://github.com/Aporial/reframe-p3r/releases/latest) sayfasından **REFRAME-P3R-v1.1.4.zip** dosyasını indirin.
2. Steam'de **Persona 3 Reload**'a sağ tıklayın ve **Özellikler → Yüklü Dosyalar → Göz at** yolunu izleyin.
3. ZIP arşivini `P3R` klasörünün bulunduğu konuma çıkarın. Böylece `d3d12.dll` dosyası, `P3R\Binaries\Win64` klasöründe `P3R.exe` dosyasının yanına yerleşir.
4. Oyunu her zamanki gibi doğrudan Steam'den başlatın.
5. Grafik ayarlarını açın ve işleme ölçeğinin (**Rendering Scale**) hemen altındaki **Kenar yumuşatma** seçeneğini kullanın. Varsa bir kalite modu seçin ve değişiklikleri onaylayın.

**`P3R\Binaries\Win64` klasöründe zaten bir `d3d12.dll` dosyası var mı?** Başka bir mod bu dosyayı kullanıyor olabilir. ZIP arşivini çıkarmadan önce dosyanın hangi moda ait olduğunu belirleyin ve o modun kaldırma talimatlarını izleyin.

ZIP arşivinde mod ve gerekli grafik çalışma zamanı kitaplıkları bulunur. Ayarlar oyunun kendi grafik menüsündedir. ReShade, harici mod yükleyici, ekran üstü arayüz, ayrı başlatıcı veya elle yapılandırma gerekmez.

### Güncelleme ve kaldırma

- **Güncelleme** · Oyunu kapatın, önceki sürümün kaldırma betiğini çalıştırın ve yeni ZIP arşivini çıkarın. Geliştirme sürümünden kalan ayarları korumak için RE:FRAME ayarları `ReFrame\Settings.ini` dosyasına kaydedene kadar `P3RGraphicsEnhanced\Settings.ini` dosyasını yerinde bırakın.
- **Kaldırma** · Oyunu kapatın ve `P3R\Binaries\Win64\ReFrame` klasöründeki `Remove-ReFrame.ps1` betiğini çalıştırın. Betik dosya sağlama toplamlarını denetler, oluşturulan ayarları ve günlükleri saklar. Yapılacak işlemleri önceden görmek için `-WhatIf` seçeneğini ekleyin. [Kurulum ve kaldırma kılavuzu](INSTALLATION.tr-TR.md)

[Kurulum](#kurulum) · [Neden işleme ölçeğini %200'e çıkarmak yerine RE:FRAME kullanayım?](#neden-i%C5%9Fleme-%C3%B6l%C3%A7e%C4%9Fini-200e-%C3%A7%C4%B1karmak-yerine-reframe-kullanay%C4%B1m) · [Hangi modu seçmeliyim?](#hangi-modu-se%C3%A7meliyim) · [Özellikler](#%C3%B6zellikler) · [Desteklenen ekran kartları](#desteklenen-ekran-kartlar%C4%B1) · [Gelişmiş ayarlar ve modlar](#geli%C5%9Fmi%C5%9F-ayarlar-ve-modlar) · [Uyumluluk ve sınırlamalar](#uyumluluk-ve-s%C4%B1n%C4%B1rlamalar) · [Sorun giderme](#sorun-giderme) · [Kaynak koddan derleme](#kaynak-koddan-derleme) · [Lisans](#lisans)

## Neden işleme ölçeğini %200'e çıkarmak yerine RE:FRAME kullanayım?

1920×1080 çözünürlükte tırtıklı kenarlar ve ince ayrıntılardaki titreşim göze çarpabilir. Oyunun işleme ölçeğini artırmak bunları azaltabilir, ancak ekran kartına daha fazla iş yükü bindirir.

| İşleme ölçeği | Oyunun 1080p ekranda işlediği çözünürlük |
| --- | --- |
| **100%** | 1920×1080, Kare başına yaklaşık 2,1 milyon piksel |
| **200%** | 3840×2160, Kare başına yaklaşık 8,3 milyon piksel |

**%200 ölçek genişliği ve yüksekliği iki, toplam piksel sayısını dört katına çıkarır.** Yerel AA, ardışık karelerden aldığı bilgileri birleştirerek %100 ölçekte kenarları yumuşatır. Bu işlemin de GPU maliyeti vardır ve %200 ölçeğe kıyasla ne kadar yük getirdiği sahneye göre değişir.

Çözünürlük yükseltme, ekranınızın gösterdiğinden daha az piksel işler. Performansı ekran kartı sınırlıyorsa FPS artabilir.

## Hangi modu seçmeliyim?

- **Orijinal TAA** · Oyunun kendi kenar yumuşatmasıdır. İşleme ölçeğini elle ayarlayabilirsiniz.
- **NVIDIA DLAA** · %100 ölçekte daha düzgün kenarlar sağlar. Görüntü kalitesi önceliğinizse uyumlu bir RTX ekran kartında bu modla başlayın.
- **AMD FSR Yerel AA** · Desteklenen donanımda %100 ölçekte AMD kenar yumuşatması sağlar.
- **Intel XeSS Yerel AA** · Desteklenen donanımda %100 ölçekte Intel kenar yumuşatması sağlar.
- **DLSS / FSR / XeSS Kalite** · Çözünürlük yükseltmeyi denemek için bu modla başlayın. Görüntüyü daha düşük çözünürlükte işler ve seçtiğiniz çıkış çözünürlüğünde yeniden oluşturur.
- **Dengeli / Performans** · Kalite modundan daha düşük bir çözünürlükte çalışarak GPU yükünü azaltır. İnce ayrıntılar biraz daha yumuşak görünebilir.

| Amacınız | Önerilen seçenek |
| --- | --- |
| RTX ekran kartında öncelik görüntü kalitesi | NVIDIA DLAA |
| RTX olmadan yerel çözünürlükte kenar yumuşatma | Varsa FSR Yerel AA veya XeSS Yerel AA |
| Daha yüksek FPS | DLSS, FSR veya XeSS **Kalite** |
| En yüksek performans | **Performans** veya daha dengeli bir sonuç için **Dengeli** |

Kullanılabilirlik ve sonuçlar ekran kartına, sürücüye ve sahneye bağlıdır. Menüde sisteminizin kullanılabilir olarak bildirdiği seçenekler gösterilir.

## Özellikler

- NVIDIA DLAA/DLSS, AMD FSR ve Intel XeSS ile yerel çözünürlükte kenar yumuşatma ve çözünürlük yükseltme.
- Oyunun kendi grafik menüsü ve SDK modlarında otomatik işleme ölçeği.
- Oyun dilini otomatik algılama ve 13 arayüz dili.
- ZIP arşivinden kurulum ve kaldırma. Oyunu Steam'den başlatabilir ve ayarları oyunun kendi menüsünden yönetebilirsiniz.

## Sistem gereksinimleri

Steam'den yasal olarak satın alınmış bir Persona 3 Reload kopyası, Windows 10 veya 11 x64, DirectX 12 ve uygun bir ekran kartı sürücüsü gerekir. RE:FRAME 1.1.4, **Steam derlemesi 22672075** (uygulama kimliği **2161700**) için hazırlanmıştır.

### Desteklenen ekran kartları

| Teknoloji | Desteklenen ekran kartları |
| --- | --- |
| **NVIDIA DLAA / DLSS** | Desteklenen sürücüye sahip uyumlu NVIDIA GeForce RTX ekran kartları. [NVIDIA DLSS](https://www.nvidia.com/en-us/geforce/technologies/dlss/) |
| **AMD FSR** | SDK, desteklenen Radeon RX 6000, 7000 ve 9000 serisi ekran kartlarında FSR 4.1.2 sunabilir. SDK gereksinimlerini karşılayan diğer ekran kartlarında FSR 3.1.6 kullanılabilir. [AMD FSR SDK](https://gpuopen.com/amd-fsr-sdk/) |
| **Intel XeSS** | Uyumlu Intel Iris Xe ve Arc ekran kartları ile Shader Model 6.4 ve DP4a hızlandırmasını destekleyen diğer üreticilerin uyumlu ekran kartları. [Intel XeSS gereksinimleri](https://github.com/intel/xess/blob/v3.0.2/doc/xess_sr_developer_guide_english.md#requirements) |

XeSS için Microsoft Visual C++ x64 Yeniden Dağıtılabilir Paketi'nin **14.40.33810 veya daha yeni** bir sürümü gerekir. Intel bu SDK için Windows 10 derleme 19043 ve Windows 11 derleme 22000'i minimum sürümler olarak listeler. [Intel XeSS dağıtımı](https://github.com/intel/xess/blob/v3.0.2/doc/xess_sr_developer_guide_english.md#deployment) · [Microsoft Visual C++ Yeniden Dağıtılabilir Paketi](https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist)

## Gelişmiş ayarlar ve modlar

**Kenar yumuşatma** bölümünden teknolojiyi, **Kalite modu** bölümünden kullanılabilir bir modu seçin. **DLSS modeli** satırı NVIDIA DLSS seçiliyken görünür. **Orijinal TAA** ile işleme ölçeğini elle ayarlayabilirsiniz. Ölçeği SDK yönetiyorsa satırda SDK'nın hedef değeri gösterilir. Değişiklikleri oyunun kendi onay ekranından kaydedebilir veya iptal edebilirsiniz. Orijinal TAA'ya dönüp işleme ölçeğini tek seferde değiştirebilir ve kaydedebilirsiniz.

**Keskinlik**, DLAA, DLSS, FSR veya XeSS ile yeniden oluşturmadan sonra ve oyun HUD’ı çizmeden önce sahne ayrıntılarını ayarlar. Varsayılan değer %0’dır (kapalı). Ayar %5’lik adımlarla %100’e çıkar. Değişikliği uygulamak için oyunu yeniden başlatmanız gerekmez. Değer, teknoloji ve kalite modu değiştiğinde korunur. Orijinal TAA bu ayarı gizler.

### Kullanılabilir modlar

| Mod | NVIDIA DLSS | AMD FSR | Intel XeSS |
| --- | --- | --- | --- |
| Yerel AA (100%) | DLAA | Yerel AA | Yerel AA |
| Ultra kalite+ | — | — | Varsa |
| Ultra kalite | — | — | Varsa |
| Kalite | ✓ | ✓ | ✓ |
| Dengeli | ✓ | ✓ | ✓ |
| Performans | ✓ | ✓ | ✓ |

Üç SDK toplamda 14'e kadar mod sunar. Oyunun orijinal TAA seçeneği de kullanılabilir.

### DLSS modelleri

| Model | İstenen model |
| --- | --- |
| **Otomatik (önerilen)** | DLAA, Kalite ve Dengeli için K modelini, Performans için M modelini ister. |
| **DLSS 4 — Standart (K)** | DLSS 4 Transformer modeli. |
| **DLSS 4.5 — Optimize (M)** | NVIDIA'nın Performans için önerdiği DLSS 4.5 modeli. |
| **DLSS 4.5 — Ayrıntı (L)** | NVIDIA'nın 4K Ultra Performance için önerdiği DLSS 4.5 modeli. Modeli kalite modundan bağımsız seçebilirsiniz. |

M ve L modelleri RTX 20 ve RTX 30 serisi ekran kartlarında daha fazla GPU süresi gerektirebilir. [NVIDIA'nın DLSS model önerileri](https://www.nvidia.com/en-us/geforce/news/dlss-4-5-super-resolution-available-now/)

### Arayüz dilleri

İngilizce, Fransızca, İtalyanca, Almanca, İspanyolca (İspanya), Japonca, Korece, Lehçe, Portekizce (Brezilya), Rusça, basitleştirilmiş Çince, geleneksel Çince ve Türkçe. RE:FRAME oyunun dilini kullanır. Dil desteklenmiyorsa İngilizce metin gösterilir. [Steam'de Persona 3 Reload](https://store.steampowered.com/app/2161700/Persona_3_Reload/)

## Uyumluluk ve sınırlamalar

- **Oyun güncellemeleri** · 22672075 dışındaki Steam derlemeleri, başka mağazalardaki sürümler, Windows dışındaki çalışma ortamları ve birden fazla D3D12 proxy'sinin birlikte kullanımı doğrulanmamıştır.
- **TAA'ya dönüş** · Oyun, tanınmayan işleme adımlarında ve düzlemsel yansımalar gibi ikincil görünümlerde orijinal TAA kullanır.
- **Testler** · Keskinlik, bir RTX 5060 üzerinde DLSS, FSR ve XeSS’nin Performans modlarında 1080p’de oyun içinde doğrulandı. Çevrimdışı testler bu GPU’da kullanılabilen 14 SDK modunun tamamını kapsadı. Radeon’da FSR 4 ve Intel Arc için optimize edilmiş XeSS, ilgili donanımda ayrıca test edilmeli. [Test kapsamı ve uyumluluk](../COMPATIBILITY.md)

## Sorun giderme

- **RE:FRAME seçenekleri görünmüyor mu?** Steam derlemesini ve `d3d12.dll` dosyasının `P3R.exe` dosyasının yanında olduğunu kontrol edin.
- **İstediğiniz teknoloji görünmüyor mu?** Donanım desteğini, sürücüyü ve `P3R\Binaries\Win64\ReFrame` klasöründeki çalışma zamanı dosyalarını kontrol edin. XeSS için Visual C++ paketinin kurulu olduğunu da doğrulayın.
- **İşleme ölçeği kilitli mi?** DLSS, FSR veya XeSS kullanırken bu beklenen bir durumdur. Elle ayarlamak için **Orijinal TAA** seçeneğini kullanın.

Daha fazla yardım için [Sorun giderme](../TROUBLESHOOTING.md) belgesine bakın. Mod, gerçek işleme çözünürlüğünü ve varsa TAA'ya dönüşü `P3R\Binaries\Win64\ReFrame\ReFrame.log` dosyasına kaydeder.

## Kaynak koddan derleme

C++ iş yükü ve Windows SDK kurulu olan **x64 Native Tools Command Prompt for Visual Studio 2022** ortamında şu komutları çalıştırın.

```powershell
powershell -NoProfile -File scripts\Fetch-Dependencies.ps1
powershell -NoProfile -File build.ps1
powershell -NoProfile -File scripts\New-Release.ps1
```

[Derleme kılavuzu](../BUILDING.md) · [Mimari](../ARCHITECTURE.md) · [Değişiklik günlüğü](../../CHANGELOG.md) · [Katkıda bulunma](../../CONTRIBUTING.md)

## Lisans

Telif hakkı © 2026 Aporia. MIT Lisansı, RE:FRAME projesinin kendi kaynak kodunu kapsar. Paketteki SDK çalışma zamanları, NVIDIA NGX yükleyicisi, MinHook/HDE ve diğer üçüncü taraf bileşenleri kendi lisans koşullarına ve bildirimlerine tabidir. Oyun ve varlıkları, hak sahiplerinin koşullarına tabidir. [MIT Lisansı](../../LICENSE) · [Üçüncü taraf bildirimleri](../../THIRD_PARTY_NOTICES.md) · [Üretici lisansları](../../licenses/)

## Yasal açıklama

RE:FRAME bağımsız ve resmî olmayan bir hayran projesidir. ATLUS, SEGA, NVIDIA, AMD veya Intel ile bağlantısı yoktur ve bu şirketler tarafından onaylanmaz ya da finanse edilmez. Tüm ticari markalar ilgili sahiplerine aittir.

RE:FRAME, yasal olarak satın alınmış bir Persona 3 Reload kopyası gerektirir. Oyun dosyalarını dağıtmaz, DRM'yi atlatmaz ve DLC'lerin kilidini açmaz. Ekran görüntüsü arayüzü belgeler ve oyuna ait görseller içerir. [Persona 3 Reload Steam son kullanıcı lisans sözleşmesi](https://store.steampowered.com/eula/2161700_eula_1)
