//////////////////////////////////////////////////////////////////////////////
//文件名			：ICDAnalysis.cpp 
//创建者			：LeeHai&Xiongjie
//创建时间		：2015-09-06 9:37
//功能描述		：ICDAnalysis的cpp实现。
//
//修改履历		：2015-09-06 9:37:26  LeeHai  新建
//
//////////////////////////////////////////////////////////////////////////////
#include "ICDAnalysis.h"
#include "mf.h"//xlc 2015-12-30 17:46 add
#include "Helper/helper.h"
#include "QDateTime"

MFile mf;//xlc 2015-12-30 17:46 add

char g_RandData[16] = {0};

CICDAnalysis::CICDAnalysis(void)
{
	upicd = new UPICD();

	memset(chCvr, 0x00, 11);
	RelatedDataCount = 0;
	memset(m_configpath, 0x00, sizeof(m_configpath));

	memset(m_ch71ScriptID, 0, sizeof(m_ch71ScriptID));
	memset(m_ch72ScriptID, 0, sizeof(m_ch72ScriptID));

	memset(m_ch71ScriptCmdList, 0, sizeof(m_ch71ScriptCmdList));
	memset(m_ch72ScriptCmdList, 0, sizeof(m_ch72ScriptCmdList));

	m_w71ScriptCmdCount = 0;
	m_w72ScriptCmdCount = 0;
	mf.mfinit();
}


CICDAnalysis::~CICDAnalysis(void)
{
	if (NULL != upicd)
	{
		delete upicd;
		upicd = NULL;
		mf.mfclose();
	}	
}


// 初始化PBOC配置，输入configPath配置文件路径
int CICDAnalysis::ICD_Init(CHAR* configPath)
{
	int iRet = OK;
	WORD wReturn = OK;
	memcpy(m_configpath, configPath, strlen(configPath));

    QString IniTradeMoney("TemTradeMoney");
    QString IniTradeMoneyExc("TemTradeMoneyExc");
    QString	IniCountry("TemCountry");
    QString	IniMoneyType("TemMoneyType");
    QString	IniTradeType("TemTradeType");
    QString	IniBankName("TemName");
    QString IniTemType("TemType");
    QString IniTemCapability("TemCapability");
    QString IniAdditionalCapability("TemAdditionalCapability");

    QString strConfigpath = S2Q(m_configpath);
    wT_TradeMoney=		g_Helper.GetPrivateProfileInt("Template", IniTradeMoney, 0,strConfigpath);//xuzhe tmp 0229
    wT_TradeMoneyExc=	g_Helper.GetPrivateProfileInt("Template", IniTradeMoneyExc, 0, strConfigpath);
    wT_Country=			g_Helper.GetPrivateProfileInt("Template", IniCountry, 156, strConfigpath);
    wT_MoneyType=		g_Helper.GetPrivateProfileInt("Template", IniMoneyType, 156, strConfigpath);
    wT_TradeType=		g_Helper.GetPrivateProfileInt("Template", IniTradeType, 0, strConfigpath);//xuzhe tmp 0229

    strT_BankName= g_Helper.GetPrivateProfileString("Template", IniBankName, "00", strConfigpath);
    strT_TemType = g_Helper.GetPrivateProfileString("Template", IniTemType, "22", strConfigpath);
    strT_TemCapability = g_Helper.GetPrivateProfileString("Template", IniTemCapability, "E0A080", strConfigpath);
    strT_AdditionalCapability = g_Helper.GetPrivateProfileString("Template", IniAdditionalCapability, "5000B0B001",  strConfigpath);
	
	upicd->F_Init(configPath, &wReturn);

    //xuzhe start add 20160930 
    RelatedDataCount = 0;
    //xuzhe end add 20160930

	return iRet;
}

// 根据标签获取对应的数据, 输入wItem为标签，输出chBuffer为对应标签的字符型值
int CICDAnalysis::ICD_InfoGet(WORD wItem, CHAR* chBuffer)
{
	int iRet = 0;
	BYTE byBuffer[DATALENGTH] = {0x00};
	WORD wDataSize = 0;
	WORD wResult = 0;

	iRet = upicd->F_InfoGet(wItem, byBuffer, sizeof(byBuffer), &wDataSize, &wResult);
	if (OK != wResult)
	{
		return wResult;
	}

	HEXtoASC((LPBYTE)chBuffer, byBuffer, wDataSize, &wResult);

	return OK;
}


// 分析RApdu，参数为CHAR*结构/数据长度Length
int CICDAnalysis::ICD_RApduAnalysis(WORD Command, CHAR* Apdu)
{
	WORD wReturn = OK;
	BYTE data[1024] = {0x00};

	ASCtoHEX(data, (LPBYTE)Apdu, strlen(Apdu) ,&wReturn);
	if (OK != wReturn)
	{
		return wReturn;
	}

	upicd->F_ICResponseMessageDataAnalysis(Command, data, strlen(Apdu)/2, &wReturn);
	if (OK != wReturn)
	{
		return wReturn;
	}

	return wReturn;
}


// 组织C_Apdu，参数为CHAR*结构/数据长度Length/数据次数
//详细细节
int CICDAnalysis::ICD_CApduAssorted(WORD Command,CHAR* Apdu, CHAR *aData, int Count)
{
	return OK;
}


// 转换 "000A152F" 形式的字符串为 {0x00, 0x0A, 0x15, 0x2F}形式的16进制数组 
void CICDAnalysis::ASCtoHEX( LPBYTE lpDest, LPBYTE lpSrc, WORD wSize, LPWORD lpwReturn )
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

// 转换 {0x00, 0x0A, 0x15, 0x2F}形式的16进制数组为 "000A152F" 形式的字符串
void CICDAnalysis::HEXtoASC( LPBYTE lpDest, LPBYTE lpSrc, WORD wSize, LPWORD lpwReturn )
{
	char Number[]="0123456789ABCDEF";

	*lpwReturn = OK;

	for( int i = 0; i < wSize; i++ )
	{
		*lpDest++ = Number[lpSrc[i] >> 4];
		*lpDest++ = Number[lpSrc[i] & 0x0f];
	}
}

// Analysis GetPSEAID APDU
int CICDAnalysis::CAPDU_GetPSEAID(CHAR* aPSEAID,CHAR* C_Apdu)
{
    QString C_Apdubuf = "";
    QString ClaInsP1P2 = "00A40400";
    QString le = "00";
	CHAR PSEAID[DATALENGTH];
	memset(PSEAID,0x00,sizeof(PSEAID));
	int alen = strlen(aPSEAID);

	for (int i=0; i<alen; i++)
	{
		sprintf(&PSEAID[i * 2], "%02X", aPSEAID[i]);
	}
    C_Apdubuf.sprintf("%s%02X%s%s", Q2S(ClaInsP1P2), alen, PSEAID, Q2S(le));
    strncpy(C_Apdu, Q2S(C_Apdubuf),C_Apdubuf.length());
	//C_ApduLength = C_Apdubuf.GetLength();


	return 0;	
}


// 选择命令紧接着的ReadRecord命令
int CICDAnalysis::CAPDU_SelReadRecord(WORD wCommand, CHAR* P1, CHAR* C_APDU)
{
	WORD wReturn = OK;
	CHAR ClaIns[5] = "00B2";
	CHAR le[3] = "00";

	BYTE bySfi[DATALENGTH];
	//CHAR chSfi[DATALENGTH];
	WORD wSfiSize = 0;
	
	memset(bySfi, 0x00, sizeof(bySfi));
	//memset(chSfi, 0x00, sizeof(chSfi));

	upicd->F_InfoGet(ICDPC_IC_SFI, bySfi, sizeof(bySfi), &wSfiSize, &wReturn);
	if (OK != wReturn)
	{
		return wReturn;
	}
	//ASCtoHEX((LPBYTE)chSfi, bySfi, wSfiSize , &wReturn);

	//计算P2
	BYTE byP2=(bySfi[0]<< 3)| 0x0C;
	if (ICDPC_SELECT_PSE == wCommand)
	{
		//P2.Format("%02X",byP2);
		sprintf(C_APDU, "%s%s%02X%s", ClaIns, P1, byP2, le);
	}

	return wReturn;
}


// RApdu的SW1SW2的状态值分析
int CICDAnalysis::ICD_RApduStatusAnalysis(CHAR* Apdu, LPWORD DataStatus)
{
	WORD wReturn = OK;
	BYTE data[1024] = {0x00};
	ASCtoHEX(data, (LPBYTE)Apdu, strlen(Apdu) ,&wReturn);
	if (OK != wReturn)
	{
		return wReturn;
	}

	upicd->F_ICResponseMessageDataStatusAnalysis(data, strlen(Apdu)/2, DataStatus, &wReturn);
	if (OK != wReturn)
	{
		return wReturn;
	}

	return wReturn;
}


// 选择文件方式选择应用，
//aOut：应用需要的文件数据组合AD/LB/PT（应用路径标识/应用标签/应用优先级
int CICDAnalysis::ICD_GetPSEAID(CHAR* aOut)
{
	WORD wReturn = OK;
	BYTE byAD[DATALENGTH];
	BYTE byLB[DATALENGTH];
	BYTE byPT[DATALENGTH];

	CHAR chAD[DATALENGTH] = {0};
	CHAR chLB[DATALENGTH] = {0};
	CHAR chPT[DATALENGTH] = {0};
	
	WORD ADSize = 0;
	WORD LBSize = 0;
	WORD PTSize = 0;
	memset(byAD, 0x00, sizeof(byAD));
	memset(byLB, 0x00, sizeof(byLB));
	memset(byPT, 0x00, sizeof(byPT));

	BYTE lpbyAppTemplateData[COMMANDMESSAGEDATASIZE] = {0x00};
	WORD wAppTemplateDataSize = 0;

	// 先取出ICDPC_IC_APPLICATION_TEMPLATE（0x61）标签下的数据
	upicd->F_InfoGet(ICDPC_IC_APPLICATION_TEMPLATE, lpbyAppTemplateData, sizeof(lpbyAppTemplateData), &wAppTemplateDataSize, &wReturn);
	if (OK != wReturn)
	{
		return wReturn;
	}

	// 解析ICDPC_IC_APPLICATION_TEMPLATE（0x61）标签下的数据，并存入数组
	upicd->F_ApplicationTemplateTLVAnalysis(lpbyAppTemplateData, wAppTemplateDataSize, &wReturn);
	if (OK != wReturn)
	{
		return wReturn;
	}

	// 分别取出应用路径标识、应用标签和应用优先级标签下的数据
	upicd->F_InfoGet(ICDPC_IC_AID, byAD, sizeof(byAD), &ADSize, &wReturn);						//AID
	if (OK != wReturn)
	{
		return wReturn;
	}

	upicd->F_InfoGet(ICDPC_IC_APPLICATION_LABEL, byLB, sizeof(byLB), &LBSize, &wReturn);			//LB
	if (OK != wReturn)
	{
		return wReturn;
	}

	upicd->F_InfoGet(ICDPC_IC_API, byPT, sizeof(byPT), &PTSize, &wReturn);						//PT:C
	if (OK != wReturn)
	{
		byPT[0] = 0x01;
		PTSize = 1;
	}

	// 十六进制转换为字符串
	HEXtoASC((LPBYTE)chAD, byAD, ADSize, &wReturn);
	HEXtoASC((LPBYTE)chLB, byLB, LBSize, &wReturn);
	HEXtoASC((LPBYTE)chPT, byPT, PTSize, &wReturn);

	sprintf(aOut, "%s&%s&%s", chAD, chLB, chPT);
	return wReturn;
}


// GPO命令前的Select命令，确定和选择应用，通过调用这条命令获得PDOL
//输入数据wADF为LC+Data;
int CICDAnalysis::CAPDU_SelectADF(CHAR* wADF, CHAR* C_Apdu)
{
	WORD wReturn = OK;
    QString CApdubuf="";
    QString ClaInsP1P2 = "00A40400";
    QString Le = "00";
    QString Lc = "";
	WORD wADFSize = strlen(wADF) / 2;

    Lc.sprintf("%02X", wADFSize);
	CApdubuf += ClaInsP1P2 + Lc + wADF +Le;
    strncpy(C_Apdu, Q2S(CApdubuf),CApdubuf.size());
	//C_ApduLength = CApdubuf.GetLength();

	return wReturn;
}


// 选择应用，具体没有操作
int CICDAnalysis::ICD_SelectADF(void)
{
	WORD wReturn = OK;

	return wReturn;
}

// Aid列表选择方式
int CICDAnalysis::CAPDU_AidSelect(CHAR *strAid, CHAR* C_APDU)
{
	WORD wReturn = OK;
    QString ClaInsP1P2("00A40400");
    QString Le = "00";
    QString Lc = "";
	//CHAR	chAid[DATALENGTH] = {"A0000003330101"};


	int iAidLen = (strlen(strAid))/2;
    Lc.sprintf("%02X%s", iAidLen, strAid);

    sprintf(C_APDU, "%s%s%s", Q2S(ClaInsP1P2), Q2S(Lc), Q2S(Le));

	return wReturn;
}

