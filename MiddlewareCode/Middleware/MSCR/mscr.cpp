// Last updated at 2015/11/26 15:24:14

#include "stdafx.h"
#include "mscr.h"


#define HM_PARAM_ERROR -10000

#define PBOCTAGDATASIZE		1024
#define PBOCAPDUSIZE		2048

CMSCR* CMSCR::m_pInstance = NULL;

CMSCR::CMSCR(const QString& strDBusObjName):CWebInteractObj(strDBusObjName)
{
	m_PreDevStatus = m_CurrDevStatus ="OFFLINE";
	m_TrackXStatus[0]=m_TrackXStatus[1]=m_TrackXStatus[2]="NOTREAD";

	m_bOpened = false;
	memset(m_szLogicalName,0,MAX_PATH);

	LOGINFO("MSCR 模块加载！！！");
}

CMSCR::~CMSCR()
{
	LOGINFO("MSCR 模块卸载！！！");
}
CMSCR *CMSCR::GetInstance()
{
    if(m_pInstance == NULL)
    {
        m_pInstance = new CMSCR("MSCR");
    }

    return m_pInstance;
}
void  CMSCR::WR_OpenComplete(DWORD dwCommand, HRESULT hResult, LPVOID lpBuffer)
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
		}
	}
	else
	{
        emit OpenFailed();
		LOGERROR("%s,打开失败，错误码 ret = %d",__FUNCTION__,hResult);
	}
}

void  CMSCR::WR_RegisterComplete(DWORD dwCommand, HRESULT hResult, LPVOID lpBuffer)
{
    if(LFS_SUCCESS != hResult)
	{
		N_Close();		
		LOGERROR("%s,注册事件失败,错误码：%d",__FUNCTION__,hResult);
        emit OpenFailed();
	}
	else
	{
		int ret = N_GetCaps();
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
void CMSCR::WR_CloseComplete(DWORD dwCommand, HRESULT hResult, LPVOID lpBuffer)
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
		LOGERROR("%s,关闭设备失败,错误码：%d",__FUNCTION__,hResult);
	}
}

// 收到打开完成消息后调用该函数
void CMSCR::WR_GetinfoComplete(DWORD dwCommand, HRESULT hResult, LPVOID lpBuffer)
{

}

