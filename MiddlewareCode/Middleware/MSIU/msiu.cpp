#include "msiu.h"
#include "AlarmLib.h"

CMSIU* CMSIU::m_pInstance = NULL;

CMSIU *CMSIU::GetInstance()
{
    if(m_pInstance == NULL)
    {
        m_pInstance = new CMSIU("MSIU");
    }

    return m_pInstance;
}

CMSIU::CMSIU(const QString &strDBusObjName) : CWebInteractObj(strDBusObjName)
{
    m_bOpened = false;
    memset(m_szLogicalName,0,MAX_PATH);

    LOGINFO("MSIU 模块加载");
}

CMSIU::~CMSIU()
{
    LOGINFO("MSIU 退出");
}



// 收到打开完成消息后调用该函数
void CMSIU::WR_OpenComplete(DWORD dwCommand, HRESULT hResult, LPVOID lpBuffer)
{
    //异步打开成功
    if(LFS_SUCCESS == hResult)
    {
        DWORD dwEvent = SERVICE_EVENTS | USER_EVENTS | SYSTEM_EVENTS | EXECUTE_EVENTS;

        REQUESTID id=0;
        int ret = m_apiCtrl.LFSAsyncRegister(dwEvent,&id);

        if(LFS_SUCCESS != ret)
        {
            N_Close();
            emit OpenFailed();
            LOGINFO("%s,注册事件失败,错误码 ret = %d",__FUNCTION__,ret);
            Alarm("08000000");
        }
    }
    else
    {
        emit OpenFailed();
        LOGINFO("%s,打开失败，错误码 ret = %d",__FUNCTION__,hResult);
        Alarm("08000000");
    }
}

// 收到注册完成消息后调用该函数
void CMSIU::WR_RegisterComplete(DWORD dwCommand, HRESULT hResult, LPVOID lpBuffer)
{
    if(LFS_SUCCESS != hResult)
    {
        N_Close();
        LOGINFO("%s,注册事件失败,错误码：hResult = %d",__FUNCTION__,hResult);
        Alarm("08000000");
        emit OpenFailed();
    }
    else
    {
        int ret = N_GetStatus();
        if(LFS_SUCCESS == ret)
        {
            N_GetCaps();
            m_bOpened = true;
            emit ConnectionOpened();
            LOGINFO("%s,事件：ConnectionOpened()",__FUNCTION__);
        }
        else
        {
            N_Close();
            LOGINFO("%s,打开设备失败,错误码：ret = %d",__FUNCTION__,ret);
            Alarm("08000000");
            emit OpenFailed();
        }
    }
}

// 收到关闭完成消息后调用该函数
void CMSIU::WR_CloseComplete(DWORD dwCommand, HRESULT hResult, LPVOID lpBuffer)
{
    if(LFS_SUCCESS == hResult)
    {
        if(m_bOpened)
        {
            m_bOpened = false;
            emit ConnectionClosed();
            LOGINFO("%s,事件：ConnectionClosed()",__FUNCTION__);
        }
    }
    else
    {
        emit CloseFailed();
        LOGINFO("%s,关闭设备失败,错误码：hResult = %d",__FUNCTION__,hResult);
        Alarm("08000000");
    }
}

// 收到打开完成消息后调用该函数
void CMSIU::WR_GetinfoComplete(DWORD dwCommand, HRESULT hResult, LPVOID lpBuffer)
{

}

// 收到执行完成消息后调用该函数
void CMSIU::WR_ExecuteComplete(DWORD dwCommand, HRESULT hResult, LPVOID lpBuffer)
{
    switch(dwCommand)
    {
    case LFS_CMD_SIU_SET_GUIDLIGHT:
        {
            if(hResult == LFS_SUCCESS)
            {
                LOGINFO("%s,LFS_CMD_SIU_SET_GUIDLIGHT 执行成功",__FUNCTION__);
            }
            else
            {
                LOGERROR("%s,LFS_CMD_SIU_SET_GUIDLIGHT 执行失败，错误码：hResult = %d",__FUNCTION__,hResult);
                Alarm("08000001");
            }
        }
        break;

    default:
        break;
    }

}

