#include "wswnd.h"
#include <QDBusConnection>
#include <QDebug>

DbusListener::DbusListener(QObject * parent,bool bWaitSync)
    :QDBusAbstractAdaptor(parent)
{
    m_bWaitSync = bWaitSync;
    m_pParent = dynamic_cast<CWSWnd *>(parent);
}

void DbusListener::event_callback(qlonglong msg_id, qlonglong pointer)
{
    //qDebug()<< "event:" << QS(lfs_msg(msg_id)) << pointer;
    qDebug()<< "event:" <<msg_id << pointer;
    if(NULL == m_pParent)
        return;

    if(m_bWaitSync)
    {
        m_pParent->WaitSyncEventCallback(msg_id,pointer);
        return;
    }

    switch(msg_id) {
    case LFS_OPEN_COMPLETE:
        m_pParent->OnOpenComplete(msg_id,pointer);
        break;
    case LFS_CLOSE_COMPLETE:
        m_pParent->OnCloseComplete(msg_id,pointer);
        break;
    case LFS_LOCK_COMPLETE:
        m_pParent->OnLockComplete(msg_id,pointer);
        break;
    case LFS_UNLOCK_COMPLETE:
        m_pParent->OnUnlockComplete(msg_id,pointer);
        break;
    case LFS_REGISTER_COMPLETE:
        m_pParent->OnRegisterComplete(msg_id,pointer);
        break;
    case LFS_DEREGISTER_COMPLETE:
        m_pParent->OnDeregisterComplete(msg_id, pointer);
        break;
    case LFS_GETINFO_COMPLETE:
        m_pParent->OnGetinfoComplete(msg_id, pointer);
        break;
    case LFS_EXECUTE_COMPLETE:
        m_pParent->OnExecuteComplete(msg_id, pointer);
        break;
    case LFS_EXECUTE_EVENT:
        m_pParent->OnExecuteEvent(msg_id, pointer);
        break;
    case LFS_SERVICE_EVENT:
        m_pParent->OnServiceEvent(msg_id, pointer);
        break;
    case LFS_USER_EVENT:
        m_pParent->OnUserEvent(msg_id, pointer);
        break;
    case LFS_SYSTEM_EVENT:
        m_pParent->OnSystemEvent(msg_id, pointer);
        break;
    }
}

CWSWnd::CWSWnd() : m_bDBusObjInited(FALSE)
{
    m_pzDBusObjName = NULL;
    m_iWaitSyncMsgid = 0;
    m_pWaitSyncResultPointer = 0;
    m_pAysncListener = NULL;
}

CWSWnd::~CWSWnd()
{
    SAFE_DELETE(m_pAysncListener);
    free(m_pzDBusObjName);
}

bool CWSWnd::InitDBusService()
{
    QDBusConnection sess_bus = QDBusConnection::sessionBus();
    if (!sess_bus.isConnected())
    {
        qDebug()<< "connect to d-bus failed";
        return false;
    }

    std::stringstream dbus_name;
    dbus_name << LFS_MGR_DBUS_NAME_PREF << ".p" << getpid();

    if (!sess_bus.registerService(dbus_name.str().c_str()))
    {
        qDebug()<< "register service failed";
        return false;
    }

    int version_required = 0x10001;
    LFSVERSION v;
    HRESULT hr = LFSStartUp(version_required, &v);
    if(hr != S_OK)
    {
        qDebug()<<"LFSStartUp调用失败， 返回"<<hr;
        return false;
    }
    qDebug() << "startUp version: " << v.version << v.low_version << v.high_version;

    return true;
}

bool CWSWnd::DestroyDBusService()
{
    std::stringstream dbus_name;
    dbus_name << LFS_MGR_DBUS_NAME_PREF << ".p" << getpid();

    if (!QDBusConnection::sessionBus().unregisterService(dbus_name.str().c_str()))
    {
        qDebug()<< "unregister service failed";
        return false;
    }


    HRESULT hr = LFSCleanUp();
    if (hr != S_OK)
    {
        qDebug("LFSCleanUp失败，返回%d", hr);
        return false;
    }

    return true;
}

