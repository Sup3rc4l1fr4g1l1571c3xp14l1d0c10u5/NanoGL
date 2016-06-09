# NanoGL

NanoGL is a simple game programming framework for programming beginners.

## Concept

  - Easy to build and distribute.
  - Simple and organized API.
  - Cross platform and portable.

## Tech

NanoGL uses a number of open source projects to work properly:

  * [NanoVG] - Antialiased 2D vector drawing library on top of OpenGL for UI and visualizations.
  * [GLFW3] - Cross-platform library for creating windows with OpenGL contexts and receiving input and events.
  * [OpenAL-Soft] - Cross-platform library implementation of the OpenAL 3D audio API.
  * [GLEW] - Cross-platform open-source C/C++ extension loading library.
  * [libmad] - High-quality MPEG audio decoder library.
  * [stb] - Single file, public domain library.

## Installation and Startup

  * Download nanogl-current.zip
  * Extract nanogl-current.zip.
  * Open nanogl-current.zip
  * If you use MinGW32.
    * move to ./nanogl-current/mingw32/
    * run `make example`
  * If you use VisualStudio.
    * open "VS2015 x86 Native Tools Command Prompt"
    * move to ./nanogl-current/vs14/ 
    * run `build-example.bat`
  * If you use MacOS X
    * Install libraries from homebrew (glfw3, glew, libmad)
    * move to ./nanogl-current/osx/
    * run `make example`

## Build

### MinGW32

requires to build

  * gcc version 4.7.2 (rubenvb-4.7.2-release)
    Thread model: win32
    * Don't use the 4.9.x of gcc, because it produces the wrong code.
      That's way I don't like gcc.
  * cmake (used to build GLFW3 and OpenAL)
  * wget (used to download packages.)
  * patch (used to patch unofficial nanovg hack.)
  * bsdtar (used to extract downloaded package.)

```sh
$ bsdtar -m -xf nanogl-current.zip
$ cd NanoGL-current/mingw
$ make
$ make example
```

### Visual Studio Professional 2015 / Visual Studio Community 2015

requires to build

  * VS2015 x86 Native Tools Command Prompt
  * GnuWin32 (Include this archive.)
    * wget(used to download packages.)
      * http://downloads.sourceforge.net/gnuwin32/wget-1.11.4-1-bin.zip
      * http://downloads.sourceforge.net/gnuwin32/wget-1.11.4-1-dep.zip
    * patch (used to patch unofficial nanovg hack.)
      * http://gnuwin32.sourceforge.net/downlinks/patch-bin-zip.php
      * Copy the file `./patches/patch.exe.manifest` in the same directory as the PATCH.EXE.
    * LibArchive (badtar: used to extract downloaded package.)
      * http://downloads.sourceforge.net/gnuwin32/libarchive-2.4.12-1-bin.zip
      * http://downloads.sourceforge.net/gnuwin32/libarchive-2.4.12-1-dep.zip

open `VS2015 x86 Native Tools Command Prompt` from start menu
```sh
> bsdtar -m -xf nanogl-current.zip
> cd NanoGL-current/vs14
> build.bat
> build-example.bat
```

### MacOS X (el capitan)

requires to build

  * Apple LLVM version 7.3.0 (clang-703.0.29)
  * Install homebrew
  * Install libraries from homebrew
    * brew install homebrew/versions/glfw3
    * brew install glew
    * brew install libmad
    
```sh
> bsdtar -m -xf nanogl-current.tgz
> cd NanoGL-current
> make -f Makefile.osx
```

## Using from CRuby

  * Install CRuby (>= 2.0)
  * Install ffi from mrubygem

### MinGW32 or MacOS X
```sh
$ make -f Makefile.rubygem
$ gem install -l nanogl-<version>.gem
$ ruby ../ruby/example/00-blank.rb
```sh

### VisualStudio
```sh
> build-mrb-nanogl.bat
> gem install -l nanogl-<version>.gem
> ruby ../ruby/example/00-blank.rb
```

## Using from MRuby

  * Install CRuby (>= 2.0)

### MinGW32 or MacOS X
```sh
$ make -f Makefile.mrb-nanogl
$ ./mrb-nanogl ../ruby/example/00-blank.rb
```sh

### VisualStudio
```sh
> build-mrb-nanogl.bat
> mrb-nanogl.exe ../ruby/mruby/example/00-blank.rb
```

License
----

  * NanoGL library and example's source code is MIT License

Todo
----

  * I want to eliminate GPL code in the future. :-)

