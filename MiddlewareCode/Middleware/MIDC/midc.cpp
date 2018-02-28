#include "midc.h"
#include "QDateTime"
#include "Helper/helper.h"
#include <AlarmLib.h>
#include "common_def.h"

#define HM_PARAM_ERROR -10000
#define PBOCTAGDATASIZE		1024
#define PBOCAPDUSIZE		2048

CMIDC* CMIDC::m_pInstance = NULL;

CMIDC::CMIDC(const QString& strDBusObjName):CWebInteractObj(strDBusObjName)
{
    m_PreDevStatus = m_CurrDevStatus ="OFFLINE";
    m_TrackXStatus[0]=m_TrackXStatus[1]=m_TrackXStatus[2]="NOTREAD";

    m_bOpened = false;
    memset(m_szLogicalName,0,MAX_PATH);

    LOGINFO("MIDC 模块加载！！！");
}

CMIDC::~CMIDC()
{
    LOGINFO("MIDC 模块卸载！！！");
}

CMIDC *CMIDC::GetInstance()
{
    if(m_pInstance == NULL)
    {
        m_pInstance = new CMIDC("MIDC");
    }

    return m_pInstance;
}

void  CMIDC::WR_OpenComplete(DWORD dwCommand, HRESULT hResult, LPVOID lpBuffer)
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
            LOGERROR("%s,注册事件失败,错误码 ret = %d",__FUNCTION__,ret);
            Alarm("01000000");
        }
    }
    else
    {
        emit OpenFailed();
        LOGERROR("%s,打开失败,错误码 ret = %d",__FUNCTION__,hResult);
        Alarm("01000000");
    }
}

void  CMIDC::WR_RegisterComplete(DWORD dwCommand, HRESULT hResult, LPVOID lpBuffer)
{
    if(LFS_SUCCESS != hResult)
    {
        N_Close();
        LOGERROR("%s,注册事件失败,错误码：hResult = %d",__FUNCTION__,hResult);
        Alarm("01000000");
        emit OpenFailed();
    }
    else
    {
        int ret = N_GetStatus();
        if (ret != LFS_SUCCESS)/////获取状态失败给初始值
        {
            N_InitStatus();
        }

        ret = N_GetCaps();
        if (ret != LFS_SUCCESS)/////获取能力失败给初始值
        {
            N_InitCaps();
        }
        m_bOpened = true;
        emit ConnectionOpened();
        LOGINFO("%s,事件：ConnectionOpened()",__FUNCTION__);
    }
}


// 收到关闭完成消息后调用该函数
void CMIDC::WR_CloseComplete(DWORD dwCommand, HRESULT hResult, LPVOID lpBuffer)
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
        Alarm("01000000");
    }
}

// 收到打开完成消息后调用该函数
void CMIDC::WR_GetinfoComplete(DWORD dwCommand, HRESULT hResult, LPVOID lpBuffer)
{

}

// 收到执行完成消息后调用该函数
void CMIDC::WR_ExecuteComplete(DWORD dwCommand, HRESULT hResult, LPVOID lpBuffer)
{
    LOGINFO("%s,IN",__FUNCTION__);

    //if (LFS_ERR_TIMEOUT == hResult )
    //{
    //	if(LFS_CMD_IDC_CHIP_IO!=dwCommand)//避免发送两次DeviceError（）事件
    //	{
    //		emit Timeout();
    //		LOGERROR("%s，命里那个超时，命令：%d,错误码：hResult = %d",__FUNCTION__,dwCommand,hResult);
    //		Alarm("01000001");
    //	}

    //	return;
    //}

    switch(dwCommand)
    {
    case LFS_CMD_IDC_EJECT_CARD:
    {
        if ( LFS_SUCCESS == hResult )
        {
            emit CardEjected();
            LOGINFO("%s,事件：CardEjected()",__FUNCTION__);
        }
        else if( LFS_ERR_CANCELED == hResult)
        {
            emit WaitCancelled();
            LOGINFO("%s,事件：WaitCancelled()",__FUNCTION__);
        }
        else
        {
            emit CardEjectFailed();
            LOGERROR("%s,事件：CardEjectFailed(),错误码：hResult = %d",__FUNCTION__,hResult);
            Alarm("01000001");
        }
    }
        break;

    case LFS_CMD_IDC_READ_RAW_DATA:
    {
        if( LFS_SUCCESS == hResult )
        {
            SaveTracks((LPLFSIDCCARDDATA *)((LPLFSRESULT)lpBuffer)->buffer);

            if(IsTracksDataOK())
            {
                if(1==m_wReadDataFlag)
                {
                    QString str1(m_chTracks[1]),str2(m_chTracks[2]);
                    if (str1 == "" || str2 == "")
                    {
                        CardAcceptFailed();
                        LOGINFO("%s,事件：CardAcceptFailed()",__FUNCTION__);
                    }
                    else
                    {
                        emit CardAccepted();
                        LOGINFO("%s,事件：CardAccepted()",__FUNCTION__);
                    }

                    str1.replace('=','D');
                    str2.replace('=','D');

                    //将2、3磁道数据写入临时存储区
                    m_DSTO.D_SetDataString(false,"TRACK2DATA",Q2S(str1));
                    m_DSTO.D_SetDataString(false,"TRACK3DATA",Q2S(str2));
                    //LOGINFO("%s^%s^%s",m_chTracks[0],m_chTracks[1],m_chTracks[2]);
                }
                else if(2==m_wReadDataFlag)
                {
                    //将2、3磁道数据写入临时存储区
                    m_DSTO.D_SetDataString(false,"TRACK2DATA",m_chTracks[1]);
                    m_DSTO.D_SetDataString(false,"TRACK3DATA",m_chTracks[2]);

                    emit CardRead();
                    LOGINFO("%s,事件：CardRead()",__FUNCTION__);
                    LOGINFO("%s^%s^%s",m_chTracks[0],m_chTracks[1],m_chTracks[2]);
                }
                else if(3==m_wReadDataFlag)
                {
                    emit ChipDataReceived(m_Token,m_chTracks[3]);
                    LOGINFO("%s,事件：ChipDataReceived(%s)",__FUNCTION__,Q2S(m_Token));
                }
            }
        }
        else if( LFS_ERR_CANCELED == hResult )
        {
            emit CardAcceptCancelled();
            LOGINFO("%s,事件：CardAcceptCancelled()",__FUNCTION__);
        }
        else
        {
            emit CardAcceptFailed();
            LOGERROR("%s,事件：CardAcceptFailed(),错误码：hResult = %d",__FUNCTION__,hResult);
            Alarm("01000001");
        }
    }
        break;

    case LFS_CMD_IDC_RESET:
    {
        if( LFS_SUCCESS == hResult )
        {
            emit ResetComplete();
            LOGINFO("%s,事件：ResetComplete()",__FUNCTION__);
        }
        else
        {
            emit ResetFailed();
            LOGERROR("%s,事件：ResetFailed(),错误码：hResult = %d",__FUNCTION__,hResult);
            Alarm("01000001");
        }
    }
        break;

    case LFS_CMD_IDC_CHIP_POWER:
    {
        if( LFS_SUCCESS == hResult )
        {
            emit ChipPowerComplete();
            LOGINFO("%s,事件：ChipPowerComplete()",__FUNCTION__);
        }
        else
        {
            emit ChipPowerFailed();
            LOGERROR("%s,事件：ChipPowerFailed(),错误码：hResult = %d",__FUNCTION__,hResult);
            Alarm("01000001");
        }
    }
        break;

    default:
        break;
    }

    LOGINFO("%s,OUT",__FUNCTION__);
}

// 收到执行事件后调用该函数
void CMIDC::WR_ExecuteEvent(DWORD dwEventID, HRESULT hResult, LPVOID lpBuffer)
{
    switch(dwEventID)
    {
    case LFS_EXEE_IDC_INVALIDTRACKDATA:
    case LFS_EXEE_IDC_INVALIDMEDIA:
        emit CardInvalid();
        LOGINFO("%s,事件：CardInvalid()",__FUNCTION__);
        break;
    case LFS_EXEE_IDC_MEDIAINSERTED:
        emit CardInserted();
        LOGINFO("%s,事件：CardInserted()",__FUNCTION__);
        break;
    case LFS_EXEE_IDC_MEDIARETAINED:
        emit CardCaptured();
        LOGINFO("%s,事件：CardCaptured()",__FUNCTION__);
        break;
    }
}

