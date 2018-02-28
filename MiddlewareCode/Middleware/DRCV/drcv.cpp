///////////////////////////
//		Create by art
//////////////////////////

#include "stdafx.h"
#include "drcv.h"

CDRCV* CDRCV::m_pInstance = NULL;

char* g_TransResponseProcessCode[][2] = 
{ //create by art
	{"",""},	
	{"0210","310000"},		//1  查询余额 ATM:"310000"
	{"0210","600000"},		//2  取款 ATM:"010000"
	{"0210","620000"},		//3  存款 	ATM:"0220" "210000"
	{"0430","600000"},		//4  取款冲正 ,ATM:"010000"
	//{"0210","380000"},		//5  查户名 ATM: "380000"
	{"0210","330000"},		//5  账户验证 ATM: "330000"
	{"0810","920000"},		//6  申请工作参数 ATM "920000"
	//{"0210","620000"},		//7  无卡无折存款 	ATM:"0220" "210000"
	{"0230","210000"},		//7  无卡无折存款，//tmp 广东农行暂用小额 	ATM:"0220" "210000"
	{"0210","390000"},		//8  查询主机现金箱余额 	ATM:"0210" "390000"
	{"0210","950000"},		//9  改密 	ATM:"0210" "950000"
	{"0210","910000"},		//10  查询账户状态 	ATM:"0210" "910000"
	{"0210","630000"},		//11  查询对公账户状态 	
	{"0210","620000"},		//12  对公存款 
	{"0210","640000"},      //13  查询客户已取金额	
};

CDRCV *CDRCV::GetInstance()
{
    if(m_pInstance == NULL)
    {
        m_pInstance = new CDRCV();
    }
    return m_pInstance;
}

CDRCV::CDRCV(QObject* pParent):QObject(pParent)
{
    //GetModuleFileName((HMODULE)AfxGetStaticModuleState()->m_hCurrentInstanceHandle, m_strConfigPath.GetBuffer(MAX_PATH), MAX_PATH);    //获取当前程序运行目录，包含应用程序的文件名
    //m_strConfigPath.ReleaseBuffer();
    //m_strConfigPath = m_strConfigPath.Left(m_strConfigPath.ReverseFind('\\'));
    //m_strConfigPath += "\\ini\\Setup.ini";
}

CDRCV::~CDRCV(void)
{

}

short CDRCV::SetDataResult(BOOL bPersistent, char *pszDataName, char *pszNewValue)
{
	short nRet=dllDSTO.D_SetDataString(bPersistent, pszDataName, pszNewValue);
	if(nRet!=0)
		LOGERROR("SetData [%s] error ", pszDataName);
	return nRet;
}

