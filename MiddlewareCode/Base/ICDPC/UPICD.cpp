#include "UPICD.h"
#include "Upicddef.h"
#include "ICSTR.h"
#include "UPICDPRM.h"
#include "UPICDTBL.h"
#include "Helper/helper.h"

BYTE byTaskVRTU[] = {"UPICDGST60800000"};

ClearCommandList stClearCommandList[ICDPC_COMMAND_MAX];    // 存入的是当前解析过的报文的标签

BOOL TDOLFlg;
BOOL ICTDOLFlg;

static char szSectionName[] = "AIDLISTXX";
static char szPANList[] = "PANXXXXX";
static char szPANSeqNumList[] = "PANSequenceNumberXX";

static char szUPICDConfigFile[MAX_PATH];

TerminalAIDList stTerminalAIDList;
BlackList stBlackList;

static BYTE byDataArea[INFOMANAGEMENTSIZE];	// data buffer
static WORD wDataCheckMode = 0;
static InfoManagementTBL stInfoManagementTBL[sizeof(wInfoType) / sizeof(wInfoType[0])];    // 此数据存入的是所有报文解析后的标签和数据
static SettingInfo stSettingInfo;	//User defined file  value (publisher ID, bond index, PME, MerchantID)

static QString strSDAVerifyData;

static WORD TM_Terminal_Mode = 1;   
static WORD wSmAlgMode = 0;
static WORD wCipherMode = 0;
static void* hChinaCipherModule = NULL;
static void* hNationalCipherModule = NULL;

static WORD wReadRecordTemplate70Mode = MANDATORY;

static CandidateList stCandidateList;

static BOOL bGlobalTACMode = TRUE; 


UPICD::UPICD(void)
{
}


UPICD::~UPICD(void)
{
}


////////////////////////////////////////////////////////////////////////////////
// FuncClass= External Function
// FuncName	= Initialize
// CallType	= Function
// Detail	= Version set
//			  The terminal user definition file (ICTERM. INI) read 
//			  blacklist file (IC _ BLACKLIST. CSV) read
//			  information management table Initialization
// Valuable	= LPWORD:lpwReturn	:output
//				OK
// Return	= OK
////////////////////////////////////////////////////////////////////////////////
long UPICD::F_Init(CHAR* configPath, LPWORD lpwReturn )
{
	WORD	wDataSize = 0;
	WORD	wReturn = OK;
	LPBYTE	lpbyDataArea = byDataArea;
	LPBYTE	lpbyEndDataArea = byDataArea + sizeof(byDataArea);
	WORD	*lpwInfoBuffer[IC_ALL_MEMBER];

	*lpwReturn = OK;

	memcpy(m_configPath, configPath, strlen(configPath));

	/*
	// Set version
	wDataSize = sizeof(byTaskVRTU);
	tlsKSHOS.F_VersionSet( byTaskVRTU, wDataSize, &wReturn );
	*/

	//UPICD.INI file read
	N_UpicdConfigFileRead(lpwReturn);                                            
	if( *lpwReturn != OK){                                                       
		//N_Alarm( BUFFER_ERR, APMSG_ICDPC_INIT );                                 
		*lpwReturn = NG;                                                         
		return OK;                                                               
	} 

	// InfoTypeTBL data storage TBL Beagle item name and size  read
	// Information management table size get
	WORD wCnt_MAX = sizeof(wInfoType) / sizeof(wInfoType[0]);

	// 初始化stInfoManagementTBL数组
	for( WORD wCnt = 0; wCnt < wCnt_MAX; wCnt++ ){
		N_InfoTypeTblGetMember( wInfoType[wCnt][IC_ITEM], IC_ALL_MEMBER, lpwInfoBuffer, lpwReturn );
		if( *lpwReturn != OK ){
			//N_Alarm( LOGIC_ERR, APMSG_ICDPC_INIT );
			return OK;
		}
		// Name of TBL item with the data signal, the initial value and the size  set
		stInfoManagementTBL[wCnt].wItem = *(lpwInfoBuffer[IC_ITEM]);
		stInfoManagementTBL[wCnt].lpbyBuffer = lpbyDataArea;
		stInfoManagementTBL[wCnt].wBuffSize = *(lpwInfoBuffer[IC_BUFFSIZE]);
		stInfoManagementTBL[wCnt].wMiniSize = *(lpwInfoBuffer[IC_MINISIZE]);
		stInfoManagementTBL[wCnt].wDataSize = 0;

		if( lpbyDataArea + *(lpwInfoBuffer[IC_BUFFSIZE]) > lpbyEndDataArea ){
			//N_Alarm( BUFFER_ERR, APMSG_ICDPC_INIT );
			*lpwReturn = NG;
			return OK;
		}
		lpbyDataArea += *(lpwInfoBuffer[IC_BUFFSIZE]);
	}
	
	// User defined file read
	N_ICTerminalFileRead( &wReturn );

	// Read ICParameter file
	//N_ICTerminalActionCodeFileRead( &wReturn );                                  

	//Read ICParameter file
	N_ICParameterFileRead( &wReturn );                                           

	// Black list file read
	N_BlackListFileRead ( &wReturn );

	// 读取配置，数据存入stInfoManagementTBL数组
	N_TerminalDataRead(&wReturn);

	//F_Clear( lpwReturn );
	return OK;
}


////////////////////////////////////////////////////////////////////////////////
// FuncClass= Internal Function
// FuncName	= N_UpicdConfigFileRead
// CallType	= Function
////////////////////////////////////////////////////////////////////////////////
long UPICD::N_UpicdConfigFileRead( LPWORD lpwReturn )
{
	*lpwReturn = OK;

    if(g_Helper.GetPrivateProfileInt("ReadRecordConfig", "Template70Mode", 1, m_configPath) != 0)
	{
		wReadRecordTemplate70Mode = MANDATORY;
	}
	else
	{
		wReadRecordTemplate70Mode = OPTION;
	}
    TM_Terminal_Mode = g_Helper.GetPrivateProfileInt("TM_TERMINAL_MODE", "TM_TERMINAL_MODE", 1, m_configPath);
	//wDataCheckMode = GetPrivateProfileInt("DataCheck", "DataCheckMode", 1, szUPICDConfigFile);
	//GetPrivateProfileString("EPPCheckSum", "EPPCheckSum", "ZT598", cEPPCheckSum, sizeof(cEPPCheckSum), szUPICDConfigFile);
	return OK;
}


// 读取终端需配置的数据
long UPICD::N_TerminalDataRead(LPWORD lpwReturn)
{ 
	char chTMData[TAGDATASIZE] = {0};	// 从配置中读到的数据
	BYTE byTMData[TAGDATASIZE] = {0x00};  // 配置中的数据转换成的十六进制形式

    // 终端国家代码
    if (g_Helper.GetPrivateProfileString("Template", "TemCountry", "0156", chTMData, sizeof(chTMData), m_configPath))
	{
		N_ASCtoHEX(byTMData, (LPBYTE)chTMData, strlen(chTMData), lpwReturn);
		if (OK != *lpwReturn)
		{
			return OK;
		}

		F_InfoSet(ICDPC_TM_TERMINAL_COUNTRY_CODE, byTMData, strlen(chTMData)/2, lpwReturn);
		if (OK != *lpwReturn)
		{
			return OK;
		}
	}

	memset(chTMData, 0, sizeof(chTMData));
	memset(byTMData, 0x00, sizeof(byTMData));

	//// 终端验证结果TVR
    //if (g_Helper.GetPrivateProfileString("Template", "TemTVR", "0000000800", chTMData, sizeof(chTMData), cFileName))
	//{
	//	N_ASCtoHEX(byTMData, (LPBYTE)chTMData, strlen(chTMData), lpwReturn);
	//	if (OK != *lpwReturn)
	//	{
	//		return OK;
	//	}

	//	F_InfoSet(ICDPC_TM_TVR, byTMData, strlen(chTMData)/2, lpwReturn);
	//	if (OK != *lpwReturn)
	//	{
	//		return OK;
	//	}
	//}

	//memset(chTMData, 0, sizeof(chTMData));
	//memset(byTMData, 0x00, sizeof(byTMData));

    // 商户名称
    if (g_Helper.GetPrivateProfileString("Template", "TemName", "00", chTMData, sizeof(chTMData), m_configPath))
	{
		N_ASCtoHEX(byTMData, (LPBYTE)chTMData, strlen(chTMData), lpwReturn);
		if (OK != *lpwReturn)
		{
			return OK;
		}

		F_InfoSet(ICDPC_TM_MERCHANT_NAME_AND_LOCATION, byTMData, strlen(chTMData)/2, lpwReturn);
		if (OK != *lpwReturn)
		{
			return OK;
		}
	}

	memset(chTMData, 0, sizeof(chTMData));
	memset(byTMData, 0x00, sizeof(byTMData));

    // 不可预知数
    if (g_Helper.GetPrivateProfileString("Template", "TemUnknowNum", "00000000", chTMData, sizeof(chTMData), m_configPath))
	{
		N_ASCtoHEX(byTMData, (LPBYTE)chTMData, strlen(chTMData), lpwReturn);
		if (OK != *lpwReturn)
		{
			return OK;
		}

		F_InfoSet(ICDPC_TM_UNPREDICTABLE_NUMBER, byTMData, strlen(chTMData)/2, lpwReturn);
		if (OK != *lpwReturn)
		{
			return OK;
		}
	}

	memset(chTMData, 0, sizeof(chTMData));
	memset(byTMData, 0x00, sizeof(byTMData));

    // 终端类型
    if (g_Helper.GetPrivateProfileString("Template", "TemType", "00", chTMData, sizeof(chTMData), m_configPath))
	{
		N_ASCtoHEX(byTMData, (LPBYTE)chTMData, strlen(chTMData), lpwReturn);
		if (OK != *lpwReturn)
		{
			return OK;
		}

		F_InfoSet(ICDPC_TM_TERMINAL_TYPE, byTMData, strlen(chTMData)/2, lpwReturn);
		if (OK != *lpwReturn)
		{
			return OK;
		}
	}

	memset(chTMData, 0, sizeof(chTMData));
	memset(byTMData, 0x00, sizeof(byTMData));

    // 终端性能
    if (g_Helper.GetPrivateProfileString("Template", "TemCapability", "000000", chTMData, sizeof(chTMData), m_configPath))
	{
		N_ASCtoHEX(byTMData, (LPBYTE)chTMData, strlen(chTMData), lpwReturn);
		if (OK != *lpwReturn)
		{
			return OK;
		}

		F_InfoSet(ICDPC_TM_TERMINAL_CAPABILITIES, byTMData, strlen(chTMData)/2, lpwReturn);
		if (OK != *lpwReturn)
		{
			return OK;
		}
	}

	memset(chTMData, 0, sizeof(chTMData));
	memset(byTMData, 0x00, sizeof(byTMData));

    // 附件终端性能
    if (g_Helper.GetPrivateProfileString("Template", "TemAdditionalCapability", "0000000000", chTMData, sizeof(chTMData), m_configPath))
	{
		N_ASCtoHEX(byTMData, (LPBYTE)chTMData, strlen(chTMData), lpwReturn);
		if (OK != *lpwReturn)
		{
			return OK;
		}

		F_InfoSet(ICDPC_TM_ADDITIONAL_TERMINAL_CAPABILITIES, byTMData, strlen(chTMData)/2, lpwReturn);
		if (OK != *lpwReturn)
		{
			return OK;
		}
	}

	memset(chTMData, 0, sizeof(chTMData));
	memset(byTMData, 0x00, sizeof(byTMData));

	return OK;
}


////////////////////////////////////////////////////////////////////////////////
// FuncClass= Internal Function
// FuncName = N_ICTerminalActionCodeFileRead
// CallType = Function
// Detail   = Read ICTerminal ActionCode file
// Valuable = LPWORD:lpwReturn  :output
//              OK
//              NG
// Return   = OK
// Specially=
////////////////////////////////////////////////////////////////////////////////
//long UPICD::N_ICTerminalActionCodeFileRead( LPWORD lpwReturn )
//{
//	char	szTAC[11];			// TAC
//	LPCSTR  szDefault = ""  ;   // Default alphabetic string
//	char    cFileName[MAX_PATH];// file name 
//	WORD	wBcdSize;
//
//	*lpwReturn = OK;
//
//	// file name (full path) get 
//	strcpy(cFileName, strConfigHome);
//	strcat(cFileName, szTACFile);
//
//	//check user define file exist or not
//	HFILE hFile = (HFILE)CreateFile( cFileName, GENERIC_READ,
//		FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL,NULL );
//	if( hFile == -1 ){
//		return OK;
//	}
//	CloseHandle( (HANDLE)hFile );
//
//	//TACDenial
//	if( GetPrivateProfileString( "TAC", "TACDenial", szDefault,									
//		szTAC, sizeof(szTAC), cFileName ) != 0 ){
//			N_JIStoBCD( (LPBYTE)szTAC, strlen(szTAC),								
//				(LPBYTE)&stTAC_PROGRESS_DENIAL, strlen(szTAC) / 2, &wBcdSize, lpwReturn );
//			if( *lpwReturn != OK ){
//				return OK;
//			}
//	}
//
//	//TACOnline
//	if( GetPrivateProfileString( "TAC", "TACOnline", szDefault,									
//		szTAC, sizeof(szTAC), cFileName ) != 0 ){
//			N_JIStoBCD( (LPBYTE)szTAC, strlen(szTAC),								
//				(LPBYTE)&stTAC_PROGRESS_ONLINE, strlen(szTAC) / 2, &wBcdSize, lpwReturn );
//			if( *lpwReturn != OK ){
//				return OK;
//			}
//	}
//
//	//TACDefault
//	if( GetPrivateProfileString( "TAC", "TACDefault", szDefault,									
//		szTAC, sizeof(szTAC), cFileName ) != 0 ){
//			N_JIStoBCD( (LPBYTE)szTAC, strlen(szTAC),								
//				(LPBYTE)&stTAC_PROGRESS_DEFAULT, strlen(szTAC) / 2, &wBcdSize, lpwReturn );
//			if( *lpwReturn != OK ){
//				return OK;
//			}
//	}
//
//	return OK;
//}

////////////////////////////////////////////////////////////////////////////////
// FuncClass= Internal Function
// FuncName	= ICTerminalFileRead
// CallType	= Function
// Detail	= get and save data to user define file
// Valuable	= LPWORD:lpwReturn	:output
//				OK
//				NG
// Return	= OK
////////////////////////////////////////////////////////////////////////////////
long UPICD::N_ICTerminalFileRead( LPWORD lpwReturn )
{
	char	szIssuerID[17];		// IssuerID		
	char	szKeyIndex[3];		// Key Index	
	char	szPME[21];			// PME			
	char	szIFD[9];			// IFD			
	char	szTAC[11];			// TAC			
	char	szMerID[16];		// memberID		
	char	szCtyCode[5];		// country  code 			
	char	szMerCCode[5];		// member distinguish code 	
	char	szPOSEMode[3];		// POS input  mode 	
	char	szTDOL[505];		// TDOL				
	char	szAcquirerId[13];									
	LPCSTR	szDefault = "";	    // Default alphabetic string                 
	WORD	wBcdSize;

	char chTMData[TAGDATASIZE] = {0};	// 从配置中读到的数据
	BYTE byTMData[TAGDATASIZE] = {0x00};  // 配置中的数据转换成的十六进制形式

	*lpwReturn = OK;

	memset( &szIssuerID[0], NULL, sizeof(szIssuerID) );
	memset( &szKeyIndex[0], NULL, sizeof(szKeyIndex) );
	memset( &szPME[0], NULL, sizeof(szPME) );			
	memset( &szIFD[0], NULL, sizeof(szIFD) );						
	memset( &szMerID[0], NULL, sizeof(szMerID) );		
	memset( &szCtyCode[0], NULL, sizeof(szCtyCode) );	
	memset( &szMerCCode[0], NULL, sizeof(szMerCCode) );
	memset( &szPOSEMode[0], NULL, sizeof(szPOSEMode) );
	memset( &szTDOL[0], NULL, sizeof(szTDOL) );			
	memset( &szAcquirerId[0], NULL, sizeof(szAcquirerId) );						

	stSettingInfo.bIssuerIDFlg = false;
	stSettingInfo.bKeyIndexFlg = false;	
	stSettingInfo.bPMEFlg	   = false;		
	stSettingInfo.bIFDFlg	   = false;		
	stSettingInfo.bMerIDFlg    = false;		
	stSettingInfo.bCtyCodeFlg  = false;		
	stSettingInfo.bMerCCodeFlg = false;	
	stSettingInfo.bPOSEModeFlg = false;
	stSettingInfo.bAcquirerIdFlg = false;

    // IssuerID
    if( g_Helper.GetPrivateProfileString( "ISSUER_ID", "IssuerID", szDefault,
        szIssuerID, sizeof(szIssuerID), m_configPath ) ){
			N_JIStoBCD( (LPBYTE)szIssuerID, strlen(szIssuerID),								  
				(LPBYTE)stSettingInfo.szIssuerID, strlen(szIssuerID) / 2, &wBcdSize, lpwReturn );	 
			if( *lpwReturn != OK ){															 
				return OK;																	 
			}																				 
			stSettingInfo.bIssuerIDFlg = true;												 
	}																					 

    // KeyIndex
    if( g_Helper.GetPrivateProfileString( "KEY_INDEX", "KeyIndex", szDefault,
        szKeyIndex, sizeof(szKeyIndex), m_configPath ) ){
			N_JIStoBCD( (LPBYTE)szKeyIndex, strlen(szKeyIndex),								  
				(LPBYTE)stSettingInfo.szKeyIndex, strlen(szKeyIndex) / 2, &wBcdSize, lpwReturn );	 
			if( *lpwReturn != OK ){															 
				return OK;																	 
			}																				 
			stSettingInfo.bKeyIndexFlg = true;												 
	}																					 

    // PME
    if( g_Helper.GetPrivateProfileString( "PME", "PME", szDefault,
        szPME, sizeof(szPME), m_configPath ) ){
			N_JIStoBCD( (LPBYTE)szPME, strlen(szPME),										  
				(LPBYTE)stSettingInfo.szPME, strlen(szPME) / 2, &wBcdSize, lpwReturn ); 
			if( *lpwReturn != OK ){															 
				return OK;																	 
			}																				 
			stSettingInfo.bPMEFlg = true;													 
	}																					 

    // IFD
    if( g_Helper.GetPrivateProfileString( "IFD", "IFD", szDefault,
        szIFD, sizeof(szIFD), m_configPath )){
			memcpy( (LPBYTE)stSettingInfo.szIFD, (LPBYTE)szIFD, sizeof(szIFD) );			   
			stSettingInfo.bIFDFlg	   = true;												 
	}


    // TAC
    if( g_Helper.GetPrivateProfileString( "TAC", "TACDenial", szDefault,	chTMData, sizeof(chTMData), m_configPath ) != 0 )
    {
		N_JIStoBCD( (LPBYTE)chTMData, strlen(chTMData),								
			(LPBYTE)&stTAC_PROGRESS_DENIAL, strlen(chTMData) / 2, &wBcdSize, lpwReturn );
		if( *lpwReturn != OK )
		{
			return OK;
		}

		N_ASCtoHEX(byTMData, (LPBYTE)chTMData, strlen(chTMData), lpwReturn);
		if (OK != *lpwReturn)
		{
			return OK;
		}

		F_InfoSet(ICDPC_TM_TAC_DENIAL, byTMData, strlen(chTMData)/2, lpwReturn);
		if (OK != *lpwReturn)
		{
			return OK;
		}																					 
	}

	memset(chTMData, 0, sizeof(chTMData));
	memset(byTMData, 0x00, sizeof(byTMData));

    if( g_Helper.GetPrivateProfileString( "TAC", "TACOnline", szDefault,	chTMData, sizeof(chTMData), m_configPath) != 0 )
    {
		N_JIStoBCD( (LPBYTE)chTMData, strlen(chTMData),								
			(LPBYTE)&stTAC_PROGRESS_ONLINE, strlen(chTMData) / 2, &wBcdSize, lpwReturn );
		if( *lpwReturn != OK )
		{
			return OK;
		}

		N_ASCtoHEX(byTMData, (LPBYTE)chTMData, strlen(chTMData), lpwReturn);
		if (OK != *lpwReturn)
		{
			return OK;
		}

		F_InfoSet(ICDPC_TM_TAC_ONLINE, byTMData, strlen(chTMData)/2, lpwReturn);
		if (OK != *lpwReturn)
		{
			return OK;
		}																					 
	}

	memset(chTMData, 0, sizeof(chTMData));
	memset(byTMData, 0x00, sizeof(byTMData));

    if( g_Helper.GetPrivateProfileString( "TAC", "TACDefault", szDefault, chTMData, sizeof(chTMData), m_configPath ) != 0 )
    {
		N_JIStoBCD( (LPBYTE)chTMData, strlen(chTMData),								
			(LPBYTE)&stTAC_PROGRESS_DEFAULT, strlen(chTMData) / 2, &wBcdSize, lpwReturn );
		if( *lpwReturn != OK )
		{
			return OK;
		}

		N_ASCtoHEX(byTMData, (LPBYTE)chTMData, strlen(chTMData), lpwReturn);
		if (OK != *lpwReturn)
		{
			return OK;
		}

		F_InfoSet(ICDPC_TM_TAC_DEFAULT, byTMData, strlen(chTMData)/2, lpwReturn);
		if (OK != *lpwReturn)
		{
			return OK;
		}																					 
	}

	memset(chTMData, 0, sizeof(chTMData));
	memset(byTMData, 0x00, sizeof(byTMData));

    // MerchantID	ICB52314
    if( g_Helper.GetPrivateProfileString( "MERCHANT_ID", "MerchantID", szDefault,
        szMerID, sizeof(szMerID), m_configPath ) ){
			memcpy( (LPBYTE)stSettingInfo.szMerID, (LPBYTE)szMerID, sizeof(szMerID) );	   
			stSettingInfo.bMerIDFlg = true;												 
	}																				 

    // CountryCode	ICB52333
    if( g_Helper.GetPrivateProfileString( "COUNTRY_CODE", "CountryCode", szDefault,
        szCtyCode, sizeof(szCtyCode), m_configPath ) ){
			N_JIStoBCD( (LPBYTE)szCtyCode, strlen(szCtyCode),											    
				(LPBYTE)stSettingInfo.szCtyCode, strlen(szCtyCode) / 2, &wBcdSize, lpwReturn );	   
			if( *lpwReturn != OK ){																		   
				return OK;																				   
			}																							   
			stSettingInfo.bCtyCodeFlg  = true;															 
	}																								 

    // Category Code	ICB52339
    if( g_Helper.GetPrivateProfileString( "MERCHANT_CATEGORY_CODE", "MerchantCategoryCode", szDefault,
        szMerCCode, sizeof(szMerCCode), m_configPath ) ){
			N_JIStoBCD( (LPBYTE)szMerCCode, strlen(szMerCCode),												    
				(LPBYTE)stSettingInfo.szMerCCode, strlen(szMerCCode) / 2, &wBcdSize, lpwReturn );	   
			if( *lpwReturn != OK ){																			   
				return OK;																					   
			}																								   
			stSettingInfo.bMerCCodeFlg = true;																 
	}																									 

    // POS Entry Mode	ICB52339
    if( g_Helper.GetPrivateProfileString( "POS_ENTRY_MODE", "POSEntryMode", szDefault,
        szPOSEMode, sizeof(szPOSEMode), m_configPath ) ){
			N_JIStoBCD( (LPBYTE)szPOSEMode, strlen(szPOSEMode),												    
				(LPBYTE)stSettingInfo.szPOSEMode, strlen(szPOSEMode) / 2, &wBcdSize, lpwReturn );	   
			if( *lpwReturn != OK ){																			   
				return OK;																					   
			}																								   
			stSettingInfo.bPOSEModeFlg = true;																 
	}																									 

	// TDOL	 
    g_Helper.GetPrivateProfileString( "TDOL", "TDOL", szDefault, szTDOL, sizeof(szTDOL), m_configPath ) ;
	N_ASCtoHEX( (LPBYTE)stSettingInfo.szTDOL, (LPBYTE)szTDOL, strlen(szTDOL), lpwReturn );		 
	if( *lpwReturn != OK ){																		 
		return OK;																				 
	}																							 

    if( g_Helper.GetPrivateProfileString( "ACQUIRERID", "ACQUIRERID", szDefault,
        szAcquirerId, sizeof(szAcquirerId), m_configPath ) ){
			N_JIStoBCD( (LPBYTE)szAcquirerId, strlen(szAcquirerId),					 
				(LPBYTE)stSettingInfo.szAcquirerId, strlen(szAcquirerId) / 2, &wBcdSize, lpwReturn );  
			if( *lpwReturn != OK ){													 
				return OK;
			}																		 
			stSettingInfo.bAcquirerIdFlg = true;									 
	}																			 
    wCipherMode = g_Helper.GetPrivateProfileInt("CIPHER", "CIPHERMODE", 0, m_configPath);
    wSmAlgMode = g_Helper.GetPrivateProfileInt("SM_ALGORITHM_MODE", "SMAlgorithmMode", 0, m_configPath);
	//N_LoadCipherDllAndModeSet(cFileName);								                         
	return OK;		
}

