//////////////////////////////////////////////////////////////////////////////////////////
// Header  = IC data management inner TBL definition
//////////////////////////////////////////////////////////////////////////////////////////
#ifndef UPICDTBL_H
#define UPICDTBL_H

//// Information management TBL ///////////////////////////////////////////////////////////////////////////

WORD wInfoType[][IC_ALL_MEMBER] =
{
	//信息类型
	{ ICDPC_IC_DF_NAME,										16,		BINARY,	UNPACK,	VAR,		0,		IC,		5	},
	{ ICDPC_IC_SFI,											1,		BINARY,	UNPACK,	CONSTANT,	0,		IC,		1	},
	{ ICDPC_IC_APPLICATION_LABEL,							16,		ANS,	UNPACK,	VAR,		0,		IC,		1	},
	{ ICDPC_IC_API,											1,		BINARY,	UNPACK,	CONSTANT,	0,		IC,		1	},
	{ ICDPC_IC_PDOL,										252,	BINARY,	UNPACK,	VAR,		0,		IC,		0	},
	{ ICDPC_IC_AID,											16,		BINARY,	UNPACK,	VAR,		0,		IC,		5	},
	{ ICDPC_IC_AIP,											2,		BINARY,	UNPACK,	CONSTANT,	0,		IC,		2	},
	{ ICDPC_IC_AFL,											252,	VA,		UNPACK,	VAR,		0,		IC,		0	},
	{ ICDPC_IC_AC,											8,		BINARY,	UNPACK,	CONSTANT,	0,		IC,		8	},
	{ ICDPC_IC_CID,											1,		BINARY,	UNPACK,	CONSTANT,	0,		IC,		1	},
	{ ICDPC_IC_ATC,											2,		BINARY,	UNPACK,	CONSTANT,	0,		IC,		2	},
	{ ICDPC_IC_LAST_ONLINE_ATC_REGISTER,					2,		BINARY,	UNPACK,	CONSTANT,	0,		IC,		2	},
	{ ICDPC_IC_SIGNED_DYNAMIC_APPLICATION_DATA,				248,	BINARY,	UNPACK,	VAR,		0,		IC,		0	},
	{ ICDPC_IC_ISSUER_APPLICATION_DATA,						32,		BINARY,	UNPACK,	VAR,		0,		IC,		0	},
	{ ICDPC_IC_CDOL1,										252,	BINARY,	UNPACK,	VAR,		0,		IC,		0	},
	{ ICDPC_IC_CDOL2,										252,	BINARY,	UNPACK,	VAR,		0,		IC,		0	},
	{ ICDPC_IC_CVM_LIST,									252,	BINARY,	UNPACK,	VAR,		0,		IC,		0	},
	{ ICDPC_IC_CARDHOLDER_NAME,								26,		ANS,	UNPACK,	VAR,		0,		IC,		2	},
	{ ICDPC_IC_CARDHOLDER_ID_NO,							40,		AN,		UNPACK,	VAR,		0,		IC,		1	},	
	{ ICDPC_IC_CARDHOLDER_ID_TYPE,							1,		CN,		PACK,	CONSTANT,	0,		IC,		1	},	
	{ ICDPC_IC_APPLICATION_VER_NO,							2,		BINARY,	UNPACK,	CONSTANT,	0,		IC,		2	},	
	{ ICDPC_IC_APPLICATION_EXPIRATION_DATE,					3,		NUM,	PACK,	CONSTANT,	0,		IC,		3	},
	{ ICDPC_IC_APPLICATION_EFFECTIVE_DATE,					3,		NUM,	PACK,	CONSTANT,	0,		IC,		3	},
	{ ICDPC_IC_PAN,											10,		CN,		PACK,	VAR,		0,		IC,		0	},
	{ ICDPC_IC_PAN_SEQUENCE_NO,								1,		NUM,	PACK,	CONSTANT,	0,		IC,		1	},
	{ ICDPC_IC_AUC,											2,		BINARY,	UNPACK,	CONSTANT,	0,		IC,		2	},
	{ ICDPC_IC_IAC_DENIAL,									5,		BINARY,	UNPACK,	CONSTANT,	0,		IC,		5	},
	{ ICDPC_IC_IAC_ONLINE,									5,		BINARY,	UNPACK,	CONSTANT,	0,		IC,		5	},
	{ ICDPC_IC_IAC_DEFAULT,									5,		BINARY,	UNPACK,	CONSTANT,	0,		IC,		5	},
	{ ICDPC_IC_SINGED_STATIC_APPLICATION_DATA,				248,	BINARY,	UNPACK,	VAR,		0,		IC,		0	},
	{ ICDPC_IC_ISSUER_PUBLIC_KEY_CERTIFICATE,				248,	BINARY,	UNPACK,	VAR,		0,		IC,		0	},
	{ ICDPC_IC_CA_PUBLIC_KEY_INDEX,							1,		BINARY,	UNPACK,	CONSTANT,	0,		IC,		1	},
	{ ICDPC_IC_ISSUER_PUBLIC_KEY_REMAINDER,					248,	BINARY,	UNPACK,	VAR,		0,		IC,		0	},
	{ ICDPC_IC_ISSUER_PUBLIC_KEY_EXPONENT,					3,		BINARY,	UNPACK,	VAR,		0,		IC,		1	},
	{ ICDPC_IC_ICC_PUBLIC_KEY_CERTIFICATE,					248,	BINARY,	UNPACK,	VAR,		0,		IC,		0	},
	{ ICDPC_IC_ICC_PUBLIC_KEY_EXPONENT,						3,		BINARY,	UNPACK,	VAR,		0,		IC,		1	},
	{ ICDPC_IC_ICC_PUBLIC_KEY_REMAINDER,					248,	BINARY,	UNPACK,	VAR,		0,		IC,		0	},
	{ ICDPC_IC_DDOL,										252,	BINARY,	UNPACK,	VAR,		0,		IC,		0	},
	{ ICDPC_IC_HOLDER_PIN_TRY_COUNTER,						1,		BINARY,	UNPACK,	CONSTANT,	0,		IC,		1	},
	{ ICDPC_IC_APPLICATION_CURRENCY_CODE,					2,		NUM,	PACK,	CONSTANT,	0,		IC,		2	},
	{ ICDPC_IC_CURRENCY_EXPONENT,							1,		NUM,	PACK,	CONSTANT,	0,		IC,		1	},
	{ ICDPC_IC_CARDHOLDER_NAME_EXTENDED,					45,		ANS,	UNPACK,	VAR,		0,		IC,		1	},
	{ ICDPC_IC_DATA_AUTHENTICATION_CODE,					2,		BINARY,	UNPACK,	CONSTANT,	0,		IC,		2	},
	{ ICDPC_IC_ISSUER_COUNTRY_CODE,							2,		NUM,	PACK,	CONSTANT,	0,		IC,		2	},
	{ ICDPC_IC_ISSUER_URL,									252,	ANS,	UNPACK,	VAR,		0,		IC,		0	},
	{ ICDPC_IC_IBAN,										34,		VA,		UNPACK,	VAR,		0,		IC,		0	},
	{ ICDPC_IC_BIC,											11,		VA,		UNPACK,	VAR,		0,		IC,		8	},
	{ ICDPC_IC_STATIC_DATA_AUTHENTICATION_TAG_LIST,			252,	BINARY,	UNPACK,	VAR,		0,		IC,		0	},
	{ ICDPC_IC_LANGUAGE_PREFERENCE,							8,		AN,		UNPACK,	VAR,		0,		IC,		2	},
	{ ICDPC_IC_FCI_ISSUER_DISCRETIONARY_DATA_EMV,			222,	VA,		UNPACK,	VAR,		0,		IC,		0	},
	{ ICDPC_IC_RESPONSE_MESSAGE_TEMPLATE_FORMAT1,			252,	VA,		UNPACK,	VAR,		0,		IC,		0	},
	{ ICDPC_IC_RESPONSE_MESSAGE_TEMPLATE_FORMAT2,			252,	VA,		UNPACK,	VAR,		0,		IC,		0	},
	{ ICDPC_IC_TEMPLATE_70,									252,	VA,		UNPACK,	VAR,		0,		IC,		0	},
	{ ICDPC_IC_FCI_TEMPLATE,								252,	VA,		UNPACK,	VAR,		0,		IC,		0	},
	{ ICDPC_IC_FCI_PROPRIETARY_TEMPLATE,					252,	VA,		UNPACK,	VAR,		0,		IC,		0	},
	{ ICDPC_IC_APPLICATION_TEMPLATE,						252,	BINARY,	UNPACK,	VAR,		0,		IC,		0	},
	{ ICDPC_IC_DDF_NAME,									16,		BINARY,	UNPACK,	VAR,		0,		IC,		5	},
	{ ICDPC_IC_DISCRETIONARY_DATA,							32,		BINARY,	UNPACK,	VAR,		0,		IC,		1	},
	{ ICDPC_IC_ISSUER_CODE_TABLE_INDEX,						1,		NUM,	PACK,	CONSTANT,	0,		IC,		1	},
	{ ICDPC_IC_AP_PREFERRED_NAME,							16,		ANS,	UNPACK,	VAR,		0,		IC,		1	},
	{ ICDPC_IC_LOWER_CONSECUTIVE_OFFLINE_LIMIT,				1,		BINARY,	UNPACK,	CONSTANT,	0,		IC,		1	},
	{ ICDPC_IC_UPPER_CONSECUTIVE_OFFLINE_LIMIT,				1,		BINARY,	UNPACK,	CONSTANT,	0,		IC,		1	},
	{ ICDPC_IC_TRACK1_DISCRETIONARY_DATA,					252,	ANS,	UNPACK,	VAR,		0,		IC,		0	},
	{ ICDPC_IC_TRACK2_DISCRETIONARY_DATA,					252,	CN,		PACK,	VAR,		0,		IC,		0	},
	{ ICDPC_IC_ICC_PIN_ENC_PUBLIC_KEY_CERTIFICATE,			248,	BINARY,	UNPACK,	VAR,		0,		IC,		0	},
	{ ICDPC_IC_ICC_PIN_ENC_PUBLIC_KEY_EXPONENT,				3,		BINARY,	UNPACK,	VAR,		0,		IC,		1	},
	{ ICDPC_IC_ICC_PIN_ENC_PUBLIC_KEY_REMAINDER,			46,		BINARY,	UNPACK,	VAR,		0,		IC,		0	},
	{ ICDPC_IC_APPLICATION_REFERENCE_CURRENCY,				8,		NUM,	PACK,	VAR,		0,		IC,		2	},
	{ ICDPC_IC_IIN,											3,		NUM,	PACK,	CONSTANT,	0,		IC,		3	},	
	{ ICDPC_IC_TRACK2_EQUIVALENT_DATA,						19,		BINARY,	UNPACK,	VAR,		0,		IC,		0	},	
	{ ICDPC_IC_SERVICE_CODE,								2,		NUM,	PACK,	CONSTANT,	0,		IC,		2	},
	{ ICDPC_IC_ISSUER_COUNTRY_CODE_A2F,						2,		AN,		UNPACK,	CONSTANT,	0,		IC,		2	},
	{ ICDPC_IC_ISSUER_COUNTRY_CODE_A3F,						3,		AN,		UNPACK,	CONSTANT,	0,		IC,		3	},
	{ ICDPC_IC_DIRECTORY_DISCRETIONARY_TEMPLATE,			252,	VA,		UNPACK,	VAR,		0,		IC,		0	},
	{ ICDPC_IC_TDOL,										252,	BINARY,	UNPACK,	VAR,		0,		IC,		0	},
	{ ICDPC_IC_APPLICATION_REFERENCE_CURRENCY_EXPONENT,		4,		NUM,	PACK,	VAR,		0,		IC,		1	},
	{ ICDPC_IC_DYNAMIC_NUMBER,								8,		BINARY,	UNPACK,	VAR,		0,		IC,		2	},
	{ ICDPC_IC_LOG_ENTRY,									2,		BINARY,	UNPACK,	CONSTANT,	0,		IC,		2	},
	{ ICDPC_IC_LOG_FORMAT,									252,	BINARY,	UNPACK,	VAR,		0,		IC,		0	},
	{ ICDPC_IC_PRODUCT_IDENTIFICATION,						16,		BINARY,	UNPACK,	VAR,		0,		IC,		16	},  
	{ ICDPC_IC_BALANCE_DATA,								6,		NUM,	PACK,	CONSTANT,	0,		IC,		6	},	
	{ ICDPC_IC_BALANCE_LIMIT,								6,		NUM,	PACK,	CONSTANT,	0,		IC,		6	},	
	{ ICDPC_IC_ISSUER_AUTHORIZATION_CODE,					6,		ANS,	UNPACK,	CONSTANT,	0,		IC,		6	},	
	{ ICDPC_IC_SINGLE_TRANSACTION_LIMIT,					6,		NUM,	PACK,	CONSTANT,	0,		IC,		6	},	
	{ ICDPC_IC_RESET_THRESHOLD,								6,		NUM,	PACK,	CONSTANT,	0,		IC,		6	},	
	{ ICDPC_IC_ISSUER_AUTHENTICATION_FLAG,					1,		BINARY,	UNPACK,	CONSTANT,	0,		IC,		0	},
	{ ICDPC_TM_COMMAND_TEMPLATE,							252,	BINARY,	UNPACK,	VAR,		0,		TM,		0	},
	{ ICDPC_TM_ACQUIRER_ID,									6,		NUM,	PACK,	CONSTANT,	0,		TM,		6	},
	{ ICDPC_TM_ADDITIONAL_TERMINAL_CAPABILITIES,			5,		BINARY,	UNPACK,	CONSTANT,	0,		TM,		5	},
	{ ICDPC_TM_AMOUNT_AUTHORISED,							6,		NUM,	PACK,	CONSTANT,	0,		TM,		6	},
	{ ICDPC_TM_AMOUNT_OTHER,								6,		NUM,	PACK,	CONSTANT,	0,		TM,		6	},
	{ ICDPC_TM_9F66,										4,		NUM,	PACK,	CONSTANT,	0,		TM,		4	},
	{ ICDPC_TM_9F7A,										1,		NUM,	PACK,	CONSTANT,	0,		TM,		1	},
	{ ICDPC_TM_CAPP_TRADE_INDICATOR,						1,		NUM,	PACK,	CONSTANT,	0,		TM,		1	},
	{ ICDPC_TM_AID,											16,		BINARY,	UNPACK,	VAR,		0,		TM,		5	},
	{ ICDPC_TM_APPLICATION_VER_NO,							2,		BINARY,	UNPACK,	CONSTANT,	0x0000,	TM,		2	},
	{ ICDPC_TM_ARC,											2,		AN,		UNPACK,	CONSTANT,	0,		TM,		2	},
	{ ICDPC_TM_CVM_RESULTS,									3,		BINARY,	UNPACK,	CONSTANT,	0,		TM,		3	},
	{ ICDPC_TM_CA_PUBLIC_KEY_INDEX,							1,		BINARY,	UNPACK,	CONSTANT,	0,		TM,		1	},
	{ ICDPC_TM_MERCHANT_ID,									15,		ANS,	UNPACK,	CONSTANT,	0,		TM,		15	},
	{ ICDPC_TM_MERCHANT_CATEGORY_CODE,						2,		NUM,	PACK,	CONSTANT,	0,		TM,		2	},
	{ ICDPC_TM_POS_ENTRY_MODE,								1,		NUM,	PACK,	CONSTANT,	0,		TM,		1	},
	{ ICDPC_TM_TERMINAL_CAPABILITIES,						3,		BINARY,	UNPACK,	CONSTANT,	0,		TM,		3	},
	{ ICDPC_TM_TERMINAL_COUNTRY_CODE,						2,		NUM,	PACK,	CONSTANT,	0x0156,	TM,		2	},
	{ ICDPC_TM_TERMINAL_FLOOR_LIMIT,						4,		BINARY,	UNPACK,	CONSTANT,	0,		TM,		4	},
	{ ICDPC_TM_TERMINAL_ID,									8,		AN,		UNPACK,	CONSTANT,	0,		TM,		8	},
	{ ICDPC_TM_TERMINAL_TYPE,								1,		NUM,	PACK,	CONSTANT,	0x14,	TM,		1	},
	{ ICDPC_TM_TVR,											5,		BINARY,	UNPACK,	CONSTANT,	0,		TM,		5	},
	{ ICDPC_TM_TRANSACTION_CURRENCY_CODE,					2,		NUM,	PACK,	CONSTANT,	0x0156,	TM,		2	},
	{ ICDPC_TM_TRANSACTION_DATE,							3,		NUM,	PACK,	CONSTANT,	0,		TM,		3	},
	{ ICDPC_TM_TRANSACTION_TIME,							3,		NUM,	PACK,	CONSTANT,	0,		TM,		3	},
	{ ICDPC_TM_TRANSACTION_PIN_DATA,						252,	BINARY,	UNPACK,	VAR,		0,		TM,		0	},
	{ ICDPC_TM_TRANSACTION_SEQUENCE_COUNTER,				4,		NUM,	PACK,	VAR,		0,		TM,		2	},
	{ ICDPC_TM_TSI,											2,		BINARY,	UNPACK,	CONSTANT,	0,		TM,		2	},
	{ ICDPC_TM_TRANSACTION_TYPE,							1,		NUM,	PACK,	CONSTANT,	0,		TM,		1	},
	{ ICDPC_TM_UNPREDICTABLE_NUMBER,						4,		BINARY,	UNPACK,	CONSTANT,	0,		TM,		4	},
	{ ICDPC_TM_TRANSACTION_CURRENCY_EXPONENT,				1,		NUM,	PACK,	CONSTANT,	0x00,	TM,		1	},
	{ ICDPC_TM_AMOUNT_AUTHORISED_BINARY,					4,		BINARY,	UNPACK,	CONSTANT,	0,		TM,		4	},
	{ ICDPC_TM_TC_HASH_VALUE,								20,		BINARY,	UNPACK,	CONSTANT,	0,		TM,		20	},
	{ ICDPC_TM_AMOUNT_OTHER_BINARY,							4,		BINARY,	UNPACK,	CONSTANT,	0,		TM,		4	},
	{ ICDPC_TM_TERMINAL_RISK_MANAGEMENT_DATA,				8,		BINARY,	UNPACK,	VAR,		0,		TM,		1	},
	{ ICDPC_TM_IFD_SERIAL_NO,								8,		AN,		UNPACK,	CONSTANT,	0,		TM,		8	},
	{ ICDPC_TM_AMOUNT_REFERENCE_CURRENCY,					4,		BINARY,	UNPACK,	CONSTANT,	0,		TM,		4	},
	{ ICDPC_TM_TRANSACTION_REFERENCE_CURRENCY_CODE,			2,		NUM,	PACK,	CONSTANT,	0x0156,	TM,		2	},
	{ ICDPC_TM_TRANSACTION_REFERENCE_CURRENCY_EXPONENT,		1,		NUM,	PACK,	CONSTANT,	0x00,	TM,		1	},
	{ ICDPC_TM_MERCHANT_NAME_AND_LOCATION,					252,	ANS,	UNPACK,	VAR,		0,		TM,		0	},
	{ ICDPC_TM_SM_ALGORITHM,								1,		BINARY,	UNPACK,	CONSTANT,	0x00,	TM,		1	},
	{ ICDPC_ISSUER_AUTHENTICATION_DATA,						16,		BINARY,	UNPACK,	VAR,		0,		ISSUER,	8	},
	{ ICDPC_ISSUER_SCRIPT_TEMPLATE1,						252,	BINARY,	UNPACK,	VAR,		0,		ISSUER,	0	},
	{ ICDPC_ISSUER_SCRIPT_TEMPLATE2,						252,	BINARY,	UNPACK,	VAR,		0,		ISSUER,	0	},
	{ ICDPC_ISSUER_SCRIPT_COMMAND,							261,	BINARY,	UNPACK,	VAR,		0,		ISSUER,	0	},
	{ ICDPC_ISSUER_SCRIPIT_ID,								4,		BINARY,	UNPACK,	CONSTANT,	0,		ISSUER,	4	},
	{ ICDPC_ISSUER_AUTHORISATION_CODE,						6,		VA,		UNPACK,	CONSTANT,	0,		ISSUER,	6	},

	// tag not exist
	// IC card
	{ ICDPC_IC_HOLDER_OLD_PIN,								252,	BINARY,	UNPACK,	VAR,		0,		IC,	0	},
	{ ICDPC_IC_HOLDER_FIX_PIN,								1,		BINARY,	UNPACK,	CONSTANT,	0,		IC,	1	},
	{ ICDPC_IC_HOLDER_NEW_PIN,								252,	BINARY,	UNPACK,	VAR,		0,		IC,	0	},
	{ ICDPC_IC_HOLDER_PIN_TRY_MAX,							1,		BINARY,	UNPACK,	CONSTANT,	0,		IC,	1	},
	{ ICDPC_IC_UNPREDICTABLE_NUMBER,						8,		BINARY,	UNPACK,	CONSTANT,	0,		IC,	8	},
	// Terminal data
	{ ICDPC_TM_CA_PUBLIC_KEY_EXPONENT,						3,		BINARY,	UNPACK,	VAR,		0,		TM,	1	},
	{ ICDPC_TM_CA_PUBLIC_KEY_MODULUS,						248,	BINARY,	UNPACK,	VAR,		0,		TM,	0	},
	{ ICDPC_TM_TAC_DENIAL,									5,		BINARY,	UNPACK,	CONSTANT,	0,		TM,	5	},
	{ ICDPC_TM_TAC_ONLINE,									5,		BINARY,	UNPACK,	CONSTANT,	0,		TM,	5	},
	{ ICDPC_TM_TAC_DEFAULT,									5,		BINARY,	UNPACK,	CONSTANT,	0,		TM,	5	},
	{ ICDPC_TM_PSE,											14,		ANS,	UNPACK,	CONSTANT,	0,		TM,	14	},
	{ ICDPC_TM_APPLICATION_SELECTION_INDICATOR,				1,		BINARY,	UNPACK,	CONSTANT,	0,		TM,	1	},
	{ ICDPC_TM_DEFAUT_DDA_OBJECT_LIST,						252,	BINARY,	UNPACK,	VAR,		0,		TM,	0	},
	{ ICDPC_TM_ISSUER_SCRIPT_RESULT,						252,	BINARY,	UNPACK,	VAR,		0,		TM,	5	},
	// Terminal data（Electronic wallet / electronic communications）
	{ ICDPC_TM_USE_ALGORITHM,								1,		BINARY,	UNPACK,	CONSTANT,	0,		TM,	1	},	
	{ ICDPC_TM_KEY_INDEX,									1,		CN,		PACK,	CONSTANT,	0,		TM,	1	},	
	{ ICDPC_TM_ATM_NO,										6,		CN,		PACK,	CONSTANT,	0,		TM,	6	},	
	{ ICDPC_TM_ISSUER_ID,									8,		CN,		PACK,	CONSTANT,	0,		TM,	8	},
	{ ICDPC_TM_PME,											10,		NUM,	PACK,	CONSTANT,	0,		TM,	10	},
	// Individual data
	{ ICDPC_AID_LIST,										1410,	BINARY,	UNPACK,	VAR,		0,		SP,	0	},
	{ ICDPC_PDOL_RELATION_DATA,								252,	BINARY,	UNPACK,	VAR,		0,		SP,	0	},
	{ ICDPC_SDOL_RELATION_DATA,								252,	BINARY,	UNPACK,	VAR,		0,		SP,	0	},
	{ ICDPC_CDOL1_RELATION_DATA,							252,	BINARY,	UNPACK,	VAR,		0,		SP,	0	},
	{ ICDPC_CDOL2_RELATION_DATA,							252,	BINARY,	UNPACK,	VAR,		0,		SP,	0	},
	{ ICDPC_DDOL_RELATION_DATA,								252,	BINARY,	UNPACK,	VAR,		0,		SP,	0	},
	{ ICDPC_TDOL_RELATION_DATA,								252,	BINARY,	UNPACK,	VAR,		0,		SP,	0	},	
	{ ICDPC_STATUS,											2,		BINARY,	UNPACK,	CONSTANT,	0,		SP,	2	},
};

