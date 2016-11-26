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
set EXAMPLEDIR=%BASEDIR%..\example

ECHO Build Examples

%MKDIR% %BASEDIR%example\
%XCP% %EXAMPLEDIR%\Image %BASEDIR%\example\Image\
%XCP% %EXAMPLEDIR%\Sound %BASEDIR%\example\Sound\
%XCP% %EXAMPLEDIR%\Font %BASEDIR%\example\Font\

pause

%CP% %BASEDIR%example.vcxproj %BASEDIR%00-Blank.vcxproj
%CP% %BASEDIR%example.vcxproj %BASEDIR%01-BasicDraw.vcxproj
%CP% %BASEDIR%example.vcxproj %BASEDIR%02-TransformSingle.vcxproj
%CP% %BASEDIR%example.vcxproj %BASEDIR%03-TransformMulti.vcxproj
%CP% %BASEDIR%example.vcxproj %BASEDIR%04-Image.vcxproj
%CP% %BASEDIR%example.vcxproj %BASEDIR%05-Keyboard.vcxproj
%CP% %BASEDIR%example.vcxproj %BASEDIR%06-Mouse.vcxproj
%CP% %BASEDIR%example.vcxproj %BASEDIR%07-Text.vcxproj
%CP% %BASEDIR%example.vcxproj %BASEDIR%08-Sprite.vcxproj
%CP% %BASEDIR%example.vcxproj %BASEDIR%09-Sound.vcxproj
%CP% %BASEDIR%example.vcxproj %BASEDIR%10-Scene.vcxproj
%CP% %BASEDIR%example.vcxproj %BASEDIR%11-Dialog.vcxproj
%CP% %BASEDIR%example.vcxproj %BASEDIR%50-15Puzzle.vcxproj
%CP% %BASEDIR%example.vcxproj %BASEDIR%51-STG.vcxproj

msbuild %BASEDIR%00-Blank.vcxproj /p:Configuration=Release /p:Platform=x86
msbuild %BASEDIR%01-BasicDraw.vcxproj /p:Configuration=Release /p:Platform=x86
msbuild %BASEDIR%02-TransformSingle.vcxproj /p:Configuration=Release /p:Platform=x86
msbuild %BASEDIR%03-TransformMulti.vcxproj /p:Configuration=Release /p:Platform=x86
msbuild %BASEDIR%04-Image.vcxproj /p:Configuration=Release /p:Platform=x86
msbuild %BASEDIR%05-Keyboard.vcxproj /p:Configuration=Release /p:Platform=x86
msbuild %BASEDIR%06-Mouse.vcxproj /p:Configuration=Release /p:Platform=x86
msbuild %BASEDIR%07-Text.vcxproj /p:Configuration=Release /p:Platform=x86
msbuild %BASEDIR%08-Sprite.vcxproj /p:Configuration=Release /p:Platform=x86
msbuild %BASEDIR%09-Sound.vcxproj /p:Configuration=Release /p:Platform=x86
msbuild %BASEDIR%10-Scene.vcxproj /p:Configuration=Release /p:Platform=x86
msbuild %BASEDIR%11-Dialog.vcxproj /p:Configuration=Release /p:Platform=x86
msbuild %BASEDIR%50-15Puzzle.vcxproj /p:Configuration=Release /p:Platform=x86
msbuild %BASEDIR%51-STG.vcxproj /p:Configuration=Release /p:Platform=x86

ECHO Build finish.

pause

