#pragma once
#include "win_type_def.h"
// Information management TBL (InfoGet, InfoSet use)
struct InfoManagementTBL {
	WORD wItem;				// Item
	WORD wDataSize;			// DataSize
	LPBYTE lpbyBuffer;		// Data Buffer
	WORD wBuffSize;			// Buff Size
	WORD wMiniSize;			// Mini Size;
	BOOL bTagReadFlg;		// Tag Read Flg
};

// 数据应用列表，如 PDOL、CDOL1、CDOL2
struct DOL
{
	WORD wTag;
	WORD wDataSize;
};


class UPICD
{
public:
	UPICD(void);
	~UPICD(void);

	long F_Init(CHAR* configPath, LPWORD lpwReturn );
	long F_ICResponseMessageDataStatusAnalysis( LPBYTE lpbyData, WORD wDataSize,
		LPWORD lpwSw1Sw2Buffer, LPWORD lpwReturn );
	long F_ICResponseMessageDataAnalysis( WORD wCommand, LPBYTE lpbyData,
		WORD wDataSize, LPWORD lpwReturn );
	long F_GetCommandMessage( BYTE byCLA, BYTE byINS,
		BYTE byP1, BYTE byP2, WORD wFormat, BYTE byLe,
		LPWORD lpwCommandVDataFieldTBL,
		WORD wCommandVDataFieldTBLLength,
		LPWORD lpwCommandTLVDataFieldTBL,
		WORD wCommandTLVDataFieldTBLLength,
		LPBYTE lpbyData, WORD wDataSize, LPWORD lpwDataSize, LPWORD lpwReturn );
	long F_InfoSet( WORD wItem, LPBYTE lpbyBuffer, WORD wDataSize, LPWORD lpwReturn );
	long F_InfoGet( WORD wItem, LPBYTE lpbyBuffer, WORD wBufSize,
		LPWORD lpwDataSize, LPWORD lpwReturn );
	long F_InfoItemClear( WORD wItem, LPWORD lpwReturn );
	long F_MandatoryDataCheck( WORD wMandatoryType, LPWORD lpwReturn );
	long F_DataGet( WORD wItem, LPWORD lpwBuffer, LPWORD lpwReturn );
	long F_UserTerminalAIDSet( LPWORD lpwReturn );
	long F_SetAID( LPWORD lpwReturn );
	long F_VersionGet( LPSTR lpsVersion, WORD wBufSize,
		LPWORD lpwDataSize, LPWORD lpwReturn );
	long F_TvrTsiSet( WORD wItem, WORD wBytePosition,
		BYTE byBit, LPWORD lpwReturn );
	long F_BlackListCheck( LPWORD lpwReturn );
	long F_DOLRelationDataMake( WORD wItem, LPWORD lpwReturn );
	long F_DOLTLVAnalysis(WORD wItem, DOL* lpDOLTagList, LPWORD wDOLTagCount, LPWORD lpwReturn);
	long F_ApplicationTemplateTLVAnalysis(LPBYTE lpbyData, WORD wDataSize, LPWORD lpwReturn);
	long F_ScriptTLVAnalysis(LPBYTE lpbyData, WORD wDataSize, LPWORD lpwReturn);
	long F_IssuerScriptTemplateAnalySis(LPBYTE lpbyData, WORD wDataSize, CHAR* chScriptCmdData, LPWORD lpwScriptCmdCount, CHAR* chScriptID, LPWORD lpwReturn);

private:
	long N_ICTerminalFileRead( LPWORD lpwReturn );
	long N_UpicdConfigFileRead( LPWORD lpwReturn );
	long N_TerminalDataRead(LPWORD lpwReturn);
	//long N_ICTerminalActionCodeFileRead( LPWORD lpwReturn );
	long N_ICParameterFileRead( LPWORD lpwReturn );
	long N_BlackListFileRead( LPWORD lpwReturn );

	long N_SelectFileCommandAnalysis( LPBYTE lpbyData, WORD wDataSize, WORD wFCIType ,LPWORD lpwReturn);
	long N_GetProcessingOptionsCommandAnalysis( LPBYTE lpbyData, WORD wDataSize, LPWORD lpwReturn );
	long N_ReadRecordCommandAnalysis( LPBYTE lpbyData, WORD wDataSize, 
		WORD wReadRecordTemplate70Mode, LPWORD lpwReturn );
	long N_GenerateACCommandAnalysis( LPBYTE lpbyData, WORD wDataSize, LPWORD lpwReturn );
	long N_InternalAuthenticateCommandAnalysis( LPBYTE lpbyData, WORD wDataSize, LPWORD lpwReturn );
	long N_GetDataCommandAnalysis( LPBYTE lpbyData, WORD wDataSize, LPWORD lpwReturn );
	long N_GetChallengeCommandAnalysis(LPBYTE lpbyData, WORD wDataSize, LPWORD lpwReturn);