long CDRCV::ParseData(const char *pszDealType, const char *pszBuffer, const long nBufLen)
{
	if(nBufLen==0 || NULL == pszBuffer)
	{
        LOGERROR("CDRCV::ParseData error!");
		return -1;
	}

    LOGINFO("CDRCV::ParseData Start");


	char szMsgType[10]={0};//报文消息类型
	char BitMap[16+1]={0};
	char szTmp[1024]={0};
	char szTmpAsc[1024] = {0};
	const char *pszSrcData=pszBuffer;
	int nDataPos = 0;
	int nDataLen = 0;
	int index = 0;
	int iMoveFlag = 0;
	int iBit = 0;
	int iTmpLen = 0;

	SetDataResult(FALSE, "PINKEY", ""); //初始化
	SetDataResult(FALSE, "MACKEY", "");	//初始化
	SetDataResult(FALSE, "HOSTAMOUNT", "");	//初始化 主机现金箱余额
	SetDataResult(FALSE, "SYSREFNUM", "");	//初始化 37域后台返回交易码 by tsx
	SetDataResult(FALSE, "RESPONSECARDTYPE", "");	//初始化 查余额返回卡类型 by tsx
	SetDataResult(FALSE, "QRYCWDMONEY", "");	//初始化 查余额返回卡类型 by tsx
	

	//取长度
	memset(szTmp,0x00,sizeof(szTmp));
	memcpy(szTmp,pszSrcData,2);
	nDataPos += 2;
	nDataLen = (BYTE)szTmp[0]*256+(BYTE)szTmp[1];
	LOGINFO("Message length：%d",nDataLen);
	//报文类型
	memset(szTmp,0x00,sizeof(szTmp));
	memcpy(szTmp,pszSrcData + nDataPos,2);
	nDataPos += 2;
	BcdToAsc(szMsgType,(unsigned char*)szTmp,4);
	LOGINFO("Message type: %s",szMsgType);
	//位图
	memset(szTmpAsc,0x00,sizeof(szTmpAsc));
	memcpy(BitMap,pszSrcData + nDataPos,8);
	nDataPos += 8;
	BcdToAsc(szTmpAsc,(unsigned char*)BitMap, 32);
	LOGINFO("Bitmap：%s", szTmpAsc);
	for(int i = 0;i < 64;i++)
	{
		index = i/8;
		iMoveFlag = (i%8)==0?0:(i%8);
		iBit = (BitMap[index]>>(7-iMoveFlag))&0x01;
		if(iBit == 0x01)
		{
			switch(i+1)
			{
			//case 1:
			//	memset(szTmpAsc,0x00,sizeof(szTmpAsc));
			//	BcdToAsc(szTmpAsc,(unsigned char*)BitMap,16);
			//	break;
			case 2: //主账号
				memset(szTmpAsc,0x00,sizeof(szTmpAsc));
				memset(szTmp,0x00,sizeof(szTmp));
				memcpy(szTmp,pszSrcData + nDataPos,1);//可变长度为1个字节BCD码
				nDataPos += 1;
				BcdToAsc(szTmpAsc,(unsigned char*)szTmp,2);
				iTmpLen = atoi(szTmpAsc);
				memset(szTmp,0x00,sizeof(szTmp));
				memcpy(szTmp,pszSrcData + nDataPos,(iTmpLen+1)/2);
				nDataPos += (iTmpLen+1)/2;
				BcdToAsc(szTmpAsc,(unsigned char*)szTmp,iTmpLen);

				LOGINFO("Accountnum: %s", szTmpAsc);
				break;
			case 3: //处理代码
				memset(szTmpAsc,0x00,sizeof(szTmpAsc));
				memset(szTmp,0x00,sizeof(szTmp));
				memcpy(szTmp,pszSrcData + nDataPos,3);
				nDataPos += 3;
				BcdToAsc(szTmpAsc,(unsigned char*)szTmp,6);
				LOGINFO("PrecessCode: %s", szTmpAsc);
				if(IsValidTransType(pszDealType, szMsgType, szTmpAsc)!=0)
				{
					return -8;
				}
				break;
			case 4: //交易金额
				memset(szTmpAsc,0x00,sizeof(szTmpAsc));
				memset(szTmp,0x00,sizeof(szTmp));
				memcpy(szTmp,pszSrcData + nDataPos,6);
				nDataPos += 6;
				BcdToAsc(szTmpAsc,(unsigned char*)szTmp,12);
				LOGINFO("Amount: %s", szTmpAsc);
				break;
			/*
			case 5:
				break;
			case 6:
				break;
			case 7:
				break;
			case 8:
				break;
			case 9:
				break;
			case 10:
				break;
			*/
			case 11://流水号
				memset(szTmpAsc,0x00,sizeof(szTmpAsc));
				memset(szTmp,0x00,sizeof(szTmp));
				memcpy(szTmp,pszSrcData + nDataPos,3);
				nDataPos += 3;
				BcdToAsc(szTmpAsc,(unsigned char*)szTmp,6);

				if(strcmp(pszDealType,"APPLYWORKPARAM") == 0)
				{//流水号初始化
					//add by art for 流水号是否通过申请工作参数初始化
                    int nInitJnlNum = 1;//GetPrivateProfileInt("JNLNUM","NeedInitJnlNum", 1, m_strConfigPath);
					if(1==nInitJnlNum)
					{
						BOOL bError=FALSE;
						try
						{
							short nRetTmp=dllDSTO.D_SetDataLong(TRUE, "JNLNUM", atoi(szTmpAsc));					
							if(nRetTmp!=0)
								bError=TRUE;
							else
								LOGINFO("APPLYWORKPARAM init [JNLNUM] OK: %s ", szTmpAsc);
						}
						catch(...)
						{
							bError=TRUE;	
						}
						if(bError)
							LOGINFO("APPLYWORKPARAM init [JNLNUM] error: %s ", szTmpAsc);
					}
				}
				LOGINFO("Serialnum: %s", szTmpAsc);
				break;
			case 12: //交易时间
				memset(szTmpAsc,0x00,sizeof(szTmpAsc));
				memset(szTmp,0x00,sizeof(szTmp));
				memcpy(szTmp,pszSrcData + nDataPos,3);
				nDataPos += 3;
				BcdToAsc(szTmpAsc,(unsigned char*)szTmp,6);
				LOGINFO("DealTime: %s", szTmpAsc);
				break;
			case 13: //交易日期
				memset(szTmpAsc,0x00,sizeof(szTmpAsc));
				memset(szTmp,0x00,sizeof(szTmp));
				memcpy(szTmp,pszSrcData + nDataPos,2);
				nDataPos += 2;
				BcdToAsc(szTmpAsc,(unsigned char*)szTmp,4);
				LOGINFO("DealDate: %s", szTmpAsc);
				break;
			/*
			case 14:
				break;
			case 15:
				break;
			case 16:
				break;
			case 17:
				break;
			case 18:
				break;
			case 19:
				break;
			case 20:
				break;
			case 21:
				break;
			*/
			case 22: //输入方式
				memset(szTmpAsc,0x00,sizeof(szTmpAsc));
				memset(szTmp,0x00,sizeof(szTmp));
				memcpy(szTmp,pszSrcData + nDataPos,2);
				nDataPos += 2;
				BcdToAsc(szTmpAsc,(unsigned char*)szTmp,4);
				LOGINFO("Entrytype: %s", szTmpAsc);
				break;
			case 23: //卡序列号
				memset(szTmpAsc,0x00,sizeof(szTmpAsc));
				memset(szTmp,0x00,sizeof(szTmp));
				memcpy(szTmp,pszSrcData + nDataPos,2);
				nDataPos += 2;
				BcdToAsc(szTmpAsc,(unsigned char*)szTmp,4);
				LOGINFO("IcSerialnum: %s", szTmpAsc);
				break;
			/*
			case 24:
				break;
			case 25:
				break;
			case 26:
				break;
			case 27:
				break;
			case 28:
				break;
			case 29:
				break;
			*/
			case 30: //手续费 D+n8
				memset(szTmp,0x00,sizeof(szTmp));
				memcpy(szTmp,pszSrcData + nDataPos,1);
				nDataPos += 1;
				memset(szTmp,0x00,sizeof(szTmp));
				memcpy(szTmp,pszSrcData + nDataPos,8);
				nDataPos += 8;

				{
					int nPoundage=0;
					nPoundage=atoi(szTmp);
					char szPoundage[16]={0};
					sprintf(szPoundage,"%d", nPoundage);					
					SetDataResult(FALSE,"POUNDAGE", szPoundage);
				}
				LOGINFO("Poundage: %s", szTmp);
				break;
			/*
			case 31:
				break;
			case 32:
				break;
			case 33:
				break;
			case 34:
				break;
			*/
			case 35:
				memset(szTmpAsc,0x00,sizeof(szTmpAsc));
				memset(szTmp,0x00,sizeof(szTmp));
				memcpy(szTmp,pszSrcData + nDataPos,1);
				nDataPos += 1;
				BcdToAsc(szTmpAsc,(unsigned char*)szTmp,2);
				iTmpLen = atoi(szTmpAsc);
				memset(szTmp,0x00,sizeof(szTmp));
				memcpy(szTmp,pszSrcData + nDataPos,(iTmpLen+1)/2);
				nDataPos += (iTmpLen+1)/2;
				LOGINFO("Field35 :[len:%d] %s", iTmpLen, szTmp);
				break;
			case 36:
				memset(szTmpAsc,0x00,sizeof(szTmpAsc));
				memset(szTmp,0x00,sizeof(szTmp));
				memcpy(szTmp,pszSrcData + nDataPos,2);
				nDataPos += 2;
				BcdToAsc(szTmpAsc,(unsigned char*)szTmp,4);
				iTmpLen = atoi(szTmpAsc);
				memset(szTmp,0x00,sizeof(szTmp));
				memcpy(szTmp,pszSrcData + nDataPos,(iTmpLen+1)/2);
				nDataPos += (iTmpLen+1)/2;
				LOGINFO("Field36 :[len:%d] %s", iTmpLen, szTmp);
				break;
			case 37:
				memset(szTmp,0x00,sizeof(szTmp));
				memcpy(szTmp,pszSrcData + nDataPos,12);
				nDataPos += 12;
				SetDataResult(FALSE,"SYSREFNUM", szTmp);
				LOGINFO("SysRefnum: %s", szTmp);
				break;
			case 38:
				memset(szTmp,0x00,sizeof(szTmp));
				memcpy(szTmp,pszSrcData + nDataPos,6);
				nDataPos += 6;
				SetDataResult(FALSE, "AUTHCODE", szTmp);
				LOGINFO("Authcode: %s", szTmp);
				break;
			case 39:
				memset(szTmp,0x00,sizeof(szTmp));
				memcpy(szTmp,pszSrcData + nDataPos,2);
				nDataPos += 2;
				SetDataResult(FALSE, "RESPONSECODE", szTmp);
				LOGINFO("Response code: %s", szTmp);
				break;
			
			case 41: //ATM编号
				memset(szTmp,0x00,sizeof(szTmp));
				memcpy(szTmp,pszSrcData + nDataPos,8);
				nDataPos += 8;
				LOGINFO("Atm number: %s", szTmp);
				break;
			/*
			case 42:
				break;
			case 43:
				break;
			case 44:
				break;
			case 45:
				break;
			case 46:
				break;
			case 47:
				break;
			*/
			case 48:
				memset(szTmpAsc,0x00,sizeof(szTmpAsc));
				memset(szTmp,0x00,sizeof(szTmp));
				memcpy(szTmp,pszSrcData + nDataPos, 2);//可变长度LLL为2个字节BCD码
				nDataPos += 2;
				BcdToAsc(szTmpAsc,(unsigned char*)szTmp,4);
				iTmpLen = atoi(szTmpAsc);
				memset(szTmp,0x00,sizeof(szTmp));
				memcpy(szTmp,pszSrcData + nDataPos,iTmpLen);
				nDataPos += iTmpLen;
				LOGINFO("field48 :[len:%d] %s", iTmpLen, szTmp);
				
				if(strcmp(pszDealType,"QRYCUSTNAME") == 0)
				{
					char szCustname[64]={0};
					memset(szCustname, 0x00, sizeof(szCustname));
					if(iTmpLen>40)
						memcpy(szCustname, szTmp, 40); 
					else
						sprintf(szCustname, szTmp);
					memset(szTmpAsc,0x00,sizeof(szTmpAsc));
					AscToBcd((unsigned char*)szTmpAsc,szCustname, strlen(szCustname));
					SetDataResult(FALSE, "CUSTOMERNAME", szTmpAsc);
				}
				if(strcmp(pszDealType,"QRYBUSSINESSACCOUNT") == 0)
				{
					char szCustname[256]={0};
					memset(szCustname, 0x00, sizeof(szCustname));
					if(iTmpLen>40)
						memcpy(szCustname, szTmp, 100); 
					else
						sprintf(szCustname, szTmp);
					char* szTmp2 = szTmp;
					int nameLength = 0;
					while(*szTmp2 != 0x20)
					{
						szTmp2++;
						nameLength++;
					}
					szCustname[nameLength] = NULL;
					SetDataResult(FALSE, "CUSTOMERNAME", szCustname);
				}
				else if(strcmp(pszDealType,"QRYHOSTAMOUNT") == 0) //查询主机现金箱余额
				{
					char sztmpHostAmount[64]={0};
					memset(sztmpHostAmount, 0x00, sizeof(sztmpHostAmount));		
					char szHostAmount[64]={0};
					memset(szHostAmount, 0x00, sizeof(szHostAmount));
					memcpy(sztmpHostAmount, szTmp+5, iTmpLen-5); //如数据: "01156          192740.00"
					int tmpHostAmountLen = iTmpLen-5;
					char* szTmp2 = sztmpHostAmount;
					int SpaceLength = 0;
					while(*szTmp2 == 0x20)
					{
						szTmp2++;
						SpaceLength++;
					}
					memcpy(szHostAmount, sztmpHostAmount+SpaceLength, tmpHostAmountLen-SpaceLength); //如数据: "01156          192740.00"
					LOGINFO("HOSTAMOUNT: %s,", szHostAmount);
					SetDataResult(FALSE, "HOSTAMOUNT", szHostAmount);
				}
				else if(strcmp(pszDealType,"INQ") == 0) //获取卡类型 add by tsx 
				{					
					SetDataResult(FALSE, "RESPONSECARDTYPE", szTmp);
					LOGINFO("RESPONSECARDTYPE code: %s", szTmp);
				}
				else if(strcmp(pszDealType,"QRYCWDMONEY") == 0) //获取取款已取金额 add by tsx 
				{				
					char szCWDMoneyAmount[64]={0};
					memset(szCWDMoneyAmount, 0x00, sizeof(szCWDMoneyAmount));
					sprintf(szCWDMoneyAmount, szTmp);
					char* szTmp2 = szCWDMoneyAmount;
					int nameLength = 0;
					while(*szTmp2 != 0x20)
					{
						szTmp2++;
						nameLength++;
					}
					szCWDMoneyAmount[nameLength] = NULL;
					SetDataResult(FALSE, "QRYCWDMONEY", szCWDMoneyAmount);
					LOGINFO("QRYCWDMONEY code: %s", szCWDMoneyAmount);
				}
				else if(strcmp(pszDealType,"INQC") == 0) //2017.9.25.1 销户查询账号
				{
					LOGINFO("pszDealType(INQC): %s", pszDealType);
					char szCardNo[256]={0};
					memset(szCardNo, 0x00, sizeof(szCardNo));
					if(iTmpLen>40)
						memcpy(szCardNo, szTmp, 100); 
					else
						sprintf(szCardNo, szTmp);
					/*char* szCardNoTmp = szTmp;
					int szCardNoLength = 0;
					while(*szCardNoTmp != 0x20)
					{
						szCardNoTmp++;
						szCardNoLength++;
					}
					szCardNo[szCardNoLength] = NULL;*/
					LOGINFO("ACCOUNTLIST: %s", szCardNo);
					SetDataResult(FALSE, "ACCOUNTLIST", szCardNo);
				}
				else if(strcmp(pszDealType,"ACCDELETE") == 0) //2017.9.25.1 销户结果
				{				
					char szAccDleleteRes[256]={0};
					memset(szAccDleleteRes, 0x00, sizeof(szAccDleleteRes));
					if(iTmpLen>40)
						memcpy(szAccDleleteRes, szTmp, 100); 
					else
						sprintf(szAccDleleteRes, szTmp);
					char* szAccDleleteTmp = szTmp;
					int szAccDleleteLength = 0;
					while(*szAccDleleteTmp != 0x20)
					{
						szAccDleleteTmp++;
						szAccDleleteLength++;
					}
					szAccDleleteRes[szAccDleleteLength] = NULL;
					SetDataResult(FALSE, "ACCDELETERES", szAccDleleteRes);
				}
				else if (strcmp(pszDealType,"QRYACCOUNTTYPE") == 0)
				{
					SetDataResult(FALSE, "QRYACCOUNTTYPE", szTmp);
					LOGINFO("QRYACCOUNTTYPE code: %s", szTmp);
				}
				break;
			case 49: //货币代码
				memset(szTmp,0x00,sizeof(szTmp));
				memcpy(szTmp,pszSrcData + nDataPos,3);
				nDataPos += 3;
				LOGINFO("MoneyType : %s", szTmp);
				break;
			/*
			case 50:
				break;
			case 51:
				break;
			*/
			case 52:
				nDataPos += 8;
				break;
			/*
			case 53:
				break;
			*/
			case 54: //附加金额
				memset(szTmpAsc,0x00,sizeof(szTmpAsc));
				memset(szTmp,0x00,sizeof(szTmp));
				memcpy(szTmp,pszSrcData + nDataPos,2);
				nDataPos += 2;
				BcdToAsc(szTmpAsc,(unsigned char*)szTmp,4);
				iTmpLen = atoi(szTmpAsc);
				memset(szTmp,0x00,sizeof(szTmp));
				memcpy(szTmp,pszSrcData + nDataPos,iTmpLen);
				nDataPos += iTmpLen;

				LOGINFO("Field54 :[len:%d] %s", iTmpLen, szTmp);

				if(strcmp(pszDealType,"INQ") == 0 && (29==iTmpLen))
				{ //156C000000123456C000000123456
					char szMoneyType[4];//币种
					memset(szMoneyType, 0x00, sizeof(szMoneyType));
					char szCurrentBal_Sign[2]={0}; //正负号
					char szAvailableBal_Sign[2]={0}; 
					char szCurrentBlance[16];
					char szAvailableBalance[16];
					memset(szCurrentBlance, 0x00, sizeof(szCurrentBlance));
					memset(szAvailableBalance, 0x00, sizeof(szAvailableBalance));

					int nPosTmp=0;
					memcpy(szMoneyType, szTmp+nPosTmp, 3);
					nPosTmp+=3;
					if(szTmp[nPosTmp]=='D')
						sprintf(szCurrentBal_Sign,"-");
					nPosTmp+=1;
					memcpy(szCurrentBlance, szTmp+nPosTmp, 12);
					nPosTmp+=12;
					if(szTmp[nPosTmp]=='D')
						sprintf(szAvailableBal_Sign,"-");
					nPosTmp+=1;
					memcpy(szAvailableBalance, szTmp+nPosTmp, 12);

					int nCurrentBalance=atoi(szCurrentBlance);
					int nAvailableBalance=atoi(szAvailableBalance);
					sprintf(szCurrentBlance, "%s%d", szCurrentBal_Sign, nCurrentBalance);
					sprintf(szAvailableBalance, "%s%d", szAvailableBal_Sign, nAvailableBalance);

					SetDataResult(FALSE,"CURRENTBALANCE", szCurrentBlance);
					SetDataResult(FALSE,"AVAILABLEBALANCE", szAvailableBalance);
				}
				break;
			case 55: //IC卡数据域
				memset(szTmpAsc,0x00,sizeof(szTmpAsc));
				memset(szTmp,0x00,sizeof(szTmp));
				memcpy(szTmp,pszSrcData + nDataPos,2);
				nDataPos += 2;
				BcdToAsc(szTmpAsc,(unsigned char*)szTmp, 4);
				iTmpLen = atoi(szTmpAsc);
				memset(szTmp,0x00,sizeof(szTmp));
				memcpy(szTmp,pszSrcData + nDataPos,iTmpLen);
				nDataPos += iTmpLen;
				LOGINFO("Field55 :[len:%d] %s", iTmpLen, szTmp);
				break;
			/*
			case 56:
				break;
			case 57:
				break;
			case 58:
				break;
			case 59:
				break;
			case 60:
				break;
			*/
			case 60: //
				memset(szTmpAsc,0x00,sizeof(szTmpAsc));
				memset(szTmp,0x00,sizeof(szTmp));
				memcpy(szTmp,pszSrcData + nDataPos,2);
				nDataPos += 2;
				BcdToAsc(szTmpAsc,(unsigned char*)szTmp,4);
				iTmpLen = atoi(szTmpAsc);
				memset(szTmp,0x00,sizeof(szTmp));
				memcpy(szTmp,pszSrcData + nDataPos,iTmpLen);
				nDataPos += iTmpLen;
				LOGINFO("Field60 :[len:%d] %s", iTmpLen, szTmp);
				break;
			case 61:
				memset(szTmpAsc,0x00,sizeof(szTmpAsc));
				memset(szTmp,0x00,sizeof(szTmp));
				memcpy(szTmp,pszSrcData + nDataPos,2);
				nDataPos += 2;
				BcdToAsc(szTmpAsc,(unsigned char*)szTmp,4);
				iTmpLen = atoi(szTmpAsc);
				memset(szTmp,0x00,sizeof(szTmp));
				memcpy(szTmp,pszSrcData + nDataPos,iTmpLen);
				nDataPos += iTmpLen;
				LOGINFO("Field61 :[len:%d] %s", iTmpLen, szTmp);
				break;
			case 62: //工作参数
				memset(szTmpAsc,0x00,sizeof(szTmpAsc));
				memset(szTmp,0x00,sizeof(szTmp));
				memcpy(szTmp,pszSrcData + nDataPos,2);
				nDataPos += 2;
				BcdToAsc(szTmpAsc,(unsigned char*)szTmp,4);
				iTmpLen = atoi(szTmpAsc);
				memset(szTmp,0x00,sizeof(szTmp));
				memcpy(szTmp,pszSrcData + nDataPos,iTmpLen);
				nDataPos += iTmpLen;
				LOGINFO("Field62 :[len:%d] %s", iTmpLen, szTmp);
				break;
			case 63: //工作密钥
				memset(szTmpAsc,0x00,sizeof(szTmpAsc));
				memset(szTmp,0x00,sizeof(szTmp));
				memcpy(szTmp,pszSrcData + nDataPos,2);
				nDataPos += 2;
				BcdToAsc(szTmpAsc,(unsigned char*)szTmp,4);
				iTmpLen = atoi(szTmpAsc);
				memset(szTmp,0x00,sizeof(szTmp));
				memcpy(szTmp,pszSrcData + nDataPos,iTmpLen);
				nDataPos += iTmpLen;
				LOGINFO("Field63 :[len:%d] %s", iTmpLen, szTmp);

				if(96==iTmpLen)
				{
					char szPinKey[32+1], szMacKey[16+1];
					memset(szPinKey, 0x00, sizeof(szPinKey));
					memset(szMacKey, 0x00, sizeof(szMacKey));

					memcpy(szPinKey, szTmp, 32);
					SetDataResult(FALSE, "PINKEY", szPinKey);
					memcpy(szMacKey, szTmp+48, 16);
					SetDataResult(FALSE, "MACKEY", szMacKey);
				}
				break;
			case 64:
				memset(szTmp,0x00,sizeof(szTmp));
				memcpy(szTmp,pszSrcData + nDataPos,8);
				nDataPos += 8;
				LOGINFO("Field64 :[len:%d] %s", iTmpLen, szTmp);
				break;
			default:
				LOGINFO("Unknown field[%d]", (i+1));
				break;
			}
		}
	}
	if((nDataPos-2) != nDataLen)
	{
		LOGERROR("Parse error");
		return -9;
	}
    LOGINFO("CDRCV::ParseData End");
	
	//解包结束
	


	return 0;
}

