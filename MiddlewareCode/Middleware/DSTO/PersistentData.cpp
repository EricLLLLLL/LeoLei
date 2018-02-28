///////////////////////////
//		Create by art
//////////////////////////

#include "PersistentData.h"
#include "ScratchPad.h" 
#include "QThread"
#include "DBManager/DBManager.h"
#include "common_def.h"
#include "QSqlQuery"
#include "QVariant"
#include "Helper/helper.h"
#include "QFile"
#include <thread>

#define  FSN_PER_PAGE  10

//CSNFile CPeristentData::m_NoteTraceFile;


CPeristentData::CPeristentData(void)
{
	m_bDataOpened=FALSE;
}

CPeristentData::~CPeristentData(void)
{
	LOGINFO("CPeristentData::~CPeristentData_ByWill");
	CloseDatabase();
}


BOOL  CPeristentData::OpenDatabase()
{
	if(m_bDataOpened)
	{
		return TRUE;
	}

    /*
	char szDB[MAX_PATH]={0};
	GetDatabase(szDB);
	m_bDataOpened=m_Sqlite.Open(szDB);
	if(!m_bDataOpened)
		LOGINFO("CPeristentData::OpenDatabase %s: False", szDB);

	return m_bDataOpened;
    */
    return m_bDataOpened;
}

void  CPeristentData::CloseDatabase()
{
    //m_Sqlite.Close();
}

short  CPeristentData::AddData(char *pszDataName, char *pszDataType, char *pszInitValue, char *pszNeedInit, char *pszNeedClear)
{
	//add by art begin 
	if(strcmp(pszDataName, "~ADDSNR~")==0)
	{ //冠字号信息加入数据库
        thread th1 (AddSNRThreadFunc, this);
        th1.detach();
		return 0;
	}
	else if(strcmp(pszDataName, "~QRYSNR~")==0)
	{//冠字号信息查询
		//LOGINFO("CPeristentData::AddData: [DataName]=%s [DataType]=%s [DataValue]=%s", pszDataName, pszDataType, pszInitValue);
		return QrySNRFunc(pszDataType, pszInitValue);
	}	
	//add by art end 

	LOGINFO("CPeristentData::AddData: [DataName]=%s [DataType]=%s [DataValue]=%s", pszDataName, pszDataType, pszInitValue);

	if(strlen(pszDataName)==0 || strlen(pszDataType)==0)
	{
		LOGINFO("DataName or DataType is blank");
		return -1;
	}

    CDBManager* pDBMgr = CDBManager::GetInstance();
    if(!pDBMgr->IsDBConnected())
    {
        return -2;
    }

    QString szSql = "insert into PersistentData (DataName, DataType, InitValue, CurrentValue, NeedInit, NeedClear) values (?, ?, ?, ?, ?, ?)";
    QVariantList stValList;
    stValList << S2Q(pszDataName) << S2Q(pszDataType) << S2Q(pszInitValue) << S2Q(pszInitValue) << S2Q(pszNeedInit) << S2Q(pszNeedClear);

    QSqlQuery stResultQuery;
    bool bSuc = pDBMgr->DBSqlQuery(szSql,stValList,stResultQuery);
    if(!bSuc)
    {
        LOGINFO("ExcuteNonQuery Result: Error.  err:%s",Q2S(pDBMgr->GetLastError()));
        return -3;
    }

	LOGINFO("ExcuteNonQuery Result: OK");

	return 0;
}

short  CPeristentData::SetData(char *pszDataName, char *pszDataType, char *pszNewValue)
{
	LOGINFO("CPeristentData::SetData: [DataName]=%s [DataType]=%s [DataValue]=%s", pszDataName, pszDataType, pszNewValue);
	if(strlen(pszDataName)==0 || strlen(pszDataType)==0)
	{
		LOGINFO("DataName or DataType is blank");
		return -1;
	}

	if(SetDataToInifile(pszDataName, pszNewValue))  //Set to Inifile
		return 0;

    CDBManager* pDBMgr = CDBManager::GetInstance();
    if(!pDBMgr->IsDBConnected())
    {
        return -2;
    }

    QString szSql = "update PersistentData set CurrentValue=? where DataName=? and DataType=?";
    QVariantList stValList;
    stValList << S2Q(pszNewValue) << S2Q(pszDataName) << S2Q(pszDataType);

    QSqlQuery stResultQuery;
    bool bSuc = pDBMgr->DBSqlQuery(szSql,stValList,stResultQuery);
    if(!bSuc)
    {
        LOGINFO("ExcuteNonQuery Result: Error.  err:%s",Q2S(pDBMgr->GetLastError()));
        return -3;
    }

	LOGINFO("ExcuteNonQuery Result: OK");

	return 0;
}

short  CPeristentData::GetData(char *pszDataName, char *pszDataType, char *pszValue)
{
	LOGINFO("CPeristentData::GetData: [DataName]=%s [DataType]=%s", pszDataName, pszDataType);

	if(strlen(pszDataName)==0 || strlen(pszDataType)==0)
	{
		LOGINFO("DataName or DataType is blank");
		return -1;
	}


	short nRet=0;

	if(GetDataFromInifile(pszDataName, pszValue))  //Get from Inifile
		return 0;

	if(strcmp(pszDataName,"BILLNORPTFILES") == 0)//add by art for 冠字号凭条打印
	{
		return GetBillNoRptFiles(pszValue);
	}

    CDBManager* pDBMgr = CDBManager::GetInstance();
    if(!pDBMgr->IsDBConnected())
    {
        sprintf(pszValue,"");
        return -2;
    }

    QString szSql = "select CurrentValue from PersistentData where DataName=? and DataType=?";
    QVariantList stValList;
    stValList << S2Q(pszDataName) << S2Q(pszDataType);

    QSqlQuery stResultQuery;
    bool bSuc = pDBMgr->DBSqlQuery(szSql,stValList,stResultQuery);
    if(!bSuc)
    {
        LOGINFO("ExcuteQuery Result: NONE");
        sprintf(pszValue, "");
        return -3;
    }

    if (stResultQuery.next())
    {
        QString strData = stResultQuery.value(0).toString();
        strcpy(pszValue,Q2S(strData));
    }

	LOGINFO("ExcuteQuery Result : [DataValue]=%s", pszValue);

	return nRet;
}

