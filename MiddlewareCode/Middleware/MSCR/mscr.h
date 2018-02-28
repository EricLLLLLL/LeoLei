// Last updated at 2015/11/26 15:24:14
#ifndef MSCR_H
#define MSCR_H

#include "Resource.h"
#include "webinteractobj.h"
#include <vector>
#include "dllDSTO.h"
#include "apictrl.h"
#include "mscr_global.h"
#include "easytrace.h"
#include <QString>


#define HM_MIDDLE_TIMEOUT 60*1000	//60s

class MSCRSHARED_EXPORT CMSCR : public CWebInteractObj
{
    Q_OBJECT

    Q_PROPERTY(QString ServiceName WRITE SetServiceName)
    Q_PROPERTY(QString StDetailedDeviceStatus READ GetStDetailedDeviceStatus)
    Q_PROPERTY(BOOL CpCanReadTrack1 READ GetCpCanReadTrack1)
    Q_PROPERTY(BOOL CpCanReadTrack2 READ GetCpCanReadTrack2)
    Q_PROPERTY(BOOL CpCanReadTrack3 READ GetCpCanReadTrack3)
    Q_PROPERTY(QString StMediaStatus READ GetStMediaStatus)
    Q_PROPERTY(QString Track1 READ GetTrack1)
    Q_PROPERTY(QString Track2 READ GetTrack2)
    Q_PROPERTY(QString Track3 READ GetTrack3)
    Q_PROPERTY(QString Track1Status READ GetTrack1Status)
    Q_PROPERTY(QString Track2Status READ GetTrack2Status)
    Q_PROPERTY(QString Track3Status READ GetTrack3Status)
public:
    CMSCR(const QString& strDBusObjName);
    virtual ~CMSCR();
    static CMSCR* GetInstance(void);
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
    BOOL GetCpCanReadTrack1(void);
    BOOL GetCpCanReadTrack2(void);
    BOOL GetCpCanReadTrack3(void);
    QString GetStMediaStatus(void);
    QString GetTrack1(void);
    QString GetTrack2(void);
    QString GetTrack3(void);
    QString GetTrack1Status(void);
    QString GetTrack2Status(void);
    QString GetTrack3Status(void);

signals:
    //信号函数，类似之前的事件
    void CardInserted(void);
    void CardAccepted(void);
    void CardAcceptFailed(void);
    void CardInvalid(void);
    void CardAcceptCancelled(void);
    void CardAcceptCancelFailed(void);
    void CardTaken(void);
    void CardRead(void);
    void ResetComplete(void);
    void ResetFailed(void);
    // 属性
public slots:
    // 方法
    SHORT CloseConnection(void);
    SHORT OpenConnection(void);
    SHORT AcceptAndReadTracks(QString TrackMap, LONG Timeout);
    SHORT AcceptAndReadAvailableTracks(QString TrackMap, LONG Timeout);
    SHORT CancelAccept(void);
    SHORT Reset(QString ResetAction,LONG Timeout);
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
    //执行LFS_CMD_IDC_Get_RAW_DATA
    int N_ReadRawData(WORD readData,LONG timeout=HM_MIDDLE_TIMEOUT);
    //执行LFS_CMD_IDC_RESET
    int N_Reset(WORD wResetIn,LONG timeout=HM_MIDDLE_TIMEOUT);
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
private:

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

    //卡号，卡序列号，有效日期，失效日期，姓名，姓名扩展，身份证号，证件类型
    QString m_CardNumber,m_CardSerial,m_StartTime,m_EndTime,m_Name,m_NameExpand,m_IDNumber,m_IType;

    CDllDSTO m_DSTO;//临时数据对象

private:
    static CMSCR* m_pInstance;
    char m_szLogicalName[MAX_PATH];
    //CApiCtrl m_apiCtrl;
    bool m_bOpened;

};
extern "C" {
MSCRSHARED_EXPORT QObject* CreateInstance(void);
}
#endif