////////////////////////////////////////////////////////////////////////////////
// FuncClass= Internal Function
// FuncName = N_ICParameterFileRead
// CallType = Function
// Detail   = ICParameter File Read
// Valuable = LPWORD:lpwReturn  :output
//              OK
//              NG
// Return   = OK
// Specially=
////////////////////////////////////////////////////////////////////////////////
long UPICD::N_ICParameterFileRead( LPWORD lpwReturn )
{
	char    szNum[3];           // tagnum
	WORD    wNumber = 0;        // DF num 
	LPCSTR  szDefault = ""  ;   // Default alphabetic string
	WORD	wCnt=0;
	WORD	wBcdSize;

	struct FileRead{
		char szDFName[33];      // DF name  get  use buffer
		char szSelectID[3];     //  choose ID get  use buffer
		char szAPVersion[5];    // APversion use buffer
		char szTACDenial[11];   // TACDenial
		char szTACOnline[11];   // TACOnline
		char szTACDefault[11];  // TACDefault
	};

	static  FileRead stFileRead[65];

	*lpwReturn = OK;

	memset( &stFileRead[0], NULL, sizeof(stFileRead) );

	// get DF num  
    wNumber = g_Helper.GetPrivateProfileInt( "LIST_COUNT", "Count", 0, m_configPath );
	// set DF num 
	stTerminalAIDList.wNum = wNumber;

	// DF num 
	for( wCnt=0; wCnt < wNumber; wCnt++ ){
		sprintf( szNum, "%02d", wCnt+1 );
		memcpy( &szSectionName[7], &szNum[0], 2 );
		// DF name 
        g_Helper.GetPrivateProfileString( szSectionName, "DFName", szDefault,
			stFileRead[wCnt].szDFName, sizeof(stFileRead[wCnt].szDFName), m_configPath );
		stTerminalAIDList.stTerminalAID[wCnt].lpsAID = stFileRead[wCnt].szDFName;

		// application choose ID
		stTerminalAIDList.stTerminalAID[wCnt].byApplicationSelectionIndicator =
            g_Helper.GetPrivateProfileInt( szSectionName, "SelectID", 0, m_configPath );

		// application version num
        g_Helper.GetPrivateProfileString( szSectionName, "Version", szDefault,
			(LPSTR)stFileRead[wCnt].szAPVersion, sizeof(stFileRead[wCnt].szAPVersion), m_configPath );
		N_JIStoBCD( (LPBYTE)stFileRead[wCnt].szAPVersion, strlen(stFileRead[wCnt].szAPVersion),		 
			(LPBYTE)stTerminalAIDList.stTerminalAID[wCnt].byAPVersionIndicator,
			strlen(stFileRead[wCnt].szAPVersion) / 2, &wBcdSize, lpwReturn);
		if( *lpwReturn != OK ){
			return OK;
		}

		if(!bGlobalTACMode)
		{
			//TACDenial
            g_Helper.GetPrivateProfileString( szSectionName, "TACDenial", szDefault,
				(LPSTR)stFileRead[wCnt].szTACDenial, sizeof(stFileRead[wCnt].szTACDenial), m_configPath );
			N_JIStoBCD( (LPBYTE)stFileRead[wCnt].szTACDenial, strlen(stFileRead[wCnt].szTACDenial),	 
				(LPBYTE)stTerminalAIDList.stTerminalAID[wCnt].byTACDenial,
				strlen(stFileRead[wCnt].szTACDenial) / 2, &wBcdSize, lpwReturn);
			if( *lpwReturn != OK ){
				return OK;
			}

			//TACOnline
            g_Helper.GetPrivateProfileString( szSectionName, "TACOnline", szDefault,
				(LPSTR)stFileRead[wCnt].szTACOnline, sizeof(stFileRead[wCnt].szTACOnline), m_configPath );
			N_JIStoBCD( (LPBYTE)stFileRead[wCnt].szTACOnline, strlen(stFileRead[wCnt].szTACOnline),	 
				(LPBYTE)stTerminalAIDList.stTerminalAID[wCnt].byTACOnline,
				strlen(stFileRead[wCnt].szTACOnline) / 2, &wBcdSize, lpwReturn);
			if( *lpwReturn != OK ){
				return OK;
			}

			//TACDefault
            g_Helper.GetPrivateProfileString( szSectionName, "TACDefault", szDefault,
				(LPSTR)stFileRead[wCnt].szTACDefault, sizeof(stFileRead[wCnt].szTACDefault), m_configPath );
			N_JIStoBCD( (LPBYTE)stFileRead[wCnt].szTACDefault, strlen(stFileRead[wCnt].szTACDefault),	 
				(LPBYTE)stTerminalAIDList.stTerminalAID[wCnt].byTACDefault,
				strlen(stFileRead[wCnt].szTACDefault) / 2, &wBcdSize, lpwReturn);
			if( *lpwReturn != OK ){
				return OK;
			}
		}
	}

	return OK;
}

////////////////////////////////////////////////////////////////////////////////
// FuncClass= Internal Function
// FuncName	= BlackListFileRead
// CallType	= Function
// Detail	= Read black list file
// Valuable	= LPWORD:lpwReturn	:output
//				OK
//				NG
// Return	= OK
// Specially=
////////////////////////////////////////////////////////////////////////////////
long UPICD::N_BlackListFileRead( LPWORD lpwReturn )
{
	char	szNum1[6];					 
	char	szNum2[3];					 
	LPCSTR	szDefault = NULL;
	WORD	wBcdSize;                          

	*lpwReturn = OK;

	struct BlackListFileRead{
		char szPAN[20];					// PAN
		char szPANSeqNum[3];			// PAN Sequence Number
	};
	static	BlackListFileRead stBlackListFileRead;

	memset( &stBlackListFileRead, NULL, sizeof(stBlackListFileRead) );
	memset( &stBlackList, NULL, sizeof(stBlackList) );

	// Set Num of PAN and PAN Sequence Number
    WORD wPANNum = g_Helper.GetPrivateProfileInt( "LIST_COUNT", "PANCount", 0, m_configPath );
    WORD wPANSeqNum = g_Helper.GetPrivateProfileInt( "LIST_COUNT", "SequenceCount", 0, m_configPath );

	if (wPANNum > 9999 || wPANSeqNum > 99){						 
		*lpwReturn = NG;										 
		//N_Alarm( PARAM_ERR, APMSG_ICDPC_GETCOMMANDMESSAGE );	 
		return OK;												 
	}															 

	stBlackList.wPANNum = wPANNum;
	stBlackList.wPANSeqNum = wPANSeqNum;

	for( WORD wCnt=0; wCnt < wPANNum; wCnt++ ){
		// Set PAN
		sprintf( szNum1, "%05d", wCnt+1 );																				 
		memcpy( &szPANList[3], szNum1, 5 );																				 
        if( g_Helper.GetPrivateProfileString( "PAN", szPANList , szDefault, stBlackListFileRead.szPAN,
			sizeof(stBlackListFileRead.szPAN), m_configPath )){                                     
				N_JIStoBCD( (LPBYTE)stBlackListFileRead.szPAN, strlen(stBlackListFileRead.szPAN),	 
					(LPBYTE)stBlackList.byPAN[wCnt], strlen(stBlackListFileRead.szPAN) / 2, &wBcdSize, lpwReturn);
				stBlackList.wPANBCDSize[wCnt] = wBcdSize;
				if( *lpwReturn != OK ){
					return OK;
				}
		}
	}																													 

	for( WORD wCnt1=0; wCnt1 < wPANSeqNum; wCnt1++ ){
		// Set PAN Sequence Number
		sprintf( szNum2, "%02d", wCnt1+1 );																				 
		memcpy( &szPANSeqNumList[17], szNum2, 2 );																		 
        if( g_Helper.GetPrivateProfileString( "PANSEQUENCENUMBER", szPANSeqNumList, szDefault, stBlackListFileRead.szPANSeqNum,
			sizeof(stBlackListFileRead.szPANSeqNum), m_configPath ) ){                              
				N_JIStoBCD( (LPBYTE)stBlackListFileRead.szPANSeqNum, strlen(stBlackListFileRead.szPANSeqNum),	 
					(LPBYTE)&stBlackList.byPANSequenceNumber[wCnt1],
					strlen(stBlackListFileRead.szPANSeqNum) / 2, &wBcdSize, lpwReturn);
				if( *lpwReturn != OK ){
					return OK;
				}
		}
	}																													 
	return OK;
}

////////////////////////////////////////////////////////////////////////////////
// FuncClass= External Function
// FuncName	= ICResponseMessageDataStatusAnalysis
// CallType	= Function
// Detail	= Get status information from the Journal of IC card in the response data
// Valuable	= LPBYTE:lpbyData		:input
//			  WORD	:wDataSize 		:input
//			  LPWORD:lpwSw1Sw2Buffer:output
//			  LPWORD:lpwReturn		:output
//				OK
//				NG
// Return	= OK
////////////////////////////////////////////////////////////////////////////////
long UPICD::F_ICResponseMessageDataStatusAnalysis( LPBYTE lpbyData, WORD wDataSize,
	LPWORD lpwSw1Sw2Buffer, LPWORD lpwReturn )
{
	LPBYTE	lpbyData_WK;
	BYTE	byStatus[2];

	if( lpwReturn == NULL ){
		//N_Alarm( PARAM_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATASTATUSANALYSIS );
		return OK;
	}
	if( lpwSw1Sw2Buffer == NULL ){
		*lpwReturn = NG;
		//N_Alarm( PARAM_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATASTATUSANALYSIS );
		return OK;
	}

	*lpwSw1Sw2Buffer = 0;
	*lpwReturn = OK;

	if( wDataSize < 2 ){
		*lpwReturn = FORMAT_ERR;
		return OK;
	}

	lpbyData_WK = lpbyData + wDataSize - 2;

	byStatus[0] = *(lpbyData_WK + 1);
	byStatus[1] = *lpbyData_WK;

	memcpy( LPBYTE(lpwSw1Sw2Buffer), byStatus, sizeof(WORD) );

	F_InfoSet( ICDPC_STATUS, byStatus, sizeof(byStatus), lpwReturn );    // 把当前数据状态存入stInfoManagementTBL数组中
	return OK;
}


////////////////////////////////////////////////////////////////////////////////
// FuncClass= External Function
// FuncName	= ICResponseMessageDataAnalysis
// CallType	= Function
// Detail	= Analysis of IC card response data storage
// Valuable	= WORD	:wCommand	:input
//				ICDPC_SELECT_PSE
//				ICDPC_SELECT_DDF
//				ICDPC_SELECT_ADF
//				ICDPC_GET_PROCESSING_OPTIONS
//				ICDPC_GENERATE_AC
//				ICDPC_READ_RECORD
//				ICDPC_INTERNAL_AUTHENTICATE
//				ICDPC_GET_DATA
//				ICDPC_GET_CHALLENGE
//			  LPBYTE:lpbyData	:input
//			  WORD	:wDataSize	:input
//			  LPWORD:lpwReturn	:output
//				OK
//				NG
//				FORMAT_ERR
// Return	= OK
////////////////////////////////////////////////////////////////////////////////
long UPICD::F_ICResponseMessageDataAnalysis( WORD wCommand, LPBYTE lpbyData,
	WORD wDataSize, LPWORD lpwReturn )
{

	if( lpwReturn == NULL ){
		//N_Alarm( PARAM_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS );
		return OK;
	}

	if( wCommand >= ICDPC_COMMAND_MAX || wCommand == 0 ){
		*lpwReturn = NG;
		//N_Alarm( PARAM_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS );
		return OK;
	}

	*lpwReturn = OK;

	// 清除所有已存入的所有解析过的报文，包括stClearCommandList和stInfoManagementTBL数组中的所有内容；
	// 因为在读取应用数据时要多次调用READ RECORD命令，每次调用READ RECORD命令所返回的报文都要保存
	if ( wCommand != ICDPC_READ_RECORD ) {	
		for( WORD i = 0; i < stClearCommandList[wCommand].wCnt; i++ ){
			F_InfoItemClear( stClearCommandList[wCommand].wTag[i], lpwReturn );
			if( *lpwReturn != OK ){
				return OK;
			}
		}
	}
	
	N_ClearCommandListClear( wCommand );

	if( wDataSize < 2 ){
		*lpwReturn = FORMAT_ERR;
		return OK;
	}
	wDataSize -= 2;

	// 根据不同的命令选择报文解析的函数
	switch( wCommand ){
	case ICDPC_SELECT_PSE:		// 目录选择												
		N_SelectFileCommandAnalysis(lpbyData, wDataSize, wCommand, lpwReturn);	
		return OK;																
	case ICDPC_SELECT_DDF:													
		N_SelectFileCommandAnalysis(lpbyData, wDataSize, wCommand, lpwReturn);	
		return OK;																
	case ICDPC_SELECT_ADF:														
		N_SelectFileCommandAnalysis(lpbyData, wDataSize, wCommand, lpwReturn);	
		return OK;																
	case ICDPC_GET_PROCESSING_OPTIONS:    // GPO命令
		N_GetProcessingOptionsCommandAnalysis(lpbyData, wDataSize, lpwReturn);
		return OK;
	case ICDPC_GENERATE_AC:
		N_GenerateACCommandAnalysis(lpbyData, wDataSize, lpwReturn);
		return OK;
	case ICDPC_READ_RECORD:
		N_ReadRecordCommandAnalysis(lpbyData, wDataSize, wReadRecordTemplate70Mode, lpwReturn);
		return OK;
	case ICDPC_INTERNAL_AUTHENTICATE:
		N_InternalAuthenticateCommandAnalysis(lpbyData, wDataSize, lpwReturn);
		return OK;
	case ICDPC_GET_DATA:
		N_GetDataCommandAnalysis(lpbyData, wDataSize, lpwReturn);
		return OK;
	case ICDPC_GET_CHALLENGE:
		N_GetChallengeCommandAnalysis(lpbyData, wDataSize, lpwReturn);
		return OK;
	}
	*lpwReturn = NG;
	//N_Alarm( PARAM_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS );
	return OK;
}


//////////////////////////////////////////////////////////////////////////////
// FuncClass= Internal Function
// FuncName	= SelectFileCommandAnalysis
// CallType	= Function
// Detail	=
// Valuable	= LPBYTE:lpbyData	:input
//			  WORD	:wDataSize	:input
//			  LPWORD:lpwReturn	:output
//				= OK
//				= NG
//				= FORMAT_ERR
// Return	= OK
//////////////////////////////////////////////////////////////////////////////
long UPICD::N_SelectFileCommandAnalysis( LPBYTE lpbyData, WORD wDataSize, WORD wFCIType ,LPWORD lpwReturn)
{
	WORD	wAnalysisSize = 0;
	LPBYTE	lpbyData_WK;
	LPBYTE	lpbyData_END;
	WORD	wTagSize = 0;
	WORD	wTag = 0;
	WORD	wVnLength = 0;
	WORD	wCommand = wFCIType;											
	WORD	&wClearCommandListCnt = stClearCommandList[wCommand].wCnt;
	WORD	wCnt;
	BOOL	bDFNFlg = FALSE;	
	BOOL	bFPTFlg = FALSE;	
	WORD	wAnalysisSize_FCI;	
	WORD	wVnLength_FCI;		
	WORD	*wICAttribute = 0;


	*lpwReturn = OK;
	lpbyData_WK = lpbyData;
	lpbyData_END = lpbyData_WK + wDataSize;

	// 清除READ RECORD命令响应报文的数据
	WORD &wClearCommandListCnt1 = stClearCommandList[ICDPC_READ_RECORD].wCnt;			

	for( WORD i = 0; i < wClearCommandListCnt1; i++ ){									
		F_InfoItemClear( stClearCommandList[ICDPC_READ_RECORD].wTag[i], lpwReturn );	
		if( *lpwReturn != OK ){															
			return OK;																	
		}																				
	}																					
	N_ClearCommandListClear( ICDPC_READ_RECORD );

	// 例如报文数据为：6F1E840E315041592E5359532E4444463031A50C8801015F2D027A689F1101019000，lpbyData_WK指向此数据的第0位（从左往右数）
	// 分析FCI模板， 输出wAnalysisSize为模板标签（6F）和字符长度（1E）所占用的字符2字节，wVnLength为6F标签下的数据长度1E
	N_TLVFormatAnalysis( ICDPC_IC_FCI_TEMPLATE, MANDATORY, lpbyData_WK,
		lpbyData_END - lpbyData_WK, &wAnalysisSize, &wVnLength, lpwReturn );
	if( *lpwReturn != OK ){
		if( *lpwReturn == FORMAT_ERR ){	
			return OK;					
		}								
		//N_Alarm( FORMAT_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS );
		return OK;
	}
	lpbyData_WK += wAnalysisSize;    // lpbyData_WK指针向右移动wAnalysisSize（2）个字节
	if( lpbyData_END != lpbyData_WK + wVnLength ){	
		*lpwReturn = FORMAT_ERR;					
		return OK;									
	}												

	// input size 
	while( lpbyData_WK < lpbyData_END ){
		// DFname analysis
		N_TLVFormatAnalysis( ICDPC_IC_DF_NAME, MANDATORY, lpbyData_WK,
			lpbyData_END - lpbyData_WK, &wAnalysisSize, &wVnLength, lpwReturn );
		if( *lpwReturn == NG ){
			//N_Alarm( FORMAT_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS );
			return OK;
		}
		if( *lpwReturn == FORMAT_ERR ){	// DFname format error 
			// templateA5 analysis
			N_TLVFormatAnalysis( ICDPC_IC_FCI_PROPRIETARY_TEMPLATE, MANDATORY, lpbyData_WK,
				lpbyData_END - lpbyData_WK, &wAnalysisSize, &wVnLength, lpwReturn );
			if( *lpwReturn != OK ){
				if( *lpwReturn == FORMAT_ERR ){	 
					N_TLVFormatAnalysis( NULL, OPTION, lpbyData_WK, lpbyData_END - lpbyData_WK,
						&wAnalysisSize, &wVnLength, lpwReturn );
					if( wAnalysisSize == 0 ){
						return OK;																
					}																			
					if( !N_TagDefineCheck(lpbyData_WK, lpbyData_END - lpbyData_WK) ){			
						*lpwReturn = OK;														
						lpbyData_WK += wAnalysisSize + wVnLength;								
						continue;																
					}																			
					if( *lpwReturn != OK ){														
						return OK;																
					}																			
					// tag  information  get 									
					N_GetTagInfo( lpbyData_WK, lpbyData_END - lpbyData_WK,						
						&wTag, &wTagSize, lpwReturn );
					switch( *lpwReturn ){														
					case NG:																	
						//N_Alarm( LOGIC_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS );		
						return OK;																
					case FORMAT_ERR:															
						return OK;																
					default:																	
						break;																	
					}																			
					lpbyData_WK += wAnalysisSize;												
					if( lpbyData_WK >= lpbyData_END ) break;									
					if( wFCIType == ICDPC_SELECT_PSE ){											
						switch( wTag ){															
						case ICDPC_IC_FCI_TEMPLATE:												
						case ICDPC_IC_SFI:														
						case ICDPC_IC_LANGUAGE_PREFERENCE:										
						case ICDPC_IC_ISSUER_CODE_TABLE_INDEX:									
						case ICDPC_IC_FCI_ISSUER_DISCRETIONARY_DATA_EMV:						
							*lpwReturn = FORMAT_ERR;											
							return OK;															
						}																		
					}else if( wFCIType == ICDPC_SELECT_DDF ){									
						switch( wTag ){															
						case ICDPC_IC_FCI_TEMPLATE:												
						case ICDPC_IC_SFI:														
						case ICDPC_IC_FCI_ISSUER_DISCRETIONARY_DATA_EMV:						
							*lpwReturn = FORMAT_ERR;											
							return OK;															
						}																		
					}else if( wFCIType == ICDPC_SELECT_ADF ){									
						switch( wTag ){										
						case ICDPC_IC_FCI_TEMPLATE:							
						case ICDPC_IC_APPLICATION_LABEL:					
						case ICDPC_IC_API:									
						case ICDPC_IC_PDOL:									
						case ICDPC_IC_LANGUAGE_PREFERENCE:					
						case ICDPC_IC_ISSUER_CODE_TABLE_INDEX:				
						case ICDPC_IC_AP_PREFERRED_NAME:					
						case ICDPC_IC_FCI_ISSUER_DISCRETIONARY_DATA_EMV:	
						case ICDPC_IC_LOG_ENTRY:							
							*lpwReturn = FORMAT_ERR;						
							return OK;										
						}
					}else{																		
						//N_Alarm( LOGIC_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS );		
						return OK;																
					}																			
					N_InfoTypeTblGetMember( wTag, IC_ACQUIRED_ADDRESS, &wICAttribute, lpwReturn );
					if( *lpwReturn != OK ){
						//N_Alarm( LOGIC_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS );	
						return OK;															
					}																		
					if( *wICAttribute == IC ){	
						F_InfoSet( wTag, lpbyData_WK, wVnLength, lpwReturn );						
						if( *lpwReturn != OK ){														
							return OK;																
						}
					}																		
					stClearCommandList[wCommand].wTag[wClearCommandListCnt] = wTag;	
					wClearCommandListCnt++;											
					lpbyData_WK += wVnLength;										
					continue;														
				}
				//N_Alarm( FORMAT_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS );
				return OK;
			}
			bFPTFlg = TRUE;	 
			lpbyData_WK += wAnalysisSize;

			// input size
			LPBYTE	lpbyData2_END = lpbyData_WK + wVnLength;
			while( lpbyData_WK < lpbyData2_END ){
				N_TLVFormatAnalysis( NULL, OPTION, lpbyData_WK, lpbyData_END - lpbyData_WK,
					&wAnalysisSize, &wVnLength, lpwReturn );
				// Error handling for FCI response data
				// ( AP Label, AP Preferred Name, Issuer Code Table Index, Language Preference )
				N_ErrorHandlingForFCIResponseData( lpbyData_WK, lpbyData_END - lpbyData_WK, 			
					wAnalysisSize, wVnLength, 							
					&wAnalysisSize_FCI, &wVnLength_FCI, lpwReturn );		
				if( wAnalysisSize_FCI > 0){																
					lpbyData_WK += wAnalysisSize_FCI + wVnLength_FCI;									
					N_TLVFormatAnalysis( NULL, OPTION, lpbyData_WK, lpbyData_END - lpbyData_WK,			
						&wAnalysisSize, &wVnLength, lpwReturn );							
				}																					
				if( wAnalysisSize == 0 ){											
					return OK;														
				}																	
				if( !N_TagDefineCheck(lpbyData_WK, lpbyData_END - lpbyData_WK) ){
					*lpwReturn = OK;							
					lpbyData_WK += wAnalysisSize + wVnLength;	
					continue;									
				}																	
				if( *lpwReturn != OK ){
					return OK;														
				}
				// tag  information  get 
				N_GetTagInfo( lpbyData_WK, lpbyData_END - lpbyData_WK, &wTag, &wTagSize, lpwReturn );
				switch( *lpwReturn ){
				case NG:
					//N_Alarm( LOGIC_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS );
					return OK;
				case FORMAT_ERR:
					return OK;
				default:
					break;
				}
				lpbyData_WK += wAnalysisSize;
				if( lpbyData_WK >= lpbyData2_END ) break;	
				if( wFCIType == ICDPC_SELECT_PSE ){									
					switch( wTag ){													
					case ICDPC_IC_FCI_TEMPLATE:										
					case ICDPC_IC_DF_NAME:											
					case ICDPC_IC_FCI_PROPRIETARY_TEMPLATE:							
						*lpwReturn = FORMAT_ERR;									
						return OK;													
					}																
				}else if( wFCIType == ICDPC_SELECT_DDF ){							
					switch( wTag ){													
					case ICDPC_IC_FCI_TEMPLATE:										
					case ICDPC_IC_DF_NAME:											
					case ICDPC_IC_FCI_PROPRIETARY_TEMPLATE:							
						*lpwReturn = FORMAT_ERR;									
						return OK;													
					}																
				}else if( wFCIType == ICDPC_SELECT_ADF ){							
					switch( wTag ){							 
					case ICDPC_IC_FCI_TEMPLATE:				 
					case ICDPC_IC_DF_NAME:					 
					case ICDPC_IC_FCI_PROPRIETARY_TEMPLATE:	 
					case ICDPC_IC_LOG_ENTRY:				 
						*lpwReturn = FORMAT_ERR;			 
						return OK;							 
					}										 
				}else{																 
					//N_Alarm( LOGIC_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS ); 
					return OK;														 
				}																	 
				N_InfoTypeTblGetMember( wTag, IC_ACQUIRED_ADDRESS, &wICAttribute, lpwReturn );		 
				if( *lpwReturn != OK ){													 
					//N_Alarm( LOGIC_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS );	 
					return OK;															 
				}																		 
				if( *wICAttribute == IC ){		
					F_InfoSet( wTag, lpbyData_WK, wVnLength, lpwReturn );
					if( *lpwReturn != OK ){
						return OK;
					}
				}																					 
				stClearCommandList[wCommand].wTag[wClearCommandListCnt] = wTag;
				wClearCommandListCnt++;
				if( wTag == ICDPC_IC_FCI_ISSUER_DISCRETIONARY_DATA_EMV ){								 
					WORD	wVnLength1 = 0;																 
					LPBYTE	lpbyData1_WK = lpbyData_WK;													 
					LPBYTE	lpbyData1_END = lpbyData_WK + wVnLength;									 
					while( lpbyData1_WK < lpbyData1_END ){												 
						N_TLVFormatAnalysis( NULL, OPTION, lpbyData1_WK, lpbyData1_END - lpbyData1_WK,	 
							&wAnalysisSize, &wVnLength1, lpwReturn );					 
						// Error handling for FCI response data
						// ( AP Label, AP Preferred Name, Issuer Code Table Index, Language Preference )
						N_ErrorHandlingForFCIResponseData( lpbyData1_WK, lpbyData1_END - lpbyData1_WK, 			 
							wAnalysisSize, wVnLength1, 							 
							&wAnalysisSize_FCI, &wVnLength_FCI, lpwReturn );		 
						if( wAnalysisSize_FCI > 0){																 
							lpbyData1_WK += wAnalysisSize_FCI + wVnLength_FCI;									 
							N_TLVFormatAnalysis( NULL, OPTION, lpbyData1_WK, lpbyData1_END - lpbyData1_WK,		 
								&wAnalysisSize, &wVnLength1, lpwReturn );							 
						}																						 
						if( wAnalysisSize == 0 ){														 	 
							return OK;																	 
						}																				 
						if( !N_TagDefineCheck(lpbyData1_WK, lpbyData1_END - lpbyData1_WK) ){			 
							*lpwReturn = OK;															 
							lpbyData1_WK += wAnalysisSize + wVnLength1;									 
							continue;																	 
						}																				 
						if( *lpwReturn != OK ){															 
							return OK;																	 
						}																				 
						// tag  information  get 																	 
						N_GetTagInfo( lpbyData1_WK, lpbyData1_END - lpbyData1_WK,						 
							&wTag, &wTagSize, lpwReturn );									 
						switch( *lpwReturn ){															 
						case NG:																		 
							//N_Alarm( LOGIC_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS );			 
							return OK;																	 
						case FORMAT_ERR:																 
							return OK;																	 
						default:																		 
							break;																		 
						}																				 
						lpbyData1_WK += wAnalysisSize;													 
						if( lpbyData1_WK >= lpbyData1_END ) break;										 
						if( wFCIType == ICDPC_SELECT_PSE ){									 
							switch( wTag ){													 
							case ICDPC_IC_FCI_TEMPLATE:										 
							case ICDPC_IC_DF_NAME:											 
							case ICDPC_IC_FCI_PROPRIETARY_TEMPLATE:							 
							case ICDPC_IC_SFI:												 
							case ICDPC_IC_LANGUAGE_PREFERENCE:								 
							case ICDPC_IC_ISSUER_CODE_TABLE_INDEX:							 
							case ICDPC_IC_FCI_ISSUER_DISCRETIONARY_DATA_EMV:				 
								*lpwReturn = FORMAT_ERR;									 
								return OK;													 
							}																 
						}else if( wFCIType == ICDPC_SELECT_DDF ){							 
							switch( wTag ){													 
							case ICDPC_IC_FCI_TEMPLATE:										 
							case ICDPC_IC_DF_NAME:											 
							case ICDPC_IC_FCI_PROPRIETARY_TEMPLATE:							 
							case ICDPC_IC_SFI:												 
							case ICDPC_IC_FCI_ISSUER_DISCRETIONARY_DATA_EMV:				 
								*lpwReturn = FORMAT_ERR;									 
								return OK;													 
							}																 
						}else if( wFCIType == ICDPC_SELECT_ADF ){							 
							switch( wTag ){										 
							case ICDPC_IC_FCI_TEMPLATE:							 
							case ICDPC_IC_DF_NAME:								 
							case ICDPC_IC_FCI_PROPRIETARY_TEMPLATE:				 
							case ICDPC_IC_APPLICATION_LABEL:					 
							case ICDPC_IC_API:									 
							case ICDPC_IC_PDOL:									 
							case ICDPC_IC_LANGUAGE_PREFERENCE:					 
							case ICDPC_IC_ISSUER_CODE_TABLE_INDEX:				 
							case ICDPC_IC_AP_PREFERRED_NAME:					 
							case ICDPC_IC_FCI_ISSUER_DISCRETIONARY_DATA_EMV:	 
								*lpwReturn = FORMAT_ERR;						 
								return OK;										 
							}													 
						}else{																 
							//N_Alarm( LOGIC_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS ); 
							return OK;														 
						}																	 
						N_InfoTypeTblGetMember( wTag, IC_ACQUIRED_ADDRESS, &wICAttribute, lpwReturn );			 
						if( *lpwReturn != OK ){													 
							//N_Alarm( LOGIC_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS );	 
							return OK;															 
						}																		 
						if( *wICAttribute == IC ){	
							F_InfoSet( wTag, lpbyData1_WK, wVnLength1, lpwReturn );							 
							if( *lpwReturn != OK ){															 
								return OK;																	 
							}
						}																						 
						stClearCommandList[wCommand].wTag[wClearCommandListCnt] = wTag;					 
						wClearCommandListCnt++;															 
						lpbyData1_WK += wVnLength1;														 
					}																					 
				}																						 
				lpbyData_WK += wVnLength;																 
			}
		}else{
			lpbyData_WK += wAnalysisSize;
			// DFname information set 
			F_InfoSet( ICDPC_IC_DF_NAME, lpbyData_WK, wVnLength, lpwReturn );
			if( *lpwReturn != OK ){
				return OK;
			}
			bDFNFlg = TRUE;	 
			stClearCommandList[wCommand].wTag[wClearCommandListCnt] = ICDPC_IC_DF_NAME;
			wClearCommandListCnt++;

			WORD wMiniSize;
			BYTE byDFName[33];												
			for ( wCnt = 0; wCnt < stTerminalAIDList.wNum; wCnt++){
				if( stTerminalAIDList.stTerminalAID[wCnt].lpsAID != NULL ){
					wMiniSize = strlen(stTerminalAIDList.stTerminalAID[wCnt].lpsAID);
					if( wMiniSize >= wVnLength ){
						// ICcard DFname(ASC code change)
						N_HEXtoASC( byDFName, lpbyData_WK, wVnLength, lpwReturn);
						if( *lpwReturn != OK ){
							return OK;
						}
                        if( memcmp( byDFName, (LPBYTE)stTerminalAIDList.stTerminalAID[wCnt].lpsAID, wMiniSize) == 0 ){
							F_InfoSet( ICDPC_TM_APPLICATION_VER_NO,
								stTerminalAIDList.stTerminalAID[wCnt].byAPVersionIndicator,
								sizeof(stTerminalAIDList.stTerminalAID[wCnt].byAPVersionIndicator), lpwReturn );
							if( *lpwReturn != OK ){
								return OK;
							}
							break;
						}
					}
				}
			}

			lpbyData_WK += wVnLength;
		}
	}
	if( !bDFNFlg || !bFPTFlg){		 
		*lpwReturn = FORMAT_ERR;	 
	}								 
	return OK;
}

