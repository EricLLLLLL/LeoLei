// Last updated at 2015/12/15 10:11:30

#include "stdafx.h"
#include "mcdm.h"
#include "Helper/helper.h"
#include "common_def.h"

#define KAL_PARAM_ERROR -10000
#define  LFS_MEM_FLAGS  (LFS_MEM_SHARE | LFS_MEM_ZEROINIT)

CMCDM* CMCDM::m_pInstance = NULL;

CMCDM::CMCDM(const QString& strDBusObjName):CWebInteractObj(strDBusObjName)
{
    LOGINFO("MCDM窗口创建");
    ////初始化定义的成员变量
    N_InitStatus();
    m_usCount=0;
    memset(m_szLogicalName,0,MAX_PATH);
    m_bOpened=false;
    m_lpCashUnitInfo=NULL;

    // 更改成1A 1B 这样的形式
    //add by ZYF begin
    m_mapUno["Slt10"]="1A";
    m_mapUno["Slt20"]="2A";
    m_mapUno["Slt30"]="3A";
    m_mapUno["Slt40"]="4A";
    m_mapUno["Slt50"]="5A";
    m_mapUno["Slt11"]="1B";
    m_mapUno["Slt21"]="2B";
    m_mapUno["Slt31"]="3B";
    m_mapUno["Slt41"]="4B";
    m_mapUno["Slt51"]="5B";
    //add by ZYF end
    GetConfigPath();

    m_nDispenseTotalAmount = 0;
    m_nDispenseAmount = 0;
}

CMCDM::~CMCDM()
{

    m_mapUno.clear();
    m_aSetCount.empty();
    m_aSetType.empty();
    m_aSetValue.empty();

    ::LFMFreeBuffer(m_lpCashUnitInfo);
    m_lpCashUnitInfo=NULL;
    LOGINFO("MCDM模块退出");
}

CMCDM *CMCDM::GetInstance()
{
    if(m_pInstance == NULL)
    {
        m_pInstance = new CMCDM("MCDM");
    }
    return m_pInstance;
}

void CMCDM::N_InitStatus()
{
    m_Status.device=LFS_CDM_DEVOFFLINE;
    m_Status.safe_door=LFS_CDM_DOORUNKNOWN;
    m_Status.dispenser=LFS_CDM_DISPCUUNKNOWN;
    m_Status.intermediate_stacker=LFS_CDM_ISUNKNOWN;
    m_PosStatus.position=LFS_CDM_POSTOP;
    m_PosStatus.position_status=LFS_CDM_PSUNKNOWN;
    m_PosStatus.shutter=LFS_CDM_SHTUNKNOWN;
    m_PosStatus.transport=LFS_CDM_TPUNKNOWN;
    m_PosStatus.transportStatus=LFS_CDM_TPSTATNOTEMPTY_UNK;
}


void CMCDM::WR_OpenComplete(DWORD dwCommand, HRESULT hResult, LPVOID buffer)
{
    //异步打开成功
    if(hResult == LFS_SUCCESS)
    {
        DWORD dwEvent = SERVICE_EVENTS | USER_EVENTS | SYSTEM_EVENTS | EXECUTE_EVENTS;
        int ret = m_apiCtrl.LFSAsyncRegister(dwEvent,&m_CurrRequestID);

        if(LFS_SUCCESS != ret)
        {
            Alarm("04300009");
            LOGERROR("MCDM模块Register命令失败，错误码为：%d",ret);
            N_Close();
            emit OpenFailed();// 修改为 OpenFailed
        }
    }
    else
    {
        Alarm("04300008");
        LOGERROR("MCDM模块Open命令失败，错误码为：%d",hResult);
        emit OpenFailed();// 修改为 OpenFailed
    }
}

void  CMCDM::WR_RegisterComplete(DWORD dwCommand, HRESULT hResult, LPVOID buffer)
{
    if(hResult!=LFS_SUCCESS)
    {
        Alarm("04300009");
        LOGERROR("MCDM模块Register命令失败，错误码为：%d",hResult);
        N_Close();
        emit OpenFailed();// 修改为 OpenFailed
    }
    else
    {
        /////OPEN完成获取能力
        int ret=N_GetCaps();
        if (ret == LFS_SUCCESS)
        {
            ret=N_CashUnitInfo();///为了上层调用取款、配钞命令时钞箱数传入问题
            LFMFreeBuffer(m_lpCashUnitInfo);/////立即释放防止内存泄露
            if (ret == LFS_SUCCESS)
            {
                m_bOpened = true;
                emit ConnectionOpened();
                LOGINFO("%s,事件：ConnectionOpened（）",__FUNCTION__);
            }
            else
            {
                Alarm("04300008");
                LOGERROR("MCDM模块CashUnitInfo命令失败，错误码为：%d",ret);
                N_Close();
                emit OpenFailed();// 修改为 OpenFailed
            }
        }
        else
        {
            Alarm("04300008");
            LOGERROR("MCDM模块GetCaps命令失败，错误码为：%d",ret);
            N_Close();
            emit OpenFailed();// 修改为 OpenFailed
        }
    }
}


// 收到关闭完成消息后调用该函数
void CMCDM::WR_CloseComplete(DWORD dwCommand, HRESULT hResult, LPVOID buffer)
{
    if(LFS_SUCCESS == hResult)
    {
        LOGINFO("MCDM模块关闭设备成功");
        if(m_bOpened)
        {
            m_bOpened = false;
            emit ConnectionClosed();
            LOGINFO("MCDM模块发送ConnectionClosed事件");
        }
    }
    else
    {
        LOGERROR("MCDM模块关闭设备失败，错误码为：%d",hResult);
        emit CloseFailed();// 修改为 CloseFailed
    }
}

// 收到打开完成消息后调用该函数
void CMCDM::WR_GetinfoComplete(DWORD dwCommand, HRESULT hResult, LPVOID buffer)
{
    switch(dwCommand)
    {
    case LFS_INF_CDM_CASH_UNIT_INFO:
        if (hResult == LFS_SUCCESS )
        {
            if (buffer == NULL || ((LPLFSRESULT)buffer)->buffer == NULL)
            {
                LOGERROR("MCDM模块WR_GetinfoComplete(LFS_INF_CDM_CASH_UNIT_INFO)返回空指针");
                return;
            }

            LPLFSCDMCUINFO lpCashUnitInfo=(LPLFSCDMCUINFO)((LPLFSRESULT)buffer)->buffer;
            if (lpCashUnitInfo)
            {
                LOGINFO("MCDM模块获取LFS_INF_CDM_CASH_UNIT_INFO成功");
                N_GetCashUnitInfo(lpCashUnitInfo);
                GetPossibleDispenseAmount();
            }
            else
            {
                LOGERROR("WR_GetinfoComplete(LFS_INF_CDM_CASH_UNIT_INFO)失败，SP返回数据为空");
            }
        }
        else
        {
            LOGERROR("WR_GetinfoComplete(LFS_INF_CDM_CASH_UNIT_INFO)失败, 错误码为：%d", hResult);
        }
        break;
    }
}

