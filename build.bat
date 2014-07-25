@echo on

set PATH_BAK=%PATH%

rmdir build-Qt4.MinGW /s /q
rmdir build-Qt4.MSVC2008 /s /q
rmdir build-Qt5.MinGW /s /q
rmdir build-Qt5.MSVC2010 /s /q
mkdir build-Qt4.MinGW
mkdir build-Qt4.MSVC2008
mkdir build-Qt5.MinGW
mkdir build-Qt5.MSVC2010

:Qt4MinGW
rem
rem Qt4 MinGW
rem From C:\Qt\4.8.6_MinGW\bin\qtvars.bat
rem
set QTDIR=C:\Qt\4.8.6_MinGW
set PATH=C:\Qt\4.8.6_MinGW\bin
set PATH=%PATH%;C:\Qt\mingw32\bin
set PATH=%PATH%;%SystemRoot%\System32
set QMAKESPEC=win32-g++-4.6
rem Build
cd build-Qt4.MinGW
qmake ..\AvalonMiner\AvalonMiner.pro
mingw32-make
set AvalonDir=AvalonMiner.Qt4MinGW
mkdir %AvalonDir%
move release\AvalonMiner.exe %AvalonDir%\AvalonMiner.exe
copy %QTDIR%\bin\QtCore4.dll %AvalonDir%
copy %QTDIR%\bin\QtGui4.dll %AvalonDir%
copy %QTDIR%\bin\QtNetwork4.dll %AvalonDir%
copy %~dp0\MinGW.runtime\* %AvalonDir%
cd ..

rem goto END

:Qt5MinGW
rem
rem Qt5 MinGW
rem From C:\Qt\Qt5.3.0_MinGW\5.3\mingw482_32\bin\qtenv2.bat
rem
set PATH=C:\Qt\Qt5.3.0_MinGW\5.3\mingw482_32\bin;C:\Qt\Qt5.3.0_MinGW\Tools\mingw482_32\bin;%SystemRoot%\System32;%SystemRoot%
set QTDIR=
set QMAKESPEC=
rem Build
cd build-Qt5.MinGW
qmake ..\AvalonMiner\AvalonMiner.pro
mingw32-make
set AvalonDir=AvalonMiner.Qt5MinGW
mkdir %AvalonDir%
move release\AvalonMiner.exe %AvalonDir%\AvalonMiner.exe
cd %AvalonDir%
windeployqt AvalonMiner.exe
cd ..\..

:Qt4MSVC
rem
rem Qt4 MSVC2008
rem From C:\Qt\4.8.6_MSVC2008\bin\qtvars.bat vsvars
rem
set QTDIR=C:\Qt\4.8.6_MSVC2008
set PATH=C:\Qt\4.8.6_MSVC2008\bin;%SystemRoot%\System32;%SystemRoot%
set QMAKESPEC=win32-msvc2008
call "C:\Program Files\Microsoft Visual Studio 9.0\VC\vcvarsall.bat" x86
rem Build
cd build-Qt4.MSVC2008
qmake ..\AvalonMiner\AvalonMiner.pro
nmake
set AvalonDir=AvalonMiner.Qt4MSVC
mkdir %AvalonDir%
move release\AvalonMiner.exe %AvalonDir%\AvalonMiner.exe
copy %QTDIR%\bin\QtCore4.dll %AvalonDir%
copy %QTDIR%\bin\QtGui4.dll %AvalonDir%
copy %QTDIR%\bin\QtNetwork4.dll %AvalonDir%
copy %~dp0\MSVC2008\* %AvalonDir%
cd ..

:Qt5MSVC
rem Qt5 MSVC2010
set QTDIR=
set PATH=C:\Qt\Qt5.3.0_MSVC2010\5.3\msvc2010_opengl\bin;%SystemRoot%\System32;%SystemRoot%
set QMAKESPEC=
call "C:\Program Files\Microsoft Visual Studio 10.0\VC\vcvarsall.bat" x86
rem Build
cd build-Qt5.MSVC2010
qmake ..\AvalonMiner\AvalonMiner.pro
nmake
set AvalonDir=AvalonMiner.Qt5MSVC
mkdir %AvalonDir%
move release\AvalonMiner.exe %AvalonDir%\AvalonMiner.exe
copy %~dp0\MSVC2010\* %AvalonDir%
cd %AvalonDir%
windeployqt AvalonMiner.exe
cd ..\..

:END
set PATH=%PATH_BAK%