// 收到执行事件后调用该函数
void CMSIU::WR_ExecuteEvent(DWORD dwEventID, HRESULT hResult, LPVOID lpBuffer)
{

}

// 收到用户事件后调用该函数
void CMSIU::WR_UserEvent(DWORD dwEventID, HRESULT hResult, LPVOID lpBuffer)
{

}

// 收到服务事件后调用该函数
void CMSIU::WR_ServiceEvent(DWORD dwEventID, HRESULT hResult, LPVOID lpBuffer)
{

}

// 收到系统事件后调用该函数
void CMSIU::WR_SystemEvent(DWORD dwEventID, HRESULT hResult, LPVOID lpBuffer)
{

}


// 以下为属性的处理函数

void CMSIU::SetServiceName(QString newVal)
{
    LOGINFO("函数：%s",__FUNCTION__);

    strcpy(m_szLogicalName, Q2S(newVal));
}

QString CMSIU::GetStDetailedDeviceStatus(void)
{
    LOGINFO("函数：%s",__FUNCTION__);

    N_GetStatus();

    return m_DetailedDeviceStatus;
}

QString CMSIU::GetStCardReaderLightStatus(void)
{
    LOGINFO("%s，方法。",__FUNCTION__);

    N_GetStatus();
    return StCardReaderLightStatus;
}

QString CMSIU::GetStPinPadLightStatus(void)
{
    LOGINFO("%s，方法。",__FUNCTION__);

    N_GetStatus();
    return StPinPadLightStatus;
}

QString CMSIU::GetStReceiptPrinterLightStatus(void)
{
    LOGINFO("%s，方法。",__FUNCTION__);

    N_GetStatus();
    return StReceiptPrinterLightStatus;
}

// 身份证指示灯
QString CMSIU::GetStScannerLightStatus(void)
{
    LOGINFO("%s，方法。",__FUNCTION__);

    return StScannerLightStatus;
}


// 以下为方法的处理函数

SHORT CMSIU::CloseConnection(void)
{
    LOGINFO("%s，方法。",__FUNCTION__);

    int ret = LFS_SUCCESS;
    if (m_bOpened)
    {
        ret=N_Close();
    }
    return ret;
}

SHORT CMSIU::OpenConnection(void)
{
    LOGINFO("%s，方法。",__FUNCTION__);

    if(m_bOpened)
    {
        emit ConnectionOpened();
        LOGINFO("%s,事件：ConnectionOpened（）",__FUNCTION__);
        return 0;
    }

    //m_apiCtrl.SetSendWnd(GetWindowHwnd());

    return N_Open();
}

SHORT CMSIU::SetCardReaderLight(QString FlashRate)
{
    LOGINFO("函数：%s",__FUNCTION__);

    WORD Command;

    if(FlashRate == "OFF" )
    {
        Command = LFS_SIU_OFF;
    }
    else if(FlashRate == "SLOW")
    {
        Command = LFS_SIU_SLOW_FLASH;
    }
    else if(FlashRate == "MEDIUM")
    {
        Command = LFS_SIU_MEDIUM_FLASH;
    }
    else if(FlashRate == "QUICK")
    {
        Command = LFS_SIU_QUICK_FLASH;
    }
    else if(FlashRate == "CONTINUOUS")
    {
        Command = LFS_SIU_CONTINUOUS;
    }
    else
    {
        LOGERROR("%s, 参数设置错误", __FUNCTION__);
        Alarm("08000002");
        return -1;
    }

    LFSSIUSETGUIDLIGHT SetGuidLight;

    SetGuidLight.guid_light = LFS_SIU_CARDUNIT ;
    SetGuidLight.command = Command;

    REQUESTID id=0;
    HRESULT ret = m_apiCtrl.LFSAsyncExecute(LFS_CMD_SIU_SET_GUIDLIGHT , &SetGuidLight, 3000, &id);
    if (LFS_SUCCESS != ret)
    {
        LOGERROR("%s, LFSAsyncExecute返回错误：%d", __FUNCTION__, ret);
        Alarm("08000001");
    }

    return ret;
}

