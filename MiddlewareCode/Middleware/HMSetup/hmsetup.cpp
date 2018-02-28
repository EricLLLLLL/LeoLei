#include "hmsetup.h"
#include "ui_hmsetup.h"
#include <QtWidgets/QVBoxLayout>
#include <modulemanager.h>
#include <QWebEngineView>
#include <QSettings>
#include <Helper/helper.h>

CHMSetup::CHMSetup(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CHMSetup)
{
    ui->setupUi(this);

    QString strExePath = QCoreApplication::applicationDirPath();
    QString strCfgPath = strExePath+"/config/config.ini";
    QString strIndexUrl = g_Helper.GetPrivateProfileString("web","index","",strCfgPath);
    QString strURL = "file://" + strExePath + strIndexUrl;

    QVBoxLayout* pLayout = new QVBoxLayout(this);
    pLayout->setContentsMargins(0,0,0,0);

    m_pWebView = new QWebEngineView(this);
    pLayout->addWidget(m_pWebView);

    __InitWebObjs();
    m_oWebChannel.registerObjects(m_hashWebObjs);
    m_pWebView->page()->setWebChannel(&m_oWebChannel);

    m_pWebView->setUrl(QUrl(strURL));
    this->setLayout(pLayout);
    QSize stDefaultSize = QSize(1024,768);
    resize(stDefaultSize);
}

CHMSetup::~CHMSetup()
{
    delete ui;
}

void CHMSetup::keyReleaseEvent(QKeyEvent *pEvent)
{
    int iKey = pEvent->key();
    switch (iKey) {
    case Qt::Key_F5:
        m_pWebView->reload();
        break;
    case Qt::Key_F11:
        if(!this->isFullScreen())
        {
            this->showFullScreen();
        }
        else
        {
            this->showNormal();
        }

        break;
    default:
        break;
    }
}

void CHMSetup::__InitWebObjs()
{
    for(int iIndex = 0; iIndex < ModuleManager::MODULE_MAX_COUNT; iIndex++)
    {
        ModuleManager::ModuleInfo stModuleInfo;
        ModuleManager::GetModuleInfoByIndex(iIndex,stModuleInfo);
        if(stModuleInfo.pModuleObj && strcmp(stModuleInfo.strModuleName,"") != 0)
        {
            m_hashWebObjs[stModuleInfo.strModuleName] = stModuleInfo.pModuleObj;
        }
    }
}
