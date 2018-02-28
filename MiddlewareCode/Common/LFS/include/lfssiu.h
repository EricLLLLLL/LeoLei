/******************************************************************************
*                                                                             *
* lfssiu.h      LFS - 传感器（SIU）定义           							  *
*                                                                             *
*               版本 1.00 (2015-07-02)                                        *
*                                                                             *
******************************************************************************/
 
#ifndef __INC_LFSSIU__H
#define __INC_LFSSIU__H

#ifdef __cplusplus
extern "C" {
#endif

#include <lfsapi.h>

/* 对齐方式 */
#pragma pack (push, 1)

/* LFSSIUCAPS.dev_lass 取值 */

#define     LFS_SERVICE_CLASS_SIU               (2)

#define     LFS_SERVICE_CLASS_NAME_SIU          "SIU"
#define     LFS_SERVICE_CLASS_VERSION_SIU       0x0001

#define     SIU_SERVICE_OFFSET                  (LFS_SERVICE_CLASS_SIU * 100)


/* 信息指令 */

#define     LFS_INF_SIU_STATUS                  (SIU_SERVICE_OFFSET + 1)
#define     LFS_INF_SIU_CAPABILITIES            (SIU_SERVICE_OFFSET + 2)


/* 执行指令 */

#define     LFS_CMD_SIU_ENABLE_EVENTS           (SIU_SERVICE_OFFSET + 1)
#define     LFS_CMD_SIU_SET_PORTS               (SIU_SERVICE_OFFSET + 2)
#define     LFS_CMD_SIU_SET_DOOR                (SIU_SERVICE_OFFSET + 3)
#define     LFS_CMD_SIU_SET_INDICATOR           (SIU_SERVICE_OFFSET + 4)
#define     LFS_CMD_SIU_SET_AUXILIARY           (SIU_SERVICE_OFFSET + 5)
#define     LFS_CMD_SIU_SET_GUIDLIGHT           (SIU_SERVICE_OFFSET + 6)
#define     LFS_CMD_SIU_RESET                   (SIU_SERVICE_OFFSET + 7)


/* 事件 */

#define     LFS_SRVE_SIU_PORT_STATUS            (SIU_SERVICE_OFFSET + 1)
#define     LFS_EXEE_SIU_PORT_ERROR             (SIU_SERVICE_OFFSET + 2)


/* LFSSIUSTATUS.device 取值 */

#define     LFS_SIU_DEVONLINE                   LFS_STAT_DEVONLINE
#define     LFS_SIU_DEVOFFLINE                  LFS_STAT_DEVOFFLINE
#define     LFS_SIU_DEVPOWEROFF                 LFS_STAT_DEVPOWEROFF
#define     LFS_SIU_DEVNODEVICE                 LFS_STAT_DEVNODEVICE
#define     LFS_SIU_DEVHWERROR                  LFS_STAT_DEVHWERROR
#define     LFS_SIU_DEVUSERERROR                LFS_STAT_DEVUSERERROR
#define     LFS_SIU_DEVBUSY                     LFS_STAT_DEVBUSY


/* sensors 数组的大小和下标上限 */

#define     LFS_SIU_SENSORS_SIZE                (32)
#define     LFS_SIU_SENSORS_MAX                 (LFS_SIU_SENSORS_SIZE - 1)


/* doors 数组的大小和下标上限 */

#define     LFS_SIU_DOORS_SIZE                  (16)
#define     LFS_SIU_DOORS_MAX                   (LFS_SIU_DOORS_SIZE - 1)


/* indicators 数组的大小和下标上限 */

#define     LFS_SIU_INDICATORS_SIZE             (16)
#define     LFS_SIU_INDICATORS_MAX              (LFS_SIU_INDICATORS_SIZE - 1)


/* auxiliaries 数组的大小和下标上限 */

#define     LFS_SIU_AUXILIARIES_SIZE            (16)
#define     LFS_SIU_AUXILIARIES_MAX             (LFS_SIU_AUXILIARIES_SIZE - 1)


/* guid_lights 数组的大小和下标上限 */

#define     LFS_SIU_GUIDLIGHTS_SIZE             (16)
#define     LFS_SIU_GUIDLIGHTS_MAX              (LFS_SIU_GUIDLIGHTS_SIZE - 1)


/* LFSSIUSTATUS.sensors [...]
          LFSSIUCAPS.sensors [...]
          LFSSIUENABLE.sensors [...]
          LFSSIUPORTEVENT.port_index
          LFSSIUPORTERROR.port_index 
		  等数组的下标 */

#define     LFS_SIU_OPERATORSWITCH              (0)
#define     LFS_SIU_TAMPER                      (1)
#define     LFS_SIU_INTTAMPER                   (2)
#define     LFS_SIU_SEISMIC                     (3)
#define     LFS_SIU_HEAT                        (4)
#define     LFS_SIU_PROXIMITY                   (5)
#define     LFS_SIU_AMBLIGHT                    (6)
#define     LFS_SIU_ENHANCEDAUDIO               (7)



/* LFSSIUSTATUS.doors [...]
           LFSSIUCAPS.doors [...]
           LFSSIUENABLE.doors [...]
           LFSSIUSETPORT.doors [...]
           LFSSIUSETDOORS.door
           LFSSIUPORTEVENT.port_index
           LFSSIUPORTERROR.port_index 
		   等数组的下标 */

#define     LFS_SIU_CABINET                     (0)
#define     LFS_SIU_SAFE                        (1)
#define     LFS_SIU_VANDALSHIELD                (2)


/* LFSSIUSTATUS.indicators [...]
           LFSSIUCAPS.indicators [...]
           LFSSIUENABLE.indicators [...]
           LFSSIUSETPORT.indicators [...]
           LFSSIUSETINDICATORS.indicator
           LFSSIUPORTEVENT.port_index
           LFSSIUPORTERROR.port_index 
		   等数组的下标 */

#define     LFS_SIU_OPENCLOSE                   (0)
#define     LFS_SIU_FASCIALIGHT                 (1)
#define     LFS_SIU_AUDIO                       (2)
#define     LFS_SIU_HEATING                     (3)


/* LFSSIUSTATUS.auxiliaries [...]
           LFSSIUCAPS.auxiliaries [...]
           LFSSIUENABLE.auxiliaries [...]
           LFSSIUSETPORT.auxiliaries [...]
           LFSSIUSETAUXILIARIES.auxiliary
           LFSSIUPORTEVENT.port_index
           LFSSIUPORTERROR.port_index 
		   等数组的下标 */

#define     LFS_SIU_VOLUME                      (0)
#define     LFS_SIU_UPS                         (1)
#define     LFS_SIU_REMOTE_STATUS_MONITOR       (2)
#define     LFS_SIU_AUDIBLE_ALARM               (3)
#define     LFS_SIU_ENHANCEDAUDIOCONTROL        (4)


/* LFSSIUSTATUS.guid_lights [...]
           LFSSIUCAPS.guid_lights [...]
           LFSSIUENABLE.guid_lights [...]
           LFSSIUSETPORT.guid_lights [...]
           LFSSIUSETGUIDLIGHTS.guid_light
           LFSSIUPORTEVENT.port_index
           LFSSIUPORTERROR.port_index 
		   等数组的下标 */

#define     LFS_SIU_CARDUNIT                    (0)
#define     LFS_SIU_PINPAD                      (1)
#define     LFS_SIU_NOTESDISPENSER              (2)
#define     LFS_SIU_COINDISPENSER               (3)
#define     LFS_SIU_RECEIPTPRINTER              (4)
#define     LFS_SIU_PASSBOOKPRINTER             (5)
#define     LFS_SIU_ENVDEPOSITORY               (6)
#define     LFS_SIU_CHEQUEUNIT                  (7)
#define     LFS_SIU_BILLACCEPTOR                (8)
#define     LFS_SIU_ENVDISPENSER                (9)
#define     LFS_SIU_DOCUMENTPRINTER             (10)
#define     LFS_SIU_COINACCEPTOR                (11)
#define     LFS_SIU_SCANNER                     (12)


/* LFSSIUSTATUS.sensors [...]
          LFSSIUSTATUS.doors [...]
          LFSSIUSTATUS.indicators [...]
          LFSSIUSTATUS.auxiliaries [...]
          LFSSIUSTATUS.guid_lights [...]
          LFSSIUCAPS.sensors [...]
          LFSSIUCAPS.doors [...]
          LFSSIUCAPS.indicators [...]
          LFSSIUCAPS.auxiliaries [...]
          LFSSIUCAPS.guid_lights [...] 
		  取值 */

#define     LFS_SIU_NOT_AVAILABLE               (0x0000)
#define     LFS_SIU_AVAILABLE                   (0x0001)


/* LFSSIUSTATUS.sensors [LFS_SIU_OPERATORSWITCH]
          LFSSIUCAPS.sensors [LFS_SIU_OPERATORSWITCH]
          LFSSIUPORTEVENT.port_status
          LFSSIUPORTERROR.port_status 
		  取值 */

#define     LFS_SIU_RUN                         (0x0001)
#define     LFS_SIU_MAINTENANCE                 (0x0002)
#define     LFS_SIU_SUPERVISOR                  (0x0004)


/* LFSSIUSTATUS.doors [...]
          LFSSIUSTATUS.indicators [LFS_SIU_OPENCLOSE]
          LFSSIUCAPS.doors [...]
          LFSSIUCAPS.indicators [LFS_SIU_OPENCLOSE]
          LFSSIUSETPORT.doors [...]
          LFSSIUSETPORT.indicators [LFS_SIU_OPENCLOSE]
          LFSSIUSETDOOR.door
          LFSSIUSETINDICATOR.command
          LFSSIUPORTEVENT.port_status
          LFSSIUPORTERROR.port_status 
		  取值 */

#define     LFS_SIU_CLOSED                      (0x0001)
#define     LFS_SIU_OPEN                        (0x0002)
#define     LFS_SIU_LOCKED                      (0x0004)
#define     LFS_SIU_BOLTED                      (0x0008)
#define     LFS_SIU_SERVICE                     (0x0010)
#define     LFS_SIU_KEYBOARD                    (0x0020)
#define     LFS_SIU_AJAR                        (0x0040)
#define     LFS_SIU_JAMMED                      (0x0080)


/* LFSSIUSTATUS.indicators [LFS_SIU_AUDIO]
          LFSSIUSETPORT.indicators [LFS_SIU_AUDIO]
          LFSSIUSETINDICATOR.command
          LFSSIUPORTEVENT.port_status
          LFSSIUPORTERROR.port_status 
		  取值 */

#define     LFS_SIU_KEYPRESS                    (0x0002)
#define     LFS_SIU_EXCLAMATION                 (0x0004)
#define     LFS_SIU_WARNING                     (0x0008)
#define     LFS_SIU_ERROR                       (0x0010)
#define     LFS_SIU_CRITICAL                    (0x0020)


/* LFSSIUSTATUS.auxiliaries [LFS_SIU_REMOTE_STATUS_MONITOR]
          LFSSIUSETPORT.auxiliaries [LFS_SIU_REMOTE_STATUS_MONITOR]
          LFSSIUSETAUXILIARY.command
          LFSSIUPORTEVENT.port_status
          LFSSIUPORTERROR.port_status 
		  取值 */

#define     LFS_SIU_GREEN_LED_ON                (0x0001)
#define     LFS_SIU_GREEN_LED_OFF               (0x0002)
#define     LFS_SIU_AMBER_LED_ON                (0x0004)
#define     LFS_SIU_AMBER_LED_OFF               (0x0008)
#define     LFS_SIU_RED_LED_ON                  (0x0010)
#define     LFS_SIU_RED_LED_OFF                 (0x0020)

/* LFSSIUSTATUS.auxiliaries [LFS_SIU_ENHANCEDAUDIOCONTROL]
          LFSSIUSETPORT.auxiliaries [LFS_SIU_ENHANCEDAUDIOCONTROL]
          LFSSIUSETAUXILIARY.command
          LFSSIUPORTEVENT.port_status
          LFSSIUPORTERROR.port_status 
		  取值 */

#define     LFS_SIU_PUBLICAUDIO_MANUAL          (0x0001)
#define     LFS_SIU_PUBLICAUDIO_AUTO            (0x0002)
#define     LFS_SIU_PUBLICAUDIO_SEMI_AUTO       (0x0004)
#define     LFS_SIU_PRIVATEAUDIO_MANUAL         (0x0008)
#define     LFS_SIU_PRIVATEAUDIO_AUTO           (0x0010)
#define     LFS_SIU_PRIVATEAUDIO_SEMI_AUTO      (0x0020)



/* LFSSIUSTATUS.sensors [...]
          LFSSIUSTATUS.indicators [...]
          LFSSIUSTATUS.auxiliaries [...]
          LFSSIUSTATUS.guid_lights [...]
          LFSSIUCAPS.sensors [...]
          LFSSIUCAPS.indicators [...]
          LFSSIUCAPS.guid_lights [...]
          LFSSIUSETPORT.indicators [...]
          LFSSIUSETPORT.auxiliaries [...]
          LFSSIUSETPORT.guid_lights [...]
          LFSSIUSETINDICATORS.command [...]
          LFSSIUSETAUXILIARY.command [...]
          LFSSIUSETGUIDLIGHTS.command [...]
          LFSSIUPORTEVENT.port_status
          LFSSIUPORTERROR.port_status 
		  取值 */

#define     LFS_SIU_OFF                         (0x0001)
#define     LFS_SIU_ON                          (0x0002)
#define     LFS_SIU_SLOW_FLASH                  (0x0004)
#define     LFS_SIU_MEDIUM_FLASH                (0x0008)
#define     LFS_SIU_QUICK_FLASH                 (0x0010)
#define     LFS_SIU_CONTINUOUS                  (0x0080)


/* LFSSIUSTATUS.sensors [LFS_SIU_PROXIMITY]
          LFSSIUSTATUS.sensors [LFS_SIU_ENHANCEDCONTROL]
          LFSSIUPORTEVENT.port_status
          LFSSIUPORTERROR.port_status 
		  取值 */

#define     LFS_SIU_PRESENT                     (0x0001)
#define     LFS_SIU_NOT_PRESENT                 (0x0002)   

/* LFSSIUCAPS.sensors [LFS_SIU_ENHANCEDAUDIO] 取值 */

#define     LFS_SIU_MANUAL                     (0x0001)
#define     LFS_SIU_AUTO                       (0x0002)   
#define     LFS_SIU_SEMI_AUTO                  (0X0004)


/* LFSSIUSTATUS.sensors [LFS_SIU_AMBLIGHT]
          LFSSIUCAPS.sensors [LFS_SIU_AMBLIGHT]
          LFSSIUPORTEVENT.port_status
          LFSSIUPORTERROR.port_status 
		  取值 */

#define     LFS_SIU_VERY_DARK                   (0x0001)
#define     LFS_SIU_DARK                        (0x0002)
#define     LFS_SIU_MEDIUM_LIGHT                (0x0004)
#define     LFS_SIU_LIGHT                       (0x0008)
#define     LFS_SIU_VERY_LIGHT                  (0x0010)


/* LFSSIUSTATUS.auxiliaries [LFS_SIU_UPS]
             LFSSIUCAPS.auxiliaries [LFS_SIU_UPS]
             LFSSIUPORTEVENT.port_status
             LFSSIUPORTERROR.port_status 
			 取值 */

#define LFS_SIU_LOW                             (0x0002)
#define LFS_SIU_ENGAGED                         (0x0004)
#define LFS_SIU_POWERING                        (0x0008)
#define LFS_SIU_RECOVERED                       (0x0010)


/* LFSSIUCAPS.type 取值 */

#define     LFS_SIU_SENSORS                     (0x0001)
#define     LFS_SIU_DOORS                       (0x0002)
#define     LFS_SIU_INDICATORS                  (0x0004)
#define     LFS_SIU_AUXILIARIES                 (0x0008)
#define     LFS_SIU_GUIDLIGHTS                  (0x0010)

/* LFSSIUCAPS.auxiliaries [LFS_SIU_ENHANCEDAUDIOCONTROL] 取值 */
#define     LFS_SIU_HEADSET_DETECTION		(0x0001)
#define     LFS_SIU_MODE_CONTROLLABLE      	(0x0002)

/* LFSSIUENABLE.sensors [...]
          LFSSIUENABLE.doors [...]
          LFSSIUENABLE.indicators [...]
          LFSSIUENABLE.auxiliaries [...]
          LFSSIUENABLE.guid_lights [...]
          LFSSIUSETPORTS.doors [...]
          LFSSIUSETPORTS.indicators [...]
          LFSSIUSETPORTS.auxiliaries [...]
          LFSSIUSETPORTS.guid_lights [...] 
		  取值 */

#define     LFS_SIU_NO_CHANGE                   (0x0000)
#define     LFS_SIU_ENABLE_EVENT                (0x0001)
#define     LFS_SIU_DISABLE_EVENT               (0x0002)


/* LFSSIUSETPORTS.doors [...]
          LFSSIUSETDOORS.command [...] 
		  取值 */

#define     LFS_SIU_BOLT                        (0x0001)
#define     LFS_SIU_UNBOLT                      (0x0002)


/* LFSSIUSETPORTS.auxiliaries [LFS_SIU_UPS]
      LFSSIUSETAUXILIARY.auxiliary [LFS_SIU_UPS] 
	  取值 */

#define LFS_SIU_ENGAGE                          (0x0001)
#define LFS_SIU_DISENGAGE                       (0x0002)


/* 错误码 */ 

#define     LFS_ERR_SIU_INVALID_PORT            (-(SIU_SERVICE_OFFSET + 1))
#define     LFS_ERR_SIU_SYNTAX                  (-(SIU_SERVICE_OFFSET + 2))
#define     LFS_ERR_SIU_PORT_ERROR              (-(SIU_SERVICE_OFFSET + 3))


/*=================================================================*/
/* 信息指令结构体及变量 */
/*=================================================================*/

typedef struct _lfs_siu_status
{
    WORD            device;
    WORD            sensors [LFS_SIU_SENSORS_SIZE];
    WORD            doors [LFS_SIU_DOORS_SIZE];
    WORD            indicators [LFS_SIU_INDICATORS_SIZE];
    WORD            auxiliaries [LFS_SIU_AUXILIARIES_SIZE];
    WORD            guid_lights [LFS_SIU_GUIDLIGHTS_SIZE];
    LPSTR           extra;
} LFSSIUSTATUS, * LPLFSSIUSTATUS;

typedef struct _lfs_siu_caps
{
    WORD            dev_class;
    WORD            dev_type;
    WORD            sensors [LFS_SIU_SENSORS_SIZE];
    WORD            doors [LFS_SIU_DOORS_SIZE];
    WORD            indicators [LFS_SIU_INDICATORS_SIZE];
    WORD            auxiliaries [LFS_SIU_AUXILIARIES_SIZE];
    WORD            guid_lights [LFS_SIU_GUIDLIGHTS_SIZE];
    LPSTR           extra;
} LFSSIUCAPS, * LPLFSSIUCAPS;


/*=================================================================*/
/* 执行指令结构体 */
/*=================================================================*/

typedef struct _lfs_siu_enable
{
    WORD            sensors [LFS_SIU_SENSORS_SIZE];
    WORD            doors [LFS_SIU_DOORS_SIZE];
    WORD            indicators [LFS_SIU_INDICATORS_SIZE];
    WORD            auxiliaries [LFS_SIU_AUXILIARIES_SIZE];
    WORD            guid_lights [LFS_SIU_GUIDLIGHTS_SIZE];
    LPSTR           extra;
} LFSSIUENABLE, * LPLFSSIUENABLE;


typedef struct _lfs_siu_set_ports
{
    WORD            doors [LFS_SIU_DOORS_SIZE];
    WORD            indicators [LFS_SIU_INDICATORS_SIZE];
    WORD            auxiliaries [LFS_SIU_AUXILIARIES_SIZE];
    WORD            guid_lights [LFS_SIU_GUIDLIGHTS_SIZE];
    LPSTR           extra;
} LFSSIUSETPORTS, * LPLFSSIUSETPORTS;


typedef struct _lfs_siu_set_door
{
    WORD            door;
    WORD            command;
} LFSSIUSETDOOR, * LPLFSSIUSETDOOR;


typedef struct _lfs_siu_set_indicator
{
    WORD            indicator;
    WORD            command;
} LFSSIUSETINDICATOR, * LPLFSSIUSETINDICATOR;


typedef struct _lfs_siu_set_auxiliary
{
    WORD            auxiliary;
    WORD            command;
} LFSSIUSETAUXILIARY, * LPLFSSIUSETAUXILIARY;


typedef struct _lfs_siu_set_guidlight
{
    WORD            guid_light;
    WORD            command;
} LFSSIUSETGUIDLIGHT, * LPLFSSIUSETGUIDLIGHT;


/*=================================================================*/
/* 事件结构体 */
/*=================================================================*/

typedef struct _lfs_siu_port_event
{
    WORD            port_type;
    WORD            port_index;
    WORD            port_status;
    LPSTR           extra;
} LFSSIUPORTEVENT, * LPLFSSIUPORTEVENT;


typedef struct _lfs_siu_port_error
{
    WORD            port_type;
    WORD            port_index;
    HRESULT         port_error;
    WORD            port_status;
    LPSTR           extra;
} LFSSIUPORTERROR, * LPLFSSIUPORTERROR;

/* 恢复原对齐方式 */
#pragma pack (pop)

#ifdef __cplusplus
}       /*extern "C"*/
#endif

#endif  /* __INC_LFSSIU__H */

