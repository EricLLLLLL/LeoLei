///////////////////////////
//		Create by art
//////////////////////////

#include "stdafx.h"
#include "dsnd.h"
#include <time.h>
#include <QTime>
#include <QDateTime>
#include <stdio.h>
#include <QDebug>
#include <QString>

CDSND* CDSND::m_pInstance = NULL;

int g_TransFields[][100] = 
{ //create by art
	{-1},	
	{2,3,11,12,13,22,23,35,36,41,48,52,55,61,62,64, -1},		//1  查询余额        
	{2,3,4,11,12,13,22,23,35,36,41,49,52,55,61,62,64, -1},		//2  取款
	{2,3,4,11,12,13,22,23,35,36,41,49,52,61,62,64, -1},		//3  存款
	{2,3,11,12,13,22,23,35,36,41,48,55,64, -1},				//4  取款冲正
	{2,3,4,11,12,13,22,41,48,61,62,64, -1},				//5  账户验证 //
	{3,12,13,41,60, -1},									//6  申请工作参数
	{2,3,4,11,12,13,22,41,49,61,62,64, -1},			//7  无卡无折存款 
	{3,11,12,13,41,61,62,64, -1},			//8  查询主机现金箱余额
	{2,3,11,12,13,22,23,35,36,41,48,52,55,61,62,64, -1},			//9  改密
	{2,3,4,11,12,13,22,41,61,62,64, -1},			//10  查询账户状态 
	{2,3,4,11,12,13,22,41,61,62,64, -1},			//11  查询对公账户状态
	{2,3,4,11,12,13,22,41,48,49,61,62,64, -1},			//12  对公存款 
	{2,3,4,11,12,13,22,41,61,62,64, -1},		//13  查询客户已取金额
	{2,3,4,11,12,13,22,41,61,62,64, -1},		//14  查询卡类型 
	{3,4,11,12,13,22,41,61,62,64, -1},		//15  销户查询 2017.9.21.1
	{2,3,4,11,12,13,22,41,61,62,64, -1},		//16  销户
	{-99}	//结束标志，后面不许加交易
};

char* g_TransProcessCode[][2] = 
{ //create by art
	{"",""},	
	{"0200","310000"},		//1  查询余额 ATM:"310000"
	{"0200","600000"},		//2  大额取款 ATM:"010000"
	{"0200","620000"},		//3  大额存款 	ATM:"0220" "210000"
	{"0420","600000"},		//4  取款冲正 ,ATM:"010000"
	{"0200","330000"},		//5  账户验证 ATM: "330000"
	{"0800","920000"},		//6  申请工作参数 ATM "920000"
	{"0220","210000"},		//7  无卡无折存款 	ATM:"0220" "210000" //tmp 广东农行临时用无卡折小额
	{"0200","390000"},		//8  查询主机现金箱余额 	ATM:"0200" "390000"
	{"0200","950000"},		//9  改密 	ATM:"0200" "950000"
	{"0200","910000"},		//10  查询账户状态 	ATM:"0200" "910000"
	{"0200","630000"},		//11  查询对公账户状态 	
	{"0200","620000"},		//12  对公存款
	{"0200","640000"},      //13  查询客户已取金额
	{"0200","650000"},		//14  查询卡类型
	{"0200","660000"},      //15  销户查询 2017.9.21.1
	{"0200","700000"},      //16  销户
};

char* g_CwcReason[] =
{//create by art
	"0201", //0 未赋值  用超时代替
	"0201",	//1 超时
	"0101", //2 出钞失败
	"0402",	//3 响应报文错
	"0301",	//4 发卡行验证失败
	"0201"  //5 其它 用超时代替
};

CDSND *CDSND::GetInstance()
{
    if(m_pInstance == NULL)
    {
        m_pInstance = new CDSND();
    }
    return m_pInstance;
}

CDSND::CDSND(QObject* pParent) : QObject(pParent)
{
    printf("");
}

CDSND::~CDSND(void)
{

}

short CDSND::GetDataString(BOOL bPersistent, char *pszDataName, char *pszValue)
{
	short nRet=dllDSTO.D_GetDataString(bPersistent, pszDataName, pszValue);
    if(nRet!=0)
        LOGERROR("GetData [%s] error ", pszDataName);
	return nRet;
}

short CDSND::GetDataLong(BOOL bPersistent, char *pszDataName, long *plongValue)
{
	short nRet=dllDSTO.D_GetDataLong(bPersistent, pszDataName, plongValue);
    if(nRet!=0)
        LOGERROR("GetData [%s] error ", pszDataName);
	return nRet;
}