// 应用初始化，CApdu为输出参数
//wTradeMoneyType	9F1A,9F2A  02	 终端国家代码与终端货币代码
//wTradeMoney		9F02	   06	 数值型
//wTradeType		9C		   01	 交易类型

//GPO命令：
//	80A8000023		head
//	8321F680		9f66
//	0000 0000 0000	9f02	money
//	0100				
//	000000000000	9f03
//	0156			9f1a
//	0000000000		95		tvr
//	0156			5f2a
//	150819			9A      time
//	00				9C      type
//	221D2220		9f37		不可预知数

//modify by LeeHai for ABC
//wTime has been delected
int CICDAnalysis::CAPDU_GetProOption(int wTradeMoneyType, int wTradeMoney, int wMoneyExc, int wTradeType, int wTime, CHAR* CApdu)
{
	WORD wReturn = OK;
	DOL PDOLTagList[DOLTAGCOUNT];
	WORD wPDOLTagCount = 0;
	BYTE byTagData[TAGDATASIZE] = {0x00};
	WORD lpwCommandVDataFieldTBL[DOLTAGCOUNT] = {0x00};
	WORD wCommandVDataFieldTBLLength = 0;
	BYTE byCAPDU[COMMANDMESSAGEDATASIZE] = {0x00};
	WORD wCAPDUDataSize = 0;
	CHAR chTerminalCountryType[5] = {0};    // 终端国家代码
	CHAR chTradeMoney[13] = {0};			//授权金额
	CHAR chMoneyExc[13] = {0};				//其他金额
	CHAR chTradeMoneyType[5] = {0};			// 交易货币代码
	CHAR chTradeType[3] = {0};
	CHAR chTime[7] = {0};

	BYTE byCLA = 0x80;
	BYTE byINS = 0xA8;
	BYTE byP1 = 0x00;
	BYTE byP2 = 0x00;
	BYTE byLe = 0x00;

	time_t timep;
	struct tm *p;
	time(&timep);
	p = localtime(&timep); //取得当地时间
	sprintf(chTime, "%02d%02d%02d", (p->tm_hour), (p->tm_min), (p->tm_sec));
	

	// 解析PDOL数据，得到PDOL的Tag列表及每个Tag需要的数据长度
	upicd->F_DOLTLVAnalysis(ICDPC_IC_PDOL, PDOLTagList, &wPDOLTagCount, &wReturn);
	if (OK != wReturn)
	{
		wPDOLTagCount  = 0;
		//return wReturn;
	}

	for (int i = 0; i < wPDOLTagCount; i++)
	{
		memset(byTagData, 0x00, sizeof(byTagData));

		switch(PDOLTagList[i].wTag)
		{
		case 0x9F66:    
			byTagData[0] = 0xF6;
			break;
		case ICDPC_TM_AMOUNT_AUTHORISED:							//授权金额
			//xuzhe start modify 20160301
			//sprintf(chTradeMoney, "%12d", wT_TradeMoney);				//从配置中读取
			sprintf(chMoneyExc, "%12d", wTradeMoney);							//从形参中得到
			//xuzhe end 
			ASCtoHEX(byTagData, (LPBYTE)chTradeMoney, strlen(chTradeMoney), &wReturn);

			myRelatedData[RelatedDataCount].Tag = ICDPC_TM_AMOUNT_AUTHORISED;
			myRelatedData[RelatedDataCount].Length = strlen(chTradeMoney)/2;
			sprintf(myRelatedData[RelatedDataCount].Value, "%s", chTradeMoney);
			RelatedDataCount += 1;
			break;
		case ICDPC_TM_AMOUNT_OTHER:			
			sprintf(chMoneyExc, "%12d", wT_TradeMoneyExc);					//从配置中读取
			ASCtoHEX(byTagData, (LPBYTE)chMoneyExc, strlen(chMoneyExc), &wReturn);

			myRelatedData[RelatedDataCount].Tag = ICDPC_TM_AMOUNT_OTHER;
			myRelatedData[RelatedDataCount].Length = strlen(chMoneyExc)/2;
			sprintf(myRelatedData[RelatedDataCount].Value, "%s", chMoneyExc);
			RelatedDataCount += 1;
			break;
		case ICDPC_TM_TERMINAL_COUNTRY_CODE:    // 终端国家代码
			sprintf(chTerminalCountryType, "%04d", wT_Country);     // 从配置中读取
			ASCtoHEX(byTagData, (LPBYTE)chTerminalCountryType, strlen(chTerminalCountryType), &wReturn);

			myRelatedData[RelatedDataCount].Tag = ICDPC_TM_TERMINAL_COUNTRY_CODE;
			myRelatedData[RelatedDataCount].Length = strlen(chTerminalCountryType)/2;
			sprintf(myRelatedData[RelatedDataCount].Value, "%s", chTerminalCountryType);
			RelatedDataCount += 1;
			break;
		case ICDPC_TM_TRANSACTION_CURRENCY_CODE:    // 交易货币代码
			sprintf(chTradeMoneyType, "%04d", wT_MoneyType); // 从配置中读取
			ASCtoHEX(byTagData, (LPBYTE)chTradeMoneyType, strlen(chTradeMoneyType), &wReturn);

			myRelatedData[RelatedDataCount].Tag = ICDPC_TM_TRANSACTION_CURRENCY_CODE;
			myRelatedData[RelatedDataCount].Length = strlen(chTradeMoneyType)/2;
			sprintf(myRelatedData[RelatedDataCount].Value, "%s", chTradeMoneyType);
			RelatedDataCount += 1;
			break;
		case ICDPC_TM_TRANSACTION_DATE:     // 交易日期
			//sprintf(chTime, "%06d", wTime);
			ASCtoHEX(byTagData, (LPBYTE)chTime, strlen(chTime), &wReturn);

			myRelatedData[RelatedDataCount].Tag = ICDPC_TM_TRANSACTION_DATE;
			myRelatedData[RelatedDataCount].Length = strlen(chTime)/2;
			sprintf(myRelatedData[RelatedDataCount].Value, "%s", chTime);
			RelatedDataCount += 1;
			break;
		case ICDPC_TM_TRANSACTION_TYPE:     // 交易类型
			//xuzhe start modify 20160301
			//sprintf(chTradeType, "%02d", wT_TradeType);
			sprintf(chTradeType, "%02d", wTradeType);							//从形参中得到
			//xuzhe end 
			ASCtoHEX(byTagData, (LPBYTE)chTradeType, strlen(chTradeType), &wReturn);

			myRelatedData[RelatedDataCount].Tag = ICDPC_TM_TRANSACTION_TYPE;
			myRelatedData[RelatedDataCount].Length = strlen(chTradeType)/2;
			sprintf(myRelatedData[RelatedDataCount].Value, "%s", chTradeType);
			RelatedDataCount += 1;
			break;

		default:
			memset(byTagData, 0x00, PDOLTagList[i].wDataSize);
			break;
		}

		// 把PDOL中的各标签对应的数据添加到数据数组中
		upicd->F_InfoSet(PDOLTagList[i].wTag, byTagData, PDOLTagList[i].wDataSize, &wReturn);
		if (OK != wReturn)
		{
			return wReturn;
		}

		lpwCommandVDataFieldTBL[i] = PDOLTagList[i].wTag;
		wCommandVDataFieldTBLLength++;
		
	}

	// 组合C_APDU命令
	upicd->F_GetCommandMessage(byCLA, byINS, byP1, byP2, COMMAND_MESSAGE_FORMAT_CLA_INS_P1_P2, byLe, 
								lpwCommandVDataFieldTBL, wCommandVDataFieldTBLLength, 0, 0, 
								byCAPDU, sizeof(byCAPDU), &wCAPDUDataSize, &wReturn);
	if (OK != wReturn)
	{
		return wReturn;
	}

	// 此命令组合中缺少0x83标签和0x83标签下数据的长度，下面加上
	BYTE tempCAPDU[COMMANDMESSAGEDATASIZE] = {0x00};
	memcpy(tempCAPDU, byCAPDU, 4);

	if (wCAPDUDataSize == 4)
	{
		tempCAPDU[4] = 0x02;
		tempCAPDU[5] = 0x83;
		tempCAPDU[6] = 0x00;
		wCAPDUDataSize = 7;
	}
	else
	{
		WORD tempSize = byCAPDU[4] + 2;
		tempCAPDU[4] = (BYTE)tempSize;
		tempCAPDU[5] = 0x83;
		memcpy(tempCAPDU + 6, byCAPDU + 4, wCAPDUDataSize - 4);
		wCAPDUDataSize = wCAPDUDataSize + 2;
	}
	
	// 把BYTE型的C_APDU命令转换成此函数导出的字符串型
	HEXtoASC((LPBYTE)CApdu, tempCAPDU, wCAPDUDataSize, &wReturn);

	return OK;
}


// 应用初始化 aAFL 文件定位器
int CICDAnalysis::ICD_GetProOption(CHAR* chAFL)
{
	WORD wReturn = OK;
	BYTE byAFL[DATALENGTH] = {0x00};
	WORD wAFLSize = 0;

	upicd->F_InfoGet(ICDPC_IC_AFL, byAFL, sizeof(byAFL), &wAFLSize, &wReturn);//第一次响应报文
	if (OK != wReturn)
	{
		return wReturn;
	}

	HEXtoASC((LPBYTE)chAFL, byAFL, wAFLSize, &wReturn);

	return wReturn;
}


// //读取应用，Get wAfl  in,Send C_Apdu[wCommandCount];
//wAFL :80101001 00104001 80101012 0010100
int CICDAnalysis::CAPDU_ReadAppData(CHAR* wAFL, CHAR* C_Apdu)
{
	WORD wReturn = OK;
    QString sAFL(wAFL);
	BYTE chSfi[DATATIME][8]={0};											//SFI短文件标识
	BYTE bySfi[DATATIME][4]={0x00};
    WORD SfiCount = sAFL.length()/8;										//SFI数目
	WORD SfiCommandCount = 0;												//每条Sfi需要的ReadRecord命令数目
	BYTE byRecordNum = 0x04;												//记录号

	//组织命令
    QString ClaIns("00B2");
    QString P1("");
    QString P2("");
    QString le("00");

	for (int i = 0; i < SfiCount; i++)
	{
        memcpy(chSfi[i], Q2S(sAFL.mid(8 * i, 8 * (i + 1))), 8);				//Get Sfi file
		ASCtoHEX(bySfi[i], (LPBYTE)chSfi[i], 8, &wReturn);					//存为0x80 0x10 0x10 0x00
		SfiCommandCount = bySfi[i][2] - bySfi[i][1] + 1;					//
		
		for (int j = 0; j < SfiCommandCount; j++)							//每个Sfi可能对应SfiCommandCount个read
		{
			CHAR chCApduTemp[DATATIME] = {0};

            P1.sprintf("%02X", j + bySfi[i][1]);
            P2.sprintf("%02X", bySfi[i][0] | byRecordNum);
			
            sprintf(chCApduTemp, "%s%s%s%s&", Q2S(ClaIns), Q2S(P1), Q2S(P2), Q2S(le));
			strcat(C_Apdu, chCApduTemp);
		}
	}

	C_Apdu[strlen(C_Apdu) - 1] = 0;

	return wReturn;
}


// Analysis the wBalance return
int CICDAnalysis::ICD_ReadBalance(CHAR* wBalance)
{
	WORD wReturn = OK;
	BYTE byBalance[DATALENGTH]={0x00};
	CHAR chBalance[DATALENGTH]={0};
	WORD BalanceSize = 0;

	upicd->F_InfoGet(ICDPC_IC_BALANCE_DATA, byBalance, sizeof(byBalance), &BalanceSize, &wReturn);
	if (OK != wReturn)
	{
		return wReturn;
	}

	HEXtoASC((LPBYTE)chBalance, byBalance, BalanceSize, &wReturn);
    QString sBalance(chBalance);
    g_Helper.TrimLeft(sBalance,"0");
    sprintf(wBalance,"%s",Q2S(sBalance));

	return wReturn;
}


// get data 9f79
int CICDAnalysis::CAPDU_GetBalance(CHAR* C_Apdu)
{
	WORD	wReturn = 0;
    QString ClaIns("80CA");
    QString P1P2("9F79");
    QString le("00");

    sprintf(C_Apdu, "%s%s%s", Q2S(ClaIns), Q2S(P1P2), Q2S(le));

	return wReturn;
}



// //get data 9f77
int CICDAnalysis::CAPDU_BalanceLimit(CHAR* C_Apdu)
{
	WORD	wReturn = 0;
    QString ClaIns("80CA");
    QString P1P2("9F77");
    QString le("00");

    sprintf(C_Apdu, "%s%s%s", Q2S(ClaIns), Q2S(P1P2), Q2S(le));
	//C_ApduLength = ClaIns.GetLength() + P1P2.GetLength() + le.GetLength();

	return wReturn;
}



