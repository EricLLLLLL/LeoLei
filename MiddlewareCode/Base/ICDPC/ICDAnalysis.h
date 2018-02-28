//////////////////////////////////////////////////////////////////////////////
//文件名			：ICDAnalysis.h 
//创建者			：LeeHai&Xiongjie
//创建时间		：2015-09-06 9:37
//功能描述		：定义ICDAnalysis的头文件。
//
//修改履历		：2015-09-06 9:37:26  LeeHai  新建
//
//////////////////////////////////////////////////////////////////////////////
#pragma once
#include "UPICD.h"
#include "UPICDPRM.h"
#include "Upicddef.h"
#include "icdpc.h"
#include "QString"

struct IcRelatedData 
{
	WORD Tag;
	WORD Length;
	CHAR Value[MAX_PATH];
};


class CICDAnalysis
{
private:
	int Count;
	UPICD *upicd;			//UPICD句柄
	CHAR m_configpath[MAX_PATH];

	CHAR chCvr[11];			//Cvr
	IcRelatedData myRelatedData[DATALENGTH];	//需要的数据域
	WORD RelatedDataCount;
	CHAR AuthReCode[5];		//授权响应码
	
	//交行临时解决方案，Add by LeeHai, 2015-12-30
	CHAR chToArqc[128];		//产生Arqc的数据，
	CHAR chArqc[128];		//Arqc
	//end

	CHAR m_ch71ScriptID[TAGDATASIZE];	// 标签71的发卡行脚本标识
	CHAR m_ch72ScriptID[TAGDATASIZE];	// 标签72的发卡行脚本标识

	CHAR m_ch71ScriptCmdList[COMMANDMESSAGEDATASIZE];	// 标签71的发卡行脚本命令
	CHAR m_ch72ScriptCmdList[COMMANDMESSAGEDATASIZE];	// 标签72的发卡行脚本命令

	WORD m_w71ScriptCmdCount;	// 标签71的脚本命令个数
	WORD m_w72ScriptCmdCount;	// 标签72的脚本命令个数

	WORD wT_TradeMoney;
	WORD wT_TradeMoneyExc;
	WORD wT_Country;
	WORD wT_MoneyType;
	WORD wT_TradeType;
    QString strT_BankName;
    QString strT_TemCapability;
    QString strT_TemType;
    QString strT_AdditionalCapability;


private:
	// 设置Cvm表
	int IN_ICD_SetCvmData(void);
	// 设置CVR表
	int IN_ICD_SetCvrData(void);
	// 组织CDOL数据
	int IN_ICD_SetCDOLData(WORD wItem, LPBYTE lpbyCDOLData, LPWORD wCDOLDataSize, LPBYTE lpbyExtraData, LPWORD wExtraDataSize);
	// 得到Aqrc数据，数据组成顺序为CID/ATC/AC/IAD 9F27/9F36/9F26/9F10
	int IN_ICD_GetArqc(CHAR* Cid, CHAR* Atc, CHAR* Ac, CHAR* Iad);
	// 应用版本号检查，输出参数bSameVersion版本号是否一致
	int IN_ICD_CheckVersion(BOOL* bSameVersion);
	// 应用用途控制检查，输出参数bNoService为是否要设置TVR的“卡片不允许所请求的服务”为1
	int IN_ICD_CheckAUC(BOOL* bNoService);
	// 比较两个BYTE数组是否有相同的位同时为1, 有相同的位则返回1，否则返回0，数组长度不同则返回-1
	int IN_ICD_CheckSameBit(LPBYTE byData1, WORD wDataSize1, LPBYTE byData2, WORD wDataSize2);
	

private:
	void ASCtoHEX( LPBYTE lpDest, LPBYTE lpSrc, WORD wSize, LPWORD lpwReturn );
	void HEXtoASC( LPBYTE lpDest, LPBYTE lpSrc, WORD wSize, LPWORD lpwReturn );
	
public:
	CICDAnalysis(void);
	~CICDAnalysis(void);
	//初始化PBOC配置，输入configPath配置文件路径
	int ICD_Init(CHAR* configPath);
	// 分析RApdu，参数为CHAR*结构/数据长度Length
	int ICD_RApduAnalysis(WORD Command, CHAR* Apdu);
	// 组织C_Apdu，参数为CHAR*结构/数据长度Length/数据次数
	int ICD_CApduAssorted(WORD Command, CHAR* Apdu, CHAR *aData, int Count);
	// Analysis GetPSEAID APDU
	int CAPDU_GetPSEAID(CHAR* aPSEAID, CHAR* C_Apdu);
	// 选择命令紧接着的ReadRecord命令
	int CAPDU_SelReadRecord(WORD wCommand, CHAR* P1, CHAR* C_APDU);
	// GPO命令前的Select命令，确定和选择应用，通过调用这条命令获得PDOL
	int CAPDU_SelectADF(CHAR* wADF, CHAR* C_Apdu);
	// 应用初始化，CApdu为输出参数
	int CAPDU_GetProOption(int wTradeMoneyType, int wTradeMoney, int wMoneyExc, int wTradeType, int wTime, CHAR* CApdu);
	// //读取应用,Get wAfl  in,Send C_Apdu[wCommandCount]
	int CAPDU_ReadAppData(CHAR* wAFL, CHAR* C_Apdu);
	// get data 9f79
	int CAPDU_GetBalance(CHAR* C_Apdu);
	// //get data 9f77
	int CAPDU_BalanceLimit(CHAR* C_Apdu);
	// GenerateAc命令，P1参数代表AAC或者ARQC命令
	int CAPDU_GenerateAc(WORD Command, CHAR* P1, CHAR* C_Apdu);
	// 外部认证的报文
	int CAPDU_ExternalAuth(CHAR* ARPCExc, CHAR* C_Apdu);
	// 脚本分析
	int CAPDU_ScriptAnalysis(CHAR* Script, CHAR* ch71ScriptID, CHAR* ch72ScriptID, CHAR* ch71ScriptCmdList, LPWORD lpw71ScriptCmdCount, CHAR* ch72ScriptCmdList, LPWORD lpw72ScriptCmdCount);
	// 处理日志的报文
	int CAPDU_ReadLog(WORD P1, CHAR* C_Apdu);
	// Get Data 9F36
	int CAPDU_GetAtcData(CHAR* C_Apdu);
	// 判断第二次Generate的类型
	int CAPDU_GeneratAc2(CHAR* C_Apdu);
	// Aid列表选择方式
	int CAPDU_AidSelect(CHAR *strAid, CHAR* C_APDU);

public:
	// RApdu的SW1SW2的状态值分析
	int ICD_RApduStatusAnalysis(CHAR* Apdu, LPWORD DataStatus);
	// 选择文件方式选择应用，
	//aOut：应用需要的文件数据组合AD/LB/PT（应用路径标识/应用标签/应用优先级
	int ICD_GetPSEAID(CHAR* aOut);
	
