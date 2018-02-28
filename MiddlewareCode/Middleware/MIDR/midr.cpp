#include "midr.h"
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QCoreApplication>
#include <AlarmLib.h>

#define HM_PARAM_ERROR -10000

CMIDR* CMIDR::m_pInstance = NULL;

CMIDR *CMIDR::GetInstance()
{
    if(m_pInstance == NULL)
    {
        m_pInstance = new CMIDR("MIDR");
    }

    return m_pInstance;
}

CMIDR::CMIDR(const QString &strDBusObjName) : CWebInteractObj(strDBusObjName)
{
    m_PreDevStatus = m_CurrDevStatus = "OFFLINE";
    m_bOpened = false;
    InitMap();

    GetConfigPath();
    memset(m_szLogicalName, 0, MAX_PATH);

    LOGINFO("二代证模块加载！！");
}

CMIDR::~CMIDR()
{
    LOGINFO("二代证模块卸载！！");
    if (m_pInstance)
    {
        delete m_pInstance;
    }
}

void  CMIDR::WR_OpenComplete(DWORD dwCommand, HRESULT hResult, LPVOID lpBuffer)
{
    //异步打开成功
    if(LFS_SUCCESS == hResult)
    {
        DWORD dwEvent = SERVICE_EVENTS | USER_EVENTS | SYSTEM_EVENTS | EXECUTE_EVENTS;
        int ret = m_apiCtrl.LFSAsyncRegister(dwEvent, &m_CurrRequestID);
        if(LFS_SUCCESS != ret)
        {
            Alarm("06000000");
            N_Close();
            emit OpenFailed();
            LOGERROR("%s, LFSAsyncRegister调用失败,错误码 ret = %d，发送OpenFailed事件", __FUNCTION__, ret);
        }
    }
    else if (LFS_ERR_TIMEOUT == hResult)
    {
        emit Timeout();
        LOGERROR("%s, WR_OpenComplete返回超时，错误码 ret = %d", __FUNCTION__, hResult);
        Alarm("06000000");
    }
    else
    {
        emit OpenFailed();
        LOGERROR("%s,WR_OpenComplete返回失败，错误码 ret = %d", __FUNCTION__, hResult);
        Alarm("06000000");
    }
}

void  CMIDR::WR_RegisterComplete(DWORD dwCommand, HRESULT hResult, LPVOID lpBuffer)
{
    if (LFS_ERR_TIMEOUT == hResult)
    {
        emit Timeout();
        LOGERROR("%s, WR_RegisterComplete返回超时，错误码 ret = %d", __FUNCTION__, hResult);
        Alarm("06000000");
    }
    else if(LFS_SUCCESS != hResult)
    {
        N_Close();
        emit OpenFailed();
        LOGERROR("%s, WR_RegisterComplete返回失败,错误码：%d", __FUNCTION__, hResult);
        Alarm("06000000");
    }
    else
    {
        int ret = N_GetCaps();
        if ( ret != LFS_SUCCESS)
        {
            N_InitCaps();
        }

        m_bOpened = true;
        emit ConnectionOpened();

        LOGINFO("%s,事件：ConnectionOpened()", __FUNCTION__);
    }
}


// 收到关闭完成消息后调用该函数
void CMIDR::WR_CloseComplete(DWORD dwCommand, HRESULT hResult, LPVOID lpBuffer)
{
    if (LFS_ERR_TIMEOUT == hResult)
    {
        emit Timeout();
        LOGERROR("%s, WR_CloseComplete返回超时，错误码 ret = %d", __FUNCTION__, hResult);
        Alarm("06000000");
    }
    else if(LFS_SUCCESS == hResult)
    {
        m_bOpened = false;
        emit ConnectionClosed();
        LOGINFO("%s,事件：ConnectionClosed()", __FUNCTION__ );
    }
    else
    {
        emit CloseFailed();
        LOGERROR("%s,关闭设备失败,错误码：%d", __FUNCTION__, hResult);
        Alarm("06000000");
    }
}

// 收到打开完成消息后调用该函数
void CMIDR::WR_GetinfoComplete(DWORD dwCommand, HRESULT hResult, LPVOID lpBuffer)
{

}