// Analysis to get balance limit
int CICDAnalysis::ICD_BalanceLimit(CHAR* wBalanceLimit)
{
	WORD wReturn = OK;
	BYTE byBalanceLimit[DATALENGTH]={0x00};
	CHAR chBalanceLimit[DATALENGTH]={0};
	WORD BalanceLimitSize = 0;

	upicd->F_InfoGet(ICDPC_IC_BALANCE_LIMIT, byBalanceLimit, sizeof(byBalanceLimit), &BalanceLimitSize, &wReturn);
	if (OK != wReturn)
	{
		return wReturn;
	}

	HEXtoASC((LPBYTE)chBalanceLimit, byBalanceLimit, BalanceLimitSize, &wReturn);
    QString sBalanceLimit(chBalanceLimit);
    g_Helper.TrimLeft(sBalanceLimit,"0");
    sprintf(wBalanceLimit,"%s",Q2S(sBalanceLimit));

	return wReturn;
}



// Get ap Start Date and End Date
int CICDAnalysis::ICD_CardDateInfo(CHAR* StartDateEndDate)
{
	WORD wReturn = OK;
	BYTE byStartDate[DATALENGTH] = {0x00};
	CHAR chStartDate[DATALENGTH] = {0};
	WORD StartDateSize = 0;

	BYTE byEndDate[DATALENGTH] = {0x00};
	CHAR chEndDate[DATALENGTH] = {0};
	WORD EndDateSize = 0;

	upicd->F_InfoGet(ICDPC_IC_APPLICATION_EFFECTIVE_DATE, byStartDate, sizeof(byStartDate), &StartDateSize, &wReturn);
	if (OK != wReturn)
	{
		return wReturn;
	}

	HEXtoASC((LPBYTE)chStartDate, byStartDate, StartDateSize, &wReturn);

	upicd->F_InfoGet(ICDPC_IC_APPLICATION_EXPIRATION_DATE, byEndDate, sizeof(byEndDate), &EndDateSize, &wReturn);
	if (OK != wReturn)
	{
		return wReturn;
	}

	HEXtoASC((LPBYTE)chEndDate, byEndDate, EndDateSize, &wReturn);
	sprintf(StartDateEndDate,"%s&%s",chStartDate,chEndDate);

	return wReturn;
}


// Get Holder infomation  
//wHoldInfo=:   chName & chNameExc & chIdeData & chIdeType
//Tag: 5F20 & 9F0B & 9F61 & 9F62
int CICDAnalysis::ICD_GetHolderInfo(CHAR* wHoldInfo)
{
	WORD wReturn = OK;
	BYTE byName[DATALENGTH] = {0x00};
	CHAR chName[DATALENGTH] = {0};
	WORD wNameSize = 0;

	BYTE byNameExc[DATALENGTH] = {0x00};
	CHAR chNameExc[DATALENGTH] = {0};
	WORD wNameExcSize = 0;

	BYTE byIdeData[DATALENGTH] = {0x00};
	CHAR chIdeData[DATALENGTH] = {0};
	WORD wIdeDataSize = 0;

	BYTE byIdeType[DATALENGTH] = {0x00};
	CHAR chIdeType[DATALENGTH] = {0};
	WORD wIdeTypeSize = 0;

	upicd->F_InfoGet(ICDPC_IC_CARDHOLDER_NAME, byName, sizeof(byName), &wNameSize, &wReturn);
	if (OK != wReturn && NODATA != wReturn)
	{
		return wReturn;
	}
	HEXtoASC((LPBYTE)chName, byName, wNameSize, &wReturn);

	upicd->F_InfoGet(ICDPC_IC_CARDHOLDER_NAME_EXTENDED, byNameExc, sizeof(byNameExc), &wNameExcSize, &wReturn);
	if (OK != wReturn && NODATA != wReturn)
	{
		return wReturn;
	}
	HEXtoASC((LPBYTE)chNameExc, byNameExc, wNameExcSize, &wReturn);

	upicd->F_InfoGet(ICDPC_IC_CARDHOLDER_ID_NO, byIdeData, sizeof(byIdeData), &wIdeDataSize, &wReturn);
	if (OK != wReturn && NODATA != wReturn)
	{
		return wReturn;
	}
	HEXtoASC((LPBYTE)chIdeData, byIdeData, wIdeDataSize, &wReturn);

	upicd->F_InfoGet(ICDPC_IC_CARDHOLDER_ID_TYPE, byIdeType, sizeof(byIdeType), &wIdeTypeSize, &wReturn);
	if (OK != wReturn && NODATA != wReturn)
	{
		return wReturn;
	}

	HEXtoASC((LPBYTE)chIdeType, byIdeType, wIdeTypeSize, &wReturn);

	sprintf(wHoldInfo, "%s&%s&%s&%s", chName, chNameExc, chIdeData, chIdeType);

	return wReturn;
}


