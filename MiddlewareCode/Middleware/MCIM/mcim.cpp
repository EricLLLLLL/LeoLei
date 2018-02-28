// Last updated at 2015/12/8 18:57:47

#include "stdafx.h"
#include "mcim.h"
#include <QSettings>
#include "Helper/helper.h"
#include "win_type_def.h"
#include "common_def.h"

#define KAL_PARAM_ERROR -10000
#define  LFS_MEM_FLAGS  (LFS_MEM_SHARE | LFS_MEM_ZEROINIT)

CMCIM* CMCIM::m_pInstance = NULL;

CMCIM::CMCIM(const QString& strDBusObjName):CWebInteractObj(strDBusObjName)
{
    LOGINFO("MCIM窗口创建");
    m_bPartRefused=FALSE;
    ////初始化定义的成员变量
    N_InitStatus();
    m_lpCashInfo=NULL;
    m_bOpened=false;

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
}

CMCIM::~CMCIM()
{
    LOGINFO("MCIM模块退出");
    m_aTotalNoteNumber.empty();
    m_aSetCount.empty();
    m_aSetType.empty();
    m_aSetValue.empty();

    m_mapNoteIdToValue.clear();
    m_mapUno.clear(); //add by art

    ::LFMFreeBuffer(m_lpCashInfo);
    m_lpCashInfo=NULL;
}

CMCIM *CMCIM::GetInstance()
{
    if(m_pInstance == NULL)
    {
        m_pInstance = new CMCIM("MCIM");
    }
    return m_pInstance;
}


void CMCIM::N_InitStatus()
{
    m_Status.device=LFS_CIM_DEVOFFLINE;
    m_Status.acceptor=LFS_CIM_ACCCUUNKNOWN;
    m_Status.safe_door=LFS_CIM_DOORUNKNOWN;
    m_PosStatus.position=LFS_CIM_POSINTOP;
    m_PosStatus.position_status=LFS_CIM_PSUNKNOWN;
    m_PosStatus.shutter=LFS_CIM_SHTUNKNOWN;
    m_PosStatus.transport=LFS_CIM_TPUNKNOWN;
    m_PosStatus.transport_status=LFS_CIM_TPSTATNOTEMPTY_UNK;
}


void CMCIM::WR_OpenComplete(DWORD dwCommand, HRESULT hResult, LPVOID buffer)
{
    //异步打开成功
    if(hResult == LFS_SUCCESS)
    {
        DWORD dwEvent = SERVICE_EVENTS | USER_EVENTS | SYSTEM_EVENTS | EXECUTE_EVENTS;
        int ret = m_apiCtrl.LFSAsyncRegister(dwEvent,&m_CurrRequestID);

        if(LFS_SUCCESS != ret)
        {
            Alarm("04300009");
            LOGERROR("MCIM模块Register命令失败，错误码为：%d",ret);
            N_Close();
            emit OpenFailed();  // 改为 OpenFailed AAA
        }
    }
    else
    {
        Alarm("04300008");
        LOGERROR("MCIM模块Open命令失败，错误码为：%d",hResult);
        emit OpenFailed();  // 改为 OpenFailed AAA
    }
}

void  CMCIM::WR_RegisterComplete(DWORD dwCommand, HRESULT hResult, LPVOID buffer)
{
    if(hResult!=LFS_SUCCESS)
    {
        Alarm("04300009");
        LOGERROR("MCIM模块Register命令失败，错误码为：%d",hResult);
        N_Close();
        emit OpenFailed();  // 改为 OpenFailed AAA
    }
    else
    {
        int ret=N_GetCaps();
        if (ret == LFS_SUCCESS)
        {

            //add by art for 一体机 begin ===1==
            int ret=N_GetBankNoteTypes();
            if (ret == LFS_SUCCESS)
            {
                //add by art for 一体机 end ===1==
                // 获取钞箱信息
                N_CashUnitInfo();
                LFMFreeBuffer(m_lpCashInfo);//////用完之后马上释放  防止泄露

                m_bOpened = true;
                emit ConnectionOpened();
                LOGINFO("MCIM模块打开成功,发送ConnectionOpened事件");

                //add by art for 一体机 begin =====2=====
            }
            else
            {
                Alarm("04300008");
                LOGERROR("MCIM模块GetBankNoteTypes命令失败，错误码为：%d",ret);
                N_Close();
                emit OpenFailed();  // 改为 OpenFailed AAA
            }
            //add by art for 一体机 end =====2=====

        }
        else
        {
            Alarm("04300008");
            LOGERROR("MCIM模块GetCaps命令失败，错误码为：%d",ret);
            N_Close();
            emit OpenFailed();  // 改为 OpenFailed AAA
        }
    }
}

// 收到关闭完成消息后调用该函数
void CMCIM::WR_CloseComplete(DWORD dwCommand, HRESULT hResult, LPVOID buffer)
{
    if(LFS_SUCCESS == hResult)
    {
        LOGINFO("MCIM模块关闭设备成功");
        m_bOpened = false;
        emit ConnectionClosed();
        LOGINFO("MCIM模块发送ConnectionClosed事件");
    }
    else
    {
        LOGERROR("MCIM模块关闭设备失败，错误码为：%d",hResult);
        emit CloseFailed();  // 改为 CloseFailed AAA
    }
}

// 收到打开完成消息后调用该函数
void CMCIM::WR_GetinfoComplete(DWORD dwCommand, HRESULT hResult, LPVOID buffer)
{
    switch(dwCommand)
    {
    case LFS_INF_CIM_CASH_UNIT_INFO:
        if (hResult == LFS_SUCCESS )
        {
            if (buffer == NULL || ((LPLFSRESULT)buffer)->buffer == NULL)
            {
                LOGERROR("MCIM模块WR_GetinfoComplete(LFS_INF_CIM_CASH_UNIT_INFO)返回空指针");
                return;
            }

            LPLFSCIMCASHINFO lpCashInfo=(LPLFSCIMCASHINFO)((LPLFSRESULT)buffer);
            if (lpCashInfo)
            {
                LOGINFO("MCIM模块获取LFS_INF_CIM_CASH_UNIT_INFO成功");
                N_GetCashUnitInfo(lpCashInfo);

                GetRemainDepositAmount();	// 更新可存金额信息
            }
            else
            {
                LOGERROR("WR_GetinfoComplete(LFS_INF_CIM_CASH_UNIT_INFO)失败，SP返回数据为空");
            }
        }
        else
        {
            LOGERROR("WR_GetinfoComplete(LFS_INF_CIM_CASH_UNIT_INFO)失败, 错误码为：%d", hResult);
        }

        break;
    }

}