// 收到执行完成消息后调用该函数
void CMCDM::WR_ExecuteComplete(DWORD dwCommand, HRESULT hResult, LPVOID buffer)
{
    if ( hResult == LFS_ERR_HARDWARE_ERROR)
    {
        char ErrorCode[9]={0};
        DWORD ErrorCodeSize=9;
        /*DWORD QueryType=REG_SZ;
        HKEY  hKey;
        LPCTSTR LpSubKey="SOFTWARE\\HEMEI\\ErrorCode\\";
        long lRet = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, LpSubKey, 0,  KEY_READ, &hKey);
        if(lRet == ERROR_SUCCESS)
        {
            lRet=::RegQueryValueEx(hKey, "URT", NULL, &QueryType, (LPBYTE)ErrorCode, &ErrorCodeSize);
            if (lRet != ERROR_SUCCESS)
            {
                LOGERROR("读出注册表失败,getlasterr=%d",GetLastError());
            }
        }
        else
        {
            LOGERROR("打开注册表失败");
        }
        ::RegCloseKey(hKey);
        Alarm(ErrorCode);*/
    }

    switch(dwCommand)
    {
    case LFS_CMD_CDM_DISPENSE:
    {
        m_nDispenseAmount=0;
        if (hResult==LFS_SUCCESS)
        {
            if (buffer == NULL ||((LPLFSRESULT)buffer)->buffer == NULL ||((LPLFSCDMDENOMINATION)((LPLFSRESULT)buffer)->buffer)->values ==NULL)
            {
                Alarm("04300003");
                LOGERROR("MCDM模块SP返回空指针");
                emit DeviceError();
                LOGERROR("MCDM模块调用LFS_CMD_CDM_DISPENSE命令失败,发送DeviceError事件");
                //this->CashDispenseFailed("",m_nDispenseAmount);
                return;
            }

            QString DispenseCashUnit;
            DispenseCashUnit=N_DispenseCashUnit((LPLFSCDMDENOMINATION)((LPLFSRESULT)buffer)->buffer);
            emit this->CashDispensed(DispenseCashUnit,m_nDispenseAmount);
            m_nDispenseTotalAmount -= m_nDispenseAmount;		// 本笔交易还剩的未出金额
            LOGINFO("MCDM模块调用LFS_CMD_CDM_DISPENSE命令成功,发送CashDispensed事件,剩余未出钞金额%d", m_nDispenseTotalAmount);

            m_Dsto.AddPersistentData("~ADDSNR~","","");	 //add by art for 冠字号信息保存

        }
        else if (hResult == LFS_ERR_CDM_NOTDISPENSABLE || hResult ==LFS_ERR_CDM_INVALIDDENOMINATION || hResult ==LFS_ERR_CDM_TOOMANYITEMS)
        {
            Alarm("05400001");
            emit this->NotDispensable();
            LOGERROR("MCDM模块调用LFS_CMD_CDM_DISPENSE命令失败,发送NotDispensable事件");
        }
        else if (hResult == LFS_ERR_CDM_ITEMSLEFT)
        {
            Alarm("05400002");
            emit this->OutPositionNotEmpty();
            LOGERROR("MCDM模块调用LFS_CMD_CDM_DISPENSE命令失败,发送OutPositionNotempty事件");
        }
        else
        {
            if (buffer == NULL ||((LPLFSRESULT)buffer)->buffer == NULL ||((LPLFSCDMDENOMINATION)((LPLFSRESULT)buffer)->buffer)->values ==NULL)
            {
                Alarm("04300003");
                LOGERROR("MCDM模块SP返回空指针");
                emit DeviceError();
                //emit this->CashDispenseFailed("",m_nDispenseAmount);
                LOGERROR("MCDM模块调用LFS_CMD_CDM_DISPENSE命令失败,发送DeviceError事件");
                return;
            }
            QString DispenseCashUnit;
            DispenseCashUnit=N_DispenseCashUnit((LPLFSCDMDENOMINATION)((LPLFSRESULT)buffer)->buffer);
            emit CashDispenseFailed(DispenseCashUnit,m_nDispenseAmount);//this->CashUnitError(DispenseCashUnit)-> 改为 CashDispenseFailed
            m_nDispenseTotalAmount -= m_nDispenseAmount;		// 本笔交易还剩的未出金额

            m_Dsto.AddPersistentData("~ADDSNR~","","");	 //add by art for 冠字号信息保存
            LOGERROR("MCDM模块调用LFS_CMD_CDM_DISPENSE命令失败,发送CashDispenseFailed事件,错误码为：%d,剩余未出钞金额%d",hResult,m_nDispenseTotalAmount);
        }

        N_CashUnitInfoAsync();
    }
        break;
    case LFS_CMD_CDM_RESET:
    {
        if (hResult == LFS_SUCCESS)
        {
            emit this->ResetComplete();
            LOGINFO("MCDM模块调用LFS_CMD_CDM_RESET命令成功,发送ResetComplete事件");
        }
        else
        {
            Alarm("05400006");
            emit this->ResetFailed();// 改为 ResetFailed
            LOGERROR("MCDM模块调用LFS_CMD_CDM_RESET命令失败,发送ResetFailed事件,错误码为：%d",hResult);
        }
    }
        break;
    case LFS_CMD_CDM_DENOMINATE:
    {
        if (hResult == LFS_SUCCESS)
        {
            if (buffer == NULL ||((LPLFSRESULT)buffer)->buffer == NULL || ((LPLFSCDMDENOMINATION)((LPLFSRESULT)buffer)->buffer)->values == NULL)
            {
                Alarm("04300003");
                LOGERROR("MCDM模块SP返回空指针");
                emit this->MixFailed();// 改为 MixFailed
                LOGERROR("MCDM模块调用LFS_CMD_CDM_DENOMINATE命令失败,发送MixFailed事件");
                return;
            }

            int i=0;
            QString csMix;
            QString csMixTmp;
            for (i=0;i<((LPLFSCDMDENOMINATION)((LPLFSRESULT)buffer)->buffer)->count;i++)
            {
                csMixTmp.clear();
                csMixTmp = QString("%1").arg(((LPLFSCDMDENOMINATION)((LPLFSRESULT)buffer)->buffer)->values[i]);
                //csMixTmp.Format("%d",((LPLFSCDMDENOMINATION)((LPLFSRESULT)buffer)->buffer)->values[i]);
                if (i<((LPLFSCDMDENOMINATION)((LPLFSRESULT)buffer)->buffer)->count-1)
                {
                    csMix+=(csMixTmp+",");
                }
                else
                {
                    csMix+=csMixTmp;
                }
            }
            emit this->MixComplete(csMix);
            LOGINFO("MCDM模块调用LFS_CMD_CDM_DENOMINATE命令成功,发送MixComplete事件,csMix=%s",Q2S(csMix));
        }
        else if (hResult == LFS_ERR_CDM_NOTDISPENSABLE)
        {
            Alarm("05400001");
            emit this->NotDispensable();
            LOGERROR("MCDM模块调用LFS_CMD_CDM_DENOMINATE命令失败,发送NotDispensable事件");
        }
        else if (hResult == LFS_ERR_CDM_ITEMSLEFT)
        {
            Alarm("05400002");
            emit this->OutPositionNotEmpty();
            LOGERROR("MCDM模块调用LFS_CMD_CDM_DENOMINATE命令失败,发送OutPositionNotempty事件");
        }
        else
        {
            emit this->MixFailed();// 改为 MixFailed
            LOGERROR("MCDM模块调用LFS_CMD_CDM_DENOMINATE命令失败,发送MixFailed事件,错误码为：%d",hResult);
        }
    }
        break;
    case LFS_CMD_CDM_START_EXCHANGE:
    {
        if (hResult==LFS_SUCCESS || hResult==LFS_ERR_CDM_EXCHANGEACTIVE) //modify by art for LFS_ERR_CDM_EXCHANGEACTIVE 也当做成功
        {
            if (buffer == NULL ||((LPLFSRESULT)buffer)->buffer == NULL)
            {
                Alarm("04300003");
                LOGERROR("MCDM模块调用LFS_CMD_CDM_START_EXCHANGE时SP返回空指针,发送StartExchangeFailed事件");
                emit this->StartExchangeFailed();   // 改为StartExchangeFailed AAA
                return;
            }
            N_GetCashUnitInfo((LPLFSCDMCUINFO)((LPLFSRESULT)buffer)->buffer);
            emit this->StartExchangeCompleted();   // 改为StartExchangeCompleted AAA
            LOGINFO("MCDM模块调用LFS_CMD_CDM_START_EXCHANGE命令成功,发送ExchangeInitiated事件");
        }
        else
        {
            Alarm("05400004");
            emit this->StartExchangeFailed();  // 改为StartExchangeFailed AAA
            LOGERROR("MCDM模块调用LFS_CMD_CDM_START_EXCHANGE命令失败,发送StartExchangeFailed事件,错误码为：%d",hResult);
        }
    }
        break;
    case LFS_CMD_CDM_END_EXCHANGE:
    {
        //////清除数据
        m_aSetCount.empty();
        m_aSetType.empty();
        m_aSetValue.empty();

        if (hResult==LFS_SUCCESS)
        {
            emit this->EndExchangeCompleted();  // 改为 EndExchangeCompleted AAA
            LOGINFO("MCDM模块调用LFS_CMD_CDM_END_EXCHANGE命令成功,发送ExchangeCompleted事件");

        }
        else
        {
            Alarm("05400005");
            emit this->EndExchangeFailed();  // 改为 EndExchangeFailed AAA
            LOGERROR("MCDM模块调用LFS_CMD_CDM_END_EXCHANGE命令失败,发送EndExchangeFailed事件,错误码为：%d",hResult);
        }
    }
        break;
    case LFS_CMD_CDM_OPEN_SHUTTER:
    {
        if (hResult==LFS_SUCCESS)
        {
            emit this->ShutterOpened();
            LOGINFO("MCDM模块调用LFS_CMD_CDM_OPEN_SHUTTER命令成功,发送ShutterOpened事件");
        }
        else
        {
            Alarm("05400007");
            emit this->ShutterOpenFailed();  // 改为 ShutterOpenFailed AAA
            LOGERROR("MCDM模块调用LFS_CMD_CDM_OPEN_SHUTTER命令失败,发送ShutterOpenFailed事件,错误码为：%d",hResult);
        }
    }
        break;
    case LFS_CMD_CDM_CLOSE_SHUTTER:
    {
        if (hResult==LFS_SUCCESS)
        {
            emit this->ShutterClosed();
            LOGINFO("MCDM模块调用LFS_CMD_CDM_CLOSE_SHUTTER命令成功,发送ShutterClosed事件");
        }
        else
        {
            Alarm("05400008");
            emit this->ShutterCloseFailed(); // 改为 ShutterCloseFailed AAA
            LOGERROR("MCDM模块调用LFS_CMD_CDM_CLOSE_SHUTTER命令失败,发送ShutterCloseFailed事件,错误码为：%d",hResult);
        }
    }
        break;
    }
}

// 收到执行事件后调用该函数
void CMCDM::WR_ExecuteEvent(DWORD dwEventID, HRESULT hResult, LPVOID buffer)
{
    switch(dwEventID)
    {
    case LFS_EXEE_CDM_CASHUNITERROR:
    {
        if (buffer == NULL ||((LPLFSRESULT)buffer)->buffer == NULL)
        {
            Alarm("05400003");
            LOGERROR("MCDM模块SP返回空指针");
            return;
        }
    }
        break;
    case LFS_EXEE_CDM_SUBDISPENSEOK:
    {
        if (buffer == NULL ||((LPLFSRESULT)buffer)->buffer == NULL)
        {
            Alarm("04300003");
            LOGERROR("MCDM模块SP返回空指针");
            return;
        }
        else
        {
            QString DispenseCashUnit;
            m_nDispenseAmount=0;
            DispenseCashUnit=N_DispenseCashUnit((LPLFSCDMDENOMINATION)((LPLFSRESULT)buffer)->buffer);
            emit this->SubDispenseOk(DispenseCashUnit,m_nDispenseAmount);
            LOGINFO("MCDM模块发送SubDispenseOk事件");
        }

    }
        break;
    }
}

// 收到用户事件后调用该函数
void CMCDM::WR_UserEvent(DWORD dwEventID, HRESULT hResult, LPVOID buffer)
{

}

