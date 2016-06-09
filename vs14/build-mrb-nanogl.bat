@echo off

set WGET=wget --no-check-certificate
set BSDTAR=bsdtar -m
set PATCH=patch
set CD=cd
set CP=copy
set XCP=xcopy /S /Y
set MKDIR=mkdir
set MV=move

set BASEDIR=%~dp0
set PATH=%PATH%;%BASEDIR%tools\bin
set PKGDIR=%BASEDIR%packages
set PATCH_DIR=%BASEDIR%..\patches
set BUILDDIR=%BASEDIR%build
set EXAMPLEDIR=%BASEDIR%..\example

REM OVERRIDE INCLUDE CRT
set UCRTVersion=10.0.10150.0
set INCLUDE=%ProgramFiles(x86)%\Windows Kits\10\include\%UCRTVersion%\ucrt;%INCLUDE%
set LIB=%ProgramFiles(x86)%\Windows Kits\10\lib\%UCRTVersion%\ucrt\x86;%LIB%

ECHO Download mruby Packages

%MKDIR% %PKGDIR%

%WGET% -O %PKGDIR%\mruby-1.2.0.zip https://github.com/mruby/mruby/archive/1.2.0.zip

ECHO Extract mruby archive

%MKDIR% %BUILDDIR%

ECHO Build mruby

%CD% %BUILDDIR%
%BSDTAR% -xf %PKGDIR%\mruby-1.2.0.zip
%PATCH% -u -p1 < %PATCH_DIR%\mruby-1.2.0-build_conf.patch

%CD% %BUILDDIR%\mruby-1.2.0

ruby ./minirake

%CD% %BASEDIR%

REM msbuild mrb-nanogl.vcxproj /p:Configuration=Debug /p:Platform=x86
msbuild mrb-nanogl.vcxproj /p:Configuration=Release /p:Platform=x86

ECHO Build finish.

%CD% %BASEDIR%

%CP% %BASEDIR%\build\mrb-nanogl\Release\Win32\bin\mrb-nanogl.exe %BASEDIR%

pause

