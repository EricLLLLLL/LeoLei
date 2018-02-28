#include "apictrl.h"
#include <list>
#include "wswnd.h"
#include <pthread.h>
#include <QEventLoop>

#define CEN_TRACE_PATH		"C:\\DATA\\MCENTRACE\\"
#define CEN_TRACE_NAME		"MCENTRACE"
#define APICTRL_MUTEX		"APICTRL_MUTEX"
#define WAIT_DBUS_XX         "WAIT_DBUS_XX"

CApiCtrl::CApiCtrl(void) : m_hApp(LFS_DEFAULT_HAPP)
    , m_lpszAppID(NULL)
    , m_hService(NULL)
{

}


CApiCtrl::~CApiCtrl(void)
{
    m_hMutex.unlock();
}

void CApiCtrl::SetDBusName(LPSTR lpDBusName)
{
    m_lpDBusName = strdup(lpDBusName);
}


HRESULT CApiCtrl::LFSCancelAsyncRequest(REQUESTID RequestID)
{
	HRESULT hResult = 0;                                                                              	

	if (m_hService != NULL)
	{
        hResult = ::LFSCancelAsyncRequest(m_hService, RequestID);
	}
	else
	{
        hResult = LFS_ERR_INVALID_HSERVICE;
	}
	return hResult;
}


HRESULT CApiCtrl::LFSOpen(LPSTR lpszLogicalName, DWORD dwTimeOut)
{
	DWORD        dwTraceLevel, dwSrvcVersion;
    LFSVERSION   SrvcVersion , SPIVersion;
	HRESULT      hResult;

	dwSrvcVersion = 0x0003;
    //     dwTraceLevel = (LFS_TRACE_API | LFS_TRACE_ALL_API |
    //                      LFS_TRACE_SPI | LFS_TRACE_ALL_SPI |
    //                      LFS_TRACE_MGR);
	dwTraceLevel = NULL;

	REQUESTID id = 0;
    CWSWnd waitWnd;
    std::stringstream dbus_obj_name;
    dbus_obj_name<<WAIT_DBUS_XX<<getpid()<<pthread_self();
    if(waitWnd.CreateDBus(dbus_obj_name.str().c_str(),true))
    {
        m_hMutex.lock();
        hResult = ::LFSAsyncOpen(lpszLogicalName, m_hApp, m_lpszAppID, dwTraceLevel, dwTimeOut, \
            &m_hService, waitWnd.GetDBusName(), dwSrvcVersion, &SrvcVersion, &SPIVersion,&id);
        m_hMutex.unlock();

        if(hResult == LFS_SUCCESS)
	    {
            LPLFSRESULT lpResult = NULL;
            QEventLoop eventloop;
            connect(&waitWnd, &CWSWnd::sig_EventCallback, &eventloop, &QEventLoop::quit);
            eventloop.exec();
            hResult = waitWnd.GetWaitSyncResult(&lpResult,id,LFS_OPEN_COMPLETE);
            ::LFSFreeResult(lpResult);
		    lpResult = NULL;
	    }

        waitWnd.DestoryDBus();
    }
    else
    {
        hResult = LFS_ERR_INVALID_DBUS_OBJECT;
    }

	return hResult;
}


