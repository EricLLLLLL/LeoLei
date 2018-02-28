#include "mpin.h"


// Last updated at 2015/11/27 10:09:30

#include "stdafx.h"
#include "mpin.h"
#include "KeyMng.h"
#include "AlarmLib.h"

#define CRYPT_ALGORITHM_ECB 1
#define CRYPT_ALGORITHM_CBC 2
#define CRYPT_ALGORITHM_MAC 3

#define HM_CRYPT_ENCRYPT 1
#define HM_CRYPT_DECRYPT 2

#define HM_PARAM_ERROR -10000


LFSPINIMPORT gImportKey;
LFSXDATA gxValue;
char gKeyName[256]={0};
char gEncKeyName[256]={0};
char gKeyValueBcd[256]={0};

WORD gCryptMode=0;
WORD gCryptAlgorithm=0;
char gCryptValueBcd[2048]={0};
LFSXDATA gxCryptData;

char gCustomerData[2048]={0};

CMPIN* CMPIN::m_pInstance = NULL;

CMPIN *CMPIN::GetInstance()
{
    if(m_pInstance == NULL)
    {
        m_pInstance = new CMPIN("MPIN");
    }

    return m_pInstance;
}


CMPIN::CMPIN(const QString& strDBusObjName) : CWebInteractObj(strDBusObjName)
{
    QString str[32] =
    {
        "0","1","2","3","4","5","6","7","8","9",
        "ENTER","CANCEL","CLEAR","BACKSPACE","HELP",
        ".","00","000","RES1","RES2","RES3",
        "RES4","RES5","RES6","RES7","RES8",
        "OEM1","OEM2","OEM3","OEM4","OEM5","OEM6"
    };
    for(int i=0;i<32;i++)
    {
        m_KeyMap.insert(std::make_pair(1<<i,str[i]));
        m_KeyMap1.insert(std::make_pair(str[i],1<<i));
    }
    m_KeyMap1.insert(std::make_pair("NUMBERS",0x03FF));//"0,1,2,3,4,5,6,7,8,9"

    QString strFormat[14]=
    {
        "3624","ANSI","ISO0","ISO1","ECI2",
        "ECI3","VISA","DIEBOLD","DIEBOLDCO","VISA3",
        "BANKSYS","EMV","ISO3","AP"
    };

    for(int i=0;i<14;i++)
    {
        m_PinBlockFormatMap.insert(std::make_pair(1<<i,strFormat[i]));
        m_PinBlockFormatMap1.insert(std::make_pair(strFormat[i],1<<i));
    }

    m_PinLength=0;

    m_PreDevStatus = m_CurrDevStatus ="OFFLINE";

    m_bOpened = false;

    m_bCancelGetPin = FALSE;
    m_bCancelGetData = FALSE;

    __GetConfigPath();

    LOGINFO("MPIN 模块加载！！！");
}

CMPIN::~CMPIN()
{
    LOGINFO("MPIN 模块卸载！！！");
    if (m_pInstance)
    {
        delete m_pInstance;
    }
}



void  CMPIN::WR_OpenComplete(DWORD dwCommand, HRESULT hResult, LPVOID buffer)
{
    //异步打开成功
    if(LFS_SUCCESS == hResult)
    {
        DWORD dwEvent = SERVICE_EVENTS | USER_EVENTS | SYSTEM_EVENTS | EXECUTE_EVENTS;
        int ret = m_apiCtrl.LFSAsyncRegister(dwEvent,&m_CurrRequestID);

        if(LFS_SUCCESS != ret)
        {
            N_Close();
            emit OpenFailed();
            LOGERROR("%s,注册事件失败,错误码： ret = %d",__FUNCTION__,ret);
            Alarm("02000000");
        }
    }
    else
    {
        emit OpenFailed();
        LOGERROR("%s,打开失败，错误码： hResult = %d",__FUNCTION__,hResult);
        Alarm("02000000");
    }
}

void  CMPIN::WR_RegisterComplete(DWORD dwCommand, HRESULT hResult, LPVOID buffer)
{
    if(LFS_SUCCESS != hResult)
    {
        N_Close();
        LOGERROR("%s,注册事件失败,错误码：hResult = %d",__FUNCTION__,hResult);
        Alarm("02000000");
        emit OpenFailed();
    }
    else
    {
        N_GetCaps();
        m_bOpened = true;
        emit ConnectionOpened();
        LOGINFO("%s,事件：ConnectionOpened()",__FUNCTION__);
    }
}


// 收到关闭完成消息后调用该函数
void CMPIN::WR_CloseComplete(DWORD dwCommand, HRESULT hResult, LPVOID buffer)
{
    if(LFS_SUCCESS == hResult)
    {
        m_bOpened = false;
        emit ConnectionClosed();
        LOGINFO("%s,事件：ConnectionClosed()",__FUNCTION__);
    }
    else
    {
        emit CloseFailed();
        LOGERROR("%s,关闭设备失败,错误码：hResult = %d",__FUNCTION__,hResult);
        Alarm("02000000");
    }
}

// 收到打开完成消息后调用该函数
void CMPIN::WR_GetinfoComplete(DWORD dwCommand, HRESULT hResult, LPVOID buffer)
{
}