//////////////////////////////////////////////////////////////////////////////
// FuncClass= Internal Function
// FuncName	= GetProcessingOptionsCommandAnalysis
// CallType	= Function
// Detail	=
// Valuable	= LPBYTE:lpbyData	:input
//			  WORD	:wDataSize	:input
//			  LPWORD:lpwReturn	:output
//				= OK
//				= NG
// Return	= OK
//////////////////////////////////////////////////////////////////////////////
long UPICD::N_GetProcessingOptionsCommandAnalysis( LPBYTE lpbyData, WORD wDataSize, LPWORD lpwReturn )
{
	WORD	wAnalysisSize = 0;
	LPBYTE	lpbyData_WK;
	LPBYTE	lpbyData_END;
	WORD	wTagSize = 0;
	WORD	wTag = 0;
	WORD	wTLSize = 0;
	WORD	*lpwFormatData[1];
	WORD	wVnLength = 0;
	BYTE	byBuff[256];
	WORD	wCommand = ICDPC_GET_PROCESSING_OPTIONS;
	WORD	&wClearCommandListCnt = stClearCommandList[wCommand].wCnt;
	BOOL	bAIPFlg = FALSE;	 
	BOOL	bAFLFlg = FALSE;	 
	WORD	*wICAttribute = 0;				 

	*lpwReturn = OK;
	lpbyData_WK = lpbyData;
	lpbyData_END = lpbyData_WK + wDataSize;

	// template80 analysis
	N_TLVFormatAnalysis( ICDPC_IC_RESPONSE_MESSAGE_TEMPLATE_FORMAT1, MANDATORY, lpbyData_WK,
		lpbyData_END - lpbyData_WK, &wAnalysisSize, &wVnLength, lpwReturn );
	if( *lpwReturn == NG ){
		//N_Alarm( FORMAT_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS );
		return OK;
	}
	if( *lpwReturn == FORMAT_ERR ){	// 80 format error 
		// template77 analysis
		N_TLVFormatAnalysis( ICDPC_IC_RESPONSE_MESSAGE_TEMPLATE_FORMAT2, MANDATORY, lpbyData_WK,
			lpbyData_END - lpbyData_WK, &wAnalysisSize, &wVnLength, lpwReturn );
		if( *lpwReturn != OK ){
			//N_Alarm( FORMAT_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS );
			return OK;
		}
		lpbyData_WK += wAnalysisSize;
		if( lpbyData_WK + wVnLength != lpbyData_END ){							 
			*lpwReturn = FORMAT_ERR;											 
			//N_Alarm( FORMAT_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS );	 
			return OK;															 
		}																		 
		// input size
		while( lpbyData_WK < lpbyData_END ){
			N_TLVFormatAnalysis( NULL, OPTION, lpbyData_WK, lpbyData_END - lpbyData_WK,
				&wAnalysisSize, &wVnLength, lpwReturn );
			if( wAnalysisSize == 0 ){											 	 
				return OK;														 
			}																	 
			if( !N_TagDefineCheck(lpbyData_WK, lpbyData_END - lpbyData_WK) ){	 
				*lpwReturn = OK;												 
				lpbyData_WK += wAnalysisSize + wVnLength;						 
				continue;														 
			}																	 
			if( *lpwReturn != OK ){												 
				return OK;														 
			}																	 
			// tag  information  get 
			N_GetTagInfo( lpbyData_WK, lpbyData_END - lpbyData_WK, &wTag, &wTagSize, lpwReturn );
			switch( *lpwReturn ){
			case NG:
				//N_Alarm( LOGIC_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS );
				return OK;
			case FORMAT_ERR:
				//N_Alarm( FORMAT_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS );	 
				return OK;															 
			default:
				break;
			}
			lpbyData_WK += wAnalysisSize;
			if( lpbyData_WK >= lpbyData_END ) break;	 
			switch( wTag ){															 
			case ICDPC_IC_AIP:														 
				bAIPFlg = TRUE;														 
				if( wVnLength == 0 ){												 
					*lpwReturn = FORMAT_ERR;										 
					return OK;														 
				}																	 
				break;																 
			case ICDPC_IC_AFL:														 
				bAFLFlg = TRUE;														 
				if( wVnLength == 0 ){												 
					*lpwReturn = FORMAT_ERR;										 
					return OK;														 
				}																	 
				break;																 
			default:																 
				break;																 
			}																		 
			N_InfoTypeTblGetMember( wTag, IC_ACQUIRED_ADDRESS, &wICAttribute, lpwReturn );		 
			if( *lpwReturn != OK ){													 
				//N_Alarm( LOGIC_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS );	 
				return OK;															 
			}																		 
			if( *wICAttribute == IC ){															 
				F_InfoSet( wTag, lpbyData_WK, wVnLength, lpwReturn );
				if( *lpwReturn != OK ){
					return OK;
				}
			}																					 
			stClearCommandList[wCommand].wTag[wClearCommandListCnt] = wTag;
			wClearCommandListCnt++;
			lpbyData_WK += wVnLength;
		}
		if( !bAIPFlg || !bAFLFlg){		 
			*lpwReturn = FORMAT_ERR;	 
		}								 
		return OK;
	}
	lpbyData_WK += wAnalysisSize;

	// AIP buffer size  get 
	N_InfoTypeTblGetMember( ICDPC_IC_AIP, IC_BUFFSIZE, lpwFormatData, lpwReturn );
	if( *lpwReturn != OK ){
		//N_Alarm( LOGIC_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS );
		return OK;
	}

	// AIP TLVmake 
	N_SetTLV( ICDPC_IC_AIP, **lpwFormatData, lpbyData_WK,
		lpbyData_END - lpbyData_WK, byBuff, &wDataSize, &wTLSize, lpwReturn );
	if( *lpwReturn != OK ){
		return OK;
	}
	// AIP analysis
	N_TLVFormatAnalysis( ICDPC_IC_AIP, OPTION, byBuff,
		wDataSize, &wAnalysisSize, &wVnLength, lpwReturn );
	if( *lpwReturn != OK ){
		//N_Alarm( FORMAT_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS );
		return OK;
	}
	// AIP information set 
	F_InfoSet( ICDPC_IC_AIP, lpbyData_WK, wVnLength, lpwReturn );
	if( *lpwReturn != OK ){
		return OK;
	}
	stClearCommandList[wCommand].wTag[wClearCommandListCnt] = ICDPC_IC_AIP;
	wClearCommandListCnt++;
	lpbyData_WK += wVnLength;

	// AFL TLVmake 
	N_SetTLV( ICDPC_IC_AFL, lpbyData_END - lpbyData_WK, lpbyData_WK,
		lpbyData_END - lpbyData_WK, byBuff, &wDataSize, &wTLSize, lpwReturn );
	if( *lpwReturn != OK ){
		return OK;
	}
	// AFL analysis
	N_TLVFormatAnalysis( ICDPC_IC_AFL, OPTION, byBuff,
		wDataSize, &wAnalysisSize, &wVnLength, lpwReturn );
	if( *lpwReturn != OK ){
		//N_Alarm( FORMAT_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS );
		return OK;
	}
	// AFL information set 
	F_InfoSet( ICDPC_IC_AFL, lpbyData_WK, wVnLength, lpwReturn );
	stClearCommandList[wCommand].wTag[wClearCommandListCnt] = ICDPC_IC_AFL;
	wClearCommandListCnt++;
	return OK;
}


//////////////////////////////////////////////////////////////////////////////
// FuncClass= Internal Function
// FuncName	= ReadRecordCommandAnalysis
// CallType	= Function
// Detail	=
// Valuable	= LPBYTE:lpbyData	:input
//			  WORD	:wDataSize	:input
//			  LPWORD:lpwReturn	:output
//				= OK
//				= NG
// Return	= OK
//////////////////////////////////////////////////////////////////////////////
long UPICD::N_ReadRecordCommandAnalysis( LPBYTE lpbyData, WORD wDataSize, 
	WORD wReadRecordTemplate70Mode, LPWORD lpwReturn )
{
	WORD	wAnalysisSize = 0;
	LPBYTE	lpbyData_WK;
	LPBYTE	lpbyData_END;
	WORD	wTagSize = 0;
	WORD	wTag = 0;
	WORD	wVnLength = 0;
	WORD	*wICAttribute = 0;				 
	WORD	wCommand = ICDPC_READ_RECORD;    //XiongJie
	WORD	&wClearCommandListCnt = stClearCommandList[wCommand].wCnt;    // XiongJie

	*lpwReturn = OK;
	lpbyData_WK = lpbyData;
	lpbyData_END = lpbyData_WK + wDataSize;

	//analysis template70
	N_TLVFormatAnalysis( ICDPC_IC_TEMPLATE_70, wReadRecordTemplate70Mode, lpbyData_WK,	 
		lpbyData_END - lpbyData_WK, &wAnalysisSize, &wVnLength, lpwReturn );
	if( *lpwReturn != OK ){
		//N_Alarm( FORMAT_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS );
		return OK;
	}
	lpbyData_WK += wAnalysisSize;
	if(MANDATORY== wReadRecordTemplate70Mode &&											 
		lpbyData_END != lpbyData_WK + wVnLength ){	 
			*lpwReturn = FORMAT_ERR;					 
			return OK;									 
	}

	F_InfoSet( ICDPC_IC_TEMPLATE_70, lpbyData_WK, wVnLength, lpwReturn );		 
	if( *lpwReturn != OK ){														 
		return OK;															 
	}

	stClearCommandList[wCommand].wTag[wClearCommandListCnt] = ICDPC_IC_TEMPLATE_70;    // XiongJie
	wClearCommandListCnt++;															   // XiongJie

	// input size
	while( lpbyData_WK < lpbyData_END ){
		N_TLVFormatAnalysis( NULL, OPTION, lpbyData_WK, lpbyData_END - lpbyData_WK,
			&wAnalysisSize, &wVnLength, lpwReturn );
		if( wAnalysisSize == 0){						
			return OK;									
		}
		if( !N_TagDefineCheck(lpbyData_WK, lpbyData_END - lpbyData_WK) ){
			*lpwReturn = OK;							
			lpbyData_WK += wAnalysisSize + wVnLength;	
			continue;									
		}

		if( *lpwReturn != OK ){
			return OK;
		}
		// tag  information  get 
		N_GetTagInfo( lpbyData_WK, lpbyData_END - lpbyData_WK, &wTag, &wTagSize, lpwReturn );
		switch( *lpwReturn ){
		case NG:
			//N_Alarm( LOGIC_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS );
			return OK;
		case FORMAT_ERR:
			return OK;
		default:
			break;
		}
		N_TagSetCheck( wTag, lpwReturn );	  
		if( *lpwReturn != OK ){
			return OK;
		}
		lpbyData_WK += wAnalysisSize;
		if( lpbyData_WK >= lpbyData_END ) break;	 
		switch( wTag ){								 
		case ICDPC_IC_APPLICATION_EXPIRATION_DATE:	 
		case ICDPC_IC_PAN:							 
		case ICDPC_IC_CDOL1:						 
		case ICDPC_IC_CDOL2:						 
			if(wVnLength == 0){						 
				*lpwReturn = FORMAT_ERR;			 
				return OK;							 
			}										 
			break;									 
		default:									 
			break;									 
		}											 
		N_InfoTypeTblGetMember( wTag, IC_ACQUIRED_ADDRESS, &wICAttribute, lpwReturn );		 
		if( *lpwReturn != OK ){													 
			//N_Alarm( LOGIC_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS );	 
			return OK;															 
		}																		 
		if( *wICAttribute == IC ){															 
			F_InfoSet( wTag, lpbyData_WK, wVnLength, lpwReturn );
			if( *lpwReturn != OK ){
				return OK;
			}
			stClearCommandList[wCommand].wTag[wClearCommandListCnt] = wTag;    // XiongJie
			wClearCommandListCnt++;											   // XiongJie
		}																					 
		if( wTag == ICDPC_IC_TDOL ){	 
			TDOLFlg = FALSE;			 
		}								 
		lpbyData_WK += wVnLength;
	}
	return OK;
}

//////////////////////////////////////////////////////////////////////////////
// FuncClass= Internal Function
// FuncName	= GenerateACCommandAnalysis
// CallType	= Function
// Detail	=
// Valuable	= LPBYTE:lpbyData	:input
//			  WORD	:wDataSize	:input
//			  LPWORD:lpwReturn	:output
//				= OK
//				= NG
// Return	= OK
//////////////////////////////////////////////////////////////////////////////
long UPICD::N_GenerateACCommandAnalysis( LPBYTE lpbyData, WORD wDataSize, LPWORD lpwReturn )
{
	WORD	wAnalysisSize = 0;
	LPBYTE	lpbyData_WK;
	LPBYTE	lpbyData_END;
	WORD	wTagSize = 0;
	WORD	wTag = 0;
	WORD	wTLSize = 0;
	WORD	*lpwFormatData[1];
	WORD	wVnLength = 0;
	BYTE	byBuff[256];
	WORD	wCommand = ICDPC_GENERATE_AC;
	WORD	&wClearCommandListCnt = stClearCommandList[wCommand].wCnt;
	BOOL	bCIDFlg = FALSE;		 
	BOOL	bATCFlg = FALSE;		 
	BOOL	bACFlg = FALSE;			 
	WORD	*wICAttribute = 0;				 

	*lpwReturn = OK;
	lpbyData_WK = lpbyData;
	lpbyData_END = lpbyData_WK + wDataSize;

	// template80 analysis
	N_TLVFormatAnalysis( ICDPC_IC_RESPONSE_MESSAGE_TEMPLATE_FORMAT1, MANDATORY, lpbyData_WK,
		lpbyData_END - lpbyData_WK, &wAnalysisSize, &wVnLength, lpwReturn );
	if( *lpwReturn == NG ){
		//N_Alarm( FORMAT_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS );
		return OK;
	}
	if( *lpwReturn == FORMAT_ERR ){	// 80 format error 
		// template77 analysis
		N_TLVFormatAnalysis( ICDPC_IC_RESPONSE_MESSAGE_TEMPLATE_FORMAT2, MANDATORY, lpbyData_WK,
			lpbyData_END - lpbyData_WK, &wAnalysisSize, &wVnLength, lpwReturn );
		if( *lpwReturn != OK ){
			//N_Alarm( FORMAT_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS );
			return OK;
		}
		lpbyData_WK += wAnalysisSize;
		if( lpbyData_WK + wVnLength != lpbyData_END ){							 
			*lpwReturn = FORMAT_ERR;											 
			//N_Alarm( FORMAT_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS );	 
			return OK;															 
		}																		 
		// input size
		while( lpbyData_WK < lpbyData_END ){
			N_TLVFormatAnalysis( NULL, OPTION, lpbyData_WK,
				lpbyData_END - lpbyData_WK, &wAnalysisSize, &wVnLength, lpwReturn );
			if( wAnalysisSize == 0 ){											 	 
				return OK;														 
			}																	 
			if( !N_TagDefineCheck(lpbyData_WK, lpbyData_END - lpbyData_WK) ){	 
				*lpwReturn = OK;												 
				lpbyData_WK += wAnalysisSize + wVnLength;						 
				continue;														 
			}																	 
			if( *lpwReturn != OK ){												 
				return OK;														 
			}																	 
			// tag  information  get 
			N_GetTagInfo( lpbyData_WK, lpbyData_END - lpbyData_WK,
				&wTag, &wTagSize, lpwReturn );
			switch( *lpwReturn ){
			case NG:
				//N_Alarm( LOGIC_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS );
				return OK;
			case FORMAT_ERR:
				//N_Alarm( FORMAT_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS );	 
				return OK;															 
			default:
				break;
			}
			lpbyData_WK += wAnalysisSize;
			if( lpbyData_WK >= lpbyData_END ) break;	 
			switch( wTag ){															 
			case ICDPC_IC_CID:														 
				bCIDFlg = TRUE;														 
				if( wVnLength == 0 ){												 
					*lpwReturn = FORMAT_ERR;										 
					return OK;														 
				}																	 
				break;																 
			case ICDPC_IC_ATC:														 
				bATCFlg = TRUE;														 
				if( wVnLength == 0 ){												 
					*lpwReturn = FORMAT_ERR;										 
					return OK;														 
				}																	 
				break;																 
			case ICDPC_IC_AC:														 
				bACFlg = TRUE;														 
				if( wVnLength == 0 ){												 
					*lpwReturn = FORMAT_ERR;										 
					return OK;														 
				}																	 
				break;																 
			case ICDPC_IC_SIGNED_DYNAMIC_APPLICATION_DATA:							 
			case ICDPC_IC_ISSUER_APPLICATION_DATA:									 
				break;																 
			default:																 
				break;																 
			}																		 
			N_InfoTypeTblGetMember( wTag, IC_ACQUIRED_ADDRESS, &wICAttribute, lpwReturn );		 
			if( *lpwReturn != OK ){													 
				//N_Alarm( LOGIC_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS );	 
				return OK;															 
			}																		 
			if( *wICAttribute == IC ){															 
				F_InfoSet( wTag, lpbyData_WK, wVnLength, lpwReturn );
				if( *lpwReturn != OK ){
					return OK;
				}
			}																					 
			stClearCommandList[wCommand].wTag[wClearCommandListCnt] = wTag;
			wClearCommandListCnt++;
			lpbyData_WK += wVnLength;
		}
		if( !bCIDFlg || !bATCFlg || !bACFlg){
			*lpwReturn = FORMAT_ERR;				 
		}											 
		return OK;
	}
	lpbyData_WK += wAnalysisSize;

	// CID buffer size  get 
	N_InfoTypeTblGetMember( ICDPC_IC_CID, IC_BUFFSIZE, lpwFormatData, lpwReturn );		 
	if( *lpwReturn != OK ){
		//N_Alarm( LOGIC_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS );
		return OK;
	}

	// CID TLVmake 
	N_SetTLV( ICDPC_IC_CID, **lpwFormatData, lpbyData_WK,								 
		lpbyData_END - lpbyData_WK, byBuff, &wDataSize, &wTLSize, lpwReturn );	 
	if( *lpwReturn != OK ){
		return OK;
	}
	// CID analysis
	N_TLVFormatAnalysis( ICDPC_IC_CID, OPTION, byBuff, wDataSize,						 
		&wAnalysisSize, &wVnLength, lpwReturn );						 
	if( *lpwReturn != OK ){
		//N_Alarm( FORMAT_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS );
		return OK;
	}
	// CIDset 
	F_InfoSet( ICDPC_IC_CID, lpbyData_WK, wVnLength, lpwReturn );
	if( *lpwReturn != OK ){
		return OK;
	}
	stClearCommandList[wCommand].wTag[wClearCommandListCnt] = ICDPC_IC_CID;
	wClearCommandListCnt++;
	lpbyData_WK += wVnLength;

	// ATC buffer size  get 
	N_InfoTypeTblGetMember( ICDPC_IC_ATC, IC_BUFFSIZE, lpwFormatData, lpwReturn );		 
	if( *lpwReturn != OK ){
		//N_Alarm( LOGIC_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS );
		return OK;
	}

	// ATC TLV make 
	N_SetTLV( ICDPC_IC_ATC, **lpwFormatData, lpbyData_WK,								 
		lpbyData_END - lpbyData_WK, byBuff, &wDataSize, &wTLSize, lpwReturn );	 
	if( *lpwReturn != OK ){
		return OK;
	}
	// ATC analysis
	N_TLVFormatAnalysis( ICDPC_IC_ATC, OPTION, byBuff, wDataSize,						 
		&wAnalysisSize, &wVnLength, lpwReturn );						 
	if( *lpwReturn != OK ){
		//N_Alarm( FORMAT_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS );
		return OK;
	}
	// ATC information set 
	F_InfoSet( ICDPC_IC_ATC, lpbyData_WK, wVnLength, lpwReturn );
	if( *lpwReturn != OK ){
		return OK;
	}
	stClearCommandList[wCommand].wTag[wClearCommandListCnt] = ICDPC_IC_ATC;
	wClearCommandListCnt++;
	lpbyData_WK += wVnLength;

	// AC buffer size  get 
	N_InfoTypeTblGetMember( ICDPC_IC_AC, IC_BUFFSIZE, lpwFormatData, lpwReturn );		 
	if( *lpwReturn != OK ){
		//N_Alarm( LOGIC_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS );
		return OK;
	}

	// AC TLVmake 
	N_SetTLV( ICDPC_IC_AC, **lpwFormatData, lpbyData_WK,								 
		lpbyData_END - lpbyData_WK, byBuff, &wDataSize, &wTLSize, lpwReturn );	 
	if( *lpwReturn != OK ){
		return OK;
	}
	// AC analysis
	N_TLVFormatAnalysis( ICDPC_IC_AC, OPTION, byBuff, wDataSize,						 
		&wAnalysisSize, &wVnLength, lpwReturn );						 
	if( *lpwReturn != OK ){
		//N_Alarm( FORMAT_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS );
		return OK;
	}
	// AC information set 
	F_InfoSet( ICDPC_IC_AC, lpbyData_WK, wVnLength, lpwReturn );
	if( *lpwReturn != OK ){
		return OK;
	}
	stClearCommandList[wCommand].wTag[wClearCommandListCnt] = ICDPC_IC_AC;
	wClearCommandListCnt++;
	lpbyData_WK += wVnLength;

	if(lpbyData_WK == lpbyData_END){					 
		return OK;										 
	}													 

	// ISSUER AP buffer size  get 
	N_InfoTypeTblGetMember( ICDPC_IC_ISSUER_APPLICATION_DATA, IC_BUFFSIZE, lpwFormatData, lpwReturn );	 
	if( *lpwReturn != OK ){
		//N_Alarm( LOGIC_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS );
		return OK;
	}

	// ISSUER AP TLVmake 
	N_SetTLV( ICDPC_IC_ISSUER_APPLICATION_DATA, lpbyData_END - lpbyData_WK, lpbyData_WK,	 
		lpbyData_END - lpbyData_WK, byBuff, &wDataSize, &wTLSize, lpwReturn );		 
	if( *lpwReturn != OK ){
		return OK;
	}
	// ISSUER APanalysis
	N_TLVFormatAnalysis( ICDPC_IC_ISSUER_APPLICATION_DATA, OPTION, byBuff, wDataSize,		 
		&wAnalysisSize, &wVnLength, lpwReturn );							 
	if( *lpwReturn != OK ){
		//N_Alarm( FORMAT_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS );
		return OK;
	}
	// ISSUER AP information set 
	F_InfoSet( ICDPC_IC_ISSUER_APPLICATION_DATA, lpbyData_WK, wVnLength, lpwReturn );
	stClearCommandList[wCommand].wTag[wClearCommandListCnt] = ICDPC_IC_ISSUER_APPLICATION_DATA;
	wClearCommandListCnt++;
	return OK;
}