// 收到执行完成消息后调用该函数
void CMIDR::WR_ExecuteComplete(DWORD dwCommand, HRESULT hResult, LPVOID lpBuffer)
{
    LOGINFO("%s,IN", __FUNCTION__);

    switch(dwCommand)
    {
    case LFS_CMD_IDC_EJECT_CARD:
        {
            if(LFS_SUCCESS == hResult)
            {
                emit CardEjected();	//没有超时，发送退卡事件
                LOGINFO("%s,事件：CardEjected（）。", __FUNCTION__);
            }
            else if(LFS_ERR_CANCELED == hResult)
            {
                emit CardWaitCanceled();
                LOGINFO("%s,事件：CardEjected（）。", __FUNCTION__);
            }
            else if (LFS_ERR_TIMEOUT == hResult)
            {
                emit Timeout();
                LOGERROR("%s,Eject超时，错误码：%d。", __FUNCTION__, hResult);
                Alarm("06000001");
            }
            else
            {
                emit CardEjectFailed();
                LOGERROR("%s,有错误产生，错误码：%d。", __FUNCTION__, hResult);
                Alarm("06000001");
            }
        }
        break;
    case LFS_CMD_IDC_RETAIN_CARD:
        {
            if(LFS_SUCCESS == hResult)
            {
                emit CardCaptured();
                LOGINFO("%s,事件：CardCaptured()。", __FUNCTION__);
            }
            else if (LFS_ERR_TIMEOUT == hResult)
            {
                emit Timeout();
                LOGERROR("%s, Capture超时，错误码：%d。",__FUNCTION__, hResult);
                Alarm("06000001");
            }
            else
            {
                emit CardCaptureFailed();
                LOGERROR("%s,有错误产生，错误码：%d。",__FUNCTION__, hResult);
                Alarm("06000001");
            }
        }
        break;
    case LFS_CMD_IDC_READ_RAW_DATA:
        {
            if(LFS_SUCCESS == hResult)
            {
                LPLFSIDCCARDDATA *lppCardData = (LPLFSIDCCARDDATA *)((LPLFSRESULT)lpBuffer)->buffer;
                if(lppCardData == NULL)
                {
                    emit CardAcceptFailed();
                    LOGINFO("%s, LFS_CMD_IDC_READ_RAW_DATA返回数据为空。", __FUNCTION__);
                    return;
                }
                if(lppCardData[0]&&lppCardData[0]->data)
                    ProcessCardData(US2Q(lppCardData[0]->data));

                if(lppCardData[1]&&lppCardData[1]->data)
                    m_FrontFilePath = US2Q(lppCardData[1]->data);

                if(lppCardData[2]&&lppCardData[2]->data)
                    m_BackFilePath = US2Q(lppCardData[2]->data);

                //CopyPicture();  暂不使用

       //         m_InfoAll.Format("%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s", \
       //             m_Name, m_Sex, m_Nation, m_BirthDay, m_Number, m_Address, m_Department, m_StartTime, m_Endtime, \
       //             m_PortraitFilePath, m_FrontFilePath, m_BackFilePath);
                m_InfoAll = QString("%1,%2,%3,%4,%5,%6,%7,%8,%9,%10,%11,%12")
                        .arg(m_Name).arg(m_Sex).arg(m_Nation).arg(m_BirthDay)
                        .arg(m_Number).arg(m_Address).arg(m_Department).arg(m_StartTime)
                        .arg(m_Endtime).arg(m_PortraitFilePath).arg(m_FrontFilePath).arg(m_BackFilePath);

                emit CardAccepted(m_InfoAll);

                LOGINFO("%s,事件：CardAccepted(%s)。",__FUNCTION__, Q2S(m_InfoAll));
            }
            else if(LFS_ERR_CANCELED == hResult)
            {
                emit CardAcceptCanceled();
                LOGINFO("%s,事件：CardAcceptCanceled()。", __FUNCTION__);
            }
            else if(hResult == LFS_ERR_IDC_INVALIDMEDIA || hResult == LFS_ERR_IDC_INVALIDDATA)
            {

                emit CardInvalid();
                LOGINFO("%s,事件：CardInvalid()", __FUNCTION__);
            }
            else if (LFS_ERR_TIMEOUT == hResult)
            {
                emit Timeout();
                LOGERROR("%s, CardAccept超时，错误码：%d。", __FUNCTION__, hResult);
                Alarm("06000001");
            }
            else
            {
                emit CardAcceptFailed();
                LOGERROR("%s,有错误产生，错误码：%d。", __FUNCTION__, hResult);
                Alarm("06000001");
            }
        }
        break;
    case LFS_CMD_IDC_RESET:
        if(LFS_SUCCESS == hResult)
        {
            emit ResetComplete();
            LOGINFO("%s,事件：ResetComplete()。", __FUNCTION__);
        }
        else if (LFS_ERR_TIMEOUT == hResult)
        {
            emit Timeout();
            LOGERROR("%s,Reset超时，错误码：%d。", __FUNCTION__, hResult);
            Alarm("06000001");
        }
        else
        {
            emit ResetFailed();
            LOGERROR("%s,有错误产生，错误码：%d。", __FUNCTION__, hResult);
            Alarm("06000001");
        }
        break;
    }

    LOGINFO("%s,OUT", __FUNCTION__);
}

