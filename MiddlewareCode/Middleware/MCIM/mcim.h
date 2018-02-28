// Last updated at 2015/12/8 18:57:47

#pragma once

#include "dllDSTO.h"
#include "apictrl.h"
#include "easytrace.h"
#include "AlarmLib.h"

#include "mcim_global.h"
#include <webinteractobj.h>
#include <QString>

//add by art begin  
#include <string>
#include <QVector>
#include <QMap>

using std::string;

typedef QMap<string, string>  CUNUM_MAP;
typedef QMap<USHORT,ULONG> NoteIdToValue;//////NoteId对应的面额
typedef QMap<USHORT,ULONG>::iterator NoteItr;
//add by art end
#define HM_MIDDLE_TIMEOUT 60*1000	//60s

////////钞箱编号 和张数
typedef struct CIMSetCount
{
    QString                           strUnitNum; /////钞箱编号如1A 1B
    ULONG                             ulCount;
}CIMSETCOUNT,*LPCIMSETCOUNT;

//////钞箱编号 和类型
typedef struct CIMSetType
{
    QString                           strUnitNum; /////钞箱编号如1A 1B
    DWORD                             fwType;
}CIMSETTYPE,*LPCIMSETTYPE;

/////////钞箱编号和面额
typedef struct CIMSetValues
{
    QString                           strUnitNum; /////钞箱编号如1A 1B
    ULONG                             ulValues;
}CIMSETVALUES,*LPCIMSETVALUES;

typedef struct CIMValueCount
{
    ULONG                            ulValues;
    ULONG                            ulCount;
}CIMVALUECOUNT,*LPCIMVALUECOUNT;
//add by art for 一体机 end


class MCIMSHARED_EXPORT CMCIM : public CWebInteractObj
{
    Q_OBJECT

    Q_PROPERTY(QString ServiceName WRITE SetServiceName)
    Q_PROPERTY(QString StDetailedDeviceStatus READ GetStDetailedDeviceStatus)
    Q_PROPERTY(QString StEscrowStatus READ GetStEscrowStatus)
    Q_PROPERTY(QString StShutterStatus READ GetStShutterStatus)
    Q_PROPERTY(QString StTransportStatus READ GetStTransportStatus)
    Q_PROPERTY(QString StInputStatus READ GetStInputStatus)
    Q_PROPERTY(QString StAcceptorStatus READ GetStAcceptorStatus)
    Q_PROPERTY(QString StSafeDoorStatus READ GetStSafeDoorStatus)
    Q_PROPERTY(BOOL CpCanEscrow READ GetCpCanEscrow)
    Q_PROPERTY(BOOL CpShutterControlSupported READ GetCpShutterControlSupported)
    Q_PROPERTY(LONG CpMaxAcceptItems READ GetCpMaxAcceptItems)
    Q_PROPERTY(BOOL CpCanDetectCashInserted READ GetCpCanDetectCashInserted)
    Q_PROPERTY(BOOL CpCanDetectCashTaken READ GetCpCanDetectCashTaken)
    Q_PROPERTY(QString CpRetractAreas READ GetCpRetractAreas)
    Q_PROPERTY(QString CpExchangeTypes READ GetCpExchangeTypes)
    Q_PROPERTY(QString LastAcceptStatus READ GetLastAcceptStatus)
    Q_PROPERTY(LONG NumOfRefused READ GetNumOfRefused)
    Q_PROPERTY(QString CUStatus READ GetCUStatus)

public:
    CMCIM(const QString& strDBusObjName);
    ~CMCIM();

    static CMCIM* GetInstance(void);

    /************************************************************************/
    /*XFS消息和事件处理                                                     */
    /************************************************************************/
    //virtual void  AfterOpenDevice(HRESULT hResult, LPVOID lpBuffer);
    virtual void  WR_OpenComplete(DWORD dwCommand, HRESULT hResult, LPVOID lpBuffer);
    virtual void  WR_RegisterComplete(DWORD dwCommand, HRESULT hResult, LPVOID lpBuffer);
    virtual void  WR_CloseComplete(DWORD dwCommand, HRESULT hResult, LPVOID lpBuffer);
    virtual void  WR_GetinfoComplete(DWORD dwCommand, HRESULT hResult, LPVOID lpBuffer);
    virtual void  WR_ExecuteComplete(DWORD dwCommand, HRESULT hResult, LPVOID lpBuffer);
    virtual void  WR_ExecuteEvent(DWORD dwEventID, HRESULT hResult, LPVOID lpBuffer);
    virtual void  WR_UserEvent(DWORD dwEventID, HRESULT hResult, LPVOID lpBuffer);
    virtual void  WR_ServiceEvent(DWORD dwEventID, HRESULT hResult, LPVOID lpBuffer);
    virtual void  WR_SystemEvent(DWORD dwEventID, HRESULT hResult, LPVOID lpBuffer);