HRESULT CApiCtrl::LFSAsyncOpen(LPSTR lpszLogicalName, DWORD dwTimeOut, LPREQUESTID lpRequestID)
{
	DWORD        dwTraceLevel, dwSrvcVersion;
    LFSVERSION   SrvcVersion, SPIVersion;
	HRESULT      hResult;

	dwSrvcVersion = 0x0003;
    //     dwTraceLevel = (LFS_TRACE_API | LFS_TRACE_ALL_API |
    //                      LFS_TRACE_SPI | LFS_TRACE_ALL_SPI |
    //                      LFS_TRACE_MGR);
	dwTraceLevel = NULL;

    m_hMutex.lock();
    hResult = ::LFSAsyncOpen(lpszLogicalName,       //Points to a null-terminated string containing the pre-defined logical name of a service.
		m_hApp,					//The application handle to be associated with the session being opened
		m_lpszAppID,			//Points to a null-terminated string containing the application ID
		dwTraceLevel,          //See WFMSetTraceLevel. NULL turns off all tracing.
        dwTimeOut,			   //Number of milliseconds to wait for completion (LFS_INDEFINITE_WAIT to specify a request that will wait until completion).
		&m_hService,            //Pointer to the service handle that the XFS Manager assigns to the service on a successful open 
        m_lpDBusName,					//The window handle which is to receive the completion message for this request
		dwSrvcVersion,         //Specifies the range of versions of the service-specific interface that the application can support
		&SrvcVersion,          //Pointer to the data structure that is to receive version support information and other details about the service-specific interface implementation (returned parameter)
		&SPIVersion,           //This pointer may be NULL if the application is not interested in receiving this information
		lpRequestID);          //Pointer to the request identifier for this request (returned parameter)
    m_hMutex.unlock();

	return hResult;
}


HRESULT CApiCtrl::LFSClose()
{
	HRESULT hResult;

	if ( m_hService != NULL ) 
	{
        CWSWnd waitWnd;
        std::stringstream dbus_obj_name;
        dbus_obj_name<<WAIT_DBUS_XX<<getpid()<<pthread_self();
        if(waitWnd.CreateDBus(dbus_obj_name.str().c_str(),true))
        {
		    REQUESTID id = 0;
            m_hMutex.lock();
            hResult = ::LFSAsyncClose(m_hService, waitWnd.GetDBusName(), &id);
            m_hMutex.unlock();

            if(hResult == LFS_SUCCESS)
		    {
                LPLFSRESULT lpResult = NULL;
                QEventLoop eventloop;
                connect(&waitWnd, &CWSWnd::sig_EventCallback, &eventloop, &QEventLoop::quit);
                eventloop.exec();
                hResult = waitWnd.GetWaitSyncResult(&lpResult,id,LFS_CLOSE_COMPLETE);
                ::LFSFreeResult(lpResult);
                lpResult = NULL;
            }

            waitWnd.DestoryDBus();
	    }
        else
        {
            hResult = LFS_ERR_INVALID_DBUS_OBJECT;
        }
    }
    else 
    {
        hResult = LFS_ERR_INVALID_HSERVICE;
    }

	return hResult;
}


HRESULT CApiCtrl::LFSAsyncClose(LPREQUESTID lpRequestID)
{
	HRESULT hResult;

	if ( m_hService != NULL ) 
	{
        m_hMutex.lock();
        hResult = ::LFSAsyncClose(m_hService,m_lpDBusName, lpRequestID);
        m_hMutex.unlock();
	} 
	else 
	{
        hResult = LFS_ERR_INVALID_HSERVICE;
	}

	return hResult;
}


HRESULT CApiCtrl::LFSRegister(DWORD dwEventClass)
{
	HRESULT      hResult;

	REQUESTID id = 0;
    CWSWnd waitWnd;
    std::stringstream dbus_obj_name;
    dbus_obj_name<<WAIT_DBUS_XX<<getpid()<<pthread_self();
    if(waitWnd.CreateDBus(dbus_obj_name.str().c_str(),true))
    {
        m_hMutex.lock();
        hResult = ::LFSAsyncRegister(m_hService, dwEventClass, waitWnd.GetDBusName(), waitWnd.GetDBusName(), &id);
        m_hMutex.unlock();

        if(hResult == LFS_SUCCESS)
	    {
            LPLFSRESULT lpResult = NULL;
            QEventLoop eventloop;
            connect(&waitWnd, &CWSWnd::sig_EventCallback, &eventloop, &QEventLoop::quit);
            eventloop.exec();
            hResult = waitWnd.GetWaitSyncResult(&lpResult,id,LFS_REGISTER_COMPLETE);
            ::LFSFreeResult(lpResult);
            lpResult = NULL;
	    }

        waitWnd.DestoryDBus();
    }
    else
    {
        hResult = LFS_ERR_INVALID_DBUS_OBJECT;
    }

	return hResult;
}


