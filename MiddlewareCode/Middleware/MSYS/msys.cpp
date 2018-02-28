#include "msys.h"
#include <AlarmLib.h>
#include "easytrace.h"
#include "../Helper/helper.h"
#include "win_type_def.h"
#include "common_def.h"

CMSYS::CMSYS()
{
}
// 以下为方法的处理函数
SHORT CMSYS::Reset(void)
{
    // 	KillProcessFromName("HMSetup.exe");
    Alarm("00000000");//清空故障码
    return 0;
}
BOOL CMSYS::SendMsg2HMSETUP(int nMsgType)
{
//    HWND hSetup = FindWindow(NULL, "HMSETUP");
//    if (hSetup)
//    {
//        if (!PostMessage(hSetup, WM_USER + nMsgType, 0, 0))
//        {
//            LOGERROR("关机消息抛送给HMSETUP失败。");
//            Alarm("M0030004");
//            return FALSE;
//        }
//    }
//    else
//    {
//        LOGERROR("没有找到HMSETUP。");
//        Alarm("M0030004");
//        return FALSE;
//    }
    return TRUE;
}
SHORT CMSYS::ShutDown(void)
{
#if 0
    OSVERSIONINFO   oi;
    oi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&oi);

    //如果是NT/2000下需先设置相关权限   
    if (oi.dwPlatformId == VER_PLATFORM_WIN32_NT)
    {
        HANDLE   handle;
        TOKEN_PRIVILEGES   tkp;

        OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &handle);
        LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);

        tkp.PrivilegeCount = 1;     //   one   privilege   to   set           
        tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

        AdjustTokenPrivileges(handle, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
    }

    //WIN98下关机或注销时可直接调用下面涵数既可   
    ::ExitWindowsEx(EWX_SHUTDOWN, 0);

#endif
    return SendMsg2HMSETUP(MSYS_MSG_SHUTDOWN) ? 0 : -1;
}

SHORT CMSYS::Reboot(void)
{
#if 0
    OSVERSIONINFO   oi;
    oi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&oi);

    //如果是NT/2000下需先设置相关权限   
    if (oi.dwPlatformId == VER_PLATFORM_WIN32_NT)
    {
        HANDLE   handle;
        TOKEN_PRIVILEGES   tkp;

        OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &handle);
        LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);

        tkp.PrivilegeCount = 1;     //   one   privilege   to   set           
        tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

        AdjustTokenPrivileges(handle, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
    }
    //WIN98下关机或注销时可直接调用下面涵数既可   
    ::ExitWindowsEx(EWX_REBOOT, 0);

#endif
    return SendMsg2HMSETUP(MSYS_MSG_REBOOT) ? 0 : -1;
}
SHORT CMSYS::OpenService(void)
{
    this->ServiceOpened();
    return 0;
}

SHORT CMSYS::CloseService(void)
{
    this->ServiceClosed();
    return 0;
}
SHORT CMSYS::UpTransFile()
{
    char szBeUpLoadedDate[20] = { 0 };
    //已经上传过的文件日期
    g_Helper.GetPrivateProfileString("MSYS", "UpLoadDate", "",szBeUpLoadedDate, sizeof(szBeUpLoadedDate), Q2S(CONST_SETUPINIFILE));
//    if (0 == strlen(szBeUpLoadedDate))
//    {//取不到就用目录创建时间的前一天
//        HANDLE hFile = CreateFile(TRANS_PATH, GENERIC_READ,          // open for reading
//            FILE_SHARE_READ,       // share for reading
//            NULL,                            // default security
//            OPEN_EXISTING,          // existing file only
//            FILE_FLAG_BACKUP_SEMANTICS, // normal file
//            NULL);
//        FILETIME ftCreate, ftModify, ftAccess;
//        if (!GetFileTime(hFile, &ftCreate, &ftAccess, &ftModify))
//        {
//            UploadFileFailed();
//            return -1;
//        }
//        CTime temp(ftCreate);
//        temp = temp - CTimeSpan(1, 0, 0, 0);
//        sprintf_s(szBeUpLoadedDate, "%04d%02d%02d", temp.GetYear(), temp.GetMonth(), temp.GetDay());
//    }

//    tm tmBeUpLoadedDate;
//    digit2time(szBeUpLoadedDate, strlen(szBeUpLoadedDate), &tmBeUpLoadedDate);

//    //已经上传过的文件日期
//    CTime ctBeUpLoadedDate(tmBeUpLoadedDate.tm_year, tmBeUpLoadedDate.tm_mon, tmBeUpLoadedDate.tm_mday, 0, 0, 0);
//    LOGINFO("已经上传过的文件日期:%04d%02d%02d.", ctBeUpLoadedDate.GetYear(), ctBeUpLoadedDate.GetMonth(), ctBeUpLoadedDate.GetDay());

//    int nUploadMode = GetPrivateProfileInt("MSYS", "UploadMode", 2, SETUP_CFG);
//    if (2 == nUploadMode)
//    {//2=仅上传当天文件
//        CTime ctToday = CTime::GetCurrentTime();
//        /*允许重复传
//        if (ctToday.GetYear()==ctBeUpLoadedDate.GetYear()&&ctToday.GetMonth()==ctBeUpLoadedDate.GetMonth()&&ctToday.GetDay()==ctBeUpLoadedDate.GetDay())
//        {
//            LOGINFO("已上传最新日志流水文件.");
//            UploadFileSucceed();
//            return 0;
//        }
//        else
//        */
//        {
//            BOOL bRet = UpTransFile((ctToday - CTimeSpan(1, 0, 0, 0)), ctToday);
//            if (bRet)
//                UploadFileSucceed();
//            else
//                UploadFileFailed();
//            return bRet ? 0 : -2;
//        }
//    }
//    else
//    {//1=上传今天之前的文件（不包括今天）
//        //最近（昨天）的文件日期
//        CTime ctLastDate(CTime::GetCurrentTime() - CTimeSpan(1, 0, 0, 0));
//        LOGINFO("最近（昨天）的文件日期:%04d%02d%02d.", ctLastDate.GetYear(), ctLastDate.GetMonth(), ctLastDate.GetDay());
//        BOOL bRet = UpTransFile(ctBeUpLoadedDate, ctLastDate);
//        if (bRet)
//            UploadFileSucceed();
//        else
//            UploadFileFailed();
//        return bRet ? 0 : -3;
//    }
}
SHORT CMSYS::OpenFrontPage(void)
{
    if (GetIEWindowHwnd())
    {
        ShowIEWindow(m_oplHwnd);
        HideIEWindow(m_splHwnd);
        OpenFrontPageComplete();
        LOGINFO("后屏切前屏成功，发送事件OpenFrontPageComplete");
    }

    return 0;
}

