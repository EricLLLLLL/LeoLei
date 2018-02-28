#include "hmsetup.h"
#include <QApplication>
#include <modulemanager.h>
#include <wswnd.h>

static QMutex mutex;

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // 加锁
    QMutexLocker locker(&mutex);

    QByteArray localMsg = msg.toLocal8Bit();
    QString strPreMsg = "";
    switch (type)
    {
    case QtDebugMsg:
        strPreMsg = QString("DEBUG");
        printf(localMsg.data());
        break;
    case QtWarningMsg:
#ifdef _DEBUG
        strPreMsg = QString("WARNNING");
#else
        return;
#endif
        break;
    case QtCriticalMsg:
        strPreMsg = QString("ERROR");
        break;
    case QtFatalMsg:
        strPreMsg = QString("ERROR");
        break;
    case QtInfoMsg:
#ifdef _DEBUG
        strPreMsg = QString("INFO");
#else
        return;
#endif
        break;
    }

    // 设置输出信息格式
    QString strDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString strMessage = QString("[%1][%2:%3(%4)]%5")
        .arg(strDateTime).arg(strPreMsg).arg(context.file).arg(context.line).arg(localMsg.constData());

    // 输出信息至文件中（读写、追加形式）
    QString strExePath = QCoreApplication::applicationDirPath();
    QString strFilePath = strExePath + "/qlog.txt";
    QFile file(strFilePath);
    file.open(QIODevice::ReadWrite | QIODevice::Append);
    QTextStream stream(&file);
    stream << strMessage << "\r\n";
    file.flush();
    file.close();

    // 解锁
}

//初始化模块
bool InitModules()
{
    bool bLoadSuc = ModuleManager::LoadAllModules();
    bool bInitSuc = CWSWnd::InitDBusService();
    if(bLoadSuc && bInitSuc)
    {
        return true;
    }
    return false;
}

void ReleaseModules()
{
    ModuleManager::ReleaseAllModules();
    CWSWnd::DestroyDBusService();
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qputenv("QTWEBENGINE_REMOTE_DEBUGGING", "9223");
    qInstallMessageHandler(myMessageOutput);
    bool bInit = InitModules();
    if(!bInit)
    {
        return 0;
    }
    CHMSetup w;
    w.show();

    return a.exec();
}