// 收到执行完成消息后调用该函数
void CMPIN::WR_ExecuteComplete(DWORD dwCommand, HRESULT hResult, LPVOID buffer)
{
    LOGINFO("%s,IN",__FUNCTION__);

    if (LFS_ERR_TIMEOUT == hResult )
    {
        emit Timeout();//发送超时事件
        LOGERROR("%s，命令超时，命令ID = %d,错误码：hResult = %d",__FUNCTION__,dwCommand,hResult);
        Alarm("02000001");
        return;
    }

    switch(dwCommand)
    {
    case LFS_CMD_PIN_CRYPT:
        {
            if ( LFS_SUCCESS == hResult )
            {
                LPLFSXDATA lpData=(LPLFSXDATA)((LPLFSRESULT)buffer)->buffer;
                if(lpData == NULL)
                {
                    emit CryptFailed();
                    LOGERROR("%s,加解密数据失败，SP数据为空。",__FUNCTION__);
                    Alarm("02000002");
                    return;
                }
                int len = lpData->length*2;
                char dataAsc[1024]={0};

                BCDToASCII((LPBYTE)dataAsc,lpData->data,len);

                if(m_wMode==HM_CRYPT_ENCRYPT && m_wCryptAlgorithm==CRYPT_ALGORITHM_MAC)
                {
                    emit this->MACGenerated(dataAsc);
                    LOGINFO("%s,事件：MACGenerated(\"%s\")",__FUNCTION__,dataAsc);
                }
                else if(m_wMode==HM_CRYPT_ENCRYPT)
                {
                    emit this->EncryptComplete(dataAsc);
                    LOGINFO("%s,事件：EncryptComplete(\"%s\")",__FUNCTION__,dataAsc);
                }
                else
                {
                    emit this->DecryptComplete(dataAsc);
                    LOGINFO("%s,事件：DecryptComplete(\"%s\")",__FUNCTION__,dataAsc);
                }
            }
            else
            {
                emit CryptFailed();
                LOGERROR("%s,LFS_CMD_PIN_CRYPT 加解密数据失败，错误码：hResult = %d",__FUNCTION__,hResult);
                Alarm("02000001");
                m_bPressCancel=false;
                return;
            }
        }
        break;

    case LFS_CMD_PIN_IMPORT_KEY:
        {
            if ( LFS_SUCCESS == hResult )
            {
                LPLFSXDATA lpxKVC = (LPLFSXDATA)((LPLFSRESULT)buffer)->buffer;

                if(lpxKVC)
                {
                    if(lpxKVC->data)
                    {
                        char chKvcAsc[1024] = {0};
                        BCDToASCII((LPBYTE)chKvcAsc,(LPBYTE)lpxKVC->data,2*lpxKVC->length);

                        m_DSTO.D_SetDataString(false,"KVC",chKvcAsc);//写到临时数据库中

                        emit this->KeyLoaded();
                        LOGINFO("%s,事件：KeyLoaded(),KVC = %s",__FUNCTION__,chKvcAsc);
                    }
                    else
                    {
                        emit KeyLoadFailed();		// 改为 KeyLoadFailed
                        LOGERROR("%s,SP数据为空。",__FUNCTION__);
                        Alarm("02000002");
                    }
                }
                else
                {
                    emit KeyLoadFailed();		// 改为 KeyLoadFailed
                    LOGERROR("%s,SP数据为空。",__FUNCTION__);
                    Alarm("02000002");
                }
            }
            else
            {
                emit KeyLoadFailed();
                LOGERROR("%s,LFS_CMD_PIN_IMPORT_KEY 失败，错误码：hResult = %d",__FUNCTION__,hResult);
                Alarm("02000001");
                m_bPressCancel=false;
                return;
            }
        }
        break;

    case LFS_CMD_PIN_GET_PIN:
        {
            // 添加判断成功和失败
            if ( LFS_SUCCESS == hResult )
            {
                if(!m_bPressCancel)
                {
                    LPLFSPINENTRY lpEntry = (LPLFSPINENTRY)(((LPLFSRESULT)buffer)->buffer);

                    if(lpEntry == NULL)
                    {
                        emit GetPinFailed();   // 改为 GetPinFailed
                        LOGERROR("%s,LFS_CMD_PIN_GET_PIN命令返回的SP数据为空。",__FUNCTION__);
                        Alarm("02000002");
                        return;
                    }

                    m_PinLength = lpEntry->digits;//pin的长度

                    emit GetPinCompleted();
                    LOGINFO("%s,事件：GetPinCompleted()",__FUNCTION__);
                }

                m_bPressCancel=false;
            }
            else if( LFS_ERR_CANCELED == hResult)
            {
                emit GetPinCancelled();
                LOGINFO("%s,取消事件：GetPinCancelled()",__FUNCTION__);
                m_bPressCancel=false;
                m_bCancelGetPin = FALSE;
            }
            else
            {
                if (m_bCancelGetPin)
                {
                    emit GetPinCancelFailed();
                    LOGERROR("%s,LFS_CMD_PIN_GET_PIN命令取消失败，错误码：hResult = %d",__FUNCTION__,hResult);
                    Alarm("02000001");
                }
                else
                {
                    emit GetPinFailed();
                    LOGERROR("%s,LFS_CMD_PIN_GET_PIN命令执行失败，错误码：hResult = %d",__FUNCTION__,hResult);
                    Alarm("02000001");
                }

                m_bPressCancel=false;
                m_bCancelGetPin = FALSE;
            }
        }
        break;

    case LFS_CMD_PIN_GET_PINBLOCK:
        {
            // 添加判断成功和失败  失败事件PinBlockFailed
            if ( LFS_SUCCESS == hResult )
            {
                char dataAsc[1024]={0};

                LPLFSXDATA lpxPinBlock = (LPLFSXDATA)((LPLFSRESULT)buffer)->buffer;

                if(lpxPinBlock == NULL || lpxPinBlock->data == NULL)
                {
                    emit PinBlockFailed();
                    LOGERROR("%s,SP数据为空。",__FUNCTION__);
                    Alarm("02000002");
                    return;
                }

                BCDToASCII((LPBYTE)dataAsc,lpxPinBlock->data,2*lpxPinBlock->length);

                m_DSTO.D_SetDataString(false,"PINBLOCK",dataAsc);//写到临时数据库

                emit this->PinBlockCompleted(dataAsc);

                LOGINFO("%s,事件：PinFormatted(%s)",__FUNCTION__,dataAsc);
            }
            else
            {
                emit PinBlockFailed();
                LOGERROR("%s,LFS_CMD_PIN_GET_PINBLOCK 失败，错误码：hResult = %d",__FUNCTION__,hResult);
                Alarm("02000001");
                m_bPressCancel=false;
            }

        }
        break;

    case LFS_CMD_PIN_GET_DATA:
        {
            // 添加判断成功和失败  失败事件GetDataFailed
            if ( LFS_SUCCESS == hResult )
            {
                if(!m_bPressCancel)//不是Cancel键
                {
                    int len=0;
                    LPLFSPINDATA lpPinData=(LPLFSPINDATA)((LPLFSRESULT)buffer)->buffer;

                    QString str("");

                    if(lpPinData)
                    {
                        LPLFSPINKEY * lppPinKeys = lpPinData->pin_keys;

                        for(int i=0;i<lpPinData->keys;i++)
                        {
                            if(lppPinKeys[i] == NULL)
                            {
                                emit GetDataFailed();
                                LOGERROR("%s,SP数据为空。",__FUNCTION__);
                                Alarm("02000002");
                                return;
                            }

                            std::map<unsigned long,QString>::iterator it = m_KeyMap.find(lppPinKeys[i]->digit);
                            if(it != m_KeyMap.end())
                            {
                                str += it->second;
                            }
                        }
                    }

                    emit this->GetDataComplete(str);
                    LOGINFO("%s,事件：GetDataComplete(\"%s\")",__FUNCTION__,Q2S(str));
                }

                m_bPressCancel=false;
            }
            else if( LFS_ERR_CANCELED == hResult)
            {
                emit GetDataCancelled();
                LOGINFO("%s,取消事件：GetDataCancelled()",__FUNCTION__);
                m_bPressCancel=false;
                m_bCancelGetData = FALSE;
                return;
            }
            else
            {
                if (m_bCancelGetData)
                {
                    emit GetDataCancelFailed();
                    LOGERROR("%s,LFS_CMD_PIN_GET_DATA命令取消失败，错误码：hResult = %d",__FUNCTION__,hResult);
                    Alarm("02000001");
                }
                else
                {
                    emit GetDataFailed();
                    LOGERROR("%s,LFS_CMD_PIN_GET_DATA命令执行失败，错误码：hResult = %d",__FUNCTION__,hResult);
                    Alarm("02000001");
                }

                m_bPressCancel=false;
                m_bCancelGetData = FALSE;
            }
        }
        break;

    case LFS_CMD_PIN_INITIALIZATION:
        {
            // 添加判断成功和失败  失败事件InitializeFailed
            if ( LFS_SUCCESS == hResult )
            {
                emit this->InitializeComplete();
                LOGINFO("%s,事件：InitializeComplete()",__FUNCTION__);
            }
            else
            {
                emit InitializeFailed();
                LOGERROR("%s,LFS_CMD_PIN_INITIALIZATION 失败，错误码：hResult = %d",__FUNCTION__,hResult);
                Alarm("02000001");
                m_bPressCancel=false;
                return;
            }
        }
        break;

    case LFS_CMD_PIN_RESET:
        {
            // 添加判断成功和失败  失败事件ResetFailed
            if ( LFS_SUCCESS == hResult )
            {
                emit this->ResetComplete();
                LOGINFO("%s,事件：ResetComplete()",__FUNCTION__);
            }
            else
            {
                emit ResetFailed();
                LOGERROR("%s,LFS_CMD_PIN_RESET 失败，错误码：hResult = %d",__FUNCTION__,hResult);
                Alarm("02000001");
                m_bPressCancel=false;
                return;
            }
        }
        break;

    default:
        break;
    }
}