//// TBL ///////////////////////////////////////////////////////////////////////////

// Num code table
	BYTE byNUMCode [256][3] =
	  {"NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG",
	   "NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG",
	   "NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG",
	   "0" ,"1" ,"2" ,"3" ,"4" ,"5" ,"6" ,"7" ,"8" ,"9" ,"NG","NG","NG","NG","NG","NG",
	   "NG","NG","NG","NG","NG","NG","F" ,"NG","NG","NG","NG","NG","NG","NG","NG","NG",
	   "NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG",
	   "NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG",
	   "NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG",
	   "NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG",
	   "NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG",
	   "NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG",
	   "NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG",
	   "NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG",
	   "NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG",
	   "NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG",
	   "NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG"
	  };

// English num code table
	BYTE byANCode [256][3] =
	  {"NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG",
	   "NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG",
	   "NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG",
	   "0" ,"1" ,"2" ,"3" ,"4" ,"5" ,"6" ,"7" ,"8" ,"9" ,"NG","NG","NG","NG","NG","NG",
	   "NG","A" ,"B" ,"C" ,"D" ,"E" ,"F" ,"G" ,"H" ,"I" ,"J" ,"K" ,"L" ,"M" ,"N" ,"O" ,
	   "P" ,"Q" ,"R" ,"S" ,"T" ,"U", "V" ,"W" ,"X" ,"Y" ,"Z" ,"NG","NG","NG","NG","NG",
	   "NG","a" ,"b" ,"c" ,"d" ,"e" ,"f" ,"g" ,"h" ,"i" ,"j" ,"k" ,"l" ,"m" ,"n" ,"o" ,
	   "p" ,"q" ,"r" ,"s" ,"t" ,"u" ,"v" ,"w" ,"x" ,"y" ,"z" ,"NG","NG","NG","NG","NG",
	   "NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG",
	   "NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG",
	   "NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG",
	   "NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG",
	   "NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG",
	   "NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG",
	   "NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG",
	   "NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG"
	  };

