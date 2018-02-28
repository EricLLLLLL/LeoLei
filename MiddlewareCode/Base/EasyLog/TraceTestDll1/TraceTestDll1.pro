#-------------------------------------------------
#
# Project created by QtCreator 2018-02-05T17:39:42
#
#-------------------------------------------------

QT       -= gui

TARGET = TraceTestDll1
TEMPLATE = lib

DEFINES += TRACETESTDLL1_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
include("../../Common.pri")

LIBS += -L$$PWD/../../../Common/Lib/$$OUT_CFG -lEasyTrace

SOURCES += \
        tracetestdll1.cpp

HEADERS += \
        tracetestdll1.h \
        tracetestdll1_global.h 

unix {
    target.path = /usr/lib
    INSTALLS += target
}
