// Last updated at 2015/12/7 10:24:36

#include "stdafx.h"
#include "mrpt.h"
#include "DRPT.h"
#include "AlarmLib.h"
#include "Helper/helper.h"
#include "win_type_def.h"

CMRPT* CMRPT::m_pInstance = NULL;

CMRPT::CMRPT(const QString& strDBusObjName):CWebInteractObj(strDBusObjName)
{
	m_bOpened=false;

	m_DeviceStatus = "OFFLINE";
	m_DetailedDeviceStatus = "OFFLINE";
	m_PreDevStatus = "OFFLINE";

	LOGINFO("MRPT 模块加载");
}

CMRPT::~CMRPT()
{
	LOGINFO("MRPT 模块卸载");
}

CMRPT *CMRPT::GetInstance()
{
    if(m_pInstance == NULL)
    {
        m_pInstance = new CMRPT("MRPT");
    }

    return m_pInstance;
}

// 收到打开完成消息后调用该函数
void  CMRPT::WR_OpenComplete(DWORD dwCommand, HRESULT hResult, LPVOID lpBuffer)
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
			LOGERROR("%s,注册事件失败,错误码 ret = %d",__FUNCTION__,ret);
			Alarm("03000000");
		}
	}
	else
	{
        emit OpenFailed();
		LOGERROR("%s,打开失败，错误码 ret = %d",__FUNCTION__,hResult);
		Alarm("03000000");
	}
}

void  CMRPT::WR_RegisterComplete(DWORD dwCommand, HRESULT hResult, LPVOID lpBuffer)
{
    if(LFS_SUCCESS != hResult)
	{
		N_Close();		// OpenFailed  AAA
		LOGERROR("%s,注册事件失败,错误码：%d",__FUNCTION__,hResult);
		Alarm("03000000");
        emit OpenFailed();
	}
	else
	{
		HRESULT ret = N_GetStatus();
        if (ret != LFS_SUCCESS)/////获取状态失败给初始值
		{
			LOGERROR("%s, 获取状态失败，ret=%d", __FUNCTION__, ret);
			Alarm("03000001");
		}
        if(LFS_SUCCESS == ret)
		{
			ret = N_GetCaps();
            if (ret != LFS_SUCCESS)
			{
			   LOGERROR("%s, 获取能力失败，ret=%d", __FUNCTION__, ret);
			   Alarm("03000001");
			}
			m_bOpened = true;
            emit ConnectionOpened();
			LOGINFO("%s,事件：ConnectionOpened()",__FUNCTION__);		

		}
		else
		{
			N_Close();		
			LOGERROR("%s,打开设备失败,错误码：%d",__FUNCTION__,ret);
			Alarm("03000000");
            emit OpenFailed();
		}
	}
}

// 收到关闭完成消息后调用该函数
void CMRPT::WR_CloseComplete(DWORD dwCommand, HRESULT hResult, LPVOID lpBuffer)
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
		Alarm("03000000");
	}
}

// 收到打开完成消息后调用该函数
void CMRPT::WR_GetinfoComplete(DWORD dwCommand, HRESULT hResult, LPVOID lpBuffer)
{
	
}

// 收到执行完成消息后调用该函数
void CMRPT::WR_ExecuteComplete(DWORD dwCommand, HRESULT hResult, LPVOID lpBuffer)
{
	switch(dwCommand)
	{
    case LFS_CMD_PTR_CONTROL_MEDIA:
        if(hResult != LFS_SUCCESS)
		{
            LOGERROR("函数：%s，命令：LFS_CMD_PTR_CONTROL_MEDIA，事件：ControlFailed",__FUNCTION__);
			Alarm("03000001");
            emit ControlFailed();
		}
		else
		{
            LOGINFO("函数：%s，命令：LFS_CMD_PTR_CONTROL_MEDIA，事件：ControlComplete",__FUNCTION__);
            emit ControlComplete();
		}
		break;
    case LFS_CMD_PTR_PRINT_FORM:
        if (hResult == LFS_ERR_TIMEOUT)
		{
            emit Timeout();
            LOGERROR("函数：%s，命令：LFS_CMD_PTR_PRINT_FORM，事件：Timeout",__FUNCTION__);
			Alarm("03000001");
		}
        else if(hResult == LFS_SUCCESS)
		{
            LOGINFO("函数：%s，命令：LFS_CMD_PTR_PRINT_FORM，事件：PrintComplete",__FUNCTION__);
			//ControlMedia("CUT",30000);
             emit PrintComplete();
		}
		else
		{
            QString sResult;
			switch(hResult)
			{
            case LFS_ERR_PTR_FORMNOTFOUND :
                sResult = "LFS_ERR_PTR_FORMNOTFOUND";
				break;
            case LFS_ERR_PTR_FORMINVALID:
                sResult = "LFS_ERR_PTR_FORMINVALID";
				break;
            case LFS_ERR_PTR_FIELDERROR:
                sResult = "LFS_ERR_PTR_FIELDERROR";
				break;
            case LFS_ERR_PTR_FIELDSPECFAILURE :
                sResult = "LFS_ERR_PTR_FIELDSPECFAILURE";
				break;
			default:
                sResult.sprintf("%d", hResult);
				break;
			}
            LOGERROR("函数：%s，命令：LFS_CMD_PTR_PRINT_FORM，事件：PrintFailed，错误码：%s",__FUNCTION__, Q2S(sResult));
			Alarm("03000001");
            emit PrintFailed();
		}
		break;
    case LFS_CMD_PTR_RAW_DATA:
        if(hResult != LFS_SUCCESS)
		{
            emit PrintFailed();
            LOGERROR("函数：%s，命令：LFS_CMD_PTR_RAW_DATA，PrintFailed事件",__FUNCTION__);
			Alarm("03000001");
		}
		else
		{
             emit PrintComplete();
            LOGINFO("函数：%s，命令：LFS_CMD_PTR_RAW_DATA，事件：%s",__FUNCTION__, "PrintComplete");
		}
		break;
    case LFS_CMD_PTR_RESET:
        if (hResult == LFS_ERR_TIMEOUT)
		{
            emit Timeout();
            LOGERROR("函数：%s，命令：LFS_CMD_PTR_RESET，Timeout事件", __FUNCTION__);
			Alarm("03000001");
		}
        else if(hResult != LFS_SUCCESS)
		{
            emit ResetFailed();
            LOGERROR("函数：%s，命令：LFS_CMD_PTR_RESET，ResetFailed事件",__FUNCTION__);
			Alarm("03000001");
		}
		else
		{
            emit ResetComplete();
            LOGINFO("函数：%s，命令：LFS_CMD_PTR_RESET，事件：%s",__FUNCTION__, "ResetComplete");
		}
		break;
	}
}

// 收到执行事件后调用该函数
void CMRPT::WR_ExecuteEvent(DWORD dwEventID, HRESULT hResult, LPVOID lpBuffer)
{
}

// 收到用户事件后调用该函数
void CMRPT::WR_UserEvent(DWORD dwEventID, HRESULT hResult, LPVOID lpBuffer)
{

}

// 收到服务事件后调用该函数
void CMRPT::WR_ServiceEvent(DWORD dwEventID, HRESULT hResult, LPVOID lpBuffer)
{
	switch(dwEventID)
	{
    case LFS_SRVE_PTR_MEDIATAKEN:
        LOGINFO("函数：%s，命令：LFS_SRVE_PTR_MEDIATAKEN，事件：PrintTaken",__FUNCTION__);
        emit PrintTaken();
		break;

	}
}

// 收到系统事件后调用该函数
void CMRPT::WR_SystemEvent(DWORD dwEventID, HRESULT hResult, LPVOID lpBuffer)
{
	switch(dwEventID)
	{
    case LFS_SYSE_DEVICE_STATUS://设备状态改变
		{
            LPLFSDEVSTATUS lpDevStatus=(LPLFSDEVSTATUS)((LPLFSRESULT)lpBuffer)->buffer;
			if(lpDevStatus == NULL)
			{
				LOGINFO("%s,SP数据为空。",__FUNCTION__);
				Alarm("03000002");
				return;
			}

            N_SetDeviceStatus(lpDevStatus->state);
			if(m_DetailedDeviceStatus!=m_PreDevStatus)
			{
                emit this->StatusChanged("RPT",m_PreDevStatus,m_DetailedDeviceStatus);
                LOGINFO("%s,事件：StatusChanged(\"StDeviceStatus\",\"%s\",\"%s\")", __FUNCTION__, Q2S(m_PreDevStatus), Q2S(m_DetailedDeviceStatus));
			}
		}
		break;
	}
}


// 以下为属性的处理函数

