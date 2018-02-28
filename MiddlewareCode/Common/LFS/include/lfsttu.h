/******************************************************************************
*                                                                             *
* lfsttu.h      LFS - 定义	                                              *
*               文字终端				                      *
*                                                                             *
*               Version 1.00 (2015-07-02)                                     *
*                                                                             *
******************************************************************************/
 
#ifndef __INC_lFSTTU__H
#define __INC_lFSTTU__H

#ifdef __cplusplus
extern "C" {
#endif

#include "lfsapi.h"

/*   be aware of alignment   */
#pragma pack(push,1)


/*LFSTTUCAPS.wClass */
#define     LFS_SERVICE_CLASS_TTU               (6)
#define     LFS_SERVICE_CLASS_NAME_TTU          "TTU"
#define     LFS_SERVICE_CLASS_VERSION_TTU       (0x0001)

#define     TTU_SERVICE_OFFSET                  (LFS_SERVICE_CLASS_TTU * 100)

/* TTU Info Commands */
#define     LFS_INF_TTU_STATUS                  (TTU_SERVICE_OFFSET + 1)
#define     LFS_INF_TTU_CAPABILITIES            (TTU_SERVICE_OFFSET + 2)
#define     LFS_INF_TTU_FORM_LIST               (TTU_SERVICE_OFFSET + 3)
#define     LFS_INF_TTU_QUERY_FORM              (TTU_SERVICE_OFFSET + 4)
#define     LFS_INF_TTU_QUERY_FIELD             (TTU_SERVICE_OFFSET + 5)
#define     LFS_INF_TTU_KEY_DETAIL              (TTU_SERVICE_OFFSET + 6)

/* TTU Command Verbs */
#define     LFS_CMD_TTU_BEEP                    (TTU_SERVICE_OFFSET + 1)
#define     LFS_CMD_TTU_CLEARSCREEN             (TTU_SERVICE_OFFSET + 2)
#define     LFS_CMD_TTU_DISPLIGHT               (TTU_SERVICE_OFFSET + 3)
#define     LFS_CMD_TTU_SET_LED                 (TTU_SERVICE_OFFSET + 4)
#define     LFS_CMD_TTU_SET_RESOLUTION          (TTU_SERVICE_OFFSET + 5)
#define     LFS_CMD_TTU_WRITE_FORM              (TTU_SERVICE_OFFSET + 6)
#define     LFS_CMD_TTU_READ_FORM               (TTU_SERVICE_OFFSET + 7)
#define     LFS_CMD_TTU_WRITE                   (TTU_SERVICE_OFFSET + 8)
#define     LFS_CMD_TTU_READ                    (TTU_SERVICE_OFFSET + 9)
#define     LFS_CMD_TTU_RESET                   (TTU_SERVICE_OFFSET + 10)

/* TTU Messages */
#define    LFS_EXEE_TTU_FIELDERROR              (TTU_SERVICE_OFFSET + 1)
#define    LFS_EXEE_TTU_FIELDWARNING            (TTU_SERVICE_OFFSET + 2)
#define    LFS_EXEE_TTU_KEY                     (TTU_SERVICE_OFFSET + 3)

/*LFSTTUSTATUS.device 取值*/
#define     LFS_TTU_DEVONLINE                   LFS_STAT_DEVONLINE
#define     LFS_TTU_DEVOFFLINE                  LFS_STAT_DEVOFFLINE
#define     LFS_TTU_DEVPOWEROFF                 LFS_STAT_DEVPOWEROFF
#define     LFS_TTU_DEVBUSY                     LFS_STAT_DEVBUSY
#define     LFS_TTU_DEVNODEVICE                 LFS_STAT_DEVNODEVICE
#define     LFS_TTU_DEVHWERROR                  LFS_STAT_DEVHWERROR
#define     LFS_TTU_DEVUSERERROR                LFS_STAT_DEVUSERERROR

/*LFSTTUSTATUS.keyboard 取值*/
#define     LFS_TTU_KBDNA                       (0)
#define     LFS_TTU_KBDON                       (1)
#define     LFS_TTU_KBDOFF                      (2)

/*LFSTTUSTATUS.key_lock 取值*/
#define     LFS_TTU_KBDLOCKNA                   (0)
#define     LFS_TTU_KBDLOCKON                   (1)
#define     LFS_TTU_KBDLOCKOFF                  (2)

#define     LFS_TTU_LEDS_MAX                    (8)

/*LFSTTUSTATUS.LEDs 取值*/
#define     LFS_TTU_LEDNA                       (0x0000)
#define     LFS_TTU_LEDOFF                      (0x0001)
#define     LFS_TTU_LEDSLOWFLASH                (0x0002)
#define     LFS_TTU_LEDMEDIUMFLASH              (0x0004)
#define     LFS_TTU_LEDQUICKFLASH               (0x0008)
#define     LFS_TTU_LEDCONTINUOUS               (0x0080)

/*LFSTTUCAPS.type 取值*/
#define     LFS_TTU_FIXED                       (0x0001)
#define     LFS_TTU_REMOVABLE                   (0x0002)

/*LFSTTUCAPS.char_support, LFSTTUWRITE.char_support 取值*/
#define     LFS_TTU_UTF8                       (0x0001)
#define     LFS_TTU_UNICODE                     (0x0002)

/*LFSTTUFRMFIELD.dev_type 取值*/
#define     LFS_TTU_FIELDTEXT                   (0)
#define     LFS_TTU_FIELDINVISIBLE              (1)
#define     LFS_TTU_FIELDPASSWORD               (2)

/*LFSTTUFRMFIELD.class 取值*/
#define     LFS_TTU_CLASSOPTIONAL               (0)
#define     LFS_TTU_CLASSSTATIC                 (1)
#define     LFS_TTU_CLASSREQUIRED               (2)

/*LFSTTUFRMFIELD.access 取值*/
#define     LFS_TTU_ACCESSREAD                  (0x0001)
#define     LFS_TTU_ACCESSWRITE                 (0x0002)

/*LFSTTUFRMFIELD.overflow 取值*/
#define     LFS_TTU_OVFTERMINATE                (0)
#define     LFS_TTU_OVFTRUNCATE                 (1)
#define     LFS_TTU_OVFOVERWRITE                (2)

/*LFSTTUWRITE.mode 取值*/
#define     LFS_TTU_POSRELATIVE                 (0)
#define     LFS_TTU_POSABSOLUTE                 (1)

/*LFSTTUWRITE.text_attr 取值*/
#define     LFS_TTU_TEXTUNDERLINE               (0x0001)
#define     LFS_TTU_TEXTINVERTED                (0x0002)
#define     LFS_TTU_TEXTFLASH                   (0x0004)

/*LFSTTUFRMREAD.echo_mode 取值*/
#define     LFS_TTU_ECHOTEXT                    (0)
#define     LFS_TTU_ECHOINVISIBLE               (1)
#define     LFS_TTU_ECHOPASSWORD                (2)

#define     LFS_TTU_BEEPOFF                     (0x0001)
#define     LFS_TTU_BEEPKEYPRESS                (0x0002)
#define     LFS_TTU_BEEPEXCLAMATION             (0x0004)
#define     LFS_TTU_BEEPWARNING                 (0x0008)
#define     LFS_TTU_BEEPERROR                   (0x0010)
#define     LFS_TTU_BEEPCRITICAL                (0x0020)
#define     LFS_TTU_BEEPCONTINUOUS              (0x0080)

/*LFSTTUFIELDFAIL.failure 取值*/

#define    LFS_TTU_FIELDREQUIRED            (0)
#define    LFS_TTU_FIELDSTATICOVWR          (1)
#define    LFS_TTU_FIELDOVERFLOW            (2)
#define    LFS_TTU_FIELDNOTFOUND            (3)
#define    LFS_TTU_FIELDNOTREAD             (4)
#define    LFS_TTU_FIELDNOTWRITE            (5)
#define    LFS_TTU_FIELDTYPENOTSUPPORTED    (6)
#define    LFS_TTU_CHARSETFORM              (7)

/*LFSTTUKEYDETAIL.command_keys 取值*/

#define LFS_TTU_NOKEY                     (0)
#define LFS_TTU_CK_ENTER                  (1)
#define LFS_TTU_CK_CANCEL                 (2)
#define LFS_TTU_CK_CLEAR                  (3)
#define LFS_TTU_CK_BACKSPACE              (4)
#define LFS_TTU_CK_HELP                   (5)
#define LFS_TTU_CK_00                     (6)
#define LFS_TTU_CK_000                    (7)
#define LFS_TTU_CK_ARROWUP                (8)
#define LFS_TTU_CK_ARROWDOWN              (9)
#define LFS_TTU_CK_ARROWLEFT              (10)
#define LFS_TTU_CK_ARROWRIGHT             (11)
#define LFS_TTU_CK_OEM1                   (12)
#define LFS_TTU_CK_OEM2                   (13)
#define LFS_TTU_CK_OEM3                   (14)
#define LFS_TTU_CK_OEM4                   (15)
#define LFS_TTU_CK_OEM5                   (16)
#define LFS_TTU_CK_OEM6                   (17)
#define LFS_TTU_CK_OEM7                   (18)
#define LFS_TTU_CK_OEM8                   (19)
#define LFS_TTU_CK_OEM9                   (20)
#define LFS_TTU_CK_OEM10                  (21)
#define LFS_TTU_CK_OEM11                  (22)
#define LFS_TTU_CK_OEM12                  (23)
#define LFS_TTU_CK_FDK01                  (24)
#define LFS_TTU_CK_FDK02                  (25)
#define LFS_TTU_CK_FDK03                  (26)
#define LFS_TTU_CK_FDK04                  (27)
#define LFS_TTU_CK_FDK05                  (28)
#define LFS_TTU_CK_FDK06                  (29)
#define LFS_TTU_CK_FDK07                  (30)
#define LFS_TTU_CK_FDK08                  (31)
#define LFS_TTU_CK_FDK09                  (32)
#define LFS_TTU_CK_FDK10                  (33)
#define LFS_TTU_CK_FDK11                  (34)
#define LFS_TTU_CK_FDK12                  (35)
#define LFS_TTU_CK_FDK13                  (36)
#define LFS_TTU_CK_FDK14                  (37)
#define LFS_TTU_CK_FDK15                  (38)
#define LFS_TTU_CK_FDK16                  (39)
#define LFS_TTU_CK_FDK17                  (40)
#define LFS_TTU_CK_FDK18                  (41)
#define LFS_TTU_CK_FDK19                  (42)
#define LFS_TTU_CK_FDK20                  (43)
#define LFS_TTU_CK_FDK21                  (44)
#define LFS_TTU_CK_FDK22                  (45)
#define LFS_TTU_CK_FDK23                  (46)
#define LFS_TTU_CK_FDK24                  (47)
#define LFS_TTU_CK_FDK25                  (48)
#define LFS_TTU_CK_FDK26                  (49)
#define LFS_TTU_CK_FDK27                  (50)
#define LFS_TTU_CK_FDK28                  (51)
#define LFS_TTU_CK_FDK29                  (52)
#define LFS_TTU_CK_FDK30                  (53)
#define LFS_TTU_CK_FDK31                  (54)
#define LFS_TTU_CK_FDK32                  (55)

/* XFS TTU Errors */ 

#define     LFS_ERR_TTU_FIELDERROR              (-(TTU_SERVICE_OFFSET + 1))
#define     LFS_ERR_TTU_FIELDINVALID            (-(TTU_SERVICE_OFFSET + 2))
#define     LFS_ERR_TTU_FIELDNOTFOUND           (-(TTU_SERVICE_OFFSET + 3))
#define     LFS_ERR_TTU_FIELDSPECFAILURE        (-(TTU_SERVICE_OFFSET + 4))
#define     LFS_ERR_TTU_FORMINVALID             (-(TTU_SERVICE_OFFSET + 5))
#define     LFS_ERR_TTU_FORMNOTFOUND            (-(TTU_SERVICE_OFFSET + 6))
#define     LFS_ERR_TTU_INVALIDLED              (-(TTU_SERVICE_OFFSET + 7))
#define     LFS_ERR_TTU_KEYCANCELED             (-(TTU_SERVICE_OFFSET + 8))
#define     LFS_ERR_TTU_MEDIAOVERFLOW           (-(TTU_SERVICE_OFFSET + 9))
#define     LFS_ERR_TTU_RESNOTSUPP              (-(TTU_SERVICE_OFFSET + 10))
#define     LFS_ERR_TTU_CHARSETDATA             (-(TTU_SERVICE_OFFSET + 11))
#define     LFS_ERR_TTU_KEYINVALID              (-(TTU_SERVICE_OFFSET + 12))
#define     LFS_ERR_TTU_KEYNOTSUPPORTED         (-(TTU_SERVICE_OFFSET + 13))
#define     LFS_ERR_TTU_NOACTIVEKEYS            (-(TTU_SERVICE_OFFSET + 14))

/*=================================================================*/
/* TTU Info Command Structures */
/*=================================================================*/

typedef struct _lfs_ttu_status
{
    WORD            device;
    WORD            keyboard;
    WORD            key_lock;
    WORD            leds[LFS_TTU_LEDS_MAX];
    WORD            display_size_x;
    WORD            display_size_y;
    LPSTR           extra;
} LFSTTUSTATUS, * LPLFSTTUSTATUS;

typedef struct _lfs_ttu_resolution
{
    WORD            size_x;
    WORD            size_y;
} LFSTTURESOLUTION, * LPLFSTTURESOLUTION;

typedef struct _lfs_ttu_caps
{
    WORD                  dev_class;
    WORD                  dev_type;
    LPLFSTTURESOLUTION *  resolutions;
    WORD                  num_of_leds;
    BOOL                  key_lock;
    BOOL                  display_light;
    BOOL                  cursor;
    BOOL                  forms;
    WORD                  char_support;
    LPSTR                 extra;
} LFSTTUCAPS, * LPLFSTTUCAPS;

typedef struct _lfs_ttu_frm_header
{
    LPSTR           form_name;
    WORD            width;
    WORD            height;
    WORD            version_major;
    WORD            version_minor;
    WORD            char_support;
    LPSTR           fields;
} LFSTTUFRMHEADER, * LPLFSTTUFRMHEADER;

typedef struct _lfs_ttu_query_field
{
    LPSTR           form_name;
    LPSTR           field_name;
} LFSTTUQUERYFIELD, * LPLFSTTUQUERYFIELD;

typedef struct _lfs_ttu_frm_field
{
    LPSTR           field_name;
    WORD            type;
    WORD            field_class;
    WORD            access;
    WORD            overflow;
    LPSTR           format;
} LFSTTUFRMFIELD, * LPLFSTTUFRMFIELD;

typedef struct _lfs_ttu_key_detail
{
    LPSTR           keys;
    LPWSTR          unicode_keys;
    LPWORD          fommand_keys;
} LFSTTUKEYDETAIL, * LPLFSTTUKEYDETAIL;

typedef struct _lfs_ttu_clear_screen
{
    WORD            position_x;
    WORD            position_y;
    WORD            width;
    WORD            height;
} LFSTTUCLEARSCREEN, * LPLFSTTUCLEARSCREEN;

typedef struct _lfs_ttu_disp_light
{
    BOOL            mode;
} LFSTTUDISPLIGHT, * LPLFSTTUDISPLIGHT;

typedef struct _lfs_ttu_set_leds
{
    WORD            led;
    WORD            command;
} LFSTTUSETLEDS, * LPLFSTTUSETLEDS;

typedef struct _lfs_ttu_write_form
{
    LPSTR           form_name;
    BOOL            clear_screen;
    LPSTR           fields;
    LPWSTR          unicode_fields;
} LFSTTUWRITEFORM, * LPLFSTTUWRITEFORM;

typedef struct _lfs_ttu_read_form
{
    LPSTR           form_name;
    LPSTR           field_names;
} LFSTTUREADFORM, * LPLFSTTUREADFORM;

typedef struct _lfs_ttu_read_form_out
{
    LPSTR           fields;
    LPWSTR          unicode_fields;
} LFSTTUREADFORMOUT, * LPLFSTTUREADFORMOUT;

typedef struct _lfs_ttu_write
{
    WORD            mode;
    SHORT           pos_x;
    SHORT           pos_y;
    WORD            text_attr;
    LPSTR           text;
    LPWSTR          unicode_text;
} LFSTTUWRITE, * LPLFSTTUWRITE;

typedef struct _lfs_ttu_read
{
    WORD            num_of_chars;
    WORD            mode;
    SHORT           pos_x;
    SHORT           pos_y;
    WORD            echo_mode;
    WORD            echo_attr;
    BOOL            cursor;
    BOOL            flush;
    BOOL            auto_end;
    LPSTR           active_keys;
    LPWSTR          active_unicode_keys;
    LPWORD          active_command_keys;
    LPWORD          terminate_command_keys;
} LFSTTUREAD, * LPLFSTTUREAD;

typedef struct _lfs_ttu_read_in
{
    LPSTR           input;
    LPWSTR          unicode_input;
} LFSTTUREADIN, * LPLFSTTUREADIN;

/*=================================================================*/
/* TTU Message Structures */
/*=================================================================*/

typedef struct _lfs_ttu_field_failure
{
    LPSTR           form_name;
    LPSTR           field_name;
    WORD            failure;
} LFSTTUFIELDFAIL, * LPLFSTTUFIELDFAIL;

typedef struct _lfs_ttu_key
{
    CHAR            key;
    WORD            unicode_key;
    WORD            command_key;
} LFSTTUKEY, * LPLFSTTUKEY;


/* restore alignment */
#pragma pack(pop)

#ifdef __cplusplus
}       /*extern "C"*/
#endif

#endif  /* __INC_lFSTTU__H */
