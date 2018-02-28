
INCLUDEPATH +=$$PWD/../../../Common/include

SOURCES += \
    $$PWD/loghelper.cpp \
    $$PWD/loghelper.h \



HEADERS += \


#动态库输出路径
DEST_ROOTDIR_DLL=$$PWD/../../../out/
#静态库输出路径
DEST_ROOTDIR_LIB=$$PWD/../../../Common/Lib/

OUT_CFG=
CONFIG(debug, debug|release){
    OUT_CFG=debug
}else{
    OUT_CFG=release
}

contains(CONFIG, "staticlib") {
    DESTDIR = $$DEST_ROOTDIR_LIB/$$OUT_CFG        #放在静态库目录下
} else {
    DESTDIR = $$DEST_ROOTDIR_DLL/$$OUT_CFG/lib        #将库放在动态库目录下
}

#OBJECTS_DIR = $$DEST_ROOTDIR/tmp/obj   #将生成的对象文件放在专门的obj文件夹下
#MOC_DIR = $$DEST_ROOTDIR/tmp/moc       #将QT自动生成的对象放在moc文件夹下
#UI_DIR = $$DEST_ROOTDIR/tmp/ui