SHORT CMSIU::SetPinPadLight(QString FlashRate)
{
    LOGINFO("函数：%s",__FUNCTION__);

    WORD Command;

    if(FlashRate == "OFF" )
    {
        Command = LFS_SIU_OFF;
    }
    else if(FlashRate == "SLOW")
    {
        Command = LFS_SIU_SLOW_FLASH;
    }
    else if(FlashRate == "MEDIUM")
    {
        Command = LFS_SIU_MEDIUM_FLASH;
    }
    else if(FlashRate == "QUICK")
    {
        Command = LFS_SIU_QUICK_FLASH;
    }
    else if(FlashRate == "CONTINUOUS")
    {
        Command = LFS_SIU_CONTINUOUS;
    }
    else
    {
        LOGERROR("%s, 参数设置错误", __FUNCTION__);
        Alarm("08000002");
        return -1;
    }

    LFSSIUSETGUIDLIGHT SetGuidLight;

    SetGuidLight.guid_light = LFS_SIU_PINPAD ;
    SetGuidLight.command = Command;

    REQUESTID id=0;
    HRESULT ret = m_apiCtrl.LFSAsyncExecute(LFS_CMD_SIU_SET_GUIDLIGHT , &SetGuidLight, 3000, &id);
    if (LFS_SUCCESS != ret)
    {
        LOGERROR("%s, LFSAsyncExecute返回错误：%d", __FUNCTION__, ret);
    }

    return ret;
}


SHORT CMSIU::SetReceiptPrinterLight(QString FlashRate)
{
    LOGINFO("函数：%s",__FUNCTION__);

    WORD Command;

    if(FlashRate == "OFF" )
    {
        Command = LFS_SIU_OFF;
    }
    else if(FlashRate == "SLOW")
    {
        Command = LFS_SIU_SLOW_FLASH;
    }
    else if(FlashRate == "MEDIUM")
    {
        Command = LFS_SIU_MEDIUM_FLASH;
    }
    else if(FlashRate == "QUICK")
    {
        Command = LFS_SIU_QUICK_FLASH;
    }
    else if(FlashRate == "CONTINUOUS")
    {
        Command = LFS_SIU_CONTINUOUS;
    }
    else
    {
        LOGERROR("%s, 参数设置错误", __FUNCTION__);
        Alarm("08000002");
        return -1;
    }

    LFSSIUSETGUIDLIGHT SetGuidLight;

    SetGuidLight.guid_light = LFS_SIU_RECEIPTPRINTER ;
    SetGuidLight.command = Command;

    REQUESTID id=0;
    HRESULT ret = m_apiCtrl.LFSAsyncExecute(LFS_CMD_SIU_SET_GUIDLIGHT , &SetGuidLight, 3000, &id);
    if (LFS_SUCCESS != ret)
    {
        LOGERROR("%s, LFSAsyncExecute返回错误：%d", __FUNCTION__, ret);
        Alarm("08000001");
    }

    return ret;
}


SHORT CMSIU::SetCustomLight(QString CustomLightName, QString FlashRate)
{
    return 0;
}

QString CMSIU::GetCustomLightStatusSync(QString CustomLightName)
{
    return "";
}