// 收到执行事件后调用该函数
void CMIDR::WR_ExecuteEvent(DWORD dwEventID, HRESULT hResult, LPVOID lpBuffer)
{
    switch(dwEventID)
    {
    case LFS_EXEE_IDC_INVALIDTRACKDATA:
    case LFS_EXEE_IDC_INVALIDMEDIA:
        emit this->CardInvalid();
        LOGINFO("%s,事件：CardInvalid()。", __FUNCTION__);
        break;
    case LFS_EXEE_IDC_MEDIAINSERTED:
        emit this->CardInserted();
        LOGINFO("%s,事件：CardInserted()。", __FUNCTION__);
        break;
    }
}

// 收到用户事件后调用该函数
void CMIDR::WR_UserEvent(DWORD dwEventID, HRESULT hResult, LPVOID lpBuffer)
{

}

// 收到服务事件后调用该函数
void CMIDR::WR_ServiceEvent(DWORD dwEventID, HRESULT hResult, LPVOID lpBuffer)
{
    switch(dwEventID)
    {
    case LFS_SRVE_IDC_MEDIAREMOVED:
        {
            emit this->CardTaken();
            LOGINFO("%s,事件：CardTaken()。", __FUNCTION__);
        }
        break;
    }
}

// 收到系统事件后调用该函数
void CMIDR::WR_SystemEvent(DWORD dwEventID, HRESULT hResult, LPVOID lpBuffer)
{
    switch(dwEventID)
    {
    case LFS_SYSE_DEVICE_STATUS://设备状态改变
        {
            LPLFSDEVSTATUS lpDevStatus = (LPLFSDEVSTATUS)((LPLFSRESULT)lpBuffer)->buffer;

            if(lpDevStatus == NULL)
            {
                LOGINFO("%s, LFS_SYSE_DEVICE_STATUS事件返回数据为空。", __FUNCTION__);
                return;
            }

            SetDeviceStatus(lpDevStatus->state);
            if(m_CurrDevStatus != m_PreDevStatus)
            {

                emit this->StatusChanged(m_szLogicalName, m_PreDevStatus, m_CurrDevStatus);
                LOGINFO("%s,事件：StatusChanged(\"StDeviceStatus\",\"%s\",\"%s\")", __FUNCTION__, Q2S(m_PreDevStatus), Q2S(m_CurrDevStatus));

            }
        }
        break;
    }
}


// 以下为属性的处理函数

void CMIDR::SetServiceName(QString newVal)
{
    LOGINFO("%s,属性,newVal = %s", __FUNCTION__, Q2S(newVal));
    strcpy(m_szLogicalName, Q2S(newVal));
}


QString CMIDR::GetStDetailedDeviceStatus(void)
{
    int ret = N_GetStatus();
    if (ret != LFS_SUCCESS)
    {
        N_InitStatus();
    }

    LOGINFO("%s,Status = %s", __FUNCTION__, Q2S(m_CurrDevStatus));

    return m_CurrDevStatus;
}

QString CMIDR::GetStMediaStatus(void)
{
    int ret = N_GetStatus();
    if (ret != LFS_SUCCESS)
    {
        N_InitStatus();
    }

    QString mediaStatus("");
    switch(m_Status.media)
    {
    case LFS_IDC_MEDIAPRESENT:
        mediaStatus="PRESENT";
        break;
    case LFS_IDC_MEDIANOTPRESENT:
        mediaStatus="NOTPRESENT";
        break;
    case LFS_IDC_MEDIAJAMMED:
        mediaStatus="JAMMED";
        break;
    case LFS_IDC_MEDIAENTERING:
        mediaStatus="INJAWS";
        break;
    default:
        mediaStatus="NOTPRESENT";
        break;
    }

    LOGINFO("%s,mediaStatus = %s,m_Status.fwMedia = %d", __FUNCTION__, Q2S(mediaStatus), m_Status.media);

    return mediaStatus;
}

BOOL CMIDR::GetCpCanEject(void)
{
    return false;
}