//////////////////////////////////////////////////////////////////////////////
// FuncClass= Internal Function
// FuncName	= InternalAuthenticateCommandAnalysis
// CallType	= Function
// Detail	=
// Valuable	= LPBYTE:lpbyData	:input
//			  WORD	:wDataSize	:input
//			  LPWORD:lpwReturn	:output
//				= OK
//				= NG
// Return	= OK
//////////////////////////////////////////////////////////////////////////////
long UPICD::N_InternalAuthenticateCommandAnalysis( LPBYTE lpbyData, WORD wDataSize, LPWORD lpwReturn )
{
	WORD	wAnalysisSize = 0;
	LPBYTE	lpbyData_WK;
	LPBYTE	lpbyData_END;
	WORD	wTagSize = 0;
	WORD	wTag = 0;
	WORD	wTLSize = 0;
	WORD	*lpwFormatData[1];
	WORD	wVnLength = 0;
	BYTE	byBuff[256];
	WORD	wCommand = ICDPC_INTERNAL_AUTHENTICATE;
	WORD	&wClearCommandListCnt = stClearCommandList[wCommand].wCnt;
	WORD	*wICAttribute = 0;				 


	*lpwReturn = OK;
	lpbyData_WK = lpbyData;
	lpbyData_END = lpbyData_WK + wDataSize;

	// analysis template80
	N_TLVFormatAnalysis( ICDPC_IC_RESPONSE_MESSAGE_TEMPLATE_FORMAT1, MANDATORY, lpbyData_WK,
		lpbyData_END - lpbyData_WK, &wAnalysisSize, &wVnLength, lpwReturn );
	if( *lpwReturn == NG ){
		//N_Alarm( FORMAT_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS );
		return OK;
	}
	if( *lpwReturn == FORMAT_ERR ){	// 80 format error 
		// analysis template77 
		N_TLVFormatAnalysis( ICDPC_IC_RESPONSE_MESSAGE_TEMPLATE_FORMAT2, MANDATORY, lpbyData_WK,
			lpbyData_END - lpbyData_WK, &wAnalysisSize, &wVnLength, lpwReturn );
		if( *lpwReturn != OK ){
			//N_Alarm( FORMAT_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS );
			return OK;
		}
		lpbyData_WK += wAnalysisSize;
		//input size
		while( lpbyData_WK < lpbyData_END ){
			N_TLVFormatAnalysis( NULL, OPTION, lpbyData_WK,
				lpbyData_END - lpbyData_WK, &wAnalysisSize, &wVnLength, lpwReturn );
			if( wAnalysisSize == 0){											 	 
				return OK;														 
			}																	 
			if( !N_TagDefineCheck(lpbyData_WK, lpbyData_END - lpbyData_WK) ){	 
				*lpwReturn = OK;												 
				lpbyData_WK += wAnalysisSize + wVnLength;						 
				continue;														 
			}																	 
			if( *lpwReturn != OK ){												 
				return OK;														 
			}																	 
			// tag information get 
			N_GetTagInfo( lpbyData_WK, lpbyData_END - lpbyData_WK,
				&wTag, &wTagSize, lpwReturn );
			switch( *lpwReturn ){
			case NG:
				//N_Alarm( LOGIC_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS );
				return OK;
			case FORMAT_ERR:
				*lpwReturn = OK;
			default:
				break;
			}
			lpbyData_WK += wAnalysisSize;
			if( lpbyData_WK >= lpbyData_END ) break;	 
			N_InfoTypeTblGetMember( wTag, IC_ACQUIRED_ADDRESS, &wICAttribute, lpwReturn );		 
			if( *lpwReturn != OK ){													 
				//N_Alarm( LOGIC_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS );	 
				return OK;															 
			}																		 
			if( *wICAttribute == IC ){															 
				F_InfoSet( wTag, lpbyData_WK, wVnLength, lpwReturn );
				if( *lpwReturn != OK ){
					return OK;
				}
			}																					 
			stClearCommandList[wCommand].wTag[wClearCommandListCnt] = wTag;
			wClearCommandListCnt++;
			lpbyData_WK += wVnLength;
		}
		return OK;
	}
	lpbyData_WK += wAnalysisSize;

	// signed dynamic AP information buffer size  get 
	N_InfoTypeTblGetMember( ICDPC_IC_SIGNED_DYNAMIC_APPLICATION_DATA, IC_BUFFSIZE, lpwFormatData, lpwReturn );
	if( *lpwReturn != OK ){
		//N_Alarm( LOGIC_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS );
		return OK;
	}

	// signed dynamic AP information TLV make 
	N_SetTLV( ICDPC_IC_SIGNED_DYNAMIC_APPLICATION_DATA, lpbyData_END - lpbyData_WK, lpbyData_WK,
		lpbyData_END - lpbyData_WK, byBuff, &wDataSize, &wTLSize, lpwReturn );
	if( *lpwReturn != OK ){
		return OK;
	}
	// signed dynamic AP information analysis
	N_TLVFormatAnalysis( ICDPC_IC_SIGNED_DYNAMIC_APPLICATION_DATA, OPTION, byBuff, wDataSize,
		&wAnalysisSize, &wVnLength, lpwReturn );
	if( *lpwReturn != OK ){
		//N_Alarm( FORMAT_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS );
		return OK;
	}
	// signed dynamic AP information set 
	F_InfoSet( ICDPC_IC_SIGNED_DYNAMIC_APPLICATION_DATA, lpbyData_WK, wVnLength, lpwReturn );
	stClearCommandList[wCommand].wTag[wClearCommandListCnt] = ICDPC_IC_SIGNED_DYNAMIC_APPLICATION_DATA;
	wClearCommandListCnt++;
	return OK;
}

//////////////////////////////////////////////////////////////////////////////
// FuncClass= Internal Function
// FuncName	= GetDataCommandAnalysis
// CallType	= Function
// Detail	=
// Valuable	= LPBYTE:lpbyData	:input
//			  WORD	:wDataSize	:input
//			  LPWORD:lpwReturn	:output
//				= OK
//				= NG
// Return	= OK
//////////////////////////////////////////////////////////////////////////////
long UPICD::N_GetDataCommandAnalysis( LPBYTE lpbyData, WORD wDataSize, LPWORD lpwReturn )
{
	WORD	wAnalysisSize = 0;
	LPBYTE	lpbyData_WK;
	LPBYTE	lpbyData_END;
	WORD	wTagSize = 0;
	WORD	wTag = 0;
	WORD	wVnLength = 0;
	WORD	wCommand = ICDPC_GET_DATA;
	WORD	&wClearCommandListCnt = stClearCommandList[wCommand].wCnt;
	WORD	*wICAttribute = 0;				 


	*lpwReturn = OK;
	lpbyData_WK = lpbyData;
	lpbyData_END = lpbyData_WK + wDataSize;

	N_TLVFormatAnalysis( NULL, OPTION, lpbyData_WK, lpbyData_END - lpbyData_WK,
		&wAnalysisSize, &wVnLength, lpwReturn );
	if( *lpwReturn != OK ){
		//N_Alarm( FORMAT_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS );
		return OK;
	}
	// tag  information  get 
	N_GetTagInfo( lpbyData_WK, lpbyData_END - lpbyData_WK, &wTag, &wTagSize, lpwReturn );
	switch( *lpwReturn ){
	case NG:
		//N_Alarm( LOGIC_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS );
		return OK;
	case FORMAT_ERR:
		return OK;
	default:
		break;
	}
	lpbyData_WK += wAnalysisSize;
	if( lpbyData_WK >= lpbyData_END ) return OK;	 
	N_InfoTypeTblGetMember( wTag, IC_ACQUIRED_ADDRESS, &wICAttribute, lpwReturn );	 
	if( *lpwReturn != OK ){														 
		//N_Alarm( LOGIC_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS );		 
		return OK;																 
	}																			 
	if( *wICAttribute == IC ){														 
		F_InfoSet( wTag, lpbyData_WK, wVnLength, lpwReturn );
	}																				 
	stClearCommandList[wCommand].wTag[wClearCommandListCnt] = wTag;
	wClearCommandListCnt++;
	return OK;
}

//////////////////////////////////////////////////////////////////////////////
// FuncClass= Internal Function
// FuncName	= GetChallengeCommandAnalysis
// CallType	= Function
// Detail	=
// Valuable	= LPBYTE:lpbyData	:input
//			  WORD	:wDataSize	:input
//			  LPWORD:lpwReturn	:output
//				= OK
//				= NG
// Return	= OK
//////////////////////////////////////////////////////////////////////////////
long UPICD::N_GetChallengeCommandAnalysis( LPBYTE lpbyData, WORD wDataSize, LPWORD lpwReturn )
{
	WORD	wAnalysisSize = 0;
	LPBYTE	lpbyData_WK;
	LPBYTE	lpbyData_END;
	WORD	wTLSize = 0;
	WORD	*lpwFormatData[1];
	WORD	wVnLength = 0;
	BYTE	byBuff[256];
	WORD	wCommand = ICDPC_GET_CHALLENGE;
	WORD	&wClearCommandListCnt = stClearCommandList[wCommand].wCnt;

	*lpwReturn = OK;
	lpbyData_WK = lpbyData;
	lpbyData_END = lpbyData_WK + wDataSize;

	//unpredictable number buffer size  get 
	N_InfoTypeTblGetMember( ICDPC_IC_UNPREDICTABLE_NUMBER, IC_BUFFSIZE, lpwFormatData, lpwReturn );
	if( *lpwReturn != OK ){
		//N_Alarm( LOGIC_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS );
		return OK;
	}

	//make TLV in unpredictable number  
	N_SetTLV( ICDPC_IC_UNPREDICTABLE_NUMBER, **lpwFormatData, lpbyData_WK,				 
		lpbyData_END - lpbyData_WK, byBuff, &wDataSize, &wTLSize, lpwReturn );
	if( *lpwReturn != OK ){
		return OK;
	}

	//unpredictable number analysis
	N_TLVFormatAnalysis( ICDPC_IC_UNPREDICTABLE_NUMBER, OPTION, byBuff,					 
		wDataSize, &wAnalysisSize, &wVnLength, lpwReturn );
	if( *lpwReturn != OK ){
		//N_Alarm( FORMAT_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS );
		return OK;
	}

	//unpredictable number information set 
	F_InfoSet( ICDPC_IC_UNPREDICTABLE_NUMBER, lpbyData_WK, wVnLength, lpwReturn );		 
	stClearCommandList[wCommand].wTag[wClearCommandListCnt] = ICDPC_TM_UNPREDICTABLE_NUMBER;
	wClearCommandListCnt++;
	return OK;
}


////////////////////////////////////////////////////////////////////////////////
// FuncClass= Internal Function
// FuncName	= TLVFormatAnalysis
// CallType	= Function
// Detail	= TLV Format data Analysis
// Valuable	= LPBYTE:lpbyTag		:input
//			  WORD	:wActionOrder	:input
//				MANDATORY
//				OPTION
//			  LPBYTE:lpbyData		:input
//			  WORD	:wDataSize		:input
//			  LPWORD:lpwAnalisisSize:output
//			  LPWORD:lpwReturn		:output
//				OK
//				NG
//				FORMAT_ERR
// Return	= OK
////////////////////////////////////////////////////////////////////////////////
long UPICD::N_TLVFormatAnalysis( WORD wCheckTagData, WORD wActionOrder,
	LPBYTE lpbyData, WORD wDataSize, LPWORD lpwAnalisisSize,
	LPWORD lpwVnLength, LPWORD lpwReturn )
{
	WORD	wTag = 0;
	WORD	wTagSize = 0;
	LPBYTE	lpbyReadBufferPtr = lpbyData;
	LPBYTE	lpbyEndBufferPtr = lpbyData + wDataSize;
	WORD	*lpwInfoBuffer[IC_ALL_MEMBER];
	WORD	wLnLength = 0;
	WORD	wVnLength = 0;
	WORD	wAnalisisSize = 0;
	LPBYTE	lpbyJisDt;
	WORD	wEditSize = 0;
	BYTE	byPaddingCode = 0x00;
	WORD	wSuppressCount = 0;
	WORD	wSiftFlg = OFF;
	WORD	wSFIData = 0;				
	WORD	wStartRecord = 0;			
	WORD	wEndRecord = 0;				
	WORD	wSDACount = 0;				
	WORD	wDataBackup = 0;			
	WORD	wCVR1 = 0;					

	//initialize 
	*lpwAnalisisSize = 0;
	*lpwVnLength = 0;
	*lpwReturn = OK;

	//get tag information   
	N_GetTagInfo( lpbyReadBufferPtr, lpbyEndBufferPtr - lpbyReadBufferPtr,
		&wTag, &wTagSize, lpwReturn );
	switch( *lpwReturn ){
	case NG:
		return OK;
	case FORMAT_ERR:
		if(wActionOrder == OPTION){
			*lpwReturn = OK;
			return OK;
		}
		return OK;
	default:
		break;
	}

	//  input data acorrespondence match input tag 
	if( (wCheckTagData != NULL) && (wCheckTagData != wTag) ){
		if( wActionOrder == OPTION ){
			*lpwReturn = OK;
			return OK;
		}
		*lpwReturn = FORMAT_ERR;
		return OK;
	}

	// analyse size point move
	lpbyReadBufferPtr += wTagSize;

	// end when end point tag is initial value
	if( (lpbyEndBufferPtr == lpbyReadBufferPtr) && (wTag == 0) ){
		*lpwAnalisisSize = lpbyReadBufferPtr - lpbyData;
		return OK;
	}

	// LnVn length analyse
	N_GetLnVnLength( lpbyReadBufferPtr, lpbyEndBufferPtr - lpbyReadBufferPtr,
		&wLnLength, &wVnLength, &wAnalisisSize, lpwReturn );
	switch( *lpwReturn ){
	case NG:
		return OK;
	case FORMAT_ERR:
		return OK;
	default:
		break;
	}

	// analyse size point move
	lpbyReadBufferPtr += wAnalisisSize;

	//check Vn length and left size
	if( lpbyEndBufferPtr - lpbyReadBufferPtr < wVnLength ){			
		*lpwReturn = FORMAT_ERR;									
		return OK;													
	}																

	//return analysed size 
	*lpwAnalisisSize = lpbyReadBufferPtr - lpbyData;				
	//return Vsize 
	*lpwVnLength = wVnLength;										

	if( wVnLength == 0 ){			
		return OK;					
	}								

	//template
	switch( wTag ){
	case ICDPC_IC_RESPONSE_MESSAGE_TEMPLATE_FORMAT1:
	case ICDPC_IC_RESPONSE_MESSAGE_TEMPLATE_FORMAT2:
	case ICDPC_IC_TEMPLATE_70:
	case ICDPC_IC_FCI_TEMPLATE:
	case ICDPC_IC_FCI_PROPRIETARY_TEMPLATE:
	case ICDPC_IC_APPLICATION_TEMPLATE:
	case ICDPC_IC_FCI_ISSUER_DISCRETIONARY_DATA_EMV:	
		*lpwVnLength = wVnLength;						
		return OK;
	default:
		break;
	}

	//SFI Data Check(1 <= SFI Value <= 30)							
	if( wTag == ICDPC_IC_SFI ){										
		if( *lpbyReadBufferPtr < 1 || *lpbyReadBufferPtr > 30 ){	
			*lpwReturn = FORMAT_ERR;								
			return OK;												
		}															
	}																

	LPBYTE lpbyCheckEndBufferPtr = lpbyReadBufferPtr + wVnLength;

	//CVM_LIST format error check
	if( wTag == ICDPC_IC_CVM_LIST ){				
		wDataBackup = wVnLength;											
		for( WORD i = 8; wDataBackup > 8; i += 2 ){							
			wCVR1 = *(lpbyReadBufferPtr+i);									
			if( (wVnLength % 2 == 1 ) || ( wCVR1 >= 0x80 ) ){	
				*lpwReturn = FORMAT_ERR;				
				return OK;								
			}
			wDataBackup -= 2; 												
		}																	
	}

	//check AFL data
	if( wTag == ICDPC_IC_AFL ){												
		wDataBackup = wVnLength;											
		for( WORD i=0; wDataBackup >= 4; i += 4 ){							
			wSFIData = *(lpbyReadBufferPtr+i)/8;							
			wStartRecord = *(lpbyReadBufferPtr+i+1);						
			wEndRecord = *(lpbyReadBufferPtr+i+2);							
			wSDACount = *(lpbyReadBufferPtr+i+3);							
			if ((wStartRecord == 0 ) || (wStartRecord > wEndRecord) ||
				(wEndRecord - wStartRecord + 1 < wSDACount) || wSFIData == 0 || wSFIData > 30) {	
					*lpwReturn = FORMAT_ERR;							
					return OK;											
			}															
			wDataBackup -= 4;											
		}																
	}

	//IC_TDOL check
	if( wTag == ICDPC_IC_TDOL ){					
		ICTDOLFlg = TRUE;							
	}												

	//  code domain check
	//get domain information 
	N_InfoTypeTblGetMember( wTag, IC_ALL_MEMBER, lpwInfoBuffer, lpwReturn );
	if( *lpwReturn != OK ){
		return OK;
	}

	//set paddingCode code   
	switch( *(lpwInfoBuffer[IC_FORMAT]) ){
	case NUM:
	case AN:
	case ANS:
		byPaddingCode = 0x00;
		break;
	case CN:
		byPaddingCode = 0xFF;
		break;
	default:
		break;
	}

	switch( *(lpwInfoBuffer[IC_FORMAT]) ){
	case NUM:
		break;
	case AN:
	case ANS:
	case CN:
		//right padding code suppress
		wSuppressCount = N_SkipReverseByte( lpbyReadBufferPtr, byPaddingCode, wVnLength );
		if( wSuppressCount == SKIP_ALL ){
			lpbyCheckEndBufferPtr = lpbyReadBufferPtr;
		}else{
			lpbyCheckEndBufferPtr = lpbyReadBufferPtr + wSuppressCount + 1;
		}
		break;
	default:
		break;
	}

	lpbyJisDt = new BYTE[wVnLength*2];
	switch( *(lpwInfoBuffer[IC_FORMAT]) ){
	case NUM:
	case CN:
		if( *(lpwInfoBuffer[IC_PACK]) == PACK ){
			if( *(lpwInfoBuffer[IC_FORMAT]) == CN ){							   
				if( (*(lpbyReadBufferPtr + wSuppressCount-1) & 0x0F) == (byPaddingCode & 0x0F) ){
					wSiftFlg = ON;
				}
			}
			//contraction assign protraction
			N_BCDtoJIS( lpbyReadBufferPtr, lpbyCheckEndBufferPtr - lpbyReadBufferPtr,	 
				lpbyJisDt, wVnLength*2, &wEditSize, lpwReturn );
			if( *lpwReturn != OK ){
				delete[] lpbyJisDt;
				return OK;
			}
			lpbyReadBufferPtr = lpbyJisDt;
			lpbyCheckEndBufferPtr = lpbyJisDt + wEditSize;
		}
		if( wSiftFlg == ON ){
			if( *(lpwInfoBuffer[IC_FORMAT]) == CN ){							  
				lpbyCheckEndBufferPtr -= sizeof(BYTE);
			}
		}
		switch( *(lpwInfoBuffer[IC_ITEM]) ){
		case ICDPC_IC_APPLICATION_EXPIRATION_DATE:
		case ICDPC_IC_APPLICATION_EFFECTIVE_DATE:
			if( lpbyCheckEndBufferPtr - lpbyReadBufferPtr < 6 ){
				*lpwReturn = FORMAT_ERR;
				delete[] lpbyJisDt;
				return OK;
			}else{	//check time limit and effective begin date
				N_DatesCheck(lpbyReadBufferPtr, lpwReturn);	 
				if( *lpwReturn != OK ){						 
					delete[] lpbyJisDt;
					return OK;
				}											 
			}
			break;
		}
		break;
	default:
		break;
	}
	switch( *(lpwInfoBuffer[IC_FORMAT]) ){
	case AN:		// english num
		switch( wTag ){						 
		case ICDPC_IC_LANGUAGE_PREFERENCE:	 
			break;							 
		default:	
			// XiongJie start 对于不符合规范的ID号，这儿会做判断并返回格式错误，我们目前改为让上层判断
			//while( lpbyReadBufferPtr < lpbyCheckEndBufferPtr ){
			//	if( (wDataCheckMode == 0 && (memcmp( byANCode[*lpbyReadBufferPtr], "NG", strlen("NG")) == 0 )) &&	 
			//		(strlen((LPSTR)byANCode[*lpbyReadBufferPtr]) == strlen("NG"))||								 
			//		(wDataCheckMode == 1 && memcmp(byANSCode[*lpbyReadBufferPtr], "NG", strlen("NG")) == 0) && 	 
			//		(strlen((LPSTR)byANSCode[*lpbyReadBufferPtr]) == strlen("NG")) ){		 //Ndatacheck AnCode->AnsCode
			//			*lpwReturn = FORMAT_ERR;
			//			delete lpbyJisDt;
			//			return OK;
			//	}
			//	lpbyReadBufferPtr++;
			//}
			break;
		}									 
		break;
	case ANS:		//special characters
		switch( wTag ){						 
		case ICDPC_IC_APPLICATION_LABEL:	 
			while( lpbyReadBufferPtr < lpbyCheckEndBufferPtr ){								
				if( (memcmp(byANSCode[*lpbyReadBufferPtr], "NG", strlen("NG")) == 0 ) &&	
					(strlen((LPSTR)byANSCode[*lpbyReadBufferPtr]) == strlen("NG")) ){		
						*lpbyReadBufferPtr = 0x2A;											
				}																			
				lpbyReadBufferPtr++;														
			}																				
			break;							 
		case ICDPC_IC_AP_PREFERRED_NAME:	 
		case ICDPC_IC_CARDHOLDER_NAME:														
		case ICDPC_IC_CARDHOLDER_NAME_EXTENDED:												
			while( lpbyReadBufferPtr < lpbyCheckEndBufferPtr ){								
				if( *lpbyReadBufferPtr > 0x80 ){											
					if( (*lpbyReadBufferPtr == 0x81 && *(lpbyReadBufferPtr+1) < 0x40) ){	
						*lpbyReadBufferPtr = 0x2A;											
						*(lpbyReadBufferPtr+1) = 0x2A;										
					}																		
					lpbyReadBufferPtr += 2;													
					continue;																
				}																			
				if( (memcmp(byANSCode[*lpbyReadBufferPtr], "NG", strlen("NG")) == 0 ) &&	
					(strlen((LPSTR)byANSCode[*lpbyReadBufferPtr]) == strlen("NG")) ){		
						*lpbyReadBufferPtr = 0x2A;											
				}																			
				lpbyReadBufferPtr++;														
			}																				
			break;							
		default:							
			while( lpbyReadBufferPtr < lpbyCheckEndBufferPtr ){
				if( (memcmp(byANSCode[*lpbyReadBufferPtr], "NG", strlen("NG")) == 0 ) &&
					(strlen((LPSTR)byANSCode[*lpbyReadBufferPtr]) == strlen("NG")) ){
						*lpwReturn = FORMAT_ERR;
						delete[] lpbyJisDt;
						return OK;
				}
				lpbyReadBufferPtr++;
			}
		}									 
		break;
	case NUM:		// num
	case CN:		// Compressed num
		while( lpbyReadBufferPtr < lpbyCheckEndBufferPtr ){
			if( (memcmp( byNUMCode[*lpbyReadBufferPtr], "NG", strlen("NG")) == 0 ) &&
				(strlen((LPSTR)byNUMCode[*lpbyReadBufferPtr]) == strlen("NG")) ){
					*lpwReturn = FORMAT_ERR;
					delete[] lpbyJisDt;
					return OK;
			}
			lpbyReadBufferPtr++;
		}
		break;
	case CH:		// letter
		break;
	case BINARY:	// No binary
		while( lpbyReadBufferPtr < lpbyCheckEndBufferPtr ){
			lpbyReadBufferPtr++;
		}
		break;
	case UC:		// User code
	case VA:
		break;
	default:
		delete[] lpbyJisDt;
		return OK;
	}

	delete[] lpbyJisDt;

	if( *(lpwInfoBuffer[IC_BUFFSIZE]) < wVnLength ){
		*lpwReturn = FORMAT_ERR;
		return OK;
	}
	// The size of the input data is equal to or more than the minimum size
	if( *(lpwInfoBuffer[IC_MINISIZE]) > wVnLength){
		*lpwReturn = FORMAT_ERR;
		return OK;
	}
	return OK;
}

////////////////////////////////////////////////////////////////////////////////
// FuncClass= Internal Function
// FuncName	= TagDefineCheck
// CallType	= Function
// Detail	= Check Tag Define
// Valuable	= LPBYTE:lpwTag	:input
// Return	= TRUE
//			  FALSE
////////////////////////////////////////////////////////////////////////////////
BOOL UPICD::N_TagDefineCheck( LPBYTE lpbyData, WORD wBufferSize )
{
	WORD wTag = 0;
	WORD wTagSize = 0;
	WORD wReturn;

	N_GetTagInfo( lpbyData, wBufferSize, &wTag, &wTagSize, &wReturn );
	if( wReturn != OK ){
		return FALSE;
	}

	WORD wCnt_MAX = sizeof(wInfoType) / sizeof(wInfoType[0]);

	for( WORD wCnt=0; wCnt<wCnt_MAX; wCnt++ ){
		if( wInfoType[wCnt][IC_ITEM] == 0 ){
			return FALSE;
		}
		if( wInfoType[wCnt][IC_ITEM] == wTag ){
			return TRUE;
		}
	}
	return FALSE;
}


//////////////////////////////////////////////////////////////////////////////
// FuncClass= Internal Function
// FuncName	= GetTagInfo
// CallType	= Function
// Detail	= return input tag information
// Valuable	= LPBYTE:lpbyData		:input
//			  WORD	:wBufferSize	:input
//			  LPWORD:lpwTag			:output
//			  LPWORD:lpwAnalisisSize:output
//			  LPWORD:lpwReturn		:output
//				= OK
//				= NG
//				= FORMAT_ERR
// Return	= OK
//////////////////////////////////////////////////////////////////////////////
long UPICD::N_GetTagInfo( LPBYTE lpbyData, WORD wBufferSize,
	LPWORD lpwTag, LPWORD lpwAnalisisSize, LPWORD lpwReturn )
{
	BYTE	byWork[2];
	LPBYTE	lpbyEndBufferPtr = lpbyData+wBufferSize;

	*lpwAnalisisSize = 0;
	*lpwReturn = OK;
	*lpwTag = 0;

	if( lpbyData == NULL ){
		*lpwReturn = NG;
		return OK;
	}
	if( wBufferSize < 1 ){
		*lpwReturn = FORMAT_ERR;
		return OK;
	}

	// tag is 0x00 or 0xFF
	for( ; ((*lpbyData == 0xFF) || (*lpbyData == 0x00)) && (lpbyData < lpbyEndBufferPtr); ){
		(*lpwAnalisisSize)++;
		lpbyData += sizeof(BYTE);
	}
	if( lpbyData == lpbyEndBufferPtr ){
		return OK;
	}
	// tag size  get 
	if( (*(lpbyData) & 0x1f) == 0x1f ){
		// size 2 bite
		if( lpbyData + 1 > lpbyEndBufferPtr ){
			//No data in next bite
			*lpwReturn = FORMAT_ERR;
			return OK;
		}
		*lpwAnalisisSize += 2;
		byWork[0] = *(lpbyData+1);
		byWork[1] = *(lpbyData);
	}else{
		// size one bite
		(*lpwAnalisisSize)++;
		byWork[1] = 0x00;
		byWork[0] = *(lpbyData);
	}
	// WORD swich
	memcpy( lpwTag, byWork, sizeof(WORD) );
	return OK;
}


