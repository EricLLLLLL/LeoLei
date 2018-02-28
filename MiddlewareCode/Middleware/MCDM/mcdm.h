// Last updated at 2015/12/15 10:11:30

#ifndef MCDM_H
#define MCDM_H

#include "AlarmLib.h"

#include <QString>
#include <dllDSTO.h>
#include <easytrace.h>
#include "apictrl.h"
#include "mcdm_global.h"
#include "webinteractobj.h"

//add by art begin
#include <map>      
#include <string>

#include <QVector>
#include <QMap>

using std::map;
using std::string;
//add by art end

#define HM_MIDDLE_TIMEOUT 60*1000	//60s


typedef QMap<string, string>  CUNUM_MAP;

////////钞箱编号 和张数
typedef struct CDMSetCount
{
    QString                           strUnitNum; /////钞箱编号如1A 1B
    ULONG                             ulCount;
}CDMSETCOUNT,*LPCDMSETCOUNT;

//////钞箱编号 和类型
typedef struct CDMSetType
{
    QString                           strUnitNum; /////钞箱编号如1A 1B
    DWORD                             fwType;
}CDMSETTYPE,*LPCDMSETTYPE;

/////////钞箱编号和面额
typedef struct CDMSetValues
{
    QString                           strUnitNum; /////钞箱编号如1A 1B
    ULONG                             ulValues;
}CDMSETVALUES,*LPCDMSETVALUES;

class MCDMSHARED_EXPORT CMCDM : public CWebInteractObj
{
    Q_OBJECT

    Q_PROPERTY(QString ServiceName WRITE SetServiceName)
    Q_PROPERTY(QString StDetailedDeviceStatus READ GetStDetailedDeviceStatus)
    Q_PROPERTY(SHORT NumLogicalCashUnits READ GetNumLogicalCashUnits)
    Q_PROPERTY(QString CUNumber READ GetCUNumber)
    Q_PROPERTY(QString CUType READ GetCUType)
    Q_PROPERTY(QString CUId READ GetCUId)
    Q_PROPERTY(QString CUStatus READ GetCUStatus)
    Q_PROPERTY(QString CUCurrentCount READ GetCUCurrentCount)
    Q_PROPERTY(QString CUCurrency READ GetCUCurrency)
    Q_PROPERTY(QString CUNoteValue READ GetCUNoteValue)
    Q_PROPERTY(QString CUInitialCount READ GetCUInitialCount)
    Q_PROPERTY(QString CUMaxThreshold READ GetCUMaxThreshold)
    Q_PROPERTY(QString CUMinThreshold READ GetCUMinThreshold)
    Q_PROPERTY(QString PUId READ GetPUId)
    Q_PROPERTY(QString PUCUNumber READ GetPUCUNumber)
    Q_PROPERTY(QString PUCurrentCount READ GetPUCurrentCount)
    Q_PROPERTY(QString PUCUId READ GetPUCUId)
    Q_PROPERTY(QString PUStatus READ GetPUStatus)
    Q_PROPERTY(QString PUInitialCount READ GetPUInitialCount)
    Q_PROPERTY(QString PURejectCount READ GetPURejectCount)
    Q_PROPERTY(BOOL CpCanRetract READ GetCpCanRetract)
    Q_PROPERTY(QString StSafeDoorStatus READ GetStSafeDoorStatus)
    Q_PROPERTY(QString StDispenserStatus READ GetStDispenserStatus)
    Q_PROPERTY(QString MixNumber READ GetMixNumber)
    Q_PROPERTY(QString StStackerStatus READ GetStStackerStatus)
    Q_PROPERTY(QString StTransportStatus READ GetStTransportStatus)
    Q_PROPERTY(QString StOutputStatus READ GetStOutputStatus)
    Q_PROPERTY(QString StShutterStatus READ GetStShutterStatus)
    Q_PROPERTY(BOOL CpCanDetectCashTaken READ GetCpCanDetectCashTaken)
    Q_PROPERTY(SHORT RemainCount READ GetRemainCount)

public:
    CMCDM(const QString& strDBusObjName);
    ~CMCDM();

