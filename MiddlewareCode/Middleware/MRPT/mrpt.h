// Last updated at 2015/12/11 10:42:36
#ifndef MRPT_H
#define MRPT_H

#pragma once

#include "Resource.h"
#include "webinteractobj.h"
#include "dllDSTO.h"
#include "DRPT.h"
#include "mrpt_global.h"
#include "easytrace.h"
#include <QString>

#define HM_MIDDLE_TIMEOUT 60*1000	//60s

#define KAL_PARAM_ERROR -10000

class MRPTSHARED_EXPORT CMRPT : public CWebInteractObj
{
    Q_OBJECT

    Q_PROPERTY(QString ServiceName WRITE SetServiceName)
    Q_PROPERTY(QString StDetailedDeviceStatus READ GetStDetailedDeviceStatus)
    Q_PROPERTY(QString StInkStatus READ GetStInkStatus)
    Q_PROPERTY(QString StMediaStatus READ GetStMediaStatus)
    Q_PROPERTY(QString StPaperStatus READ GetStPaperStatus)
    Q_PROPERTY(BOOL CpCanEject READ GetCpCanEject)
    Q_PROPERTY(BOOL CpCanCapture READ GetCpCanCapture)
    Q_PROPERTY(BOOL CpCanStack READ GetCpCanStack)
    Q_PROPERTY(BOOL CpCanDetectMediaTaken READ GetCpCanDetectMediaTaken)
    Q_PROPERTY(QString FormName READ GetFormName)
    Q_PROPERTY(QString FormBase READ GetFormBase)
    Q_PROPERTY(LONG FormUnitX READ GetFormUnitX)
    Q_PROPERTY(LONG FormUnitY READ GetFormUnitY)
    Q_PROPERTY(LONG FormWidth READ GetFormWidth)
    Q_PROPERTY(LONG FormHeight READ GetFormHeight)
    Q_PROPERTY(QString FormAlignment READ GetFormAlignment)
    Q_PROPERTY(QString FormOrientation READ GetFormOrientation)
    Q_PROPERTY(LONG FormOffsetX READ GetFormOffsetX)
    Q_PROPERTY(LONG FormOffsetY READ GetFormOffsetY)
    Q_PROPERTY(LONG FormVersionMajor READ GetFormVersionMajor)
    Q_PROPERTY(LONG FormVersionMinor READ GetFormVersionMinor)
    Q_PROPERTY(QString FormUserPrompt READ GetFormUserPrompt)
    Q_PROPERTY(QString FormFields READ GetFormFields)
    Q_PROPERTY(QString FormFieldsIndexCount READ GetFormFieldsIndexCount)
    Q_PROPERTY(QString FormFieldsType READ GetFormFieldsType)
    Q_PROPERTY(QString FormFieldsClass READ GetFormFieldsClass)
    Q_PROPERTY(QString FormFieldsAccess READ GetFormFieldsAccess)
    Q_PROPERTY(QString FormFieldsOverflow READ GetFormFieldsOverflow)
    Q_PROPERTY(QString FormFieldsInitialValue READ GetFormFieldsInitialValue)
    Q_PROPERTY(QString FormFieldsFormat READ GetFormFieldsFormat)
    Q_PROPERTY(QString MediaName READ GetMediaName)
    Q_PROPERTY(QString MediaType READ GetMediaType)
    Q_PROPERTY(QString MediaBase READ GetMediaBase)
    Q_PROPERTY(LONG MediaUnitX READ GetMediaUnitX)
    Q_PROPERTY(LONG MediaUnitY READ GetMediaUnitY)
    Q_PROPERTY(LONG MediaSizeWidth READ GetMediaSizeWidth)
    Q_PROPERTY(LONG MediaSizeHeight READ GetMediaSizeHeight)
    Q_PROPERTY(LONG MediaPageCount READ GetMediaPageCount)
    Q_PROPERTY(LONG MediaLineCount READ GetMediaLineCount)
    Q_PROPERTY(LONG MediaPrintAreaX READ GetMediaPrintAreaX)
    Q_PROPERTY(LONG MediaPrintAreaY READ GetMediaPrintAreaY)
    Q_PROPERTY(LONG MediaPrintAreaWidth READ GetMediaPrintAreaWidth)
    Q_PROPERTY(LONG MediaPrintAreaHeight READ GetMediaPrintAreaHeight)
    Q_PROPERTY(LONG MediaRestrictedAreaX READ GetMediaRestrictedAreaX)
    Q_PROPERTY(LONG MediaRestrictedAreaY READ GetMediaRestrictedAreaY)
    Q_PROPERTY(LONG MediaRestrictedAreaWidth READ GetMediaRestrictedAreaWidth)
    Q_PROPERTY(LONG MediaRestrictedAreaHeight READ GetMediaRestrictedAreaHeight)
    Q_PROPERTY(LONG MediaStagger READ GetMediaStagger)
    Q_PROPERTY(QString MediaFoldType READ GetMediaFoldType)
public:
    CMRPT(const QString& strDBusObjName);
    virtual ~CMRPT();
    static CMRPT* GetInstance(void);
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
    QString GetStInkStatus(void);
    QString GetStMediaStatus(void);
    QString GetStPaperStatus(void);
    BOOL GetCpCanEject(void);
    BOOL GetCpCanCapture(void);
    BOOL GetCpCanStack(void);
    BOOL GetCpCanDetectMediaTaken(void);
    QString GetFormName(void);
    QString GetFormBase(void);
    LONG GetFormUnitX(void);
    LONG GetFormUnitY(void);
    LONG GetFormWidth(void);
    LONG GetFormHeight(void);
    QString GetFormAlignment(void);
    QString GetFormOrientation(void);
    LONG GetFormOffsetX(void);
    LONG GetFormOffsetY(void);
    LONG GetFormVersionMajor(void);
    LONG GetFormVersionMinor(void);
    QString GetFormUserPrompt(void);
    QString GetFormFields(void);
    QString GetFormFieldsIndexCount(void);
    QString GetFormFieldsType(void);
    QString GetFormFieldsClass(void);
    QString GetFormFieldsAccess(void);
    QString GetFormFieldsOverflow(void);
    QString GetFormFieldsInitialValue(void);
    QString GetFormFieldsFormat(void);
    QString GetMediaName(void);
    QString GetMediaType(void);
    QString GetMediaBase(void);
    LONG GetMediaUnitX(void);
    LONG GetMediaUnitY(void);
    LONG GetMediaSizeWidth(void);
    LONG GetMediaSizeHeight(void);
    LONG GetMediaPageCount(void);
    LONG GetMediaLineCount(void);
    LONG GetMediaPrintAreaX(void);
    LONG GetMediaPrintAreaY(void);
    LONG GetMediaPrintAreaWidth(void);
    LONG GetMediaPrintAreaHeight(void);
    LONG GetMediaRestrictedAreaX(void);
    LONG GetMediaRestrictedAreaY(void);
    LONG GetMediaRestrictedAreaWidth(void);
    LONG GetMediaRestrictedAreaHeight(void);
    LONG GetMediaStagger(void);
    QString GetMediaFoldType(void);

signals:
    //信号函数，类似之前的事件
    void PrintComplete(void);
    void PrintFailed(void);
    void ControlComplete(void);
    void ControlFailed(void);
    void PrintTaken(void);
    void WaitCancelled(void);
    void ResetComplete(void);
    void ResetFailed(void);

public slots:
    // 方法
    SHORT CloseConnectionSync(void);
    SHORT OpenConnectionSync(void);
    SHORT CloseConnection(void);
    SHORT OpenConnection(void);
    SHORT Print(QString FormName, QString FieldValues, LONG Timeout);
    SHORT ControlMedia(QString MediaAction, LONG Timeout);
    QString GetFormNamesSync(void);
    SHORT UpdateFormInfoSync(QString FormName);
    QString GetMediaNamesSync(void);
    SHORT UpdateMediaInfoSync(QString MediaName);
    SHORT PrintFile(QString FileName, LONG Timeout, BOOL DeleteFile);
    SHORT Reset(QString ResetAction, LONG Timeout);
    int Execute(int ID, LPVOID lpData);

private:

