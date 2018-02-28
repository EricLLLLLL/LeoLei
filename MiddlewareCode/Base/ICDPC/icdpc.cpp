
#define	 ICDPCLIB
#include "icdpc.h"
#include "ICDAnalysis.h"

CICDPC::CICDPC(void)
{
	m_Icd=new CICDAnalysis();
}


CICDPC::~CICDPC(void)
{
	if (m_Icd != NULL)
	{
		delete m_Icd;
		m_Icd=NULL;
	}
}


int CICDPC::ICDPC_Init(CHAR* configPath)
{
	int iResult = OK;
	iResult = m_Icd->ICD_Init(configPath);

	return iResult;
}


int CICDPC::ICDPC_AnalysisR_Apdu(WORD Command,CHAR* Apdu)
{
	int IResult=OK;

	IResult=m_Icd->ICD_RApduAnalysis(Command, Apdu);
	if (OK!=IResult)
	{
		return IResult;
	}

	return IResult;
}


int CICDPC::ICDPC_GetC_Apdu(WORD Command,CHAR* Apdu, CHAR* aIn, int Count)
{
	int IResult=OK;

	IResult=m_Icd->ICD_CApduAssorted(Command,Apdu, aIn, Count);
	if (OK!=IResult)
	{
		return IResult;
	}

	return IResult;
}

int CICDPC::ICDPC_GetPSEAID(CHAR* aOut)
{
	int IResult=OK;

	IResult=m_Icd->ICD_GetPSEAID(aOut);
	if (OK!=IResult)
	{
		return IResult;
	}

	return IResult;
}

//int CICDPC::ICDPC_SelectADF(CHAR* aIn)
//{
//	int IResult=OK;
//	//IResult=m_Icd.ICDPC_SelectADF(aIn);
//	return IResult;
//}

int CICDPC::ICDPC_GetProOption(CHAR* aOut)
{
	int IResult=OK;

	IResult=m_Icd->ICD_GetProOption(aOut);
	if (OK!=IResult)
	{
		return IResult;
	}

	return IResult;
}

//int CICDPC::ICDPC_ReadAppData(CHAR* aAFL)
//{
//	int IResult=OK;
//	//IResult=m_Icd.ICDPC_ReadAppData(aAFL);
//	return IResult;
//}

int CICDPC::ICDPC_ReadBalance(CHAR* aOut)
{
	int IResult=OK;

	IResult=m_Icd->ICD_ReadBalance(aOut);
	if (OK!=IResult)
	{
		return IResult;
	}

	return IResult;
}

int CICDPC::ICDPC_ReadBalanceLimit(CHAR* aOut)
{
	int IResult=OK;
	
	IResult=m_Icd->ICD_BalanceLimit(aOut);
	if (OK!=IResult)
	{
		return IResult;
	}

	return IResult;
}

int CICDPC::ICDPC_CardDateInfo(CHAR* aOut)
{
	int IResult=OK;

	IResult=m_Icd->ICD_CardDateInfo(aOut);
	if (OK!=IResult)
	{
		return IResult;
	}

	return IResult;
}

int CICDPC::ICDPC_GetHolderInfo(CHAR* aOut)
{
	int IResult=OK;

	IResult=m_Icd->ICD_GetHolderInfo(aOut);
	if (OK!=IResult)
	{
		return IResult;
	}

	return IResult;
}

int CICDPC::ICDPC_CheckDataAuthentication(BOOL* bSupportCDA, BOOL* bSupportDDA, BOOL* bSupportSDA)
{
	int IReturn = OK;
	IReturn = m_Icd->ICD_DaDataCheck(bSupportCDA, bSupportDDA, bSupportSDA);
	return IReturn;
}

int CICDPC::ICDPC_ProcessLimit()
{
	int IReturn = OK;
	IReturn = m_Icd->ICD_ProcessLimit();
	return IReturn;
}

int CICDPC::ICDPC_TeminalActiveAnaiysis(CHAR* C_APDU)
{
	int IReturn = OK;
	IReturn = m_Icd->ICD_TemActiveAnaiysis(C_APDU);
	return IReturn;
}

int CICDPC::ICDPC_InitForLoad(CHAR* CardDataExc, CHAR* ForArqc, CHAR* Arqc)
{
	int IResult = OK;
	IResult = m_Icd->ICD_InitForLoad(CardDataExc, ForArqc, Arqc);
	return IResult;
}

int CICDPC::ICDPC_ReadIcTLV(CHAR* aPAan, CHAR* aInAD, CHAR* aInAR, CHAR* aOut)
{
	int IResult=OK;
	IResult=m_Icd->ICD_Re55FieldDataExc(aInAR, aOut);//lihai modify 2015-12-30
	return IResult;
}

int CICDPC::ICDPC_ExternalAuth(CHAR* R_APDU, CHAR* chStatus)
{
	int IResult=OK;
	IResult=m_Icd->ICD_ExternalAuth(R_APDU, chStatus);
	return IResult;
}

int CICDPC::ICDPC_AnalyScript(CHAR* aInscript, CHAR* ch71ScriptID, CHAR* ch72ScriptID, CHAR* ch71ScriptCmdList, LPWORD lpw71ScriptCmdCount, CHAR* ch72ScriptCmdList, LPWORD lpw72ScriptCmdCount)
{
	int IResult=OK;
	IResult=m_Icd->CAPDU_ScriptAnalysis(aInscript, ch71ScriptID, ch72ScriptID, ch71ScriptCmdList, lpw71ScriptCmdCount, ch72ScriptCmdList, lpw72ScriptCmdCount);
	return IResult;
}