void CDRCV::BcdToAsc(char *AscData,unsigned char *BcdData,const int AcsnDataPos)
{
	if(NULL == BcdData||AscData== NULL)
		return ;
	int	i = 0, index = 0,iMoveFlag = 0;
	char tmp = 0x00;
	AscData[AcsnDataPos] = 0x00;
	for(i = 0; i < AcsnDataPos; i++) 
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


void CDRCV::AscToBcd(unsigned char *BcdData,char *AscData,const int AscDataLen)
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

int CDRCV::IsValidTransType(const char *pszDealType, const char *pszMsgType, const char *pszProcessCode)
{
	if(strcmp(pszDealType,"APPLYWORKPARAM") == 0)
	{
		if((strcmp(pszProcessCode,"920000") != 0) || (strcmp(pszMsgType,"0810") != 0) )
			return -1;
	}
	if(strcmp(pszDealType,"INQ") == 0)
	{
		if((strcmp(pszProcessCode,"310000") != 0) || (strcmp(pszMsgType,"0210") != 0) )
			return -1;
	}
	if(strcmp(pszDealType,"QRYCUSTNAME") == 0)
	{
		//if((strcmp(pszProcessCode,"380000") != 0) || (strcmp(pszMsgType,"0210") != 0) ) 
		if((strcmp(pszProcessCode,"330000") != 0) || (strcmp(pszMsgType,"0210") != 0) ) //查户名改成 账户验证
			return -1;
	}
	if(strcmp(pszDealType,"CWD") == 0)
	{
		if( ((strcmp(pszProcessCode,"600000") != 0) && (strcmp(pszProcessCode,"010000") != 0)) || (strcmp(pszMsgType,"0210") != 0)) //modify by tsx for ATM取款
			return -1;
	}
	if(strcmp(pszDealType,"DEP") == 0)
	{
		if( ((strcmp(pszProcessCode,"620000") != 0) && (strcmp(pszProcessCode,"210000") != 0)) || ((strcmp(pszMsgType,"0230") != 0) && (strcmp(pszMsgType,"0210") != 0)) ) //modify by tsx for ATM存款
			return -1;
	}
	if(strcmp(pszDealType,"CWC") == 0)
	{
		if((strcmp(pszProcessCode,"600000") != 0 && strcmp(pszProcessCode,"640000") != 0) || (strcmp(pszMsgType,"0430") != 0) )
			return -1;
	}
	if(strcmp(pszDealType,"NOCARDDEP") == 0)
	{
		//if((strcmp(pszProcessCode,"620000") != 0) || (strcmp(pszMsgType,"0210") != 0) )
		if((strcmp(pszProcessCode,"210000") != 0) || (strcmp(pszMsgType,"0230") != 0) )//tmp 广东农行无卡临时用小额
			return -1;
	}
	if(strcmp(pszDealType,"QRYBUSSINESSACCOUNT") == 0)
	{
		if((strcmp(pszProcessCode,"630000") != 0) || (strcmp(pszMsgType,"0210") != 0) )
			return -1;
	}
	if(strcmp(pszDealType,"BUSSINESSDEP") == 0)
	{
		if((strcmp(pszProcessCode,"620000") != 0) || (strcmp(pszMsgType,"0210") != 0) )
			return -1;
	}
	if(strcmp(pszDealType,"QRYCWDMONEY") == 0)
	{
		if((strcmp(pszProcessCode,"640000") != 0) || (strcmp(pszMsgType,"0210") != 0) )
			return -1;
	}

	return 0;
}

QObject *CreateInstance()
{
    return CDRCV::GetInstance();
}