    static CMCDM* GetInstance(void);
    /************************************************************************/
    /*XFS消息和事件处理                                                     */
    /************************************************************************/
    // virtual void  AfterOpenDevice(HRESULT hResult, LPVOID lpBuffer);
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
    virtual SHORT GetNumLogicalCashUnits();
    virtual QString GetCUNumber();
    virtual QString GetCUType();
    virtual QString GetCUId();
    virtual QString GetCUStatus();
    virtual QString GetCUCurrentCount();
    virtual QString GetCUCurrency();
    virtual QString GetCUNoteValue();
    virtual QString GetCUInitialCount();
    virtual QString GetCUMaxThreshold();
    virtual QString GetCUMinThreshold();
    virtual QString GetPUId();
    virtual QString GetPUCUNumber();
    virtual QString GetPUCurrentCount();
    virtual QString GetPUCUId();
    virtual QString GetPUStatus();

    virtual QString GetPUInitialCount();
    virtual QString GetPURejectCount();
    virtual BOOL GetCpCanRetract();
    virtual QString GetStSafeDoorStatus();
    virtual QString GetStDispenserStatus();
    virtual QString GetMixNumber();
    virtual QString GetStStackerStatus();
    virtual QString GetStTransportStatus();
    virtual QString GetStOutputStatus();
    virtual QString GetStShutterStatus();
    virtual BOOL GetCpCanDetectCashTaken();
    virtual SHORT GetRemainCount();

signals:
    //信号函数，类似之前的事件
    void CashTaken(void);
    void MixComplete(QString Mix);
    void MixFailed(void);
    void CashPresented(void);
    void ResetComplete(void);
    void ResetFailed(void);
    void NotDispensable(void);
    void CashDispensed(QString CashUnit, LONG Amount);
    void CashDispenseFailed(QString CashUnit, LONG Amount);
    void CashUnitInfoChanged(SHORT CashUnitNumber);
    void DispenseAmountIncorrect(void);
    void OutPositionNotEmpty(void);
    void SubDispenseOk(QString CashUnit, LONG Amount);
    void StartExchangeCompleted(void);
    void StartExchangeFailed(void);
    void EndExchangeCompleted(void);
    void EndExchangeFailed(void);
    void ShutterOpened(void);
    void ShutterOpenFailed(void);
    void ShutterClosed(void);
    void ShutterCloseFailed(void);


public slots:
    //方法，JS可直接调用
    // 方法
    virtual SHORT CloseConnection(void);
    virtual SHORT OpenConnection(void);
    void Mix(LONG Amount, QString Currency, QString MixAlgorithm);
    void MixAndDispense(LONG Amount, QString Currency, QString MixAlgorithm);
    void Dispense(LONG Amount, QString NoteCounts, QString Currency, QString MixAlgorithm);
    void Present(LONG Timeout);
    void StartExchange(void);
    void EndExchange(void);
    SHORT Reset(QString RetractArea, SHORT ID,LONG Timeout);
    int Execute(int ID, LPVOID lpData);
    SHORT SetUnitTypeSync(QString Type);//新增 设置钞箱类型
    SHORT SetUnitCountSync(QString Unit);//新增 设置钞箱钞票张数
    SHORT SetUnitValueSync(QString Value);//新增 设置钞箱面额
    void  SetDispenseAmount(LONG Amount);
    SHORT OpenShutter(LONG Timeout);
    SHORT CloseShutter(LONG Timeout);

private:

    void GetConfigPath(void);//得到 MCRM.ini 配置文件的路径

    //////执行open命令
    int N_Open();
    ////执行close命令
    int N_Close();