    // 属性
    virtual void SetServiceName(QString newVal);
    virtual QString GetStDetailedDeviceStatus();
    virtual QString GetStEscrowStatus();
    virtual QString GetStShutterStatus();
    virtual QString GetStTransportStatus();
    virtual QString GetStInputStatus();
    virtual QString GetStAcceptorStatus();
    virtual QString GetStSafeDoorStatus();
    virtual BOOL GetCpCanEscrow();
    virtual BOOL GetCpShutterControlSupported();
    virtual LONG GetCpMaxAcceptItems();
    virtual BOOL GetCpCanDetectCashInserted();
    virtual BOOL GetCpCanDetectCashTaken();
    virtual QString GetCpRetractAreas();
    virtual QString GetCpExchangeTypes();
    virtual QString GetLastAcceptStatus();
    virtual LONG GetNumOfRefused();
    virtual QString GetCUStatus();

signals:
    //信号函数，类似之前的事件
    void CashAccepted(QString TotalItems, BOOL PartRefused);
    void CashRefused(void);
    void CashAcceptCancelled(void);
    void CashAcceptCancelFailed(void);
    void CashInserted(void);
    void EscrowedCashStored(QString Unit, BOOL PartRetained);
    void EscrowedCashStoreFailed(void);
    void EscrowedCashEjected(QString CashInfo);
    void EscrowedCashEjectFailed(void);
    void RefusedCashEjected(void);
    void CashTaken(void);
    void ShutterOpened(void);
    void ShutterOpenFailed(void);
    void ShutterClosed(void);
    void ShutterCloseFailed(void);
    void StartExchangeCompleted(void);
    void StartExchangeFailed(void);
    void EndExchangeCompleted(void);
    void EndExchangeFailed(void);
    void AcceptCashPrepared(void);
    void AcceptCashAlreadyActive(void);
    void AcceptCashPrepareFailed(void);
    void ResetComplete(void);
    void ResetFailed(void);
    void SafeDoorOpened(void);
    void SafeDoorClosed(void);
    void SetSupportValueCompleted(void);
    void SetSupportValueFailed(void);



public slots:
    // 方法
    virtual SHORT CloseConnection(void);
    virtual SHORT OpenConnection(void);
    SHORT AcceptCash(LONG Timeout);
    SHORT CancelAcceptCash(void);
    SHORT StoreEscrowedCash(LONG Timeout);
    SHORT EjectEscrowedCash(LONG Timeout);
    SHORT OpenShutter(LONG Timeout);
    SHORT CloseShutter(LONG Timeout);
    SHORT Reset(QString RetractArea, SHORT ID,LONG Timeout);
    LONG GetLastAcceptedAmountSync(QString Currency, QString Validity);
    void StartExchange(void);
    void EndExchange(void);
    SHORT SetUnitCountSync(QString Unit);
    SHORT SetUnitValueSync(QString Value);
    SHORT SetUnitTypeSync(QString Type);
    SHORT SetSupportValue(QString Value);
    LONG PrepareForAcceptCash(void);
    SHORT StatisticsData(void);
    int Execute(int ID, LPVOID lpData);
private:

    void GetConfigPath(void);//得到 MCRM.ini 配置文件的路径


    //////执行open命令
    int N_Open();
    ////执行close命令
    int N_Close();

