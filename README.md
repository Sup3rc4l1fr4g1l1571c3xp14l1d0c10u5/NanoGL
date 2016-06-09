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
    * MinGW32 �𗘗p���Ă���ꍇ
      * bash ���N������ ./nanogl-win32/mingw �Ɉړ�
      * make -f Makefile.example �Ɠ��͂���ƃT���v���̃R���p�C�����s���܂��B
    * VisualStudio 2015 �𗘗p���Ă���ꍇ
      * VS2015 x86 Native Tools Command Prompt ���N������ ./nanogl-win32/vs14 �Ɉړ�
      * build-example.bat �����s����ƃT���v���̃R���p�C�����s���܂�
    * MacOS X (el capitan) �𗘗p���Ă���ꍇ
      * �R���p�C���ς݃o�C�i���͗��p�ł��܂���B�\�[�X����r���h���Ă�������

## �\�[�X����r���h

### MinGW/MSYS

�r���h�ɂ͈ȉ��̊����K�v�ł�

  * gcc version 4.7.2 (rubenvb-4.7.2-release) Thread model: win32
    * ��΂� 4.9.x �n�� gcc ���g��Ȃ��ł��������Bgcc���̂̃o�O�ŕs���ȃR�[�h����������܂��B�����猙�Ȃ̂� gcc �́B
  * libavrt (gcc4.9.x�n��mingw32��������������Ďg��)
  * cmake (GLFW3 �� OpenAL �̃R���p�C���ɕK�v�ł��B)
  * wget (���p���Ă���I�[�v���\�[�X�p�b�P�[�W�̎擾�ɕK�v�ł�)
  * patch (�������̏C���p�b�`�̓K�p�ɕK�v�ł��B)
  * bsdtar (�_�E�����[�h�����p�b�P�[�W�̓W�J�ɗ��p���܂��B.)

MinGW/MSYS �� bash.exe ���J���Ĉȉ������s���܂��B

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
  * gem ffi 

### MinGW32 or MacOS X

Terminal.app ���J���A�eOS�����f�B���N�g���ňȉ������s���܂��B

```sh
$ make -f Makefile.rubygem
$ gem install -l nanogl-<version>.gem
$ ruby ../ruby/example/00-blank.rb
```sh

### VisualStudio

�X�^�[�g���j���[����`VS2015 x86 Native Tools Command Prompt` ���J����VS14�����f�B���N�g���ňȉ������s���܂��B

```sh
> build-mrb-nanogl.bat
> gem install -l nanogl-<version>.gem
> ruby ../ruby/example/00-blank.rb
```

## mruby�Ŏg���ꍇ

  * Install CRuby (>= 2.0)

### MinGW32 or MacOS X

Terminal.app ���J���A�eOS�����f�B���N�g���ňȉ������s���܂��B

```sh
$ make -f Makefile.mrb-nanogl
$ ./mrb-nanogl ../ruby/example/00-blank.rb
```sh

### VisualStudio

�X�^�[�g���j���[����`VS2015 x86 Native Tools Command Prompt` ���J����VS14�����f�B���N�g���ňȉ������s���܂��B

```sh
> build-mrb-nanogl.bat
> mrb-nanogl.exe ../ruby/mruby/example/00-blank.rb
```

# ���C�Z���X

  * ��L�I�[�v���\�[�X�v���W�F�N�g�̐��ʕ��������ANanoGL�\�[�X�R�[�h��example�t�H���_�̍��� NYSL Version 0.9982 �ł��B
  * OpenAL��libmad��GPL2 ���C�Z���X�̂��߁ANanoGL���X�^�e�B�b�N�����N�������s�t�@�C���� GPL2 ���C�Z���X�ɂȂ�܂��B�����ӂ��������B

# �\��

  * GPL�̔r��
    * �_�C�i�~�b�N�����N�ł̍쐬
    * MP3�f�R�[�_��Ǝ������������͔p�~
    * libsoundio��portaudio���̔�GPL���C�u�����ւ̈ڍs