void CMRPT::SetServiceName(QString newVal)
{
	LOGINFO("函数：%s",__FUNCTION__);
    memcpy(m_szLogicalName,Q2S(newVal),newVal.length());
}

QString CMRPT::GetStDetailedDeviceStatus(void)
{
	LOGINFO("函数：%s",__FUNCTION__);

	HRESULT ret = N_GetStatus();
    if (ret != LFS_SUCCESS)/////获取状态失败给初始值
	{
		LOGERROR("%s, 获取状态失败，ret=%d", __FUNCTION__, ret);
		Alarm("03000001");
	}

    return m_DetailedDeviceStatus;
}

QString CMRPT::GetStInkStatus(void)
{
	LOGINFO("函数：%s",__FUNCTION__);

	HRESULT ret = N_GetStatus();
    if (ret != LFS_SUCCESS)/////获取状态失败给初始值
	{
		LOGERROR("%s, 获取状态失败，ret=%d", __FUNCTION__, ret);
		Alarm("03000001");
	}
    QString szfInk;

    switch(m_Status.ink)
	{
    case LFS_PTR_INKFULL:
		szfInk = "FULL";
		break;
    case LFS_PTR_INKLOW:
		szfInk = "LOW";
		break;
    case LFS_PTR_INKOUT:
		szfInk = "LOW";
		break;
	default:
		szfInk = "UNKNOWN";
		break;
	}
	return szfInk;
}

QString CMRPT::GetStMediaStatus(void)
{
	LOGINFO("函数：%s",__FUNCTION__);

	HRESULT ret = N_GetStatus();
    if (ret != LFS_SUCCESS)/////获取状态失败给初始值
	{
		LOGERROR("%s, 获取状态失败，ret=%d", __FUNCTION__, ret);
		Alarm("03000001");
	}

    QString mediaStatus;
    switch(m_Status.media)
	{
    case LFS_PTR_MEDIAPRESENT:
		mediaStatus="PRESENT";
		break;
    case LFS_PTR_MEDIANOTPRESENT:
		mediaStatus="NOTPRESENT";
		break;
    case LFS_PTR_MEDIAJAMMED:
		mediaStatus="JAMMED";
		break;
    case LFS_PTR_MEDIANOTSUPP:
		mediaStatus="NOTSUPP";
		break;
    case LFS_PTR_MEDIAUNKNOWN:
		mediaStatus="UNKNOWN";
		break;
    case LFS_PTR_MEDIAENTERING:
		mediaStatus="INJAWS";
		break;
    case LFS_PTR_MEDIARETRACTED:
		mediaStatus="TRACTED";
		break;
	default:
		mediaStatus="UNKNOWN";
		break;
	}
	return mediaStatus;
}

QString CMRPT::GetStPaperStatus(void)
{
	LOGINFO("函数：%s",__FUNCTION__);

	HRESULT ret = N_GetStatus();
    if (ret != LFS_SUCCESS)/////获取状态失败给初始值
	{
		LOGERROR("%s, 获取状态失败，ret=%d", __FUNCTION__, ret);
		Alarm("03000001");
	}

    QString PaperStatus;

    switch(m_Status.paper[LFS_PTR_SUPPLYUPPER])
	{
    case LFS_PTR_PAPERFULL:
		PaperStatus = "FULL";
		break;
    case LFS_PTR_PAPERLOW:
		PaperStatus = "LOW";
		break;
    case LFS_PTR_PAPEROUT:
		PaperStatus = "OUT";
		break;
	default:
		PaperStatus = "UNKNOWN";
		break;
	}
    return PaperStatus;
}


BOOL CMRPT::GetCpCanEject(void)
{
	LOGINFO("函数：%s",__FUNCTION__);
	
    return m_Caps.control & LFS_PTR_CTRLEJECT;
}

BOOL CMRPT::GetCpCanCapture(void)
{
	LOGINFO("函数：%s",__FUNCTION__);

    return m_Caps.control & LFS_PTR_CTRLRETRACT;
}

BOOL CMRPT::GetCpCanStack(void)
{
	LOGINFO("函数：%s",__FUNCTION__);

    return m_Caps.control & LFS_PTR_CTRLSTACK;
}

BOOL CMRPT::GetCpCanDetectMediaTaken(void)
{
	LOGINFO("函数：%s",__FUNCTION__);

    return m_Caps.media_taken;
}


QString CMRPT::GetFormName(void)
{
	LOGINFO("函数：%s",__FUNCTION__);

	return FormName;
}

QString CMRPT::GetFormBase(void)
{
	LOGINFO("函数：%s",__FUNCTION__);

    return FormBase;
}

LONG CMRPT::GetFormUnitX(void)
{
	LOGINFO("函数：%s",__FUNCTION__);

    return FormUnitX;
}

LONG CMRPT::GetFormUnitY(void)
{
	LOGINFO("函数：%s",__FUNCTION__);

    return FormUnitY;
}

LONG CMRPT::GetFormWidth(void)
{
	LOGINFO("函数：%s",__FUNCTION__);

    return FormWidth;
}

LONG CMRPT::GetFormHeight(void)
{
	LOGINFO("函数：%s",__FUNCTION__);

    return FormHeight;
}

QString CMRPT::GetFormAlignment(void)
{
	LOGINFO("函数：%s",__FUNCTION__);

    return FormAlignment;
}

QString CMRPT::GetFormOrientation(void)
{
	LOGINFO("函数：%s",__FUNCTION__);

    return FormOrientation;
}

LONG CMRPT::GetFormOffsetX(void)
{
	LOGINFO("函数：%s",__FUNCTION__);

    return FormOffsetX;
}

LONG CMRPT::GetFormOffsetY(void)
{
	LOGINFO("函数：%s",__FUNCTION__);

    return FormOffsetY;
}

LONG CMRPT::GetFormVersionMajor(void)
{
	LOGINFO("函数：%s",__FUNCTION__);

    return FormVersionMajor;
}

LONG CMRPT::GetFormVersionMinor(void)
{
	LOGINFO("函数：%s",__FUNCTION__);

    return FormVersionMinor;
}

QString CMRPT::GetFormUserPrompt(void)
{
	LOGINFO("函数：%s",__FUNCTION__);

    return FormUserPrompt;
}

QString CMRPT::GetFormFields(void)
{
	LOGINFO("函数：%s",__FUNCTION__);

    return FormFields;
}

QString CMRPT::GetFormFieldsIndexCount(void)
{
	LOGINFO("函数：%s",__FUNCTION__);

    return "";
}

QString CMRPT::GetFormFieldsType(void)
{
	LOGINFO("函数：%s",__FUNCTION__);

    return "";
}

QString CMRPT::GetFormFieldsClass(void)
{
	LOGINFO("函数：%s",__FUNCTION__);

    return "";
}

QString CMRPT::GetFormFieldsAccess(void)
{
	LOGINFO("函数：%s",__FUNCTION__);

    return "";
}

QString CMRPT::GetFormFieldsOverflow(void)
{
	LOGINFO("函数：%s",__FUNCTION__);

    return "";
}

QString CMRPT::GetFormFieldsInitialValue(void)
{
	LOGINFO("函数：%s",__FUNCTION__);

    return "";
}

QString CMRPT::GetFormFieldsFormat(void)
{
	LOGINFO("函数：%s",__FUNCTION__);

    return "";
}

QString CMRPT::GetMediaName(void)
{
	LOGINFO("函数：%s",__FUNCTION__);

    return MediaName;
}

QString CMRPT::GetMediaType(void)
{
	LOGINFO("函数：%s",__FUNCTION__);

    return MediaType;
}

QString CMRPT::GetMediaBase(void)
{
	LOGINFO("函数：%s",__FUNCTION__);

    return MediaBase;
}

LONG CMRPT::GetMediaUnitX(void)
{
	LOGINFO("函数：%s",__FUNCTION__);

    return MediaUnitX;
}

LONG CMRPT::GetMediaUnitY(void)
{
	LOGINFO("函数：%s",__FUNCTION__);

    return MediaUnitY;
}

LONG CMRPT::GetMediaSizeWidth(void)
{
	LOGINFO("函数：%s",__FUNCTION__);

    return MediaSizeWidth;
}

LONG CMRPT::GetMediaSizeHeight(void)
{
	LOGINFO("函数：%s",__FUNCTION__);

    return MediaSizeHeight;
}

LONG CMRPT::GetMediaPageCount(void)
{
	LOGINFO("函数：%s",__FUNCTION__);

    return MediaPageCount;
}

LONG CMRPT::GetMediaLineCount(void)
{
	LOGINFO("函数：%s",__FUNCTION__);

    return MediaLineCount;
}

