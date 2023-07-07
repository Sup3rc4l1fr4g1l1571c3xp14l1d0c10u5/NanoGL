# NanoGL

NanoGL はプログラミング初学者向けのシンプルなゲームプログラミング用フレームワークです。

## 概要

- コンパイルや配布が手軽で容易
- IDEの利用を前提として整理された単純なAPI
- ささやかながらクロスプラットフォーム(Windows/MacOSX/Ubuntu)対応
- 学習の参考向けによくあるゲームの実装例を提供
- CRuby, mruby での利用が可能です。言語仕様の差異に注意して実装すれば、CRubyとmrubyで同一のコードが動作します。

## 利用技術

NanoGL ではいくつかのオープンソースプロジェクトの成果物を用いています。

* [NanoVG] - Antialiased 2D vector drawing library on top of OpenGL for UI and visualizations.
* [GLFW3] - Cross-platform library for creating windows with OpenGL contexts and receiving input and events.
* [miniaudio] - An audio playback and capture library in a single source file.
* [glad] - Multi-Language Vulkan/GL/GLES/EGL/GLX/WGL Loader-Generator.
* [stb] - Single file, public domain library.

## `libnanogl.a`ビルド方法

### Windows環境の場合
1. MSYS32 を導入してください。
   - 32bit環境向けの場合
     - https://repo.msys2.org/distrib/i686/msys2-base-i686-20210705.sfx.exe
   - 64bit環境向けの場合
     - https://repo.msys2.org/distrib/x86_64/msys2-base-x86_64-20230526.sfx.exe

2. 追加パッケージを導入します
   1. msys.bat を起動します。
   2. 以下のコマンドを実行し、追加パッケージを導入します。
      ```shell
      $ pacman -S git
      $ pacman -S python3
      $ pacman -S python3-pip
      ```

3. ビルドを行います。
   1. `msys.bat` を起動して `./nanogl/` に移動
   2. `make all` を実行

### Mac環境の場合

1. `XCode Command Line Tools` を導入してください。
    1. Appstoreから `XCode` を導入します。
    2. ターミナルを開き以下のコマンドを実行して`XCode Command Line Tools` を導入します。
       ```shell
       $ xcode-select --install
       ```
2. Homebrew を導入してください。
3. 以下のコマンドを入力し、追加パッケージを導入します。
   ```shell
   $ brew install python3
   ```
4. ビルドを行います。
   1. ターミナル上で `./nanogl/` に移動
   2. `make all` を実行

### Ubuntu環境の場合

（動作確認がとれしたい追記予定）

## サンプルのビルド

1. 事前に `libnanogl.a` をビルドしてください。 
2. msys.bat もしくは ターミナルを起動し、`./example/` に移動
3. `make all` を実行

## 各言語からの利用

### C言語から利用する場合

- `src`フォルダを`include`パスに追加してください。
- リンク時に`libnanogl.a`を追加してください。
- プラットフォームごとに追加のコンパイル設定をしてください。

#### Windows環境でのコンパイル設定
- リンクする￥ライブラリに `mingw32`, `winmm`, `Avrt`, `OpenGL32`, `Gdi32` を追加してください。
- コンパイル時のフラグに `-mwindows` を追加してください。
- CソースをUTF8で記述している場合はコンパイル時のフラグに `-finput-charset=utf-8 -fexec-charset=cp932` を追加してください。  

#### macOS環境でのコンパイル設定
- リンクするフレームワークに `OpenGL`, `Foundation`, `IOKit`, `CoreAudio`, `Cocoa`, `AppKit` を追加してください。

### `CRuby`から利用する場合

`CRuby`向けに`nanogl`の`gem`を提供しています。

1. 事前に `libnanogl.a` をビルドしてください。 
2. msys.bat もしくは ターミナルを起動し、`./ruby/cruby/gem/` に移動
3. `make all` を実行
4. `rake build` を実行
5. `gem install -l ./pkg/nanogl-<version>.gem`で作成した`gem`をインストール

インストール後、`./example/`フォルダ内の `*.rb` が実行できるようになります。


### mruby に組み込んで利用する場合

(ここは古い手順になっています。動作が確認され次第記載しなおします。)

ビルドには`CRuby`と以下の追加パッケージが必要です
手元の環境に応じて導入してください。

  - `bison`
  - `flex`
  - `git`


```sh
$ make -f Makefile.mrb-nanogl
$ ./mrb-nanogl ../example/00-blank.rb
```

## ライセンス

-  NanoGLが利用しているオープンソースプロジェクト成果物やライブラリを除く、NanoGLソースコードとexampleフォルダの作例は NYSL Version 0.9982 です。
- 0.3.0以前はGPLライセンスのライブラリに依存していましたが、0.3.0からはGPLライセンスのライブラリが不要になりました。

## 予定

- 株式会社まちづくり三鷹が発行している「オブジェクト指向言語「Rubyプログラミング入門」テキスト（初心者用）」への対応
- 利用することになる dxruby on nanogl のdxruby対応度を向上

