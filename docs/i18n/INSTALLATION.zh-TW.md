# RE:FRAME — Persona 3 Reload 1.1.5

作者為 Aporia。適用於在 Steam 合法購買的 Persona 3 Reload，應用程式 ID **2161700**，組建 **22672075**。需要 Windows x64 和 DirectX 12。

## 更新內容

城市地圖可以使用所選的 DLSS、FSR 或 XeSS 超解析度模式。如果無法修正 3D 背景，地圖會恢復遊戲原有的算圖比例，直到地圖關閉。

**銳利度**使用 FidelityFX CAS，在 DLAA、DLSS、FSR 或 XeSS 完成重建後、HUD 繪製前調整場景細節。預設為 0%（關閉），以 5% 為一格調至 100%。在圖形選單套用後立即生效，無須重新啟動。切換技術或畫質模式仍會保留相同數值。原生 TAA 會隱藏此選項。

## 安裝或更新

1. 關閉遊戲。在 Steam 中開啟**內容 → 已安裝檔案 → 瀏覽**。
2. 若 `P3R\Binaries\Win64` 內已有 `d3d12.dll`，請依照原模組的說明移除。 更新 RE:FRAME 時，請先執行舊版的 `Remove-ReFrame.ps1`，再解壓縮新的 ZIP 檔案。
3. 將 ZIP 的 SHA-256 與隨附的 `.sha256` 檔案比對。將 **REFRAME-P3R-v1.1.5.zip** 解壓縮至包含 `P3R` 的資料夾。從 Steam 啟動，在圖形選單選擇技術、畫質模式與銳利度。

保留 `ReFrame\Settings.ini` 以沿用設定。從舊開發版升級時，請保留 `P3RGraphicsEnhanced\Settings.ini`，直到 RE:FRAME 將設定儲存至新資料夾。XeSS 需要 Microsoft Visual C++ x64 可轉散發套件 14.40.33810 或更新版本。

## 解除安裝

關閉遊戲，在 `P3R\Binaries\Win64\ReFrame` 執行 `Remove-ReFrame.ps1`。使用 `-WhatIf` 預覽要移除的檔案。指令碼會驗證清單和檔案雜湊值，並保留產生的設定與記錄。

## 檢查銳利度

在同一場景中維持技術、畫質模式與鏡頭位置，依序套用 0% 和 100%。每次變更後離開選單。比較頭髮與細緻紋理，再移動鏡頭檢查輪廓、HUD 和字幕。確認取消會捨棄變更，而套用的數值在重新啟動後仍保留。

`P3R\Binaries\Win64\ReFrame\ReFrame.log` 會記錄目前啟用的技術與套用 CAS 的影格數。穩定取樣 240 格時，0% 會顯示 `240 frames (0 CAS)`，每格皆套用濾鏡時會顯示 `240 frames (240 CAS)`。

已在 RTX 5060 上以 1080p 驗證 DLSS、FSR 和 XeSS 效能模式的遊戲內銳利度。離線測試涵蓋該顯示卡可用的全部 14 種 SDK 模式。Radeon 上的 FSR 4 和針對 Intel Arc 最佳化的 XeSS 仍須在相應硬體上測試。

## 授權

RE:FRAME 是獨立的同好專案。壓縮檔中的 `Licenses` 包含 RE:FRAME 的 MIT 授權，以及 FidelityFX CAS、各廠商執行階段和 MinHook 的獨立聲明。