// 收到执行完成消息后调用该函数
void CMSCR::WR_ExecuteComplete(DWORD dwCommand, HRESULT hResult, LPVOID lpBuffer)
{
	LOGINFO("%s,IN",__FUNCTION__);

    if (LFS_ERR_TIMEOUT == hResult )
	{
        if(LFS_CMD_IDC_CHIP_IO!=dwCommand)//避免发送两次DeviceError（）事件
		{
            emit Timeout();
			LOGERROR("%s，命令超时，命令：%d,错误码：hResult = %d",__FUNCTION__,dwCommand,hResult);
		}

		return;
	}

	switch(dwCommand)
	{
    case LFS_CMD_IDC_READ_RAW_DATA:
		{
            if ( LFS_SUCCESS == hResult)
			{
                SaveTracks((LPLFSIDCCARDDATA *)((LPLFSRESULT)lpBuffer)->buffer);

				if(IsTracksDataOK())
				{
					if(1==m_wReadDataFlag)
					{
                        QString str1(m_chTracks[1]),str2(m_chTracks[2]);

                        str1.replace('=','D');
                        str2.replace('=','D');

						//将2、3磁道数据写入临时存储区
						//m_DSTO.D_SetDataString(false,"TRACK2DATA",m_chTracks[1]);
						//m_DSTO.D_SetDataString(false,"TRACK3DATA",m_chTracks[2]);

                        m_DSTO.D_SetDataString(false,"TRACK2DATA",Q2S(str1));
                        m_DSTO.D_SetDataString(false,"TRACK3DATA",Q2S(str2));

                        emit CardAccepted();
						LOGINFO("%s,事件：CardAccepted()",__FUNCTION__);
						LOGINFO("%s^%s^%s",m_chTracks[0],m_chTracks[1],m_chTracks[2]);
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
				}
			}
            else if(LFS_ERR_CANCELED == hResult)
			{
                emit this->CardAcceptCancelled();
				LOGINFO("%s,事件：CardAcceptCancelled()",__FUNCTION__);
				break;
			}
			else// 添加失败事件 CardAcceptFailed
			{
                emit CardAcceptFailed();
				LOGINFO("%s,事件：CardAcceptFailed(),错误码：hResult = %d",__FUNCTION__,hResult);
			}
		}
		break;

    case LFS_CMD_IDC_CHIP_IO:
		break;

    case LFS_CMD_IDC_RESET:
		{
            if ( LFS_SUCCESS == hResult)
			{
                emit this->ResetComplete();
				LOGINFO("%s,事件：ResetComplete()",__FUNCTION__);
			}
            else if (LFS_ERR_CANCELED == hResult)
			{
                LOGINFO("%s,LFS_CMD_IDC_RESET命令被取消,错误码：hResult = %d",__FUNCTION__,hResult);
			}
			else// 添加失败事件 ResetFailed 
			{
                emit ResetFailed();
				LOGINFO("%s,事件：ResetFailed(),错误码：hResult = %d",__FUNCTION__,hResult);
			}			
		}
		break;

    case LFS_CMD_IDC_CHIP_POWER:
		break;

	default:
		break;
	}

	LOGINFO("%s,OUT",__FUNCTION__);
}

// 收到执行事件后调用该函数
void CMSCR::WR_ExecuteEvent(DWORD dwEventID, HRESULT hResult, LPVOID lpBuffer)
{
	switch(dwEventID)
	{
    case LFS_EXEE_IDC_INVALIDTRACKDATA:
    case LFS_EXEE_IDC_INVALIDMEDIA:
        emit this->CardInvalid();
		LOGINFO("%s,事件：CardInvalid()",__FUNCTION__);
		break;
    case LFS_EXEE_IDC_MEDIAINSERTED:
        emit this->CardInserted();
		LOGINFO("%s,事件：CardInserted()",__FUNCTION__);
		break;
	}
}

// 收到用户事件后调用该函数
void CMSCR::WR_UserEvent(DWORD dwEventID, HRESULT hResult, LPVOID lpBuffer)
{

}

// 收到服务事件后调用该函数
void CMSCR::WR_ServiceEvent(DWORD dwEventID, HRESULT hResult, LPVOID lpBuffer)
{
	switch(dwEventID)
	{
    case LFS_SRVE_IDC_MEDIAREMOVED:
		{
            emit this->CardTaken();
			LOGINFO("%s,事件：CardTaken()",__FUNCTION__);
		}
		break;
	}
}

// 收到系统事件后调用该函数
void CMSCR::WR_SystemEvent(DWORD dwEventID, HRESULT hResult, LPVOID lpBuffer)
{
	switch(dwEventID)
	{
    case LFS_SYSE_DEVICE_STATUS://设备状态改变
		{
            LPLFSDEVSTATUS lpDevStatus=(LPLFSDEVSTATUS)((LPLFSRESULT)lpBuffer)->buffer;
			if(lpDevStatus == NULL)
			{
				LOGERROR("%s,SP数据为空。",__FUNCTION__);
				return;
			}

            SetDeviceStatus(lpDevStatus->state);
			if(m_CurrDevStatus!=m_PreDevStatus)
			{
			
                emit this->StatusChanged("SCR",m_PreDevStatus,m_CurrDevStatus);
                LOGINFO("%s,事件：StatusChanged(\"StDeviceStatus\",\"%s\",\"%s\")",__FUNCTION__,Q2S(m_PreDevStatus),Q2S(m_CurrDevStatus));

			}
		}
		break;
	}
}


// 以下为属性的处理函数

void CMSCR::SetServiceName(QString newVal)
{
    LOGINFO("%s,属性,newVal = %s",__FUNCTION__,Q2S(newVal));

    strcpy(m_szLogicalName,Q2S(newVal));
}

QString CMSCR::GetStDetailedDeviceStatus(void)
{
	int ret = N_GetStatus();
    if (ret != LFS_SUCCESS)/////获取状态失败给初始值
	{
		N_InitStatus();
	}

    LOGINFO("%s,Status = %s",__FUNCTION__,Q2S(m_CurrDevStatus));

	return m_CurrDevStatus;
}

BOOL CMSCR::GetCpCanReadTrack1(void)
{
    bool bTrack = ((m_Caps.read_tracks&LFS_IDC_TRACK1)==LFS_IDC_TRACK1) ? true:false;

	return bTrack;
}

BOOL CMSCR::GetCpCanReadTrack2(void)
{
    bool bTrack = ((m_Caps.read_tracks&LFS_IDC_TRACK2)==LFS_IDC_TRACK2) ? true:false;
	return bTrack;
}

BOOL CMSCR::GetCpCanReadTrack3(void)
{
    bool bTrack = ((m_Caps.read_tracks&LFS_IDC_TRACK3)==LFS_IDC_TRACK3) ? true:false;
	return bTrack;
}


QString CMSCR::GetStMediaStatus(void)
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
		mediaStatus="PRESENT";
		break;
	}

    LOGINFO("%s,mediaStatus = %s,m_Status.fwMedia = %d",__FUNCTION__,Q2S(mediaStatus),m_Status.media);

	return mediaStatus;
}


