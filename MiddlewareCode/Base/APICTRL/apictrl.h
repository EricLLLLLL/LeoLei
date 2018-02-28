#pragma once

#include "win_type_def.h"
#include "lfsspi.h"
#include "lfscdm.h"
#include "lfscim.h"
#include "lfsidc.h"
#include "lfspin.h"
#include "lfsptr.h"
#include "lfssiu.h"
#include "lfsttu.h"
#include "lfsvdm.h"
#include <QObject>
#include "QMutex"

class CApiCtrl:public QObject
{
    Q_OBJECT
public:
	CApiCtrl(void);
	virtual ~CApiCtrl(void);

    void SetDBusName(LPSTR lpDBusName);

public:
    HRESULT LFSCancelAsyncRequest(REQUESTID RequestID);
    HRESULT LFSOpen(LPSTR lpszLogicalName, DWORD dwTimeOut);
    HRESULT LFSAsyncOpen(LPSTR lpszLogicalName, DWORD dwTimeOut, LPREQUESTID lpRequestID);
    HRESULT LFSClose();
    HRESULT LFSAsyncClose(LPREQUESTID lpRequestID);
    HRESULT LFSRegister(DWORD dwEventClass);
    HRESULT LFSAsyncRegister(DWORD dwEventClass, LPREQUESTID lpRequestID);
    HRESULT LFSDeregister(DWORD dwEventClass);
    HRESULT LFSAsyncDeregister(DWORD dwEventClass, LPREQUESTID lpRequestID);

    //注意：不在使用lppResult指针时，上层需要调用::LFSFreeResult(lpResult)释放内存。
    HRESULT LFSGetInfo(DWORD dwCategory, LPVOID lpQueryDetails, DWORD dwTimeOut, LPLFSRESULT *lppResult);
    HRESULT LFSAsyncGetInfo(DWORD dwCategory, LPVOID lpQueryDetails, DWORD dwTimeOut, LPREQUESTID lpRequestID);

    //注意：不在使用lppResult指针时，上层需要调用::LFSFreeResult(lpResult)释放内存。
    HRESULT LFSExecute(DWORD dwCommand, LPVOID lpCmdData,DWORD dwTimeOut, LPLFSRESULT *lppResult);
    HRESULT LFSAsyncExecute(DWORD dwCommand, LPVOID lpCmdData, DWORD dwTimeOut, LPREQUESTID lpRequestID);

private slots:
    HRESULT WaitSyncComplete(LPLFSRESULT *lppResult, REQUESTID requestID, UINT uMsg);//等待同步命令完成
protected:
	HAPP      m_hApp;		//应用程序句柄与打开会话相关联
	LPSTR     m_lpszAppID;	//指向含有应用程序ID的空终止字符	
	HSERVICE  m_hService;	//服务句柄
    QMutex    m_hMutex;		//互斥对象句柄
    LPSTR     m_lpDBusName; //DBus对象名
};