// English num code special characters table
	BYTE byANSCode [256][3] =
	  {"NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG",
	   "NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG",
	   " " ,"!" ,"\"","#" ,"$" ,"%" ,"&" ,"'" ,"(" ,")" ,"*" ,"+" ,"," ,"-" ,"." ,"/" ,
	   "0" ,"1" ,"2" ,"3" ,"4" ,"5" ,"6" ,"7" ,"8" ,"9" ,":" ,";" ,"<" ,"=" ,">" ,"?" ,
	   "@" ,"A" ,"B" ,"C" ,"D" ,"E" ,"F" ,"G" ,"H" ,"I" ,"J" ,"K" ,"L" ,"M" ,"N" ,"O" ,
	   "P" ,"Q" ,"R" ,"S" ,"T" ,"U", "V" ,"W" ,"X" ,"Y" ,"Z" ,"[" ,"\\","]" ,"^" ,"_" ,
	   "`" ,"a" ,"b" ,"c" ,"d" ,"e" ,"f" ,"g" ,"h" ,"i" ,"j" ,"k" ,"l" ,"m" ,"n" ,"o" ,
	   "p" ,"q" ,"r" ,"s" ,"t" ,"u" ,"v" ,"w" ,"x" ,"y" ,"z" ,"{" ,"|" ,"}" ,"~" ,"NG",
	   "NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG",
	   "NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG",
	   "NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG",
	   "NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG",
	   "NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG",
	   "NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG",
	   "NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG",
	   "NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG","NG"
	  };

