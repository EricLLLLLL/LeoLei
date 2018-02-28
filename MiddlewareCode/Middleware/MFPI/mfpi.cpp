// Last updated at 2015/12/8 8:44:03

#include "stdafx.h"
#include "mfpi.h"
#include "common_def.h"

#define HM_PARAM_ERROR -10000

CMFPI* CMFPI::m_pInstance = NULL;

CMFPI::CMFPI(const QString& strDBusObjName):CWebInteractObj(strDBusObjName)
{
	m_bIdentify = false;
	m_Tracks = 0;
	m_PreDevStatus = m_CurrDevStatus ="OFFLINE";
	m_Fpiprovider = 2;

	GetConfigPath();//得到配置文件的路径
	m_bOpened = false;
	memset(m_szLogicalName,0,MAX_PATH);

	LOGINFO("MFPI 模块加载！");
}

CMFPI::~CMFPI()
{
    LOGINFO("MFPI 模块卸载！");
}

CMFPI *CMFPI::GetInstance()
{
    if(m_pInstance == NULL)
    {
        m_pInstance = new CMFPI("MFPI");
    }
    return m_pInstance;
}


// 收到打开完成消息后调用该函数
void  CMFPI::WR_OpenComplete(DWORD dwCommand, HRESULT hResult, LPVOID lpBuffer)
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
			LOGERROR("%s,注册事件失败,错误码:ret = %d",__FUNCTION__,ret);
            Alarm("07000000");
		}
	}
	else
	{
        emit OpenFailed();
		LOGERROR("%s,打开失败，错误码:hResult = %d",__FUNCTION__,hResult);
        Alarm("07000000");
	}
}

void  CMFPI::WR_RegisterComplete(DWORD dwCommand, HRESULT hResult, LPVOID lpBuffer)
{
    if(LFS_SUCCESS != hResult)
	{
		N_Close();		
        emit OpenFailed();
		LOGERROR("%s,注册事件失败,错误码:hResult = %d",__FUNCTION__,hResult);
        Alarm("07000000");
	}
	else
	{
		m_bOpened = true;
        emit ConnectionOpened();
		LOGINFO("%s,事件：ConnectionOpened()",__FUNCTION__);
	}
}

// 收到关闭完成消息后调用该函数
void CMFPI::WR_CloseComplete(DWORD dwCommand, HRESULT hResult, LPVOID lpBuffer)
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
        Alarm("07000000");
	}
}

// 收到打开完成消息后调用该函数
void CMFPI::WR_GetinfoComplete(DWORD dwCommand, HRESULT hResult, LPVOID lpBuffer)
{
	
}

// 收到执行完成消息后调用该函数
void CMFPI::WR_ExecuteComplete(DWORD dwCommand, HRESULT hResult, LPVOID lpBuffer)
{
	LOGINFO("%s,IN",__FUNCTION__);

	switch(dwCommand)
	{
    case LFS_CMD_IDC_READ_RAW_DATA:
		{
			m_hResult = N_AfterReadRawData(hResult,lpBuffer);
		}
		break;
    case LFS_CMD_IDC_RESET:
		{
            if(LFS_SUCCESS==hResult)
			{
                emit ResetComplete();
				LOGINFO("%s,事件：ResetComplete()。",__FUNCTION__);
			}
			else
			{
                emit ResetFailed();
				LOGERROR("%s,有错误产生，错误码：hResult = %d",__FUNCTION__,hResult);
                Alarm("07000000");
			}
		}
		break;
	}
	LOGINFO("%s,OUT",__FUNCTION__);
}

// 收到执行事件后调用该函数
void CMFPI::WR_ExecuteEvent(DWORD dwEventID, HRESULT hResult, LPVOID lpBuffer)
{

}

// 收到用户事件后调用该函数
void CMFPI::WR_UserEvent(DWORD dwEventID, HRESULT hResult, LPVOID lpBuffer)
{

}

