/******************************************************************************
*                                                                             *
* lfscdm.h      LFS - 出钞机芯 (CDM) 定义                       *
*                                                                             *
*               Version 1.00 (2015-07-02)                                       *
*                                                                             *
******************************************************************************/
 
#ifndef __INC_LFSCDM__H
#define __INC_LFSCDM__H

#ifdef __cplusplus
extern "C" {
#endif

#include <lfsapi.h>

/* 对齐 */
#pragma pack (push, 1)

/* LFSCDMCAPS.dev_class的取值*/

#define     LFS_SERVICE_CLASS_CDM               (4)
#define     LFS_SERVICE_CLASS_VERSION_CDM       0x0001
#define     LFS_SERVICE_CLASS_NAME_CDM          "CDM"

#define     CDM_SERVICE_OFFSET                  (LFS_SERVICE_CLASS_CDM * 100)

/* CDM状态命令 */

#define     LFS_INF_CDM_STATUS                  (CDM_SERVICE_OFFSET + 1)
#define     LFS_INF_CDM_CAPABILITIES            (CDM_SERVICE_OFFSET + 2)
#define     LFS_INF_CDM_CASH_UNIT_INFO          (CDM_SERVICE_OFFSET + 3)
#define     LFS_INF_CDM_TELLER_INFO             (CDM_SERVICE_OFFSET + 4)
#define     LFS_INF_CDM_CURRENCY_EXP            (CDM_SERVICE_OFFSET + 6)
#define     LFS_INF_CDM_MIX_TYPES               (CDM_SERVICE_OFFSET + 7)
#define     LFS_INF_CDM_MIX_TABLE               (CDM_SERVICE_OFFSET + 8)
#define     LFS_INF_CDM_PRESENT_STATUS          (CDM_SERVICE_OFFSET + 9)

/* CDM执行命令 */

#define     LFS_CMD_CDM_DENOMINATE              (CDM_SERVICE_OFFSET + 1)
#define     LFS_CMD_CDM_DISPENSE                (CDM_SERVICE_OFFSET + 2)
#define     LFS_CMD_CDM_PRESENT                 (CDM_SERVICE_OFFSET + 3)
#define     LFS_CMD_CDM_REJECT                  (CDM_SERVICE_OFFSET + 4)
#define     LFS_CMD_CDM_RETRACT                 (CDM_SERVICE_OFFSET + 5)
#define     LFS_CMD_CDM_OPEN_SHUTTER            (CDM_SERVICE_OFFSET + 7)
#define     LFS_CMD_CDM_CLOSE_SHUTTER           (CDM_SERVICE_OFFSET + 8)
#define     LFS_CMD_CDM_SET_TELLER_INFO         (CDM_SERVICE_OFFSET + 9)
#define     LFS_CMD_CDM_SET_CASH_UNIT_INFO      (CDM_SERVICE_OFFSET + 10)
#define     LFS_CMD_CDM_START_EXCHANGE          (CDM_SERVICE_OFFSET + 11)
#define     LFS_CMD_CDM_END_EXCHANGE            (CDM_SERVICE_OFFSET + 12)
#define     LFS_CMD_CDM_OPEN_SAFE_DOOR          (CDM_SERVICE_OFFSET + 13)
#define     LFS_CMD_CDM_CALIBRATE_CASH_UNIT     (CDM_SERVICE_OFFSET + 15)
#define     LFS_CMD_CDM_SET_MIX_TABLE           (CDM_SERVICE_OFFSET + 20)
#define     LFS_CMD_CDM_RESET                   (CDM_SERVICE_OFFSET + 21)
#define     LFS_CMD_CDM_TEST_CASH_UNITS         (CDM_SERVICE_OFFSET + 22)
#define     LFS_CMD_CDM_COUNT                   (CDM_SERVICE_OFFSET + 23)

/* CDM 事件 */

#define     LFS_SRVE_CDM_SAFEDOOROPEN           (CDM_SERVICE_OFFSET + 1)
#define     LFS_SRVE_CDM_SAFEDOORCLOSED         (CDM_SERVICE_OFFSET + 2)
#define     LFS_USRE_CDM_CASHUNITTHRESHOLD      (CDM_SERVICE_OFFSET + 3)
#define     LFS_SRVE_CDM_CASHUNITINFOCHANGED    (CDM_SERVICE_OFFSET + 4)
#define     LFS_SRVE_CDM_TELLERINFOCHANGED      (CDM_SERVICE_OFFSET + 5)
#define     LFS_EXEE_CDM_DELAYEDDISPENSE        (CDM_SERVICE_OFFSET + 6)
#define     LFS_EXEE_CDM_STARTDISPENSE          (CDM_SERVICE_OFFSET + 7)
#define     LFS_EXEE_CDM_CASHUNITERROR          (CDM_SERVICE_OFFSET + 8)
#define     LFS_SRVE_CDM_ITEMSTAKEN             (CDM_SERVICE_OFFSET + 9)
#define     LFS_EXEE_CDM_PARTIALDISPENSE        (CDM_SERVICE_OFFSET + 10)
#define     LFS_EXEE_CDM_SUBDISPENSEOK          (CDM_SERVICE_OFFSET + 11)
#define     LFS_SRVE_CDM_ITEMSPRESENTED         (CDM_SERVICE_OFFSET + 13)
#define     LFS_SRVE_CDM_COUNTS_CHANGED         (CDM_SERVICE_OFFSET + 14)
#define     LFS_EXEE_CDM_INCOMPLETEDISPENSE     (CDM_SERVICE_OFFSET + 15)
#define     LFS_EXEE_CDM_NOTEERROR              (CDM_SERVICE_OFFSET + 16)
#define     LFS_SRVE_CDM_MEDIADETECTED          (CDM_SERVICE_OFFSET + 17) 


/* LFSCDMSTATUS.device的取值*/
#define     LFS_CDM_DEVONLINE                   LFS_STAT_DEVONLINE
#define     LFS_CDM_DEVOFFLINE                  LFS_STAT_DEVOFFLINE
#define     LFS_CDM_DEVPOWEROFF                 LFS_STAT_DEVPOWEROFF
#define     LFS_CDM_DEVNODEVICE                 LFS_STAT_DEVNODEVICE
#define     LFS_CDM_DEVHWERROR                  LFS_STAT_DEVHWERROR
#define     LFS_CDM_DEVUSERERROR                LFS_STAT_DEVUSERERROR
#define     LFS_CDM_DEVBUSY                     LFS_STAT_DEVBUSY

/* LFSCDMSTATUS.safe_door的取值 */

#define     LFS_CDM_DOORNOTSUPPORTED            (1)
#define     LFS_CDM_DOOROPEN                    (2)
#define     LFS_CDM_DOORCLOSED                  (3)
#define     LFS_CDM_DOORUNKNOWN                 (5)

/* LFSCDMSTATUS.dispenser的取值 */

#define     LFS_CDM_DISPOK                      (0)
#define     LFS_CDM_DISPCUSTATE                 (1)
#define     LFS_CDM_DISPCUSTOP                  (2)
#define     LFS_CDM_DISPCUUNKNOWN               (3)

/* LFSCDMSTATUS.intermediate_stacker的取值 */

#define     LFS_CDM_ISEMPTY                     (0)
#define     LFS_CDM_ISNOTEMPTY                  (1)
#define     LFS_CDM_ISNOTEMPTYCUST              (2)
#define     LFS_CDM_ISNOTEMPTYUNK               (3)
#define     LFS_CDM_ISUNKNOWN                   (4)
#define     LFS_CDM_ISNOTSUPPORTED              (5)


/* LFSCDMOUTPOS.shutter的取值 */

#define     LFS_CDM_SHTCLOSED                   (0)
#define     LFS_CDM_SHTOPEN                     (1)
#define     LFS_CDM_SHTJAMMED                   (2)
#define     LFS_CDM_SHTUNKNOWN                  (3)
#define     LFS_CDM_SHTNOTSUPPORTED             (4)

/* LFSCDMOUTPOS.position_status的取值 */

#define     LFS_CDM_PSEMPTY                     (0)
#define     LFS_CDM_PSNOTEMPTY                  (1)
#define     LFS_CDM_PSUNKNOWN                   (2)
#define     LFS_CDM_PSNOTSUPPORTED              (3)

/* LFSCDMOUTPOS.transport的取值 */

#define     LFS_CDM_TPOK                        (0)
#define     LFS_CDM_TPINOP                      (1)
#define     LFS_CDM_TPUNKNOWN                   (2)
#define     LFS_CDM_TPNOTSUPPORTED              (3)

/* LFSCDMOUTPOS.transport_status的取值 */

#define     LFS_CDM_TPSTATEMPTY                 (0)
#define     LFS_CDM_TPSTATNOTEMPTY              (1)
#define     LFS_CDM_TPSTATNOTEMPTYCUST          (2)
#define     LFS_CDM_TPSTATNOTEMPTY_UNK          (3)
#define     LFS_CDM_TPSTATNOTSUPPORTED          (4)


/* LFSCDMCAPS.dev_type的取值 */

#define     LFS_CDM_TELLERBILL                  (0)
#define     LFS_CDM_SELFSERVICEBILL             (1)
#define     LFS_CDM_TELLERCOIN                  (2)
#define     LFS_CDM_SELFSERVICECOIN             (3)

/* LFSCDMCAPS.retract_areas的取值 */
/* LFSCDMRETRACT.retract_area的取值 */

#define     LFS_CDM_RA_RETRACT                  (0x0001)
#define     LFS_CDM_RA_TRANSPORT                (0x0002)
#define     LFS_CDM_RA_STACKER                  (0x0004)
#define     LFS_CDM_RA_REJECT                   (0x0008)
#define     LFS_CDM_RA_NOTSUPP                  (0x0010)

/* LFSCDMCAPS.retract_transport_actions的取值 */
/* LFSCDMCAPS.retract_stacker_actions的取值 */

#define     LFS_CDM_PRESENT                     (0x0001)
#define     LFS_CDM_RETRACT                     (0x0002)
#define     LFS_CDM_REJECT                      (0x0004)
#define     LFS_CDM_NOTSUPP                     (0x0008)

/* LFSCDMCAPS.move_items的取值 */

#define     LFS_CDM_FROMCU                      (0x0001)
#define     LFS_CDM_TOCU                        (0x0002)
#define     LFS_CDM_TOTRANSPORT                 (0x0004)

/* LFSCDMCASHUNIT.type的取值 */

#define     LFS_CDM_TYPENA                      (1)
#define     LFS_CDM_TYPEREJECTCASSETTE          (2)
#define     LFS_CDM_TYPEBILLCASSETTE            (3)
#define     LFS_CDM_TYPECOINCYLINDER            (4)
#define     LFS_CDM_TYPECOINDISPENSER           (5)
#define     LFS_CDM_TYPERETRACTCASSETTE         (6)
#define     LFS_CDM_TYPECOUPON                  (7)
#define     LFS_CDM_TYPEDOCUMENT                (8)
#define     LFS_CDM_TYPEREPCONTAINER           (11)
#define     LFS_CDM_TYPERECYCLING              (12)

/* LFSCDMCASHUNIT.status的取值 */

#define     LFS_CDM_STATCUOK                    (0)
#define     LFS_CDM_STATCUFULL                  (1)
#define     LFS_CDM_STATCUHIGH                  (2)
#define     LFS_CDM_STATCULOW                   (3)
#define     LFS_CDM_STATCUEMPTY                 (4)
#define     LFS_CDM_STATCUINOP                  (5)
#define     LFS_CDM_STATCUMISSING               (6)
#define     LFS_CDM_STATCUNOVAL                 (7)
#define     LFS_CDM_STATCUNOREF                 (8)
#define     LFS_CDM_STATCUMANIP                 (9)

/* LFSCDMMIXTYPE.mix_type的取值 */

#define     LFS_CDM_MIXALGORITHM                (1)
#define     LFS_CDM_MIXTABLE                    (2)

/* LFSCDMMIXTYPE.mix_number取值 */

#define     LFS_CDM_INDIVIDUAL                  (0)

/* LFSCDMMIXTYPE.sub_type的取值 (mix algorithms的缺省值) */
#define     LFS_CDM_MIX_MINIMUM_NUMBER_OF_BILLS            (1)
#define     LFS_CDM_MIX_EQUAL_EMPTYING_OF_CASH_UNITS       (2)
#define     LFS_CDM_MIX_MAXIMUM_NUMBER_OF_CASH_UNITS       (3)

/* LFSCDMPRESENTSTATUS.present_state的取值 */

#define     LFS_CDM_PRESENTED                   (1)
#define     LFS_CDM_NOTPRESENTED                (2)
#define     LFS_CDM_UNKNOWN                     (3)

/* LFSCDMDISPENSE.position的取值 */
/* LFSCDMCAPS.positions的取值 */
/* LFSCDMOUTPOS.position的取值 */
/* LFSCDMTELLERPOS.position的取值 */
/* LFSCDMTELLERDETAILS.output_position的取值 */
/* LFSCDMPHYSICALCU.position的取值 */

#define     LFS_CDM_POSNULL                     (0x0000)
#define     LFS_CDM_POSLEFT                     (0x0001)
#define     LFS_CDM_POSRIGHT                    (0x0002)
#define     LFS_CDM_POSCENTER                   (0x0004)
#define     LFS_CDM_POSTOP                      (0x0040)
#define     LFS_CDM_POSBOTTOM                   (0x0080)
#define     LFS_CDM_POSREJECT                   (0x0100)
#define     LFS_CDM_POSFRONT                    (0x0800)
#define     LFS_CDM_POSREAR                     (0x1000)

/* LFSCDMTELLERDETAILS.input_position的取值 */
#define     LFS_CDM_POSINLEFT                   (0x0001)
#define     LFS_CDM_POSINRIGHT                  (0x0002)
#define     LFS_CDM_POSINCENTER                 (0x0004)
#define     LFS_CDM_POSINTOP                    (0x0008)
#define     LFS_CDM_POSINBOTTOM                 (0x0010)
#define     LFS_CDM_POSINFRONT                  (0x0020)
#define     LFS_CDM_POSINREAR                   (0x0040)

/* exchange_type的取值 */
#define     LFS_CDM_EXBYHAND                    (0x0001)
#define     LFS_CDM_EXTOCASSETTES               (0x0002)


/* LFSCDMTELLERUPDATE.action的取值 */

#define     LFS_CDM_CREATE_TELLER               (1)
#define     LFS_CDM_MODIFY_TELLER               (2)
#define     LFS_CDM_DELETE_TELLER               (3)


/* LFSCDMCUERROR.failure的取值 */

#define     LFS_CDM_CASHUNITEMPTY               (1)
#define     LFS_CDM_CASHUNITERROR               (2)
#define     LFS_CDM_CASHUNITFULL                (4)
#define     LFS_CDM_CASHUNITLOCKED              (5)
#define     LFS_CDM_CASHUNITINVALID             (6)
#define     LFS_CDM_CASHUNITCONFIG              (7)


/* LFS_EXEE_CDM_NOTESERROR中reason的取值*/

#define     LFS_CDM_DOUBLENOTEDETECTED          (1)
#define     LFS_CDM_LONGNOTEDETECTED            (2)
#define     LFS_CDM_SKEWEDNOTE                  (3)
#define     LFS_CDM_INCORRECTCOUNT              (4)
#define     LFS_CDM_NOTESTOOCLOSE               (5)


/* LFS CDM 错误码 */ 

#define LFS_ERR_CDM_INVALIDCURRENCY         (-(CDM_SERVICE_OFFSET + 0))
#define LFS_ERR_CDM_INVALIDTELLERID         (-(CDM_SERVICE_OFFSET + 1))
#define LFS_ERR_CDM_CASHUNITERROR           (-(CDM_SERVICE_OFFSET + 2))
#define LFS_ERR_CDM_INVALIDDENOMINATION     (-(CDM_SERVICE_OFFSET + 3))
#define LFS_ERR_CDM_INVALIDMIXNUMBER        (-(CDM_SERVICE_OFFSET + 4))
#define LFS_ERR_CDM_NOCURRENCYMIX           (-(CDM_SERVICE_OFFSET + 5))
#define LFS_ERR_CDM_NOTDISPENSABLE          (-(CDM_SERVICE_OFFSET + 6))
#define LFS_ERR_CDM_TOOMANYITEMS            (-(CDM_SERVICE_OFFSET + 7))
#define LFS_ERR_CDM_UNSUPPOSITION           (-(CDM_SERVICE_OFFSET + 8))
#define LFS_ERR_CDM_SAFEDOOROPEN            (-(CDM_SERVICE_OFFSET + 10))
#define LFS_ERR_CDM_SHUTTERNOTOPEN          (-(CDM_SERVICE_OFFSET + 12))
#define LFS_ERR_CDM_SHUTTEROPEN             (-(CDM_SERVICE_OFFSET + 13))
#define LFS_ERR_CDM_SHUTTERCLOSED           (-(CDM_SERVICE_OFFSET + 14))
#define LFS_ERR_CDM_INVALIDCASHUNIT         (-(CDM_SERVICE_OFFSET + 15))
#define LFS_ERR_CDM_NOITEMS                 (-(CDM_SERVICE_OFFSET + 16))
#define LFS_ERR_CDM_EXCHANGEACTIVE          (-(CDM_SERVICE_OFFSET + 17))
#define LFS_ERR_CDM_NOEXCHANGEACTIVE        (-(CDM_SERVICE_OFFSET + 18))
#define LFS_ERR_CDM_SHUTTERNOTCLOSED        (-(CDM_SERVICE_OFFSET + 19))
#define LFS_ERR_CDM_PRERRORNOITEMS          (-(CDM_SERVICE_OFFSET + 20))
#define LFS_ERR_CDM_PRERRORITEMS            (-(CDM_SERVICE_OFFSET + 21))
#define LFS_ERR_CDM_PRERRORUNKNOWN          (-(CDM_SERVICE_OFFSET + 22))
#define LFS_ERR_CDM_ITEMSTAKEN              (-(CDM_SERVICE_OFFSET + 23))
#define LFS_ERR_CDM_INVALIDMIXTABLE         (-(CDM_SERVICE_OFFSET + 27))
#define LFS_ERR_CDM_OUTPUTPOS_NOT_EMPTY     (-(CDM_SERVICE_OFFSET + 28))
#define LFS_ERR_CDM_INVALIDRETRACTPOSITION  (-(CDM_SERVICE_OFFSET + 29))
#define LFS_ERR_CDM_NOTRETRACTAREA          (-(CDM_SERVICE_OFFSET + 30))
#define LFS_ERR_CDM_NOCASHBOXPRESENT        (-(CDM_SERVICE_OFFSET + 33))
#define LFS_ERR_CDM_AMOUNTNOTINMIXTABLE     (-(CDM_SERVICE_OFFSET + 34))
#define LFS_ERR_CDM_ITEMSNOTTAKEN           (-(CDM_SERVICE_OFFSET + 35))
#define LFS_ERR_CDM_ITEMSLEFT               (-(CDM_SERVICE_OFFSET + 36))


/*=================================================================*/
/* CDM 状态命令结构体 */
/*=================================================================*/

typedef struct _lfs_cdm_position
{
    WORD            position;
    WORD            shutter;
    WORD            position_status;
    WORD            transport;
    WORD            transportStatus;
} LFSCDMOUTPOS, * LPLFSCDMOUTPOS;

typedef struct _lfs_cdm_status
{
    WORD             device;
    WORD             safe_door; 
    WORD             dispenser;
    WORD             intermediate_stacker;
    LPLFSCDMOUTPOS * positions;
    LPSTR            extra;
} LFSCDMSTATUS, * LPLFSCDMSTATUS;

typedef struct _lfs_cdm_caps
{
    WORD            dev_class;
    WORD            dev_type;
    WORD            max_dispense_items;
    BOOL            compound;
    BOOL            shutter;
    BOOL            shutter_control;
    WORD            retract_areas;
    WORD            retract_transport_actions;
    WORD            retract_stacker_actions;
    BOOL            safe_door;
    BOOL            cash_box;
    BOOL            intermediate_stacker;   
    BOOL            items_taken_sensor;
    WORD            positions;
    WORD            move_items;
    WORD            exchange_type;
    LPSTR           extra;
} LFSCDMCAPS, * LPLFSCDMCAPS;

typedef struct _lfs_cdm_physicalcu
{
    LPSTR           physical_position_name;
    CHAR            unit_id[5];
    ULONG           initial_count;
    ULONG           count;
    ULONG           reject_count;
    ULONG           maximum;
    USHORT          p_status;
    BOOL            hardware_sensor;
} LFSCDMPHCU, * LPLFSCDMPHCU;

typedef struct _lfs_cdm_cashunit
{
    USHORT          number;
    USHORT          type;
    LPSTR           cash_unit_name;
    CHAR            unit_id[5];
    CHAR            currency_id[3];
    ULONG           values;
    ULONG           initial_count;
    ULONG           count;
    ULONG           rejectCount;
    ULONG           minimum;
    ULONG           maximum;
    BOOL            app_cock;
    USHORT          status;
    USHORT          num_physical_cus;
    LPLFSCDMPHCU    *physical;
} LFSCDMCASHUNIT, * LPLFSCDMCASHUNIT;

typedef struct _lfs_cdm_cu_info
{
    USHORT          teller_id;
    USHORT          count;
    LPLFSCDMCASHUNIT *list;
} LFSCDMCUINFO, * LPLFSCDMCUINFO;

typedef struct _lfs_cdm_teller_info
{
    USHORT          teller_id;
    CHAR            currency_id[3];
} LFSCDMTELLERINFO, * LPLFSCDMTELLERINFO;

typedef struct _lfs_cdm_teller_totals
{
   char             currency_id[3];
   ULONG            items_received;
   ULONG            items_dispensed;
   ULONG            coins_received;
   ULONG            coins_dispensed;
   ULONG            cash_box_received;
   ULONG            cash_box_dispensed;
} LFSCDMTELLERTOTALS, * LPLFSCDMTELLERTOTALS;

typedef struct _lfs_cdm_teller_details
{
    USHORT                teller_id;
    ULONG                 input_position;
    WORD                  output_position;
    LPLFSCDMTELLERTOTALS *teller_totals;
} LFSCDMTELLERDETAILS, * LPLFSCDMTELLERDETAILS;


typedef struct _lfs_cdm_currency_exp
{
    CHAR            currency_id[3];
    SHORT           exponent;
} LFSCDMCURRENCYEXP, * LPLFSCDMCURRENCYEXP;

typedef struct _lfs_cdm_mix_type
{
    USHORT          mix_number;
    USHORT          mix_type;
    USHORT          sub_type;
    LPSTR           name;
} LFSCDMMIXTYPE, * LPLFSCDMMIXTYPE;

typedef struct _lfs_cdm_mix_row
{
    ULONG           amount;
    LPUSHORT        mixture;
} LFSCDMMIXROW, * LPLFSCDMMIXROW;

typedef struct _lfs_cdm_mix_table
{
    USHORT          mix_number;
    LPSTR           name;
    USHORT          rows;
    USHORT          cols;
    LPULONG         mix_header;
    LPLFSCDMMIXROW  *mix_rows;
} LFSCDMMIXTABLE, * LPLFSCDMMIXTABLE;

typedef struct _LFS_cdm_denomination
{
    CHAR            currency_id[3];
    ULONG           amount;
    USHORT          count;
    LPULONG         values;
    ULONG           cash_box;
} LFSCDMDENOMINATION, * LPLFSCDMDENOMINATION;
                         
typedef struct _lfs_cdm_present_status
{
    LPLFSCDMDENOMINATION  denomination;
    WORD                  present_state;
    LPSTR                 extra;
} LFSCDMPRESENTSTATUS, * LPLFSCDMPRESENTSTATUS;


/*=================================================================*/
/* CDM执行命令结构体*/
/*=================================================================*/

typedef struct _LFS_cdm_denominate
{
    USHORT                teller_id;
    USHORT                mix_number;
    LPLFSCDMDENOMINATION  denomination;
} LFSCDMDENOMINATE, * LPLFSCDMDENOMINATE;

typedef struct _lfs_cdm_dispense
{
    USHORT                teller_id;
    USHORT                mix_number;
    WORD                  position;
    BOOL                  present;
    LPLFSCDMDENOMINATION  denomination;
} LFSCDMDISPENSE, * LPLFSCDMDISPENSE;

typedef struct _lfs_cdm_physical_cu
{
    BOOL      empty_all;
    WORD      position;
    LPSTR     physical_position_name;
} LFSCDMPHYSICALCU, *LPLFSCDMPHYSICALCU;

typedef struct _lfs_cdm_counted_phys_cu
{
    LPSTR        physical_position_name;
    CHAR         unit_id[5];
    ULONG        dispensed;
    ULONG        counted;
    USHORT       p_status;
} LFSCDMCOUNTEDPHYSCU, *LPLFSCDMCOUNTEDPHYSCU;

typedef struct _lfs_cdm_count
{
    USHORT                  num_physical_cus;
    LPLFSCDMCOUNTEDPHYSCU  *counted_phys_cus;
} LFSCDMCOUNT, *LPLFSCDMCOUNT;


typedef struct _lfs_cdm_retract
{
    WORD                  output_position;
    USHORT                retract_area;
    USHORT                index;
} LFSCDMRETRACT, * LPLFSCDMRETRACT;

typedef struct _lfs_cdm_teller_update
{
    USHORT                action;
    LPLFSCDMTELLERDETAILS teller_details;
} LFSCDMTELLERUPDATE, * LPLFSCDMTELLERUPDATE;


typedef struct _lfs_cdm_start_ex
{
    WORD             exchange_type;
    USHORT           teller_id;
    USHORT           count;
    LPUSHORT         cu_num_list;
} LFSCDMSTARTEX, * LPLFSCDMSTARTEX;

typedef struct _lfs_cdm_itemposition
{
    USHORT             number;
    LPLFSCDMRETRACT    retract_area;
    WORD               output_position;
} LFSCDMITEMPOSITION, * LPLFSCDMITEMPOSITION;

typedef struct _lfs_cdm_calibrate
{
    USHORT                   number;
    USHORT                   num_of_bills;
    LPLFSCDMITEMPOSITION    *position;
} LFSCDMCALIBRATE, * LPLFSCDMCALIBRATE;


/*=================================================================*/
/* CDM事件结构体 */
/*=================================================================*/

typedef struct _lfs_cdm_cu_error
{
    WORD              failure;
    LPLFSCDMCASHUNIT  cash_unit;
} LFSCDMCUERROR, * LPLFSCDMCUERROR;

typedef struct _lfs_cdm_counts_changed
{
    USHORT            count;
    USHORT           *cu_num_list;
} LFSCDMCOUNTSCHANGED, * LPLFSCDMCOUNTSCHANGED;


/* 恢复原有字节的对齐方式 */
#pragma pack (pop)

#ifdef __cplusplus
}       /*extern "C"*/
#endif

#endif  /* __INC_LFSCDM__H */

