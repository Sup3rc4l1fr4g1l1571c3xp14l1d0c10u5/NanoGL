@echo off

set WGET=wget --no-check-certificate
set BSDTAR=bsdtar -m
set PATCH=patch
set CD=cd
set CP=copy
set XCP=xcopy
set MKDIR=mkdir
set MV=move

set BASEDIR=%~dp0
set PATH=%PATH%;%BASEDIR%tools\bin
set PKGDIR=%BASEDIR%packages
set BUILDDIR=%BASEDIR%build
set PATCHDIR=%BASEDIR%..\patches
set NANOGLDIR=%BASEDIR%..\src
set LIBDIR=%BASEDIR%..\lib

ECHO Download Packages
%MKDIR% %PKGDIR%

IF NOT EXIST %PKGDIR%\nanovg-cacb00b852079db23c180f2e6cbff41eef673783.zip (
%WGET% -O %PKGDIR%\nanovg-cacb00b852079db23c180f2e6cbff41eef673783.zip https://github.com/memononen/nanovg/archive/cacb00b852079db23c180f2e6cbff41eef673783.zip
)

IF NOT EXIST %PKGDIR%\stb-6e4154737c51c1298e35cc6fc387dd365cc32ac9.zip (
%WGET% -O %PKGDIR%\stb-6e4154737c51c1298e35cc6fc387dd365cc32ac9.zip https://github.com/nothings/stb/archive/6e4154737c51c1298e35cc6fc387dd365cc32ac9.zip
)

IF NOT EXIST %PKGDIR%\TinyJPEG-56b6635a23a439a86026c81a5e6c4d27c2d35cd0.zip (
%WGET% -O %PKGDIR%\TinyJPEG-56b6635a23a439a86026c81a5e6c4d27c2d35cd0.zip https://github.com/serge-rgb/TinyJPEG/archive/56b6635a23a439a86026c81a5e6c4d27c2d35cd0.zip
)

IF NOT EXIST %PKGDIR%\glew-1.13.0.zip (
%WGET% -O %PKGDIR%\glew-1.13.0.zip https://sourceforge.net/projects/glew/files/glew/1.13.0/glew-1.13.0.zip/download
)

IF NOT EXIST %PKGDIR%\glfw-3.1.2.zip (
%WGET% -O %PKGDIR%\glfw-3.1.2.zip https://sourceforge.net/projects/glfw/files/glfw/3.1.2/glfw-3.1.2.zip/download
)

IF NOT EXIST %PKGDIR%\openal-soft-1.17.2.tar.bz2 (
%WGET% -O %PKGDIR%\openal-soft-1.17.2.tar.bz2 http://kcat.strangesoft.net/openal-releases/openal-soft-1.17.2.tar.bz2
)

IF NOT EXIST %PKGDIR%\libmad-0.15.1b.tar.gz (
%WGET% -O %PKGDIR%\libmad-0.15.1b.tar.gz https://sourceforge.net/projects/mad/files/libmad/0.15.1b/libmad-0.15.1b.tar.gz/download
)

ECHO Extract archive

%MKDIR% %BUILDDIR%

%BSDTAR% -xf %PKGDIR%\nanovg-cacb00b852079db23c180f2e6cbff41eef673783.zip
%PATCH% -d .\nanovg-cacb00b852079db23c180f2e6cbff41eef673783 -u -p1 < %PATCHDIR%\nanovg-cacb00b852079db23c180f2e6cbff41eef673783.patch
%MV% .\nanovg-cacb00b852079db23c180f2e6cbff41eef673783 %BUILDDIR%\

%BSDTAR% -xf %PKGDIR%\stb-6e4154737c51c1298e35cc6fc387dd365cc32ac9.zip
%MV% .\stb-6e4154737c51c1298e35cc6fc387dd365cc32ac9 %BUILDDIR%\

%BSDTAR% -xf %PKGDIR%\TinyJPEG-56b6635a23a439a86026c81a5e6c4d27c2d35cd0.zip
%MV% .\TinyJPEG-56b6635a23a439a86026c81a5e6c4d27c2d35cd0 %BUILDDIR%\

%BSDTAR% -xf %PKGDIR%\glew-1.13.0.zip 
%MV% .\glew-1.13.0 %BUILDDIR%\

%BSDTAR% -xf %PKGDIR%\glfw-3.1.2.zip
%MV% .\glfw-3.1.2 %BUILDDIR%\

%BSDTAR% -xf %PKGDIR%\openal-soft-1.17.2.tar.bz2
%MV% .\openal-soft-1.17.2 %BUILDDIR%\

%BSDTAR% -xf %PKGDIR%\libmad-0.15.1b.tar.gz
%CP% ".\libmad-0.15.1b\msvc++\config.h" ".\libmad-0.15.1b\config.h"
%CP% ".\libmad-0.15.1b\msvc++\mad.h" ".\libmad-0.15.1b\mad.h"
%MV% .\libmad-0.15.1b %BUILDDIR%\

ECHO Build library.

msbuild nanogl.sln /p:Configuration=Debug /p:Platform=x86
msbuild nanogl.sln /p:Configuration=Release /p:Platform=x86

ECHO Copy library and header to directory

%MKDIR% %LIBDIR%
%CP% %BASEDIR%Debug\Win32\lib\nanogld.lib %LIBDIR%\nanogld.lib
%CP% %BASEDIR%Release\Win32\lib\nanogl.lib %LIBDIR%\nanogl.lib

ECHO Build finish.

pause