// 相关数据检查
//Tvr 0x95
 int CICDAnalysis::ICD_TemplateCheck(void)
{
	WORD wReturn = OK;
	BYTE byInfo = 0x00;
	WORD wTag[4]={ICDPC_IC_APPLICATION_EXPIRATION_DATE, ICDPC_IC_PAN, ICDPC_IC_CDOL1, ICDPC_IC_CDOL2};

	for (int i = 0; i < 4; i++)
	{
		upicd->F_MandatoryDataCheck(wTag[i], &wReturn);
		if (MANDATORY_CHECK_ERR == wReturn)
		{
			byInfo = 0x20;
			upicd->F_TvrTsiSet(ICDPC_TM_TVR, 1, byInfo, &wReturn);
			if (OK != wReturn)
			{
				return wReturn;
			}
		}
		else if (OK != wReturn)
		{
			return wReturn;
		}
	}

	return wReturn;
}

 // 脱机数据认证，检查终端和IC卡是否支持CDA、DDA、SDA脱机认证
 int CICDAnalysis::ICD_DaDataCheck(BOOL* bSupportCDA, BOOL* bSupportDDA, BOOL* bSupportSDA)
 {
	 BYTE byAIPBuffer[4] = {0x00};
	 WORD wAIPBufferSize = 0;
	 BYTE byTMBuffer[4] = {0x00};
	 WORD wTMBufferSize = 0;
	 WORD wReturn = 0;
	 BYTE byInfo = 0x80;   // 脱机数据认证未执行

	 // 初始化TVR数据
	 upicd->F_InfoItemClear(ICDPC_TM_TVR, &wReturn);
	 if (OK != wReturn)
	 {
		 return wReturn;
	 }

	 //初始化TSI数据
	 upicd->F_InfoItemClear(ICDPC_TM_TSI, &wReturn);
	 if (OK != wReturn)
	 {
		 return wReturn;
	 }

	 // 获取AIP数据，查看此IC卡是否支持SDA、DDA、CDA，AIP的第一个字节的b7、b6、b1位分别代表SDA、DDA、CDA
	 upicd->F_InfoGet(ICDPC_IC_AIP, byAIPBuffer, sizeof(byAIPBuffer), &wAIPBufferSize, &wReturn);
	 if (OK != wReturn)
	 {
		 return wReturn;
	 }

	 // 获取终端性能，终端性能第3个字节中的b8、b7、b4位分别代表SDA、DDA、CDA（参见PBOC3.0第6部分附录A.2）
	 upicd->F_InfoGet(ICDPC_TM_TERMINAL_CAPABILITIES, byTMBuffer, sizeof(byTMBuffer), &wTMBufferSize, &wReturn);
	 if (OK != wReturn)
	 {
		 return wReturn;
	 }

	 if ( (byAIPBuffer[0] & 0x01) && (byTMBuffer[2] & 0x08) )    // 判断终端和IC卡是否同时支持CDA
	 {
		 *bSupportCDA = true;
	 }
	 if ( (byAIPBuffer[0] & 0x20) && (byTMBuffer[2] & 0x40) )    // 判断终端和IC卡是否同时支持DDA
	 {
		 *bSupportDDA = true;
	 }
	 if ( (byAIPBuffer[0] & 0x40) && (byTMBuffer[2] & 0x80) )    // 判断终端和IC卡是否同时支持SDA
	 {
		 *bSupportSDA = true;
	 }

	 // 如果三种脱机数据认证都不支持，则设置TVR中的“未进行脱机数据认证”位为1
	 if (!bSupportCDA && !bSupportDDA && !bSupportSDA)
	 {
		 upicd->F_TvrTsiSet(ICDPC_TM_TVR, 1, byInfo, &wReturn);
		 if (OK != wReturn)
		 {
			 return wReturn;
		 }
	 }

	 return 0;
 }

 // 静态数据认证
 int CICDAnalysis::ICD_StaticDataAuthentication()
 {
	 BYTE byAID[TAGDATASIZE] = {0x00};
	 WORD wAIDSize = 0;
	 BYTE byRID[5] = {0x00};
	 BYTE byPKI[TAGDATASIZE] = {0x00};
	 WORD wPKISize = 0;
	 WORD wReturn = OK;

	 // 获取AID
	 upicd->F_InfoGet(ICDPC_IC_AID, byAID, sizeof(byAID), &wAIDSize, &wReturn);
	 if (OK != wReturn)
	 {
		 return wReturn;
	 }

	 // 提取AID的前5个字节，即为RID
	 memcpy(byRID, byAID, 5);

	 // 获取CA公钥索引（PKI）
	 upicd->F_InfoGet(ICDPC_IC_CA_PUBLIC_KEY_INDEX, byPKI, sizeof(byPKI), &wPKISize, &wReturn);
	 if (OK != wReturn)
	 {
		 return wReturn;
	 }

	 // 步骤1：取得CA公钥

	 // 步骤2：发卡行公钥的获取



	 return OK;
 }


 // 处理限制，具体包括：应用版本号检查/应用用途控制/应用生效日期检查/应用失效日期检查
 //Set Trv 
 int CICDAnalysis::ICD_ProcessLimit()
 {
	 WORD wReturn = OK;
	 BYTE byInfo = 0x00;

	 // 1. 应用版本号检查（IC 0x9f08 TM 0x9f09）
	 BOOL bSameVersion = TRUE;

	 int iRet = IN_ICD_CheckVersion(&bSameVersion);
	 if (iRet)
	 {
		 return iRet;
	 }
	 // 如果版本号不一致，则设置TVR中的“IC卡和终端应用版本不一致”位为1
	 if (!bSameVersion)
	 {
		 byInfo = 0x80;
		 upicd->F_TvrTsiSet(ICDPC_TM_TVR, 2, byInfo, &wReturn);
		 if (OK != wReturn)
		 {
			 return wReturn;
		 }
	 }
	 
	 // 2. 应用用途控制检查
	 BOOL bNoService = FALSE;

	 iRet = IN_ICD_CheckAUC(&bNoService);
	 if (iRet)
	 {
		 return iRet;
	 }
	 if (bNoService)    // 卡片不允许所请求的服务
	 {
		 byInfo = 0x10;
		 upicd->F_TvrTsiSet(ICDPC_TM_TVR, 2, byInfo, &wReturn);
		 if (OK != wReturn)
		 {
			 return wReturn;
		 }
	 }
	
	 // 3. 应用生效日期检查  5F25
	 // 获取当前日期
	 char chTime[9] = {0};
	 char chCurrentDate[7] = {0};

     QDateTime stDateTime = QDateTime::currentDateTime();
     sprintf(chTime, "%4d%02d%02d", stDateTime.date().year(), stDateTime.date().month(), stDateTime.date().day());
	 memcpy(chCurrentDate, chTime + 2, 6);

	 // 获取生效日期
	 BYTE byIcStartDate[DATALENGTH] = {0x00};
	 CHAR chIcStartDate[DATALENGTH] = {0};
	 WORD wIcStartDatesize = 0;

	 upicd->F_InfoGet(ICDPC_IC_APPLICATION_EFFECTIVE_DATE, byIcStartDate, sizeof(byIcStartDate), &wIcStartDatesize, &wReturn);
	 if (OK != wReturn)
	 {
		 return wReturn;
	 }

	 HEXtoASC((LPBYTE)chIcStartDate, byIcStartDate, wIcStartDatesize, &wReturn);
	 // 比较生效日期和当前日期，如果当前日期在生效日期之前，则设置TVR中的“应用尚未生效”位为1
	 if (strcmp(chIcStartDate, chCurrentDate) > 0)
	 {
		 byInfo = 0x20;
		 upicd->F_TvrTsiSet(ICDPC_TM_TVR, 2, byInfo, &wReturn);
		 if (OK != wReturn)
		 {
			 return wReturn;
		 }
	 }

	 // 4. 应用失效日期检查	5F24
	 BYTE byIcEndDate[DATALENGTH] = {0x00};
	 CHAR chIcEndDate[DATALENGTH] = {0};
	 WORD wIcEndDatesize = 0;

	 upicd->F_InfoGet(ICDPC_IC_APPLICATION_EXPIRATION_DATE, byIcEndDate, sizeof(byIcEndDate), &wIcEndDatesize, &wReturn);
	 if (OK != wReturn)
	 {
		 return wReturn;
	 }

	 HEXtoASC((LPBYTE)chIcEndDate, byIcEndDate, wIcEndDatesize, &wReturn);
	 // 比较失效日期和当前日期，如果当前日期在失效日期之后，则设置TVR中的“应用已过期”位为1
     if (strcmp(chIcEndDate, chCurrentDate) < 0)
	 {
		 byInfo = 0x40;
		 upicd->F_TvrTsiSet(ICDPC_TM_TVR, 2, byInfo, &wReturn);
		 if (OK != wReturn)
		 {
			 return wReturn;
		 }
	 }

	 return wReturn;
 }


 // 持卡人验证
 int CICDAnalysis::ICD_HolderCheckCVMCode(LPBYTE byCVMCodeList, LPWORD wCVMCodeCount)
 {
	 WORD wReturn;
	 BYTE byInfo;

	//获取AIP，判断是否支持持卡人验证
	 BYTE byIcAip[DATALENGTH] = {0x00};
	 WORD wIcAipsize = 0;

	 upicd->F_InfoGet(ICDPC_IC_AIP, byIcAip, sizeof(byIcAip), &wIcAipsize, &wReturn);
	 if (OK != wReturn)
	 {
		 return wReturn;
	 }
	 // AIP中的第1个字节b5位表示卡片是否支持持卡人验证
	 if (!(byIcAip[0] & 0x10))  //如果不支持
	 {
		 return OK;
	 }
	 
	 // 如果卡片支持持卡人验证，则获取IC卡中的CVM

	 BYTE byIcCvm[DATALENGTH] = {0x00};
	 WORD wIcCvmsize = 0;

	 upicd->F_InfoGet(ICDPC_IC_CVM_LIST, byIcCvm, sizeof(byIcCvm), &wIcCvmsize, &wReturn);
	 if ((OK != wReturn) && (NODATA != wReturn))
	 {
		 return wReturn;
	 }
	 else if (NODATA == wReturn)	// 如果卡片中没有CVM列表数据，则设置TVR中的“IC卡数据缺少”位为1，并终止持卡人验证过程
	 {
		 byInfo = 0x20;
		 upicd->F_TvrTsiSet(ICDPC_TM_TVR, 1, byInfo, &wReturn);
		 if (OK != wReturn)
		 {
			 return wReturn;
		 }

		 return wReturn;
	 }

	 // 选择CVM
	 BYTE byTradeType[TAGDATASIZE] = {0x00};    // 交易类型
	 WORD wTradeTypeSize = 0;
	 BYTE byTMType[TAGDATASIZE] = {0x00};	// 终端类型
	 WORD wTMTypeSize = 0;

	 WORD wCVMIndex = 8;
	 *wCVMCodeCount = 0;

	 while(wCVMIndex < wIcCvmsize)
	 {
		 switch(byIcCvm[wCVMIndex + 1])
		 {
		 case 0x00:		// 总是
			 byCVMCodeList[*wCVMCodeCount] = byIcCvm[wCVMIndex];
			 wCVMCodeCount++;
			 break;
		 case 0x01:		// 如果是ATM现金交易
			 // 获取终端类型
			 upicd->F_InfoGet(ICDPC_TM_TERMINAL_TYPE, byTMType, sizeof(byTMType), &wTMTypeSize, &wReturn);
			 if (OK != wReturn)
			 {
				 return wReturn;
			 }
			 // 获取交易类型
			 upicd->F_InfoGet(ICDPC_TM_TRANSACTION_TYPE, byTradeType, sizeof(byTradeType), &wTradeTypeSize, &wReturn);
			 if (OK != wReturn)
			 {
				 return wReturn;
			 }
			 // 判断是否为ATM现金交易
			 if (byTMType[0] == 0x14 && (byTradeType[0] == 0x63 || byTradeType[0] == 0x02 || byTradeType[0] == 0x01))
			 {
				 byCVMCodeList[*wCVMCodeCount] = byIcCvm[wCVMIndex];
				 wCVMCodeCount++;
			 }
			 break;
		 case 0x02:		// 如果不是ATM现金交易或有人值守的现金交易或返现交易
			 break;
		 case 0x03:		// 如果终端支持这个CVM
			 break;
		 case 0x04:		// 如果是人工值守现金交易
			 break;
		 case 0x05:		// 如果是返现交易
			 break;
		 case 0x06:		// 如果交易货币等于应用货币代码而且小于X值
			 break;
		 case 0x07:		// 如果交易货币等于应用货币代码而且大于X值
			 break;
		 case 0x08:		// 如果交易货币等于应用货币代码而且小于Y值
			 break;
		 case 0x09:		// 如果交易货币等于应用货币代码而且大于Y值
			 break;
		 default:
			 break;
		 }
		 
		 // 是否继续下一个CVM处理
		 if (!(byIcCvm[wCVMIndex] & 0x40))    // 如果第7位不为1，则不要处理下一个CVM
		 {
			 break;
		 }
	 }


	 //if (1 == (byIcAip[0] & 0x10))			//判断是否AIP支持 
	 //{
		// //取第一个CVM   ???预留
		// IN_ICD_SetCvmData();

		// IN_ICD_SetCvrData();


	 //}
	 //else
	 //{
		// //Cvm验证失败,设置Tvr,设置byIcTvr[2] | 0x80
		// byInfo = 0x80;
		// upicd->F_TvrTsiSet(ICDPC_TM_TVR, 3, byInfo, &wReturn);
		// if (OK != wReturn)
		// {
		//	 return wReturn;
		// }
		// return wReturn;
	 //}

	 return wReturn;
 }


 // 设置Cvm表，CVM	8e 预留
 int CICDAnalysis::IN_ICD_SetCvmData(void)
 {
	 return 0;
 }


 // 设置CVR表，CVR  9f10 的第4 个字节开始 一共4个字节的长度 预留
 int CICDAnalysis::IN_ICD_SetCvrData(void)
 {
	 return 0;
 }



 // GenerateAc命令，P1参数代表AAC或者ARQC命令
 int CICDAnalysis::CAPDU_GenerateAc(WORD Command, CHAR* P1, CHAR* C_Apdu)
 {
	 WORD	wReturn = OK;
	 CHAR ClaIns[5] = "80AE";
	 CHAR P2[3] = "00";
	 CHAR Lc[3] = {0};
	 CHAR Data[DATALENGTH] = {0};
	 CHAR le[3] = "00";
	 
	 //Cdol分析函数
	 //获取CDOL的解析
	 BYTE byTemData[DATALENGTH] = {0x00};
	 CHAR chTemData[DATALENGTH] = {0};
	 BYTE byTemDataExc[DATALENGTH] = {0x00};
	 CHAR chTemDataExc[DATALENGTH] = {0};
	 WORD wTemDataSize = 0;
	 WORD wTemDataExcSize = 0;

	 wReturn = IN_ICD_SetCDOLData(Command, byTemData, &wTemDataSize, byTemDataExc, &wTemDataExcSize);
	 if (OK != wReturn)
	 {
		 return wReturn;
	 }

	 HEXtoASC((LPBYTE)chTemData, byTemData, wTemDataSize, &wReturn);
	 HEXtoASC((LPBYTE)chTemDataExc, byTemDataExc, wTemDataExcSize, &wReturn);
	 sprintf(Data, "%s%s", chTemData, chTemDataExc);
	 sprintf(Lc, "%02X", wTemDataSize + wTemDataExcSize);
	 sprintf(C_Apdu, "%s%s%s%s%s%s", ClaIns, P1, P2, Lc, Data, le);

	 return wReturn;
 }


 // 终端风险管理，包括最低限额检查、随机选择交易、频度检查；分别为三个函数 预留待补充？？？
 int CICDAnalysis::ICD_TemRiskManage(void)
 {
	 return 0;
 }


 // 终端行为分析，主要是TAC||IAC分别与TVR的比较, 并返回Generate AC命令的C_APDU
 int CICDAnalysis::ICD_TemActiveAnaiysis(CHAR* C_APDU)
 {
	 BYTE byTVR[TAGDATASIZE] = {0x00};
	 WORD wTVRSize = 0;
	 BYTE byIACDenial[TAGDATASIZE] = {0x00};    // IAC-拒绝
	 WORD wIACDenialSize = 0;
	 BYTE byIACDefault[TAGDATASIZE] = {0x00};	// IAC-缺省
	 WORD wIACDefaultSize = 0;
	 BYTE byIACOnline[TAGDATASIZE] = {0x00};    // IAC-联机
	 WORD wIACOnlineSize = 0;
	 BYTE byTACDenial[TAGDATASIZE] = {0x00};    // TAC-拒绝
	 WORD wTACDenialSize = 0;
	 BYTE byTACDefault[TAGDATASIZE] = {0x00};	// TAC-缺省
	 WORD wTACDefaultSize = 0;
	 BYTE byTACOnline[TAGDATASIZE] = {0x00};    // TAC-联机
	 WORD wTACOnlineSize = 0;
	 BYTE byTMType[TAGDATASIZE] = {0x00};   // 终端类型
	 WORD wTMTypeSize = 0;
	 CHAR chP1[3] = {0};
	 WORD wReturn = OK;

	 BYTE AAC = 0x00;
	 BYTE TC = 0x40;
	 BYTE ARQC = 0x80;

	 // 获取TVR数据
	 upicd->F_InfoGet(ICDPC_TM_TVR, byTVR, sizeof(byTVR), &wTVRSize, &wReturn);
	 if (OK != wReturn)
	 {
		 return wReturn;
	 }

	 // 获取IAC-拒绝数据
	 upicd->F_InfoGet(ICDPC_IC_IAC_DENIAL, byIACDenial, sizeof(byIACDenial), &wIACDenialSize, &wReturn);
	 if (NODATA == wReturn)    // 若IAC-拒绝不存在，则默认为 0000000000
	 {
		 memset(byIACDenial, 0x00, sizeof(byIACDenial));
		 wIACDenialSize = 5;
	 }
	 else if (OK != wReturn)
	 {
		 return wReturn;
	 }

	 // 获取TAC-拒绝数据
	 upicd->F_InfoGet(ICDPC_TM_TAC_DENIAL, byTACDenial, sizeof(byTACDenial), &wTACDenialSize, &wReturn);
	 if (NODATA == wReturn)    // 若IAC-拒绝不存在，则默认为 0000000000
	 {
		 memset(byTACDenial, 0x00, sizeof(byTACDenial));
		 wTACDenialSize = 5;
	 }
	 else if (OK != wReturn)
	 {
		 return wReturn;
	 }

	 // 比较 IAC-拒绝 TAC-拒绝 与 TVR 对应的位有没有同时为1的，如果有，则Generate AC命令中的P1设为AAC（0x00），授权响应码（0x8A）设为“Z1”
	 if (IN_ICD_CheckSameBit(byIACDenial, wIACDenialSize, byTVR, wTVRSize) ||
		 IN_ICD_CheckSameBit(byTACDenial, wTACDenialSize, byTVR, wTVRSize))
	 {
		 upicd->F_InfoSet(ICDPC_TM_ARC, (LPBYTE)"Z1", 2, &wReturn);

		 sprintf(chP1, "%02X", AAC);
		 CAPDU_GenerateAc(ICDPC_IC_CDOL1, chP1, C_APDU);
		 return OK;
	 }

	 // 获取终端类型
	 upicd->F_InfoGet(ICDPC_TM_TERMINAL_TYPE, byTMType, sizeof(byTMType), &wTMTypeSize, &wReturn);
	 if (OK != wReturn)
	 {
		 return wReturn;
	 }

	 // 判断终端是否具有联机能力
	 if (byTMType[0] == 0x11 || byTMType[0] == 0x21 || byTMType[0] == 0x14 || byTMType[0] == 0x24 || byTMType[0] == 0x34 ||
		 byTMType[0] == 0x12 || byTMType[0] == 0x22 || byTMType[0] == 0x15 || byTMType[0] == 0x25 || byTMType[0] == 0x35)    // 如果终端类型为仅联机或具有联机能力的脱机，则认为终端具有联机能力
	 {
		 // 获取IAC-联机数据
		 upicd->F_InfoGet(ICDPC_IC_IAC_ONLINE, byIACOnline, sizeof(byIACOnline), &wIACOnlineSize, &wReturn);
		 if (NODATA == wReturn)    // 若IAC-联机不存在，则默认为 FFFFFFFFFF
		 {
			 memset(byIACOnline, 0xFF, sizeof(byIACOnline));
			 wIACOnlineSize = 5;
		 }
		 else if (OK != wReturn)
		 {
			 return wReturn;
		 }

		 // 获取TAC-联机数据
		 upicd->F_InfoGet(ICDPC_TM_TAC_ONLINE, byTACOnline, sizeof(byTACOnline), &wTACOnlineSize, &wReturn);
		 if (NODATA == wReturn)    // 若IAC-拒绝不存在，则默认为 0000000000
		 {
			 memset(byTACOnline, 0x00, sizeof(byTACOnline));
			 wTACOnlineSize = 5;
		 }
		 else if (OK != wReturn)
		 {
			 return wReturn;
		 }

		 // 比较 IAC-联机 TAC-联机 与 TVR 对应的位有没有同时为1
		 if (IN_ICD_CheckSameBit(byIACOnline, wIACOnlineSize, byTVR, wTVRSize) || 
			 IN_ICD_CheckSameBit(byTACOnline, wTACOnlineSize, byTVR, wTVRSize))
		 {
			 sprintf(chP1, "%02X", ARQC);
			 CAPDU_GenerateAc(ICDPC_IC_CDOL1, chP1, C_APDU);
			 return OK;
		 }
	 }
	 else    // 若终端不具有联机能力
	 {
		 // 获取 IAC-缺省
		 upicd->F_InfoGet(ICDPC_IC_IAC_DEFAULT, byIACDefault, sizeof(byIACDefault), &wIACDefaultSize, &wReturn);
		 if (NODATA == wReturn)    // 若IAC-联机不存在，则默认为 FFFFFFFFFF
		 {
			 memset(byIACDefault, 0xFF, sizeof(wIACDefaultSize));
			 wIACOnlineSize = 5;
		 }
		 else if (OK != wReturn)
		 {
			 return wReturn;
		 }

		 // 获取TAC-缺省数据
		 upicd->F_InfoGet(ICDPC_TM_TAC_DEFAULT, byTACDefault, sizeof(byTACDefault), &wTACDefaultSize, &wReturn);
		 if (NODATA == wReturn)    // 若IAC-拒绝不存在，则默认为 0000000000
		 {
			 memset(byTACDefault, 0x00, sizeof(byTACDefault));
			 wTACDefaultSize = 5;
		 }
		 else if (OK != wReturn)
		 {
			 return wReturn;
		 }

		 // 比较 IAC-缺省 TAC-缺省 与 TVR 对应的位有没有同时为1
		 if (IN_ICD_CheckSameBit(byIACDefault, wIACDefaultSize, byTVR, wTVRSize) || 
			 IN_ICD_CheckSameBit(byTACDefault, wTACDefaultSize, byTVR, wTVRSize))
		 {
			 upicd->F_InfoSet(ICDPC_TM_ARC, (LPBYTE)"Z1", 2, &wReturn);

			 sprintf(chP1, "%02X", AAC);
			 CAPDU_GenerateAc(ICDPC_IC_CDOL1, chP1, C_APDU);
			 
			 return OK;
		 }
	 }

	 // 如果 比较 IAC-联机 TAC-联机 与 TVR 对应的位没有同时为1，或者 IAC-缺省 TAC-缺省 与 TVR 对应的位也没有同时为1

	 // 设置ARC为Y1
	 upicd->F_InfoSet(ICDPC_TM_ARC, (LPBYTE)"Y1", 2, &wReturn);
	 // 设P1为TC
	 //xuzhe start modify
	 sprintf(chP1, "%02X", TC);
	 //sprintf(chP1, "%02X", 0x80);
	 //xuzhe end
	 CAPDU_GenerateAc(ICDPC_IC_CDOL1, chP1, C_APDU);

	 return OK;
 }


 // 圈存圈取初始化，标志为第一次的GenerateAc命令的结束
 int CICDAnalysis::ICD_InitForLoad(CHAR* CardDataExc, CHAR* ForArqc, CHAR* Arqc)
 {
	 WORD wReturn = OK;

	 //主账号Pan,5A
	 BYTE byPan[DATALENGTH] = {0x00};
	 CHAR chPan[DATALENGTH] = {0};
	 WORD wPanSize = 0;

	 upicd->F_InfoGet(ICDPC_IC_PAN, byPan, sizeof(byPan), &wPanSize, &wReturn);
	 if (OK != wReturn)
	 {
		 return wReturn;
	 }
	 HEXtoASC((LPBYTE)chPan, byPan, wPanSize, &wReturn);

	 mf.mfwrite("chPan=");
	 mf.mfwrite(chPan);

	 int len= strlen(chPan);//xlc 2015-12-30 add
	 if(chPan[len-1]=='F')
	 {
		 chPan[len-1]=0;
	 }

	 //主账号序列号,5F34
	 BYTE bySerialNum[DATALENGTH] = {0x00};
	 CHAR chSerialNum[DATALENGTH] = {0};
	 WORD wSerialNumSize = 0;

	 upicd->F_InfoGet(ICDPC_IC_PAN_SEQUENCE_NO, bySerialNum, sizeof(bySerialNum), &wSerialNumSize, &wReturn);
	 if (OK != wReturn)
	 {
		 return wReturn;
	 }
	 HEXtoASC((LPBYTE)chSerialNum, bySerialNum, wSerialNumSize, &wReturn);

	 mf.mfwrite("chSerialNum=");
	 mf.mfwrite(chSerialNum);

	 sprintf(CardDataExc, "%s%s", chPan, chSerialNum);

	 //Aip数据 ,9f87
	 BYTE byAip[DATALENGTH] = {0x00};
	 CHAR chAip[DATALENGTH] = {0};
	 WORD wAipSize = 0;

	 upicd->F_InfoGet(ICDPC_IC_AIP, byAip, sizeof(byAip), &wAipSize, &wReturn);
	 if (OK != wReturn)
	 {
		 return wReturn;
	 }
	 HEXtoASC((LPBYTE)chAip, byAip, wAipSize, &wReturn);
	 myRelatedData[RelatedDataCount].Tag = ICDPC_IC_AIP;
	 myRelatedData[RelatedDataCount].Length = wAipSize;
	 sprintf(myRelatedData[RelatedDataCount].Value, "%s", chAip);
	 RelatedDataCount += 1;

	 //Atc数据 ,9f36
	 BYTE byAtc[DATALENGTH] = {0x00};
	 CHAR chAtc[DATALENGTH] = {0};
	 WORD wAtcSize = 0;

	 upicd->F_InfoGet(ICDPC_IC_ATC, byAtc, sizeof(byAtc), &wAtcSize, &wReturn);
	 if (OK != wReturn)
	 {
		 return wReturn;
	 }
	 HEXtoASC((LPBYTE)chAtc, byAtc, wAtcSize, &wReturn);

	 //9f10数据，用来获取CVR  ???应该由前面定义，这里不标准
	 /*BYTE byCvrExc[DATALENGTH] = {0x00};
	 CHAR chCvrExc[DATALENGTH] = {0};
	 WORD wCvrExcSize = 0;

	 upicd->F_InfoGet(ICDPC_IC_ISSUER_APPLICATION_DATA, byCvrExc, sizeof(byCvrExc), &wCvrExcSize, &wReturn);
	 if (OK != wReturn)
	 {
		 return wReturn;
	 }
	 HEXtoASC((LPBYTE)chCvrExc, byCvrExc, wCvrExcSize, &wReturn);*/
	 sprintf(chCvr, "0000000800");
	 /*memcpy(chCvr, chCvrExc + 7, 8);*/

	 //获取CDOL的解析
	 BYTE byTemData[DATALENGTH] = {0x00};
	 CHAR chTemData[DATALENGTH] = {0};
	 BYTE byTemDataExc[DATALENGTH] = {0x00};
	 WORD wTemDataSize = 0;
	 WORD wTemDataExcSize = 0;

	 IN_ICD_SetCDOLData(ICDPC_IC_CDOL1, byTemData, &wTemDataSize, byTemDataExc, &wTemDataExcSize);
	 if (OK != wReturn)
	 {
		 return wReturn;
	 }

	 HEXtoASC((LPBYTE)chTemData, byTemData, wTemDataSize, &wReturn);
	 sprintf(ForArqc, "%s%s%s%s", chTemData, chAip, chAtc, chCvr);

	 //获得ARQC数据 
	 CHAR chCid[3] = {0};
	 CHAR chAtc2[5] = {0};
	 CHAR chAc[17] = {0};
	 CHAR chIad[DATALENGTH] = {0};
	 wReturn = IN_ICD_GetArqc(chCid, chAtc2, chAc, chIad);
	 if (OK != wReturn)
	 {
		 return wReturn;
	 }

	 sprintf(Arqc, "%s%s%s%s", chCid, chAtc2, chAc, chIad);

	 //Add by LeeHai
	 sprintf(chToArqc, "%s", ForArqc);
	 sprintf(chArqc, "%s", Arqc);
	 //End

	 return wReturn;
 }

 // 圈存圈提结束
 int CICDAnalysis::ICD_FinishLoad(CHAR* chOut)
 {
	 BYTE byCID[TAGDATASIZE] = {0x00};
	 WORD wCIDSize = 0;
	 BYTE byATC[TAGDATASIZE] = {0x00};
	 WORD wATCSize = 0;
	 BYTE byAC[TAGDATASIZE] = {0x00};
	 WORD wACSize = 0;
	 BYTE byIAD[TAGDATASIZE] = {0x00};
	 WORD wIADSize = 0;
	 BYTE byTCData[COMMANDMESSAGEDATASIZE] = {0x00};
	 WORD wTCDataSize = 0;
	 WORD wReturn = 0;

	 // 获取CID
	 upicd->F_InfoGet(ICDPC_IC_CID, byCID, sizeof(byCID), &wCIDSize, &wReturn);
	 if (OK != wReturn)
	 {
		 return wReturn;
	 }
	 memcpy(byTCData, byCID, wCIDSize);
	 wTCDataSize += wCIDSize;

	 // 获取ATC
	 upicd->F_InfoGet(ICDPC_IC_ATC, byATC, sizeof(byATC), &wATCSize, &wReturn);
	 if (OK != wReturn)
	 {
		 return wReturn;
	 }
	 memcpy(byTCData + wTCDataSize, byATC, wATCSize);
	 wTCDataSize += wATCSize;

	 // 获取AC
	 upicd->F_InfoGet(ICDPC_IC_AC, byAC, sizeof(byAC), &wACSize, &wReturn);
	 if (OK != wReturn)
	 {
		 return wReturn;
	 }
	 memcpy(byTCData + wTCDataSize, byAC, wACSize);
	 wTCDataSize += wACSize;

	 // 获取IAD
	 upicd->F_InfoGet(ICDPC_IC_ISSUER_APPLICATION_DATA, byIAD, sizeof(byIAD), &wIADSize, &wReturn);
	 if (OK != wReturn)
	 {
		 return wReturn;
	 }
	 memcpy(byTCData + wTCDataSize, byIAD, wIADSize);
	 wTCDataSize += wIADSize;

	 HEXtoASC((LPBYTE)chOut, byTCData, wTCDataSize, &wReturn);

	 return OK;
 }


 // 获取55域数据，根据ARQC去得到55域数据
 int CICDAnalysis::ICD_Re55FieldData(CHAR* Arqc, CHAR* FiftyFiveData)
 {
	 WORD wReturn = OK;
	 CHAR ch55fData[MAX_PATH] = {0};								//55域数据
	 WORD For55FieldData[14] = {					//55域数据标签表

		 //必选项
		 ICDPC_IC_CID,                           //CID                 9F27:	Arqc
		 ICDPC_IC_ATC,							 //ATC                 9F36:	Arqc
		 ICDPC_IC_AC,							 //AC                  9F26:	Arqc
		 ICDPC_IC_ISSUER_APPLICATION_DATA,		 //IAD                 9F10:	Arqc
		 ICDPC_TM_UNPREDICTABLE_NUMBER,			 //UnNum               9f37:不可预知数	组织
		 ICDPC_TM_TRANSACTION_DATE,				 //TraData             9A:交易日期
		 ICDPC_TM_TRANSACTION_TYPE,				 //TraType             9C:交易类型
		 ICDPC_TM_AMOUNT_AUTHORISED,			 //TraMon              9F02:授权金额
		 ICDPC_TM_TRANSACTION_CURRENCY_CODE,	 //TraCode             5F2A:交易货币代码
		 ICDPC_IC_AIP,							 //Aip                 82:应用交互特征
		 ICDPC_TM_TERMINAL_COUNTRY_CODE,		 //TemContryCode       9F1A：终端国家代码
		 ICDPC_TM_AMOUNT_OTHER,					 //OtherMon            9F03:其他金额
		 ICDPC_TM_TERMINAL_CAPABILITIES,		 //TemCap              9F33:终端性能

		 //可选项    ？？？待配置添加
		 //Cvmr                9F34:持卡人验证结果方法
		 //TemType             9F35:终端类型
		 //Ifd                 9F1E:接口设备序列号
		 //Df                  84:专用文件名称
		 //TemApVer            9F09:应用版本号
		 //IsAutherData        91:发卡行数据认证
		 //IssScriptTem1       71:发卡行脚本1
		 //IssScriptTem2       72:发卡行脚本2
		 //IssScriptResult     DF31:发卡行脚本结果
		 //Eciac               9F74:电子现金发卡行授权码
		 //CardProIda          9F63:卡产品标识信息
	 };

	 //9f37数据，不可预知数
	 BYTE byUnknowData[DATALENGTH] = {0x00};
	 CHAR chUnknowData[DATALENGTH] = {0};
	 WORD wUnknowDataSize = 0;

	 upicd->F_InfoGet(ICDPC_TM_UNPREDICTABLE_NUMBER, byUnknowData, sizeof(byUnknowData), &wUnknowDataSize, &wReturn);
	 if (OK != wReturn)
	 {
		 return wReturn;
	 }

	 HEXtoASC((LPBYTE)chUnknowData, byUnknowData, wUnknowDataSize, &wReturn);
	 myRelatedData[RelatedDataCount].Tag = ICDPC_TM_UNPREDICTABLE_NUMBER;
	 myRelatedData[RelatedDataCount].Length = wUnknowDataSize;
	 sprintf(myRelatedData[RelatedDataCount].Value, "%s", chUnknowData);
	 RelatedDataCount += 1;

	 //终端性能
	 BYTE byTMCapability[TAGDATASIZE] = {0x00};
	 CHAR chTMCapability[TAGDATASIZE] = {0};
	 WORD wTMCapabilitySize = 0;

	 upicd->F_InfoGet(ICDPC_TM_TERMINAL_CAPABILITIES, byTMCapability, sizeof(byTMCapability), &wTMCapabilitySize, &wReturn);
	 if (OK != wReturn)
	 {
		 return wReturn;
	 }

	 HEXtoASC((LPBYTE)chTMCapability, byTMCapability, wTMCapabilitySize, &wReturn);
	 myRelatedData[RelatedDataCount].Tag = ICDPC_TM_TERMINAL_CAPABILITIES;
	 myRelatedData[RelatedDataCount].Length = wTMCapabilitySize;
	 sprintf(myRelatedData[RelatedDataCount].Value, "%s", chTMCapability);
	 RelatedDataCount += 1;

	 for (int i = 0; i < 13; i++)
	 {
		 for (int j = 0; j <= RelatedDataCount; j++)
		 {
			 if (For55FieldData[i] == myRelatedData[j].Tag)
			 {
				 CHAR TLVData[DATALENGTH] = {0};
				 sprintf(TLVData, "%02X%02X%s", myRelatedData[j].Tag, myRelatedData[j].Length, myRelatedData[j].Value);
				 strcat(FiftyFiveData, TLVData);

				 wReturn = OK;
				 break;
			 }
			 else
			 {
				 wReturn = NODATA;
			 }
		 }
		 //str55fData.Append();
	 }

	 return wReturn;
 }

 //add by LeeHai 交行55域数据临时解决方案 2015.12.30
 int CICDAnalysis::ICD_Re55FieldDataExc(CHAR* Arqc, CHAR* FiftyFiveData)
 {
	 WORD wReturn = OK;

	 char szData[1024]={0};
	 char OutData[1024]={0};
	 char temp[255]={0};
	 char szAD[255] = {0x00};
     QString strLength;

	 memset(temp,0x00,sizeof(temp));
	 memset(szData,0x00,sizeof(szData));

	 memcpy(szAD,chToArqc,strlen(chToArqc));

	 if(strlen(chArqc) >= 22)
		 memcpy(szData,chArqc,strlen(chArqc));
	 else
	 {
		 return 0;
	 }

	 //AC(圈存为ARQC)
	 memcpy(OutData,"9F2608",6);
	 memset(temp,0x00,sizeof(temp));
	 memcpy(temp,szData+6,16);
	 strcat(OutData,temp);

	 //CID(密文信息)
	 strcat(OutData,"9F2701");
	 memset(temp,0x00,sizeof(temp));
	 memcpy(temp,szData,2);
	 strcat(OutData,temp);

	 //发卡行应用数据
	 strcat(OutData,"9F10");
	 memset(temp,0x00,sizeof(temp));
	 if(strlen(szData)-22 > 2)
	 {
         strLength.sprintf("%02X",(strlen(szData)-22)/2);
         memcpy(temp,Q2S(strLength),strLength.length());
		 strcat(OutData,temp);
		 memset(temp,0x00,sizeof(temp));
		 memcpy(temp,szData+22,strlen(szData)-22);
		 strcat(OutData,temp);
	 }
	 else
		 strcat(OutData,"00");

	 //不可预知数 mark
	 strcat(OutData,"9F3704");
	 memset(temp,0x00,sizeof(temp));

	 //xuzhe start add
	 memcpy(temp,g_RandData,8);
	 //xuzhe end

	 strcat(OutData,temp);

	 //ATC(应用交易序号)
	 strcat(OutData,"9F3602");
	 memset(temp,0x00,sizeof(temp));
	 memcpy(temp,szData+2,4);
	 strcat(OutData,temp);

	 //终端验证结果
	 strcat(OutData,"9505");
	 memset(temp,0x00,sizeof(temp));
	 memcpy(temp,szAD+28,10);
	 
	 //modify by LeeHai 2016.2.26
	 strcat(OutData,temp);
	 //strcat(OutData, "8000040000");//yihua
	 //end modify


	 //交易日期
	 strcat(OutData,"9A03");
	 memset(temp,0x00,sizeof(temp));
	 memcpy(temp,szAD+42,6);
	 strcat(OutData,temp);

	 //交易类型
	 strcat(OutData,"9C01");
	 memset(temp,0x00,sizeof(temp));
	 memcpy(temp,szAD+48,2);
	 
	 //start modify xuzhe 0226
	 strcat(OutData,temp);
	 //strcat(OutData,"01");//yihua
	 //end modify

	 //授权金额
	 strcat(OutData,"9F0206");
	 memset(temp,0x00,sizeof(temp));
	 memcpy(temp,szAD,12);

	 //start modify xuzhe 0226
	 strcat(OutData,temp);
	 //strcat(OutData,"000002000000");
	 //end modify

	 //终端货币代码
	 strcat(OutData,"5F2A02");
	 memset(temp,0x00,sizeof(temp));
	 memcpy(temp,szAD+38,4);
	 strcat(OutData,temp);

	 //应用交互特性(AIP)
	 strcat(OutData,"8202");
	 memset(temp,0x00,sizeof(temp));
	 memcpy(temp,szAD+58,4);
	 strcat(OutData,temp);

	 //终端国家代码
	 strcat(OutData,"9F1A02");
	 memset(temp,0x00,sizeof(temp));
	 memcpy(temp,szAD+24,4);
	 strcat(OutData,temp);

	 //其他金额
	 strcat(OutData,"9F0306");
	 memset(temp,0x00,sizeof(temp));
	 memcpy(temp,szAD+12,12);
	 strcat(OutData,temp);


	 //终端性能
	 //strcat(OutData,"9F3303204100");
	 //strcat(OutData,"9F3303E0F8C8");
	 strcat(OutData,"9F3303604020");//xuzhe - tmp yihua

	 //strcat(OutData,"9F34033F0000");
	 strcat(OutData,"9F3403020300");//modify by LeeHai 2016.2.26
	 

	 sprintf(FiftyFiveData, "%s",OutData);
	 return 0;
 }
 //end

 // 解析外部认证的指令返回，输入R_Apdu为External Auth的响应报文，输出chStatus为SW1SW2
 int CICDAnalysis::ICD_ExternalAuth(CHAR* R_Apdu, CHAR* chStatus)
 {
	 WORD wReturn = OK;

	 // 设置TSI中的“发卡行认证已执行”位为1
	 upicd->F_TvrTsiSet(ICDPC_TM_TSI, 1, 0x10, &wReturn);
	 if (OK != wReturn)
	 {
		 return wReturn;
	 }

	 // 判断外部认证是否成功
	 memcpy(chStatus, R_Apdu + strlen(R_Apdu) - 4, 4);

	 if (strcmp(chStatus, "9000"))    // 如果返回报文SW1SW2不是9000，则设置TVR中的“发卡行认证不成功”位为1
	 {
		 upicd->F_TvrTsiSet(ICDPC_TM_TVR, 5, 0x40, &wReturn);
		 if (OK != wReturn)
		 {
			 return wReturn;
		 }
	 }

	 return wReturn;
 }


 // 执行脚本命令后返回的报文分析, 
 // 输入chTag为“71”或“72”
 // 输入wNum是指脚本命令的序号（从1开始），
 // 输入R_APDU为脚本处理命令返回的报文，
 // 输入chScriptID为脚本标识
 // 输出ReScript
 int CICDAnalysis::ICD_ScriptResult(CHAR* chTag, WORD wNum, CHAR* chScriptID, CHAR* R_APDU, CHAR* ReScript)
 {
	 CHAR chSW1[3] = {0};
	 BYTE bySRData[5] = {0x00};    // 发卡行脚本结果（0xDF31）
	 CHAR chSRData[11] = {0};
	 CHAR chScriID[9] = {0};
	 WORD wSRDataSize = 0;
	 WORD wReturn = OK;

	 if (!strcmp(chScriptID, ""))
	 {
		 memcpy(chScriID, "00000000", 8);
	 }
	 
	 // 设置TSI中的“脚本处理已执行”位为1
	 upicd->F_TvrTsiSet(ICDPC_TM_TSI, 1, 0x04, &wReturn);
	 if (OK != wReturn)
	 {
		 return wReturn;
	 }

	 memcpy(chSW1, R_APDU + strlen(R_APDU) - 4, 2);    // 提取返回报文中的SW1

	 // 如果SW1不是90、62或63，表示脚本处理失败
	 if (strcmp(chSW1, "90") && strcmp(chSW1, "62") && strcmp(chSW1, "63"))
	 {
		 // 如果执行的是标签为71的脚本，则设置TVR中的“最后一次Generate AC命令之前脚本处理失败”
		 if (!strcmp(chTag, "71"))
		 {
			 upicd->F_TvrTsiSet(ICDPC_TM_TVR, 5, 0x20, &wReturn);
			 if (OK != wReturn)
			 {
				 return wReturn;
			 }
		 }
		 else if (!strcmp(chTag, "72"))  // 如果执行的是标签为72的脚本，则设置TVR中的“最后一次Generate AC命令之后脚本处理失败”
		 {
			 upicd->F_TvrTsiSet(ICDPC_TM_TVR, 5, 0x10, &wReturn);
			 if (OK != wReturn)
			 {
				 return wReturn;
			 }
		 }

		 // 设置脚本结果的第一个字节第5位为1，低半字节为出错的命令序号
		 BYTE byNum = 0x00;

		 if (wNum > 15)
		 {
			 wNum = 15;
		 }
		 byNum = wNum;

		 bySRData[0] = bySRData[0] | 0x10 | byNum;
		 sprintf(chSRData, "%02X%s", bySRData[0], chScriID);
		 ASCtoHEX(bySRData, (LPBYTE)chSRData, strlen(chSRData), &wReturn);

		 upicd->F_InfoSet(ICDPC_TM_ISSUER_SCRIPT_RESULT, bySRData, 5, &wReturn);
		 if (OK != wReturn)
		 {
			 return wReturn;
		 }

		 return OK;
	 }

	 // 如果SW2是90、62或63，则处理继续

	 // 设置脚本结果第一字节为20
	 bySRData[0] = 0x20;
	 sprintf(chSRData, "%02X%s", bySRData[0], chScriID);
	 ASCtoHEX(bySRData, (LPBYTE)chSRData, strlen(chSRData), &wReturn);

	 upicd->F_InfoSet(ICDPC_TM_ISSUER_SCRIPT_RESULT, bySRData, 5, &wReturn);
	 if (OK != wReturn)
	 {
		 return wReturn;
	 }

	 WORD ScriptResult[11] = {
		 ICDPC_TM_TERMINAL_CAPABILITIES,	//终端性能			9F33  
		 ICDPC_TM_TVR,						//终端验证结果，		95   
		 ICDPC_TM_UNPREDICTABLE_NUMBER,		//不可预知数			9F37 
		 ICDPC_IC_ISSUER_APPLICATION_DATA,	//发卡行应用数据		9F10 
		 ICDPC_IC_AC,						//应用密文			9F26  
		 ICDPC_IC_ATC,						//应用交易计数器		9F36 
		 ICDPC_IC_AIP,						//应用交互特征		82    
		 ICDPC_TM_TERMINAL_COUNTRY_CODE,	//终端国家代码		9F1A  
		 ICDPC_TM_TRANSACTION_DATE,			//交易日期			9A    
		 ICDPC_TM_ISSUER_SCRIPT_RESULT,		//发卡行脚本结		DF31

	 };


	 for (int i = 0; i < 9; i++)
	 {
		 for (int j = 0; j < DATALENGTH; j++)
		 {
			 if (ScriptResult[i] == myRelatedData[j].Tag)
			 {
				 CHAR TLVData[DATALENGTH] = {0};
				 sprintf(TLVData, "%02X%02X%s", myRelatedData[j].Tag, myRelatedData[j].Length, myRelatedData[j].Value);
				 strcat(ReScript, TLVData);
				 break;
			 }
			 else
			 {
				 wReturn = NODATA;
			 }
		 }
	 }

	 strcat(ReScript, chSRData);
	 
	 return wReturn;
 }


 // 交易结束的标志位读第二次Cdol,并分析结果是否返回TC的数据
 int CICDAnalysis::ICD_AnalyEndTag(CHAR* Tc)
 {
	 WORD wReturn = OK;
	 //获得ARQC数据 
	 CHAR chCid[2] = {0};
	 CHAR chAtc2[5] = {0};
	 CHAR chAc[17] = {0};
	 CHAR chIad[33] = {0};
	 wReturn = IN_ICD_GetArqc(chCid, chAtc2, chAc, chIad);
	 if (OK != wReturn)
	 {
		 return wReturn;
	 }

	 sprintf(Tc, "%s%s%s%s", chCid, chAtc2, chAc, chIad);
 }


 // 前提是需要ICDPC直接储存脚本，执行脚本
 int CICDAnalysis::CAPDU_ScriptAnalysis(CHAR* Script, CHAR* ch71ScriptID, CHAR* ch72ScriptID, CHAR* ch71ScriptCmdList, LPWORD lpw71ScriptCmdCount, CHAR* ch72ScriptCmdList, LPWORD lpw72ScriptCmdCount)
 {
	 WORD wReturn = OK;
	 BYTE by7172Data[DATALENGTH] = {0x00};
	 WORD w7172DataSize = 0;
	 BYTE byScript[DATALENGTH] = {0x00};
	 //*lpw71ScriptCmdCount = 0;
	 //*lpw72ScriptCmdCount = 0;

	 ASCtoHEX(byScript, (LPBYTE)Script, strlen(Script), &wReturn);

	 // 解析报文，得到71标签或72标签下的数据
	 upicd->F_ScriptTLVAnalysis(byScript, strlen(Script)/2, &wReturn);
	 if (OK != wReturn)
	 {
		 return wReturn;
	 }

	 // 解析71或72标签下的数据
	 upicd->F_InfoGet(ICDPC_ISSUER_SCRIPT_TEMPLATE1, by7172Data, sizeof(by7172Data), &w7172DataSize, &wReturn);
	 if (OK == wReturn)
	 {
		 upicd->F_IssuerScriptTemplateAnalySis(by7172Data, w7172DataSize, ch71ScriptCmdList, lpw71ScriptCmdCount, ch71ScriptID, &wReturn);

		 // 保存数据
		 memcpy(m_ch71ScriptID, ch71ScriptID, strlen(ch71ScriptID));
		 memcpy(m_ch71ScriptCmdList, ch71ScriptCmdList, strlen(ch71ScriptCmdList));
		 m_w71ScriptCmdCount = *lpw71ScriptCmdCount;
	 }

	 memset(by7172Data, 0x00, sizeof(by7172Data));

	 upicd->F_InfoGet(ICDPC_ISSUER_SCRIPT_TEMPLATE2, by7172Data, sizeof(by7172Data), &w7172DataSize, &wReturn);
	 if (OK == wReturn)
	 {
		 upicd->F_IssuerScriptTemplateAnalySis(by7172Data, w7172DataSize, ch72ScriptCmdList, lpw72ScriptCmdCount, ch72ScriptID, &wReturn);

		 // 保存数据
		 memcpy(m_ch72ScriptID, ch72ScriptID, strlen(ch72ScriptID));
		 memcpy(m_ch72ScriptCmdList, ch72ScriptCmdList, strlen(ch72ScriptCmdList));
		 m_w72ScriptCmdCount = *lpw72ScriptCmdCount;
	 }

	 return wReturn;
 }


 // 读取卡片日志
 int CICDAnalysis::ICD_ReadLog(CHAR* chLogData, CHAR* Log)
 {
	 WORD wReturn = OK;
	 DOL logTagList[DOLTAGCOUNT];
	 WORD wLogTagCount = 0;
	 CHAR* chData_WK = chLogData;
 	 CHAR chLogReturn[1024] = {0};
	 WORD wLogLength = 0;
// 	 strcpy(chTem, chLogData);
	 
	 upicd->F_DOLTLVAnalysis(ICDPC_IC_LOG_FORMAT, logTagList, &wLogTagCount, &wReturn);
	 if (OK != wReturn)
	 {
		 return wReturn;
	 }

	 for (int i = 0; i < wLogTagCount; i++)
	 {
		 CHAR chTemp[TAGDATASIZE] = {0};

		 switch(logTagList[i].wTag)
		 {
		 case ICDPC_TM_TRANSACTION_DATE:
			 memcpy(chTemp, "~DA", 3);
			 break;
		 case ICDPC_TM_TRANSACTION_TIME:
			 memcpy(chTemp, "~TM", 3);
			 break;
		 case ICDPC_TM_AMOUNT_AUTHORISED:
			 memcpy(chTemp, "~AM", 3);
			 break;
		 case ICDPC_TM_AMOUNT_OTHER:
			 memcpy(chTemp, "~OM", 3);
			 break;
		 case ICDPC_TM_TERMINAL_COUNTRY_CODE:
			 memcpy(chTemp, "~TC", 3);
			 break;
		 case ICDPC_TM_TRANSACTION_CURRENCY_CODE:
			 memcpy(chTemp, "~CC", 3);
			 break;
		 case ICDPC_TM_MERCHANT_NAME_AND_LOCATION:
			 memcpy(chTemp, "~TN", 3);
			 break;
		 case ICDPC_TM_TRANSACTION_TYPE:
			 memcpy(chTemp, "~TT", 3);
			 break;
		 case ICDPC_IC_ATC:
			 memcpy(chTemp, "~AC", 3);
			 break;
		 default:
			 return FORMAT_ERR;
		 }

		 memcpy(chTemp + 3, chData_WK, logTagList[i].wDataSize * 2);
		 //memcpy(chTemp + 3, chData_WK, logTagList[i].wDataSize * 2);
		 //strcat_s(Log, 1024, chTemp);
		 strcat(chLogReturn, chTemp);
		 chData_WK += logTagList[i].wDataSize * 2;
		 wLogLength += logTagList[i].wDataSize * 2;
	 }

	 memcpy(Log, chLogReturn, wLogLength);
	 return wReturn;
 }


 // 读取二磁道数据并解析出来
 int CICDAnalysis::ICD_GetTrack2Data(CHAR* Track2Data)
 {
	 WORD wReturn = OK;

	 //2磁道数据 ,57
	 BYTE byTrade2Data[DATALENGTH] = {0x00};
	 CHAR chTrade2Data[DATALENGTH] = {0};
	 WORD wTrade2DataSize = 0;

	 upicd->F_InfoGet(ICDPC_IC_TRACK2_EQUIVALENT_DATA, byTrade2Data, sizeof(byTrade2Data), &wTrade2DataSize, &wReturn);
	 if (OK != wReturn)
	 {
		 return wReturn;
	 }

	 HEXtoASC((LPBYTE)chTrade2Data, byTrade2Data, wTrade2DataSize, &wReturn);
	 Track2Data = strtok(chTrade2Data, "D");

	 return wReturn;
 }