////////////////////////////////////////////////////////////////////////////////
// FuncClass= Internal Function
// FuncName	= InfoTypeTblGetMember
// CallType	= Function
// Detail	= Get Member in InfoType Tbl 
// Valuable	= WORD	:wTag		:input
//			= WORD	:wData		:input
//				IC_ITEM
//				IC_BUFFSIZE
//				IC_FORMAT
//				IC_PACK
//				IC_CLEARSIZE
//				IC_VALUE
//				IC_ACQUIRED_ADDRESS
//				IC_ALL_MEMBER
//			= LPWORD:lpwBuffer	:output
//			= LPWORD:lpwReturn	:output
//				OK
//				NG
// Return	=
////////////////////////////////////////////////////////////////////////////////
void UPICD::N_InfoTypeTblGetMember( WORD wTag, WORD wData, WORD **lpwBuffer, LPWORD lpwReturn )
{
	WORD wCnt2 = 0;

	*lpwReturn = OK;

	if( wData > IC_ALL_MEMBER ){
		*lpwReturn=NG;
		return;
	}

	//table num
	WORD wCnt_MAX = sizeof(wInfoType) / sizeof(wInfoType[0]);

	for( WORD wCnt=0; wCnt < wCnt_MAX; wCnt++ ){
		if( wInfoType[wCnt][IC_ITEM] == wTag ){
			if( wData == IC_ALL_MEMBER ){
				//get all Member
				for( wCnt2=0; wCnt2 < IC_ALL_MEMBER; wCnt2++ ){
					*(lpwBuffer + wCnt2) = &(wInfoType[wCnt][wCnt2]);
				}
				return;
			}else{
				//one item member require
				*lpwBuffer = &(wInfoType[wCnt][wData]);
				return;
			}
		}
	}
	*lpwReturn = NG;
}


//////////////////////////////////////////////////////////////////////////////
// FuncClass= Internal Function
// FuncName	= N_ErrorHandlingForFCIResponseData
// CallType	= Function
// Detail	=
// Valuable	= LPBYTE:lpbyData			:input
//			  WORD	:wDataSize			:input
//			  LPWORD:wAnalysisSize		:input
//			  LPWORD:wVnLength			:input
//			  LPWORD:lpwAnalysisSize	:output
//			  LPWORD:lpwVnLength		:output
//			  LPWORD:lpwReturn			:output
// Return	= OK
// 			= NG
// 			= FORMAT_ERR
//////////////////////////////////////////////////////////////////////////////
long UPICD::N_ErrorHandlingForFCIResponseData( LPBYTE lpbyData, WORD wDataSize,
	WORD wAnalysisSize, WORD wVnLength,
	LPWORD lpwAnalysisSize_FCI, LPWORD lpwVnLength_FCI, LPWORD lpwReturn )
{
	LPBYTE	lpbyData_WK;
	LPBYTE	lpbyData_END;
	WORD	wReturn;	 
	WORD	wReturn1;	 
	WORD	wTag = 0;
	WORD	wTagSize = 0;
	WORD	wCnt_MAX = sizeof(stInfoManagementTBL) / sizeof(stInfoManagementTBL[0]);

	*lpwAnalysisSize_FCI = 0;
	*lpwVnLength_FCI = 0;
	lpbyData_WK = lpbyData;
	lpbyData_END = lpbyData_WK + wDataSize;

	// Get Tag Information
	N_GetTagInfo( lpbyData_WK, lpbyData_END - lpbyData_WK, &wTag, &wTagSize, &wReturn );	 
	switch( wReturn ){																		 
	case NG:
		//N_Alarm( LOGIC_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS );
		return OK;
	case FORMAT_ERR:
		return OK;
	default:
		break;
	}

	switch( wTag ){
	case ICDPC_IC_APPLICATION_LABEL:
	case ICDPC_IC_LANGUAGE_PREFERENCE:
	case ICDPC_IC_ISSUER_CODE_TABLE_INDEX:
	case ICDPC_IC_AP_PREFERRED_NAME:
		// Check Length
		for( WORD wCnt=0; wCnt<wCnt_MAX; wCnt++ ){
			if( stInfoManagementTBL[wCnt].wItem == wTag ){
				if( stInfoManagementTBL[wCnt].wBuffSize < wVnLength ||
					stInfoManagementTBL[wCnt].wMiniSize > wVnLength ){
						*lpwReturn = FORMAT_ERR;
				}
			}
		}

		// Check Invalid Data Element
		lpbyData_WK += wAnalysisSize + wVnLength;
		if( *lpwReturn != OK ){
			*lpwAnalysisSize_FCI = wAnalysisSize;
			*lpwVnLength_FCI = lpbyData_WK - lpbyData - *lpwAnalysisSize_FCI;
		}
		// Omit Reduandant Trailing Data
		while( !N_TagDefineCheck(lpbyData_WK, lpbyData_END - lpbyData_WK) ){
			if( lpbyData_WK >= lpbyData_END ) break;
			lpbyData_WK++;
			*lpwAnalysisSize_FCI = wAnalysisSize;
			*lpwVnLength_FCI = lpbyData_WK - lpbyData - *lpwAnalysisSize_FCI;
		}
		*lpwReturn = OK;
		return OK;
	default:
		break;
	}
	// XiongJie start， SELECT命令中注释掉判断是否有存储过此标签的数据
	/*N_TagSetCheck( wTag, &wReturn1 );	 
	if( wReturn1 != OK ){				 
		*lpwReturn = FORMAT_ERR;		 
	}	*/	
	// XiongJie End
	return OK;
}

//////////////////////////////////////////////////////////////////////////////
// FuncClass= Internal Function
// FuncName	= SetTLVLength
// CallType	= Function
// Detail	= Set TLV Length
// Valuable	= WORD	:wLength	:input
//			  LPBYTE:lpbyBuff	:outout
//			  WORD	:wBuffSize	:inout
//			  LPWORD:lpwDataSize:outout
//			  LPWORD:lpwReturn	:outout
//				= OK
//				= FORMAT_ERR
// Return	= OK
//////////////////////////////////////////////////////////////////////////////
long UPICD::N_SetTLVLength( WORD wLength, LPBYTE lpbyBuff, WORD wBuffSize,
	LPWORD lpwDataSize, LPWORD lpwReturn )
{
	WORD wLength_WK=0;
	BYTE byDataSize;

	*lpwDataSize = 0;
	*lpwReturn = OK;

	//Less than 127bite 
	if( wLength <= 127 ){
		*lpbyBuff = BYTE(wLength);
		lpbyBuff++;
		(*lpwDataSize)++;
		return OK;
	}

	//More than 127bite 
	//Figure out necessary bite size
	wLength_WK = wLength;
	byDataSize = 1;
	while( wLength_WK >= 256 ){
		wLength_WK /= 256;
		byDataSize++;
	}

	//buffer short check 
	if( wBuffSize < (WORD)(byDataSize + 1) ){
		*lpwDataSize = 0;
		*lpwReturn = FORMAT_ERR;
		return OK;
	}

	//Save necessary bite size
	*lpbyBuff = byDataSize;
	*lpbyBuff |= 0x80;
	lpbyBuff++;
	(*lpwDataSize)++;

	//size save 
	memcpy( lpbyBuff, &wLength, byDataSize );
	lpbyBuff += byDataSize;
	*lpwDataSize += byDataSize;
	return OK;
}


////////////////////////////////////////////////////////////////////////////////
// FuncClass= Internal Function
// FuncName	= GetLnVnLength
// CallType	= Function
// Detail	= Get Length of Ln and Vn
// Valuable	= LPBYTE:lpbyData		:input
//			  WORD	:wDataSize		:input
//			  LPWORD:lpwLnLength	:output
//			  LPWORD:lpwVnLength	:output
//			  LPWORD:lpwAnalisisSize:output
//			  LPWORD:lpwReturn		:output
//				= OK
//				= NG
//				= FORMAT_ERR
// Return	= OK
////////////////////////////////////////////////////////////////////////////////
long UPICD::N_GetLnVnLength( LPBYTE lpbyData, WORD wDataSize, LPWORD lpwLnLength,
	LPWORD lpwVnLength, LPWORD lpwAnalisisSize, LPWORD lpwReturn )
{
	LPBYTE	lpbyReadBufferPtr = lpbyData;
	LPBYTE	lpbyEndBufferPtr = lpbyData+wDataSize;
	WORD	wAddLnLength = 0;
	BYTE	byAddLnLength[15];
	WORD	wNum = 0;
	WORD	i = 0;
	WORD	j = 0;

	*lpwAnalisisSize = 0;
	*lpwReturn = OK;

	if( lpbyData == NULL ){
		*lpwReturn = NG;
		return OK;
	}
	if( wDataSize < 1 ){
		*lpwReturn = FORMAT_ERR;
		return OK;
	}

	*lpwVnLength = 0;
	*lpwLnLength = 1;
	if( (*lpbyReadBufferPtr & 0x80) == 0 ){
		*lpwVnLength = *(lpbyReadBufferPtr) & 0x7F;
	}else{
		wAddLnLength = (*lpbyReadBufferPtr & 0x0F);
		if( lpbyReadBufferPtr + 1 > lpbyEndBufferPtr ){
			*lpwReturn = FORMAT_ERR;
			return OK;
		}
		lpbyReadBufferPtr++;
		for( i=0; i < wAddLnLength; i++ ){
			if( lpbyReadBufferPtr + 1 > lpbyEndBufferPtr ){
				*lpwReturn = FORMAT_ERR;
				return OK;
			}
			byAddLnLength[i] = *(lpbyReadBufferPtr) & 0xFF;
			lpbyReadBufferPtr++;
			(*lpwLnLength)++;
		}
		for( i=0; i < wAddLnLength; i++ ){
			if( (byAddLnLength[i] & 0xff) != 0 ){
				break;
			}
		}
		if( wAddLnLength - i > 2 ){
			*lpwReturn = FORMAT_ERR;
			return OK;
		}
		for( i=0; i < wAddLnLength; i++ ){
			wNum = 0;
			wNum = byAddLnLength[i];
			for( j = 0; j< wAddLnLength - i - 1; j++ ){
				wNum = wNum << 8;
			}
			if( *lpwVnLength + wNum > 65535 ){
				*lpwReturn = FORMAT_ERR;
				return OK;
			}
			*lpwVnLength += wNum;
		}
	}
	if( (*lpwLnLength + *lpwVnLength) > 65535 ){
		*lpwReturn = FORMAT_ERR;
		return OK;
	}
	*lpwAnalisisSize = *lpwLnLength;
	*lpwReturn = OK;
	return OK;
}

////////////////////////////////////////////////////////////////////////////////
// FuncClass= External Function
// FuncName	= SkipReverseByte
// CallType	= Function
// Detail	=
// Valuable	= LPBYTE :lpbyBuf	:input
//			  BYTE	:byFindCode	:input
//			  WORD	:wBufSize	:input
// Return	= n
////////////////////////////////////////////////////////////////////////////////
WORD UPICD::N_SkipReverseByte( LPBYTE lpbyBuf, BYTE bySkipCode, WORD wBufSize )
{
	WORD i;

	if( wBufSize != 0 ){
		for( i = wBufSize; i != 0; i-- ){
			if( *(lpbyBuf + (i - 1)) != bySkipCode ){
				return i - 1;
			}
		}
	}
	return SKIP_ALL;
}

////////////////////////////////////////////////////////////////////////////////
// FuncClass= External Function
// FuncName	= BCDtoJIS
// CallType	= Function
// Detail	= Compressed digital data into JIS data
// Valuable	= LPBYTE:lpbyInput		:input
//			  WORD	:wBcdSize		:input
//			  LPBYTE:lpbyOutput		:output
//			  WORD	:wJisSize		:input
//			  LPWORD:lpwJisSize		:output
//			  LPWORD:lpwReturn		:output
//				OK
//				NG
// Return	= OK
// Specially=
////////////////////////////////////////////////////////////////////////////////
long UPICD::N_BCDtoJIS( LPBYTE lpbyInput, WORD wBcdSize,
	LPBYTE lpbyOutput, WORD wJisSize, LPWORD lpwJisSize, LPWORD lpwReturn )
{
	WORD i = 0;
	WORD wEditSize = 0;

	if( lpwReturn == NULL ){
		return OK;
	}
	if( lpbyOutput == NULL ){
		*lpwReturn = NG;
		return OK;
	}

	*lpwJisSize = 0;
	*lpwReturn=OK;

	if( lpbyInput == NULL ){
		*lpwReturn = NG;
		return OK;
	}
	if( (wJisSize == 0) || (wBcdSize == 0) ){
		return OK;
	}
	if( wJisSize < wBcdSize * 2 ){
		*lpwReturn = NG;
		return OK;
	}

	if( wBcdSize != 0 ){
		wEditSize = wBcdSize * 2;
		for( i=0;i<wEditSize;i++ ){
			if( (i % 2) == 0 ){
				*(lpbyOutput + i) = *(lpbyInput + (i / 2)) >> 4;
			}else {
				*(lpbyOutput + i) = *(lpbyInput + (i / 2)) & 0x0f;
			}
			if( *(lpbyOutput + i) <= 9 ){
				*(lpbyOutput + i) |= '0';
			}else {
				*(lpbyOutput + i) += ('A'-10);
			}
		}
	}
	*lpwJisSize = i;
	return OK;
}

////////////////////////////////////////////////////////////////////////////////
// FuncClass= Internal Function
// FuncName	= DatesCheck
// CallType	= Function
// Detail	= check time limit and effective begin date
// Valuable	= LPBYTE:lpbyReadBufferPtr	:input
//			  LPWORD:lpwReturn			:output
//				OK
//				FORMAT_ERR
// Return	=
////////////////////////////////////////////////////////////////////////////////
void UPICD::N_DatesCheck(LPBYTE lpbyReadBufferPtr, LPWORD lpwReturn)
{
	WORD	wYear = 0, wMonth = 0, wDay = 0;
	BYTE	byYear[2];
	BYTE	byMonth[2];
	BYTE	byDay[2];

	*lpwReturn = OK;

	memcpy( byYear, lpbyReadBufferPtr, 2 );
	memcpy( byMonth, lpbyReadBufferPtr+2, 2 );
	memcpy( byDay, lpbyReadBufferPtr+4, 2 );
	wYear = N_atoi( byYear, 2 );
	wMonth = N_atoi( byMonth, 2 );
	wDay = N_atoi( byDay, 2 );

	switch (wMonth){
	case 1:
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
	case 12:
		if(wDay < 1 || wDay > 31){
			*lpwReturn = FORMAT_ERR;
			return;
		}
		break;
	case 2:
		if(wDay < 1 || wDay > 29){
			*lpwReturn = FORMAT_ERR;
			return;
		}else if(wYear % 4 != 0){		 
			if(wDay > 28){
				*lpwReturn = FORMAT_ERR;
				return;
			}
		}
		break;
	case 4:
	case 6:
	case 9:
	case 11:
		if(wDay < 1 || wDay > 30){
			*lpwReturn = FORMAT_ERR;
			return;
		}
		break;
	default:
		*lpwReturn = FORMAT_ERR;
		return;
	}
	return;
}

//////////////////////////////////////////////////////////////////////////////
// FuncClass= Internal Function
// FuncName	= ASCtoINT
// CallType	= Function
// Detail	=
// Valuable	= LPBYTE:lpbyData	:input
//			  WORD	:wDataSize	:input
// Return	= n : changed value
//////////////////////////////////////////////////////////////////////////////
int UPICD::N_atoi( LPBYTE lpbyData, WORD wDataSize )
{
	BYTE byData[100];
    WORD ibySize = sizeof(byData);
    int iLen = std::min(wDataSize, ibySize);
    memcpy( byData, lpbyData,iLen);
	if( wDataSize < sizeof(byData) ){
		*(byData + wDataSize) = NULL;
	}
	return atoi( (LPSTR)byData );
}