LONG CMRPT::GetMediaPrintAreaX(void)
{
	LOGINFO("函数：%s",__FUNCTION__);

    return MediaPrintAreaX;
}

LONG CMRPT::GetMediaPrintAreaY(void)
{
	LOGINFO("函数：%s",__FUNCTION__);

    return MediaPrintAreaY;
}

LONG CMRPT::GetMediaPrintAreaWidth(void)
{
	LOGINFO("函数：%s",__FUNCTION__);

    return MediaPrintAreaWidth;
}

LONG CMRPT::GetMediaPrintAreaHeight(void)
{
	LOGINFO("函数：%s",__FUNCTION__);

    return MediaPrintAreaHeight;
}

LONG CMRPT::GetMediaRestrictedAreaX(void)
{
	LOGINFO("函数：%s",__FUNCTION__);

    return MediaRestrictedAreaX;
}

LONG CMRPT::GetMediaRestrictedAreaY(void)
{
	LOGINFO("函数：%s",__FUNCTION__);

    return MediaRestrictedAreaY;
}

LONG CMRPT::GetMediaRestrictedAreaWidth(void)
{
	LOGINFO("函数：%s",__FUNCTION__);

    return MediaRestrictedAreaWidth;
}

LONG CMRPT::GetMediaRestrictedAreaHeight(void)
{
	LOGINFO("函数：%s",__FUNCTION__);

    return MediaRestrictedAreaHeight;
}

LONG CMRPT::GetMediaStagger(void)
{
	LOGINFO("函数：%s",__FUNCTION__);

    return MediaStagger;
}

QString CMRPT::GetMediaFoldType(void)
{
	LOGINFO("函数：%s",__FUNCTION__);

    return MediaFoldType;
}


// 以下为方法的处理函数


SHORT CMRPT::CloseConnection(void)
{
	LOGINFO("%s，方法。",__FUNCTION__);
    int ret = LFS_SUCCESS;
	if (m_bOpened)
	{
		ret=N_Close();
	}
	return 0;
}

SHORT CMRPT::OpenConnection(void)
{
	LOGINFO("%s，方法。",__FUNCTION__);

	if(m_bOpened)
	{
        emit ConnectionOpened();
		LOGINFO("%s,事件：ConnectionOpened（）",__FUNCTION__);
		return 0;
	}
   // m_apiCtrl.SetDBusName(m_lpDBusName);
	return N_Open();
}

SHORT CMRPT::Reset(QString ResetAction, LONG Timeout)
{
	LOGINFO("函数：%s",__FUNCTION__);

    LFSPTRRESET Reset;

    if(QString::compare(ResetAction,"RETRACT") == 0)
	{
		//不支持回收，暂时不实现
		LOGERROR("函数：%s，不支持回收，暂时不实现",__FUNCTION__);
	}
    else if(QString::compare(ResetAction,"EJECT") == 0 || ResetAction.length() == 0)
	{
        Reset.media_control = LFS_PTR_CTRLEJECT;

		REQUESTID id;
        HRESULT ret = m_apiCtrl.LFSAsyncExecute(LFS_CMD_PTR_RESET, &Reset, Timeout, &id);

        if(ret != LFS_SUCCESS)
		{
            emit ResetFailed();
			Alarm("03000001");
			LOGERROR("函数：%s，事件：ResetFailed",__FUNCTION__);
		}
	}
	else
	{
        emit ResetFailed();
		Alarm("03000001");
		LOGERROR("函数：%s，事件：ResetFailed，参数错误",__FUNCTION__);
	}

    return 0;
}


SHORT CMRPT::Print(QString FormName, QString FieldValues, LONG Timeout)
{
	LOGINFO("函数：%s",__FUNCTION__);

	//从DSTO获取数据
    if(FieldValues.length()==0)
	{
		N_SetCurTimeToDB();
		M_GetFieldValue(FormName, FieldValues);

        QString BoxAountptr;
		BoxAountptr = CountBoxAount(FormName); //凭条：计算个钞箱金额、总金额

        if(BoxAountptr.length())
		{
			FieldValues += ","+BoxAountptr;
		}
	}
    LOGINFO("函数：%s，FormName = %s, FieldValues = %s",__FUNCTION__, Q2S(FormName), Q2S(FieldValues));

    LFSPTRPRINTFORM printForm;
	CHAR* lpFormName = NULL; 
	CHAR *lpFieldValues = NULL; 

    printForm.media_name=NULL;//add by art for SP修改后要判断此值，相应增加这句

    if(FormName.length() !=0)
	{
        lpFormName = new char[FormName.length()+1];
        strcpy(lpFormName, Q2S(FormName));

        printForm.form_name = lpFormName;

        if(FieldValues.length() != 0)
		{
            lpFieldValues = new char[FieldValues.length()+2];
            LOGINFO("函数：%s，FieldValues=%s",__FUNCTION__,Q2S(FieldValues));
			FieldsConvert(FieldValues, lpFieldValues); //解析,把分隔符 ',' 替换为'\0',并在lpFieldValues末尾添加2个'\0'
			
            printForm.fields = lpFieldValues;
		}
		else
		{
            printForm.fields = NULL;
		}
        printForm.unicode_fields = NULL;

        printForm.alignment = LFS_PTR_ALNUSEFORMDEFN   ;  //按form指定的对齐方式
        printForm.resolution = LFS_PTR_RESHIGH ;   //高分辨率
        printForm.offset_x = LFS_PTR_OFFSETUSEFORMDEFN ;  //偏移按照form指定的数值
        printForm.offset_y = LFS_PTR_OFFSETUSEFORMDEFN;   //偏移按照form指定的数值
        printForm.media_control = LFS_PTR_CTRLFLUSH|LFS_PTR_CTRLEJECT|LFS_PTR_CTRLCUT;  //
        printForm.paper_source = LFS_PTR_PAPERANY ;  //纸张来源由service确定

		REQUESTID id=0;
        HRESULT ret = m_apiCtrl.LFSAsyncExecute(LFS_CMD_PTR_PRINT_FORM, &printForm, Timeout, &id);
        if(ret != LFS_SUCCESS)
		{
            emit PrintFailed();
			Alarm("03000001");
			LOGERROR("函数：%s，事件：PrintFailed",__FUNCTION__);
		}

		if(lpFormName)
		{
			delete[] lpFormName;
		}
		
		if(lpFieldValues)
		{
			delete[] lpFieldValues;
		}		
	}

    return 0;
}

SHORT CMRPT::ControlMedia(QString MediaAction, LONG Timeout)
{
	LOGINFO("函数：%s",__FUNCTION__);

	DWORD dwMediaControl;
	
	if(Timeout < 0)
	{
        emit ControlFailed();
		LOGERROR("函数：%s，事件：ControlFailed，原因：参数错误",__FUNCTION__);
		Alarm("03000002");
		return 0;
	}

	dwMediaControl = GetMediaControl(MediaAction);
	if(dwMediaControl == 0)
	{
        emit ControlFailed();
		LOGERROR("函数：%s，事件：ControlFailed，原因：参数错误",__FUNCTION__);
		Alarm("03000002");
		return 0;
	}

	REQUESTID id=0;
    HRESULT ret = m_apiCtrl.LFSAsyncExecute(LFS_CMD_PTR_CONTROL_MEDIA, &dwMediaControl, Timeout, &id);
    if(ret != LFS_SUCCESS)
	{
        emit ControlFailed();
		LOGERROR("函数：%s，事件：ControlFailed",__FUNCTION__);
		Alarm("03000001");
	}

    return 0;
}

QString CMRPT::GetFormNamesSync(void)
{
	LOGINFO("函数：%s",__FUNCTION__);

	m_isGetFormNames = TRUE;

    FormNameList.clear();
	
    LPLFSRESULT lpResult = NULL;
    HRESULT ret = m_apiCtrl.LFSGetInfo(LFS_INF_PTR_FORM_LIST,NULL,HM_MIDDLE_TIMEOUT,&lpResult);
    if(LFS_SUCCESS != ret)
	{
        FormNameList.clear();
        LOGERROR("函数：%s, LFS_INF_PTR_FORM_LIST返回失败，ret=%d",__FUNCTION__, ret);
		Alarm("03000001");
	}
	else
	{
        N_AnalyseString((char*)lpResult->buffer, FormNameList);
        LOGINFO("函数：%s，FormNameList = %s",__FUNCTION__, Q2S(FormNameList));
	}

	if(lpResult)
	{
        LFMFreeBuffer(lpResult);
	}

	return FormNameList;
}

