// Last updated at 2015/12/9 16:54:04

#include "mtcp_global.h"
#include <QObject>

#include "dllDRCV.h"
#include "dllDSND.h"
#include "dllDSTO.h"


#include "win_type_def.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

class MTCPSHARED_EXPORT CMTCP : public QObject
{
    Q_OBJECT
public:
    CMTCP(QObject* pParent=NULL);
    ~CMTCP();

    static CMTCP *GetInstance(void);

signals:
    //信号函数，类似之前的事件回调函数
    void SendFailed(void);
    void OnRecved(const QString& check);
    void CompositionDataFail(void);
    void CompositionDataCompleted(const QString& Data);
    void AnalysisFailed(void);
    void Timeout(void);

public slots:
    //方法，JS可直接调用
    void SendToHost(const QString& MAC, LONG Timeout);
    void CompositionData(const QString& TransactionType);

private:
	bool AnalysisRecvPassbookBuf(char *pszBuf, int *pnBufLen);
	bool GetMasterDataQueryBuf(char *pszBuf, long *pnBufLen);
	bool GetPassbookBuf(char *pszBuf, long *pnBufLen);
	bool EncryptTrace(LPBYTE outData, const LPBYTE inData, int len);

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

	//功能：格式化字符串originalSt。每行32个字符，每两个字符间用空格分隔。
	//参数：originalStr 原始字符串。
	//返回值：格式化后的字符串。若originalStr为空或长度为奇数，返回字符串=originalStr。
    QString FormatData(QString originalStr);

	void GetConfigPath();//得到配置文件的路径

public:
    static void *ThreadProc(void *lpParam);

private:
	long m_Port;//端口号
    QString m_strIP;//服务器IP地址
	long m_PassbookPort;//存折补登端口号
    QString m_strPassbookIP;//存折补登服务器IP地址
	int m_iRetry;//尝试连接主机次数

    //HANDLE m_hThread;
    pthread_t m_pThread;

    int m_sHost;
    struct sockaddr_in m_servAddr;

	CDllDSTO m_DSTO;
	CDllDSND m_DSend;
	CDllDRCV m_DRecv;


    QString m_TranscationType;

    QString m_Data;//报文数据

    QString m_strConfigPath;//配置文件的路径
	int m_Timeout;//
    static CMTCP* m_pInstance;
};

extern "C" {
    MTCPSHARED_EXPORT QObject* CreateInstance();
}
