#include "mdat.h"
#include "easytrace.h"
#include "Helper/helper.h"
#include "common_def.h"
#include "AlarmLib.h"

#define MAX_BUFFER_LEN 10*1024
CMDAT* CMDAT::m_pInstance = NULL;

CMDAT::CMDAT(QObject* pParent):QObject(pParent)
{
    m_IsGetDataSyncOK=FALSE;
    LOGINFO("MDAT模块加载!!");
}

CMDAT::~CMDAT()
{
    LOGINFO("MDAT模块卸载!!");
}

CMDAT *CMDAT::GetInstance()
{
    if(m_pInstance == NULL)
    {
        m_pInstance = new CMDAT();
    }
    return m_pInstance;
}

bool CMDAT::GetIsGetDataSyncOK(void)
{
    return m_IsGetDataSyncOK;
}

//永久数据异步
short CMDAT::AddPersistentData(const QString& DataName,const QString& DataType, const QString& DataValue)
{
    LOGINFO("AddPersistentData: [DataName]=%s [DataName]=%s [NewDataValue]=%s", Q2S(DataName), Q2S(DataType), Q2S(DataValue));

    short nRet=dllDSTO.AddPersistentData(Q2S(DataName.toUpper()), Q2S(DataType.toUpper()),Q2S(DataValue));

    if(nRet==0)
        emit AddPersistentDataComplete(DataName);
    else
        emit AddPersistentDataError(DataName, nRet);

    LOGINFO("AddPersistentData: Ret %d", nRet);

    return nRet;
}

//永久数据异步
short CMDAT::GetPersistentData(const QString& DataName,const QString& DataType)
{
    char *pszValue=new char[MAX_BUFFER_LEN];
    memset(pszValue, 0x00, MAX_BUFFER_LEN);
    short nRet=dllDSTO.GetPersistentData(Q2S(DataName.toUpper()), Q2S(DataType.toUpper()), pszValue);
    QString sValue=pszValue;
    delete [] pszValue;
    if(nRet==0)
        emit GetPersistentDataComplete(DataName, DataType, sValue);
    else
        emit GetPersistentDataError(DataName, nRet);

    LOGINFO("GetPersistentData: [DataName]=%s [DataName]=%s [DataValue]=%s", Q2S(DataName), Q2S(DataType), Q2S(sValue));

    return nRet;
}

//永久数据异步
short CMDAT::SetPersistentData(QString DataName, QString DataType, QString NewDataValue)
{
    LOGINFO("SetPersistentData: [DataName]=%s [DataName]=%s [NewDataValue]=%s", Q2S(DataName), Q2S(DataType), Q2S(NewDataValue));

    short nRet=dllDSTO.SetPersistentData(Q2S(DataName.toUpper()), Q2S(DataType.toUpper()), Q2S(NewDataValue));

    if(nRet==0)
        emit SetPersistentDataComplete(DataName);
    else
        emit SetPersistentDataError(DataName, nRet);

    LOGINFO("SetPersistentData: Ret %d", nRet);

    return nRet;
}

short CMDAT::TimedLock(LONG Timeout)
{
    return 0;
}

//永久数据同步
QString CMDAT::GetPersistentDataSync(QString DataName, QString DataType)
{
    char *pszValue=new char[MAX_BUFFER_LEN];
    memset(pszValue, 0x00, MAX_BUFFER_LEN);
    short nRet=dllDSTO.GetPersistentData(Q2S(DataName.toUpper()), Q2S(DataType.toUpper()), pszValue);
    QString sValue=pszValue;
    delete [] pszValue;
    if(nRet==0)
        m_IsGetDataSyncOK=TRUE;
    else
        m_IsGetDataSyncOK=FALSE;

    LOGINFO("GetPersistentDataSync: [DataName]=%s [DataName]=%s [DataValue]=%s", Q2S(DataName), Q2S(DataType), Q2S(sValue));

    return sValue;
}

