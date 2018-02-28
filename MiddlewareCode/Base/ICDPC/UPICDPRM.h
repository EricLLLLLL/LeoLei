////////////////////////////////////////////////////////////////////////////////
// Header  = IC DATA TASK
// Date    = 2005/08/30
// VRT	   = 01-00-00-00
////////////////////////////////////////////////////////////////////////////////
#ifndef UPICDprm_h
#define UPICDprm_h

// IC CARD DATA(INFO GET,SET ITEM ID)
enum eInsideTag {
	ICDPC_IC_DF_NAME								=		0x84,		// DF(Dedicated File)
	ICDPC_IC_SFI									=		0x88,		// SFI(Short File Identifier)
	ICDPC_IC_APPLICATION_LABEL						=		0x50,		// IC APPLICATION LABEL
                                                                        
	ICDPC_IC_API									=		0x87,		// API Processing cis-position
	ICDPC_IC_PDOL									=		0x9F38,		// PDOL(Processing Options Data Object List)
	ICDPC_IC_AID									=		0x4F,		// ADF(Application Identifier)name
	ICDPC_IC_AIP									=		0x82,		// AIP(Application Interchange Profile)
	ICDPC_IC_AFL									=		0x94,		// AFL(Application File Locator)
	ICDPC_IC_AC										=		0x9F26,		// AC(Application Cryptogram)
	ICDPC_IC_CID									=		0x9F27,		// AC INFO DATA
	ICDPC_IC_ATC									=		0x9F36,		// deal tally register
	ICDPC_IC_LAST_ONLINE_ATC_REGISTER				=		0x9F13,		// LAST ONLINE ATC REGISTER
                                                                        
	ICDPC_IC_SIGNED_DYNAMIC_APPLICATION_DATA		=		0x9F4B,		// SIGNED DYNAMIC APPLICATION DATA
                                                                        
	ICDPC_IC_ISSUER_APPLICATION_DATA				=		0x9F10,		// ISSUER APPLICATION DATA
	ICDPC_IC_CDOL1									=		0x8C,		// CDOL1
	ICDPC_IC_CDOL2									=		0x8D,		// CDOL2
	ICDPC_IC_CVM_LIST								=		0x8E,		// CVM LIST
                                                                        
	ICDPC_IC_CARDHOLDER_NAME						=		0x5F20,		// CARDHOLDER NAME
	ICDPC_IC_CARDHOLDER_ID_NO						=		0x9F61,		// CARDHOLDER ID NO				 
	ICDPC_IC_CARDHOLDER_ID_TYPE						=		0x9F62,		// CARDHOLDER ID TYPE			 
	ICDPC_IC_APPLICATION_VER_NO						=		0x9F08,		// APPLICATION VER NO			
	ICDPC_IC_APPLICATION_EXPIRATION_DATE			=		0x5F24,		// APPLICATION EXPIRATION DATE
	ICDPC_IC_APPLICATION_EFFECTIVE_DATE				=		0x5F25,		// APPLICATION EFFECTIVE DATE	
	ICDPC_IC_PAN									=		0x5A,		// PAN						
	ICDPC_IC_PAN_SEQUENCE_NO						=		0x5F34,		// PAN SEQUENCE NO			
                                                                        
	ICDPC_IC_AUC									=		0x9F07,		// affinor control
	ICDPC_IC_IAC_DENIAL								=		0x9F0E,		// IAC DENIAL	
	ICDPC_IC_IAC_ONLINE								=		0x9F0F,		// IAC ONLINE	
                                                                        
	ICDPC_IC_IAC_DEFAULT							=		0x9F0D,		// IAC DEFAULT				
	ICDPC_IC_SINGED_STATIC_APPLICATION_DATA			=		0x93,		// SINGED STATIC APPLICATION DATA
                                                                        
	ICDPC_IC_ISSUER_PUBLIC_KEY_CERTIFICATE			=		0x90,		// ISSUER PUBLIC KEY CERTIFICATE	
	ICDPC_IC_CA_PUBLIC_KEY_INDEX					=		0x8F,		// CA PUBLIC KEY INDEX			
                                                                        