SHORT CMRPT::UpdateFormInfoSync(QString FormName)
{
	LOGINFO("函数：%s",__FUNCTION__);
	
    LPSTR lpszFormName = new CHAR[FormName.length()+1];
    strcpy(lpszFormName, Q2S(FormName));

    LPLFSRESULT lpResult = NULL;
    HRESULT ret = m_apiCtrl.LFSGetInfo(LFS_INF_PTR_QUERY_FORM, NULL, HM_MIDDLE_TIMEOUT, &lpResult);

    if(LFS_SUCCESS == ret)
	{
        LPLFSFRMHEADER lpHeader =  (LPLFSFRMHEADER)(((LPLFSRESULT)lpResult)->buffer);
		if(lpHeader)
		{
            FormName = lpHeader->form_name;
            FormBase.sprintf("%d",  lpHeader->base);
            FormUnitX  = lpHeader->unit_x;
            FormUnitY  = lpHeader->unit_y;
            FormWidth  = lpHeader->width;
            FormHeight = lpHeader->height;
            FormAlignment.sprintf("%d", lpHeader->alignment);
            FormOrientation.sprintf("%d",lpHeader->orientation);
            FormOffsetX = lpHeader->offset_x;
            FormOffsetY = lpHeader->offset_y;
            FormVersionMajor = lpHeader->version_major;
            FormVersionMinor = lpHeader->version_minor;
            FormUserPrompt = lpHeader->user_prompt;

            FormFields.clear();
            N_AnalyseString(lpHeader->fields, FormFields);
		}
		else
		{
			ret = -1;
			LOGERROR("%s, SP数据为空。", __FUNCTION__);
			Alarm("03000002");
		}
		
	}
	else
	{
        QString szEvent;
        if(ret == LFS_ERR_PTR_FORMNOTFOUND)
		{
			szEvent = "FORMNOTFOUND";
		}
        else if(ret == LFS_ERR_PTR_FORMINVALID)
		{
			szEvent = "FORMINVALID";
		}
		else
		{
			szEvent = "OtherError";
		}
        LOGINFO("函数：%s，错误为：%s",__FUNCTION__, Q2S(szEvent));
		Alarm("03000001");
	}

    ::LFSFreeResult(lpResult);
    return ret;
}

QString CMRPT::GetMediaNamesSync(void)
{
	LOGINFO("函数：%s",__FUNCTION__);

    QString MediaNames;

    LPLFSRESULT lppResult = NULL;

    HRESULT ret = m_apiCtrl.LFSGetInfo(LFS_INF_PTR_MEDIA_LIST, NULL, HM_MIDDLE_TIMEOUT, &lppResult);
    if(LFS_SUCCESS != ret)
	{
        MediaNames.clear();
        LOGERROR("函数：%s,LFS_INF_PTR_MEDIA_LIST返回失败, ret=%d", __FUNCTION__, ret);
		Alarm("03000001");
	}
	else
	{
        N_AnalyseString((char*)lppResult->buffer, MediaNames);
        LOGINFO("函数：%s，MediaNames = %s",__FUNCTION__, Q2S(MediaNames));
	}

	if(lppResult)
	{
        LFMFreeBuffer(lppResult);
	}
    return Q2S(MediaNames);
}

SHORT CMRPT::UpdateMediaInfoSync(QString MediaName)
{
	LOGINFO("函数：%s",__FUNCTION__);

    LPLFSRESULT lppResult = NULL;
    LPLFSFRMMEDIA lpMedia = NULL;

    LPSTR lpszMediaName  = new CHAR[MediaName.length()+1];
    memcpy(lpszMediaName,Q2S(MediaName),MediaName.length());
    //strcpy(lpszMediaName, MediaName.GetBuffer(0));

    HRESULT ret =m_apiCtrl.LFSGetInfo(LFS_INF_PTR_QUERY_FORM, lpszMediaName, 0, &lppResult);
    if(LFS_SUCCESS!=ret)
	{
        LOGERROR("函数：%s,LFS_INF_PTR_QUERY_FORM返回失败, ret=%d", __FUNCTION__, ret);
		Alarm("03000001");
	}
	else
	{
        lpMedia =  (LPLFSFRMMEDIA)(lppResult->buffer);

		this->MediaName = MediaName;
        MediaType.sprintf("%d", lpMedia->media_type);
        MediaBase.sprintf("%d", lpMedia->base);
        MediaUnitX = lpMedia->unit_x;
        MediaUnitY = lpMedia->unit_y;
        MediaSizeWidth = lpMedia->size_width;
        MediaSizeHeight = lpMedia->size_height;
        MediaPageCount = lpMedia->page_count;
        MediaLineCount = lpMedia->line_count;
        MediaPrintAreaX = lpMedia->print_area_x;
        MediaPrintAreaY = lpMedia->print_area_y;
        MediaPrintAreaWidth = lpMedia->print_area_width;
        MediaPrintAreaHeight = lpMedia->print_area_height;
        MediaRestrictedAreaX = lpMedia->restricted_area_x;
        MediaRestrictedAreaY = lpMedia->restricted_area_y;
        MediaRestrictedAreaWidth = lpMedia->restricted_area_width;
        MediaRestrictedAreaHeight = lpMedia->restricted_area_height;
        MediaStagger = lpMedia->stagger;
       MediaFoldType =  QString::number(lpMedia->fold_type, 10);
       // MediaFoldType.sprintf(lpMedia->fold_type);
	}

	if(lpMedia)
	{
        LFMFreeBuffer(lpMedia);
	}
    return 0;
}

SHORT CMRPT::PrintFile(QString FileName, LONG Timeout, BOOL DeleteFile)
{
	LOGINFO("函数：%s",__FUNCTION__);
    QString line,lines;
    QFile m_file(FileName);
	try
	{
        BOOL isOpen = m_file.open(QIODevice::ReadWrite|QIODevice::Append|QIODevice::Truncate);

		if(!isOpen)
		{
            emit PrintFailed();
            LOGERROR("函数：%s，事件：PrintFailed, 打开文件失败，GetLastError=%d",__FUNCTION__,-1);
			Alarm("03000001");
		}
		else
		{
            QTextStream stream( &m_file );

            int n = 1;
            while ( !stream.atEnd() ) {
                line = stream.readLine(); // 不包括“\n”的一行文本
                lines += line;
               }
            LFSPTRRAWDATA RawData;
//			ULONG fSize;
//            fSize = m_file.size();

            LPBYTE fData = new BYTE[lines.length()+1];
            memcpy(fData,Q2US(lines),lines.length()+1);
           // m_file.read(fData, fSize);
            RawData.input_data = LFS_PTR_INPUTDATA;
            RawData.data = fData;
            RawData.size = lines.length();

			REQUESTID id=0;
            HRESULT ret = m_apiCtrl.LFSAsyncExecute(LFS_CMD_PTR_RAW_DATA, &RawData, Timeout, &id);

            m_file.close();
            QDir dir;
			if(DeleteFile)
			{
                dir.remove(FileName);
			}

            if(ret != LFS_SUCCESS)
			{
                emit PrintFailed();
				LOGERROR("函数：%s，事件：PrintFailed",__FUNCTION__);
				Alarm("03000001");
			}
		}
	}
    catch (...)
	{
        //e->Delete();
        emit PrintFailed();
	}

    return 0;
}

//user start

HRESULT CMRPT::N_Open()
{

	REQUESTID id=0;
    int ret = m_apiCtrl.LFSAsyncOpen(m_szLogicalName,0,&id);

    if(LFS_SUCCESS!=ret)
	{
		LOGERROR("%s,关闭失败！",__FUNCTION__);
		Alarm("03000000");
        emit OpenFailed();
	}

	return ret;
}

HRESULT CMRPT::N_Close()
{
	REQUESTID id=0;
    int ret = m_apiCtrl.LFSAsyncClose(&id);

    if(LFS_SUCCESS!=ret)
	{
		LOGERROR("%s,打开失败！",__FUNCTION__);
		Alarm("03000000");
        emit CloseFailed();
	}

	return ret;
}


HRESULT CMRPT::N_GetStatus()
{
    LPLFSRESULT lpResult = NULL;
    int ret = m_apiCtrl.LFSGetInfo(LFS_INF_PTR_STATUS, NULL, HM_MIDDLE_TIMEOUT, &lpResult);
    if(LFS_SUCCESS == ret)
	{
        LPLFSPTRSTATUS lpStatus = (LPLFSPTRSTATUS)((LPLFSRESULT)lpResult)->buffer;
		if(lpStatus)
		{
            memcpy(&m_Status,lpStatus,sizeof(LFSPTRSTATUS));
            N_SetDeviceStatus(m_Status.device);
		}
		else
		{
			ret = -1;
			LOGERROR("%s, SP数据为空。", __FUNCTION__);
			Alarm("03000002");
		}
	}
	else
	{
		LOGERROR("%s, 获取状态失败错误码为：%d", __FUNCTION__,ret);
		Alarm("03000001");
	}
    ::LFSFreeResult(lpResult);
	return ret;
}