// 收到执行事件后调用该函数
void CMPIN::WR_ExecuteEvent(DWORD dwEventID, HRESULT hResult, LPVOID buffer)
{
    switch(dwEventID)
    {
    case LFS_EXEE_PIN_KEY:
        {
            LPLFSPINKEY lpKey=(LPLFSPINKEY)((LPLFSRESULT)buffer)->buffer;
            QString strKey("*");
            std::map<unsigned long,QString>::iterator itm;

            itm=m_KeyMap.find(lpKey->digit);
            if(itm!=m_KeyMap.end())
                strKey=itm->second;

            if(lpKey->digit==LFS_PIN_FK_CANCEL)
            {
                m_bPressCancel=true;
                if(1==m_wPinOrData)
                {
                    emit GetPinCancelled();
                    LOGINFO("%s,取消事件：GetPinCancelled()。",__FUNCTION__);
                }
                else
                {
                    emit this->KeyPressed(strKey,lpKey->digit);
                    LOGINFO("%s,Key：%s。",__FUNCTION__,Q2S(strKey));

                    emit GetDataCancelled();		// GetDataCancelled
                    LOGINFO("%s,事件：GetDataCancelled（）。",__FUNCTION__);
                }
            }
            else
            {
                emit KeyPressed(strKey,lpKey->digit);

                LOGINFO("%s,Key:%s",__FUNCTION__,Q2S(strKey));
            }
        }
        break;
    }
}

// 收到用户事件后调用该函数
void CMPIN::WR_UserEvent(DWORD dwEventID, HRESULT hResult, LPVOID buffer)
{

}

// 收到服务事件后调用该函数
void CMPIN::WR_ServiceEvent(DWORD dwEventID, HRESULT hResult, LPVOID buffer)
{

}

// 收到系统事件后调用该函数
void CMPIN::WR_SystemEvent(DWORD dwEventID, HRESULT hResult, LPVOID buffer)
{
    switch(dwEventID)
    {
    case LFS_SYSE_DEVICE_STATUS://设备状态改变
        {
            LPLFSDEVSTATUS lpDevStatus=(LPLFSDEVSTATUS)((LPLFSRESULT)buffer)->buffer;
            if(lpDevStatus == NULL)
            {
                LOGINFO("%s, LFS_SYSE_DEVICE_STATUS，(LPLFSDEVSTATUS)((LPLFSRESULT)buffer)->buffer数据为空", __FUNCTION__);
                return;
            }
            SetDeviceStatus(lpDevStatus->state);
            if(m_CurrDevStatus!=m_PreDevStatus)
            {
                emit this->StatusChanged("PIN", m_PreDevStatus, m_CurrDevStatus);
                LOGINFO("%s,状态改变事件：StatusChanged(\"StDeviceStatus\",%s,%s)",__FUNCTION__,Q2S(m_PreDevStatus),Q2S(m_CurrDevStatus));

            }
        }
        break;
    }
}


// 以下为属性的处理函数


void CMPIN::SetServiceName(QString newVal)
{
    LOGINFO("%s,属性,newVal = %s",__FUNCTION__,Q2S(newVal));

    strcpy(m_szLogicalName,newVal.toStdString().c_str());
}

QString CMPIN::GetStDetailedDeviceStatus(void)
{
    N_GetStatus();
    LOGINFO("%s,Status = %s",__FUNCTION__,Q2S(m_CurrDevStatus));
    return m_CurrDevStatus;
}


BOOL CMPIN::GetCpCanCBC(void)
{
    return ((m_Caps.algorithms&LFS_PIN_CRYPTDESCBC)==LFS_PIN_CRYPTDESCBC);
}

BOOL CMPIN::GetCpCanECB(void)
{

    return ((m_Caps.algorithms&LFS_PIN_CRYPTDESECB)==LFS_PIN_CRYPTDESECB);
}

BOOL CMPIN::GetCpCanRSA(void)
{

    return ((m_Caps.algorithms&LFS_PIN_CRYPTRSA)==LFS_PIN_CRYPTRSA);
}

BOOL CMPIN::GetCpCanMAC(void)
{

    return ((m_Caps.algorithms&LFS_PIN_CRYPTDESMAC)==LFS_PIN_CRYPTDESMAC);
}

BOOL CMPIN::GetCpCanTripleECB(void)
{

    return ((m_Caps.algorithms&LFS_PIN_CRYPTTRIDESECB)==LFS_PIN_CRYPTTRIDESECB);
}

BOOL CMPIN::GetCpCanTripleCBC(void)
{

    return ((m_Caps.algorithms&LFS_PIN_CRYPTTRIDESCBC)==LFS_PIN_CRYPTTRIDESCBC);
}

BOOL CMPIN::GetCpCanTripleCFB(void)
{

    return ((m_Caps.algorithms&LFS_PIN_CRYPTTRIDESCFB)==LFS_PIN_CRYPTTRIDESCFB);
}

BOOL CMPIN::GetCpCanTripleMAC(void)
{

    return ((m_Caps.algorithms&LFS_PIN_CRYPTTRIDESMAC)==LFS_PIN_CRYPTTRIDESMAC);
}

QString CMPIN::GetCpKeysSupported(void)
{

    return m_SuppKeysName;
}

SHORT CMPIN::GetPINLength(void)
{

    return m_PinLength;
}

QString CMPIN::GetCpPINFormatsSupported(void)
{
    return m_SuppPinFormats;
}


// 以下为方法的处理函数

SHORT CMPIN::CloseConnection(void)
{
    LOGINFO("%s",__FUNCTION__);

    int ret = LFS_SUCCESS;
    if (m_bOpened)
    {
        ret=N_Close();
    }
    return ret;
}

SHORT CMPIN::OpenConnection(void)
{
    LOGINFO("%s",__FUNCTION__);

    if(m_bOpened)
    {
        emit ConnectionOpened();
        LOGINFO("%s,事件：ConnectionOpened（）",__FUNCTION__);
        return 0;
    }

 //   m_apiCtrl.SetSendWnd(GetWindowHwnd());

    return N_Open();
}

void CMPIN::Reset(LONG timeout)
{
    N_Reset(timeout);
}

SHORT CMPIN::LoadMasterKey(QString KeyName, QString KeyValue, LONG Timeout)
{
    LOGINFO("%s(\"%s\",\"%s\")",__FUNCTION__,Q2S(KeyName),Q2S(KeyValue));

    if(KeyName == "" || KeyValue == "")
    {
        LOGERROR("%s,参数错误。",__FUNCTION__);
        Alarm("02000002");
        emit KeyLoadFailed();	// KeyLoadFailed
        return HM_PARAM_ERROR;
    }

    LFSPINIMPORT import=GetImportKeyParam(KeyName,"",KeyValue);

    return N_ImportKey(&import,Timeout);
}