    //执行WFS_INF_CIM_STATUS
    int N_GetStatus();
    ////初始化状态
    void N_InitStatus();
    //执行WFS_INF_CIM_CAPABILITIES
    int N_GetCaps();
    /////WFS_INF_CIM_CASH_IN_STATUS
    int N_CashInStatus();
    /////WFS_INF_CIM_CASH_UNIT_INFO
    int N_CashUnitInfo();
    // 获取钞箱状态（异步）
    int N_CashUnitInfoAsync();
    //执行WFS_CMD_CIM_CASH_IN_START
    int N_CashInStart(LPLFSCIMCASHINSTART lpCashInStart);
    //执行WFS_CMD_CIM_CASH_IN
    int N_CashIn(LONG timeout=0);
    ////执行WFS_CMD_CIM_CASH_IN_END
    int N_CashInEnd(LONG Timeout);
    ////////执行WFS_CMD_CIM_OPEN_SHUTTER
    int N_OpenShutter(LPWORD lpfwPosition,LONG Timeout);
    /////执行WFS_CMD_CIM_CLOSE_SHUTTER
    int N_CloseShutter(LPWORD lpfwPosition,LONG Timeout);
    //////执行WFS_CMD_CIM_START_EXCHANGE
    int N_StartExchage(LPLFSCIMSTARTEX lpStartEx);
    //////执行WFS_CMD_CIM_END_EXCHANGE
    int N_EndExchage(void);
    ///////执行WFS_CMD_CIM_CASH_IN_ROLLBACK
    int N_CashInRollBack(LONG timeout=0);
    ///执行WFS_CMD_CIM_RESET
    int N_Reset(LPLFSCIMITEMPOSITION lpResetIn,LONG Timeout);
    ////取消当前命令
    int N_Cancel(REQUESTID requestID);
    ///计算纸币存入的张数
    LONG N_TotalItems(LPLFSCIMNOTENUMBERLIST lpBuf);
    /////保存设备状态
    int N_Status(LPLFSCIMSTATUS lpStatus);
    /////保存能力
    int N_Capabilities(LPLFSCIMCAPS lpCaps);
    /////保存钞箱信息
    int N_GetCashUnitInfo(LPLFSCIMCASHINFO lpCashInfo);
    //////保存Cashin状态
    int N_GetCashInStatus(LPLFSCIMCASHINSTATUS lpCashInStatus);
    //add by art for 一体机 begin
    ////执行WFS_INF_CIM_BANKNOTE_TYPES
    int N_GetBankNoteTypes(void);
    ////拷贝SP返回NoteType
    //add by art for 一体机 end
    int N_CopyNoteType(LPLFSCIMNOTETYPELIST lpNoteTypeList);
    /////将调用WFS_CMD_CIM_CASH_IN的输出值赋给m_NoteNumberList
    //int N_Strcpy(LPWFSCIMNOTENUMBERLIST lpBuf);
    /////xfs设备状态与kal设备状态转换
    QString N_XFSDevStatusToKalDevStatus(DWORD dwStatus);
    //xfs钞箱状态与kal钞箱状态转换
    QString N_XFSCUStatusToKalCUStatus(USHORT usStatus);
    // 获取可存金额
    void GetRemainDepositAmount(void);

    SHORT N_StatisticsData();
    // 初始化NoteType
    void InitialNoteType();

    int N_ConfigureNoteType(LPUSHORT lpusNoteIDs);

    QString m_strConfigPath;//配置文件的路径
    CDllDSTO m_Dsto;  ///数据类
    USHORT m_usCount;////钞箱数
    USHORT m_usTellerID;
    char m_szLogicalName[MAX_PATH];
    bool m_bOpened;
    BOOL m_bPartRefused;  //是否有拒钞
    BOOL m_CashInABFalg; ////存款是否有入回收箱
    QString m_CashInUnitCount;////存款进入各钞箱张数
    REQUESTID m_CurrRequestID;
    REQUESTID m_CashInID;
    LFSCIMSTATUS m_Status;
    QString m_CurDevStatus;
    QString m_PreDevStatus;
    LFSCIMCASHINSTATUS m_CashInStatus;
    LFSCIMCAPS m_Caps;
    QVector<LFSCIMNOTENUMBER> m_aTotalNoteNumber;
    QVector<CIMSETCOUNT> m_aSetCount;

    QVector<CIMSETTYPE> m_aSetType;
    QVector<CIMSETVALUES> m_aSetValue;
    LPLFSCIMCASHINFO m_lpCashInfo;/////钞箱信息

    //xuzhe start 0419 add
    LFSCIMINPOS m_PosStatus;//入钞口的状态
    //xuzhe end 0419 add

    HRESULT N_CopyCimCashInfo(LPVOID p_lpSrc); //add by art

    CUNUM_MAP m_mapUno; //钞箱UnitID对应的钞箱序号 //add by art
    NoteIdToValue m_mapNoteIdToValue;
    QString m_csValueCount;			// CashIn Start后累计的入钞详情
    static CMCIM* m_pInstance;
};

extern "C" {
MCIMSHARED_EXPORT QObject* CreateInstance(void);
}