bool CWSWnd::CreateDBus(const char* pzDBusObjName,bool bWaitSync)
{
    m_pzDBusObjName = strdup(pzDBusObjName);

    QDBusConnection sess_bus = QDBusConnection::sessionBus();
    if (!sess_bus.isConnected())
    {
        qDebug() << "connect to d-bus failed";
        return false;
    }

    m_pAysncListener = new DbusListener(this);
    bool bSuc = sess_bus.registerObject(m_pzDBusObjName, this);
    if(!bSuc)
    {
        qDebug() << "register dbus obj failed";
        return false;
    }

    m_bDBusObjInited = true;
    return true;
}

void CWSWnd::DestoryDBus()
{
    QDBusConnection::sessionBus().unregisterObject(m_pzDBusObjName);
    SAFE_DELETE(m_pAysncListener);
    free(m_pzDBusObjName);
    m_pzDBusObjName = NULL;
    m_bDBusObjInited = false;
}

LPSTR CWSWnd::GetDBusName()
{
    return m_pzDBusObjName;
}

bool CWSWnd::IsDBusInit()
{
    return m_bDBusObjInited;
}

void CWSWnd::WaitSyncEventCallback(qlonglong msg_id, qlonglong pointer)
{
    m_iWaitSyncMsgid = msg_id;
    m_pWaitSyncResultPointer = pointer;
    emit sig_EventCallback();
}

HRESULT CWSWnd::GetWaitSyncResult(LPLFSRESULT *lppResult, REQUESTID requestID, UINT uMsg)
{
    HRESULT ret = LFS_SUCCESS;
    LPLFSRESULT        lpResult = NULL;
    lpResult  = (LPLFSRESULT)m_pWaitSyncResultPointer;

    if(lpResult && m_iWaitSyncMsgid == uMsg && lpResult->request_id == requestID)
    {
        *lppResult = lpResult;
        ret = lpResult->result;
    }

    m_iWaitSyncMsgid = 0;
    m_pWaitSyncResultPointer = 0;
    return ret;
}

LRESULT CWSWnd::OnOpenComplete(WPARAM wParam, LPARAM lParam)
{
    LPLFSRESULT        lpResult = NULL;
	DWORD              dwCommand = 0;

	if (lParam == NULL)
	{
        WR_OpenComplete(dwCommand, LFS_ERR_INTERNAL_ERROR, NULL);
	} 
	else
	{
        lpResult  = (LPLFSRESULT)lParam;
        dwCommand = lpResult->u.command_code;

        WR_OpenComplete(dwCommand, lpResult->result, lpResult);

        LFSFreeResult(lpResult);
	}

	return 0L;
}

LRESULT CWSWnd::OnCloseComplete(WPARAM wParam, LPARAM lParam)
{
    LPLFSRESULT        lpResult = NULL;
	DWORD              dwCommand = 0;

	if (lParam == NULL)
	{
        WR_CloseComplete(dwCommand, LFS_ERR_INTERNAL_ERROR, NULL);
	} 
	else
	{
        lpResult  = (LPLFSRESULT)lParam;
        dwCommand = lpResult->u.command_code;

        WR_CloseComplete(dwCommand, lpResult->result, lpResult);

        LFSFreeResult(lpResult);
	}

	return 0L;
}

LRESULT CWSWnd::OnLockComplete(WPARAM wParam, LPARAM lParam)
{
    LPLFSRESULT        lpResult = NULL;
	DWORD              dwCommand = 0;

	if (lParam == NULL)
	{
        WR_LockComplete(dwCommand, LFS_ERR_INTERNAL_ERROR, NULL);
	} 
	else
	{
        lpResult  = (LPLFSRESULT)lParam;
        dwCommand = lpResult->u.command_code;

        WR_LockComplete(dwCommand, lpResult->result, lpResult);

        LFSFreeResult(lpResult);
	}

	return 0L;
}

LRESULT CWSWnd::OnUnlockComplete(WPARAM wParam, LPARAM lParam)
{
    LPLFSRESULT        lpResult = NULL;
	DWORD              dwCommand = 0;

	if (lParam == NULL)
	{
        WR_UnlockComplete(dwCommand, LFS_ERR_INTERNAL_ERROR, NULL);
	} 
	else
	{
        lpResult  = (LPLFSRESULT)lParam;
        dwCommand = lpResult->u.command_code;

        WR_UnlockComplete(dwCommand, lpResult->result, lpResult);

        LFSFreeResult(lpResult);
	}

	return 0L;
}

