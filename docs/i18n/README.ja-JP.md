# RE:FRAME — Persona 3 Reload

[English](../../README.md) · [Français](README.fr-FR.md) · [Italiano](README.it-IT.md) · [Deutsch](README.de-DE.md) · [Español (España)](README.es-ES.md) · **[日本語](README.ja-JP.md)** · [한국어](README.ko-KR.md) · [Polski](README.pl-PL.md) · [Português (Brasil)](README.pt-BR.md) · [Русский](README.ru-RU.md) · [简体中文](README.zh-CN.md) · [繁體中文](README.zh-TW.md) · [Türkçe](README.tr-TR.md) · [Українська](README.uk-UA.md)

<!-- source-readme: 8deef3923bb5be1cd0937c169e899628281472fb -->

![バージョン 1.1.4](https://img.shields.io/badge/release-1.1.4-2469ad) ![Windows x64](https://img.shields.io/badge/platform-Windows%20x64-2469ad) ![DirectX 12](https://img.shields.io/badge/renderer-DirectX%2012-107c10) [![MIT ライセンス](https://img.shields.io/badge/license-MIT-blue)](../../LICENSE)

**Persona 3 Reload** の輪郭のギザつきやちらつきを、描画倍率を200%に上げずに抑えられます。DLAA や FSR・XeSS のネイティブAAなら、画面と同じ解像度で輪郭を滑らかにできます。フレームレートを重視するなら、アップスケーリングを選んでください。

**[RE:FRAME の公開リリース](https://github.com/Aporial/reframe-p3r/releases/latest)**

![RE:FRAME の項目が表示された Persona 3 Reload のグラフィック設定画面](../images/settings.png)

*RE:FRAME の項目は、ゲーム本体のグラフィック設定に表示されます。*

## インストール

1. ゲームを終了し、[GitHub Releases](https://github.com/Aporial/reframe-p3r/releases/latest) から **REFRAME-P3R-v1.1.4.zip** をダウンロードします。
2. Steam で **Persona 3 Reload** を右クリックし、**プロパティ → インストール済みファイル → 参照**を選びます。
3. `P3R` がすでに入っているフォルダーに ZIP を展開します。`P3R\Binaries\Win64` の `P3R.exe` と同じ場所に `d3d12.dll` が入ります。
4. いつもどおり Steam からゲームを起動します。
5. グラフィック設定を開き、描画倍率（**Rendering Scale**）のすぐ下にある **アンチエイリアス方式** を選びます。表示される場合は品質モードも選び、変更を確定します。

**`P3R\Binaries\Win64` にすでに `d3d12.dll` がある場合**は、別の MOD が使っている可能性があります。どの MOD のものか確認し、その MOD の手順に従って削除してから ZIP を展開してください。

ZIP には MOD 本体と必要なグラフィック用ライブラリが入っています。設定はゲーム内で行います。ReShade、外部の MOD ローダー、オーバーレイ、別のランチャー、設定ファイルの手動編集は不要です。

### 更新とアンインストール

- **更新** · ゲームを終了し、旧版のアンインストーラーを実行してから、新しい ZIP を展開します。開発版の設定を引き継ぐには、RE:FRAME が `ReFrame\Settings.ini` に保存するまで `P3RGraphicsEnhanced\Settings.ini` を残してください。
- **アンインストール** · ゲームを終了し、`P3R\Binaries\Win64\ReFrame` から `Remove-ReFrame.ps1` を実行します。スクリプトはファイルのハッシュを確認し、生成された設定やログは残します。`-WhatIf` を付けると削除対象を事前に確認できます。 [インストールと削除の手順](INSTALLATION.ja-JP.md)

[インストール](#%E3%82%A4%E3%83%B3%E3%82%B9%E3%83%88%E3%83%BC%E3%83%AB) · [描画倍率を200%にしなくてもよい理由](#%E6%8F%8F%E7%94%BB%E5%80%8D%E7%8E%87%E3%82%92200%E3%81%AB%E3%81%97%E3%81%AA%E3%81%8F%E3%81%A6%E3%82%82%E3%82%88%E3%81%84%E7%90%86%E7%94%B1) · [どのモードを選ぶ？](#%E3%81%A9%E3%81%AE%E3%83%A2%E3%83%BC%E3%83%89%E3%82%92%E9%81%B8%E3%81%B6) · [主な機能](#%E4%B8%BB%E3%81%AA%E6%A9%9F%E8%83%BD) · [対応グラフィックカード](#%E5%AF%BE%E5%BF%9C%E3%82%B0%E3%83%A9%E3%83%95%E3%82%A3%E3%83%83%E3%82%AF%E3%82%AB%E3%83%BC%E3%83%89) · [詳細設定とモード](#%E8%A9%B3%E7%B4%B0%E8%A8%AD%E5%AE%9A%E3%81%A8%E3%83%A2%E3%83%BC%E3%83%89) · [互換性と制限](#%E4%BA%92%E6%8F%9B%E6%80%A7%E3%81%A8%E5%88%B6%E9%99%90) · [トラブルシューティング](#%E3%83%88%E3%83%A9%E3%83%96%E3%83%AB%E3%82%B7%E3%83%A5%E3%83%BC%E3%83%86%E3%82%A3%E3%83%B3%E3%82%B0) · [ソースコードからビルド](#%E3%82%BD%E3%83%BC%E3%82%B9%E3%82%B3%E3%83%BC%E3%83%89%E3%81%8B%E3%82%89%E3%83%93%E3%83%AB%E3%83%89) · [ライセンス](#%E3%83%A9%E3%82%A4%E3%82%BB%E3%83%B3%E3%82%B9)

## 描画倍率を200%にしなくてもよい理由

1920×1080 では、輪郭のギザつきや細かな部分のちらつきが目立つことがあります。描画倍率を上げると滑らかになりますが、GPU の負荷も上がります。

| 描画倍率 | 1080p画面でゲームが描画する解像度 |
| --- | --- |
| **100%** | 1920×1080, 1フレームあたり約210万画素 |
| **200%** | 3840×2160, 1フレームあたり約830万画素 |

**200%では横も縦も2倍になり、計算する画素数は4倍です。** ネイティブAAは前後のフレームの情報を使い、100%で輪郭を整えます。この処理にも GPU の負荷はあり、200%の描画と比べた見え方は場面によって変わります。

アップスケーリングは、画面に表示するより少ない画素で描画してから画像を拡大します。GPU が性能のボトルネックなら、FPS が上がる場合があります。

## どのモードを選ぶ？

- **標準TAA** · ゲーム標準のアンチエイリアスです。描画倍率は手動で変更できます。
- **NVIDIA DLAA** · 100%の描画で輪郭を滑らかにします。対応する RTX を使っていて画質を重視する場合は、まずこれを試してください。
- **AMD FSR ネイティブAA** · 対応ハードウェアで、AMD の方式を使って100%で輪郭を補正します。
- **Intel XeSS ネイティブAA** · 対応ハードウェアで、Intel の方式を使って100%で輪郭を補正します。
- **DLSS / FSR / XeSS 品質** · アップスケーリングを試す際の最初の選択肢です。低い解像度で描画し、選択した出力解像度に再構成します。
- **バランス / パフォーマンス** · **品質** より低い解像度で描画して GPU の負荷をさらに抑えます。細部はややぼやけて見える場合があります。

| 重視すること | おすすめの選択肢 |
| --- | --- |
| RTX で画質を優先 | NVIDIA DLAA |
| RTX 以外で等倍のアンチエイリアス  |  FSR または XeSS の **ネイティブAA**（対応環境のみ） |
| FPS を上げたい  |  DLSS、FSR、XeSS の **品質** |
| 性能を優先  |  **パフォーマンス**、画質とのバランスを取るなら **バランス** |

利用できる方式や効果は、GPU、ドライバー、場面によって異なります。メニューには、その環境で利用可能と判定された項目が表示されます。

## 主な機能

- NVIDIA DLAA/DLSS、AMD FSR、Intel XeSS による等倍のアンチエイリアスとアップスケーリング。
- ゲーム内のグラフィック設定と、SDK のモードに応じた描画倍率の自動調整。
- 13の表示言語を自動で切り替え。
- ZIP から導入でき、削除用スクリプトも同梱。Steam から起動し、ゲーム本体のメニューと操作を使用。

## 動作要件

RE:FRAME には Steam で正規に購入した Persona 3 Reload、Windows 10 または 11 x64、DirectX 12 と対応するグラフィックドライバーが必要です。1.1.4 は **Steam ビルド 22672075**（アプリ ID **2161700**）向けです。

### 対応グラフィックカード

| 方式 | 対応 GPU |
| --- | --- |
| **NVIDIA DLAA / DLSS** | 対応ドライバーを使用した互換性のある GeForce RTX。 [NVIDIA DLSS](https://www.nvidia.com/en-us/geforce/technologies/dlss/) |
| **AMD FSR** | 対応する Radeon RX 6000、7000、9000 シリーズでは、SDK が FSR 4.1.2 を提供する場合があります。SDK の要件を満たすほかの GPU では FSR 3.1.6 を利用できます。 [AMD FSR SDK](https://gpuopen.com/amd-fsr-sdk/) |
| **Intel XeSS** | 対応する Intel Iris Xe、Arc、および Shader Model 6.4 と DP4a アクセラレーションに対応した他社製 GPU。 [Intel XeSS の動作要件](https://github.com/intel/xess/blob/v3.0.2/doc/xess_sr_developer_guide_english.md#requirements) |

XeSS には Microsoft Visual C++ x64 再頒布可能パッケージ **14.40.33810 以降**が必要です。Intel のこの SDK の最小要件は、Windows 10 がビルド 19043、Windows 11 がビルド 22000 です。 [Intel XeSS の導入要件](https://github.com/intel/xess/blob/v3.0.2/doc/xess_sr_developer_guide_english.md#deployment) · [Microsoft Visual C++ 再頒布可能パッケージ](https://learn.microsoft.com/en-us/cpp/windows/latest-supported-vc-redist)

## 詳細設定とモード

**アンチエイリアス方式** で方式を選び、利用できる **品質モード** を選択します。NVIDIA DLSS では **DLSSモデル** が表示されます。**標準TAA** なら描画倍率を手動で変更でき、SDK が制御する場合は目標値が表示されます。ゲームの確認画面で変更を確定またはキャンセルできます。**標準TAA** に戻すときに新しい倍率も選び、まとめて確定できます。

**シャープネス**は DLAA、DLSS、FSR、XeSS の再構成後、HUD の描画前に画像の細部を調整します。0%（初期値、無効）から 100% まで 5% 刻みです。変更を適用すると再起動せずに反映されます。値は方式や品質モードを切り替えても保持されます。標準TAAでは表示されません。

### 利用できるモード

| モード | NVIDIA DLSS | AMD FSR | Intel XeSS |
| --- | --- | --- | --- |
| ネイティブAA (100%) | DLAA | ネイティブAA | ネイティブAA |
| 最高品質+ | — | — | 利用可能な場合 |
| 最高品質 | — | — | 利用可能な場合 |
| 品質 | ✓ | ✓ | ✓ |
| バランス | ✓ | ✓ | ✓ |
| パフォーマンス | ✓ | ✓ | ✓ |

3つの SDK を合わせて最大14モードを利用できます。**標準TAA** も選択できます。

### DLSSモデル

| モデル | 使用するモデル |
| --- | --- |
| **自動（推奨）** | DLAA、**品質**、**バランス** では K、**パフォーマンス** では M を使います。 |
| **DLSS 4 — 標準 (K)** | DLSS 4 の Transformer モデル。 |
| **DLSS 4.5 — 最適化 (M)** | NVIDIA が **パフォーマンス** 向けに推奨する DLSS 4.5 のモデル。 |
| **DLSS 4.5 — 精細 (L)** | NVIDIA が 4K のウルトラパフォーマンス向けに推奨する DLSS 4.5 のモデル。モデルは品質モードとは別に選べます。 |

M と L は、RTX 20・30 シリーズでは GPU の処理により時間がかかる場合があります。 [NVIDIA の DLSS モデルガイド](https://www.nvidia.com/en-us/geforce/news/dlss-4-5-super-resolution-available-now/)

### 対応言語

英語、フランス語、イタリア語、ドイツ語、スペイン語（スペイン）、日本語、韓国語、ポーランド語、ポルトガル語（ブラジル）、ロシア語、中国語（簡体字）、中国語（繁体字）、トルコ語。RE:FRAME はゲームの表示言語に従い、非対応の言語では英語を表示します。 [Steam の Persona 3 Reload](https://store.steampowered.com/app/2161700/Persona_3_Reload/)

## 互換性と制限

- **ゲームの更新** · Steam ビルド 22672075 以外、他のストア版、Windows 以外の実行環境、複数の D3D12 プロキシの併用は未確認です。
- **標準モードへの切り替え** · 認識できない描画処理や平面反射などの副次的なビューでは、ゲーム本体が **標準TAA** を使用します。
- **テスト** · RTX 5060 を用い、DLSS、FSR、XeSS のパフォーマンスモードで 1080p のゲーム内検証を行いました。同 GPU で利用できる SDK の全 14 モードをオフラインでも検証しました。Radeon の FSR 4 と Intel Arc 向けに最適化された XeSS は、対応する実機での検証が残っています。 [テスト範囲と互換性](../COMPATIBILITY.md)

## トラブルシューティング

- **RE:FRAME の項目がない場合**は Steam のビルド番号を確認し、`d3d12.dll` が `P3R.exe` と同じ場所にあるか調べてください。
- **使いたい方式が表示されない場合**は GPU の対応状況、ドライバー、`P3R\Binaries\Win64\ReFrame` 内のライブラリを確認してください。XeSS では Visual C++ 再頒布可能パッケージも確認してください。
- **描画倍率が変更できない場合**は DLSS、FSR、XeSS が倍率を管理しています。手動で変えるには **標準TAA** を選びます。

詳しくは [トラブルシューティング](../TROUBLESHOOTING.md) を参照してください。実際の描画解像度や標準モードに戻った記録は `P3R\Binaries\Win64\ReFrame\ReFrame.log` に残ります。

## ソースコードからビルド

C++ ワークロードと Windows SDK がインストールされた **Visual Studio 2022 の x64 Native Tools Command Prompt** を開き、次のコマンドを実行します。

```powershell
powershell -NoProfile -File scripts\Fetch-Dependencies.ps1
powershell -NoProfile -File build.ps1
powershell -NoProfile -File scripts\New-Release.ps1
```

[ビルド手順](../BUILDING.md) · [アーキテクチャ](../ARCHITECTURE.md) · [変更履歴](../../CHANGELOG.md) · [コントリビューション](../../CONTRIBUTING.md)

## ライセンス

Copyright © 2026 Aporia. MIT ライセンスは RE:FRAME 独自のソースコードに適用されます。同梱の SDK ランタイム、NVIDIA NGX ローダー、MinHook/HDE などの第三者製コンポーネントには、それぞれの条件と通知が適用されます。ゲーム本体と素材には権利者の条件が適用されます。 [MIT ライセンス](../../LICENSE) · [サードパーティーに関する通知](../../THIRD_PARTY_NOTICES.md) · [各社のライセンス](../../licenses/)

## 免責事項

RE:FRAME は、ファンが制作した独立した非公式プロジェクトです。ATLUS、SEGA、NVIDIA、AMD、Intel との提携はなく、これらの企業による承認や協賛も受けていません。すべての商標は各権利者に帰属します。

RE:FRAME の利用には正規に購入した Persona 3 Reload が必要です。ゲームファイルを配布せず、DRM を回避せず、DLC を解放しません。スクリーンショットは設定画面を説明するもので、ゲームの素材が写っています。 [Persona 3 Reload の Steam 利用許諾契約](https://store.steampowered.com/eula/2161700_eula_1)