BOOL CMIDR::GetCpCanCapture(void)
{
    return false;
}

QString CMIDR::GetName(void)
{
    LOGINFO("%s,属性,m_Name = %s", __FUNCTION__, Q2S(m_Name));

    return m_Name;
}

QString CMIDR::GetSex(void)
{
    LOGINFO("%s,属性,m_Sex = %s", __FUNCTION__, Q2S(m_Sex));

    return m_Sex;
}

QString CMIDR::GetNation(void)
{
    LOGINFO("%s,属性,m_Nation = %s", __FUNCTION__, Q2S(m_Nation));

    return m_Nation;
}

QString CMIDR::GetBirthday(void)
{
    LOGINFO("%s,属性,m_BirthDay = %s", __FUNCTION__, Q2S(m_BirthDay));

    return m_BirthDay;
}

QString CMIDR::GetAddress(void)
{
    LOGINFO("%s,属性,m_Address = %s", __FUNCTION__, Q2S(m_Address));

    return m_Address;
}

QString CMIDR::GetNumber(void)
{
    LOGINFO("%s,属性,m_Number = %s", __FUNCTION__, Q2S(m_Number));

    return m_Number;
}

QString CMIDR::GetDepartment(void)
{
    LOGINFO("%s,属性,m_Department = %s", __FUNCTION__, Q2S(m_Department));

    return m_Department;
}

QString CMIDR::GetStartTime(void)
{
    LOGINFO("%s,属性,m_StartTime = %s", __FUNCTION__, Q2S(m_StartTime));

    return m_StartTime;
}

QString CMIDR::GetEndTime(void)
{
    LOGINFO("%s,属性,m_Endtime = %s", __FUNCTION__, Q2S(m_Endtime));

    return m_Endtime;
}

QString CMIDR::GetPortraitFilePath(void)
{
    LOGINFO("%s,属性,m_PortraitFilePath = %s", __FUNCTION__, Q2S(m_PortraitFilePath));

    return m_PortraitFilePath;
}

QString CMIDR::GetFrontFilePath(void)
{
    LOGINFO("%s,属性,m_FrontFilePath = %s", __FUNCTION__, Q2S(m_FrontFilePath));

    return m_FrontFilePath;
}

void CMIDR::SetFrontFilePath(QString newVal)
{
    LOGINFO("%s,属性,newVal = %s", __FUNCTION__, Q2S(newVal));

    m_FrontFilePath = newVal;
}

QString CMIDR::GetBackFilePath(void)
{
    LOGINFO("%s,属性,m_BackFilePath = %s", __FUNCTION__, Q2S(m_BackFilePath));

    return m_BackFilePath;
}

void CMIDR::SetBackFilePath(QString newVal)
{
    LOGINFO("%s,属性,newVal = %s", __FUNCTION__, Q2S(newVal));

    m_BackFilePath = newVal;
}


// 以下为方法的处理函数

SHORT CMIDR::CloseConnection(void)
{
    LOGINFO("%s，方法。", __FUNCTION__);
    int ret = LFS_SUCCESS;
    if (m_bOpened)
    {
        ret = N_Close();
    }

    return ret;
}

SHORT CMIDR::OpenConnection(void)
{
    LOGINFO("%s，方法。", __FUNCTION__);

    if(m_bOpened)
    {
        emit ConnectionOpened();
        LOGINFO("%s,事件：ConnectionOpened（）", __FUNCTION__);
        return 0;
    }

    //m_apiCtrl.SetSendWnd(GetWindowHwnd());
    return N_Open();
}


SHORT CMIDR::Reset(QString ResetAction)
{
    LOGINFO("%s,方法。", __FUNCTION__);

    WORD action=0;

    if(ResetAction == "RETRACT")
        action = LFS_IDC_RETAIN;
    else if(ResetAction == "EJECT")
        action = LFS_IDC_EJECT;
    else if(ResetAction == "NOACTION")
        action = LFS_IDC_NOACTION;
    else
    {
        emit ResetFailed();
        LOGINFO("%s,参数错误。", __FUNCTION__);
        return HM_PARAM_ERROR;
    }

    return N_Reset(action);
}

SHORT CMIDR::AcceptAndRead(LONG Timeout)
{
    LOGINFO("%s,方法。", __FUNCTION__);

    //数据清空
    m_Name = m_Sex = m_Nation = m_BirthDay = m_Number = m_Address = m_Department = m_StartTime = m_Endtime = "";
    m_InfoAll = "";
    m_PortraitFilePath = m_BackFilePath = m_FrontFilePath = "";

    int ret = N_ReadRawData(LFS_IDC_TRACK1 | LFS_IDC_TRACK2 | LFS_IDC_TRACK3, Timeout);
    if (LFS_SUCCESS != ret)
    {
        emit CardAcceptFailed();
    }

    m_AcceptRequestID = m_CurrRequestID;

    return ret;
}

