/******************************************************************************
*                                                                             *
* lfsptr.h    LFS - 打印机(PTR)定义 definitions                               *
*                                                                             *
*             版本 1.00  (2015-07-02)                                         *
*                                                                             *
******************************************************************************/

#ifndef __INC_LFSPTR__H
#define __INC_LFSPTR__H

#ifdef __cplusplus
extern "C" {
#endif

#include <lfsapi.h>

/*   对齐方式   */
#pragma pack(push,1)


/* LFSPTRCAPS.dev_class 取值 */

#define    LFS_SERVICE_CLASS_PTR            (1)
#define    LFS_SERVICE_CLASS_VERSION_PTR    (0x0001) /* Version 1.00 */
#define    LFS_SERVICE_CLASS_NAME_PTR       "PTR"

#define    PTR_SERVICE_OFFSET               (LFS_SERVICE_CLASS_PTR * 100)


/* 信息指令 */

#define    LFS_INF_PTR_STATUS               (PTR_SERVICE_OFFSET + 1)
#define    LFS_INF_PTR_CAPABILITIES         (PTR_SERVICE_OFFSET + 2)
#define    LFS_INF_PTR_FORM_LIST            (PTR_SERVICE_OFFSET + 3)
#define    LFS_INF_PTR_MEDIA_LIST           (PTR_SERVICE_OFFSET + 4)
#define    LFS_INF_PTR_QUERY_FORM           (PTR_SERVICE_OFFSET + 5)
#define    LFS_INF_PTR_QUERY_MEDIA          (PTR_SERVICE_OFFSET + 6)
#define    LFS_INF_PTR_QUERY_FIELD          (PTR_SERVICE_OFFSET + 7)


/* 执行指令 */

#define    LFS_CMD_PTR_CONTROL_MEDIA        (PTR_SERVICE_OFFSET + 1)
#define    LFS_CMD_PTR_PRINT_FORM           (PTR_SERVICE_OFFSET + 2)
#define    LFS_CMD_PTR_READ_FORM            (PTR_SERVICE_OFFSET + 3)
#define    LFS_CMD_PTR_RAW_DATA             (PTR_SERVICE_OFFSET + 4)
#define    LFS_CMD_PTR_MEDIA_EXTENTS        (PTR_SERVICE_OFFSET + 5)
#define    LFS_CMD_PTR_RESET_COUNT          (PTR_SERVICE_OFFSET + 6)
#define    LFS_CMD_PTR_READ_IMAGE           (PTR_SERVICE_OFFSET + 7)
#define    LFS_CMD_PTR_RESET                (PTR_SERVICE_OFFSET + 8)
#define    LFS_CMD_PTR_RETRACT_MEDIA        (PTR_SERVICE_OFFSET + 9)
#define    LFS_CMD_PTR_DISPENSE_PAPER       (PTR_SERVICE_OFFSET + 10)


/* 事件 */

#define    LFS_EXEE_PTR_NOMEDIA             (PTR_SERVICE_OFFSET + 1)
#define    LFS_EXEE_PTR_MEDIAINSERTED       (PTR_SERVICE_OFFSET + 2)
#define    LFS_EXEE_PTR_FIELDERROR          (PTR_SERVICE_OFFSET + 3)
#define    LFS_EXEE_PTR_FIELDWARNING        (PTR_SERVICE_OFFSET + 4)
#define    LFS_USRE_PTR_RETRACTBINTHRESHOLD (PTR_SERVICE_OFFSET + 5)
#define    LFS_SRVE_PTR_MEDIATAKEN          (PTR_SERVICE_OFFSET + 6)
#define    LFS_USRE_PTR_PAPERTHRESHOLD      (PTR_SERVICE_OFFSET + 7)
#define    LFS_USRE_PTR_TONERTHRESHOLD      (PTR_SERVICE_OFFSET + 8)
#define    LFS_SRVE_PTR_MEDIAINSERTED       (PTR_SERVICE_OFFSET + 9)
#define    LFS_USRE_PTR_LAMPTHRESHOLD       (PTR_SERVICE_OFFSET + 10)
#define    LFS_USRE_PTR_INKTHRESHOLD        (PTR_SERVICE_OFFSET + 11)
#define    LFS_SRVE_PTR_MEDIADETECTED       (PTR_SERVICE_OFFSET + 12)


/* LFSPTRSTATUS.device 取值 */

#define    LFS_PTR_DEVONLINE                LFS_STAT_DEVONLINE
#define    LFS_PTR_DEVOFFLINE               LFS_STAT_DEVOFFLINE
#define    LFS_PTR_DEVPOWEROFF              LFS_STAT_DEVPOWEROFF
#define    LFS_PTR_DEVNODEVICE              LFS_STAT_DEVNODEVICE
#define    LFS_PTR_DEVHWERROR               LFS_STAT_DEVHWERROR
#define    LFS_PTR_DEVUSERERROR             LFS_STAT_DEVUSERERROR
#define    LFS_PTR_DEVBUSY                  LFS_STAT_DEVBUSY


/* LFSPTRSTATUS.media and
             LFSPTRMEDIADETECTED.position 
			 取值 */

#define    LFS_PTR_MEDIAPRESENT             (0)
#define    LFS_PTR_MEDIANOTPRESENT          (1)
#define    LFS_PTR_MEDIAJAMMED              (2)
#define    LFS_PTR_MEDIANOTSUPP             (3)
#define    LFS_PTR_MEDIAUNKNOWN             (4)
#define    LFS_PTR_MEDIAENTERING            (5)
#define    LFS_PTR_MEDIARETRACTED           (6)


/* paper 数组的大小和下标上限  */

#define    LFS_PTR_SUPPLYSIZE    (16)
#define    LFS_PTR_SUPPLYMAX     (LFS_PTR_SUPPLYSIZE - 1)


/* LFSPTRSTATUS.paper [...] 数组的下标 */

#define    LFS_PTR_SUPPLYUPPER              (0)
#define    LFS_PTR_SUPPLYLOWER              (1)
#define    LFS_PTR_SUPPLYEXTERNAL           (2)
#define    LFS_PTR_SUPPLYAUX                (3)
#define    LFS_PTR_SUPPLYAUX2               (4)
#define    LFS_PTR_SUPPLYPARK               (5)


/* LFSPTRSTATUS.paper and
             LFSPTRPAPERTHRESHOLD.paper_threshold 
			 取值 */

#define    LFS_PTR_PAPERFULL                (0)
#define    LFS_PTR_PAPERLOW                 (1)
#define    LFS_PTR_PAPEROUT                 (2)
#define    LFS_PTR_PAPERNOTSUPP             (3)
#define    LFS_PTR_PAPERUNKNOWN             (4)
#define    LFS_PTR_PAPERJAMMED              (5)


/* LFSPTRSTATUS.toner 取值 */

#define    LFS_PTR_TONERFULL                (0)
#define    LFS_PTR_TONERLOW                 (1)
#define    LFS_PTR_TONEROUT                 (2)
#define    LFS_PTR_TONERNOTSUPP             (3)
#define    LFS_PTR_TONERUNKNOWN             (4)


/* LFSPTRSTATUS.ink 取值 */

#define    LFS_PTR_INKFULL                  (0)
#define    LFS_PTR_INKLOW                   (1)
#define    LFS_PTR_INKOUT                   (2)
#define    LFS_PTR_INKNOTSUPP               (3)
#define    LFS_PTR_INKUNKNOWN               (4)


/* LFSPTRSTATUS.lamp 取值 */

#define    LFS_PTR_LAMPOK                   (0)
#define    LFS_PTR_LAMPFADING               (1)
#define    LFS_PTR_LAMPINOP                 (2)
#define    LFS_PTR_LAMPNOTSUPP              (3)
#define    LFS_PTR_LAMPUNKNOWN              (4)


/* LFSPTRSTATUS.retract_bin and
             LFSPTRBINTHRESHOLD.retract_bin 
			 取值 */

#define    LFS_PTR_RETRACTBINOK             (0)
#define    LFS_PTR_RETRACTBINFULL           (1)
#define    LFS_PTR_RETRACTNOTSUPP           (2)
#define    LFS_PTR_RETRACTUNKNOWN           (3)
#define    LFS_PTR_RETRACTBINHIGH           (4)


/* LFSPTRCAPS.type 取值 */

#define    LFS_PTR_TYPERECEIPT              0x0001
#define    LFS_PTR_TYPEPASSBOOK             0x0002
#define    LFS_PTR_TYPEJOURNAL              0x0004
#define    LFS_PTR_TYPEDOCUMENT             0x0008
#define    LFS_PTR_TYPESCANNER              0x0010


/* LFSPTRCAPS.resolution, LFSPTRPRINTFORM.resolution 取值 */

#define    LFS_PTR_RESLOW                   0x0001
#define    LFS_PTR_RESMED                   0x0002
#define    LFS_PTR_RESHIGH                  0x0004
#define    LFS_PTR_RESVERYHIGH              0x0008


/* LFSPTRCAPS.read_form 取值 */

#define    LFS_PTR_READOCR                  0x0001
#define    LFS_PTR_READMICR                 0x0002
#define    LFS_PTR_READMSF                  0x0004
#define    LFS_PTR_READBARCODE              0x0008
#define    LFS_PTR_READPAGEMARK             0x0010
#define    LFS_PTR_READIMAGE                0x0020
#define    LFS_PTR_READEMPTYLINE            0x0040


/* LFSPTRCAPS.write_form 取值 */

#define    LFS_PTR_WRITETEXT                0x0001
#define    LFS_PTR_WRITEGRAPHICS            0x0002
#define    LFS_PTR_WRITEOCR                 0x0004
#define    LFS_PTR_WRITEMICR                0x0008
#define    LFS_PTR_WRITEMSF                 0x0010
#define    LFS_PTR_WRITEBARCODE             0x0020
#define    LFS_PTR_WRITESTAMP               0x0040


/* LFSPTRCAPS.extents 取值 */

#define    LFS_PTR_EXTHORIZONTAL            0x0001
#define    LFS_PTR_EXTVERTICAL              0x0002


/* LFSPTRCAPS.control, media_control 取值 */

#define    LFS_PTR_CTRLEJECT                0x0001
#define    LFS_PTR_CTRLPERFORATE            0x0002
#define    LFS_PTR_CTRLCUT                  0x0004
#define    LFS_PTR_CTRLSKIP                 0x0008
#define    LFS_PTR_CTRLFLUSH                0x0010
#define    LFS_PTR_CTRLRETRACT              0x0020
#define    LFS_PTR_CTRLSTACK                0x0040
#define    LFS_PTR_CTRLPARTIALCUT           0x0080
#define    LFS_PTR_CTRLALARM                0x0100
#define    LFS_PTR_CTRLATPFORWARD           0x0200
#define    LFS_PTR_CTRLATPBACKWARD          0x0400
#define    LFS_PTR_CTRLTURNMEDIA            0x0800
#define    LFS_PTR_CTRLSTAMP                0x1000
#define    LFS_PTR_CTRLPARK                 0x2000


/* LFSPTRCAPS.paper_sources,
             LFSFRMMEDIA.paper_sources,
             LFSPTRPRINTFORM.paper_source and
             LFSPTRPAPERTHRESHOLD.paper_source  
			取值 */

#define    LFS_PTR_PAPERANY                 0x0001
#define    LFS_PTR_PAPERUPPER               0x0002
#define    LFS_PTR_PAPERLOWER               0x0004
#define    LFS_PTR_PAPEREXTERNAL            0x0008
#define    LFS_PTR_PAPERAUX                 0x0010
#define    LFS_PTR_PAPERAUX2                0x0020
#define    LFS_PTR_PAPERPARK                0x0040


/* LFSPTRCAPS.image_type,
             LFSPTRIMAGEREQUEST.front_image_format and
             LFSPTRIMAGEREQUEST.back_image_format 
			 取值 */

#define    LFS_PTR_IMAGETIF                 0x0001
#define    LFS_PTR_IMAGEWMF                 0x0002
#define    LFS_PTR_IMAGEBMP                 0x0004


/* LFSPTRCAPS.front_image_color_format,
             LFSPTRCAPS.back_image_color_format,
             LFSPTRIMAGEREQUEST.front_image_color_format and
             LFSPTRIMAGEREQUEST.back_image_color_format 
			 取值 */

#define    LFS_PTR_IMAGECOLORBINARY         0x0001
#define    LFS_PTR_IMAGECOLORGRAYSCALE      0x0002
#define    LFS_PTR_IMAGECOLORFULL           0x0004


/* LFSPTRCAPS.codeline_format and
             LFSPTRIMAGEREQUEST.codeline_format
			 取值 */

#define    LFS_PTR_CODELINECMC7             0x0001
#define    LFS_PTR_CODELINEE13B             0x0002
#define    LFS_PTR_CODELINEOCR              0x0004


/* LFSPTRCAPS.fwImageSource,
             LFSPTRIMAGEREQUEST.image_source and
             LFSPTRIMAGE.image_source 
			 取值 */

#define    LFS_PTR_IMAGEFRONT               0x0001
#define    LFS_PTR_IMAGEBACK                0x0002
#define    LFS_PTR_CODELINE                 0x0004


/* LFSPTRCAPS.char_support, LFSFRMHEADER.char_support 取值 */

#define    LFS_PTR_UTF8	                    0x0001
#define    LFS_PTR_UNICODE                  0x0002


/* LFSFRMHEADER.base, LFSFRMMEDIA.base, LFSPTRMEDIAUNIT.base 取值 */

#define    LFS_FRM_INCH                     (0)
#define    LFS_FRM_MM                       (1)
#define    LFS_FRM_ROWCOLUMN                (2)


/* LFSFRMHEADER.alignment 取值 */

#define    LFS_FRM_TOPLEFT                  (0)
#define    LFS_FRM_TOPRIGHT                 (1)
#define    LFS_FRM_BOTTOMLEFT               (2)
#define    LFS_FRM_BOTTOMRIGHT              (3)


/* LFSFRMHEADER.orientation 取值 */

#define    LFS_FRM_PORTRAIT                 (0)
#define    LFS_FRM_LANDSCAPE                (1)


/* LFSFRMMEDIA.media_type 取值 */

#define    LFS_FRM_MEDIAGENERIC             (0)
#define    LFS_FRM_MEDIAPASSBOOK            (1)
#define    LFS_FRM_MEDIAMULTIPART           (2)


/* LFSFRMMEDIA.fold_type 取值 */

#define    LFS_FRM_FOLDNONE                 (0)
#define    LFS_FRM_FOLDHORIZONTAL           (1)
#define    LFS_FRM_FOLDVERTICAL             (2)


/* LFSFRMFIELD.type 取值 */

#define    LFS_FRM_FIELDTEXT                (0)
#define    LFS_FRM_FIELDMICR                (1)
#define    LFS_FRM_FIELDOCR                 (2)
#define    LFS_FRM_FIELDMSF                 (3)
#define    LFS_FRM_FIELDBARCODE             (4)
#define    LFS_FRM_FIELDGRAPHIC             (5)
#define    LFS_FRM_FIELDPAGEMARK            (6)


/* LFSFRMFIELD.class 取值 */

#define    LFS_FRM_CLASSSTATIC              (0)
#define    LFS_FRM_CLASSOPTIONAL            (1)
#define    LFS_FRM_CLASSREQUIRED            (2)


/* LFSFRMFIELD.access 取值 */

#define    LFS_FRM_ACCESSREAD               0x0001
#define    LFS_FRM_ACCESSWRITE              0x0002


/* LFSFRMFIELD.overflow 取值 */

#define    LFS_FRM_OVFTERMINATE             (0)
#define    LFS_FRM_OVFTRUNCATE              (1)
#define    LFS_FRM_OVFBESTFIT               (2)
#define    LFS_FRM_OVFOVERWRITE             (3)
#define    LFS_FRM_OVFWORDWRAP              (4)


/* LFSPTRFIELDFAIL.failure 取值 */

#define    LFS_PTR_FIELDREQUIRED            (0)
#define    LFS_PTR_FIELDSTATICOVWR          (1)
#define    LFS_PTR_FIELDOVERFLOW            (2)
#define    LFS_PTR_FIELDNOTFOUND            (3)
#define    LFS_PTR_FIELDNOTREAD             (4)
#define    LFS_PTR_FIELDNOTWRITE            (5)
#define    LFS_PTR_FIELDHWERROR             (6)
#define    LFS_PTR_FIELDTYPENOTSUPPORTED    (7)
#define    LFS_PTR_FIELDGRAPHIC             (8)
#define    LFS_PTR_CHARSETFORM              (9)


/* LFSPTRPRINTFORM.alignment 取值 */

#define    LFS_PTR_ALNUSEFORMDEFN           (0)
#define    LFS_PTR_ALNTOPLEFT               (1)
#define    LFS_PTR_ALNTOPRIGHT              (2)
#define    LFS_PTR_ALNBOTTOMLEFT            (3)
#define    LFS_PTR_ALNBOTTOMRIGHT           (4)


/* LFSPTRPRINTFORM.offset_x and LFSPTRPRINTFORM.offset_y 取值*/

#define    LFS_PTR_OFFSETUSEFORMDEFN        0xffff


/* LFSPTRRAWDATA.input_data 取值 */

#define    LFS_PTR_NOINPUTDATA              (0)
#define    LFS_PTR_INPUTDATA                (1)


/* LFSPTRIMAGE.status 取值 */

#define     LFS_PTR_DATAOK                  (0)
#define     LFS_PTR_DATASRCNOTSUPP          (1)
#define     LFS_PTR_DATASRCMISSING          (2)


/* 错误码 */ 

#define    LFS_ERR_PTR_FORMNOTFOUND         (-(PTR_SERVICE_OFFSET + 0))
#define    LFS_ERR_PTR_FIELDNOTFOUND        (-(PTR_SERVICE_OFFSET + 1))
#define    LFS_ERR_PTR_NOMEDIAPRESENT       (-(PTR_SERVICE_OFFSET + 2))
#define    LFS_ERR_PTR_READNOTSUPPORTED     (-(PTR_SERVICE_OFFSET + 3))
#define    LFS_ERR_PTR_FLUSHFAIL            (-(PTR_SERVICE_OFFSET + 4))
#define    LFS_ERR_PTR_MEDIAOVERFLOW        (-(PTR_SERVICE_OFFSET + 5))
#define    LFS_ERR_PTR_FIELDSPECFAILURE     (-(PTR_SERVICE_OFFSET + 6))
#define    LFS_ERR_PTR_FIELDERROR           (-(PTR_SERVICE_OFFSET + 7))
#define    LFS_ERR_PTR_MEDIANOTFOUND        (-(PTR_SERVICE_OFFSET + 8))
#define    LFS_ERR_PTR_EXTENTNOTSUPPORTED   (-(PTR_SERVICE_OFFSET + 9))
#define    LFS_ERR_PTR_MEDIAINVALID         (-(PTR_SERVICE_OFFSET + 10))
#define    LFS_ERR_PTR_FORMINVALID          (-(PTR_SERVICE_OFFSET + 11))
#define    LFS_ERR_PTR_FIELDINVALID         (-(PTR_SERVICE_OFFSET + 12))
#define    LFS_ERR_PTR_MEDIASKEWED          (-(PTR_SERVICE_OFFSET + 13))
#define    LFS_ERR_PTR_RETRACTBINFULL       (-(PTR_SERVICE_OFFSET + 14))
#define    LFS_ERR_PTR_STACKERFULL          (-(PTR_SERVICE_OFFSET + 15))
#define    LFS_ERR_PTR_PAGETURNFAIL         (-(PTR_SERVICE_OFFSET + 16))
#define    LFS_ERR_PTR_MEDIATURNFAIL        (-(PTR_SERVICE_OFFSET + 17))
#define    LFS_ERR_PTR_SHUTTERFAIL          (-(PTR_SERVICE_OFFSET + 18))
#define    LFS_ERR_PTR_MEDIAJAMMED          (-(PTR_SERVICE_OFFSET + 19))
#define    LFS_ERR_PTR_FILE_IO_ERROR        (-(PTR_SERVICE_OFFSET + 20))
#define    LFS_ERR_PTR_CHARSETDATA          (-(PTR_SERVICE_OFFSET + 21))
#define    LFS_ERR_PTR_PAPERJAMMED          (-(PTR_SERVICE_OFFSET + 22))
#define    LFS_ERR_PTR_PAPEROUT             (-(PTR_SERVICE_OFFSET + 23))
#define    LFS_ERR_PTR_INKOUT               (-(PTR_SERVICE_OFFSET + 24))
#define    LFS_ERR_PTR_TONEROUT             (-(PTR_SERVICE_OFFSET + 25))
#define    LFS_ERR_PTR_LAMPINOP             (-(PTR_SERVICE_OFFSET + 26))
#define    LFS_ERR_PTR_SOURCEINVALID        (-(PTR_SERVICE_OFFSET + 27))
#define    LFS_ERR_PTR_SEQUENCEINVALID      (-(PTR_SERVICE_OFFSET + 28))
#define    LFS_ERR_PTR_MEDIASIZE            (-(PTR_SERVICE_OFFSET + 29))


/*=================================================================*/
/* 信息指令结构体 */
/*=================================================================*/

typedef struct _lfs_ptr_retract_bins
{
   WORD      retract_bin;
   USHORT    retract_count;
} LFSPTRRETRACTBINS, * LPLFSPTRRETRACTBINS;

typedef struct _lfs_ptr_status        
{
    WORD            device;
    WORD            media;
    WORD            paper[LFS_PTR_SUPPLYSIZE];
    WORD            toner;
    WORD            ink;
    WORD            lamp;
    LPLFSPTRRETRACTBINS *retract_bins;
    USHORT          media_on_stacker;
    LPSTR           extra;
} LFSPTRSTATUS, * LPLFSPTRSTATUS;

typedef struct _lfs_ptr_caps
{
    WORD            dev_class;
    WORD            dev_type;
    BOOL            compound;
    WORD            resolution;
    WORD            read_form;
    WORD            write_form;
    WORD            extents;
    WORD            control;
    USHORT          max_media_on_stacker;
    BOOL            accept_media;
    BOOL            multi_page;
    WORD            paper_sources;
    BOOL            media_taken;
    USHORT          retract_bins;
    LPUSHORT        max_retract;
    WORD            image_type;
    WORD            front_image_color_format;
    WORD            back_image_color_format;
    WORD            codeline_format;
    WORD            image_source;
    WORD            char_support;
    BOOL            dispense_paper;
    LPSTR           extra;
} LFSPTRCAPS, * LPLFSPTRCAPS;

typedef struct _lfs_frm_header
{
    LPSTR           form_name;
    WORD            base;
    WORD            unit_x;
    WORD            unit_y;
    WORD            width;
    WORD            height;
    WORD            alignment;
    WORD            orientation;
    WORD            offset_x;
    WORD            offset_y;
    WORD            version_major;
    WORD            version_minor;
    LPSTR           user_prompt;
    WORD            char_support;
    LPSTR           fields;
} LFSFRMHEADER, * LPLFSFRMHEADER;

typedef struct _lfs_frm_media
{
    WORD            media_type;
    WORD            base;
    WORD            unit_x;
    WORD            unit_y;
    WORD            size_width;
    WORD            size_height;
    WORD            page_count;
    WORD            line_count;
    WORD            print_area_x;
    WORD            print_area_y;
    WORD            print_area_width;
    WORD            print_area_height;
    WORD            restricted_area_x;
    WORD            restricted_area_y;
    WORD            restricted_area_width;
    WORD            restricted_area_height;
    WORD            stagger;
    WORD            fold_type;
    WORD            paper_sources;
} LFSFRMMEDIA, * LPLFSFRMMEDIA;

typedef struct _lfs_ptr_query_field
{
    LPSTR           form_name;
    LPSTR           field_name;
} LFSPTRQUERYFIELD, * LPLFSPTRQUERYFIELD;

typedef struct _lfs_frm_field
{
    LPSTR           field_name;
    WORD            index_count;
    WORD            type;
    WORD            field_class;
    WORD            access;
    WORD            overflow;
    LPSTR           initial_value;
    LPWSTR          unicode_initial_value;
    LPSTR           format;
    LPWSTR          unicode_format;
} LFSFRMFIELD, * LPLFSFRMFIELD;


/*=================================================================*/
/* 执行指令结构体 */
/*=================================================================*/

typedef struct _lfs_ptr_print_form
{
    LPSTR           form_name; 
    LPSTR           media_name;
    WORD            alignment;
    WORD            offset_x;
    WORD            offset_y;
    WORD            resolution;
    DWORD           media_control;
    LPSTR           fields;
    LPWSTR          unicode_fields;
    WORD            paper_source;
} LFSPTRPRINTFORM, * LPLFSPTRPRINTFORM;

typedef struct _lfs_ptr_read_form
{
    LPSTR           form_name;
    LPSTR           field_names;
    LPSTR           media_name;
    DWORD           media_control;
} LFSPTRREADFORM, * LPLFSPTRREADFORM;

typedef struct _lfs_ptr_read_form_out
{	
    LPSTR           fields;
    LPWSTR          unicode_fields;
} LFSPTRREADFORMOUT, * LPLFSPTRREADFORMOUT;

typedef struct _lfs_ptr_raw_data
{
    WORD            input_data;
    ULONG           size;
    LPBYTE          data;
} LFSPTRRAWDATA, * LPLFSPTRRAWDATA;

typedef struct _lfs_ptr_raw_data_in
{
    ULONG           size;
    LPBYTE          data;
} LFSPTRRAWDATAIN, * LPLFSPTRRAWDATAIN;

typedef struct _lfs_ptr_media_unit
{
    WORD            base;
    WORD            unit_x;
    WORD            unit_y;
} LFSPTRMEDIAUNIT, * LPLFSPTRMEDIAUNIT;

typedef struct _lfs_ptr_media_ext
{
    ULONG           size_x;
    ULONG           size_y;
} LFSPTRMEDIAEXT, * LPLFSPTRMEDIAEXT;

typedef struct _lfs_ptr_image_request
{
    WORD           front_image_type;
    WORD           back_image_type;
    WORD           front_image_color_format;
    WORD           back_image_color_format;
    WORD           codeline_format;
    WORD           image_source;
    LPSTR          front_image_file;
    LPSTR          back_image_file;
} LFSPTRIMAGEREQUEST, * LPLFSPTRIMAGEREQUEST;

typedef struct _lfs_ptr_image
{
    WORD           image_source;
    WORD           status;
    ULONG          data_length;
    LPBYTE         data;
} LFSPTRIMAGE, * LPLFSPTRIMAGE;

typedef struct _lfs_ptr_reset
{
    DWORD         media_control;
    USHORT        retract_bin_number;
} LFSPTRRESET, * LPLFSPTRRESET;


/*=================================================================*/
/* 事件结构体 */
/*=================================================================*/

typedef struct _lfs_ptr_field_failure
{
    LPSTR           form_name;
    LPSTR           field_name;
    WORD            failure;
} LFSPTRFIELDFAIL, * LPLFSPTRFIELDFAIL;

typedef struct _lfs_ptr_bin_threshold
{
    USHORT        bin_number;
    WORD          retract_bin;
} LFSPTRBINTHRESHOLD, * LPLFSPTRBINTHRESHOLD;

typedef struct _lfs_ptr_paper_threshold
{
     WORD          paper_source;
     WORD          paper_threshold;
} LFSPTRPAPERTHRESHOLD, * LPLFSPTRPAPERTHRESHOLD;

typedef struct _lfs_ptr_media_detected
{
    WORD          position;
    USHORT        retract_bin_number;
} LFSPTRMEDIADETECTED, * LPLFSPTRMEDIADETECTED;

/*   恢复对齐方式   */
#pragma pack(pop)

#ifdef __cplusplus
}       /*extern "C"*/
#endif
#endif    /* __INC_LFSPTR__H */