SHORT CMPIN::Initialize(void)
{
    LOGINFO("%s(void)",__FUNCTION__);

    LFSPININIT init;
    init.ident=NULL;
    init.key=NULL;

    return N_Initialization(&init);
}
SHORT CMPIN::LoadWorkKey(QString KeyName, QString KeyValue, QString EncKeyName)
{
    LOGINFO("%s(\"%s\",\"%s\",\"%s\")",__FUNCTION__,Q2S(KeyName),Q2S(KeyValue),Q2S(EncKeyName));

    if(KeyName == "" || KeyValue == "" || EncKeyName == "")
    {
        LOGERROR("%s,参数错误。",__FUNCTION__);
        Alarm("02000002");
        emit KeyLoadFailed();	 //KeyLoadFailed
        return HM_PARAM_ERROR;
    }

    LFSPINIMPORT import=GetImportKeyParam(KeyName,EncKeyName,KeyValue);

    return N_ImportKey(&import);
}
SHORT CMPIN::EncryptECB(QString Value, QString KeyName, SHORT PadChar)
{
    LOGINFO("%s(\"%s\",\"%s\",%d)",__FUNCTION__,Q2S(Value),Q2S(KeyName),PadChar);

    if(Value == "" || KeyName == "")
    {
        LOGERROR("%s,参数错误。",__FUNCTION__);
        Alarm("02000002");
        emit CryptFailed();	// CryptFailed
        return HM_PARAM_ERROR;
    }

    m_wCryptAlgorithm=CRYPT_ALGORITHM_ECB;
    m_wMode=HM_CRYPT_ENCRYPT;

    gCryptMode=LFS_PIN_MODEENCRYPT;
    gCryptAlgorithm=GetCryptAlgorithm(CRYPT_ALGORITHM_ECB,KeyName.toStdString().c_str());
    LFSPINCRYPT crypt = GetCryptParam(Value,KeyName,PadChar);

    return N_Crypt(&crypt);
}
SHORT CMPIN::DecryptECB(QString Value, QString KeyName, SHORT PadChar)
{
    LOGINFO("%s(\"%s\",\"%s\",%d)",__FUNCTION__,Q2S(Value),Q2S(KeyName),PadChar);

    if(Value == "" || KeyName == "")
    {
        LOGERROR("%s,参数错误。",__FUNCTION__);
        Alarm("02000002");
        emit CryptFailed();	// CryptFailed
        return HM_PARAM_ERROR;
    }

    m_wCryptAlgorithm=CRYPT_ALGORITHM_ECB;
    m_wMode=HM_CRYPT_DECRYPT;

    gCryptMode=LFS_PIN_MODEDECRYPT;
    gCryptAlgorithm=GetCryptAlgorithm(CRYPT_ALGORITHM_ECB,KeyName.toStdString().c_str());
    LFSPINCRYPT crypt = GetCryptParam(Value,KeyName,PadChar);

    return N_Crypt(&crypt);
}
SHORT CMPIN::EncryptCBC(QString Value, QString KeyName, QString IVName, SHORT PadChar)
{
    LOGINFO("%s(\"%s\",\"%s\",\"%s\",%d)",__FUNCTION__,Q2S(Value),Q2S(KeyName),Q2S(IVName),PadChar);

    if(Value == "" || KeyName == "")
    {
        LOGERROR("%s,参数错误。",__FUNCTION__);
        Alarm("02000002");
        emit CryptFailed();	// CryptFailed
        return HM_PARAM_ERROR;
    }

    m_wCryptAlgorithm=CRYPT_ALGORITHM_CBC;
    m_wMode=HM_CRYPT_ENCRYPT;

    gCryptMode=LFS_PIN_MODEENCRYPT;
    gCryptAlgorithm=GetCryptAlgorithm(CRYPT_ALGORITHM_CBC,KeyName.toStdString().c_str());
    LFSPINCRYPT crypt = GetCryptParam(Value,KeyName,PadChar);

    return N_Crypt(&crypt);
}
SHORT CMPIN::DecryptCBC(QString Value, QString KeyName, QString IVName, SHORT PadChar)
{
    LOGINFO("%s(\"%s\",\"%s\",\"%s\",%d)",__FUNCTION__,Q2S(Value),Q2S(KeyName),Q2S(IVName),PadChar);

    if(Value == "" || KeyName == "")
    {
        LOGERROR("%s,参数错误。",__FUNCTION__);
        Alarm("02000002");
        emit CryptFailed();		// CryptFailed
        return HM_PARAM_ERROR;
    }

    m_wCryptAlgorithm=CRYPT_ALGORITHM_CBC;
    m_wMode=HM_CRYPT_DECRYPT;

    gCryptMode=LFS_PIN_MODEDECRYPT;
    gCryptAlgorithm=GetCryptAlgorithm(CRYPT_ALGORITHM_CBC,KeyName.toStdString().c_str());
    LFSPINCRYPT crypt = GetCryptParam(Value,KeyName,PadChar);

    return N_Crypt(&crypt);
}
SHORT CMPIN::GenerateMAC(QString Value, QString KeyName, QString IVName, SHORT PadChar)
{
    LOGINFO("%s(\"%s\",\"%s\",\"%s\",%d)",__FUNCTION__,Q2S(Value),Q2S(KeyName),Q2S(IVName),PadChar);

    if(Value == "" || KeyName == "" )
    {
        LOGERROR("%s,参数错误。",__FUNCTION__);
        Alarm("02000002");
        emit CryptFailed();		// CryptFailed
        return HM_PARAM_ERROR;
    }

    m_wCryptAlgorithm=CRYPT_ALGORITHM_MAC;
    m_wMode=HM_CRYPT_ENCRYPT;

    gCryptMode=LFS_PIN_MODEENCRYPT;
    gCryptAlgorithm=GetCryptAlgorithm(CRYPT_ALGORITHM_MAC,KeyName.toStdString().c_str());

    LOGINFO("%s,11111111111,gCryptAlgorithm = %d",__FUNCTION__,gCryptAlgorithm);

    LFSPINCRYPT crypt = GetCryptParam(Value,KeyName,PadChar);

    return N_Crypt(&crypt);
}

SHORT CMPIN::GenerateMACClearIV(QString Value, QString KeyName, QString IV, SHORT PadChar)
{
    LOGINFO("%s(\"%s\",\"%s\",\"%s\",%d)",__FUNCTION__,Q2S(Value),Q2S(KeyName),Q2S(IV),PadChar);

    return GenerateMAC(Value,KeyName,"",PadChar);
}


SHORT CMPIN::GetPin(SHORT MinLength, SHORT MaxLength, BOOL AutoEnd, QString ActiveKeys, QString TerminatorKeys, LONG Timeout)
{
    LOGINFO("%s(%d,%d,%d,\"%s\",\"%s\",%d)",__FUNCTION__,MinLength,MaxLength,AutoEnd,Q2S(ActiveKeys),Q2S(TerminatorKeys),Timeout);

    if(MinLength>MaxLength || ActiveKeys =="" || TerminatorKeys=="")
    {
        LOGERROR("%s,参数错误",__FUNCTION__);
        Alarm("02000002");
        emit GetPinFailed();	// GetPinFailed
        return HM_PARAM_ERROR;
    }

    m_wPinOrData = 1;

    LFSPINGETPIN getPin;
    getPin.auto_end=AutoEnd;
    getPin.echo='*';
    getPin.terminate_fdks=0;
    getPin.active_fdks=0;
    getPin.max_len=MaxLength;
    getPin.min_len=MinLength;
    getPin.active_keys=GetKeysCode(ActiveKeys);
    getPin.terminate_keys=GetKeysCode(TerminatorKeys);

    int ret = N_GetPin(&getPin,Timeout);
    m_PinRequestID = m_CurrRequestID;

    return ret;
}

SHORT CMPIN::PinBlock(QString Format, QString CustomerData, SHORT PadChar, QString KeyName, QString XORData, QString SecondKeyName)
{
    LOGINFO("%s(\"%s\",\"%s\",%d,\"%s\",\"%s\",\"%s\")",__FUNCTION__,Q2S(Format),Q2S(CustomerData),PadChar,Q2S(KeyName),Q2S(XORData),Q2S(SecondKeyName));

    LFSPINBLOCK pinBlock;

    strcpy(gKeyName,KeyName.toStdString().c_str());
  //  KeyName.ReleaseBuffer();
    pinBlock.key=gKeyName;

    strcpy(gCustomerData,CustomerData.toStdString().c_str());
  //  CustomerData.ReleaseBuffer();
    pinBlock.customer_data=gCustomerData;

    pinBlock.padding=PadChar;
    pinBlock.key_enc_key=NULL;
    pinBlock.xor_data=NULL;
    pinBlock.format = GetPinBlockFormat(Format);

    return N_GetPinBlock(&pinBlock);
}

SHORT CMPIN::CancelGetPin(void)
{
    LOGINFO("%s()",__FUNCTION__);
    int ret = N_Cancel(m_PinRequestID);
    if(LFS_SUCCESS != ret)
    {
        emit GetPinCancelFailed();// 添加失败事件 GetPinCancelFailed
    }
    else
    {
        m_bCancelGetPin = TRUE;
    }

    return ret;
}