HRESULT CMRPT::N_GetCaps()
{
    LPLFSRESULT lpResult = NULL;
    int ret = m_apiCtrl.LFSGetInfo(LFS_INF_PTR_CAPABILITIES, NULL, HM_MIDDLE_TIMEOUT, &lpResult);
    if(LFS_SUCCESS == ret)
	{
        LPLFSPTRCAPS lpCaps = (LPLFSPTRCAPS)((LPLFSRESULT)lpResult)->buffer;
		if(lpCaps)
		{
            memcpy(&m_Caps,lpCaps,sizeof(LFSPTRCAPS));
		}
		else
		{
			ret = -1;
			LOGERROR("%s, SP数据为空。", __FUNCTION__);
			Alarm("03000002");
		}
	}
	else
	{
		LOGERROR("%s, 获取状态失败错误码为：%d", __FUNCTION__,ret);
		Alarm("03000001");
	}
    ::LFSFreeResult(lpResult);
	return ret;
}

void CMRPT::N_SetDeviceStatus(WORD wStatus)
{
	m_PreDevStatus=m_DetailedDeviceStatus;
	switch(wStatus)
	{
        case LFS_PTR_DEVONLINE:
			m_DetailedDeviceStatus="ONLINE";
			break;
        case LFS_PTR_DEVOFFLINE:
        case LFS_PTR_DEVHWERROR:
			m_DetailedDeviceStatus="OFFLINE";
			break;
        case LFS_PTR_DEVPOWEROFF:
			m_DetailedDeviceStatus="POWEROFF";
			break;
        case LFS_PTR_DEVNODEVICE:
			m_DetailedDeviceStatus="NODEVICE";
			break;
        case LFS_PTR_DEVUSERERROR:
			m_DetailedDeviceStatus="USERERROR";
			break;
        case LFS_PTR_DEVBUSY:
			m_DetailedDeviceStatus="BUSY";
			break;
	}

	switch(wStatus)
	{
    case LFS_PTR_DEVONLINE:
		m_DeviceStatus = "HEALTHY";
		break;
    case LFS_PTR_DEVNODEVICE:
		m_DeviceStatus = "NODEVICE";
		break;
	default:
		m_DeviceStatus = "FATAL";
		break;
	}
}

/*
功能：解析字符串，例将  abc'\0'efg'\0'hij'\0''\0'  转换为 abc,efg,hij'\0'
*/
void CMRPT::N_AnalyseString(char *lps, QString &estring)
{
	if(!lps)
		return;

	int NullCharNumber = 0;
	int iStart = 0;
	int iEnd = 0;
	while(1)
	{
		if( lps[iEnd] == '\0' )
		{
			if(iStart != iEnd)
			{
				if(iStart != 0)
				{
					estring += ',';
				}

				estring += lps+iStart;
	
				iStart = iEnd + 1;		
			}

			NullCharNumber++;
		}
		else
		{
			NullCharNumber = 0;
		}
		
		iEnd++;

		if(NullCharNumber == 2)
			break;
	}

}

void CMRPT::FieldsConvert(QString krFieldValues, char *wsFieldValues)
{
    QString szfeild;

	int itcma = 0;
	int itstart = itcma;

	while(1)
	{
        if((itcma = krFieldValues.indexOf(',', itcma)) != -1)
		{
            if(krFieldValues.at(itcma-1) == '/') //这个逗号是字段内容，不是字段分隔符。
			{
                krFieldValues.remove(itcma-1); //删除 '/'
                //krFieldValues.ReleaseBuffer();
				
				continue; //继续查找
			}

            szfeild = krFieldValues.mid(itstart, itcma-itstart);//获取field

            strcpy(wsFieldValues+itstart, Q2S(szfeild));

			//改变游标，开始下一次查找。
			itcma++;
			itstart = itcma;
		}
		else
		{
            szfeild = krFieldValues.mid(itstart);

            strcpy(wsFieldValues+itstart, Q2S(szfeild));

            itstart += szfeild.length()+1;

			//再添加一个 '\0'，以'\0''\0'结束
			wsFieldValues[itstart] = '\0';

			break;
		}
	}
}

DWORD CMRPT::GetMediaControl(QString MediaAction)
{
	MediaActionMap ActionMap[] = { 
        {"EJECT",        LFS_PTR_CTRLEJECT},
        {"PERFORATE",    LFS_PTR_CTRLPERFORATE},
        {"CUT",          LFS_PTR_CTRLCUT},
        {"SKIP",         LFS_PTR_CTRLSKIP},
        {"FLUSH",        LFS_PTR_CTRLFLUSH},
        {"RETRACT",      LFS_PTR_CTRLRETRACT},
        {"STACK",        LFS_PTR_CTRLSTACK},
        {"PARTIALCUT",   LFS_PTR_CTRLPARTIALCUT},
        {"ALARM",        LFS_PTR_CTRLALARM},
        {"PAGEFORWARD",  LFS_PTR_CTRLATPFORWARD},
        {"PAGEBACKWARD", LFS_PTR_CTRLATPBACKWARD},
        {"TURNMEDIA",    LFS_PTR_CTRLTURNMEDIA},
        {"STAMP",        LFS_PTR_CTRLSTAMP},
        {"PARK",         LFS_PTR_CTRLPARK},
        //{"DROP",         LFS_PTR_CTRLEXPEL},
       // {"RELOAD",       LFS_PTR_CTRLEJECTTOTRANSPORT}
	};


	int arraylenght = 0;
	DWORD dwMediaControl = 0;

	arraylenght = sizeof(ActionMap)/sizeof(MediaActionMap);

	for(int i=0; i!=arraylenght; i++)
	{
        if(MediaAction.indexOf(ActionMap[i].szMediaAction) != -1)
		{
			dwMediaControl |= ActionMap[i].dwMediaAction;
		}
	}

	return dwMediaControl;
}

int CMRPT::M_GetFieldValue(QString FormName, QString &FieldValues)
{
	LOGINFO("函数：%s",__FUNCTION__);

	int itForm;
	for(itForm=0; itForm<FormNum; itForm++)
	{
		if(FormName == dstoForm[itForm].FormName)
			break;
	}

	if(itForm < FormNum)
	{
		for(int i=0; i<dstoForm[itForm].FieldNum; i++)
		{
            QString sValue;
			long lValue = 0;
			LPBYTE lpbyteValue = NULL;

			short sRet;
			if(dstoForm[itForm].lpdstoField[i].DataType == "STRING")
			{
				sRet = N_GetDataFromDSTO(dstoForm[itForm].lpdstoField[i].bPersistent, dstoForm[itForm].lpdstoField[i].FieldName, sValue);
			}
			else if(dstoForm[itForm].lpdstoField[i].DataType == "LONG")
			{
				sRet = N_GetDataFromDSTO(dstoForm[itForm].lpdstoField[i].bPersistent, dstoForm[itForm].lpdstoField[i].FieldName, lValue);
			}
			else if(dstoForm[itForm].lpdstoField[i].DataType == "LPBYTE")
			{
				sRet = N_GetDataFromDSTO(dstoForm[itForm].lpdstoField[i].bPersistent, dstoForm[itForm].lpdstoField[i].FieldName, lpbyteValue);
			}

			//if(sRet)
			{
                QString FieldValue;

				if(dstoForm[itForm].lpdstoField[i].DataType == "STRING")
				{
					FieldValue = sValue;
				}
				else if(dstoForm[itForm].lpdstoField[i].DataType == "LONG")
				{
					//add by art begin
					if(dstoForm[itForm].lpdstoField[i].FieldName=="JNLNUM" || dstoForm[itForm].lpdstoField[i].FieldName=="CWCJNLNUM")
                        FieldValue.sprintf("%06d", lValue);
					else
					//add by art end
                        FieldValue.sprintf("%d", lValue);
				}
				else if(dstoForm[itForm].lpdstoField[i].DataType == "LPBYTE")
				{
                    FieldValue = US2Q(lpbyteValue);
				}

				//修改样式
				ChangeShowStyle(dstoForm[itForm].FormName, dstoForm[itForm].lpdstoField[i].FieldName, dstoForm[itForm].lpdstoField[i].SizeX, FieldValue);
	

				FieldValues += dstoForm[itForm].lpdstoField[i].FieldName;
				FieldValues += "=";
				FieldValues += FieldValue;

				if(i+1 < dstoForm[itForm].FieldNum)
				{
					FieldValues += ",";
				}
			}
		}

	}
	return 0;
}

