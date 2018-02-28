///////////////////////////
//		Create by art
//////////////////////////

#include "stdafx.h"
#include "dllDSTO.h"
#include "easytrace.h"

CDllDSTO::CDllDSTO(void)
{
    m_pDllDSTOInterface = NULL;
    Load();
}

CDllDSTO::~CDllDSTO(void)
{
   //SAFE_DELETE(m_pDllDSTOInterface);该对象在卸载内部释放
   DLLDSTO::UnLoadLibrary();
   m_pDllDSTOInterface = NULL;
}

bool CDllDSTO::Load()
{
    if(m_pDllDSTOInterface == NULL)
    {
        m_pDllDSTOInterface = DLLDSTO::Get_DSTO_Interface();
        if(m_pDllDSTOInterface == NULL)
        {
            LOGERROR("load DSTO failed.");
            return false;
        }
    }

    return true;
}

short CDllDSTO::AddPersistentData(char *pszDataName, char *pszDataType, char *pszInitValue)
{
    short  nRet = -1;
    if(m_pDllDSTOInterface == NULL)
    {
        return nRet;
    }

    nRet = m_pDllDSTOInterface->AddPersistentData(pszDataName,pszDataType,pszInitValue);
    return nRet;
}

short CDllDSTO::SetPersistentData(char *pszDataName, char *pszDataType, char *pszNewValue)
{
    //Load();
	short  nRet = -1;
    if(m_pDllDSTOInterface == NULL)
    {
        return nRet;
    }
    nRet = m_pDllDSTOInterface->SetPersistentData(pszDataName,pszDataType,pszNewValue);
    return nRet;
}

short CDllDSTO::GetPersistentData(char *pszDataName, char *pszDataType, char *pszValue )
{
    //Load();
	short  nRet = -1;
    if(m_pDllDSTOInterface == NULL)
    {
        return nRet;
    }
    nRet = m_pDllDSTOInterface->GetPersistentData(pszDataName,pszDataType,pszValue);
	return nRet;
}

short CDllDSTO::SetData(char *pszDataName, char *pszDataType, char *pszInitValue)
{
   // Load();
	short  nRet = -1;
    if(m_pDllDSTOInterface == NULL)
    {
        return nRet;
    }
    nRet = m_pDllDSTOInterface->SetData(pszDataName,pszDataType,pszInitValue);
	return nRet;
}

short CDllDSTO::GetData(char *pszDataName, char *pszDataType, char *pszValue )
{
    //Load();
	short  nRet = -1;
    nRet = m_pDllDSTOInterface->GetData(pszDataName,pszDataType,pszValue);
	return nRet;
}

short CDllDSTO::InitDatas()
{
   // Load();
	short  nRet = -1;
    nRet = m_pDllDSTOInterface->InitDatas();
	return nRet;
}

short CDllDSTO::ClearData()
{
    //Load();
	short  nRet = -1;
    if(m_pDllDSTOInterface == NULL)
    {
        return nRet;
    }
    nRet = m_pDllDSTOInterface->ClearData();
	return nRet;
}


//============以下供DXXX及其它模块调用接口(除MDAT外)===============================
short CDllDSTO::D_SetDataString(BOOL bPersistent, char *pszDataName, char *pszNewValue)
{
    //Load();
	short  nRet = -1;
    nRet = m_pDllDSTOInterface->D_SetDataString(bPersistent,pszDataName,pszNewValue);
	return nRet;
}

short CDllDSTO::D_GetDataString(BOOL bPersistent, char *pszDataName, char *pszValue)
{
    //Load();
	short  nRet = -1;
    nRet = m_pDllDSTOInterface->D_GetDataString(bPersistent,pszDataName,pszValue);
	return nRet;
}

short CDllDSTO::D_SetDataLong(BOOL bPersistent, char *pszDataName, long nNewValue)
{
    //Load();
	short  nRet = -1;
    nRet = m_pDllDSTOInterface->D_SetDataLong(bPersistent,pszDataName,nNewValue);
	return nRet;
}

short CDllDSTO::D_GetDataLong(BOOL bPersistent, char *pszDataName, long *plongValue)
{
    //Load();
	short  nRet = -1;
    nRet = m_pDllDSTOInterface->D_GetDataLong(bPersistent,pszDataName,plongValue);
	return nRet;
}

short CDllDSTO::D_SetDataBytes(BOOL bPersistent, char *pszDataName, LPBYTE pbyNewValue, WORD wSize)
{
    //Load();
	short  nRet = -1;
    nRet = m_pDllDSTOInterface->D_SetDataBytes(bPersistent,pszDataName,pbyNewValue,wSize);
	return nRet;
}

short CDllDSTO::D_GetDataBytes(BOOL bPersistent, char *pszDataName, LPBYTE pbyValue)
{
    //Load();
	short  nRet = -1;
    nRet = m_pDllDSTOInterface->D_GetDataBytes(bPersistent,pszDataName,pbyValue);
	return nRet;
}

short CDllDSTO::GetBankCodeJsonData(void)
{
    //Load();
	short  nRet = -1;
    nRet = m_pDllDSTOInterface->GetBankCodeJsonData();
	return nRet;
}

short CDllDSTO::GetProcCodeJsonData(void)
{
    //Load();
	short  nRet = -1;
    nRet = m_pDllDSTOInterface->GetProcCodeJsonData();
	return nRet;
}

short CDllDSTO::GetQRCodePic(void)
{
    //Load();
	short  nRet = -1;
    nRet = m_pDllDSTOInterface->GetQRCodePic();
	return nRet;
}

short CDllDSTO::GetMD5Data(void)
{
    //Load();
	short  nRet = -1;
    nRet = m_pDllDSTOInterface->GetMD5Data();
	return nRet;
}