BOOL  CPeristentData::GetDataFromInifile(char *pszDataName, char *pszValue)
{ //部分数据项使用ini文件
	BOOL bRet=FALSE;
    QString strVal = "";
    QString strCfgPath = QCoreApplication::applicationDirPath() + CONST_SETUPINIFILE;
	if(strcmp(pszDataName,"IDCHECKURL") == 0) //联网核查服务网址
	{
        strVal = g_Helper.GetPrivateProfileString("IDCHECK","IDCHECKURL","http://10.232.53.166:9080/CIIS/services/IdchkService",strCfgPath);
		bRet=TRUE;
	}
	else if(strcmp(pszDataName,"HOSTIP") == 0)	//P端主机IP地址
	{
        strVal = g_Helper.GetPrivateProfileString("MTCP","HOSTIP","127.0.0.1", strCfgPath);
		bRet=TRUE;
	}
	else if(strcmp(pszDataName,"HOSTPORT") == 0) //P端主机端口
	{
        strVal = g_Helper.GetPrivateProfileString("MTCP","HOSTPORT","3014", strCfgPath);
		bRet=TRUE;
	}
	else if(strcmp(pszDataName,"LARGEAMOUNT_MIN") == 0) //大额交易标准(即大额最小金额，单位元)
	{
        strVal = g_Helper.GetPrivateProfileString("DEAL","LARGEAMOUNT_MIN","50000",strCfgPath);
		bRet=TRUE;
	}
	else if(strcmp(pszDataName,"NOCARD_LARGEAMOUNT_MIN") == 0) //无卡大额交易标准(即大额最小金额，单位元)
	{
        strVal = g_Helper.GetPrivateProfileString("DEAL","NOCARD_LARGEAMOUNT_MIN","10000",strCfgPath);
		bRet=TRUE;
	}
    strcpy(pszValue,Q2S(strVal));

	return bRet;
}

BOOL CPeristentData::SetDataToInifile(char *pszDataName, char *pszNewValue)
{ //部分数据项使用ini文件
	BOOL bRet=FALSE;
    QString strCfgPath = QCoreApplication::applicationDirPath() + CONST_SETUPINIFILE;
	if(strcmp(pszDataName,"HOSTIP") == 0) 	//P端主机IP地址
	{
        g_Helper.WritePrivateProfileString("MTCP","HOSTIP", S2Q(pszNewValue), strCfgPath);
		bRet=TRUE;
	}
	else if(strcmp(pszDataName,"HOSTPORT") == 0) //P端主机端口
	{
        g_Helper.WritePrivateProfileString("MTCP","HOSTPORT", S2Q(pszNewValue), strCfgPath);
		bRet=TRUE;
	}
	else if(strcmp(pszDataName,"LARGEAMOUNT_MIN") == 0) //大额交易标准(即大额最小金额，单位元)
	{
        g_Helper.WritePrivateProfileString("DEAL","LARGEAMOUNT_MIN", S2Q(pszNewValue), strCfgPath);
		bRet=TRUE;
	}
	else if(strcmp(pszDataName,"NOCARD_LARGEAMOUNT_MIN") == 0) //无卡大额交易标准(即大额最小金额，单位元)
	{
        g_Helper.WritePrivateProfileString("DEAL","NOCARD_LARGEAMOUNT_MIN", S2Q(pszNewValue), strCfgPath);
		bRet=TRUE;
	}

	return bRet;
}


