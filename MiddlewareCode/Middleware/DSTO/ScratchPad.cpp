///////////////////////////
//		Create by art
//////////////////////////

#include "ScratchPad.h"
#include "easytrace.h"
#include "DBManager/DBManager.h"
#include "common_def.h"
#include "QSqlQuery"
#include "QVariant"

SCRATCHMAP CScratchPad::m_ScratchPad;
SCRATCHMAP CScratchPad::m_ScratchPadInit;
SCRATCHMAP CScratchPad::m_ScratchPadClear;
bool CScratchPad::m_bLoaded=false;
bool CScratchPad::m_bCleared=false;

CScratchPad::CScratchPad(void)
{

}

CScratchPad::~CScratchPad(void)
{

}

short  CScratchPad::SetData(char *pszDataName, char *pszDataType, char *pszNewValue)
{
    LOGINFO("CScratchPad::SetData [DataName]=%s [DataType]=%s [DataValue]=%s", pszDataName, pszDataType, pszNewValue);
    if(strlen(pszDataName)==0 || strlen(pszDataType)==0)
    {
        LOGERROR("DataName or DataType is blank");
        return -1;
    }

    string sKey(pszDataName);
    sKey+="~^";
    sKey+=pszDataType;

    CScratchPad::m_ScratchPad[sKey]=pszNewValue;
    //int nSize=CScratchPad::m_ScratchPad.size();

    return 0;
}

short  CScratchPad::GetData(char *pszDataName, char *pszDataType, char *pszValue)
{
    LOGINFO("CScratchPad::GetData [DataName]=%s [DataType]=%s", pszDataName, pszDataType);
    if(strlen(pszDataName)==0 || strlen(pszDataType)==0)
    {
        LOGINFO("DataName or DataType is blank");
        return -1;
    }

    short nRet=0;

    string sKey(pszDataName);
    sKey+="~^";
    sKey+=pszDataType;

    SCRATCHMAP::iterator iter;
    iter = CScratchPad::m_ScratchPad.find(sKey);
    if(iter!=CScratchPad::m_ScratchPad.end())
    {
        strcpy(pszValue, (iter->second).c_str());
    }
    else
    {
        LOGINFO("Result: NONE");
        sprintf(pszValue, "");
        nRet=-2;
    }

    LOGINFO("Result: [DataValue]=%s", pszValue);

    return nRet;
}

short  CScratchPad::InitDatas()
{
    if(!m_bLoaded)
        LoadInitValues();

    SCRATCHMAP::iterator it;

    for(it=CScratchPad::m_ScratchPadInit.begin(); it!=CScratchPad::m_ScratchPadInit.end(); ++it)
    {
        CScratchPad::m_ScratchPad[it->first]=it->second;
    }

    LOGINFO("CScratchPad::InitDatas");

    return 0;
}

short CScratchPad::ClearData()
{
    if(!m_bCleared)
        LoadClearValues();

    SCRATCHMAP::iterator it;

    for(it=CScratchPad::m_ScratchPadClear.begin(); it!=CScratchPad::m_ScratchPadClear.end(); ++it)
    {
        CScratchPad::m_ScratchPad[it->first]=it->second;
    }

    LOGINFO("CScratchPad::ClearData");

    return 0;
}

short CScratchPad::LoadClearValues()
{
    m_bCleared=true;

    CDBManager* pDBMgr = CDBManager::GetInstance();
    if(!pDBMgr->IsDBConnected())
    {
        LOGINFO("CScratchPad::LoadClearValues: db is disconnected.  err:%s",Q2S(pDBMgr->GetLastError()));
        return -2;
    }

    char szSql[1024];
    sprintf(szSql,  "select DataName, DataType, InitValue, NeedClear from ScratchPad");
    QVariantList stValList;
    QSqlQuery stResultQuery;
    bool bSuc = pDBMgr->DBSqlQuery(S2Q(szSql),stValList,stResultQuery);
    if(!bSuc)
    {
        LOGINFO("CScratchPad::LoadClearValues: DBSqlQuery failed.  err:%s",Q2S(pDBMgr->GetLastError()));
        return -3;
    }

    while (stResultQuery.next())
    {
        string strKey ="";
        QString strDataName = stResultQuery.value(0).toString();
        QString strDataType = stResultQuery.value(1).toString();
        int bNeedClear = stResultQuery.value(3).toInt();
        strKey += Q2S(strDataName);
        strKey += "~^";
        strKey += Q2S(strDataType);
        if(bNeedClear)
        {
            CScratchPad::m_ScratchPadClear[strKey]=Q2S(stResultQuery.value(2).toString());
        }
    }

    LOGINFO("CScratchPad::LoadClearValues");

    return 0;
}

short  CScratchPad::LoadInitValues()
{
    m_bLoaded=true;

    CDBManager* pDBMgr = CDBManager::GetInstance();
    if(!pDBMgr->IsDBConnected())
    {
        LOGINFO("CScratchPad::LoadInitValues: db is disconnected.  err:%s",Q2S(pDBMgr->GetLastError()));
        return -2;
    }

    char szSql[1024];
    sprintf(szSql,  "select DataName, DataType, InitValue from ScratchPad");

    QVariantList stValList;
    QSqlQuery stResultQuery;
    bool bSuc = pDBMgr->DBSqlQuery(S2Q(szSql),stValList,stResultQuery);
    if(!bSuc)
    {
        LOGINFO("CScratchPad::LoadClearValues: DBSqlQuery failed.  err:%s",Q2S(pDBMgr->GetLastError()));
        return -3;
    }

    while (stResultQuery.next())
    {
        string strKey ="";
        QString strDataName = stResultQuery.value(0).toString();
        QString strDataType = stResultQuery.value(1).toString();
        strKey += Q2S(strDataName);
        strKey += "~^";
        strKey += Q2S(strDataType);
        CScratchPad::m_ScratchPadInit[strKey]=Q2S(stResultQuery.value(2).toString());
    }
    stResultQuery.finish();

    LOGINFO("CScratchPad::LoadInitValues");

    return 0;
}
