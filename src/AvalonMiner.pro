#-------------------------------------------------
#
# Project created by QtCreator 2014-05-24T04:57:32
#
#-------------------------------------------------

QT       += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG   += qwt

CONFIG   += warn_on release
CONFIG(debug, debug|release){
    DESTDIR = ./debug
    OBJECTS_DIR = debug/.obj
    MOC_DIR = debug/.moc
    RCC_DIR = debug/.rcc
    UI_DIR = debug/.ui
}
CONFIG(release, debug|release){
    DESTDIR = ./release
    OBJECTS_DIR = release/.obj
    MOC_DIR = release/.moc
    RCC_DIR = release/.rcc
    UI_DIR = release/.ui
}

TARGET = AvalonMiner
TEMPLATE = app


SOURCES += main.cpp\
    mainwindow.cpp \
    minerconfig.cpp \
    bfgminer.cpp \
    rpcthread.cpp \
    minerplot.cpp \
    minerstat.cpp

HEADERS  += mainwindow.h \
    minerconfig.h \
    bfgminer.h \
    bfgconf.h \
    rpcthread.h \
    minerplot.h \
    minerstat.h

FORMS    += mainwindow.ui

RESOURCES += Resources.qrc

isEqual(QT_MAJOR_VERSION, 4): RC_FILE = logo.rc
isEqual(QT_MAJOR_VERSION, 5): RC_ICONS = logo.ico

