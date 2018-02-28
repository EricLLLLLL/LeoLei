/******************************************************************************
*                                                                             *
* lfscim.h      LFS - Cash Acceptor (CIM) definitions                         *
*                                                                             *
*               Version 1.00 (2015-07-02)                                     *
*                                                                             *
******************************************************************************/

#ifndef __INC_LFSCIM__H
#define __INC_LFSCIM__H

#ifdef __cplusplus
extern "C" {
#endif

#include <lfsapi.h>

/* 对齐方式 */
#pragma pack (push, 1)

/* LFSCIMCAPS.dev_class取值 */

#define     LFS_SERVICE_CLASS_CIM               (9)
#define     LFS_SERVICE_CLASS_VERSION_CIM       0x0003
#define     LFS_SERVICE_CLASS_NAME_CIM          "CIM"
#define     CIM_SERVICE_OFFSET                  (LFS_SERVICE_CLASS_CIM * 100)

/* CIM 信息指令 */

#define     LFS_INF_CIM_STATUS                  (CIM_SERVICE_OFFSET + 1)
#define     LFS_INF_CIM_CAPABILITIES            (CIM_SERVICE_OFFSET + 2)
#define     LFS_INF_CIM_CASH_UNIT_INFO          (CIM_SERVICE_OFFSET + 3)
#define     LFS_INF_CIM_TELLER_INFO             (CIM_SERVICE_OFFSET + 4)
#define     LFS_INF_CIM_CURRENCY_EXP            (CIM_SERVICE_OFFSET + 5)
#define     LFS_INF_CIM_BANKNOTE_TYPES          (CIM_SERVICE_OFFSET + 6)
#define     LFS_INF_CIM_CASH_IN_STATUS          (CIM_SERVICE_OFFSET + 7)

/* CIM 执行指令 */

#define     LFS_CMD_CIM_CASH_IN_START           (CIM_SERVICE_OFFSET + 1)
#define     LFS_CMD_CIM_CASH_IN                 (CIM_SERVICE_OFFSET + 2)
#define     LFS_CMD_CIM_CASH_IN_END             (CIM_SERVICE_OFFSET + 3)
#define     LFS_CMD_CIM_CASH_IN_ROLLBACK        (CIM_SERVICE_OFFSET + 4)
#define     LFS_CMD_CIM_RETRACT                 (CIM_SERVICE_OFFSET + 5)
#define     LFS_CMD_CIM_OPEN_SHUTTER            (CIM_SERVICE_OFFSET + 6)
#define     LFS_CMD_CIM_CLOSE_SHUTTER           (CIM_SERVICE_OFFSET + 7)
#define     LFS_CMD_CIM_SET_TELLER_INFO         (CIM_SERVICE_OFFSET + 8)
#define     LFS_CMD_CIM_SET_CASH_UNIT_INFO      (CIM_SERVICE_OFFSET + 9)
#define     LFS_CMD_CIM_START_EXCHANGE          (CIM_SERVICE_OFFSET + 10)
#define     LFS_CMD_CIM_END_EXCHANGE            (CIM_SERVICE_OFFSET + 11)
#define     LFS_CMD_CIM_OPEN_SAFE_DOOR          (CIM_SERVICE_OFFSET + 12)
#define     LFS_CMD_CIM_RESET                   (CIM_SERVICE_OFFSET + 13)
#define     LFS_CMD_CIM_CONFIGURE_CASH_IN_UNITS (CIM_SERVICE_OFFSET + 14)
#define     LFS_CMD_CIM_CONFIGURE_NOTETYPES     (CIM_SERVICE_OFFSET + 15)


/* CIM 事件 */

#define     LFS_SRVE_CIM_SAFEDOOROPEN           (CIM_SERVICE_OFFSET + 1)
#define     LFS_SRVE_CIM_SAFEDOORCLOSED         (CIM_SERVICE_OFFSET + 2)
#define     LFS_USRE_CIM_CASHUNITTHRESHOLD      (CIM_SERVICE_OFFSET + 3)
#define     LFS_SRVE_CIM_CASHUNITINFOCHANGED    (CIM_SERVICE_OFFSET + 4)
#define     LFS_SRVE_CIM_TELLERINFOCHANGED      (CIM_SERVICE_OFFSET + 5)
#define     LFS_EXEE_CIM_CASHUNITERROR          (CIM_SERVICE_OFFSET + 6)
#define     LFS_SRVE_CIM_ITEMSTAKEN             (CIM_SERVICE_OFFSET + 7)
#define     LFS_SRVE_CIM_COUNTS_CHANGED         (CIM_SERVICE_OFFSET + 8)
#define     LFS_EXEE_CIM_INPUTREFUSE            (CIM_SERVICE_OFFSET + 9)
#define     LFS_SRVE_CIM_ITEMSPRESENTED         (CIM_SERVICE_OFFSET + 10)
#define     LFS_SRVE_CIM_ITEMSINSERTED          (CIM_SERVICE_OFFSET + 11)
#define     LFS_EXEE_CIM_NOTEERROR              (CIM_SERVICE_OFFSET + 12)
#define     LFS_SRVE_CIM_MEDIADETECTED          (CIM_SERVICE_OFFSET + 14)


/* LFSCIMSTATUS.device取值 */

#define     LFS_CIM_DEVONLINE                   LFS_STAT_DEVONLINE
#define     LFS_CIM_DEVOFFLINE                  LFS_STAT_DEVOFFLINE
#define     LFS_CIM_DEVPOWEROFF                 LFS_STAT_DEVPOWEROFF
#define     LFS_CIM_DEVNODEVICE                 LFS_STAT_DEVNODEVICE
#define     LFS_CIM_DEVUSERERROR                LFS_STAT_DEVUSERERROR
#define     LFS_CIM_DEVHWERROR                  LFS_STAT_DEVHWERROR
#define     LFS_CIM_DEVBUSY                     LFS_STAT_DEVBUSY


/* LFSCIMSTATUS.safe_door取值 */

#define     LFS_CIM_DOORNOTSUPPORTED           	(1)
#define     LFS_CIM_DOOROPEN                    (2)
#define     LFS_CIM_DOORCLOSED                  (3)
#define     LFS_CIM_DOORUNKNOWN                 (4)

/* LFSCIMSTATUS.acceptor取值 */

#define     LFS_CIM_ACCOK                       (0)
#define     LFS_CIM_ACCCUSTATE                  (1)
#define     LFS_CIM_ACCCUSTOP                   (2)
#define     LFS_CIM_ACCCUUNKNOWN                (3)

/* LFSCIMSTATUS.intermediate_stacker取值*/

#define     LFS_CIM_ISEMPTY                     (0)
#define     LFS_CIM_ISNOTEMPTY                  (1)
#define     LFS_CIM_ISFULL                      (2)
#define     LFS_CIM_ISUNKNOWN                   (4)
#define     LFS_CIM_ISNOTSUPPORTED              (5)


/* LFSCIMSTATUS.stacker_items取值 */

#define     LFS_CIM_CUSTOMERACCESS              (0)
#define     LFS_CIM_NOCUSTOMERACCESS            (1)
#define     LFS_CIM_ACCESSUNKNOWN               (2)
#define     LFS_CIM_NOITEMS                     (4)

/* LFSCIMSTATUS.banknote_reader取值 */

#define     LFS_CIM_BNROK                       (0)
#define     LFS_CIM_BNRINOP                     (1)
#define     LFS_CIM_BNRUNKNOWN                  (2)
#define     LFS_CIM_BNRNOTSUPPORTED             (3)

/* LFSCIMSTATUS.shutter取值 */

#define     LFS_CIM_SHTCLOSED                   (0)
#define     LFS_CIM_SHTOPEN                     (1)
#define     LFS_CIM_SHTJAMMED                   (2)
#define     LFS_CIM_SHTUNKNOWN                  (3)
#define     LFS_CIM_SHTNOTSUPPORTED             (4)

/* LFSCIMINPOS.position_status取值 */

#define     LFS_CIM_PSEMPTY                     (0)
#define     LFS_CIM_PSNOTEMPTY                  (1)
#define     LFS_CIM_PSUNKNOWN                   (2)
#define     LFS_CIM_PSNOTSUPPORTED              (3)

/* LFSCIMSTATUS.transport取值 */

#define     LFS_CIM_TPOK                        (0)
#define     LFS_CIM_TPINOP                      (1)
#define     LFS_CIM_TPUNKNOWN                   (2)
#define     LFS_CIM_TPNOTSUPPORTED              (3)

/* 取值LFSCIMINPOS.transport_status取值 */

#define     LFS_CIM_TPSTATEMPTY                 (0)
#define     LFS_CIM_TPSTATNOTEMPTY              (1)
#define     LFS_CIM_TPSTATNOTEMPTYCUST          (2)
#define     LFS_CIM_TPSTATNOTEMPTY_UNK          (3)
#define     LFS_CIM_TPSTATNOTSUPPORTED          (4)

/* LFSCIMCAPS.dev_type取值 */

#define     LFS_CIM_TELLERBILL                  (0)
#define     LFS_CIM_SELFSERVICEBILL             (1)
#define     LFS_CIM_TELLERCOIN                  (2)
#define     LFS_CIM_SELFSERVICECOIN             (3)

/* LFSCIMCAPS.exchange_type取值 */
/* LFSCIMSTARTEX.exchange_type取值 */

#define     LFS_CIM_EXBYHAND                    (0x0001)
#define     LFS_CIM_EXTOCASSETTES               (0x0002)
#define     LFS_CIM_CLEARRECYCLER               (0x0004)
#define     LFS_CIM_DEPOSITINTO                 (0x0008)



/* LFSCIMCAPS.retract_transport_actions取值 */
/* LFSCIMCAPS.retract_stacker_actions取值 */

#define     LFS_CIM_PRESENT                     (0x0001)
#define     LFS_CIM_RETRACT                     (0x0002)
#define     LFS_CIM_NOTSUPP                     (0x0004)

/* LFSCIMCASHIN.type取值 */

#define     LFS_CIM_TYPERECYCLING               (1)
#define     LFS_CIM_TYPECASHIN                  (2)
#define     LFS_CIM_TYPEREPCONTAINER            (3)
#define     LFS_CIM_TYPERETRACTCASSETTE         (4)

/* LFSCIMCASHIN.item_type取值 */
/* LFSCIMCASHINTYPE.type取值 */

#define     LFS_CIM_CITYPALL                    (0x0001)
#define     LFS_CIM_CITYPUNFIT                  (0x0002)
#define     LFS_CIM_CITYPINDIVIDUAL             (0x0004)

/* LFSCIMCASHIN.status取值 */
/* LFSCIMPHCU.p_status取值 */

#define     LFS_CIM_STATCUOK                    (0)
#define     LFS_CIM_STATCUFULL                  (1)
#define     LFS_CIM_STATCUHIGH                  (2)
#define     LFS_CIM_STATCULOW                   (3)
#define     LFS_CIM_STATCUEMPTY                 (4)
#define     LFS_CIM_STATCUINOP                  (5)
#define     LFS_CIM_STATCUMISSING               (6)
#define     LFS_CIM_STATCUNOVAL                 (7)
#define     LFS_CIM_STATCUNOREF                 (8)
#define     LFS_CIM_STATCUMANIP                 (9)


/* LFSCIMSTATUS.positions取值 */
/* LFSCIMCAPS.positions取值 */
/* LFSCIMINPOS.position取值 */
/* LFSCIMTELLERDETAILS.input_position取值 */ 
/* LFSCIMCASHINSTART.input_position取值 */ 


#define     LFS_CIM_POSNULL                       (0x0000)
#define     LFS_CIM_POSINLEFT                     (0x0001)
#define     LFS_CIM_POSINRIGHT                    (0x0002)
#define     LFS_CIM_POSINCENTER                   (0x0004)
#define     LFS_CIM_POSINTOP                      (0x0008)
#define     LFS_CIM_POSINBOTTOM                   (0x0010)
#define     LFS_CIM_POSINFRONT                    (0x0020)
#define     LFS_CIM_POSINREAR                     (0x0040)


/* LFSCIMSTATUS.positions取值 */
/* LFSCIMCAPS.positions取值 */
/* LFSCIMTELLERDETAILS.output_position取值 */ 
/* LFSCIMCASHINSTART.output_position取值 */ 
/* LFSCIMOUTPUT.position取值 */

#define     LFS_CIM_POSOUTLEFT                     (0x0080)
#define     LFS_CIM_POSOUTRIGHT                    (0x0100)
#define     LFS_CIM_POSOUTCENTER                   (0x0200)
#define     LFS_CIM_POSOUTTOP                      (0x0400)
#define     LFS_CIM_POSOUTBOTTOM                   (0x0800)
#define     LFS_CIM_POSOUTFRONT                    (0x1000)
#define     LFS_CIM_POSOUTREAR                     (0x2000)

/* LFSCIMCASHINSTATUS.status取值 */

#define     LFS_CIM_CIOK                           (0)
#define     LFS_CIM_CIROLLBACK                     (1)
#define     LFS_CIM_CIACTIVE                       (2)
#define     LFS_CIM_CIRETRACT                      (3)
#define     LFS_CIM_CIUNKNOWN                      (4)


/* LFSCIMCAPS.retract_areas取值 */
/* LFSCIMRETRACT.retract_area取值 */

#define     LFS_CIM_RA_RETRACT                     (0x0001)
#define     LFS_CIM_RA_TRANSPORT                   (0x0002)
#define     LFS_CIM_RA_STACKER                     (0x0004)
#define     LFS_CIM_RA_BILLCASSETTES               (0x0008)
#define     LFS_CIM_RA_NOTSUPP                     (0x0010)
		


/* 纸币level取值 */
#define LFS_CIM_LEVEL_1 (1)
#define LFS_CIM_LEVEL_2 (2)
#define LFS_CIM_LEVEL_3 (3)
#define LFS_CIM_LEVEL_4 (4)


/* LFSCIMTELLERUPDATE.action取值 */

#define     LFS_CIM_CREATE_TELLER                  (1)
#define     LFS_CIM_MODIFY_TELLER                  (2)
#define     LFS_CIM_DELETE_TELLER                  (3)


/* LFSCIMCUERROR.failure取值 */

#define     LFS_CIM_CASHUNITEMPTY                  (1)
#define     LFS_CIM_CASHUNITERROR                  (2)
#define     LFS_CIM_CASHUNITFULL                   (3)
#define     LFS_CIM_CASHUNITLOCKED                 (4)
#define     LFS_CIM_CASHUNITNOTCONF                (5)
#define     LFS_CIM_CASHUNITINVALID                (6)
#define     LFS_CIM_CASHUNITCONFIG                 (7)
#define     LFS_CIM_FEEDMODULEPROBLEM              (8)

/* LFS_EXEE_CIM_INPUTREFUSE事件发生原因 */

#define     LFS_CIM_CASHINUNITFULL              (1)
#define     LFS_CIM_INVALIDBILL                 (2)
#define     LFS_CIM_NOBILLSTODEPOSIT            (3)
#define     LFS_CIM_DEPOSITFAILURE              (4)
#define     LFS_CIM_COMMINPCOMPFAILURE          (5)
#define     LFS_CIM_STACKERFULL                 (6)

/* LFS_EXEE_CIM_NOTESERROR事件发生原因 */

#define     LFS_CIM_DOUBLENOTEDETECTED          (1)	
#define     LFS_CIM_LONGNOTEDETECTED            (2)	
#define     LFS_CIM_SKEWEDNOTE                  (3)	
#define     LFS_CIM_INCORRECTCOUNT              (4)	
#define     LFS_CIM_NOTESTOOCLOSE               (5)	

/* 错误码 */ 

#define LFS_ERR_CIM_INVALIDCURRENCY         (-(CIM_SERVICE_OFFSET + 0))
#define LFS_ERR_CIM_INVALIDTELLERID         (-(CIM_SERVICE_OFFSET + 1))
#define LFS_ERR_CIM_CASHUNITERROR           (-(CIM_SERVICE_OFFSET + 2))
#define LFS_ERR_CIM_TOOMANYITEMS            (-(CIM_SERVICE_OFFSET + 7))
#define LFS_ERR_CIM_UNSUPPOSITION           (-(CIM_SERVICE_OFFSET + 8))
#define LFS_ERR_CIM_SAFEDOOROPEN            (-(CIM_SERVICE_OFFSET + 10))
#define LFS_ERR_CIM_SHUTTERNOTOPEN          (-(CIM_SERVICE_OFFSET + 12))
#define LFS_ERR_CIM_SHUTTEROPEN             (-(CIM_SERVICE_OFFSET + 13))
#define LFS_ERR_CIM_SHUTTERCLOSED           (-(CIM_SERVICE_OFFSET + 14))
#define LFS_ERR_CIM_INVALIDCASHUNIT         (-(CIM_SERVICE_OFFSET + 15))
#define LFS_ERR_CIM_NOITEMS                 (-(CIM_SERVICE_OFFSET + 16))
#define LFS_ERR_CIM_EXCHANGEACTIVE          (-(CIM_SERVICE_OFFSET + 17))
#define LFS_ERR_CIM_NOEXCHANGEACTIVE        (-(CIM_SERVICE_OFFSET + 18))
#define LFS_ERR_CIM_SHUTTERNOTCLOSED        (-(CIM_SERVICE_OFFSET + 19))
#define LFS_ERR_CIM_ITEMSTAKEN              (-(CIM_SERVICE_OFFSET + 23))
#define LFS_ERR_CIM_CASHINACTIVE            (-(CIM_SERVICE_OFFSET + 25))
#define LFS_ERR_CIM_NOCASHINACTIVE          (-(CIM_SERVICE_OFFSET + 26))
#define LFS_ERR_CIM_POSITION_NOT_EMPTY      (-(CIM_SERVICE_OFFSET + 28))
#define LFS_ERR_CIM_INVALIDRETRACTPOSITION  (-(CIM_SERVICE_OFFSET + 34))
#define LFS_ERR_CIM_NOTRETRACTAREA          (-(CIM_SERVICE_OFFSET + 35))

/*=================================================================*/
/* CIM 信息指令结构体 */
/*=================================================================*/

typedef struct _lfs_cim_inpos
{
    WORD            position;
    WORD            shutter;
    WORD            position_status;
    WORD            transport;
    WORD            transport_status;
} LFSCIMINPOS, * LPLFSCIMINPOS;

typedef struct _lfs_cim_status
{
    WORD             device;
    WORD             safe_door; 
    WORD             acceptor;
    WORD             intermediate_stacker; 
    WORD             stacker_items;
    WORD             banknote_reader;
    BOOL             drop_box;
    LPLFSCIMINPOS *  positions;
    LPSTR            extra;
} LFSCIMSTATUS, * LPLFSCIMSTATUS;

typedef struct _lfs_cim_caps
{
    WORD            dev_class;
    WORD            dev_type;
    WORD            max_cashin_items;
    BOOL            compound;
    BOOL            shutter; 
    BOOL            shutter_control;
    BOOL            safe_door; 
    BOOL            cash_box;
    BOOL            refill;
    WORD            intermediate_stacker;
    BOOL            items_taken_sensor;
    BOOL            items_inserted_sensor;
    WORD            positions;
    WORD            exchange_type;
    WORD            retract_areas;
    WORD            retract_transport_actions;
    WORD            retract_stacker_actions;
    LPSTR           extra;
} LFSCIMCAPS, * LPLFSCIMCAPS;

typedef struct _lfs_cim_physicalcu
{
    LPSTR           physical_position_name;
    CHAR            unit_id [5];
    ULONG           cash_in_count;
    ULONG           count;
    ULONG           maximum;
    USHORT          p_status;
    BOOL            hardware_sensors;
    LPSTR           extra;
} LFSCIMPHCU, * LPLFSCIMPHCU;

typedef struct _lfs_cim_note_number
{
    USHORT          note_id;
    ULONG           count;
} LFSCIMNOTENUMBER, * LPLFSCIMNOTENUMBER;

typedef struct _lfs_cim_note_number_list
{
    USHORT                num_of_note_numbers;
    LPLFSCIMNOTENUMBER    *note_number;
} LFSCIMNOTENUMBERLIST, * LPLFSCIMNOTENUMBERLIST;

typedef struct _lfs_cim_cash_in
{
    USHORT              number;						
    DWORD               type;							
    DWORD               item_type;							
    CHAR                unit_id[5];							
    CHAR                currency_id[3];							
    ULONG               values;							
    ULONG               cash_in_count;							
    ULONG               count; 							
    ULONG               maximum;							
    USHORT              status;							
	BOOL                app_lock;
	LPLFSCIMNOTENUMBERLIST  note_number_list;
	USHORT                  num_physical_cus;
	LPLFSCIMPHCU *      physical;
    LPSTR               extra;
} LFSCIMCASHIN, * LPLFSCIMCASHIN;


typedef struct _lfs_cim_cash_info
{
    USHORT              cashin_count;
    LPLFSCIMCASHIN     *cash_in;
} LFSCIMCASHINFO, * LPLFSCIMCASHINFO;

typedef struct _lfs_cim_teller_info
{
    USHORT          teller_id;
    CHAR            currency_id[3];
} LFSCIMTELLERINFO, * LPLFSCIMTELLERINFO;

typedef struct _lfs_cim_teller_totals
{
   CHAR             currency_id[3];
   ULONG            items_received;
   ULONG            items_dispensed;
   ULONG            coins_received;
   ULONG            coins_dispensed;
   ULONG            cash_box_received;
   ULONG            cash_box_dispensed;
} LFSCIMTELLERTOTALS, * LPLFSCIMTELLERTOTALS;

typedef struct _lfs_cim_teller_details
{
    USHORT          teller_id;
    WORD            input_position;
    WORD            putput_position;
    LPLFSCIMTELLERTOTALS * teller_totals;
} LFSCIMTELLERDETAILS, * LPLFSCIMTELLERDETAILS;



typedef struct _lfs_cim_currency_exp
{
    CHAR            currency_id[3];
    SHORT           exponent;
} LFSCIMCURRENCYEXP, * LPLFSCIMCURRENCYEXP;


typedef struct _lfs_cim_note_type
{
    USHORT          note_id;
    CHAR            currency_id[3];
    ULONG           values;
    USHORT          release;
    BOOL            configured;
} LFSCIMNOTETYPE, * LPLFSCIMNOTETYPE;

typedef struct _lfs_cim_note_type_list
{
    USHORT              num_of_note_types;
    LPLFSCIMNOTETYPE  * note_types;
} LFSCIMNOTETYPELIST, * LPLFSCIMNOTETYPELIST;


typedef struct _lfs_cim_cash_in_status
{
    WORD                    status;
    USHORT                  num_of_refused;
    LPLFSCIMNOTENUMBERLIST  note_number_list;
    LPSTR                   extra;
} LFSCIMCASHINSTATUS, * LPLFSCIMCASHINSTATUS;


/*=================================================================*/
/* CIM 执行指令结构体 */
/*=================================================================*/


typedef struct _lfs_cim_cash_in_start
{
    USHORT               teller_id;
    BOOL                 use_recycle_units;
    WORD                 output_position;
    WORD                 input_position;
} LFSCIMCASHINSTART, * LPLFSCIMCASHINSTART;

typedef struct _lfs_cim_retract
{
    WORD             output_position;
    USHORT           retract_area;
    USHORT           index;
} LFSCIMRETRACT, * LPLFSCIMRETRACT;

typedef struct _lfs_cim_teller_update
{
    USHORT                      action;
    LPLFSCIMTELLERDETAILS    	teller_details;
} LFSCIMTELLERUPDATE,   * LPLFSCIMTELLERUPDATE;


typedef struct _lfs_cim_output
{
    USHORT           logical_number;
    WORD             position;
    USHORT           number;
} LFSCIMOUTPUT, * LPLFSCIMOUTPUT;

typedef struct _lfs_cim_start_ex
{
    WORD             exchange_type;
    USHORT           teller_id;
    USHORT           count;
    LPUSHORT         cu_numList;
    LPLFSCIMOUTPUT   output;
} LFSCIMSTARTEX, * LPLFSCIMSTARTEX;

typedef struct _lfs_cim_itemposition
{
    USHORT             number;
    LPLFSCIMRETRACT    retract_area;
    WORD               output_position;
} LFSCIMITEMPOSITION, * LPLFSCIMITEMPOSITION;


typedef struct _lfs_cim_cash_in_type
{
    USHORT             number;
    DWORD              type;
    LPUSHORT           note_ids;
} LFSCIMCASHINTYPE, * LPLFSCIMCASHINTYPE;


/*=================================================================*/
/* CIM 事件结构体 */
/*=================================================================*/

typedef struct _lfs_cim_cu_error
{
    WORD                failure;
    LPLFSCIMCASHIN      cash_unit;
} LFSCIMCUERROR, * LPLFSCIMCUERROR;

typedef struct _lfs_cim_counts_changed
{
    USHORT            count;
    USHORT           *cu_num_list;
} LFSCIMCOUNTSCHANGED, * LPLFSCIMCOUNTSCHANGED;

/* 恢复原有字节的对齐方式 */
#pragma pack (pop)

#ifdef __cplusplus
}       /*extern "C"*/
#endif

#endif  /* __INC_LFSCIM */