// 收到用户事件后调用该函数
void CMIDC::WR_UserEvent(DWORD dwEventID, HRESULT hResult, LPVOID lpBuffer)
{

}

// 收到服务事件后调用该函数
void CMIDC::WR_ServiceEvent(DWORD dwEventID, HRESULT hResult, LPVOID lpBuffer)
{
    switch(dwEventID)
    {
    case LFS_SRVE_IDC_MEDIAREMOVED:
    {
        emit CardTaken();
        LOGINFO("%s,事件：CardTaken()",__FUNCTION__);
    }
        break;
    }
}

// 收到系统事件后调用该函数
void CMIDC::WR_SystemEvent(DWORD dwEventID, HRESULT hResult, LPVOID lpBuffer)
{
    switch(dwEventID)
    {
    case LFS_SYSE_DEVICE_STATUS://设备状态改变
    {
        LPLFSDEVSTATUS lpDevStatus=(LPLFSDEVSTATUS)((LPLFSRESULT)lpBuffer)->buffer;
        if(lpDevStatus == NULL)
        {
            LOGERROR("%s,SP数据为空。",__FUNCTION__);
            Alarm("01000002");
            return;
        }

        SetDeviceStatus(lpDevStatus->state);
        if(m_CurrDevStatus!=m_PreDevStatus)
        {

            emit StatusChanged(m_szLogicalName,m_PreDevStatus,m_CurrDevStatus);
            LOGINFO("%s,事件：StatusChanged(\"StDeviceStatus\",\"%s\",\"%s\")",__FUNCTION__,Q2S(m_PreDevStatus),Q2S(m_CurrDevStatus));
        }
    }
        break;
    }
}

void CMIDC::SetServiceName(QString newVal)
{
    LOGINFO("%s,属性,newVal = %s",__FUNCTION__,Q2S(newVal));

    strcpy(m_szLogicalName,Q2S(newVal));
}

QString CMIDC::GetStDetailedDeviceStatus()
{
    // 改写  AAA
    int ret = N_GetStatus();
    if (ret != LFS_SUCCESS)/////获取状态失败给初始值
    {
        N_InitStatus();
    }

    LOGINFO("%s,Status = %s",__FUNCTION__,Q2S(m_CurrDevStatus));

    return m_CurrDevStatus;
}

// 回收箱容量
SHORT CMIDC::GetCpBinSize(void)
{
    return m_Caps.cards;
}

BOOL CMIDC::GetCpCanCapture(void)
{
    if (m_Caps.cards == 0)
    {
        return FALSE;
    }
    return TRUE;
}

BOOL CMIDC::GetCpCanEject(void)
{
    return TRUE;
}

BOOL CMIDC::GetCpCanReadTrack1(void)
{
    bool bTrack = ((m_Caps.read_tracks&LFS_IDC_TRACK1)==LFS_IDC_TRACK1) ? true:false;

    return bTrack;
}

BOOL CMIDC::GetCpCanReadTrack2(void)
{
    bool bTrack = ((m_Caps.read_tracks&LFS_IDC_TRACK2)==LFS_IDC_TRACK2) ? true:false;
    return bTrack;
}

BOOL CMIDC::GetCpCanReadTrack3(void)
{
    bool bTrack = ((m_Caps.read_tracks&LFS_IDC_TRACK3)==LFS_IDC_TRACK3) ? true:false;
    return bTrack;
}

BOOL CMIDC::GetCpCanReadTrackJIS2(void)
{
    // bool bTrack = ((m_Caps.read_tracks&LFS_IDC_FRONT_TRACK_1)==LFS_IDC_FRONT_TRACK_1) ? true:false;
    // return bTrack;
    return false;
}

BOOL CMIDC::GetCpCanWriteTrack1(void)
{
    bool bTrack = ((m_Caps.write_tracks&LFS_IDC_TRACK1)==LFS_IDC_TRACK1) ? true:false;

    return bTrack;
}

BOOL CMIDC::GetCpCanWriteTrack2(void)
{
    bool bTrack = ((m_Caps.write_tracks&LFS_IDC_TRACK2)==LFS_IDC_TRACK2) ? true:false;

    return bTrack;
}

BOOL CMIDC::GetCpCanWriteTrack3(void)
{
    bool bTrack = ((m_Caps.write_tracks&LFS_IDC_TRACK3)==LFS_IDC_TRACK3) ? true:false;

    return bTrack;
}

QString CMIDC::GetCpSecurity(void)
{

    return "NOSECURITY";
}

QString CMIDC::GetCpVariant(void)
{
    QString type("");
    switch(m_Caps.dev_type)
    {
    case LFS_IDC_TYPEMOTOR:
        type="MOTORISED";
        break;
    case LFS_IDC_TYPESWIPE:
        type="SWIPE";
        break;
    case LFS_IDC_TYPEDIP:
        type="DIP";
        break;
    default:
        type="UNKNOWN";
        break;
    }
    return type;
}

QString CMIDC::GetCpChipPower(void)
{
    QString strChipPower("");

    if((m_Caps.chip_power&LFS_IDC_NOTSUPP)==LFS_IDC_NOTSUPP)
        strChipPower +="NOTSUPPORTED,";
    if((m_Caps.chip_power&LFS_IDC_CHIPPOWERCOLD)==LFS_IDC_CHIPPOWERCOLD)
        strChipPower += "COLDRESET,";
    if((m_Caps.chip_power&LFS_IDC_CHIPPOWERWARM)==LFS_IDC_CHIPPOWERWARM)
        strChipPower += "WARMRESET,";
    if((m_Caps.chip_power&LFS_IDC_CHIPPOWEROFF)==LFS_IDC_CHIPPOWEROFF)
        strChipPower += "POWEROFF,";

    strChipPower=strChipPower.mid(0,strChipPower.length()-1);

    return strChipPower;
}

BOOL CMIDC::GetCpPowerSaveModeControl(void)
{

    return false;
}

BOOL CMIDC::GetCpSupportedGuidelights(void)
{

    return false;
}

SHORT CMIDC::GetStBinCount(void)
{
    int ret = N_GetStatus();
    if (ret != LFS_SUCCESS)/////获取状态失败给初始值
    {
        N_InitStatus();
    }
    return m_Status.cards;
}

QString CMIDC::GetStBinStatus(void)
{
    int ret = N_GetStatus();
    if (ret != LFS_SUCCESS)/////获取状态失败给初始值
    {
        N_InitStatus();
    }

    QString binStatus("");
    switch(m_Status.retain_bin)
    {
    case LFS_IDC_RETAINBINOK:
        binStatus="BINOK";
        break;
    case LFS_IDC_RETAINBINFULL:
        binStatus="BINHIGH";
        break;
    case LFS_IDC_RETAINBINHIGH:
        binStatus="BINFULL";
        break;
        //case LFS_IDC_RETAINBINMISSING:
        //    binStatus="NOBIN";
        //    break;
    default:
        binStatus="NOBIN";
        break;
    }
    return binStatus;
}

QString CMIDC::GetStMediaStatus(void)
{
    int ret = N_GetStatus();
    if (ret != LFS_SUCCESS)/////获取状态失败给初始值
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

    LOGINFO("%s,mediaStatus = %s,m_Status.fwMedia = %d",__FUNCTION__,Q2S(mediaStatus),m_Status.media);

    return mediaStatus;
}