SHORT CMSIU::SetScannerLight(QString FlashRate)
{
    LOGINFO("函数：%s",__FUNCTION__);

    WORD Command;

    if(FlashRate == "OFF" )
    {
        Command = LFS_SIU_OFF;
    }
    else if(FlashRate == "SLOW")
    {
        Command = LFS_SIU_SLOW_FLASH;
    }
    else if(FlashRate == "MEDIUM")
    {
        Command = LFS_SIU_MEDIUM_FLASH;
    }
    else if(FlashRate == "QUICK")
    {
        Command = LFS_SIU_QUICK_FLASH;
    }
    else if(FlashRate == "CONTINUOUS")
    {
        Command = LFS_SIU_CONTINUOUS;
    }
    else
    {
        LOGERROR("%s, 参数设置错误", __FUNCTION__);
        Alarm("08000002");
        return -1;
    }

    LFSSIUSETGUIDLIGHT SetGuidLight;

    SetGuidLight.guid_light = LFS_SIU_SCANNER ;
    SetGuidLight.command = Command;

    REQUESTID id=0;
    HRESULT ret = m_apiCtrl.LFSAsyncExecute(LFS_CMD_SIU_SET_GUIDLIGHT , &SetGuidLight, 3000, &id);
    if (LFS_SUCCESS != ret)
    {
        LOGERROR("%s, LFSAsyncExecute返回错误：%d", __FUNCTION__, ret);
        Alarm("08000001");
    }

    return 0;
}

// 添加超时
void CMSIU::Reset(LONG Timeout)
{
    REQUESTID id=0;
    HRESULT ret = m_apiCtrl.LFSAsyncExecute(LFS_CMD_SIU_RESET, NULL, Timeout, &id);
    if(ret != LFS_SUCCESS)
    {
        emit ResetFailed();
    }
}

//user s
HRESULT CMSIU::N_Open()
{
    REQUESTID id=0;
    int ret = m_apiCtrl.LFSAsyncOpen(m_szLogicalName,0,&id);

    if(LFS_SUCCESS!=ret)
    {
        LOGERROR("%s,关闭失败！",__FUNCTION__);
        Alarm("08000000");
        emit CloseFailed();
    }

    return ret;
}

HRESULT CMSIU::N_Close()
{
    REQUESTID id=0;
    int ret = m_apiCtrl.LFSAsyncClose(&id);

    if(LFS_SUCCESS!=ret)
    {
        LOGERROR("%s,打开失败！",__FUNCTION__);
        Alarm("08000000");
        emit CloseFailed();
    }

    return ret;
}


HRESULT CMSIU::N_GetStatus()
{
    LPLFSRESULT lpBuffer = NULL;
    int ret = m_apiCtrl.LFSGetInfo(LFS_INF_SIU_STATUS ,NULL,0,&lpBuffer);

    if(LFS_SUCCESS == ret)
    {
        LPLFSSIUSTATUS lpStatus = (LPLFSSIUSTATUS)((LPLFSRESULT)lpBuffer)->buffer;
        memcpy(&m_Status,lpStatus,sizeof(LFSSIUSTATUS));
        N_SetDeviceStatus(m_Status.device);
        N_SetLigntStatus(m_Status.guid_lights);
    }
    else
    {
        LOGERROR("%s,执行错误，错误码：ret = %d",__FUNCTION__,ret);
        Alarm("08000001");
    }
    return ret;
}

HRESULT CMSIU::N_GetCaps()
{
    LPLFSRESULT lpBuffer = NULL;
    int ret = m_apiCtrl.LFSGetInfo(LFS_INF_SIU_CAPABILITIES,NULL,0,&lpBuffer);

    if(LFS_SUCCESS == ret)
    {
        LPLFSSIUCAPS lpCaps = (LPLFSSIUCAPS)((LPLFSRESULT)lpBuffer)->buffer;
        memcpy(&m_Caps,lpCaps,sizeof(LFSSIUCAPS));
    }
    else
    {
        LOGERROR("%s,执行错误，错误码：ret = %d",__FUNCTION__,ret);
        Alarm("08000001");
    }
    return ret;
}

