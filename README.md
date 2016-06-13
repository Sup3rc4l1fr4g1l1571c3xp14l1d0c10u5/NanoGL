# NanoGL

NanoGL �̓v���O���~���O���w�Ҍ����̃V���v���ȃQ�[���v���O���~���O�p�t���[�����[�N�ł��B

## �T�v

  - �R���p�C����z�z����y�ŗe��
  - IDE�̗��p��O��Ƃ��Đ������ꂽAPI
  - �����₩�Ȃ���N���X�v���b�g�t�H�[���Ή�
  - �悭����Q�[���̎�������

## ���p�Z�p

NanoGL �ł͂������̃I�[�v���\�[�X�v���W�F�N�g��p���Ă��܂��B

  * [NanoVG] - Antialiased 2D vector drawing library on top of OpenGL for UI and visualizations.
  * [GLFW3] - Cross-platform library for creating windows with OpenGL contexts and receiving input and events.
  * [OpenAL-Soft] - Cross-platform library implementation of the OpenAL 3D audio API.
  * [GLEW] - Cross-platform open-source C/C++ extension loading library.
  * [libmad] - High-quality MPEG audio decoder library.
  * [stb] - Single file, public domain library.

## �C���X�g�[������T���v���N���܂�

  * �R���p�C���ς݃o�C�i�����g���i��y�ł������߁j
    * nanogl-win32.zip ���_�E�����[�h����
    * nanogl-win32.zip ��W�J����
    * nanogl-win32 �t�H���_���J��
    * MinGW32/MSYS �𗘗p���Ă���ꍇ
      * msys.bat ���N������ ./nanogl-win32/mingw �Ɉړ�
      * make -f Makefile.example �Ɠ��͂���ƃT���v���̃R���p�C�����s���܂��B
    * VisualStudio 2015 �𗘗p���Ă���ꍇ
      * VS2015 x86 Native Tools Command Prompt ���N������ ./nanogl-win32/vs14 �Ɉړ�
      * build-example.bat �����s����ƃT���v���̃R���p�C�����s���܂�
    * MacOS X (el capitan) �𗘗p���Ă���ꍇ
      * �R���p�C���ς݃o�C�i���͗��p�ł��܂���B�\�[�X����r���h���Ă�������

## �\�[�X����r���h

### MinGW/MSYS

�r���h�ɂ͈ȉ��̊����K�v�ł�

  * MSYS 1.0.11
    * https://sourceforge.net/projects/mingw/files/MSYS/Base/msys-core/msys-1.0.11/msysCORE-1.0.11-bin.tar.gz
    * �ǉ��ňȉ��̃p�b�P�[�W���K�v�ł�
      * https://sourceforge.net/projects/mingw/files/MSYS/Base/gettext/gettext-0.18.1.1-1/libintl-0.18.1.1-1-msys-1.0.17-dll-8.tar.lzma
      * https://sourceforge.net/projects/mingw/files/MSYS/Base/libiconv/libiconv-1.14-1/libiconv-1.14-1-msys-1.0.17-dll-2.tar.lzma
      * https://sourceforge.net/projects/mingw/files/MSYS/Base/xz/xz-4.999.9beta_20100401-1/liblzma-4.999.9beta_20100401-1-msys-1.0.13-dll-1.tar.gz
      * https://sourceforge.net/projects/mingw/files/MSYS/Extension/libxml2/libxml2-2.7.6-1/libxml2-2.7.6-1-msys-1.0.13-dll-2.tar.lzma
      * https://sourceforge.net/projects/mingw/files/MSYS/Extension/zlib/zlib-1.2.7-1/zlib-1.2.7-1-msys-1.0.17-dll.tar.lzma
  * gcc version 4.8.5 (thread:win32 exception:sjlj)
    * http://nbtelecom.dl.sourceforge.net/project/mingw-w64/Toolchains%20targetting%20Win32/Personal%20Builds/mingw-builds/4.8.5/threads-win32/sjlj/i686-4.8.5-release-win32-sjlj-rt_v4-rev0.7z
  * cmake (GLFW3 �� OpenAL �̃R���p�C���ɕK�v�ł��B)
    * https://cmake.org/files/v3.6/cmake-3.6.0-rc1-win32-x86.zip
  * wget (���p���Ă���I�[�v���\�[�X�p�b�P�[�W�̎擾�ɕK�v�ł�)
    * https://sourceforge.net/projects/mingw/files/MSYS/Extension/wget/wget-1.12-1/wget-1.12-1-msys-1.0.13-bin.tar.lzma
    * https://sourceforge.net/projects/mingw/files/MSYS/Extension/openssl/openssl-1.0.0-1/libopenssl-1.0.0-1-msys-1.0.13-dll-100.tar.lzma
  * patch (�������̏C���p�b�`�̓K�p�ɕK�v�ł��B)
    * https://sourceforge.net/projects/mingw/files/MSYS/Extension/patch/patch-2.6.1-1/patch-2.6.1-1-msys-1.0.13-bin.tar.lzma
  * bsdtar (�_�E�����[�h�����p�b�P�[�W�̓W�J�ɗ��p���܂��B.)
    * https://sourceforge.net/projects/mingw/files/MSYS/Extension/libarchive/libarchive-2.8.3-1/libarchive-2.8.3-1-msys-1.0.13-dll-2.tar.lzma
    * https://sourceforge.net/projects/mingw/files/MSYS/Extension/libarchive/libarchive-2.8.3-1/bsdtar-2.8.3-1-msys-1.0.13-bin.tar.lzma

msys.bat ���J���Ĉȉ������s���܂��B

```sh
$ bsdtar -m -xf nanogl-current.zip
$ cd NanoGL-current/mingw
$ make
$ make example
```

### Visual Studio Professional 2015 / Visual Studio Community 2015

�r���h�ɂ͈ȉ��̊����K�v�ł�

  * VS2015 x86 Native Tools Command Prompt
  * GnuWin32�Ɋ܂܂��wget, patch, LibArchive (build-tools.bat�Ŏ����I��tools�f�B���N�g���ɃZ�b�g�A�b�v����܂�)
    * wget (���p���Ă���I�[�v���\�[�X�p�b�P�[�W�̎擾�ɕK�v�ł�)
      * http://downloads.sourceforge.net/gnuwin32/wget-1.11.4-1-bin.zip
      * http://downloads.sourceforge.net/gnuwin32/wget-1.11.4-1-dep.zip
    * patch (�������̏C���p�b�`�̓K�p�ɕK�v�ł��B)
      * http://gnuwin32.sourceforge.net/downlinks/patch-bin-zip.php
      * Copy the file `./patches/patch.exe.manifest` in the same directory as the PATCH.EXE.
    * bsdtar (�_�E�����[�h�����p�b�P�[�W�̓W�J�ɗ��p���܂��B.)
      * http://downloads.sourceforge.net/gnuwin32/libarchive-2.4.12-1-bin.zip
      * http://downloads.sourceforge.net/gnuwin32/libarchive-2.4.12-1-dep.zip

�X�^�[�g���j���[����`VS2015 x86 Native Tools Command Prompt` ���J���Ĉȉ������s���܂��B

```sh
> bsdtar -m -xf nanogl-current.zip
> cd NanoGL-current/vs14
> build.bat
> build-example.bat
```

### MacOS X (el capitan)

�r���h�ɂ͈ȉ��̊����K�v�ł�

  * Apple LLVM version 7.3.0 (clang-703.0.29)
  * homebrew�̃C���X�g�[��
  * homebrew�o�R�ňȉ��̃��C�u�������C���X�g�[��
    * glfw3
      * brew install homebrew/versions/glfw3
    * glew
      * brew install glew
    * libmad
      * brew install libmad
    
Terminal.app ���J���Ĉȉ������s���܂��B

```sh
> bsdtar -m -xf nanogl-current.tgz
> cd NanoGL-current/osx
> make 
```

## CRuby�Ŏg���ꍇ

gem ���r���h���ăC���X�g�[�����邱�Ƃ�CRuby����NanoGL�����p�\�ɂȂ�܂��B

���p�ɂ͈ȉ��̊����K�v�ł�

  * CRuby (>= 2.0) 
  * gem bundler
  * gem ffi

### MinGW32 or MacOS X

Terminal.app ���J���A�eOS�����f�B���N�g���ňȉ������s���܂��B

```sh
$ make -f Makefile.rubygem
$ gem install -l nanogl-<version>.gem
$ ruby ../example/00-blank.rb
```sh

### VisualStudio

�X�^�[�g���j���[����`VS2015 x86 Native Tools Command Prompt` ���J����VS14�����f�B���N�g���ňȉ������s���܂��B

```sh
> build-mrb-nanogl.bat
> gem install -l nanogl-<version>.gem
> ruby ../example/00-blank.rb
```

## mruby�Ŏg���ꍇ

  * CRuby (>=2.0)
  * bison
  * flex

### MinGW32 

�r���h�ɂ͈ȉ��̊����K�v�ł�

  * bison
    * https://sourceforge.net/projects/mingw/files/MSYS/Extension/bison/bison-2.4.2-1/bison-2.4.2-1-msys-1.0.13-bin.tar.lzma
    * �ǉ��ňȉ��̃p�b�P�[�W���K�v�ł�
      * https://sourceforge.net/projects/mingw/files/MSYS/Extension/m4/m4-1.4.16-2/m4-1.4.16-2-msys-1.0.17-bin.tar.lzma
  * flex
    * https://sourceforge.net/projects/mingw/files/MSYS/Extension/flex/flex-2.5.35-2/flex-2.5.35-2-msys-1.0.13-bin.tar.lzma

msys.bat ���J���Ĉȉ������s���܂��B

```sh
$ make -f Makefile.mrb-nanogl
$ ./mrb-nanogl ../example/00-blank.rb
```sh

### MacOS X

Terminal.app ���J���A�eOS�����f�B���N�g���ňȉ������s���܂��B

```sh
$ make -f Makefile.mrb-nanogl
$ ./mrb-nanogl ../example/00-blank.rb
```sh

### VisualStudio

�r���h�ɂ͈ȉ��̊����K�v�ł�

  * GnuWin32�Ɋ܂܂��bison, flex (build-tools.bat�Ŏ����I��tools�f�B���N�g���ɃZ�b�g�A�b�v����܂�)
    * bison (mruby�̃R���p�C���ɕK�v�ł�)
      * http://downloads.sourceforge.net/gnuwin32/bison-2.4.1-bin.zip
      * http://downloads.sourceforge.net/gnuwin32/bison-2.4.1-dep.zip
    * flex (mruby�̃R���p�C���ɕK�v�ł�)
      * http://gnuwin32.sourceforge.net/downlinks/flex-bin-zip.php

�X�^�[�g���j���[����`VS2015 x86 Native Tools Command Prompt` ���J����VS14�����f�B���N�g���ňȉ������s���܂��B

```sh
> build-mrb-nanogl.bat
> mrb-nanogl.exe ../example/00-blank.rb
```

# ���C�Z���X

  * NanoGL�����p���Ă���I�[�v���\�[�X�v���W�F�N�g���ʕ��⃉�C�u�����������ANanoGL�\�[�X�R�[�h��example�t�H���_�̍��� NYSL Version 0.9982 �ł��B
  * OpenAL��libmad��GPL2 ���C�Z���X�̂��߁ANanoGL���X�^�e�B�b�N�����N�������s�t�@�C���� GPL2 ���C�Z���X�ɂȂ�܂��B�����ӂ��������B

# �\��

  * GPL�̔r��
    * �_�C�i�~�b�N�����N�ł̍쐬
    * MP3�f�R�[�_��Ǝ������������͔p�~
    * libsoundio��portaudio���̔�GPL���C�u�����ւ̈ڍs

