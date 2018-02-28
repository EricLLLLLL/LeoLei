#ifndef DSTO_H
#define DSTO_H

#include "dsto_global.h"
#include <DSTOInterface.h>
#include "PersistentData.h"
#include "ScratchPad.h"

class DSTOSHARED_EXPORT CDSTOApp : public DllDSTOInterface
{
public:
    CDSTOApp();
    ~CDSTOApp();
// 重写
public:
    static CDSTOApp* GetInstance();
    static void DestroyInstance();

    bool Init();

    //=============仅供MDAT调用接口===========
    short AddPersistentData(char *pszDataName, char *pszDataType, char *pszInitValue);
    short SetPersistentData(char *pszDataName, char *pszDataType, char *pszNewValue);
    short GetPersistentData(char *pszDataName, char *pszDataType, char *pszValue);

    short SetData(char *pszDataName, char *pszDataType, char *pszNewValue);
    short GetData(char *pszDataName, char *pszDataType, char *pszValue);

    short InitDatas();
    short ClearData();///////针对继续交易，清除需要清除的数据

    //=============供DXXX及其它模块调用接口===========
    short D_SetDataString(BOOL bPersistent, char *pszDataName, char *pszNewValue);
    short D_GetDataString(BOOL bPersistent, char *pszDataName, char *pszValue);
    short D_SetDataLong(BOOL bPersistent, char *pszDataName, long nNewValue);
    short D_GetDataLong(BOOL bPersistent, char *pszDataName, long *plongValue);
    short D_SetDataBytes(BOOL bPersistent, char *pszDataName, LPBYTE pbyNewValue, WORD wSize);
    short D_GetDataBytes(BOOL bPersistent, char *pszDataName, LPBYTE pbyValue);
    short GetBankCodeJsonData(void); // 调用DSTO获取json文件
    short GetProcCodeJsonData(void);
    short GetQRCodePic(void);
    short GetMD5Data(void);


private:
    static CDSTOApp* m_pInstance;
    CPeristentData objPersistentData;
    CScratchPad objScratchPad;
    QString m_strExePath;
};

extern "C" {
    DSTOSHARED_EXPORT DllDSTOInterface* CreateInstance(void);
    DSTOSHARED_EXPORT void DestroyInstance();
}

#endif // DSTO_H