QString CMIDC::GetStChipStatus(void)
{
    int ret = N_GetStatus();
    if (ret != LFS_SUCCESS)/////获取状态失败给初始值
    {
        N_InitStatus();
    }

    QString chipStatus("");
    switch(m_Status.chip_power)
    {
    case LFS_IDC_CHIPONLINE:
        chipStatus="ONLINE";
        break;
    case LFS_IDC_CHIPPOWEREDOFF:
        chipStatus="POWEROFF";
        break;
    case LFS_IDC_CHIPBUSY:
        chipStatus="BUSY";
        break;
    case LFS_IDC_CHIPHWERROR:
        chipStatus="ERROR";
        break;
    case LFS_IDC_CHIPUNKNOWN:
        chipStatus="UNKNOWN";
        break;
    case LFS_IDC_CHIPNODEVICE:
        chipStatus="NOCHIP";
        break;
    }
    return chipStatus;
}

QString CMIDC::GetTrack1(void)
{

    return QString(m_chTracks[0]);
}

QString CMIDC::GetTrack2(void)
{
    QString str(m_chTracks[1]);

    return str;
}

QString CMIDC::GetTrack3(void)
{
    QString str(m_chTracks[2]);

    return str;
}

QString CMIDC::GetTrack1Status(void)
{

    return m_TrackXStatus[0];
}

QString CMIDC::GetTrack2Status(void)
{

    return m_TrackXStatus[1];
}

QString CMIDC::GetTrack3Status(void)
{

    return m_TrackXStatus[2];
}

QString CMIDC::GetAnswerToReset(void)
{

    return QString(m_chTracks[3]);
}


QString CMIDC::GetCardNumber(void)
{
    LOGINFO("%s,属性，m_CardNumber = %s",__FUNCTION__,Q2S(m_CardNumber));

    return m_CardNumber;
}

QString CMIDC::GetCardSerial(void)
{
    LOGINFO("%s,属性，m_CardSerial = %s",__FUNCTION__,Q2S(m_CardSerial));

    return m_CardSerial;
}

QString CMIDC::GetStartTime(void)
{
    LOGINFO("%s,属性，m_StartTime = %s",__FUNCTION__,Q2S(m_StartTime));

    return m_StartTime;
}

QString CMIDC::GetEndTime(void)
{
    LOGINFO("%s,属性，m_EndTime = %s",__FUNCTION__,Q2S(m_EndTime));

    return m_EndTime;
}

QString CMIDC::GetName(void)
{
    LOGINFO("%s,属性，m_Name = %s",__FUNCTION__,Q2S(m_Name));

    return m_Name;
}

QString CMIDC::GetNameExpend(void)
{
    LOGINFO("%s,属性，m_NameExpand = %s",__FUNCTION__,Q2S(m_NameExpand));

    return m_NameExpand;
}

QString CMIDC::GetIDNumber(void)
{
    LOGINFO("%s,属性，m_IDNumber = %s",__FUNCTION__,Q2S(m_IDNumber));

    return m_IDNumber;
}

QString CMIDC::GetIType(void)
{
    LOGINFO("%s,属性，m_IType = %s",__FUNCTION__,Q2S(m_IType));

    return m_IType;
}


short CMIDC::OpenConnection()
{
    LOGINFO("%s，方法。",__FUNCTION__);
    if(m_bOpened)
    {
        emit ConnectionOpened();
        LOGINFO("%s,事件：ConnectionOpened（）",__FUNCTION__);
        return 0;
    }

    return N_Open();
}

short CMIDC::CloseConnection()
{
    LOGINFO("%s，方法。",__FUNCTION__);
    int ret = LFS_SUCCESS;
    if (m_bOpened)
    {
        ret=N_Close();
    }
    return ret;
}


SHORT CMIDC::AcceptAndReadTracks(QString TrackMap, LONG Timeout)
{
    LOGINFO("方法：%s(%s,%d)",__FUNCTION__,Q2S(TrackMap),Timeout);

    memset(m_chTracks,0,sizeof(m_chTracks));//数据清空

    m_wReadDataFlag=1;

    WORD wReadData = GetTracks(TrackMap);

    return N_ReadRawData(wReadData,Timeout);
}

SHORT CMIDC::AcceptAndReadAvailableTracks(QString TrackMap, LONG Timeout)
{
    LOGINFO("方法：%s(%s,%d)",__FUNCTION__,Q2S(TrackMap),Timeout);

    return AcceptAndReadTracks(TrackMap,Timeout);
}

SHORT CMIDC::CancelAccept(void)
{
    LOGINFO("方法：%s(void)",__FUNCTION__);

    return N_Cancel(0);
}

SHORT CMIDC::Eject(LONG Timeout)
{
    LOGINFO("方法：%s(%d)",__FUNCTION__,Timeout);

    int ret = N_EjectCard(Timeout);

    return ret;
}

SHORT CMIDC::Capture(LONG Timeout)//增加了超时参数 xlc 2017-1-12
{
    LOGINFO("方法：%s(void)",__FUNCTION__);

    int ret = this->N_RetainCard(Timeout);

    return ret;
}


SHORT CMIDC::AcceptAndChipInitialise(QString Token, LONG Timeout)
{
    LOGINFO("方法：%s(%s,%d)",__FUNCTION__,Q2S(Token),Timeout);

    memset(m_chTracks,0,sizeof(m_chTracks));//数据清空

    m_wReadDataFlag=3;
    m_Token = Token;

    int ret = N_ReadRawData(LFS_IDC_CHIP,Timeout);

    return ret;
}

SHORT CMIDC::ChipPower(QString Action)
{
    LOGINFO("方法：%s(%s)",__FUNCTION__,Q2S(Action));

    WORD wAction=0;

    if(Action == "COLDRESET")
        wAction=LFS_IDC_CHIPPOWERCOLD;
    else if(Action == "WARMRESET")
        wAction=LFS_IDC_CHIPPOWERWARM;
    else if(Action == "POWEROFF")
        wAction=LFS_IDC_CHIPPOWEROFF;
    else
    {
        ChipPowerFailed();
        LOGERROR("%s,参数错误。",__FUNCTION__);
        Alarm("01000002");
        return HM_PARAM_ERROR;
    }

    return N_ChipPower(wAction);
}

SHORT CMIDC::ResetBinCountSync(void)
{
    LOGINFO("方法：%s(void)",__FUNCTION__);

    int ret = N_ResetCount();

    return ret;
}

SHORT CMIDC::Reset(QString ResetAction,long TimeOut)
{
    LOGINFO("方法：%s(%s)",__FUNCTION__,Q2S(ResetAction));

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
        LOGERROR("%s,参数错误。",__FUNCTION__);
        Alarm("01000002");
        return HM_PARAM_ERROR;
    }

    return N_Reset(action,TimeOut);
}


//取消当前正在执行的命令
int CMIDC::N_Cancel(WORD requestID)
{
    int ret = m_apiCtrl.LFSCancelAsyncRequest(requestID);
    if(LFS_SUCCESS!=ret)
    {
        LOGERROR("%s,执行错误,错误码：ret = %d",__FUNCTION__,ret);
        Alarm("01000001");
        emit DeviceError();
    }

    return ret;
}


//执行LFS_INF_IDC_STATUS
int CMIDC::N_GetStatus()
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
            Alarm("01000002");
        }
    }
    else
    {
        LOGERROR("%s, 获取状态失败,错误码：ret = %d", __FUNCTION__,ret);
        Alarm("01000001");
    }

    ::LFSFreeResult(lpResult);
    return ret;
}

//执行LFS_INF_IDC_CAPABILITIES
int CMIDC::N_GetCaps()
{
    LPLFSRESULT lpResult = NULL;
    int ret = m_apiCtrl.LFSGetInfo(LFS_INF_IDC_CAPABILITIES, NULL, HM_MIDDLE_TIMEOUT, &lpResult);
    if(LFS_SUCCESS == ret)
    {
        LPLFSIDCCAPS lpCaps = (LPLFSIDCCAPS)((LPLFSRESULT)lpResult)->buffer;
        if(lpCaps)
            memcpy(&m_Caps, lpCaps, sizeof(LFSIDCCAPS));
        else
        {
            ret = -1;
            LOGERROR("%s, SP数据为空。", __FUNCTION__);
            Alarm("01000002");
        }
    }
    else
    {
        LOGERROR("%s, 获取能力失败,错误码：ret = %d", __FUNCTION__,ret);
        Alarm("01000001");
    }
    ::LFSFreeResult(lpResult);

    return ret;
}


