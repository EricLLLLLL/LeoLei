/******************************************************************************
*                                                                             *
* lfsspi.h      LFS - SPI 函数、类型与定义                                   *
*                                                                             *
*               Version 1.00  --  2015-07-02                                  *
*                                                                             *
******************************************************************************/

#ifndef __inc_lfsspi__h
#define __inc_lfsspi__h

#ifdef __cplusplus
extern "C" {
#endif

#include <lfsapi.h>

typedef HANDLE HPROVIDER; 

#include <lfsadmin.h>

/*   字节对齐设置为1   */
#pragma pack(push,1)

/****** SPI 函数 *************************************************************/

HRESULT extern LFSAPI LFPCancelAsyncRequest ( HSERVICE service, REQUESTID request_id);

HRESULT extern LFSAPI LFPClose ( HSERVICE service, LPSTR object_name, REQUESTID req_id);

HRESULT extern LFSAPI LFPDeregister ( HSERVICE service, DWORD event_class, LPSTR object_reg_name, LPSTR object_name, REQUESTID req_id);

HRESULT extern LFSAPI LFPExecute ( HSERVICE service, DWORD command, LPVOID cmd_data, DWORD timeout, LPSTR object_name, REQUESTID req_id);

HRESULT extern LFSAPI LFPGetInfo ( HSERVICE service, DWORD category, LPVOID query_details, DWORD timeout, LPSTR object_name, REQUESTID req_id);

HRESULT extern LFSAPI LFPLock ( HSERVICE service, DWORD timeout, LPSTR object_name, REQUESTID req_id);

HRESULT extern LFSAPI LFPOpen ( HSERVICE service, LPSTR logical_name, HAPP app_handle, LPSTR app_id, DWORD trace_level, DWORD timeout, LPSTR object_name, REQUESTID req_id, HPROVIDER provider, DWORD spi_versions_required, LPLFSVERSION spi_version, DWORD srvc_versions_required, LPLFSVERSION srvc_version);

HRESULT extern LFSAPI LFPRegister ( HSERVICE service,  DWORD event_class, LPSTR object_reg_name, LPSTR object_name, REQUESTID req_id);

HRESULT extern LFSAPI LFPSetTraceLevel ( HSERVICE service, DWORD trace_level);

HRESULT extern LFSAPI LFPUnloadService (  );

HRESULT extern LFSAPI LFPUnlock ( HSERVICE service, LPSTR object_name, REQUESTID req_id);


/*   恢复字节对齐方式   */
#pragma pack(pop)

#ifdef __cplusplus
}    /*extern "C"*/
#endif

#endif  /* __inc_lfsspi__h */