// 收到执行完成消息后调用该函数
void CMCIM::WR_ExecuteComplete(DWORD dwCommand, HRESULT hResult, LPVOID buffer)
{
    if (hResult == LFS_ERR_HARDWARE_ERROR)
    {
        char ErrorCode[9]={0};
        DWORD ErrorCodeSize=9;

        QSettings reg("HKEY_LOCAL_MACHINE\\SOFTWARE\\HEMEI\\ErrorCode", QSettings::NativeFormat);
        reg.setValue("URT", "1");
        /*
                DWORD QueryType=REG_SZ;
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
        LOGERROR("ErrorCode=%s",ErrorCode);*/
        Alarm(ErrorCode);
    }

    switch(dwCommand)
    {
    case LFS_CMD_CIM_CASH_IN_START:
    {
        if(hResult==LFS_SUCCESS)
        {

            // xiongjie start 20161014 获取NoteID等信息，以防CIM Open时未获取到值
            if (m_mapNoteIdToValue.empty())
            {
                N_GetBankNoteTypes();
            }
            // xiongjie end

            emit this->AcceptCashPrepared();
            LOGINFO("MCIM模块调用LFS_CMD_CIM_CASH_IN_START命令成功,发送AcceptCashPrepared事件");
        }
        else if (hResult==LFS_ERR_CIM_CASHINACTIVE)
        {
            Alarm("04300001");
            emit this->AcceptCashAlreadyActive();
            LOGERROR("MCIM模块调用LFS_CMD_CIM_CASH_IN_START命令失败,发送AcceptCashAlreadyActive事件");
        }
        else
        {
            Alarm("04300002");
            emit this->AcceptCashPrepareFailed();  // AcceptCashPrepareFailed  AAA
            LOGERROR("MCIM模块调用LFS_CMD_CIM_CASH_IN_START命令失败,发送AcceptCashPrepareFailed事件,错误码为：%d",hResult);
        }
    }
        break;
    case LFS_CMD_CIM_CASH_IN:
    {
        if (hResult==LFS_ERR_CANCELED)
        {
            emit this->CashAcceptCancelled();
            LOGINFO("MCIM模块调用LFS_CMD_CIM_CASH_IN命令被取消,发送CashAcceptCancelled事件");
            break;
        }
        else //if (hResult==LFS_SUCCESS)  失败的情况也正常返回
        {
            if (buffer == NULL )
            {
                LOGERROR("MCIM模块调用LFS_CMD_CIM_CASH_IN时SP返回空指针");
                emit this->CashAccepted(m_csValueCount, m_bPartRefused);	// xiongjie 20161015 存款有异常也要上报存款详情
                LOGINFO("MCIM模块发送CashAccepted(%s,%d)事件", m_csValueCount.toStdString().c_str(), m_bPartRefused);
                return;
            }

            int i=0;
            int ValueTypeCount=0; 	//add by art for 一体机
            QString csValueCount;
            QString csValueCountTmp;

            if (((LPLFSRESULT)buffer) == NULL)
            {
                emit this->CashAccepted(m_csValueCount, m_bPartRefused);
                LOGINFO("MCIM模块调用LFS_CMD_CIM_CASH_IN命令成功,发送CashAccepted(%s,%d)事件", m_csValueCount.toStdString().c_str(), m_bPartRefused);
                return;
            }

            // 改变noteID对应面额保存方式，采用MAP,调整结构 AAA
            //////将每次cashin 的结果保存在m_aTotalNoteNumber中
            N_TotalItems((LPLFSCIMNOTENUMBERLIST)((*(LPLFSRESULT)buffer).buffer));

            ///先将NoteID 转换为面额，再加入到aValueCount中 加面额相等的count相加
            QVector<CIMValueCount> aValueCount;

            for (i=0;i<m_aTotalNoteNumber.size();i++)
            {
                CIMValueCount aValueCountTmp;

                NoteItr Itr=m_mapNoteIdToValue.find(m_aTotalNoteNumber.at(i).note_id);
                if (Itr != m_mapNoteIdToValue.end())
                {
                    aValueCountTmp.ulValues=m_mapNoteIdToValue[m_aTotalNoteNumber.at(i).note_id];
                }
                aValueCountTmp.ulValues=m_aTotalNoteNumber.at(i).count;

                int j=0;
                for (j=0;j<aValueCount.size();j++)
                {
                    if (aValueCount.at(j).ulValues == aValueCountTmp.ulValues)
                    {
                        aValueCountTmp.ulCount+=aValueCount.at(j).ulCount;
                        aValueCount.insert(j, aValueCountTmp);
                        //aValueCount.SetAt(j,aValueCountTmp);
                        break;
                    }
                }
                if (j == aValueCount.size())////没有相同面额
                {
                    aValueCount.insert(j,aValueCountTmp);
                }

            }

            int k=0;
            for (k=0;k<aValueCount.size();k++)
            {
                csValueCountTmp = QString("%1%2").arg( aValueCount.at(k).ulValues, aValueCount.at(k).ulCount);
                //csValueCountTmp.Format("%d:%d",aValueCount.at(k).values,aValueCount.at(k).count);
                if (k<aValueCount.size()-1)
                {
                    csValueCount+=(csValueCountTmp+",");
                }
                else
                {
                    csValueCount+=csValueCountTmp;
                }
            }

            m_Dsto.AddPersistentData("~ADDSNR~","","");	 //add by art for 冠字号信息保存

            m_csValueCount = csValueCount;		// xiongjie 20161015 保存每次CashIn的累计入钞详情
            emit this->CashAccepted(csValueCount,m_bPartRefused);
            LOGINFO("MCIM模块调用LFS_CMD_CIM_CASH_IN命令成功,发送CashAccepted(%s,%d)事件",csValueCount.toStdString().c_str(),m_bPartRefused);
        }
    }
        break;
    case LFS_CMD_CIM_CASH_IN_END:
    {
        if (hResult==LFS_SUCCESS)
        {

            m_CashInABFalg=FALSE;
            m_CashInUnitCount.isEmpty();
            if (buffer == NULL ||((LPLFSRESULT)buffer)->buffer == NULL ||((LPLFSCIMCASHINFO)((LPLFSRESULT)buffer)->buffer)->cash_in == NULL)
            {
                Alarm("04300003");
                LOGINFO("MCIM模块调用LFS_CMD_CIM_CASH_IN_END时SP返回空指针"); //modify by art
                emit this->DeviceError();
                LOGINFO("MCIM模块发送DeviceError事件");
                return;
            }
            QString csCashInUnitCountTmp;
            int i=0;

            for (i=0;i<((LPLFSCIMCASHINFO)((LPLFSRESULT)buffer)->buffer)->cashin_count;i++)
            {

                //add by art begin
                char szUnitIdTmp[6]={0};
                memcpy(szUnitIdTmp, ((LPLFSCIMCASHINFO)((LPLFSRESULT)buffer)->buffer)->cash_in[i]->currency_id, 5);
                //add by art end
                csCashInUnitCountTmp = QString("%1%2").arg( m_mapUno[szUnitIdTmp].data(), ((LPLFSCIMCASHINFO)((LPLFSRESULT)buffer)->buffer)->cash_in[i]->cash_in_count);
                //csCashInUnitCountTmp.Format("%s:%d",m_mapUno[szUnitIdTmp].data(),((LPLFSCIMCASHINFO)((LPLFSRESULT)buffer)->buffer)->cash_in[i]->count); //modify by art
                if (((LPLFSCIMCASHINFO)((LPLFSRESULT)buffer)->buffer)->cash_in[i]->type == LFS_CIM_TYPERETRACTCASSETTE &&
                        ((LPLFSCIMCASHINFO)((LPLFSRESULT)buffer)->buffer)->cash_in[i]->count > 0)
                {
                    m_CashInABFalg=TRUE;
                }

                if (i<(((LPLFSCIMCASHINFO)((LPLFSRESULT)buffer)->buffer)->cashin_count)-1)
                {
                    m_CashInUnitCount+=(csCashInUnitCountTmp+",");
                }
                else
                {
                    m_CashInUnitCount+=csCashInUnitCountTmp;
                }

            }
            emit this->EscrowedCashStored(m_CashInUnitCount,m_CashInABFalg);
            LOGINFO("MCIM模块调用LFS_CMD_CIM_CASH_IN_END命令成功,发送EscrowedCashStored事件,m_CashInUnitCount=%s,m_CashInABFalg=%d",Q2S(m_CashInUnitCount),m_CashInABFalg);
        }
        else if(hResult == LFS_ERR_CIM_NOITEMS)   // xiongjie start 1013
        {
            emit this->EscrowedCashStored("0:0", FALSE);
            LOGINFO("MCIM模块调用LFS_CMD_CIM_CASH_IN_END命令结束，发送EscrowedCashStored事件，本次存款0张");
        }   // xiongjie end
        else
        {
            //Alarm("04300004");
            emit this->EscrowedCashStoreFailed();  // 添加 EscrowedCashStoreFailed  AAA
            LOGERROR("MCIM模块调用LFS_CMD_CIM_CASH_IN_END命令失败,发送EscrowedCashStoreFailed事件,错误码为:%d",hResult);
        }

        // 获取钞箱信息  xiongjie add
        N_CashUnitInfoAsync();

    }
        break;
    case LFS_CMD_CIM_RESET:
    {
        if (hResult==LFS_SUCCESS)
        {
            emit this->ResetComplete();
            LOGINFO("MCIM模块调用LFS_CMD_CIM_RESET命令成功,发送ResetComplete事件");
        }
        else
        {
            Alarm("05400006");
            emit this->ResetFailed();   // ResetFailed AAA
            LOGERROR("MCIM模块调用LFS_CMD_CIM_RESET命令失败,发送ResetFailed事件,错误码为：%d",hResult);
        }
    }
        break;
    case LFS_CMD_CIM_START_EXCHANGE:
    {
        if (hResult==LFS_SUCCESS || hResult==LFS_ERR_CIM_EXCHANGEACTIVE) //modify by art for LFS_ERR_CIM_EXCHANGEACTIVE 也当做成功
        {
            if (buffer == NULL ||((LPLFSRESULT)buffer)->buffer == NULL)
            {
                Alarm("04300003");
                LOGERROR("MCIM模块调用LFS_CMD_CIM_START_EXCHANGE时SP返回空指针");
                emit this->StartExchangeFailed();   // 改为StartExchangeFailed AAA
                LOGERROR("MCIM模块发送StartExchangeFailed事件");
                return;
            }
            N_GetCashUnitInfo((LPLFSCIMCASHINFO)((LPLFSRESULT)buffer)->buffer);
            emit this->StartExchangeCompleted();   // 改为StartExchangeCompleted AAA
            LOGINFO("MCIM模块调用LFS_CMD_CIM_START_EXCHANGE命令成功,发送StartExchangeCompleted事件");
        }
        else
        {
            Alarm("05400004");
            emit this->StartExchangeFailed();  // 改为StartExchangeFailed AAA
            LOGERROR("MCIM模块调用LFS_CMD_CIM_START_EXCHANGE命令失败,发送StartExchangeFailed事件,错误码为：%d",hResult);
        }
    }
        break;
    case LFS_CMD_CIM_END_EXCHANGE:
    {
        if (hResult==LFS_SUCCESS)
        {
            /////将加钞张数写入永久数据
            int i=0;
            for (i=0;i<m_aSetCount.size();i++)
            {
                char BoxName[40]={0};
                char BoxCount[10]={0};

                /*sprintf(BoxName,"ENCASEBILLS%s",m_aSetCount.at(i).strUnitNum); *///
                sprintf(BoxName,"ENCASEBILLS%d",i+1);
                sprintf(BoxCount,"%d",m_aSetCount.at(i).ulCount);
                m_Dsto.D_SetDataString(TRUE,BoxName,BoxCount);
                LOGINFO("%s加钞张数：%s",BoxName,BoxCount);
            }
            emit this->EndExchangeCompleted();  // 改为 EndExchangeCompleted AAA
            LOGINFO("MCIM模块调用LFS_CMD_CIM_END_EXCHANGE命令成功,发送ExchangeCompleted事件");
            //////清除数据
            m_aSetCount.empty();
            m_aSetType.empty();
            m_aSetValue.empty();
        }
        else
        {
            Alarm("05400005");
            emit this->EndExchangeFailed();  // 改为 EndExchangeFailed AAA
            LOGERROR("MCIM模块调用LFS_CMD_CIM_END_EXCHANGE命令失败,发送EndExchangeFailed事件,错误码为：%d",hResult);
            //////清除数据
            m_aSetCount.empty();
            m_aSetType.empty();
            m_aSetValue.empty();
        }
    }
        break;
    case LFS_CMD_CIM_OPEN_SHUTTER:
    {
        if (hResult==LFS_SUCCESS)
        {
            emit this->ShutterOpened();
            LOGINFO("MCIM模块调用LFS_CMD_CIM_OPEN_SHUTTER命令成功,发送ShutterOpened事件");
        }
        else
        {
            Alarm("05400007");
            emit this->ShutterOpenFailed();  // 改为 ShutterOpenFailed AAA
            LOGERROR("MCIM模块调用LFS_CMD_CIM_OPEN_SHUTTER命令失败,发送ShutterOpenFailed事件");
        }
    }
        break;
    case LFS_CMD_CIM_CLOSE_SHUTTER:
    {
        if (hResult==LFS_SUCCESS)
        {
            emit this->ShutterClosed();
            LOGINFO("MCIM模块调用LFS_CMD_CIM_CLOSE_SHUTTER命令成功,发送ShutterClosed事件");
        }
        else
        {
            Alarm("05400008");
            emit this->ShutterCloseFailed(); // 改为 ShutterCloseFailed AAA
            LOGERROR("MCIM模块调用LFS_CMD_CIM_OPEN_SHUTTER命令失败,发送ShutterCloseFailed事件");
        }
    }
        break;
    case LFS_CMD_CIM_CASH_IN_ROLLBACK:
    {
        if (hResult==LFS_SUCCESS)
        {
            //start xuzhe add 20160405
            QString strInfo("");
            QString str("");
            LPLFSCIMCASHINFO lpCashInfo = (LPLFSCIMCASHINFO)((LPLFSRESULT)buffer)->buffer;
            if(lpCashInfo == NULL)
            {
                emit EscrowedCashEjectFailed();   // 改为 EscrowedCashEjectFailed AAA
                LOGERROR("LPLFSCIMCASHINFO指针为空，参数错误");
            }
            else
            {
                for(int i = 0;i<lpCashInfo->cashin_count;++i)
                {
                    LPLFSCIMCASHIN lpCashIn = *((lpCashInfo->cash_in)+i);
                    if(lpCashIn == NULL)
                    {
                        emit EscrowedCashEjectFailed();   // 改为 EscrowedCashEjectFailed AAA
                        LOGERROR("LPLFSCIMCASHIN指针为空，参数错误,i = %d,count = %d",i,lpCashInfo->cashin_count);
                        return;
                    }
                    else
                    {
                        //add by art begin
                        char szUnitIdTmp[6]={0};
                        memcpy(szUnitIdTmp, lpCashIn->unit_id, sizeof(lpCashIn->unit_id));
                        //add by art end

                        //钞箱编号：面值：张数
                        //str.Format("%d:%d:%d",lpCashIn->number,lpCashIn->values,lpCashIn->count);
                        str = QString("%1%2%3").arg(QString(m_mapUno[szUnitIdTmp].c_str()).arg(lpCashIn->values).arg(lpCashIn->count));
                        //str.Format("%s:%d:%d", m_mapUno[szUnitIdTmp].data(),lpCashIn->values,lpCashIn->count); //modify by art
                        strInfo += str + ",";
                    }
                }

                if(strInfo != "")
                    strInfo = strInfo.mid(0,strInfo.length()-1);//去掉最后一个逗号

                emit this->EscrowedCashEjected(strInfo);
                LOGINFO("MCIM模块调用LFS_CMD_CIM_CASH_IN_ROLLBACK命令成功,发送EscrowedCashEjected事件");
                LOGINFO("RollBack各个钞箱出钞信息(钞箱编号：面值：张数)：%s",strInfo.toStdString().c_str());

            }
            //end xuzhe


            //this->EscrowedCashEjected(strInfo);
            //LOGINFO("MCIM模块调用LFS_CMD_CIM_CASH_IN_ROLLBACK命令成功,发送EscrowedCashEjected事件");
        }
        else
        {
            emit this->EscrowedCashEjectFailed();   // 改为 EscrowedCashEjectFailed AAA
            LOGERROR("MCIM模块调用LFS_CMD_CIM_CASH_IN_ROLLBACK命令失败,发送DeviceError事件");
        }
    }
        break;
    case LFS_CMD_CIM_CONFIGURE_NOTETYPES:
    {
        if (hResult == LFS_SUCCESS)
        {
            emit SetSupportValueCompleted();
            LOGINFO("MCIM模块调用LFS_CMD_CIM_CONFIGURE_NOTETYPES命令成功,发送SetSupportValueCompleted事件");
        }
        else
        {
            emit SetSupportValueFailed();
            LOGINFO("MCIM模块调用LFS_CMD_CIM_CONFIGURE_NOTETYPES命令失败,发送SetSupportValueFailed事件,错误码为：%d",hResult);
        }

    }
        break;
    }
}

