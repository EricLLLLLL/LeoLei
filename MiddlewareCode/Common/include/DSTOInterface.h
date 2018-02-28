/***********************************************
  *@CopyRight：和美（深圳）信息技术股份有限公司
  *@date:2018-01-27 11:23:35
  *@author:xiaoyp
  *@brief:
  调用数据库模块的接口文件
***********************************************/
#pragma once
#include "win_type_def.h"
#include "QLibrary"
#include "easytrace.h"
#include "common_def.h"

struct DllDSTOInterface
{
    //仅在初始化数据库时调用
    virtual bool Init() = 0;

    //=============仅供MDAT调用接口===========
    virtual short AddPersistentData(char *pszDataName, char *pszDataType, char *pszInitValue) = 0;
    virtual short SetPersistentData(char *pszDataName, char *pszDataType, char *pszNewValue) = 0;
    virtual short GetPersistentData(char *pszDataName, char *pszDataType, char *pszValue) = 0;

    virtual short SetData(char *pszDataName, char *pszDataType, char *pszNewValue) = 0;
    virtual short GetData(char *pszDataName, char *pszDataType, char *pszValue) = 0;

    virtual short InitDatas() = 0;
    virtual short ClearData() = 0;///////针对继续交易，清除需要清除的数据

    //=============供DXXX及其它模块调用接口===========
    virtual short D_SetDataString(BOOL bPersistent, char *pszDataName, char *pszNewValue) = 0;
    virtual short D_GetDataString(BOOL bPersistent, char *pszDataName, char *pszValue) = 0;
    virtual short D_SetDataLong(BOOL bPersistent, char *pszDataName, long nNewValue) = 0;
    virtual short D_GetDataLong(BOOL bPersistent, char *pszDataName, long *plongValue) = 0;
    virtual short D_SetDataBytes(BOOL bPersistent, char *pszDataName, LPBYTE pbyNewValue, WORD wSize) = 0;
    virtual short D_GetDataBytes(BOOL bPersistent, char *pszDataName, LPBYTE pbyValue) = 0;
    virtual short GetBankCodeJsonData(void) = 0; // 调用DSTO获取json文件
    virtual short GetProcCodeJsonData(void) = 0;
    virtual short GetQRCodePic(void) = 0;
    virtual short GetMD5Data(void) = 0;
};

namespace DLLDSTO {
    typedef DllDSTOInterface* (*GetInterface)();
    typedef void (*DestroyInterface)();

    static QLibrary* pDSTOLib = NULL;

    static bool LoadLibrary()
    {
        if (pDSTOLib == NULL)
        {
            QString strExePath = QCoreApplication::applicationDirPath();
            QString strLibPath = strExePath + OUT_LIB_DIR + "libDSTO.so";
            pDSTOLib = new QLibrary(strLibPath);
            if(pDSTOLib == NULL)
            {
                LOGERROR("create QLibrary failed. strLibPath=%s",Q2S(strLibPath));
                return false;
            }

            if(!pDSTOLib->load())
            {
                LOGERROR("load module failed.  error:%s  strLibPath=%s",Q2S(pDSTOLib->errorString()),Q2S(strLibPath));
                return false;
            }
            return true;
        }
        return true;
    }

    static void UnLoadLibrary()
    {
        if (pDSTOLib)
        {
            DestroyInterface pFunc = pDSTOLib->resolve("DestroyInstance");
            if(pFunc)
            {
                pFunc();
            }
            pDSTOLib->unload();
            SAFE_DELETE(pDSTOLib);
        }
    }

    static DllDSTOInterface* Get_DSTO_Interface()
    {
        bool bLoad = LoadLibrary();
        if(!bLoad)
        {
            return NULL;
        }

        GetInterface pFunc = 0;
        pFunc = (GetInterface)pDSTOLib->resolve("CreateInstance");
        if (pFunc)
        {
            DllDSTOInterface* pInterface = pFunc();
            bool bInit = pInterface->Init();
            if(!bInit)
            {
                LOGERROR("%s CDllDSTO Init failed.",__FUNCTION__);
                return NULL;
            }
            return pInterface;
        }
        return NULL;
    }
}