//执行LFS_CMD_IDC_EJECT_CARD
int CMIDC::N_EjectCard(LONG timeout)
{
    timeout = (-1==timeout) ? 0 : timeout;

    if(timeout<0)
    {
        LOGERROR("%s,时间参数错误。设置了-1之外的负数,timeout = %d",__FUNCTION__,timeout);
        Alarm("01000002");
        emit CardEjectFailed();
        return HM_PARAM_ERROR;
    }

    int ret = N_GetStatus();//检测设备状态，是否有卡

    if(LFS_SUCCESS != ret)
    {
        N_InitStatus();
    }

    if(m_Status.media==LFS_IDC_MEDIANOTPRESENT)//设备中没有卡，直接发送CardTaken事件
    {
        LOGINFO("%s,事件：CardTaken()。",__FUNCTION__);
        emit CardTaken();
        return 0;
    }

    //   LFSIDCEJECTCARD param;
    //  param.wEjectPosition=LFS_IDC_EXITPOSITION; //使用3.0标准，没有该参数，于是注释掉

    ret=m_apiCtrl.LFSAsyncExecute(LFS_CMD_IDC_EJECT_CARD,NULL,timeout,&m_CurrRequestID);
    if(LFS_SUCCESS!=ret)
    {
        LOGERROR("%s,执行错误,错误码：ret = %d",__FUNCTION__,ret);
        Alarm("01000001");
        emit CardEjectFailed();//ErrorSend(ret);
    }

    return ret;
}

//执行LFS_CMD_IDC_RETAIN_CARD
int CMIDC::N_RetainCard(LONG timeout)
{
    int ret = N_GetStatus();

    if(LFS_SUCCESS != ret)
    {
        N_InitStatus();
    }

    if(m_Status.media==LFS_IDC_MEDIANOTPRESENT)
    {
        emit CardTaken();
        LOGINFO("%s,事件：CardTaken()。",__FUNCTION__);
        return 0;
    }

    ret=m_apiCtrl.LFSAsyncExecute(LFS_CMD_IDC_RETAIN_CARD,NULL,timeout,&m_CurrRequestID);
    if(LFS_SUCCESS!=ret)
    {
        emit CardCaptureFailed();//ErrorSend(ret);
        LOGERROR("%s,执行错误,错误码：ret = %d",__FUNCTION__,ret);
        Alarm("01000001");
    }
    return ret;
}

//执行LFS_CMD_IDC_RESET_COUNT
int CMIDC::N_ResetCount(LONG timeout)
{
    LPLFSRESULT lpResult = NULL;
    int ret = m_apiCtrl.LFSExecute(LFS_CMD_IDC_RESET_COUNT, NULL, timeout, &lpResult);
    if(LFS_SUCCESS != ret)
    {
        emit DeviceError();
        LOGERROR("%s, 执行错误,错误码：ret = %d", __FUNCTION__, ret);
        Alarm("01000001");
    }

    ::LFSFreeResult(lpResult);

    return ret;
}


//执行LFS_CMD_IDC_READ_RAW_DATA
int CMIDC::N_ReadRawData(WORD readData,LONG timeout)
{
    timeout = (-1==timeout) ? 0 : timeout;//

    if(timeout<0)
    {
        emit CardAcceptFailed();
        LOGERROR("%s,时间参数错误。设置了-1之外的负数,timeout = %d",__FUNCTION__,timeout);
        Alarm("01000002");
        return HM_PARAM_ERROR;
    }

    int ret=m_apiCtrl.LFSAsyncExecute(LFS_CMD_IDC_READ_RAW_DATA,&readData,timeout,&m_CurrRequestID);
    if(LFS_SUCCESS!=ret)
    {
        emit CardAcceptFailed();
        LOGERROR("%s,执行错误,错误码：ret = %d",__FUNCTION__,ret);
        Alarm("01000001");
    }

    return ret;
}


//执行LFS_CMD_IDC_RESET
int CMIDC::N_Reset(WORD wResetIn,LONG timeout)
{
    int ret = m_apiCtrl.LFSAsyncExecute(LFS_CMD_IDC_RESET, &wResetIn, timeout, &m_CurrRequestID);
    if(LFS_SUCCESS!=ret)
    {
        emit ResetFailed();
        LOGERROR("%s,执行错误,错误码：ret = %d",__FUNCTION__,ret);
        Alarm("01000001");
    }

    return ret;
}

int CMIDC::N_ChipPower(WORD wChipPower,LONG timeout)
{
    int ret = m_apiCtrl.LFSAsyncExecute(LFS_CMD_IDC_CHIP_POWER, &wChipPower, timeout, &m_CurrRequestID);
    if(LFS_SUCCESS!=ret)
    {
        ChipPowerFailed();
        LOGERROR("%s,执行错误,错误码：ret = %d",__FUNCTION__,ret);
        Alarm("01000001");
    }

    return ret;
}

int CMIDC::N_Open()
{

    int ret = m_apiCtrl.LFSAsyncOpen(m_szLogicalName, HM_MIDDLE_TIMEOUT, &m_CurrRequestID);

    if(LFS_SUCCESS!=ret)
    {
        LOGERROR("%s,打开失败！错误码：ret = %d",__FUNCTION__,ret);
        Alarm("01000000");
        emit OpenFailed();
    }

    return ret;
}

int CMIDC::N_Close()
{
    int ret = m_apiCtrl.LFSAsyncClose(&m_CurrRequestID);

    if(LFS_SUCCESS!=ret)
    {
        LOGERROR("%s,关闭失败！错误码：ret = %d",__FUNCTION__,ret);
        Alarm("01000000");
        emit CloseFailed();
    }

    return ret;
}


//得到以sep分割的所有子串
CMIDC::VerStr CMIDC::Token_All(const QString str,const char sep)
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

bool CMIDC::ASCIIToBCD(unsigned char *bcd, const unsigned char *asc, int nLen)
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

//功能：将BCD码转换为ASCII码
//如 0x00 0x4D转成字符串"004D"
bool CMIDC::BCDToASCII(LPBYTE asc, const LPBYTE bcd, int len)
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


void CMIDC::SetDeviceStatus(WORD wStatus)
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


WORD CMIDC::GetTracks(const QString strTracks)
{
    VerStr strTrackMap = Token_All(strTracks,',');
    WORD wReadData=0;

    for(VerStr::iterator it = strTrackMap.begin();it != strTrackMap.end();it++)
    {
        if("1"== *it)
            wReadData |= LFS_IDC_TRACK1;
        else if("2"==*it)
            wReadData |= LFS_IDC_TRACK2;
        else if("3"==*it)
            wReadData |= LFS_IDC_TRACK3;
        else if("CHIP"==*it)
            wReadData |= LFS_IDC_CHIP;
        //else if("JIS2"==*it)
        //    wReadData |= LFS_IDC_FRONT_TRACK_1;
        else if("SECURITY"==*it)
            wReadData |= LFS_IDC_SECURITY;
    }

    return wReadData;
}