QString CMSCR::GetTrack1(void)
{

    return QString(m_chTracks[0]);
}

QString CMSCR::GetTrack2(void)
{
    QString str(m_chTracks[1]);

	return str;
}

QString CMSCR::GetTrack3(void)
{
    QString str(m_chTracks[2]);

	return str;
}

QString CMSCR::GetTrack1Status(void)
{

	return m_TrackXStatus[0];
}

QString CMSCR::GetTrack2Status(void)
{

	return m_TrackXStatus[1];
}

QString CMSCR::GetTrack3Status(void)
{

	return m_TrackXStatus[2];
}





// 以下为方法的处理函数

SHORT CMSCR::CloseConnection(void)
{
	LOGINFO("%s，方法。",__FUNCTION__);
    int ret = LFS_SUCCESS;
	if (m_bOpened)
	{
		ret=N_Close();
	}
	return 0;
}
SHORT CMSCR::OpenConnection(void)
{
	LOGINFO("%s，方法。",__FUNCTION__);

	if(m_bOpened)
	{
        emit ConnectionOpened();
		LOGERROR("%s,事件：ConnectionOpened（）",__FUNCTION__);
		return 0;
	}

    //m_apiCtrl.SetSendWnd(GetWindowHwnd());

	return N_Open();
}


SHORT CMSCR::AcceptAndReadTracks(QString TrackMap, LONG Timeout)
{
    LOGINFO("方法：%s(%s,%d)",__FUNCTION__,Q2S(TrackMap),Timeout);

	memset(m_chTracks,0,sizeof(m_chTracks));//数据清空

	m_wReadDataFlag=1;

	WORD wReadData = GetTracks(TrackMap);

	return N_ReadRawData(wReadData,Timeout);
}

SHORT CMSCR::AcceptAndReadAvailableTracks(QString TrackMap, LONG Timeout)
{
    LOGINFO("方法：%s(%s,%d)",__FUNCTION__,Q2S(TrackMap),Timeout);

	return AcceptAndReadTracks(TrackMap,Timeout);
}

SHORT CMSCR::CancelAccept(void)
{
	LOGINFO("方法：%s(void)",__FUNCTION__);

	return N_Cancel(0);
}

// 添加超时
SHORT CMSCR::Reset(QString ResetAction,LONG Timeout)
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
        emit ResetFailed();//ErrorSend(HM_PARAM_ERROR);		// ResetFailed
		LOGERROR("%s,参数错误。",__FUNCTION__);
		return HM_PARAM_ERROR;
	}

	return N_Reset(action,Timeout);
}

//取消当前正在执行的命令
int CMSCR::N_Cancel(WORD requestID)
{
    int ret = m_apiCtrl.LFSCancelAsyncRequest(requestID);
    if(LFS_SUCCESS!=ret)
	{
		LOGERROR("%s,执行错误，错误码：ret = %d。",__FUNCTION__,ret);
        emit CardAcceptCancelFailed();// CardAcceptCancelFailed
	}

	return ret;
}


//执行LFS_INF_IDC_STATUS
int CMSCR::N_GetStatus()
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
		}
	}
	else
	{
		LOGERROR("%s, 获取状态失败错误码为：%d", __FUNCTION__,ret);
	}

    ::LFSFreeResult(lpResult);
	return ret;
}

