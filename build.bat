@echo on

set PATH_BAK=%PATH%
set WORKING_DIR=%CD%

rem rmdir build-Qt4.MinGW /s /q
rem rmdir build-Qt4.MSVC2008 /s /q
rmdir build-Qt5.MinGW /s /q
rmdir build-Qt5.MSVC2010 /s /q
rem rmdir AvalonMiner.Qt4MinGW /s /q
rmdir AvalonMiner.Qt5MinGW /s /q
rem rmdir AvalonMiner.Qt4MSVC /s /q
rmdir AvalonMiner.Qt5MSVC /s /q
rem mkdir build-Qt4.MinGW
rem mkdir build-Qt4.MSVC2008
mkdir build-Qt5.MinGW
mkdir build-Qt5.MSVC2010

rem :Qt4MinGW
rem rem
rem rem Qt4 MinGW
rem rem From C:\Qt\4.8.6_MinGW\bin\qtvars.bat
rem rem
rem set QTDIR=C:\Qt\4.8.6_MinGW
rem set PATH=C:\Qt\4.8.6_MinGW\bin
rem set PATH=%PATH%;C:\Qt\mingw32\bin
rem set PATH=%PATH%;%SystemRoot%\System32
rem set QMAKESPEC=win32-g++-4.6
rem rem Build
rem cd %WORKING_DIR%\build-Qt4.MinGW
rem qmake %WORKING_DIR%\repo\src\AvalonMiner.pro
rem mingw32-make
rem set AvalonDir=%WORKING_DIR%\AvalonMiner.Qt4MinGW
rem mkdir %AvalonDir%
rem move release\AvalonMiner.exe %AvalonDir%\AvalonMiner.exe
rem copy %QTDIR%\bin\QtCore4.dll %AvalonDir%
rem copy %QTDIR%\bin\QtGui4.dll %AvalonDir%
rem copy %QTDIR%\bin\QtNetwork4.dll %AvalonDir%
rem copy %QTDIR%\bin\QtSvg4.dll %AvalonDir%
rem copy %WORKING_DIR%\MinGW.runtime\* %AvalonDir%
rem mkdir %AvalonDir%\bfgminer
rem xcopy /E /H /K %WORKING_DIR%\bfgminer %AvalonDir%\bfgminer
rem copy %WORKING_DIR%\repo\avalon-example.ini %AvalonDir%\avalon.ini
rem cd %WORKING_DIR%

rem rem goto END

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
copy release\AvalonMiner.exe %AvalonDir%\AvalonMiner.exe
cd %AvalonDir%
windeployqt AvalonMiner.exe
xcopy /E /H /K /Q %WORKING_DIR%\bfgminer %AvalonDir%\bfgminer\
copy %WORKING_DIR%\repo\avalon-example.ini %AvalonDir%\avalon.ini
cd %WORKING_DIR%

rem :Qt4MSVC
rem rem
rem rem Qt4 MSVC2008
rem From C:\Qt\4.8.6_MSVC2008\bin\qtvars.bat vsvars
rem rem
rem set QTDIR=C:\Qt\4.8.6_MSVC2008
rem set PATH=C:\Qt\4.8.6_MSVC2008\bin;%SystemRoot%\System32;%SystemRoot%
rem set QMAKESPEC=win32-msvc2008
rem call "C:\Program Files\Microsoft Visual Studio 9.0\VC\vcvarsall.bat" x86
rem rem Build
rem cd %WORKING_DIR%\build-Qt4.MSVC2008
rem qmake %WORKING_DIR%\repo\src\AvalonMiner.pro
rem nmake
rem set AvalonDir=%WORKING_DIR%\AvalonMiner.Qt4MSVC
rem mkdir %AvalonDir%
rem move release\AvalonMiner.exe %AvalonDir%\AvalonMiner.exe
rem copy %QTDIR%\bin\QtCore4.dll %AvalonDir%
rem copy %QTDIR%\bin\QtGui4.dll %AvalonDir%
rem copy %QTDIR%\bin\QtNetwork4.dll %AvalonDir%
rem copy %QTDIR%\bin\QtSvg4.dll %AvalonDir%
rem copy %WORKING_DIR%\MSVC2008\* %AvalonDir%
rem mkdir %AvalonDir%\bfgminer
rem xcopy /E /H /K %WORKING_DIR%\bfgminer %AvalonDir%\bfgminer
rem copy %WORKING_DIR%\repo\avalon-example.ini %AvalonDir%\avalon.ini
rem cd %WORKING_DIR%

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
copy release\AvalonMiner.exe %AvalonDir%\AvalonMiner.exe
copy %WORKING_DIR%\MSVC2010\* %AvalonDir%
cd %AvalonDir%
windeployqt AvalonMiner.exe
xcopy /E /H /K /Q %WORKING_DIR%\bfgminer %AvalonDir%\bfgminer\
copy %WORKING_DIR%\repo\avalon-example.ini %AvalonDir%\avalon.ini
cd %WORKING_DIR%

:END
set PATH=%PATH_BAK%