void CMRPT::ChangeShowStyle(QString FormName, QString FieldName, int SizeX, QString &FieldValue)
{
	if(FormName == "ReceiptCash_Print_szABC" || FormName == "RejectCash_Print_szABC"
	  || FormName == "Receipt_Sales_szABC" || FormName == "Receipt_RTC_szABC" 
	  || FormName == "Receipt_CTR_szABC" || FormName == "Receipt_Trans_szABC" 
	  || FormName == "Receipt_ReDEPCash_szABC")
	{
		//给卡号后几位设置 * 号
        if(FieldName == "CARDNO" && FieldValue.length() > 5)
		{
			for(int i=2 ; i<=5; i++)
			{
                FieldValue.replace(FieldValue.length()-i, FieldValue.length(),"*");
			}
		}
	}

	if (FormName == "Receipt_Sales_szABC" || FormName == "Receipt_RTC_szABC" 
		|| FormName == "Receipt_CTR_szABC" || FormName == "Receipt_Trans_szABC")
	{
		// 隐藏客户姓氏
        if (FieldName == "CUSTOMERNAME" && FieldValue.length() > 2)
		{
            FieldValue.replace(0,1, "*");
		}
	}


	if(FormName == "TOTAL_szABC" || FormName == "SUBTOTAL_szABC"
		|| FormName == "TOTAL_szABC_haveReDep" || FormName == "SUBTOTAL_szABC_haveReDep")
	{
        if((FieldName == "ATMC_DISPENSEAMOUNT" || FieldName == "ATMC_DEPOSITAMOUNT") && FieldValue.length())
		{
            int AMOUNT = FieldValue.toInt();

			AMOUNT = AMOUNT / 100;  //把金额单位从“分”转换为“元”

            FieldValue.sprintf("%d.00", AMOUNT);
		}

        if(FieldName == "BOX1VALUEFMT" && FieldValue.length())
		{
			//钞箱1 纸币面额
			BOXVALUEFMT[0] = FieldValue;
		}
        if(FieldName == "BOX2VALUEFMT" && FieldValue.length())
		{
			//钞箱2 纸币面额
			BOXVALUEFMT[1] = FieldValue;
		}
        if(FieldName == "BOX3VALUEFMT" && FieldValue.length())
		{
			//钞箱3 纸币面额
			BOXVALUEFMT[2] = FieldValue;
		}
        if(FieldName == "BOX4VALUEFMT" && FieldValue.length())
		{
			//钞箱4 纸币面额
			BOXVALUEFMT[3] = FieldValue;
		}
        if(FieldName == "BOX5VALUEFMT" && FieldValue.length())
		{
			//钞箱5 纸币面额
			BOXVALUEFMT[4] = FieldValue;
		}
		//add by art for 双室钞箱 begin
        if(FieldName == "BOX6VALUEFMT" && FieldValue.length())
		{
			//钞箱6 纸币面额
			BOXVALUEFMT[5] = FieldValue;
		}
        if(FieldName == "BOX7VALUEFMT" && FieldValue.length())
		{
			//钞箱7 纸币面额
			BOXVALUEFMT[6] = FieldValue;
		}
		//add by art for 双室钞箱 end


        if(FieldName == "BOX1NOTENUMBERFMT" && FieldValue.length())
		{
			//钞箱1 剩余张数
			NOTENUM[0] = FieldValue;
		}
        if(FieldName == "BOX2NOTENUMBERFMT" && FieldValue.length())
		{
			//钞箱2 剩余张数
			NOTENUM[1] = FieldValue;
		}
        if(FieldName == "BOX3NOTENUMBERFMT" && FieldValue.length())
		{
			//钞箱3 剩余张数
			NOTENUM[2] = FieldValue;
		}
        if(FieldName == "BOX4NOTENUMBERFMT" && FieldValue.length())
		{
			//钞箱4 剩余张数
			NOTENUM[3] = FieldValue;
		}
        if(FieldName == "BOX5NOTENUMBERFMT" && FieldValue.length())
		{
			//钞箱5 剩余张数
			NOTENUM[4] = FieldValue;
		}
		//add by art for 双室钞箱 begin===
        if(FieldName == "BOX6NOTENUMBERFMT" && FieldValue.length())
		{
			//钞箱6 剩余张数
			NOTENUM[5] = FieldValue;
		}
        if(FieldName == "BOX7NOTENUMBERFMT" && FieldValue.length())
		{
			//钞箱7 剩余张数
			NOTENUM[6] = FieldValue;
		}
		
	}

	if(FormName == "ReceiptCash_Print_szABC" || FormName == "RejectCash_Print_szABC")
	{
        if(FieldName == "TRANSAMOUNT" && FieldValue.length())
		{
            int TRANSAMOUNT = FieldValue.toInt(); //这里是以分为单位,无小数点

			//comment by art begin for 严重BUG!!!，没有精确到分!
			//TRANSAMOUNT = TRANSAMOUNT / 100;  //把金额单位从“分”转换为“元”
            //FieldValue.sprintf("%d.00", TRANSAMOUNT);
			//comment by art end

			//add by art begin for 转为以元为单位，带小数点
			int nYuan = TRANSAMOUNT /  100;
			int nFen = TRANSAMOUNT %  100;
            FieldValue.sprintf("%d.%02d", nYuan, nFen);
			//add by art bend

		}
		
        if(FieldName == "POUNDAGE" && FieldValue.length())
		{
            int POUNDAGE = FieldValue.toInt();

			//comment by art begin for 严重BUG!!!，没有精确到分!
			//POUNDAGE = POUNDAGE / 100;  //把金额单位从“分”转换为“元”
            //FieldValue.sprintf("%d.00", POUNDAGE);
			//comment by art end

			//add by art begin for 转为以元为单位，带小数点
			int nYuan = POUNDAGE /  100;
			int nFen = POUNDAGE %  100;
            FieldValue.sprintf("%d.%02d", nYuan, nFen);
			//add by art bend

		}
	}

	if(FormName == "ReceiptBillLoadingCountSet_szABC")
	{
        if(FieldName == "BOX1VALUEFMT" && FieldValue.length())
		{
			//钞箱1 纸币面额
			BOXVALUEFMT[0] = FieldValue;
		}
        if(FieldName == "BOX2VALUEFMT" && FieldValue.length())
		{
			//钞箱2 纸币面额
			BOXVALUEFMT[1] = FieldValue;
		}
        if(FieldName == "BOX3VALUEFMT" && FieldValue.length())
		{
			//钞箱3 纸币面额
			BOXVALUEFMT[2] = FieldValue;
		}
        if(FieldName == "BOX4VALUEFMT" && FieldValue.length())
		{
			//钞箱4 纸币面额
			BOXVALUEFMT[3] = FieldValue;
		}
        if(FieldName == "BOX5VALUEFMT" && FieldValue.length())
		{
			//钞箱5 纸币面额
			BOXVALUEFMT[4] = FieldValue;
		}
		//add by art for 双室钞箱 begin===
        if(FieldName == "BOX6VALUEFMT" && FieldValue.length())
		{
			//钞箱6 纸币面额
			BOXVALUEFMT[5] = FieldValue;
		}
        if(FieldName == "BOX7VALUEFMT" && FieldValue.length())
		{
			//钞箱7 纸币面额
			BOXVALUEFMT[6] = FieldValue;
		}
		//add by art for 双室钞箱 end===

        if(FieldName == "ENCASEBILLS1" && FieldValue.length())
		{
			//钞箱1 加钞张数
			NOTENUM[0] = FieldValue;
		}
        if(FieldName == "ENCASEBILLS2" && FieldValue.length())
		{
			//钞箱2 加钞张数
			NOTENUM[1] = FieldValue;
		}
        if(FieldName == "ENCASEBILLS3" && FieldValue.length())
		{
			//钞箱3 加钞张数
			NOTENUM[2] = FieldValue;
		}
        if(FieldName == "ENCASEBILLS4" && FieldValue.length())
		{
			//钞箱4 加钞张数
			NOTENUM[3] = FieldValue;
		}
        if(FieldName == "ENCASEBILLS5" && FieldValue.length())
		{
			//钞箱5 加钞张数
			NOTENUM[4] = FieldValue;
		}
		//add by art for 双室钞箱 end===
        if(FieldName == "ENCASEBILLS6" && FieldValue.length())
		{
			//钞箱6 加钞张数
			NOTENUM[5] = FieldValue;
		}
        if(FieldName == "ENCASEBILLS7" && FieldValue.length())
		{
			//钞箱7 加钞张数
			NOTENUM[6] = FieldValue;
		}
		//add by art for 双室钞箱 end===
	}

	if(FormName == "ExChangeMoney_Print_szABC")
	{
		//test
		//N_SetDataToDSTO(false, "EXCHANGEBILLCOUNTIN", "100:50,50:1,20:2,1:5");
		//N_SetDataToDSTO(false, "EXCHANGEBILLCOUNTOUT", "100:50,50:1,20:2,10:19,1:5");
		//test

        if((FieldName == "EXCHANGEBILLCOUNTIN" || FieldName == "EXCHANGEBILLCOUNTOUT") && FieldValue.length())
		{
            QStringList FieldArray;
            N_QStringtok(FieldValue, ",", FieldArray);

            QString RMB100 = "0";
            QString RMB50  = "0";
            QString RMB20  = "0";
            QString RMB10  = "0";
            QString RMB5   = "0";
            QString RMB1   = "0";

            for(int i=0; i<FieldArray.size(); i++)
			{
                QStringList des;
                N_QStringtok(FieldArray.at(i), ":", des);

                if(des.at(0) == "100")
				{
                    RMB100 = des.at(1);
				}
                else if(des.at(0) == "50")
				{
                    RMB50 = des.at(1);
				}
                else if(des.at(0) == "20")
				{
                    RMB20 = des.at(1);
				}
                else if(des.at(0) == "10")
				{
                    RMB10 = des.at(1);
				}
                else if(des.at(0) == "5")
				{
                    RMB5 = des.at(1);
				}
                else if(des.at(0) == "1")
				{
                    RMB1 = des.at(1);
				}
			}

			AddSpace(RMB100, 5);
			AddSpace(RMB50, 5);
			AddSpace(RMB20, 5);
			AddSpace(RMB10, 5);
			AddSpace(RMB5, 5);
			AddSpace(RMB1, 5);
			FieldValue = RMB100+RMB50+RMB20+RMB10+RMB5+RMB1;
		}

        if(FieldName == "TRANSAMOUNT" && FieldValue.length())
		{
            int TRANSAMOUNT = FieldValue.toInt();

			//comment by art begin for 严重BUG!!!，没有精确到分!
			//TRANSAMOUNT = TRANSAMOUNT / 100;  //把金额单位从“分”转换为“元”
            //FieldValue.sprintf("%d.00", TRANSAMOUNT);
			//comment by art end

			//add by art begin for 转为以元为单位，带小数点
			int nYuan = TRANSAMOUNT /  100;
			int nFen = TRANSAMOUNT %  100;
            FieldValue.sprintf("%d.%02d", nYuan, nFen);
			//add by art bend
		}
	}

	// 取款失败回存交易凭条处理
	if ( FormName == "Receipt_ReDEPCash_szABC")
	{
		if((FieldName == "TRANSAMOUNT" || FieldName == "DISPENAMOUNT" || FieldName == "TAKENAMOUNT")
            && FieldValue.length())
		{
            int TRANSAMOUNT = FieldValue.toInt();

			int nYuan = TRANSAMOUNT /  100;
			int nFen = TRANSAMOUNT %  100;
            FieldValue.sprintf("%d.%02d", nYuan, nFen);
		}

        if (FieldName == "JNLNUM2" && FieldValue.length())
		{
            int iJulnum = FieldValue.toInt();
            FieldValue.sprintf("%06d", iJulnum);
		}
	}

	// 转账交易凭条处理
	if (FormName == "Receipt_Trans_szABC")
	{
        if (FieldName == "TRANSAMOUNT" && FieldValue.length())
		{
            int TRANSAMOUNT = FieldValue.toInt();

			int nYuan = TRANSAMOUNT /  100;
			int nFen = TRANSAMOUNT %  100;
            FieldValue.sprintf("%d.%02d", nYuan, nFen);
		}

        if (FieldName == "TFRBANK" && FieldValue.length() >= 40)   // 避免打印银行名称时字段过程导致报错
		{
			char szTmp[40];
			memset(szTmp, 0, sizeof(szTmp));
            int iTmp = FieldValue.indexOf("行");
            memcpy(szTmp, Q2S(FieldValue), iTmp+2);
            FieldValue.sprintf("%s", szTmp);
		}
	}

	// 销户凭条数据处理
	if (FormName == "Receipt_Sales_szABC")
	{
        if(FieldName == "POUNDAGE" && FieldValue.length())
		{
            int POUNDAGE = FieldValue.toInt();

			int nYuan = POUNDAGE /  100;
			int nFen = POUNDAGE %  100;
            FieldValue.sprintf("%d.%02d", nYuan, nFen);
		}
	}

	// 活期转定期|活期转定期 凭条数据处理
	if (FormName == "Receipt_CTR_szABC" || FormName == "Receipt_RTC_szABC")
	{
        if (FieldName == "TRANSAMOUNT" && FieldValue.length() && FormName == "Receipt_CTR_szABC")
		{
            int TRANSAMOUNT = FieldValue.toInt();

			int nYuan = TRANSAMOUNT /  100;
			int nFen = TRANSAMOUNT %  100;
            FieldValue.sprintf("%d.%02d", nYuan, nFen);
		}

        if (FieldName == "DEALTYPE" && FieldValue.length())
		{
            int iDealType = FieldValue.toInt();
			if (1 == iDealType)
			{
                FieldValue.sprintf("活期转定期");
			}
			else if(2 == iDealType)
			{
                FieldValue.sprintf("定期转活期");
			}
		}

        if (FieldName == "WITHDRAWING" && FieldValue.length())
		{
			// 目前只支持全部提取
            FieldValue.sprintf("全部提取");
		}

        if ((FieldName == "DEPOSIT" || FieldName == "AGREEMENT" )&& FieldValue.length())
		{
            if (!QString::compare(FieldValue,"M003"))
			{
                FieldValue.sprintf("三个月");
			}
            else if (!QString::compare(FieldValue,"M006"))
			{
                FieldValue.sprintf("六个月");
			}
            else if (!QString::compare(FieldValue,"Y001"))
			{
                FieldValue.sprintf("一年");
			}
            else if (!QString::compare(FieldValue,"Y002"))
			{
                FieldValue.sprintf("两年");
			}
            else if (!QString::compare(FieldValue,"Y003"))
			{
                FieldValue.sprintf("三年");
			}
            else if (!QString::compare(FieldValue,"Y005"))
			{
                FieldValue.sprintf("五年");
			}
}
        if (FieldName == "DEALTYPEFLAG" && FieldValue.length())
		{
            int iTransType = FieldValue.toInt();
			if (1 == iTransType)
			{
                FieldValue.sprintf("预约转存");
			}
			else if(2 == iTransType)
			{
                FieldValue.sprintf("不预约转存");
			}
		}
	}

	// 微信销户凭条处理
	if (FormName == "Receipt_SalesForWX_szABC")
	{
// 		if (FieldName == "COMMENTS"	&& FieldValue.length() == 0)
// 		{
// 			FieldValue.sprintf("C:\\TCR\\jsapp\\Framework\\QRCode.png");
// 		}
	}

	AddSpace(FieldValue, SizeX);
}