	ICDPC_IC_ISSUER_PUBLIC_KEY_REMAINDER			=		0x92,		// ISSUER PUBLIC KEY REMAINDER
	ICDPC_IC_ISSUER_PUBLIC_KEY_EXPONENT				=		0x9F32,		// ISSUER PUBLIC KEY EXPONENT	
	ICDPC_IC_ICC_PUBLIC_KEY_CERTIFICATE				=		0x9F46,		// ICC PUBLIC KEY CERTIFICATE	
	ICDPC_IC_ICC_PUBLIC_KEY_EXPONENT				=		0x9F47,		// ICC PUBLIC KEY EXPONENT	
	ICDPC_IC_ICC_PUBLIC_KEY_REMAINDER				=		0x9F48,		// ICC PUBLIC KEY REMAINDER	
	ICDPC_IC_DDOL									=		0x9F49,		// DDOL						
	ICDPC_IC_HOLDER_PIN_TRY_COUNTER					=		0x9F17,		// HOLDER PIN TRY COUNTER		
	ICDPC_IC_PRODUCT_IDENTIFICATION					=		0x9F63,		// PRODUCT IDENTIFICATION		 
                                                                        
	ICDPC_IC_APPLICATION_CURRENCY_CODE				=		0x9F42,		// APPLICATION CURRENCY CODE			
	ICDPC_IC_CURRENCY_EXPONENT						=		0x9F44,		// CURRENCY EXPONENT					
	ICDPC_IC_CARDHOLDER_NAME_EXTENDED				=		0x9F0B,		// CARDHOLDER NAME EXTENDED			
	ICDPC_IC_DATA_AUTHENTICATION_CODE				=		0x9F45,		// DATA AUTHENTICATION CODE			
	ICDPC_IC_ISSUER_COUNTRY_CODE					=		0x5F28,		// ISSUER COUNTRY CODE				
	ICDPC_IC_ISSUER_URL								=		0x5F50,		// ISSUER URL							
	ICDPC_IC_IBAN									=		0x5F53,		// IBAN								
	ICDPC_IC_BIC									=		0x5F54,		// BIC								
	ICDPC_IC_STATIC_DATA_AUTHENTICATION_TAG_LIST	=		0x9F4A,		// STATIC DATA AUTHENTICATION TAG LIST
	ICDPC_IC_LANGUAGE_PREFERENCE					=		0x5F2D,		// LANGUAGE PREFERENCE				
	ICDPC_IC_FCI_ISSUER_DISCRETIONARY_DATA_EMV		=		0xBF0C,		// FCI ISSUER DISCRETIONARY DATA EMV	
	ICDPC_IC_RESPONSE_MESSAGE_TEMPLATE_FORMAT1		=		0x80,		// RESPONSE MESSAGE TEMPLATE FORMAT1	
	ICDPC_IC_RESPONSE_MESSAGE_TEMPLATE_FORMAT2		=		0x77,		// RESPONSE MESSAGE TEMPLATE FORMAT2	
	ICDPC_IC_TEMPLATE_70							=		0x70,		// TEMPLATE 70						
	ICDPC_IC_FCI_TEMPLATE							=		0x6F,		// FCI TEMPLATE						
	ICDPC_IC_FCI_PROPRIETARY_TEMPLATE				=		0xA5,		// FCI PROPRIETARY TEMPLATE			
	ICDPC_IC_APPLICATION_TEMPLATE					=		0x61,		// APPLICATION TEMPLATE				
	ICDPC_IC_DDF_NAME								=		0x9D,		// DDF NAME							
	ICDPC_IC_DISCRETIONARY_DATA						=		0x9F05,		// DISCRETIONARY DATA					
	ICDPC_IC_ISSUER_CODE_TABLE_INDEX				=		0x9F11,		// ISSUER CODE TABLE INDEX			
                                                                        
