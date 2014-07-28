@echo on

set PATH_BAK=%PATH%
set WORKING_DIR=%CD%

rmdir build-Qt4.MinGW /s /q
rmdir build-Qt4.MSVC2008 /s /q
rmdir build-Qt5.MinGW /s /q
rmdir build-Qt5.MSVC2010 /s /q
rmdir AvalonMiner.Qt4MinGW /s /q
rmdir AvalonMiner.Qt5MinGW /s /q
rmdir AvalonMiner.Qt4MSVC /s /q
rmdir AvalonMiner.Qt5MSVC /s /q
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
cd %WORKING_DIR%\build-Qt4.MinGW
qmake %WORKING_DIR%\repo\src\AvalonMiner.pro
mingw32-make
set AvalonDir=%WORKING_DIR%\AvalonMiner.Qt4MinGW
mkdir %AvalonDir%
move release\AvalonMiner.exe %AvalonDir%\AvalonMiner.exe
copy %QTDIR%\bin\QtCore4.dll %AvalonDir%
copy %QTDIR%\bin\QtGui4.dll %AvalonDir%
copy %QTDIR%\bin\QtNetwork4.dll %AvalonDir%
copy %QTDIR%\bin\QtSvg4.dll %AvalonDir%
copy %WORKING_DIR%\MinGW.runtime\* %AvalonDir%
mkdir %AvalonDir%\bfgminer
xcopy /E /H /K %WORKING_DIR%\bfgminer %AvalonDir%\bfgminer
copy %WORKING_DIR%\repo\avalon-example-nano.ini %AvalonDir%\avalon.ini
cd %WORKING_DIR%

rem goto END

:Qt5MinGW
rem
rem Qt5 MinGW
rem From C:\Qt\Qt5.3.1_MinGW\5.3\mingw482_32\bin\qtenv2.bat
rem
set PATH=C:\Qt\Qt5.3.1_MinGW\5.3\mingw482_32\bin;C:\Qt\Qt5.3.1_MinGW\Tools\mingw482_32\bin;%SystemRoot%\System32;%SystemRoot%
set QTDIR=
set QMAKESPEC=
rem Build
cd %WORKING_DIR%\build-Qt5.MinGW
qmake %WORKING_DIR%\repo\src\AvalonMiner.pro
mingw32-make
set AvalonDir=%WORKING_DIR%\AvalonMiner.Qt5MinGW
mkdir %AvalonDir%
move release\AvalonMiner.exe %AvalonDir%\AvalonMiner.exe
cd %AvalonDir%
windeployqt AvalonMiner.exe
mkdir %AvalonDir%\bfgminer
xcopy /E /H /K %WORKING_DIR%\bfgminer %AvalonDir%\bfgminer
copy %WORKING_DIR%\repo\avalon-example-nano.ini %AvalonDir%\avalon.ini
cd %WORKING_DIR%

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
cd %WORKING_DIR%\build-Qt4.MSVC2008
qmake %WORKING_DIR%\repo\src\AvalonMiner.pro
nmake
set AvalonDir=%WORKING_DIR%\AvalonMiner.Qt4MSVC
mkdir %AvalonDir%
move release\AvalonMiner.exe %AvalonDir%\AvalonMiner.exe
copy %QTDIR%\bin\QtCore4.dll %AvalonDir%
copy %QTDIR%\bin\QtGui4.dll %AvalonDir%
copy %QTDIR%\bin\QtNetwork4.dll %AvalonDir%
copy %QTDIR%\bin\QtSvg4.dll %AvalonDir%
copy %WORKING_DIR%\MSVC2008\* %AvalonDir%
mkdir %AvalonDir%\bfgminer
xcopy /E /H /K %WORKING_DIR%\bfgminer %AvalonDir%\bfgminer
copy %WORKING_DIR%\repo\avalon-example-nano.ini %AvalonDir%\avalon.ini
cd %WORKING_DIR%

:Qt5MSVC
rem Qt5 MSVC2010
set QTDIR=
set PATH=C:\Qt\Qt5.3.1_MSVC2010\5.3\msvc2010_opengl\bin;%SystemRoot%\System32;%SystemRoot%
set QMAKESPEC=
call "C:\Program Files\Microsoft Visual Studio 10.0\VC\vcvarsall.bat" x86
rem Build
cd %WORKING_DIR%\build-Qt5.MSVC2010
qmake %WORKING_DIR%\repo\src\AvalonMiner.pro
nmake
set AvalonDir=%WORKING_DIR%\AvalonMiner.Qt5MSVC
mkdir %AvalonDir%
move release\AvalonMiner.exe %AvalonDir%\AvalonMiner.exe
copy %WORKING_DIR%\MSVC2010\* %AvalonDir%
cd %AvalonDir%
windeployqt AvalonMiner.exe
mkdir %AvalonDir%\bfgminer
xcopy /E /H /K %WORKING_DIR%\bfgminer %AvalonDir%\bfgminer
copy %WORKING_DIR%\repo\avalon-example-nano.ini %AvalonDir%\avalon.ini
cd %WORKING_DIR%

:END
set PATH=%PATH_BAK%