// 收到执行事件后调用该函数
void CMCIM::WR_ExecuteEvent(DWORD dwEventID, HRESULT hResult, LPVOID buffer)
{
    switch(dwEventID)
    {
    case LFS_EXEE_CIM_INPUTREFUSE:
    {
        emit this->CashRefused();
        m_bPartRefused=TRUE;
        LOGINFO("MCIM模块发送CashRefused事件");
    }
        break;
    case LFS_EXEE_CIM_CASHUNITERROR:
        //case LFS_EXEE_CIM_INPUT_P6:
        //case LFS_EXEE_CIM_NOTEERROR:
        //case LFS_EXEE_CIM_SUBCASHIN:
        //case LFS_EXEE_CIM_INFO_AVAILABLE:
        //case LFS_EXEE_CIM_INSERTITEMS:
        break;

    }

}

// 收到用户事件后调用该函数
void CMCIM::WR_UserEvent(DWORD dwEventID, HRESULT hResult, LPVOID buffer)
{

}

// 收到服务事件后调用该函数
void CMCIM::WR_ServiceEvent(DWORD dwEventID, HRESULT hResult, LPVOID buffer)
{
    switch(dwEventID)
    {
    case LFS_SRVE_CIM_ITEMSTAKEN:
    {
        emit this->CashTaken();
        LOGINFO("MCIM模块发送CashTaken事件");
    }
        break;
    case LFS_SRVE_CIM_ITEMSPRESENTED:
    {
        emit this->RefusedCashEjected();
        LOGINFO("MCIM模块发送RefusedCashEjected事件");
    }
        break;
    case LFS_SRVE_CIM_ITEMSINSERTED:
    {
        emit this->CashInserted();
        LOGINFO("MCIM模块发送CashInserted事件");
    }
        break;
    case LFS_SRVE_CIM_CASHUNITINFOCHANGED:
    {
        LOGINFO("%s, 收到LFS_SRVE_CIM_CASHUNITINFOCHANGED事件", __FUNCTION__);
        N_CashUnitInfoAsync();    // 更新钞箱信息
    }
        break;
    case LFS_SRVE_CIM_SAFEDOOROPEN:
    {
        Alarm("04300005");
        emit this->SafeDoorOpened();
        LOGINFO("MCIM模块发送SafeDoorOpened事件");
    }
        break;
    case LFS_SRVE_CIM_SAFEDOORCLOSED:
    {
        emit this->SafeDoorClosed();
        LOGINFO("MCIM模块发送SafeDoorClosed事件");
    }
        break;
    case LFS_SRVE_CIM_COUNTS_CHANGED:
    {
        LOGINFO("%s, 收到LFS_SRVE_CIM_COUNTS_CHANGED事件", __FUNCTION__);
        N_CashUnitInfoAsync();    // 更新钞箱信息
    }
        break;

    }

}

// 收到系统事件后调用该函数
void CMCIM::WR_SystemEvent(DWORD dwEventID, HRESULT hResult, LPVOID buffer)
{
    switch(dwEventID)
    {
    case LFS_SYSE_DEVICE_STATUS:
    {
        LOGINFO("MCIM模块收到SP上送的LFS_SYSE_DEVICE_STATUS事件");
        LPLFSDEVSTATUS lpDevStatus=(LPLFSDEVSTATUS)((LPLFSRESULT)buffer)->buffer;
        m_CurDevStatus=N_XFSDevStatusToKalDevStatus(lpDevStatus->state);
        m_PreDevStatus=N_XFSDevStatusToKalDevStatus(m_Status.device);
        if (m_CurDevStatus != m_PreDevStatus) //modify by art
        {
            emit this->StatusChanged(m_szLogicalName,m_PreDevStatus,m_CurDevStatus);
            LOGINFO("MCIM模块发送StatusChanged事件,m_PreDevStatus=%s,m_CurDevStatus=%s",Q2S(m_PreDevStatus),Q2S(m_CurDevStatus));


            //tmp add by art begin for 解决"设备离线 OFFLINE时SP获取BankNoteTypes返回0"的异常
            if(m_mapNoteIdToValue.empty() && m_CurDevStatus=="ONLINE")
                N_GetBankNoteTypes();
            //tmp add by art end
        }
    }
        break;
    }
}


// 以下为属性的处理函数


void CMCIM::SetServiceName(QString newVal)
{
    LOGINFO("%s,属性,newVal = %s",__FUNCTION__,newVal.toStdString().c_str());

    strcpy(m_szLogicalName,newVal.toStdString().c_str());
}


QString CMCIM::GetLastAcceptStatus(void)
{
    int ret=N_CashInStatus();
    QString csAcceptStatus;
    if (ret!=LFS_SUCCESS)
    {
        Alarm("04300003");
        LOGERROR("MCIM模块调用N_CashInStatus失败,错误码为：%d",ret);
        return "";
    }
    else
    {
        if (m_CashInStatus.status == LFS_CIM_CIOK)
        {
            csAcceptStatus = "ACCEPTED";
        }
        else if (m_CashInStatus.status == LFS_CIM_CIROLLBACK)
        {
            csAcceptStatus = "EJECTED";
        }
        else if (m_CashInStatus.status == LFS_CIM_CIACTIVE)
        {
            csAcceptStatus = "ACTIVE";
        }
        else if (m_CashInStatus.status == LFS_CIM_CIRETRACT)
        {
            csAcceptStatus = "RETRACTED";
        }
        else
        {
            csAcceptStatus = "UNKNOWN";
        }
    }
    LOGINFO("%s,属性,存款模块状态 = %s",__FUNCTION__,csAcceptStatus.toStdString().c_str());
    return csAcceptStatus;
}


QString CMCIM::GetStDetailedDeviceStatus(void)
{
    int ret = N_GetStatus();
    QString csDeviceStatus;
    csDeviceStatus=N_XFSDevStatusToKalDevStatus(m_Status.device);
    LOGINFO("%s,属性,设备状态 = %s",__FUNCTION__,csDeviceStatus.toStdString().c_str());
    return csDeviceStatus;
}


QString CMCIM::GetStEscrowStatus(void)
{

    return "NOESCROW";
}

QString CMCIM::GetStShutterStatus(void)
{
    int ret = N_GetStatus();
    QString strShutterStatus("");
    switch(m_PosStatus.shutter)
    {
    case LFS_CIM_SHTCLOSED:
        strShutterStatus = "CLOSED";
        break;
    case LFS_CIM_SHTOPEN:
        strShutterStatus = "OPEN";
        break;
    default:
        strShutterStatus = "UNKNOWN";
    }
    LOGINFO("%s,属性,入钞口状态 = %s",__FUNCTION__,strShutterStatus.toStdString().c_str());
    return strShutterStatus;
}

QString CMCIM::GetStTransportStatus(void)
{
    QString csTransportStatus;
    if ((*(m_Status.positions))->transport==LFS_CIM_TPOK)
    {
        csTransportStatus = "HEALTHY";
    }
    else if ((*(m_Status.positions))->transport==LFS_CIM_TPINOP)
    {
        csTransportStatus = "FATAL";
    }
    else
    {
        csTransportStatus = "UNKNOWN";
    }

    LOGINFO("%s,属性,通道状态 = %s",__FUNCTION__,csTransportStatus.toStdString().c_str());
    return csTransportStatus;
}

// 改为 GetStInputStatus AAA
QString CMCIM::GetStInputStatus(void)
{
    //xuzhe start 0419 add
    int ret = N_GetStatus();
    QString strStatus("");

    switch(m_PosStatus.position_status)
    {
    case LFS_CDM_PSEMPTY:
        strStatus = "EMPTY";
        break;
    case LFS_CDM_PSNOTEMPTY:
        strStatus = "NOTEMPTY";
        break;
    default:
        strStatus = "UNKNOWN";
    }

    LOGINFO("%s,属性,入钞口状态 = %s",__FUNCTION__,strStatus.toStdString().c_str());

    //xuzhe end 0419 add

    return strStatus;
}

QString CMCIM::GetStAcceptorStatus(void)
{
    int ret = N_GetStatus();
    QString csAcceptorStatus;
    if (m_Status.acceptor==LFS_CIM_ACCOK)
    {
        csAcceptorStatus = "OK";
    }
    else if (m_Status.acceptor==LFS_CIM_ACCCUSTATE)
    {
        csAcceptorStatus = "STATE";
    }
    else if (m_Status.acceptor==LFS_CIM_ACCCUSTOP)
    {
        csAcceptorStatus = "STOP";
    }
    else
    {
        csAcceptorStatus = "UNKNOWN";
    }

    LOGINFO("%s,属性,存款模块状态 = %s",__FUNCTION__,csAcceptorStatus.toStdString().c_str());
    return csAcceptorStatus;
}

QString CMCIM::GetStSafeDoorStatus(void)
{
    int ret = N_GetStatus();
    QString csSafeDoorStatus;
    if (m_Status.safe_door==LFS_CIM_DOORNOTSUPPORTED)
    {
        csSafeDoorStatus = "NOTSUPPORTED";
    }
    else if (m_Status.safe_door==LFS_CIM_DOOROPEN)
    {
        csSafeDoorStatus = "OPEN";
    }
    else if (m_Status.safe_door==LFS_CIM_DOORCLOSED)
    {
        csSafeDoorStatus = "CLOSED";
    }
    else
    {
        csSafeDoorStatus = "UNKNOWN";
    }
    LOGINFO("%s,属性,安全门状态 = %s",__FUNCTION__,csSafeDoorStatus.toStdString().c_str());
    return csSafeDoorStatus;
}


BOOL CMCIM::GetCpCanEscrow(void)
{
    LOGINFO("%s,属性 fwIntermediateStacker = %d",__FUNCTION__,m_Caps.intermediate_stacker);
    return m_Caps.intermediate_stacker;
}

