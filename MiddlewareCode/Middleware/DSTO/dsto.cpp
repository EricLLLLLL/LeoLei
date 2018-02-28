#include "dsto.h"
#include "common_def.h"
#include "artFunc.h"
#include "DBManager/DBManager.h"
#include "Helper/helper.h"

CDSTOApp* CDSTOApp::m_pInstance = NULL;

CDSTOApp::CDSTOApp()
{
    m_strExePath = QCoreApplication::applicationDirPath();
}

CDSTOApp::~CDSTOApp()
{
    CDBManager::DestroyInstance();
}

CDSTOApp* CDSTOApp:: GetInstance()
{
    if(m_pInstance == NULL)
    {
        m_pInstance = new CDSTOApp();
    }

    return m_pInstance;
}

void CDSTOApp::DestroyInstance()
{
    SAFE_DELETE(m_pInstance);
}

bool CDSTOApp::Init()
{
    QString strLogDBFilePath = m_strExePath + DB_DATABASE_NAME; //= m_strExePath + DB_FILE_DIR + LOG_DB_FILE_NAME;s
    CDBManager * pMgr = CDBManager::GetInstance();
    if(pMgr->IsDBConnected())
    {
        return true;
    }

    bool bCreateSuc = pMgr->CreateDBConnection(SQLITE_DB_CONNECTION, strLogDBFilePath);
    if (!bCreateSuc)
    {
        return false;
    }

    return true;
}

short CDSTOApp::AddPersistentData(char *pszDataName, char *pszDataType, char *pszInitValue)
{
    short nRet;
    nRet = objPersistentData.AddData(pszDataName, pszDataType, pszInitValue, "0", "0");
    return nRet;
}

short CDSTOApp::SetPersistentData(char *pszDataName, char *pszDataType, char *pszNewValue)
{
    short nRet;
    nRet = objPersistentData.SetData(pszDataName, pszDataType, pszNewValue);
    return nRet;
}

short CDSTOApp::GetPersistentData(char *pszDataName, char *pszDataType, char *pszValue)
{
    return objPersistentData.GetData(pszDataName, pszDataType, pszValue);
}

short CDSTOApp::SetData(char *pszDataName, char *pszDataType, char *pszNewValue)
{
    short nRet;
    nRet = objScratchPad.SetData(pszDataName, pszDataType, pszNewValue);
    return nRet;
}

short CDSTOApp::GetData(char *pszDataName, char *pszDataType, char *pszValue)
{
    short nRet;
    nRet = objScratchPad.GetData(pszDataName, pszDataType, pszValue);
    return nRet;
}

short CDSTOApp::InitDatas()
{
    short nRet=0;
    nRet=objScratchPad.InitDatas();

    return nRet;
}

short CDSTOApp::ClearData()
{
    short nRet=0;
    nRet=objScratchPad.ClearData();

    return nRet;
}

short CDSTOApp::D_SetDataString(BOOL bPersistent, char *pszDataName, char *pszNewValue)
{
    short nRet=0;
    if(bPersistent)
        nRet=objPersistentData.SetData(pszDataName, "STRING", pszNewValue);
    else
        nRet=objScratchPad.SetData(pszDataName, "STRING", pszNewValue);

    return nRet;
}

short CDSTOApp::D_GetDataString(BOOL bPersistent, char *pszDataName, char *pszValue)
{
    short nRet=0;
    if(bPersistent)
        nRet=objPersistentData.GetData(pszDataName, "STRING", pszValue);
    else
        nRet=objScratchPad.GetData(pszDataName, "STRING", pszValue);

    return nRet;
}

short CDSTOApp::D_SetDataLong(BOOL bPersistent, char *pszDataName, long nNewValue)
{
    QString sValue;
    long arrayLong[1];
    arrayLong[0]=nNewValue;
    LongArray2String(sValue, arrayLong, 1);

    short nRet=0;
    if(bPersistent)
        nRet=objPersistentData.SetData(pszDataName, "LONG", Q2S(sValue));
    else
        nRet=objScratchPad.SetData(pszDataName, "LONG", Q2S(sValue));

    return nRet;
}

short CDSTOApp::D_GetDataLong(BOOL bPersistent, char *pszDataName, long *plongValue)
{
    char szValue[16];
    memset(szValue, 0x00, sizeof(szValue));

    short nRet=0;
    if(bPersistent)
        nRet=objPersistentData.GetData(pszDataName, "LONG", szValue);
    else
        nRet=objScratchPad.GetData(pszDataName, "LONG", szValue);

    if(nRet==0)
        *plongValue=atoi(szValue);

    return nRet;
}

short CDSTOApp::D_SetDataBytes(BOOL bPersistent, char *pszDataName, LPBYTE pbyNewValue, WORD wSize)
{
    BYTE *pbyValue=new BYTE[wSize*2];

    ByteArray2String(pbyValue, pbyNewValue, wSize);

    short nRet=0;
    if(bPersistent)
        nRet=objPersistentData.SetData(pszDataName, "BYTEARRAY", (char*)pbyValue);
    else
        nRet=objScratchPad.SetData(pszDataName, "BYTEARRAY", (char*)pbyValue);

    return nRet;
}

short CDSTOApp::D_GetDataBytes(BOOL bPersistent, char *pszDataName, LPBYTE pbyValue)
{
    char szValue[16];
    memset(szValue, 0x00, sizeof(szValue));

    short nRet=0;
    if(bPersistent)
        nRet=objPersistentData.GetData(pszDataName, "BYTEARRAY", szValue);
    else
        nRet=objScratchPad.GetData(pszDataName, "BYTEARRAY", szValue);

    if(nRet==0)
        String2ByteArray(pbyValue, (LPBYTE)szValue, strlen(szValue));

    return nRet;
}

short CDSTOApp::GetBankCodeJsonData()
{
    short nRet=0;
    nRet=objPersistentData.GetBankCodeJsonData();

    return nRet;
}

short CDSTOApp::GetProcCodeJsonData()
{
    short nRet=0;
    nRet=objPersistentData.GetQRCodePic();
    return nRet;
}

short CDSTOApp::GetQRCodePic()
{
    short nRet=0;
    nRet=objPersistentData.GetQRCodePic();

    return nRet;
}

short CDSTOApp::GetMD5Data()
{
    short nRet=0;
    nRet=objPersistentData.GetMD5Data();

    return nRet;
}

DllDSTOInterface *CreateInstance()
{
    return CDSTOApp::GetInstance();
}

void DestroyInstance()
{
    CDSTOApp::DestroyInstance();
}