int CICDPC::ICDPC_FinishLoad(CHAR* aOut)
{
	int IResult=OK;
	IResult=m_Icd->ICD_FinishLoad(aOut);
	return IResult;
}

int CICDPC::ICDPC_ScriptResult(CHAR* chTag, WORD wNum, CHAR* chScriptID, CHAR* R_APDU, CHAR* ReScript)
{
	int IResult=OK;
	IResult = m_Icd->ICD_ScriptResult(chTag, wNum, chScriptID, R_APDU, ReScript);
	return IResult;
}

int CICDPC::ICDPC_ReadLog(CHAR* aIn, CHAR* aOut)
{
	int IResult=OK;
	IResult = m_Icd->ICD_ReadLog(aIn, aOut);
	return IResult;
}

int CICDPC::ICDPC_GetTrack2Data(CHAR* aOut)
{
	int IResult=OK;
	IResult=m_Icd->ICD_GetTrack2Data(aOut);
	return IResult;
}


// R_Apdu分析后的状态返回
int CICDPC::ICDPC_AnalysisR_ApduStatus(CHAR* Apdu, LPWORD DataStatus)
{
	int IResult=OK;

	IResult=m_Icd->ICD_RApduStatusAnalysis(Apdu, DataStatus);
	if (OK!=IResult)
	{
		return IResult;
	}

	return IResult;
}

// 根据标签获取对应的数据
int CICDPC::ICDPC_InfoGet(WORD wItem, CHAR* chBuffer)
{
	int iResult = m_Icd->ICD_InfoGet(wItem, chBuffer);
	return iResult;
}

// pse选择capdu获取
int CICDPC::ICDPC_AssGetPSEAID_Capdu(CHAR* aPSERAID, CHAR* Apdu)
{
	int IResult=OK;

	IResult=m_Icd->CAPDU_GetPSEAID(aPSERAID,Apdu);
	if (OK!=IResult)
	{
		return IResult;
	}

	return IResult;
}



// 第一个ReadRecord命令，上层根据读取的返回值确定是否更改P1读下去，aCommond为Read命令
int CICDPC::ICDPC_AssSelectReadRecord_Capdu(WORD aCommand, WORD wP1, CHAR* C_APDU)
{
	int IResult = OK;
	CHAR chP1[3] = {0};

	sprintf(chP1, "%02d", wP1);

	IResult = m_Icd->CAPDU_SelReadRecord(aCommand, chP1, C_APDU);
	return IResult;
}


// select for gpo
int CICDPC::ICDPC_AssSelectADF_Capdu(CHAR* aADF, CHAR* C_Apdu)
{
	int IResult=OK;

	IResult=m_Icd->CAPDU_SelectADF(aADF, C_Apdu);
	if (OK!=IResult)
	{
		return IResult;
	}

	return IResult;
}


// Need Time /TradeMoneyType
int CICDPC::ICDPC_AssGetProOption_Capdu(int aTradeMoneyType, int aTradeMoney, int aTradeType, int aTime, CHAR* C_Apdu)
{
	int IResult=OK;

	IResult=m_Icd->CAPDU_GetProOption(aTradeMoneyType, aTradeMoney, 0, aTradeType, aTime, C_Apdu);
	if (OK!=IResult)
	{
		return IResult;
	}

	return IResult;
}


// 取得AFL,分析出C_Apdu[aCommandCount]
int CICDPC::ICDPC_AssReadAppData_Capdu(CHAR* aAfl, CHAR* C_Apdu)
{
	int IResult=OK;

	IResult=m_Icd->CAPDU_ReadAppData(aAfl, C_Apdu);
	if (OK!=IResult)
	{
		return IResult;
	}

	return IResult;
}


// Get Data command
int CICDPC::ICDPC_AssGetBalance_Capdu(CHAR* C_Apdu)
{
	int IResult=OK;

	IResult=m_Icd->CAPDU_GetBalance(C_Apdu);
	if (OK!=IResult)
	{
		return IResult;
	}

	return IResult;
}


// Get Balance limited Data by Getdata command
int CICDPC::ICDPC_AssBalanceLimit_Capdu(CHAR* C_Apdu)
{
	int IResult=OK;

	IResult=m_Icd->CAPDU_BalanceLimit(C_Apdu);
	if (OK!=IResult)
	{
		return IResult;
	}

	return IResult;
}

// Get ATC Data
int CICDPC::ICDPC_AssGetATCData_Capdu(CHAR* C_Apdu)
{
	int IResult=OK;
	IResult = m_Icd->CAPDU_GetAtcData(C_Apdu);
	return IResult;
}

// Get External Auth CAPDU
int CICDPC::ICDPC_AssExternalAuth_Capdu(CHAR* ARPCExc, CHAR* C_Apdu)
{
	int IResult=OK;
	IResult = m_Icd->CAPDU_ExternalAuth(ARPCExc, C_Apdu);
	return IResult;
}

// 获取卡片日志格式的C_APDU（GET DATA）
int CICDPC::ICDPC_GetLogFormat_Capdu(CHAR* C_Apdu)
{
	int iResult = m_Icd->CAPDU_GetLogFormat(C_Apdu);
	return iResult;
}

// 获取详细日志的C_APDU, P1：第P1条日志 （READ RECORD）
int CICDPC::ICDPC_ReadLog_Capdu(WORD P1, CHAR* C_Apdu)
{
	int iResult = m_Icd->CAPDU_ReadLog(P1, C_Apdu);
	return iResult;
}


// Aid选择方式的C_APDU
int CICDPC::ICDPC_AidSelect_Capdu(CHAR* strAid, CHAR* C_Apdu)
{
	int iResult = m_Icd->CAPDU_AidSelect(strAid, C_Apdu);
	return iResult;
}