short CDSND::InitDataString(BOOL bPersistent, char *pszDataName, char *pszNewValue)
{
	short nRet=dllDSTO.D_SetDataString(bPersistent, pszDataName, pszNewValue);
    if(nRet!=0)
        LOGERROR("InitData [%s] error ", pszDataName);
	return nRet;
}

long CDSND::PackageData(const char *pszDealType, char *pszBuffer, long *pnBufLen, char *pszMacData, long *pnMacDataLen)
{ //create by art
	if(pszDealType==NULL || strlen(pszDealType)==0 || pszBuffer==NULL)
	{
		return -1;
	}
    LOGINFO("CABC8583Package::PackageData [pszDealType]=%s", pszDealType);

	//对接收的报文结果发送前先进行初始化
	InitDataString(FALSE, "RESPONSECODE", "");
	InitDataString(FALSE, "AUTHCODE", "");
	InitDataString(FALSE, "SYSREFNUM", "");
	InitDataString(FALSE, "CURRENTBALANCE", "");
	InitDataString(FALSE, "AVAILABLEBALANCE", "");
	//InitDataString(FALSE, "CUSTOMERNAME", "");
	InitDataString(FALSE, "PINKEY", "");
	InitDataString(FALSE, "MACKEY", "");
	InitDataString(FALSE, "POUNDAGE", "");
	InitDataString(FALSE, "HOSTAMOUNT", ""); //主机现金箱余额


	long nRet=0;
	int nBufLen=0, nMacDataLen=0;
	TRADE_TYPE enType=enTradeType_NONE;
	if(strcmp(pszDealType,"APPLYWORKPARAM") == 0)
	{
		enType=enTradeType_APPLYWORKPARAM;
	}
	else if(strcmp(pszDealType,"INQ") == 0)
	{
		enType=enTradeType_INQ;
	}
	else if(strcmp(pszDealType,"QRYCUSTNAME") == 0)
	{
		enType=enTradeType_QRYCUST;
	}
	else if(strcmp(pszDealType,"CWD") == 0)
	{
		enType=enTradeType_CWD;
	}
	else if(strcmp(pszDealType,"DEP") == 0)
	{
		enType=enTradeType_DEP;
	}
	else if(strcmp(pszDealType,"CWC") == 0)
	{
		enType=enTradeType_CWC;
	}
	else if(strcmp(pszDealType,"NOCARDDEP") == 0)
	{
		enType=enTradeType_NOCARDDEP;
	}
	else if(strcmp(pszDealType,"QRYHOSTAMOUNT") == 0)
	{
		enType=enTradeType_QRYHOSTAMOUNT;
	}
	else if(strcmp(pszDealType,"PWD") == 0)
	{
		enType=enTradeType_PWD;
	}
	else if(strcmp(pszDealType,"QRYACCOUNTSTATE") == 0)
	{
		enType=enTradeType_QRYACCOUNTSTATE;
	}
	else if(strcmp(pszDealType,"QRYBUSSINESSACCOUNT") == 0)
	{
		enType=enTradeType_QRYBUSSINESSACCOUNT;
	}
	else if(strcmp(pszDealType,"BUSSINESSDEP") == 0)
	{
		enType=enTradeType_BUSSINESSDEP;
	}
	else if(strcmp(pszDealType,"QRYCWDMONEY") == 0)
	{
		enType=enTradeType_QRYCWDMONEY;
	}
	else if (strcmp(pszDealType,"QRYACCOUNTTYPE") == 0)
	{
		enType = enTradeType_QRYACCOUNTTYPE;
	}
	else if(strcmp(pszDealType,"INQC") == 0) //2017.9.21.1
	{
		enType=enTradeType_QRYACCOUNTCANCELLATION;
	}
	else if(strcmp(pszDealType,"ACCDELETE") == 0)
	{
		enType=enTradeType_ACCOUNTCANCELLATION;
	}

	nRet =CreateMsg(enType, pszBuffer, nBufLen, pszMacData, nMacDataLen);

	if(nRet==0)
	{
		*pnBufLen=nBufLen;
		*pnMacDataLen=nMacDataLen;
	}
	else
	{
        LOGINFO("CABC8583Package::PackageData [pszDealType]=%s  nRet=%d", pszDealType, nRet);
	}
	
	return nRet;
}