HRESULT CApiCtrl::LFSAsyncRegister(DWORD dwEventClass, LPREQUESTID lpRequestID)
{
	HRESULT hResult;

	if ( m_hService != NULL ) 
	{
        m_hMutex.lock();
        hResult = ::LFSAsyncRegister(m_hService, dwEventClass, m_lpDBusName, m_lpDBusName, lpRequestID);
        m_hMutex.unlock();
	} 
	else 
	{
        hResult = LFS_ERR_INVALID_HSERVICE;
	}

	return hResult;
}


HRESULT CApiCtrl::LFSDeregister(DWORD dwEventClass)
{
	HRESULT hResult;

	if ( m_hService != NULL ) 
	{
        CWSWnd waitWnd;
        std::stringstream dbus_obj_name;
        dbus_obj_name<<WAIT_DBUS_XX<<getpid()<<pthread_self();
        if(waitWnd.CreateDBus(dbus_obj_name.str().c_str(),true))
        {
		    REQUESTID id = 0;
            m_hMutex.lock();
            hResult = ::LFSAsyncDeregister(m_hService, dwEventClass, waitWnd.GetDBusName(), waitWnd.GetDBusName(), &id);
            m_hMutex.unlock();
            if(hResult == LFS_SUCCESS)
		    {
                LPLFSRESULT lpResult = NULL;
                QEventLoop eventloop;
                connect(&waitWnd, &CWSWnd::sig_EventCallback, &eventloop, &QEventLoop::quit);
                eventloop.exec();
                hResult = waitWnd.GetWaitSyncResult(&lpResult,id,LFS_DEREGISTER_COMPLETE);
                ::LFSFreeResult(lpResult);
                lpResult = NULL;
		    }

            waitWnd.DestoryDBus();
        }
        else
        {
            hResult = LFS_ERR_INVALID_DBUS_OBJECT;
        }
	} 
	else 
	{
        hResult = LFS_ERR_INVALID_HSERVICE;
	}

	return hResult;
}


HRESULT CApiCtrl::LFSAsyncDeregister(DWORD dwEventClass, LPREQUESTID lpRequestID)
{
	HRESULT hResult;

	if ( m_hService != NULL ) 
	{
        m_hMutex.lock();
        hResult = ::LFSAsyncDeregister(m_hService, dwEventClass, m_lpDBusName, m_lpDBusName, lpRequestID);
        m_hMutex.unlock();
	} 
	else 
	{
        hResult = LFS_ERR_INVALID_HSERVICE;
	}

	return hResult;
}


HRESULT CApiCtrl::LFSGetInfo(DWORD dwCategory, LPVOID lpQueryDetails, DWORD dwTimeOut, LPLFSRESULT *lppResult)
{
	HRESULT    hResult;
	REQUESTID id = 0;

	if ( m_hService != NULL ) 
	{
        CWSWnd waitWnd;
        std::stringstream dbus_obj_name;
        dbus_obj_name<<WAIT_DBUS_XX<<getpid()<<pthread_self();
        if(waitWnd.CreateDBus(dbus_obj_name.str().c_str(),true))
        {
            m_hMutex.lock();
            hResult = ::LFSAsyncGetInfo(m_hService, dwCategory, lpQueryDetails, dwTimeOut, waitWnd.GetDBusName(), &id);
            m_hMutex.unlock();

            if(hResult == LFS_SUCCESS)
            {
                QEventLoop eventloop;
                connect(&waitWnd, &CWSWnd::sig_EventCallback, &eventloop, &QEventLoop::quit);
                eventloop.exec();
                hResult = waitWnd.GetWaitSyncResult(lppResult,id,LFS_GETINFO_COMPLETE);
            }

            waitWnd.DestoryDBus();
        }
        else
        {
            hResult = LFS_ERR_INVALID_DBUS_OBJECT;
        }
	} 
	else 
	{
        hResult = LFS_ERR_INVALID_HSERVICE;
	}

	return hResult;
}