	long N_GetCommandMessage( BYTE byCLA,BYTE byINS,BYTE byP1,BYTE byP2,
		WORD wFormat, BYTE byLe,
		LPWORD lpwCommandVDataFieldTBL, WORD wCommandVDataFieldTBLLength,
		LPWORD lpwCommandTLVDataFieldTBL, WORD wCommandTLVDataFieldTBLLength,
		LPBYTE lpbyData, WORD wDataSize, LPWORD lpwDataSize, LPWORD lpwReturn );
	long N_TLVFormatAnalysis( WORD wCheckTagData, WORD wActionOrder,
		LPBYTE lpbyData, WORD wDataSize, LPWORD lpwAnalisisSize,
		LPWORD lpwVnLength, LPWORD lpwReturn );
	BOOL N_TagDefineCheck( LPBYTE lpbyData, WORD wBufferSize );
	long N_GetTagInfo( LPBYTE lpbyData, WORD wBufferSize, LPWORD lpwTag, LPWORD lpwAnalisisSize, LPWORD lpwReturn );
	void N_InfoTypeTblGetMember( WORD wTag, WORD wData, WORD **lpwBuffer, LPWORD lpwReturn );
	long N_ErrorHandlingForFCIResponseData( LPBYTE lpbyData, WORD wDataSize,
		WORD wAnalysisSize, WORD wVnLength, LPWORD lpwAnalysisSize_FCI, LPWORD lpwVnLength_FCI, LPWORD lpwReturn );
	long N_SetTLVLength( WORD wLength, LPBYTE lpbyBuff, WORD wBuffSize,
		LPWORD lpwDataSize, LPWORD lpwReturn );
	long N_GetLnVnLength( LPBYTE lpbyData, WORD wDataSize, LPWORD lpwLnLength,
		LPWORD lpwVnLength, LPWORD lpwAnalisisSize, LPWORD lpwReturn );
	WORD N_SkipReverseByte( LPBYTE lpbyBuf, BYTE bySkipCode, WORD wBufSize );
	long N_BCDtoJIS( LPBYTE lpbyInput, WORD wBcdSize,
		LPBYTE lpbyOutput, WORD wJisSize, LPWORD lpwJisSize, LPWORD lpwReturn );
	void N_DatesCheck(LPBYTE lpbyReadBufferPtr, LPWORD lpwReturn);
	int N_atoi( LPBYTE lpbyData, WORD wDataSize );
	void N_TagSetCheck( WORD wTag, LPWORD lpwReturn );
	long N_HostIssuerDataAnalysis( LPBYTE lpbyData, WORD wDataSize, LPWORD lpwReturn );
	void N_HEXtoASC(LPBYTE lpDest, LPBYTE lpSrc, WORD wSize, LPWORD lpwReturn);
	BOOL N_InitDataSet( WORD wItem, LPBYTE lpbyDataBuffer, WORD wBufferSize, WORD wValue );
	void N_ClearCommandListClear( WORD wCommand );
	long N_SetTLV( WORD wTag, WORD wLength, LPBYTE lpbyValue,
		WORD wBuffSize, LPBYTE lpbyBuff, LPWORD lpwDataSize,
		LPWORD lpwTLSize, LPWORD lpwReturn );
	long N_SetTag( WORD wTag, LPBYTE lpbyBuff, WORD wBuffSize,
		LPWORD lpwDataSize, LPWORD lpwReturn );
	void N_TagSizeGet(WORD wTag, LPWORD lpwTagSize);
	BOOL N_DOLTagCheck(WORD wItem);
	long N_GetOneByteLength( LPBYTE lpbyData, WORD wDataSize, LPWORD lpwLnLength,
		LPWORD lpwVnLength, LPWORD lpwAnalisisSize, LPWORD lpwReturn );
	long N_SetOneByteLength( WORD wLength, LPBYTE lpbyBuff, WORD wBuffSize,
		LPWORD lpwDataSize, LPWORD lpwReturn );
	void N_ASCtoHEX( LPBYTE lpDest, LPBYTE lpSrc, WORD wSize, LPWORD lpwReturn );
	long N_JIStoBCD(LPBYTE lpbyInput, WORD lpwJisSize,
		LPBYTE lpbyOutput, WORD wBcdSize, LPWORD lpwBcdSize, LPWORD lpwReturn );

private:
	CHAR m_configPath[MAX_PATH];
	
};