void CDSND::SetBitMap(char *BitMap,short FiledsNum, bool IsSet)
{
	if(FiledsNum > 64||FiledsNum < 1)
	{
        LOGINFO("CABC8583Package::SetBitMap [FiledsNum]=%d", FiledsNum);
		return ;
	}
	char tmp = 0x00;
	int index = (FiledsNum-1)/8;
	int i = (FiledsNum-1)%8;
	tmp = 0x01;
	if(IsSet)
	{
		tmp = (tmp<<(7-i));
		BitMap[index] |= tmp;
	}
	else
	{
		tmp = ~(tmp<<(7-i));
		BitMap[index] &= tmp;
	}

}

void CDSND::AscToBcd(unsigned char *BcdData,char *AscData,const int AscDataLen)
{
	if(NULL==BcdData||AscData==NULL)
		return ;
	int iLen = 0,i = 0,j = 0,index = 0,iMoveFlag;
	iLen =(AscDataLen + AscDataLen%2) / 2;
	memset(BcdData,0x00,iLen);
	for(i = 0; i < AscDataLen; i++)
	{
		index = (i/2);
		iMoveFlag = ((i%2) == 0? 4:0);
		if( (AscData[i] >= '0') && (AscData[i] <= '9'))
		{
			BcdData[index] |= ((AscData[i] - '0')&0x0f)<<iMoveFlag;
		}
		else if( (AscData[i] >= 'a') && (AscData[i] <= 'f'))
		{
			BcdData[index] |= ((AscData[i] - 'a' + 10)& 0x0f)<<iMoveFlag;
		}
		else if( (AscData[i] >= 'A') && (AscData[i] <= 'F') )
		{
			BcdData[index] |= ((AscData[i] - 'A' + 10)& 0x0f)<<iMoveFlag;
		}
		else
		{
			BcdData[index]|=(0x00 & 0x0f)<<iMoveFlag;
		}
	}
	return;
}

void CDSND::BcdToAsc(char *AscData,unsigned char *BcdData,const int AcsDataLen)
{
	if(NULL == BcdData||AscData== NULL)
		return ;
	int	i = 0, index = 0,iMoveFlag = 0;
	char tmp = 0x00;
	AscData[AcsDataLen] = 0x00;
	for(i = 0; i < AcsDataLen; i++) 
	{
		index = i/2;
		iMoveFlag = ((i%2) == 0? 4:0);
		tmp = (BcdData[index] >> iMoveFlag) & 0x0f;
		if(tmp > 0x09)
			AscData[i] = (tmp - 0x0A ) + 'A';
		else	
			AscData[i] = tmp + '0';
	}
}