void CMIDC::SaveTracks(LPLFSIDCCARDDATA *lppCardData)
{
    m_TrackXStatus[0]=m_TrackXStatus[1]=m_TrackXStatus[2]="NOTREAD";

    for(int i=0;NULL!=lppCardData[i];i++)
    {
        if(lppCardData[i]->data_source==LFS_IDC_TRACK1)
        {
            LOGINFO("%s,保存第1磁道数据，lpbData = %d",__FUNCTION__,lppCardData[i]->data);
            if(LFS_IDC_DATAOK==lppCardData[i]->status)
            {
                if(lppCardData[i]->data)
                    memcpy(m_chTracks[0],lppCardData[i]->data,lppCardData[i]->data_length+1);
                else
                    LOGINFO("%s,第一磁道数据为空",__FUNCTION__);
                m_TrackXStatus[0]="READ";
            }
            else if(LFS_IDC_DATAMISSING==lppCardData[i]->status)
                m_TrackXStatus[0]="BLANK";
            else
                m_TrackXStatus[0]="INVALID";
        }
        else if(lppCardData[i]->data_source==LFS_IDC_TRACK2)
        {
            LOGINFO("%s,保存第2磁道数据，lpbData = %d",__FUNCTION__,lppCardData[i]->data);
            if(LFS_IDC_DATAOK==lppCardData[i]->status)
            {
                if(lppCardData[i]->data)
                {
                    memcpy(m_chTracks[1],lppCardData[i]->data,lppCardData[i]->data_length+1);

                    QString strTrack2("%1");
                    strTrack2.arg(m_chTracks[1]);
                    int Index=strTrack2.indexOf("=");
                    const QChar IcFlag=strTrack2.at(Index+5);/////获取IC卡标志位  二磁道=号后第五位代表IC卡标志位 95599为纯磁条卡不是降级卡
                    if ( strTrack2.mid(0,5) != "95599" && (IcFlag == '2' || IcFlag == '6'))
                    {
                        m_DSTO.D_SetDataString(false,"CARDTYPE","CHIPCARD");
                        m_DSTO.D_SetDataLong(false,"FALLBACKCARD",1);////降级标致
                    }
                }
                else
                {
                    LOGINFO("%s,第二磁道数据为空",__FUNCTION__);
                }
                m_TrackXStatus[1]="READ";
            }
            else if(LFS_IDC_DATAMISSING==lppCardData[i]->status)
                m_TrackXStatus[1]="BLANK";
            else
                m_TrackXStatus[1]="INVALID";
        }
        else if(lppCardData[i]->data_source==LFS_IDC_TRACK3)
        {
            LOGINFO("%s,保存第3磁道数据，lpbData = %d",__FUNCTION__,lppCardData[i]->data);
            if(LFS_IDC_DATAOK==lppCardData[i]->status)
            {
                if(lppCardData[i]->data)
                    memcpy(m_chTracks[2],lppCardData[i]->data,lppCardData[i]->data_length+1);
                else
                    LOGINFO("%s,第三磁道数据为空",__FUNCTION__);
                m_TrackXStatus[2]="READ";
            }
            else if(LFS_IDC_DATAMISSING==lppCardData[i]->status)
                m_TrackXStatus[2]="BLANK";
            else
                m_TrackXStatus[2]="INVALID";
        }
        else if(lppCardData[i]->data_source==LFS_IDC_CHIP)
        {

            if(lppCardData[i]->data)
            {
                m_DSTO.D_SetDataString(false,"CARDTYPE","CHIPCARD");
                BCDToASCII((LPBYTE)m_chTracks[3],lppCardData[i]->data,2*lppCardData[i]->data_length);
            }

            else
            {
                LOGINFO("%s,CHIP为空",__FUNCTION__);
                memset(m_chTracks[3],0,sizeof(m_chTracks[3]));
            }
        }
    }
}


bool CMIDC::IsTracksDataOK()
{
    for(int i=0;i<3;i++)
    {
        if(m_TrackXStatus[i]=="INVALID")
            return false;
    }

    return true;
}



SHORT CMIDC::PBOCGetICInfo(LONG Timeout)
{
    LOGINFO("%s,IN",__FUNCTION__);

    //数据清空
    m_CardNumber = m_CardSerial = m_StartTime = m_EndTime = m_Name = m_NameExpand = m_IDNumber = m_IType = "";

    if(LFS_SUCCESS != N_SelectADF())
    {
        LOGERROR("%s,ADF选择失败",__FUNCTION__);
        Alarm("01000001");
        emit GetICInfoFailed();
        return -1;
    }

    // XiongJie start  20160928
    char chTMoney[32] = {0};
    char chTType[32] = {0};
    int tMoney = 0,tType = 0;
    m_DSTO.D_GetDataString(false,"TRANSAMOUNT",chTMoney);//
    m_DSTO.D_GetDataString(false,"TRANSACTIONTYPE",chTType);

    tMoney = atoi(chTMoney);
    if(tMoney == 0)
        tMoney == 10000;
    if("CWD" == QString(chTType))
    {
        tType = 0x01;
    }
    else if("DEP" == QString(chTType))
    {

        tType = 0x02;
    }
    else if("INQ" == QString(chTType))
    {
        tType = 0x30;
    }
    else
    {
        tType = 0x01;
    }

    LOGINFO("%s,tMoney = %d,tType = %d",__FUNCTION__,tMoney,tType);

    QString strAFL = N_PBOCGetProOption(tMoney, tType);// 交易初始化

    //QString strAFL = N_PBOCGetProOption(10000, 0);// 交易初始化

    // XiongJie end 20160928

    if("" == strAFL)
    {
        LOGERROR("%s,交易初始化失败.",__FUNCTION__);
        Alarm("01000001");
        emit GetICInfoFailed();
        return -1;
    }

    if("" == N_PBOCReadAppData(Q2S(strAFL)))// 读应用数据
    {
        LOGERROR("%s,读应用数据失败.",__FUNCTION__);
        Alarm("01000001");
        emit GetICInfoFailed();
        return -1;
    }

    if("" == N_PBOCGetICInfo())// 获取IC卡基本信息
    {
        LOGERROR("%s,获取IC卡基本信息失败.",__FUNCTION__);
        Alarm("01000001");
        emit GetICInfoFailed();
        return -1;
    }

    QString strTrack2Data = N_PBOCGetTrack2Data();//获取2磁道信息
    if("" == strTrack2Data)
    {
        LOGERROR("%s,获取2磁道信息失败",__FUNCTION__);
        Alarm("01000001");
        emit GetICInfoFailed();
        return -1;
    }

    /*LOGINFO("第二磁道数据：%s",strTrack2Data);

    strTrack2Data.Replace('D','d');

    LOGINFO("第二磁道数据1：%s",strTrack2Data);*/

    //写临时数据
    m_DSTO.D_SetDataString(false,"TRACK2DATA",Q2S(strTrack2Data));
    m_DSTO.D_SetDataString(false,"TRACK3DATA","");
    m_DSTO.D_SetDataString(false,"ICSERIALNUM",Q2S(m_CardSerial));

    emit GetICInfoCompleted();//发送完成事件
    LOGINFO("%s,事件：GetICInfoCompleted（）",__FUNCTION__);

    LOGINFO("%s,OUT",__FUNCTION__);

    return 0;
}