    //执行LFS_INF_CDM_STATUS
    int N_GetStatus();
    ////初始化状态
    void N_InitStatus();
    //执行LFS_INF_CDM_CAPABILITIES
    int N_GetCaps();
    /////LFS_INF_CDM_CASH_UNIT_INFO
    int N_CashUnitInfo();
    // 获取钞箱信息（异步）
    int N_CashUnitInfoAsync();
    //////LFS_CMD_CDM_DENOMINATE
    int N_Denominate(LPLFSCDMDENOMINATE lpDenominate);
    //////LFS_CMD_CDM_DISPENSE
    int N_Dispense(LPLFSCDMDISPENSE lpDispense);
    ////////LFS_CMD_CDM_REJECT
    int N_Reject();
    //////////LFS_CMD_CDM_RETRACT
    int N_Retract(LPLFSCDMRETRACT lpRetract);
    ////取消Present or DispenseAndPresent命令
    int N_Cancel(REQUESTID requestID);
    ///执行LFS_CMD_CDM_RESET
    int N_Reset(LPLFSCDMITEMPOSITION lpResetIn,LONG Timeout);
    //////LFS_CMD_CDM_START_EXCHANGE
    int N_StartExchange(LPLFSCDMSTARTEX lpStartEx);
    //////LFS_CMD_CDM_END_EXCHANGE
    int N_EndExchange(void);
    /////
    int N_Status(LPLFSCDMSTATUS lpStatus);
    /////
    int N_Capabilities(LPLFSCDMCAPS lpCaps);
    /////保存钞箱信息
    int N_GetCashUnitInfo(LPLFSCDMCUINFO lpCashUnitInfo);
    ////出钞出错返回各面额出钞张数
    QString N_DispenseCashUnit(LPLFSCDMDENOMINATION lpDenomination);
    /////xfs设备状态与kal设备状态转换
    QString N_XFSDevStatusToKalDevStatus(DWORD dwStatus);
    //////xfs钞箱类型与kal钞箱类型转换
    QString N_XFSCUTypeToKalCUType(USHORT usType);
    ///////xfs钞箱状态与kal钞箱状态转换
    QString N_XFSCUStatusToKalCUStatus(USHORT usStatus);
    // 获取可取金额
    void GetPossibleDispenseAmount(void);
    //////分配内存并保存
    HRESULT N_CopyCdmCashInfo(LPVOID p_lpSrc); //add by zyf

    ////////执行LFS_CMD_CDM_OPEN_SHUTTER
    int N_OpenShutter(LPWORD lpfwPosition,LONG Timeout);
    /////执行LFS_CMD_CDM_CLOSE_SHUTTER
    int N_CloseShutter(LPWORD lpfwPosition,LONG Timeout);

    QString m_strConfigPath;//配置文件的路径
    CDllDSTO m_Dsto;  ///数据类
    USHORT m_usCount;////钞箱数
    USHORT m_usTellerID;
    ULONG m_uValues[10];//钞箱出钞数 //tmp by art: old 5?
    char m_szLogicalName[MAX_PATH];
    bool m_bOpened;
    LFSCDMSTATUS m_Status;
    QString m_CurrDevStatus;
    LFSCDMCAPS m_Caps;
    REQUESTID m_CurrRequestID;
    REQUESTID m_WaitForCashTakenID;
    QString m_CurDevStatus;
    QString m_PreDevStatus;

    LPLFSCDMCUINFO m_lpCashUnitInfo;/////钞箱信息
    QVector<CDMSETCOUNT> m_aSetCount;
    QVector<CDMSETTYPE> m_aSetType;
    QVector<CDMSETVALUES> m_aSetValue;

    //add by zyf begin
    CUNUM_MAP m_mapUno; //钞箱UnitID对应的钞箱序号
    //add by zyf end


    //xuzhe start 0419 add
    LFSCDMOUTPOS m_PosStatus;//出钞口的状态
    //xuzhe end 0419 add

    LONG m_nDispenseTotalAmount;	// 本笔交易预出钞总金额
    LONG m_nDispenseAmount;			// 本次Dispense出钞金额

    static CMCDM *m_pInstance;
};


extern "C" {
MCDMSHARED_EXPORT QObject* CreateInstance(void);
}

#endif