HRESULT CApiCtrl::LFSAsyncGetInfo(DWORD dwCategory, LPVOID lpQueryDetails, DWORD dwTimeOut, LPREQUESTID lpRequestID)
{
	HRESULT    hResult;

	if ( m_hService != NULL ) 
	{
        m_hMutex.lock();
        hResult = ::LFSAsyncGetInfo(m_hService,      //Handle to the Service Provider as returned by LFSOpen or LFSAsyncOpen
            dwCategory,			//Specifies the category of the query (e.g. for a printer, LFS_INF_PTR_STATUS to request status or LFS_INF_PTR_CAPABILITIES to request capabilities)
			lpQueryDetails,		//Pointer to the data structure to be passed to the Service Provider
            dwTimeOut,			//Number of milliseconds to wait for completion (LFS_INDEFINITE_WAIT to specify a request that will wait until completion)
            m_lpDBusName,				//The window handle which is to receive the completion message for this request
			lpRequestID);		//The request identifier for this request (returned parameter)
        m_hMutex.unlock();
	} 
	else 
	{
        hResult = LFS_ERR_INVALID_HSERVICE;
	}

	return hResult;
}


HRESULT CApiCtrl::LFSExecute(DWORD dwCommand, LPVOID lpCmdData,DWORD dwTimeOut, LPLFSRESULT *lppResult)
{
	HRESULT    hResult;
	REQUESTID id = 0;

	if ( m_hService != NULL ) 
	{
        CWSWnd waitWnd;
        std::stringstream dbus_obj_name;
        dbus_obj_name<<WAIT_DBUS_XX<<getpid()<<pthread_self();
        if(waitWnd.CreateDBus(dbus_obj_name.str().c_str(),true))
        {
            m_hMutex.lock();
            hResult = ::LFSAsyncExecute(m_hService, dwCommand, lpCmdData, dwTimeOut, waitWnd.GetDBusName(), &id);
            m_hMutex.unlock();

            if(hResult == LFS_SUCCESS)
		    {
                QEventLoop eventloop;
                connect(&waitWnd, &CWSWnd::sig_EventCallback, &eventloop, &QEventLoop::quit);
                eventloop.exec();
                hResult = waitWnd.GetWaitSyncResult(lppResult,id,LFS_EXECUTE_COMPLETE);
		    }

            waitWnd.DestoryDBus();
        }
        else
        {
            hResult = LFS_ERR_INVALID_DBUS_OBJECT;
        }
	} 
	else 
	{
        hResult = LFS_ERR_INVALID_HSERVICE;
	}

	return hResult;
}


HRESULT CApiCtrl::LFSAsyncExecute(DWORD dwCommand, LPVOID lpCmdData, DWORD dwTimeOut, LPREQUESTID lpRequestID)
{
	HRESULT hResult;

	if ( m_hService != NULL ) 
	{
        m_hMutex.lock();
        hResult = ::LFSAsyncExecute(m_hService,      //Handle to the service as returned by LFSOpen or LFSAsyncOpen.
			dwCommand,       //Command to be executed by the Service Provider.
			lpCmdData,       //Pointer to the data structure to be passed to the Service Provider
            dwTimeOut,       //Number of milliseconds to wait for completion (LFS_INDEFINITE_WAIT to specify a request that will wait until completion)
            m_lpDBusName,			 //The window handle which is to receive the completion message for this request
			lpRequestID);    //Pointer to the request identifier for this request (returned parameter)
        m_hMutex.unlock();
	} 
	else 
	{
        hResult = LFS_ERR_INVALID_HSERVICE;
	}

	return hResult;
}