int CDSND::CreateMsg(const TRADE_TYPE enType, char *pszOutData,int& nOutDataLen,char *pszOutMacData,int& nOutMacDataLen)
{ //create by art
	if(NULL == pszOutData)
	{
		return -1;
	}
	nOutMacDataLen=0;
	nOutDataLen=0;

	bool bHaveMac=false;
	bool bIcCard=false;
	char szData[2048] = {0},szTmp[1024] = {0},szTmpAsc[500] = {0},BitMap[8+1]={0},szVarLen[10]={0};
	int iDataLen = 2, iTmpLen = 0;

    QDateTime st;
    st = QDateTime::currentDateTime();


	GetDataString(FALSE, "CARDTYPE", szTmpAsc);
	if( memcmp(szTmpAsc,"CHIPCARD", strlen("CHIPCARD")) ==0 )
		bIcCard=true;
	
	char szAccountNo[20]={0};

	//报文类型
	memset(szTmp,0x00,sizeof(szTmp));
	memset(szTmpAsc,0x00,sizeof(szTmpAsc));
	//add by tsx for ATM存取款 begin
	GetDataString(FALSE, "ATMORTCR", szTmpAsc);
	if(strcmp(szTmpAsc, "ATM") == 0 && (enType == enTradeType_CWD)){
		memcpy(szTmpAsc , "0200", 6);
	}else if(strcmp(szTmpAsc, "ATM") == 0 && (enType == enTradeType_DEP)){
		memcpy(szTmpAsc , "0220", 6);
	}else {
		memcpy(szTmpAsc , g_TransProcessCode[enType][0], 4);
	}
	//add by tsx for ATM存取款 end
	AscToBcd((unsigned char *)szTmp , szTmpAsc , 4);
	memcpy(szData + iDataLen, szTmp, 2);
	iDataLen += 2;
	//位图
	iDataLen += 8;

	//LOGINFO("CABC8583Package::CreateMsg [g_TransFields]=%d", enType);
	int nFieldNum=0;
	for(int i=0; g_TransFields[enType][i]!=-1; i++)
	{
		memset(szTmp,0x00,sizeof(szTmp));
		memset(szTmpAsc,0x00,sizeof(szTmpAsc));
		memset(szVarLen,0x00,sizeof(szVarLen));

		nFieldNum=g_TransFields[enType][i];

		switch(nFieldNum)
		{
		case 2: //主账号 --2
			
			GetDataString(FALSE, "CWDDEALTYPE", szTmpAsc);
			if(strcmp(szTmpAsc, "PASSBOOK") == 0 )
			{
				char szPBTmpAsc[100];
				GetDataString(FALSE, "CARDNO", szPBTmpAsc);
				
				memcpy( szTmpAsc, "41", 2);//深圳存折卡号前需要加41
				memcpy( szTmpAsc + 2, szPBTmpAsc, strlen(szPBTmpAsc));
			}else{
				GetDataString(FALSE, "CARDNO", szTmpAsc);
			}
			//add by grb for 存折取款 end
			iTmpLen = strlen(szTmpAsc);
			if(iTmpLen ==0)
			{
				return -2;
			}
			memcpy(szAccountNo, szTmpAsc, sizeof(szAccountNo)); //最大长度不超过szAccountNo

			SetBitMap(BitMap,nFieldNum);
			sprintf(szVarLen,"%02d",iTmpLen);
			AscToBcd((unsigned char *)szTmp , szVarLen , 2);
			memcpy(szData + iDataLen, szTmp,1);
			iDataLen += 1;
			memset(szTmp,0x00,sizeof(szTmp));
			iTmpLen = (iTmpLen+iTmpLen%2)/2;
			AscToBcd((unsigned char*)szTmp, szTmpAsc, 2*iTmpLen);
			memcpy(szData + iDataLen, szTmp, iTmpLen);
			iDataLen += iTmpLen;			
			break;
		case 3: 
			//add by tsx for ATM存取款 begin
			GetDataString(FALSE, "ATMORTCR", szTmpAsc);
			if(strcmp(szTmpAsc, "ATM") == 0 && (enType == enTradeType_CWD))
			{
				memset(szTmpAsc,0x00,sizeof(szTmpAsc));
				memcpy(szTmpAsc , "010000", 6);
			}else if(strcmp(szTmpAsc, "ATM") == 0 && (enType == enTradeType_DEP)){
				memset(szTmpAsc,0x00,sizeof(szTmpAsc));
				memcpy(szTmpAsc , "210000", 6);
			}else {
				memset(szTmpAsc,0x00,sizeof(szTmpAsc));
				memcpy(szTmpAsc , g_TransProcessCode[enType][1], 6);
			}
			//add by tsx for ATM存取款 end
			SetBitMap(BitMap,nFieldNum);
			AscToBcd((unsigned char *)szTmp , szTmpAsc , 6);
			memcpy(szData + iDataLen, szTmp, 3);
			iDataLen += 3;
			break;
		case 4: //交易金额
			{
				SetBitMap(BitMap,nFieldNum);
				GetDataString(FALSE, "TRANSAMOUNT", szTmpAsc);
				int nAmountStrLen=strlen(szTmpAsc);
				char szHeadZero[12+1];
				memset(szHeadZero, 0x30, sizeof(szHeadZero));
				szHeadZero[12]=0x00;
				//LOGINFO("test by art(Get Field4) szTmpAsc: %s nAmountStrLen:%d  szHeadZero: %s", szTmpAsc,nAmountStrLen,szHeadZero );
				memcpy(szHeadZero+12-nAmountStrLen, szTmpAsc, nAmountStrLen);

				AscToBcd((unsigned char *)szTmp, szHeadZero, 12);
				//LOGINFO("test by art(Get Field4) szTmpAsc: %s nAmountStrLen:%d  szHeadZero: %s, szTmp: %s", szTmpAsc,nAmountStrLen,szHeadZero,szTmp );
				memcpy(szData + iDataLen, szTmp, 6);
				iDataLen += 6;
			}
			break;
		case 11: //ATM流水号 --11
			{
				SetBitMap(BitMap,nFieldNum);
				long nJnlnum=0;
				GetDataLong(TRUE, "JNLNUM", &nJnlnum);
				sprintf(szTmpAsc,"%06d", nJnlnum);//获取流水号
				AscToBcd((unsigned char *)szTmp , szTmpAsc , 6);
				memcpy(szData + iDataLen, szTmp, 3);
				iDataLen += 3;
			}
			break;
		case 12: //交易时间  -12
			SetBitMap(BitMap,nFieldNum);
            //QString strBuffer2 = st.toString("HHmmss");
            sprintf(szTmpAsc,"%s","");
            AscToBcd((unsigned char *)szTmp , szTmpAsc, 6);
			memcpy(szData + iDataLen, szTmp, 3);
			iDataLen += 3;
			break;
		case 13: //交易日期  --13
			SetBitMap(BitMap,nFieldNum);
            //QString strBuffer13 = st.toString("MM-dd");
            //sprintf(szTmpAsc,"%s",strBuffer13.toLatin1().data());
            AscToBcd((unsigned char *)szTmp , szTmpAsc , 4);
			memcpy(szData + iDataLen, szTmp, 2);
			iDataLen += 2;
			break;
		case 22: //输入方式 --22
			SetBitMap(BitMap,nFieldNum);
			if(enType==enTradeType_NOCARDDEP)
			{
				strcpy(szTmpAsc,"0012");  //OK: tmp by art : old: "0011"  
			}else if(enType==enTradeType_QRYACCOUNTSTATE 
				|| enType==enTradeType_QRYCUST) //(enType==enTradeType_QRYCUST && bIsNoCardTrans)
			{
				strcpy(szTmpAsc,"0012");
			}
			else if(bIcCard)
			{
				LONG FallBackCardFlg=0;
				GetDataLong(false,"FALLBACKCARD",&FallBackCardFlg);
				if(FallBackCardFlg == 0)
				{
					strcpy(szTmpAsc,"0051");
				}
				else/////降级交易
				{
					strcpy(szTmpAsc,"0921");
				}
				
			}
			else
			{
				strcpy(szTmpAsc,"0901");
			}
			
			AscToBcd((unsigned char *)szTmp , szTmpAsc , 4);
			memcpy(szData + iDataLen, szTmp, 2);
			iDataLen += 2;
			break;
		case 23: //IC卡序号 --23
			if(bIcCard)
			{
				char szIcSN[5]={0};
				GetDataString(FALSE, "ICSERIALNUM", szIcSN);
				sprintf(szTmpAsc,"%04d", atoi(szIcSN));
				SetBitMap(BitMap,nFieldNum);
				AscToBcd((unsigned char *)szTmp , szTmpAsc , 4);
				memcpy(szData + iDataLen, szTmp, 2);
				iDataLen += 2;
			}
			break;
		case 35: //二磁道 ---35
			/*//tmp by art begin
			if(enType==enTradeType_NOCARDDEP){
				memcpy(szTmpAsc, "0000000000000000000d00000000000000000", 37);
				//memcpy(szTmpAsc, szAccountNo, strlen(szAccountNo));
			}else{
				GetDataString(FALSE, "TRACK2DATA", szTmpAsc);
			}*/
			GetDataString(FALSE, "TRACK2DATA", szTmpAsc);
			iTmpLen = strlen(szTmpAsc);
			if(iTmpLen > 0)
			{
				SetBitMap(BitMap,nFieldNum);
				sprintf(szVarLen,"%02d",iTmpLen);
				AscToBcd((unsigned char *)szTmp , szVarLen , 2);
				memcpy(szData + iDataLen, szTmp, 1);
				iDataLen += 1;
				iTmpLen = (iTmpLen+iTmpLen%2)/2;
				AscToBcd((unsigned char*)szTmp, szTmpAsc, 2*iTmpLen);
				memcpy(szData + iDataLen, szTmp, iTmpLen);
				iDataLen += iTmpLen;
			}
			break;
		case 36: //三磁道 ---36
			//tmp by art begin
			//if(bIsNoCardTrans&& (enType==enTradeType_QRYACCOUNTSTATE 
			//	|| enType==enTradeType_QRYCUST || enType==enTradeType_NOCARDDEP))
			//{
			//	break;
				/*
				memcpy(szTmpAsc, "990000000000000000000d000000000000000000000000000000000000000dd000000000000d000000000000d000000000000000", 104);
				memcpy(szTmpAsc+2, szAccountNo, strlen(szAccountNo));
				iTmpLen = strlen(szTmpAsc);
				if(iTmpLen > 0)
				{
					SetBitMap(BitMap,nFieldNum);
					sprintf(szVarLen,"%04d",iTmpLen);
					AscToBcd((unsigned char *)szTmp , szVarLen , 4);
					memcpy(szData + iDataLen, szTmp, 2);
					iDataLen += 2;
					iTmpLen = (iTmpLen+iTmpLen%2)/2;
					AscToBcd((unsigned char*)szTmp, szTmpAsc, 2*iTmpLen);
					memcpy(szData + iDataLen, szTmp, iTmpLen);
					iDataLen += iTmpLen;
				}
				//*/
			//}
			//else
			//tmp by art end
			if(!bIcCard)
			{
				/*
				//add by grb for 存折取款 begin
				GetDataString(FALSE, "CWDDEALTYPE", szTmpAsc);
				if(strcmp(szTmpAsc, "PASSBOOK") == 0 && enType == enTradeType_CWD)
				{
					char szPBTmpAsc[100]={0};
					GetDataString(FALSE, "TRACK3DATA", szPBTmpAsc);
					LOGINFO("TRACK3DATA = [%s] ", szPBTmpAsc);
					if(strlen(szPBTmpAsc)==0){
						LOGINFO("TRACK3DATA_lenth==0");
						break;
					}
					memcpy( szTmpAsc, szPBTmpAsc, strlen(szPBTmpAsc)+1);
				}else{
					GetDataString(FALSE, "TRACK3DATA", szTmpAsc);
				}
				//add by grb for 存折取款 end
				*///modify by tsx 深圳农行不需判断。
				GetDataString(FALSE, "TRACK3DATA", szTmpAsc);  //modify by tsx 深圳农行不需判断。
				iTmpLen = strlen(szTmpAsc);
				if(iTmpLen > 0)
				{
					SetBitMap(BitMap,nFieldNum);
					sprintf(szVarLen,"%04d",iTmpLen);
					AscToBcd((unsigned char *)szTmp , szVarLen , 4);
					memcpy(szData + iDataLen, szTmp, 2);
					iDataLen += 2;
					iTmpLen = (iTmpLen+iTmpLen%2)/2;
					AscToBcd((unsigned char*)szTmp, szTmpAsc, 2*iTmpLen);
					memcpy(szData + iDataLen, szTmp, iTmpLen);
					iDataLen += iTmpLen;
				}
			}
			break;
		case 41: //获取ATM终端号 --41
			SetBitMap(BitMap,nFieldNum);
			GetDataString(TRUE, "TERMINALNUM", szTmpAsc);
			memcpy(szData + iDataLen, szTmpAsc, 8);
			iDataLen += 8;
			break;
		case 48: //附加数据
			if(enType==enTradeType_CWC)
			{
				SetBitMap(BitMap,nFieldNum);
				
				long nCwcJnlnum=0;
				GetDataLong(FALSE, "CWCJNLNUM", &nCwcJnlnum);
				long nCwcReason=0;
				GetDataLong(FALSE, "CWCREASON", &nCwcReason);
				sprintf(szTmpAsc,"%06d%s",nCwcJnlnum, g_CwcReason[nCwcReason]);
				iTmpLen  = strlen(szTmpAsc);
				sprintf(szVarLen,"%04d",iTmpLen);
				AscToBcd((unsigned char*)szTmp,szVarLen,4);
				memcpy(szData + iDataLen, szTmp, 2);
				iDataLen += 2;
				memcpy(szData + iDataLen, szTmpAsc, iTmpLen);
				iDataLen += iTmpLen;
			}
			else if(enType==enTradeType_PWD)
			{
				SetBitMap(BitMap,nFieldNum);
	
				GetDataString(FALSE, "NEWPINBLOCK", szTmpAsc);
				
				iTmpLen  = strlen(szTmpAsc);
				sprintf(szVarLen,"%04d",iTmpLen);
				AscToBcd((unsigned char*)szTmp,szVarLen,4);
				memcpy(szData + iDataLen, szTmp, 2);
				iDataLen += 2;
				memcpy(szData + iDataLen, szTmpAsc, iTmpLen);
				iDataLen += iTmpLen;
			}
			else if(enType==enTradeType_BUSSINESSDEP)
			{
				SetBitMap(BitMap,nFieldNum);

				GetDataString(FALSE, "BUSSINESSDEPINFO", szTmpAsc);

				iTmpLen  = strlen(szTmpAsc);
				sprintf(szVarLen,"%04d",iTmpLen);
				AscToBcd((unsigned char*)szTmp,szVarLen,4);
				memcpy(szData + iDataLen, szTmp, 2);
				iDataLen += 2;
				memcpy(szData + iDataLen, szTmpAsc, iTmpLen);
				iDataLen += iTmpLen;
			}
			else if(enType==enTradeType_QRYCUST)
			{
				SetBitMap(BitMap,nFieldNum);

				memcpy(szTmpAsc,"06", 2);
				iTmpLen  = strlen(szTmpAsc); //存款查户名
				sprintf(szVarLen,"%04d",iTmpLen);
				AscToBcd((unsigned char*)szTmp,szVarLen,4);
				memcpy(szData + iDataLen, szTmp, 2);
				iDataLen += 2;
				memcpy(szData + iDataLen, szTmpAsc, iTmpLen);
				iDataLen += iTmpLen;
			}else if (enType == enTradeType_INQ)  //add by grb for 余额查询
			{
				GetDataString(FALSE, "BALANCEINQUIRYTYPE", szTmpAsc);
				if(strcmp(szTmpAsc, "ACCTYPE") == 0)
				{
					SetBitMap(BitMap, nFieldNum);
					iTmpLen = strlen(szTmpAsc);
					sprintf(szVarLen,"%04d",iTmpLen);
					AscToBcd((unsigned char*)szTmp,szVarLen,4);
					memcpy(szData + iDataLen, szTmp, 2);
					iDataLen += 2;
					memcpy(szData+iDataLen, "ACCTYPE", iTmpLen);
					iDataLen += iTmpLen;
				} 

			}else if (enType == enTradeType_QRYACCOUNTCANCELLATION)  //销户查询 2017.9.21.1
			{
				SetBitMap(BitMap,nFieldNum);
				GetDataString(FALSE, "IDNUMBER", szTmpAsc);//获取身份证号码
				memcpy(szData + iDataLen, szTmpAsc, 18);
				iDataLen += 18;
			}else if (enType == enTradeType_ACCOUNTCANCELLATION)  //销户
			{
				SetBitMap(BitMap,nFieldNum);

				GetDataString(FALSE, "IDNUMBER", szTmpAsc);//获取身份证号码
				memcpy(szData + iDataLen, szTmpAsc, 18);
				iDataLen += 18;

				memcpy(szTmpAsc,"10", 2);//销户标识
				//iTmpLen  = strlen(szTmpAsc); 
				//sprintf(szVarLen,"%04d",iTmpLen);
				//AscToBcd((unsigned char*)szTmp,szVarLen,4);
				//memcpy(szData + iDataLen, szTmp, 2);
				iDataLen += 2;
				//memcpy(szData + iDataLen, szTmpAsc, iTmpLen);
				//iDataLen += iTmpLen;

				GetDataString(FALSE, "TRANSFERINACC", szTmpAsc);//TRANSFERINACC 转入账号
				memcpy(szData + iDataLen, szTmpAsc, 19);
				iDataLen += 19;

			}
			break;
		case 49: //货币代码
			SetBitMap(BitMap,nFieldNum);
			memcpy(szData + iDataLen, "156", 3);
			iDataLen += 3;
			break;
		case 52: //个人识别号密文 --52
			SetBitMap(BitMap,nFieldNum);
			if(enType==enTradeType_PWD)
			{
				GetDataString(FALSE, "OLDPINBLOCK", szTmpAsc);
			}
			else
			{
				GetDataString(FALSE, "PINBLOCK", szTmpAsc);
			}
			AscToBcd((unsigned char *)szTmp , szTmpAsc ,16);
			memcpy(szData + iDataLen, szTmp, 8);
			iDataLen += 8;
			break;
		case 55: //IC卡数据域 ----55
			if(bIcCard)
			{
				GetDataString(FALSE, "ICCARDDATA", szTmpAsc);
				iTmpLen = strlen(szTmpAsc);
				if(iTmpLen > 0)
				{
					SetBitMap(BitMap,nFieldNum);
                    sprintf(szVarLen,"%04d",(iTmpLen + iTmpLen%2)/2);
					AscToBcd((unsigned char *)szTmp , szVarLen , 4);
					memcpy(szData + iDataLen, szTmp, 2);
					iDataLen += 2;
					memset(szTmp,0x00,sizeof(szTmp));
					AscToBcd((unsigned char *)szTmp , szTmpAsc , iTmpLen);
					memcpy(szData + iDataLen, szTmp, (iTmpLen+iTmpLen%2)/2);
					iDataLen += (iTmpLen+iTmpLen%2)/2;
				}
			}
			break;
		case 60: //操作员号
			SetBitMap(BitMap,nFieldNum);
			memcpy(szTmpAsc,"00", 2);
			memcpy(szData + iDataLen, szTmpAsc, strlen(szTmpAsc));
			iTmpLen  = strlen(szTmpAsc);
			sprintf(szVarLen,"%04d",iTmpLen);
			AscToBcd((unsigned char*)szTmp,szVarLen,4);
			memcpy(szData + iDataLen, szTmp, 2);
			iDataLen += 2;
			memcpy(szData + iDataLen, szTmpAsc, iTmpLen);
			iDataLen += iTmpLen;
			break;
		case 61: //ATM状态 ----61
			{
				SetBitMap(BitMap,nFieldNum);
			
				char szDevState[128]={0};
				GetDataString(FALSE, "DEVICESTATE", szDevState);//目前只获取的是1-5号钞箱数据12*5=60个

				//memcpy(szTmpAsc,"1111555111551000000000001156501001005156501001000156501001000000000000000",73); //test tmp: ATM
				//memcpy(szTmpAsc,"1111555111551156501001500156500501000156500202000000000000000156500101000",73); //test tmp: TCR
				memcpy(szTmpAsc,"1111555111551", 13); //各模块状态 //test tmp 
				memcpy(szTmpAsc+13,szDevState, 60);//1-5号钞箱数据
				iTmpLen  = strlen(szTmpAsc);
				sprintf(szVarLen,"%04d",iTmpLen);
				AscToBcd((unsigned char*)szTmp,szVarLen,4);
				memcpy(szData + iDataLen, szTmp, 2);
				iDataLen += 2;
				memcpy(szData + iDataLen, szTmpAsc, iTmpLen);
				iDataLen += iTmpLen;
			}
			break;
		case 62: //工作参数 ----62
			SetBitMap(BitMap,nFieldNum);
			//memcpy(szTmpAsc,"YH08.01.070000002000010060030306001016006211220",47); //
			//memcpy(szTmpAsc,"HMTCR02.520000002000005060030306001016006243330",47); //test tmp: ATM
			memcpy(szTmpAsc,"HMTCR02.520000002000005060030306001016006233343",47); //test tmp：TCR
			iTmpLen  = strlen(szTmpAsc);
			sprintf(szVarLen,"%04d",iTmpLen);
			AscToBcd((unsigned char*)szTmp,szVarLen,4);
			memcpy(szData + iDataLen, szTmp, 2);
			iDataLen += 2;
			memcpy(szData + iDataLen, szTmpAsc, iTmpLen);
			iDataLen += iTmpLen;
			break;
		case 64: //Mac域 --64
			bHaveMac=true;
			SetBitMap(BitMap,nFieldNum);
			//设置位图域
			memcpy(szData + 4,BitMap,8);
			nOutMacDataLen = (iDataLen - 2);   //输出nOutMacDataLen
			memcpy(pszOutMacData,szData + 2,nOutMacDataLen);  //输出nOutMacDataLen
			break;
		default:
			//未知域
			break;
		}
		LOGINFO("enType:%d  nFieldNum:%d  iDataLen:%d",enType,nFieldNum, iDataLen);
	}

	//设置位图域
	memcpy(szData + 4,BitMap,8);

	int nMsgLength=0;
	if(bHaveMac)
	{
		nOutMacDataLen = (iDataLen - 2);   //输出nOutMacDataLen
		memcpy(pszOutMacData,szData + 2,nOutMacDataLen);  //输出nOutMacDataLen

		nMsgLength=iDataLen+8-2;  //报文长度应包括Mac长度8
	}
	else
	{
		nMsgLength=iDataLen-2;
	}

	memset(szTmpAsc,0x00,sizeof(szTmpAsc));
	sprintf(szTmpAsc,"%04X",nMsgLength); 
	AscToBcd((unsigned char*)szTmp,szTmpAsc,4);
	memcpy(szData,szTmp,2);//报文长度

	memcpy(pszOutData,szData,iDataLen); //输出未包括Mac域的报文, 当包含mac域时完整报文应为 pszOutData+Mac值
	nOutDataLen = iDataLen;	 //输出未包括Mac域的报文长度   

	return 0;
}

QObject *CreateInstance()
{
    return CDSND::GetInstance();
}