	ICDPC_IC_AP_PREFERRED_NAME						=		0x9F12,		// AP PREFERRED NAME					
	ICDPC_IC_LOWER_CONSECUTIVE_OFFLINE_LIMIT		=		0x9F14,		// LOWER CONSECUTIVE OFFLINE LIMIT	
	ICDPC_IC_UPPER_CONSECUTIVE_OFFLINE_LIMIT		=		0x9F23,		// UPPER CONSECUTIVE OFFLINE LIMIT	
	ICDPC_IC_TRACK1_DISCRETIONARY_DATA				=		0x9F1F,		// TRACK1 DISCRETIONARY DATA			
	ICDPC_IC_TRACK2_DISCRETIONARY_DATA				=		0x9F20,		// TRACK2 DISCRETIONARY DATA			
	ICDPC_IC_ICC_PIN_ENC_PUBLIC_KEY_CERTIFICATE		=		0x9F2D,		// ICC PIN ENC PUBLIC KEY CERTIFICATE	
	ICDPC_IC_ICC_PIN_ENC_PUBLIC_KEY_EXPONENT		=		0x9F2E,		// ICC PIN ENC PUBLIC KEY EXPONENT	
	ICDPC_IC_ICC_PIN_ENC_PUBLIC_KEY_REMAINDER		=		0x9F2F,		// ICC PIN ENC PUBLIC KEY REMAINDER	
	ICDPC_IC_APPLICATION_REFERENCE_CURRENCY			=		0x9F3B,		// APPLICATION REFERENCE CURRENCY		
	ICDPC_IC_IIN									=		0x42,		// IIN								
	ICDPC_IC_TRACK2_EQUIVALENT_DATA					=		0x57,		// TRACK2 EQUIVALENT DATA				
	ICDPC_IC_SERVICE_CODE							=		0x5F30,		// SERVICE CODE
                                                                        
	ICDPC_IC_ISSUER_COUNTRY_CODE_A2F				=		0x5F55,		// ISSUER COUNTRY CODE A2F				
	ICDPC_IC_ISSUER_COUNTRY_CODE_A3F				=		0x5F56,		// ISSUER COUNTRY CODE A3F				
	ICDPC_IC_DIRECTORY_DISCRETIONARY_TEMPLATE		=		0x73,		// DIRECTORY DISCRETIONARY TEMPLATE		
                                                                           
	ICDPC_IC_TDOL									=		0x97,		// TDOL									
	ICDPC_IC_APPLICATION_REFERENCE_CURRENCY_EXPONENT=		0x9F43,		// APPLICATION REFERENCE CURRENCY EXPONENT
	ICDPC_IC_DYNAMIC_NUMBER							=		0x9F4C,		// DYNAMIC NUMBER							
	ICDPC_IC_LOG_ENTRY								=		0x9F4D,		// LOG ENTRY
                                                                        
	ICDPC_IC_LOG_FORMAT								=		0x9F4F,		// LOG FORMAT					
	ICDPC_IC_BALANCE_DATA							=		0x9F79,		// BALANCE DATA				   
	ICDPC_IC_BALANCE_LIMIT							=		0x9F77,		// BALANCE LIMIT				   
	ICDPC_IC_ISSUER_AUTHORIZATION_CODE				=		0x9F74,		// ISSUER AUTHORIZATION CODE	   
	ICDPC_IC_SINGLE_TRANSACTION_LIMIT				=		0x9F78,		// SINGLE TRANSACTION LIMIT	   
	ICDPC_IC_RESET_THRESHOLD						=		0x9F6D,		// RESET THRESHOLD			   
	ICDPC_IC_ISSUER_AUTHENTICATION_FLAG				=		0x9F56,		// ISSUER AUTHENTICATION FLAG	   
                                                                        
