#ifndef MODULEMANAGER_H
#define MODULEMANAGER_H
#include <QLibrary>

namespace ModuleManager
{
    struct ModuleInfo
    {
        char strModuleName[256];
        QLibrary* pLoadLibrary;
        QObject* pModuleObj;
    };

    static int MODULE_MAX_COUNT = 32;

    bool LoadAllModules();
    void GetModuleInfoByIndex(int iIndex,ModuleInfo& stModuleInfo);
    void ReleaseAllModules();

};

#endif // MODULEMANAGER_H
