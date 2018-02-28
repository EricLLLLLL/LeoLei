#ifndef MSIU_H
#define MSIU_H

#include "msiu_global.h"
#include "webinteractobj.h"
#include "win_type_def.h"
#include "easytrace.h"
#include <QString>

class MSIUSHARED_EXPORT CMSIU : public CWebInteractObj
{
    Q_OBJECT

    // 属性
    Q_PROPERTY(QString ServiceName WRITE SetServiceName)
    Q_PROPERTY(QString StDetailedDeviceStatus READ GetStDetailedDeviceStatus)
    Q_PROPERTY(QString StCardReaderLightStatus READ GetStCardReaderLightStatus)
    Q_PROPERTY(QString StPinPadLightStatus READ GetStPinPadLightStatus)
    Q_PROPERTY(QString StReceiptPrinterLightStatus READ GetStReceiptPrinterLightStatus)
    Q_PROPERTY(QString StScannerLightStatus READ GetStScannerLightStatus)

public:
    CMSIU(const QString& strDBusObjName);
    ~CMSIU();

    static CMSIU* GetInstance(void);

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
    void SetServiceName(QString newVal);
    QString GetStDetailedDeviceStatus(void);
    QString GetStCardReaderLightStatus(void);
    QString GetStPinPadLightStatus(void);
    QString GetStReceiptPrinterLightStatus(void);
    QString GetStScannerLightStatus(void);

signals:
    void ResetComplete(void);
    void ResetFailed(void);

public slots:
    SHORT CloseConnection(void);
    SHORT OpenConnection(void);
    SHORT SetCardReaderLight(QString FlashRate);
    SHORT SetPinPadLight(QString FlashRate);
    SHORT SetReceiptPrinterLight(QString FlashRate);
    SHORT SetCustomLight(QString CustomLightName, QString FlashRate);
    QString GetCustomLightStatusSync(QString CustomLightName);
    SHORT SetScannerLight(QString FlashRate);
    void Reset(LONG Timeout);
    int Execute(int ID, LPVOID lpData);

private:
    HRESULT N_Open();
    HRESULT N_Close();

    //执行LFS_INF_SIU_STATUS
    HRESULT N_GetStatus();

    //执行LFS_INF_SIU_CAPABILITIES
    HRESULT N_GetCaps();

    void N_SetDeviceStatus(WORD wStatus);
    //设置指示灯状态
    void N_SetLigntStatus(WORD *GuidLights);
    QString N_GetLigntStatus(WORD flags);

private:
    static CMSIU* m_pInstance;

    char m_szLogicalName[MAX_PATH];
    //CApiCtrl m_apiCtrl;
    bool m_bOpened;
    QString m_PreDevStatus,m_DetailedDeviceStatus; //设备前一时刻的状态和当前设备的详细状态
    QString m_DeviceStatus; //设备状态
    LFSSIUSTATUS m_Status;  //设备状态
    LFSSIUCAPS m_Caps;  //设备能力

    QString StCardReaderLightStatus;
    QString StPinPadLightStatus;
    QString StNoteDispenserLightStatus;
    QString StCoinDispenserLightStatus;
    QString StReceiptPrinterLightStatus;
    QString StPassbookPrinterLightStatus;
    QString StEnvelopeDepositoryLightStatus;
    QString StEnvelopeDispenserLightStatus;
    QString StChequeUnitLightStatus;
    QString StBillAcceptorLightStatus;
    QString StCoinAcceptorLightStatus;
    QString StDocumentPrinterLightStatus;
    QString StScannerLightStatus;

    bool isEnterPowerSave;
};

extern "C" {
MSIUSHARED_EXPORT QObject* CreateInstance(void);
}

#endif // MSIU_H
