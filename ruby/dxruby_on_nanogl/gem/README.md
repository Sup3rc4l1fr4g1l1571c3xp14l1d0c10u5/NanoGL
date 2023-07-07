# dxruby on nanogl

## これは何？

- Windows環境以外で nanogl を使って dxruby ライブラリの一部命令を模する薄いラッパーです。
  - 実験的な実装なので、完全な互換性はありません。 
  - 現時点では dxruby144-samples.zip の classic_sample が辛うじて動作します。
    - 行頭の #!ruby -Ks の削除が必要です。

## 動作確認環境

- MacBook Air (M2, 2022), macOS 13 Ventura
- HP ProDesk 600 G6 (Intel Core i7-10700), Windows 10 Pro
- ASRock DeskMini A300 (Ryzen 3200G), Windows 10 Pro

## ビルド方法

1. このフォルダ内で以下のコマンドを実行します。
   ```shell
   $ rake build
   ```
2. `pkg`フォルダ中に `dxruby_on_nanogl-<version>.gem` が生成されます。

## インストール方法

1. 事前に `nanogl` の gem をインストールしてください。

2. このフォルダ内で以下のコマンドを実行します。
   ```shell
   $ gem install -l pkg/dxruby_on_nanogl-<version>.gem
   ```

## 使い方

- `require 'dxruby'` の代わりに `require 'dxruby_on_nanogl'` と記述してください。
- 対応している命令で動く範囲でそれっぽく動きます。

## 既知の問題

- クラスの継承関係などは再現していません。
- ラッパー部分はrubyで実装しているため、Imageクラスのピクセル操作などは非常に低速です。
- フォントレンダリングエンジンの関係でフォントの描画などの挙動が一致しません。
- システムに導入済みのフォントを列挙していないため、システムフォントが使えません。
- スクリーンショット命令は dds 形式をサポートしません。
- Soundクラスは midi形式 をサポートしません。

