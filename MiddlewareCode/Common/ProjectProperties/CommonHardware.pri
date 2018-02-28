OUT_CFG=
CONFIG(debug, debug|release){
    OUT_CFG=debug
    CONFIG+=precompile_header       #debug模式下使用预编译头
    PRECOMPILED_HEADER = stdafx.h
}else{
    OUT_CFG=release
}

INCLUDEPATH +=../../Common/ \
            ../../Common/include \
            ../../Common/LFS/include

#包含外部库
LIB_DIR=../../Common/Lib/
LIBS += -L../../Common/LFS/lib/ -llfs \
        -L$$LIB_DIR/$$OUT_CFG -lEasyTrace \
        -L$$LIB_DIR/$$OUT_CFG -lAPICTRL

#源文件
SOURCES += \
    ../../Common/Helper/helper.cpp \
    ../../Common/Helper/helper.h \
    ../../Common/webinteractobj.h \
    ../../Common/webinteractobj.cpp \

HEADERS += \



DEST_ROOTDIR=../../out/

contains(TEMPLATE, "lib") {
    DESTDIR = $$DEST_ROOTDIR/$$OUT_CFG/lib        #将库放在lib文件夹下
} else {
    DESTDIR = $$DEST_ROOTDIR/$$OUT_CFG/        #将应用程序放在bin目录下
}
#OBJECTS_DIR = $$DEST_ROOTDIR/tmp/obj   #将生成的对象文件放在专门的obj文件夹下
#MOC_DIR = $$DEST_ROOTDIR/tmp/moc       #将QT自动生成的对象放在moc文件夹下
#UI_DIR = $$DEST_ROOTDIR/tmp/ui