    //user start

    HRESULT N_Open();
    HRESULT N_Close();

    //执行LFS_INF_PTR_STATUS
    HRESULT N_GetStatus();
    //执行LFS_INF_PTR_CAPABILITIES
    HRESULT N_GetCaps();

    void N_SetDeviceStatus(WORD wStatus);

    void N_AnalyseString(char *lps, QString &estring);

public:
    //static VOID CALLBACK TimerProc(HWND hwnd,UINT uMsg, UINT_PTR idEvent,DWORD dwTime);

    //将卡尔形式的krFieldValues转换为 wosa格式的wsFieldValues
    //例：将
    void FieldsConvert(QString krFieldValues, char *wsFieldValues);


    typedef struct{

        QString szMediaAction;    //卡尔标准MediaAction 字符串 如 “CUT”
        DWORD dwMediaAction;  //卡尔标准MediaAction对应的XFS标准的宏  如 “LFS_PTR_CTRLCUT”

    }MediaActionMap;

    DWORD GetMediaControl(QString MediaAction);
    //user end

    //user start
private:
    QString m_PreDevStatus,m_DetailedDeviceStatus; //设备前一时刻的状态和当前设备的详细状态

    QString m_DeviceStatus; //DeviceStatus

    LFSPTRSTATUS m_Status;  //设备状态


