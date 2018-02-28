/************************************************************************
*                                                                       *
* lfsapi.h      XFS - API functions, types, and definitions             *
*                                                                       *
*               Version 1.00  --  (May 10 2017)                         *
*                                                                       *
************************************************************************/

#ifndef __inc_lfsapi__h
#define __inc_lfsapi__h

#ifdef __cplusplus
extern "C" {
#endif

/*   be aware of alignment   */
#pragma pack(push,1)

/****** types *****************************************/

#include <sys/types.h>
#include <pthread.h>

typedef void * HANDLE;
typedef void * LPVOID;
typedef char * LPSTR;
typedef unsigned int DWORD;
typedef DWORD* LPDWORD;
typedef unsigned short WORD;
typedef WORD* LPWORD;
typedef unsigned char BYTE;
typedef BYTE* LPBYTE;
typedef long LONG;
typedef int BOOL;

typedef struct _SYSTEMTIME {
    WORD year;
    WORD month;
    WORD day_of_week;
    WORD day;
    WORD hour;
    WORD minute;
    WORD second;
    WORD milli_seconds;
} SYSTEMTIME, *PSYSTEMTIME;

#define LFSAPI              

/****** d-bus && DM_USER ****************************************/

#define LFS_MGR_DBUS_NAME_PREF			"org.lfs.manager"
#define LFS_MGR_DBUS_INTF_NAME			"org.lfs.windows_compatible_intf"
#define LFS_MGR_DBUS_METHOD_NAME		"event_callback"

#define DM_USER                                 (0x0400)

#define TRUE                                     (1)
#define FALSE                                    (0)



/****** common *********************************************************/

typedef unsigned short USHORT;
typedef char CHAR;
typedef short SHORT;
typedef unsigned long ULONG;
typedef unsigned char UCHAR;
typedef SHORT * LPSHORT;
typedef LPVOID * LPLPVOID;
typedef ULONG * LPULONG;
typedef USHORT * LPUSHORT;

typedef HANDLE HPROVIDER;

typedef ULONG REQUESTID;
typedef REQUESTID * LPREQUESTID;

typedef HANDLE HAPP;
typedef HAPP * LPHAPP;

typedef USHORT HSERVICE;
typedef HSERVICE * LPHSERVICE;

typedef LONG HRESULT;
typedef HRESULT * LPHRESULT;

typedef BOOL (LFSAPI * LFSBLOCKINGHOOK)();
typedef LFSBLOCKINGHOOK * LPLFSBLOCKINGHOOK;


/****** String lengths ****************************************************/

#define LFSDDESCRIPTION_LEN                     256
#define LFSDSYSSTATUS_LEN                       256

/****** Values of LFSDEVSTATUS.state *****************************************/

#define LFS_STAT_DEVONLINE                      (0)
#define LFS_STAT_DEVOFFLINE                     (1)
#define LFS_STAT_DEVPOWEROFF                    (2)
#define LFS_STAT_DEVNODEVICE                    (3)
#define LFS_STAT_DEVHWERROR                     (4)
#define LFS_STAT_DEVUSERERROR                   (5)
#define LFS_STAT_DEVBUSY                        (6)

/****** Value of LFS_DEFAULT_HAPP ********************************************/

#define LFS_DEFAULT_HAPP                        (0)

/****** Data Structures  *****************************************************/

typedef struct _lfs_result
{
    REQUESTID       request_id;
    HSERVICE        service;
    SYSTEMTIME      timestamp;
    HRESULT         result;
    union {
        DWORD       command_code;
        DWORD       event_id;
    } u;
    LPVOID          buffer;
} LFSRESULT, * LPLFSRESULT;

typedef struct _lfsversion
{
    WORD            version;
    WORD            low_version;
    WORD            high_version;
    CHAR            description[LFSDDESCRIPTION_LEN+1];
    CHAR            system_status[LFSDSYSSTATUS_LEN+1];
} LFSVERSION, * LPLFSVERSION;

/****** Message Structures ***************************************************/

typedef struct _lfs_devstatus
{
    LPSTR           physical_name;
    LPSTR           workstation_name;
    DWORD           state;
} LFSDEVSTATUS, * LPLFSDEVSTATUS;
  
typedef struct _lfs_undevmsg
{
    LPSTR           logical_name;
    LPSTR           workstation_name;
    LPSTR           app_id;
    DWORD           size;
    LPBYTE          description;
    DWORD           msg;
    LPLFSRESULT     lfs_result;
} LFSUNDEVMSG, * LPLFSUNDEVMSG;  

typedef struct _lfs_appdisc
{
    LPSTR           logical_name;
    LPSTR           workstation_name;
    LPSTR           app_id;
} LFSAPPDISC, * LPLFSAPPDISC;
                    
typedef struct _lfs_hwerror
{
    LPSTR           logical_name;
    LPSTR           physical_name;
    LPSTR           workstation_name;
    LPSTR           app_id;
    DWORD           action;
    DWORD           size;
    LPBYTE          description;
} LFSHWERROR, * LPLFSHWERROR;

typedef struct _lfs_vrsnerror
{
    LPSTR           logical_name;
    LPSTR           workstation_name;
    LPSTR           app_id;
    DWORD           size;
    LPBYTE          description;
    LPLFSVERSION    lfs_version;
} LFSVRSNERROR, * LPLFSVRSNERROR;

/****** Error codes ************************************************************/

#define LFS_SUCCESS                             (0)
#define LFS_ERR_ALREADY_STARTED                 (-1)
#define LFS_ERR_API_VER_TOO_HIGH                (-2)
#define LFS_ERR_API_VER_TOO_LOW                 (-3)
#define LFS_ERR_CANCELED                        (-4)
#define LFS_ERR_DEV_NOT_READY                   (-13)
#define LFS_ERR_HARDWARE_ERROR                  (-14)
#define LFS_ERR_INTERNAL_ERROR                  (-15)
#define LFS_ERR_INVALID_ADDRESS                 (-16)
#define LFS_ERR_INVALID_APP_HANDLE              (-17)
#define LFS_ERR_INVALID_BUFFER                  (-18)
#define LFS_ERR_INVALID_CATEGORY                (-19)
#define LFS_ERR_INVALID_COMMAND                 (-20)
#define LFS_ERR_INVALID_EVENT_CLASS             (-21)
#define LFS_ERR_INVALID_HSERVICE                (-22)
#define LFS_ERR_INVALID_HPROVIDER               (-23)
#define LFS_ERR_INVALID_DBUS_OBJECT             (-24)
#define LFS_ERR_INVALID_DBUS_REG_OBJECT         (-25)
#define LFS_ERR_INVALID_POINTER                 (-26)
#define LFS_ERR_INVALID_REQ_ID                  (-27)
#define LFS_ERR_INVALID_RESULT                  (-28)
#define LFS_ERR_INVALID_SERVPROV                (-29)
#define LFS_ERR_INVALID_TIMER                   (-30)
#define LFS_ERR_INVALID_TRACELEVEL              (-31)
#define LFS_ERR_LOCKED                          (-32)
#define LFS_ERR_NO_BLOCKING_CALL                (-33)
#define LFS_ERR_NO_SERVPROV                     (-34)
#define LFS_ERR_NO_SUCH_THREAD                  (-35)
#define LFS_ERR_NO_TIMER                        (-36)
#define LFS_ERR_NOT_LOCKED                      (-37)
#define LFS_ERR_NOT_OK_TO_UNLOAD                (-38)
#define LFS_ERR_NOT_STARTED                     (-39)
#define LFS_ERR_NOT_REGISTERED                  (-40)
#define LFS_ERR_OP_IN_PROGRESS                  (-41)
#define LFS_ERR_OUT_OF_MEMORY                   (-42)
#define LFS_ERR_SERVICE_NOT_FOUND               (-43)
#define LFS_ERR_SPI_VER_TOO_HIGH                (-44)
#define LFS_ERR_SPI_VER_TOO_LOW                 (-45)
#define LFS_ERR_SRVC_VER_TOO_HIGH               (-46)
#define LFS_ERR_SRVC_VER_TOO_LOW                (-47)
#define LFS_ERR_TIMEOUT                         (-48)
#define LFS_ERR_UNSUPP_CATEGORY                 (-49)
#define LFS_ERR_UNSUPP_COMMAND                  (-50)
#define LFS_ERR_VERSION_ERROR_IN_SRVC           (-51)
#define LFS_ERR_INVALID_DATA                    (-52)
#define LFS_ERR_SOFTWARE_ERROR                  (-53)
#define LFS_ERR_CONNECTION_LOST                 (-54)
#define LFS_ERR_USER_ERROR                      (-55)
#define LFS_ERR_UNSUPP_DATA                     (-56)

#define LFS_INDEFINITE_WAIT                     0

/****** Messages **********************************************************/

#define LFS_OPEN_COMPLETE                       (DM_USER + 1)
#define LFS_CLOSE_COMPLETE                      (DM_USER + 2)
#define LFS_LOCK_COMPLETE                       (DM_USER + 3)
#define LFS_UNLOCK_COMPLETE                     (DM_USER + 4)
#define LFS_REGISTER_COMPLETE                   (DM_USER + 5)
#define LFS_DEREGISTER_COMPLETE                 (DM_USER + 6)
#define LFS_GETINFO_COMPLETE                    (DM_USER + 7)
#define LFS_EXECUTE_COMPLETE                    (DM_USER + 8)

#define LFS_EXECUTE_EVENT                       (DM_USER + 20)
#define LFS_SERVICE_EVENT                       (DM_USER + 21)
#define LFS_USER_EVENT                          (DM_USER + 22)
#define LFS_SYSTEM_EVENT                        (DM_USER + 23)

#define LFS_TIMER_EVENT                         (DM_USER + 100)

/****** Event Classes **********************************************************/

#define SERVICE_EVENTS                          (1)
#define USER_EVENTS                             (2)
#define SYSTEM_EVENTS                           (4)
#define EXECUTE_EVENTS                          (8)

/****** System Event IDs ********************************************************/

#define LFS_SYSE_UNDELIVERABLE_MSG              (1)
#define LFS_SYSE_HARDWARE_ERROR                 (2)
#define LFS_SYSE_VERSION_ERROR                  (3)
#define LFS_SYSE_DEVICE_STATUS                  (4)
#define LFS_SYSE_APP_DISCONNECT                 (5)
#define LFS_SYSE_SOFTWARE_ERROR                 (6)
#define LFS_SYSE_USER_ERROR                     (7)
#define LFS_SYSE_LOCK_REQUESTED                 (8)

/****** LFS Trace Level ******************************************************/

#define LFS_TRACE_API                           0x00000001
#define LFS_TRACE_ALL_API                       0x00000002
#define LFS_TRACE_SPI                           0x00000004
#define LFS_TRACE_ALL_SPI                       0x00000008
#define LFS_TRACE_MGR                           0x00000010

/****** LFS Error Actions *********************************************/

#define LFS_ERR_ACT_NOACTION                    (0x0000)
#define LFS_ERR_ACT_RESET                       (0x0001)
#define LFS_ERR_ACT_SWERROR                     (0x0002)
#define LFS_ERR_ACT_CONFIG                      (0x0004)
#define LFS_ERR_ACT_HWCLEAR                     (0x0008)
#define LFS_ERR_ACT_HWMAINT                     (0x0010)
#define LFS_ERR_ACT_SUSPEND                     (0x0020)

/****** API functions **********************************************************/

HRESULT extern LFSAPI LFSCancelAsyncRequest (HSERVICE service, REQUESTID request_id);

HRESULT extern LFSAPI LFSCancelBlockingCall (pthread_t thread_id);

HRESULT extern LFSAPI LFSCleanUp ();

HRESULT extern LFSAPI LFSClose (HSERVICE service);

HRESULT extern LFSAPI LFSAsyncClose (HSERVICE service, LPSTR object_name, LPREQUESTID request_id);

HRESULT extern LFSAPI LFSCreateAppHandle (LPHAPP app_handle);

HRESULT extern LFSAPI LFSDeregister (HSERVICE service, DWORD event_class, LPSTR object_reg_name);

HRESULT extern LFSAPI LFSAsyncDeregister (HSERVICE service, DWORD event_class, LPSTR object_reg_name, LPSTR object_name, LPREQUESTID request_id);

HRESULT extern LFSAPI LFSDestroyAppHandle (HAPP app_handle);

HRESULT extern LFSAPI LFSExecute (HSERVICE service, DWORD command, LPVOID cmd_data, DWORD timeout, LPLFSRESULT * result);

HRESULT extern LFSAPI LFSAsyncExecute (HSERVICE service, DWORD command, LPVOID cmd_data, DWORD timeout, LPSTR object_name, LPREQUESTID request_id);

HRESULT extern LFSAPI LFSFreeResult (LPLFSRESULT result);

HRESULT extern LFSAPI LFSGetInfo (HSERVICE service, DWORD category, LPVOID query_details, DWORD timeout, LPLFSRESULT * result);

HRESULT extern LFSAPI LFSAsyncGetInfo (HSERVICE service, DWORD category, LPVOID query_details, DWORD timeout, LPSTR object_name, LPREQUESTID request_id);

BOOL extern LFSAPI LFSIsBlocking ();

HRESULT extern LFSAPI LFSLock (HSERVICE service, DWORD timeout , LPLFSRESULT * result);

HRESULT extern LFSAPI LFSAsyncLock (HSERVICE service, DWORD timeout, LPSTR object_name,  LPREQUESTID request_id);

HRESULT extern LFSAPI LFSOpen (LPSTR logical_name, HAPP app_handle, LPSTR app_id, DWORD trace_level, DWORD timeout, DWORD srvc_versions_required, LPLFSVERSION srvc_version, LPLFSVERSION spi_version, LPHSERVICE service);

HRESULT extern LFSAPI LFSAsyncOpen (LPSTR logical_name, HAPP app_handle, LPSTR app_id, DWORD trace_level, DWORD timeout, LPHSERVICE service, LPSTR object_name, DWORD srvc_versions_required, LPLFSVERSION srvc_version, LPLFSVERSION spi_version, LPREQUESTID request_id);

HRESULT extern LFSAPI LFSRegister (HSERVICE service, DWORD event_class, LPSTR object_reg_name);

HRESULT extern LFSAPI LFSAsyncRegister (HSERVICE service, DWORD event_class, LPSTR object_reg_name, LPSTR object_name, LPREQUESTID request_id);

HRESULT extern LFSAPI LFSSetBlockingHook (LFSBLOCKINGHOOK block_func, LPLFSBLOCKINGHOOK prev_func);

HRESULT extern LFSAPI LFSStartUp (DWORD versions_required, LPLFSVERSION lfs_version);

HRESULT extern LFSAPI LFSUnhookBlockingHook ();

HRESULT extern LFSAPI LFSUnlock (HSERVICE service);

HRESULT extern LFSAPI LFSAsyncUnlock (HSERVICE service, LPSTR object_name, LPREQUESTID request_id);

HRESULT extern LFSAPI LFMSetTraceLevel (HSERVICE service, DWORD trace_level);


/*   restore alignment   */
#pragma pack(pop)

#ifdef __cplusplus
}       /*extern "C"*/
#endif

#endif  /* __inc_lfsapi__h */