// 组织CDOL数据
// wItem		: input : 0x8C(CDOL1)或者0x8D(CDOL2)
// lpbyCDOLData	: output: 组织完成后的数据
// wCDOLDataSize: output: 组织完成后的数据的长度
int CICDAnalysis::IN_ICD_SetCDOLData(WORD wItem, LPBYTE lpbyCDOLData, LPWORD wCDOLDataSize, LPBYTE lpbyExtraData, LPWORD wExtraDataSize)
{
	DOL sCDOLTags[DOLTAGCOUNT] = {0};
	WORD wCDOLTagsCount = 0;
	WORD wReturn = OK;

	//memset(byBuff, 0X00, sizeof(byBuff));
	time_t timep;
	struct tm *p;
	time(&timep);
	p = localtime(&timep); //取得当地时间

	// 获取CDOL中所有标签列表
	upicd->F_DOLTLVAnalysis(wItem, sCDOLTags, &wCDOLTagsCount, &wReturn);
	if (OK != wReturn)
	{
		return wReturn;
	}

	for (int i = 0; i < wCDOLTagsCount; i++)
	{
		BYTE byBuff[TAGDATASIZE] = {0x00};
		WORD wBuffSize = 0;

		upicd->F_InfoGet(sCDOLTags[i].wTag, byBuff, sizeof(byBuff), &wBuffSize, &wReturn);
		if (OK != wReturn || sCDOLTags[i].wDataSize != wBuffSize)
		{
			memset(byBuff, 0x00, sizeof(byBuff));
			wBuffSize = sCDOLTags[i].wDataSize;
		}
		
		// 如果标签为0x9F21(交易时间) 或者 0x9F4E(商户名称)，则存入ExtraData中
		if (sCDOLTags[i].wTag == 0x9F21)
		{
			memset(byBuff, 0X00, sizeof(byBuff));
			//CHAR chTime[6] = {0};
			//sprintf(chTime, "%02d%02d%02d", (p->tm_hour), (p->tm_min), (p->tm_sec));
			//memcpy(byBuff, (LPBYTE)chTime, 3);

			byBuff[0]=(p->tm_hour/10)*16+p->tm_hour%10;//xlc 2015-12-30 modify
			byBuff[1]=(p->tm_min/10)*16+p->tm_min%10 ;
			byBuff[2]=(p->tm_sec/10)*16+p->tm_sec%10 ;

			memcpy(lpbyExtraData + (*wExtraDataSize), byBuff, wBuffSize);
			(*wExtraDataSize) += wBuffSize;
		}
		else if (sCDOLTags[i].wTag == 0x9F4E)
		{
			//xuzhe start end add
			/*char chName[] ={0x30,0x30,0x30,0x30,0x20,0x30,0x30,0x30,0x30,0x30,0x31,0x38,0x32,0x30,0x30,0x30,0x00,0x00,0x00,0x00};
			memcpy(byBuff,chName,20);
			wBuffSize = 20;*/
		
			memcpy(lpbyExtraData + (*wExtraDataSize), byBuff, wBuffSize);
			(*wExtraDataSize) += wBuffSize;
		}
		else if(sCDOLTags[i].wTag == 0x9A)
		{
			memset(byBuff, 0X00, sizeof(byBuff));
			//CHAR data[6] = {0};
			//sprintf(data, "%02d%02d%02d", (1900 + p->tm_year), (p->tm_mon), (p->tm_mday));
			//memcpy(byBuff, data, 6);

			byBuff[0]=((1900 + p->tm_year)%100)/10*16+(1900 + p->tm_year)%10;//xlc 2015-12-30 modify
			byBuff[1]=((p->tm_mon+1)/10)*16+(p->tm_mon+1)%10 ;
			byBuff[2]=(p->tm_mday/10)*16+p->tm_mday%10 ;

			memcpy(lpbyCDOLData + (*wCDOLDataSize), byBuff, wBuffSize);
			(*wCDOLDataSize) += wBuffSize;
		}
		else if (sCDOLTags[i].wTag == 0x9F37)
		{
			//去随即数赋值，
			memset(byBuff, 0X00, sizeof(byBuff));

			//xuzhe start modify
			/*int a[4] = {rand()%99, rand()%99, rand()%99, rand()%99};
			for (int i = 0; i<=4; i++)
			{
				byBuff[i] = (a[i])/10*16 + (a[i])%10;
			}*/

			byBuff[0] = 0x01;
			byBuff[1] = 0x02;
			byBuff[2] = 0x03;
			byBuff[3] = 0x05;
			WORD re;
			HEXtoASC((LPBYTE)g_RandData,byBuff,4,&re);
			//xuzhe end

			memcpy(lpbyCDOLData + (*wCDOLDataSize), byBuff, wBuffSize);
			(*wCDOLDataSize) += wBuffSize;
		}
		//Add by LeeHai 2016.2.26 Tag95(TVR)
		/*else if (sCDOLTags[i].wTag == 0x95)
		{
			byBuff[0] = 0x80;
			byBuff[1] = 0x00;
			byBuff[2] = 0x04;
			byBuff[3] = 0x00;
			byBuff[4] = 0x00;

			memcpy(lpbyCDOLData + (*wCDOLDataSize), byBuff, wBuffSize);
			(*wCDOLDataSize) += wBuffSize;
		}*/
		//end
		/*else if(sCDOLTags[i].wTag == 0x9F02)
		{
			byBuff[0] = 0x00;
			byBuff[1] = 0x00;
			byBuff[2] = 0x02;
			byBuff[3] = 0x00;
			byBuff[4] = 0x00;
			byBuff[5] = 0x00;

			memcpy(lpbyCDOLData + (*wCDOLDataSize), byBuff, wBuffSize);
			(*wCDOLDataSize) += wBuffSize;
		}
		else if(sCDOLTags[i].wTag == 0x9C)
		{
			byBuff[0] = 0x01;

			memcpy(lpbyCDOLData + (*wCDOLDataSize), byBuff, wBuffSize);
			(*wCDOLDataSize) += wBuffSize;
		}*/
		else
		{
			memcpy(lpbyCDOLData + (*wCDOLDataSize), byBuff, wBuffSize);
			(*wCDOLDataSize) += wBuffSize;
		}
	}

	return OK;
}

