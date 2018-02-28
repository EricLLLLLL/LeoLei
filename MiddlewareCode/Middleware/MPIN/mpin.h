#ifndef MPIN_H
#define MPIN_H

#include "mpin_global.h"
#include "webinteractobj.h"
#include "easytrace.h"
#include "dllDSTO.h"
#include "apictrl.h"
#include "Helper/helper.h"
#include <QString>


#define HM_MIDDLE_TIMEOUT 60*1000	//60s

class MPINSHARED_EXPORT CMPIN : public CWebInteractObj
{
    Q_OBJECT

    Q_PROPERTY(QString ServiceName WRITE SetServiceName)
    Q_PROPERTY(QString StDetailedDeviceStatus READ GetStDetailedDeviceStatus)
    Q_PROPERTY(BOOL CpCanCBC READ GetCpCanCBC)
    Q_PROPERTY(BOOL CpCanECB READ GetCpCanECB)
    Q_PROPERTY(BOOL CpCanRSA READ GetCpCanRSA)
    Q_PROPERTY(BOOL CpCanMAC READ GetCpCanMAC)
    Q_PROPERTY(BOOL CpCanTripleECB READ GetCpCanTripleECB)
    Q_PROPERTY(BOOL CpCanTripleCBC READ GetCpCanTripleCBC)
    Q_PROPERTY(BOOL CpCanTripleCFB READ GetCpCanTripleCFB)
    Q_PROPERTY(BOOL CpCanTripleMAC READ GetCpCanTripleMAC)
    Q_PROPERTY(QString CpKeysSupported READ GetCpKeysSupported)
    Q_PROPERTY(SHORT PINLength READ GetPINLength)
    Q_PROPERTY(QString CpPINFormatsSupported READ GetCpPINFormatsSupported)

public:
    CMPIN(const QString& strDBusObjName);
    ~CMPIN();

    static CMPIN* GetInstance(void);

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
    BOOL GetCpCanCBC(void);
    BOOL GetCpCanECB(void);
    BOOL GetCpCanRSA(void);
    BOOL GetCpCanMAC(void);
    BOOL GetCpCanTripleECB(void);
    BOOL GetCpCanTripleCBC(void);
    BOOL GetCpCanTripleCFB(void);
    BOOL GetCpCanTripleMAC(void);
    QString GetCpKeysSupported(void);
    SHORT GetPINLength(void);
    QString GetCpPINFormatsSupported(void);


signals:
    void GetPinCompleted(void);
    void GetPinFailed(void);
    void PinBlockCompleted(QString PinBlock);
    void PinBlockFailed(void);
    void GetPinCancelled(void);
    void GetPinCancelFailed(void);
    void KeyPressed(QString Key, LONG KeyCode);
    void GetDataComplete(QString EnteredString);
    void GetDataFailed(void);
    void EncryptComplete(QString Data);
    void DecryptComplete(QString Data);
    void MACGenerated(QString MAC);
    void CryptFailed(void);
    void GetDataCancelled(void);
    void GetDataCancelFailed(void);
    void InitializeComplete(void);
    void InitializeFailed(void);
    void KeyLoaded(void);
    void KeyLoadFailed(void);
    void ResetComplete(void);
    void ResetFailed(void);

public slots:
    SHORT CloseConnection(void);
    SHORT OpenConnection(void);
    void Reset(LONG timeout);
    SHORT LoadMasterKey(QString KeyName, QString KeyValue, LONG Timeout);
    SHORT Initialize(void);
    SHORT LoadWorkKey(QString KeyName, QString KeyValue, QString EncKeyName);
    SHORT EncryptECB(QString Value, QString KeyName, SHORT PadChar);
    SHORT DecryptECB(QString Value, QString KeyName, SHORT PadChar);
    SHORT EncryptCBC(QString Value, QString KeyName, QString IVName, SHORT PadChar);
    SHORT DecryptCBC(QString Value, QString KeyName, QString IVName, SHORT PadChar);
    SHORT GenerateMAC(QString Value, QString KeyName, QString IVName, SHORT PadChar);
    SHORT GenerateMACClearIV(QString Value, QString KeyName, QString IV, SHORT PadChar);
    SHORT GetPin(SHORT MinLength, SHORT MaxLength, BOOL AutoEnd, QString ActiveKeys, QString TerminatorKeys, LONG Timeout);
    SHORT PinBlock(QString Format, QString CustomerData, SHORT PadChar, QString KeyName, QString XORData, QString SecondKeyName);
    SHORT CancelGetPin(void);
    SHORT GetData(SHORT MaxKeys, BOOL AutoEnd, QString ActiveKeys, QString TerminatorKeys, LONG Timeout);
    SHORT CancelGetData(void);
    QString GetKeyNamesSync(void);
    QString GetKeyUseFlagsSync(QString KeyName);
    BOOL KeyIsValidSync(QString KeyName);
    SHORT ExtendedLoadKey(QString KeyName, QString KeyValue, QString UseFlags, QString Identification);
    SHORT ExtendedLoadEncryptedKey(QString KeyName, QString KeyValue, QString EncKeyName, QString UseFlags, QString Identification);
    int Execute(int ID, LPVOID lpData);

private:
    //异步执行wosa命令