SHORT CMIDR::Eject(LONG Timeout)
{
    LOGINFO("%s,方法。", __FUNCTION__);

    int ret = N_EjectCard(Timeout);

    m_EjectRequestID = m_CurrRequestID;
    return ret;
}

SHORT CMIDR::Capture(LONG Timeout)
{
    LOGINFO("%s,方法。", __FUNCTION__);

    return N_RetainCard();
}

SHORT CMIDR::CancelAccept(void)
{
    LOGINFO("%s,方法。", __FUNCTION__);

    if (0 != N_Cancel(m_AcceptRequestID))
    {
        emit CancelAcceptCardFailed();
        return -1;
    }

    return 0;
}

SHORT CMIDR::CancelWait(void)
{
    LOGINFO("%s,方法。", __FUNCTION__);

    if (0 != N_Cancel(m_EjectRequestID))
    {
        emit CancelWaitFailed();
        return -1;
    }

    return 0;
}

int CMIDR::N_Open()
{
    int ret = m_apiCtrl.LFSAsyncOpen(m_szLogicalName, HM_MIDDLE_TIMEOUT, &m_CurrRequestID);
    if(LFS_SUCCESS != ret)
    {
        LOGERROR("%s,打开失败！", __FUNCTION__);
        Alarm("06000000");
        emit OpenFailed();
    }

    return ret;
}

int CMIDR::N_Close()
{
    int ret = m_apiCtrl.LFSAsyncClose(&m_CurrRequestID);
    if(LFS_SUCCESS != ret)
    {
        LOGERROR("%s,关闭失败！", __FUNCTION__);
        Alarm("06000000");
        emit CloseFailed();
    }

    return ret;
}



//取消当前正在执行的命令
int CMIDR::N_Cancel(WORD requestID)
{
    int ret = m_apiCtrl.LFSCancelAsyncRequest(requestID);
    if(LFS_SUCCESS != ret)
    {
        LOGERROR("%s,执行错误，错误码：%d。", __FUNCTION__, ret);
        Alarm("06000000");
    }

    return ret;
}

//执行LFS_INF_IDC_STATUS
int CMIDR::N_GetStatus()
{
    LPLFSRESULT lpResult = NULL;
    int ret = m_apiCtrl.LFSGetInfo(LFS_INF_IDC_STATUS, NULL, HM_MIDDLE_TIMEOUT, &lpResult);

    if(LFS_SUCCESS == ret)
    {
        LPLFSIDCSTATUS lpStatus = (LPLFSIDCSTATUS)((LPLFSRESULT)lpResult)->buffer;
        if(lpStatus)
        {
            memcpy(&m_Status, lpStatus, sizeof(LFSIDCSTATUS));
            SetDeviceStatus(m_Status.device);
        }
        else
        {
            ret = -1;
            LOGERROR("%s, SP数据为空。", __FUNCTION__);
            Alarm("06000002");
        }
    }
    else
    {
        LOGERROR("%s, 获取状态失败错误码为：%d", __FUNCTION__,ret);
        Alarm("06000001");
    }

    ::LFSFreeResult(lpResult);
    return ret;

}

//执行LFS_INF_IDC_CAPABILITIES
int CMIDR::N_GetCaps()
{
    LPLFSRESULT lpResult = NULL;
    int ret = m_apiCtrl.LFSGetInfo(LFS_INF_IDC_CAPABILITIES, NULL, HM_MIDDLE_TIMEOUT, &lpResult);
    if(LFS_SUCCESS == ret)
    {
        LPLFSIDCCAPS lpCaps = (LPLFSIDCCAPS)((LPLFSRESULT)lpResult)->buffer;
        if(lpCaps)
        {
            memcpy(&m_Caps, lpCaps, sizeof(LFSIDCCAPS));
        }
        else
        {
            ret = -1;
            LOGERROR("%s, LFS_INF_IDC_CAPABILITIES返回数据为空。", __FUNCTION__);
            Alarm("06000002");
        }
    }
    else
    {
        LOGERROR("%s, 获取能力失败错误码为：%d。", __FUNCTION__,ret);
        Alarm("06000001");

    }

    ::LFSFreeResult(lpResult);

    return ret;
}