SHORT CMIDC::PBOCReadIcTLV(long Timeout)
{
    LOGINFO("%s,IN",__FUNCTION__);

    //char chFilePath[MAX_PATH]={0};
    //GetModuleFileName(NULL,chFilePath,MAX_PATH);
    //QString strFilePath(chFilePath);
    //strFilePath = strFilePath.Mid(0,strFilePath.ReverseFind('\\'));
    //strFilePath += "\\ini\\ICDPC.ini";

    //int ret = m_Icdpc.ICDPC_Init(strFilePath.GetBuffer(0));

    //LOGINFO("配置文件：%s",strFilePath);

    //if(LFS_SUCCESS != ret)
    //{
    //	LOGINFO("%s,初始化PBOC失败!",__FUNCTION__);
    //	emit DeviceError();
    //	return ret;
    //}

    //QString strAD = N_PBOCGetPSEAID();// 选择PSE文件

    //if(strAD == "")
    //{
    //	LOGINFO("%s,选择PSE文件失败.",__FUNCTION__);
    //	emit DeviceError();
    //	return -1;
    //}

    //if("" == N_PBOCSelectADF(strAD))//选择应用
    //{
    //	LOGINFO("%s,选择应用失败.",__FUNCTION__);
    //	emit DeviceError();
    //	return -1;
    //}


    /*  XiongJie start 20160928
    if(LFS_SUCCESS != N_SelectADF())
    {
        LOGINFO("%s,ADF选择失败",__FUNCTION__);
        emit DeviceError();
        return -1;
    }

    char chTMoney[32] = {0};
    char chTType[32] = {0};
    int tMoney = 0,tType = 0;
    m_DSTO.D_GetDataString(false,"TRANSAMOUNT",chTMoney);//
    m_DSTO.D_GetDataString(false,"TRANSACTIONTYPE",chTType);

    tMoney = atoi(chTMoney);
    if(tMoney == 0)
        tMoney == 10000;
    if("CWD" == QString(chTType))
    {
        tType = 0x01;
    }
    else if("DEP" == QString(chTType))
    {

        tType = 0x02;
    }
    else if("INQ" == QString(chTType))
    {
        tType = 0x30;
    }
    else
    {
        tType = 0x01;
    }

    LOGINFO("%s,tMoney = %d,tType = %d",__FUNCTION__,tMoney,tType);

    QString strAFL = N_PBOCGetProOption(tMoney, tType);// 交易初始化

    if("" == strAFL)
    {
        LOGINFO("%s,交易初始化失败.",__FUNCTION__);
        emit DeviceError();
        return -1;
    }

    if("" == N_PBOCReadAppData(strAFL))// 读应用数据
    {
        LOGINFO("%s,读应用数据失败.",__FUNCTION__);
        emit DeviceError();
        return -1;
    }

    */ //  XiongJie end 20160928

    QString str = N_PBOCInitForLoad();// 圈存圈提初始化
    if("" == str)
    {
        LOGERROR("%s,圈存圈提初始化失败.",__FUNCTION__);
        Alarm("01000001");
        emit ReadIcTLVFailed();
        return -1;
    }
    QString strPA(""),strAD(""),strAR("");
    int index1 = str.indexOf("^PA");
    int index2 = str.indexOf("^AD");
    int index3 = str.indexOf("^AR");
    strPA = str.mid(index1+3,index2);
    strAD = str.mid(index2+3,index3);
    strAR = str.mid(index3+3,str.length()-(index3+3));

    QString info = N_PBOCReadIcTLV(Q2S(strPA),Q2S(strAD),Q2S(strAR));// 获取55域上送数据
    QString otherStr("9F3501149F1E0831333031383238359F09020020");
    info += otherStr;

    //写到临时存储区
    m_DSTO.D_SetDataString(false,"ICCARDDATA",Q2S(info));


    emit ReadIcTLVCompleted(info);//发送完成事件
    LOGINFO("%s,事件：ReadIcTLVCompleted(\"%s\")",__FUNCTION__,Q2S(info));

    LOGINFO("%s,OUT",__FUNCTION__);

    return 0;
}


// 选择PSE文件
QString CMIDC::N_PBOCGetPSEAID(LPCTSTR aIn)
{
    LOGINFO("%s,IN,选择PSE文件",__FUNCTION__);

    QString sRet = "";

    CHAR chCApdu[PBOCAPDUSIZE] = {0};
    CHAR chRApdu[PBOCAPDUSIZE] = {0};
    WORD wP1 = 1;
    CHAR chAD[PBOCTAGDATASIZE] = {0};
    CHAR chLB[PBOCTAGDATASIZE] = {0};
    CHAR chPT[PBOCTAGDATASIZE] = {0};
    CHAR chOut[PBOCTAGDATASIZE] = {0};

    // 组合选择PSE文件的SELECT命令
    m_Icdpc.ICDPC_AssGetPSEAID_Capdu((CHAR*)aIn, chCApdu);

    int iRet = N_ExecuteChipIO(chCApdu, chRApdu);
    if (iRet)
    {
        LOGERROR("%s,组合选择PSE文件的SELECT命令 失败,错误码：iRet = %d",__FUNCTION__,iRet);
        return "";
    }

    // 进行报文解析
    iRet = m_Icdpc.ICDPC_AnalysisR_Apdu(ICDPC_SELECT_PSE, chRApdu);
    if (iRet)
    {
        LOGERROR("%s,进行报文解析 失败 ,错误码：iRet = %d",__FUNCTION__,iRet);
        return "";
    }

    // 组织Read Record命令, 发送

    memset(chCApdu, 0, sizeof(chCApdu));
    memset(chRApdu, 0, sizeof(chRApdu));

    // 组合应用选择中Read Record命令的C_APDU命令
    m_Icdpc.ICDPC_AssSelectReadRecord_Capdu(ICDPC_SELECT_PSE, wP1, chCApdu);

    // 往读卡器发送命令
    iRet = N_ExecuteChipIO(chCApdu, chRApdu);
    if (iRet)
    {
        LOGERROR("%s,往读卡器发送命令 失败,错误码：iRet = %d",__FUNCTION__,iRet);
        return "";
    }

    //进行报文Read Record的R_APDU解析
    iRet = m_Icdpc.ICDPC_AnalysisR_Apdu(ICDPC_READ_RECORD, chRApdu);
    if (iRet)
    {
        LOGERROR("%s,进行报文Read Record的R_APDU解析 失败,错误码：iRet = %d",__FUNCTION__,iRet);
        return "";
    }

    iRet = m_Icdpc.ICDPC_GetPSEAID(chOut);

    CHAR* temp = strtok(chOut, "&");
    if (temp)
    {
        memcpy(chAD, temp, strlen(temp));
    }

    if (!strcmp(chAD, "A000000333010101"))
    {
        m_cardType = 1;
    }
    else if (!strcmp(chAD, "A000000333010102"))
    {
        m_cardType = 2;
    }

    LOGINFO("%s,OUT",__FUNCTION__);

    return chAD;
}


// 执行CHIP IO命令
SHORT CMIDC::N_ExecuteChipIO(const CHAR* chCApdu, CHAR* chRApdu)
{
    LOGINFO("chCApdu = %s",chCApdu);

    REQUESTID id = 0;
    WORD wdataStatus = 0x00;
    LFSIDCCHIPIO wIdcChipIOSelect = {0};
    BYTE byCApdu[PBOCAPDUSIZE] = {0x00};

    ASCIIToBCD(byCApdu,(LPBYTE)chCApdu,strlen(chCApdu));

    wIdcChipIOSelect.chip_protocol = LFS_IDC_CHIPT1;
    wIdcChipIOSelect.chip_data_length = strlen(chCApdu)/2;
    wIdcChipIOSelect.chip_data = (BYTE*)byCApdu;

    LPLFSRESULT lpResult = NULL;
    HRESULT hr = m_apiCtrl.LFSExecute(LFS_CMD_IDC_CHIP_IO, (LPVOID)&wIdcChipIOSelect, HM_MIDDLE_TIMEOUT, &lpResult);

    if(hr != LFS_SUCCESS)
    {
        LOGERROR("%s,LFS_CMD_IDC_CHIP_IO命令执行失败,错误码：hr = %d",__FUNCTION__,hr);
        return -1;
    }
    else
    {
        LPLFSIDCCHIPIO lpChipIoOut = (LPLFSIDCCHIPIO)((LPLFSRESULT)lpResult)->buffer;

        if(lpChipIoOut)
        {
            unsigned char buf[1024] = {0};
            if(lpChipIoOut->chip_data)
            {
                BCDToASCII((LPBYTE)buf,lpChipIoOut->chip_data,2*lpChipIoOut->chip_data_length);
                m_szChipOut = US2Q(buf);
            }
        }
    }

    memcpy(chRApdu, Q2US(m_szChipOut), m_szChipOut.length());
    LOGINFO("chRApdu = %s",chRApdu);

    // 查询返回状态
    m_Icdpc.ICDPC_AnalysisR_ApduStatus(chRApdu, &wdataStatus);
    if (wdataStatus != 0x9000)
    {
        return -11;
    }

    return 0;
}


// 应用选择
QString CMIDC::N_PBOCSelectADF(LPCTSTR aIn)
{
    LOGINFO("%s,IN,应用选择",__FUNCTION__);

    QString sRet = "";
    CHAR chCApdu[PBOCAPDUSIZE] = {0};
    CHAR chRApdu[PBOCAPDUSIZE] = {0};

    // 组合C_APDU命令
    m_Icdpc.ICDPC_AssSelectADF_Capdu((CHAR*)aIn, chCApdu);

    // 发送C_APDU命令给读卡器，返回R_APDU报文
    int iRet = N_ExecuteChipIO(chCApdu, chRApdu);
    if (iRet)
    {
        LOGERROR("%s,N_ExecuteChipIO命令执行失败,错误码：iRet = %d",__FUNCTION__,iRet);
        Alarm("01000001");
        return "";
    }

    // 进行报文解析
    iRet = m_Icdpc.ICDPC_AnalysisR_Apdu(ICDPC_SELECT_ADF, chRApdu);
    if (iRet)
    {
        LOGERROR("%s,m_Icdpc.ICDPC_AnalysisR_Apdu命令执行失败,错误码：iRet = %d",__FUNCTION__,iRet);
        Alarm("01000001");
        return "";
    }

    LOGINFO("%s,OUT",__FUNCTION__);

    return "0";
}