    int N_Open();
    int N_Close();

    //取消
    int N_Cancel(WORD requestID);

    //导入密钥
    int N_ImportKey(LPLFSPINIMPORT lpImport,LONG timeout=HM_MIDDLE_TIMEOUT);

    //加解密
    int N_Crypt(LPLFSPINCRYPT lpCrypt,LONG timeout=HM_MIDDLE_TIMEOUT);

    //Get Pin
    int N_GetPin(LPLFSPINGETPIN lpGetPin,LONG timeout=HM_MIDDLE_TIMEOUT);

    //Get PinBlock
    int N_GetPinBlock(LPLFSPINBLOCK lpPinBlock,LONG timeout=HM_MIDDLE_TIMEOUT);

    //Get Data
    int N_GetData(LPLFSPINGETDATA lpPinGetData,LONG timeout=HM_MIDDLE_TIMEOUT);

    //LFS_CMD_PIN_INITIALIZATION
    int N_Initialization(LPLFSPININIT lpInit,LONG timeout=HM_MIDDLE_TIMEOUT);

    //LFS_CMD_PIN_RESET
    int N_Reset(LONG timeout=0);

    //LFS_INF_PIN_KEY_DETAIL
    int N_KeyDetail(LPSTR lpsKeyName);

    //设备能力
    int N_GetCaps();

    //设备状态
    int N_GetStatus();
    //
    int N_FuncKeyDetail();

    //设置设备状态
    void SetDeviceStatus(WORD wStatus);

    //得到键值字符串
    void SaveFuncKeysName(ULONG uFuncKeys);

    //得到支持的PinFormat
    void SavePinFormats(ULONG uPinFormats);


    LFSPINIMPORT GetImportKeyParam(QString keyName,QString encKeyName,QString keyValue,WORD use=0x27);
    LFSPINCRYPT GetCryptParam(QString value,QString keyName,BYTE padChar);
    int GetCryptAlgorithm(int algorithm,const char * keyName);
    long GetKeysCode(QString strKeys);
    long GetPinBlockFormat(QString strFormat);


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


    typedef std::vector<QString> VerStr;

    //得到以sep分割的所有子串
    VerStr Token_All(const QString str,const char sep);

    //从C:/HMKiosk/Config/data/key.data文件取key的长度
    int __GetKeyLenFromFile(const char * szKeyName);

    //从注册表SOFTWARE\\XFS\\HMKeyManage\\取key的长度
    int __GetKeyLenFormReg(const char * szKeyName);
    //获取setup.ini配置所在路径
    void __GetConfigPath();

private:
    LFSPINSTATUS m_Status;
    LFSPINCAPS m_Caps;
    QString m_PreDevStatus,m_CurrDevStatus;//设备前一时刻的状态和当前设备的状态
    QString m_SuppKeysName;//设备支持的键值
    int m_PinLength;//输入Pin的长度
    QString m_SuppPinFormats;

    REQUESTID m_CurrRequestID;

    std::map<unsigned long,QString> m_KeyMap;//键值到键字符串的映射
    std::map<QString,unsigned long> m_KeyMap1;//键字符串到键值的映射
    std::map<unsigned long,QString> m_PinBlockFormatMap;
    std::map<QString,unsigned long> m_PinBlockFormatMap1;

    WORD m_wCryptAlgorithm;//加解密算法(ECB,CBC)
    WORD m_wMode;//模式，加密or解密

    WORD m_wKeyDetailType;//1 = GetKeyNames,2 = GetKeyFlags

    WORD m_wPinOrData;//1 = BUFFERPIN,2 = USERENTRY

    bool m_bPressCancel;//cancel键是否按下

    REQUESTID m_PinRequestID,m_DataRequestID;//GetPin、GetData请求ID


    QString m_strKeyDetail;

    CDllDSTO m_DSTO;

    BOOL m_bCancelGetPin;		// 此时是否是执行的CancelGetPin
    BOOL m_bCancelGetData;		// 此时是否是执行的CancelGetData

    QString m_strConfigPath; //配置文件路径

private:
    static CMPIN *m_pInstance;

    char m_szLogicalName[MAX_PATH];
    bool m_bOpened;

};

extern "C" {
MPINSHARED_EXPORT QObject* CreateInstance(void);
}

#endif // MPIN_H