BOOL CMCIM::GetCpShutterControlSupported(void)
{
    LOGINFO("%s,属性 bShutterControl = %d",__FUNCTION__,m_Caps.shutter_control);
    return m_Caps.shutter_control;
}

LONG CMCIM::GetCpMaxAcceptItems(void)
{
    LOGINFO("%s,属性 wMaxCashInItems = %d",__FUNCTION__,m_Caps.max_cashin_items);
    return m_Caps.max_cashin_items;
}

BOOL CMCIM::GetCpCanDetectCashInserted(void)
{
    LOGINFO("%s,属性 bItemsInsertedSensor = %d",__FUNCTION__,m_Caps.items_inserted_sensor);
    return m_Caps.items_inserted_sensor;
}

BOOL CMCIM::GetCpCanDetectCashTaken(void)
{
    LOGINFO("%s,属性 bItemsTakenSensor = %d",__FUNCTION__,m_Caps.items_taken_sensor);
    return m_Caps.items_taken_sensor;
}

QString CMCIM::GetCpRetractAreas(void)
{
    QString csRetractAreas;
    if (m_Caps.retract_areas==LFS_CIM_RA_RETRACT)
    {
        csRetractAreas = "RETRACT";
    }
    //else if (m_Caps.retract_areas==LFS_CIM_RA_REJECT)
    //{
    //	csRetractAreas = "REJECT";
    //}
    else if (m_Caps.retract_areas==LFS_CIM_RA_TRANSPORT)
    {
        csRetractAreas = "TRANSPORT";
    }
    else if (m_Caps.retract_areas==LFS_CIM_RA_STACKER)
    {
        csRetractAreas = "ESCROW";
    }
    else if (m_Caps.retract_areas==LFS_CIM_RA_BILLCASSETTES)
    {
        csRetractAreas = "CASHUNITS";
    }
    else
    {
        csRetractAreas = "NOTSUPPORTED";   ///kal无此返回值
    }

    LOGINFO("%s,属性,RetractAreas = %s",__FUNCTION__,csRetractAreas.toStdString().c_str());
    return csRetractAreas;
}

QString CMCIM::GetCpExchangeTypes(void)
{
    QString csExchangeType;
    if (m_Caps.exchange_type==LFS_CIM_EXBYHAND)
    {
        csExchangeType = "BYHAND";
    }
    else if (m_Caps.exchange_type==LFS_CIM_EXTOCASSETTES)
    {
        csExchangeType = "REPLENISHMENTUNIT";
    }
    else if (m_Caps.exchange_type==LFS_CIM_CLEARRECYCLER)
    {
        csExchangeType = "CLEARRECYCLER";
    }
    else
    {
        csExchangeType = "FROMDEPOSIT";
    }

    LOGINFO("%s,属性,ExchangeType = %s",__FUNCTION__,csExchangeType.toStdString().c_str());
    return csExchangeType;
}


LONG CMCIM::GetNumOfRefused(void)
{
    int ret = N_CashInStatus();
    if (ret != LFS_SUCCESS)
    {
        Alarm("04300003");
        LOGERROR("MCIM模块调用N_CashInStatus失败,错误码为：%d",ret);
        return 0;
    }

    LONG ulNumOfRefused = 0;
    ulNumOfRefused = m_CashInStatus.num_of_refused;
    LOGINFO("%s,属性,本次存款拒钞张数 = %d",__FUNCTION__,ulNumOfRefused);

    return ulNumOfRefused;
}

// 获取各钞箱状态，格式例如：1A:FULL,2A:LOW ...
QString CMCIM::GetCUStatus(void)
{
    QString strStatus;
    int ret = N_CashUnitInfo();
    if (LFS_SUCCESS != ret)
    {
        LOGERROR("MCIM模块调用GetCUStatus时获取钞箱状态失败");
        return "";
    }

    for (int i = 0; i < m_lpCashInfo->cashin_count; i++)
    {
        QString strUnitName;
        char szUnitIdTmp[6]={0};
        memcpy(szUnitIdTmp, m_lpCashInfo->cash_in[i]->currency_id, 5);
        strUnitName = QString("%1").arg(m_mapUno[szUnitIdTmp].data());
        //strUnitName.Format("%s:", m_mapUno[szUnitIdTmp].data());
        strStatus += (strUnitName + N_XFSCUStatusToKalCUStatus(m_lpCashInfo->cash_in[i]->status) + ",");
    }

    if(strStatus.size()>1)
    {
        strStatus = strStatus.left(strStatus.length()-1); // 去除最后的","
    }

    LOGINFO("MCIM模块返回钞箱状态为：%s", Q2S(strStatus));

    return strStatus;
}

// 剩余可存金额
void CMCIM::GetRemainDepositAmount(void)
{
    LONG nRemainAmount = 0;

    for (int i = 0; i < m_lpCashInfo->cashin_count; i++)
    {
        ULONG values = m_lpCashInfo->cash_in[i]->values;	// 钞箱面额
        LONG lCounts = m_lpCashInfo->cash_in[i]->maximum + 200 - m_lpCashInfo->cash_in[i]->count;  // 钞箱还可存入的张数
        if (lCounts < 0 || LFS_CIM_STATCUFULL == m_lpCashInfo->cash_in[i]->status || LFS_CIM_STATCUMISSING == m_lpCashInfo->cash_in[i]->status ||
                LFS_CIM_STATCUINOP == m_lpCashInfo->cash_in[i]->status || LFS_CIM_STATCUMANIP == m_lpCashInfo->cash_in[i]->status)
        {
            lCounts = 0;
        }

        nRemainAmount += values * lCounts;
    }

    m_Dsto.D_SetDataLong(FALSE, "REMAINDEPOSITAMOUNT", nRemainAmount);
    LOGINFO("可存金额%d", nRemainAmount);
}

// 以下为方法的处理函数


SHORT CMCIM::CloseConnection(void)
{
    LOGINFO("%s，方法。",__FUNCTION__);
    int ret = LFS_SUCCESS;
    if (m_bOpened)
    {
        ret=N_Close();
    }
    return ret;
}

SHORT CMCIM::OpenConnection(void)
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
        return LFS_SUCCESS;
    }
}


SHORT CMCIM::AcceptCash(LONG Timeout)
{
    LOGINFO("%s，方法。Timeout=%d",__FUNCTION__,Timeout);
    m_bPartRefused=FALSE;
    int ret=N_CashIn(Timeout);
    return ret;
}


SHORT CMCIM::CancelAcceptCash(void)
{
    LOGINFO("%s，方法。",__FUNCTION__);
    int ret=N_Cancel(m_CashInID);
    return ret;
}

SHORT CMCIM::StoreEscrowedCash(LONG Timeout)
{
    LOGINFO("%s，方法。Timeout=%d",__FUNCTION__,Timeout);
    m_aTotalNoteNumber.empty();    //清除张数计数
    int ret=N_CashInEnd(Timeout);
    return ret;
}

SHORT CMCIM::EjectEscrowedCash(LONG Timeout)
{
    LOGINFO("%s，方法。Timeout=%d",__FUNCTION__,Timeout);
    int ret=N_CashInRollBack(Timeout);
    return ret;
}



SHORT CMCIM::OpenShutter(LONG Timeout)
{
    LOGINFO("%s，方法。Timeout=%d",__FUNCTION__,Timeout);
    WORD fwPosition;
    fwPosition=LFS_CIM_POSNULL;
    int ret=N_OpenShutter(&fwPosition,Timeout);
    return ret;
}

SHORT CMCIM::CloseShutter(LONG Timeout)
{
    LOGINFO("%s，方法。Timeout=%d",__FUNCTION__,Timeout);
    WORD fwPosition;
    fwPosition=LFS_CIM_POSNULL;
    int ret=N_CloseShutter(&fwPosition,Timeout);
    return ret;
}


// 添加超时 AAA
SHORT CMCIM::Reset(QString RetractArea, SHORT ID,LONG Timeout)
{
    LOGINFO("%s，方法。RetractArea=%s,ID=%d,Timeout=%d",__FUNCTION__,RetractArea.toStdString().c_str(),ID,Timeout);
    LFSCIMITEMPOSITION ResetIn;
    LFSCIMRETRACT  RetractAreaTmp;

    memset(&RetractAreaTmp,0,sizeof(LFSCIMRETRACT));
    ResetIn.retract_area=&RetractAreaTmp;
    ResetIn.number=ID;		//modify by art
    RetractAreaTmp.index=0; //modify by art
    ResetIn.output_position=LFS_CIM_POSNULL;

    if (RetractArea == "RETRACT")
    {
        RetractAreaTmp.retract_area=LFS_CIM_RA_RETRACT;
    }
    else if ((RetractArea=="TRANSPORT"))
    {
        RetractAreaTmp.retract_area=LFS_CIM_RA_TRANSPORT;
    }
    else if ((RetractArea=="ESCROW"))
    {
        RetractAreaTmp.retract_area=LFS_CIM_RA_STACKER;
    }
    else if ((RetractArea=="CASHUNITS"))
    {
        RetractAreaTmp.retract_area=LFS_CIM_RA_BILLCASSETTES;
    }
    else if ((RetractArea=="EJECT"))
    {
        //RetractAreaTmp.retract_area=LFS_CIM_RA_REJECT;
    }
    else if ((RetractArea=="REJECT"))
    {
        //RetractAreaTmp.retract_area=LFS_CIM_RA_REJECT;
    }
    int ret=N_Reset(&ResetIn,Timeout);
    return ret;
}

LONG CMCIM::GetLastAcceptedAmountSync(QString Currency, QString Validity)
{
    LOGINFO("%s，方法。Currency=%s,Validity=%s",__FUNCTION__,Currency.toStdString().c_str(),Validity.toStdString().c_str());
    if (Currency.isEmpty()|| Validity.isEmpty())
    {
        Alarm("04300006");
        LOGINFO("应用调用GetLastAcceptedAmountSync传入参数错误");
        return -1;
    }
    LONG LastAcceptAmount=0;
    if((Validity=="VALID"))
    {
        int i=0;
        for (i=0;i<m_aTotalNoteNumber.size();i++)
        {
            NoteItr Itr=m_mapNoteIdToValue.find(m_aTotalNoteNumber.at(i).note_id);
            if (Itr != m_mapNoteIdToValue.end())
            {
                LastAcceptAmount+=((m_aTotalNoteNumber.at(i).count) * m_mapNoteIdToValue[m_aTotalNoteNumber.at(i).note_id]); //modify by art for 一体机
            }

        }
    }
    LOGINFO("GetLastAcceptedAmountSync命令完成,返回金额为：%d",LastAcceptAmount);
    return LastAcceptAmount;
}


// 改为 StartExchange， 不要参数  AAA
void CMCIM::StartExchange()
{
    LOGINFO("%s，方法。",__FUNCTION__);
    LFSCIMSTARTEX StartEx;
    LFSCIMOUTPUT  Output;

    memset(&Output,NULL,sizeof(LFSCIMOUTPUT));

    StartEx.exchange_type=LFS_CIM_EXBYHAND;
    StartEx.output=&Output;
    StartEx.count=0;
    StartEx.teller_id=0;
    StartEx.cu_numList=0;
    int ret=N_StartExchage(&StartEx);
}