// 收到服务事件后调用该函数
void CMCDM::WR_ServiceEvent(DWORD dwEventID, HRESULT hResult, LPVOID buffer)
{
    switch(dwEventID)
    {
    case LFS_SRVE_CDM_ITEMSTAKEN:
    {
        emit this->CashTaken();
        LOGINFO("MCDM模块发送CashTaken事件");
    }
        break;
    case LFS_SRVE_CDM_CASHUNITINFOCHANGED:
    {
        LOGINFO("%s, 收到LFS_SRVE_CDM_CASHUNITINFOCHANGED事件", __FUNCTION__);
        N_CashUnitInfoAsync();
    }
        break;
    case LFS_SRVE_CDM_COUNTS_CHANGED:
    {
        LOGINFO("%s, 收到LFS_SRVE_CDM_COUNTS_CHANGED事件", __FUNCTION__);
        N_CashUnitInfoAsync();
    }
        break;

    }
}

// 收到系统事件后调用该函数
void CMCDM::WR_SystemEvent(DWORD dwEventID, HRESULT hResult, LPVOID buffer)
{
    switch(dwEventID)
    {
    case LFS_SYSE_DEVICE_STATUS:
    {
        if (buffer == NULL ||((LPLFSRESULT)buffer)->buffer == NULL)
        {
            LOGERROR("MCDM模块发送LFS_SYSE_DEVICE_STATUS事件SP返回空指针");
            return;
        }

        LOGINFO("MCDM模块收到SP上送的LFS_SYSE_DEVICE_STATUS事件");
        LPLFSDEVSTATUS lpDevStatus=(LPLFSDEVSTATUS)((LPLFSRESULT)buffer)->buffer;
        m_CurDevStatus=N_XFSDevStatusToKalDevStatus(lpDevStatus->state);
        m_PreDevStatus=N_XFSDevStatusToKalDevStatus(m_Status.device);

        if (m_CurDevStatus!=m_PreDevStatus)//不相等发送状态改变事件//xuzhe end 0420 modify
        {
            emit this->StatusChanged(m_szLogicalName,m_PreDevStatus,m_CurDevStatus);
            LOGINFO("MCDM模块发送StatusChanged事件,m_PreDevStatus=%s,m_CurDevStatus=%s",Q2S(m_PreDevStatus),Q2S(m_CurDevStatus));

        }
    }
        break;
    }
}


// 以下为属性的处理函数

SHORT CMCDM::GetRemainCount()
{
    SHORT RemainCount=g_Helper.GetPrivateProfileInt("MCRM", "CashUnitRemainCount", 30, m_strConfigPath);
    LOGINFO("%s,属性,钞箱保留张数 = %d",__FUNCTION__,RemainCount);
    return RemainCount;
}

void CMCDM::SetServiceName(QString newVal)
{
    LOGINFO("%s,属性,newVal = %s",__FUNCTION__,Q2S(newVal));

    strcpy(m_szLogicalName,Q2S(newVal));
}


QString CMCDM::GetStDetailedDeviceStatus(void)
{
    int ret=N_GetStatus();
    if (ret!=LFS_SUCCESS)
    {
        N_InitStatus();
    }
    QString csDeviceStatus;
    csDeviceStatus=N_XFSDevStatusToKalDevStatus(m_Status.device);
    LOGINFO("%s,属性,设备状态 = %s",__FUNCTION__,Q2S(csDeviceStatus));
    return csDeviceStatus;
}

QString CMCDM::GetCUNumber(void)
{

    return "";
}

QString CMCDM::GetCUId(void)
{

    return "";
}

////////返回钞箱：类型，钞箱：类型 这种格式 
QString CMCDM::GetCUType(void)
{
    QString csCUTypeTmp;
    int ret=N_CashUnitInfo();
    if (ret!=LFS_SUCCESS)
    {
        LOGERROR("MCDM模块调用GetCUType时获取钞箱状态失败");
        return "";
    }


    int i=0;
    for(i=0;i<m_lpCashUnitInfo->count;i++)
    {
        char BoxName[40]={0};
        char BoxType[10]={0};
        QString csUnitName;
        /*char szUnitIdTmp[6]={0};
        memcpy(szUnitIdTmp, m_lpCashUnitInfo->list[i]->unit_id, 5);
        csUnitName.Format("%s:",m_mapUno[szUnitIdTmp].data());
        sprintf(BoxName,"BOX%sTYPEFMT",m_mapUno[szUnitIdTmp].data());*/
        sprintf(BoxName,"BOX%dTYPEFMT",i+1);
        if (m_lpCashUnitInfo->list[i]->type == LFS_CDM_TYPERECYCLING)
        {
            memcpy(BoxType,"环流",strlen("环流"));
        }
        else
        {
            memcpy(BoxType,"回收",strlen("回收"));
        }

        m_Dsto.D_SetDataString(TRUE,BoxName,BoxType);
        LOGINFO("%s钞箱类型：%s",BoxName,BoxType);
        csCUTypeTmp+=(csUnitName+N_XFSCUTypeToKalCUType(m_lpCashUnitInfo->list[i]->type)+",");
    }

    if(csCUTypeTmp.length()>1)
        csCUTypeTmp=csCUTypeTmp.left(csCUTypeTmp.length()-1); //去除最后的","
    //add by art end

    LOGINFO("MCDM模块返回钞箱类型为：%s",Q2S(csCUTypeTmp));

    LFMFreeBuffer(m_lpCashUnitInfo);/////用完立即释放防止泄露
    return csCUTypeTmp;
}

QString CMCDM::GetCUStatus(void)
{
    QString csCUStatusTmp;
    int ret=N_CashUnitInfo();
    if (ret!=LFS_SUCCESS)
    {
        LOGERROR("MCDM模块调用GetCUStatus时获取钞箱状态失败");
        return "";
    }

    /////zyf mod
    int i=0;
    for(i=0;i<m_lpCashUnitInfo->count;i++)
    {
        QString csUnitName;
        char szUnitIdTmp[6]={0};
        memcpy(szUnitIdTmp, m_lpCashUnitInfo->list[i]->unit_id, 5);
        //csUnitName.Format("%s:",m_mapUno[szUnitIdTmp].data());
        csUnitName = QString("%1").arg(m_mapUno[szUnitIdTmp].data());
        csCUStatusTmp+=(csUnitName+N_XFSCUStatusToKalCUStatus(m_lpCashUnitInfo->list[i]->status)+",");
    }
    if(csCUStatusTmp.length()>1)
        csCUStatusTmp=csCUStatusTmp.left(csCUStatusTmp.length()-1); //去除最后的","
    //zyf mod

    LOGINFO("MCDM模块返回钞箱状态为：%s",Q2S(csCUStatusTmp));

    LFMFreeBuffer(m_lpCashUnitInfo);/////用完立即释放防止泄露
    return csCUStatusTmp;
}

QString CMCDM::GetCUCurrentCount(void)
{

    return "";
}

SHORT CMCDM::GetNumLogicalCashUnits(void)
{

    return 0;
}

QString CMCDM::GetCUCurrency(void)
{

    return "";
}

QString CMCDM::GetCUNoteValue(void)
{
    int ret=N_CashUnitInfo();
    if (ret!=LFS_SUCCESS)
    {
        LOGERROR("MCDM模块调用GetCUNoteValue时获取钞箱状态失败");
        return "";
    }

    QString csNoteValue;
    QString csNoteValueTmp;


    //add by zyf begin
    int i=0;
    for(i=0;i<m_lpCashUnitInfo->count;i++)
    {///将钞箱面额写入数据模块
        char BoxName[40]={0};
        char BoxValue[10]={0};
        QString csUnitName;
        char szUnitIdTmp[6]={0};
        memcpy(szUnitIdTmp, m_lpCashUnitInfo->list[i]->unit_id, 5);
        //csUnitName.Format("%s:",m_mapUno[szUnitIdTmp].data());
        csUnitName = QString("%1").arg(m_mapUno[szUnitIdTmp].data());
        /*sprintf(BoxName,"BOX%sVALUEFMT", m_mapUno[szUnitIdTmp].data());*/
        sprintf(BoxName,"BOX%dVALUEFMT", i+1);
        sprintf(BoxValue,"%d",m_lpCashUnitInfo->list[i]->values);
        m_Dsto.D_SetDataString(TRUE,BoxName,BoxValue);
        LOGINFO("%s钞箱面额：%s",BoxName,BoxValue);

        csNoteValueTmp.clear();
        //csNoteValueTmp.Format("%d",m_lpCashUnitInfo->list[i]->values);
        csNoteValueTmp = QString("%1").arg(m_lpCashUnitInfo->list[i]->values);
        csNoteValue+=(csUnitName+csNoteValueTmp+",");
    }
    if(csNoteValue.length()>1)
        csNoteValue=csNoteValue.left(csNoteValue.length()-1); //去除最后的","
    //add by zyf end

    LOGINFO("MCDM模块返回钞箱面额为：%s",Q2S(csNoteValue));

    LFMFreeBuffer(m_lpCashUnitInfo);/////用完立即释放防止泄露
    return csNoteValue;
}


QString CMCDM::GetCUInitialCount(void)
{

    return "";
}

QString CMCDM::GetCUMaxThreshold(void)
{

    return "";
}

QString CMCDM::GetCUMinThreshold(void)
{
    return "";
}

QString CMCDM::GetPUId(void)
{
    return "";
}