DWORD CPeristentData::AddSNRThreadFunc(LPVOID lpPara)
{
	CPeristentData *pPstData=(CPeristentData*)lpPara;
	LOGINFO("CPeristentData::AddSNRThreadFunc begin");

	static char szSNRFile[MAX_PATH]={0}; //SP保存冠字号信息的文本文件
	if(strlen(szSNRFile)==0)
    {
        QString strFilePath = g_Helper.GetPrivateProfileString("PhysicalConfig","SerialTextFile",CONST_INSTALL_DIR+"SNRInfo.ini", CONST_INSTALL_DIR+"/HMKiosk/Config/ini/CRM_UR.ini");
        strcpy(szSNRFile,Q2S(strFilePath));
    }

    if(!QFile::exists(S2Q(szSNRFile)))
	{
		LOGERROR("%s does not exist", szSNRFile);
		return -1;
	}

    CDBManager* pDBMgr = CDBManager::GetInstance();
    if (!pDBMgr->IsDBConnected())
	{
		return -2;
	}

	static int s_nFileIndex=0;
	s_nFileIndex++;
	char szTmpFile[MAX_PATH];
    QString strIniDir = CONST_DATA_DIR + "/SNR/";
    g_Helper.CreateDirPath(strIniDir);
    QString strIniFile = strIniDir+"TmpSNR%d.ini";
    sprintf(szTmpFile, Q2S(strIniFile), s_nFileIndex);

    if(!QFile::copy(S2Q(szSNRFile), S2Q(szTmpFile)))
	{
        LOGINFO("Copy to file %s error", szTmpFile);
		return -4;
	}

	char szJnlNum[16]={0};
	pPstData->GetData("JNLNUM", "LONG", szJnlNum);

	char szMachineNum[16]={0};
	pPstData->GetData("TERMINALNUM", "STRING", szMachineNum);
	char szAccountNo[32]={0};
	char szDealType[32]={0};

	//tmp by art begin
	string sKey;
	sKey="CARDNO";
	sKey+="~^";
	sKey+="STRING";
	SCRATCHMAP::iterator iter; 
	iter = CScratchPad::m_ScratchPad.find(sKey);
	if(iter!=CScratchPad::m_ScratchPad.end())
	{
		sprintf(szAccountNo, (iter->second).c_str());
	}
	sKey="DEALTYPE";
	sKey+="~^";
	sKey+="STRING";
	iter = CScratchPad::m_ScratchPad.find(sKey);
	if(iter!=CScratchPad::m_ScratchPad.end())
	{
		if(strcmp((iter->second).c_str(), "DEP存款")==0)
			sprintf(szDealType, "DEP");
		else if(strcmp((iter->second).c_str(), "CWD取款")==0) 
			sprintf(szDealType, "CWD");
		else if(strcmp((iter->second).c_str(), "存折取款")==0) 
			sprintf(szDealType, "PCWD");
		else if(strcmp((iter->second).c_str(), "零钞兑换")==0)
			sprintf(szDealType, "EXCASH");
		else if(strcmp((iter->second).c_str(), "无卡无折存款")==0)
			sprintf(szDealType, "PDEP");
		else if(strcmp((iter->second).c_str(), "对公存款")==0)
			sprintf(szDealType, "BDEP");
	}
	//tmp by art end

	char szOperationTime[32]={0};
    QString strOpTime = g_Helper.GetPrivateProfileString("Cash_Info","OperationTime","", S2Q(szTmpFile));
    strncpy(szOperationTime,Q2S(strOpTime),strOpTime.length());
    int nOperationType=g_Helper.GetPrivateProfileInt("Cash_Info", "OperationType",0 ,S2Q(szTmpFile));

    QString strSql;
	int nLevel=0;
	int nCountIndex=0;
	char szLevelName[32]={0}, szValue[8]={0}, szNoteVersion[8]={0}, szSerialNumber[16]={0}, szCurrency[8]={0};
	char szImageFile[MAX_PATH]={0}; //, szWholeNoteImageFile1[MAX_PATH]={0}, szWholeNoteImageFile2[MAX_PATH]={0};
	char szLevelCountName[16]={0};
	int nLevelCount=0;
	char szWriteLine[222] = {0};
    QString strWriteIntoFile = "";
    //pPstData->m_NoteTraceFile.SetCharInfo(INFOCHOICE_MachineType,"HM8766S");
    //pPstData->m_NoteTraceFile.SetCharInfo(INFOCHOICE_MachineNum,szMachineNum);

    //pPstData->m_Sqlite.BeginTransaction();
    pDBMgr->DBSqlQueryTransactBegin();
	for(int i=4; i>=2; i--)
	{
		nLevel=i;
		sprintf(szLevelCountName, "LEVEL%d_COUNT", i);
        nLevelCount=g_Helper.GetPrivateProfileInt("Cash_Info", szLevelCountName,0 ,szTmpFile);

		for(int j=0; j<nLevelCount; j++)
		{
			if (j < 1000)
			{
				sprintf(szLevelName, "LEVEL%d_%03d", i, j+1);
			}
			else
			{
				sprintf(szLevelName, "LEVEL%d_%d", i, j+1);
			}

            nCountIndex=g_Helper.GetPrivateProfileInt(szLevelName, "Index",0 ,szTmpFile);
            QString strValue = g_Helper.GetPrivateProfileString(szLevelName,"Value","", szTmpFile);
            strncpy(szValue,Q2S(strValue),strValue.size());
            QString strNoteVersion = g_Helper.GetPrivateProfileString(szLevelName,"NoteVersion","", szTmpFile);
            strncpy(szNoteVersion,Q2S(strNoteVersion),strNoteVersion.size());
            QString strCurrency = g_Helper.GetPrivateProfileString(szLevelName,"Currency","", szTmpFile);
            strncpy(szCurrency,Q2S(strCurrency),strCurrency.size());
            QString strSerialNumber = g_Helper.GetPrivateProfileString(szLevelName,"SerialNumber","", szTmpFile);
            strncpy(szSerialNumber,Q2S(strSerialNumber),strSerialNumber.size());
            QString strImageFile = g_Helper.GetPrivateProfileString(szLevelName,"ImageFile","", szTmpFile);
            strncpy(szImageFile,Q2S(strImageFile),strImageFile.size());
            //GetPrivateProfileString(szLevelName,"WholeNoteImageFile1","",szWholeNoteImageFile1, sizeof(szWholeNoteImageFile1), szTmpFile);
			//GetPrivateProfileString(szLevelName,"WholeNoteImageFile2","",szWholeNoteImageFile2, sizeof(szWholeNoteImageFile2), szTmpFile);

            strSql = "insert into SNRInfo (CountIndex, OperationTime, Currency, Value, Level, NoteVersion, SerialNumber, ImageFile, OperationType, JnlNum, AccountNo, DealType) \
                             values (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
            QVariantList stValList;
            stValList << nCountIndex << S2Q(szOperationTime) << S2Q(szCurrency) << S2Q(szValue) << nLevel << S2Q(szNoteVersion)
                      << S2Q(szSerialNumber) << S2Q(szImageFile) << nOperationType << S2Q(szJnlNum) << S2Q(szAccountNo) << S2Q(szDealType);
            QSqlQuery stResultQuery;
            bool bSuc = pDBMgr->DBSqlQuery(strSql,stValList,stResultQuery);
            if(!bSuc)
            {
                LOGINFO("ExcuteNonQuery Result: Error---[%s]  err:%s",Q2S(strSql),Q2S(pDBMgr->GetLastError()));
                return -3;
            }

			//2016-09-21 15:26:50
            QString strDateTime = szOperationTime;
            strDateTime.replace("-","");
            strDateTime.replace(":","");
            strDateTime.replace(" ","|!");
			//取得纸币成色
			char	szQualityCode[3];
            QString strQualityCode = g_Helper.GetPrivateProfileString(szLevelName,"Quality","90", szTmpFile);
            strncpy(szQualityCode,Q2S(strQualityCode),strQualityCode.size());
            int nTradeType = 0;
			//业务类型	1-取款，2-存款，3-自助设备加钞，默认为0
			if(strcmp(szDealType, "DEP")==0)
				nTradeType=2;
			else if(strcmp(szDealType, "CWD")==0)
				nTradeType=1;
			else
				nTradeType=0;
            sprintf(szWriteLine, "%s|!%s|!%s|!%s|!%s|!%s|!%s|!%s%s|!%s|!%06d|!%06d|!%d|!%s|!%s|!%s",
                             Q2S(strDateTime), szCurrency, szNoteVersion, szValue, szQualityCode, szSerialNumber,"2","HM--HM8766S---", szMachineNum,"0", atoi(szJnlNum), nCountIndex+1, nTradeType,"0", szAccountNo, szImageFile);
			strWriteIntoFile += szWriteLine;
			strWriteIntoFile += "\x0A";
		}
	}
    //pPstData->m_NoteTraceFile.MakeSNFile(strWriteIntoFile);
    //pPstData->m_Sqlite.CommitTransaction();
    pDBMgr->DBSqlQueryTransactCommit();
	LOGINFO("CPeristentData::AddSNRThreadFunc end");

	return 0;
}

/*
std::string to_utf8(const wchar_t* buffer,int len)
{
	int nChars =::WideCharToMultiByte(
		CP_UTF8,
		0,
		buffer,
		len,
		NULL,
		0,
		NULL,
		NULL);
	if(nChars ==0)return"";

	string newbuffer;
	newbuffer.resize(nChars);
	::WideCharToMultiByte(
		CP_UTF8,
		0,
		buffer,
		len,
		const_cast<char*>(newbuffer.c_str()),
		nChars,
		NULL,
		NULL); 

	return newbuffer;
}*/