void CMSIU::N_SetDeviceStatus(WORD wStatus)
{
    m_PreDevStatus=m_DetailedDeviceStatus;
    switch(wStatus)
    {
        case LFS_SIU_DEVONLINE:
            m_DetailedDeviceStatus="ONLINE";
            break;
        case LFS_SIU_DEVOFFLINE:
        case LFS_SIU_DEVHWERROR:
            m_DetailedDeviceStatus="OFFLINE";
            break;
        case LFS_SIU_DEVPOWEROFF:
            m_DetailedDeviceStatus="POWEROFF";
            break;
        case LFS_SIU_DEVNODEVICE:
            m_DetailedDeviceStatus="NODEVICE";
            break;
        case LFS_SIU_DEVUSERERROR:
            m_DetailedDeviceStatus="USERERROR";
            break;
        case LFS_SIU_DEVBUSY:
            m_DetailedDeviceStatus="BUSY";
            break;
    }

    switch(wStatus)
    {
    case LFS_SIU_DEVONLINE:
        m_DeviceStatus = "HEALTHY";
        break;
    case LFS_SIU_DEVNODEVICE:
        m_DeviceStatus = "NODEVICE";
        break;
    default:
        m_DeviceStatus = "FATAL";
        break;
    }
}


void CMSIU::N_SetLigntStatus(WORD *GuidLights)
{
    StCardReaderLightStatus = N_GetLigntStatus(GuidLights[LFS_SIU_CARDUNIT]);
    StPinPadLightStatus = N_GetLigntStatus(GuidLights[LFS_SIU_PINPAD]);
    StNoteDispenserLightStatus = N_GetLigntStatus(GuidLights[LFS_SIU_NOTESDISPENSER]);
    StCoinDispenserLightStatus = N_GetLigntStatus(GuidLights[LFS_SIU_COINDISPENSER]);
    StReceiptPrinterLightStatus = N_GetLigntStatus(GuidLights[LFS_SIU_RECEIPTPRINTER]);
    StPassbookPrinterLightStatus = N_GetLigntStatus(GuidLights[LFS_SIU_PASSBOOKPRINTER]);
    StEnvelopeDepositoryLightStatus = N_GetLigntStatus(GuidLights[LFS_SIU_ENVDEPOSITORY]);
    StEnvelopeDispenserLightStatus = N_GetLigntStatus(GuidLights[LFS_SIU_ENVDISPENSER]);
    StChequeUnitLightStatus = N_GetLigntStatus(GuidLights[LFS_SIU_CHEQUEUNIT]);
    StBillAcceptorLightStatus = N_GetLigntStatus(GuidLights[LFS_SIU_BILLACCEPTOR]);
    StCoinAcceptorLightStatus = N_GetLigntStatus(GuidLights[LFS_SIU_COINACCEPTOR]);
    StDocumentPrinterLightStatus = N_GetLigntStatus(GuidLights[LFS_SIU_DOCUMENTPRINTER]);
    StScannerLightStatus = N_GetLigntStatus(GuidLights[LFS_SIU_SCANNER]);

}

QString CMSIU::N_GetLigntStatus(WORD flags)
{
    switch(flags)
    {
    case LFS_SIU_NOT_AVAILABLE :
        return "NODEVICE";
    case LFS_SIU_OFF:
    case LFS_SIU_SLOW_FLASH:
    case LFS_SIU_MEDIUM_FLASH:
    case LFS_SIU_QUICK_FLASH:
    case LFS_SIU_CONTINUOUS:
        return "HEALTHY";
    }
}
//user e

int CMSIU::Execute(int ID, LPVOID lpData)
{
    switch (ID)
    {
    case LFS_INF_SIU_STATUS:
        N_GetStatus();
        memcpy(lpData, Q2S(m_DetailedDeviceStatus), m_DetailedDeviceStatus.length());
        break;

    default:
        break;
    }
    return 0;
}

QObject *CreateInstance()
{
    return CMSIU::GetInstance();
}