QString CMCDM::GetPUCurrentCount(void)
{
    int ret=N_CashUnitInfo();
    if (ret!=LFS_SUCCESS)
    {
        LOGERROR("MCDM模块调用GetPUCurrentCount时获取钞箱状态失败");
        return "";
    }
    int i=0;
    QString csCurrentCount;
    QString csCurrentCountTmp;

    //add by art begin
    for(i=0;i<m_lpCashUnitInfo->count;i++)
    {/////剩余张数设到数据模块中
        char BoxName[40]={0};
        char BoxCount[10]={0};
        QString csUnitName;
        char szUnitIdTmp[6]={0};
        memcpy(szUnitIdTmp, m_lpCashUnitInfo->list[i]->unit_id, 5);
        //csUnitName.Format("%s:",m_mapUno[szUnitIdTmp].data());
        csUnitName = QString("%1").arg(m_mapUno[szUnitIdTmp].data());
        /*sprintf(BoxName,"BOX%sNOTENUMBERFMT", m_mapUno[szUnitIdTmp].data());*/
        sprintf(BoxName,"BOX%dNOTENUMBERFMT", i+1);
        sprintf(BoxCount,"%d",m_lpCashUnitInfo->list[i]->count);
        m_Dsto.D_SetDataString(TRUE,BoxName,BoxCount);
        LOGINFO("%s钞箱剩余张数：%s",BoxName,BoxCount);

        csCurrentCountTmp.clear();
        //csCurrentCountTmp.Format("%d",m_lpCashUnitInfo->list[i]->count);
        csCurrentCountTmp = QString("%1").arg(m_lpCashUnitInfo->list[i]->count);
        csCurrentCount+=(csUnitName+csCurrentCountTmp+",");
    }
    if(csCurrentCount.length()>1)
        csCurrentCount=csCurrentCount.left(csCurrentCount.length()-1); //去除最后的","
    //add by art end

    LOGINFO("MCDM模块返回各钞箱当前张数为：%s",Q2S(csCurrentCount));

    LFMFreeBuffer(m_lpCashUnitInfo);/////用完立即释放防止泄露
    return csCurrentCount;

}

QString CMCDM::GetPUCUId(void)
{

    return "";
}

QString CMCDM::GetPUStatus(void)
{

    return "";
}

QString CMCDM::GetPUInitialCount(void)
{
    return "";
}

//////此函数返回值格式为 钞箱编码：RejectCount,钞箱编码：RejectCount  
/////////其中钞箱编码格式为1A 1B 等
QString CMCDM::GetPURejectCount(void)
{
    int ret=N_CashUnitInfo();
    if (ret!=LFS_SUCCESS)
    {
        LOGERROR("MCDM模块调用GetPURejectCount时获取钞箱状态失败");
        return "";
    }
    int i=0;
    QString csRejectCount;
    QString csRejectCountTmp;


    //add by art begin
    for (i=0;i<m_lpCashUnitInfo->count;i++)
    {
        char szUnitIdTmp[6]={0};
        memcpy(szUnitIdTmp, m_lpCashUnitInfo->list[i]->unit_id, 5);
        //csRejectCountTmp.Format("%s:%d",m_mapUno[szUnitIdTmp].data(),m_lpCashUnitInfo->list[i]->ulRejectCount);
        csRejectCountTmp = QString("%1%2").arg(m_mapUno[szUnitIdTmp].data()).arg(m_lpCashUnitInfo->list[i]->rejectCount);
        csRejectCount+=(csRejectCountTmp+",");
    }

    csRejectCount=csRejectCount.left(csRejectCount.length()-1); //去除最后的","

    //add by art end

    LFMFreeBuffer(m_lpCashUnitInfo);//////用完之后马上释放  防止泄露
    LOGINFO("各钞箱出钞RJ：%s",Q2S(csRejectCount));
    return csRejectCount;
}

QString CMCDM::GetPUCUNumber(void)
{

    return "";
}

BOOL CMCDM::GetCpCanRetract(void)
{

    return FALSE;
}

QString CMCDM::GetStSafeDoorStatus(void)
{

    return "";
}

QString CMCDM::GetStDispenserStatus(void)
{
    int ret = N_GetStatus();
    if (ret != LFS_SUCCESS)
    {
        N_InitStatus();
    }
    if (m_Status.dispenser == LFS_CDM_DISPOK)
    {
        return "OK";
    }
    else if (m_Status.dispenser == LFS_CDM_DISPCUSTATE)
    {
        return "STATE";
    }
    else if (m_Status.dispenser == LFS_CDM_DISPCUSTOP)
    {
        return "STOP";
    }
    else
    {
        return "UNKNOWN";
    }

}

QString CMCDM::GetMixNumber(void)
{

    return "";
}

QString CMCDM::GetStStackerStatus(void)
{

    return "";
}

QString CMCDM::GetStTransportStatus(void)
{

    return "";
}

QString CMCDM::GetStOutputStatus(void)//修改为 GetStOutputStatus
{
    //xuzhe start 0419 add
    int ret = N_GetStatus();
    if (ret != LFS_SUCCESS)
    {
        N_InitStatus();
    }
    QString strStatus("");

    switch(m_PosStatus.position_status)
    {
    case LFS_CDM_PSEMPTY:
        strStatus = "empty";
        break;
    case LFS_CDM_PSNOTEMPTY:
        strStatus = "NOTempty";
        break;
    default:
        strStatus = "UNKNOWN";
    }

    LOGINFO("出钞口的状态：str = %s,dw = %d",Q2S(strStatus),m_PosStatus.position_status);

    //xuzhe end 0419 add

    return strStatus;
}

QString CMCDM::GetStShutterStatus(void)
{
    int ret=N_GetStatus();
    if (ret!=LFS_SUCCESS)
    {
        N_InitStatus();
    }

    QString strShutterStatus("");
    switch(m_PosStatus.shutter)
    {
    case LFS_CDM_SHTCLOSED:
        strShutterStatus = "CLOSED";
        break;
    case LFS_CDM_SHTOPEN:
        strShutterStatus = "OPEN";
        break;
    default:
        strShutterStatus = "UNKNOWN";
    }

    LOGINFO("出钞口的状态：%s",Q2S(strShutterStatus));
    return strShutterStatus;
}

BOOL CMCDM::GetCpCanDetectCashTaken(void)
{

    return 0;
}


// 以下为方法的处理函数

SHORT CMCDM::CloseConnection(void)
{
    LOGINFO("%s，方法。",__FUNCTION__);
    int ret = LFS_SUCCESS;
    if (m_bOpened)
    {
        ret=N_Close();
    }

    return ret;
}

SHORT CMCDM::OpenConnection(void)
{
    LOGINFO("%s，方法。",__FUNCTION__);
    if (!m_bOpened)
    {
        //m_apiCtrl.SetSendWnd(GetWindowHwnd());
        int ret =N_Open();
        if (ret!=LFS_SUCCESS)
        {
            Alarm("04300008");
            emit OpenFailed();
        }
        return ret;
    }
    else
    {
        emit ConnectionOpened();
        LOGINFO("%s,事件：ConnectionOpened（）",__FUNCTION__);
        return 0;
    }
}

SHORT CMCDM::OpenShutter(LONG Timeout)
{
    LOGINFO("%s，方法。",__FUNCTION__);
    WORD position;
    position=LFS_CDM_POSNULL;
    int ret=N_OpenShutter(&position,Timeout);
    return ret;
}

SHORT CMCDM::CloseShutter(LONG Timeout)
{
    LOGINFO("%s，方法。",__FUNCTION__);
    WORD position;
    position=LFS_CDM_POSNULL;
    int ret=N_CloseShutter(&position,Timeout);
    return ret;
}

void CMCDM::Mix(LONG Amount, QString Currency, QString MixAlgorithm)
{
    if ( Currency.isEmpty() || MixAlgorithm.isEmpty() || Amount == 0)
    {
        Alarm("04300006");
        emit MixFailed();
        LOGERROR("MCDM模块AP传入参数有误");
        return;
    }

    LFSCDMDENOMINATE Denominate;
    LFSCDMDENOMINATION Denomination;
    Denominate.teller_id=m_usTellerID;
    Denominate.mix_number=atoi(Q2S(MixAlgorithm));
    Denominate.denomination=&Denomination;
    memset( &Denomination, NULL, sizeof(LFSCDMDENOMINATION) );
    Denomination.amount=Amount;
    memcpy(Denomination.currency_id,Q2S(Currency),3);
    Denomination.cash_box=0;
    Denomination.count=0;
    Denomination.values=0;
    N_Denominate(&Denominate);
}

void CMCDM::MixAndDispense(LONG Amount, QString Currency, QString MixAlgorithm)
{
    if ( Currency.isEmpty() || MixAlgorithm.isEmpty())
    {
        Alarm("04300006");
        emit this->CashDispenseFailed("",m_nDispenseAmount);
        LOGERROR("MCDM模块AP传入参数有误");
        return;
    }

    LFSCDMDISPENSE Dispense;
    LFSCDMDENOMINATION Denomination;
    LOGINFO("开始MixAndDispense命令");
    /*if (m_count == 0)
    {
        return ;
    }*/
    ULONG Values[]={0};
    Dispense.teller_id=m_usTellerID;
    Dispense.position=LFS_CDM_POSNULL;
    Dispense.mix_number=atoi(Q2S(MixAlgorithm));
    Dispense.present=TRUE;
    Dispense.denomination=&Denomination;
    memset( &Denomination, NULL, sizeof(LFSCDMDENOMINATION) );
    Denomination.amount=Amount;
    memcpy(Denomination.currency_id,Q2S(Currency),3);
    Denomination.cash_box=0;
    Denomination.count=m_usCount;
    Denomination.values=m_uValues;
    N_Dispense(&Dispense);

}

