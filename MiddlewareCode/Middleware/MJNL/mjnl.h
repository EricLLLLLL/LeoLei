// Last updated at 2016/7/20 9:23:24
#ifndef MJNL_H
#define MJNL_H

#include "Resource.h"
#include "JnlFormFormat.h"
#include "easytrace.h"
#include "dllDSTO.h"
#include "AlarmLib.h"
#include "mjnl_global.h"
#include "QString"
#include "Helper/helper.h"
class MJNLSHARED_EXPORT CMJNL:public QObject
{
    Q_OBJECT

public:
    CMJNL();
   // CMJNL();
    ~CMJNL();
    static CMJNL* GetInstance(void);

public slots:
    // 方法
    SHORT PrintSync(QString strJnlFromName);
    SHORT WriteLogSync(QString Buffer, SHORT LogLevel);

private:
    void    N_GetConfigPath();
    void    N_ChangeShowStyle(QString FormName, QString FieldName, int SizeX, QString &FieldValue);
    QString N_CountBoxAount(QString FormName);
	SHORT   N_InitFrom(void);
    int     N_GetFieldValue(QString FormName, QString &FieldValues);
	int     N_AddMachineSettleAcct(std::string &strFormat);
	int     N_AddMachineSettleAcct_haveReDep(std::string &strFormat);
    int     N_AddTime(QString FormName, QString &FieldValues);
	int     N_WriteElectJnl(const char *pInfo, ...);
	//写JsLog
	SHORT   N_DeleteOldFile();
	SHORT   N_ElectDeleteOldFile();
    bool    N_CreateFileDirectory(char* szPath);
    //static DWORD WINAPI MyWriteLogFunc(LPVOID lpPara);

    bool    N_GetDataFromDSTO(BOOL bPersistent, QString DataName, QString &Value);
    bool    N_GetDataFromDSTO(BOOL bPersistent, QString DataName, long &Value);
    bool    N_GetDataFromDSTO(BOOL bPersistent, QString DataName, LPBYTE Value);

    QString BOXVALUEFMT[7];   //钞箱1、2、3、4、5的纸币面额
    QString NOTENUM[7];       //钞箱1、2、3、4、5的纸币张数
    QString BOXAMOUNT[7];     //钞箱1、2、3、4、5的金额
    QString ALLBOXNOTEAMOUNT; //钞箱总金额

    CJnlFormFormat m_JnlFormFormat;

    QString m_strFormPathName;
    QString m_strJnlPath;
    QString m_JsLogPath;

	WORD m_wExJsLogTime;
	//写日志文件流
	FILE *m_pFile;
	//日志保存天数
	int m_JnlSaveDays;
	int m_JsLogSaveDays;

	CDllDSTO dllDSTO;
    static CMJNL* m_pInstance;
};

extern "C" {
MJNLSHARED_EXPORT QObject* CreateInstance(void);
}
#endif