SHORT CMCIM::StatisticsData() //modify by art
{
    LOGINFO("%s，方法。",__FUNCTION__);
    int ret=N_StatisticsData();
    LFMFreeBuffer(m_lpCashInfo);
    return ret;
}

SHORT CMCIM::N_StatisticsData() //add by art
{
    int ret=N_CashUnitInfo();
    if (ret != LFS_SUCCESS)
    {
        return -1;
    }

    int k=0;
    int iBoxTotalCount=0;
    char cBoxTotalCount[10]={0};

    int nCimABAmount=0; //add by art for 计算回收箱存款金额
    int nCdmABAmount=0; //add by art for 计算回收箱出钞RJ金额

    for (k=0;k<m_lpCashInfo->cashin_count;k++)
    {
        //add by art begin
        /*char szUnitIdTmp[6]={0};
        memcpy(szUnitIdTmp, m_lpCashInfo->cash_in[k]->cUnitID, 5);
                char cBoxNumTmp[3]={0};
                sprintf(cBoxNumTmp,"%s",m_mapUno[szUnitIdTmp].data());*/
        //add by art end

        char BoxName[40]={0};
        char BoxDispensedCount[10]={0};
        char BoxCashInCount[10]={0};
        char BoxRejectCount[10]={0};
        //sprintf(BoxName,"ATMC_BOX%dDISPENSEFMT",m_lpCashInfo->cash_in[k]->number);
        //sprintf(BoxName,"ATMC_BOX%sDISPENSEFMT",cBoxNumTmp); //modify by art
        sprintf(BoxName,"ATMC_BOX%dDISPENSEFMT",k+1); //modify by zyf
        sprintf(BoxDispensedCount,"%d",m_lpCashInfo->cash_in[k]->count);
        sprintf(BoxCashInCount,"%d",m_lpCashInfo->cash_in[k]->cash_in_count);
        sprintf(BoxRejectCount,"%d",m_lpCashInfo->cash_in[k]->number);
        m_Dsto.D_SetDataString(TRUE,BoxName,BoxDispensedCount);
        memset(BoxName,0,40);
        //sprintf(BoxName,"ATMC_BOX%dDEPOSITFMT",m_lpCashInfo->cash_in[k]->number);
        //sprintf(BoxName,"ATMC_BOX%sDEPOSITFMT",cBoxNumTmp); //modify by art
        sprintf(BoxName,"ATMC_BOX%dDEPOSITFMT",k+1); //modify by zyf
        m_Dsto.D_SetDataString(TRUE,BoxName,BoxCashInCount);
        memset(BoxName,0,40);
        //sprintf(BoxName,"ATMC_BOX%dDISPENSERJFMT",m_lpCashInfo->cash_in[k]->number);
        //sprintf(BoxName,"ATMC_BOX%sDISPENSERJFMT",cBoxNumTmp); //modify by art
        sprintf(BoxName,"ATMC_BOX%dDISPENSERJFMT",k+1); //modify by zyf
        m_Dsto.D_SetDataString(TRUE,BoxName,BoxRejectCount);
        LOGINFO("%s统计数据：BoxDispensedCount=%s, BoxCashInCount=%s,BoxRejectCount=%s",BoxName,BoxDispensedCount,BoxCashInCount,BoxRejectCount);
        if (m_lpCashInfo->cash_in[k]->type == LFS_CIM_TYPERECYCLING || m_lpCashInfo->cash_in[k]->type == LFS_CIM_TYPECASHIN)
        {
            iBoxTotalCount+=m_lpCashInfo->cash_in[k]->count;
        }

        //add by art begin for 计算回收箱存款金额
        if(LFS_CIM_TYPERETRACTCASSETTE==m_lpCashInfo->cash_in[k]->type)
        {
            LPLFSCIMNOTENUMBERLIST note_number_list=m_lpCashInfo->cash_in[k]->note_number_list;
            if( (note_number_list!=NULL) && (note_number_list->num_of_note_numbers>0))
            {
                ULONG uValue=0;
                for(int i=0; i<note_number_list->num_of_note_numbers; i++)
                {
                    uValue=0;
                    NoteItr Itr=m_mapNoteIdToValue.find(note_number_list->note_number[i]->note_id);
                    if (Itr != m_mapNoteIdToValue.end())
                    {
                        uValue=m_mapNoteIdToValue[note_number_list->note_number[i]->note_id];
                    }

                    nCimABAmount+=note_number_list->note_number[i]->count*uValue;
                }

            }
            else
            {
                LOGINFO("回收箱入钞明细没有!!");
            }
        }
        //add by art end for 计算回收箱存款金额

        //add by art begin for 计算回收箱出钞RJ金额
        nCdmABAmount+=m_lpCashInfo->cash_in[k]->count*m_lpCashInfo->cash_in[k]->values; //注意RJ面值不可靠，有可能是NG钞
        //add by art end for 计算回收箱出钞RJ金额
    }

    //add by art begin for 计算回收箱总金额
    m_Dsto.D_SetDataLong(FALSE,"ABAMOUNT",  nCimABAmount);////// SP修改后 总回收箱=回收明细（取款拒钞也加入回收明细中）
    //m_Dsto.D_SetDataLong(FALSE,"ABAMOUNT",  nCimABAmount+nCdmABAmount); //总回收箱=入钞回收+出钞拒钞
    LOGINFO("回收箱估算金额(CIM+CDM)：%d = %d + %d", nCimABAmount+nCdmABAmount, nCimABAmount, nCdmABAmount);
    //add by art end for 计算回收箱总金额

    sprintf(cBoxTotalCount,"%d",iBoxTotalCount);
    m_Dsto.D_SetDataString(TRUE,"ALLBOXNOTENUMBER2",cBoxTotalCount);
    LOGINFO("钞箱剩余总张数（不含回收箱）：%s",cBoxTotalCount);

    return 0;
}


SHORT CMCIM::SetUnitCountSync(QString Unit) //modify by art
{
    SHORT iRet=N_StatisticsData();
    if(iRet!=0)
    {
        return iRet;
    }

    if (Unit.isEmpty())
    {
        Alarm("04300006");
        LOGINFO("MCIM模块调用SetUnitCountSync命令参数错误");
        return KAL_PARAM_ERROR;
    }

    LOGINFO("%s，方法。Unit=%s",__FUNCTION__,Unit.toStdString().c_str());
    if (m_aSetCount.size()!=0)
    {
        m_aSetCount.empty();
    }
    int i=0;
    QString strSub;
    QString csNumber;
    QString csCount;
    CIMSETCOUNT SetUnitCount;

    while(!(strSub = Unit.section(',', i)).isEmpty())
    {
        csNumber = strSub.section(':', 0);
        csCount = strSub.section(':', 1);

        SetUnitCount.strUnitNum=csNumber;
        SetUnitCount.ulCount=atoi(csCount.toStdString().c_str());
        m_aSetCount.push_back(SetUnitCount);
        LOGINFO("MCIM模块钞箱为%s,张数为%d",Q2S(SetUnitCount.strUnitNum),SetUnitCount.ulCount);
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
        LOGINFO("MCIM模块钞箱为%s,张数为%d",SetUnitCount.strUnitNum,SetUnitCount.count);
                i++;
    }*/
    return 0;
}


void CMCIM::EndExchange(void)
{
    LOGINFO("%s，方法。",__FUNCTION__);
    int i=0;
    int j=0;

    ///////在清机加钞之前将剩余总张数，出钞张数和入钞张数以及出RJ张数保存至数据模块


    if (m_aSetCount.size() > 0)
    {
        for (i=0;i<m_aSetCount.size();i++)
        {
            for (j=0;j<m_lpCashInfo->cashin_count;j++)
            {
                //add by art begin
                char szUnitIdTmp[6]={0};
                memcpy(szUnitIdTmp, m_lpCashInfo->cash_in[j]->unit_id, 5);
                //add by art end

                if ((m_aSetCount.at(i).strUnitNum==m_mapUno[szUnitIdTmp].data())) //modify by art
                {
                    m_lpCashInfo->cash_in[j]->count=m_aSetCount.at(i).ulCount;
                    break;
                }
            }
        }
    }

    if (m_aSetType.size() > 0)
    {
        for (i=0;i<m_aSetType.size();i++)
        {
            for (j=0;j<m_lpCashInfo->cashin_count;j++)
            {
                //add by art begin
                char szUnitIdTmp[6]={0};
                memcpy(szUnitIdTmp, m_lpCashInfo->cash_in[j]->unit_id, 5);
                //add by art end

                if ((m_aSetType.at(i).strUnitNum==m_mapUno[szUnitIdTmp].data())) //modify by art
                {
                    m_lpCashInfo->cash_in[j]->type=m_aSetType.at(i).fwType;
                    break;
                }
            }
        }
    }

    if (m_aSetValue.size() > 0)
    {
        for (i=0;i<m_aSetValue.size();i++)
        {
            for (j=0;j<m_lpCashInfo->cashin_count;j++)
            {
                //add by art begin
                char szUnitIdTmp[6]={0};
                memcpy(szUnitIdTmp, m_lpCashInfo->cash_in[j]->unit_id, 5);
                //add by art end

                //if (m_aSetCount.at(i).number == m_lpCashInfo->cash_in[j]->number)
                if (!strcmp(Q2S(m_aSetValue.at(i).strUnitNum),m_mapUno[szUnitIdTmp].data())) //modify by art
                {
                    m_lpCashInfo->cash_in[j]->values=m_aSetValue.at(i).ulValues;
                    break;
                }
            }
        }
    }

    int ret=N_EndExchage();
    ////调完END_EXCHAGE立即释放内存

    ::LFMFreeBuffer(m_lpCashInfo);
}


/////////设置钞箱类型
SHORT CMCIM::SetUnitTypeSync(QString Type)
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
        LOGINFO("MCIM模块调用SetUnitTypeSync命令参数错误");
        return KAL_PARAM_ERROR;
    }

    if (m_aSetType.size()!=0)
    {
        m_aSetType.empty();
    }
    LOGINFO("%s，方法。Type=%s",__FUNCTION__,Type.toStdString().c_str());
    int i=0;
    QString strSub;
    QString csNumber;
    QString csType;
    CIMSETTYPE SetUnitType;

    while(!(strSub = Type.section(',', i)).isEmpty())
    {
        csNumber = strSub.section(':', 0);
        csType = strSub.section(':', 1);

        SetUnitType.strUnitNum=csNumber;
        SetUnitType.fwType=atoi(csType.toStdString().c_str());
        m_aSetType.push_back(SetUnitType);
        LOGINFO("MCIM模块钞箱为%s,类型为%d",Q2S(SetUnitType.strUnitNum),SetUnitType.fwType);
        i++;
    }
    /*
        while(::AfxExtractSubString(strSub,Type,i,','))
        {
                ::AfxExtractSubString(csNumber,strSub,0,':');
                ::AfxExtractSubString(csType,strSub,1,':');
                SetUnitType.strUnitNum=csNumber;
        SetUnitType.type=atoi(csType);
                m_aSetType.Add(SetUnitType);
        LOGINFO("MCIM模块钞箱为%s,类型为%d",SetUnitType.strUnitNum,SetUnitType.type);
                i++;
    }*/

    return 0;
}