LRESULT CWSWnd::OnRegisterComplete(WPARAM wParam, LPARAM lParam)
{
    LPLFSRESULT        lpResult = NULL;
	DWORD              dwCommand = 0;

	if (lParam == NULL)
	{
        WR_RegisterComplete(dwCommand, LFS_ERR_INTERNAL_ERROR, NULL);
	} 
	else
	{
        lpResult  = (LPLFSRESULT)lParam;
        dwCommand = lpResult->u.command_code;

        WR_RegisterComplete(dwCommand, lpResult->result, lpResult);

        LFSFreeResult(lpResult);
	}

	return 0L;
}

LRESULT CWSWnd::OnDeregisterComplete(WPARAM wParam, LPARAM lParam)
{
    LPLFSRESULT        lpResult = NULL;
	DWORD              dwCommand = 0;

	if (lParam == NULL)
	{
        WR_DeregisterComplete(dwCommand, LFS_ERR_INTERNAL_ERROR, NULL);
	} 
	else
	{
        lpResult  = (LPLFSRESULT)lParam;
        dwCommand = lpResult->u.command_code;

        WR_DeregisterComplete(dwCommand, lpResult->result, lpResult);

        LFSFreeResult(lpResult);
	}

	return 0L;
}

LRESULT CWSWnd::OnGetinfoComplete(WPARAM wParam, LPARAM lParam)
{
    LPLFSRESULT        lpResult = NULL;
	DWORD              dwCommand = 0;

	if (lParam == NULL)
	{
        WR_GetinfoComplete(dwCommand, LFS_ERR_INTERNAL_ERROR, NULL);
	} 
	else
	{
        lpResult  = (LPLFSRESULT)lParam;
        dwCommand = lpResult->u.command_code;

        WR_GetinfoComplete(dwCommand, lpResult->result, lpResult);

        LFSFreeResult(lpResult);
	}

	return 0L;
}

LRESULT CWSWnd::OnExecuteComplete(WPARAM wParam, LPARAM lParam)
{
    LPLFSRESULT        lpResult = NULL;
	DWORD              dwCommand = 0;

	if (lParam == NULL)
	{
        WR_ExecuteComplete(dwCommand, LFS_ERR_INTERNAL_ERROR, NULL);
	} 
	else
	{
        lpResult  = (LPLFSRESULT)lParam;
        dwCommand = lpResult->u.command_code;

        WR_ExecuteComplete(dwCommand, lpResult->result, lpResult);

        LFSFreeResult(lpResult);
	}

	return 0L;
}

LRESULT CWSWnd::OnExecuteEvent(WPARAM wParam, LPARAM lParam)
{
    LPLFSRESULT        lpResult = NULL;
    DWORD              event_id = 0;

	if (lParam == NULL)
	{
        WR_ExecuteEvent(event_id, LFS_ERR_INTERNAL_ERROR, NULL);
	} 
	else
	{
        lpResult  = (LPLFSRESULT)lParam;
        event_id = lpResult->u.event_id;

        WR_ExecuteEvent(event_id, lpResult->result, lpResult);

        LFSFreeResult(lpResult);
	}

	return 0L;
}

LRESULT CWSWnd::OnUserEvent(WPARAM wParam, LPARAM lParam)
{
    LPLFSRESULT        lpResult = NULL;
    DWORD              event_id = 0;

	if (lParam == NULL)
	{
        WR_UserEvent(event_id, LFS_ERR_INTERNAL_ERROR, NULL);
	} 
	else
	{
        lpResult  = (LPLFSRESULT)lParam;
        event_id = lpResult->u.event_id;

        WR_UserEvent(event_id, lpResult->result, lpResult);

        LFSFreeResult(lpResult);
	}

	return 0L;
}