//永久数据同步
short CMDAT::SetPersistentDataSync(QString DataName, QString DataType, QString NewDataValue)
{
    LOGINFO("SetPersistentDataSync: [DataName]=%s [DataName]=%s [NewDataValue]=%s", Q2S(DataName), Q2S(DataType), Q2S(NewDataValue));

    short nRet=dllDSTO.SetPersistentData(Q2S(DataName.toUpper()), Q2S(DataType.toUpper()), Q2S(NewDataValue));

    LOGINFO("SetPersistentDataSync: Ret %d", nRet);

    return nRet;
}

//临时数据
QString CMDAT::GetDataSync(QString DataName, QString DataType)
{
    char *pszValue=new char[MAX_BUFFER_LEN];
    memset(pszValue, 0x00, MAX_BUFFER_LEN);
    short nRet=dllDSTO.GetData(Q2S(DataName.toUpper()),Q2S(DataType.toUpper()), pszValue);
    QString sValue=pszValue;
    delete [] pszValue;
    if(nRet==0)
        m_IsGetDataSyncOK=TRUE;
    else
        m_IsGetDataSyncOK=FALSE;

    LOGINFO("GetDataSync: [DataName]=%s [DataName]=%s [DataValue]=%s", Q2S(DataName), Q2S(DataType), Q2S(sValue));

    return sValue;
}

//临时数据
short CMDAT::SetDataSync(QString DataName, QString DataType, QString NewDataValue)
{
    LOGINFO("SetDataSync: [DataName]=%s [DataName]=%s [NewDataValue]=%s", Q2S(DataName), Q2S(DataType), Q2S(NewDataValue));

    if ("CashInTransactionMaxAmount" == DataName)
    {
        //定义有关的 hKEY, 在查询结束时要关闭。
        QString m_text = NewDataValue;
        QString strInstallPath = CONST_INSTALL_DIR + hm_crm_limit_amount;
        g_Helper.WritePrivateProfileString("limitAmount","amount",m_text,strInstallPath);
    }
    short nRet=dllDSTO.SetData(Q2S(DataName.toUpper()), Q2S(DataType.toUpper()), Q2S(NewDataValue));

    LOGINFO("SetDataSync: Ret %d", nRet);

    return nRet;
}

// 读取ini配置文件
QString CMDAT::GetPrivateProfileSync(QString SectionName, QString KeyName, QString Default, QString FileName)
{
    QString strValue;
    strValue = g_Helper.GetPrivateProfileString(SectionName, KeyName, Default, FileName);
    return strValue;
}

// 写ini配置文件
short CMDAT::WritePrivateProfileSync(QString SectionName, QString KeyName, QString Value, QString FileName)
{
    BOOL bRet = TRUE;
    bRet = g_Helper.WritePrivateProfileString(SectionName, KeyName, Value, FileName);
    if (!bRet)
    {
        Alarm("M0020000");
        return -1;
    }

    return 0;
}

// add by zengwh for 深圳农行三期需求生成json文件供应用使用
short CMDAT::GetBankCodeJsonData(void)
{
    BOOL bRet = dllDSTO.GetBankCodeJsonData();

    if (!bRet)
    {
        Alarm("M0020000");
        return -1;
    }

    return 0;
}

short CMDAT::GetProcCodeJsonData(void)
{
    BOOL bRet = dllDSTO.GetProcCodeJsonData();

    if (!bRet)
    {
        Alarm("M0020000");
        return -1;
    }

    return 0;
}

// add by zengwh for 深圳农行三期需求，微信销户需要MD5值以及二维码图片
short CMDAT::GetQRCodePic(void)
{
    BOOL bRet = dllDSTO.GetQRCodePic();

    if (!bRet)
    {
        Alarm("M0020000");
        return -1;
    }

    return 0;
}

short CMDAT::GetMD5Data(void)
{
    BOOL bRet = dllDSTO.GetMD5Data();

    if (!bRet)
    {
        Alarm("M0020000");
        return -1;
    }

    return 0;
}

short CMDAT::InitDatasSync()
{
    short nRet=dllDSTO.InitDatas();
    LOGINFO("InitDatasSync: Ret %d", nRet);

    return nRet;
}




QObject *CreateInstance()
{
    return CMDAT::GetInstance();
}