	ICDPC_TM_COMMAND_TEMPLATE						=		0x83,		// COMMAND TEMPLATE						
	ICDPC_TM_ACQUIRER_ID							=		0x9F01,		// ACQUIRER ID							
	ICDPC_TM_ADDITIONAL_TERMINAL_CAPABILITIES		=		0x9F40,		// ADDITIONAL TERMINAL CAPABILITIES		
	ICDPC_TM_AMOUNT_AUTHORISED						=		0x9F02,		// AMOUNT AUTHORISED						
	ICDPC_TM_AMOUNT_OTHER							=		0x9F03,		// AMOUNT OTHER		
	ICDPC_TM_9F66									=		0x9F66,		// 9F66
	ICDPC_TM_9F7A									=		0x9F7A,		// 9F7A
	ICDPC_TM_CAPP_TRADE_INDICATOR					=		0xDF60,		// CAPP交易指示位 （CAPP 扩展应用）
	ICDPC_TM_AID									=		0x9F06,		// AID									
	ICDPC_TM_APPLICATION_VER_NO						=		0x9F09,		// APPLICATION VER NO						
	ICDPC_TM_ARC									=		0x8A,		// ARC									
	ICDPC_TM_CVM_RESULTS							=		0x9F34,		// CVM RESULTS							
	ICDPC_TM_CA_PUBLIC_KEY_INDEX					=		0x9F22,		// CA PUBLIC KEY INDEX					
	ICDPC_TM_MERCHANT_ID							=		0x9F16,		// MERCHANT ID							
	ICDPC_TM_MERCHANT_CATEGORY_CODE					=		0x9F15,		// MERCHANT CATEGORY CODE					
	ICDPC_TM_POS_ENTRY_MODE							=		0x9F39,		// POS ENTRY MODE							
	ICDPC_TM_TERMINAL_CAPABILITIES					=		0x9F33,		// TERMINAL CAPABILITIES					
	ICDPC_TM_TERMINAL_COUNTRY_CODE					=		0x9F1A,		// TERMINAL COUNTRY CODE					
	ICDPC_TM_TERMINAL_FLOOR_LIMIT					=		0x9F1B,		// TERMINAL FLOOR LIMIT					
	ICDPC_TM_TERMINAL_ID							=		0x9F1C,		// TERMINAL ID							
	ICDPC_TM_TERMINAL_TYPE							=		0x9F35,		// TERMINAL TYPE							
	ICDPC_TM_TVR									=		0x95,		// TVR									
	ICDPC_TM_TRANSACTION_CURRENCY_CODE				=		0x5F2A,		// TRANSACTION CURRENCY CODE				
	ICDPC_TM_TRANSACTION_DATE						=		0x9A,		// TRANSACTION DATE						
	ICDPC_TM_TRANSACTION_TIME						=		0x9F21,		// TRANSACTION TIME						
	ICDPC_TM_TRANSACTION_PIN_DATA					=		0x99,		// TRANSACTION PIN DATA					
	ICDPC_TM_TRANSACTION_SEQUENCE_COUNTER			=		0x9F41,		// TRANSACTION SEQUENCE COUNTER			
	ICDPC_TM_TSI									=		0x9B,		// TSI									
	ICDPC_TM_TRANSACTION_TYPE						=		0x9C,		// TRANSACTION TYPE						
	ICDPC_TM_UNPREDICTABLE_NUMBER					=		0x9F37,		// UNPREDICTABLE NUMBER					
	ICDPC_TM_TRANSACTION_CURRENCY_EXPONENT			=		0x5F36,		// TRANSACTION CURRENCY EXPONENT			
	ICDPC_TM_AMOUNT_AUTHORISED_BINARY				=		0x81,		// AMOUNT AUTHORISED BINARY				
	ICDPC_TM_TC_HASH_VALUE							=		0x98,		// TC HASH VALUE							
	ICDPC_TM_AMOUNT_OTHER_BINARY					=		0x9F04,		// AMOUNT OTHER BINARY					
	ICDPC_TM_TERMINAL_RISK_MANAGEMENT_DATA			=		0x9F1D,		// TERMINAL RISK MANAGEMENT DATA			
	ICDPC_TM_IFD_SERIAL_NO							=		0x9F1E,		// IFD SERIAL NO							
	ICDPC_TM_AMOUNT_REFERENCE_CURRENCY				=		0x9F3A,		// AMOUNT REFERENCE CURRENCY				
	ICDPC_TM_TRANSACTION_REFERENCE_CURRENCY_CODE	=		0x9F3C,		// TRANSACTION REFERENCE CURRENCY CODE	
	ICDPC_TM_TRANSACTION_REFERENCE_CURRENCY_EXPONENT=		0x9F3D,		// TRANSACTION REFERENCE CURRENCY EXPONENT
	ICDPC_TM_MERCHANT_NAME_AND_LOCATION				=		0x9F4E,		// MERCHANT NAME AND LOCATION				
	ICDPC_ISSUER_AUTHENTICATION_DATA				=		0x91,		// ISSUER AUTHENTICATION DATA				
	ICDPC_ISSUER_SCRIPT_TEMPLATE1					=		0x71,		// ISSUER SCRIPT TEMPLATE1					
	ICDPC_ISSUER_SCRIPT_TEMPLATE2					=		0x72,		// ISSUER SCRIPT TEMPLATE2					
	ICDPC_ISSUER_SCRIPT_COMMAND						=		0x86,		// ISSUER SCRIPT COMMAND						
	ICDPC_ISSUER_SCRIPIT_ID							=		0x9F18,		// ISSUER SCRIPIT ID							
	ICDPC_ISSUER_AUTHORISATION_CODE					=		0x89,		// ISSUER AUTHORISATION CODE					
	ICDPC_TM_ISSUER_SCRIPT_RESULT					=		0xDF31,		// TM ISSUER SCRIPT RESULT
	ICDPC_TM_SM_ALGORITHM 							=		0xDF69		//TM SM ALGORITHM		 
};                                                                      
                                                                        