// 收到服务事件后调用该函数
void CMFPI::WR_ServiceEvent(DWORD dwEventID, HRESULT hResult, LPVOID lpBuffer)
{
	LOGINFO("%s",__FUNCTION__);

	switch(dwEventID)
	{
    case LFS_SRVE_IDC_MEDIAREMOVED:
		{
			//////浙江维尔
			if (m_Fpiprovider == 1)
			{
				if(m_FingerData == "1")//没有达到3次，继续录入指纹
				{
                    m_Tracks = LFS_IDC_CHIP|LFS_IDC_TRACK2;
                    N_ReadRawData(LFS_IDC_CHIP|LFS_IDC_TRACK2,30000);
				}
				else if(m_FingerData == "2")
				{
                    m_Tracks = LFS_IDC_CHIP|LFS_IDC_TRACK3;
                    N_ReadRawData(LFS_IDC_CHIP|LFS_IDC_TRACK3,30000);
				}
			}
            emit FingerMoved();//抛手指移开事件
		}
		break;
	}

	LOGINFO("%s",__FUNCTION__);
}

// 收到系统事件后调用该函数
void CMFPI::WR_SystemEvent(DWORD dwEventID, HRESULT hResult, LPVOID lpBuffer)
{
	switch(dwEventID)
	{
    case LFS_SYSE_DEVICE_STATUS://设备状态改变
		{
            LPLFSDEVSTATUS lpDevStatus=(LPLFSDEVSTATUS)((LPLFSRESULT)lpBuffer);
			if(lpDevStatus == NULL)
			{
				LOGERROR("%s,SP数据为空。",__FUNCTION__);
                Alarm("07000002");
				return;
			}

            SetDeviceStatus(lpDevStatus->state);
			if(m_CurrDevStatus!=m_PreDevStatus)
			{
				
                emit this->StatusChanged(m_szLogicalName,m_PreDevStatus,m_CurrDevStatus);
                LOGINFO("%s,事件：StatusChanged(\"StDeviceStatus\",\"%s\",\"%s\")",__FUNCTION__,m_PreDevStatus.toStdString().c_str(),m_CurrDevStatus.toStdString().c_str());
			}
		}
		break;
	}
}


// 以下为属性的处理函数

void CMFPI::SetServiceName(QString newVal)
{
    LOGINFO("%s,属性,newVal = %s",__FUNCTION__,newVal.toStdString().c_str());
    strcpy(m_szLogicalName,newVal.toStdString().c_str());
}

QString CMFPI::GetStDetailedDeviceStatus(void)
{
	int ret = N_GetStatus();

    if(LFS_SUCCESS != ret)
	{
		N_InitStatus();
	}
    LOGINFO("%s,Status = %s",__FUNCTION__,m_CurrDevStatus.toStdString().c_str());
	return m_CurrDevStatus;
}



QString CMFPI::GetStMediaStatus(void)// 改为 GetStMediaStatus
{
	int ret = N_GetStatus();

    if(LFS_SUCCESS != ret)
	{
		N_InitStatus();
	}

    QString strStatus("");

    if(LFS_IDC_MEDIANOTPRESENT == m_Status.media)
		strStatus = "NOTPRESENT";
    else if(LFS_IDC_MEDIAPRESENT == m_Status.media)
		strStatus = "PRESENT";
	else 
		strStatus = "UNKNOWN";

    LOGINFO("%s,属性,strStatus = %s",__FUNCTION__,strStatus.toStdString().c_str());

	return strStatus;
}


// 以下为方法的处理函数

SHORT CMFPI::CloseConnection(void)
{
	LOGINFO("%s，方法。",__FUNCTION__);
	
    int ret = LFS_SUCCESS;
	if (m_bOpened)
	{
		ret=N_Close();
	}

	return ret;
}

SHORT CMFPI::OpenConnection(void)
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

// 添加超时
void CMFPI::Reset(LONG Timeout)
{
	LOGINFO("%s，方法。",__FUNCTION__);

    N_Reset(LFS_IDC_NOACTION,Timeout);
}