// 得到Aqrc数据，数据组成顺序为CID/ATC/AC/IAD 9F27/9F36/9F26/9F10
int CICDAnalysis::IN_ICD_GetArqc(CHAR* Cid, CHAR* Atc, CHAR* Ac, CHAR* Iad)
{
	WORD wReturn = OK;

	//9f27数据，用来获取CID
	BYTE byCid[DATALENGTH] = {0x00};
	WORD wCidSize = 0;

	upicd->F_InfoGet(ICDPC_IC_CID, byCid, sizeof(byCid), &wCidSize, &wReturn);
	if (OK != wReturn)
	{
		return wReturn;
	}

	HEXtoASC((LPBYTE)Cid, byCid, wCidSize, &wReturn);
	myRelatedData[RelatedDataCount].Tag = ICDPC_IC_CID;
	myRelatedData[RelatedDataCount].Length = wCidSize;
	sprintf(myRelatedData[RelatedDataCount].Value, "%s", Cid);
	RelatedDataCount += 1;

	//9f36数据，用来获取ATC
	BYTE byAtc[DATALENGTH] = {0x00};
	WORD wAtcSize = 0;

	upicd->F_InfoGet(ICDPC_IC_ATC, byAtc, sizeof(byAtc), &wAtcSize, &wReturn);
	if (OK != wReturn)
	{
		return wReturn;
	}

	HEXtoASC((LPBYTE)Atc, byAtc, wAtcSize, &wReturn);
	myRelatedData[RelatedDataCount].Tag = ICDPC_IC_ATC;
	myRelatedData[RelatedDataCount].Length = wAtcSize;
	sprintf(myRelatedData[RelatedDataCount].Value, "%s", Atc);
	RelatedDataCount += 1;

	//9f26数据，用来获取AC
	BYTE byAc[DATALENGTH] = {0x00};
	WORD wAcSize = 0;

	upicd->F_InfoGet(ICDPC_IC_AC, byAc, sizeof(byAc), &wAcSize, &wReturn);
	if (OK != wReturn)
	{
		return wReturn;
	}

	HEXtoASC((LPBYTE)Ac, byAc, wAcSize, &wReturn);
	myRelatedData[RelatedDataCount].Tag = ICDPC_IC_AC;
	myRelatedData[RelatedDataCount].Length = wAcSize;
	sprintf(myRelatedData[RelatedDataCount].Value, "%s", Ac);
	RelatedDataCount += 1;

	//9f10数据，用来获取IAD
	BYTE byIad[DATALENGTH] = {0x00};
	WORD wIadSize = 0;

	upicd->F_InfoGet(ICDPC_IC_ISSUER_APPLICATION_DATA, byIad, sizeof(byIad), &wIadSize, &wReturn);
	if (OK != wReturn)
	{
		return wReturn;
	}

	HEXtoASC((LPBYTE)Iad, byIad, wIadSize, &wReturn);
	myRelatedData[RelatedDataCount].Tag = ICDPC_IC_ISSUER_APPLICATION_DATA;
	myRelatedData[RelatedDataCount].Length = wIadSize;
	sprintf(myRelatedData[RelatedDataCount].Value, "%s", Iad);
	RelatedDataCount += 1;

	return wReturn;
}