//// Terminal capacity ///////////////////////////////////////////////////////////////////////////

	struct TerminalCapabilities {
		BYTE byCardDataInput;	// Card data input function 
		BYTE byCVM; 			// CVMfunction
		BYTE bySec; 			// Security function
	} stTerminalCapabilities[] = {
		0x60,					
		0x40,					
		0x20,
	};

	struct AdditionalTerminalCapabilities {		
		BYTE byAtcData[5];						
	} stAdditionalTerminalCapabilities[] = {	
		0x80,	
		0x00,						
		0xB0,						
		0x50,						
		0x01,					
	};


	struct IssuerScriptResult {	
		BYTE byISResult[5];		
	} stIssuerScriptResult[] = {
		0x00,					
		0x00,					
		0x00,					
		0x00,					
		0x00,					
	};

//// Terminal action code ///////////////////////////////////////////////////////////////

	struct ActionCode{
		BYTE byTacData[5];
	};

	ActionCode stTAC_PROGRESS_DENIAL = {
		//PROGRESS DENIAL
		0x00,					
		0x00,					
		0x00,					
		0x00,					
		0x00					
	};
	ActionCode stTAC_PROGRESS_ONLINE = {
		//PROGRESS ONLINE
		0x00,					
		0x00,					
		0x00,					
		0x00,					
		0x00,					
	};
	ActionCode stTAC_PROGRESS_DEFAULT = {
		//PROGRESS DEFAULT
		0x00,					
		0x00,					
		0x00,					
		0x00,					
		0x00,					
	};

#endif