enum eOutsideTag {                                                      
	// IC card                                                        
	ICDPC_IC_HOLDER_OLD_PIN,											//HOLDER OLD PIN,	
	ICDPC_IC_HOLDER_FIX_PIN,											//HOLDER FIX PIN,	
	ICDPC_IC_HOLDER_NEW_PIN,											//HOLDER NEW PIN,	
	ICDPC_IC_HOLDER_PIN_TRY_MAX,										//HOLDER PIN TRY MAX,
	ICDPC_IC_UNPREDICTABLE_NUMBER,									    //UNPREDICTABLE NUMBER,
                                                                        
	// Terminal data                                                       
	ICDPC_TM_CA_PUBLIC_KEY_EXPONENT,									//CA PUBLIC KEY EXPONENT,		
	ICDPC_TM_CA_PUBLIC_KEY_MODULUS,										//CA PUBLIC KEY MODULUS,			
	ICDPC_TM_DEFAUT_DDA_OBJECT_LIST,									//DEFAUT DDA OBJECT LIST,		
	ICDPC_TM_TAC_DENIAL,												//TAC DENIAL,					
                                                                          
	ICDPC_TM_TAC_ONLINE,												//TAC ONLINE,					
	ICDPC_TM_TAC_DEFAULT,												//TAC DEFAULT,					
                                                                          
	ICDPC_TM_PSE,														//PSE,							
	ICDPC_TM_TRANSACTION_PIN_NEW_DATA,									//TRANSACTION PIN NEW DATA,		
	ICDPC_TM_APPLICATION_SELECTION_INDICATOR,							//APPLICATION SELECTION INDICATOR,
	// Terminal data  （Electronic wallet / electronic communications）                                
	ICDPC_TM_KEY_INDEX,													// KEY INDEX,				
                                                                           
