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

IF NOT EXIST %PKGDIR%\nanovg-b83cf926525e7cea8d2483da2a75852b8c7b6d28.zip (
%WGET% -O %PKGDIR%\nanovg-b83cf926525e7cea8d2483da2a75852b8c7b6d28.zip https://github.com/memononen/nanovg/archive/b83cf926525e7cea8d2483da2a75852b8c7b6d28.zip
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

%BSDTAR% -xf %PKGDIR%\nanovg-b83cf926525e7cea8d2483da2a75852b8c7b6d28.zip
%PATCH% -d .\nanovg-b83cf926525e7cea8d2483da2a75852b8c7b6d28 -u -p1 < %PATCHDIR%\nanovg-b83cf926525e7cea8d2483da2a75852b8c7b6d28.patch
%MV% .\nanovg-b83cf926525e7cea8d2483da2a75852b8c7b6d28 %BUILDDIR%\

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