short CPeristentData::QrySNRFunc(char *pszFileName, char *pszParam)
{
	LOGINFO("CPeristentData::QrySNRFunc begin");
	LOGINFO("QrySNRFunc pszParam=%s, pszFileName=%s", pszParam, pszFileName);
    //if (!OpenDatabase())
    //{
    //	return -2;
    //}

    CDBManager* pDBMgr = CDBManager::GetInstance();
    if(!pDBMgr->IsDBConnected())
    {
        LOGINFO("CPeristentData::QrySNRFunc: db is disconnected.  err:%s",Q2S(pDBMgr->GetLastError()));
        return -2;
    }


	char *p = NULL;
	char szDelim[] = "~";

	p = strtok(pszParam, szDelim);
	char szStartTime[32];
	sprintf(szStartTime, p);
	p = strtok(NULL, szDelim);
	char szEndTime[32];
	sprintf(szEndTime, p);
	p = strtok(NULL, szDelim);
	char szSno[32];
	sprintf(szSno, p);
	p = strtok(NULL, szDelim);
	char szJnlNum[16];
	sprintf(szJnlNum, p);
	p = strtok(NULL, szDelim);
	char szAccountNo[32];
	sprintf(szAccountNo, p);


	if(strlen(szStartTime)==0 || strcmp(szStartTime,"*")==0)
		sprintf(szStartTime, " ");

	if(strlen(szEndTime)==0 || strcmp(szEndTime,"*")==0)
		sprintf(szEndTime, "9999-99-99 99:99:99");

	char szSnoCondition[64]={0}; 
	if(strlen(szSno)==0 || strcmp(szSno,"*")==0 || strcmp(szSno,"__________")==0)
	{
		sprintf(szSnoCondition, "");
	}
	else
	{
		sprintf(szSnoCondition, " and (SerialNumber like \'%s\') ", szSno);
	}

	char szJnlNumCondition[64]={0}; 
	if(strlen(szJnlNum)==0 || strcmp(szJnlNum,"*")==0)
	{
		sprintf(szJnlNumCondition, "");
	}
	else
	{
		sprintf(szJnlNumCondition, " and (JnlNum=%d) ", atoi(szJnlNum));
	}

	char szAccountNoCondition[64]={0}; 
	if(strlen(szAccountNo)==0 || strcmp(szAccountNo,"*")==0)
	{
		sprintf(szAccountNoCondition, "");
	}
	else
	{
        sprintf(szAccountNoCondition, " and (AccountNo=\'%s\') ", szAccountNo);
	}

	char szRecord[MAX_PATH+512];
    QString strSql = "select OperationTime, CountIndex, Currency, Value, Level, NoteVersion, SerialNumber, ImageFile, OperationType, JnlNum, AccountNo, DealType from SNRInfo \
                    where OperationTime>=? and OperationTime<=? ? ? ? \
                    order by OperationTime desc, CountIndex desc";
    LOGINFO("szSql=%s", Q2S(strSql));
    QVariantList stValList;
    stValList << S2Q(szStartTime) << S2Q(szEndTime) << S2Q(szSnoCondition) << S2Q(szJnlNumCondition) << S2Q(szAccountNoCondition);

    QSqlQuery stResultQuery;
    bool bSuc = pDBMgr->DBSqlQuery(strSql,stValList,stResultQuery);
    if(!bSuc)
    {
        LOGINFO("%s: DBSqlQuery failed.  err:%s",__FUNCTION__,Q2S(pDBMgr->GetLastError()));
        return -3;
    }

	int nCount=0;
    QString sBuffer;

	sBuffer = "<xml>\n";
    QString sTempBuffer ;//add xlc 2017-2-15 查询的FSN信息 保存到XML文件中（以前是保存到txt文件中）

	char szWhat[32]={0};//根据交易类型DealType，入钞出钞(OperationType)综合判断结果
	char szLevel[8]={0}; //真钞或拒钞
    while(stResultQuery.next())
	{
		nCount++;
        if(stResultQuery.value(4).toInt()!=4) //Level!=4 拒钞
			sprintf(szLevel, "拒钞");
		else
			sprintf(szLevel,"真钞");

        if(stResultQuery.value(11).toString()== "DEP") //DealType
		{
            if(stResultQuery.value(8).toInt()==1) //OperationType
				sprintf(szWhat, "存款入钞");
			else
				sprintf(szWhat, "存款取消");
		}
        else if(stResultQuery.value(11).toString() == "CWD") //DealType
		{
			sprintf(szWhat, "取款出钞");
		}
        else if(stResultQuery.value(11).toString() =="PCWD") //DealType
		{
			sprintf(szWhat, "存折取款");
		}
        else if(stResultQuery.value(11).toString() == "PDEP") //DealType
		{
			sprintf(szWhat, "无卡折存款");
		}
        else if(stResultQuery.value(11).toString()== "BDEP") //DealType
		{
			sprintf(szWhat, "对公存款");
		}
        else if(stResultQuery.value(11).toString()== "EXCASH") //DealType
		{
            if(stResultQuery.value(8).toInt()==1) //OperationType
				sprintf(szWhat, "兑换入钞");
			else
				sprintf(szWhat, "兑换出钞");
		}
		else
			sprintf(szWhat, "");

		//文件行字段顺序：OperationTime, What(DealType&OperationType), ImageFile，SerialNumber,Currency, Value, Level(拒钞), JnlNum, AccountNo, CountIndex 
		//sprintf(szRecord, "%s,%s,%s,%s,%s,%s,%s,%06d,%s,%d\n", 
		//	sqlReader.GetStringValue(0), szWhat, sqlReader.GetStringValue(7), sqlReader.GetStringValue(6),
		//	sqlReader.GetStringValue(2), sqlReader.GetStringValue(3), szLevel,
		//	sqlReader.GetIntValue(9), sqlReader.GetStringValue(10),  sqlReader.GetIntValue(1));
		//sBuffer+=szRecord;

//	<item id = "1" DataTime="2/14" DealType="dep" Imgaddr="C:/wee" Currency="Currency1" Denomination="Denomination1" NoteType="NoteType" JnlNum="JnlNum" CardNum="CardNum" NoteId="NoteId"></item>
		sprintf(szRecord,"	<item "
			"id = \"%d\" "
			"CrownNum = \"%s\" "
			"DataTime=\"%s\" "
			"DealType=\"%s\" "
			"Imgaddr=\"%s\" "
			"Currency=\"%s\" "
			"Denomination=\"%s\" "
			"NoteType=\"%s\" "
			"JnlNum=\"%06d\" "
			"CardNum=\"%s\" "
			"NoteId=\"%d\""
			"></item>\n",
			( nCount%FSN_PER_PAGE == 0 ? FSN_PER_PAGE : nCount%FSN_PER_PAGE),
            Q2S(stResultQuery.value(6).toString()),
            Q2S(stResultQuery.value(0).toString()),
			szWhat, 
            Q2S(stResultQuery.value(7).toString()),
            Q2S(stResultQuery.value(2).toString()),
            Q2S(stResultQuery.value(3).toString()),
			szLevel,
            stResultQuery.value(9).toInt(),
            Q2S(stResultQuery.value(10).toString()),
            stResultQuery.value(1).toInt());
		sTempBuffer += szRecord;

		if (0 == nCount%FSN_PER_PAGE)
		{
            QString strTempData;
            strTempData.sprintf("<page num=\"%d\">\n",FSN_PER_PAGE);
			sBuffer+= strTempData;//"<page num=\"10\">\n";
			sBuffer+=sTempBuffer;
			sBuffer+="</page>\n";
			sTempBuffer = "";
		}
	}
    //sqlReader.Close();

	if (0 != nCount%FSN_PER_PAGE)
	{
        QString strEnd;
        strEnd.sprintf("<page num=\"%d\">\n",nCount%FSN_PER_PAGE);
        sBuffer+=strEnd;
		sBuffer+=sTempBuffer;
		sBuffer+="</page>\n";

		sTempBuffer = "";
	}

	sBuffer+= "<\/xml>\n";

	LOGINFO("ExcuteQuery Result: nCount=%d", nCount);

	if(nCount>0)
	{
		FILE *pFile = fopen(pszFileName,"w");
		if(pFile == NULL)
		{
			LOGINFO("Create file [%s] fail", pszFileName);
			return -3;
		}
		else
		{
            //WCHAR *wchFolderPath = new WCHAR[sBuffer.GetLength()*2+2];
            //int nPathSize = MultiByteToWideChar(CP_ACP, 0, sBuffer, sBuffer.GetLength() + 1, NULL, 0);
            //MultiByteToWideChar(CP_ACP, 0, sBuffer, sBuffer.GetLength() + 1, wchFolderPath, nPathSize);
            //std::string outtext = to_utf8(wchFolderPath,sBuffer.GetLength()*2+2);

            fprintf(pFile,"%s",Q2S(sBuffer));
			fflush(pFile);
			fclose(pFile);
			LOGINFO("Write file [%s] succeed", pszFileName);
            //delete[] wchFolderPath;
		}
	}

	LOGINFO("CPeristentData::QrySNRFunc end");

	return nCount;
}