	// 选择应用，具体没有操作
	int ICD_SelectADF(void);
	
	// 应用初始化 wAFL 文件定位器
	int ICD_GetProOption(CHAR* chAFL);
	
	// Analysis the wBalance return
	int ICD_ReadBalance(CHAR* wBalance);
	
	// Analysis to get balance limit
	int ICD_BalanceLimit(CHAR* wBalanceLimit);

	// Get ap Start Date and End Date
	int ICD_CardDateInfo(CHAR* StartDateEndDate);

	// Get Holder infomation  wHoldInfo=chName/chNameExc/chIdeData/chIdeType=5F20/9F0B/9F61/9F62
	int ICD_GetHolderInfo(CHAR* wHoldInfo);
	// 相关数据检查
	int ICD_TemplateCheck(void);
	// 处理限制，具体包括：应用版本号检查/应用用途控制/应用生效日期检查/应用失效日期检查
	int ICD_ProcessLimit();
	// 持卡人验证，第一步返回所有符合条件的CVM Code
	int ICD_HolderCheckCVMCode(LPBYTE byCVMCodeList, LPWORD wCVMCodeCount);
	// 终端风险管理，包括最低限额检查、随机选择交易、频度检查；分别为三个函数 预留待补充？？？
	int ICD_TemRiskManage(void);
	// 终端行为分析，主要是TAC||IAC分别与TVR的比较，并返回Generate AC命令的C_APDU
	int ICD_TemActiveAnaiysis(CHAR* C_APDU);
	// 获取55域数据，根据ARQC去得到55域数据
	int ICD_Re55FieldData(CHAR* Arqc, CHAR* FiftyFiveData);
	// 解析外部认证的指令返回，输入R_Apdu为External Auth的响应报文，输出chStatus为SW1SW2
	int ICD_ExternalAuth(CHAR* R_Apdu, CHAR* chStatus);
	// 执行脚本命令后返回的报文分析, 输入chTag为“71”或“72”，输入wNum是指脚本命令的序号（从1开始），输入R_APDU为脚本处理命令返回的报文，输出ReScript
	int ICD_ScriptResult(CHAR* chTag, WORD wNum, CHAR* chScriptID, CHAR* R_APDU, CHAR* ReScript);
	// 交易结束的标志位读第二次Cdol,并分析结果是否返回TC的数据
	int ICD_AnalyEndTag(CHAR* Tc);
	// 读取卡片日志
	int ICD_ReadLog(CHAR* chLogData, CHAR* Log);
	// 读取二磁道数据并解析出来
	int ICD_GetTrack2Data(CHAR* Track2Data);
	// 圈存圈取初始化，标志为第一次的GenerateAc命令的结束
	int ICD_InitForLoad(CHAR* CardDataExc, CHAR* ForArqc, CHAR* Arqc);	
	// 圈存圈提结束
	int ICD_FinishLoad(CHAR* chOut);
	// Get LogFormat
	int CAPDU_GetLogFormat(CHAR* C_Apdu);
	// 脱机数据认证，检查终端和IC卡是否支持CDA、DDA、SDA脱机认证
	int ICD_DaDataCheck(BOOL* bSupportCDA, BOOL* bSupportDDA, BOOL* bSupportSDA);
	// 静态数据认证
	int ICD_StaticDataAuthentication();
	// 根据标签获取对应的数据
	int ICD_InfoGet(WORD wItem, CHAR* chBuffer);
	

	// 获取55域数据，根据ARQC去得到55域数据
	int ICD_Re55FieldDataExc(CHAR* Arqc, CHAR* FiftyFiveData);
	
};

