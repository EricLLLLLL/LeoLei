#ifndef MIDR_H
#define MIDR_H

#include "midr_global.h"
#include "webinteractobj.h"
#include "win_type_def.h"
#include "easytrace.h"
#include "apictrl.h"
#include "Helper/helper.h"
#include <QString>
#include <map>

#define HM_MIDDLE_TIMEOUT 60*1000	//60s

class MIDRSHARED_EXPORT CMIDR : public CWebInteractObj
{
    Q_OBJECT

    Q_PROPERTY(QString ServiceName WRITE SetServiceName)
    Q_PROPERTY(QString StDetailedDeviceStatus READ GetStDetailedDeviceStatus)
    Q_PROPERTY(QString StMediaStatus READ GetStMediaStatus)
    Q_PROPERTY(BOOL CpCanEject READ GetCpCanEject)
    Q_PROPERTY(BOOL CpCanCapture READ GetCpCanCapture)
    Q_PROPERTY(QString Name READ GetName)
    Q_PROPERTY(QString Sex READ GetSex)
    Q_PROPERTY(QString Nation READ GetNation)
    Q_PROPERTY(QString Birthday READ GetBirthday)
    Q_PROPERTY(QString Address READ GetAddress)
    Q_PROPERTY(QString Number READ GetNumber)
    Q_PROPERTY(QString Department READ GetDepartment)
    Q_PROPERTY(QString StartTime READ GetStartTime)
    Q_PROPERTY(QString EndTime READ GetEndTime)
    Q_PROPERTY(QString PortraitFilePath READ GetPortraitFilePath)
    Q_PROPERTY(QString FrontFilePath READ GetFrontFilePath WRITE SetFrontFilePath)
    Q_PROPERTY(QString BackFilePath READ GetBackFilePath WRITE SetBackFilePath)

public:
    CMIDR(const QString& strDBusObjName);
    ~CMIDR();

    static CMIDR* GetInstance(void);

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

    QString GetStMediaStatus(void);
    BOOL GetCpCanEject(void);
    BOOL GetCpCanCapture(void);
    QString GetName(void);
    QString GetSex(void);
    QString GetNation(void);
    QString GetBirthday(void);
    QString GetAddress(void);
    QString GetNumber(void);
    QString GetDepartment(void);
    QString GetStartTime(void);
    QString GetEndTime(void);
    QString GetPortraitFilePath(void);
    QString GetFrontFilePath(void);
    void SetFrontFilePath(QString newVal);
    QString GetBackFilePath(void);
    void SetBackFilePath(QString newVal);

signals:
    void CardAccepted(QString Info);
    void CardInserted(void);
    void CardInvalid(void);
    void CardAcceptFailed(void);
    void CardEjected(void);
    void CardEjectFailed(void);
    void CardTaken(void);
    void CardCaptured(void);
    void CardCaptureFailed(void);
    void CardAcceptCanceled(void);
    void CancelAcceptCardFailed(void);
    void CardWaitCanceled(void);
    void CancelWaitFailed(void);
    void ResetComplete(void);
    void ResetFailed(void);

public slots:
    SHORT CloseConnection(void);
    SHORT OpenConnection(void);

    SHORT Reset(QString ResetAction);
    SHORT AcceptAndRead(LONG Timeout);
    SHORT Eject(LONG Timeout);
    SHORT Capture(LONG Timeout);
    SHORT CancelAccept(void);
    SHORT CancelWait(void);
    int Execute(int ID, LPVOID lpData);

private:
    int N_Open();
    int N_Close();

    //取消当前正在执行的命令
    int N_Cancel(WORD requestID);

    //执行LFS_INF_IDC_STATUS
    int N_GetStatus();
    /////初始化状态
    void N_InitStatus();
    //执行LFS_INF_IDC_CAPABILITIES
    int N_GetCaps();
    /////初始化能力
    void N_InitCaps();
    //执行LFS_CMD_IDC_EJECT_CARD
    int N_EjectCard(LONG timeout=HM_MIDDLE_TIMEOUT);

    //执行LFS_CMD_IDC_RETAIN_CARD
    int N_RetainCard(LONG timeout=HM_MIDDLE_TIMEOUT);

    //执行LFS_CMD_IDC_RESET
    int N_Reset(WORD wResetIn,LONG timeout=HM_MIDDLE_TIMEOUT);

    //执行LFS_CMD_IDC_Get_RAW_DATA
    int N_ReadRawData(WORD readData,LONG timeout=HM_MIDDLE_TIMEOUT);


    void SetDeviceStatus(WORD wStatus);

    void ProcessCardData(QString strInfo);


    typedef std::vector<QString> VerStr;

    //得到以sep分割的所有子串
    VerStr Token_All(const QString str,const char sep);

    void InitMap();

    void CopyPicture();

    void GetConfigPath();

private:
    LFSIDCSTATUS m_Status;//设备状态
    LFSIDCCAPS m_Caps;//设备能力
    QString m_PreDevStatus,m_CurrDevStatus;//设备前一时刻的状态和当前设备的状态
    QString m_Name,m_Sex,m_Nation,m_BirthDay,m_Number,m_Address,m_Department,m_StartTime,m_Endtime;
    QString m_InfoAll;//身份证所有信息，用逗号分隔
    QString m_PortraitFilePath,m_BackFilePath,m_FrontFilePath;

    REQUESTID m_CurrRequestID;
    REQUESTID m_AcceptRequestID,m_EjectRequestID;


    std::map<QString,QString> m_NationMap;

    QString m_strConfigPath;
    int m_iNameFormatType;
    QString m_strPicturePath;

private:
    char m_szLogicalName[MAX_PATH];
    //CApiCtrl m_apiCtrl;
    bool m_bOpened;

    static CMIDR* m_pInstance;

};

extern "C" {
MIDRSHARED_EXPORT QObject* CreateInstance(void);
}

#endif // MIDR_H
