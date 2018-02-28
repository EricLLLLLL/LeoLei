#pragma once
#include <QObject>
#include <win_type_def.h>
#include "lfsspi.h"
#include <map>
#include <unistd.h>
//#include <lfsutils.h>
#include <sstream>
#include <QtDBus/QDBusAbstractAdaptor>

class CWSWnd;
class DbusListener : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", LFS_MGR_DBUS_INTF_NAME)
public:
    DbusListener(QObject* parent,bool bWaitSync=false);

public slots:
    void event_callback(qlonglong msg_id, qlonglong pointer);

private:
    CWSWnd *m_pParent;
    bool m_bWaitSync;
};

class CWSWnd:public QObject
{
    Q_OBJECT

public:
    CWSWnd();
	virtual ~CWSWnd(void);
public:
    static bool InitDBusService();
    static bool DestroyDBusService();
    bool CreateDBus(const char* pzDBusObjName,bool bWaitSync=false);
    void DestoryDBus();
    LPSTR GetDBusName();
    bool IsDBusInit();

signals:
    void sig_EventCallback();

public:
    void WaitSyncEventCallback(qlonglong msg_id, qlonglong pointer);
    HRESULT GetWaitSyncResult(LPLFSRESULT *lppResult,REQUESTID requestID, UINT uMsg);

protected:
	//dwCommand 命令类型，hResult 执行结果，lpBuffer 执行结果指针
	virtual void  WR_OpenComplete(DWORD dwCommand, HRESULT hResult, LPVOID lpBuffer){}
	virtual void  WR_CloseComplete(DWORD dwCommand, HRESULT hResult, LPVOID lpBuffer){}
	virtual void  WR_LockComplete(DWORD dwCommand, HRESULT hResult, LPVOID lpBuffer){}
	virtual void  WR_UnlockComplete(DWORD dwCommand, HRESULT hResult, LPVOID lpBuffer){}
	virtual void  WR_RegisterComplete(DWORD dwCommand, HRESULT hResult, LPVOID lpBuffer){}
	virtual void  WR_DeregisterComplete(DWORD dwCommand, HRESULT hResult, LPVOID lpBuffer){}
	virtual void  WR_GetinfoComplete(DWORD dwCommand, HRESULT hResult, LPVOID lpBuffer){}
	virtual void  WR_ExecuteComplete(DWORD dwCommand, HRESULT hResult, LPVOID lpBuffer){}

	virtual void  WR_ExecuteEvent(DWORD dwEventID, HRESULT hResult, LPVOID lpBuffer){}
	virtual void  WR_UserEvent(DWORD dwEventID, HRESULT hResult, LPVOID lpBuffer){}
	virtual void  WR_ServiceEvent(DWORD dwEventID, HRESULT hResult, LPVOID lpBuffer){}
	virtual void  WR_SystemEvent(DWORD dwEventID, HRESULT hResult, LPVOID lpBuffer){}
public:
	//命令执行完成消息
	LRESULT  OnOpenComplete(WPARAM wParam, LPARAM lParam);			//打开完成事件
	LRESULT  OnCloseComplete(WPARAM wParam, LPARAM lParam);			//关闭完成事件
	LRESULT  OnLockComplete(WPARAM wParam, LPARAM lParam);			//上锁完成事件
	LRESULT  OnUnlockComplete(WPARAM wParam, LPARAM lParam);		//解锁完成事件
	LRESULT  OnRegisterComplete(WPARAM wParam, LPARAM lParam);		//注册完成事件
	LRESULT  OnDeregisterComplete(WPARAM wParam, LPARAM lParam);	//反注册完成事件
	LRESULT  OnGetinfoComplete(WPARAM wParam, LPARAM lParam);		//获取信息完成事件
	LRESULT  OnExecuteComplete(WPARAM wParam, LPARAM lParam);		//执行完成事件

	//事件消息
	LRESULT  OnExecuteEvent(WPARAM wParam, LPARAM lParam);			//执行事件
	LRESULT  OnUserEvent(WPARAM wParam, LPARAM lParam);				//用户事件
	LRESULT  OnServiceEvent(WPARAM wParam, LPARAM lParam);			//服务事件
	LRESULT  OnSystemEvent(WPARAM wParam, LPARAM lParam);			//系统事件

  //  static LRESULT MsgProc(UINT msg, WPARAM wParam, LPARAM lParam);
private:
    BOOL m_bDBusObjInited; //DBus对象是否成功注册
    char* m_pzDBusObjName; //注册的Dbus对象名
    qlonglong m_iWaitSyncMsgid;
    qlonglong m_pWaitSyncResultPointer;

    DbusListener* m_pAysncListener;

};

