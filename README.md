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
    * MinGW32/MSYS を利用している場合
      * msys.bat を起動して ./nanogl-win32/mingw に移動
      * make -f Makefile.example と入力するとサンプルのコンパイルが行われます。
    * VisualStudio 2015 を利用している場合
      * VS2015 x86 Native Tools Command Prompt を起動して ./nanogl-win32/vs14 に移動
      * build-example.bat を実行するとサンプルのコンパイルが行われます
    * MacOS X (el capitan) を利用している場合
      * コンパイル済みバイナリは利用できません。ソースからビルドしてください

## ソースからビルド

### MinGW/MSYS

ビルドには以下の環境が必要です

  * MSYS 1.0.11
    * https://sourceforge.net/projects/mingw/files/MSYS/Base/msys-core/msys-1.0.11/msysCORE-1.0.11-bin.tar.gz
    * 追加で以下のパッケージが必要です
      * https://sourceforge.net/projects/mingw/files/MSYS/Base/gettext/gettext-0.18.1.1-1/libintl-0.18.1.1-1-msys-1.0.17-dll-8.tar.lzma
      * https://sourceforge.net/projects/mingw/files/MSYS/Base/libiconv/libiconv-1.14-1/libiconv-1.14-1-msys-1.0.17-dll-2.tar.lzma
      * https://sourceforge.net/projects/mingw/files/MSYS/Base/xz/xz-4.999.9beta_20100401-1/liblzma-4.999.9beta_20100401-1-msys-1.0.13-dll-1.tar.gz
      * https://sourceforge.net/projects/mingw/files/MSYS/Extension/libxml2/libxml2-2.7.6-1/libxml2-2.7.6-1-msys-1.0.13-dll-2.tar.lzma
      * https://sourceforge.net/projects/mingw/files/MSYS/Extension/zlib/zlib-1.2.7-1/zlib-1.2.7-1-msys-1.0.17-dll.tar.lzma
  * gcc version 4.8.5 (thread:win32 exception:sjlj)
    * http://nbtelecom.dl.sourceforge.net/project/mingw-w64/Toolchains%20targetting%20Win32/Personal%20Builds/mingw-builds/4.8.5/threads-win32/sjlj/i686-4.8.5-release-win32-sjlj-rt_v4-rev0.7z
  * cmake (GLFW3 と OpenAL のコンパイルに必要です。)
    * https://cmake.org/files/v3.6/cmake-3.6.0-rc1-win32-x86.zip
  * wget (利用しているオープンソースパッケージの取得に必要です)
    * https://sourceforge.net/projects/mingw/files/MSYS/Extension/wget/wget-1.12-1/wget-1.12-1-msys-1.0.13-bin.tar.lzma
    * https://sourceforge.net/projects/mingw/files/MSYS/Extension/openssl/openssl-1.0.0-1/libopenssl-1.0.0-1-msys-1.0.13-dll-100.tar.lzma
  * patch (いくつかの修正パッチの適用に必要です。)
    * https://sourceforge.net/projects/mingw/files/MSYS/Extension/patch/patch-2.6.1-1/patch-2.6.1-1-msys-1.0.13-bin.tar.lzma
  * bsdtar (ダウンロードしたパッケージの展開に利用します。.)
    * https://sourceforge.net/projects/mingw/files/MSYS/Extension/libarchive/libarchive-2.8.3-1/libarchive-2.8.3-1-msys-1.0.13-dll-2.tar.lzma
    * https://sourceforge.net/projects/mingw/files/MSYS/Extension/libarchive/libarchive-2.8.3-1/bsdtar-2.8.3-1-msys-1.0.13-bin.tar.lzma

msys.bat を開いて以下を実行します。

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
  * gem bundler
  * gem ffi

### MinGW32 or MacOS X

Terminal.app を開き、各OS向けディレクトリで以下を実行します。

```sh
$ make -f Makefile.rubygem
$ gem install -l nanogl-<version>.gem
$ ruby ../example/00-blank.rb
```sh

### VisualStudio

スタートメニューから`VS2015 x86 Native Tools Command Prompt` を開いてVS14向けディレクトリで以下を実行します。

```sh
> build-mrb-nanogl.bat
> gem install -l nanogl-<version>.gem
> ruby ../example/00-blank.rb
```

## mrubyで使う場合

  * CRuby (>=2.0)
  * bison
  * flex

### MinGW32 

ビルドには以下の環境が必要です

  * bison
    * https://sourceforge.net/projects/mingw/files/MSYS/Extension/bison/bison-2.4.2-1/bison-2.4.2-1-msys-1.0.13-bin.tar.lzma
    * 追加で以下のパッケージが必要です
      * https://sourceforge.net/projects/mingw/files/MSYS/Extension/m4/m4-1.4.16-2/m4-1.4.16-2-msys-1.0.17-bin.tar.lzma
  * flex
    * https://sourceforge.net/projects/mingw/files/MSYS/Extension/flex/flex-2.5.35-2/flex-2.5.35-2-msys-1.0.13-bin.tar.lzma

msys.bat を開いて以下を実行します。

```sh
$ make -f Makefile.mrb-nanogl
$ ./mrb-nanogl ../example/00-blank.rb
```sh

### MacOS X

Terminal.app を開き、各OS向けディレクトリで以下を実行します。

```sh
$ make -f Makefile.mrb-nanogl
$ ./mrb-nanogl ../example/00-blank.rb
```sh

### VisualStudio

ビルドには以下の環境が必要です

  * GnuWin32に含まれるbison, flex (build-tools.batで自動的にtoolsディレクトリにセットアップされます)
    * bison (mrubyのコンパイルに必要です)
      * http://downloads.sourceforge.net/gnuwin32/bison-2.4.1-bin.zip
      * http://downloads.sourceforge.net/gnuwin32/bison-2.4.1-dep.zip
    * flex (mrubyのコンパイルに必要です)
      * http://gnuwin32.sourceforge.net/downlinks/flex-bin-zip.php

スタートメニューから`VS2015 x86 Native Tools Command Prompt` を開いてVS14向けディレクトリで以下を実行します。

```sh
> build-mrb-nanogl.bat
> mrb-nanogl.exe ../example/00-blank.rb
```

# ライセンス

  * NanoGLが利用しているオープンソースプロジェクト成果物やライブラリを除く、NanoGLソースコードとexampleフォルダの作例は NYSL Version 0.9982 です。
  * OpenALやlibmadがGPL2 ライセンスのため、NanoGLをスタティックリンクした実行ファイルは GPL2 ライセンスになります。ご注意ください。

# 予定

  * GPLの排除
    * ダイナミックリンク版の作成
    * MP3デコーダを独自実装もしくは廃止
    * libsoundioやportaudio等の非GPLライブラリへの移行