SHORT CMPIN::GetData(SHORT MaxKeys, BOOL AutoEnd, QString ActiveKeys, QString TerminatorKeys, LONG Timeout)
{
    LOGINFO("%s(%d,%d,\"%s\",\"%s\",%d)",__FUNCTION__,MaxKeys,AutoEnd,Q2S(ActiveKeys),Q2S(TerminatorKeys),Timeout);

    if(ActiveKeys== "" || TerminatorKeys == "")
    {
        LOGERROR("%s,参数错误。",__FUNCTION__);
        Alarm("02000002");
        emit GetDataFailed();
        return HM_PARAM_ERROR;
    }

    m_wPinOrData=2;

    LFSPINGETDATA pinGetData;

    pinGetData.auto_end = AutoEnd;
    pinGetData.active_fdks=0;
    pinGetData.terminate_fdks=0;
    pinGetData.active_keys=GetKeysCode(ActiveKeys);
    pinGetData.terminate_keys=GetKeysCode(TerminatorKeys);
    pinGetData.max_len=MaxKeys;

    int ret = N_GetData(&pinGetData,Timeout);
    m_DataRequestID = m_CurrRequestID;

    return ret;
}

SHORT CMPIN::CancelGetData(void)
{
    LOGINFO("%s()",__FUNCTION__);
    int ret = N_Cancel(m_DataRequestID);

    if(LFS_SUCCESS != ret)// 添加失败事件
    {
        emit GetDataCancelFailed();
    }
    else
    {
        m_bCancelGetData = TRUE;
    }

    return ret;
}

// 修改为GetKeyNamesSync
QString CMPIN::GetKeyNamesSync(void)
{
    LOGINFO("%s(void)",__FUNCTION__);

    m_wKeyDetailType = 1;

    N_KeyDetail(NULL);
    return m_strKeyDetail;
}

// 修改为 GetKeyUseFlagsSync
QString CMPIN::GetKeyUseFlagsSync(QString KeyName)
{
    LOGINFO("%s(\"%s\")",__FUNCTION__,Q2S(KeyName));

    m_wKeyDetailType = 2;

    N_KeyDetail((LPSTR)KeyName.toStdString().c_str());
    //KeyName.ReleaseBuffer();
    return m_strKeyDetail;
}

// 修改为 KeyIsValidSync
BOOL CMPIN::KeyIsValidSync(QString KeyName)
{
    LOGINFO("%s,方法",__FUNCTION__);

    m_wKeyDetailType = 1;

    N_KeyDetail(NULL);

    int index = m_strKeyDetail.indexOf(KeyName);

    return ((-1==index) ? false : true);
}

SHORT CMPIN::ExtendedLoadKey(QString KeyName, QString KeyValue, QString UseFlags, QString Identification)
{
    LOGINFO("%s(\"%s\",\"%s\",\"%s\",\"%s\")",__FUNCTION__,Q2S(KeyName),Q2S(KeyValue),Q2S(UseFlags),Q2S(Identification));

    if(KeyName == "" || KeyValue == "" || UseFlags == "")
    {
        LOGERROR("%s,参数错误。",__FUNCTION__);
        Alarm("02000002");
        emit KeyLoadFailed();	// 改为 KeyLoadFailed
        return HM_PARAM_ERROR;
    }

    WORD use = 0;
    if(UseFlags.indexOf("CRYPT") != -1)
        use |= LFS_PIN_USECRYPT;
    if(UseFlags.indexOf("FUNCTION") != -1)
        use |= LFS_PIN_USEFUNCTION;
    if(UseFlags.indexOf("MACING") != -1)
        use |= LFS_PIN_USEMACING;
    if(UseFlags.indexOf("KEYENCKEY") != -1)
        use |= LFS_PIN_USEKEYENCKEY;

    LFSPINIMPORT import=GetImportKeyParam(KeyName,"",KeyValue,use);

    return N_ImportKey(&import);
}

SHORT CMPIN::ExtendedLoadEncryptedKey(QString KeyName, QString KeyValue, QString EncKeyName, QString UseFlags, QString Identification)
{
    LOGINFO("%s(\"%s\",\"%s\",\"%s\",\"%s\",\"%s\")",__FUNCTION__,Q2S(KeyName),Q2S(KeyValue),Q2S(EncKeyName),Q2S(UseFlags),Q2S(Identification));

    if(KeyName == "" || KeyValue == "" || EncKeyName == "")
    {
        LOGERROR("%s,参数错误。",__FUNCTION__);
        Alarm("02000002");
        emit KeyLoadFailed();	// 改为 KeyLoadFailed
        return HM_PARAM_ERROR;
    }

    WORD use = 0;
    if(UseFlags.indexOf("CRYPT") != -1)
        use |= LFS_PIN_USECRYPT;
    if(UseFlags.indexOf("FUNCTION") != -1)
        use |= LFS_PIN_USEFUNCTION;
    if(UseFlags.indexOf("MACING") != -1)
        use |= LFS_PIN_USEMACING;
    if(UseFlags.indexOf("KEYENCKEY") != -1)
        use |= LFS_PIN_USEKEYENCKEY;

    LFSPINIMPORT import=GetImportKeyParam(KeyName,EncKeyName,KeyValue,use);

    return N_ImportKey(&import);
}

int CMPIN::N_Open()
{
    int ret = m_apiCtrl.LFSAsyncOpen(m_szLogicalName,HM_MIDDLE_TIMEOUT,&m_CurrRequestID);

    if(LFS_SUCCESS != ret)
    {
        LOGERROR("%s,打开失败！",__FUNCTION__);
        Alarm("02000000");
        emit OpenFailed();//ErrorSend(ret);
    }

    return ret;
}

int CMPIN::N_Close()
{
    int ret = m_apiCtrl.LFSAsyncClose(&m_CurrRequestID);

    if(LFS_SUCCESS!=ret)
    {
        LOGERROR("%s,关闭失败！",__FUNCTION__);
        Alarm("02000000");
        emit CloseFailed();//ErrorSend(ret);
    }

    return ret;
}

//取消
int CMPIN::N_Cancel(WORD requestID)
{
    int ret = m_apiCtrl.LFSCancelAsyncRequest(requestID);
    if(LFS_SUCCESS != ret)
    {
        LOGERROR("%s,执行错误，错误码：ret = %d",__FUNCTION__,ret);
        Alarm("02000001");
    }

    return ret;
}

//导入密钥
int CMPIN::N_ImportKey(LPLFSPINIMPORT lpImport,LONG timeout)
{
    int ret=m_apiCtrl.LFSAsyncExecute(LFS_CMD_PIN_IMPORT_KEY,lpImport,timeout,&m_CurrRequestID);

    if(ret!=LFS_SUCCESS)
    {
        LOGERROR("%s,执行错误，错误码：ret = %d",__FUNCTION__,ret);
        Alarm("02000001");
        emit KeyLoadFailed();
    }

    return ret;
}

//加解密
int CMPIN::N_Crypt(LPLFSPINCRYPT lpCrypt,LONG timeout)
{
    LOGINFO("%s,IN",__FUNCTION__);

    int ret = m_apiCtrl.LFSAsyncExecute(LFS_CMD_PIN_CRYPT,lpCrypt,timeout,&m_CurrRequestID);

    if(ret!=LFS_SUCCESS)
    {
        LOGERROR("%s,执行错误，错误码：ret = %d",__FUNCTION__,ret);
        Alarm("02000001");
        emit CryptFailed();		// CryptFailed
    }

    LOGINFO("%s,OUT",__FUNCTION__);

    return ret;
}