///////设置钞箱面额  
SHORT CMCIM::SetUnitValueSync(QString Value)
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
        LOGINFO("MCIM模块调用SetUnitValueSync命令参数错误");
        return KAL_PARAM_ERROR;
    }

    if (m_aSetValue.size()!=0)
    {
        m_aSetValue.empty();
    }
    LOGINFO("%s，方法。Value=%s",__FUNCTION__,Value.toStdString().c_str());
    int i=0;
    QString strSub;
    QString csNumber;
    QString csValue;
    CIMSETVALUES SetUnitValue;

    while(!(strSub = Value.section(',', i)).isEmpty())
    {
        csNumber = strSub.section(':', 0);
        csValue = strSub.section(':', 1);

        SetUnitValue.strUnitNum=csNumber;
        SetUnitValue.ulValues=atoi(csValue.toStdString().c_str());
        m_aSetValue.push_back(SetUnitValue);
        LOGINFO("MCIM模块钞箱为%s,面额为%d",SetUnitValue.strUnitNum.toStdString().c_str(),SetUnitValue.ulValues);
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
        LOGINFO("MCIM模块钞箱为%s,面额为%d",SetUnitValue.strUnitNum,SetUnitValue.values);
                i++;
    }*/

    return 0;
}

////////////设置支持哪些面额存入
SHORT CMCIM::SetSupportValue(QString Value)
{
    LOGINFO("%s，方法。Value=%s",__FUNCTION__,Value.toStdString().c_str());
    QVector<USHORT> vNoteId;
    QString csValue;
    int i=0;
    USHORT* note_id=NULL;

    while(!(csValue = Value.section(',', i)).isEmpty())
    {
        QMap<USHORT, ULONG>::const_iterator Itr = m_mapNoteIdToValue.constBegin();
        while (Itr != m_mapNoteIdToValue.constEnd()) {
            if (Itr.value() == atoi(Q2S(csValue)))
            {
                vNoteId.push_back(Itr.key());
            }
        }

        Itr++;
    }
    /*
        while(::AfxExtractSubString(csValue,Value,i,','))
        {
        const_iterator Itr=m_mapNoteIdToValue.begin();
                for(;Itr!=m_mapNoteIdToValue.end();++Itr)
                {
                        if (Itr->second == atoi(csValue))
                        {
                                vNoteId.push_back(Itr->first);
                        }
                }
                i++;
    }*/

    if(vNoteId.size() >= 0)
    {
        note_id = new USHORT[vNoteId.size()+1];
    }
    else
    {
        Alarm("04300006");
        LOGERROR("调用SetSupportValue传入参数有误");
        return -1;
    }

    QVector<USHORT>::iterator vItr=vNoteId.begin();

    for (i=0;i<vNoteId.size();i++)
    {
        note_id[i]=*vItr;
        ++vItr;
    }
    note_id[i]=NULL;

    int ret=N_ConfigureNoteType(note_id);

    delete[] note_id;

    return ret;
}


LONG CMCIM::PrepareForAcceptCash(void)
{
    LOGINFO("%s，方法.",__FUNCTION__);
    //start xuzhe 20160406 add
    m_aTotalNoteNumber.empty();    //清除张数计数
    //end xuzhe

    m_csValueCount = "0:0";    // xiongjie 20161015

    //add by art begin for 定额存款
    char szCashinMaxAmount[32];
    memset(szCashinMaxAmount, 0x00, sizeof(szCashinMaxAmount));
    short nRetTmp=m_Dsto.D_GetDataString(FALSE, "CASHINMAXAMOUNT", szCashinMaxAmount);
    if(nRetTmp!=0)
        sprintf(szCashinMaxAmount, "0");

    /*
        HKEY  hKey;
        LPCTSTR LpSubKey="SOFTWARE\\XFS\\App\\";
        long lRet = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, LpSubKey, 0,  KEY_WRITE, &hKey);
        if(lRet == ERROR_SUCCESS)
        {
                ::RegSetValueEx(hKey, "CashInTransactionMaxAmount", 0, REG_SZ, (const byte *)szCashinMaxAmount, strlen(szCashinMaxAmount));
        }
        else
        {
                LOGINFO("设置CASHINMAXAMOUNT不成功");
        }
        ::RegCloseKey(hKey);
        //add by art end for 定额存款
*/


    LFSCIMCASHINSTART CashInStart;
    CashInStart.teller_id=0;
    CashInStart.use_recycle_units=TRUE;
    CashInStart.input_position=LFS_CIM_POSNULL;
    CashInStart.output_position=LFS_CIM_POSNULL;
    int ret=N_CashInStart(&CashInStart);
    return ret;
}


int CMCIM::N_ConfigureNoteType(LPUSHORT lpnote_ids)
{
    int ret=m_apiCtrl.LFSAsyncExecute(LFS_CMD_CIM_CONFIGURE_NOTETYPES,lpnote_ids,HM_MIDDLE_TIMEOUT,&m_CurrRequestID);
    if (ret!=LFS_SUCCESS)
    {
        Alarm("04300007");
        emit SetSupportValueFailed();
        LOGERROR("MCIM模块LFS_CMD_CIM_CONFIGURE_NOTETYPES命令失败,错误码为：%d",ret);
    }

    return ret;
}

int CMCIM::N_Open()
{

    int ret = m_apiCtrl.LFSAsyncOpen(m_szLogicalName,0,&m_CurrRequestID);

    if(LFS_SUCCESS!=ret)
    {
        Alarm("04300007");
        emit OpenFailed();
        LOGERROR("MCIM模块Open命令失败,错误码为：%d",ret);
    }

    return ret;
}

int CMCIM::N_Close()
{
    int ret = m_apiCtrl.LFSAsyncClose(&m_CurrRequestID);

    if(LFS_SUCCESS!=ret)
    {
        Alarm("04300007");
        emit CloseFailed();
        LOGERROR("MCIM模块Close命令失败,错误码为：%d",ret);
    }

    return ret;
}

//执行LFS_INF_CIM_STATUS
int CMCIM::N_GetStatus()
{
    LPLFSRESULT lpResult = NULL;
    int ret =m_apiCtrl.LFSGetInfo(LFS_INF_CIM_STATUS, NULL, HM_MIDDLE_TIMEOUT, &lpResult);
    if (ret == LFS_SUCCESS )
    {
        LPLFSCIMSTATUS lpStatus = (LPLFSCIMSTATUS)((LPLFSRESULT)lpResult)->buffer;
        if (lpStatus)
        {
            LOGINFO("MCIM模块获取状态成功");
            N_Status(lpStatus);
        }
        else
        {
            Alarm("04300003");
            ret = -1;
            LOGERROR("MCIM模块获取状态失败，SP返回数据为空");
        }

    }
    else
    {
        Alarm("04300007");
        LOGERROR("MCIM模块GetStatus命令失败,错误码为：%d",ret);
    }

    ::LFSFreeResult(lpResult);
    return ret;
}

//执行LFS_INF_CIM_CAPABILITIES
int CMCIM::N_GetCaps()
{
    LPLFSRESULT lpResult = NULL;
    int ret =m_apiCtrl.LFSGetInfo(LFS_INF_CIM_CAPABILITIES, NULL, HM_MIDDLE_TIMEOUT, &lpResult);
    if (ret == LFS_SUCCESS )
    {
        LPLFSCIMCAPS lpCaps = (LPLFSCIMCAPS)((LPLFSRESULT)lpResult)->buffer;
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
        LOGERROR("MCIM模块GetCaps命令失败,错误码为：%d",ret);
    }

    ::LFSFreeResult(lpResult);
    return ret;
}

//add by art for 一体机 begin
/////LFS_INF_CIM_BANKNOTE_TYPES
int CMCIM::N_GetBankNoteTypes()
{
    LPLFSRESULT lpResult = NULL;
    int ret =m_apiCtrl.LFSGetInfo(LFS_INF_CIM_BANKNOTE_TYPES, NULL, HM_MIDDLE_TIMEOUT, &lpResult);
    if (ret == LFS_SUCCESS )
    {
        LPLFSCIMNOTETYPELIST lpNoteTypeList=(LPLFSCIMNOTETYPELIST)((LPLFSRESULT)lpResult)->buffer;
        if (lpNoteTypeList)
        {
            LOGINFO("MCIM模块获取BANKNOTE_TYPES成功");
            N_CopyNoteType(lpNoteTypeList);
        }
        else
        {
            Alarm("04300003");
            ret = -1;
            LOGERROR("MCIM模块获取BANKNOTE_TYPES失败，SP返回数据为空");
        }


    }
    else
    {
        Alarm("04300007");
        LOGERROR("MCIM模块LFS_INF_CIM_BANKNOTE_TYPES命令失败,错误码为：%d",ret);
    }

    ::LFSFreeResult(lpResult);
    return ret;
}
//add by art for 一体机 end

//////LFS_INF_CIM_CASH_IN_STATUS
int CMCIM::N_CashInStatus()
{
    LPLFSRESULT lpResult = NULL;
    int ret =m_apiCtrl.LFSGetInfo(LFS_INF_CIM_CASH_IN_STATUS, NULL, HM_MIDDLE_TIMEOUT, &lpResult);
    if (ret == LFS_SUCCESS )
    {
        LPLFSCIMCASHINSTATUS lpCashInStatus=(LPLFSCIMCASHINSTATUS)((LPLFSRESULT)lpResult)->buffer;
        if (lpCashInStatus)
        {
            LOGINFO("MCIM模块获取LFS_INF_CIM_CASH_IN_STATUS成功");
            N_GetCashInStatus(lpCashInStatus);
        }
        else
        {
            Alarm("04300003");
            ret = -1;
            LOGERROR("MCIM模块获取LFS_INF_CIM_CASH_IN_STATUS失败，SP返回数据为空");
        }
    }
    else
    {
        Alarm("04300007");
        LOGERROR("MCIM模块CASH_UNIT_STATUS命令失败,错误码为：%d",ret);
    }

    ::LFSFreeResult(lpResult);
    return ret;
}

//////执行LFS_INF_CIM_CASH_UNIT_INFO
int CMCIM::N_CashUnitInfo()
{
    LPLFSRESULT lpResult = NULL;
    int ret =m_apiCtrl.LFSGetInfo(LFS_INF_CIM_CASH_UNIT_INFO, NULL, HM_MIDDLE_TIMEOUT, &lpResult);
    if (ret == LFS_SUCCESS )
    {
        LPLFSCIMCASHINFO lpCashInfo=(LPLFSCIMCASHINFO)((LPLFSRESULT)lpResult)->buffer;
        if (lpCashInfo)
        {
            LOGINFO("MCIM模块获取LFS_INF_CIM_CASH_UNIT_INFO成功");
            N_GetCashUnitInfo(lpCashInfo);

            GetRemainDepositAmount();	// 更新可存金额信息
        }
        else
        {
            Alarm("04300003");
            ret = -1;
            LOGERROR("MCIM模块获取LFS_INF_CIM_CASH_UNIT_INFO失败，SP返回数据为空");
        }
    }
    else
    {
        Alarm("04300007");
        LOGERROR("MCIM模块CASH_UNIT_INFO命令失败,错误码为：%d",ret);
    }

    ::LFSFreeResult(lpResult);
    return ret;
}

