(I’m sorry that my poor English confuses you.)

# NanoGL

NanoGL is a simple game programming framework for beginning programmers.

## Overview

- Simple and easy to compile and distribute
- Simple API organized for use with IDEs
- Modest cross-platform (Windows/MacOSX/Ubuntu) support
- Provides examples of common game implementations for learning reference
- Available in CRuby and mruby. If you implement it with attention to the differences in language specifications, the same code will work in CRuby and mruby.

## Utilized Technologies

NanoGL uses artifacts from several open source projects.

* [NanoVG] - Antialiased 2D vector drawing library on top of OpenGL for UI and visualizations.
* [GLFW3] - Cross-platform library for creating windows with OpenGL contexts and receiving input and events.
* [miniaudio] - An audio playback and capture library in a single source file.
* [glad] - Multi-Language Vulkan/GL/GLES/EGL/GLX/WGL Loader-Generator.
* [stb] - Single file, public domain library.

## How to build `libnanogl.a

### For Windows environment

1. Install MSYS32.
   - For 32bit environment
     - https://repo.msys2.org/distrib/i686/msys2-base-i686-20210705.sfx.exe
   - For 64bit environment
     - https://repo.msys2.org/distrib/x86_64/msys2-base-x86_64-20230526.sfx.exe

2. Install additional packages
   1. Run msys.bat.
   2. Execute the following command to install the additional packages.
      ```shell
      $ pacman -S git
      $ pacman -S python3
      $ pacman -S python3-pip
      ````

3. Build.
   1. Start `msys.bat` and go to `./nanogl/`.
   2. Execute `make all`.

### For Mac environment

1. Install the `XCode Command Line Tools`.
    1. Install `XCode` from the Appstore.
    2. Open a terminal and execute the following command to install `XCode Command Line Tools`.
       ```shell
       $ xcode-select --install
       ````
2. Install Homebrew.
3. Install additional packages by entering the following command.
   ```shell
   $ brew install python3
   ````
4. Perform the build.
   (1). On the terminal, go to `./nanogl/` on the terminal 2.
   (2). Execute `make all`.

### In Ubuntu environment

(We will add a note when we have confirmed that it works)

## Build the sample

1. Build `libnanogl.a` beforehand.
2. Run `msys.bat` or terminal and go to `./example/`.
3. Execute `make all`.

## Using from various languages

### Using from C language

- Add the `src` folder to your `include` path.
- Add `libnanogl.a` when linking.
- Please make additional compile settings for each platform.

#### Compile settings in Windows environment

- Please add `mingw32`, `winmm`, `Avrt`, `OpenGL32`, `Gdi32` to the ³ library you link.
- Add `-mwindows` to the compile-time flags.
- If your C source is written in UTF8, please add `-finput-charset=utf-8 -fexec-charset=cp932` to the compile-time flags.  

#### Compile settings in macOS environment

- Add `OpenGL`, `Foundation`, `IOKit`, `CoreAudio`, `Cocoa`, `AppKit` to linked frameworks.

### For use from `CRuby`.

We provide `nanogl` `gem` for `CRuby`.

1. Please build `libnanogl.a` beforehand. 
2. Run msys.bat or Terminal, and type `./ruby/cruby/gem/`.
3. Execute `make all`.
4. Run `rake build`.
5. Run `gem install -l . Install the `gem` created by `/pkg/nanogl-<version>.gem`.

After installation, you will find the `gem` in the `../example/` folder, `*.rb` will be executable.

### If you want to integrate it into `mruby` and use it.

(This is an old procedure. We will update this page as soon as we confirm that it works)

`CRuby` and the following additional packages are required to build.
Please install them according to your environment.

- `bison` (for the build)
- `flex`
- `git`

```sh
$ make -f Makefile.mrb-nanogl
$ ./mrb-nanogl ./example/00-blank.rb
```

## License.

- Except for the open source project artifacts and libraries used in NanoGL, the NanoGL source code and examples in the examples folder are NYSL version 0.9982.
- Prior to version 0.3.0, NanoGL relied on GPL-licensed libraries; as of 0.3.0, GPL-licensed libraries are no longer required.

## Schedule

- Compatible with "Introduction to the Object-Oriented Language 'Ruby Programming' Text (Introductory Edition)" (published by City Planning Mitaka Co.
- Improved `dxruby` support in `nanogl`.
