/******************************************************************************
*                                                                             *
* lfsidc.h    LFS - 读卡器(IDC)定义                                           *
*                                                                             *
*             Version 1.00  (2015-07-02)                                      *
*                                                                             *
******************************************************************************/
 
#ifndef __INC_LFSIDC__H
#define __INC_LFSIDC__H

#ifdef __cplusplus
extern "C" {
#endif

#include <lfsapi.h>

/*   设置字节对齐方式   */
#pragma pack(push,1)


/* LFSIDCCAPS.dev_class取值 */

#define     LFS_SERVICE_CLASS_IDC               (3)
#define     LFS_SERVICE_CLASS_NAME_IDC          "IDC"
#define     LFS_SERVICE_CLASS_VERSION_IDC       0x0001

#define     IDC_SERVICE_OFFSET                  (LFS_SERVICE_CLASS_IDC * 100)

/* 信息指令 */

#define     LFS_INF_IDC_STATUS                  (IDC_SERVICE_OFFSET + 1)
#define     LFS_INF_IDC_CAPABILITIES            (IDC_SERVICE_OFFSET + 2)
#define     LFS_INF_IDC_FORM_LIST               (IDC_SERVICE_OFFSET + 3)
#define     LFS_INF_IDC_QUERY_FORM              (IDC_SERVICE_OFFSET + 4)

/* 执行指令 */

#define     LFS_CMD_IDC_READ_TRACK              (IDC_SERVICE_OFFSET + 1)
#define     LFS_CMD_IDC_WRITE_TRACK             (IDC_SERVICE_OFFSET + 2)
#define     LFS_CMD_IDC_EJECT_CARD              (IDC_SERVICE_OFFSET + 3)
#define     LFS_CMD_IDC_RETAIN_CARD             (IDC_SERVICE_OFFSET + 4)
#define     LFS_CMD_IDC_RESET_COUNT             (IDC_SERVICE_OFFSET + 5)
#define     LFS_CMD_IDC_SETKEY                  (IDC_SERVICE_OFFSET + 6)
#define     LFS_CMD_IDC_READ_RAW_DATA           (IDC_SERVICE_OFFSET + 7)
#define     LFS_CMD_IDC_WRITE_RAW_DATA          (IDC_SERVICE_OFFSET + 8)
#define     LFS_CMD_IDC_CHIP_IO                 (IDC_SERVICE_OFFSET + 9)
#define     LFS_CMD_IDC_RESET                   (IDC_SERVICE_OFFSET + 10)
#define     LFS_CMD_IDC_CHIP_POWER              (IDC_SERVICE_OFFSET + 11)
#define     LFS_CMD_IDC_PARSE_DATA              (IDC_SERVICE_OFFSET + 12)


/* 事件 */

#define     LFS_EXEE_IDC_INVALIDTRACKDATA       (IDC_SERVICE_OFFSET + 1)
#define     LFS_EXEE_IDC_MEDIAINSERTED          (IDC_SERVICE_OFFSET + 3)
#define     LFS_SRVE_IDC_MEDIAREMOVED           (IDC_SERVICE_OFFSET + 4)
#define     LFS_SRVE_IDC_CARDACTION             (IDC_SERVICE_OFFSET + 5)
#define     LFS_USRE_IDC_RETAINBINTHRESHOLD     (IDC_SERVICE_OFFSET + 6)
#define     LFS_EXEE_IDC_INVALIDMEDIA           (IDC_SERVICE_OFFSET + 7)
#define     LFS_EXEE_IDC_MEDIARETAINED          (IDC_SERVICE_OFFSET + 8)
#define     LFS_SRVE_IDC_MEDIADETECTED          (IDC_SERVICE_OFFSET + 9)

/* LFSIDCSTATUS.device 取值 */
#define     LFS_IDC_DEVONLINE                   LFS_STAT_DEVONLINE
#define     LFS_IDC_DEVOFFLINE                  LFS_STAT_DEVOFFLINE
#define     LFS_IDC_DEVPOWEROFF                 LFS_STAT_DEVPOWEROFF
#define     LFS_IDC_DEVNODEVICE                 LFS_STAT_DEVNODEVICE
#define     LFS_IDC_DEVHWERROR                  LFS_STAT_DEVHWERROR
#define     LFS_IDC_DEVUSERERROR                LFS_STAT_DEVUSERERROR
#define     LFS_IDC_DEVBUSY                     LFS_STAT_DEVBUSY

/* LFSIDCSTATUS.media, LFSIDCRETAINCARD.position,  */
/* LFSIDCCARDACT.position 取值  */

#define     LFS_IDC_MEDIAPRESENT                (1)
#define     LFS_IDC_MEDIANOTPRESENT             (2)
#define     LFS_IDC_MEDIAJAMMED                 (3)
#define     LFS_IDC_MEDIANOTSUPP                (4)
#define     LFS_IDC_MEDIAUNKNOWN                (5)
#define     LFS_IDC_MEDIAENTERING               (6)
#define     LFS_IDC_MEDIALATCHED                (7)

/* LFSIDCSTATUS.retain_bin 取值 */

#define     LFS_IDC_RETAINBINOK                 (1)
#define     LFS_IDC_RETAINNOTSUPP               (2)
#define     LFS_IDC_RETAINBINFULL               (3)
#define     LFS_IDC_RETAINBINHIGH               (4)

/* LFSIDCSTATUS.security 取值 */

#define     LFS_IDC_SECNOTSUPP                  (1)
#define     LFS_IDC_SECNOTREADY                 (2)
#define     LFS_IDC_SECOPEN                     (3)

/* LFSIDCSTATUS.chip_power 取值 */

#define     LFS_IDC_CHIPONLINE                  (0)
#define     LFS_IDC_CHIPPOWEREDOFF              (1)
#define     LFS_IDC_CHIPBUSY                    (2)
#define     LFS_IDC_CHIPNODEVICE                (3)
#define     LFS_IDC_CHIPHWERROR                 (4)
#define     LFS_IDC_CHIPNOCARD                  (5)
#define     LFS_IDC_CHIPNOTSUPP                 (6)
#define     LFS_IDC_CHIPUNKNOWN                 (7)


/* LFSIDCCAPS.dev_type 取值 */

#define     LFS_IDC_TYPEMOTOR                   (1)
#define     LFS_IDC_TYPESWIPE                   (2)
#define     LFS_IDC_TYPEDIP                     (3)
#define     LFS_IDC_TYPECONTACTLESS             (4)
#define     LFS_IDC_TYPELATCHEDDIP              (5)
#define     LFS_IDC_TYPEPERMANENT               (6)

/* LFSIDCCAPS.read_tracks, LFSIDCCAPS.write_tracks,
             LFSIDCCARDDATA.data_source 取值 */

#define     LFS_IDC_NOTSUPP                     0x0000
#define     LFS_IDC_TRACK1                      0x0001
#define     LFS_IDC_TRACK2                      0x0002
#define     LFS_IDC_TRACK3                      0x0004

/* LFSIDCCARDDATA.data_source 取值 */

#define     LFS_IDC_CHIP                        0x0008
#define     LFS_IDC_SECURITY                    0x0010
#define     LFS_IDC_FLUXINACTIVE                0x0020
#define     LFS_IDC_TRACK_WM                    0x8000

/* LFSIDCCAPS.chip_protocols 取值 */

#define     LFS_IDC_CHIPT0                      0x0001
#define     LFS_IDC_CHIPT1                      0x0002
#define     LFS_IDC_CHIPT2                      0x0004
#define     LFS_IDC_CHIPT3                      0x0008
#define     LFS_IDC_CHIPT4                      0x0010
#define     LFS_IDC_CHIPT5                      0x0020
#define     LFS_IDC_CHIPT6                      0x0040
#define     LFS_IDC_CHIPT7                      0x0080
#define     LFS_IDC_CHIPT8                      0x0100
#define     LFS_IDC_CHIPT9                      0x0200
#define     LFS_IDC_CHIPT10                     0x0400
#define     LFS_IDC_CHIPT11                     0x0800
#define     LFS_IDC_CHIPT12                     0x1000
#define     LFS_IDC_CHIPT13                     0x2000
#define     LFS_IDC_CHIPT14                     0x4000
#define     LFS_IDC_CHIPT15                     0x8000

/* LFSIDCCAPS.sec_type 取值 */

#define     LFS_IDC_SECNOTSUPP                  (1)
#define     LFS_IDC_SECMMBOX                    (2)
#define     LFS_IDC_SECCIM86                    (3)

/* LFSIDCCAPS.power_on_option, LFSIDCCAPS.power_off_option 取值  */

#define     LFS_IDC_NOACTION                    (1)
#define     LFS_IDC_EJECT                       (2)
#define     LFS_IDC_RETAIN                      (3)
#define     LFS_IDC_EJECTTHENRETAIN             (4)
#define     LFS_IDC_READPOSITION                (5)

/* LFSIDCCAPS.write_mode; LFSIDCWRITETRACK.write_method, LFSIDCCARDDATA.write_method 取值 */

#define     LFS_IDC_UNKNOWN                     0x0001
#define     LFS_IDC_LOCO                        0x0002
#define     LFS_IDC_HICO                        0x0004
#define     LFS_IDC_AUTO                        0x0008

/* LFSIDCCAPS.chip_power 取值 */

#define     LFS_IDC_CHIPPOWERCOLD               0x0002
#define     LFS_IDC_CHIPPOWERWARM               0x0004
#define     LFS_IDC_CHIPPOWEROFF                0x0008

/* LFSIDCFORM.action 取值 */

#define     LFS_IDC_ACTIONREAD                  0x0001
#define     LFS_IDC_ACTIONWRITE                 0x0002

/* LFSIDCTRACKEVENT.status, LFSIDCCARDDATA.status 取值 */

#define     LFS_IDC_DATAOK                      (0)
#define     LFS_IDC_DATAMISSING                 (1)
#define     LFS_IDC_DATAINVALID                 (2)
#define     LFS_IDC_DATATOOLONG                 (3)
#define     LFS_IDC_DATATOOSHORT                (4)
#define     LFS_IDC_DATASRCNOTSUPP              (5)
#define     LFS_IDC_DATASRCMISSING              (6)

/* LFSIDCCARDACT.action 取值 */

#define     LFS_IDC_CARDRETAINED                (1)
#define     LFS_IDC_CARDEJECTED                 (2)
#define     LFS_IDC_CARDREADPOSITION            (3)
#define     LFS_IDC_CARDJAMMED                  (4)

/* LFSIDCCARDDATA.data 安全模块返回值 */

#define     LFS_IDC_SEC_READLEVEL1              '1'
#define     LFS_IDC_SEC_READLEVEL2              '2'
#define     LFS_IDC_SEC_READLEVEL3              '3'
#define     LFS_IDC_SEC_READLEVEL4              '4'
#define     LFS_IDC_SEC_READLEVEL5              '5'
#define     LFS_IDC_SEC_BADREADLEVEL            '6'
#define     LFS_IDC_SEC_NODATA                  '7'
#define     LFS_IDC_SEC_DATAINVAL               '8'
#define     LFS_IDC_SEC_HWERROR                 '9'
#define     LFS_IDC_SEC_NOINIT                  'A'

/* 错误码 */ 

#define LFS_ERR_IDC_MEDIAJAM                    (-(IDC_SERVICE_OFFSET + 0))
#define LFS_ERR_IDC_NOMEDIA                     (-(IDC_SERVICE_OFFSET + 1))
#define LFS_ERR_IDC_MEDIARETAINED               (-(IDC_SERVICE_OFFSET + 2))
#define LFS_ERR_IDC_RETAINBINFULL               (-(IDC_SERVICE_OFFSET + 3))
#define LFS_ERR_IDC_INVALIDDATA                 (-(IDC_SERVICE_OFFSET + 4))
#define LFS_ERR_IDC_INVALIDMEDIA                (-(IDC_SERVICE_OFFSET + 5))
#define LFS_ERR_IDC_FORMNOTFOUND                (-(IDC_SERVICE_OFFSET + 6))
#define LFS_ERR_IDC_FORMINVALID                 (-(IDC_SERVICE_OFFSET + 7))
#define LFS_ERR_IDC_DATASYNTAX                  (-(IDC_SERVICE_OFFSET + 8))
#define LFS_ERR_IDC_SHUTTERFAIL                 (-(IDC_SERVICE_OFFSET + 9))
#define LFS_ERR_IDC_SECURITYFAIL                (-(IDC_SERVICE_OFFSET + 10))
#define LFS_ERR_IDC_PROTOCOLNOTSUPP             (-(IDC_SERVICE_OFFSET + 11))
#define LFS_ERR_IDC_ATRNOTOBTAINED              (-(IDC_SERVICE_OFFSET + 12))
#define LFS_ERR_IDC_INVALIDKEY                  (-(IDC_SERVICE_OFFSET + 13))
#define LFS_ERR_IDC_WRITE_METHOD                (-(IDC_SERVICE_OFFSET + 14))
#define LFS_ERR_IDC_CHIPPOWERNOTSUPP            (-(IDC_SERVICE_OFFSET + 15))
#define LFS_ERR_IDC_CARDTOOSHORT                (-(IDC_SERVICE_OFFSET + 16))
#define LFS_ERR_IDC_CARDTOOLONG                 (-(IDC_SERVICE_OFFSET + 17))

/*=================================================================*/
/* 信息指令结构体 */
/*=================================================================*/

typedef struct _lfs_idc_status
{
    WORD            device;
    WORD            media;
    WORD            retain_bin;
    WORD            security;
    USHORT          cards;
    WORD            chip_power;
    LPSTR           extra;
} LFSIDCSTATUS, * LPLFSIDCSTATUS;


typedef struct _lfs_idc_caps
{
    WORD            dev_class;
    WORD            dev_type;
    BOOL            compound;
    WORD            read_tracks;
    WORD            write_tracks;
    WORD            chip_protocols;
    USHORT          cards;
    WORD            sec_type;
    WORD            power_on_option;
    WORD            power_off_option;
    BOOL            flux_sensor_programmable;
    BOOL            read_write_access_following_eject;
    WORD            write_mode;
    WORD            chip_power;
    LPSTR           extra;
} LFSIDCCAPS, * LPLFSIDCCAPS;

typedef struct _lfs_idc_form
{
    LPSTR           form_name;
    CHAR            field_separator_track1;
    CHAR            field_separator_track2;
    CHAR            field_separator_track3;
    WORD            action;
    LPSTR           tracks;
    BOOL            secure;
    LPSTR           track1_fields;
    LPSTR           track2_fields;
    LPSTR           track3_fields;
} LFSIDCFORM, * LPLFSIDCFORM;

/*=================================================================*/
/* 执行指令结构体 */
/*=================================================================*/

typedef struct _lfs_idc_write_track
{
    LPSTR           form_name;
    LPSTR           track_data;
    WORD            write_method;
} LFSIDCWRITETRACK, * LPLFSIDCWRITETRACK;


typedef struct _lfs_idc_retain_card
{
    USHORT          count;
    WORD            position;
} LFSIDCRETAINCARD, * LPLFSIDCRETAINCARD;


typedef struct _lfs_idc_setkey
{
    USHORT          key_len;
    LPBYTE          key_value;
} LFSIDCSETKEY, * LPLFSIDCSETKEY;


typedef struct _lfs_idc_card_data
{
    WORD            data_source;
    WORD            status;
    ULONG           data_length;
    LPBYTE          data;
    WORD            write_method;
} LFSIDCCARDDATA, * LPLFSIDCCARDDATA;

typedef struct _lfs_idc_chip_io
{
    WORD           chip_protocol;
    ULONG          chip_data_length;
    LPBYTE         chip_data;
} LFSIDCCHIPIO, * LPLFSIDCCHIPIO;

typedef struct _lfs_idc_chip_power_out
{
    ULONG          chip_data_length;
    LPBYTE         chip_data;
} LFSIDCCHIPPOWEROUT, * LPLFSIDCCHIPPOWEROUT;


typedef struct _lfs_idc_parse_data
{
    LPSTR             form_name;
    LPLFSIDCCARDDATA  *card_data;
} LFSIDCPARSEDATA, * LPLFSIDCPARSEDATA;


/*=================================================================*/
/* 事件结构体 */
/*=================================================================*/

typedef struct _lfs_idc_track_event
{
    WORD            status;
    LPSTR           track;
    LPSTR           data;
} LFSIDCTRACKEVENT, * LPLFSIDCTRACKEVENT;

typedef struct _lfs_idc_card_act
{
    WORD            action;
    WORD            position;
} LFSIDCCARDACT, * LPLFSIDCCARDACT;


/*   恢复原有字节对齐方式   */
#pragma pack(pop)

#ifdef __cplusplus
}       /*extern "C"*/
#endif

#endif  /* __INC_LFSIDC__H */