void CMFPI::DataMatch(LONG Timeout, QString Data)
{
	char chData[2048] = {0};
    LFSIDCCHIPIO chipIoIn;
	memset(chData,0,2048);
    sprintf(chData,"%s^%s",m_FingerData.toStdString().c_str(),Data.toStdString().c_str());
    chipIoIn.chip_protocol = LFS_IDC_CHIPT0|LFS_IDC_CHIPT1;
    chipIoIn.chip_data = (LPBYTE)chData;
    chipIoIn.chip_data_length = strlen(chData);

	int ret = N_ChipIO(&chipIoIn,Timeout);
}

void CMFPI::Identify(LONG Timeout)
{
	LOGINFO("%s，方法。",__FUNCTION__);

	//得到指纹特征值
    m_Tracks = LFS_IDC_TRACK1;
    int ret = N_ReadRawData(LFS_IDC_TRACK1,Timeout);
}

void CMFPI::CancelIdentify(void)
{
	LOGINFO("%s，方法。",__FUNCTION__);

	N_Cancel(0);

}

void CMFPI::AcquireData(LONG Timeout)
{
	LOGINFO("%s，方法。",__FUNCTION__);

    m_Tracks = LFS_IDC_CHIP|LFS_IDC_TRACK1;

    int ret = N_ReadRawData(LFS_IDC_CHIP|LFS_IDC_TRACK1,Timeout);
}

// 添加 CancelAcquireData 
void CMFPI::CancelAcquireData(void)
{
	LOGINFO("%s，方法。",__FUNCTION__);

	N_Cancel(0);
}

int CMFPI::N_Open()
{
    int ret = m_apiCtrl.LFSAsyncOpen(m_szLogicalName,HM_MIDDLE_TIMEOUT,&m_CurrRequestID);

    if(LFS_SUCCESS!=ret)
	{
		LOGERROR("%s,打开失败！错误码：ret = %d",__FUNCTION__,ret);
        Alarm("07000000");
        emit OpenFailed();
	}
	/////获取产商
    m_Fpiprovider=help.GetPrivateProfileInt("MFPI","Fpiprovider",2,m_strConfigPath);

	return ret;
}

int CMFPI::N_Close()
{
    int ret = m_apiCtrl.LFSAsyncClose(&m_CurrRequestID);

    if(LFS_SUCCESS!=ret)
	{
		LOGERROR("%s,关闭失败！错误码：ret = %d",__FUNCTION__,ret);
        Alarm("07000000");
        emit CloseFailed();
	}

	return ret;
}

//取消当前正在执行的命令
int CMFPI::N_Cancel(WORD requestID)
{
    int ret = m_apiCtrl.LFSCancelAsyncRequest(0);
    if(LFS_SUCCESS!=ret)
	{
        emit DeviceError();
		LOGERROR("%s,执行错误，错误码：%d",__FUNCTION__,ret);
        Alarm("07000000");
	}

	return ret;
}

//执行LFS_INF_IDC_STATUS
int CMFPI::N_GetStatus()
{
    LPLFSRESULT lpResult = NULL;
    int ret = m_apiCtrl.LFSGetInfo(LFS_INF_IDC_STATUS, NULL, HM_MIDDLE_TIMEOUT, &lpResult);

    if(LFS_SUCCESS == ret)
	{
        LPLFSIDCSTATUS lpStatus = (LPLFSIDCSTATUS)((LPLFSRESULT)lpResult);
		if(lpStatus)
		{
            memcpy(&m_Status, lpStatus, sizeof(LFSIDCSTATUS));
            SetDeviceStatus(m_Status.device);
		}
		else
		{
			ret = -1;
			LOGERROR("%s, SP数据为空。", __FUNCTION__);
            Alarm("07000002");
		}
	}
	else
	{
		LOGERROR("%s, 获取状态失败错误码为：ret = %d", __FUNCTION__,ret);
        Alarm("07000001");
	}

    ::LFSFreeResult(lpResult);
	return ret;
}

