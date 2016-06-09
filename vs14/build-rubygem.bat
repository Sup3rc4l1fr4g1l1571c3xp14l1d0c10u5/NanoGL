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

ECHO Copy rubygem Packages

%MKDIR% %BUILDDIR%cruby

%XCP% %BASEDIR%..\ruby\cruby\* %BUILDDIR%\cruby\

ECHO Build gem

%CD% %BASEDIR%

REM msbuild nanogl.dll.vcxproj /p:Configuration=Debug /p:Platform=x86
msbuild nanogl.dll.vcxproj /p:Configuration=Release /p:Platform=x86

%CP% %BUILDDIR%\cruby\nanogl.dll\Release\lib\NanoGL.dll %BUILDDIR%\cruby\gem\lib

%CD% %BUILDDIR%\cruby\gem

cmd /C rake build

ECHO Build finish.

%CD% %BASEDIR%
%CP% %BUILDDIR%\cruby\gem\pkg\nanogl-0.1.0.gem %BASEDIR%\nanogl-0.1.0.gem

pause