// 获取钞箱状态（异步）
int CMCIM::N_CashUnitInfoAsync()
{
    int ret = m_apiCtrl.LFSAsyncGetInfo(LFS_INF_CIM_CASH_UNIT_INFO, 0, 0, &m_CurrRequestID);
    if (ret != LFS_SUCCESS )
    {
        LOGERROR("MCIM模块异步CASH_UNIT_INFO命令失败, 错误码为：%d", ret);
        return ret;
    }

    return 0;
}

//执行LFS_CMD_CIM_CASH_IN_START
int CMCIM::N_CashInStart(LPLFSCIMCASHINSTART lpCashInStart)
{

    int ret=m_apiCtrl.LFSAsyncExecute(LFS_CMD_CIM_CASH_IN_START,lpCashInStart,HM_MIDDLE_TIMEOUT,&m_CurrRequestID);
    if (ret!=LFS_SUCCESS)
    {
        Alarm("04300007");
        emit AcceptCashPrepareFailed();
        LOGERROR("MCIM模块CashInStart命令失败,错误码为：%d",ret);
    }
    return ret;
}

//执行LFS_CMD_CIM_CASH_IN
int CMCIM::N_CashIn(LONG timeout)
{
    if (timeout == -1)
    {
        timeout=LFS_INDEFINITE_WAIT;
    }

    int ret=m_apiCtrl.LFSAsyncExecute(LFS_CMD_CIM_CASH_IN,0,timeout,&m_CurrRequestID);
    if (ret!=LFS_SUCCESS)
    {
        Alarm("04300007");
        emit DeviceError();
        LOGERROR("MCIM模块CashIn命令失败,错误码为：%d",ret);
    }
    m_CashInID=m_CurrRequestID;
    return ret;
}

////执行LFS_CMD_CIM_CASH_IN_END
int CMCIM::N_CashInEnd(LONG Timeout)
{
    if (Timeout == -1)
    {
        Timeout=LFS_INDEFINITE_WAIT;
    }

    int ret=m_apiCtrl.LFSAsyncExecute(LFS_CMD_CIM_CASH_IN_END,0,Timeout,&m_CurrRequestID);
    if (ret!=LFS_SUCCESS)
    {
        Alarm("04300007");
        emit EscrowedCashStoreFailed();
        LOGERROR("MCIM模块CashInEnd命令失败,错误码为：%d",ret);
    }

    return ret;
}

//////执行LFS_CMD_CIM_START_EXCHANGE
int CMCIM::N_StartExchage(LPLFSCIMSTARTEX lpStartEx)
{
    int ret=m_apiCtrl.LFSAsyncExecute(LFS_CMD_CIM_START_EXCHANGE,lpStartEx,HM_MIDDLE_TIMEOUT,&m_CurrRequestID);
    if (ret!=LFS_SUCCESS)
    {
        Alarm("04300007");
        emit StartExchangeFailed();
        LOGERROR("MCIM模块LFS_CMD_CIM_START_EXCHANGE命令失败,错误码为：%d",ret);
    }

    return ret;
}

int CMCIM::N_EndExchage()
{

    int ret=m_apiCtrl.LFSAsyncExecute(LFS_CMD_CIM_END_EXCHANGE,m_lpCashInfo,HM_MIDDLE_TIMEOUT,&m_CurrRequestID); //tmp by art
    if (ret!=LFS_SUCCESS)
    {
        Alarm("04300007");
        emit EndExchangeFailed();
        LOGERROR("MCIM模块LFS_CMD_CIM_END_EXCHANGE命令失败,错误码为：%d",ret);
    }

    return ret;
}

////执行LFS_CMD_CIM_RESET
int CMCIM::N_Reset(LPLFSCIMITEMPOSITION lpResetIn,LONG Timeout)
{
    if (Timeout == -1)
    {
        Timeout=LFS_INDEFINITE_WAIT;
    }

    int ret=m_apiCtrl.LFSAsyncExecute(LFS_CMD_CIM_RESET,lpResetIn,Timeout,&m_CurrRequestID);
    if (ret!=LFS_SUCCESS)
    {
        Alarm("04300007");
        emit ResetFailed();
        LOGERROR("MCIM模块Reset命令失败,错误码为：%d",ret);
    }
    return ret;
}

int CMCIM::N_CashInRollBack(LONG timeout)
{
    if (timeout == -1)
    {
        timeout=LFS_INDEFINITE_WAIT;
    }

    int ret=m_apiCtrl.LFSAsyncExecute(LFS_CMD_CIM_CASH_IN_ROLLBACK,0,timeout,&m_CurrRequestID);
    if (ret!=LFS_SUCCESS)
    {
        Alarm("04300007");
        emit EscrowedCashEjectFailed();
        LOGERROR("MCIM模块CashInRollBack命令失败,错误码为：%d",ret);
    }
    return ret;
}

int CMCIM::N_OpenShutter(LPWORD lpfwPosition,LONG Timeout)
{
    if (Timeout == -1)
    {
        Timeout=LFS_INDEFINITE_WAIT;
    }

    int ret=m_apiCtrl.LFSAsyncExecute(LFS_CMD_CIM_OPEN_SHUTTER,lpfwPosition,Timeout,&m_CurrRequestID);
    if (ret!=LFS_SUCCESS)
    {
        Alarm("04300007");
        emit ShutterOpenFailed();
        LOGERROR("MCIM模块OpenShutter命令失败,错误码为：%d",ret);
    }
    return ret;
}

int CMCIM::N_CloseShutter(LPWORD lpfwPosition,LONG Timeout)
{
    if (Timeout == -1)
    {
        Timeout=LFS_INDEFINITE_WAIT;
    }

    int ret=m_apiCtrl.LFSAsyncExecute(LFS_CMD_CIM_CLOSE_SHUTTER,lpfwPosition,Timeout,&m_CurrRequestID);
    if (ret!=LFS_SUCCESS)
    {
        Alarm("04300007");
        emit ShutterCloseFailed();
        LOGERROR("MCIM模块CloseShutter命令失败,错误码为：%d",ret);
    }
    return ret;
}

//取消AcceptCash命令
int CMCIM::N_Cancel(REQUESTID requestID)
{
    int ret = m_apiCtrl.LFSCancelAsyncRequest(requestID);
    if(LFS_SUCCESS!=ret)
    {
        Alarm("04300007");
        emit CashAcceptCancelFailed();
        LOGERROR("MCIM模块Cancel命令失败,错误码为：%d",ret);
    }

    return ret;
}

int CMCIM::N_Status(LPLFSCIMSTATUS lpStatus)
{
    if (lpStatus == NULL)
    {
        Alarm("04300003");
        LOGINFO("MCIM模块SP返回设备状态为空指针");
        return KAL_PARAM_ERROR;
    }
    m_Status.device=lpStatus->device;
    m_Status.drop_box=lpStatus->drop_box;
    m_Status.acceptor=lpStatus->acceptor;
    m_Status.safe_door=lpStatus->safe_door;
    m_Status.stacker_items=lpStatus->stacker_items;
    m_Status.banknote_reader=lpStatus->banknote_reader;
    m_Status.intermediate_stacker=lpStatus->intermediate_stacker;
    //m_Status.mixedmode=lpStatus->;
    //m_Status.usPowerSaveRecoveryTime=lpStatus->usPowerSaveRecoveryTime;
    //m_Status.wAntiFraudModule=lpStatus->wAntiFraudModule;


    //xuzhe start 0419 add
    //保存入钞口的状态
    if(lpStatus->positions&&lpStatus->positions[0])
    {
        //入钞口
        m_PosStatus.position = lpStatus->positions[0]->position;
        m_PosStatus.position_status = lpStatus->positions[0]->position_status;
        m_PosStatus.shutter = lpStatus->positions[0]->shutter;
        m_PosStatus.transport = lpStatus->positions[0]->transport;
        m_PosStatus.position_status = lpStatus->positions[0]->position_status;
    }
    else
    {
        Alarm("04300003");
        LOGINFO("指针为空");
    }
    //xuzhe end 0419 add

    return 0;
}

int CMCIM::N_Capabilities(LPLFSCIMCAPS lpCaps)
{
    if (lpCaps == NULL)
    {
        LOGINFO("MCIM模块SP返回设备能力为空指针");
        return KAL_PARAM_ERROR;
    }
    m_Caps.cash_box=lpCaps->cash_box;
    //m_Caps.bCompareSignatures=lpCaps->bCompareSignatures;
    //m_Caps.bAntiFraudModule=lpCaps->bAntiFraudModule;
    m_Caps.compound=lpCaps->compound;
    //m_Caps.bDeviceLockControl=lpCaps->bDeviceLockControl;
    m_Caps.items_inserted_sensor=lpCaps->items_inserted_sensor;
    m_Caps.items_taken_sensor=lpCaps->items_taken_sensor;
    //m_Caps.bMixedDepositAndRollback=lpCaps->bMixedDepositAndRollback;
    //m_Caps.bPowerSaveControl=lpCaps->bPowerSaveControl;
    m_Caps.refill=lpCaps->refill;
    //m_Caps.replenish=lpCaps->bReplenish;
    m_Caps.safe_door=lpCaps->safe_door;
    m_Caps.shutter=lpCaps->shutter;
    m_Caps.shutter_control=lpCaps->shutter_control;
    //m_Caps.dwItemInfoTypes=lpCaps->dwItemInfoTypes;
    m_Caps.max_cashin_items=lpCaps->max_cashin_items;
    //m_Caps.fwCountActions=lpCaps;
    m_Caps.exchange_type=lpCaps->exchange_type;
    m_Caps.intermediate_stacker=lpCaps->intermediate_stacker;
    m_Caps.retract_areas=lpCaps->retract_areas;
    m_Caps.retract_stacker_actions=lpCaps->retract_stacker_actions;
    m_Caps.retract_transport_actions=lpCaps->retract_transport_actions;
    m_Caps.dev_type=lpCaps->dev_type;
    m_Caps.dev_class=lpCaps->dev_class;
    m_Caps.max_cashin_items=lpCaps->max_cashin_items;
    //m_Caps.wMixedMode=lpCaps->wMixedMode;

    return 0;
}

int CMCIM::N_GetCashUnitInfo(LPLFSCIMCASHINFO lpCashInfo)
{
    ///释放之前分配的内存
    ::LFMFreeBuffer(m_lpCashInfo);

    HRESULT hrResult = LFS_SUCCESS;
    m_lpCashInfo=NULL;

    hrResult=::LFMAllocateBuffer(sizeof(LFSCIMCASHINFO), LFS_MEM_FLAGS, (LPVOID *)&m_lpCashInfo);
    if (hrResult != LFS_SUCCESS)
    {
        ::LFMFreeBuffer(m_lpCashInfo);

        LOGERROR("分配LFSCIMCASHINFO结构体内存失败");

        return hrResult;
    }

    hrResult=N_CopyCimCashInfo((LPVOID)lpCashInfo); //add by art

    return hrResult;
}

int CMCIM::N_GetCashInStatus(LPLFSCIMCASHINSTATUS lpCashInStatus)
{
    m_CashInStatus.num_of_refused=lpCashInStatus->num_of_refused;
    m_CashInStatus.status=lpCashInStatus->status;
    return 0;
}



