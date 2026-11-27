# RE:FRAME — Persona 3 Reload 1.1.5

作者为 Aporia。适用于在 Steam 合法购买的 Persona 3 Reload，应用 ID **2161700**，构建版本 **22672075**。需要 Windows x64 和 DirectX 12。

## 更新内容

城市地图可以使用所选的 DLSS、FSR 或 XeSS 超分辨率模式。如果无法修正 3D 背景，地图将恢复游戏原有的渲染比例，直到地图关闭。

**锐度**使用 FidelityFX CAS，在 DLAA、DLSS、FSR 或 XeSS 完成重建后、HUD 绘制前调整场景细节。默认为 0%（关闭），可按 5% 的步长调至 100%。在图形菜单应用后立即生效，无需重启。切换技术或画质模式时会保留同一个值。原生 TAA 会隐藏此选项。

## 安装或更新

1. 关闭游戏。在 Steam 中打开**属性 → 已安装文件 → 浏览**。
2. 如果 `P3R\Binaries\Win64` 中已有 `d3d12.dll`，请按照所属模组的说明卸载。 更新 RE:FRAME 时，请先运行旧版本的 `Remove-ReFrame.ps1`，再解压新的 ZIP 文件。
3. 将 ZIP 的 SHA-256 与随附的 `.sha256` 文件比对。将 **REFRAME-P3R-v1.1.5.zip** 解压到包含 `P3R` 的文件夹。通过 Steam 启动游戏，在图形菜单选择技术、画质模式和锐度。

保留 `ReFrame\Settings.ini` 以便沿用设置。如果从较早的开发版升级，请保留 `P3RGraphicsEnhanced\Settings.ini`，直到 RE:FRAME 将设置保存到新文件夹。XeSS 需要 Microsoft Visual C++ x64 可再发行组件 14.40.33810 或更高版本。

## 卸载

关闭游戏，在 `P3R\Binaries\Win64\ReFrame` 中运行 `Remove-ReFrame.ps1`。可用 `-WhatIf` 预览删除项。脚本会验证清单和文件哈希，并保留生成的设置与日志。

## 检查锐度

在同一场景中保持技术、画质模式和镜头位置不变，依次应用 0% 和 100%。每次更改后退出菜单。对比头发和细小纹理，再移动镜头检查轮廓、HUD 和字幕。确认取消会放弃更改，而应用后的值在重启后仍然保留。

`P3R\Binaries\Win64\ReFrame\ReFrame.log` 会记录当前技术和应用 CAS 的帧数。稳定采样 240 帧时，0% 显示 `240 frames (0 CAS)`，全部应用滤镜时显示 `240 frames (240 CAS)`。

已在 RTX 5060 上以 1080p 分别验证 DLSS、FSR 和 XeSS 性能模式下的游戏内锐度。离线测试覆盖该显卡可用的全部 14 种 SDK 模式。Radeon 上的 FSR 4 和针对 Intel Arc 优化的 XeSS 仍需在对应硬件上测试。

## 许可证

RE:FRAME 是独立的粉丝项目。压缩包中的 `Licenses` 包含 RE:FRAME 的 MIT 许可证，以及 FidelityFX CAS、各厂商运行库和 MinHook 的单独声明。