//执行LFS_CMD_IDC_Get_RAW_DATA
int CMFPI::N_ReadRawData(WORD readData,LONG timeout)
{
	m_FingerData = "";

	timeout = (-1==timeout) ? 0 : timeout;//

	if(timeout<0)
	{
        if ( readData == LFS_IDC_TRACK1)
		{
            emit IdentifyFailed();
		}
		else
		{
            emit DataAcquireFailed();
		}
		LOGERROR("%s,时间参数错误。设置了-1之外的负数,timeout = %d",__FUNCTION__,timeout);
        Alarm("07000001");
		return HM_PARAM_ERROR;
	}

    int ret=m_apiCtrl.LFSAsyncExecute(LFS_CMD_IDC_READ_RAW_DATA,&readData,timeout,&m_CurrRequestID);
    if(LFS_SUCCESS!=ret)
	{
        if ( readData == LFS_IDC_TRACK1)
		{
            emit IdentifyFailed();
		}
		else
		{
            emit DataAcquireFailed();
		}
		LOGERROR("%s,执行错误，错误码：ret = %d",__FUNCTION__,ret);
        Alarm("07000001");
	}

	return ret;
}

//执行LFS_CMD_IDC_RESET
int CMFPI::N_Reset(WORD wResetIn,LONG timeout)
{
    int ret=m_apiCtrl.LFSAsyncExecute(LFS_CMD_IDC_RESET,&wResetIn,timeout,&m_CurrRequestID);
    if(LFS_SUCCESS!=ret)
	{
        emit ResetFailed();
		LOGERROR("%s,执行错误，错误码：ret = %d",__FUNCTION__,ret);
        Alarm("07000002");
	}

	return ret;
}

//执行LFS_CMD_IDC_CHIP_IO
int CMFPI::N_ChipIO(LPLFSIDCCHIPIO lpChipIoIn,LONG timeout)
{
	timeout = (-1==timeout) ? 0 : timeout;

	if(timeout<0)
	{
        emit MatchFailed();
		LOGERROR("%s,时间参数错误。设置了-1之外的负数,timeout = %d",__FUNCTION__,timeout);
        Alarm("07000002");
		return HM_PARAM_ERROR;
	}

    LPLFSRESULT lpResult = NULL;
    int ret=m_apiCtrl.LFSExecute(LFS_CMD_IDC_CHIP_IO, lpChipIoIn, timeout, &lpResult);
    if(LFS_SUCCESS == ret)
	{
        m_hResult = N_AfterChipIO(((LPLFSRESULT)lpResult)->result,lpResult);
	}
	else
	{
        emit MatchFailed();
		LOGERROR("%s,执行错误，错误码：ret = %d",__FUNCTION__,ret);
        Alarm("07000001");
	}
    ::LFSFreeResult(lpResult);
	return ret;
}