// 交易初始化
QString CMIDC::N_PBOCGetProOption(LONG aTradeMoney, SHORT aTradeType)
{
    LOGINFO("%s,IN,交易初始化",__FUNCTION__);

    QString sRet = "";
    CHAR chCApdu[PBOCAPDUSIZE] = {0};
    CHAR chRApdu[PBOCAPDUSIZE] = {0};
    CHAR chAFL[PBOCTAGDATASIZE] = {0};


    QDateTime EtTime = QDateTime::currentDateTime();   // 获取当前时间
    QString csTime = EtTime.toString("yyyyMMdd");
    csTime = csTime.right(6);

    int aTime = atoi(Q2S(csTime));

    int iRet = m_Icdpc.ICDPC_AssGetProOption_Capdu(156, aTradeMoney, aTradeType, aTime, chCApdu);

    // 发送C_APDU命令给读卡器，返回R_APDU报文
    iRet = N_ExecuteChipIO(chCApdu, chRApdu);
    if (iRet)
    {
        LOGERROR("%s,N_ExecuteChipIO命令执行失败,错误码：iRet = %d",__FUNCTION__,iRet);
        return "";
    }

    // 如果状态为0x9000，则进行报文解析
    iRet = m_Icdpc.ICDPC_AnalysisR_Apdu(ICDPC_GET_PROCESSING_OPTIONS, chRApdu);
    if (iRet)
    {
        LOGERROR("%s,ICDPC_AnalysisR_Apdu命令执行失败,错误码：iRet = %d",__FUNCTION__,iRet);
        return "";
    }

    iRet = m_Icdpc.ICDPC_GetProOption(chAFL);
    if (iRet)
    {
        LOGERROR("%s,m_Icdpc.ICDPC_GetProOption命令执行失败,错误码：iRet = %d",__FUNCTION__,iRet);
        return "";
    }

    LOGINFO("%s,OUT",__FUNCTION__);

    return chAFL;
}


// 读应用数据
QString CMIDC::N_PBOCReadAppData(LPCTSTR aAFL)
{
    LOGINFO("%s,IN,读应用数据",__FUNCTION__);

    QString sRet = "";
    CHAR chCAPDUList[DATALENGTH] = {0};
    CHAR chRAPDU[1024] = {0};
    WORD wCommandCount = 0;

    if((strlen(aAFL) %2) != 0)
    {
        return "";
    }

    int iRet = m_Icdpc.ICDPC_AssReadAppData_Capdu((CHAR*)aAFL, chCAPDUList);

    CHAR* chCAPDU = nullptr;
    chCAPDU = strtok(chCAPDUList, "&");

    while(chCAPDU != nullptr)
    {
        memset(chRAPDU, 0, sizeof(chRAPDU));

        // 发送C_APDU命令给读卡器，返回R_APDU报文
        iRet = N_ExecuteChipIO(chCAPDU, chRAPDU);

        //xuzhe start 0420 modify
        //if (iRet)
        //{
        //	//continue;
        //	//return "";//执行错误就返回，不再继续执行
        //}
        //xuzhe end 0420 modify

        //如果状态为0x9000，则进行报文解析
        m_Icdpc.ICDPC_AnalysisR_Apdu(ICDPC_READ_RECORD, chRAPDU);

        chCAPDU = strtok(nullptr, "&");
    }

    LOGINFO("%s,OUT",__FUNCTION__);

    return "0";
}


// 圈存圈提初始化
QString CMIDC::N_PBOCInitForLoad()
{
    LOGINFO("%s,IN,圈存圈提初始化",__FUNCTION__);

    QString sRet = "";
    CHAR chCardDataExc[DATALENGTH] = {0};
    CHAR chForARQC[DATALENGTH] = {0};
    CHAR chARQC[DATALENGTH] = {0};
    BOOL bSupportCDA = FALSE;
    BOOL bSupportDDA = FALSE;
    BOOL bSupportSDA = FALSE;
    WORD wdataStatus = 0x00;

    // 脱机数据认证
    int iRet = m_Icdpc.ICDPC_CheckDataAuthentication(&bSupportCDA, &bSupportDDA, &bSupportSDA);    // 检测是否支持CDA、DDA、SDA数据认证
    if (!bSupportCDA && !bSupportDDA && !bSupportSDA )
    {

    }

    // 获取ATC数据
    CHAR chCAPDU[DATALENGTH] = {0};
    CHAR chRAPDU[DATALENGTH] = {0};

    iRet = m_Icdpc.ICDPC_AssGetATCData_Capdu(chCAPDU);

    // 发送C_APDU命令给读卡器，返回R_APDU报文
    iRet = N_ExecuteChipIO(chCAPDU, chRAPDU);
    if (iRet)
    {
        LOGERROR("%s,N_ExecuteChipIO命令执行失败,错误码：iRet = %d",__FUNCTION__,iRet);
        return "";
    }

    iRet = m_Icdpc.ICDPC_AnalysisR_Apdu(ICDPC_GET_DATA, chRAPDU);
    if (iRet)
    {
        LOGERROR("%s,m_Icdpc.ICDPC_AnalysisR_Apdu命令执行失败,错误码：iRet = %d",__FUNCTION__,iRet);
        return "";
    }

    // 终端行为分析

    memset(chCAPDU, 0, sizeof(chCAPDU));
    memset(chRAPDU, 0, sizeof(chRAPDU));

    // Generate AC命令
    iRet = m_Icdpc.ICDPC_TeminalActiveAnaiysis(chCAPDU);

    // 发送C_APDU命令给读卡器，返回R_APDU报文
    iRet = N_ExecuteChipIO(chCAPDU, chRAPDU);
    if (iRet)
    {
        LOGERROR("%s,N_ExecuteChipIO命令执行失败,错误码：iRet = %d",__FUNCTION__,iRet);
        return "";
    }

    iRet = m_Icdpc.ICDPC_AnalysisR_Apdu(ICDPC_GENERATE_AC, chRAPDU);
    if (iRet)
    {
        LOGERROR("%s,m_Icdpc.ICDPC_AnalysisR_Apdu命令执行失败,错误码：iRet = %d",__FUNCTION__,iRet);
        return "";
    }

    // 获取圈存圈提初始化所需数据
    iRet = m_Icdpc.ICDPC_InitForLoad(chCardDataExc, chForARQC, chARQC);
    if (iRet)
    {
        LOGERROR("%s,m_Icdpc.ICDPC_InitForLoad命令执行失败,错误码：iRet = %d",__FUNCTION__,iRet);
        return "";
    }

    sRet = "^RT0^PA%1^AD%2^AR%3";
    sRet = sRet.arg(chCardDataExc).arg(chForARQC).arg(chARQC);

    LOGINFO("%s,OUT",__FUNCTION__);

    return sRet;
}


// 获取55域上送数据
QString CMIDC::N_PBOCReadIcTLV(LPCTSTR aInPA, LPCTSTR aInAD, LPCTSTR aInAR)
{
    LOGINFO("%s,IN,获取55域上送数据",__FUNCTION__);

    CHAR ch55Data[PBOCTAGDATASIZE] = {0};

    m_Icdpc.ICDPC_ReadIcTLV((CHAR*)aInPA, (CHAR*)aInAD, (CHAR*)aInAR, ch55Data);

    LOGINFO("%s,OUT",__FUNCTION__);

    return ch55Data;
}