LONG CMCIM::N_TotalItems(LPLFSCIMNOTENUMBERLIST lpBuf)
{

    int i=0;
    for (i=0;i<lpBuf->num_of_note_numbers;i++)
    {

        LFSCIMNOTENUMBER NoteNumberTmp;


        NoteNumberTmp.count=lpBuf->note_number[i]->count;
        NoteNumberTmp.note_id=lpBuf->note_number[i]->note_id;
        LOGINFO("MCIM模块存入NoteID:%d 张数:%d",NoteNumberTmp.note_id,NoteNumberTmp.count); //modify by art for 一体机

        m_aTotalNoteNumber.push_back(NoteNumberTmp);

    }
    return 0;
}

QString CMCIM::N_XFSDevStatusToKalDevStatus(DWORD dwStatus)
{
    if (dwStatus==LFS_CIM_DEVONLINE)
    {
        return "ONLINE";
    }
    else if (dwStatus==LFS_CIM_DEVOFFLINE)
    {
        return "OFFLINE";
    }
    else if (dwStatus==LFS_CIM_DEVPOWEROFF)
    {
        return "POWEROFF";
    }
    else if (dwStatus==LFS_CIM_DEVNODEVICE)
    {
        return "NODEVICE";
    }
    else if (dwStatus==LFS_CIM_DEVHWERROR)
    {
        return "HARDWAREERROR";
    }
    else if (dwStatus==LFS_CIM_DEVUSERERROR)
    {
        return "USERERROR";
    }
    else if (dwStatus==LFS_CIM_DEVBUSY)
    {
        return "BUSY";
    }
    //else if (dwStatus==LFS_CIM_DEVFRAUDATTEMPT)
    //{
    //	return "FRAUDATTEMPT";
    //}
    else
    {
        return "POTENTIALFRAUD";
    }
}

QString CMCIM::N_XFSCUStatusToKalCUStatus(USHORT status)
{
    QString strCUStatus = "UNKNOWN";
    switch(status)
    {
    case LFS_CIM_STATCUOK:
        strCUStatus = "HEALTHY";
        break;
    case LFS_CIM_STATCUFULL:
        strCUStatus = "FULL";
        break;
    case LFS_CIM_STATCUHIGH:
        strCUStatus = "HIGH";
        break;
    case LFS_CIM_STATCULOW:
        strCUStatus = "LOW";
        break;
    case LFS_CIM_STATCUEMPTY:
        strCUStatus = "EMPTY";
        break;
    case LFS_CIM_STATCUINOP:
        strCUStatus = "INOPERATIVE";
        break;
    case LFS_CIM_STATCUMISSING:
        strCUStatus = "MISSING";
        break;
    case LFS_CIM_STATCUNOVAL:
        strCUStatus = "NOVALUES";
        break;
    case LFS_CIM_STATCUNOREF:
        strCUStatus = "NOREFERENCE";
        break;
    case LFS_CIM_STATCUMANIP:
        strCUStatus = "MANIPULATED";
        break;
    }

    return strCUStatus;
}

//add by art for 一体机 begin
int CMCIM::N_CopyNoteType(LPLFSCIMNOTETYPELIST lpNoteTypeList)
{
    // xiongjie start 20161014, 多次获取NoteType会出现重复
    if (lpNoteTypeList->num_of_note_types == 0)
    {
        InitialNoteType();
    }
    else if (lpNoteTypeList->num_of_note_types != 0)
    {
        m_mapNoteIdToValue.clear();////清空
    }
    // xiongjie end

    int i=0;
    for (i=0;i<lpNoteTypeList->num_of_note_types;i++)
    {
        m_mapNoteIdToValue[lpNoteTypeList->note_types[i]->note_id]=lpNoteTypeList->note_types[i]->values;
    }

    return 0;
}
//add by art for 一体机 end


//add by art for 一体机 begin ========
HRESULT CMCIM::N_CopyCimCashInfo(LPVOID p_lpSrc)
{
    HRESULT l_hrResult = LFS_SUCCESS;
    LPLFSCIMCASHINFO l_lpSrc = (LPLFSCIMCASHINFO)p_lpSrc;
    LPLFSCIMCASHINFO l_lpDest = m_lpCashInfo;

    if (NULL == p_lpSrc)
    {
        return l_hrResult;
    }

    l_lpDest->cashin_count = l_lpSrc->cashin_count;

    if (l_lpSrc->cashin_count > 0)
    {
        l_hrResult = ::LFMAllocateMore(sizeof(LPLFSCIMCASHIN)*l_lpSrc->cashin_count, l_lpDest, (LPVOID*)&l_lpDest->cash_in);
        if (LFS_SUCCESS != l_hrResult)
        {
            LFMFreeBuffer(l_lpDest);

            return l_hrResult;
        }

        for (int i = 0; i < l_lpSrc->cashin_count; i++)
        {
            l_hrResult = ::LFMAllocateMore(sizeof(LFSCIMCASHIN), l_lpDest, (LPVOID*)&l_lpDest->cash_in[i]);
            if (LFS_SUCCESS != l_hrResult)
            {
                LFMFreeBuffer(l_lpDest);

                return l_hrResult;
            }
            memcpy(l_lpDest->cash_in[i], l_lpSrc->cash_in[i], sizeof(LFSCIMCASHIN));

            l_hrResult = ::LFMAllocateMore(sizeof(LFSCIMNOTENUMBERLIST), l_lpDest, (LPVOID*)&l_lpDest->cash_in[i]->note_number_list);
            if (LFS_SUCCESS != l_hrResult)
            {
                LFMFreeBuffer(l_lpDest);

                return l_hrResult;
            }
            memcpy(l_lpDest->cash_in[i]->note_number_list, l_lpSrc->cash_in[i]->note_number_list, sizeof(LFSCIMNOTENUMBERLIST));

            if (l_lpSrc->cash_in[i]->note_number_list->num_of_note_numbers > 0)
            {
                l_hrResult = ::LFMAllocateMore(sizeof(LPLFSCIMNOTENUMBER)*l_lpSrc->cash_in[i]->note_number_list->num_of_note_numbers, l_lpDest, (LPVOID*)&l_lpDest->cash_in[i]->note_number_list->note_number);
                if (LFS_SUCCESS != l_hrResult)
                {
                    LFMFreeBuffer(l_lpDest);

                    return l_hrResult;
                }

                for (int iNoteList = 0; iNoteList < l_lpSrc->cash_in[i]->note_number_list->num_of_note_numbers; iNoteList++)
                {
                    l_hrResult = ::LFMAllocateMore(sizeof(LFSCIMNOTENUMBER), l_lpDest, (LPVOID*)&l_lpDest->cash_in[i]->note_number_list->note_number[iNoteList]);
                    if (LFS_SUCCESS != l_hrResult)
                    {
                        LFMFreeBuffer(l_lpDest);

                        return l_hrResult;
                    }
                    memcpy(l_lpDest->cash_in[i]->note_number_list->note_number[iNoteList], l_lpSrc->cash_in[i]->note_number_list->note_number[iNoteList], sizeof(LFSCIMNOTENUMBER));

                }

            }

            if (l_lpSrc->cash_in[i]->num_physical_cus > 0)
            {
                l_hrResult = ::LFMAllocateMore(sizeof(LPLFSCIMPHCU)*l_lpSrc->cash_in[i]->num_physical_cus, l_lpDest, (LPVOID*)&l_lpDest->cash_in[i]->physical);
                if (LFS_SUCCESS != l_hrResult)
                {
                    LFMFreeBuffer(l_lpDest);

                    return l_hrResult;
                }

                for (int t = 0; t < l_lpSrc->cash_in[i]->num_physical_cus; t++)
                {
                    l_hrResult = ::LFMAllocateMore(sizeof(LFSCIMPHCU), l_lpDest, (LPVOID*)&l_lpDest->cash_in[i]->physical[t]);
                    if (LFS_SUCCESS != l_hrResult)
                    {
                        LFMFreeBuffer(l_lpDest);

                        return l_hrResult;
                    }
                    memcpy(l_lpDest->cash_in[i]->physical[t], l_lpSrc->cash_in[i]->physical[t], sizeof(LFSCIMPHCU));

                    if (l_lpSrc->cash_in[i]->physical[t]->physical_position_name != NULL && strlen(l_lpSrc->cash_in[i]->physical[t]->physical_position_name) > 0)
                    {
                        int l_iLen = strlen(l_lpSrc->cash_in[i]->physical[t]->physical_position_name) + 1;
                        l_hrResult = ::LFMAllocateMore(l_iLen, l_lpDest, (LPVOID*)&l_lpDest->cash_in[i]->physical[t]->physical_position_name);
                        if (LFS_SUCCESS != l_hrResult)
                        {
                            LFMFreeBuffer(l_lpDest);

                            return l_hrResult;
                        }
                        memcpy(l_lpDest->cash_in[i]->physical[t]->physical_position_name, l_lpSrc->cash_in[i]->physical[t]->physical_position_name, l_iLen);
                    }
                }
            }

        }
    }

    return l_hrResult;
}
//add by art for 一体机 end========


// xiongjie add 20161026, 因为SP有问题，经常会出现获取NoteType为空的异常，暂时写成固定值
void CMCIM::InitialNoteType()
{
    m_mapNoteIdToValue.clear();

    m_mapNoteIdToValue[1]=10;
    m_mapNoteIdToValue[2]=50;
    m_mapNoteIdToValue[3]=100;
    m_mapNoteIdToValue[4]=100;
    m_mapNoteIdToValue[5]=20;
    m_mapNoteIdToValue[6]=10;
    m_mapNoteIdToValue[7]=50;
    m_mapNoteIdToValue[8]=1;
    m_mapNoteIdToValue[9]=5;
}

int CMCIM::Execute(int ID, LPVOID lpData)
{
    LOGINFO("%s，方法。ID=%d", __FUNCTION__, ID);

    int nRet = 0;

    switch (ID)
    {
    case LFS_INF_CIM_STATUS:
    {
        nRet = N_GetStatus();
        if (LFS_SUCCESS == nRet)
        {
            memcpy(lpData, &m_Status, sizeof(LFSCIMSTATUS));
        }
    }
        break;
    case LFS_INF_CIM_CASH_UNIT_INFO:
    {
        nRet = N_CashUnitInfo();
        if (LFS_SUCCESS == nRet && m_lpCashInfo != NULL)
        {
            memcpy(lpData,m_lpCashInfo,sizeof(LFSCIMCASHINFO));
        }

        LFMFreeBuffer(m_lpCashInfo);
    }
        break;
        /////////////约定此ID返回入钞口信息
    case LFS_INF_CIM_TELLER_INFO:
    {
        nRet = N_GetStatus();
        if (LFS_SUCCESS == nRet)
        {
            memcpy(lpData, &m_PosStatus, sizeof(LFSCIMINPOS));
        }
    }
        break;

    default:
        break;
    }

    return nRet;
}

//得到 MCRM.ini 配置文件的路径
void CMCIM::GetConfigPath()
{
    QString strFilePath = g_Helper.GetModuleFileName((void*)&CMCIM::GetInstance);
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

QObject *CreateInstance()
{
    return CMCIM::GetInstance();
}
