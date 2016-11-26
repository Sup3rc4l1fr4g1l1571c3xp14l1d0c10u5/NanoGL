@ECHO off
setlocal enabledelayedexpansion

IF EXIST %windir%\Microsoft.NET\Framework\v3.5\csc.exe (
ECHO "Use .NET Framework 3.5"
set CSC=%windir%\Microsoft.NET\Framework\v3.5\csc.exe
) ELSE IF EXIST %windir%\Microsoft.NET\Framework\v4.0.30319\csc.exe (
ECHO "Use .NET Framework 4.0.30319"
set CSC=%windir%\Microsoft.NET\Framework\v4.0.30319\csc.exe
) ELSE (
ECHO ".NET Framework is not found."
GOTO END
)

IF NOT EXIST nwget.exe (
ECHO "'Build nwget.exe (nano wget like tool) from source code.'"
!CSC! nwget.cs
) else (
ECHO "nwget.exe is exists."
)

IF EXIST nwget.exe (
set NWGET=nwget.exe
) ELSE (
ECHO "nwget.exe not found. Compile failed?"
GOTO END
)

IF NOT EXIST UnzipExample.exe (
ECHO "Download and building unzip tool."
!NWGET! Unzip.cs https://github.com/yallie/unzip/raw/49e5f9f0ff612581d473b9f062f1db4820cf20d3/Unzip.cs
!NWGET! UnzipExample.cs https://github.com/yallie/unzip/raw/49e5f9f0ff612581d473b9f062f1db4820cf20d3/UsageExample.cs
!CSC! Unzip.cs UnzipExample.cs
) else (
ECHO "UnzipExample.exe is exists."
)

IF EXIST UnzipExample.exe (
set UNZIP=UnzipExample.exe
) ELSE (
ECHO "UnzipExample.exe not found. Compile failed?"
GOTO END
)

ECHO "Download building tools"

mkdir tools

ECHO "Download bison-2.4.1 Binary and Dependencies"
IF NOT EXIST tools/bison-2.4.1-bin.zip !NWGET! tools/bison-2.4.1-bin.zip http://downloads.sourceforge.net/gnuwin32/bison-2.4.1-bin.zip
IF NOT EXIST tools/bison-2.4.1-dep.zip !NWGET! tools/bison-2.4.1-dep.zip http://downloads.sourceforge.net/gnuwin32/bison-2.4.1-dep.zip

ECHO "Download flex-2.5.4a-1 Binary (no Dependencies)"
IF NOT EXIST tools/flex-2.5.4a-1-bin.zip !NWGET! tools/flex-2.5.4a-1-bin.zip http://downloads.sourceforge.net/gnuwin32/flex-2.5.4a-1-bin.zip

ECHO "Download libarchive-2.5.12-1 Binary and Dependencies (Include bsdtar)"
IF NOT EXIST tools/libarchive-2.4.12-1-bin.zip !NWGET! tools/libarchive-2.4.12-1-bin.zip http://downloads.sourceforge.net/gnuwin32/libarchive-2.4.12-1-bin.zip
IF NOT EXIST tools/libarchive-2.4.12-1-dep.zip !NWGET! tools/libarchive-2.4.12-1-dep.zip http://downloads.sourceforge.net/gnuwin32/libarchive-2.4.12-1-dep.zip

ECHO "Download patch-2.5.9 Binary (no Dependencies, but manifestfile is need.)"
IF NOT EXIST tools/patch-2.5.9-7-bin.zip !NWGET! tools/patch-2.5.9-7-bin.zip http://downloads.sourceforge.net/gnuwin32/patch-2.5.9-7-bin.zip

ECHO "Download wget Binary and Dependencies"
IF NOT EXIST tools/wget-1.11.4-1-bin.zip !NWGET! tools/wget-1.11.4-1-bin.zip http://downloads.sourceforge.net/gnuwin32/wget-1.11.4-1-bin.zip
IF NOT EXIST tools/wget-1.11.4-1-dep.zip !NWGET! tools/wget-1.11.4-1-dep.zip http://downloads.sourceforge.net/gnuwin32/wget-1.11.4-1-dep.zip

ECHO "Extract ZIP files"

!UNZIP! tools/bison-2.4.1-bin.zip ./tools
!UNZIP! tools/bison-2.4.1-dep.zip ./tools
!UNZIP! tools/flex-2.5.4a-1-bin.zip ./tools
!UNZIP! tools/libarchive-2.4.12-1-bin.zip ./tools
!UNZIP! tools/libarchive-2.4.12-1-dep.zip ./tools
!UNZIP! tools/patch-2.5.9-7-bin.zip ./tools
!UNZIP! tools/wget-1.11.4-1-bin.zip ./tools
!UNZIP! tools/wget-1.11.4-1-dep.zip ./tools

COPY ..\patches\patch.exe.manifest .\tools\bin

ECHO "Extract Finished"

:END

endlocal

pause

