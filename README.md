# NanoGL

NanoGL はプログラミング初学者向けのシンプルなゲームプログラミング用フレームワークです。

## 概要

  - コンパイルや配布が手軽で容易
  - IDEの利用を前提として整理されたAPI
  - ささやかながらクロスプラットフォーム対応
  - よくあるゲームの実装例を提供

## 利用技術

NanoGL ではいくつかのオープンソースプロジェクトを用いています。

  * [NanoVG] - Antialiased 2D vector drawing library on top of OpenGL for UI and visualizations.
  * [GLFW3] - Cross-platform library for creating windows with OpenGL contexts and receiving input and events.
  * [OpenAL-Soft] - Cross-platform library implementation of the OpenAL 3D audio API.
  * [GLEW] - Cross-platform open-source C/C++ extension loading library.
  * [libmad] - High-quality MPEG audio decoder library.
  * [stb] - Single file, public domain library.

## インストールからサンプル起動まで

  * コンパイル済みバイナリを使う（手軽でおすすめ）
    * nanogl-win32.zip をダウンロードする
    * nanogl-win32.zip を展開する
    * nanogl-win32 フォルダを開く
    * MinGW32 を利用している場合
      * bash を起動して ./nanogl-win32/mingw に移動
      * make -f Makefile.example と入力するとサンプルのコンパイルが行われます。
    * VisualStudio 2015 を利用している場合
      * VS2015 x86 Native Tools Command Prompt を起動して ./nanogl-win32/vs14 に移動
      * build-example.bat を実行するとサンプルのコンパイルが行われます
    * MacOS X (el capitan) を利用している場合
      * コンパイル済みバイナリは利用できません。ソースからビルドしてください

## ソースからビルド

### MinGW/MSYS

ビルドには以下の環境が必要です

  * gcc version 4.7.2 (rubenvb-4.7.2-release) Thread model: win32
    * 絶対に 4.9.x 系の gcc を使わないでください。gcc自体のバグで不正なコードが生成されます。だから嫌なのよ gcc は。
  * libavrt (gcc4.9.x系のmingw32から引っこ抜いて使う)
  * cmake (GLFW3 と OpenAL のコンパイルに必要です。)
  * wget (利用しているオープンソースパッケージの取得に必要です)
  * patch (いくつかの修正パッチの適用に必要です。)
  * bsdtar (ダウンロードしたパッケージの展開に利用します。.)

MinGW/MSYS の bash.exe を開いて以下を実行します。

```sh
$ bsdtar -m -xf nanogl-current.zip
$ cd NanoGL-current/mingw
$ make
$ make example
```

### Visual Studio Professional 2015 / Visual Studio Community 2015

ビルドには以下の環境が必要です

  * VS2015 x86 Native Tools Command Prompt
  * GnuWin32に含まれるwget, patch, LibArchive (build-tools.batで自動的にtoolsディレクトリにセットアップされます)
    * wget (利用しているオープンソースパッケージの取得に必要です)
      * http://downloads.sourceforge.net/gnuwin32/wget-1.11.4-1-bin.zip
      * http://downloads.sourceforge.net/gnuwin32/wget-1.11.4-1-dep.zip
    * patch (いくつかの修正パッチの適用に必要です。)
      * http://gnuwin32.sourceforge.net/downlinks/patch-bin-zip.php
      * Copy the file `./patches/patch.exe.manifest` in the same directory as the PATCH.EXE.
    * bsdtar (ダウンロードしたパッケージの展開に利用します。.)
      * http://downloads.sourceforge.net/gnuwin32/libarchive-2.4.12-1-bin.zip
      * http://downloads.sourceforge.net/gnuwin32/libarchive-2.4.12-1-dep.zip

スタートメニューから`VS2015 x86 Native Tools Command Prompt` を開いて以下を実行します。

```sh
> bsdtar -m -xf nanogl-current.zip
> cd NanoGL-current/vs14
> build.bat
> build-example.bat
```

### MacOS X (el capitan)

ビルドには以下の環境が必要です

  * Apple LLVM version 7.3.0 (clang-703.0.29)
  * homebrewのインストール
  * homebrew経由で以下のライブラリをインストール
    * glfw3
      * brew install homebrew/versions/glfw3
    * glew
      * brew install glew
    * libmad
      * brew install libmad
    
Terminal.app を開いて以下を実行します。

```sh
> bsdtar -m -xf nanogl-current.tgz
> cd NanoGL-current/osx
> make 
```

## CRubyで使う場合

gem をビルドしてインストールすることでCRubyからNanoGLが利用可能になります。

利用には以下の環境が必要です

  * CRuby (>= 2.0) 
  * gem ffi 

### MinGW32 or MacOS X

Terminal.app を開き、各OS向けディレクトリで以下を実行します。

```sh
$ make -f Makefile.rubygem
$ gem install -l nanogl-<version>.gem
$ ruby ../ruby/example/00-blank.rb
```sh

### VisualStudio

スタートメニューから`VS2015 x86 Native Tools Command Prompt` を開いてVS14向けディレクトリで以下を実行します。

```sh
> build-mrb-nanogl.bat
> gem install -l nanogl-<version>.gem
> ruby ../ruby/example/00-blank.rb
```

## mrubyで使う場合

  * Install CRuby (>= 2.0)

### MinGW32 or MacOS X

Terminal.app を開き、各OS向けディレクトリで以下を実行します。

```sh
$ make -f Makefile.mrb-nanogl
$ ./mrb-nanogl ../ruby/example/00-blank.rb
```sh

### VisualStudio

スタートメニューから`VS2015 x86 Native Tools Command Prompt` を開いてVS14向けディレクトリで以下を実行します。

```sh
> build-mrb-nanogl.bat
> mrb-nanogl.exe ../ruby/mruby/example/00-blank.rb
```

# ライセンス

  * 上記オープンソースプロジェクトの成果物を除く、NanoGLソースコードとexampleフォルダの作例は NYSL Version 0.9982 です。
  * OpenALやlibmadがGPL2 ライセンスのため、NanoGLをスタティックリンクした実行ファイルは GPL2 ライセンスになります。ご注意ください。

# 予定

  * GPLの排除
    * ダイナミックリンク版の作成
    * MP3デコーダを独自実装もしくは廃止
    * libsoundioやportaudio等の非GPLライブラリへの移行