//Get Pin
int CMPIN::N_GetPin(LPLFSPINGETPIN lpGetPin,LONG timeout)
{
    timeout = (-1==timeout) ? 0 : timeout;

    if(timeout < 0)
    {
        emit GetPinFailed();		// GetPinFailed
        return HM_PARAM_ERROR;
    }

    m_PinLength=0;

    int ret= m_apiCtrl.LFSAsyncExecute(LFS_CMD_PIN_GET_PIN,lpGetPin,timeout,&m_CurrRequestID);

    if(LFS_SUCCESS!=ret)
    {
        LOGERROR("%s,执行错误，错误码：ret = %d",__FUNCTION__,ret);
        Alarm("02000001");
        emit GetPinFailed();// GetPinFailed
    }

    return ret;
}

//Get PinBlock
int CMPIN::N_GetPinBlock(LPLFSPINBLOCK lpPinBlock,LONG timeout)
{
    int ret=m_apiCtrl.LFSAsyncExecute(LFS_CMD_PIN_GET_PINBLOCK,lpPinBlock,timeout,&m_CurrRequestID);

    if(LFS_SUCCESS!=ret)
    {
        LOGERROR("%s,执行错误，错误码：ret = %d",__FUNCTION__,ret);
        Alarm("02000001");
        emit PinBlockFailed();	// PinBlockFailed
    }

    return ret;
}

//Get Data
int CMPIN::N_GetData(LPLFSPINGETDATA lpPinGetData,LONG timeout)
{
    timeout = (-1==timeout) ? 0 : timeout;

    if(timeout < 0)
    {
        LOGERROR("%s,时间参数错误。设置了-1之外的负数,timeout = %d",__FUNCTION__,timeout);
        Alarm("02000002");
        emit GetDataFailed();		// GetDataFailed
        return HM_PARAM_ERROR;
    }

    int ret=m_apiCtrl.LFSAsyncExecute(LFS_CMD_PIN_GET_DATA,lpPinGetData,timeout,&m_CurrRequestID);

    if(LFS_SUCCESS!=ret)
    {
        LOGERROR("%s,执行错误，错误码：ret = %d",__FUNCTION__,ret);
        Alarm("02000001");
        emit GetDataFailed();	// GetDataFailed
    }

    return ret;
}

//LFS_CMD_PIN_INITIALIZATION
int CMPIN::N_Initialization(LPLFSPININIT lpInit,LONG timeout)
{
    int ret = m_apiCtrl.LFSAsyncExecute(LFS_CMD_PIN_INITIALIZATION,lpInit,timeout,&m_CurrRequestID);
    if(LFS_SUCCESS!=ret)
    {
        LOGERROR("%s,执行错误，错误码：ret = %d",__FUNCTION__,ret);
        Alarm("02000001");
        emit InitializeFailed();		// InitializeFailed
    }

    return ret;
}

//LFS_CMD_PIN_RESET
int CMPIN::N_Reset(LONG timeout)
{
    int ret=0;

    ret = m_apiCtrl.LFSAsyncExecute(LFS_CMD_PIN_RESET,NULL,0,&m_CurrRequestID);

    if(LFS_SUCCESS!=ret)
    {
        LOGERROR("%s,执行错误，错误码：ret = %d",__FUNCTION__,ret);
        Alarm("02000001");
        emit ResetFailed();//ErrorSend(ret);
    }

    return ret;
}

//LFS_INF_PIN_KEY_DETAIL
int CMPIN::N_KeyDetail(LPSTR lpsKeyName)
{
    LPLFSRESULT buffer = NULL;
    int ret = m_apiCtrl.LFSGetInfo(LFS_INF_PIN_KEY_DETAIL,lpsKeyName,HM_MIDDLE_TIMEOUT,&buffer);
    if(LFS_SUCCESS == ret)
    {
        m_strKeyDetail = "";
        LPLFSPINKEYDETAIL *lppKeyDetail = (LPLFSPINKEYDETAIL *)((LPLFSRESULT)buffer)->buffer;
        if(NULL == lppKeyDetail)
        {
            LOGERROR("%s,SP数据为空。",__FUNCTION__);
            Alarm("02000002");
            return ret;
        }

        if(1==m_wKeyDetailType)
        {
            QString strKeyNames("");

            for(int i=0;lppKeyDetail&&lppKeyDetail[i];i++)
            {
                strKeyNames+=lppKeyDetail[i]->key_name;
                strKeyNames+=",";
            }

            strKeyNames=strKeyNames.mid(0,strKeyNames.length()-1);//去掉最后一个Comma

            m_strKeyDetail = strKeyNames;
        }
        else
        {
            QString strKeyUseFlags("");

            if(lppKeyDetail&&lppKeyDetail[0])
            {
                WORD keyUse=lppKeyDetail[0]->use;

                if(LFS_PIN_USECRYPT==(keyUse&LFS_PIN_USECRYPT))
                    strKeyUseFlags+=QString("CRYPT")+QString(",");
                if(LFS_PIN_USEFUNCTION==(keyUse&LFS_PIN_USEFUNCTION))
                    strKeyUseFlags+=QString("FUNCTION")+QString(",");
                if(LFS_PIN_USEMACING==(keyUse&LFS_PIN_USEMACING))
                    strKeyUseFlags+=QString("MACING")+QString(",");
                if(LFS_PIN_USEKEYENCKEY==(keyUse&LFS_PIN_USEKEYENCKEY))
                    strKeyUseFlags+=QString("KEYENCKEY")+QString(",");
                if(LFS_PIN_USENODUPLICATE==(keyUse&LFS_PIN_USENODUPLICATE))
                    strKeyUseFlags+=QString("NODUPLICATE")+QString(",");
                if(LFS_PIN_USESVENCKEY==(keyUse&LFS_PIN_USESVENCKEY))
                    strKeyUseFlags+=QString("SVENCKEY")+QString(",");
                if(LFS_PIN_USECONSTRUCT==(keyUse&LFS_PIN_USECONSTRUCT))
                    strKeyUseFlags+=QString("CONSTRUCT")+QString(",");
                if(LFS_PIN_USESECURECONSTRUCT==(keyUse&LFS_PIN_USESECURECONSTRUCT))
                    strKeyUseFlags+=QString("SECURECONSTRUCT")+QString(",");
              //  if(LFS_PIN_USEANSTR31MASTER==(keyUse&LFS_PIN_USEANSTR31MASTER))
              //      strKeyUseFlags+=QString("ANSTR31MASTER")+QString(",");
                if (LFS_PIN_USESM4 == (keyUse & LFS_PIN_USESM4)) //add
                    strKeyUseFlags += QString("SM4") + QString(",");


                strKeyUseFlags=strKeyUseFlags.mid(0,strKeyUseFlags.length()-1);//去掉最后一个Comma
            }

            m_strKeyDetail = strKeyUseFlags;
        }
    }
    else
    {
        //ErrorSend(hResult);
        m_strKeyDetail = "";
        LOGERROR("%s,错误，错误码：ret = %d",__FUNCTION__,ret);
        Alarm("02000001");
    }
    ::LFSFreeResult(buffer);
    return ret;
}

//设备能力
int CMPIN::N_GetCaps()
{
    LPLFSRESULT buffer = NULL;
    int ret = m_apiCtrl.LFSGetInfo(LFS_INF_PIN_CAPABILITIES,NULL,HM_MIDDLE_TIMEOUT,&buffer);
    if(LFS_SUCCESS == ret)
    {
        LPLFSPINCAPS lpCaps = (LPLFSPINCAPS)((LPLFSRESULT)buffer)->buffer;
        if(lpCaps)
            memcpy(&m_Caps,lpCaps,sizeof(LFSPINCAPS));
        else
        {
            LOGERROR("%s,SP数据为空。",__FUNCTION__);
            Alarm("02000002");
            return ret;
        }
        SavePinFormats(m_Caps.pin_formats);
    }
    else
    {
        LOGERROR("%s,错误，错误码：ret = %d",__FUNCTION__,ret);
        Alarm("02000001");
    }
    ::LFSFreeResult(buffer);
    return ret;
}