SHORT CMSYS::OpenManagePage(void)
{
    if (GetIEWindowHwnd())
    {
        ShowIEWindow(m_splHwnd);
        HideIEWindow(m_oplHwnd);
        OpenManagePageComplete();
        LOGINFO("前屏切后屏成功，发送事件OpenManagePageComplete");
    }

    return 0;
}
SHORT CMSYS::StartPollingStatusCheckSync()
{
    /*LoadAllDll();
    RefreshStatus();
    int nStatusPollInterval = GetPrivateProfileInt("MSYS", "StatusPollInterval", SYS_STATUS_CHECK_GAP, SETUP_CFG);
    SetTimer(NULL, SYS_TIMER_ID_STATUS_CHECK, nStatusPollInterval*1000, StatusCheckTimerProc);*/
    return 0;
}
BOOL CMSYS::GetIEWindowHwnd()
{
    // 交易界面
//    m_oplHwnd = ::FindWindow("IEFrame", "index - Microsoft Internet Explorer");
//    // 管理界面
//    m_splHwnd = ::FindWindow("IEFrame", "Supervise - Microsoft Internet Explorer");

//    if (m_oplHwnd == NULL || m_splHwnd == NULL)
//    {
//        LOGERROR("未找到IE窗口，index窗口句柄%d, Supervise窗口句柄%d", m_oplHwnd, m_splHwnd);
//        return FALSE;
//    }

    return TRUE;
}

BOOL CMSYS::ShowIEWindow(HWND hwnd)
{
//    return ::SetWindowPos(hwnd, HWND_TOP, 0, 0, 1024, 768, SWP_SHOWWINDOW);
}

BOOL CMSYS::HideIEWindow(HWND hwnd)
{
//    return ::SetWindowPos(hwnd, HWND_TOP, 0, 10000, 1024, 768, NULL);
}

