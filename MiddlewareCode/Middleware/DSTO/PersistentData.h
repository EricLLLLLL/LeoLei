///////////////////////////
//		Create by art
//////////////////////////

#pragma once

#include "easytrace.h"
#include "win_type_def.h"

class CPeristentData
{
public:
	CPeristentData(void);
	~CPeristentData(void);

protected:
private:
    //static CSNFile m_NoteTraceFile;
    //SQLite m_Sqlite;
	BOOL m_bDataOpened;
	BOOL OpenDatabase();
	void CloseDatabase();
	BOOL GetDataFromInifile(char *pszDataName, char *pszValue);
	BOOL SetDataToInifile(char *pszDataName, char *pszNewValue);

    static DWORD AddSNRThreadFunc(LPVOID lpPara);   //将冠字号信息加入数据库

	short QrySNRFunc(char *pszFileName, char *pszParam); //冠字号信息查询
	short GetBillNoRptFiles(char *pszRptFiles); //add by art for 冠字号凭条打印： 获取冠字号凭条页数和文件名前缀，生成要打印的冠字号文件

	short CreateCommonRptContent(char *pszRptContent); //add by art for 冠字号凭条打印: 冠字号凭条中的普通凭条内容
	short GetScratchPadData(const char *pszDataName, char *pszValue); //获取临时数据
    short SetScratchPadData(char *pszDataName, char *pszDataType, char *pszNewValue); //设置临时数据

public:
	short AddData(char *pszDataName, char *pszDataType, char *pszInitValue, char *pszNeedInit, char *pszNeedClear);
	short SetData(char *pszDataName, char *pszDataType,	char *pszNewValue);
	short GetData(char *pszDataName, char *pszDataType,	char *pszValue);

public:
    // add by zengwh for 深圳农行三期需求生成json文件供应用使用
    short GetBankCodeJsonData(void);
    short GetProcCodeJsonData(void);
    // add by zengwh for 深圳农行三期需求，微信销户需要MD5值以及二维码图片
    short GetQRCodePic(void);
    short GetMD5Data(void);

private:
    QString GetBankName(QString strBankCod);
    SHORT GetProcCodandName(std::map<int, QString>& MapItoCforProvCod, int* iTotal, int iKey[]);
    SHORT GetGargnNameandCode(std::map<QString, QString>& mMapCtoCforGafeeCod, int* iTotal, QStringList &csKey, int iProcCode);
    SHORT GetBankCodeandName(std::map<QString, QString>& MapCtoCforBankCod, int* iTotal, QStringList &csKey);

};