void CMCDM::Dispense(LONG Amount, QString NoteCounts, QString Currency, QString MixAlgorithm)
{
    if ( Currency.isEmpty() || MixAlgorithm.isEmpty() || NoteCounts.isEmpty())
    {
        Alarm("04300006");
        emit this->DeviceError();
        LOGERROR("MCDM模块AP传入参数有误");
        return;
    }
    LFSCDMDISPENSE Dispense;
    LFSCDMDENOMINATION Denomination;
    LOGINFO("开始Dispense命令Amount=%d,NoteCounts=%s,Currency=%s,MixAlgorithm=%s",Amount,Q2S(NoteCounts),Q2S(Currency),Q2S(MixAlgorithm));

    int ret=N_CashUnitInfo();
    if (ret!=LFS_SUCCESS)
    {
        LOGERROR("MCDM模块获取钞箱信息失败");
        return;
    }

    memset(m_uValues,0,sizeof(m_uValues));
    QString strSub;
    LONG amount = 0;
    int i=0;

    for (i=0;i<m_usCount;i++)
    {
        strSub = NoteCounts.section(',', i);
        //::AfxExtractSubString(strSub, NoteCounts, i, ',');
        m_uValues[i]=atoi(Q2S(strSub));
        if (atoi(Q2S(MixAlgorithm)) == 0)
        {
            amount += m_uValues[i] * m_lpCashUnitInfo->list[i]->values;
        }
        else
        {
            amount=Amount;
        }
    }

    LFMFreeBuffer(m_lpCashUnitInfo);

    if (m_nDispenseTotalAmount - amount < 0)
    {
        LOGERROR("本次预取金额%d, 累计Dispense出钞金额将超出预取出金额,发送DispenseAmountIncorrect事件", amount);
        emit this->DispenseAmountIncorrect();
        return;
    }

    Dispense.teller_id=m_usTellerID;
    Dispense.position=LFS_CDM_POSNULL;
    Dispense.mix_number=atoi(Q2S(MixAlgorithm));
    Dispense.present=FALSE;
    Dispense.denomination=&Denomination;
    memset( &Denomination, NULL, sizeof(LFSCDMDENOMINATION) );
    Denomination.amount=Amount;
    memcpy(Denomination.currency_id,Q2S(Currency),3);
    Denomination.cash_box=0;
    Denomination.count=m_usCount;
    Denomination.values=m_uValues;
    N_Dispense(&Dispense);

}

// 设置要取款的金额，确保本笔交易累次出钞的总和与要取的金额相等
void CMCDM::SetDispenseAmount(LONG Amount)
{
    m_nDispenseTotalAmount = Amount;
    LOGINFO("本笔交易预取金额%d", m_nDispenseTotalAmount);
}

void CMCDM::Present(LONG Timeout)
{
    
}

void CMCDM::StartExchange(void)// 改为 StartExchange 
{
    LFSCDMSTARTEX StartEx;

    StartEx.exchange_type=LFS_CDM_EXBYHAND;
    StartEx.count=0;
    StartEx.teller_id=0;
    StartEx.cu_num_list=0;
    int ret=N_StartExchange(&StartEx);
}

void CMCDM::EndExchange(void)// 改为 EndExchange
{
    int i=0;
    int j=0;

    ///////在清机加钞之前将剩余总张数，出钞张数和入钞张数以及出RJ张数保存至数据模块


    if (m_aSetCount.size() > 0)
    {
        for (i=0;i<m_aSetCount.size();i++)
        {
            for (j=0;j<m_lpCashUnitInfo->count;j++)
            {
                //add by art begin
                char szUnitIdTmp[6]={0};
                memcpy(szUnitIdTmp, m_lpCashUnitInfo->list[i]->unit_id, 5);
                //add by art end

                if ((m_aSetCount.at(i).strUnitNum == m_mapUno[szUnitIdTmp].data())) //modify by art
                {
                    m_lpCashUnitInfo->list[i]->count=m_aSetCount.at(i).ulCount;
                    break;
                }
            }
        }
    }

    if (m_aSetType.size() > 0)
    {
        for (i=0;i<m_aSetType.size();i++)
        {
            for (j=0;j<m_lpCashUnitInfo->count;j++)
            {
                //add by art begin
                char szUnitIdTmp[6]={0};
                memcpy(szUnitIdTmp, m_lpCashUnitInfo->list[i]->unit_id, 5);
                //add by art end

                if ((m_aSetType.at(i).strUnitNum == m_mapUno[szUnitIdTmp].data())) //modify by art
                {
                    m_lpCashUnitInfo->list[i]->type=m_aSetType.at(i).fwType;
                    break;
                }
            }
        }
    }

    if (m_aSetValue.size() > 0)
    {
        for (i=0;i<m_aSetValue.size();i++)
        {
            for (j=0;j<m_lpCashUnitInfo->count;j++)
            {
                //add by art begin
                char szUnitIdTmp[6]={0};
                memcpy(szUnitIdTmp, m_lpCashUnitInfo->list[i]->unit_id, 5);
                //add by art end

                //if (m_aSetCount.at(i).usNumber == m_lpCashInfo->lppCashIn[j]->usNumber)
                if ((m_aSetValue.at(i).strUnitNum == m_mapUno[szUnitIdTmp].data())) //modify by art
                {
                    m_lpCashUnitInfo->list[i]->values=m_aSetValue.at(i).ulValues;
                    break;
                }
            }
        }
    }

    int ret=N_EndExchange();
    ////调完END_EXCHAGE立即释放内存

    ::LFMFreeBuffer(m_lpCashUnitInfo);
}


SHORT CMCDM::Reset(QString RetractArea, SHORT ID,LONG Timeout)// 添加超时
{
    LFSCDMITEMPOSITION ResetIn;
    LFSCDMRETRACT  RetractAreaTmp;

    LOGINFO("开始Reset命令");
    memset(&RetractAreaTmp,0,sizeof(LFSCDMRETRACT));
    ResetIn.retract_area=&RetractAreaTmp;
    ResetIn.number=ID;
    ResetIn.output_position=LFS_CDM_POSNULL;
    RetractAreaTmp.index=0;

    if ((RetractArea == "RETRACT"))
    {
        RetractAreaTmp.retract_area=LFS_CDM_RA_RETRACT;

    }
    else if ((RetractArea=="TRANSPORT"))
    {
        RetractAreaTmp.retract_area=LFS_CDM_RA_TRANSPORT;
    }
    else if ((RetractArea=="ESCROW"))
    {
        RetractAreaTmp.retract_area=LFS_CDM_RA_STACKER;
    }
    else if ((RetractArea=="EJECT"))
    {
        RetractAreaTmp.retract_area=LFS_CDM_RA_REJECT;
    }
    else if ((RetractArea=="REJECT"))
    {
        RetractAreaTmp.retract_area=LFS_CDM_RA_REJECT;
    }
    int ret=N_Reset(&ResetIn,Timeout);
    return ret;
}


// 计算目前可取金额
void CMCDM::GetPossibleDispenseAmount(void)
{
    LONG possibleDispense100Amount = 0;
    LONG possibleDispenseOtherAmount = 0;
    LONG possibleDispenseAmount1 = 0;
    LONG possibleDispenseAmount2 = 0;
    LONG possibleDispenseAmount3 = 0;
    int nWasteMoneyRate = g_Helper.GetPrivateProfileInt("MCDM", "WasteMoneyRate", 0, m_strConfigPath);
    int nCashUnitRemainCount = g_Helper.GetPrivateProfileInt("MCRM", "CashUnitRemainCount", 30, m_strConfigPath);    // 从MCRM.ini配置文件中获取预留张数

    int i=0;
    ULONG values[10] = {};
    ULONG counts[10] = {};
    ULONG ulMaxCount[10] = {};
    USHORT usType[10] = {};
    USHORT usStatus[10] = {};

    for (i=0; i<m_lpCashUnitInfo->count; i++)
    {
        values[i] = m_lpCashUnitInfo->list[i]->values;
        counts[i] =  m_lpCashUnitInfo->list[i]->count;
        ulMaxCount[i] =  m_lpCashUnitInfo->list[i]->maximum;
        usType[i] =  m_lpCashUnitInfo->list[i]->type;
        usStatus[i] = m_lpCashUnitInfo->list[i]->status;
    }

    // 根据回收箱张数和废钞率，判断可取金额
    if (0 != nWasteMoneyRate)
    {
        possibleDispenseAmount1 = ((2700 - counts[3]) * 100 / (double)nWasteMoneyRate) * 100;
        if (possibleDispenseAmount1 < 0)
        {
            possibleDispenseAmount1 = 0;
        }
    }

    // 目前可取的循环箱的总金额
    for (i = 0; i < m_lpCashUnitInfo->count; i++)
    {
        if (LFS_CDM_TYPERECYCLING == usType[i] && values[i] == 100)/////100可取金额
        {
            int count = counts[i] - nCashUnitRemainCount;
            if (count < 0 || LFS_CDM_STATCUEMPTY == usStatus[i] || LFS_CDM_STATCUINOP == usStatus[i] ||
                    LFS_CDM_STATCUMISSING == usStatus[i] || LFS_CDM_STATCUMANIP == usStatus[i])
            {
                count = 0;
            }
            possibleDispenseAmount2 += (count * (100 - nWasteMoneyRate) / 100) * values[i];
        }
        if (LFS_CDM_TYPERECYCLING == usType[i] && values[i] != 100)/////零钞可取金额
        {
            int count = counts[i] - nCashUnitRemainCount;
            if (count < 0 || LFS_CDM_STATCUEMPTY == usStatus[i] || LFS_CDM_STATCUINOP == usStatus[i] ||
                    LFS_CDM_STATCUMISSING == usStatus[i] || LFS_CDM_STATCUMANIP == usStatus[i])
            {
                count = 0;
            }
            possibleDispenseAmount3 += (count * (100 - nWasteMoneyRate) / 100) * values[i];
        }
    }

    if (0 == nWasteMoneyRate)
    {
        possibleDispense100Amount = possibleDispenseAmount2;
        possibleDispenseOtherAmount=possibleDispenseAmount3;
    }
    else
    {
        possibleDispense100Amount = possibleDispenseAmount1 < possibleDispenseAmount2 ? possibleDispenseAmount1 : possibleDispenseAmount2;
        possibleDispenseOtherAmount = possibleDispenseAmount1 < possibleDispenseAmount3 ? possibleDispenseAmount1 : possibleDispenseAmount3;
    }

    m_Dsto.D_SetDataLong(FALSE, "POSSIBLEDISPENSE100AMOUNT", possibleDispense100Amount);
    m_Dsto.D_SetDataLong(FALSE, "POSSIBLEDISPENSEOTHERAMOUNT", possibleDispenseOtherAmount);
    LOGINFO("循环箱目前100可取的总金额为%d,零钞可取总金额为%d", possibleDispense100Amount,possibleDispenseOtherAmount);
}