bool CMRPT::N_GetDataFromDSTO(BOOL bPersistent, QString DataName, QString &Value)
{//modify by art: 修改原1024的限制，放大
	LOGINFO("函数：%s",__FUNCTION__);

	char szValue[10*1024]={0};
    short nRet = dllDSTO.D_GetDataString(bPersistent, Q2S(DataName),szValue);
		
	if(nRet!=0)
	{
        LOGERROR("函数：%s，ValueName = %s GetValueFalse",__FUNCTION__, Q2S(DataName));
		Alarm("03000003");
		return false;
	}
	Value = szValue;

    LOGINFO("函数：%s，ValueName = %s, bPersistent = %s, Value = %s",__FUNCTION__, Q2S(DataName), ((bPersistent)?("true"):("false")), szValue);
	return true;
}

bool CMRPT::N_GetDataFromDSTO(BOOL bPersistent, QString DataName, long &Value)
{
	LOGINFO("函数：%s",__FUNCTION__);

    short nRet = dllDSTO.D_GetDataLong(bPersistent, Q2S(DataName), &Value);
		
	if(nRet!=0)
	{
        LOGERROR("函数：%s，ValueName = %s GetValueFalse",__FUNCTION__, Q2S(DataName));
		Alarm("03000003");
		return false;
	}

    LOGINFO("函数：%s，ValueName = %s, bPersistent = %s, Value = %d",__FUNCTION__, Q2S(DataName), ((bPersistent)?("true"):("false")), Value);

	return true;
}
bool CMRPT::N_GetDataFromDSTO(BOOL bPersistent, QString DataName, LPBYTE Value)
{
	LOGINFO("函数：%s",__FUNCTION__);

    short nRet = dllDSTO.D_GetDataBytes(bPersistent, Q2S(DataName), Value);
		
	if(nRet!=0)
	{
        LOGERROR("函数：%s，ValueName = %s GetValueFalse",__FUNCTION__, Q2S(DataName));
		Alarm("03000003");
		return false;
	}

    LOGINFO("函数：%s，ValueName = %s, bPersistent = %s, Value = %s",__FUNCTION__, Q2S(DataName), ((bPersistent)?("true"):("false")), Value);

	return true;
}