//add by art for 冠字号凭条打印
//获取冠字号凭条页数和文件名前缀，生成要打印的冠字号文件
short CPeristentData::GetBillNoRptFiles(char *pszRptFiles)
{
	LOGINFO("CPeristentData::GetBillNoRptFiles begin");
    CDBManager* pDBMgr = CDBManager::GetInstance();
    if (!pDBMgr->IsDBConnected())
	{
		return -2;
	}

	char szCommonRptContent[1024]={0};
	CreateCommonRptContent(szCommonRptContent);

	//tmp by art begin
	char szDealStartTime[32]={0};
	GetScratchPadData("DEALSTARTTIME", szDealStartTime);
	//tmp by art end

    QString szSql = "select SerialNumber from SNRInfo \
                    where OperationTime>=? and  Level=4 \
					and (DealType='CWD' or (DealType='DEP' and OperationType=1) )\
                    order by OperationTime, CountIndex";
    LOGINFO("szSql=%s", Q2S(szSql));

    QVariantList stValList;
    stValList << S2Q(szDealStartTime);

    QSqlQuery stResultQuery;
    bool bSuc = pDBMgr->DBSqlQuery(szSql,stValList,stResultQuery);
    if(!bSuc)
    {
        LOGINFO("%s: DBSqlQuery failed.  err:%s",__FUNCTION__,Q2S(pDBMgr->GetLastError()));
        return -3;
    }

	int nCount=0, nPageCount=0;
    QString sPageBuffer[100];//暂定最大100页
    QString sDelimiter;
    while(stResultQuery.next())
	{
		nCount++;
		if((nCount-1)%200==0) //一页200条冠字号
			nPageCount++;

		if( nCount%4==0 )  //一行打4条冠字号
			sDelimiter="\n";
		else
			sDelimiter="  ";
        sPageBuffer[nPageCount-1]+=stResultQuery.value(0).toString()+sDelimiter;
	}
    //sqlReader.Close();

	LOGINFO("ExcuteQuery Result: nCount=%d  nPageCount=%d", nCount, nPageCount);

    QString strDir = CONST_INSTALL_DIR + "/DATA/MiddleLog/MRPT/BillNoRpt";
    const char *CONST_BILLNORPTFILE= Q2S(strDir); //冠字号凭条文件前缀

	if(nCount>0)
	{
		sprintf(pszRptFiles,"%d,%s", nPageCount, CONST_BILLNORPTFILE);  //返回的总页数值

		char szBillNoFile[MAX_PATH]={0};
		FILE *pFile=NULL;
		for(int i=0; i<nPageCount; i++)
		{
			sprintf(szBillNoFile, "%s%d.txt",CONST_BILLNORPTFILE, i+1); 
			pFile = fopen(szBillNoFile,"w");
			if(pFile == NULL)
			{
				LOGINFO("Create file [%s] fail", szBillNoFile);
				return -3;
			}
			else
			{
				if(i==0)
					fprintf(pFile,"%s",szCommonRptContent);//第一页上面打普通凭条内容

                fprintf(pFile,"%s",Q2S(sPageBuffer[i]));
				fprintf(pFile,"\n ============共%d页  第%d页============", nPageCount, i+1);
				fflush(pFile);
				fclose(pFile);
				LOGINFO("Write file [%s] succeed", szBillNoFile);
			}

		}
	}

	LOGINFO("CPeristentData::GetBillNoRptFiles end");

	return 0;
}

