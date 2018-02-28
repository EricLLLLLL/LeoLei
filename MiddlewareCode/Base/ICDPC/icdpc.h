/***********************************************
  *@CopyRight：和美（深圳）信息技术股份有限公司
  *@date:2018-01-30 16:32:04
  *@author:xiaoyp
  *@brief:
    导出icdpc接口类

***********************************************/
#pragma once
#include "win_type_def.h"
#include <QtCore/qglobal.h>

#if defined(ICDPC_LIBRARY)
#  define ICDPCSHARED_EXPORT Q_DECL_EXPORT
#else
#  define ICDPCSHARED_EXPORT Q_DECL_IMPORT
#endif

#define DATALENGTH													257
#define DATATIME													32

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
#define	ICDPC_COMMAND_MAX											11


//#define ICDPC_SELECTPSEAID											21
//#define ICDPC_SELECTADF												22
//#define ICDPC_SELECTDDF												23
//#define ICDPC_SELECTRECORD											24			//选择时的Readrecord
//#define ICDPC_GPOSELECT												25			//Gpo前面的Select,Get PDOL信息
//#define ICDPC_GPOREAD												26			//GPO命令
//#define ICDPC_READAPP												27			//读取应用数据
//#define ICDPC_GETDATA												28			//读取余额
//
//#define ICDPC_GETPROOPTION											31			//应用初始化

//Return Status
#define ICDPC_RETURN_SUCESS											0x9000



//ATMDEVICE?
#define ICDPC_TEM_ATMDEVICE											50
#define ICDPC_TEM_NOTATMDEVICE										51


#define OK						0
#define NG						1
#define NODATA					2

class CICDAnalysis;
//struct CHAR*
//{
//	CHAR*		Apdu;			//命令的发送报文
//	int			ApduLength;			//命令报文的长度
//};
class ICDPCSHARED_EXPORT  CICDPC
{
private:
	CICDAnalysis *m_Icd;
public:
	CICDPC(void);
	~CICDPC(void);

	//初始化PBOC配置
	// configPath	in  配置文件路径
	int ICDPC_Init(CHAR* configPath);

	//R_Apdu数据传入
	//Apdu		in,
	//Count		in
	int ICDPC_AnalysisR_Apdu(WORD Command, CHAR* Apdu);
	//C_Apdu数据传出
	//Apdu		out
	//aIn		in
	//Count		out
	int ICDPC_GetC_Apdu(WORD Command, CHAR* Apdu, CHAR* aIn, int Count);
	//选择PSE文件
	int ICDPC_GetPSEAID(CHAR* aOut);
	////选择应用
	//int ICDPC_SelectADF(CHAR* aIn);
	//交易初始化
	int ICDPC_GetProOption(CHAR* aOut);
	////读应用数据
	//int ICDPC_ReadAppData(CHAR* aAFL);
	//读卡片余额
	int ICDPC_ReadBalance(CHAR* aOut);
	//读取卡片的最高限额
	int ICDPC_ReadBalanceLimit(CHAR* aOut);
	//获取卡片日期信息
	int ICDPC_CardDateInfo(CHAR* aOut);
	//获取持卡人信息
	int ICDPC_GetHolderInfo(CHAR* aOut);
	// 脱机数据认证，检测终端和IC卡是否支持CDA、DDA、SDA数据认证
	int ICDPC_CheckDataAuthentication(BOOL* bSupportCDA, BOOL* bSupportDDA, BOOL* bSupportSDA);
	// 处理限制
	int ICDPC_ProcessLimit();
	// 终端行为分析
	int ICDPC_TeminalActiveAnaiysis(CHAR* C_APDU);
	//圈存、圈取初始化
	int ICDPC_InitForLoad(CHAR* CardDataExc, CHAR* ForArqc, CHAR* Arqc);
	//获取55域数据
	int ICDPC_ReadIcTLV(CHAR* aPAan, CHAR* aInAD, CHAR* aInAR, CHAR* aOut);
	//解析外部认证指令的返回, 输入R_APDU为External Auth命令的响应报文, 输出chStatus为SW1SW2
	int ICDPC_ExternalAuth(CHAR* R_APDU, CHAR* chStatus);
	//脚本分析
	int ICDPC_AnalyScript(CHAR* aInscript, CHAR* ch71ScriptID, CHAR* ch72ScriptID, CHAR* ch71ScriptCmdList, LPWORD lpw71ScriptCmdCount, CHAR* ch72ScriptCmdList, LPWORD lpw72ScriptCmdCount);
	//圈存圈取结束
	int ICDPC_FinishLoad(CHAR* aOut);
	// 执行脚本命令后返回的报文分析, 
	// 输入chTag为“71”或“72”
	// 输入wNum是指脚本命令的序号（从1开始），
	// 输入R_APDU为脚本处理命令返回的报文，
	// 输入chScriptID为脚本标识
	// 输出ReScript
	int ICDPC_ScriptResult(CHAR* chTag, WORD wNum, CHAR* chScriptID, CHAR* R_APDU, CHAR* ReScript);
	//读取卡片交易日志
	int ICDPC_ReadLog(CHAR* aIn, CHAR* aOut);
	//解析2磁道
	int ICDPC_GetTrack2Data(CHAR* aOut);
	// R_Apdu分析后的状态返回
	int ICDPC_AnalysisR_ApduStatus(CHAR* Apdu, LPWORD DataStatus);
	// 根据标签获取对应的数据
	int ICDPC_InfoGet(WORD wItem, CHAR* chBuffer);

public:
	// pse选择capdu获取
	int ICDPC_AssGetPSEAID_Capdu(CHAR* aPSERAID, CHAR* Apdu);
	// 第一个ReadRecord命令，上层根据读取的返回值确定是否更改P1读下去，aCommond为Read命令
	int ICDPC_AssSelectReadRecord_Capdu(WORD aCommand, WORD wP1, CHAR* C_APDU);
	// select for gpo
	int ICDPC_AssSelectADF_Capdu(CHAR* aADF, CHAR* C_Apdu);
	// Need time TradeMoneyType
	int ICDPC_AssGetProOption_Capdu(int aTradeMoneyType, int aTradeMoney, int aTradeType, int aTime, CHAR* C_Apdu);
	// 取得AFL,分析出C_Apdu[n]
	int ICDPC_AssReadAppData_Capdu(CHAR* aAfl, CHAR* C_Apdu);
	// Get Data command
	int ICDPC_AssGetBalance_Capdu(CHAR* C_Apdu);
	// Get Balance limited Data by Getdata command
	int ICDPC_AssBalanceLimit_Capdu(CHAR* C_Apdu);
	// Get ATC Data
	int ICDPC_AssGetATCData_Capdu(CHAR* C_Apdu);
	// 组织External Auth CAPDU，输入ARPCExc为ARPC+4字节的授权响应码
	int ICDPC_AssExternalAuth_Capdu(CHAR* ARPCExc, CHAR* C_Apdu);
	// 获取卡片日志格式的C_APDU（GET DATA）
	int ICDPC_GetLogFormat_Capdu(CHAR* C_Apdu);
	// 获取详细日志的C_APDU, P1：第P1条日志 （READ RECORD）
	int ICDPC_ReadLog_Capdu(WORD P1, CHAR* C_Apdu);
	// Aid选择方式的C_APDU
	int ICDPC_AidSelect_Capdu(CHAR* strAid, CHAR* C_Apdu);
};