//设备状态
int CMPIN::N_GetStatus()
{
    LPLFSRESULT buffer = NULL;
    int ret = m_apiCtrl.LFSGetInfo(LFS_INF_PIN_STATUS,NULL,HM_MIDDLE_TIMEOUT,&buffer);
    if(LFS_SUCCESS == ret)
    {
        LPLFSPINSTATUS lpStatus = (LPLFSPINSTATUS)((LPLFSRESULT)buffer)->buffer;
        if(lpStatus)
        {
            memcpy(&m_Status,lpStatus,sizeof(LFSPINSTATUS));
            SetDeviceStatus(m_Status.device);
        }
        else
        {
            emit DeviceError();
            LOGERROR("%s,SP数据为空。",__FUNCTION__);
            Alarm("02000002");
            return ret;
        }

    }
    else
    {
        LOGERROR("%s,错误，错误码：%d",__FUNCTION__,ret);
        Alarm("02000001");
    }
    ::LFSFreeResult(buffer);
    return ret;
}


int CMPIN::N_FuncKeyDetail()
{
    ULONG mask = 0x00000000;

    //  改为同步
    //int ret = m_apiCtrl.LFSAsyncGetInfo(LFS_INF_PIN_FUNCKEY_DETAIL,&mask,HM_midDLE_TIMEOUT,&m_CurrRequestID);
    //if(LFS_SUCCESS!=ret)
    //{
    //	LOGINFO("%s,执行错误，错误码：%d",__FUNCTION__,ret);
    //	ErrorSend(ret);
    //}xxxxxxl


    LPLFSRESULT buffer = NULL;
    int ret = m_apiCtrl.LFSGetInfo(LFS_INF_PIN_FUNCKEY_DETAIL,&mask,HM_MIDDLE_TIMEOUT,&buffer);

    if(LFS_SUCCESS == ret)
    {
        SaveFuncKeysName(((LPLFSPINFUNCKEYDETAIL)((LPLFSRESULT)buffer)->buffer)->func_mask);
    }
    else
    {
        LOGERROR("%s,错误，错误码：ret = %d",__FUNCTION__,ret);
        Alarm("02000001");
    }

    return ret;
}


void CMPIN::SetDeviceStatus(WORD wStatus)
{
    m_PreDevStatus=m_CurrDevStatus;
    switch(wStatus)
    {
    case LFS_IDC_DEVONLINE:
        m_CurrDevStatus="ONLINE";
        break;
    case LFS_IDC_DEVOFFLINE:
    case LFS_IDC_DEVHWERROR:
        m_CurrDevStatus="OFFLINE";
        break;
    case LFS_IDC_DEVPOWEROFF:
        m_CurrDevStatus="POWEROFF";
        break;
    case LFS_IDC_DEVNODEVICE:
        m_CurrDevStatus="NODEVICE";
        break;
    case LFS_IDC_DEVUSERERROR:
        m_CurrDevStatus="USERERROR";
        break;
    case LFS_IDC_DEVBUSY:
        m_CurrDevStatus="BUSY";
        break;
    }
}


LFSPINIMPORT CMPIN::GetImportKeyParam(QString keyName,QString encKeyName,QString keyValue,WORD use)
{
    memset(gEncKeyName,0,sizeof(gEncKeyName));
    memset(gKeyName,0,sizeof(gKeyName));
    memset(gKeyValueBcd,0,sizeof(gKeyValueBcd));

    LFSPINIMPORT import;

    import.use=use;

    strcpy(gEncKeyName,encKeyName.toStdString().c_str());
   // encKeyName.ReleaseBuffer();
    import.enc_key=gEncKeyName;

    strcpy(gKeyName,keyName.toStdString().c_str());
    import.key=gKeyName;
   // keyName.ReleaseBuffer();

    import.ident=NULL;
    import.value=&gxValue;

    gxValue.length=keyValue.length()/2;
    this->ASCIIToBCD((LPBYTE)gKeyValueBcd,(LPBYTE)keyValue.toStdString().c_str(),keyValue.length());
   // keyValue.ReleaseBuffer();
    gxValue.data=(LPBYTE)gKeyValueBcd;

    return import;
}

LFSPINCRYPT CMPIN::GetCryptParam(QString value,QString keyName,BYTE padChar)
{
    LOGINFO("%s,IN",__FUNCTION__);

    memset(gKeyName,0,sizeof(gKeyName));
    memset(gCryptValueBcd,0,sizeof(gCryptValueBcd));

    LFSPINCRYPT crypt;
    crypt.compression=0x00;
    crypt.padding=padChar;
    crypt.start_value_key=NULL;
    crypt.key_enc_key=NULL;
    crypt.start_value=NULL;
    crypt.algorithm=gCryptAlgorithm;
    crypt.mode=gCryptMode;

    strcpy(gKeyName,keyName.toStdString().c_str());
   // keyName.ReleaseBuffer();
    crypt.key=gKeyName;

    crypt.crypt_data=&gxCryptData;
    ASCIIToBCD((LPBYTE)gCryptValueBcd,(LPBYTE)value.toStdString().c_str(),value.length());
    gxCryptData.length=value.length()/2;
    gxCryptData.data=(LPBYTE)gCryptValueBcd;

    LOGINFO("%s,OUT",__FUNCTION__);

    return crypt;
}


int CMPIN::GetCryptAlgorithm(int algorithm,const char * keyName)
{
    LOGINFO("%s,IN",__FUNCTION__);
    char szKeyConfigType[8]={0};
    g_Helper.GetPrivateProfileString("MPIN","ReadKeyConfigMethod","1",szKeyConfigType,sizeof(szKeyConfigType),Q2S(m_strConfigPath));
    LOGINFO("config_Path = %s,KeyConfigType = %s",Q2S(m_strConfigPath),szKeyConfigType);
    int iKeyLen = 0;
    if (!strcmp(szKeyConfigType,"1"))
    {
       iKeyLen = __GetKeyLenFromFile(keyName);
    }
    else
    {
       iKeyLen = __GetKeyLenFormReg(keyName);
    }

    LOGINFO("keyLen = %d,algorithm = %d,OUT", iKeyLen,algorithm);

    if(CRYPT_ALGORITHM_ECB==algorithm && iKeyLen==8)
        return LFS_PIN_CRYPTDESECB;
    else if(CRYPT_ALGORITHM_ECB==algorithm && iKeyLen==16)
        return LFS_PIN_CRYPTTRIDESECB;
    else if(CRYPT_ALGORITHM_CBC==algorithm && iKeyLen==8)
        return LFS_PIN_CRYPTDESCBC;
    else if(CRYPT_ALGORITHM_CBC==algorithm && iKeyLen==16)
        return LFS_PIN_CRYPTTRIDESCBC;
    else if(CRYPT_ALGORITHM_MAC==algorithm && iKeyLen==8)
        return LFS_PIN_CRYPTDESMAC;
    else if(CRYPT_ALGORITHM_MAC==algorithm && iKeyLen==16)
        return LFS_PIN_CRYPTTRIDESMAC;
    else
        return -1;
}


long CMPIN::GetKeysCode(QString strKeys)
{
    long lKeys=0;

    VerStr vecKeys=Token_All(strKeys,',');

    int i=0;
    for(VerStr::iterator it=vecKeys.begin();it!=vecKeys.end();it++)
    {
        std::map<QString,unsigned long>::iterator itm;
        itm=m_KeyMap1.find(*it);
        if(itm!=m_KeyMap1.end())
            lKeys|=itm->second;
    }

    return lKeys;
}


long CMPIN::GetPinBlockFormat(QString strFormat)
{
    long lFormat=0;

    std::map<QString,unsigned long>::iterator itm;
    itm=m_PinBlockFormatMap1.find(strFormat);

    if(itm==m_PinBlockFormatMap1.end())
        return -1;
    lFormat=itm->second;

    return lFormat;
}