int CMFPI::N_AfterReadRawData(HRESULT hResult, LPVOID lpBuffer)
{
    LPLFSIDCCARDDATA *lppCardData = (LPLFSIDCCARDDATA *)((LPLFSRESULT)lpBuffer);

    if(LFS_SUCCESS == hResult)
	{
		if(lppCardData == NULL)
		{
            emit DeviceError();
			LOGERROR("%s,SP数据为空。",__FUNCTION__);
            Alarm("07000002");
			return -1;
		}

        QString str = (char *)lppCardData[0]->data;
        int index = str.indexOf('|');
		if(index != -1)
            m_FingerData = str.mid(index+1,str.length()-index-1);
		else
			m_FingerData = str;

        if(LFS_IDC_CHIP == lppCardData[0]->data_source)
		{
            emit DataAcquired(m_FingerData);
            LOGINFO("%s,事件：DataAcquired(%s)",__FUNCTION__,m_FingerData.toStdString().c_str());
		}
        else if(LFS_IDC_TRACK1 == lppCardData[0]->data_source)
		{
            emit IdentifyComplete(m_FingerData);
            LOGINFO("%s,事件：IdentifyComplete(%s),len = %d",__FUNCTION__,m_FingerData.mid(0,200).toStdString().c_str(),m_FingerData.length());
		}
	}
    else if(LFS_ERR_CANCELED == hResult && LFS_IDC_TRACK1 == m_Tracks)
	{
        emit IdentifyCancelled();
		LOGINFO("%s,事件：IdentifyCancelled()",__FUNCTION__);
	}
    else if(LFS_SUCCESS != hResult && LFS_IDC_TRACK1 == m_Tracks )
	{
        emit IdentifyFailed();
		LOGINFO("%s,事件：IdentifyFailed,错误码：hResult = %d",__FUNCTION__,hResult);
	}
    else if (LFS_ERR_CANCELED == hResult && ((LFS_IDC_CHIP && m_Tracks) == LFS_IDC_CHIP))// 添加 AcquireDataCancelled 事件，
	{
        emit AcquireDataCancelled();
		LOGINFO("%s,事件：AcquireDataCancelled",__FUNCTION__);
	}
    else if (LFS_SUCCESS != hResult && ((LFS_IDC_CHIP && m_Tracks) == LFS_IDC_CHIP) )//添加 DataAcquireFailed 事件
	{
        emit DataAcquireFailed();
		LOGINFO("%s,事件：DataAcquireFailed,错误码：hResult = %d",__FUNCTION__,hResult);
	}
	/*	else
	{
	DeviceError();
	LOGERROR("%s,执行错误，错误码：hResult = %d",__FUNCTION__,hResult);
	}	*/  

	return hResult;
}

int CMFPI::N_AfterChipIO(HRESULT hResult, LPVOID lpBuffer)
{
    if(LFS_SUCCESS == hResult)
	{
        LPLFSIDCCHIPIO lpChipIoOut = (LPLFSIDCCHIPIO)((LPLFSRESULT)lpBuffer);

		if(lpChipIoOut == NULL)
		{
            emit MatchFailed();//DeviceError();
			LOGERROR("%s,SP数据为空。",__FUNCTION__);
            Alarm("07000002");
			return -1;
		}
        m_bIdentify = (0==strcmp((char *)lpChipIoOut->chip_data,"0")) ? true:false;
		if (m_bIdentify)
		{
            emit MatchComplete();
			LOGINFO("%s,事件：MatchComplete()",__FUNCTION__);
		}
		else
		{
            emit MatchFailed();
			LOGINFO("%s,事件：MatchFailed(),错误码：false",__FUNCTION__);
		}
		
	}
	else
	{
        emit MatchFailed();
		LOGINFO("%s,事件：MatchFailed(),错误码：hResult = %d",__FUNCTION__,hResult);
	}

	return hResult;
}

void CMFPI::SetDeviceStatus(WORD wStatus)
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

void CMFPI::GetConfigPath()
{
    //char chFilePath[MAX_PATH]={0};
    QString strFilePath = help.GetModuleFileName((void*)&CMFPI::GetInstance);
    if(!strFilePath.isEmpty())
    {
        strFilePath = strFilePath.mid(0,strFilePath.indexOf('/'));
        strFilePath += "/ini/Setup.ini";
        m_strConfigPath = strFilePath;
    }
    else
    {
        m_strConfigPath = "";
    }
}

void CMFPI::N_InitStatus()
{
    m_Status.device=LFS_IDC_DEVNODEVICE;
    m_Status.media=LFS_IDC_MEDIANOTPRESENT;
    m_Status.cards=0;
    m_Status.retain_bin=LFS_IDC_RETAINNOTSUPP;
    m_Status.security=LFS_IDC_SECNOTREADY;
    m_Status.chip_power=LFS_IDC_CHIPUNKNOWN;
    SetDeviceStatus(m_Status.device);
}

int CMFPI::Execute(int ID, LPVOID lpData)
{
	switch (ID)
	{
    case LFS_INF_IDC_STATUS:
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
    return CMFPI::GetInstance();
}