bool CMRPT::N_SetDataToDSTO(BOOL bPersistent, QString DataName, QString Value)
{
	LOGINFO("函数：%s",__FUNCTION__);

    short nRet = dllDSTO.D_SetDataString(bPersistent, Q2S(DataName), Q2S(Value));
		
	if(nRet!=0)
	{
        LOGERROR("函数：%s，ValueName = %s SetValueFalse",__FUNCTION__, Q2S(DataName));
		Alarm("03000003");
		return false;
	}

    LOGINFO("函数：%s，ValueName = %s, bPersistent = %s, Value = %s",__FUNCTION__, Q2S(DataName), ((bPersistent)?("true"):("false")), Q2S(Value));

	return true;
}

bool CMRPT::N_SetDataToDSTO(BOOL bPersistent, QString DataName, long Value)
{
	LOGINFO("函数：%s",__FUNCTION__);

    short nRet = dllDSTO.D_SetDataLong(bPersistent, Q2S(DataName), Value);
		
	if(nRet!=0)
	{
        LOGERROR("函数：%s，ValueName = %s SetValueFalse",__FUNCTION__, Q2S(DataName));
		Alarm("03000003");
		return false;
	}

    LOGINFO("函数：%s，ValueName = %s, bPersistent = %s, Value = %d",__FUNCTION__, Q2S(DataName), ((bPersistent)?("true"):("false")), Value);

	return true;
}

bool CMRPT::N_SetDataToDSTO(BOOL bPersistent, QString DataName, LPBYTE Value, WORD wSize)
{
	LOGINFO("函数：%s",__FUNCTION__);

    short nRet = dllDSTO.D_SetDataBytes(bPersistent, Q2S(DataName), Value, wSize);
		
	if(nRet!=0)
	{
        LOGERROR("函数：%s，ValueName = %s SetValueFalse",__FUNCTION__, Q2S(DataName));
		Alarm("03000003");
		return false;
	}

    LOGINFO("函数：%s，ValueName = %s, bPersistent = %s, Value = %s",__FUNCTION__, Q2S(DataName), ((bPersistent)?("true"):("false")), Value);

	return true;
}

bool CMRPT::N_SetCurTimeToDB()
{
	LOGINFO("函数：%s",__FUNCTION__);

	//获取系统时间
    //CTime tCurr;
    //tCurr = CTime::GetCurrentTime();
     QDateTime EtTime = QDateTime::currentDateTime();
    QString HANDLE_DATE;
    QString HANDLE_TIME_S;
    HANDLE_DATE.sprintf("%04d/%02d/%02d",EtTime.date().year(),EtTime.date().month(),EtTime.date().day());
    //HANDLE_DATE.sprintf("%04d/%02d/%02d",tCurr.GetYear(),tCurr.GetMonth(), tCurr.GetDay());
    HANDLE_TIME_S.sprintf("%02d:%02d:%02d",EtTime.time().hour(),EtTime.time().minute(),EtTime.time().second());
   // HANDLE_TIME_S.sprintf("%02d:%02d:%02d",tCurr.GetHour(),tCurr.GetMinute(), tCurr.GetSecond());

    LOGINFO("函数：%s, CurDate = %s, CurTime = %s",__FUNCTION__, Q2S(HANDLE_DATE), Q2S(HANDLE_TIME_S));

	bool bret1 = N_SetDataToDSTO(false, "Date", HANDLE_DATE); 
	bool bret2 = N_SetDataToDSTO(false, "Time", HANDLE_TIME_S); 

	return bret1 && bret2;
}

void CMRPT::AddSpace(QString &Value, int SizeX)
{
    int ValueLen = Value.length();
	if(ValueLen < SizeX)
	{
		for(; ValueLen<SizeX; ValueLen++)
		{
			Value += " ";
		}
	}
}

/***************************
功能：切割字符串
参数：
QString src：源字符串
QString delim：分隔符
QStringList &desArr:输出数组
***************************/
void CMRPT::N_QStringtok(QString src, QString delim, QStringList &desArr)
{
	int itcma = 0;
	int itstart = itcma;

    if(src.length()>0)
	{
		while(1) 
		{
            if((itcma = src.indexOf(Q2S(delim), itcma)) != -1)
			{
                desArr.join(src.mid(itstart, itcma-itstart));
	
				//改变游标，开始下一次查找。
                itcma += delim.length();
				itstart = itcma;
			}
			else
			{
                desArr.join(src.mid(itstart));
				break;
			}
		}
	}
}

//QString BOXVALUEFMT[5]; //钞箱1、2、3、4、5的纸币面额
//QString NOTENUM[5]; //钞箱1、2、3、4、5的纸币张数
//QString BOXAMOUNT[5]; //钞箱1、2、3、4、5的金额
//QString ALLBOXNOTEAMOUNT; //钞箱总金额
QString CMRPT::CountBoxAount(QString FormName) //凭条：计算个钞箱金额、总金额
{
	LOGINFO("函数：%s",__FUNCTION__);

    QString FieldValues;

	if(FormName == "TOTAL_szABC" || FormName == "SUBTOTAL_szABC" || FormName == "ReceiptBillLoadingCountSet_szABC"
		|| FormName == "TOTAL_szABC_haveReDep" || FormName == "SUBTOTAL_szABC_haveReDep")
	{
		int AllboxCount = 0; //钞箱总金额

		//计算钞箱金额
		for(int i=0; i<7; i++) //modify by art for 双室钞箱
		{
            int NoteVALUEFMT = BOXVALUEFMT[i].toInt();//纸币面额
            int NoteNum = NOTENUM[i].toInt(); //纸币张

			int Amount = NoteNum*NoteVALUEFMT;  //金额
			//add by art begin for 计算回收箱总金额
			if( (i+1==4) && NoteNum>0 ) //回收箱且张数大于0
			{			
				LONG nABAmount=0;
				N_GetDataFromDSTO(false,"ABAMOUNT", nABAmount);
				Amount=nABAmount;
                BOXAMOUNT[i].sprintf("%d.00 ?", Amount);
			}
			//add by art end for 计算回收箱总金额
            BOXAMOUNT[i].sprintf("%d.00", Amount);

			AllboxCount += Amount; //钞箱总金额

            QString FieldName;
            FieldName.sprintf("%s%d%s", "BOX",i+1,"AMOUNT");

			FieldValues += FieldName;
			FieldValues += "=";
			FieldValues += BOXAMOUNT[i];

            LOGINFO("函数：%s，ValueName = %s FieldValues=%s",__FUNCTION__, Q2S(FieldName), Q2S(BOXAMOUNT[i]));

			FieldValues += ",";

		}

        ALLBOXNOTEAMOUNT.sprintf("%d.00", AllboxCount);

		FieldValues += "ALLBOXNOTEAMOUNT";
		FieldValues += "=";

		FieldValues += ALLBOXNOTEAMOUNT;

        LOGINFO("函数：%s，ValueName = %s FieldValues=%s",__FUNCTION__, "ALLBOXNOTEAMOUNT", Q2S(ALLBOXNOTEAMOUNT));
	}

	return FieldValues;
}


//user end
int CMRPT::Execute(int ID, LPVOID lpData)
{
    int nRet = 0;
    switch (ID)
    {
        case LFS_INF_PTR_STATUS:
            {
                nRet = N_GetStatus();
                if (LFS_SUCCESS == nRet)
                {
                    memcpy(lpData, &m_Status, sizeof(LFSCIMSTATUS));//2017.12.06.01
                }

                /*N_GetStatus();
                memcpy(lpData, m_DetailedDeviceStatus, m_DetailedDeviceStatus.length());
                */
            }

            break;

        default:
            break;
    }
    //return 0;
    return nRet;
}
//void CMRPT::PrintComplete(void){}
//void CMRPT::PrintFailed(void){}
//void CMRPT::ControlComplete(void){}
//void CMRPT::ControlFailed(void){}
//void CMRPT::PrintTaken(void){}
//void CMRPT::WaitCancelled(void){}
//void CMRPT::ResetComplete(void){}
//void CMRPT::ResetFailed(void){}



QObject *CreateInstance()
{
    return CMRPT::GetInstance();
}
