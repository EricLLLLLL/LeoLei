/****************************************************************************
*                                                                           *
*xfspin.h XFS - 密码键盘 (PIN) 定义     	    *
*                                                                           *
*            Version 1.00  (2015-07/06)                                     *
*                                                                           *
*************************************************************************** */

#ifndef __INC_LFSPIN__H
#define __INC_LFSPIN__H

#ifdef __cplusplus
extern "C" {
#endif

#include <lfsapi.h>

/*   be aware of alignment     */
#pragma pack(push,1)


/* LFSPINCAPS.dev_class  的取值*/

#define LFS_SERVICE_CLASS_PIN             (5)
#define LFS_SERVICE_CLASS_VERSION_PIN     (0x0001) /* Version 1.00   的取值*/
#define LFS_SERVICE_CLASS_NAME_PIN        "PIN"

#define PIN_SERVICE_OFFSET                (LFS_SERVICE_CLASS_PIN * 100)

/* PIN 状态命令   的取值*/

#define LFS_INF_PIN_STATUS                (PIN_SERVICE_OFFSET + 1)
#define LFS_INF_PIN_CAPABILITIES          (PIN_SERVICE_OFFSET + 2)
#define LFS_INF_PIN_KEY_DETAIL            (PIN_SERVICE_OFFSET + 4)
#define LFS_INF_PIN_FUNCKEY_DETAIL        (PIN_SERVICE_OFFSET + 5)
#define LFS_INF_PIN_HSM_TDATA             (PIN_SERVICE_OFFSET + 6)
#define LFS_INF_PIN_SECUREKEY_DETAIL      (PIN_SERVICE_OFFSET + 8)


/* PIN 硬件命令   的取值*/

#define LFS_CMD_PIN_CRYPT                 (PIN_SERVICE_OFFSET + 1)
#define LFS_CMD_PIN_IMPORT_KEY            (PIN_SERVICE_OFFSET + 3)
#define LFS_CMD_PIN_GET_PIN               (PIN_SERVICE_OFFSET + 5)
#define LFS_CMD_PIN_GET_PINBLOCK          (PIN_SERVICE_OFFSET + 7)
#define LFS_CMD_PIN_GET_DATA              (PIN_SERVICE_OFFSET + 8)
#define LFS_CMD_PIN_INITIALIZATION        (PIN_SERVICE_OFFSET + 9)
#define LFS_CMD_PIN_LOCAL_DES             (PIN_SERVICE_OFFSET + 10)
#define LFS_CMD_PIN_LOCAL_EUROCHEQUE      (PIN_SERVICE_OFFSET + 11)
#define LFS_CMD_PIN_LOCAL_VISA            (PIN_SERVICE_OFFSET + 12)
#define LFS_CMD_PIN_CREATE_OFFSET         (PIN_SERVICE_OFFSET + 13)
#define LFS_CMD_PIN_DERIVE_KEY            (PIN_SERVICE_OFFSET + 14)
#define LFS_CMD_PIN_PRESENT_IDC           (PIN_SERVICE_OFFSET + 15)
#define LFS_CMD_PIN_LOCAL_BANKSYS         (PIN_SERVICE_OFFSET + 16)
#define LFS_CMD_PIN_BANKSYS_IO            (PIN_SERVICE_OFFSET + 17)
#define LFS_CMD_PIN_RESET                 (PIN_SERVICE_OFFSET + 18)
#define LFS_CMD_PIN_HSM_SET_TDATA         (PIN_SERVICE_OFFSET + 19)
#define LFS_CMD_PIN_SECURE_MSG_SEND       (PIN_SERVICE_OFFSET + 20)
#define LFS_CMD_PIN_SECURE_MSG_RECEIVE    (PIN_SERVICE_OFFSET + 21)
#define LFS_CMD_PIN_GET_JOURNAL           (PIN_SERVICE_OFFSET + 22)
#define LFS_CMD_PIN_ENC_IO                (PIN_SERVICE_OFFSET + 24)
#define LFS_CMD_PIN_HSM_INIT              (PIN_SERVICE_OFFSET + 25)
#define LFS_CMD_PIN_IMPORT_RSA_PUBLIC_KEY  (PIN_SERVICE_OFFSET + 26)
#define LFS_CMD_PIN_EXPORT_RSA_ISSUER_SIGNED_ITEM (PIN_SERVICE_OFFSET + 27)
#define LFS_CMD_PIN_IMPORT_RSA_SIGNED_DES_KEY (PIN_SERVICE_OFFSET + 28)
#define LFS_CMD_PIN_GENERATE_RSA_KEY_PAIR (PIN_SERVICE_OFFSET + 29)
#define LFS_CMD_PIN_EXPORT_RSA_EPP_SIGNED_ITEM (PIN_SERVICE_OFFSET + 30)
#define LFS_CMD_PIN_LOAD_CERTIFICATE      (PIN_SERVICE_OFFSET + 31)
#define LFS_CMD_PIN_GET_CERTIFICATE       (PIN_SERVICE_OFFSET + 32)
#define LFS_CMD_PIN_REPLACE_CERTIFICATE   (PIN_SERVICE_OFFSET + 33)
#define LFS_CMD_PIN_START_KEY_EXCHANGE    (PIN_SERVICE_OFFSET + 34)
#define LFS_CMD_PIN_IMPORT_RSA_ENCIPHERED_PKCS7_KEY (PIN_SERVICE_OFFSET + 35)
#define LFS_CMD_PIN_EMV_IMPORT_PUBLIC_KEY (PIN_SERVICE_OFFSET + 36)
#define LFS_CMD_PIN_DIGEST                (PIN_SERVICE_OFFSET + 37)
#define LFS_CMD_PIN_SECUREKEY_ENTRY       (PIN_SERVICE_OFFSET + 38)
#define LFS_CMD_PIN_GENERATE_KCV          (PIN_SERVICE_OFFSET + 39)
#define LFS_CMD_PIN_GET_PINBLOCK_EX       (PIN_SERVICE_OFFSET + 53) 


/* PIN 消息   的取值*/

#define LFS_EXEE_PIN_KEY                  (PIN_SERVICE_OFFSET + 1)
#define LFS_SRVE_PIN_INITIALIZED          (PIN_SERVICE_OFFSET + 2)
#define LFS_SRVE_PIN_ILLEGAL_KEY_ACCESS   (PIN_SERVICE_OFFSET + 3)
#define LFS_SRVE_PIN_OPT_REQUIRED         (PIN_SERVICE_OFFSET + 4)
#define LFS_SRVE_PIN_HSM_TDATA_CHANGED    (PIN_SERVICE_OFFSET + 5)
#define LFS_SRVE_PIN_CERTIFICATE_CHANGE   (PIN_SERVICE_OFFSET + 6)

/*LFSPINSTATUS.device   的取值*/

#define LFS_PIN_DEVONLINE                 LFS_STAT_DEVONLINE
#define LFS_PIN_DEVOFFLINE                LFS_STAT_DEVOFFLINE
#define LFS_PIN_DEVPOWEROFF               LFS_STAT_DEVPOWEROFF
#define LFS_PIN_DEVNODEVICE               LFS_STAT_DEVNODEVICE
#define LFS_PIN_DEVHWERROR                LFS_STAT_DEVHWERROR
#define LFS_PIN_DEVUSERERROR              LFS_STAT_DEVUSERERROR
#define LFS_PIN_DEVBUSY                   LFS_STAT_DEVBUSY

/* LFSPINSTATUS.enc_stat 的取值*/

#define LFS_PIN_ENCREADY                  (0)
#define LFS_PIN_ENCNOTREADY               (1)
#define LFS_PIN_ENCNOTINITIALIZED         (2)
#define LFS_PIN_ENCBUSY                   (3)
#define LFS_PIN_ENCUNDEFINED              (4)
#define LFS_PIN_ENCINITIALIZED            (5)

/*LFSPINCAPS.dev_type  的取值*/

#define LFS_PIN_TYPEEPP                   (0x0001)
#define LFS_PIN_TYPEEDM                   (0x0002)
#define LFS_PIN_TYPEHSM                   (0x0004)

/*LFSPINCAPS.algorithms, LFSPINCRYPT.algorithm   的取值*/

#define LFS_PIN_CRYPTDESECB               (0x0001)
#define LFS_PIN_CRYPTDESCBC               (0x0002)
#define LFS_PIN_CRYPTDESCFB               (0x0004)
#define LFS_PIN_CRYPTRSA                  (0x0008)
#define LFS_PIN_CRYPTECMA                 (0x0010)
#define LFS_PIN_CRYPTDESMAC               (0x0020)
#define LFS_PIN_CRYPTTRIDESECB            (0x0040)
#define LFS_PIN_CRYPTTRIDESCBC            (0x0080)
#define LFS_PIN_CRYPTTRIDESCFB            (0x0100)
#define LFS_PIN_CRYPTTRIDESMAC            (0x0200)
#define LFS_PIN_CRYPTMAAMAC               (0x0400)
#define LFS_PIN_CRYPTSM4                  (0x1000)
#define LFS_PIN_CRYPTSM4MAC               (0x2000) 

/*LFSPINCAPS.pin_formats   的取值*/

#define LFS_PIN_FORM3624                  (0x0001)
#define LFS_PIN_FORMANSI                  (0x0002)
#define LFS_PIN_FORMISO0                  (0x0004)
#define LFS_PIN_FORMISO1                  (0x0008)
#define LFS_PIN_FORMECI2                  (0x0010)
#define LFS_PIN_FORMECI3                  (0x0020)
#define LFS_PIN_FORMVISA                  (0x0040)
#define LFS_PIN_FORMDIEBOLD               (0x0080)
#define LFS_PIN_FORMDIEBOLDCO             (0x0100)
#define LFS_PIN_FORMVISA3                 (0x0200)
#define LFS_PIN_FORMBANKSYS               (0x0400)
#define LFS_PIN_FORMEMV                   (0x0800)
#define LFS_PIN_FORMISO3                  (0x2000)

/*LFSPINCAPS.derivation_algorithms   的取值*/

#define LFS_PIN_CHIP_ZKA                  (0x0001)

/*LFSPINCAPS.presentation_algorithms   的取值*/

#define LFS_PIN_PRESENT_CLEAR             (0x0001)

/*LFSPINCAPS.display   的取值*/

#define LFS_PIN_DISPNONE                  (1)
#define LFS_PIN_DISPLEDTHROUGH            (2)
#define LFS_PIN_DISPDISPLAY               (3)

/*LFSPINCAPS.id_key   的取值*/

#define LFS_PIN_IDKEYINITIALIZATION       (0x0001)
#define LFS_PIN_IDKEYIMPORT               (0x0002)

/*LFSPINCAPS.validation_algorithms   的取值*/

#define LFS_PIN_DES                       (0x0001)
#define LFS_PIN_EUROCHEQUE                (0x0002)
#define LFS_PIN_VISA                      (0x0004)
#define LFS_PIN_DES_OFFSET                (0x0008)
#define LFS_PIN_BANKSYS                   (0x0010)

/*LFSPINCAPS.key_check_modes and
          LFSPINIMPORTKEYEX.key_check_mode   的取值*/

#define LFS_PIN_KCVNONE                   (0x0000)
#define LFS_PIN_KCVSELF                   (0x0001)
#define LFS_PIN_KCVZERO                   (0x0002)

/*LFSPINKEYDETAIL.use and LFSPINKEYDETAILEX.use   的取值*/

#define LFS_PIN_USECRYPT                  (0x0001)
#define LFS_PIN_USEFUNCTION               (0x0002)
#define LFS_PIN_USEMACING                 (0x0004)
#define LFS_PIN_USEKEYENCKEY              (0x0020)
#define LFS_PIN_USENODUPLICATE            (0x0040)
#define LFS_PIN_USESVENCKEY               (0x0080)
#define LFS_PIN_USECONSTRUCT              (0x0100)
#define LFS_PIN_USESECURECONSTRUCT        (0x0200)
#define LFS_PIN_USESM4			  (0x8000) 

/* AdditionalLFSPINKEYDETAILEX.use   的取值*/

#define LFS_PIN_USEPINLOCAL              (0x10000)
#define LFS_PIN_USERSAPUBLIC             (0x20000)
#define LFS_PIN_USERSAPRIVATE            (0x40000)
#define LFS_PIN_USECHIPINFO             (0x100000)
#define LFS_PIN_USECHIPPIN              (0x200000)
#define LFS_PIN_USECHIPPS               (0x400000)
#define LFS_PIN_USECHIPMAC              (0x800000)
#define LFS_PIN_USECHIPLT              (0x1000000)
#define LFS_PIN_USECHIPMACLZ           (0x2000000)
#define LFS_PIN_USECHIPMACAZ           (0x4000000)
#define LFS_PIN_USERSAPUBLICVERIFY     (0x8000000)
#define LFS_PIN_USERSAPRIVATESIGN     (0x10000000)


/*LFSPINFUNCKEYDETAIL.func_mask   的取值*/

#define LFS_PIN_FK_0                      (0x00000001)
#define LFS_PIN_FK_1                      (0x00000002)
#define LFS_PIN_FK_2                      (0x00000004)
#define LFS_PIN_FK_3                      (0x00000008)
#define LFS_PIN_FK_4                      (0x00000010)
#define LFS_PIN_FK_5                      (0x00000020)
#define LFS_PIN_FK_6                      (0x00000040)
#define LFS_PIN_FK_7                      (0x00000080)
#define LFS_PIN_FK_8                      (0x00000100)
#define LFS_PIN_FK_9                      (0x00000200)
#define LFS_PIN_FK_ENTER                  (0x00000400)
#define LFS_PIN_FK_CANCEL                 (0x00000800)
#define LFS_PIN_FK_CLEAR                  (0x00001000)
#define LFS_PIN_FK_BACKSPACE              (0x00002000)
#define LFS_PIN_FK_HELP                   (0x00004000)
#define LFS_PIN_FK_DECPOINT               (0x00008000)
#define LFS_PIN_FK_00                     (0x00010000)
#define LFS_PIN_FK_000                    (0x00020000)
#define LFS_PIN_FK_RES1                   (0x00040000)
#define LFS_PIN_FK_RES2                   (0x00080000)
#define LFS_PIN_FK_RES3                   (0x00100000)
#define LFS_PIN_FK_RES4                   (0x00200000)
#define LFS_PIN_FK_RES5                   (0x00400000)
#define LFS_PIN_FK_RES6                   (0x00800000)
#define LFS_PIN_FK_RES7                   (0x01000000)
#define LFS_PIN_FK_RES8                   (0x02000000)
#define LFS_PIN_FK_OEM1                   (0x04000000)
#define LFS_PIN_FK_OEM2                   (0x08000000)
#define LFS_PIN_FK_OEM3                   (0x10000000)
#define LFS_PIN_FK_OEM4                   (0x20000000)
#define LFS_PIN_FK_OEM5                   (0x40000000)
#define LFS_PIN_FK_OEM6                   (0x80000000)

/* additionalLFSPINFUNCKEYDETAIL.func_mask   的取值*/
#define LFS_PIN_FK_UNUSED			(0x00000000)

#define LFS_PIN_FK_A                    LFS_PIN_FK_RES1
#define LFS_PIN_FK_B                    LFS_PIN_FK_RES2
#define LFS_PIN_FK_C                    LFS_PIN_FK_RES3
#define LFS_PIN_FK_D                    LFS_PIN_FK_RES4
#define LFS_PIN_FK_E                    LFS_PIN_FK_RES5
#define LFS_PIN_FK_F                    LFS_PIN_FK_RES6	
#define LFS_PIN_FK_SHIFT                LFS_PIN_FK_RES7

/*LFSPINFUNCKEY.fdk   的取值*/

#define LFS_PIN_FK_FDK01                  (0x00000001)
#define LFS_PIN_FK_FDK02                  (0x00000002)
#define LFS_PIN_FK_FDK03                  (0x00000004)
#define LFS_PIN_FK_FDK04                  (0x00000008)
#define LFS_PIN_FK_FDK05                  (0x00000010)
#define LFS_PIN_FK_FDK06                  (0x00000020)
#define LFS_PIN_FK_FDK07                  (0x00000040)
#define LFS_PIN_FK_FDK08                  (0x00000080)
#define LFS_PIN_FK_FDK09                  (0x00000100)
#define LFS_PIN_FK_FDK10                  (0x00000200)
#define LFS_PIN_FK_FDK11                  (0x00000400)
#define LFS_PIN_FK_FDK12                  (0x00000800)
#define LFS_PIN_FK_FDK13                  (0x00001000)
#define LFS_PIN_FK_FDK14                  (0x00002000)
#define LFS_PIN_FK_FDK15                  (0x00004000)
#define LFS_PIN_FK_FDK16                  (0x00008000)
#define LFS_PIN_FK_FDK17                  (0x00010000)
#define LFS_PIN_FK_FDK18                  (0x00020000)
#define LFS_PIN_FK_FDK19                  (0x00040000)
#define LFS_PIN_FK_FDK20                  (0x00080000)
#define LFS_PIN_FK_FDK21                  (0x00100000)
#define LFS_PIN_FK_FDK22                  (0x00200000)
#define LFS_PIN_FK_FDK23                  (0x00400000)
#define LFS_PIN_FK_FDK24                  (0x00800000)
#define LFS_PIN_FK_FDK25                  (0x01000000)
#define LFS_PIN_FK_FDK26                  (0x02000000)
#define LFS_PIN_FK_FDK27                  (0x04000000)
#define LFS_PIN_FK_FDK28                  (0x08000000)
#define LFS_PIN_FK_FDK29                  (0x10000000)
#define LFS_PIN_FK_FDK30                  (0x20000000)
#define LFS_PIN_FK_FDK31                  (0x40000000)
#define LFS_PIN_FK_FDK32                  (0x80000000)

/*LFSPINCRYPT.mode   的取值*/

#define LFS_PIN_MODEENCRYPT               (1)
#define LFS_PIN_MODEDECRYPT               (2)
#define LFS_PIN_MODERANDOM                (3)

/*LFSPINENTRY.completion   的取值*/

#define LFS_PIN_COMPAUTO                  (0)
#define LFS_PIN_COMPENTER                 (1)
#define LFS_PIN_COMPCANCEL                (2)
#define LFS_PIN_COMPCONTINUE              (6)
#define LFS_PIN_COMPCLEAR                 (7)
#define LFS_PIN_COMPBACKSPACE             (8)
#define LFS_PIN_COMPFDK                   (9)
#define LFS_PIN_COMPHELP                  (10)
#define LFS_PIN_COMPFK                    (11)
#define LFS_PIN_COMPCONTFDK               (12)


/*LFSPINSECMSG.protocol   的取值*/
#define LFS_PIN_PROTISOAS                 (1)
#define LFS_PIN_PROTISOLZ                 (2)
#define LFS_PIN_PROTISOPS                 (3)
#define LFS_PIN_PROTCHIPZKA               (4)
#define LFS_PIN_PROTRAWDATA               (5)
#define LFS_PIN_PROTPBM                   (6)
#define LFS_PIN_PROTHSMLDI                (7)
#define LFS_PIN_PROTGENAS                 (8)


/*LFSPINHSMINIT.init_mode.   的取值*/
#define LFS_PIN_INITTEMP                  (1)
#define LFS_PIN_INITDEFINITE              (2)
#define LFS_PIN_INITIRREVERSIBLE          (3)

/*LFSPINENCIO.protocol   的取值*/
#define LFS_PIN_ENC_PROT_CH               (0x0001)
#define LFS_PIN_ENC_PROT_GIECB              (0x0002)

/* LFS_SRVE_PIN_CERTIFICATE_CHANGE   的取值*/
#define LFS_PIN_CERT_PRIMARY              (0x00000001)
#define LFS_PIN_CERT_SECONDARY            (0x00000002)
#define LFS_PIN_CERT_NOTREADY             (0x00000004)

/* LFSPINCAPS.rsa_authentication_scheme 和 fast-track 能力扩展参数REMOTE_KEY_SCHEME 的取值*/
#define LFS_PIN_RSA_AUTH_2PARTY_SIG       (0x00000001)
#define LFS_PIN_RSA_AUTH_3PARTY_CERT      (0x00000002)

/* LFSPINCAPS.signature_scheme 和 fast-track 能力扩展参数 SIGNATURE_CAPABILITIES 的取值*/
#define LFS_PIN_SIG_GEN_RSA_KEY_PAIR          (0x00000001)
#define LFS_PIN_SIG_RANDOM_NUMBER             (0x00000002)
#define LFS_PIN_SIG_EXPORT_EPP_ID             (0x00000004)

/*LFSPINIMPORTRSAPUBLICKEY. rsa_signature_algorithm   的取值*/
#define LFS_PIN_SIGN_NA                   (0)
#define LFS_PIN_SIGN_RSASSA_PKCS1_V1_5    (0x00000001)
#define LFS_PIN_SIGN_RSASSA_PSS           (0x00000002)

/*LFSPINIMPORTRSAPUBLICKEYOUTPUT.rsa_key_check_mode   的取值*/
#define LFS_PIN_RSA_KCV_NONE              (0x00000000)
#define LFS_PIN_RSA_KCV_SHA1              (0x00000001)

/*LFSPINEXPORTRSAISSUERSIGNEDITEM.export_item_type */
/*         和  LFSPINEXPORTRSAEPPSIGNEDITEM.export_item_type  的取值*/
#define LFS_PIN_EXPORT_EPP_ID             (0x0001)
#define LFS_PIN_EXPORT_PUBLIC_KEY         (0x0002)

/*LFSPINIMPORTRSASIGNEDDESKEY. rsa_enc_ipher_algorithm   的取值*/
#define LFS_PIN_CRYPT_RSAES_PKCS1_V1_5    (0x00000001)
#define LFS_PIN_CRYPT_RSAES_OAEP          (0x00000002)

/*LFSPINGENERATERSAKEYPAIR.exponent_value   的取值*/
#define LFS_PIN_DEFAULT                   (0)
#define LFS_PIN_EXPONENT_1                (1)
#define LFS_PIN_EXPONENT_4                (2)
#define LFS_PIN_EXPONENT_16               (3)

/*LFSPINIMPORTRSASIGNEDDESKEYOUTPUT.key_length 和  */
/*		LFSPINIMPORTRSAENCIPHEREDPKCS7KEYOUTPUT.key_length   的取值*/
#define LFS_PIN_KEYSINGLE			  (0x0001)
#define LFS_PIN_KEYDOUBLE			  (0x0002)

/*LFSPINGETCERTIFICATE.get_certificate    的取值*/
#define LFS_PIN_PUBLICENCKEY              (1)
#define LFS_PIN_PUBLICVERIFICATIONKEY     (2)


/* LFSPINEMVIMPORTPUBLICKEY.import_scheme   的取值*/
#define LFS_PIN_EMV_IMPORT_PLAIN_CA       (0x0001)
#define LFS_PIN_EMV_IMPORT_CHKSUM_CA      (0x0002)
#define LFS_PIN_EMV_IMPORT_EPI_CA         (0x0003)
#define LFS_PIN_EMV_IMPORT_ISSUER         (0x0004)
#define LFS_PIN_EMV_IMPORT_ICC            (0x0005)
#define LFS_PIN_EMV_IMPORT_ICC_PIN        (0x0006)
#define LFS_PIN_EMV_IMPORT_PKCSV1_5_CA    (0x0007)

/* LFSPINDIGEST.hash_algorithm   的取值*/
#define LFS_PIN_HASH_SHA1_DIGEST          (0x0001)

/*LFSPINSECUREKEYDETAIL.key_entry_mode   的取值*/
#define LFS_PIN_SECUREKEY_NOTSUPP      (0x0000)
#define LFS_PIN_SECUREKEY_REG_SHIFT    (0x0001)
#define LFS_PIN_SECUREKEY_REG_UNIQUE   (0x0002)
#define LFS_PIN_SECUREKEY_IRREG_SHIFT  (0x0004)
#define LFS_PIN_SECUREKEY_IRREG_UNIQUE (0x0008)


/* LFS PIN 错误   的取值*/ 

#define LFS_ERR_PIN_KEYNOTFOUND           (-(PIN_SERVICE_OFFSET + 0))
#define LFS_ERR_PIN_MODENOTSUPPORTED      (-(PIN_SERVICE_OFFSET + 1))
#define LFS_ERR_PIN_ACCESSDENIED          (-(PIN_SERVICE_OFFSET + 2))
#define LFS_ERR_PIN_INVALIDID             (-(PIN_SERVICE_OFFSET + 3))
#define LFS_ERR_PIN_DUPLICATEKEY          (-(PIN_SERVICE_OFFSET + 4))
#define LFS_ERR_PIN_KEYNOVALUE            (-(PIN_SERVICE_OFFSET + 6))
#define LFS_ERR_PIN_USEVIOLATION          (-(PIN_SERVICE_OFFSET + 7))
#define LFS_ERR_PIN_NOPIN                 (-(PIN_SERVICE_OFFSET + 8))
#define LFS_ERR_PIN_INVALIDKEYLENGTH      (-(PIN_SERVICE_OFFSET + 9))
#define LFS_ERR_PIN_KEYINVALID            (-(PIN_SERVICE_OFFSET + 10))
#define LFS_ERR_PIN_KEYNOTSUPPORTED       (-(PIN_SERVICE_OFFSET + 11))
#define LFS_ERR_PIN_NOACTIVEKEYS          (-(PIN_SERVICE_OFFSET + 12))
#define LFS_ERR_PIN_NOTERMINATEKEYS       (-(PIN_SERVICE_OFFSET + 14))
#define LFS_ERR_PIN_MINIMUMLENGTH         (-(PIN_SERVICE_OFFSET + 15))
#define LFS_ERR_PIN_PROTOCOLNOTSUPP       (-(PIN_SERVICE_OFFSET + 16))
#define LFS_ERR_PIN_INVALIDDATA           (-(PIN_SERVICE_OFFSET + 17))
#define LFS_ERR_PIN_NOTALLOWED            (-(PIN_SERVICE_OFFSET + 18))
#define LFS_ERR_PIN_NOKEYRAM              (-(PIN_SERVICE_OFFSET + 19))
#define LFS_ERR_PIN_NOCHIPTRANSACTIVE     (-(PIN_SERVICE_OFFSET + 20))
#define LFS_ERR_PIN_ALGORITHMNOTSUPP      (-(PIN_SERVICE_OFFSET + 21))
#define LFS_ERR_PIN_FORMATNOTSUPP         (-(PIN_SERVICE_OFFSET + 22))
#define LFS_ERR_PIN_HSMSTATEINVALID       (-(PIN_SERVICE_OFFSET + 23))
#define LFS_ERR_PIN_MACINVALID            (-(PIN_SERVICE_OFFSET + 24))
#define LFS_ERR_PIN_PROTINVALID           (-(PIN_SERVICE_OFFSET + 25))
#define LFS_ERR_PIN_FORMATINVALID         (-(PIN_SERVICE_OFFSET + 26))
#define LFS_ERR_PIN_CONTENTINVALID        (-(PIN_SERVICE_OFFSET + 27))
#define LFS_ERR_PIN_SIG_NOT_SUPP          (-(PIN_SERVICE_OFFSET + 29))
#define LFS_ERR_PIN_INVALID_MOD_LEN       (-(PIN_SERVICE_OFFSET + 31))
#define LFS_ERR_PIN_INVALIDCERTSTATE      (-(PIN_SERVICE_OFFSET + 32))
#define LFS_ERR_PIN_KEY_GENERATION_ERROR  (-(PIN_SERVICE_OFFSET + 33))
#define LFS_ERR_PIN_EMV_VERIFY_FAILED     (-(PIN_SERVICE_OFFSET + 34))
#define LFS_ERR_PIN_RANDOMINVALID         (-(PIN_SERVICE_OFFSET + 35))
#define LFS_ERR_PIN_SIGNATUREINVALID      (-(PIN_SERVICE_OFFSET + 36))
#define LFS_ERR_PIN_SNSCDINVALID          (-(PIN_SERVICE_OFFSET + 37))
#define LFS_ERR_PIN_NORSAKEYPAIR          (-(PIN_SERVICE_OFFSET + 38))



/*=================================================================     */
/* PIN 命令结构 和变量  取值*/
/*=================================================================  	 */

typedef struct _lfs_pin_status
{
    WORD                device;
    WORD                enc_stat;
    LPSTR               extra;
} LFSPINSTATUS, * LPLFSPINSTATUS;

typedef struct _lfs_pin_caps
{
    WORD                dev_class;
    WORD                type;
    BOOL                compound;
    USHORT              key_num;
    WORD                algorithms;
    WORD                pin_formats;
    WORD                derivation_algorithms;
    WORD                presentation_algorithms;
    WORD                display;
    BOOL                id_connect;
    WORD                id_key;
    WORD                validation_algorithms;
    WORD                key_check_modes;
    LPSTR               extra;
} LFSPINCAPS, * LPLFSPINCAPS;


typedef struct _lfs_pin_fdk
{
    ULONG               fdk;
    USHORT              x_position;
    USHORT              y_position;
} LFSPINFDK, * LPLFSPINFDK;

typedef struct _lfs_pin_func_key_detail
{
    ULONG               func_mask;
    USHORT              number_fdks;
    LPLFSPINFDK       * fdks;
} LFSPINFUNCKEYDETAIL, * LPLFSPINFUNCKEYDETAIL;

typedef struct _lfs_pin_key_detail
{
    LPSTR         key_name;
    DWORD         use;
    BYTE          generation;
    BYTE          version;
    BYTE          activating_date[4];
    BYTE          expiry_date[4];
    BOOL          loaded;
} LFSPINKEYDETAIL, * LPLFSPINKEYDETAIL;

/* LFS_INF_PIN_SECUREKEY_DETAIL 输出结构   */
typedef struct _lfs_pin_hex_keys
{
    USHORT         x_pos;
    USHORT         y_pos;
    USHORT         x_size;
    USHORT         y_size;
    ULONG          fk;
    ULONG          shift_fk;
} LFSPINHEXKEYS, * LPLFSPINHEXKEYS;

/* LFS_INF_PIN_SECUREKEY_DETAIL 的结构   */
typedef struct _lfs_pin_secure_key_detail
{
    WORD                   key_entry_mode;
    LPLFSPINFUNCKEYDETAIL  func_key_detail;
    ULONG                  clear_fdk;
    ULONG                  cancel_fdk;
    ULONG                  backspace_fdk;
    ULONG                  enter_fdk;
    WORD                   columns;
    WORD                   rows;
    LPLFSPINHEXKEYS      * hex_keys;
} LFSPINSECUREKEYDETAIL, * LPLFSPINSECUREKEYDETAIL;


/*=================================================================   */
/* PIN 执行命令结构   */
/*=================================================================   */

typedef struct _lfs_hex_data
{
    USHORT              length;
    LPBYTE              data;
} LFSXDATA, * LPLFSXDATA;

typedef struct _lfs_pin_crypt
{
    WORD                mode;
    LPSTR               key;
    LPLFSXDATA          key_enc_key;
    WORD                algorithm;
    LPSTR               start_value_key;
    LPLFSXDATA          start_value;
    BYTE                padding;
    BYTE                compression;
    LPLFSXDATA          crypt_data;
} LFSPINCRYPT, * LPLFSPINCRYPT;

typedef struct _lfs_pin_import
{
    LPSTR               key;
    LPSTR               enc_key;
    LPLFSXDATA          ident;
    LPLFSXDATA          value;
    WORD                use;
} LFSPINIMPORT, * LPLFSPINIMPORT;

typedef struct _lfs_pin_derive
{
    WORD                derivation_algorithm;
    LPSTR               key;
    LPSTR               key_gen_key;
    LPSTR               start_value_key;
    LPLFSXDATA          start_value;
    BYTE                padding;
    LPLFSXDATA          input_data;
    LPLFSXDATA          ident;
 } LFSPINDERIVE, * LPLFSPINDERIVE;

typedef struct _lfs_pin_getpin
{
    USHORT              min_len;
    USHORT              max_len;
    BOOL                auto_end;
    CHAR                echo;
    ULONG               active_fdks;
    ULONG               active_keys;
    ULONG               terminate_fdks;
    ULONG               terminate_keys;
} LFSPINGETPIN, * LPLFSPINGETPIN;

typedef struct _lfs_pin_entry
{
    USHORT              digits;
    WORD                completion;
} LFSPINENTRY, * LPLFSPINENTRY;

typedef struct _lfs_pin_local_des
{
    LPSTR               validation_data;
    LPSTR               offset;
    BYTE                padding;
    USHORT              max_pin;
    USHORT              val_digits;
    BOOL                no_leading_zero;
    LPSTR               key;
    LPLFSXDATA          key_enc_key;
    LPSTR               dec_table;
} LFSPINLOCALDES, * LPLFSPINLOCALDES;

typedef struct _lfs_pin_create_offset
{
    LPSTR               validation_data;
    BYTE                padding;
    USHORT              max_pin;
    USHORT              val_digits;
    LPSTR               key;
    LPLFSXDATA          key_enc_key;
    LPSTR               dec_table;
} LFSPINCREATEOFFSET, * LPLFSPINCREATEOFFSET;

typedef struct _lfs_pin_local_eurocheque
{
    LPSTR               eurocheque_data;
    LPSTR               pvv;
    WORD                first_enc_digits;
    WORD                first_enc_offset;
    WORD                pvv_digits;
    WORD                pvv_offset;
    LPSTR               key;
    LPLFSXDATA          key_encKey;
    LPSTR               dec_table;
} LFSPINLOCALEUROCHEQUE, * LPLFSPINLOCALEUROCHEQUE;

typedef struct _lfs_pin_local_visa
{
    LPSTR               pan;
    LPSTR               pvv;
    WORD                pvv_digits;
    LPSTR               key;
    LPLFSXDATA          key_enc_key;
} LFSPINLOCALVISA, * LPLFSPINLOCALVISA;

typedef struct _lfs_pin_presentidc
{
    WORD                present_algorithm;
    WORD                chip_protocol;
    ULONG               chip_data_length;
    LPBYTE              chip_data;
    LPVOID              algorithm_data;
} LFSPINPRESENTIDC, * LPLFSPINPRESENTIDC;

typedef struct _lfs_pin_present_result
{
    WORD                chip_protocol;
    ULONG               chip_data_length;
    LPBYTE              chip_data;
} LFSPINPRESENTRESULT, * LPLFSPINPRESENTRESULT;

typedef struct _lfs_pin_presentclear
{
    ULONG               pin_pointer;
    USHORT              pin_offset;
} LFSPINPRESENTCLEAR, * LPLFSPINPRESENTCLEAR;


typedef struct _lfs_pin_block
{
    LPSTR               customer_data;
    LPSTR               xor_data;
    BYTE                padding;
    WORD                format;
    LPSTR               key;
    LPSTR               key_enc_key;
    DWORD		algorithm;
} LFSPINBLOCK, * LPLFSPINBLOCK;


typedef struct _lfs_pin_getdata
{
    USHORT              max_len;
    BOOL                auto_end;
    ULONG               active_fdks;
    ULONG               active_keys;
    ULONG               terminate_fdks;
    ULONG               terminate_keys;
} LFSPINGETDATA, * LPLFSPINGETDATA;

typedef struct _lfs_pin_key
{
    WORD          completion;
    ULONG         digit;
} LFSPINKEY, * LPLFSPINKEY;

typedef struct _lfs_pin_data
{
    USHORT              keys;
    LPLFSPINKEY        *pin_keys;
    WORD                completion;
} LFSPINDATA, * LPLFSPINDATA;

typedef struct _lfs_pin_init
{
    LPLFSXDATA          ident;
    LPLFSXDATA          key;
} LFSPININIT, * LPLFSPININIT;

typedef struct _lfs_pin_local_banksys
{
    LPLFSXDATA          atm_vac;
} LFSPINLOCALBANKSYS, * LPLFSPINLOCALBANKSYS;

typedef struct _lfs_pin_banksys_io
{
    ULONG               length;
    LPBYTE              data;
} LFSPINBANKSYSIO, * LPLFSPINBANKSYSIO;

typedef struct _lfs_pin_secure_message
    {
    WORD          protocol;
    ULONG         length;
    LPBYTE        msg;
} LFSPINSECMSG, * LPLFSPINSECMSG;

typedef struct _lfs_pin_import_key_ex
{
    LPSTR       key;
    LPSTR       enc_key;
    LPLFSXDATA  value;
    LPLFSXDATA  control_vector;
    DWORD       use;
    WORD        key_check_mode;
    LPLFSXDATA  key_check_value;
} LFSPINIMPORTKEYEX, * LPLFSPINIMPORTKEYEX;

typedef struct _lfs_pin_enc_io
{
    WORD          protocol;
    ULONG         data_length;
    LPVOID        data;
} LFSPINENCIO, *LPLFSPINENCIO;



/* LFS_CMD_PIN_SECUREKEY_ENTRY 命令输入结构*/
typedef struct _lfs_pin_secure_key_entry
{
    USHORT        key_len;
    BOOL          auto_end;
    ULONG         active_fdks;
    ULONG         active_keys;
    ULONG         terminate_fdks;
    ULONG         terminate_keys;
    WORD          verification_type;
} LFSPINSECUREKEYENTRY, * LPLFSPINSECUREKEYENTRY;

/* LFS_CMD_PIN_SECUREKEY_ENTRY 命令输出结构  */
typedef struct _lfs_pin_secure_key_entry_out
{
    USHORT       digits;
    WORD         completion;
    LPLFSXDATA   kcv;
} LFSPINSECUREKEYENTRYOUT, * LPLFSPINSECUREKEYENTRYOUT;

typedef struct _lfs_pin_import_rsa_public_key
{
    LPSTR        key;
    LPLFSXDATA   value;
    DWORD        use;
    LPSTR        sig_key;
    DWORD        rsa_signature_algorithm;
    LPLFSXDATA   signature;
} LFSPINIMPORTRSAPUBLICKEY, * LPLFSPINIMPORTRSAPUBLICKEY;

typedef struct _lfs_pin_import_rsa_public_key_output
{
    DWORD        rsa_key_check_mode;
    LPLFSXDATA   key_check_value;
} LFSPINIMPORTRSAPUBLICKEYOUTPUT, * LPLFSPINIMPORTRSAPUBLICKEYOUTPUT;

typedef struct _lfs_pin_export_rsa_issuer_signed_item
{
    WORD         export_item_type;
    LPSTR        name;
} LFSPINEXPORTRSAISSUERSIGNEDITEM, * LPLFSPINEXPORTRSAISSUERSIGNEDITEM;

typedef struct _lfs_pin_export_rsa_issuer_signed_item_output
{
    LPLFSXDATA   value;
    DWORD        rsa_signature_algorithm;
    LPLFSXDATA   signature;
}LFSPINEXPORTRSAISSUERSIGNEDITEMOUTPUT, * LPLFSPINEXPORTRSAISSUERSIGNEDITEMOUTPUT;

typedef struct _lfs_pin_import_rsa_signed_des_key
{
    LPSTR        key;
    LPSTR        decrypt_key;
    DWORD        rsa_enc_ipher_algorithm;
    LPLFSXDATA   value;
    DWORD        use;
    LPSTR        sig_key;
    DWORD        rsa_signature_algorithm;
    LPLFSXDATA   signature;
} LFSPINIMPORTRSASIGNEDDESKEY, * LPLFSPINIMPORTRSASIGNEDDESKEY;

typedef struct _lfs_pin_import_rsa_signed_des_key_output
{
    WORD         key_length;
    WORD         key_check_mode;
    LPLFSXDATA   key_check_value;
} LFSPINIMPORTRSASIGNEDDESKEYOUTPUT, * LPLFSPINIMPORTRSASIGNEDDESKEYOUTPUT;

typedef struct _lfs_pin_generate_rsa_key
{
    LPSTR        key;
    DWORD        use;
    WORD         modulus_length;
    WORD         exponent_value;
} LFSPINGENERATERSAKEYPAIR, * LPLFSPINGENERATERSAKEYPAIR;

typedef struct _lfs_pin_export_rsa_epp_signed_item
{
    WORD         export_item_type;
    LPSTR        name;
    LPSTR        sig_key;
    DWORD        signature_algorithm;
} LFSPINEXPORTRSAEPPSIGNEDITEM, * LPLFSPINEXPORTRSAEPPSIGNEDITEM;

typedef struct _lfs_pin_export_rsa_epp_signed_item_output
{
    LPLFSXDATA   value;
    LPLFSXDATA   self_signature;
    LPLFSXDATA   signature;
} LFSPINEXPORTRSAEPPSIGNEDITEMOUTPUT, * LPLFSPINEXPORTRSAEPPSIGNEDITEMOUTPUT;



typedef struct _lfs_pin_load_certificate
{
    LPLFSXDATA   load_certificate;
} LFSPINLOADCERTIFICATE, *LPLFSPINLOADCERTIFICATE;

typedef struct _lfs_pin_load_certificate_output
{
    LPLFSXDATA   certificate_data;
} LFSPINLOADCERTIFICATEOUTPUT, *LPLFSPINLOADCERTIFICATEOUTPUT;

typedef struct _lfs_pin_get_certificate
{
    WORD         get_certificate;
} LFSPINGETCERTIFICATE, *LPLFSPINGETCERTIFICATE;

typedef struct _lfs_pin_get_certificate_output
{
    LPLFSXDATA   certificate;
} LFSPINGETCERTIFICATEOUTPUT, *LPLFSPINGETCERTIFICATEOUTPUT;

typedef struct lfs_pin_replace_certificate
{
    LPLFSXDATA   replace_certificate;
} LFSPINREPLACECERTIFICATE, *LPLFSPINREPLACECERTIFICATE;

typedef struct _lfs_pin_replace_certificate_output
{
   LPLFSXDATA    new_certificate_data;
} LFSPINREPLACECERTIFICATEOUTPUT, *LPLFSPINREPLACECERTIFICATEOUTPUT;

typedef struct _lfs_pin_start_key_exchange
{
    LPLFSXDATA   random_item;
} LFSPINSTARTKEYEXCHANGE, *LPLFSPINSTARTKEYEXCHANGE;

typedef struct _lfs_pin_import_rsa_enciphered_pkcs7_key
{
    LPLFSXDATA   import_rsa_key_in;
    LPSTR        key;
    DWORD        use;
} LFSPINIMPORTRSAENCIPHEREDPKCS7KEY, * LPLFSPINIMPORTRSAENCIPHEREDPKCS7KEY;

typedef struct _lfs_pin_import_rsa_enciphered_pkcs7_key_output
{
    WORD	 key_length;
    LPLFSXDATA   rsa_data;
}LFSPINIMPORTRSAENCIPHEREDPKCS7KEYOUTPUT, *LPLFSPINIMPORTRSAENCIPHEREDPKCS7KEYOUTPUT;

typedef struct _lfs_pin_emv_import_public_key
{
    LPSTR         key;
    DWORD         use;
    WORD          import_scheme;
    LPLFSXDATA    import_data;
    LPSTR         sig_key;
} LFSPINEMVIMPORTPUBLICKEY, * LPLFSPINEMVIMPORTPUBLICKEY;
typedef struct _lfs_pin_emv_import_public_key_output
{
    LPSTR         expiry_date;
} LFSPINEMVIMPORTPUBLICKEYOUTPUT, * LPLFSPINEMVIMPORTPUBLICKEYOUTPUT;

typedef struct _lfs_pin_digest
{
    WORD          hash_algorithm;
    LPLFSXDATA    digest_input;
} LFSPINDIGEST, * LPLFSPINDIGEST;

typedef struct _lfs_pin_digest_output
{
    LPLFSXDATA    digest_output;
} LFSPINDIGESTOUTPUT, * LPLFSPINDIGESTOUTPUT;

typedef struct _lfs_pin_hsm_init
{
    WORD          init_mode;
    LPLFSXDATA    online_time;
} LFSPINHSMINIT, * LPLFSPINHSMINIT;

typedef struct _lfs_pin_generate_kcv
{
    LPSTR         key;
    WORD          key_check_mode;
} LFSPINGENERATEKCV, * LPLFSPINGENERATEKCV;

typedef struct _lfs_pin_kcv
{
    LPLFSXDATA    kcv;
} LFSPINKCV, * LPLFSPINKCV;

/*=================================================================   */
/* PIN 消息结构    */
/*=================================================================   */

typedef struct _lfs_pin_access
{
    LPSTR         key_name;
    LONG          error_code;
} LFSPINACCESS, * LPLFSPINACCESS;


/* restore alignment    */
#pragma pack(pop)

#ifdef __cplusplus
}       /*extern "C"   */
#endif

#endif    /* __INC_LFSPIN__H    */