LRESULT CWSWnd::OnServiceEvent(WPARAM wParam, LPARAM lParam)
{
    LPLFSRESULT        lpResult = NULL;
    DWORD              event_id = 0;

	if (lParam == NULL)
	{
        WR_ServiceEvent(event_id, LFS_ERR_INTERNAL_ERROR, NULL);
	} 
	else
	{
        lpResult  = (LPLFSRESULT)lParam;
        event_id = lpResult->u.event_id;

        WR_ServiceEvent(event_id, lpResult->result, lpResult);

        LFSFreeResult(lpResult);
	}

	return 0L;
}

LRESULT CWSWnd::OnSystemEvent(WPARAM wParam, LPARAM lParam)
{
    LPLFSRESULT        lpResult = NULL;
    DWORD              event_id = 0;

	if (lParam == NULL)
	{
        WR_SystemEvent(event_id, LFS_ERR_INTERNAL_ERROR, NULL);
	} 
	else
	{
        lpResult  = (LPLFSRESULT)lParam;
        event_id = lpResult->u.event_id;

        WR_SystemEvent(event_id, lpResult->result, lpResult);

        LFSFreeResult(lpResult);
	}

    return 0L;
}

/*
LRESULT CWSWnd::MsgProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
    std::map<HWND, CWSWnd *>::iterator it = m_hwnd_object_map.find(hWnd);
    if(it == m_hwnd_object_map.end())
    {
        return ::DefWindowProc(hWnd, msg, wParam, lParam);
    }

    CWSWnd * pWSWnd = it->second;

	switch ( msg )
	{
	case WFS_OPEN_COMPLETE:
		{
            CMCenTrace::WriteResultTrace(msg, (LPLFSRESULT)lParam);
			pWSWnd->OnOpenComplete(wParam, lParam);
		}
		break;
	case WFS_CLOSE_COMPLETE:
		{
            CMCenTrace::WriteResultTrace(msg, (LPLFSRESULT)lParam);
			pWSWnd->OnCloseComplete(wParam, lParam);
		}
		break;
	case WFS_LOCK_COMPLETE:
		{
            CMCenTrace::WriteResultTrace(msg, (LPLFSRESULT)lParam);
			pWSWnd->OnLockComplete(wParam, lParam);
		}
		break;
	case WFS_UNLOCK_COMPLETE:
		{
            CMCenTrace::WriteResultTrace(msg, (LPLFSRESULT)lParam);
			pWSWnd->OnUnlockComplete(wParam, lParam);
		}
		break;
	case WFS_REGISTER_COMPLETE:
		{
            CMCenTrace::WriteResultTrace(msg, (LPLFSRESULT)lParam);
			pWSWnd->OnRegisterComplete(wParam, lParam);
		}
		break;
	case WFS_DEREGISTER_COMPLETE:
		{
            CMCenTrace::WriteResultTrace(msg, (LPLFSRESULT)lParam);
			pWSWnd->OnDeregisterComplete(wParam, lParam);
		}
		break;
	case WFS_GETINFO_COMPLETE:
		{
            CMCenTrace::WriteResultTrace(msg, (LPLFSRESULT)lParam);
			pWSWnd->OnGetinfoComplete(wParam, lParam);
		}
		break;
	case WFS_EXECUTE_COMPLETE:
		{
            CMCenTrace::WriteResultTrace(msg, (LPLFSRESULT)lParam);
			pWSWnd->OnExecuteComplete(wParam, lParam);
		}
		break;
	case WFS_EXECUTE_EVENT:
		{
            CMCenTrace::WriteResultTrace(msg, (LPLFSRESULT)lParam);
			pWSWnd->OnExecuteEvent(wParam, lParam);
		}
		break;
	case WFS_SERVICE_EVENT:
		{
            CMCenTrace::WriteResultTrace(msg, (LPLFSRESULT)lParam);
			pWSWnd->OnServiceEvent(wParam, lParam);
		}
		break;
	case WFS_USER_EVENT:
		{
            CMCenTrace::WriteResultTrace(msg, (LPLFSRESULT)lParam);
			pWSWnd->OnUserEvent(wParam, lParam);
		}
		break;
	case WFS_SYSTEM_EVENT:
		{
            CMCenTrace::WriteResultTrace(msg, (LPLFSRESULT)lParam);
			pWSWnd->OnSystemEvent(wParam, lParam);
		}
		break;
	default:
		return ::DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return 0;
}

*/
