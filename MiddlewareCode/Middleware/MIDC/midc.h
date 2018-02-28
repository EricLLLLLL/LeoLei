#ifndef MIDC_H
#define MIDC_H

#include "midc_global.h"
#include <webinteractobj.h>
#include <QString>
#include <dllDSTO.h>
#include <easytrace.h>
#include <icdpc.h>

#define HM_MIDDLE_TIMEOUT 60*1000	//60s

class MIDCSHARED_EXPORT CMIDC:public CWebInteractObj
{
    Q_OBJECT

    Q_PROPERTY(QString ServiceName WRITE SetServiceName)
    Q_PROPERTY(QString StDetailedDeviceStatus READ GetStDetailedDeviceStatus)
    Q_PROPERTY(SHORT CpBinSize READ GetCpBinSize)
    Q_PROPERTY(BOOL CpCanCapture READ GetCpCanCapture)
    Q_PROPERTY(BOOL CpCanEject READ GetCpCanEject)
    Q_PROPERTY(BOOL CpCanReadTrack1 READ GetCpCanReadTrack1)
    Q_PROPERTY(BOOL CpCanReadTrack2 READ GetCpCanReadTrack2)
    Q_PROPERTY(BOOL CpCanReadTrack3 READ GetCpCanReadTrack3)
    Q_PROPERTY(BOOL CpCanReadTrackJIS2 READ GetCpCanReadTrackJIS2)
    Q_PROPERTY(BOOL CpCanWriteTrack1 READ GetCpCanWriteTrack1)
    Q_PROPERTY(BOOL CpCanWriteTrack2 READ GetCpCanWriteTrack2)
    Q_PROPERTY(BOOL CpCanWriteTrack3 READ GetCpCanWriteTrack3)
    Q_PROPERTY(QString CpSecurity READ GetCpSecurity)
    Q_PROPERTY(QString CpVariant READ GetCpVariant)
    Q_PROPERTY(QString CpChipPower READ GetCpChipPower)
    Q_PROPERTY(BOOL CpPowerSaveModeControl READ GetCpPowerSaveModeControl)
    Q_PROPERTY(BOOL CpSupportedGuidelights READ GetCpSupportedGuidelights)
    Q_PROPERTY(SHORT StBinCount READ GetStBinCount)
    Q_PROPERTY(QString StBinStatus READ GetStBinStatus)
    Q_PROPERTY(QString StMediaStatus READ GetStMediaStatus)
    Q_PROPERTY(QString StChipStatus READ GetStChipStatus)
    Q_PROPERTY(QString Track1 READ GetTrack1)
    Q_PROPERTY(QString Track2 READ GetTrack2)
    Q_PROPERTY(QString Track3 READ GetTrack3)
    Q_PROPERTY(QString Track1Status READ GetTrack1Status)
    Q_PROPERTY(QString Track2Status READ GetTrack2Status)
    Q_PROPERTY(QString Track3Status READ GetTrack3Status)
    Q_PROPERTY(QString AnswerToReset READ GetAnswerToReset)
    Q_PROPERTY(QString CardNumber READ GetCardNumber)
    Q_PROPERTY(QString CardSerial READ GetCardSerial)
    Q_PROPERTY(QString StartTime READ GetStartTime)
    Q_PROPERTY(QString EndTime READ GetEndTime)
    Q_PROPERTY(QString Name READ GetName)
    Q_PROPERTY(QString NameExpend READ GetNameExpend)
    Q_PROPERTY(QString IDNumber READ GetIDNumber)
    Q_PROPERTY(QString IType READ GetIType)

public:
    CMIDC(const QString& strDBusObjName);
    ~CMIDC();

    static CMIDC* GetInstance(void);

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

    //属性处理函数
    virtual void SetServiceName(QString newVal);
    virtual QString GetStDetailedDeviceStatus();

    SHORT GetCpBinSize(void);
    BOOL GetCpCanCapture(void);
    BOOL GetCpCanEject(void);
    BOOL GetCpCanReadTrack1(void);
    BOOL GetCpCanReadTrack2(void);
    BOOL GetCpCanReadTrack3(void);
    BOOL GetCpCanReadTrackJIS2(void);
    BOOL GetCpCanWriteTrack1(void);
    BOOL GetCpCanWriteTrack2(void);
    BOOL GetCpCanWriteTrack3(void);
    QString GetCpSecurity(void);
    QString GetCpVariant(void);
    QString GetCpChipPower(void);
    BOOL GetCpPowerSaveModeControl(void);
    BOOL GetCpSupportedGuidelights(void);
    SHORT GetStBinCount(void);
    QString GetStBinStatus(void);
    QString GetStMediaStatus(void);
    QString GetStChipStatus(void);
    QString GetTrack1(void);
    QString GetTrack2(void);
    QString GetTrack3(void);
    QString GetTrack1Status(void);
    QString GetTrack2Status(void);
    QString GetTrack3Status(void);
    QString GetAnswerToReset(void);
    QString GetCardNumber(void);
    QString GetCardSerial(void);
    QString GetStartTime(void);
    QString GetEndTime(void);
    QString GetName(void);
    QString GetNameExpend(void);
    QString GetIDNumber(void);
    QString GetIType(void);

signals:
    //信号函数，类似之前的事件
    //事件
    void CardInserted(void);
    void CardAccepted(void);
    void CardAcceptFailed(void);
    void CardInvalid(void);
    void CardAcceptCancelled(void);
    void CardTaken(void);
    void WaitCancelled(void);
    void CardCaptured(void);
    void CardCaptureFailed(void);
    void CardEjected(void);
    void CardEjectFailed(void);
    void CardRead(void);
    void ChipPowerComplete(void);
    void ChipPowerFailed(void);
    void ChipInvalid(QString Token);
    void ChipDataReceived(QString Token, QString Bytes);
    void ResetComplete(void);
    void ResetFailed(void);
    void GetICInfoCompleted(void);
    void GetICInfoFailed(void);
    void ReadIcTLVCompleted(QString Info);
    void ReadIcTLVFailed(void);

public slots:
    //方法，JS可直接调用
    virtual short OpenConnection(void);