//执行LFS_CMD_IDC_EJECT_CARD
int CMIDR::N_EjectCard(LONG timeout)
{
    timeout = (-1==timeout) ? 0 : timeout;

    if(timeout < 0)
    {
        emit CardEjectFailed();
        LOGERROR("%s,参数错误。", __FUNCTION__);
        Alarm("06000002");
        return HM_PARAM_ERROR;
    }

    int ret = N_GetStatus();	//检测设备状态，是否有卡
    if(LFS_SUCCESS != ret)
    {
        N_InitStatus();
    }

    if(m_Status.media == LFS_IDC_MEDIANOTPRESENT)	//设备中没有卡，直接发送CardTaken事件
    {
        emit this->CardTaken();
        LOGINFO("%s,事件：CardTaken()。", __FUNCTION__);
        return 0;
    }

    //LFSIDCEJECTCARD param;
    //param.wEjectPosition = LFS_IDC_EXITPOSITION;

    ret=m_apiCtrl.LFSAsyncExecute(LFS_CMD_IDC_EJECT_CARD, NULL, timeout, &m_CurrRequestID);
    if(LFS_SUCCESS != ret)
    {
        emit CardEjectFailed();
        LOGINFO("%s,有错误产生,错误码：%d。", __FUNCTION__, ret);
        Alarm("06000001");
    }

    return ret;
}

//执行LFS_CMD_IDC_RETAIN_CARD
int CMIDR::N_RetainCard(LONG timeout)
{
    int ret = N_GetStatus();

    if(LFS_SUCCESS != ret)
    {
        N_InitStatus();
    }

    if(m_Status.media == LFS_IDC_MEDIANOTPRESENT)
    {
        emit this->CardTaken();
        LOGINFO("%s,事件：CardTaken()。", __FUNCTION__);
        return 0;
    }

    ret=m_apiCtrl.LFSAsyncExecute(LFS_CMD_IDC_RETAIN_CARD,NULL,timeout,&m_CurrRequestID);
    if(LFS_SUCCESS!=ret)
    {
        emit CardCaptureFailed();
        LOGINFO("%s,有错误产生,错误码：%d。",__FUNCTION__,ret);
        Alarm("06000001");
    }
    return ret;
}

//执行LFS_CMD_IDC_RESET
int CMIDR::N_Reset(WORD wResetIn,LONG timeout)
{
    int ret = m_apiCtrl.LFSAsyncExecute(LFS_CMD_IDC_RESET, &wResetIn, timeout, &m_CurrRequestID);
    if(LFS_SUCCESS != ret)
    {
        emit ResetFailed();
        LOGINFO("%s,有错误产生,错误码：%d。", __FUNCTION__, ret);
        Alarm("06000001");
    }

    return ret;
}

//执行LFS_CMD_IDC_Get_RAW_DATA
int CMIDR::N_ReadRawData(WORD readData, LONG timeout)
{
    timeout = (-1==timeout) ? 0 : timeout;

    if(timeout < 0)
    {
        LOGINFO("%s,参数错误。", __FUNCTION__);
        Alarm("06000002");
        emit CardAcceptFailed();
        return HM_PARAM_ERROR;
    }

    readData = LFS_IDC_TRACK1 | LFS_IDC_TRACK2 | LFS_IDC_TRACK3;

    int ret = m_apiCtrl.LFSAsyncExecute(LFS_CMD_IDC_READ_RAW_DATA, &readData, timeout, &m_CurrRequestID);
    if(LFS_SUCCESS != ret)
    {
        emit CardAcceptFailed();
        LOGINFO("%s,LFSAsyncExecute有错误产生,错误码：%d。", __FUNCTION__, ret);
        Alarm("06000002");
    }

    return ret;
}


void CMIDR::SetDeviceStatus(WORD wStatus)
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


void CMIDR::ProcessCardData(QString strInfo)
{
    LOGINFO("%s,strInfo = %s",__FUNCTION__,Q2S(strInfo));
    if(strInfo == "")
        return ;

    strInfo.replace('|',',');
    strInfo.remove(' ');

    VerStr vec = Token_All(strInfo,',');

    if(vec.size()!=10)
    {
        LOGINFO("%s,size = %d",__FUNCTION__,vec.size());
        return;
    }

    m_Name = vec[0];
    if(vec[1] == "0")
        m_Sex = "女";
    else if(vec[1] == "1")
        m_Sex = "男";
    else
        m_Sex = vec[1];

    std::map<QString,QString>::iterator it = m_NationMap.find(vec[2]);
    if(it!=m_NationMap.end())
        m_Nation = it->second;
    else
        m_Nation = vec[2];

    m_BirthDay = vec[3];
    m_Number = vec[4];
    m_Address = vec[5];
    m_Department = vec[6];
    m_StartTime = vec[7];
    m_Endtime = vec[8];
    m_PortraitFilePath = vec[9];
}

