/**************************************************************************
*                                                                         *
* lfsvdm.h LFS - 	厂商模式	(VDM) 定义	                  *
*                                                                         *
*                    Version 1.00 (2015-07-02)                            *
*                                                                         *
**************************************************************************/
 
#ifndef __INC_lFSVDM__H
#define __INC_lFSVDM__H

#ifdef __cplusplus
extern "C" {
#endif

#include "lfsapi.h"

/* be aware of alignment */
#pragma pack(push,1)

/* values of LFSVDMCAPS.dev_class */

#define LFS_SERVICE_CLASS_VDM             (7)
#define LFS_SERVICE_CLASS_VERSION_VDM     (0x0001) /* Version 1.00 */
#define LFS_SERVICE_CLASS_NAME_VDM        "VDM"

#define VDM_SERVICE_OFFSET                (LFS_SERVICE_CLASS_VDM * 100)

/* VDM Info Commands */

#define LFS_INF_VDM_STATUS                (VDM_SERVICE_OFFSET + 1)
#define LFS_INF_VDM_CAPABILITIES          (VDM_SERVICE_OFFSET + 2)

/* VDM Execute Commands */

#define LFS_CMD_VDM_ENTER_MODE_REQ        (VDM_SERVICE_OFFSET + 1)
#define LFS_CMD_VDM_ENTER_MODE_ACK        (VDM_SERVICE_OFFSET + 2)
#define LFS_CMD_VDM_EXIT_MODE_REQ         (VDM_SERVICE_OFFSET + 3)
#define LFS_CMD_VDM_EXIT_MODE_ACK         (VDM_SERVICE_OFFSET + 4)

/* VDM Messages */

#define LFS_SRVE_VDM_ENTER_MODE_REQ       (VDM_SERVICE_OFFSET + 1)
#define LFS_SRVE_VDM_EXIT_MODE_REQ        (VDM_SERVICE_OFFSET + 2)
#define LFS_SYSE_VDM_MODEENTERED          (VDM_SERVICE_OFFSET + 3)
#define LFS_SYSE_VDM_MODEEXITED           (VDM_SERVICE_OFFSET + 4)

/*LFSVDMSTATUS.device ȡֵ*/
#define LFS_VDM_DEVONLINE                 LFS_STAT_DEVONLINE
#define LFS_VDM_DEVOFFLINE                LFS_STAT_DEVOFFLINE

/*LFSVDMSTATUS.service ȡֵ*/
#define LFS_VDM_ENTERPENDING              (0)
#define LFS_VDM_ACTIVE                    (1)
#define LFS_VDM_EXITPENDING               (2)
#define LFS_VDM_INACTIVE                  (3)

/*=================================================================*/
/* VDM Info Command Structures and variables */
/*=================================================================*/

typedef struct _lfs_vdm_appstatus
{
    LPSTR               app_id;
    WORD                app_status;
} LFSVDMAPPSTATUS, * LPLFSVDMAPPSTATUS;

typedef struct _lfs_vdm_status
{
     WORD               device;
     WORD               service;
     LPLFSVDMAPPSTATUS  *app_status;
     LPSTR              extra;
} LFSVDMSTATUS, * LPLFSVDMSTATUS;

typedef struct _lfs_vdm_caps
{
     WORD               dev_class;
     LPSTR              extra;
} LFSVDMCAPS, * LPLFSVDMCAPS;

/*=================================================================*/
/* VDM Execute Command Structures */
/*=================================================================*/

/*=================================================================*/
/* VDM Message Structures */
/*=================================================================*/

/* restore alignment */
#pragma pack(pop)

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif  /* __INC_lFSVDM__H */