	ICDPC_TM_ATM_NO,													// ATM NO,				
	ICDPC_TM_ISSUER_ID,													// ISSUER ID,				
	ICDPC_TM_PME,														// PME,					
	ICDPC_TM_USE_ALGORITHM,												// USE ALGORITHM,				 
	//individual data                                                 
	ICDPC_PRIORITY_LIST_DF_NO,											// PRIORITY LIST DF NO
	ICDPC_AID_LIST,														// AID LIST
	ICDPC_PDOL_RELATION_DATA,											// PDOL RELATION DATA
	ICDPC_SDOL_RELATION_DATA,											// SDOL RELATION DATA
	ICDPC_CDOL1_RELATION_DATA,											// CDOL1 RELATION DATA
	ICDPC_CDOL2_RELATION_DATA,											// CDOL2 RELATION DATA
	ICDPC_DDOL_RELATION_DATA,											// DDOL RELATION DATA
	ICDPC_TDOL_RELATION_DATA,										    // TDOL RELATION DATA	 
	ICDPC_COMMAND_HEADDER,												// COMMAND HEADDER
	ICDPC_STATUS,														// STATUS
	ICDPC_DYNAMIC_DATA_AUTHENTICATION,									// DYNAMIC DATA AUTHENTICATION
	ICDPC_TERMINAL_AUTHENTICATION,										// TERMINAL AUTHENTICATION
	ICDPC_UPDATE_BINALY,												// DATA BUFFER
	ICDPC_INFO_MANAGEMENT_TBL,											// INFO MANAGEMENT TBLLE//ICB52332
	ICDPC_DEFAULT_TDOL,												    // DEFAULT TDOL	 
    ICDPC_APPLICATION_LIST,												 										    
    ICDPC_ICPARAMETER,                                                  // ICParameter           
    ICDPC_ICTEMINALACTIONCODE,                                          // ICTerminalActionCode  
    ICDPC_HOSTISSUERDATA,                                               // HostIssuerData        
    ICDPC_ICPARAMETER_FILEPATH,                                         // ICParameterFilePath           
    ICDPC_ICTEMINALACTIONCODE_FILEPATH,                                 // ICTerminalActionCodeFilePath  
	ICDPC_OFFLINEAUTHSUPPORT,											// OFFLINE AUTHSUPPORT					 
	ICDPC_SDA_VERIFYDATA,												// OFFLINE VERIFYDATA			 

};

// command list
#define	ICDPC_GENERATE_AC											1
#define	ICDPC_GET_CHALLENGE											2                       
#define	ICDPC_GET_DATA												3
#define	ICDPC_GET_PROCESSING_OPTIONS								4
#define	ICDPC_INTERNAL_AUTHENTICATE									5                      
#define	ICDPC_READ_RECORD											6
#define ICDPC_HOST_SCRIPT											7	                     
#define	ICDPC_SELECT_PSE											8						 
#define	ICDPC_SELECT_DDF											9						 
#define	ICDPC_SELECT_ADF											10						 
#define	ICDPC_COMMAND_MAX											11	// COMMAND MAX+1 	   

// application software choose ID
#define PARTIAL														0x31

#define NON															2

// ENDE CODE
#define FORMAT_ERR													11
#define TAG_DOUBLE_SET												12
#define SFI_ERR														13
#define AFL_ERR														14
#define DUPLICATE													15
#define FULLMATCH													16
#define PARTIALMATCH												17
#define NOMATCH														18
#define BLACKLIST_CHECK_ERR											19			 
#define MANDATORY_CHECK_ERR											20			 
#define OFFLINE_AUTH_ERR											21			 


// COMMAND MESSAGE FORMAT
#define COMMAND_MESSAGE_FORMAT_CLA_INS_P1_P2						0
#define COMMAND_MESSAGE_FORMAT_CLA_INS_P1_P2_LE						1


// check need item
#define ICDPC_ALWAYS												1
#define ICDPC_OFFLINESTATIC											2
#define ICDPC_OFFLINEDYNAMIC										3

#define ICDPC_SDA													0x0001
#define ICDPC_DDA													0x0002
#define ICDPC_CDA													0x0004

// CheckSumType
#define ICDPC_ORIGINAL												1				 
#define ICDPC_EPP													2				 
#define ICDPC_ICTERM												3				 

#endif