//得到以sep分割的所有子串
CMIDR::VerStr CMIDR::Token_All(const QString str,const char sep)
{
    VerStr verStr;
    int front=0,back=0;

    if(""==str)
        return verStr;

    do{
        front = str.indexOf(sep,back);
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


void CMIDR::InitMap()
{
    m_NationMap.insert(std::make_pair("01","汉族"));
    m_NationMap.insert(std::make_pair("02","蒙古族"));
    m_NationMap.insert(std::make_pair("03","回族"));
    m_NationMap.insert(std::make_pair("04","藏族"));
    m_NationMap.insert(std::make_pair("05","维吾尔族"));

    m_NationMap.insert(std::make_pair("06","苗族"));
    m_NationMap.insert(std::make_pair("07","彝族"));
    m_NationMap.insert(std::make_pair("08","壮族"));
    m_NationMap.insert(std::make_pair("09","布依族"));
    m_NationMap.insert(std::make_pair("10","朝鲜族"));

    m_NationMap.insert(std::make_pair("11","满族"));
    m_NationMap.insert(std::make_pair("12","侗族"));
    m_NationMap.insert(std::make_pair("13","瑶族"));
    m_NationMap.insert(std::make_pair("14","白族"));
    m_NationMap.insert(std::make_pair("15","土家族"));

    m_NationMap.insert(std::make_pair("16","哈尼族"));
    m_NationMap.insert(std::make_pair("17","哈萨克族"));
    m_NationMap.insert(std::make_pair("18","傣族"));
    m_NationMap.insert(std::make_pair("19","黎族"));
    m_NationMap.insert(std::make_pair("20","傈僳族"));

    m_NationMap.insert(std::make_pair("21","佤族"));
    m_NationMap.insert(std::make_pair("22","畲族"));
    m_NationMap.insert(std::make_pair("23","高山族"));
    m_NationMap.insert(std::make_pair("24","拉祜族"));
    m_NationMap.insert(std::make_pair("25","水族"));

    m_NationMap.insert(std::make_pair("26","东乡族"));
    m_NationMap.insert(std::make_pair("27","纳西族"));
    m_NationMap.insert(std::make_pair("28","景颇族"));
    m_NationMap.insert(std::make_pair("29","柯尔克孜族"));
    m_NationMap.insert(std::make_pair("30","土族"));

    m_NationMap.insert(std::make_pair("31","达斡尔族"));
    m_NationMap.insert(std::make_pair("32","仫佬族"));
    m_NationMap.insert(std::make_pair("33","羌族"));
    m_NationMap.insert(std::make_pair("34","布朗族"));
    m_NationMap.insert(std::make_pair("35","撒拉族"));

    m_NationMap.insert(std::make_pair("36","毛难族"));
    m_NationMap.insert(std::make_pair("37","仡佬族"));
    m_NationMap.insert(std::make_pair("38","锡伯族"));
    m_NationMap.insert(std::make_pair("39","阿昌族"));
    m_NationMap.insert(std::make_pair("40","普米族"));

    m_NationMap.insert(std::make_pair("41","塔吉克族"));
    m_NationMap.insert(std::make_pair("42","怒族"));
    m_NationMap.insert(std::make_pair("43","乌孜别克族"));
    m_NationMap.insert(std::make_pair("44","俄罗斯族"));
    m_NationMap.insert(std::make_pair("45","鄂温克族"));

    m_NationMap.insert(std::make_pair("46","崩龙族"));
    m_NationMap.insert(std::make_pair("47","保安族"));
    m_NationMap.insert(std::make_pair("48","裕固族"));
    m_NationMap.insert(std::make_pair("49","京族"));
    m_NationMap.insert(std::make_pair("50","塔塔尔族"));

    m_NationMap.insert(std::make_pair("51","独龙族"));
    m_NationMap.insert(std::make_pair("52","鄂伦春族"));
    m_NationMap.insert(std::make_pair("53","赫哲族"));
    m_NationMap.insert(std::make_pair("54","门巴族"));
    m_NationMap.insert(std::make_pair("55","珞巴族"));

    m_NationMap.insert(std::make_pair("56","基诺族"));
    m_NationMap.insert(std::make_pair("97","其他"));
    m_NationMap.insert(std::make_pair("98","外国血统"));
}

void CMIDR::CopyPicture()
{
    int iType = g_Helper.GetPrivateProfileInt("MIDR", "NameFormatType", 0, m_strConfigPath);
    QString strPath = g_Helper.GetPrivateProfileString("MIDR", "PicturePath", "", m_strConfigPath);

    if(1 == iType)      //使用农行标准
    {
        //SHCreateDirectoryEx(NULL, chPath, NULL);
        QDir dir;
        dir.mkpath(strPath);

        QString strTime = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
        QString strPathName;
        QString strPictureFormat("bmp");

        strPictureFormat = m_FrontFilePath.right(3);
        //strPathName.sprintf("%s\\%s_%s_front.%s", Q2S(strPath), Q2S(m_Number), Q2S(strTime), Q2S(strPictureFormat));    //
        strPathName = QString("%1\\%2_%3_front.%4")
                .arg(strPath).arg(m_Number).arg(strTime).arg(strPictureFormat);
        QFile::copy(m_FrontFilePath, strPathName);
        m_FrontFilePath = strPathName;

        strPictureFormat = m_BackFilePath.right(3);
        //strPathName.sprintf("%s\\%s_%s_back.%s", strPath, m_Number, strTime, strPictureFormat);
        strPathName = QString("%s\\%s_%s_back.%s")
                .arg(strPath).arg(m_Number).arg(strTime).arg(strPictureFormat);
        QFile::copy(m_BackFilePath, strPathName);
        m_BackFilePath = strPathName;

        strPictureFormat = m_PortraitFilePath.right(3);
        //strPathName.sprintf("%s\\%s_%s_head.%s", strPath, m_Number, strTime, strPictureFormat);
        strPathName = QString("%1\\%2_%3_head.%4")
                .arg(strPath).arg(m_Number).arg(strTime).arg(strPictureFormat);
        QFile::copy(m_PortraitFilePath, strPathName);
        m_PortraitFilePath = strPathName;
    }
}

void CMIDR::GetConfigPath()
{
    //char chFilePath[MAX_PATH]={0};
    //GetModuleFileName(NULL,chFilePath,MAX_PATH);
    QString strFilePath = QCoreApplication::applicationDirPath() + "/lib/";
    if(!strFilePath.isEmpty())
    {
        strFilePath += "/ini/Setup.ini";
        m_strConfigPath = strFilePath;
    }
    else
    {
        m_strConfigPath = "";
    }
}

void CMIDR::N_InitStatus()
{
    m_Status.device=LFS_IDC_DEVNODEVICE;
    m_Status.media=LFS_IDC_MEDIANOTPRESENT;
    m_Status.cards=0;
    m_Status.retain_bin=LFS_IDC_RETAINBINOK;
    m_Status.security=LFS_IDC_SECNOTREADY;
    m_Status.chip_power=LFS_IDC_CHIPUNKNOWN;
    SetDeviceStatus(m_Status.device);
}

void CMIDR::N_InitCaps()
{
    m_Caps.dev_class=LFS_SERVICE_CLASS_IDC;
    m_Caps.dev_type=LFS_IDC_TYPESWIPE;
    m_Caps.compound=FALSE;
    m_Caps.read_tracks=LFS_IDC_NOTSUPP;
    m_Caps.write_tracks=LFS_IDC_NOTSUPP;
    m_Caps.chip_protocols=LFS_IDC_NOTSUPP;
    m_Caps.cards=0;
    m_Caps.sec_type=LFS_IDC_NOTSUPP;
    m_Caps.power_on_option=LFS_IDC_NOACTION;
    m_Caps.power_off_option=LFS_IDC_NOACTION;
    m_Caps.flux_sensor_programmable=FALSE;
    m_Caps.read_write_access_following_eject=FALSE;
    m_Caps.write_mode=LFS_IDC_NOTSUPP;
    m_Caps.chip_power=LFS_IDC_NOTSUPP;
}

int CMIDR::Execute(int ID, LPVOID lpData)
{
    switch (ID)
    {
    case LFS_INF_IDC_STATUS:
        N_GetStatus();
        memcpy(lpData, Q2S(m_CurrDevStatus), m_CurrDevStatus.length());
        break;

    default:
        break;
    }
    return 0;
}

QObject *CreateInstance()
{
    return CMIDR::GetInstance();
}