int CMCDM::N_Open()
{	
    int ret = m_apiCtrl.LFSAsyncOpen(m_szLogicalName,0,&m_CurrRequestID);

    if(LFS_SUCCESS!=ret)
    {
        Alarm("04300007");
        emit OpenFailed();
        LOGERROR("MCDM模块Open命令失败,错误码为：%d",ret);
    }

    return ret;
}

int CMCDM::N_Close()
{
    int ret = m_apiCtrl.LFSAsyncClose(&m_CurrRequestID);

    if(LFS_SUCCESS!=ret)
    {
        Alarm("04300007");
        emit CloseFailed();
        LOGERROR("MCDM模块Close命令失败,错误码为：%d",ret);
    }

    return ret;
}

int CMCDM::N_OpenShutter(LPWORD lpposition,LONG Timeout)
{
    if (Timeout == -1)
    {
        Timeout=LFS_INDEFINITE_WAIT;
    }
    int ret=m_apiCtrl.LFSAsyncExecute(LFS_CMD_CDM_OPEN_SHUTTER,lpposition,Timeout,&m_CurrRequestID);
    if (ret!=LFS_SUCCESS)
    {
        Alarm("04300007");
        emit ShutterOpenFailed();
        LOGERROR("MCIM模块OpenShutter命令失败,错误码为：%d",ret);
    }
    return ret;
}

int CMCDM::N_CloseShutter(LPWORD lpposition,LONG Timeout)
{
    if (Timeout == -1)
    {
        Timeout=LFS_INDEFINITE_WAIT;
    }
    int ret=m_apiCtrl.LFSAsyncExecute(LFS_CMD_CDM_CLOSE_SHUTTER,lpposition,Timeout,&m_CurrRequestID);
    if (ret!=LFS_SUCCESS)
    {
        Alarm("04300007");
        emit ShutterCloseFailed();
        LOGERROR("MCIM模块CloseShutter命令失败,错误码为：%d",ret);
    }
    return ret;
}

//////执行LFS_CMD_CDM_START_EXCHANGE
int CMCDM::N_StartExchange(LPLFSCDMSTARTEX lpStartEx)
{
    int ret=m_apiCtrl.LFSAsyncExecute(LFS_CMD_CDM_START_EXCHANGE,lpStartEx,HM_MIDDLE_TIMEOUT,&m_CurrRequestID);
    if (ret!=LFS_SUCCESS)
    {
        Alarm("04300007");
        emit StartExchangeFailed();
        LOGERROR("MCDM模块LFS_CMD_CDM_START_EXCHANGE命令失败,错误码为：%d",ret);
    }

    return ret;
}

int CMCDM::N_EndExchange()
{
    int ret=m_apiCtrl.LFSAsyncExecute(LFS_CMD_CDM_END_EXCHANGE,m_lpCashUnitInfo,HM_MIDDLE_TIMEOUT,&m_CurrRequestID); //tmp by art
    if (ret!=LFS_SUCCESS)
    {
        Alarm("04300007");
        emit EndExchangeFailed();
        LOGERROR("MCDM模块LFS_CMD_CDM_END_EXCHANGE命令失败,错误码为：%d",ret);
    }

    return ret;
}



//执行LFS_INF_CDM_STATUS
int CMCDM::N_GetStatus()
{
    LPLFSRESULT lpResult = NULL;
    int ret =m_apiCtrl.LFSGetInfo(LFS_INF_CDM_STATUS, NULL, HM_MIDDLE_TIMEOUT, &lpResult);
    if (ret == LFS_SUCCESS )
    {
        LPLFSCDMSTATUS lpStatus = (LPLFSCDMSTATUS)((LPLFSRESULT)lpResult)->buffer;
        if (lpStatus)
        {
            LOGINFO("MCDM模块获取状态成功");
            N_Status(lpStatus);
        }
        else
        {
            Alarm("04300003");
            ret = -1;
            LOGERROR("MCDM模块获取状态失败，SP返回数据为空");
        }

    }
    else
    {
        Alarm("04300007");
        LOGERROR("MCDM模块GetStatus命令失败,错误码为：%d",ret);
    }

    ::LFSFreeResult(lpResult);
    return ret;
}

//执行LFS_INF_CDM_CAPABILITIES
int CMCDM::N_GetCaps()
{
    LPLFSRESULT lpResult = NULL;
    int ret =m_apiCtrl.LFSGetInfo(LFS_INF_CDM_CAPABILITIES, NULL, HM_MIDDLE_TIMEOUT, &lpResult);
    if (ret == LFS_SUCCESS )
    {
        LPLFSCDMCAPS lpCaps = (LPLFSCDMCAPS)((LPLFSRESULT)lpResult)->buffer;
        if (lpCaps)
        {
            LOGINFO("MCDM模块获取能力成功");
            N_Capabilities(lpCaps);
        }
        else
        {
            Alarm("04300003");
            ret = -1;
            LOGERROR("MCDM模块获取能力失败，SP返回数据为空");
        }

    }
    else
    {
        Alarm("04300007");
        LOGERROR("MCDM模块GetCaps命令失败,错误码为：%d",ret);
    }

    ::LFSFreeResult(lpResult);
    return ret;
}

int CMCDM::N_Status(LPLFSCDMSTATUS lpStatus)
{
    if (lpStatus == NULL)
    {
        Alarm("04300003");
        LOGERROR("MCDM模块SP返回设备状态为NULL");
        return KAL_PARAM_ERROR;
    }
    m_Status.device=lpStatus->device;
    m_Status.dispenser=lpStatus->dispenser;
    m_Status.intermediate_stacker=lpStatus->intermediate_stacker;
    m_Status.safe_door=lpStatus->safe_door;
    //m_Status.power_saverecovery_time=lpStatus->usPowerSaveRecoveryTime;
    //m_Status.positions=lpStatus->positions;

    //xuzhe start 0419 add
    //保存出钞口的状态
    if(lpStatus->positions&&lpStatus->positions[0])
    {
        m_PosStatus.position = lpStatus->positions[0]->position;
        m_PosStatus.position_status = lpStatus->positions[0]->position_status;
        m_PosStatus.shutter = lpStatus->positions[0]->shutter;
        m_PosStatus.transport = lpStatus->positions[0]->transport;
        m_PosStatus.transportStatus = lpStatus->positions[0]->transportStatus;
    }
    else
    {
        Alarm("04300003");
        LOGERROR("指针为空");
    }
    //xuzhe end 0419 add

    return 0;
}

int CMCDM::N_Capabilities(LPLFSCDMCAPS lpCaps)
{
    if (lpCaps == NULL)
    {
        Alarm("04300003");
        LOGERROR("MCDM模块SP返回设备能力为NULL");
        return KAL_PARAM_ERROR;
    }

    m_Caps.cash_box=lpCaps->cash_box;
    m_Caps.compound=lpCaps->compound;
    /// m_Caps.anti_fraud_module=lpCaps->bAntiFraudModule;
    m_Caps.items_taken_sensor=lpCaps->items_taken_sensor;
    //m_Caps.bPowerSaveControl=lpCaps->bPowerSaveControl;
    //m_Caps.bPrepareDispense=lpCaps->bPrepareDispense;
    m_Caps.safe_door=lpCaps->safe_door;
    m_Caps.shutter=lpCaps->shutter;
    m_Caps.shutter_control=lpCaps->shutter_control;
    m_Caps.exchange_type=lpCaps->exchange_type;
    m_Caps.move_items=lpCaps->move_items;
    m_Caps.positions=lpCaps->positions;
    m_Caps.retract_areas=lpCaps->retract_areas;
    m_Caps.retract_stacker_actions=lpCaps->retract_stacker_actions;
    m_Caps.retract_transport_actions=lpCaps->retract_transport_actions;
    m_Caps.dev_class=lpCaps->dev_class;
    m_Caps.max_dispense_items=lpCaps->max_dispense_items;


    return 0;
}

////执行LFS_INF_CDM_CASH_UNIT_INFO
int CMCDM::N_CashUnitInfo()
{
    LPLFSRESULT lpResult = NULL;
    int ret =m_apiCtrl.LFSGetInfo(LFS_INF_CDM_CASH_UNIT_INFO, NULL, HM_MIDDLE_TIMEOUT, &lpResult);
    if (ret == LFS_SUCCESS )
    {
        LPLFSCDMCUINFO lpCashUnitInfo=(LPLFSCDMCUINFO)((LPLFSRESULT)lpResult)->buffer;
        if (lpCashUnitInfo)
        {
            LOGINFO("MCDM模块获取钞箱信息成功");
            N_GetCashUnitInfo(lpCashUnitInfo);
            GetPossibleDispenseAmount();
        }
        else
        {
            Alarm("04300003");
            ret = -1;
            LOGERROR("MCDM模块获取钞箱信息失败，SP返回数据为空");
        }
    }
    else
    {
        Alarm("04300007");
        LOGERROR("MCDM模块CASH_UNIT_INFO命令失败,错误码为：%d",ret);
    }

    ::LFSFreeResult(lpResult);
    return ret;
}

// 获取钞箱信息（异步）
int CMCDM::N_CashUnitInfoAsync()
{
    int ret = m_apiCtrl.LFSAsyncGetInfo(LFS_INF_CDM_CASH_UNIT_INFO, 0, 0, &m_CurrRequestID);
    if (ret != LFS_SUCCESS )
    {
        LOGERROR("MCDM模块异步CASH_UNIT_INFO命令失败,错误码为：%d", ret);
        return ret;
    }

    return 0;
}

int CMCDM::N_Denominate(LPLFSCDMDENOMINATE lpDenominate)
{
    int ret=m_apiCtrl.LFSAsyncExecute(LFS_CMD_CDM_DENOMINATE,lpDenominate,HM_MIDDLE_TIMEOUT,&m_CurrRequestID);
    if (ret!=LFS_SUCCESS)
    {
        Alarm("04300007");
        emit MixFailed();
        LOGERROR("发送MixFailed事件，调用N_Denominate失败，错误码为：%d",ret);
    }

    return ret;
}