SHORT CMSYS::InitialSync(void)
{
//    WORD i;

//    //Get file path
//    wTransactionDiffer = GetPrivateProfileInt("Non-volatile",
//        "TransactionDiffer", 9999, MTRN_CFG);
//    if (wTransactionDiffer == 9999)
//    {
//        LOGINFO("InitialSync(void): Get-Non-volatile-TransactionDiffer=%d,检查%s", wTransactionDiffer, MTRN_CFG);
//        Alarm("M0030002");
//        return -1;
//    }
//    memset(sTrnDifferInfo, 0x00, sizeof sTrnDifferInfo);
//    memset(sTrnInfo, 0x00, sizeof sTrnInfo);

//    wTrnDifferNum = GetPrivateProfileInt("TransactionDifferTableSize",
//        "TrnDifferNum",
//        0,
//        MTRN_CFG);
//    if (wTrnDifferNum <= 0)
//    {
//        LOGINFO("InitialSync(void): Get-TransactionDifferTableSize-TrnDifferNum=%d,检查%s", wTrnDifferNum, MTRN_CFG);
//        Alarm("M0030002");
//        return -2;
//    }

//    //Read properties of transaction
//    for (i = 0; i < wTrnDifferNum; i++)
//    {
//        QString strTransactionDiffer("TransactionDiffer");
//        strTransactionDiffer.AppendFormat("%d", i + 1);
//        sTrnDifferInfo[i].wTransactionDiffer = GetPrivateProfileInt(strTransactionDiffer,
//            "TransactionDiffer", 0, MTRN_CFG);
//        sTrnDifferInfo[i].wSizeofbyTrnDifferName = (WORD)GetPrivateProfileString(strTransactionDiffer,
//            "TrnDifferName", " ",
//            (LPSTR)sTrnDifferInfo[i].byTrnDifferName,
//            sizeof sTrnDifferInfo[i].byTrnDifferName,
//            MTRN_CFG);
//        sTrnDifferInfo[i].wSizeofbyCancelTrnName = (WORD)GetPrivateProfileString(strTransactionDiffer,
//            "CancelTrnName", " ",
//            (LPSTR)sTrnDifferInfo[i].byCancelTrnName,
//            sizeof sTrnDifferInfo[i].byCancelTrnName,
//            MTRN_CFG);
//        sTrnDifferInfo[i].wSizeofbyTrnDifferAbbreviation = (WORD)GetPrivateProfileString(strTransactionDiffer,
//            "TrnDifferAbbreviation", " ",
//            (LPSTR)sTrnDifferInfo[i].byTrnDifferAbbreviation,
//            sizeof sTrnDifferInfo[i].byTrnDifferAbbreviation,
//            MTRN_CFG);
//        sTrnDifferInfo[i].wSizeofbyCancelTrnAbbreviation = GetPrivateProfileString(strTransactionDiffer,
//            "CancelTrnAbbreviation", " ",
//            (LPSTR)sTrnDifferInfo[i].byCancelTrnAbbreviation,
//            sizeof sTrnDifferInfo[i].byCancelTrnAbbreviation,
//            MTRN_CFG);
//        sTrnDifferInfo[i].wIcTrnType = GetPrivateProfileInt(strTransactionDiffer,
//            "IcTrnType", 0, MTRN_CFG);
//        sTrnDifferInfo[i].wEcService = GetPrivateProfileInt(strTransactionDiffer,
//            "EcService", 0, MTRN_CFG);
//        sTrnDifferInfo[i].wCashDirection = GetPrivateProfileInt(strTransactionDiffer,
//            "CashDirection", 0, MTRN_CFG);
//        sTrnDifferInfo[i].wOperateType = GetPrivateProfileInt(strTransactionDiffer,
//            "OperateType", 0, MTRN_CFG);
//        sTrnDifferInfo[i].wTrnMsg = GetPrivateProfileInt(strTransactionDiffer,
//            "TrnMsg", 0, MTRN_CFG);
//        sTrnDifferInfo[i].wCancelMsg = GetPrivateProfileInt(strTransactionDiffer,
//            "CancelMsg", 0, MTRN_CFG);
//        sTrnDifferInfo[i].wSizeofbyJnlFwdFmt = (WORD)GetPrivateProfileString(strTransactionDiffer,		//GV800027
//            "JnlFwdFmt", "",
//            (LPSTR)sTrnDifferInfo[i].byJnlFwdFmt,
//            sizeof(sTrnDifferInfo[i].byJnlFwdFmt),
//            MTRN_CFG);
//        sTrnDifferInfo[i].wSizeofbyCancelJnlFwdFmt = (WORD)GetPrivateProfileString(strTransactionDiffer,//GV800027
//            "CancelJnlFwdFmt",
//            "",
//            (LPSTR)sTrnDifferInfo[i].byCancelJnlFwdFmt,
//            sizeof(sTrnDifferInfo[i].byCancelJnlFwdFmt),
//            MTRN_CFG);
//        sTrnDifferInfo[i].wStatisticalType = GetPrivateProfileInt(strTransactionDiffer,			//GV800027
//            "StatisticalType", 0, MTRN_CFG);
//        sTrnDifferInfo[i].wCancelStatisticalType = GetPrivateProfileInt(strTransactionDiffer,		//GV800027
//            "CancelStatisticalType", 0, MTRN_CFG);
//        sTrnDifferInfo[i].wTransactionFile = GetPrivateProfileInt(strTransactionDiffer,			//GV800027
//            "TransactionFile", 0, MTRN_CFG);
//        sTrnDifferInfo[i].wSizeofbyNTransactionKind = (WORD)GetPrivateProfileString(strTransactionDiffer,//GV800027
//            "NTransactionKind", "",
//            (LPSTR)sTrnDifferInfo[i].byNTransactionKind,
//            sizeof(sTrnDifferInfo[i].byNTransactionKind),
//            MTRN_CFG);
//        sTrnDifferInfo[i].wNSummation = GetPrivateProfileInt(strTransactionDiffer,
//            "NSummation", 0, MTRN_CFG);
//        sTrnDifferInfo[i].wNSummationC = GetPrivateProfileInt(strTransactionDiffer,
//            "NSummationC", 0, MTRN_CFG);


//        QString StrDiffer, StrValue;
//        StrDiffer.Format("%d", sTrnDifferInfo[i].wTransactionDiffer);
//        sTrnDifferInfo[i].wTranLimitAmountRule = GetPrivateProfileInt(StrDiffer, "TranLimitAmount", 0, MTRNLIMIT_CFG);
//        // 		LOGINFO("获取wTranLimitAmountRule = %d,%s", sTrnDifferInfo[i].wTranLimitAmountRule, sTrnDifferInfo[i].byTrnDifferName);
//        if (sTrnDifferInfo[i].wTranLimitAmountRule > 0)
//        {
//            sTrnDifferInfo[i].wTranLimitAmountReal = sTrnDifferInfo[i].wTranLimitAmountRule;
//            StrValue.Format("%d", sTrnDifferInfo[i].wTranLimitAmountRule);
//            WritePrivateProfileString(strTransactionDiffer, "TranLimitAmount", StrValue, MTRN_CFG);
//            // 			LOGINFO("更新文件wTranLimitAmountRule = %d,%s", sTrnDifferInfo[i].wTranLimitAmountReal, sTrnDifferInfo[i].byTrnDifferName);
//        }
//        else
//        {
//            sTrnDifferInfo[i].wTranLimitAmountReal = GetPrivateProfileInt(StrDiffer, "TranLimitAmount", 0, MTRN_CFG);
//        }

//        sTrnDifferInfo[i].wAuthorizedAmountRule = GetPrivateProfileInt(StrDiffer, "AuthorizedAmount", 0, MTRNLIMIT_CFG);
//        // 		LOGINFO("获取wAuthorizedAmountRule = %d,%s", sTrnDifferInfo[i].wAuthorizedAmountRule, sTrnDifferInfo[i].byTrnDifferName);
//        if (sTrnDifferInfo[i].wAuthorizedAmountRule > 0)
//        {
//            sTrnDifferInfo[i].wAuthorizedAmountReal = sTrnDifferInfo[i].wAuthorizedAmountRule;
//            StrValue.Format("%d", sTrnDifferInfo[i].wAuthorizedAmountRule);
//            WritePrivateProfileString(strTransactionDiffer, "AuthorizedAmount", StrValue, MTRN_CFG);
//            // 			LOGINFO("更新文件wTranLimitAmountRule = %d,%s", sTrnDifferInfo[i].wAuthorizedAmountRule, sTrnDifferInfo[i].byTrnDifferName);
//        }
//        else
//        {
//            sTrnDifferInfo[i].wTranLimitAmountReal = GetPrivateProfileInt(StrDiffer, "AuthorizedAmount", 0, MTRN_CFG);
//        }
//    }

//    //Read possible transaction information
//    wTrnNum = GetPrivateProfileInt("TransactionTableSize",										//GV800027
//        "TransactionNum",
//        0,
//        MTRN_CFG);
//    if (wTrnNum <= 0)
//    {
//        LOGINFO("InitialSync(void): Get-TransactionTableSize-TransactionNum=%d,检查%s", wTrnNum, MTRN_CFG);
//        Alarm("M0030002");
//        return -3;
//    }
//    for (i = 0; i < wTrnNum; i++)
//    {
//        QString strTransaction("Transaction");
//        strTransaction.AppendFormat("%d", i + 1);
//        sTrnInfo[i].wTransactionDiffer = GetPrivateProfileInt(strTransaction,
//            "TransactionDiffer", 0, MTRN_CFG);
//        sTrnInfo[i].wSizeofbyTrnName = (WORD)GetPrivateProfileString(strTransaction,					//GV800027
//            "TransactionName", "",
//            (LPSTR)sTrnInfo[i].byTrnName,
//            sizeof(sTrnInfo[i].byTrnName),
//            MTRN_CFG);
//        sTrnInfo[i].wSizeofbyKocMode = (WORD)GetPrivateProfileString(strTransaction,					//GV800027 GV800074(QT#00012)
//            "KocMode", "",
//            (LPSTR)sTrnInfo[i].byKocMode,
//            sizeof(sTrnInfo[i].byKocMode),
//            MTRN_CFG);
//        sTrnInfo[i].wCardType = GetPrivateProfileInt(strTransaction,								//GV800027
//            "CardType", 0, MTRN_CFG);
//        sTrnInfo[i].wCashDirection = GetPrivateProfileInt(strTransaction,							//GV800027
//            "CashDirection", 0, MTRN_CFG);
//        sTrnInfo[i].wIdentityCard = GetPrivateProfileInt(strTransaction,							//GV800027
//            "IdentityCard", 0, MTRN_CFG);
//        sTrnInfo[i].wFingerPrint = GetPrivateProfileInt(strTransaction,							//GV800027
//            "FingerPrint", 0, MTRN_CFG);
//        sTrnInfo[i].wTransactionSupp = OK;													//GV800074(SK1#0047)
//        sTrnInfo[i].byTrnPossibility = NG;
//        sTrnInfo[i].byTrnPossibilityBk = NG;
//    }
    LOGINFO("Init初始化完成！");
    return 0;
}