// 处理限制中的版本号检查
// bSameVersion: output： TRUE - 版本号相同， FALSE - 版本号不同
int CICDAnalysis::IN_ICD_CheckVersion(BOOL* bSameVersion)
{
	BYTE byIcVer[DATALENGTH] = {0x00};
	CHAR chIcVer[DATALENGTH] = {0};
	WORD wIcVersize = 0;
	BYTE byTMVer[DATALENGTH] = {0x00};
	CHAR chTMVer[DATALENGTH] = {0};
	WORD wTMVerSize = 0;
	WORD wReturn = OK;

	// 获取卡片中的应用版本号（0x9F08）
	upicd->F_InfoGet(ICDPC_IC_APPLICATION_VER_NO, byIcVer, sizeof(byIcVer), &wIcVersize, &wReturn);
	if (OK != wReturn && NODATA != wReturn)
	{
		return wReturn;
	}
	else if (NODATA == wReturn)		// 如果没有获取到版本号，则默认版本是一致的
	{
		*bSameVersion = TRUE;
		return OK;
	}

	HEXtoASC((LPBYTE)chIcVer, byIcVer, wIcVersize, &wReturn);

	// 如果获取到卡片应用版本号，则继续获取终端应用版本号
	// 获取终端应用版本号（0x9F09）
	upicd->F_InfoGet(ICDPC_TM_APPLICATION_VER_NO, byTMVer, sizeof(byTMVer), &wTMVerSize, &wReturn);
	if (OK != wReturn && NODATA != wReturn)    //如果终端没有返回应用版本，则默认应用版本相同。
	{
		return wReturn;
	}
	else if(NODATA == wReturn)
	{
		*bSameVersion = TRUE;
		return OK;
	}
	HEXtoASC((LPBYTE)chTMVer, byTMVer, wTMVerSize, &wReturn);

	if (strcmp(chTMVer, chIcVer))    // IC卡和终端的应用版本号不同
	{
		*bSameVersion = FALSE;
	}
	else 
	{
		*bSameVersion = TRUE;
	}
	
	return OK;
}