// 获取IC卡基本信息
QString CMIDC::N_PBOCGetICInfo()
{
    LOGINFO("%s,IN,获取IC卡基本信息",__FUNCTION__);

    QString sRet = "";

    CHAR chPA[21] = {0};	// 卡号
    CHAR chPS[3] = {0};	// 卡号序号

    int iRet = m_Icdpc.ICDPC_InfoGet(0x5A, chPA);
    if (iRet)
    {
        LOGERROR("%s,m_Icdpc.ICDPC_InfoGet 1命令执行失败,错误码：iRet = %d",__FUNCTION__,iRet);
        return "";
    }

    int len = strlen(chPA);
    if (chPA[len-1] == 'F')
    {
        chPA[len-1] = 0;
    }

    iRet = m_Icdpc.ICDPC_InfoGet(0x5F34, chPS);
    if (iRet)
    {
        LOGERROR("%s,m_Icdpc.ICDPC_InfoGet 2命令执行失败,错误码：iRet = %d",__FUNCTION__,iRet);
        return "";
    }

    m_CardNumber = QString(chPA);
    m_CardSerial = QString(chPS);


    CHAR chFD[10] = {0};	// 生效日期
    CHAR chED[10] = {0};	// 失效日期

    iRet = m_Icdpc.ICDPC_InfoGet(0x5F25, chFD);
    if (iRet)
    {
        LOGERROR("%s,m_Icdpc.ICDPC_InfoGet 3命令执行失败,错误码：iRet = %d",__FUNCTION__,iRet);
        return "^RT-15^FD^ED";
    }

    iRet = m_Icdpc.ICDPC_InfoGet(0x5F24, chED);
    if (iRet)
    {
        LOGERROR("%s,m_Icdpc.ICDPC_InfoGet 4命令执行失败,错误码：iRet = %d",__FUNCTION__,iRet);
        return "^RT-15^FD^ED";
    }

    m_StartTime = QString(chFD);
    m_EndTime = QString(chED);


    CHAR chNM[PBOCTAGDATASIZE] = {0};	// 姓名
    CHAR chNE[PBOCTAGDATASIZE] = {0};	// 姓名扩展
    CHAR chID[PBOCTAGDATASIZE] = {0};	// 身份证号
    CHAR chIT[PBOCTAGDATASIZE] = {0};	// 证件类型

    m_Icdpc.ICDPC_InfoGet(0x5F20, chNM);
    m_Icdpc.ICDPC_InfoGet(0x9F0B, chNE);

    m_Icdpc.ICDPC_InfoGet(0x9F61, chID);
    m_Icdpc.ICDPC_InfoGet(0x9F62, chIT);

    m_Name = QString(chNM);
    m_NameExpand = QString(chNE);
    m_IDNumber = QString(chID);
    m_IType = QString(chIT);

    LOGINFO("%s,OUT",__FUNCTION__);

    return "0";
}


// 读取二磁道信息
QString CMIDC::N_PBOCGetTrack2Data()
{
    LOGINFO("%s,IN,读取二磁道信息",__FUNCTION__);

    QString sRet = "";
    CHAR chTrack2Data[PBOCTAGDATASIZE] = {0};

    //Add By LeeHai 2015-12-23
    CHAR chRAPDU[1024] = {0};
    CHAR chCAPDU[1024] = {0};

    strcpy(chCAPDU, "00B2010C00");

    // 发送C_APDU命令给读卡器，返回R_APDU报文
    int iRet = N_ExecuteChipIO(chCAPDU, chRAPDU);
    if (0 != iRet)
    {
        LOGERROR("%s, N_ExecuteChipIO命令执行失败, 错误码：iRet = %d", __FUNCTION__, iRet);
        return "";
    }

    // 如果状态为0x9000，则进行报文解析
    m_Icdpc.ICDPC_AnalysisR_Apdu(ICDPC_READ_RECORD, chRAPDU);
    //End 2015-12-23

    iRet = m_Icdpc.ICDPC_InfoGet(0x57, chTrack2Data);
    if (iRet)
    {
        LOGERROR("%s,m_Icdpc.ICDPC_InfoGet命令执行失败,错误码：iRet = %d",__FUNCTION__,iRet);
        return "";
    }

    int len = strlen(chTrack2Data);
    if (chTrack2Data[len-1] == 'F')
    {
        chTrack2Data[len-1] = 0;
    }

    sRet = QString(chTrack2Data);

    LOGINFO("%s,OUT",__FUNCTION__);

    return sRet;
}


int CMIDC::N_PBOCAidList(QString configPath)
{
    LOGINFO("%s,IN,AID列表选择",__FUNCTION__);

    QString strAid = "";
    CHAR c_Apdu[PBOCAPDUSIZE] = {0};
    CHAR r_Apdu[PBOCAPDUSIZE] = {0};
    int list_Count = 0;
    int iRet = -1;

    list_Count = g_Helper.GetPrivateProfileInt("AID_LIST_COUNT","Count",0,configPath);

    QString strApp("");
    for(int i = 0;i<list_Count;i++)
    {
        strAid = "";
        memset(c_Apdu,0,sizeof(c_Apdu));
        memset(r_Apdu,0,sizeof(r_Apdu));

        strApp = "AIDLIST%1";
        strApp = strApp.arg(i,2,10,QChar('0'));
        strAid = g_Helper.GetPrivateProfileString(strApp,"DFName","",configPath);

        m_Icdpc.ICDPC_AidSelect_Capdu(Q2S(strAid),c_Apdu);

        iRet = N_ExecuteChipIO(c_Apdu,r_Apdu);

        if(iRet != 0)
            continue;
        else
        {
            m_Icdpc.ICDPC_AnalysisR_Apdu(ICDPC_SELECT_DDF,r_Apdu);
            break;
        }
    }

    LOGINFO("%s,OUT,iRet = %d",__FUNCTION__,iRet);

    return iRet;

}


int CMIDC::N_SelectADF()
{
    GetPBOCConfigPath();

    LOGINFO("PBOCConfigPath = %s",Q2S(m_PBOCConfigPath));

    int ret = m_Icdpc.ICDPC_Init(Q2S(m_PBOCConfigPath));
    if(LFS_SUCCESS != ret)
    {
        LOGERROR("%s,初始化PBOC失败!错误码：ret = %d",__FUNCTION__,ret);
        return ret;
    }

    //return N_PBOCAidList(m_PBOCConfigPath);

    QString strAD = N_PBOCGetPSEAID();// 选择PSE文件

    if(strAD == "")
    {
        LOGERROR("%s,选择PSE文件失败。开始选择Aid列表。",__FUNCTION__);

        ret = N_PBOCAidList(m_PBOCConfigPath);
        if(ret != 0)
        {
            LOGERROR("%s,Aid列表选择失败1,错误码：ret = %d",__FUNCTION__,ret);
        }
    }
    else
    {
        if("" == N_PBOCSelectADF(Q2S(strAD)))//选择应用
        {
            LOGERROR("%s,选择应用失败。开始选择Aid列表。",__FUNCTION__);

            ret = N_PBOCAidList(m_PBOCConfigPath);
            if(ret != 0)
            {
                LOGERROR("%s,Aid列表选择失败2,错误码：ret = %d",__FUNCTION__,ret);
            }
        }
    }

    return ret;
}



void CMIDC::GetPBOCConfigPath()
{
    QString strFilePath = g_Helper.GetModuleFileName((void*)&CMIDC::GetInstance);
    if(!strFilePath.isEmpty())
    {
        strFilePath = strFilePath.mid(0,strFilePath.lastIndexOf('/'));
        strFilePath += "/ini/ICDPC.ini";
        m_PBOCConfigPath = strFilePath;
    }
    else
    {
        m_PBOCConfigPath = "";
    }
}


void CMIDC::N_InitStatus()
{
    m_Status.device=LFS_IDC_DEVNODEVICE;
    m_Status.media=LFS_IDC_MEDIANOTPRESENT;
    m_Status.cards=0;
    m_Status.retain_bin=LFS_IDC_RETAINBINOK;
    m_Status.security=LFS_IDC_SECNOTREADY;
    m_Status.chip_power=LFS_IDC_CHIPUNKNOWN;
    SetDeviceStatus(m_Status.device);
}

void CMIDC::N_InitCaps()
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

int CMIDC::Execute(int ID, LPVOID lpData)
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
    return CMIDC::GetInstance();
}