//add by art for 冠字号凭条打印: 冠字号凭条中的普通凭条内容
short CPeristentData::CreateCommonRptContent(char *pszRptContent)
{
	char szTmp[256];
	int nTmp=0;
	try
	{
		memset(szTmp, 0x00, sizeof(szTmp));
		GetScratchPadData("CARDNO", szTmp);
		strcat(pszRptContent, "卡号: ");
		if(strlen(szTmp)>=5)
			memset(szTmp+strlen(szTmp)-5, '*', 4);   //卡数倒数第2到5位用"*"替换
		strcat(pszRptContent, szTmp);
		strcat(pszRptContent, "\n");

		memset(szTmp, 0x00, sizeof(szTmp));
		GetData("TERMINALNUM","STRING",szTmp); //永久数据
		strcat(pszRptContent, "设备编号: ");
		strcat(pszRptContent, szTmp);
		strcat(pszRptContent, "    ");

		memset(szTmp, 0x00, sizeof(szTmp));
		GetScratchPadData("DEALTYPE", szTmp);
		strcat(pszRptContent, "交易类型: ");
		strcat(pszRptContent, szTmp);
		strcat(pszRptContent, "\n");

		memset(szTmp, 0x00, sizeof(szTmp));
		GetScratchPadData("TRANSAMOUNT", szTmp);
		nTmp=atoi(szTmp);
		sprintf(szTmp, "%d.%02d", nTmp/100, nTmp % 100); //金额注意分转为以元为单位，加"."号
		strcat(pszRptContent, "交易金额: ");
		strcat(pszRptContent, szTmp);
		strcat(pszRptContent, "  ");

		memset(szTmp, 0x00, sizeof(szTmp));
		GetScratchPadData("POUNDAGE", szTmp);
		nTmp=atoi(szTmp);
		sprintf(szTmp, "%d.%02d", nTmp/100, nTmp % 100); //手续费注意分转为以元为单位，加"."号
		strcat(pszRptContent, "手续费: ");
		strcat(pszRptContent, szTmp);
		strcat(pszRptContent, "\n");

		/*
		memset(szTmp, 0x00, sizeof(szTmp));
		GetScratchPadData("AUTHCODE", szTmp);
		strcat(pszRptContent, "授权码: ");
		strcat(pszRptContent, szTmp);
		*/

		memset(szTmp, 0x00, sizeof(szTmp));
		GetData("JNLNUM","LONG",szTmp); //永久数据
		int nJnlNum=atoi(szTmp);
		sprintf(szTmp, "%06d", nJnlNum);
		strcat(pszRptContent, "流水号: ");
		strcat(pszRptContent, szTmp);
		strcat(pszRptContent, "      ");

		/*
		memset(szTmp, 0x00, sizeof(szTmp));
		GetScratchPadData("SYSREFNUM", szTmp);
		strcat(pszRptContent, "参考号: ");
		strcat(pszRptContent, szTmp);
		strcat(pszRptContent, "  ");
		*/
		memset(szTmp, 0x00, sizeof(szTmp));
		GetScratchPadData("RESPONSECODE", szTmp);
		strcat(pszRptContent, "响应吗: ");
		strcat(pszRptContent, szTmp);
		strcat(pszRptContent, "\n");

		/*??
		memset(szTmp, 0x00, sizeof(szTmp));
		GetScratchPadData("TRANSACTIONRESULT", szTmp);
		strcat(pszRptContent, "交易结果: ");
		strcat(pszRptContent, szTmp);
		strcat(pszRptContent, "      ");
		*/
	}
    catch (exception* e)
	{
		LOGERROR("CPeristentData::CreateCommonRptContent fail!");
	}

	return 0;
}

//获取临时数据
short CPeristentData::GetScratchPadData(const char *pszDataName, char *pszValue)
{
	short nRet=0;

	string sKey;
	sKey=pszDataName;
	sKey+="~^";
	sKey+="STRING";
	SCRATCHMAP::iterator iter; 
	iter = CScratchPad::m_ScratchPad.find(sKey);
	if(iter!=CScratchPad::m_ScratchPad.end())
	{
		sprintf(pszValue, (iter->second).c_str());
	}
	else
	{
		sprintf(pszValue, "");
		nRet=-2;
	}

	return nRet;
}


// 设置临时数据
short CPeristentData::SetScratchPadData(char *pszDataName, char *pszDataType, char *pszNewValue)
{
    string sKey(pszDataName);
    sKey+="~^";
    sKey+=pszDataType;

    CScratchPad::m_ScratchPad[sKey]=pszNewValue;

    return 0;
}

// add by zengwh for 深圳农行三期需求生成json文件供应用使用
short CPeristentData::GetBankCodeJsonData(void)
{
    /*
    LOGINFO("Use GetBankCodeJsonData!开始生成bank.json文件");
    SHORT nRet=0;

    map<QString, QString> MapCtoCforBankCod;
    int iBankTotal = 0;
    QStringArray strBankCodeKey;

    nRet = GetBankCodeandName(MapCtoCforBankCod, &iBankTotal, strBankCodeKey);
    if (0 == nRet)
    {
        return -1;
    }

    Json::Value root;
    Json::Value person;
    QString strBankCodeTmp, strBankNameTmp, strBankNo;

    for (int i = 0; i < iBankTotal; i++)
    {
        strBankCodeTmp.Empty();
        strBankNameTmp.Empty();
        strBankNo.Empty();

        strBankCodeTmp = strBankCodeKey.GetAt(i);
        person["bankcod"] = strBankCodeTmp.GetBuffer(strBankCodeTmp.GetLength());
        strBankNameTmp = MapCtoCforBankCod[strBankCodeTmp];
        person["banknam"] = strBankNameTmp.GetBuffer(strBankNameTmp.GetLength());

        strBankNo = GetBankName(strBankCodeTmp);
        person["bankno"] = strBankNo.GetBuffer(strBankNo.GetLength());

        root.append(person);
    }

    // json格式写入
    Json::StyledWriter styled_writer;
    std::string json_file = styled_writer.write(root);

    ofstream ofs;
    ofs.open("C:\\TCR\\jsapp\\Framework\\jsonData\\bank.json");
    assert(ofs.is_open());

    QString strWrite;
    strWrite.Format("%s",json_file);
    ofs<<G2U(strWrite.GetBuffer(strWrite.GetLength()));
    ofs.close();

    LOGINFO("End GetBankCodeJsonData!已生成bank.json文件");
    */
    return 0;
}

