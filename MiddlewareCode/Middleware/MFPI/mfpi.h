// Last updated at 2015/12/8 8:44:03

#pragma once


#include <vector>
#include "dllDSTO.h"
#include "apictrl.h"
#include "easytrace.h"
#include "AlarmLib.h"

#include "Helper/helper.h"
#include "mfpi_global.h"
#include "webinteractobj.h"
#include <QString>
#include <QObject>

#define HM_MIDDLE_TIMEOUT 60*1000	//60s

class MFPISHARED_EXPORT CMFPI : public CWebInteractObj
{
    Q_OBJECT

    Q_PROPERTY(QString ServiceName WRITE SetServiceName)
    Q_PROPERTY(QString StDetailedDeviceStatus READ GetStDetailedDeviceStatus)
    Q_PROPERTY(QString StMediaStatus READ GetStMediaStatus)

public:
    CMFPI(const QString& strDBusObjName);
    ~CMFPI();

    static CMFPI* GetInstance(void);

    /************************************************************************/
    /*XFS消息和事件处理                                                     */
    /************************************************************************/
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
    virtual QString GetStDetailedDeviceStatus();
    virtual QString GetStMediaStatus();
    virtual void SetServiceName(QString newVal);

signals:
    //信号函数，类似之前的事件
    void IdentifyComplete(QString Data);
    void IdentifyFailed(void);
    void IdentifyCancelled(void);
    void FingerMoved(void);
    void DataAcquired(QString Data);
    void DataAcquireFailed(void);
    void AcquireDataCancelled(void);
    void ResetComplete(void);
    void ResetFailed(void);
    void MatchComplete(void);
    void MatchFailed(void);

public slots:
    //方法，JS可直接调用// 方法
    SHORT CloseConnection(void);
    SHORT OpenConnection(void);
    void Reset(LONG Timeout);
    void Identify(LONG Timeout);
    void CancelIdentify(void);
    void AcquireData(LONG Timeout);
    void CancelAcquireData(void);
    void DataMatch(LONG Timeout, QString Data);
    int Execute(int ID, LPVOID lpData);


private:
    //根据错误码发送相应的事件
    //void ErrorSend(int err);
    int N_Open();
    int N_Close();

    //取消当前正在执行的命令
    int N_Cancel(WORD requestID);

    //执行WFS_INF_IDC_STATUS
    int N_GetStatus();
    /////初始化状态
    void N_InitStatus();
    //执行WFS_CMD_IDC_Get_RAW_DATA
    int N_ReadRawData(WORD readData,LONG timeout=HM_MIDDLE_TIMEOUT);

    //执行WFS_CMD_IDC_RESET
    int N_Reset(WORD wResetIn,LONG timeout=HM_MIDDLE_TIMEOUT);

    //执行WFS_CMD_IDC_CHIP_IO
    int N_ChipIO(LPLFSIDCCHIPIO lpChipIoIn,LONG timeout=HM_MIDDLE_TIMEOUT);

    int N_AfterReadRawData(HRESULT hResult, LPVOID lpBuffer);

    int N_AfterChipIO(HRESULT hResult, LPVOID lpBuffer);

    void SetDeviceStatus(WORD wStatus);
    void GetConfigPath();//得到配置文件的路径


private:
    QString m_PreDevStatus,m_CurrDevStatus;//设备前一时刻的状态和当前设备的状态
    LFSIDCSTATUS m_Status;//设备状态
    REQUESTID m_CurrRequestID;
    WORD m_Tracks;
    QString m_FingerData;
    BOOL m_bIdentify;//是否匹配成功
    WORD m_hResult;//命令执行的结果

    std::vector<QString> m_TmpDatas;

    QString m_strConfigPath;//配置文件的路径
    CDllDSTO m_DSTO;
    int m_Fpiprovider;////指纹厂商
private:
    char m_szLogicalName[MAX_PATH];
    bool m_bOpened;
    CHelper help;
    static CMFPI* m_pInstance;
};

extern "C" {
MFPISHARED_EXPORT QObject* CreateInstance(void);
}
