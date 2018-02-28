#include "modulemanager.h"
#include "Helper/helper.h"
#include <QDebug>

namespace ModuleManager
{
    typedef QObject* (*CreateModuleFuc)();

    static ModuleInfo Module_Array[32];


    bool LoadAllModules()
    {
        QString strExePath = QCoreApplication::applicationDirPath();
        QString strCfgPath = strExePath + "/config/Module.ini";
        QString strHardwareModule = g_Helper.GetPrivateProfileString("MODULE","Module_H","",strCfgPath);
        QStringList listHardwareModule = strHardwareModule.split(",");

        QString strSoftwareModule = g_Helper.GetPrivateProfileString("MODULE","Module_N","",strCfgPath);
        QStringList listSoftwareModule = strSoftwareModule.split(",");

        QString strEXModule =  g_Helper.GetPrivateProfileString("MODULE_Ex","Module","",strCfgPath);
        QStringList listEXModule = strEXModule.split(",");

        QStringList listAllModule = listHardwareModule + listSoftwareModule + listEXModule;

        memset(Module_Array,0,sizeof(Module_Array));
        for(int iIndex=0; iIndex < listAllModule.count()&&iIndex < MODULE_MAX_COUNT;iIndex++)
        {
            QString strModule = listAllModule.at(iIndex);
            if(strModule == "")
            {
                continue;
            }

            QString strLibPath = strExePath + "/lib/lib" + strModule + ".so";
            QLibrary * pLib = new QLibrary(strLibPath);
            if(pLib == NULL)
            {
                qCritical()<<"create QLibrary failed. strLibPath="<<strLibPath;
                continue;
            }

            pLib->load();
            if(!pLib->isLoaded())
            {
                qCritical()<<"load module failed.  strModule="<<strModule<<"   error:"<<pLib->errorString();
                continue;
            }

            CreateModuleFuc Func;
            Func = (CreateModuleFuc)pLib->resolve("CreateInstance");
            if(Func)
            {
                QObject* pModuleObj = Func();
                if(pModuleObj)
                {
                    ModuleInfo& stModuleInfo = Module_Array[iIndex];
                    strncpy(stModuleInfo.strModuleName,strModule.toLocal8Bit().data(),256);
                    stModuleInfo.pLoadLibrary = pLib;
                    stModuleInfo.pModuleObj = pModuleObj;
                    qInfo()<<"Load module success.  strModule="<<strModule;
                }
                else
                {
                    qCritical()<<"Get the module obj failed. strModule="<<strModule;
                }
            }
            else
            {
                qCritical()<<"Get the module export function address failed. strModule="<<strModule;
            }
        }

        return true;
    }

    void ReleaseAllModules()
    {
        for(int iIndex = 0; iIndex < MODULE_MAX_COUNT; iIndex++)
        {
            ModuleInfo& stModuleInfo = Module_Array[iIndex];
            if(stModuleInfo.pLoadLibrary && stModuleInfo.pModuleObj)
            {
                SAFE_DELETE(stModuleInfo.pModuleObj);
                stModuleInfo.pLoadLibrary->unload();
                SAFE_DELETE(stModuleInfo.pLoadLibrary);
                qInfo()<<"unload module success. module="<<stModuleInfo.strModuleName;
            }
        }
        memset(Module_Array,0,sizeof(Module_Array));
    }

    void GetModuleInfoByIndex(int iIndex, ModuleInfo &stModuleInfo)
    {
        if(iIndex < 0 || iIndex >= MODULE_MAX_COUNT)
        {
            return;
        }

        stModuleInfo = Module_Array[iIndex];
    }

}