    virtual short CloseConnection(void);

    SHORT AcceptAndReadTracks(QString TrackMap, LONG Timeout);
    SHORT AcceptAndReadAvailableTracks(QString TrackMap, LONG Timeout);
    SHORT CancelAccept(void);
    SHORT Eject(LONG Timeout);
    SHORT Capture(LONG Timeout);
    SHORT AcceptAndChipInitialise(QString Token, LONG Timeout);
    SHORT ChipPower(QString Action);
    SHORT ResetBinCountSync(void);
    SHORT Reset(QString ResetAction,long TimeOut);
    SHORT PBOCGetICInfo(LONG Timeout);
    SHORT PBOCReadIcTLV(LONG Timeout);
    int Execute(int ID, LPVOID lpData);

private:
    //根据错误码发送相应的事件
    //void ErrorSend(int err);//去除掉该函数

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
    //执行LFS_CMD_IDC_RESET_COUNT
    int N_ResetCount(LONG timeout=HM_MIDDLE_TIMEOUT);
    //执行LFS_CMD_IDC_Get_RAW_DATA
    int N_ReadRawData(WORD readData,LONG timeout=HM_MIDDLE_TIMEOUT);
    //执行LFS_CMD_IDC_RESET
    int N_Reset(WORD wResetIn,LONG timeout=HM_MIDDLE_TIMEOUT);
    //执行LFS_CMD_IDC_CHIP_POWER
    int N_ChipPower(WORD wChipPower,LONG timeout=HM_MIDDLE_TIMEOUT);

    QString N_PBOCGetPSEAID(LPCTSTR aIn = "1PAY.SYS.DDF01");
    SHORT N_ExecuteChipIO(const CHAR* chCApdu, CHAR* chRApdu);
    QString N_PBOCSelectADF(LPCTSTR aIn);
    // 交易初始化
    QString N_PBOCGetProOption(LONG aTradeMoney, SHORT aTradeType);
    // 读应用数据
    QString N_PBOCReadAppData(LPCTSTR aAFL);
    // 圈存圈提初始化
    QString N_PBOCInitForLoad();
    // 获取55域上送数据
    QString N_PBOCReadIcTLV(LPCTSTR aInPA, LPCTSTR aInAD, LPCTSTR aInAR);
    // 获取IC卡基本信息
    QString N_PBOCGetICInfo();
    // 读取二磁道信息
    QString N_PBOCGetTrack2Data();

    int N_PBOCAidList(QString configPath);

    int N_SelectADF();

private:
        typedef std::vector<QString> VerStr;

        //得到以sep分割的所有子串
        VerStr Token_All(const QString str,const char sep);

        //功能：将BCD码转换为ASCII码
        //参数： asc   asc码数据输出
        //       bcd   bcd码数据
        //       len   bcd码数据长度
        //返回值：参照公共返回值列表及函数返回值列表
        bool BCDToASCII(LPBYTE asc, const LPBYTE bcd, int len);

        //功能：将ASCII码转换为BCD码
        //参数： bcd   bcd码数据输出
        //       asc   asc码数据
        //       len   asc数据长度
        //返回值：参照公共返回值列表及函数返回值列表
        bool  ASCIIToBCD(unsigned char *bcd, const unsigned char *asc, int nLen);

        void SetDeviceStatus(WORD wStatus);

        //
        WORD GetTracks(const QString strTracks);

        //
        //功能：保存磁道数据在m_chTracks数组中
        //输入：lppCardData 磁道数据结构体
        //返回值：无。
        void SaveTracks(LPLFSIDCCARDDATA *lppCardData);

        bool IsTracksDataOK();

        void GetPBOCConfigPath();
private:
    static CMIDC* m_pInstance;

    REQUESTID m_CurrRequestID;
    QString m_Token;

    //ocx中AcceptAndReadTracks（）、ReadTracks（）方法和AcceptAndChipInitialise（）都是调用ReadRawData（）。
    //完成时，分别返回CardAccepted事件、返回CardRead事件和ChipDataReceived事件
    //1-返回CardAccepted事件，2-返回CardRead事件，3-返回ChipDataReceived事件。
    WORD m_wReadDataFlag;

    char m_chTracks[4][1024];//track1,track2,track3,chip(ATR)
    QString m_TrackXStatus[3];//磁道数据状态

    QString m_PreDevStatus,m_CurrDevStatus;//设备前一时刻的状态和当前设备的状态

    LFSIDCSTATUS m_Status;//设备状态

    LFSIDCCAPS m_Caps;//设备能力



    CICDPC m_Icdpc;
    //卡号，卡序列号，有效日期，失效日期，姓名，姓名扩展，身份证号，证件类型
    QString m_CardNumber,m_CardSerial,m_StartTime,m_EndTime,m_Name,m_NameExpand,m_IDNumber,m_IType;

    int m_cardType;
    QString m_szChipOut;	// 发送CHIP IO命令后，卡片所返回的数据

    CDllDSTO m_DSTO;//临时数据对象

    QString m_PBOCConfigPath;//PBOC配置文件路径

private:
    char m_szLogicalName[MAX_PATH];
    bool m_bOpened;

};

extern "C" {
MIDCSHARED_EXPORT QObject* CreateInstance(void);
}

#endif // MIDC_H