short CPeristentData::GetProcCodeJsonData(void)
{
    /*
    LOGINFO("Use GetProcCodeJsonData!开始生成省市关联json文件");
    map<QString, QString> MapCtoCforGafeeCod;
    map<int, QString> MapItoCforProvCod;
    SHORT nRet = 0;

    int iProcCodTotal = 0;
    int iProcCodKey[100] = {0};
    nRet = GetProcCodandName(MapItoCforProvCod, &iProcCodTotal, iProcCodKey);
    if (0 == nRet)
    {
        return -1;
    }

    QString strProcNameTmp;
    QString strGafeeNameTmp;
    char szArea[100];

    Json::Value ProcRoot;
    Json::Value ProcPerson;

    // 生成省市json表
    for (int i = 0; i < iProcCodTotal; i++)
    {
        ProcPerson["provcod"] = iProcCodKey[i];
        strProcNameTmp = MapItoCforProvCod[iProcCodKey[i]];
        ProcPerson["gargnnam"] = strProcNameTmp.GetBuffer(strProcNameTmp.GetLength());

        ProcRoot.append(ProcPerson);
    }

    // json格式写入
    Json::StyledWriter styled_writer;
    std::string json_file = styled_writer.write(ProcRoot);

    QString strWrite;
    strWrite.Format("%s",json_file);

    ofstream ofs;
    ofs.open("C:\\TCR\\jsapp\\Framework\\jsonData\\prov.json");
    assert(ofs.is_open());
    ofs<<G2U(strWrite.GetBuffer(strWrite.GetLength()));
    ofs.close();

    LOGINFO("省级表生成完毕");

    // 方便AP进行三级联动，将数据切分
    for (int i = 0; i < iProcCodTotal; i++)
    {
        Json::Value root;
        Json::Value person;

        person["provcod"] = iProcCodKey[i];
        strProcNameTmp = MapItoCforProvCod[iProcCodKey[i]];
        person["gargnnam"] = strProcNameTmp.GetBuffer(strProcNameTmp.GetLength());

        int iGafeeCodTotal = 0;
        QStringArray strGafeeCodKey;
        nRet = GetGargnNameandCode(MapCtoCforGafeeCod, &iGafeeCodTotal, strGafeeCodKey, iProcCodKey[i]);
        if (0 == nRet)
        {
            LOGINFO("生成省市关联json文件失败");
            return -1;
        }

        for (int j = 0; j < iGafeeCodTotal; j++)
        {
            strGafeeNameTmp = strGafeeCodKey.GetAt(j);
            person["area"][j]["gafeecod"] = strGafeeNameTmp.GetBuffer(strGafeeNameTmp.GetLength());
            strGafeeNameTmp = MapCtoCforGafeeCod[strGafeeCodKey.GetAt(j)];
            person["area"][j]["gargnnam"] = strGafeeNameTmp.GetBuffer(strGafeeNameTmp.GetLength());
        }

        root.append(person);

        // json格式写入
        Json::StyledWriter styled_writer;
        std::string json_file = styled_writer.write(root);

        memset(szArea, 0, sizeof(szArea));
        sprintf_s(szArea, "C:\\TCR\\jsapp\\Framework\\jsonData\\json\\area%d.json",iProcCodKey[i]);
        strWrite.Empty();
        strWrite.Format("%s",json_file);

        ofstream ofs;
        ofs.open(szArea);
        assert(ofs.is_open());
        ofs<<G2U(strWrite.GetBuffer(strWrite.GetLength()));
        ofs.close();
    }
    LOGINFO("End GetProcCodeJsonData!已生成省市关联json文件");
    */
    return 0;
}

short CPeristentData::GetQRCodePic(void)
{
    /*
    SHORT nRet = 0;

    // base64数据转图片
    char szBase64Data[1024*10];
    int iDataLength = 0;
    int iBase64Decode = 0;
    CMD5 md5;

    memset(szBase64Data, 0, sizeof(szBase64Data));
    GetScratchPadData("PICBASE64", szBase64Data);

    iDataLength = strlen(szBase64Data);

    USES_CONVERSION;

    FILE *fh = 0;
    long recWrit;
    if (0 != fopen_s(&fh, QRCODE_PIC_PATH, "wb"))
    {
        return -1;
    }

    string strbase64Deocde = md5.Decode(szBase64Data, iDataLength, iBase64Decode);
    recWrit = fwrite(strbase64Deocde.c_str(), iBase64Decode, 1, fh);
    LOGINFO("recWrit:%d", recWrit);
    fclose(fh);

    // 为了让SP打印二维码，将png转bmp
    LOGINFO("开始转存图片");
    CImage image;
    image.Load(QRCODE_PIC_PATH);
    image.Save(QRCODE_PIC_PATH_FOR_BMP);
    DeleteFile(QRCODE_PIC_PATH); // 转换之后删除
    LOGINFO("转存图片结束");
    */
    return 0;
}

short CPeristentData::GetMD5Data(void)
{
    /*
    char szQrType[20];
    char szDeviceNo[20];
    char szSerialNo[20];
    char szTimestamp[20];
    char szMD5Key[100];

    memset(szQrType, 0, sizeof(szQrType));
    memset(szDeviceNo, 0, sizeof(szDeviceNo));
    memset(szSerialNo, 0, sizeof(szSerialNo));
    memset(szTimestamp, 0, sizeof(szTimestamp));
    memset(szMD5Key, 0, sizeof(szMD5Key));

    // 获取加密参数
    GetScratchPadData("QRTYPE", szQrType);
    GetScratchPadData("DEVICENO", szDeviceNo);
    GetScratchPadData("SERIALNO", szSerialNo);
    GetScratchPadData("TIMESTAMP", szTimestamp);
    GetScratchPadData("MD5", szMD5Key);


    // 根据银行要求组建数据并进行MD5加密
    char szEncodeTmp[1024];
    memset(szEncodeTmp, 0, sizeof(szEncodeTmp));
    sprintf_s(szEncodeTmp, "deviceNo=%s&qrType=%s&serialNo=%s&timestamp=%s%s",
        szDeviceNo, szQrType, szSerialNo, szTimestamp, szMD5Key);
    LOGINFO("Encode:%s", szEncodeTmp);

    CMD5 md5;
    md5.update(szEncodeTmp);
    char szMD5Value[33];
    memset(szMD5Value, 0, sizeof(szMD5Value));
    strcpy(szMD5Value, md5.toMD5String32(MD5_UPPERCASE).c_str());
    SetScratchPadData("MD5VALUE", "STRING", szMD5Value);
    LOGINFO("MD5Value:%s", szMD5Value);
    */
    return 0;
}