int CMCDM::N_Dispense(LPLFSCDMDISPENSE lpDispense)
{
    int ret=m_apiCtrl.LFSAsyncExecute(LFS_CMD_CDM_DISPENSE,lpDispense,LFS_INDEFINITE_WAIT,&m_CurrRequestID);
    if (ret!=LFS_SUCCESS)
    {
        Alarm("04300007");
        emit DeviceError();
        LOGERROR("发送DeviceError事件，调用N_Dispense失败，错误码为：%d",ret);
    }

    return ret;
}

int CMCDM::N_Reject()
{
    int ret=m_apiCtrl.LFSAsyncExecute(LFS_CMD_CDM_REJECT,0,300000,&m_CurrRequestID);
    if (ret!=LFS_SUCCESS)
    {
        Alarm("04300007");
        LOGERROR("调用N_Reject失败，错误码为：%d",ret);
    }
    return ret;
}

int CMCDM::N_Retract(LPLFSCDMRETRACT lpRetract)
{
    int ret=m_apiCtrl.LFSAsyncExecute(LFS_CMD_CDM_RETRACT,lpRetract,300000,&m_CurrRequestID);
    if (ret!=LFS_SUCCESS)
    {
        Alarm("04300007");
        LOGERROR("调用N_Retract失败，错误码为：%d",ret);
    }
    return ret;
}

int CMCDM::N_Reset(LPLFSCDMITEMPOSITION lpResetIn,LONG Timeout)
{
    if (Timeout == -1)
    {
        Timeout=LFS_INDEFINITE_WAIT;
    }

    int ret=m_apiCtrl.LFSAsyncExecute(LFS_CMD_CDM_RESET,lpResetIn,Timeout,&m_CurrRequestID);
    if (ret!=LFS_SUCCESS)
    {
        Alarm("04300007");
        emit ResetFailed();
        LOGERROR("发送ResetFailed事件，调用N_Reset失败，错误码为：%d",ret);
    }
    return ret;
}

//取消Present or DispenseAndPresent命令
int CMCDM::N_Cancel(REQUESTID requestID)
{
    int ret = m_apiCtrl.LFSCancelAsyncRequest(requestID);
    if(LFS_SUCCESS!=ret)
    {
        Alarm("04300007");
        emit DeviceError();
    }

    return ret;
}


int CMCDM::N_GetCashUnitInfo(LPLFSCDMCUINFO lpCashUnitInfo)
{
    ///释放之前分配的内存
    ::LFMFreeBuffer(m_lpCashUnitInfo);

    HRESULT hrResult = LFS_SUCCESS;
    m_lpCashUnitInfo=NULL;

    hrResult=::LFMAllocateBuffer(sizeof(LFSCDMCUINFO), LFS_MEM_FLAGS, (LPVOID *)&m_lpCashUnitInfo);
    if (hrResult != LFS_SUCCESS)
    {
        ::LFMFreeBuffer(m_lpCashUnitInfo);

        LOGERROR("分配LFSCDMCASHINFO结构体内存失败");

        return hrResult;
    }

    hrResult=N_CopyCdmCashInfo((LPVOID)lpCashUnitInfo); //add by art

    return hrResult;
}

//add by art for 一体机 begin ========
HRESULT CMCDM::N_CopyCdmCashInfo(LPVOID p_lpSrc)
{
    HRESULT l_hrResult = LFS_SUCCESS;
    LPLFSCDMCUINFO l_lpSrc = (LPLFSCDMCUINFO)p_lpSrc;
    LPLFSCDMCUINFO l_lpDest = m_lpCashUnitInfo;

    if (NULL == p_lpSrc)
    {
        return l_hrResult;
    }

    m_usCount=l_lpSrc->count;
    m_usTellerID=l_lpSrc->teller_id;
    l_lpDest->count = l_lpSrc->count;
    l_lpDest->teller_id=l_lpSrc->teller_id;

    if (l_lpSrc->count > 0)
    {
        l_hrResult = ::LFMAllocateMore(sizeof(LPLFSCDMCASHUNIT)*l_lpSrc->count, l_lpDest, (LPVOID*)&l_lpDest->list);
        if (LFS_SUCCESS != l_hrResult)
        {
            LFMFreeBuffer(l_lpDest);

            return l_hrResult;
        }

        for (int i = 0; i < l_lpSrc->count; i++)
        {
            l_hrResult = ::LFMAllocateMore(sizeof(LFSCDMCASHUNIT), l_lpDest, (LPVOID*)&l_lpDest->list[i]);
            if (LFS_SUCCESS != l_hrResult)
            {
                LFMFreeBuffer(l_lpDest);

                return l_hrResult;
            }
            memcpy(l_lpDest->list[i], l_lpSrc->list[i], sizeof(LFSCDMCASHUNIT));

            int iLen=strlen(l_lpSrc->list[i]->cash_unit_name);

            l_hrResult = ::LFMAllocateMore(iLen+1, l_lpDest, (LPVOID*)&l_lpSrc->list[i]->cash_unit_name);
            if (LFS_SUCCESS != l_hrResult)
            {
                LFMFreeBuffer(l_lpDest);

                return l_hrResult;
            }

            memcpy(l_lpDest->list[i]->cash_unit_name,l_lpSrc->list[i]->cash_unit_name,iLen);

            if (l_lpSrc->list[i]->num_physical_cus > 0)
            {
                l_hrResult = ::LFMAllocateMore(sizeof(LPLFSCDMPHCU)*l_lpSrc->list[i]->num_physical_cus, l_lpDest, (LPVOID*)&l_lpDest->list[i]->physical);
                if (LFS_SUCCESS != l_hrResult)
                {
                    LFMFreeBuffer(l_lpDest);

                    return l_hrResult;
                }

                for (int t = 0; t < l_lpSrc->list[i]->num_physical_cus; t++)
                {
                    l_hrResult = ::LFMAllocateMore(sizeof(LFSCDMPHCU), l_lpDest, (LPVOID*)&l_lpDest->list[i]->physical[t]);
                    if (LFS_SUCCESS != l_hrResult)
                    {
                        LFMFreeBuffer(l_lpDest);

                        return l_hrResult;
                    }
                    memcpy(l_lpDest->list[i]->physical[t], l_lpSrc->list[i]->physical[t], sizeof(LFSCDMPHCU));

                    if (l_lpSrc->list[i]->physical[t]->physical_position_name != NULL && strlen(l_lpSrc->list[i]->physical[t]->physical_position_name) > 0)
                    {
                        int l_iLen = strlen(l_lpSrc->list[i]->physical[t]->physical_position_name) + 1;
                        l_hrResult = ::LFMAllocateMore(l_iLen, l_lpDest, (LPVOID*)&l_lpDest->list[i]->physical[t]->physical_position_name);
                        if (LFS_SUCCESS != l_hrResult)
                        {
                            LFMFreeBuffer(l_lpDest);

                            return l_hrResult;
                        }
                        memcpy(l_lpDest->list[i]->physical[t]->physical_position_name, l_lpSrc->list[i]->physical[t]->physical_position_name, l_iLen);
                    }
                }
            }

        }
    }

    return l_hrResult;
}
//add by art for 一体机 end========


QString CMCDM::N_DispenseCashUnit(LPLFSCDMDENOMINATION denomination)
{
    int i=0;
    QString CashUnitCount;
    QString DispenseCashUnitCount;
    for (i=0;i<denomination->count;i++)
    {
        CashUnitCount.clear();
        //CashUnitCount.Format("%d",denomination->values[i]);
        CashUnitCount = QString("%1").arg(denomination->values[i]);
        if (i<denomination->count-1)
        {
            DispenseCashUnitCount+=(CashUnitCount+",");
        }
        else
        {
            DispenseCashUnitCount+=CashUnitCount;
        }

    }
    LOGINFO("MCDM模块返回各钞箱出钞张数为：%s",Q2S(DispenseCashUnitCount));

    m_nDispenseAmount = denomination->amount;
    LOGINFO("MCDM模块本次出钞金额为：%d", m_nDispenseAmount);

    return DispenseCashUnitCount;
}

QString CMCDM::N_XFSDevStatusToKalDevStatus(DWORD dwStatus)
{
    if (dwStatus==LFS_CDM_DEVONLINE)
    {
        LOGINFO("MCDM模块设备状态为ONLINE");
        return "ONLINE";
    }
    else if (dwStatus==LFS_CDM_DEVOFFLINE)
    {
        LOGINFO("MCDM模块设备状态为OFFLINE");
        return "OFFLINE";
    }
    else if (dwStatus==LFS_CDM_DEVPOWEROFF)
    {
        LOGINFO("MCDM模块设备状态为POWEROFF");
        return "POWEROFF";
    }
    else if (dwStatus==LFS_CDM_DEVNODEVICE)
    {
        LOGINFO("MCDM模块设备状态为NODEVICE");
        return "NODEVICE";
    }
    else if (dwStatus==LFS_CDM_DEVHWERROR)
    {
        LOGINFO("MCDM模块设备状态为HARDWAREERROR");
        return "HARDWAREERROR";
    }
    else if (dwStatus==LFS_CDM_DEVUSERERROR)
    {
        LOGINFO("MCDM模块设备状态为USERERROR");
        return "USERERROR";
    }
    else if (dwStatus==LFS_CDM_DEVBUSY)
    {
        LOGINFO("MCDM模块设备状态为BUSY");
        return "BUSY";
    }
    else if (dwStatus==LFS_CDM_DEVPOWEROFF)
    {
        LOGINFO("MCDM模块设备状态为FRAUDATTEMPT");
        return "FRAUDATTEMPT";
    }
    else
    {
        LOGINFO("MCDM模块设备状态为POTENTIALFRAUD");
        return "POTENTIALFRAUD";
    }
}