//执行LFS_INF_IDC_CAPABILITIES
int CMSCR::N_GetCaps()
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
		}
	}
	else
	{
		LOGERROR("%s, 获取能力失败错误码为：%d。", __FUNCTION__,ret);

	}
    ::LFSFreeResult(lpResult);
	return ret;
}

//执行LFS_CMD_IDC_READ_RAW_DATA
int CMSCR::N_ReadRawData(WORD readData,LONG timeout)
{
	timeout = (-1==timeout) ? 0 : timeout;//

	if(timeout<0)
	{
        emit CardAcceptFailed();		// CardAcceptFailed
		LOGERROR("%s,时间参数错误。设置了-1之外的负数,timeout = %d",__FUNCTION__,timeout);
		return HM_PARAM_ERROR;
	}

    int ret=m_apiCtrl.LFSAsyncExecute(LFS_CMD_IDC_READ_RAW_DATA,&readData,timeout,&m_CurrRequestID);
    if(LFS_SUCCESS!=ret)
	{
        emit CardAcceptFailed();  // CardAcceptFailed
		LOGERROR("%s,执行错误,错误码：ret = %d",__FUNCTION__,ret);
	}

	return ret;
}

//执行LFS_CMD_IDC_RESET
int CMSCR::N_Reset(WORD wResetIn,LONG timeout)
{
    int ret=m_apiCtrl.LFSAsyncExecute(LFS_CMD_IDC_RESET,&wResetIn,timeout,&m_CurrRequestID);
    if(LFS_SUCCESS!=ret)
	{
        emit ResetFailed();//ResetFailed
		LOGERROR("%s,执行错误,错误码：%d",__FUNCTION__,ret);
	}

	return ret;
}

int CMSCR::N_Open()
{
    int ret = m_apiCtrl.LFSAsyncOpen(m_szLogicalName,HM_MIDDLE_TIMEOUT,&m_CurrRequestID);

    if(LFS_SUCCESS!=ret)
	{
		LOGERROR("%s,打开失败！",__FUNCTION__);
        emit OpenFailed();		// OpenFailed
	}

	return ret;
}

int CMSCR::N_Close()
{
    int ret = m_apiCtrl.LFSAsyncClose(&m_CurrRequestID);

    if(LFS_SUCCESS!=ret)
	{
		LOGERROR("%s,关闭失败！",__FUNCTION__);
        emit CloseFailed();  // CloseFailed
	}

	return ret;
}


//得到以sep分割的所有子串
CMSCR::VerStr CMSCR::Token_All(const QString str,const char sep)
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

bool CMSCR::ASCIIToBCD(unsigned char *bcd, const unsigned char *asc, int nLen)
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
bool CMSCR::BCDToASCII(LPBYTE asc, const LPBYTE bcd, int len)
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


void CMSCR::SetDeviceStatus(WORD wStatus)
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


WORD CMSCR::GetTracks(const QString strTracks)
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
           // wReadData |= LFS_IDC_FRONT_TRACK_1;
		else if("SECURITY"==*it)
            wReadData |= LFS_IDC_SECURITY;
	}

	return wReadData;
}


void CMSCR::SaveTracks(LPLFSIDCCARDDATA *lppCardData)
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
                    memcpy(m_chTracks[1],lppCardData[i]->data,lppCardData[i]->data_length+1);
				else
					LOGINFO("%s,第二磁道数据为空",__FUNCTION__);
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
                BCDToASCII((LPBYTE)m_chTracks[3],lppCardData[i]->data,2*lppCardData[i]->data_length);
			else
			{
				LOGINFO("%s,CHIP为空",__FUNCTION__);
				memset(m_chTracks[3],0,sizeof(m_chTracks[3]));
			}
		}
	}
}


bool CMSCR::IsTracksDataOK()
{
	for(int i=0;i<3;i++)
	{
		if(m_TrackXStatus[i]=="INVALID")
			return false;
	}

	return true;
}

void CMSCR::N_InitStatus()
{
	
}

void CMSCR::N_InitCaps()
{
	
}

int CMSCR::Execute(int ID, LPVOID lpData)
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
    return CMSCR::GetInstance();
}
