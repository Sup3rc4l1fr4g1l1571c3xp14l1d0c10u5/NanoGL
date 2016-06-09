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
%XCP% %EXAMPLEDIR% %BASEDIR%\example

%CP% %BASEDIR%game.vcxproj %BASEDIR%example\00-Blank
%CP% %BASEDIR%game.vcxproj %BASEDIR%example\01-BasicDraw
%CP% %BASEDIR%game.vcxproj %BASEDIR%example\02-TransformSingle
%CP% %BASEDIR%game.vcxproj %BASEDIR%example\03-TransformMulti
%CP% %BASEDIR%game.vcxproj %BASEDIR%example\04-Image
%CP% %BASEDIR%game.vcxproj %BASEDIR%example\05-Keyboard
%CP% %BASEDIR%game.vcxproj %BASEDIR%example\06-Mouse
%CP% %BASEDIR%game.vcxproj %BASEDIR%example\07-Text
%CP% %BASEDIR%game.vcxproj %BASEDIR%example\08-Sprite
%CP% %BASEDIR%game.vcxproj %BASEDIR%example\09-Sound
%CP% %BASEDIR%game.vcxproj %BASEDIR%example\10-Scene
%CP% %BASEDIR%game.vcxproj %BASEDIR%example\50-15Puzzle
%CP% %BASEDIR%game.vcxproj %BASEDIR%example\51-STG

msbuild %BASEDIR%example\00-Blank\game.vcxproj /p:Configuration=Release /p:Platform=x86
msbuild %BASEDIR%example\01-BasicDraw\game.vcxproj /p:Configuration=Release /p:Platform=x86
msbuild %BASEDIR%example\02-TransformSingle\game.vcxproj /p:Configuration=Release /p:Platform=x86
msbuild %BASEDIR%example\03-TransformMulti\game.vcxproj /p:Configuration=Release /p:Platform=x86
msbuild %BASEDIR%example\04-Image\game.vcxproj /p:Configuration=Release /p:Platform=x86
msbuild %BASEDIR%example\05-Keyboard\game.vcxproj /p:Configuration=Release /p:Platform=x86
msbuild %BASEDIR%example\06-Mouse\game.vcxproj /p:Configuration=Release /p:Platform=x86
msbuild %BASEDIR%example\07-Text\game.vcxproj /p:Configuration=Release /p:Platform=x86
msbuild %BASEDIR%example\08-Sprite\game.vcxproj /p:Configuration=Release /p:Platform=x86
msbuild %BASEDIR%example\09-Sound\game.vcxproj /p:Configuration=Release /p:Platform=x86
msbuild %BASEDIR%example\10-Scene\game.vcxproj /p:Configuration=Release /p:Platform=x86
msbuild %BASEDIR%example\50-15Puzzle\game.vcxproj /p:Configuration=Release /p:Platform=x86
msbuild %BASEDIR%example\51-STG\game.vcxproj /p:Configuration=Release /p:Platform=x86

ECHO Build finish.

pause