QString CMSYS::GetPartsStatusSync(void)
{
//    GetModulesStatus();
//    GetCashUnitInfo();
//    CSZABCStatusMonitor sm;
//    return sm.GetPartsStatus(m_MapModulesStatus);
}

///////////////////////////////////////////////////////////////////////////////
// Function Type = External Function
// Function Name = Set possible transaction
// Function Description = Check the ATM, and find out the services that
//							it can provide at the current time
// Parameter = 
//			INPUT : WORD wMCUstate :MCU's state
//			INPUT : WORD wSPRstate :BRM's state
//          INPUT : WORD wSPRstate : PB's state
// Return Code = OK,NG
///////////////////////////////////////////////////////////////////////////////
QString CMSYS::PossibleTransactionSync(void)
{
//    GetModulesStatus();
//    WORD wMCUstate = ("ONLINE" == m_MapModulesStatus[SYS_CARD_READER]) ? 1 : 0;
//    QString strCIMStatus = m_MapModulesStatus[SYS_CASH_DEPOSIT];
//    QString strCDMStatus = m_MapModulesStatus[SYS_CASH_DISPENSE];
//    WORD wBRMstate = 1;
//    LOGINFO("strCIMStatus：%s,strCDMStatus：%s,fwAcceptor：%d,fwDispenser：%d,fwPositionStatusCIM: %d,fwPositionStatusCDM: %d",
//        strCIMStatus, strCDMStatus, fwAcceptor, fwDispenser, fwPositionStatusCIM, fwPositionStatusCDM);
//    if (SYS_DOOR_CLOSE != m_wDoorsStatus[2])
//    {
//        LOGERROR("金库门没关,m_wDoorsStatus[2]=%d", m_wDoorsStatus[2]);
//        Alarm("M0030007");
//        wBRMstate = 0;
//    }
//    else if (SYS_DOOR_CLOSE != m_wDoorsStatus[0] && SYS_DOOR_NONE != m_wDoorsStatus[0])
//    {
//        LOGERROR("入钞口门没关,m_wDoorsStatus[0]=%d", m_wDoorsStatus[0]);
//        Alarm("M003000A");
//        wBRMstate = 0;
//    }
//    else if (SYS_DOOR_CLOSE != m_wDoorsStatus[1] && SYS_DOOR_NONE != m_wDoorsStatus[1])
//    {
//        LOGERROR("出钞口门没关,m_wDoorsStatus[1]=%d", m_wDoorsStatus[1]);
//        Alarm("M003000B");
//        wBRMstate = 0;
//    } /*else if (WFS_CIM_PSEMPTY!=fwPositionStatusCIM){
//        LOGERROR("入钞口不为空");
//        Alarm("M003000C");
//        wBRMstate = 0;
//	} */else if (WFS_CDM_PSEMPTY != fwPositionStatusCDM)
//    {
//        LOGERROR("出钞口不为空");
//        Alarm("M003000D");
//        wBRMstate = 0;
//    }
//else if ("ONLINE" == strCIMStatus&&"ONLINE" == strCDMStatus
//    && (WFS_CIM_ACCOK == fwAcceptor || WFS_CIM_ACCCUSTATE == fwAcceptor)
//    && (WFS_CDM_DISPOK == fwDispenser || WFS_CDM_DISPCUSTATE == fwDispenser))
//{
//    wBRMstate = 1;
//}
//else if ("ONLINE" == strCIMStatus && (WFS_CIM_ACCOK == fwAcceptor || WFS_CIM_ACCCUSTATE == fwAcceptor))
//{
//    wBRMstate = 266;
//}
//else if ("ONLINE" == strCDMStatus && (WFS_CDM_DISPOK == fwDispenser || WFS_CDM_DISPCUSTATE == fwDispenser))
//{
//    wBRMstate = 265;
//}
//else
//{
//    wBRMstate = 0;
//}
//    WORD wPBstate = ("ONLINE" == m_MapModulesStatus[SYS_SWIPT_CARD_READER]) ? 1 : 0;
//    WORD wIDRstate = ("ONLINE" == m_MapModulesStatus[SYS_IDENTITY_CARD]) ? 1 : 0;
//    WORD wFPIstate = ("ONLINE" == m_MapModulesStatus[SYS_FINGER_PRINT]) ? 1 : 0;
//	WORD wRPTstate = ("ONLINE" == m_MapModulesStatus[SYS_RECEIPT_PRINT]) ? 1 : 0;//凭条打印
//    WORD	i;
//    WORD	wKocModeValue;//工注设定值
//    short   ret;
//    long    Posibledispense100amount = 0;
//    long    PosibledispenseOTHERamount = 0;
//    long    Remaindepsitamount = 0;

//    LOGINFO("MCU状态：%d,BRM状态：%d,PB状态：%d,IDR状态：%d,FPI状态：%d,RPT状态：%d", wMCUstate, wBRMstate, wPBstate, wIDRstate, wFPIstate, wRPTstate);

//    for (i = 0; i < wTrnNum; i++)
//    {
//        sTrnInfo[i].byTrnPossibility = OK;
//        sTrnInfo[i].wTransactionSupp = OK;
//    }

//    //根据MCU的状态，设定可用交易
//    switch (wMCUstate)
//    {
//    case MTRN_OK:
//        break;
//    case MTRN_NG:
//        for (i = 0; i < wTrnNum; i++)
//        {
//            if (sTrnInfo[i].wCardType == MTRN_CARD ||
//                (sTrnInfo[i].wCardType == MTRN_CARD_PB && wPBstate == MTRN_NG) ||
//                sTrnInfo[i].wCardType == MTRN_ICCARD)
//            {
//                sTrnInfo[i].byTrnPossibility = NG;
//            }
//        }
//        break;
//    case MTRN_ICNG:
//        for (i = 0; i < wTrnNum; i++)
//        {
//            if (sTrnInfo[i].wCardType == MTRN_ICCARD)
//            {
//                sTrnInfo[i].byTrnPossibility = NG;
//            }
//        }
//        break;
//    default:
//        return strNG;
//    }

//    //根据BRM设备的状态，设定可用交易
//    switch (wBRMstate)
//    {
//    case MTRN_IGNORE:
//    case MTRN_OK:
//        break;
//    case MTRN_NG:
//        for (i = 0; i < wTrnNum; i++)
//        {
//            if (sTrnInfo[i].wCashDirection == MTRN_CASHIN ||
//                sTrnInfo[i].wCashDirection == MTRN_CASHINANDOUT ||
//                sTrnInfo[i].wCashDirection == MTRN_CASHOUT)
//            {
//                sTrnInfo[i].byTrnPossibility = NG;
//            }
//        }
//        break;
//    case MTRN_CASHINNG:
//        for (i = 0; i < wTrnNum; i++)
//        {
//            if (sTrnInfo[i].wCashDirection == MTRN_CASHINANDOUT ||
//                sTrnInfo[i].wCashDirection == MTRN_CASHIN)
//            {
//                sTrnInfo[i].byTrnPossibility = NG;
//            }
//        }
//        break;
//    case MTRN_CASHOUTNG:
//    case MTRN_ABFUL:
//        for (i = 0; i < wTrnNum; i++)
//        {
//            if (sTrnInfo[i].wCashDirection == MTRN_CASHINANDOUT ||
//                sTrnInfo[i].wCashDirection == MTRN_CASHOUT)
//            {
//                sTrnInfo[i].byTrnPossibility = NG;
//            }
//        }
//        break;
//    default:
//        return strNG;
//    }

//    { //根据钞箱的空或满控制交易是否可用
//        ret = m_DSTO.D_GetDataLong(FALSE, g_pszPosibledispense100amount, &Posibledispense100amount);//获取钞箱可取金额
//        ret = m_DSTO.D_GetDataLong(FALSE, g_pszPosibledispenseOTHERamount, &PosibledispenseOTHERamount);//获取钞箱可取金额
//        ret = m_DSTO.D_GetDataLong(FALSE, "REMAINDEPOSITAMOUNT", &Remaindepsitamount);  ///获取钞箱可存金额
//        LOGINFO("可取金额Posibledispense100amount：%d,可取金额PosibledispenseOTHERamount：%d,可存金额Remaindepsitamount：%d", Posibledispense100amount, PosibledispenseOTHERamount, Remaindepsitamount);
//        if (Posibledispense100amount == 0 && 0 == PosibledispenseOTHERamount)
//        {              //钞箱为空是不支持取款
//            for (i = 0; i < wTrnNum; i++)
//            {
//                if (sTrnInfo[i].wCashDirection == MTRN_CASHINANDOUT ||
//                    sTrnInfo[i].wCashDirection == MTRN_CASHOUT)
//                {
//                    sTrnInfo[i].byTrnPossibility = NG;
//                }
//            }
//        }
//        if (Remaindepsitamount == 0)
//        {                 //钞箱为满时不支持存款
//            for (i = 0; i < wTrnNum; i++)
//            {
//                if (sTrnInfo[i].wCashDirection == MTRN_CASHINANDOUT ||
//                    sTrnInfo[i].wCashDirection == MTRN_CASHIN)
//                {
//                    sTrnInfo[i].byTrnPossibility = NG;
//                }
//            }
//        }
//    }

//    //根据PB设备的状态，设定可用交易
//    switch (wPBstate)
//    {
//    case MTRN_OK:
//        break;
//    case MTRN_NG:
//        for (i = 0; i < wTrnNum; i++)
//        {
//            if (sTrnInfo[i].wCardType == MTRN_PB ||
//                (sTrnInfo[i].wCardType == MTRN_CARD_PB && wMCUstate == MTRN_NG))
//            {
//                sTrnInfo[i].byTrnPossibility = NG;
//            }
//        }
//        break;
//    default:
//        return strNG;
//    }
//    //根据IDR设备的状态，设定可用交易
//    switch (wIDRstate)
//    {
//    case MTRN_OK:
//        break;
//    case MTRN_NG:
//        for (i = 0; i < wTrnNum; i++)
//        {
//            if (sTrnInfo[i].wIdentityCard == MTRN_NEED)
//            {
//                sTrnInfo[i].byTrnPossibility = NG;
//            }
//        }
//        break;
//    default:
//        return strNG;
//    }
//    //根据FPI设备的状态，设定可用交易
//    switch (wFPIstate)
//    {
//    case MTRN_OK:
//        break;
//    case MTRN_NG:
//        for (i = 0; i < wTrnNum; i++)
//        {
//            if (sTrnInfo[i].wFingerPrint == MTRN_NEED)
//            {
//                sTrnInfo[i].byTrnPossibility = NG;
//            }
//        }
//        break;
//    default:
//        return strNG;
//    }

//	//根据RPT（凭条打印）设备的状态，设定可用交易
//	switch (wRPTstate)
//	{
//	case MTRN_OK:
//		{
//			for (i = 0; i < wTrnNum; i++)
//			{
//				if (MTRN_ACCOUNTCANCELLATION == sTrnInfo[i].wTransactionDiffer)
//				{
//					LOGINFO("凭条打印机纸的状态:%s", m_strPaperState);//2017.12.05.1
//					if (m_strPaperState == "OUT" || m_strPaperState == "UNKNOWN")
//					{
//						sTrnInfo[i].byTrnPossibility = NG;
//					}

//				}
//			}

//		}
		
//		break;
//	case MTRN_NG:
//		for (i = 0; i < wTrnNum; i++)
//		{
//			if (MTRN_ACCOUNTCANCELLATION == sTrnInfo[i].wTransactionDiffer)
//			{
//				sTrnInfo[i].byTrnPossibility = NG;
//			}
//		}
//		break;
//	default:
//		return strNG;
//	}

//    //工作参数控制交易
//    //TODO:

//    //获取设定中的配置交易是否可用
//    for (i = 0; i < wTrnNum; i++)
//    {
//        QString strTransaction("Transaction");
//        strTransaction.AppendFormat("%d", i + 1);
//        wKocModeValue = GetPrivateProfileInt(strTransaction,					//GV800027
//            "TransactionSupport", 1,
//            MTransaction_CFG);
//        // 		LOGINFO("i = %d,wKocModeValue = %d",i,wKocModeValue);
//        if (wKocModeValue == 0)
//        {
//            sTrnInfo[i].byTrnPossibility = NG;
//            sTrnInfo[i].wTransactionSupp = NG;
//        }
//    }
//    //取得联网审核、指纹验证等等网络是否正常
//    //	CSZABCStatusMonitor statusmonitor;
//    //	statusmonitor.CheckConnect("127.0.0.1",3014);

//        //返回交易可用列表
    QString Str;
//    QString Str2;
//    for (i = 0; i < wTrnNum; i++)
//    {
//        Str += sTrnInfo[i].byTrnPossibility ? "1," : "0,";
//        Str2 += (char*)(sTrnInfo[i].byTrnName);
//        Str2 += sTrnInfo[i].byTrnPossibility ? "(1)," : "(0),";
//    }
//    LOGINFO("返回交易可用列表:%s，详细列表：%s", Str, Str2);
    return Str;
}

