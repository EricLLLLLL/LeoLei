OUT_CFG=
CONFIG(debug, debug|release){
    OUT_CFG=debug
    CONFIG+=precompile_header       #debug模式下使用预编译头
    PRECOMPILED_HEADER = stdafx.h
}else{
    OUT_CFG=release
}

DEST_ROOTDIR=$$PWD/../../out/

INCLUDEPATH +=$$PWD/../../Common/ \
            $$PWD/../../Common/include

#包含外部库
LIB_DIR=$$PWD/../../Common/Lib/
LIBS += -L$$LIB_DIR/$$OUT_CFG -lEasyTrace \
        -L$$DEST_ROOTDIR/$$OUT_CFG/lib -lAlarm

#源文件
SOURCES += \
    $$PWD/../../Common/Helper/helper.cpp \
    $$PWD/../../Common/Helper/helper.h \

HEADERS += \


contains(TEMPLATE, "lib") {
    DESTDIR = $$DEST_ROOTDIR/$$OUT_CFG/lib        #将库放在lib文件夹下
} else {
    DESTDIR = $$DEST_ROOTDIR/$$OUT_CFG/        #将应用程序放在bin目录下
}
#OBJECTS_DIR = $$DEST_ROOTDIR/tmp/obj   #将生成的对象文件放在专门的obj文件夹下
#MOC_DIR = $$DEST_ROOTDIR/tmp/moc       #将QT自动生成的对象放在moc文件夹下
#UI_DIR = $$DEST_ROOTDIR/tmp/ui