bool CMPIN::ASCIIToBCD(unsigned char *bcd, const unsigned char *asc, int nLen)
{
    if ( bcd == NULL || asc == NULL )
    {
        return false;
    }

    char *p = new char[nLen + 1];
    memset(p, 0, nLen + 1);
    int	i = 0;
    int j = 0;

    for (i = 0; i < nLen; ++i)
    {
        if( (asc[i] >= '0') && (asc[i] <= '9') )
        {
            p[i] = asc[i] - '0';
        }
        else if( (asc[i] >= 'a') && (asc[i] <= 'f') )
        {
            p[i] = asc[i] - 'a' + 10;
        }
        else if( (asc[i] >= 'A') && (asc[i] <= 'F') )
        {
            p[i] = asc[i] - 'A' + 10;
        }
        else
        {
            p[i] = asc[i] - '0';
        }
    }

    /*两两配对*/
    j = (nLen + nLen % 2) / 2;
    if ( nLen % 2 )
    {
        p[nLen + 1] = 0;
    }

    for ( i = 0; i < j; ++i )
    {
        bcd[i] = (p[2 * i] & 0x0f) << 4;
        bcd[i] |= (p[2 * i + 1] & 0x0f);
    }

    delete []p;

    return true;
}

bool CMPIN::BCDToASCII(LPBYTE asc, const LPBYTE bcd, int len)
{
    if ( asc == NULL || bcd == NULL )
    {
        return false;
    }

    int	i, j;

    j = (len + len % 2 )/2;
    for ( i = 0; i < j; ++i )
    {
        asc[2 * i]		= (bcd[i] >> 4) & 0x0f;
        asc[2 * i + 1]	= bcd[i] & 0x0f;
    }

    for ( i = 0; i < len; ++i )
    {
        if ( asc[i] > 0x09)
            asc[i] = 'A' + asc[i] - 0x0a;
        else
            asc[i] += '0';
    }

    asc[len] = 0;

    return true;
}

//得到以sep分割的所有子串
CMPIN::VerStr CMPIN::Token_All(const QString str,const char sep)
{
    VerStr verStr;
    int front=0,back=0;

    if(""==str)
        return verStr;

    do{
        front = str.indexOf(sep, back);
        if(-1==front)
        {
            verStr.push_back(str.mid(back,str.length()-back));
        }
        else
        {
            verStr.push_back(str.mid(back,front-back));

            back=front+1;
        }
    }while(-1!=front);

    return verStr;
}


int CMPIN::__GetKeyLenFromFile( const char * szKeyName )
{
    LOGINFO("%s,IN",__FUNCTION__);
/*
    HKEY hKey;
    char strDevName[MAX_PATH]="";
    QString strLogicSrv=".DEFAULT\\XFS\\LOGICAL_SERVICES\\";

    strLogicSrv+=m_szLogicalName;
    DWORD size=0;

    char config_Path[MAX_PATH]={0};

    //有SP逻辑名得到设备名
    if(ERROR_SUCCESS==RegOpenKeyEx(HKEY_USERS,strLogicSrv,0,KEY_READ,&hKey))
    {
        size=MAX_PATH;
        RegQueryValueEx(hKey,"Provider",NULL,NULL,(LPBYTE)strDevName,&size);
        RegCloseKey(hKey);
    }

    QString strSrvDev="SOFTWARE\\XFS\\SERVICE_PROVIDERS\\";
    strSrvDev+=strDevName;

    //得到SP相关信息
    if(ERROR_SUCCESS==RegOpenKeyEx(HKEY_LOCAL_MACHINE,strSrvDev,0,KEY_READ,&hKey))
    {
        //得到配置文件路径
        size=MAX_PATH;
        RegQueryValueEx(hKey,"config",NULL,NULL,(LPBYTE)config_Path,&size);

        RegCloseKey(hKey);
    }

    //char keyDataPath[MAX_PATH]={0};
    //GetPrivateProfileString("PIN","KeyPath","",keyDataPath,MAX_PATH,config_Path);
    QString keyDataPath = g_Helper.GetPrivateProfileString("PIN", "keyName", "", config_Path);

    LOGINFO("config_Path = %s,keyDataPath = %s",config_Path,keyDataPath);

    CKeyMng keyMng;
    keyMng.Initial(keyDataPath,10,4);

    LOGINFO("KeyMng Initial END");

    stSingleKey * lpKey=keyMng.GetKey(szKeyName);
    if(lpKey==NULL)
        return -1;

    int keyLen=lpKey->m_Length;
    return keyLen;
    */
    return 0;
}

int CMPIN::__GetKeyLenFormReg( const char * szKeyName )
{
    LOGINFO("%s,IN",__FUNCTION__);
/*
    // 修改从注册表获取Key长度
    HKEY hKey;
    int iKeyLen = 0;
    QString strSrvDev="SOFTWARE\\XFS\\HMKeyManage\\";
    strSrvDev += szKeyName;

    long lRet = 0;
    //得到PIN KEY相关信息
    if(ERROR_SUCCESS==RegOpenKeyEx(HKEY_LOCAL_MACHINE,strSrvDev,0,KEY_READ,&hKey))
    {
        DWORD size=MAX_PATH;
        lRet = RegQueryValueEx(hKey,"Length",NULL,NULL,(LPBYTE)&iKeyLen,&size);
        if (ERROR_SUCCESS != lRet)
        {
            LOGERROR("读出注册表失败,getlasterr=%d",GetLastError());
            iKeyLen = 16;
        }
    }
    else
    {
        LOGERROR("打开注册表失败,getlasterr=%d",GetLastError());
        iKeyLen = 16;
    }

    RegCloseKey(hKey);
    return iKeyLen;
*/
    return 0;
}

void CMPIN::__GetConfigPath()
{
//    char chFilePath[MAX_PATH]={0};
//    GetModuleFileName(NULL,chFilePath,MAX_PATH);
    QString strFilePath = g_Helper.GetModuleFileName((void*)(&CMPIN::GetInstance));
    //if(chFilePath != NULL)
    if (!strFilePath.isEmpty())
    {
//        QString strFilePath(chFilePath);
        strFilePath = strFilePath.mid(0,strFilePath.lastIndexOf('/'));
        strFilePath += "/ini/Setup.ini";
        m_strConfigPath = strFilePath;
    }
    else
    {
        m_strConfigPath = "";
    }
}

void CMPIN::SaveFuncKeysName(ULONG uFuncKeys)
{
    for(int i=0;i<32;i++)
    {
        int key = uFuncKeys & (1<<i);

        if(0x0000!=key)
        {
            std::map<unsigned long,QString>::iterator it = m_KeyMap.find(key);
            if(it!=m_KeyMap.end())
                m_SuppKeysName += it->second + QString(",");
        }
    }

    m_SuppKeysName = m_SuppKeysName.mid(0,m_SuppKeysName.length()-1);
}


void CMPIN::SavePinFormats(ULONG uPinFormats)
{
    for(int i=0;i<14;i++)
    {
        int format = uPinFormats & (1<<i);
        if(0x0000!=format)
        {
            std::map<unsigned long,QString>::iterator it = m_PinBlockFormatMap.find(format);
            if(it!=m_PinBlockFormatMap.end())
                m_SuppPinFormats += it->second + QString(",");
        }
    }

    m_SuppPinFormats = m_SuppPinFormats.mid(0,m_SuppPinFormats.length()-1);
}

int CMPIN::Execute(int ID, LPVOID lpData)
{
    LOGINFO("Execute ID: %d", ID);
    switch (ID)
    {
    case LFS_INF_PIN_STATUS:
        N_GetStatus();
        memcpy(lpData, m_CurrDevStatus.toStdString().c_str(), m_CurrDevStatus.length());
        break;

    default:
        break;
    }

    return 0;
}

QObject *CreateInstance()
{
    return CMPIN::GetInstance();
}