///////////////////////////////////////////////////////////////////////////////
// Function Type = External Function
// Function Name = Clear Item
// Function Description = Clear Item
// Parameter = 
//			INPUT : WORD wClearDiffer :clear differ
//				MTRN_CONTINUE : clear for continue trading
//				MTRN_USERCHANGE : clear for user change
//				MTRN_TRANSACTIONEND : clear for transaction end
//				MTRN_INITIALRESTOR : clear for initial restor
//			OUTPUT: LPWORD lpwReturn :return code
//										OK(normal)
//										NG(abnormal)
// Return Code = OK,NG
///////////////////////////////////////////////////////////////////////////////
SHORT CMSYS::ClearSync(SHORT ClearDiffer)
{
//    QString strTmp;

//    if (ClearDiffer == NULL)
//    {
//        LOGERROR("应用调用F_Clear传入参数为空");
//        Alarm("M0030009");
//        return NG;
//    }

//    strTmp = "";
//    switch (ClearDiffer)
//    {
//    case MTRN_CONTINUE:
//        wTransactionDiffer = MTRN_NONAME;															//GV800005(DD#00069)
//        break;
//        //case MTRN_USERCHANGE :
//        //wTransactionDiffer = MTRN_NONAME;															//GV800005(DD#00069)
//        //	break;
//    case MTRN_TRANSACTIONEND:
//        wTransactionDiffer = MTRN_NONAME;															//GV800005(DD#00069)
//        break;
//    case MTRN_INITIALRESTOR:
//        wTransactionDiffer = MTRN_NONAME;															//GV800005(DD#00069)
//        break;
//    default:
//        LOGERROR("应用调用F_Clear->wClearDiffer传入参数错误");
//        Alarm("M0030009");
//        return NG;
//    }

//    strTmp.Format("%d", wTransactionDiffer);
//    if (WritePrivateProfileString("Non-volatile", "TransactionDiffer", strTmp, MTRN_CFG) == 0)
//    {	//GV800027
//        LOGERROR("应用调用F_Clear发生错误");
//        Alarm("M0030008");
//        return NG;
//    }
//    return OK;
}

