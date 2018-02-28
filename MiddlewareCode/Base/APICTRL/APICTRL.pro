#-------------------------------------------------
#
# Project created by QtCreator 2018-01-24T16:42:26
#
#-------------------------------------------------

QT       -= gui
QT += dbus

TARGET = APICTRL
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
include("../Common.pri")

#包含外部库
LIBS += -L$$PWD/../../Common/LFS/lib/ -llfs

SOURCES += \
        apictrl.cpp \
    wswnd.cpp \


HEADERS += \
        apictrl.h \
    wswnd.h \

unix {
    target.path = /usr/lib
    INSTALLS += target
}

QMAKE_POST_LINK = cp -rf $$PWD/apictrl.h $$PWD/../../Common/include && cp -rf $$PWD/wswnd.h $$PWD/../../Common/include
