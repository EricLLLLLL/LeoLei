#-------------------------------------------------
#
# Project created by QtCreator 2018-01-27T11:47:02
#
#-------------------------------------------------

QT       -= gui

TARGET = MTCP
TEMPLATE = lib

DEFINES += MTCP_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
include("../../Common/ProjectProperties/Common.pri")

#包含外部库
LIBS += -L$$PWD/../../Common/lib/debug/ -llfs

#包含路径
INCLUDEPATH += $$PWD/../../Common/include

SOURCES += \
        mtcp.cpp \
    ../../Common/dllDSTO.cpp \
    ../../Common/dllDRCV.cpp \
    ../../Common/dllDSND.cpp

HEADERS += \
        mtcp.h \
        mtcp_global.h \ 
    ../../Common/LFS/include/win_type_def.h \
    ../../Common/dllDSTO.h \
    ../../Common/dllDRCV.h \
    ../../Common/dllDSND.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
