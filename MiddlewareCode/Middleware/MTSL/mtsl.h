// Last updated at 2016/9/23 14:15:36
#ifndef  MTSL_H
#define  MTSL_H

#include "Resource.h"
#include "easytrace.h"
//#include "sqlite3.h"
#include <vector>
#include "dllDSTO.h"
#include <iostream>
#include <fstream>
#include <string>
#include "mtsl_global.h"
#include "QString"
#include "Helper/helper.h"
#include "QMutex"
using namespace std;
using std::string;
using std::vector;

enum ENUM_SEARCH_TYPE {
	stDefault = 0,
	stByTime =1,
	stByCardNo,
	stBySerialNo
};

enum ENUM_RECORD_TYPE
{
	WriteTransRecord=0,//////写交易记录
	WriteRetainCardRecord,//////写吞卡记录
	WriteCheckTransRecord,/////写对账记录
	ReadTransAndRetainCardRecord,//////读交易记录和吞卡记录
	RenameRecord,////////重命名交易和吞卡记录
	WriteCloseAccount//////写销户记录
};


class MTSLSHARED_EXPORT CMTSL:public QObject
{
    Q_OBJECT

public:
    CMTSL();
    virtual ~CMTSL();
    static CMTSL* GetInstance(void);

public slots:
    LONG AddTransLogSync(QString strTransLog);
    LONG CountTypeSync(QString strStartDate, QString strEndDate, QString strType);
    QString SumTypeMoneySync(QString strStartDate, QString strEndDate, QString strType);
	////////返回每一项值为 笔数：金额：失败笔数
	////////总共八项为银行卡小额取款，银行卡大额取款，存折取款，银行卡小额存款，银行卡大额存款，无卡无折小额存款，无卡无折大额存款，对公业务存款
    QString SumDataSync(QString strStartDate, QString strEndDate);
    LONG HandleRecordFileSync(long ID,QString strTransData);////操作记录文件
    LONG InitJnlSync(QString sFileName, LONG nLinesPerPage);
	LONG UnInitJnlSync(void);
	//nUpDownFlag--- 1:下翻 -1: 下翻; nLineOrPageFlag--- 0: 翻页, 1:移行
    QString ReadJNLSync(long nUpDownFlag,long nLineOrPageFlag);
	//nBeginFlag--- 1:从头开始查找, 0:查找下一条;  nType--- 1:按时间 2:按卡号 3:按流水号
    QString SearchJNLSync(long nBeginFlag,long nType,QString sSubString);
    LONG InitFsnSync(QString sFileName);
	LONG UnInitFsnSync(void);
    QString ReadFSNSync(void);
    SHORT SearchFSN(QString startTime, QString endTime,QString strSno, QString strJnlNum, QString strAccountNo);
	//////更新交易记录 传的数据一条记录
    SHORT UpdateRecord(QString Data);
	///////更新对账文件记录  传一条记录
    SHORT UpdateCheckRecord(QString Data);
	//////更新数据库交易记录数据
    SHORT UpdateTransLogSync(QString strTransLog);


    //static DWORD WINAPI ThreadProc(LPVOID lpParam);
signals:
    void SearchFSNComplete(QString results);
    void SearchFSNFail(void);
    void UpdateRecordComplete(void);
    void UpdateRecordFailed(void);
    void UpdateCheckComplete(void);
    void UpdateCheckFailed(void);
private:
	void Init(void);
	void UnInit(void);
	void GetConfigPath(void);//得到配置文件的路径
    void N_GetDatabasePath(void);
	void N_InitUpCheckFile(void);
	void N_InitDataBase(void);
    int CreateFileDirectory(char* szPath);

    int WriteTransFile(QString sData); //add by zyf for 写入交易记录文件
	int ReadTransFile(); //add by zyf for 获取现金交易统计和现金异常交易记录
	int RenameTransFile(); //add by zyf for 清机时改名本周期的交易记录文件

	int ReadTransReDEPFile();	// add by zwh for 获取取款回存取款失败金额回存交易记录
	int ReadCloseAccountFile(); // add by zwh for 获取销户交易记录

    int WriteCloseAccountFile(QString sData);	// add by zwh for 写入销户记录信息
	int RenameCloseAccountFile();				// add by zwh for 清机时改名本周起的销户记录

    int WriteCardRetainFile(QString sData); //add by zyf for 写入吞卡记录文件
	int ReadRetainCardFile(); //add by zyf for 获取吞卡记录
	int RenameCardRetainFile(); //add by zyf for 清机时改名本周期的吞卡记录文件

    int WriteCheckTransFile(QString sData);//////写对账文件

	void N_UpdateRecord(void);
	void N_UpdateCheckRecord(void);
	
    SHORT N_SearchFSN(QString startTime, QString endTime,QString strSno, QString strJnlNum, QString strAccountNo);

    typedef std::vector<QString> VerStr;
    VerStr Token_All(const QString str,const char sep);
    QString G2U(const char* gb2312);//GB2312到UTF-8的转换

    //CRITICAL_SECTION HandleRecordFile_critical;
	HANDLE m_hSearchEvent;
	HANDLE m_hUpdateRecordEvent;
	HANDLE m_hUpdateCheckEvent;
	HANDLE m_hThread;
	char m_chSearchResultPath[MAX_PATH];//冠字号搜索结果保存路径
	BOOL m_bOpened;//////数据是否打开
	BOOL m_bJnlInit;//////电子流水流是否初始化
	BOOL m_bFsnInit;//////冠字号流是否初始化
    //sqlite3 * m_lpDB;               //数据库对象
    QString m_strDataBasePathName;  //数据库路径名
    QString m_strConfigPath;//配置文件的路径
    QString m_strCheckFilePath;////对账文件路径
    QString m_strCheckFileName;/////对账文件名
	CDllDSTO dllDSTO;
	fstream m_FsnStream;////fsn
	/////////////jnl
	fstream m_JnlStream;
    QString m_sFileName;
    QString m_UpdateRecordData;
    QString m_UpdateCheckRecordData;
	LONG m_nLinesPerPage;  //每页行数
	LONG m_nLinesCount;	   //总行数	
	LONG m_nCurrentPos;   //当前行号
    static CMTSL* m_pInstance;
    QMutex m_oMutex;
};
extern "C" {
MTSLSHARED_EXPORT QObject* CreateInstance(void);
}
#endif