QString CPeristentData::GetBankName(QString strBankCod)
{
    /*
    QString StrRet;
    if (TRUE != OpenDatabase())
    {
        return StrRet;
    }

    char szSql[10*1024];
    char pszValue[10*1024];

    // 获取联行号
    memset(szSql, 0, sizeof(szSql));
    memset(pszValue, 0, sizeof(pszValue));
    sprintf_s(szSql ,"select bankno from param_bankclasses where bankflag = 2 and bankcod = \'%s\'", strBankCod);
    SQLiteDataReader sqlReader = m_Sqlite.ExcuteQuery(G2U(szSql).c_str());
    if(sqlReader.Read())
    {
        strcpy(pszValue, U2G( sqlReader.GetStringValue(0) ).c_str());
    }
    else
    {
        sprintf(pszValue, "");
    }
    sqlReader.Close();

    StrRet.Format(pszValue);
    return StrRet;
    */
    return "";
}

SHORT CPeristentData::GetProcCodandName(std::map<int, QString>& MapItoCforProvCod, int* iTotal, int iKey[])
{
    SHORT nRet = 0;
    /*
    if (TRUE != OpenDatabase())
    {
        return nRet;
    }

    QString cQuerySql;
    int iTmp[1024*10];
    int iProvCodTmp = 0;
    int nRow;
    int nColumn;
    int index;

    char **dbResult;
    QString strTmp;
    cQuerySql.Format("SELECT provcod FROM param_bankarea where gafeecod = \'0000\'");

    if(m_Sqlite.ExcuteQueryTable(G2U(cQuerySql).c_str(),nRow,nColumn,dbResult) == SQLITE_OK)
    {
        index = nColumn; //前面说过 dbResult 前面第一行数据是字段名称，从 nColumn 索引开始才是真正的数
        for(int i = 0; i < nRow ; i++ )
        {
            iProvCodTmp = atoi(dbResult[index]);
            iTmp[i] = iProvCodTmp;
            index++;
        }
    }
    else
    {
        return nRet;
    }

    char szSql[10*1024];
    QString strGargNameTmp;
    int iIndexTmp = 0;

    for (int i = 0; i < (index - nColumn); i++)
    {
        memset(szSql, 0, sizeof(szSql));
        sprintf_s(szSql, "select gargnnam from param_bankarea where gafeecod=\'0000\' and provcod=\'%d\'", iTmp[i]);
        SQLiteDataReader sqlReader = m_Sqlite.ExcuteQuery(G2U(szSql).c_str());
        if(sqlReader.Read())
        {
            strGargNameTmp.Empty();
            strGargNameTmp.Format(U2G(sqlReader.GetStringValue(0) ).c_str());
            MapItoCforProvCod[iTmp[i]] = strGargNameTmp;
            iKey[iIndexTmp++] = iTmp[i];
        }
        sqlReader.Close();
    }

    *iTotal = iIndexTmp;
    nRet = 1;
    */
    return nRet;
}

SHORT CPeristentData::GetGargnNameandCode(std::map<QString, QString>& MapCtoCforGafeeCod, int* iTotal, QStringList& csKey, int iProcCode)
{
    SHORT nRet = 0;
    /*
    if (TRUE != OpenDatabase())
    {
        return nRet;
    }

    QString cQuerySql;
    QString strTmp;
    int nRow;
    int nColumn;
    int index;
    int iGafeeCodIndex = 0, iGargnNameIndex = 0;

    char **dbResult;
    char szSql[10*1024];

    memset(szSql, 0, sizeof(szSql));
    sprintf_s(szSql, "SELECT gafeecod, gargnnam FROM param_bankarea where provcod=\'%d\' and gafeecod != \'0000\'", iProcCode);
    cQuerySql.Format(szSql);

    if(m_Sqlite.ExcuteQueryTable(G2U(cQuerySql).c_str(),nRow,nColumn,dbResult) == SQLITE_OK)
    {
        // 第一列为gafeecod，第二列为gargnnam
        QString strGafeeCodTmp, strGargnNameTmp;
        index = nColumn;
        for (int x = 0; x < nRow; x++)
        {
            for (int y = 0; y < nColumn; y++)
            {
                strTmp = dbResult[index];
                if (0 == y % 2)
                {
                    strGafeeCodTmp.Format("%s",U2G(strTmp.GetBuffer(strTmp.GetLength())).c_str()) ;
                }
                else
                {
                    strGargnNameTmp.Format("%s",U2G(strTmp.GetBuffer(strTmp.GetLength())).c_str());
                    MapCtoCforGafeeCod[strGafeeCodTmp] = strGargnNameTmp;
                    csKey.Add(strGafeeCodTmp);
                    iGafeeCodIndex++;
                    iGargnNameIndex++;
                }
                ++index;
            }
        }
    }
    else
    {
        return nRet;
    }

    *iTotal = iGafeeCodIndex;
    nRet = 1;
    */
    return nRet;
}

SHORT CPeristentData::GetBankCodeandName(std::map<QString, QString>& MapCtoCforBankCod, int* iTotal, QStringList &csKey)
{
    SHORT nRet = 0;
    /*
    if (TRUE != OpenDatabase())
    {
        return nRet;
    }

    QString cQuerySql;
    QString strTmp;
    int nRow;
    int nColumn;
    int index;
    char **dbResult;
    char szSql[10*1024];
    int iBankCodeTotal = 0;

    memset(szSql, 0, sizeof(szSql));
    sprintf_s(szSql, "SELECT bankcod, banknam FROM param_bankclasses where bankflag = 1");
    cQuerySql.Format(szSql);

    if(m_Sqlite.ExcuteQueryTable(G2U(cQuerySql).c_str(),nRow,nColumn,dbResult) == SQLITE_OK)
    {
        // 第一列为bankcod，第二列为banknam
        QString strBankCodTmp, strBankNameTmp;
        index = nColumn;

        for (int x = 0; x < nRow; x++)
        {
            for (int y = 0; y < nColumn; y++)
            {
                strTmp = dbResult[index];
                if (0 == y % 2)
                {
                    strBankCodTmp.Empty();
                    strBankCodTmp.Format("%s",U2G(strTmp.GetBuffer(strTmp.GetLength())).c_str()) ;
                }
                else
                {
                    strBankNameTmp.Empty();
                    strBankNameTmp.Format("%s",U2G(strTmp.GetBuffer(strTmp.GetLength())).c_str()) ;
                    MapCtoCforBankCod[strBankCodTmp] = strBankNameTmp;
                    csKey.Add(strBankCodTmp);
                    iBankCodeTotal++;
                }
                ++index;
            }
        }
    }
    else
    {
        return nRet;
    }

    *iTotal = iBankCodeTotal;
    nRet = 1;
    */
    return nRet;
}