    LFSPTRCAPS m_Caps;  //设备能力

    int m_isGetFormNames; //GetFormNames函数执行标识
    QString FormNameList;//

    bool m_isPrintTaken;
    bool m_isTimeout;//是否超时
    long m_Timeout; //超时时间

private:
    QString FormName;
    QString FormBase;
    LONG FormUnitX;
    LONG FormUnitY;
    LONG FormWidth;
    LONG FormHeight;
    QString FormAlignment;
    QString FormOrientation;
    LONG FormOffsetX;
    LONG FormOffsetY;
    LONG FormVersionMajor;
    LONG FormVersionMinor;
    QString FormUserPrompt;
    QString FormFields;

    QString  MediaName;
    QString  MediaType;
    QString  MediaBase;
    LONG  MediaUnitX;
    LONG  MediaUnitY;
    LONG  MediaSizeWidth;
    LONG  MediaSizeHeight;
    LONG  MediaPageCount;
    LONG  MediaLineCount;
    LONG  MediaPrintAreaX;
    LONG  MediaPrintAreaY;
    LONG  MediaPrintAreaWidth;
    LONG  MediaPrintAreaHeight;
    LONG  MediaRestrictedAreaX;
    LONG  MediaRestrictedAreaY;
    LONG  MediaRestrictedAreaWidth;
    LONG  MediaRestrictedAreaHeight;
    LONG  MediaStagger;
    QString  MediaFoldType;

    //从DSTO获取凭条数据
private:
    QString BOXVALUEFMT[7]; //钞箱1、2、3、4、5的纸币面额 //modify by art for 双室钞箱, 加6、7钞箱
    QString NOTENUM[7]; //钞箱1、2、3、4、5的纸币张数 //modify by art for 双室钞箱, 加6、7钞箱
    QString BOXAMOUNT[7]; //钞箱1、2、3、4、5的金额//modify by art for 双室钞箱, 加6、7钞箱
    QString ALLBOXNOTEAMOUNT; //钞箱总金额
    QString CountBoxAount(QString FormName); //凭条：计算个钞箱金额、总金额
public:
    /*int N_GetFieldValue(QString FormName, QString &FieldValues);*/
    int M_GetFieldValue(QString FormName, QString &FieldValues);
private:
    bool N_SetCurTimeToDB();
    bool N_GetDataFromDSTO(BOOL bPersistent, QString DataName, QString &Value);
    bool N_GetDataFromDSTO(BOOL bPersistent, QString DataName, long &Value);
    bool N_GetDataFromDSTO(BOOL bPersistent, QString DataName, LPBYTE Value);

    bool N_SetDataToDSTO(BOOL bPersistent, QString DataName, QString Value);
    bool N_SetDataToDSTO(BOOL bPersistent, QString DataName, long Value);
    bool N_SetDataToDSTO(BOOL bPersistent, QString DataName, LPBYTE Value, WORD wSize);

    void ChangeShowStyle(QString FormName, QString FieldName, int SizeX, QString &FieldValue);
    void AddSpace(QString &Value, int SizeX);

    void N_QStringtok(QString src, QString delim, QStringList &desArr);

    //user end

private:
    static CMRPT* m_pInstance;
    CDllDSTO dllDSTO;
    char m_szLogicalName[MAX_PATH];
    bool m_bOpened;
};
extern "C" {
MRPTSHARED_EXPORT QObject* CreateInstance(void);
}
#endif