///////////////////////////////////////////////////////////////////////////////
// Function Type = External Function
// Function Name = Clear Item
// Function Description = Clear Item
// Parameter = 
//			INPUT : WORD wItem :clear differ
//				MTRN_POSSIBLETRN : clear possibility of transactions
//				MTRN_POSSIBLETRNBK : clear possibility of transactions(BK)
//				MTRN_TRANSACTIONDIFFER : clear transaction differ
//			OUTPUT: LPWORD lpwReturn :return code
//										OK(normal)
//										NG(abnormal)
// Return Code = OK,NG
///////////////////////////////////////////////////////////////////////////////, 
SHORT CMSYS::ItemClearSync(SHORT Item)
{
//    WORD	i;
//    CString strTmp;

//    if (Item == NULL)
//    {
//        LOGERROR("应用调用F_ItemClear传入参数错误");
//        Alarm("M0030009");
//        return NG;
//    }

//    switch (Item)
//    {
//    case MTRN_POSSIBLETRN:
//        for (i = 0; i < wTrnNum; i++)
//        {
//            sTrnInfo[i].byTrnPossibility = MTRN_NG;
//        }
//        break;
//    case MTRN_POSSIBLETRNBK:
//        for (i = 0; i < wTrnNum; i++)
//        {
//            sTrnInfo[i].byTrnPossibilityBk = MTRN_NG;
//        }
//        break;
//    case MTRN_TRANSACTIONDIFFER:
//        wTransactionDiffer = MTRN_NONAME;															//GV800005(DD#00069)
//        strTmp = "";
//        strTmp.Format("%d", wTransactionDiffer);
//        if (WritePrivateProfileString("Non-volatile", "TransactionDiffer", strTmp, MTRN_CFG) == 0)
//        {//GV800027
//            LOGERROR("应用调用F_ItemClear传入参数错误");
//            Alarm("M0030009");
//            return NG;
//        }
//        break;
//    default:
//        LOGERROR("应用调用F_ItemClear传入参数错误");
//        Alarm("M0030009");
//        return NG;
//    }
//    return OK;
}

