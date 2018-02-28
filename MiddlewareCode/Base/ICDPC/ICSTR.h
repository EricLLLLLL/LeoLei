//////////////////////////////////////////////////////////////////////////////
// Header  = IC Data structure
// Date    = 2005/08/22
// VRT	   = 01-00-00-00
//////////////////////////////////////////////////////////////////////////////
#ifndef ICSTR_H
#define ICSTR_H

	// Candidate management data list
	struct Candidate{
		char szDFName[33];			// DF NAME Pointer
		char szAppliLabel[17];		// APLabel pointer
		char szAppPrefName[17];		// AP Pre pointer		 
		BYTE byIssCodeTblIndex[2];	// code table index	 
		char szLngPreference[9];	// pre words				 
		char szIIN[4];				// ID number			 
		BOOL bAuthorizedFlag;		// Authorized Flag
	};

	// Candidate management data list
	struct CandidateList{
		WORD wCandidateNum;			// Candidate Num
		Candidate stCandidate[64];	// Candidate management data list
	};

	// User defined AID
	struct TerminalAID {
		LPSTR lpsAID;							// AID String pointer
		BYTE byApplicationSelectionIndicator;	// Application Selection Indicator
		BYTE byAPVersionIndicator[2];			// version number of the application
        BYTE byTACDenial[5];                    // TACDenial         
        BYTE byTACOnline[5];                    // TACOnline         
        BYTE byTACDefault[5];                   // TACDefault        
	};

	// User defined AID list
	struct TerminalAIDList {
		WORD wNum;						// DFnumber(1～64)
		TerminalAID stTerminalAID[64];	// DFString pointer name
	};

	struct SettingInfo{
		char szIssuerID[8];	// ID buffer
		BOOL bIssuerIDFlg;	 
		char szKeyIndex[1];	// acquiring key index buffer
		BOOL bKeyIndexFlg;	 
		char szPME[10];		// PMEAcquisition buffer
		BOOL bPMEFlg;		 
		char szIFD[9];		// IFDAcquisition buffer
		BOOL bIFDFlg;		 
		char szMerID[15];	// Member ID			 
		BOOL bMerIDFlg;		 
		char szCtyCode[2];	// Country code			 
		BOOL bCtyCodeFlg;	 
		char szMerCCode[2];	// Member store classification code	 
		BOOL bMerCCodeFlg;	 
		char szPOSEMode[1];	// POSInput mode	 
		BOOL bPOSEModeFlg;	 
		char szTDOL[252];	// TDOL				 
		char szAcquirerId[6];													 
		BOOL bAcquirerIdFlg;													 
	};

	// Clear command list
	struct ClearCommandList {
		WORD wTag[30];
		WORD wCnt;
	};
	
	// Black list
	struct BlackList {
		WORD wPANNum;
		WORD wPANSeqNum;
		WORD wPANBCDSize[10000];
		BYTE byPAN[10000][10];
		BYTE byPANSequenceNumber[100];
	};

	struct Action {
		BOOL	bByteBit1_8;	// Not imple certification online data
		BOOL	bByteBit1_7;	// online certification fail
		BOOL	bByteBit1_6;	// IC card lost data
		BOOL	bByteBit1_5;	// Exception file on the terminal
		BOOL	bByteBit1_4;	// online certification fail
		BOOL	bByteBit1_3;	// DDA and AC fail
		BOOL	bByteBit1_2;	// prepare
		BOOL	bByteBit1_1;	// prepare
		BOOL	bByteBit2_8;	// The IC card and the terminal and the application have different 
		BOOL	bByteBit2_7;	// The application and revocation
		BOOL	bByteBit2_6;	// The application is not effective
		BOOL	bByteBit2_5;	// service is not allowed in this card
		BOOL	bByteBit2_4;	// New card
		BOOL	bByteBit2_3;	// prepare
		BOOL	bByteBit2_2;	// prepare
		BOOL	bByteBit2_1;	// prepare
		BOOL	bByteBit3_8;	// card owner certification fail
		BOOL	bByteBit3_7;	// ＣＶＭ not Distinguish
		BOOL	bByteBit3_6;	// ＰＩＮ over try
		BOOL	bByteBit3_5;	// ＰＩＮ function not exist
		BOOL	bByteBit3_4;	// Enter the pin requirements exist but do not enter
		BOOL	bByteBit3_3;	// Online PIN is input
		BOOL	bByteBit3_2;	// prepare
		BOOL	bByteBit3_1;	// prepare
		BOOL	bByteBit4_8;	// More than the trading limi
		BOOL	bByteBit4_7;	// More than the online processing possible maximum offline continuous terminal limit value
		BOOL	bByteBit4_6;	// More than the online processing not possible maximum continuous offline terminal limit value
		BOOL	bByteBit4_5;	// Online processing should choose random trade
		BOOL	bByteBit4_4;	// forced online transactions
		BOOL	bByteBit4_3;	// prepare
		BOOL	bByteBit4_2;	// prepare
		BOOL	bByteBit4_1;	// prepare
		BOOL	bByteBit5_8;	// Online data authentication has not been implemented
		BOOL	bByteBit5_7;	// Using the default TDOL
		BOOL	bByteBit5_6;	// The script before the GENERATEAC process failed
		BOOL	bByteBit5_5;	// The final GENERATEAC script processing after the failed
		BOOL	bByteBit5_4;	// prepare
		BOOL	bByteBit5_3;	// prepare
		BOOL	bByteBit5_2;	// prepare
		BOOL	bByteBit5_1;	// prepare
	};

	struct TAC {//action data terminal
		BOOL	bSwitchOrder;
		Action	stDenial;			 
		Action	stOnline;
		Action	stDefault;
	};

	struct CAPublicKey{    // CA公钥

	};

#endif