// 应用用途控制检查
// bNoService： output:  TRUE - 设置TVR的“卡片不允许所请求的服务”为1, FALSE - 设置TVR的“卡片不允许所请求的服务”为0
int CICDAnalysis::IN_ICD_CheckAUC(BOOL* bNoService)
{
	BYTE byIcAuc[DATALENGTH] = {0x00};	   // AUC
	WORD wIcAucsize = 0;
	BYTE byTMType[DATALENGTH] = {0x00};  // 终端类型
	WORD wTMTypeSize = 0;
	BYTE byIssuerCountryCode[DATALENGTH] = {0x00};  // 发卡行国家代码5F28
	CHAR chIssuerCountryCode[DATALENGTH] = {0};
	WORD wIssuerCountryCodeSize = 0;
	BYTE byTMCountryCode[DATALENGTH] = {0x00};    // 终端国家代码9F1A
	CHAR chTMCountryCode[DATALENGTH] = {0};
	WORD wTMCountryCode = 0;
	BYTE byTradeType[DATALENGTH] = {0x00};  // 交易类型
	WORD wTradeTypeSize = 0;
	WORD wReturn = OK;

	// 获取IC卡中的AUC
	upicd->F_InfoGet(ICDPC_IC_AUC, byIcAuc, sizeof(byIcAuc), &wIcAucsize, &wReturn);
	if (OK != wReturn && NODATA != wReturn)
	{
		return wReturn;
	}
	else if (NODATA == wReturn)
	{
		*bNoService = TRUE;
		return OK;
	}

	// 获取终端类型
	upicd->F_InfoGet(ICDPC_TM_TERMINAL_TYPE, byTMType, sizeof(byTMType), &wTMTypeSize, &wReturn);
	if (OK != wReturn && NODATA != wReturn)
	{
		return wReturn;
	}
	else if (NODATA == wReturn)
	{
		*bNoService = TRUE;
		return OK;
	}

	if (byTMType[0] == 0x14 && !(byIcAuc[0] & 0x20))    // 如果终端类型为ATM，AUC中的“在ATM有效”为不为1，则设置TVR“卡片不允许所请求的服务”位为1
	{
		*bNoService = TRUE;
		return OK;
	}
	else if (byTMType[0] != 0x14 && !(byIcAuc[0] & 0x01))    // 如果终端类型不是ATM，AUC中的“在非ATM终端上有效”为不为1，则设置TVR“卡片不允许所请求的服务”位为1
	{
		*bNoService = TRUE;
		return OK;
	}

	// 获取发卡行国家代码
	upicd->F_InfoGet(ICDPC_IC_ISSUER_COUNTRY_CODE, byIssuerCountryCode, sizeof(byIssuerCountryCode), &wIssuerCountryCodeSize, &wReturn);
	if (OK != wReturn && NODATA != wReturn)
	{
		return wReturn;
	}
	else if (NODATA == wReturn)
	{
		*bNoService = TRUE;
		return OK;
	}

	// AUC和发卡行国家代码都存在

	// 获取交易类型
	upicd->F_InfoGet(ICDPC_TM_TRANSACTION_TYPE, byTradeType, sizeof(byTradeType), &wTradeTypeSize, &wReturn);
	if (OK != wReturn && NODATA != wReturn)
	{
		return wReturn;
	}
	else if (NODATA == wReturn)
	{
		*bNoService = TRUE;
		return OK;
	}

	// 获取终端国家代码
	upicd->F_InfoGet(ICDPC_TM_TRANSACTION_TYPE, byTMCountryCode, sizeof(byTMCountryCode), &wTMCountryCode, &wReturn);
	if (OK != wReturn && NODATA != wReturn)
	{
		return wReturn;
	}
	else if (NODATA == wReturn)
	{
		*bNoService = TRUE;
		return OK;
	}

	// 应用用途检查时的终端行为
	// 如果交易类型为现金交易，交易类型见交行需求交易初始化的参数
	if (byTradeType[0] == 0x01 || byTradeType[0] == 0x02)
	{
		// 如果发卡行国家代码与终端国家代码匹配
		if (byIssuerCountryCode[0] == byTMCountryCode[0] && byIssuerCountryCode[1] == byTMCountryCode[1])
		{
			// 如果AUC中“国内现金交易有效”位不为1，则设置TVR“卡片不允许所请求的服务”位为1
			if (!(byIcAuc[0] & 0x80))
			{
				*bNoService = TRUE;
			}
		}
		else    // 发卡行国家代码与终端国家代码不匹配
		{
			// 如果AUC中“国际现金交易有效”位不为1，则设置TVR“卡片不允许所请求的服务”位为1
			if (!(byIcAuc[0] & 0x40))
			{
				*bNoService = TRUE;
			}
		}
	}
	else if (byTradeType[0] == 0x00)    // 如果交易类型为消费
	{
		// 如果发卡行国家代码与终端国家代码匹配
		if (byIssuerCountryCode[0] == byTMCountryCode[0] && byIssuerCountryCode[1] == byTMCountryCode[1])
		{
			// 如果AUC中“国内商品有效”位或者“国内服务有效”都不为1，则设置TVR“卡片不允许所请求的服务”位为1
			if (!(byIcAuc[0] & 0x20) && !(byIcAuc[0] & 0x08))
			{
				*bNoService = TRUE;
			}
		}
		else    // 发卡行国家代码与终端国家代码不匹配
		{
			// 如果AUC中“国际商品有效”位或者“国际服务有效”位都不为1，则设置TVR“卡片不允许所请求的服务”位为1
			if (!(byIcAuc[0] & 0x10) && !(byIcAuc[0] & 0x04))
			{
				*bNoService = TRUE;
			}
		}
	}
	// 如果交易中存在返现金额， ？？？？暂时保留，后期加入 


	return OK;
}

// 比较两个BYTE数组是否有相同的位同时为1, 有相同的位则返回1，否则返回0, 数组长度不同则返回-1
int CICDAnalysis::IN_ICD_CheckSameBit(LPBYTE byData1, WORD wDataSize1, LPBYTE byData2, WORD wDataSize2)
{
	if (wDataSize1 != wDataSize2)
	{
		return -1;
	}

	for (int i = 0; i < wDataSize1; i++)
	{
		if (byData1[i] & byData2[i])
		{
			return 1;
		}
	}

	return 0;
}

// 外部认证的报文
int CICDAnalysis::CAPDU_ExternalAuth(CHAR* ARPCExc, CHAR* C_Apdu)
{
	WORD wReturn = OK;
	CHAR ClaInsP1P2[9] = "00820000";
	CHAR Lc[3] = "";
	CHAR chArpc[DATALENGTH] = {0};
	CHAR chExc[5] = {0};
	WORD wArpcExcLength = strlen(ARPCExc);

	memcpy(chArpc, ARPCExc, wArpcExcLength-4);
	memcpy(chExc, ARPCExc + wArpcExcLength - 4, 4);
	sprintf(AuthReCode, "%s", chExc);

	sprintf(Lc, "%02X", strlen(ARPCExc)/2);
	sprintf(C_Apdu, "%s%s%s", ClaInsP1P2, Lc, ARPCExc);

	return wReturn;
}


// 读日志的报文
int CICDAnalysis::CAPDU_ReadLog(WORD P1, CHAR* C_Apdu)
{
	WORD wReturn = OK;
	CHAR ClaIns[5] = "00B2";
	CHAR P2[3] = "5C";
	CHAR le[3] = "00";

	sprintf(C_Apdu, "%s%02X%s%s", ClaIns, P1, P2, le);

	return wReturn;
}


// //Get ATC Data 9F36
int CICDAnalysis::CAPDU_GetAtcData(CHAR* C_Apdu)
{
	WORD	wReturn = OK;
    QString ClaIns("80CA");
    QString P1P2("9F36");
    QString le("00");

    sprintf(C_Apdu, "%s%s%s", Q2S(ClaIns), Q2S(P1P2), Q2S(le));

	return wReturn;
}


// Get LogFormat
int CICDAnalysis::CAPDU_GetLogFormat(CHAR* C_Apdu)
{
	WORD	wReturn = OK;
    QString ClaIns("80CA");
    QString P1P2("9F4F");
    QString le("00");

    sprintf(C_Apdu, "%s%s%s", Q2S(ClaIns), Q2S(P1P2), Q2S(le));

	return wReturn;
}


// 判断第二次Generate的类型
int CICDAnalysis::CAPDU_GeneratAc2(CHAR* C_Apdu)
{
	CHAR P1[3] = {0};
    if (strcmp(AuthReCode, "0000")||
        strcmp(AuthReCode, "0100")||
        strcmp(AuthReCode, "0101"))
	{
		sprintf(P1, "40");
		CAPDU_GenerateAc(ICDPC_IC_CDOL2, P1, C_Apdu);
	}
    else if (strcmp(AuthReCode, "0001")||
        strcmp(AuthReCode, "0010"))
	{
		sprintf(P1, "80");
		CAPDU_GenerateAc(ICDPC_IC_CDOL2, P1, C_Apdu);
	}
	
	return 0;
}



