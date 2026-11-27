# RE:FRAME — Persona 3 Reload 1.1.4

Aporia tarafından hazırlandı. Steam’den yasal olarak satın alınmış Persona 3 Reload için, uygulama kimliği **2161700**, derleme **22672075**. Windows x64 ve DirectX 12 gerekir.

## Yenilikler

Şehir haritası oyunun kendi işleme ölçeğini kullanır ve 3B arka plan harita alanını doldurur. Haritayı kapattığınızda seçtiğiniz yükseltme modu yeniden devreye girer.

**Keskinlik**, DLAA, DLSS, FSR veya XeSS ile görüntü yeniden oluşturulduktan sonra ve HUD çizilmeden önce FidelityFX CAS ile sahne ayrıntılarını ayarlar. Varsayılan değer %0’dır (kapalı). Ayar %5’lik adımlarla %100’e çıkar. Değişikliği grafik menüsünde uygulayın. Yeniden başlatma gerekmez ve değer tüm teknolojiler ile kalite modlarında ortaktır. Orijinal TAA bu ayarı gizler.

## Kurulum ve güncelleme

1. Oyunu kapatın. Steam’de **Özellikler → Yüklü Dosyalar → Göz At** yolunu açın.
2. `P3R\Binaries\Win64` klasöründe `d3d12.dll` varsa dosyanın ait olduğu modun kaldırma adımlarını izleyin. RE:FRAME’i güncellemek için önce önceki sürümün `Remove-ReFrame.ps1` betiğini çalıştırın, ardından yeni ZIP dosyasını çıkarın.
3. ZIP dosyasının SHA-256 değerini `.sha256` dosyasıyla karşılaştırın. **REFRAME-P3R-v1.1.4.zip** arşivini `P3R` klasörünü içeren klasöre çıkarın. Oyunu Steam’den açıp grafik menüsünden teknoloji, kalite modu ve keskinliği seçin.

Ayarları korumak için `ReFrame\Settings.ini` dosyasını saklayın. Eski bir geliştirme sürümünden geçiyorsanız RE:FRAME yeni klasöre kaydedene kadar `P3RGraphicsEnhanced\Settings.ini` dosyasını saklayın. XeSS için Microsoft Visual C++ Redistributable x64 14.40.33810 veya üzeri gerekir.

## Kaldırma

Oyunu kapatın ve `P3R\Binaries\Win64\ReFrame` klasöründen `Remove-ReFrame.ps1` betiğini çalıştırın. `-WhatIf` hangi dosyaların silineceğini gösterir. Betik manifesti ve dosya karmalarını denetler, ayarları ve günlükleri korur.

## Keskinliği kontrol etme

Aynı sahnede teknoloji, kalite modu ve kamera konumunu koruyarak önce %0, sonra %100 uygulayın. Her değişiklikten sonra menüden çıkın. Saçları ve ince dokuları karşılaştırın. Kamerayı hareket ettirip kenarları, HUD’ı ve altyazıları kontrol edin. İptal değişikliği geri almalı, Uygula ise yeniden başlatmadan sonra değeri korumalıdır.

`P3R\Binaries\Win64\ReFrame\ReFrame.log` günlüğünde etkin teknoloji ve CAS uygulanan kare sayısı görünür. Kararlı 240 karelik bir örnekte %0 için `240 frames (0 CAS)`, filtre her karede çalıştığında `240 frames (240 CAS)` görünür.

Keskinlik, bir RTX 5060 üzerinde DLSS, FSR ve XeSS’nin Performans modlarında 1080p’de oyun içinde doğrulandı. Çevrimdışı testler bu GPU’da kullanılabilen 14 SDK modunun tamamını kapsadı. Radeon’da FSR 4 ve Intel Arc için optimize edilmiş XeSS, ilgili donanımda ayrıca test edilmeli.

## Lisans

RE:FRAME bağımsız bir hayran projesidir. `Licenses` klasöründe RE:FRAME’in MIT lisansı ile FidelityFX CAS, üretici çalışma zamanı kitaplıkları ve MinHook’un ayrı lisans bildirimleri bulunur.