///////////////////////////////////////////////////////////////////////////////
// Function Type = External Function
// Function Name = Set Data
// Function Description = Setting data to the specified item of WORD type
// Parameter = 
//			INPUT : WORD wItem :item name
//				MTRN_TRANSACTIONDIFFER : transaction Name
//			INPUT : WORD wValue :item value
// Return Code = OK,NG
///////////////////////////////////////////////////////////////////////////////
SHORT CMSYS::DataSetSync(SHORT Item, LONG Value)
{

//    CString strTmp;
//    strTmp = "";
//    switch (Item)
//    {
//    case MTRN_TRANSACTIONDIFFER:
//        wTransactionDiffer = (WORD)Value;
//        LOGINFO("设置交易类型：%d", Value);
//        break;
//    case MTRN_TRANLIMITAMOUNTRULE:
//        wTranLimitAmountRule = Value;
//        N_SetLimitAmountToIniFile(wTransactionDiffer, MTRN_TRANLIMITAMOUNTRULE, wTranLimitAmountRule);
//        LOGINFO("设置交易限额：%d", Value);
//        break;
//    case MTRN_AUTHORIZEDAMOUNTRULE:
//        wAuthorizedAmountRule = Value;
//        N_SetLimitAmountToIniFile(wTransactionDiffer, MTRN_AUTHORIZEDAMOUNTRULE, wAuthorizedAmountRule);
//        LOGINFO("设置授权限额：%d", Value);
//        break;
//    default:
//        return NG;
//    }

//    strTmp.Format("%d", wTransactionDiffer);
//    if (WritePrivateProfileString("Non-volatile", "TransactionDiffer", strTmp, MTRN_CFG) == 0)
//    {
//        return NG;
//    }
//    return OK;
}
///////////////////////////////////////////////////////////////////////////////
// Function Type = External Function
// Function Name = Get Data
// Function Description = Getting value from the specified item of WORD type
// Parameter = 
//			INPUT : WORD wItem :item name
//				MTRN_TRANSACTIONDIFFER : transaction name
//				MTRN_ICTRNTYPE : transaction identify for IC card
//				MTRN_ECSERVICE : mark for EC transactions
//				MTRN_OPERATETYPE : mark for inquiry/execute
//				MTRN_CASHDIRECTION : mark of deposit/withdraw
//				MTRN_TRNMSG : transaction message
//				MTRN_CANCELMSG : cancel message
//				MTRN_STATISTICALTYPE : statistical type
//				MTRN_CANCELSTATISTICALTYPE : statistical type of cancel transaction
//			OUTPUT: LPWORD lpwBuffer :item value
// Return Code = OK,NG
///////////////////////////////////////////////////////////////////////////////
LONG CMSYS::DataGetSync(SHORT Item)
{

//    short ret;
//    LONG sValue = 0;
//    if (Item == NULL)
//    {
//        LOGERROR("应用调用F_DataGet传入参数错误");
//        Alarm("M0030009");
//        return NG;
//    }

//    switch (Item)
//    {
//    case MTRN_TRANSACTIONDIFFER:
//        sValue = wTransactionDiffer;
//        break;
//    case MTRN_ICTRNTYPE:
//        if (N_QueryTableData(MTRN_ICTRNTYPE, &wIcTrnType) != OK)
//        {
//            LOGERROR("F_DataGet->N_QueryTableData(MTRN_ICTRNTYP):failed");
//            Alarm("M0030008");
//            return NG;
//        }
//        sValue = wIcTrnType;
//        break;
//    case MTRN_ECSERVICE:
//        if (N_QueryTableData(MTRN_ECSERVICE, &wEcService) != OK)
//        {
//            LOGERROR("F_DataGet->N_QueryTableData(MTRN_ECSERVICE):failed");																	//GST810B11
//            Alarm("M0030008");
//            return NG;
//        }
//        sValue = wEcService;
//        break;
//    case MTRN_OPERATETYPE:
//        if (N_QueryTableData(MTRN_OPERATETYPE, &wOperateType) != OK)
//        {
//            LOGERROR("F_DataGet->N_QueryTableData(MTRN_OPERATETYPE):failed");																	//GST810B11
//            Alarm("M0030008");
//            return NG;
//        }
//        sValue = wOperateType;
//        break;
//    case MTRN_CASHDIRECTION:
//        if (N_QueryTableData(MTRN_CASHDIRECTION, &wCashDirection) != OK)
//        {
//            LOGERROR("F_DataGet->N_QueryTableData(MTRN_CASHDIRECTION):failed");																	//GST810B11
//            Alarm("M0030008");
//            return NG;
//        }
//        sValue = wCashDirection;
//        break;
//    case MTRN_TRNMSG:
//        if (N_QueryTableData(MTRN_TRNMSG, &wTrnMsg) != OK)
//        {
//            LOGERROR("F_DataGet->N_QueryTableData(MTRN_TRNMSG):failed");																	//GST810B11
//            Alarm("M0030008");
//            return NG;
//        }
//        sValue = wTrnMsg;
//        break;
//    case MTRN_CANCELMSG:
//        if (N_QueryTableData(MTRN_CANCELMSG, &wCancelMsg) != OK)
//        {
//            LOGERROR("F_DataGet->N_QueryTableData(MTRN_CANCELMSG):failed");																	//GST810B11
//            Alarm("M0030008");
//            return NG;
//        }
//        sValue = wCancelMsg;
//        break;
//    case MTRN_STATISTICALTYPE:
//        if (N_QueryTableData(MTRN_STATISTICALTYPE, &wStatisticalType) != OK)
//        {
//            LOGERROR("F_DataGet->N_QueryTableData(MTRN_STATISTICALTYPE):failed");																	//GST810B11
//            Alarm("M0030008");
//            return NG;
//        }
//        sValue = wStatisticalType;
//        break;
//    case MTRN_CANCELSTATISTICALTYPE:
//        if (N_QueryTableData(MTRN_CANCELSTATISTICALTYPE, &wCancelStatisticalType) != OK)
//        {
//            LOGERROR("F_DataGet->N_QueryTableData(MTRN_CANCELSTATISTICALTYPE):failed");																	//GST810B11
//            Alarm("M0030008");
//            return NG;
//        }
//        sValue = wCancelStatisticalType;
//        break;
//    case MTRN_TRANLIMITAMOUNTREAL:
//        if (N_QueryTableData(MTRN_TRANLIMITAMOUNTREAL, &wTranLimitAmountReal) != OK)
//        {
//            LOGERROR("F_DataGet->N_QueryTableData(MTRN_TRANLIMITAMOUNTREAL):failed");																	//GST810B11
//            Alarm("M0030008");
//            return NG;
//        }
//        sValue = wTranLimitAmountReal;
//        LOGINFO("获取交易限额wTranLimitAmountReal=%d", sValue);
//        break;
//    case MTRN_AUTHORIZEDAMOUNTREAL:
//        if (N_QueryTableData(MTRN_AUTHORIZEDAMOUNTREAL, &wAuthorizedAmountReal) != OK)
//        {
//            LOGERROR("F_DataGet->N_QueryTableData(MTRN_AUTHORIZEDAMOUNTREAL):failed");																	//GST810B11
//            Alarm("M0030008");
//            return NG;
//        }
//        sValue = wAuthorizedAmountReal;
//        LOGINFO("获取授权限额wAuthorizedAmountReal=%d", sValue);
//        break;
//    case MTRN_TRANLIMITAMOUNTRULE:
//        if (N_QueryTableData(MTRN_TRANLIMITAMOUNTRULE, &wTranLimitAmountRule) != OK)
//        {
//            LOGERROR("F_DataGet->N_QueryTableData(MTRN_TRANLIMITAMOUNTREAL):failed");																	//GST810B11
//            Alarm("M0030008");
//            return NG;
//        }
//        sValue = wTranLimitAmountRule;
//        LOGINFO("获取交易限额wTranLimitAmountRule=%d", sValue);
//        break;
//    case MTRN_AUTHORIZEDAMOUNTRULE:
//        if (N_QueryTableData(MTRN_AUTHORIZEDAMOUNTRULE, &wAuthorizedAmountRule) != OK)
//        {
//            LOGERROR("F_DataGet->N_QueryTableData(MTRN_AUTHORIZEDAMOUNTREAL):failed");																	//GST810B11
//            Alarm("M0030008");
//            return NG;
//        }
//        sValue = wAuthorizedAmountRule;
//        LOGINFO("获取授权限额wAuthorizedAmountRule=%d", sValue);
//        break;
//    case  MTRN_REMAINDEPSITAMOUT:
//        ret = m_DSTO.D_GetDataLong(FALSE, "REMAINDEPOSITAMOUNT", &sValue);
//        LOGINFO("获取钞箱剩余可存金额为Remaindepostamount=%d", sValue);
//        break;
//    case  MSYS_RENAMEALARMFILE:
//        sValue = RenameAlarmFile();
//        break;
//    case  MSYS_BCDSTARTSERVICESYNC:     //北辰德影像软件启动
//        sValue = N_BcdStartServiceSync();
//        break;
//    case  MSYS_BCDENDSERVICESYNC:      //北辰德影像软件关闭
//        sValue = N_BcdEndServiceSync();
//        break;
//    default:
//        LOGERROR("F_DataGet: failed");																	//GST810B11
//        Alarm("M0030008");
//        return NG;
//    }
//    LOGINFO("F_DataGet: sValue = %d", sValue);
//    return sValue;
}
///////////////////////////////////////////////////////////////////////////////
// Function Type = External Function
// Function Name = Get Information
// Function Description = Getting value from the specified item of BYTE type
// Parameter = 
//			INPUT : WORD wItem :item name
//				MTRN_JNLFWDFMT : journal forward format
//				MTRN_CANCELJNLFWDFMT : journal forward format for cancel transaction
//				MTRN_TRNDIFFERNAME : transaction differ's name
//				MTRN_TRNDIFFERABBREVIATION : transaction differ's abbreviation
//				MTRN_CANCELTRNNAME : cancel transaction's name
//				MTRN_CANCELTRNABBREVIATION : abbreviation for cancel transaction differ
//				MTRN_TRNDIFFERNAMES : a collection of all transaction differ's names
//				MTRN_CANCELTRNNAMES : a collection of all cancel transaction's names
//				MTRN_TRNDIFFERABBREVIATIONS : a collection of all transaction differ's abbreviations
//				MTRN_CANCELTRNABBREVIATIONS : a collection of all cancel transaction's abbreviations
//				MTRN_TRNNAMES : a collection of all transaction's names
//				MTRN_TRNSPOSSIBILITY : possibility arrary of all transactions
//				MTRN_POSSIBLETRNNAMES : a collection of possible transaction's names
//			OUTPUT: LPBYTE lpbyBuffer :item value
//			INPUT : WORD wBufSize :buffer size
//			OUTPUT: LPWORD lpwDataSize :item size
//			OUTPUT: LPWORD lpwReturn :return code
//										OK(normal)
//										NG(abnormal)
// Return Code = OK,NG
///////////////////////////////////////////////////////////////////////////////
QString CMSYS::InfoGetSync(SHORT Item)
{
//    CString str;
//    switch (Item)
//    {
//    case MTRN_JNLFWDFMT:
//        wSizeofbyJnlFwdFmt = sizeof(byJnlFwdFmt);
//        if (N_QueryTableInfo(MTRN_JNLFWDFMT, byJnlFwdFmt, &wSizeofbyJnlFwdFmt) != 0)
//        {
//            return strNG;
//        }
//        str = CString((char*)byJnlFwdFmt, wSizeofbyJnlFwdFmt);
//        return str;
//        break;
//    case MTRN_CANCELJNLFWDFMT:
//        wSizeofbyCancelJnlFwdFmt = sizeof(byCancelJnlFwdFmt);
//        if (N_QueryTableInfo(MTRN_CANCELJNLFWDFMT, byCancelJnlFwdFmt, &wSizeofbyCancelJnlFwdFmt) != OK)
//        {
//            return strNG;
//        }
//        str = CString((char*)byCancelJnlFwdFmt, wSizeofbyCancelJnlFwdFmt);
//        return str;
//        break;
//    case MTRN_TRNDIFFERNAME:
//        wSizeofbyTrnDifferName = sizeof(byTrnDifferName);
//        if (N_QueryTableInfo(MTRN_TRNDIFFERNAME, byTrnDifferName, &wSizeofbyTrnDifferName) != OK)
//        {
//            return strNG;
//        }
//        str = CString((char*)byTrnDifferName, wSizeofbyTrnDifferName);
//        return str;
//        break;
//    case MTRN_CANCELTRNNAME:
//        wSizeofbyCancelTrnName = sizeof(byCancelTrnName);
//        if (N_QueryTableInfo(MTRN_CANCELTRNNAME, byCancelTrnName, &wSizeofbyCancelTrnName) != OK)
//        {
//            return strNG;
//        }
//        str = CString((char*)byCancelTrnName, wSizeofbyCancelTrnName);
//        return str;
//        break;
//    case MTRN_TRNDIFFERABBREVIATION:
//        wSizeofbyTrnDifferAbbreviation = sizeof(byTrnDifferAbbreviation);
//        if (N_QueryTableInfo(MTRN_TRNDIFFERABBREVIATION, byTrnDifferAbbreviation, &wSizeofbyTrnDifferAbbreviation) != OK)
//        {
//            return strNG;
//        }
//        str = CString((char*)byTrnDifferAbbreviation, wSizeofbyTrnDifferAbbreviation);
//        return str;
//        break;
//    case MTRN_CANCELTRNABBREVIATION:
//        wSizeofbyCancelTrnAbbreviation = sizeof(byCancelTrnAbbreviation);
//        if (N_QueryTableInfo(MTRN_CANCELTRNABBREVIATION, byCancelTrnAbbreviation, &wSizeofbyCancelTrnAbbreviation) != OK)
//        {
//            return strNG;
//        }
//        str = CString((char*)byCancelTrnAbbreviation, wSizeofbyCancelTrnAbbreviation);
//        return str;
//        break;
//    case MTRN_ERRORINFO:
//    {
//        int nLen = GetAlarm(NULL, 0);
//        if (0 < nLen)
//        {
//            char *pszErrorInfo = str.GetBuffer(nLen);
//            GetAlarm(pszErrorInfo, nLen);
//            str.ReleaseBuffer();
//        }
//        return str;
//    }
//    break;
//    default:
//        return strNG;
//    }
}
// 以下为属性的处理函数



BOOL CMSYS::GetIsBusying(void)
{
//    return m_bBusying;
}

void CMSYS::SetIsBusying(BOOL newVal)
{
//    m_bBusying = newVal;

//    //这里将应用是否空闲的状态写入C:\\TCR\\jsapp\\ini\\HMTCRUpdata.ini 配置文件中
//    //是为了升级程序HMTCRUpdata.exe 在升级时判断有没有客户在做交易时使用
//    if (m_bBusying)
//    {
//        WritePrivateProfileString("System","APIsBusying","1","C:\\TCR\\jsapp\\ini\\HMTCRUpdata.ini");
//    }
//    else
//    {
//        WritePrivateProfileString("System","APIsBusying","0","C:\\TCR\\jsapp\\ini\\HMTCRUpdata.ini");
//    }
}
