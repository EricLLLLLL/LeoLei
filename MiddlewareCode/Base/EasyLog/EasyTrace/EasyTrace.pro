#-------------------------------------------------
#
# Project created by QtCreator 2018-02-02T15:25:29
#
#-------------------------------------------------

QT       -= gui

TARGET = EasyTrace
TEMPLATE = lib
CONFIG += staticlib

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
#include("../../Common.pri")

OUT_CFG=
CONFIG(debug, debug|release){
    OUT_CFG=debug
}else{
    OUT_CFG=release
}

INCLUDEPATH +=$$PWD/../../../Common/ \
              $$PWD/../../../Common/include \

SOURCES += \
        easytrace.cpp

HEADERS += \
        easytrace.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}

QMAKE_POST_LINK = cp -rf $$PWD/easytrace.h $$quote($$PWD/../../../Common/include)