////////////////////////////////////////////////////////////////////////////////
// FuncClass= Internal Function
// FuncName	= TagSetCheck
// CallType	= Function
// Detail	= Tag Set Check
// Valuable	= WORD	:wTag			:input
//			  WORD	:wDataSize		:input
//			  LPWORD:lpwReturn		:output
//				OK
//				NG
//				FORMAT_ERR
// Return	=
////////////////////////////////////////////////////////////////////////////////
void UPICD::N_TagSetCheck( WORD wTag, LPWORD lpwReturn )
{
	*lpwReturn = OK;

	// table num
	WORD wCnt_MAX = sizeof(stInfoManagementTBL) / sizeof(stInfoManagementTBL[0]);

	for( WORD wCnt=0; wCnt < wCnt_MAX; wCnt++ ){
		if( stInfoManagementTBL[wCnt].wItem == wTag ){
			if( stInfoManagementTBL[wCnt].bTagReadFlg == TRUE ){
				//*lpwReturn = FORMAT_ERR;					//Modify By LeeHai,第二次校验不通过
				return;
			}else{
				stInfoManagementTBL[wCnt].bTagReadFlg = TRUE;
				return;
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// FuncClass= External Function
// FuncName	= InfoSet
// CallType	= Function
// Detail	= Specified numerical value set
// Valuable	= WORD	:wItem		:input
//			  LPBYTE:lpbyBuffer	:input
//			  WORD	:wDataSize	:input
//			  LPWORD:lpwReturn	:output
//				OK
//				NG
// Return	= OK
// Specially=
////////////////////////////////////////////////////////////////////////////////
long UPICD::F_InfoSet( WORD wItem, LPBYTE lpbyBuffer, WORD wDataSize, LPWORD lpwReturn )
{
	if( lpwReturn == NULL ){
		//N_Alarm( PARAM_ERR, APMSG_ICDPC_INFOSET );
		return OK;
	}
	if( lpbyBuffer == NULL ){
		*lpwReturn=NG;
		//N_Alarm( PARAM_ERR, APMSG_ICDPC_INFOSET );
		return OK;
	}

	//HostIssuerData                                                             
	if( wItem == ICDPC_HOSTISSUERDATA){ 							         
		N_HostIssuerDataAnalysis(lpbyBuffer,wDataSize,lpwReturn);		         
		return OK;													         
	}																         

	if( wItem == ICDPC_SDA_VERIFYDATA){ 										 
        strSDAVerifyData = US2Q(lpbyBuffer);//QString((char*)lpbyBuffer, wDataSize);
		*lpwReturn = OK;														 
		return OK;																 
	}																			 
	*lpwReturn = OK;

	WORD *lpwInfoBuffer[IC_ALL_MEMBER];

	WORD wCnt_MAX = sizeof(stInfoManagementTBL) / sizeof(stInfoManagementTBL[0]);

	N_InfoTypeTblGetMember( wItem, IC_ALL_MEMBER, lpwInfoBuffer, lpwReturn );
	if( *lpwReturn != OK ){
		//N_Alarm( LOGIC_ERR, APMSG_ICDPC_INFOSET );
		return OK;
	}
	if (0 == wDataSize)															 
	{                       													 
		*lpwReturn = OK ; 														 
		return OK;																 
	} 																			 

	for( WORD wCnt=0; wCnt<wCnt_MAX; wCnt++ ){
		if( stInfoManagementTBL[wCnt].wItem == wItem ){
			// The size of the input data buffer size is less than or equal to
			if( stInfoManagementTBL[wCnt].wBuffSize < wDataSize ||
				stInfoManagementTBL[wCnt].wMiniSize > wDataSize ){
					//N_Alarm( BUFFER_ERR, APMSG_ICDPC_INFOSET );
					*lpwReturn = NG;
					return OK;
			}
			memcpy( stInfoManagementTBL[wCnt].lpbyBuffer, lpbyBuffer, wDataSize );
			stInfoManagementTBL[wCnt].wDataSize = wDataSize;
			return OK;
		}
	}
	*lpwReturn = NG;
	//N_Alarm( LOGIC_ERR, APMSG_ICDPC_INFOSET );
	return OK;
}

//////////////////////////////////////////////////////////////////////////////
// FuncClass= Internal Function
// FuncName	= GetBalanceCommandAnalysis
// CallType	= Function
// Detail	=
// Valuable	= LPBYTE:lpbyData	:input
//			  WORD	:wDataSize	:input
//			  LPWORD:lpwReturn	:output
// Return	= OK
//////////////////////////////////////////////////////////////////////////////
long UPICD::N_HostIssuerDataAnalysis( LPBYTE lpbyData, WORD wDataSize, LPWORD lpwReturn )
{
	LPBYTE	lpbyData_WK,lpbyData_END;
	WORD	wCommand = ICDPC_HOST_SCRIPT;
	WORD	&wClearCommandListCnt = stClearCommandList[wCommand].wCnt;
	WORD	wAnalysisSize = 0;
	WORD	wVnLength = 0;
	WORD	wTag = 0;
	WORD	wTagSize = 0;
	WORD	*wICAttribute = 0;													 
	*lpwReturn = OK;
	lpbyData_WK = lpbyData;
	lpbyData_END = lpbyData_WK + wDataSize;

	for( WORD i = 0; i < wClearCommandListCnt; i++ ){							 
		F_InfoItemClear( stClearCommandList[wCommand].wTag[i], lpwReturn );		 
		if( *lpwReturn != OK ){													 
			return OK;															 
		}																		 
	}																			 
	N_ClearCommandListClear(wCommand);

	while( lpbyData_WK < lpbyData_END ){
		N_TLVFormatAnalysis( NULL, OPTION, lpbyData_WK, lpbyData_END - lpbyData_WK,
			&wAnalysisSize, &wVnLength, lpwReturn );
		if( wAnalysisSize == 0 ){					
			return OK;								
		}											
		if( !N_TagDefineCheck(lpbyData_WK, lpbyData_END - lpbyData_WK) ){
			*lpwReturn = OK;						
			lpbyData_WK += wAnalysisSize + wDataSize;	
			continue;									
		}																
		if( *lpwReturn != OK ){
			return OK;													
		}
		//tag information get 
		N_GetTagInfo( lpbyData_WK, lpbyData_END - lpbyData_WK, &wTag, &wTagSize, lpwReturn );
		switch( *lpwReturn ){
		case NG:
			//N_Alarm( LOGIC_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS );
			return OK;
		case FORMAT_ERR:
			return OK;
		default:
			break;
		}
		lpbyData_WK += wAnalysisSize;
		if( lpbyData_WK >= lpbyData_END ) break;	
		N_InfoTypeTblGetMember( wTag, IC_ACQUIRED_ADDRESS, &wICAttribute, lpwReturn ); 
		if( *lpwReturn != OK ){													 
			//N_Alarm( LOGIC_ERR, APMSG_ICDPC_ICRESPONSEMESSAGEDATAANALYSIS );	 
			return OK;															 
		}																		 
		if( *wICAttribute == ISSUER ){											 
			F_InfoSet( wTag, lpbyData_WK, wVnLength, lpwReturn );
			if( *lpwReturn != OK ){
				return OK;
			}
			stClearCommandList[wCommand].wTag[wClearCommandListCnt] = wTag;							 
			wClearCommandListCnt++;																	 
		}																		 
		lpbyData_WK += wVnLength;
	}

	return OK;
}


////////////////////////////////////////////////////////////////////////////////
// FuncClass= External Function
// FuncName	= HEXtoASC
// CallType	= Function
// Detail	= switch ＨＥＸ to ＡＳＣＩＩ
// Valuable	= LPBYTE:lpDest		:output
//			  LPBYTE:lpSrc		:input
//			  WORD	:wSize		:input
//			  LPWORD:lpwReturn	:output
//				OK
// Return	=
////////////////////////////////////////////////////////////////////////////////
void UPICD::N_HEXtoASC(LPBYTE lpDest, LPBYTE lpSrc, WORD wSize, LPWORD lpwReturn)
{
	char Number[]="0123456789ABCDEF";

	*lpwReturn = OK;

	for( int i = 0; i < wSize; i++ )
	{
		*lpDest++ = Number[lpSrc[i] >> 4];
		*lpDest++ = Number[lpSrc[i] & 0x0f];
	}
}

////////////////////////////////////////////////////////////////////////////////
// FuncClass= External Function
// FuncName	= InfoItemClear
// CallType	= Function
// Detail	= clear the data value
// Valuable	= WORD	:wItem		:input
//			  LPWORD:lpwReturn	:output
//				OK
// Return	= OK
////////////////////////////////////////////////////////////////////////////////
long UPICD::F_InfoItemClear( WORD wItem, LPWORD lpwReturn )
{
	WORD *lpwInfoBuffer[IC_ALL_MEMBER];
	WORD wMNameSize;												 
	WORD wMLocationSize;											 
	BYTE byMNameBuf[4];												 
	BYTE byMLocationBuf[11];										 
	BYTE byMNameAndLocation[252];									 
	wMNameSize = 0;													 
	wMLocationSize = 0;												 


	if( lpwReturn == NULL ){
		//N_Alarm( PARAM_ERR, APMSG_ICDPC_INFOITEMCLEAR );
		return NG;
	}

	*lpwReturn = OK;

	/*
	// Clear item is ICParameter
	if( wItem == ICDPC_ICPARAMETER ){                                            
		N_ICParameterFileRead(lpwReturn);                                        
		return OK;                                                               
	}                                                                            

	// Clear item is ICParameter
	if( wItem == ICDPC_ICTEMINALACTIONCODE ){                                    
		N_ICTerminalActionCodeFileRead(lpwReturn);                               
		return OK;                                                               
	}                                                                            

	//Clear item is application of management list
	if( wItem == ICDPC_APPLICATION_LIST ){										 
		N_APManagementListClear();
		return OK;
	}


	//Clear item is Information management table			 
	if( wItem == ICDPC_INFO_MANAGEMENT_TBL ){	 
		N_InfoManagementTBLClear();				 
		return OK;								 
	}											 
	*/


	// Management table size get
	WORD wCnt_MAX = sizeof(wInfoType) / sizeof(wInfoType[0]);
	// Management table size
	for( WORD wCnt = 0; wCnt < wCnt_MAX; wCnt++ ){
		N_InfoTypeTblGetMember( wInfoType[wCnt][IC_ITEM], IC_ALL_MEMBER, lpwInfoBuffer, lpwReturn );
		if( *lpwReturn != OK ){
			//N_Alarm( LOGIC_ERR, APMSG_ICDPC_INFOITEMCLEAR );
			return OK;
		}
		// Search management table enter item
		if( *(lpwInfoBuffer[IC_ITEM]) == wItem ){
			// The data size is fixed?
			if( *(lpwInfoBuffer[IC_CLEARSIZE]) == CONSTANT ){
				//The initial value setting function call
				if( N_InitDataSet( *(lpwInfoBuffer[IC_ITEM]), stInfoManagementTBL[wCnt].lpbyBuffer,
					*(lpwInfoBuffer[IC_BUFFSIZE]), *(lpwInfoBuffer[IC_VALUE]) ) ){
						stInfoManagementTBL[wCnt].wDataSize = *(lpwInfoBuffer[IC_MINISIZE]);
				}else{
					stInfoManagementTBL[wCnt].wDataSize = 0;
				}
			}else{
				// The data size is variable
				*(stInfoManagementTBL[wCnt].lpbyBuffer) = NULL;
				stInfoManagementTBL[wCnt].wDataSize = 0;
				if( *(lpwInfoBuffer[IC_ITEM]) == ICDPC_IC_TDOL ){																 
					ICTDOLFlg = FALSE;																							 
					if( strlen(stSettingInfo.szTDOL) > 0 ){																		 
						memcpy( stInfoManagementTBL[wCnt].lpbyBuffer, &(stSettingInfo.szTDOL), strlen(stSettingInfo.szTDOL) );	 
						stInfoManagementTBL[wCnt].wDataSize = strlen(stSettingInfo.szTDOL);										 
						TDOLFlg = TRUE;																							 
					}else{																										 
						TDOLFlg = FALSE;																						 
					}																											 
				}																												 
				if( *(lpwInfoBuffer[IC_ITEM])  == ICDPC_TM_MERCHANT_NAME_AND_LOCATION ){										 
					//tlsUSKOC.F_InitialInfoGet( USKOC_PERIOD_CD, byMNameBuf, sizeof(byMNameBuf), &wMNameSize, lpwReturn );									 
					if (*lpwReturn != OK){																						 
						//N_Alarm( BUFFER_ERR, APMSG_ICDPC_CLEAR );																 
						return OK;																								 
					}																											 
					//tlsUSKOC.F_InitialInfoGet( USKOC_BRANCHNUMBER, byMLocationBuf, sizeof(byMLocationBuf), &wMLocationSize, lpwReturn );							 
					if (*lpwReturn != OK){																						 
						//N_Alarm( BUFFER_ERR, APMSG_ICDPC_CLEAR );																 
						return OK;																								 
					}																											 
					memcpy( byMNameAndLocation, byMNameBuf, wMNameSize );														 
					memset( byMNameAndLocation + wMNameSize, ' ', 1 );															 
					memcpy( byMNameAndLocation + wMNameSize + 1, byMLocationBuf, wMLocationSize );								 
					memcpy( byMNameAndLocation + wMNameSize + 1 + wMLocationSize, "0000", 4 );									 
					stInfoManagementTBL[wCnt].wDataSize = wMNameSize + wMLocationSize + 5;										  
					memcpy( stInfoManagementTBL[wCnt].lpbyBuffer, byMNameAndLocation, stInfoManagementTBL[wCnt].wDataSize );	 
				}																												 

			}
			return OK;
		}
	}

	// clear item not exist
	//N_Alarm( INTERFACE_ERR, APMSG_ICDPC_INFOITEMCLEAR );
	*lpwReturn = NG;
	return OK;
}

////////////////////////////////////////////////////////////////////////////////
// FuncClass= Internal Function
// FuncName	= InitDataSet
// CallType	= Function
// Detail	= InitData Set
// Valuable	= WORD	:wItem			:input
//			  LPBYTE:lpbyDataBuffer	:output
//			  WORD	:wBufferSize	:input
//			  WORD	:wValue			:input
// Return	= TRUE
//			  FALSE
// Specially=
////////////////////////////////////////////////////////////////////////////////
BOOL UPICD::N_InitDataSet( WORD wItem, LPBYTE lpbyDataBuffer, WORD wBufferSize, WORD wValue )
{
	WORD wReturn;													   
	WORD wSize;														   
	BYTE byBuf[15];													   
	wSize = 0;														   

	//initialize 
	switch( wItem ){
	case ICDPC_TM_TERMINAL_CAPABILITIES:
		memcpy( lpbyDataBuffer, &stTerminalCapabilities, wBufferSize );
		break;
	case ICDPC_TM_ADDITIONAL_TERMINAL_CAPABILITIES:						 
		memcpy( lpbyDataBuffer, &stAdditionalTerminalCapabilities, wBufferSize );	 
		break;															 
	case ICDPC_TM_ISSUER_SCRIPT_RESULT:									 
		memcpy( lpbyDataBuffer, &stIssuerScriptResult, wBufferSize );	 
		break;															 
	case ICDPC_TM_TAC_DENIAL:
		memcpy( lpbyDataBuffer, &stTAC_PROGRESS_DENIAL, wBufferSize );
		break;
	case ICDPC_TM_TAC_ONLINE:
		memcpy( lpbyDataBuffer, &stTAC_PROGRESS_ONLINE, wBufferSize );
		break;
	case ICDPC_TM_TAC_DEFAULT:
		memcpy( lpbyDataBuffer, &stTAC_PROGRESS_DEFAULT, wBufferSize );
		break;
	case ICDPC_TM_PSE:
		memcpy( lpbyDataBuffer, "1PAY.SYS.DDF01", wBufferSize );
		break;
	case ICDPC_TM_ISSUER_ID:
		if( stSettingInfo.bIssuerIDFlg == false ) return FALSE;			 
		memcpy( lpbyDataBuffer, &(stSettingInfo.szIssuerID), wBufferSize );
		break;
	case ICDPC_TM_KEY_INDEX:
		if( stSettingInfo.bKeyIndexFlg == false ) return FALSE;			 
		memcpy( lpbyDataBuffer, &(stSettingInfo.szKeyIndex), wBufferSize );
		break;
	case ICDPC_TM_PME:
		if( stSettingInfo.bPMEFlg == false ) return FALSE;				 
		memcpy( lpbyDataBuffer, &(stSettingInfo.szPME), wBufferSize );
		break;
	case ICDPC_TM_IFD_SERIAL_NO:										 
		if( stSettingInfo.bIFDFlg == false ) return FALSE;				 
		memcpy( lpbyDataBuffer, &(stSettingInfo.szIFD), wBufferSize );	 
		break;															 
	case ICDPC_TM_TVR:													 
		memcpy( lpbyDataBuffer, "\x00\x00\x00\x00\x00", wBufferSize );	 
		break;															 
	case ICDPC_TM_TSI:													 
		memcpy( lpbyDataBuffer, "\x00\x00", wBufferSize );				 
		break;															 
	case ICDPC_TM_MERCHANT_ID:											 
		if( stSettingInfo.bMerIDFlg == false ) return FALSE;			 
		memcpy( lpbyDataBuffer, &(stSettingInfo.szMerID), wBufferSize ); 
		break;															 
	case ICDPC_TM_TERMINAL_COUNTRY_CODE:								 
	case ICDPC_TM_TRANSACTION_CURRENCY_CODE:							 
	case ICDPC_TM_TRANSACTION_REFERENCE_CURRENCY_CODE:					 
		if( stSettingInfo.bCtyCodeFlg == false ) return FALSE;			 
		memcpy( lpbyDataBuffer, &(stSettingInfo.szCtyCode), wBufferSize ); 
		break;															 
	case ICDPC_TM_MERCHANT_CATEGORY_CODE:								 
		if( stSettingInfo.bMerCCodeFlg == false ) return FALSE;			 
		memcpy( lpbyDataBuffer, &(stSettingInfo.szMerCCode), wBufferSize ); 
		break;															 
	case ICDPC_TM_POS_ENTRY_MODE:										 
		if( stSettingInfo.bPOSEModeFlg == false ) return FALSE;			 
		memcpy( lpbyDataBuffer, &(stSettingInfo.szPOSEMode), wBufferSize ); 
		break;															 
	case ICDPC_TM_TERMINAL_ID:											 
		//tlsUSKOC.F_InitialInfoGet( USKOC_MACHINENUMBER, byBuf, sizeof(byBuf), &wSize, &wReturn );				 
		if (wReturn != OK){												 
			//N_Alarm( BUFFER_ERR, APMSG_ICDPC_INIT );					 
			return FALSE;												   
		}																 
		if(0== TM_Terminal_Mode){										 
			memcpy( lpbyDataBuffer, byBuf, 8 );							 
		}else{															 
			memcpy( lpbyDataBuffer, byBuf + wSize - wBufferSize, wBufferSize );  
		}																 
		break;															 																//CPB33004(PG#00008)
	case ICDPC_TM_SM_ALGORITHM:											 
		if( wSmAlgMode == 0 ){											 
			memset(lpbyDataBuffer, 0x00,wBufferSize);					 
		}else{															 
			memset(lpbyDataBuffer, 0x00,wBufferSize-1);					 
			memcpy(lpbyDataBuffer+wBufferSize-1,"\x01",1);				 
		}																 
		break;															 
	case ICDPC_TM_ACQUIRER_ID:													 
		if( stSettingInfo.bAcquirerIdFlg == false ) return FALSE;				 
		memcpy( lpbyDataBuffer, &(stSettingInfo.szAcquirerId), wBufferSize );	 
		break;																	 
	case ICDPC_TM_USE_ALGORITHM:												 
		memset( lpbyDataBuffer, 0x00, wBufferSize );							 
		break;																	 
	default:
		memset( lpbyDataBuffer, wValue, wBufferSize );
		if( wValue == 0 ){												 
			return FALSE;												 
		}																 
	}
	return TRUE;														 
}

////////////////////////////////////////////////////////////////////////////////
// FuncClass= Internal Function
// FuncName	= APManagementListClear
// CallType	= Function
// Detail	= Clear stClearCommandList
// Valuable	= WORD	:wCommand	:input
// Return	=
////////////////////////////////////////////////////////////////////////////////
void UPICD::N_ClearCommandListClear( WORD wCommand )
{
	WORD wCnt_MAX = sizeof(stClearCommandList[wCommand].wTag) / sizeof(stClearCommandList[wCommand].wTag[0]);

	for( WORD wCnt=0; wCnt < wCnt_MAX; wCnt++ ){
		stClearCommandList[wCommand].wTag[wCnt] = 0;
	}
	stClearCommandList[wCommand].wCnt = 0;
}


//////////////////////////////////////////////////////////////////////////////
// FuncClass= Internal Function
// FuncName	= SetTLV
// CallType	= Function
// Detail	= Save TLV type ic information to buffer
// Valuable	= WORD	:wTag		:input
//			  WORD	:wLength	:input
//			  LPBYTE:lpbyValue	:input
//			  WORD	:wBuffSize	:input
//			  LPBYTE:lpbyBuff	:outout
//			  LPWORD:lpwDataSize:outout
//			  LPWORD:lpwTLSize	:outout
//			  LPWORD:lpwReturn	:outout
//				= OK
//				= NG
//				= FORMAT_ERR
// Return	= OK
//////////////////////////////////////////////////////////////////////////////
long UPICD::N_SetTLV( WORD wTag, WORD wLength, LPBYTE lpbyValue,
	WORD wBuffSize, LPBYTE lpbyBuff, LPWORD lpwDataSize,
	LPWORD lpwTLSize, LPWORD lpwReturn )
{
	WORD wDataSize = 0;

	*lpwDataSize = 0;
	*lpwTLSize = 0;
	*lpwReturn = OK;

	//tag (T)use possible buffer is igger than size of WORD
	if( wBuffSize > sizeof(WORD) ){
		//tag (T)save 
		N_SetTag( wTag, lpbyBuff, sizeof(WORD), &wDataSize, lpwReturn );
		if( *lpwReturn != OK ){
			*lpwDataSize = 0;
			return OK;
		}
	}else{
		//tag (T)save 
		N_SetTag( wTag, lpbyBuff, wBuffSize, &wDataSize, lpwReturn );
		if( *lpwReturn != OK ){
			*lpwDataSize = 0;
			return OK;
		}
	}
	*lpwDataSize = *lpwDataSize + wDataSize;

	//length (L) use possible buffer is igger than size of WORD
	if( wBuffSize - *lpwDataSize > sizeof(WORD) ){
		//length (L)save 
		N_SetTLVLength( wLength, lpbyBuff + *lpwDataSize, sizeof(WORD), &wDataSize, lpwReturn );
		if( *lpwReturn != OK ){
			*lpwDataSize = 0;
			return OK;	//Restriction of the size OF WORD ,buffer short is NG
		}
	}else{
		//length (L)save 
		N_SetTLVLength( wLength, lpbyBuff + *lpwDataSize, wBuffSize - *lpwDataSize, &wDataSize, lpwReturn );
		if( *lpwReturn != OK ){
			*lpwDataSize = 0;
			return OK;
		}
	}
	*lpwDataSize += wDataSize;
	// TLsize 
	*lpwTLSize = *lpwDataSize;

	//buffer short check 
	if( wLength > wBuffSize ){
		*lpwDataSize = 0;
		*lpwReturn = FORMAT_ERR;
		return OK;
	}

	//value (V)save 
	memcpy( lpbyBuff + *lpwDataSize, lpbyValue, wLength );
	*lpwDataSize += wLength;
	return OK;
}

//////////////////////////////////////////////////////////////////////////////
// FuncClass= Internal Function
// FuncName	= SetTag
// CallType	= Function
// Detail	= Set Tag information
// Valuable	= WORD	:wTag		:input
//			  LPBYTE:lpbyBuff	:outout
//			  WORD	:wBuffSize	:inout
//			  LPWORD:lpwDataSize:outout
// Return	= OK
//////////////////////////////////////////////////////////////////////////////
long UPICD::N_SetTag( WORD wTag, LPBYTE lpbyBuff, WORD wBuffSize,
	LPWORD lpwDataSize, LPWORD lpwReturn )
{
	WORD wGetTagSize=0;
	BYTE byTag[2];

	*lpwDataSize = 0;
	*lpwReturn = OK;

	//tag size get 
	N_TagSizeGet( wTag, &wGetTagSize );

	//buffer short check 
	if( wBuffSize < wGetTagSize ){
		*lpwDataSize = 0;
		*lpwReturn = FORMAT_ERR;
		return OK;
	}

	//tag set 
	if( wGetTagSize == 2 ){
		// size 2bite 
		byTag[0] = HIBYTE(wTag);
		byTag[1] = LOBYTE(wTag);
	}else{
		// size 1 bite 
		byTag[0] = LOBYTE(wTag);
	}
	memcpy( lpbyBuff, byTag, wGetTagSize );
	lpbyBuff += wGetTagSize;
	*lpwDataSize += wGetTagSize;
	return OK;
}

//////////////////////////////////////////////////////////////////////////////
// FuncClass= Internal Function
// FuncName	= TagSizeGet
// CallType	= Function
// Detail	= Ta gSize Get
// Valuable	= WORD	:wTag	:input
//			  LPBYTE:byBit	:input
// Return	=
//////////////////////////////////////////////////////////////////////////////
void UPICD::N_TagSizeGet(WORD wTag, LPWORD lpwTagSize)
{
	if( (wTag & 0x1F00) == 0x1F00 ){
		*lpwTagSize = 2;
	}else{
		*lpwTagSize = 1;
	}
}

////////////////////////////////////////////////////////////////////////////////
// FuncClass= Internal Function
// FuncName	= DOLTagCheck
// CallType	= Function
// Detail	= check DOL tag
// Valuable	= WORD	:wtag		:input
// Return	= TRUE
//			  FALSE
////////////////////////////////////////////////////////////////////////////////
BOOL UPICD::N_DOLTagCheck(WORD wItem)
{
	BYTE	byDOL[252];
	WORD	wDOLSize=0;
	WORD	wReturn;
	BYTE	byDOLRelationData[252];
	BYTE	byDOLRelationDataBuf[252];
	LPBYTE	lpbyDOL_WK;
	LPBYTE	lpbyDOL_END;
	LPBYTE	lpbyDOLRelationDataPtr;
	LPBYTE	lpbyDOL_BK;
	WORD	wTagSize = 0;
	WORD	wLnLength = 0;
	WORD	wVnLength = 0;
	WORD	wAnalisisSize = 0;
	WORD	wTag = 0;

	F_InfoGet( wItem, byDOL, 252, &wDOLSize, &wReturn );
	if( wReturn == NG ){
		return FALSE;
	}

	//set DOL tag list front point to buffer
	lpbyDOL_WK = byDOL;
	//set end point to buffer
	lpbyDOL_END = byDOL + wDOLSize;
	// set DOL related  data start point 
	lpbyDOLRelationDataPtr = byDOLRelationData;

	// DOLtag list data size
	while( lpbyDOL_WK < lpbyDOL_END ){
		lpbyDOL_BK = lpbyDOL_WK;
		// work buffer initialize 
		memset( byDOLRelationDataBuf, 0x00, 252 );
		// get tag information   
		N_GetTagInfo( lpbyDOL_WK, lpbyDOL_END - lpbyDOL_WK, &wTag, &wTagSize, &wReturn );
		switch( wReturn ){
		case NG:
		case FORMAT_ERR:
			return FALSE;
		default:
			break;
		}
		if (wTag == ICDPC_TM_TC_HASH_VALUE){
			return TRUE;
		}
		lpbyDOL_WK += wTagSize;
		// break when end point tag is initial value 
		if( (lpbyDOL_WK == lpbyDOL_END) && (wTag == 0) ){
			break;
		}
		//one byte length analyse
		N_GetOneByteLength( lpbyDOL_WK, lpbyDOL_END - lpbyDOL_WK, &wLnLength,
			&wVnLength, &wAnalisisSize, &wReturn );
		switch( wReturn ){
		case NG:
		case FORMAT_ERR:
			return FALSE;
		default:
			break;
		}
		lpbyDOL_WK += wLnLength;
	}
	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
// FuncClass= Internal Function
// FuncName	= GetOneByteLength
// CallType	= Function
// Detail	= Get oneByte Length
// Valuable	= LPBYTE:lpbyData		:input
//			  WORD	:wDataSize		:input
//			  LPWORD:lpwLnLength	:output
//			  LPWORD:lpwVnLength	:output
//			  LPWORD:lpwAnalisisSize:output
//			  LPWORD:lpwReturn		:output
//				= OK
//				= NG
//				= FORMAT_ERR
// Return	= OK
////////////////////////////////////////////////////////////////////////////////
long UPICD::N_GetOneByteLength( LPBYTE lpbyData, WORD wDataSize, LPWORD lpwLnLength,
	LPWORD lpwVnLength, LPWORD lpwAnalisisSize, LPWORD lpwReturn )
{
	*lpwAnalisisSize = 0;
	*lpwReturn=OK;

	if( lpbyData == NULL ){
		*lpwReturn = NG;
		return OK;
	}
	if( wDataSize < 1 ){
		*lpwReturn = FORMAT_ERR;
		return OK;
	}

	*lpwVnLength = *lpbyData;
	*lpwLnLength = 1;
	*lpwAnalisisSize = 1;
	*lpwReturn = OK;
	return OK;
}

//////////////////////////////////////////////////////////////////////////////
// FuncClass= Internal Function
// FuncName	= SetOneByteLength
// CallType	= Function
// Detail	= Set OneByte Length
// Valuable	= WORD	:wLength	:input
//			  LPBYTE:lpbyBuff	:outout
//			  WORD	:wBuffSize	:inout
//			  LPWORD:lpwDataSize:outout
//			  LPWORD:lpwReturn	:outout
//				= OK
// Return	= OK
//////////////////////////////////////////////////////////////////////////////
long UPICD::N_SetOneByteLength( WORD wLength, LPBYTE lpbyBuff, WORD wBuffSize,
	LPWORD lpwDataSize, LPWORD lpwReturn )
{
	*lpwReturn = OK;
	*lpbyBuff = BYTE(wLength);
	*lpwDataSize = 1;
	return OK;
}

////////////////////////////////////////////////////////////////////////////////
// FuncClass= Internal Function
// FuncName	= GetCommandMessage
// CallType	= Function
// Detail	= Generate command message
// Valuable	= BYTE	:byCLA							:input
//			  BYTE	:byINS							:input
//			  BYTE	:byP1							:input
//			  BYTE	:byP2							:input
//			  WORD	:wFormat						:input
//				COMMAND_MESSAGE_FORMAT_CLA_INS_P1_P2
//				COMMAND_MESSAGE_FORMAT_CLA_INS_P1_P2_LE
//			  BYTE	:byLe							:input
//			  LPWORD:lpwCommandVDataFieldTBL		:input
//			  WORD	:wCommandVDataFieldTBLLength	:input
//			  LPWORD:lpwCommandTLVDataFieldTBL		:input
//			  WORD	:wCommandTLVDataFieldTBLLength	:input
//			  LPBYTE:lpbyData						:output
//			  WORD	:wDataSize						:input
//			  LPWORD:lpwDataSize					:output
//			  LPWORD:lpwReturn						:output
//				OK
//				NG
//				FORMAT_ERR
//				NODATA
// Return	= OK
////////////////////////////////////////////////////////////////////////////////
long UPICD::N_GetCommandMessage( BYTE byCLA,BYTE byINS,BYTE byP1,BYTE byP2,
	WORD wFormat, BYTE byLe,
	LPWORD lpwCommandVDataFieldTBL, WORD wCommandVDataFieldTBLLength,
	LPWORD lpwCommandTLVDataFieldTBL, WORD wCommandTLVDataFieldTBLLength,
	LPBYTE lpbyData, WORD wDataSize, LPWORD lpwDataSize, LPWORD lpwReturn )
{
	BYTE	byMCUSendData[COMMANDMESSAGEDATASIZE];	//MCU send message buffer
	LPBYTE	lpbyMCUSendData_Crnt;					//MCU send message buffer current address
	WORD	wMCUSendDataSize = 0;					//MCU send message buffersavedatasize
	BYTE	byICDataFld[COMMANDMESSAGEDATASIZE];	//ICdata field buffer
	LPBYTE	lpbyICDataFld_Crnt;						//ICdata field buffer current address
	WORD	wICDataFldSize = 0;						//ICdata field buffer save data size
	BYTE	byData[256];							//work buffer
	WORD	wGetDataSize = 0;						//work buffer save data size
	WORD	wDataSize_WK = 0;
	WORD	wDataSize_WK2 = 0;
	WORD	wCnt=0;

	*lpwReturn = OK;
	*lpwDataSize = 0;

	if((wCommandVDataFieldTBLLength != 0 )&&(wCommandTLVDataFieldTBLLength) != 0){
		*lpwReturn = NG;
		//N_Alarm( PARAM_ERR, APMSG_ICDPC_GETCOMMANDMESSAGE );
		return OK;
	}

	lpbyMCUSendData_Crnt = &byMCUSendData[2];	//MCU-I/F use data size
	wMCUSendDataSize = 2;
	lpbyICDataFld_Crnt = byICDataFld;
	wICDataFldSize = 0;

	*lpbyMCUSendData_Crnt = byCLA;			//CLA
	*(lpbyMCUSendData_Crnt+1) = byINS;		//INS
	*(lpbyMCUSendData_Crnt+2) = byP1;		//P1
	*(lpbyMCUSendData_Crnt+3) = byP2;		//P2
	lpbyMCUSendData_Crnt = lpbyMCUSendData_Crnt + 4;
	wMCUSendDataSize = wMCUSendDataSize + 4;

	if( lpwCommandTLVDataFieldTBL != NULL && wCommandTLVDataFieldTBLLength > 0 ){	//data field exist
		for( wCnt = 0; wCnt < wCommandTLVDataFieldTBLLength; wCnt++ ){			//data field loop
			F_InfoGet( lpwCommandTLVDataFieldTBL[wCnt],							//get data field information(→workbuffer)
				byData, sizeof(byData), &wGetDataSize, lpwReturn );
			if( *lpwReturn == NG ){												//Fail to get data field information	 
				return OK;
			}
			N_SetTLV( lpwCommandTLVDataFieldTBL[wCnt],							//data field TLVset(workbuffer→ICdata field buffer)
				wGetDataSize, byData,
				sizeof(byICDataFld) - wICDataFldSize, lpbyICDataFld_Crnt,
				&wDataSize_WK, &wDataSize_WK2, lpwReturn );
			if( *lpwReturn != OK ){												//Fail to get data field information
				return OK;
			}
			lpbyICDataFld_Crnt += wDataSize_WK;
			wICDataFldSize += wDataSize_WK;
		}
		if( wMCUSendDataSize + sizeof(WORD) > sizeof(byMCUSendData) ){			//buffer short
			*lpwReturn = FORMAT_ERR;
			return OK;
		}
		N_SetOneByteLength( wICDataFldSize, lpbyMCUSendData_Crnt,				//Lc set(→MCUsend message buffer)
			sizeof(WORD), &wDataSize_WK, lpwReturn );
		if( *lpwReturn != OK ){													//Fail to get data field information
			return OK;
		}
		lpbyMCUSendData_Crnt += wDataSize_WK;
		wMCUSendDataSize += wDataSize_WK;

		if( wMCUSendDataSize + wICDataFldSize > sizeof(byMCUSendData) ){		//buffer short
			*lpwReturn = FORMAT_ERR;
			return OK;
		}
		memcpy( lpbyMCUSendData_Crnt, byICDataFld, wICDataFldSize );			// data  field setICdata field buffer→MCUsend message buffer
		lpbyMCUSendData_Crnt += wICDataFldSize;
		wMCUSendDataSize += wICDataFldSize;
	}else if( lpwCommandVDataFieldTBL != NULL &&								//data field exist
		wCommandVDataFieldTBLLength > 0 ){
			for( wCnt = 0; wCnt < wCommandVDataFieldTBLLength; wCnt++ ){			//data field loop
				F_InfoGet( lpwCommandVDataFieldTBL[wCnt], lpbyICDataFld_Crnt,		//get data field information  (→workbuffer)
					sizeof(byICDataFld) - wICDataFldSize, &wDataSize_WK, lpwReturn );
				if( *lpwReturn == NG ){												//Fail to get data field information	 
					return OK;
				}
				lpbyICDataFld_Crnt += wDataSize_WK;
				wICDataFldSize += wDataSize_WK;
			}
			if( wMCUSendDataSize + sizeof(WORD) > sizeof(byMCUSendData) ){			//buffer short
				*lpwReturn = FORMAT_ERR;
				return OK;
			}
			N_SetOneByteLength( wICDataFldSize, lpbyMCUSendData_Crnt,				//Lcset(→MCU send message buffer)
				sizeof(WORD), &wDataSize_WK, lpwReturn );
			if( *lpwReturn != OK ){													//Fail to get data field information
				return OK;
			}
			lpbyMCUSendData_Crnt += wDataSize_WK;
			wMCUSendDataSize += wDataSize_WK;

			if( wMCUSendDataSize + wICDataFldSize > sizeof(byMCUSendData) ){		//buffer short
				*lpwReturn = FORMAT_ERR;
				return OK;
			}
			memcpy( lpbyMCUSendData_Crnt, byICDataFld, wICDataFldSize );			// data field set ICdata field buffer→MCU send message buffer
			lpbyMCUSendData_Crnt += wICDataFldSize;
			wMCUSendDataSize += wICDataFldSize;
	}

	if( wFormat == COMMAND_MESSAGE_FORMAT_CLA_INS_P1_P2_LE ){					//Len exist in command mode
		if( wMCUSendDataSize + 1 > sizeof(byMCUSendData) ){					//buffer short
			*lpwReturn = FORMAT_ERR;
			return OK;
		}
		*lpbyMCUSendData_Crnt = byLe;											//command information set Le→MCU send message buffer
		lpbyMCUSendData_Crnt++;
		wMCUSendDataSize++;
	}

	wDataSize_WK = wMCUSendDataSize - 2;										//set MCU-I/F use data size 
	memcpy( byMCUSendData, &wDataSize_WK, sizeof(WORD) );
	if (wMCUSendDataSize > wDataSize){											//buffer short
		*lpwReturn = FORMAT_ERR;
		return OK;
	}
	memcpy(lpbyData, byMCUSendData + 2, wDataSize_WK);							//save MCU send message buffer to data save front buffer 
	*lpwDataSize = wDataSize_WK;
	return OK;
}


//void UPICD::GetCommandAndManagementTBL(WORD wCommand, InfoManagementTBL* stManagementTBLList, WORD& listNum)
//{	
//	listNum = 0;
//	WORD wCnt = stClearCommandList[wCommand].wCnt;
//	WORD wCnt_MAX = sizeof(stInfoManagementTBL) / sizeof(stInfoManagementTBL[0]);
//
//	for (WORD i = 0; i < wCnt; i++)
//	{
//		for( WORD j = 0; j < wCnt_MAX; j++ )
//		{
//			if( stInfoManagementTBL[j].wItem == stClearCommandList[wCommand].wTag[i] )
//			{
//				stManagementTBLList[i] = stInfoManagementTBL[j];
//				listNum++;
//				break;
//			}
//		}
//	}
//}



////////////////////////////////////////////////////////////////////////////////
// FuncClass= External Function
// FuncName	= GetCommandMessage
// CallType	= Function
// Detail	= Generating command message
// Valuable	= BYTE	byCLA							:input
//			  BYTE	byINS							:input
//			  BYTE	byP1							:input
//			  BYTE	byP2							:input
//			  WORD	wFormat							:input
//				COMMAND_MESSAGE_FORMAT_CLA_INS_P1_P2
//				COMMAND_MESSAGE_FORMAT_CLA_INS_P1_P2_LE
//			  BYTE	byLe							:input
//			  LPWORD lpwCommandVDataFieldTBL		:input
//			  WORD	wCommandVDataFieldTBLLength		:input
//			  LPWORD lpwCommandTLVDataFieldTBL		:input
//			  WORD	wCommandTLVDataFieldTBLLength	:input
//			  LPBYTE lpbyData						:output
//			  WORD	wDataSize						:input
//			  LPWORD lpwDataSize					:output
//			  LPWORD lpwReturn						:output
//				OK
//				NG
//				FORMAT_ERR
//				NODATA
// Return	= OK
////////////////////////////////////////////////////////////////////////////////
long UPICD::F_GetCommandMessage( BYTE byCLA,BYTE byINS,BYTE byP1,BYTE byP2,
	WORD wFormat, BYTE byLe,
	LPWORD lpwCommandVDataFieldTBL, WORD wCommandVDataFieldTBLLength,
	LPWORD lpwCommandTLVDataFieldTBL, WORD wCommandTLVDataFieldTBLLength,
	LPBYTE lpbyData, WORD wDataSize, LPWORD lpwDataSize, LPWORD lpwReturn )
{
	if( lpwReturn == NULL ){
		//N_Alarm( PARAM_ERR, APMSG_ICDPC_GETCOMMANDMESSAGE );
		return OK;
	}
	if( (lpbyData == NULL) || (lpwDataSize == NULL) ){
		*lpwReturn = NG;
		//N_Alarm( PARAM_ERR, APMSG_ICDPC_GETCOMMANDMESSAGE );
		return OK;
	}

	*lpwDataSize = 0;
	*lpwReturn = OK;

	F_InfoItemClear( ICDPC_STATUS, lpwReturn );
	if( *lpwReturn != OK ){
		return OK;
	}

	N_GetCommandMessage( byCLA, byINS, byP1, byP2, wFormat, byLe,
		lpwCommandVDataFieldTBL, wCommandVDataFieldTBLLength,
		lpwCommandTLVDataFieldTBL,wCommandTLVDataFieldTBLLength,
		lpbyData, wDataSize, lpwDataSize, lpwReturn );
	return OK;
}

////////////////////////////////////////////////////////////////////////////////
// FuncClass= External Function
// FuncName	= InfoGet
// CallType	= Function
// Detail	= Get from the data values
// Valuable	= WORD	:wItem		:input
//			  LPBYTE:lpbyBuffer	:output
//			  WORD	:wBufSize	:input
//			  LPWORD:lpwDataSize:output
//			  LPWORD:lpwReturn	:output
//				OK
//				NG
//				NODATA
// Return	= OK
////////////////////////////////////////////////////////////////////////////////
long UPICD::F_InfoGet( WORD wItem, LPBYTE lpbyBuffer, WORD wBufSize,
	LPWORD lpwDataSize, LPWORD lpwReturn )
{
	if( lpwReturn == NULL ){
		//N_Alarm( PARAM_ERR, APMSG_ICDPC_INFOGET );
		return OK;
	}
	if( (lpbyBuffer == NULL) || (lpwDataSize == NULL) ){
		*lpwReturn = NG;
		//N_Alarm( PARAM_ERR, APMSG_ICDPC_INFOGET );
		return OK;
	}

	*lpwReturn = OK;
	*lpwDataSize = 0;

	if ( wItem == ICDPC_APPLICATION_LIST ){	
		if(wBufSize < sizeof(stCandidateList)){
			*lpwReturn = NG;
			//N_Alarm( BUFFER_ERR, APMSG_ICDPC_INFOGET);
			return OK;
		}
		memcpy(lpbyBuffer, &(stCandidateList.wCandidateNum), sizeof(WORD));
		lpbyBuffer += sizeof(WORD);
		for(int i=0; i<stCandidateList.wCandidateNum; i++){
			memcpy(lpbyBuffer, &stCandidateList.stCandidate[i], sizeof(Candidate));
			lpbyBuffer += sizeof(Candidate);
		}
		*lpwDataSize = sizeof(stCandidateList);
		return OK;
	}

	//if( wItem == ICDPC_ICPARAMETER_FILEPATH ||                                 
	//	wItem == ICDPC_ICTEMINALACTIONCODE_FILEPATH){                          
	//		char cFileName[MAX_PATH];                                          
	//		strcpy(cFileName, strConfigHome);                                  
	//		if(wItem == ICDPC_ICPARAMETER_FILEPATH){                           
	//			strcat(cFileName, szICParameterFile);                          
	//		}else{                                                             
	//			strcat(cFileName, szTACFile);                                  
	//		}                                                                  
	//		WORD wFileNameLength = strlen(cFileName) + 1;                      
	//		if(wBufSize < wFileNameLength){                                    
	//			*lpwReturn = NG;                                               
	//			//N_Alarm( BUFFER_ERR, APMSG_ICDPC_INFOGET);                     
	//			return OK;                                                     
	//		}                                                                  
	//		strcpy((char*)lpbyBuffer, cFileName);                              
	//		*lpwDataSize = wFileNameLength;                                    
	//		return OK;                                                         
	//}                                                                          

	WORD wCnt_MAX = sizeof(stInfoManagementTBL) / sizeof(stInfoManagementTBL[0]);

	for( WORD wCnt=0; wCnt<wCnt_MAX; wCnt++ ){
		if( stInfoManagementTBL[wCnt].wItem == wItem ){
			WORD wDataSize = stInfoManagementTBL[wCnt].wDataSize;
			if( wDataSize == 0 ){			
				*lpwReturn = NODATA;		
				return OK;					
			}								
			if( wBufSize < wDataSize ){
				*lpwReturn = NG;
				//N_Alarm( BUFFER_ERR, APMSG_ICDPC_INFOGET );
				return OK;
			}
			if( wInfoType[wCnt][IC_CLEARSIZE] == CONSTANT ){
				if( wInfoType[wCnt][IC_BUFFSIZE] != wDataSize ){
					*lpwReturn = NG;
					//N_Alarm( LOGIC_ERR, APMSG_ICDPC_INFOGET );
					return OK;
				}
			}
			memcpy( lpbyBuffer, stInfoManagementTBL[wCnt].lpbyBuffer, wDataSize );
			*lpwDataSize = wDataSize;
			return OK;
		}
	}
	*lpwReturn = NG;
	//N_Alarm( LOGIC_ERR, APMSG_ICDPC_INFOGET );
	return OK;
}

////////////////////////////////////////////////////////////////////////////////
// FuncClass= External Function
// FuncName	= MandatoryDataCheck
// CallType	= Function
// Detail	= Whether all the specified input data lead be checked  
// Valuable	= LPWORD:wMandatoryType		:input
//			  LPWORD:lpwReturn:output	:output
//				OK
//				NG
//				CHECK_ERR
// Return	= OK
////////////////////////////////////////////////////////////////////////////////
long UPICD::F_MandatoryDataCheck( WORD wMandatoryType, LPWORD lpwReturn )
{
	WORD wDataSize = 0;
	BYTE byData[252];
	BYTE byUseAlgorithm[1];

	if( lpwReturn == NULL ){
		//N_Alarm( PARAM_ERR, APMSG_ICDPC_MANDATORYDATACHECK );
		return OK;
	}
	if( wMandatoryType == NULL ){
		*lpwReturn = NG;
		//N_Alarm( PARAM_ERR, APMSG_ICDPC_MANDATORYDATACHECK );
		return OK;
	}

	*lpwReturn = OK;

	if( wMandatoryType != ICDPC_ALWAYS ){								 
		F_InfoGet( ICDPC_TM_USE_ALGORITHM, (LPBYTE)byUseAlgorithm,		  
			sizeof(byUseAlgorithm), &wDataSize, lpwReturn );  
		if( *lpwReturn != OK ){											 
			if( *lpwReturn == NODATA ){									 
				*lpwReturn = MANDATORY_CHECK_ERR;						 
				return OK;												 
			}															 
			//N_Alarm( LOGIC_ERR, APMSG_ICDPC_MANDATORYDATACHECK );		 
			return OK;													 
		}																 
	}																	 
	switch( wMandatoryType ){
		// Parameter check
	case ICDPC_ALWAYS:
		F_InfoGet( ICDPC_IC_APPLICATION_EXPIRATION_DATE, (LPBYTE)byData,
			sizeof(byData), &wDataSize, lpwReturn );
		if( *lpwReturn != OK ){			 
			if( *lpwReturn == NODATA ){	 
				*lpwReturn = MANDATORY_CHECK_ERR;					 
				return OK;
			}							 
			//N_Alarm( LOGIC_ERR, APMSG_ICDPC_MANDATORYDATACHECK );	 
			return OK;					 
		}
		F_InfoGet( ICDPC_IC_PAN, (LPBYTE)byData,
			sizeof(byData), &wDataSize, lpwReturn );
		if( *lpwReturn != OK ){			 
			if( *lpwReturn == NODATA ){	 
				*lpwReturn = MANDATORY_CHECK_ERR;					 
				return OK;
			}							 
			//N_Alarm( LOGIC_ERR, APMSG_ICDPC_MANDATORYDATACHECK );	 
			return OK;					 
		}
		F_InfoGet( ICDPC_IC_CDOL1, (LPBYTE)byData,
			sizeof(byData), &wDataSize, lpwReturn );
		if( *lpwReturn != OK ){			 
			if( *lpwReturn == NODATA ){	 
				*lpwReturn = MANDATORY_CHECK_ERR;					 
				return OK;
			}							 
			//N_Alarm( LOGIC_ERR, APMSG_ICDPC_MANDATORYDATACHECK );	 
			return OK;					 
		}
		F_InfoGet( ICDPC_IC_CDOL2, (LPBYTE)byData,
			sizeof(byData), &wDataSize, lpwReturn );
		if( *lpwReturn != OK ){			 
			if( *lpwReturn == NODATA ){	 
				*lpwReturn = MANDATORY_CHECK_ERR;					 
				return OK;
			}							 
			//N_Alarm( LOGIC_ERR, APMSG_ICDPC_MANDATORYDATACHECK );	 
			return OK;					 
		}
		return OK;
		// Parameter check static authentication
	case ICDPC_OFFLINESTATIC:
		F_InfoGet( ICDPC_IC_CA_PUBLIC_KEY_INDEX, (LPBYTE)byData,
			sizeof(byData), &wDataSize, lpwReturn );
		if( *lpwReturn != OK ){			 
			if( *lpwReturn == NODATA ){	 
				*lpwReturn = MANDATORY_CHECK_ERR;					 
				return OK;
			}							 
			//N_Alarm( LOGIC_ERR, APMSG_ICDPC_MANDATORYDATACHECK );	 
			return OK;					 
		}
		F_InfoGet( ICDPC_IC_ISSUER_PUBLIC_KEY_CERTIFICATE, (LPBYTE)byData,
			sizeof(byData), &wDataSize, lpwReturn );
		if( *lpwReturn != OK ){			 
			if( *lpwReturn == NODATA ){	 
				*lpwReturn = MANDATORY_CHECK_ERR;					 
				return OK;
			}							 
			//N_Alarm( LOGIC_ERR, APMSG_ICDPC_MANDATORYDATACHECK );	 
			return OK;					 
		}
		F_InfoGet( ICDPC_IC_SINGED_STATIC_APPLICATION_DATA, (LPBYTE)byData,
			sizeof(byData), &wDataSize, lpwReturn );
		if( *lpwReturn != OK ){			 
			if( *lpwReturn == NODATA ){	 
				*lpwReturn = MANDATORY_CHECK_ERR;					 
				return OK;
			}							 
			//N_Alarm( LOGIC_ERR, APMSG_ICDPC_MANDATORYDATACHECK );	 
			return OK;					 
		}
		if( byUseAlgorithm[0] ==0x00 ){								 
			F_InfoGet( ICDPC_IC_ISSUER_PUBLIC_KEY_REMAINDER, (LPBYTE)byData,
				sizeof(byData), &wDataSize, lpwReturn );
			if( *lpwReturn != OK ){			 
				if( *lpwReturn == NODATA ){	 
					*lpwReturn = MANDATORY_CHECK_ERR;					 
					return OK;
				}							 
				//N_Alarm( LOGIC_ERR, APMSG_ICDPC_MANDATORYDATACHECK );	 
				return OK;					 
			}
			F_InfoGet( ICDPC_IC_ISSUER_PUBLIC_KEY_EXPONENT, (LPBYTE)byData,
				sizeof(byData), &wDataSize, lpwReturn );
			if( *lpwReturn != OK ){			 
				if( *lpwReturn == NODATA ){	 
					*lpwReturn = MANDATORY_CHECK_ERR;					 
					return OK;
				}							 
				//N_Alarm( LOGIC_ERR, APMSG_ICDPC_MANDATORYDATACHECK );	 
				return OK;					 
			}
		}
		return OK;
		// Parameter check dynamic authentication
	case ICDPC_OFFLINEDYNAMIC:
		F_InfoGet( ICDPC_IC_CA_PUBLIC_KEY_INDEX, (LPBYTE)byData,
			sizeof(byData), &wDataSize, lpwReturn );
		if( *lpwReturn != OK ){			 
			if( *lpwReturn == NODATA ){	 
				*lpwReturn = MANDATORY_CHECK_ERR;					 
				return OK;
			}							 
			//N_Alarm( LOGIC_ERR, APMSG_ICDPC_MANDATORYDATACHECK );	 
			return OK;					 
		}
		F_InfoGet( ICDPC_IC_ISSUER_PUBLIC_KEY_CERTIFICATE, (LPBYTE)byData,
			sizeof(byData), &wDataSize, lpwReturn );
		if( *lpwReturn != OK ){			 
			if( *lpwReturn == NODATA ){	 
				*lpwReturn = MANDATORY_CHECK_ERR;					 
				return OK;
			}							 
			//N_Alarm( LOGIC_ERR, APMSG_ICDPC_MANDATORYDATACHECK );	 
			return OK;					 
		}
		F_InfoGet( ICDPC_IC_ISSUER_PUBLIC_KEY_REMAINDER, (LPBYTE)byData,
			sizeof(byData), &wDataSize, lpwReturn );
		if( *lpwReturn != OK ){			 
			if( *lpwReturn == NODATA ){	 
				*lpwReturn = MANDATORY_CHECK_ERR;					 
				return OK;
			}							 
			//N_Alarm( LOGIC_ERR, APMSG_ICDPC_MANDATORYDATACHECK );	 
			return OK;					 
		}
		F_InfoGet( ICDPC_IC_ISSUER_PUBLIC_KEY_EXPONENT, (LPBYTE)byData,
			sizeof(byData), &wDataSize, lpwReturn );
		if( *lpwReturn != OK ){			 
			if( *lpwReturn == NODATA ){	 
				*lpwReturn = MANDATORY_CHECK_ERR;					 
				return OK;
			}							 
			//N_Alarm( LOGIC_ERR, APMSG_ICDPC_MANDATORYDATACHECK );	 
			return OK;					 
		}
		F_InfoGet( ICDPC_IC_ICC_PUBLIC_KEY_CERTIFICATE, (LPBYTE)byData,
			sizeof(byData), &wDataSize, lpwReturn );
		if( *lpwReturn != OK ){			 
			if( *lpwReturn == NODATA ){	 
				*lpwReturn = MANDATORY_CHECK_ERR;					 
				return OK;
			}							 
			//N_Alarm( LOGIC_ERR, APMSG_ICDPC_MANDATORYDATACHECK );	 
			return OK;					 
		}
		F_InfoGet( ICDPC_IC_ICC_PUBLIC_KEY_EXPONENT, (LPBYTE)byData,
			sizeof(byData), &wDataSize, lpwReturn );
		if( *lpwReturn != OK ){			 
			if( *lpwReturn == NODATA ){	 
				*lpwReturn = MANDATORY_CHECK_ERR;					 
				return OK;
			}							 
			//N_Alarm( LOGIC_ERR, APMSG_ICDPC_MANDATORYDATACHECK );	 
			return OK;					 
		}
		F_InfoGet( ICDPC_IC_ICC_PUBLIC_KEY_REMAINDER, (LPBYTE)byData,
			sizeof(byData), &wDataSize, lpwReturn );
		if( *lpwReturn != OK ){			 
			if( *lpwReturn == NODATA ){	 
				*lpwReturn = MANDATORY_CHECK_ERR;					 
				return OK;
			}							 
			//N_Alarm( LOGIC_ERR, APMSG_ICDPC_MANDATORYDATACHECK );	 
			return OK;					 
		}
		F_InfoGet( ICDPC_IC_DDOL, (LPBYTE)byData,
			sizeof(byData), &wDataSize, lpwReturn );
		if( *lpwReturn != OK ){			 
			if( *lpwReturn == NODATA ){	 
				*lpwReturn = MANDATORY_CHECK_ERR;					 
				return OK;
			}							 
			//N_Alarm( LOGIC_ERR, APMSG_ICDPC_MANDATORYDATACHECK );	 
			return OK;					 
		}
		return OK;
	}
	// other
	//N_Alarm( PARAM_ERR, APMSG_ICDPC_MANDATORYDATACHECK );	 
	*lpwReturn = NG;
	return OK;
}

////////////////////////////////////////////////////////////////////////////////
// FuncClass= External Function
// FuncName	= DataGet
// CallType	= Function
// Detail	= Specified WORD type data value get
// Valuable	= WORD	:wItem		:input
//				ICDPC_PRIORITY_LIST_DF_NO
//			  LPWORD:lpwBuffer	:output
//			  LPWORD:lpwReturn	:output
//				OK
//				NG
// Return	= OK
////////////////////////////////////////////////////////////////////////////////
long UPICD::F_DataGet( WORD wItem, LPWORD lpwBuffer, LPWORD lpwReturn )
{
	BYTE byUsedAlgorithm[2];
	WORD wSize;
	if( lpwReturn == NULL ){
		//N_Alarm( PARAM_ERR, APMSG_ICDPC_DATAGET );
		return OK;
	}
	if( lpwBuffer == NULL ){
		*lpwReturn = NG;
		//N_Alarm( PARAM_ERR, APMSG_ICDPC_DATAGET );
		return OK;
	}

	*lpwReturn = OK;

	switch( wItem ){
	case ICDPC_PRIORITY_LIST_DF_NO:
		*lpwBuffer = stCandidateList.wCandidateNum;
		break;
	case ICDPC_DEFAULT_TDOL:
		//The implementation of DefaultTDOL examination
		if (TDOLFlg && !ICTDOLFlg && N_DOLTagCheck(ICDPC_IC_CDOL1)){	 
			*lpwBuffer = TRUE;											 
		}else{															 
			*lpwBuffer = FALSE;											 
		}																 
		break;
	case ICDPC_OFFLINEAUTHSUPPORT:                                       
		F_InfoGet( ICDPC_TM_USE_ALGORITHM, (LPBYTE)byUsedAlgorithm,sizeof(byUsedAlgorithm), &wSize, lpwReturn );     
		if( *lpwReturn != OK ){                                          
			return OK;                                                   
		}                                                                
		if( ( byUsedAlgorithm[0] == 0x01 && (wCipherMode & 0x02) != 0 && hChinaCipherModule != NULL) ||              
			( byUsedAlgorithm[0] == 0x00 && (wCipherMode & 0x01) != 0 && hNationalCipherModule != NULL) ){           
				*lpwBuffer = ICDPC_SDA;                                      
		}else{                                                           
			*lpwBuffer = 0;                                              
		}																 
		break;
	default :
		//N_Alarm( INTERFACE_ERR, APMSG_ICDPC_DATAGET );
		*lpwReturn = NG;
	}
	return OK;
}

////////////////////////////////////////////////////////////////////////////////
// FuncClass= External Function
// FuncName	= UserTerminalAIDSet
// CallType	= Function
// Detail	= Set the user defined file list to IC AID TBL data management
//			  Return Terminal AID list pointer
// Valuable	= LPWORD:lpwReturn	:output
//				OK
//				NG
// Return	= OK
////////////////////////////////////////////////////////////////////////////////
long UPICD::F_UserTerminalAIDSet( LPWORD lpwReturn )
{
	if( lpwReturn == NULL ){
		//N_Alarm( PARAM_ERR, APMSG_ICDPC_USERTERMINALAIDSET );
		return OK;
	}

	*lpwReturn = OK;

	// Set the user defined file list to IC AID TBL data management
	F_InfoSet( ICDPC_AID_LIST, (LPBYTE)&stTerminalAIDList,
		sizeof(stTerminalAIDList), lpwReturn );
	return OK;
}

////////////////////////////////////////////////////////////////////////////////
// FuncClass= External Function
// FuncName	= SetAID
// CallType	= Function
// Detail	= Set the specified item to AID
// Input 	= LPWORD:lpwReturn	:output
//				OK
//				NG
//				NODATA
// Return	= OK
////////////////////////////////////////////////////////////////////////////////
long UPICD::F_SetAID( LPWORD lpwReturn )
{
	BYTE byAPID[16];
	WORD wDataSize = 0;
	WORD wAIDSize = 0;
	TerminalAIDList stSetAIDList;

	if( lpwReturn == NULL ){
		//N_Alarm( PARAM_ERR, APMSG_ICDPC_SETAID );
		return OK;
	}

	*lpwReturn = OK;

	// AID list get
	F_InfoGet( ICDPC_AID_LIST, (LPBYTE)&stSetAIDList,
		sizeof(stSetAIDList), &wDataSize, lpwReturn );
	if( *lpwReturn == NG ){
		//N_Alarm( LOGIC_ERR, APMSG_ICDPC_SETAID );
		return OK;
	}

	// List / AID number (DF number) is 0, the return value is NODATA
	if( stSetAIDList.wNum == 0 ){
		*lpwReturn = NODATA;
		return OK;
	}

	// DF size set
	wAIDSize = strlen( stSetAIDList.stTerminalAID[0].lpsAID );

	// Pointer to the string variable AID made for BCD codes
	N_ASCtoHEX(byAPID, (LPBYTE)stSetAIDList.stTerminalAID[0].lpsAID, wAIDSize, lpwReturn);
	if( *lpwReturn != OK ){
		return OK;
	}

	// File identification set
	F_InfoSet( ICDPC_TM_AID, byAPID, wAIDSize/2, lpwReturn );
	if( *lpwReturn != OK ){
		return OK;
	}

	// Application program choose id set
	F_InfoSet( ICDPC_TM_APPLICATION_SELECTION_INDICATOR,
		&(stSetAIDList.stTerminalAID[0].byApplicationSelectionIndicator),
		sizeof(BYTE), lpwReturn );
	if( *lpwReturn != OK ){
		return OK;
	}

	// Set the terminal application version number 
	F_InfoSet( ICDPC_TM_APPLICATION_VER_NO,
		stSetAIDList.stTerminalAID[0].byAPVersionIndicator,
		sizeof(stSetAIDList.stTerminalAID[0].byAPVersionIndicator), lpwReturn );
	if( *lpwReturn != OK ){
		return OK;
	}

	if(!bGlobalTACMode)                                                         
	{                                                                           
		//TACDenial                                                             
		F_InfoSet( ICDPC_TM_TAC_DENIAL,                                         
			stSetAIDList.stTerminalAID[0].byTACDenial,                          
			sizeof(stSetAIDList.stTerminalAID[0].byTACDenial), lpwReturn );     
		if( *lpwReturn != OK ){                                                 
			return OK;                                                          
		}                                                                       
		//TACOnline                                                             
		F_InfoSet( ICDPC_TM_TAC_ONLINE,                                         
			stSetAIDList.stTerminalAID[0].byTACOnline,                          
			sizeof(stSetAIDList.stTerminalAID[0].byTACOnline), lpwReturn );     
		if( *lpwReturn != OK ){                                                 
			return OK;                                                          
		}                                                                       
		//TACDefault                                                            
		F_InfoSet( ICDPC_TM_TAC_DEFAULT,                                        
			stSetAIDList.stTerminalAID[0].byTACDefault,                         
			sizeof(stSetAIDList.stTerminalAID[0].byTACDefault), lpwReturn );    
		if( *lpwReturn != OK ){                                                 
			return OK;                                                          
		}                                                                       
	}                                                                           

	// Reduce the set AID
	stSetAIDList.wNum -= 1;
	memmove( &(stSetAIDList.stTerminalAID[0].lpsAID),
		&(stSetAIDList.stTerminalAID[1].lpsAID),
		(stSetAIDList.wNum)*(sizeof(stSetAIDList.stTerminalAID[1])));
	F_InfoSet( ICDPC_AID_LIST, (LPBYTE)&stSetAIDList, sizeof(stSetAIDList), lpwReturn );
	return OK;
}

////////////////////////////////////////////////////////////////////////////////
// FuncClass= External Function
// FuncName	= VersionGet
// CallType	= Function
// Detail	= return class version num 
// Valuable	= LPSTR	:lpsVersion		:output
//			  WORD	:wBufSize		:input
//			  LPWORD:lpwDataSize	:output
//			  LPWORD:lpwReturn		:output
//				OK
// Return	= OK
////////////////////////////////////////////////////////////////////////////////
long UPICD::F_VersionGet( LPSTR lpsVersion, WORD wBufSize, LPWORD lpwDataSize, LPWORD lpwReturn )
{
    WORD wByLen = sizeof(byTaskVRTU);
    *lpwDataSize = std::min( wBufSize, wByLen);
	memcpy( (LPBYTE)lpsVersion, byTaskVRTU, *lpwDataSize );
	*lpwReturn = OK;
	return OK;
}

////////////////////////////////////////////////////////////////////////////////
// FuncClass= External Function
// FuncName	= TvrTsiSet
// CallType	= Function
// Detail	= TVR/TSI bit information set
// Valuable	= WORD	:wItem		:input
//			  WORD	:wByteici	:input
//			  BYTE	:byBit		:input
//			  LPWORD:lpwReturn	:output
//				OK
//				NG
// Return	= OK
// Specially=
// Date		= 2005/09/27
// History	= UPICDSTD,01-00-00-00,05/09/27 Horii :
////////////////////////////////////////////////////////////////////////////////
long UPICD::F_TvrTsiSet( WORD wItem, WORD wBytePosition, BYTE byBit, LPWORD lpwReturn )
{
	BYTE byData[5];
	WORD wDataSize = 0;

	if( lpwReturn == NULL ){
		//N_Alarm( PARAM_ERR, APMSG_ICDPC_TVRTSISET );
		return OK;
	}

	// Item get
	F_InfoGet( wItem, byData, sizeof(byData), &wDataSize, lpwReturn );
	if( *lpwReturn == NG ){
		//N_Alarm( INTERFACE_ERR, APMSG_ICDPC_TVRTSISET );
		return OK;
	}

	// Error when data save byte position is 0 or bigger than set data size
	if( wBytePosition == 0 || wBytePosition > wDataSize ){
		*lpwReturn = NG;
		//N_Alarm( INTERFACE_ERR, APMSG_ICDPC_TVRTSISET );
		return NG;
	}

	//Set specified bit at specified byte position
	byData[wBytePosition-1] = byData[wBytePosition-1] | byBit;

	//Set specified item
	F_InfoSet( wItem, byData, wDataSize, lpwReturn );
	return OK;
}

//////////////////////////////////////////////////////////////////////////////
// FuncClass= Internal Function
// FuncName	= BlackListCheck
// CallType	= Function
// Detail	= ICC account number exception file (blacklist) log checked
// Valuable	= LPWORD:lpwReturn	:outout
//				OK
//				NG
//				CHECK_ERR
// Return	= OK
//////////////////////////////////////////////////////////////////////////////
long UPICD::F_BlackListCheck( LPWORD lpwReturn )
{

	BYTE	byPAN[10];																				
	BYTE	byPANSeqNum[1];																			
	WORD	wPANSize;																				
	WORD	wPANSeqNumSize;																			
	BOOL	bPANExist= FALSE;																		

	if( lpwReturn == NULL ){
		//N_Alarm( PARAM_ERR, APMSG_ICDPC_BLACKLISTCHECK );
		return OK;
	}

	F_InfoGet( ICDPC_IC_PAN, byPAN, sizeof(byPAN), &wPANSize, lpwReturn );							
	if( *lpwReturn == NG ){																			
		return OK;																					
	} else if ( *lpwReturn != NODATA ){																
		for( WORD wCnt = 0; wCnt < stBlackList.wPANNum; wCnt++ ){									
			if( !memcmp( byPAN, stBlackList.byPAN[wCnt], wPANSize) && 								
				stBlackList.wPANBCDSize[wCnt] == wPANSize){											
					bPANExist=TRUE;																	
			}																						
		}																							
	}																								
	if(bPANExist){																					
		F_InfoGet( ICDPC_IC_PAN_SEQUENCE_NO, byPANSeqNum, sizeof(BYTE), &wPANSeqNumSize, lpwReturn );
		if( *lpwReturn == NG ){																		
			return OK;																				
		} else if ( *lpwReturn != NODATA ){															
			for( WORD wCnt1 = 0; wCnt1 < stBlackList.wPANSeqNum; wCnt1++ ){							
				if( !memcmp( byPANSeqNum, &stBlackList.byPANSequenceNumber[wCnt1], wPANSeqNumSize)){
					*lpwReturn = BLACKLIST_CHECK_ERR;												
					return OK;																		
				}																					
			}																						
		}																							
	}																								

	*lpwReturn = OK;
	return OK;
}

////////////////////////////////////////////////////////////////////////////////
// FuncClass= External Function
// FuncName	= DOLRelationDataMake
// CallType	= Function
// Detail	= Made of DOL related data from the specified dol
// Valuable	= WORD	:wItem			:input
//				ICDPC_IC_PDOL
//				ICDPC_IC_CDOL1
//				ICDPC_IC_CDOL2
//				ICDPC_IC_DDOL
//				ICDPC_IC_STATIC_DATA_AUTHENTICATION_TAG_LIST
//			  LPWORD:lpwReturn		:output
//				OK
//				NG
//				FORMAT_ERR
// Return	= OK
////////////////////////////////////////////////////////////////////////////////
long UPICD::F_DOLRelationDataMake( WORD wItem, LPWORD lpwReturn )
{
	if( lpwReturn == NULL ){
		//N_Alarm( PARAM_ERR, APMSG_ICDPC_DOLRELATIONDATAMAKE );
		return OK;
	}

	BYTE	byDOL[252];						// DOL label list obtained by buffer
	WORD	wDOLSize=0;						// DOL tag list size get
	LPBYTE	lpbyDOL_WK;						// DOL tag list end pointer
	LPBYTE	lpbyDOL_END;					// DOL tag list end pointer
	BYTE	byDOLRelationData[252];			// DOL related data buffer
	BYTE	byDOLRelationDataBuf[252];		// DOL related data manage buffer
	LPBYTE	lpbyStartPtr;					// DOL related data start pointer
	LPBYTE	lpbyDOLRelationDataPtr;			// DOL related data indicator
	WORD	wDOLRelationDataSize=0;			// DOL related data size
	WORD	wTag = 0;						// tags get use
	WORD	wTagSize = 0;					// tag size get use
	WORD	wLnLength = 0;					// Ln length
	WORD	wVnLength = 0;					// Vn length
	WORD	wAnalisisSize = 0;				// analysis size 
	WORD	*lpwInfoBuffer[IC_ALL_MEMBER];	// all items
	WORD	wTLSize = 0;					// Length size
	WORD	wDataSize = 0;					// Data size
	LPBYTE	lpbyDOL_BK;						// DOL tag list backup pointer	 

	*lpwReturn = OK;

	// DOL tag list get
	F_InfoGet( wItem, byDOL, 252, &wDOLSize, lpwReturn );
	if( *lpwReturn == NG ){
		return OK;
	}

	// The DOL tag list head buffer first pointer
	if( wItem == ICDPC_IC_PDOL){												
		F_InfoItemClear(ICDPC_TM_USE_ALGORITHM, lpwReturn);						
		if( *lpwReturn == NG ){													
			return OK;															
		}																		
	}																			
	lpbyDOL_WK = byDOL;
	// buffer set end pointer
	lpbyDOL_END = byDOL + wDOLSize;
	// DOL related data set pointer
	lpbyDOLRelationDataPtr = lpbyStartPtr = byDOLRelationData;
	LPBYTE lpbyDOLRelationDataEndPtr = lpbyDOLRelationDataPtr + 252;

	switch( wItem ){
	case ICDPC_IC_PDOL:
		//The DOL tag list does not exist
		if( wDOLSize == 0 ){
			*lpbyDOLRelationDataPtr = LOBYTE(ICDPC_TM_COMMAND_TEMPLATE);
			lpbyDOLRelationDataPtr++;
			// Information set
			*lpbyDOLRelationDataPtr = 0x00;
			lpbyDOLRelationDataPtr++;
			F_InfoSet( ICDPC_TM_COMMAND_TEMPLATE, lpbyStartPtr, lpbyDOLRelationDataPtr - lpbyStartPtr, lpwReturn );
			return OK;
		}
		break;
	case ICDPC_IC_CDOL1:
	case ICDPC_IC_CDOL2:
	case ICDPC_IC_DDOL:
	case ICDPC_IC_TDOL:	 
	case ICDPC_IC_STATIC_DATA_AUTHENTICATION_TAG_LIST:
		break;
	default:
		//N_Alarm( INTERFACE_ERR, APMSG_ICDPC_DOLRELATIONDATAMAKE );
		*lpwReturn = NG;
		return OK;
	}

	// The DOL tag list data size
	while( lpbyDOL_WK < lpbyDOL_END ){
		lpbyDOL_BK = lpbyDOL_WK;	 
		wDOLRelationDataSize = 0;	 
		// Buffer initialization
		memset( byDOLRelationDataBuf, 0x00, 252 );
		// Label information obtained
		N_GetTagInfo( lpbyDOL_WK, lpbyDOL_END - lpbyDOL_WK, &wTag, &wTagSize, lpwReturn );
		switch( *lpwReturn ){
		case NG:
			//N_Alarm( LOGIC_ERR, APMSG_ICDPC_DOLRELATIONDATAMAKE );
			return OK;
		case FORMAT_ERR:
			return OK;
		default:
			break;
		}
		lpbyDOL_WK += wTagSize;
		// Break when the markup pointer is the initial value 
		if( (lpbyDOL_WK == lpbyDOL_END) && (wTag == 0) ){
			break;
		}
		// Analysis of single byte strength
		N_GetOneByteLength( lpbyDOL_WK, lpbyDOL_END - lpbyDOL_WK, &wLnLength,
			&wVnLength, &wAnalisisSize, lpwReturn );
		switch( *lpwReturn ){
		case FORMAT_ERR:
			return OK;
		case NG:
			*lpwReturn = NG;
			//N_Alarm( LOGIC_ERR, APMSG_ICDPC_DOLRELATIONDATAMAKE );
			return OK;
		default:
			break;
		}
		lpbyDOL_WK += wLnLength;
		if( N_TagDefineCheck(lpbyDOL_BK, lpbyDOL_END - lpbyDOL_BK) ){	 
			// information obtained
			N_InfoTypeTblGetMember( wTag, IC_ALL_MEMBER, lpwInfoBuffer, lpwReturn );
			if( *lpwReturn != OK ){
				//N_Alarm( LOGIC_ERR, APMSG_ICDPC_DOLRELATIONDATAMAKE );
				return OK;
			}
			// Vn get
			F_InfoGet( wTag, byDOLRelationDataBuf, *(lpwInfoBuffer[IC_BUFFSIZE]), &wDOLRelationDataSize, lpwReturn );
			if( *lpwReturn == NG ){
				return OK;
			}
			*lpwReturn = OK;							 
		}																 
		if( wDOLRelationDataSize == 0 ){
			// Tn not exist:Ln NULL
			memset( byDOLRelationDataBuf, 0x00, wVnLength );
		}else{
			// Label list of L larger than their actual size
			if( wVnLength < wDOLRelationDataSize ){
				switch( *(lpwInfoBuffer[IC_FORMAT]) ){
				case NUM:
					//cut the left of data element
					memmove( byDOLRelationDataBuf, byDOLRelationDataBuf + (wDOLRelationDataSize - wVnLength), wVnLength );
					break;
				default:
					// cut the right of data element
					// (Related data buffer editing)
					break;
				}
				// Label list  L is bigger than the actual time
			} else if( wVnLength > wDOLRelationDataSize ){
				switch( *(lpwInfoBuffer[IC_FORMAT]) ){
				case CN:
					// move oxFF at the end
					memset( byDOLRelationDataBuf + wDOLRelationDataSize, 0xFF, wVnLength - wDOLRelationDataSize );
					break;
				case AN:
				case ANS:
					// move ox00 at the end
					memset( byDOLRelationDataBuf + wDOLRelationDataSize, 0x00, wVnLength - wDOLRelationDataSize );
					break;
				case NUM:
					// move ox00 at the front left part
					memmove( byDOLRelationDataBuf + (wVnLength-wDOLRelationDataSize), byDOLRelationDataBuf, wDOLRelationDataSize );
					memset( byDOLRelationDataBuf, 0x00, wVnLength - wDOLRelationDataSize );
					break;
				default:
					break;
				}
			}
		}
		if( lpbyDOLRelationDataPtr + wVnLength > lpbyDOLRelationDataEndPtr ){
			*lpwReturn = FORMAT_ERR;
			return OK;
		}
		// Related data buffer
		if( wItem == ICDPC_IC_PDOL){															
			if( wTag == ICDPC_TM_SM_ALGORITHM ){												
				F_InfoSet(ICDPC_TM_USE_ALGORITHM, byDOLRelationDataBuf,wVnLength, lpwReturn);	
				if( *lpwReturn != OK ){															
					return OK;																	
				}																				
			}																					
		}																						
		memcpy( lpbyDOLRelationDataPtr, byDOLRelationDataBuf, wVnLength );
		// pointer update
		lpbyDOLRelationDataPtr += wVnLength;
	}
	// Set the DOL tag name in the PDOL tag list
	if( wItem == ICDPC_IC_PDOL ){
		// buffer Initialization
		memset( byDOLRelationDataBuf, 0x00, 252 );
		// TLV generate
		N_SetTLV( ICDPC_TM_COMMAND_TEMPLATE, lpbyDOLRelationDataPtr - lpbyStartPtr,
			lpbyStartPtr, 252, byDOLRelationDataBuf, &wDataSize, &wTLSize, lpwReturn );
		if( *lpwReturn != OK ){
			return OK;
		}
		F_InfoSet( ICDPC_TM_COMMAND_TEMPLATE, byDOLRelationDataBuf, wDataSize, lpwReturn );
		if( *lpwReturn != OK ){
			return OK;
		}
	}

	// Related data set
	switch( wItem ){
	case ICDPC_IC_PDOL:
		F_InfoSet( ICDPC_PDOL_RELATION_DATA, lpbyStartPtr, lpbyDOLRelationDataPtr - lpbyStartPtr, lpwReturn );
		break;
	case ICDPC_IC_CDOL1:
		F_InfoSet( ICDPC_CDOL1_RELATION_DATA, lpbyStartPtr, lpbyDOLRelationDataPtr - lpbyStartPtr, lpwReturn );
		break;
	case ICDPC_IC_CDOL2:
		F_InfoSet( ICDPC_CDOL2_RELATION_DATA, lpbyStartPtr, lpbyDOLRelationDataPtr - lpbyStartPtr, lpwReturn );
		break;
	case ICDPC_IC_DDOL:
		F_InfoSet( ICDPC_DDOL_RELATION_DATA, lpbyStartPtr, lpbyDOLRelationDataPtr - lpbyStartPtr, lpwReturn );
		break;
	case ICDPC_IC_TDOL:																							 
		F_InfoSet( ICDPC_TDOL_RELATION_DATA, lpbyStartPtr, lpbyDOLRelationDataPtr - lpbyStartPtr, lpwReturn );	 
		break;																									 
	case ICDPC_IC_STATIC_DATA_AUTHENTICATION_TAG_LIST:
		F_InfoSet( ICDPC_SDOL_RELATION_DATA, lpbyStartPtr, lpbyDOLRelationDataPtr - lpbyStartPtr, lpwReturn );
		break;
	default:
		//N_Alarm( INTERFACE_ERR, APMSG_ICDPC_DOLRELATIONDATAMAKE );
		*lpwReturn = NG;
	}
	return OK;
}


////////////////////////////////////////////////////////////////////////////////
// FuncClass= External Function
// FuncName	= F_PDLOTLVAAnalysis
// CallType	= Function
// Detail	= Analysis PDOL tag 
// Valuable	=
//			  WORD	:wItem		:input
//			  PDOL* :lpPDOLTagList	:output
//			  WORD	:wPDOLTagCount	:output
//			  LPWORD:lpwReturn		:output
//				OK
//				NG
//				FORMAT_ERR
// Return	=
// History  = 2015/09/10, XiongJie
////////////////////////////////////////////////////////////////////////////////
long UPICD::F_DOLTLVAnalysis(WORD wItem, DOL* lpDOLTagList, LPWORD wDOLTagCount, LPWORD lpwReturn)
{
	BYTE	byDOL[256] = {0x00};
	WORD	wDOLSize = 0;
	LPBYTE	lpbyData_WK;
	LPBYTE	lpbyData_END;
	WORD	wTag = 0;
	WORD	wTagSize = 0;

	*wDOLTagCount = 0;

	// DOL标签下的数据
	F_InfoGet(wItem, byDOL, sizeof(byDOL), &wDOLSize, lpwReturn);
	if( *lpwReturn != OK ){
		return OK;
	}

	lpbyData_WK = byDOL;
	lpbyData_END = lpbyData_WK + wDOLSize;

	// 分析DOL
	while(lpbyData_WK < lpbyData_END)
	{
		N_GetTagInfo(lpbyData_WK, lpbyData_END - lpbyData_WK, &wTag, &wTagSize, lpwReturn);
		switch( *lpwReturn )
		{
		case NG:
			return OK;
		case FORMAT_ERR:
			return OK;
		default:
			break;
		}

		lpDOLTagList[*wDOLTagCount].wTag = wTag;
		lpDOLTagList[*wDOLTagCount].wDataSize = lpbyData_WK[wTagSize];
		(*wDOLTagCount)++;

		lpbyData_WK += wTagSize + 1;
	}	

	return OK;
}

////////////////////////////////////////////////////////////////////////////////
// FuncClass= External Function
// FuncName	= F_PDLOTLVAAnalysis
// CallType	= Function
// Detail	= Analysis PDOL tag 
// Valuable	= LPBYTE:lpbyData		:input
//			  WORD	:wDataSize		:input
//			  PDOL* :lpPDOLTagList	:output
//			  WORD	:wPDOLTagCount	:output
//			  LPWORD:lpwReturn		:output
//				OK
//				NG
//				FORMAT_ERR
// Return	=
// History  = 2015/09/10, XiongJie
////////////////////////////////////////////////////////////////////////////////
long UPICD::F_ApplicationTemplateTLVAnalysis(LPBYTE lpbyData, WORD wDataSize, LPWORD lpwReturn)
{
	LPBYTE lpbyData_WK = lpbyData;
	LPBYTE lpbyData_END = lpbyData_WK + wDataSize;
	WORD wAnalysisSize = 0;
	WORD wVnLength = 0;
	WORD wTag = 0;
	WORD wTagSize = 0;

	while(lpbyData_WK < lpbyData_END)
	{
		N_TLVFormatAnalysis( NULL, OPTION, lpbyData_WK, lpbyData_END - lpbyData_WK,	 
			&wAnalysisSize, &wVnLength, lpwReturn );
		if( *lpwReturn != OK ){
			return OK;
		}

		N_GetTagInfo(lpbyData_WK, lpbyData_END - lpbyData_WK, &wTag, &wTagSize, lpwReturn);
		switch( *lpwReturn )
		{
		case NG:
			return OK;
		case FORMAT_ERR:
			return OK;
		default:
			break;
		}

		lpbyData_WK += wAnalysisSize;

		F_InfoSet( wTag, lpbyData_WK, wVnLength, lpwReturn );
		if( *lpwReturn != OK ){
			return OK;
		}

		lpbyData_WK += wVnLength;
	}
	
	return OK;
}

// 脚本脚本标识解析
// lpbyData		: input	: 输入的包含脚步命令的报文
// wDataSize	: input	: 输入报文的长度
long UPICD::F_ScriptTLVAnalysis(LPBYTE lpbyData, WORD wDataSize, LPWORD lpwReturn)
{
	LPBYTE lpbyData_WK = lpbyData;
	LPBYTE lpbyData_END = lpbyData_WK + wDataSize;
	WORD wAnalysisSize = 0;
	WORD wVnLength = 0;
	WORD wTag = 0;
	WORD wTagSize = 0;

	while(lpbyData_WK < lpbyData_END)
	{
		N_TLVFormatAnalysis( NULL, OPTION, lpbyData_WK, lpbyData_END - lpbyData_WK,	 
			&wAnalysisSize, &wVnLength, lpwReturn );
		if( *lpwReturn != OK ){
			return OK;
		}

		N_GetTagInfo(lpbyData_WK, lpbyData_END - lpbyData_WK, &wTag, &wTagSize, lpwReturn);
		switch( *lpwReturn )
		{
		case NG:
			return OK;
		case FORMAT_ERR:
			return OK;
		default:
			break;
		}

		lpbyData_WK += wAnalysisSize;

		if (ICDPC_ISSUER_SCRIPT_TEMPLATE1 == wTag || ICDPC_ISSUER_SCRIPT_TEMPLATE2 == wTag)
		{
			F_InfoSet( wTag, lpbyData_WK, wVnLength, lpwReturn );
			if( *lpwReturn != OK )
			{
				return OK;
			}
		}
		
		lpbyData_WK += wVnLength;
	}

	return OK;
}

// 解析71或72标签下的数据
// lpbyData			: input : 71或72标签下的数据 十六进行数组数据
// wDataSize		: input : 数据长度
// chScriptCmdData	: output: 解析出来的所有的脚步命令（0x86标签）的数据，以&分割 xxxx&xxxx，  字符串类型
// chScriptID		: output: 发卡行脚本标识（0x9F18）标签下是数据， 字符串类型
long UPICD::F_IssuerScriptTemplateAnalySis(LPBYTE lpbyData, WORD wDataSize, CHAR* chScriptCmdData, LPWORD lpwScriptCmdCount, CHAR* chScriptID, LPWORD lpwReturn)
{
	LPBYTE lpbyData_WK = lpbyData;
	LPBYTE lpbyData_END = lpbyData_WK + wDataSize;
	WORD wAnalysisSize = 0;
	WORD wVnLength = 0;
	WORD wTag = 0;
	WORD wTagSize = 0;
	*lpwScriptCmdCount = 0;

	while(lpbyData_WK < lpbyData_END)
	{
		N_TLVFormatAnalysis( NULL, OPTION, lpbyData_WK, lpbyData_END - lpbyData_WK,	 
			&wAnalysisSize, &wVnLength, lpwReturn );
		if( *lpwReturn != OK ){
			return OK;
		}

		N_GetTagInfo(lpbyData_WK, lpbyData_END - lpbyData_WK, &wTag, &wTagSize, lpwReturn);
		switch( *lpwReturn )
		{
		case NG:
			return OK;
		case FORMAT_ERR:
			return OK;
		default:
			break;
		}

		lpbyData_WK += wAnalysisSize;

		if (ICDPC_ISSUER_SCRIPT_COMMAND == wTag)
		{
			CHAR chTemp[TAGDATASIZE] = {0};
			N_HEXtoASC((LPBYTE)chTemp, lpbyData_WK, wVnLength, lpwReturn);
			chTemp[wVnLength*2 + 1] = '&';

			strcat(chScriptCmdData, chTemp);
			(*lpwScriptCmdCount)++;
		}
		else if (ICDPC_ISSUER_SCRIPIT_ID == wTag)
		{
			N_HEXtoASC((LPBYTE)chScriptID, lpbyData_WK, wVnLength, lpwReturn);
		}

		lpbyData_WK += wVnLength;
	}

	//chScriptCmdData[strlen(chScriptCmdData) - 1] = 0; //modify by LeeHai 2015.12.31 //解决脚本分析末尾少发了一个字符
	chScriptCmdData[strlen(chScriptCmdData)] = 0;
	return OK;
}

////////////////////////////////////////////////////////////////////////////////
// FuncClass= External Function
// FuncName	= ASCtoHEX
// CallType	= Function
// Detail	= switch ＡＳＣＩＩto ＨＥＸ 
// Valuable	= LPBYTE:lpDest		:output
//			  LPBYTE:lpSrc		:input
//			  WORD	:wSize		:input
//			  LPWORD:lpwReturn	:output
//				OK
//				NG
// Return	=
////////////////////////////////////////////////////////////////////////////////
void UPICD::N_ASCtoHEX( LPBYTE lpDest, LPBYTE lpSrc, WORD wSize, LPWORD lpwReturn )
{
	int High, Low;

	*lpwReturn = OK;

	if( wSize % 2 ){
		*lpwReturn = NG;
		return;
	}

	for( int i = 0; i < wSize; i += 2 ){
		if( lpSrc[i] >= '0' && lpSrc[i] <= '9' )
			High = lpSrc[i] - '0';
		else if( lpSrc[i] >= 'A' && lpSrc[i] <= 'F' )
			High = lpSrc[i] - 'A' + 10;
		else if( lpSrc[i] >= 'a' && lpSrc[i] <= 'f' )
			High = lpSrc[i] - 'a' + 10;
		else {
			*lpwReturn = NG;
			return;
		}
		if( lpSrc[i+1] >= '0' && lpSrc[i+1] <= '9' )
			Low = lpSrc[i+1]-'0';
		else if( lpSrc[i+1] >= 'A' && lpSrc[i+1] <= 'F' )
			Low = lpSrc[i+1] - 'A' + 10;
		else if( lpSrc[i+1] >= 'a' && lpSrc[i+1] <= 'f' )
			Low = lpSrc[i+1] - 'a' + 10;
		else {
			*lpwReturn = NG;
			return;
		}
		*lpDest++ = BYTE(High * 16 + Low);
	}
}

////////////////////////////////////////////////////////////////////////////////
// FuncClass= External Function
// FuncName	= JIStoBCD
// CallType	= Function
// Detail	= JIS to BCD
// Valuable	= LPBYTE:lpbyInput		:input
//			  WORD	:wJisSize		:input
//			  LPBYTE:lpbyOutput		:output
//			  LPWORD:lpwBcdSize		:output
//			  LPWORD:lpwReturn		:output
//				OK
//				NG
// Return	= OK
////////////////////////////////////////////////////////////////////////////////
long UPICD::N_JIStoBCD( LPBYTE lpbyInput, WORD wJisSize,
	LPBYTE lpbyOutput, WORD wBcdSize, LPWORD lpwBcdSize, LPWORD lpwReturn )
{
	WORD i = 0;
	WORD wHigh = 0;
	WORD wEnglish = 0;

	if( lpwReturn == NULL ){
		return OK;
	}
	if( lpbyOutput == NULL ){
		*lpwReturn = NG;
		return OK;
	}

	*lpwBcdSize = 0;
	*lpwReturn = OK;

	if( wJisSize/2 > wBcdSize ){
		*lpwReturn = NG;
		return OK;
	}

	if( (wJisSize % 2) == 0 ){
		wHigh = ON;
	}else{
		wHigh = OFF;
	}
	*lpbyOutput = 0;
	for( i=0;i<wJisSize;i++ ){
		if( (*(lpbyInput+i) >= 0x41) && (*(lpbyInput+i) <= 0x46) || (*(lpbyInput+i) >= 0x61) && (*(lpbyInput+i) <= 0x66) ){
			wEnglish = 9;
		}else {
			wEnglish = 0;
		}
		if( wHigh == ON ){
			*(lpbyOutput + (*lpwBcdSize)) = ((*(lpbyInput+i) & 0x0f) + LOBYTE(wEnglish)) << 4;
			wHigh = OFF;
		}else {
			*(lpbyOutput + (*lpwBcdSize)) = *(lpbyOutput + *lpwBcdSize) | ((*(lpbyInput+i) & 0x0f) + LOBYTE(wEnglish));
			wHigh = ON;
			(*lpwBcdSize)++;
		}
	}
	return OK;
}