QString CMCDM::N_XFSCUTypeToKalCUType(USHORT usType)
{
    if (usType==LFS_CDM_TYPENA)
    {
        return "NOTAPPLICABLE";
    }
    else if(usType==LFS_CDM_TYPEREJECTCASSETTE)
    {
        return "REJECTCASSETTE";
    }
    else if (usType==LFS_CDM_TYPEBILLCASSETTE)
    {
        return "BILLCASSETTE";
    }
    else if (usType==LFS_CDM_TYPECOINCYLINDER)
    {
        return "COINCYLINDER";
    }
    else if (usType==LFS_CDM_TYPERETRACTCASSETTE)
    {
        return "RETRACTCASSETTE";
    }
    else if (usType==LFS_CDM_TYPECOINDISPENSER)
    {
        return "COINDISPENSER";
    }
    else if (usType==LFS_CDM_TYPECOUPON)
    {
        return "COUPON";
    }
    else if (usType==LFS_CDM_TYPEDOCUMENT)
    {
        return "DOCUMENT";
    }
    else if (usType==LFS_CDM_TYPEREPCONTAINER)
    {
        return "REPCONTAINER";
    }
    else if (usType==LFS_CDM_TYPERECYCLING)
    {
        return "RECYLCLINGCASSETTE";
    }
    else
    {
        Alarm("04300003");
        /////SP返回类型错误
        LOGINFO("SP返回类型错误");
        return "";
    }
}

QString CMCDM::N_XFSCUStatusToKalCUStatus(USHORT usStatus)
{
    if (usStatus==LFS_CDM_STATCUOK)
    {
        return "HEALTHY";
    }
    else if (usStatus==LFS_CDM_STATCUFULL)
    {
        return "FULL";
    }
    else if (usStatus==LFS_CDM_STATCUHIGH)
    {
        return "HIGH";
    }
    else if (usStatus==LFS_CDM_STATCULOW)
    {
        return "LOW";
    }
    else if (usStatus==LFS_CDM_STATCUEMPTY)
    {
        return "empty";
    }
    else if (usStatus==LFS_CDM_STATCUINOP)
    {
        return "INOPERATIVE";
    }
    else if (usStatus==LFS_CDM_STATCUMISSING)
    {
        return "MISSING";
    }
    else if (usStatus==LFS_CDM_STATCUNOVAL)
    {
        return "NOVALUES";
    }
    else if (usStatus==LFS_CDM_STATCUNOREF)
    {
        return "NOREFERENCE";
    }
    else if (usStatus==LFS_CDM_STATCUMANIP)
    {
        return "MANIPULATED";
    }
    else
    {
        Alarm("04300003");
        //////SP返回钞箱状态错误
        LOGERROR("SP返回钞箱状态错误");
        return "";
    }
}

int CMCDM::Execute(int ID, LPVOID lpData)
{
    LOGINFO("%s，方法。ID=%d", __FUNCTION__, ID);

    int nRet = 0;

    switch (ID)
    {
    case LFS_INF_CDM_STATUS:
    {
        nRet = N_GetStatus();
        if (LFS_SUCCESS == nRet)
        {
            memcpy(lpData, &m_Status, sizeof(LFSCDMSTATUS));
        }
    }
        break;
    case LFS_INF_CDM_CASH_UNIT_INFO:
    {
        nRet = N_CashUnitInfo();
        if (LFS_SUCCESS == nRet && m_lpCashUnitInfo != NULL)
        {
            memcpy(lpData,m_lpCashUnitInfo,sizeof(LFSCDMCUINFO));
        }

        LFMFreeBuffer(m_lpCashUnitInfo);
    }
        break;
        /////////////约定此ID返回出钞口信息
    case LFS_INF_CDM_TELLER_INFO:
    {
        nRet = N_GetStatus();
        if (LFS_SUCCESS == nRet)
        {
            memcpy(lpData, &m_PosStatus, sizeof(LFSCDMOUTPOS));
        }
    }
        break;
    default:
        break;
    }

    return nRet;
}

//得到 MCRM.ini 配置文件的路径
void CMCDM::GetConfigPath()
{
    QString strFilePath = g_Helper.GetModuleFileName((void*)&CMCDM::GetInstance);
    if(!strFilePath.isEmpty())
    {
        strFilePath = strFilePath.mid(0,strFilePath.indexOf('/'));
        strFilePath += "/ini/MCRM.ini";
        m_strConfigPath = strFilePath;
    }
    else
    {
        m_strConfigPath = "";
    }
}

SHORT CMCDM::SetUnitTypeSync(QString Type)//新增 设置钞箱类型 AAA 未完成
{
    //////只改钞箱类型，，不改计数，需要更新为最新钞箱计数
    int ret=N_CashUnitInfo();
    if (ret != LFS_SUCCESS)
    {
        return -1;
    }

    if (Type.isEmpty())
    {
        Alarm("04300006");
        LOGINFO("MCDM模块调用SetUnitTypeSync命令参数错误");
        return KAL_PARAM_ERROR;
    }

    if (m_aSetType.size()!=0)
    {
        m_aSetType.empty();
    }
    LOGINFO("MCDM模块开始调用SetUnitTypeSync命令Type=%s",Q2S(Type));
    int i=0;
    QString strSub;
    QString csNumber;
    QString csType;
    CDMSETTYPE SetUnitType;
    while(!(strSub = Type.section(',', i)).isEmpty())
    {
        csNumber = strSub.section(',', i);
        csType = strSub.section(',', i);
        SetUnitType.strUnitNum=csNumber;
        SetUnitType.fwType=atoi(Q2S(csType));
        m_aSetType.push_back(SetUnitType);
        LOGINFO("MCDM模块钞箱为%s,类型为%d",Q2S(SetUnitType.strUnitNum),SetUnitType.fwType);
        i++;
    }
    /*
    while(::AfxExtractSubString(strSub,Type,i,','))
    {
        ::AfxExtractSubString(csNumber,strSub,0,':');
        ::AfxExtractSubString(csType,strSub,1,':');
        SetUnitType.strUnitNum=csNumber;
        SetUnitType.fwType=atoi(csType);
        m_aSetType.Add(SetUnitType);
        LOGINFO("MCDM模块钞箱为%s,类型为%d",SetUnitType.strUnitNum,SetUnitType.fwType);
        i++;
    }*/

    return 0;
}

SHORT CMCDM::SetUnitCountSync(QString Unit)//新增 设置钞箱钞票张数 AAA 未完成
{
    LOGINFO("开始调用SetUnitCountSync命令，先统计数据再清机");

    if (Unit.isEmpty())
    {
        Alarm("04300006");
        LOGINFO("MCDM模块调用SetUnitCountSync命令参数错误");
        return KAL_PARAM_ERROR;
    }

    if (m_aSetCount.size()!=0)
    {
        m_aSetCount.empty();
    }
    LOGINFO("MCDM模块开始调用SetUnitCountSync命令Unit=%s",Q2S(Unit));
    int i=0;
    QString strSub;
    QString csNumber;
    QString csCount;
    CDMSETCOUNT SetUnitCount;
    while(!(strSub = Unit.section(',', i)).isEmpty())
    {
        csNumber = strSub.section(',', i);
        csCount = strSub.section(',', i);
        SetUnitCount.strUnitNum=csNumber;
        SetUnitCount.ulCount=atoi(Q2S(csCount));
        m_aSetCount.push_back(SetUnitCount);
        LOGINFO("MCDM模块钞箱为%s,类型为%d",Q2S(SetUnitCount.strUnitNum),SetUnitCount.ulCount);
        i++;
    }
    /*
    while(::AfxExtractSubString(strSub,Unit,i,','))
    {
        ::AfxExtractSubString(csNumber,strSub,0,':');
        ::AfxExtractSubString(csCount,strSub,1,':');
        SetUnitCount.strUnitNum=csNumber;
        SetUnitCount.count=atoi(csCount);
        m_aSetCount.Add(SetUnitCount);
        LOGINFO("MCDM模块钞箱为%s,张数为%d",SetUnitCount.strUnitNum,SetUnitCount.count);
        i++;
    }*/
    return 0;
}

SHORT CMCDM::SetUnitValueSync(QString Value)//新增 设置钞箱面额 AAA 未完成
{
    //////只改钞箱面额，，不改计数，需要更新为最新钞箱计数
    int ret=N_CashUnitInfo();
    if (ret != LFS_SUCCESS)
    {
        return -1;
    }

    if (Value.isEmpty())
    {
        Alarm("04300006");
        LOGINFO("MCDM模块调用SetUnitValueSync命令参数错误");
        return KAL_PARAM_ERROR;
    }

    if (m_aSetValue.size()!=0)
    {
        m_aSetValue.empty();
    }
    LOGINFO("MCDM模块开始调用SetUnitValueSync命令Value=%s",Q2S(Value));
    int i=0;
    QString strSub;
    QString csNumber;
    QString csValue;
    CDMSETVALUES SetUnitValue;
    while(!(strSub = Value.section(',', i)).isEmpty())
    {
        csNumber = strSub.section(',', i);
        csValue = strSub.section(',', i);
        SetUnitValue.strUnitNum=csNumber;
        SetUnitValue.ulValues=atoi(Q2S(csValue));
        m_aSetValue.push_back(SetUnitValue);
        LOGINFO("MCDM模块钞箱为%s,类型为%d",Q2S(SetUnitValue.strUnitNum),SetUnitValue.ulValues);
        i++;
    }
    /*
    while(::AfxExtractSubString(strSub,Value,i,','))
    {
        ::AfxExtractSubString(csNumber,strSub,0,':');
        ::AfxExtractSubString(csValue,strSub,1,':');
        SetUnitValue.strUnitNum=csNumber;
        SetUnitValue.values=atoi(csValue);
        m_aSetValue.Add(SetUnitValue);
        LOGINFO("MCDM模块钞箱为%s,面额为%d",SetUnitValue.strUnitNum,SetUnitValue.values);
        i++;
    }*/

    return 0;
}

QObject *CreateInstance()
{
    return CMCDM::GetInstance();
}
