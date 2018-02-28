// Last updated at 2016/9/23 14:15:36

#include "stdafx.h"
#include "mtsl.h"
#include "common_def.h"
#include "AlarmLib.h"

#define DATABASE_NAME                   "TCR.db3"
#define DATABASE_TABLE_NAME             "TransLog"
QString strCNT_ACCTFILENAME = CONST_DATA_DIR+"/Acct/Acct.txt";
const char* CNT_ACCTFILENAME=Q2S(strCNT_ACCTFILENAME); //add by zyf add for 交易记录文件
QString strCNT_ACCTFILEPATH = CONST_DATA_DIR+"/Acct";
char* CNT_ACCTFILEPATH=Q2S(strCNT_ACCTFILEPATH);   ////zyf add
QString strCNT_CARDRETAINFILENAME = CONST_DATA_DIR + "/CardRetain/CardRetain.txt";
const char* CNT_CARDRETAINFILENAME=Q2S(strCNT_CARDRETAINFILENAME); //add by zyf add for 吞卡记录文件
QString strCNT_CARDRETAINPATH =CONST_DATA_DIR +"/CardRetain";
char* CNT_CARDRETAINPATH=Q2S(strCNT_CARDRETAINPATH);/////ZYF ADD
QString strCNT_CLOSEACCOUNTNAME = CONST_DATA_DIR + "/Close/Close.txt";
const char* CNT_CLOSEACCOUNTNAME=Q2S(strCNT_CLOSEACCOUNTNAME); // add by zwh add for 销户交易记录表
QString strCNT_CLOSEACCOUNTPATH =CONST_DATA_DIR + "/Close";
char* CNT_CLOSEACCOUNTPATH=Q2S(strCNT_CLOSEACCOUNTPATH);


LONG g_lCount = 0;
double g_Money = 0;
QString gStartTime(""),gEndTime(""),gSno("*"), gJnlNum(""), gAccountNo(""); //modify by art
QString gSumData("");
QString gRowDataTmp("");
int g_i=0;

int CallBack_CountType(void * param,int n_col,char **col_value,char **col_name)
{
    if(col_value && *col_value)
    {
        g_lCount = atoi(col_value[0]);
    }

    return 0;
}

int CallBack_SumMoney(void * param,int n_col,char **col_value,char **col_name)
{
    if(col_value && *col_value)
    {
        g_Money = atof(col_value[0]);
    }

    return 0;
}



int CallBack_SumData(void * param,int n_col,char **col_value,char **col_name)
{
	if (col_value && *col_value)
	{
        QString DataTmp(col_value[0]);
		if (DataTmp == "")
		{
			DataTmp="0";
		}

		if (g_i < 53)
		{
			gRowDataTmp+=(DataTmp+",");
			gSumData+=gRowDataTmp;
            gRowDataTmp.clear();
            gRowDataTmp.clear();
		}
		else
		{
			gSumData+=DataTmp;
		}
		
		g_i++;
	}
	else
	{
        QString DataTmp("0");
		
		if (g_i < 53)
		{
			gRowDataTmp+=(DataTmp+",");
			gSumData+=gRowDataTmp;
            gRowDataTmp.clear();
		}
		else
		{
			gSumData+=DataTmp;
		}
		
		g_i++;
	}
	return 0;
}

CMTSL* CMTSL::m_pInstance = NULL;

CMTSL::CMTSL()
{
	LOGINFO("MTSL模块创建");
	m_strCheckFilePath="";
	m_nCurrentPos=0;
	m_nLinesCount=0;
	m_nLinesPerPage=0;
	m_sFileName="";
	m_strDataBasePathName="";
    m_UpdateRecordData="";
    m_UpdateCheckRecordData="";
	m_bOpened=FALSE;
	m_bJnlInit=FALSE;
	m_bFsnInit=FALSE;
    //m_lpDB=NULL;
    //InitializeCriticalSection(&HandleRecordFile_critical);

    Init();
	
}

CMTSL::~CMTSL()
{
    UnInit();
    //DeleteCriticalSection(&HandleRecordFile_critical);
	LOGINFO("MTSL模块退出");
}
CMTSL *CMTSL::GetInstance()
{
    if(m_pInstance == NULL)
    {
        m_pInstance = new CMTSL();
    }
    return m_pInstance;
}
void CMTSL::Init()
{
	GetConfigPath();//得到配置文件的路径

    QString str_chSearchResultPath = g_Helper.GetPrivateProfileString("MFSN","SearchResultPath","",m_strConfigPath);
    memcpy(m_chSearchResultPath,Q2S(str_chSearchResultPath),str_chSearchResultPath.length());
    //m_hSearchEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
    //m_hUpdateRecordEvent=CreateEvent(NULL,TRUE,FALSE,NULL);
    //m_hUpdateCheckEvent=CreateEvent(NULL,TRUE,FALSE,NULL);
	DWORD dw = 0;
    //m_hThread = CreateThread(NULL,0,ThreadProc,this,0,&dw);

	N_InitUpCheckFile();////初始化相关上传对账文件

	N_InitDataBase();////////初始化相关数据库主要是打开和创建表
}

void CMTSL::UnInit()
{
	
//	if(m_bOpened)
//	{
//		sqlite3_close(m_lpDB);//关闭数据库
//		m_lpDB = NULL;
//		m_bOpened = false;
//		LOGINFO("Close DataBase Success");
//	}

//	CloseHandle(m_hSearchEvent);
//	CloseHandle(m_hUpdateRecordEvent);
//	CloseHandle(m_hUpdateCheckEvent);
//	CloseHandle(m_hThread);

	UnInitFsnSync();////关闭FSN文件
	UnInitJnlSync();/////////关闭JNL文件
}

LONG CMTSL::InitJnlSync(QString sFileName, LONG nLinesPerPage)
{
	m_sFileName="";
    if(QString::compare(sFileName,"")==0 || nLinesPerPage<=0 )
	{
		LOGERROR("%s函数传入参数有误",__FUNCTION__);
		return -1;
	}
	
	if(m_JnlStream.is_open())
	{
		LOGERROR("%s, 文件已被打开，关闭此文件", __FUNCTION__);
		m_JnlStream.close();
		m_bJnlInit = FALSE;
	}

    m_JnlStream.open(sFileName.toStdString(), std::ios::in);
	if(m_JnlStream.fail())
	{
        LOGERROR("%s打开文件失败, GetLastError=%d",__FUNCTION__, -2);
		return -2;
	}
		

	m_sFileName=sFileName;
	m_nLinesPerPage=nLinesPerPage;

	std::string strLine;  
	m_nLinesCount=0;
	while(getline(m_JnlStream,strLine))  
	{
// 		cout<<strLine <<endl;                
		m_nLinesCount++;
	}

	m_nCurrentPos=m_nLinesCount;
	m_bJnlInit=TRUE;

	return 0;
}


LONG CMTSL::UnInitJnlSync()
{
	if(m_JnlStream.is_open())
	{
		m_JnlStream.close();
		m_bJnlInit=FALSE;
	}

	return 0;
}

LONG CMTSL::InitFsnSync(QString sFileName)
{
    if(QString::compare(sFileName,"")==0 )
	{
		LOGERROR("%s函数传入参数有误",__FUNCTION__);
		return -1;
	}
		

    m_FsnStream.open(sFileName.toStdString(), ios::in);
	if(m_FsnStream.fail())
	{
		LOGERROR("%s打开文件失败",__FUNCTION__);
		return -2;
	}

	m_bFsnInit=TRUE;

	return 0;
}

LONG CMTSL::UnInitFsnSync()
{
	if (m_FsnStream.is_open())
	{
		m_FsnStream.close();
		m_bFsnInit=FALSE;
	}

	return 0;
}

QString CMTSL::ReadFSNSync()
{
    QString strResult;

	strResult="";
	if (m_bFsnInit)
	{
		char szBuffer[256];
		if(!m_FsnStream.eof())
		{
			m_FsnStream.getline(szBuffer,sizeof(szBuffer),'\n');
            strResult.sprintf("%s",szBuffer);
		}
	}

	return strResult;
}

SHORT CMTSL::SearchFSN(QString startTime, QString endTime,QString strSno, QString strJnlNum, QString strAccountNo)
{
    LOGINFO("%s,IN,stime=%s,etime=%s,strSno = %s,strJnlNum = %s,strAccountNo = %s",__FUNCTION__,Q2S(startTime),Q2S(endTime),Q2S(strSno), Q2S(strJnlNum), Q2S(strAccountNo));

	//modify by art begin
    gStartTime = startTime.trimmed();
	if(""==gStartTime)
		gStartTime="*";
    gEndTime = endTime.trimmed();
	if(""==gEndTime)
		gEndTime="*";
    gSno = strSno.trimmed(); //add by art
    gSno.replace('*','_'); //add by art
	if(""==gSno)
		gSno="*";
    gJnlNum=strJnlNum.trimmed();  //add by art
	if(""==gJnlNum)
		gJnlNum="*";
    gAccountNo=strAccountNo.trimmed(); //add by art
	if(""==gAccountNo)
		gAccountNo="*";
	//modify by art end

    //SetEvent(m_hSearchEvent);//异步调用

	LOGINFO("%s,OUT",__FUNCTION__);

	return 0;
}

SHORT CMTSL::N_SearchFSN(QString startTime, QString endTime,QString strSno, QString strJnlNum, QString strAccountNo)//modify by art
{
	//add by art for 冠字号信息查询	begin
	char szPara[128]={0};
    sprintf(szPara, "%s~%s~%s~%s~%s", Q2S(startTime), Q2S(endTime), Q2S(strSno), Q2S(strJnlNum), Q2S(strAccountNo));
	short nRet=dllDSTO.AddPersistentData("~QRYSNR~",m_chSearchResultPath,szPara);
	if(nRet>0)
	{
        emit SearchFSNComplete(m_chSearchResultPath);
		LOGINFO("事件,SearchFSNComplete,总记录数：%d", nRet);
	}
	else
	{
        emit SearchFSNFail();
		LOGERROR("事件,SearchFSNFail, nRet=%d", nRet);
	}

	return 0;
}

//DWORD WINAPI CMTSL::ThreadProc(LPVOID lpParam)
//{
//	int ret = 0;
//	while(1)
//	{
//		CMTSL * pTsl = (CMTSL *)lpParam;
//		HANDLE handles[3] = {pTsl->m_hSearchEvent,pTsl->m_hUpdateRecordEvent,pTsl->m_hUpdateCheckEvent};
//		DWORD ret = 0;
//		ret = WaitForMultipleObjects(3,handles,false,INFINITE);
//		if( WAIT_OBJECT_0 == ret) //modify by art
//		{
//			EnterCriticalSection(&pTsl->HandleRecordFile_critical);
//			ResetEvent(pTsl->m_hSearchEvent);
//			ret = pTsl->N_SearchFSN(gStartTime,gEndTime,gSno, gJnlNum, gAccountNo);
//			LeaveCriticalSection(&pTsl->HandleRecordFile_critical);
//		}
//		else if ( ret == (WAIT_OBJECT_0+1))
//		{
//			EnterCriticalSection(&pTsl->HandleRecordFile_critical);
//			ResetEvent(pTsl->m_hUpdateRecordEvent);
//			pTsl->N_UpdateRecord();
//			LeaveCriticalSection(&pTsl->HandleRecordFile_critical);
//		}
//		else if (ret == (WAIT_OBJECT_0+2))
//		{
//			EnterCriticalSection(&pTsl->HandleRecordFile_critical);
//			ResetEvent(pTsl->m_hUpdateCheckEvent);
//			pTsl->N_UpdateCheckRecord();
//			LeaveCriticalSection(&pTsl->HandleRecordFile_critical);
//		}
//		Sleep(100);
//	}
//	return 0;
//}



//nUpDownFlag--- 1:下翻 -1: 下翻; nLineOrPageFlag--- 0: 翻页, 1:移行
QString CMTSL::ReadJNLSync(long nUpDownFlag,long nLineOrPageFlag)
{
    QString strResult;

	strResult="";
	if (m_bJnlInit)
	{
		if(m_sFileName=="")
			return strResult;

		LONG nScrollLines=0;	
		if(1==nLineOrPageFlag)
			nScrollLines=1*nUpDownFlag;
		else
			nScrollLines=nUpDownFlag*m_nLinesPerPage;


		m_nCurrentPos=m_nCurrentPos+ nScrollLines;
		if(m_nCurrentPos<0)
			m_nCurrentPos=0;
		if(m_nCurrentPos>m_nLinesCount-m_nLinesPerPage)
		{
			m_nCurrentPos=m_nLinesCount-m_nLinesPerPage;
		}

		std::string strLine;  
		m_JnlStream.clear();
		m_JnlStream.seekg(0,ios::beg); 
		for(int i=0;i<m_nCurrentPos;i++)
		{
			getline(m_JnlStream,strLine);
			if(m_JnlStream.eof())
			{
				return strResult;
			}
		}

		for(int i=0;i<m_nLinesPerPage+1;i++)
		{
			getline(m_JnlStream,strLine);
            QString strAppend = "";
            strAppend.sprintf("%c%s",'\n',strLine.c_str());
            strResult += strAppend;
			//strResult=strResult+"\n"+szBuffer;
			if(m_JnlStream.eof())
			{
				return strResult;
			}
		}
	}
	

	return strResult;
}

//nBeginFlag--- 1:从头开始查找, 0:查找下一条;  nType--- 1:按时间 2:按卡号折号 3:按流水号
QString CMTSL::SearchJNLSync(long nBeginFlag,long nType,QString sSubString)
{
    QString strResult;

	strResult="";
	if (m_bJnlInit)
	{
        if(sSubString.length()==0)
			return strResult;

		LONG oldCurrentPos=m_nCurrentPos;

		char s[256];
		switch( ENUM_SEARCH_TYPE(nType))
		{
		case stByTime:
            sprintf(s, "----%s:", Q2S(sSubString));  // 如"yyyy/mm/dd hh:mm"  可查到分钟, 在字符串中"---2016/04/05 11:20:00--"
			break;
		case stByCardNo:
            sprintf(s, Q2S(sSubString));  // 卡号, 在字符串中"折号卡号：6228480129990648773"
			break;
		case stBySerialNo:
            sprintf(s, "流水号   %s", Q2S(sSubString));  // 在字符串中"流水号 123456"
			break;
		default:
            sprintf(s, Q2S(sSubString));
			break;
		}

		std::string strLine;  
		std::string strCardNum; 
		if(1==nBeginFlag)
		{
			m_nCurrentPos=0;
			m_JnlStream.clear();
			m_JnlStream.seekg(0,ios::beg);
		}
		else
		{
			m_nCurrentPos+=m_nLinesPerPage;  //test
		}

		BOOL bFound=FALSE;
		int istrLineLen=0;
		while(getline(m_JnlStream,strLine))  
		{
			istrLineLen=strLine.length();               
			m_nCurrentPos++;
			if (nType == stByCardNo)
			{
// 				strCardNum=strLine.substr(8,istrLineLen-9);
// 				if(strstr(strCardNum.c_str(), s)!=NULL)
// 				{
// 					bFound=TRUE;
// 					break;
// 				}			
				// add by duke begin
				// 原查找存在崩溃和输入字符串较短查到其他非卡号字符串
				size_t found1 = strLine.find("卡号");  
				size_t found2 = strLine.find("折号");  
				size_t found3 = strLine.find("账号");  
				if (found1 != string::npos || found2 != string::npos || found3 != string::npos)
				{
					if (strLine.find(s) != string::npos)
					{
						bFound=TRUE;
						break;
					}
				}
				// add by duke end
			}
			else
			{
				if(strstr(strLine.c_str(), s)!=NULL)
				{
					bFound=TRUE;
					break;
				}
			}
			
		}

		if(!bFound)
		{
			m_nCurrentPos=oldCurrentPos;
			return strResult;
		}

		for(int i=0;i<m_nLinesPerPage+1;i++)
		{
            QString strAppend = "";
            strAppend.sprintf("%c%s",'\n',strLine.c_str());
            strResult += strAppend; //查到的第一行要先加上
			getline(m_JnlStream,strLine);
			if(m_JnlStream.eof())
			{
				break;
			}
		}
	}

	return strResult;
}

int CMTSL::CreateFileDirectory(char* szPath)
{
    if ( QDir(szPath).exists()) return 0;

    if (!QDir(szPath).mkpath(szPath))
    {
        LOGERROR("CreateFileDirectory fail, nRet = -1");
        Alarm("M0010001");
        return -1;
    }

	return 0;
}

//void CMTSL::N_InitDataBase()
//{
//	N_GetDatabasePath();///////获取数据库路径

//    QString str = G2U(m_strDataBasePathName);

//    int ret = sqlite3_open(str, &m_lpDB);

//	if(SQLITE_OK != ret)
//	{
//		LOGERROR("Open DataBase Fail");
//	}
//	else
//	{
//		m_bOpened=TRUE;
//		char * lpErrMsg = NULL;
//        QString strSql("");
//		strSql.sprintf("Create Table %s (ID integer primary key autoincrement, \
//					  Date text, Time text, CardNo text, JnlNum text, Type text, \
//					  Money float, IDNo text, ImageHeadPath text, Result text)", DATABASE_TABLE_NAME);
//		LOGINFO("[SQL] [%s]", strSql);

//        ret = sqlite3_exec(m_lpDB, strSql, NULL, NULL, &lpErrMsg);
//		if(SQLITE_OK == ret)
//		{
//			LOGINFO("Open DataBase Success And Create Table Success");
//		}
//		else
//		{
//            QString strTmp("");
//            QString strErr(lpErrMsg);
//			strTmp.sprintf("table %s already exists", DATABASE_TABLE_NAME);
//			if(strTmp == strErr)
//			{
//				LOGINFO("Open DataBase Success And Create Table Success(table already exists)");
//			}
//			else
//			{
//				sqlite3_close(m_lpDB);//关闭数据库
//				m_lpDB = NULL;
//				LOGERROR("Open DataBase Success, but Create Table Fail");
//			}
//		}
//	}
//}

///////返回值0 代表成功 其他代表失败
int CMTSL::WriteTransFile(QString sData)
{
	std::fstream fn;

	try
	{
		CreateFileDirectory(CNT_ACCTFILEPATH);
		fn.open(CNT_ACCTFILENAME, std::ios::app);
		if(fn.fail())
		{
			LOGERROR("%s创建打开交易记录文件失败",__FUNCTION__);
			return -1;
		}
			

        QDateTime EtTime = QDateTime::currentDateTime();
        QString strTime;
        strTime.sprintf("%04d/%02d/%02d %02d:%02d:%02d,", EtTime.date().year(), EtTime.date().month(), EtTime.date().day()
                        , EtTime.time().hour(), EtTime.time().minute(), EtTime.time().second());

		LONG nJnlNum=0;
		dllDSTO.D_GetDataLong(TRUE, "JNLNUM", &nJnlNum);

        QString s;
        s.sprintf("%s, %06d, %s\n", Q2S(strTime), nJnlNum, Q2S(sData));

        fn<<s.toStdString();
		fn.close();
	}
    catch (...)
	{
        ////e->Delete();
		LOGERROR("%s函数捕捉到错误",__FUNCTION__);
		return -1;
	}

	return 0;
}

///////返回值0 代表成功 其他代表失败
int CMTSL::WriteCardRetainFile(QString sData)
{
	std::fstream fn;

	try
	{
		CreateFileDirectory(CNT_CARDRETAINPATH);
		fn.open(CNT_CARDRETAINFILENAME, std::ios::app);
		if(fn.fail())
		{
			LOGERROR("%s创建打开吞卡记录文件失败",__FUNCTION__);
			return -1;
		}
			

        QDateTime EtTime = QDateTime::currentDateTime();
        QString strTime;
        strTime.sprintf("%04d/%02d/%02d %02d:%02d:%02d,", EtTime.date().year(), EtTime.date().month(), EtTime.date().day()
                        , EtTime.time().hour(), EtTime.time().minute(), EtTime.time().second());
        LONG nJnlNum=0;
        dllDSTO.D_GetDataLong(TRUE, "JNLNUM", &nJnlNum);
        QString s;
        s.sprintf("%s, %06d, %s\n", Q2S(strTime), nJnlNum, Q2S(sData));

        fn<<s.toStdString();
        fn.close();
	}
    catch (...)
	{
        ////e->Delete();
		LOGERROR("%s函数捕捉到错误",__FUNCTION__);
		return -1;
	}

	return 0;
}

///////返回值0 代表成功 其他代表失败
int CMTSL::WriteCheckTransFile(QString sData)
{
	std::fstream fn;

	try
	{
        CreateFileDirectory(Q2S(m_strCheckFilePath));
        QString m_newFilename = m_strCheckFilePath+m_strCheckFileName;
        fn.open(Q2S(m_newFilename), std::ios::app);
		if(fn.fail())
		{
			LOGERROR("%s创建打开对账记录文件失败",__FUNCTION__);
			return -1;
		}
			
        QString s;
        s.sprintf("%s\n", Q2S(sData));

        fn<<s.toStdString();
		fn.close();
	}
    catch (...)
	{
        ////e->Delete();
		LOGERROR("%s函数捕捉到错误",__FUNCTION__);
		return -1;
	}

	return 0;
}

///////返回值0 代表成功 其他代表失败
int CMTSL::ReadTransFile()
{
	std::fstream fn;
	char szBuffer[256]={0};

	try
	{
		fn.open(CNT_ACCTFILENAME, std::ios::in);
		if(fn.fail())
		{
			dllDSTO.D_SetDataString(FALSE, "DEPCOUNT", "0");
			dllDSTO.D_SetDataString(FALSE, "CWDCOUNT", "0");
			dllDSTO.D_SetDataString(FALSE, "DEPAMOUNT", "0");
			dllDSTO.D_SetDataString(FALSE, "CWDAMOUNT", "0");

			dllDSTO.D_SetDataString(FALSE, "ALLERRTRANS", "无现金异常交易");
			LOGERROR("%s打开交易记录文件失败",__FUNCTION__);
			return -1;
		}

        QString sTransType, sRetCode, sReason, sTransAmount,sInOutAmount;
        QString sAccountNo, sTime, sJnlnum;
		char cSplit=',';
        QString sAllErrTrans;
		LONG nDepCount=0, nCwdCount=0; //交易笔数
		LONG nDepAmount=0, nCwdAmount=0; //交易总金额
		LONG nTransAmount=0; //当笔交易金额
        QString sTmp, sReasonTmp;
		while(!fn.eof())
		{
			fn.getline(szBuffer,sizeof(szBuffer),'\n');
			if(strlen(szBuffer)<30) //小于一定长度不是交易数据
				continue;
			LOGINFO("szBuffer=%s", szBuffer); //tmp by art
			//每一行数据格式: sTime, sJnlnum, sAccoutNo, sTransType, sTransAmount, sInOutAmount, sRetCode, Reason
            QStringList strlist=S2Q(szBuffer).split(",");
            //::AfxExtractSubString (sTransType, szBuffer, 3, cSplit);
            sTransType = strlist.at(3);
            //::AfxExtractSubString (sTransAmount, szBuffer, 4, cSplit);
            sTransAmount = strlist.at(4);
//			::AfxExtractSubString (sInOutAmount, szBuffer, 5, cSplit);
            sInOutAmount = strlist.at(5);
//			::AfxExtractSubString (sRetCode, szBuffer, 6, cSplit);
            sRetCode = strlist.at(6);
//			::AfxExtractSubString (sReason, szBuffer, 7, cSplit);
            sReason = strlist.at(7);
//			::AfxExtractSubString (sTime, szBuffer, 0, cSplit);
            sTime = strlist.at(0);
//			::AfxExtractSubString (sJnlnum, szBuffer,1, cSplit);
            sJnlnum = strlist.at(1);
//			::AfxExtractSubString (sAccountNo, szBuffer, 2, cSplit);
            sAccountNo = strlist.at(2);

			nTransAmount=0;
			try
			{
                if(sTransAmount.trimmed()!="")
                    nTransAmount=sTransAmount.trimmed().toInt();
			}
            catch (...)
			{				
			}

            if (sReason.trimmed().indexOf("REP") >= 0) // 区别回存交易
			{
				continue;
			}
            else if( sRetCode.trimmed()!="00" || sReason.trimmed()!="") //过滤出异常交易
			{
				if("AT"==sReason)
				{
					sReasonTmp="网络异常";
				}
				else if("SF"==sReason)
				{
					sReasonTmp="入钞卡钞";
				}
				else if("PF"==sReason)
				{
					sReasonTmp="出钞"+sInOutAmount;
				}
				else if("PARTIN"==sReason)
				{
					sReasonTmp="部分上账";
				}
				else if("ATPARTIN"==sReason)
				{
					sReasonTmp="部分上账网络异常";
				}
				else
				{
					sReasonTmp=sReason;
				}
				//格式化各域到固定长度 （如果用"，"注意是全角"，"，防止影响凭条分隔符)
                sTmp.sprintf("%-19s %6s %-19s\n%8s %8s.00 %8s %-16s\n", Q2S(sTime), Q2S(sJnlnum), Q2S(sAccountNo), Q2S(sTransType), Q2S(sTransAmount), Q2S(sRetCode), Q2S(sReasonTmp));
                LOGINFO("sTmp=%s", Q2S(sTmp)); //tmp by art
				sAllErrTrans+=sTmp;
			}

            if("DEP"==sTransType.trimmed())
			{
				nDepCount++;
				nDepAmount+=nTransAmount;
			}
            else if("CWD"==sTransType.trimmed())
			{
				nCwdCount++;
                if("00"==sRetCode.trimmed())
				{
					nCwdAmount+=nTransAmount;
				}
			}
		}

		fn.close();

		LOGINFO("nDepCount=%d nDepAmount=%d nCwdCount=%d nCwdAmount=%d", nDepCount, nDepAmount, nCwdCount, nCwdAmount); //tmp by art

        QString sDepCount, sCwdCount, sDepAmount, sCwdAmount;
        sDepCount.sprintf("%d", nDepCount);
        sCwdCount.sprintf("%d", nCwdCount);
        sDepAmount.sprintf("%d.00", nDepAmount);
        sCwdAmount.sprintf("%d.00", nCwdAmount);
        dllDSTO.D_SetDataString(FALSE, "DEPCOUNT", Q2S(sDepCount));
        dllDSTO.D_SetDataString(FALSE, "CWDCOUNT", Q2S(sCwdCount));
        dllDSTO.D_SetDataString(FALSE, "DEPAMOUNT", Q2S(sDepAmount));
        dllDSTO.D_SetDataString(FALSE, "CWDAMOUNT", Q2S(sCwdAmount));

		if(sAllErrTrans!="")
		{
            QString sErrTitle;
            sErrTitle.sprintf("%-19s %6s %-19s\n%8s %11s %8s %-16s\n","日期时间","流水号", "账号", "交易类型", "交易金额", "响应码","备注");
			sAllErrTrans=sErrTitle+sAllErrTrans;
		}
		else
		{
			sAllErrTrans="无现金异常交易";
		}
        dllDSTO.D_SetDataString(FALSE, "ALLERRTRANS", Q2S(sAllErrTrans));

	}
    catch (...)
	{
        ////e->Delete();
		LOGERROR("%s函数捕捉到错误",__FUNCTION__);
		return -1;
	}

	return 0;
}


///////返回值0 代表成功 其他代表失败
int CMTSL::ReadRetainCardFile()
{
	std::fstream fn;
	char szBuffer[256]={0};

	try
	{
		fn.open(CNT_CARDRETAINFILENAME, std::ios::in);
		if(fn.fail())
		{
			dllDSTO.D_SetDataString(FALSE, "CARDRETAINRECORD", "无吞卡记录");
			LOGERROR("%s打开吞卡记录文件失败",__FUNCTION__);
			return -1;
		}

        QString sAccountNo, sTime, sReason;
		char cSplit=',';
        QString sAll;
        QString sTmp;
		while(!fn.eof())
		{
			fn.getline(szBuffer,sizeof(szBuffer),'\n');
			if(strlen(szBuffer)<30) //小于一定长度不是交易数据
				continue;
			LOGINFO("szBuffer=%s", szBuffer); //tmp by art
			//每一行数据格式: sTime, sAccoutNo, Reason
            QStringList strlist=S2Q(szBuffer).split(",");
            //::AfxExtractSubString (sTransType, szBuffer, 3, cSplit);
            sTime = strlist.at(0);
            //::AfxExtractSubString (sTime, szBuffer, 0, cSplit);
            sAccountNo = strlist.at(1);
            //::AfxExtractSubString (sAccountNo, szBuffer, 1, cSplit);
            sReason = strlist.at(2);
            //::AfxExtractSubString (sReason, szBuffer, 2, cSplit);

			//格式化各域到固定长度 （注意是全角"，"，防止影响凭条分隔符
            //sTmp.sprintf("%-19s %-19s %-8s\n", sTime, sAccountNo, sReason);
            sTmp.sprintf("%-19s %-19s\n", Q2S(sTime), Q2S(sAccountNo));
			//WriteLog("sTmp=%s", sTmp); //tmp by art
			sAll+=sTmp;
		}
		fn.close();

		if(sAll!="")
		{
            QString sTitle;
            //sTitle.sprintf("%-19s %-19s %-8s\n","日期时间","卡号", "备注");
            sTitle.sprintf("%-19s %-19s\n","日期时间","卡号");
			sAll=sTitle+sAll;
		}
		else
		{
			sAll="无吞卡记录";
		}
        dllDSTO.D_SetDataString(FALSE, "CARDRETAINRECORD", Q2S(sAll));

	}
    catch (...)
	{
        //e->Delete();
		LOGERROR("%s函数捕捉到错误",__FUNCTION__);
		return -1;
	}

	return 0;
}

///////返回值0 代表成功 其他代表失败
int CMTSL::RenameTransFile()
{
    if(! QDir(CNT_ACCTFILENAME).exists())
	{
		LOGERROR("%s文件不存在",CNT_ACCTFILENAME);
		return -1;
	}

    QDateTime EtTime = QDateTime::currentDateTime();
    QString sTime;
    QDir m_dir;
    sTime.sprintf("%04d/%02d/%02d %02d:%02d:%02d,", EtTime.date().year(), EtTime.date().month(), EtTime.date().day()
                    , EtTime.time().hour(), EtTime.time().minute(), EtTime.time().second());
    QString sOldfn=CNT_ACCTFILENAME;
    QString sNewfn=sOldfn.left(sOldfn.length()-4)+sTime+sOldfn.right(4); //如为"Acct.txt",则改名为类似 "Acct20161018091033.txt"
    if(!m_dir.rename(sOldfn,sNewfn))
	{
        LOGERROR("Fail to rename %s to %s ",Q2S(sOldfn), Q2S(sNewfn));
		return -1;
	}

	return 0;
}

///////返回值0 代表成功 其他代表失败
int CMTSL::ReadTransReDEPFile()
{
	std::fstream fn;
	char szBuffer[256]={0};

	try
	{
		fn.open(CNT_ACCTFILENAME, std::ios::in);
		if (fn.fail())
		{
			dllDSTO.D_SetDataString(FALSE, "REDEPAMOUNT", "无回存记录");
			LOGERROR("%s打开交易记录文件失败",__FUNCTION__);
			return -1;
		}

        QString sTransType, sRetCode, sReason, sTransAmount,sInOutAmount;
        QString sAccountNo, sTime, sJnlnum;
		char cSplit=',';
        QString sReDepTrans;
		LONG nReDepCount=0;  //回存交易笔数
		LONG nReDepAmount=0; //回存交易总金额
		LONG nTransAmount=0; //当笔交易金额
        QString sTmp, sReasonTmp;
		while(!fn.eof())
		{
			fn.getline(szBuffer,sizeof(szBuffer),'\n');
			if(strlen(szBuffer)<30) //小于一定长度不是交易数据
				continue;
			//LOGINFO("szBuffer=%s", szBuffer); //tmp by art
			//每一行数据格式: sTime, sJnlnum, sAccoutNo, sTransType, sTransAmount, sInOutAmount, sRetCode, Reason
            QStringList strlist=S2Q(szBuffer).split(",");
            //::AfxExtractSubString (sTransType, szBuffer, 3, cSplit);
            sTransType = strlist.at(3);
            //::AfxExtractSubString (sTransAmount, szBuffer, 4, cSplit);
            sTransAmount = strlist.at(4);
//			::AfxExtractSubString (sInOutAmount, szBuffer, 5, cSplit);
            sInOutAmount = strlist.at(5);
//			::AfxExtractSubString (sRetCode, szBuffer, 6, cSplit);
            sRetCode = strlist.at(6);
//			::AfxExtractSubString (sReason, szBuffer, 7, cSplit);
            sReason = strlist.at(7);
//			::AfxExtractSubString (sTime, szBuffer, 0, cSplit);
            sTime = strlist.at(0);
//			::AfxExtractSubString (sJnlnum, szBuffer,1, cSplit);
            sJnlnum = strlist.at(1);
//			::AfxExtractSubString (sAccountNo, szBuffer, 2, cSplit);
            sAccountNo = strlist.at(2);
//			::AfxExtractSubString (sTime, szBuffer, 0, cSplit);
//			::AfxExtractSubString (sJnlnum, szBuffer,1, cSplit);
//			::AfxExtractSubString (sAccountNo, szBuffer, 2, cSplit);
//			::AfxExtractSubString (sTransType, szBuffer, 3, cSplit);
//			::AfxExtractSubString (sTransAmount, szBuffer, 4, cSplit);
//			::AfxExtractSubString (sInOutAmount, szBuffer, 5, cSplit);
//			::AfxExtractSubString (sRetCode, szBuffer, 6, cSplit);
//			::AfxExtractSubString (sReason, szBuffer, 7, cSplit);

			nTransAmount=0;
			try
			{
                if(sTransAmount.trimmed()!="")
				{
                    nTransAmount=sTransAmount.trimmed().toInt();
				}
			}
            catch (...)
			{				
			}

			// 确认交易类型是否为回存交易
            if(sReason.trimmed().indexOf("REP") >= 0)
			{
                if (sRetCode.trimmed() == "00")
				{
					sReason="回存成功";
				}
				else
				{
					sReason="回存失败";
				}
				sReasonTmp = sReason;
				//格式化各域到固定长度 （如果用"，"注意是全角"，"，防止影响凭条分隔符)
                sTmp.sprintf("%-19s %6s %-19s\n%8s %8s.00 %8s %-16s\n", Q2S(sTime), Q2S(sJnlnum), Q2S(sAccountNo), Q2S(sTransType), Q2S(sTransAmount), Q2S(sRetCode), Q2S(sReasonTmp));
                LOGINFO("sTmp=%s", Q2S(sTmp)); //tmp by art
				sReDepTrans+=sTmp;

				nReDepCount++;
				nReDepCount+=nTransAmount;
			}
		}
        LOGINFO("sReDepTrans:%s", Q2S(sReDepTrans));
		fn.close();

		LOGINFO("nReDepCount=%d nReDepAmount=%d", nReDepCount, nReDepAmount); 

        QString sReDepCount, sReDepAmount;
        sReDepCount.sprintf("%d", nReDepCount);
        sReDepAmount.sprintf("%d.00", nReDepAmount);

		if(sReDepTrans != "")
		{
            QString sErrTitle;
            sErrTitle.sprintf("%-19s %6s %-19s\n%8s %11s %8s %-16s\n","日期时间","流水号", "账号", "交易类型", "交易金额", "响应码","备注");
			sReDepTrans=sErrTitle+sReDepTrans;
		}
		else
		{
			sReDepTrans="无回存现金交易";
		}
        dllDSTO.D_SetDataString(FALSE, "REDEPAMOUNT", Q2S(sReDepTrans));

	}
    catch (...)
	{
        //e->Delete();
		LOGERROR("%s函数捕捉到错误",__FUNCTION__);
		return -1;
	}

	return 0;
}

int CMTSL::ReadCloseAccountFile()
{
	std::fstream fn;
	char szBuffer[256]={0};

	try
	{
		fn.open(CNT_CLOSEACCOUNTNAME, std::ios::in);
		if (fn.fail())
		{
			dllDSTO.D_SetDataString(FALSE, "ALLCLOSETRANS", "无销户记录");
			LOGERROR("%s打开销户记录文件失败",__FUNCTION__);
			return -1;
		}

        QString sTime, sJnlnum, sAccoutNo, sTransNo ,sTransAmount, sRetCode, sReason;
        QString sTmp, sReasonTmp;
        QString sAllCloseTrans;
		LONG nTransAmount=0; //当笔转账金额
		char cSplit=',';

		while(!fn.eof())
		{
			fn.getline(szBuffer,sizeof(szBuffer),'\n');
			if(strlen(szBuffer)<30) //小于一定长度不是交易数据
				continue;
			// 每一行数据可是: 时间,  流水号, 销户号,    转入号,    转入金额,     结果,     备注
			// 每一行数据格式: sTime, sJnlnum, sAccoutNo ,sTransNo, sTransAmount, sRetCode, sReason
            QStringList strlist=S2Q(szBuffer).split(",");
            //::AfxExtractSubString (sTransType, szBuffer, 3, cSplit);
            sTime = strlist.at(0);
            //::AfxExtractSubString (sTime, szBuffer, 0, cSplit);
            sJnlnum = strlist.at(1);
            //::AfxExtractSubString (sJnlnum, szBuffer,1, cSplit);
            sAccoutNo = strlist.at(2);
            //::AfxExtractSubString (sAccoutNo, szBuffer, 2, cSplit);
            sTransNo = strlist.at(3);
            //::AfxExtractSubString (sTransNo, szBuffer, 3, cSplit);
            sTransAmount = strlist.at(4);
            //::AfxExtractSubString (sTransAmount, szBuffer, 4, cSplit);
            sRetCode = strlist.at(5);
            //::AfxExtractSubString (sRetCode, szBuffer, 5, cSplit);
            sReason = strlist.at(6);
            //::AfxExtractSubString (sReason, szBuffer, 6, cSplit);

			nTransAmount=0;
			try
			{
                if(sTransAmount.trimmed()!="")
				{
                    nTransAmount=sTransAmount.trimmed().toInt();
				}
			}
            catch (...)
			{				
			}

            if( sRetCode.trimmed()!="00" || sReason.trimmed()!="") //过滤出异常交易
			{
				if("AT"==sReason)
				{
					sReasonTmp="网络异常";
				}
				else
				{
					sReasonTmp=sReason;
				}
			}
			//格式化各域到固定长度 （如果用"，"注意是全角"，"，防止影响凭条分隔符)
            //sTmp.sprintf("%-19s %6s\n[%19s]→[%19s]\n%8s %8s %-16s\n", sTime, sJnlnum, sAccoutNo, sTransNo, sTransAmount, sRetCode, sReasonTmp);
            sTmp.sprintf("日期时间  %s\n流水号   %6s\n销户账号 %19s\n转入账号 %19s\n转入金额  %8s\n响应码 %8s %s\n------------------------------\n", Q2S(sTime), Q2S(sJnlnum), Q2S(sAccoutNo), Q2S(sTransNo), Q2S(sTransAmount), Q2S(sRetCode), Q2S(sReasonTmp));
            LOGINFO("sTmp=%s", Q2S(sTmp)); //tmp by art
			sAllCloseTrans+=sTmp;
		}

		fn.close();

		if(sAllCloseTrans!="")
		{
            /*QString sErrTitle;
            sErrTitle.sprintf("%-19s %6s\n%-19s %-19s\n%11s %8s %-16s\n","日期时间","流水号", "销户账号", "转入账户", "转入金额", "响应码","备注");
			sAllCloseTrans=sErrTitle+sAllCloseTrans;*/
		}
		else
		{
			sAllCloseTrans="无销户记录";
		}
        dllDSTO.D_SetDataString(FALSE, "ALLCLOSETRANS", Q2S(sAllCloseTrans));

	}
    catch (...)
	{
        //e->Delete();
		LOGERROR("%s函数捕捉到错误",__FUNCTION__);
		return -1;
	}

	return 0;
}

///////返回值0 代表成功 其他代表失败
int CMTSL::WriteCloseAccountFile(QString sData)
{
	std::fstream fn;

	try
	{
		CreateFileDirectory(CNT_CLOSEACCOUNTPATH);
		fn.open(CNT_CLOSEACCOUNTNAME, std::ios::app);
		if(fn.fail())
		{
			LOGERROR("%s创建销户记录文件失败",__FUNCTION__);
			return -1;
		}
        QDateTime EtTime = QDateTime::currentDateTime();
        QString sTime;
        sTime.sprintf("%04d/%02d/%02d %02d:%02d:%02d,", EtTime.date().year(), EtTime.date().month(), EtTime.date().day()
                        , EtTime.time().hour(), EtTime.time().minute(), EtTime.time().second());
		LONG nJnlNum=0;
		dllDSTO.D_GetDataLong(TRUE, "JNLNUM", &nJnlNum);

        QString s;
        s.sprintf("%s, %06d, %s\n", Q2S(sTime), nJnlNum, Q2S(sData));

        fn<<s.toStdString();
		fn.close();
	}
    catch (...)
	{
        //e->Delete();
		LOGERROR("%s函数捕捉到错误",__FUNCTION__);
		return -1;
	}
	return 0;
}

///////返回值0 代表成功 其他代表失败
int CMTSL::RenameCloseAccountFile()
{
    if(!QDir(CNT_CLOSEACCOUNTNAME).exists())
	{
		LOGERROR("%s文件不存在",CNT_CLOSEACCOUNTNAME);
		return -1;
	}

    QDateTime EtTime = QDateTime::currentDateTime();
    QString sTime;
    QDir dir;
    sTime.sprintf("%04d/%02d/%02d %02d:%02d:%02d,", EtTime.date().year(), EtTime.date().month(), EtTime.date().day()
                    , EtTime.time().hour(), EtTime.time().minute(), EtTime.time().second());

    QString sOldfn=CNT_CLOSEACCOUNTNAME;
    QString sNewfn=sOldfn.left(sOldfn.length()-4)+sTime+sOldfn.right(4);
    if(!dir.rename(sOldfn, sNewfn))
	{
        LOGERROR("Fail to rename %s to %s ",Q2S(sOldfn), Q2S(sNewfn));
		return -1;
	}

	return 0;
}

///////返回值0 代表成功 其他代表失败
int CMTSL::RenameCardRetainFile()
{
    if(!QDir(CNT_CARDRETAINFILENAME).exists())
	{
		LOGERROR("%s文件不存在",CNT_CARDRETAINFILENAME);
		return -1;
	}

    QDateTime EtTime = QDateTime::currentDateTime();
    QString sTime;
    QDir dir;
    sTime.sprintf("%04d/%02d/%02d %02d:%02d:%02d,", EtTime.date().year(), EtTime.date().month(), EtTime.date().day()
                    , EtTime.time().hour(), EtTime.time().minute(), EtTime.time().second());
    QString sOldfn=CNT_CARDRETAINFILENAME;
    QString sNewfn=sOldfn.left(sOldfn.length()-4)+sTime+sOldfn.right(4);
    if(!dir.rename(sOldfn, sNewfn))
    {
        LOGERROR("Fail to rename %s to %s ",Q2S(sOldfn), Q2S(sNewfn));
        return -1;
    }

	return 0;
}

/////////////////////返回值0 代表成功 其他代表失败
//////HandleRecordFileSync 函数说明
//   WriteTransRecord=0,//////写交易记录
//   WriteRetainCardRecord,//////写吞卡记录
//	//WriteCheckTransRecord,/////写对账记录
//	//ReadTransRecord,//////读交易记录
//	//ReadRetainCardRecord,////读吞卡记录
//	//RenameRecord////////重命名交易和吞卡记录
////////////////////////////////////////////
LONG CMTSL::HandleRecordFileSync(long ID,QString strTransData)
{
	int ret=0;
    //EnterCriticalSection(&HandleRecordFile_critical);
    m_oMutex.lock();
	if ( ID  == WriteTransRecord)
	{
        if(strTransData.isEmpty() || strTransData == "")
		{
			LOGERROR("%s函数传入参数有误,记录数据为空",__FUNCTION__);
			return -1;
		}
		ret=WriteTransFile(strTransData);
        m_oMutex.unlock();
        //LeaveCriticalSection(&HandleRecordFile_critical);
	}
	else if ( ID  == WriteRetainCardRecord)
	{
        if(strTransData.isEmpty() || strTransData == "")
		{
			LOGERROR("%s函数传入参数有误，记录数据为空",__FUNCTION__);
			return -1;
		}
		ret=WriteCardRetainFile(strTransData);
        m_oMutex.unlock();
        //LeaveCriticalSection(&HandleRecordFile_critical);
	}
	else if ( ID == WriteCheckTransRecord)
	{
        if(strTransData.isEmpty() || strTransData == "")
		{
			LOGERROR("%s函数传入参数有误，记录数据为空",__FUNCTION__);
			return -1;
		}
		ret=WriteCheckTransFile(strTransData);
        m_oMutex.unlock();
        //LeaveCriticalSection(&HandleRecordFile_critical);
	}
	else if ( ID == ReadTransAndRetainCardRecord)
	{
		ret=ReadTransFile();
		ret=ReadRetainCardFile();
		ret=ReadTransReDEPFile();
		ret = ReadCloseAccountFile();
        //LeaveCriticalSection(&HandleRecordFile_critical);
        m_oMutex.unlock();
	}
	else if ( ID == RenameRecord)
	{
		ret=RenameTransFile();
		ret=RenameCardRetainFile();
		ret=RenameCloseAccountFile();
        //LeaveCriticalSection(&HandleRecordFile_critical);
        m_oMutex.unlock();
	}
	else if ( ID == WriteCloseAccount)
	{
		ret=WriteCloseAccountFile(strTransData);
        //LeaveCriticalSection(&HandleRecordFile_critical);
        m_oMutex.unlock();
	}
	else
	{
		LOGERROR("调用%s函数，传入参数有误没有对应的记录文件操作",__FUNCTION__);
		return -1;
	}

	LOGINFO("%s函数,返回值为：%d",__FUNCTION__,ret);
	return ret;
}

//////更新交易记录 传的数据一条数据
SHORT CMTSL::UpdateRecord(QString Data)
{
    QDateTime EtTime = QDateTime::currentDateTime();
    QString sTime;
    sTime.sprintf("%04d/%02d/%02d %02d:%02d:%02d,", EtTime.date().year(), EtTime.date().month(), EtTime.date().day()
                    , EtTime.time().hour(), EtTime.time().minute(), EtTime.time().second());
	LONG nJnlNum=0;
	dllDSTO.D_GetDataLong(TRUE, "JNLNUM", &nJnlNum);

    m_UpdateRecordData.clear();
    m_UpdateRecordData.sprintf("%s, %06d, %s\n", Q2S(sTime), nJnlNum, Q2S(Data));

    //SetEvent(m_hUpdateRecordEvent);//异步调用

	LOGINFO("%s,OUT",__FUNCTION__);

	return 0;
}

SHORT CMTSL::UpdateCheckRecord(QString Data)
{

	m_UpdateCheckRecordData=Data;
    //SetEvent(m_hUpdateCheckEvent);//异步调用

	LOGINFO("%s,OUT",__FUNCTION__);

	return 0;
}

//LONG CMTSL::AddTransLogSync(QString strTransLog)
//{
//    LOGINFO("[AddTransLog]--------------------------[start]");

//	int ret = -1;
//    //"日期|时间|卡号|流水号|交易类型|金额|身份证号|身份证头像路径|交易结果"
//    VerStr vs = Token_All(strTransLog, '|');

//	if (!m_bOpened)////数据库没有打开
//	{
//		LOGERROR("%s函数失败，数据库没有打开", __FUNCTION__);
//		return -1;
//	}

//    if(9 != vs.size())
//    {
//        LOGERROR("%s, input param error,  input = %s", __FUNCTION__, strTransLog);
//    }
//    else
//    {
        
//        char * lpErrMsg = NULL;
//        QString strSql("");
//        double dMoney = atof(vs[5]);

//        strSql.sprintf("insert into %s (Date, Time, CardNo, JnlNum, Type, Money, IDNo, ImageHeadPath, Result) \
//                      values ('%s', '%s', '%s', '%s', '%s', %.2f, '%s', '%s', '%s')", \
//                      DATABASE_TABLE_NAME, vs[0], vs[1], vs[2], vs[3], vs[4], dMoney, vs[6], vs[7], vs[8]);
//        LOGINFO("[SQL] [%s]", strSql);

//        ret = sqlite3_exec(m_lpDB, strSql, NULL, NULL, &lpErrMsg);

//        if(SQLITE_OK != ret)
//        {
//            LOGERROR("insert data fail. error code : %d, error describe : %s", ret, lpErrMsg);
//        }

//    }

//    LOGINFO("[AddTransLog]--------------------------[end],返回值为：%d",ret);
//	return ret;
//}

//SHORT CMTSL::UpdateTransLogSync(QString strTransLog)
//{
//	LOGINFO("[UpdateTransLogSync]--------------------------[start]");

//	int ret = -1;
//	//"日期|时间|卡号|流水号|交易类型|金额|身份证号|身份证头像路径|交易结果"
//	VerStr vs = Token_All(strTransLog, '|');

//	if (!m_bOpened)////数据库没有打开
//	{
//		LOGERROR("%s函数失败，数据库没有打开", __FUNCTION__);
//		return -1;
//	}

//	if(9 != vs.size())
//	{
//		LOGERROR("%s, input param error,  input = %s", __FUNCTION__, strTransLog);
//	}
//	else
//	{
//		char * lpErrMsg = NULL;
//        QString strSql("");
//        double dMoney = atof(vs[5]);
//		//////先删掉最后一条记录
//		strSql="delete from TransLog where ID=(select max(ID) from TransLog)";
//        ret = sqlite3_exec(m_lpDB, strSql, NULL, NULL, &lpErrMsg);

//		if(SQLITE_OK != ret)
//		{
//			LOGERROR("insert data fail. error code : %d, error describe : %s", ret, lpErrMsg);
//		}
//		else
//		{
//			//////再插入更正的记录
//            strSql.clear();
//            strSql.sprintf("insert into %s (Date, Time, CardNo, JnlNum, Type, Money, IDNo, ImageHeadPath, Result) \
//						  values ('%s', '%s', '%s', '%s', '%s', %.2f, '%s', '%s', '%s')", \
//						  DATABASE_TABLE_NAME, vs[0], vs[1], vs[2], vs[3], vs[4], dMoney, vs[6], vs[7], vs[8]);
//			LOGINFO("[SQL] [%s]", strSql);

//            ret = sqlite3_exec(m_lpDB, strSql, NULL, NULL, &lpErrMsg);

//			if(SQLITE_OK != ret)
//			{
//				LOGERROR("insert data fail. error code : %d, error describe : %s", ret, lpErrMsg);
//			}
//		}

		

//	}

//	LOGINFO("[UpdateTransLogSync]--------------------------[end],返回值为：%d",ret);
//	return ret;
//}

//LONG CMTSL::CountTypeSync(QString strStartDate, QString strEndDate, QString strType)
//{
//    LOGINFO("[CountTypeSync]--------------------------[start]");

//    g_lCount = 0;

//    LONG rst = 0;

//	if (!m_bOpened)////数据库没有打开
//	{
//		LOGERROR("%s函数失败，数据库没有打开", __FUNCTION__);
//		return 0;
//	}

//    if("" == strStartDate || "" == strEndDate || "" == strType)
//    {
//        LOGERROR("%s, input param error.[Event] FatalError(). \
//                        strStartDate = %s, strEndDate = %s, strType = %s", \
//                        __FUNCTION__, strStartDate, strEndDate, strType);
//    }
//    else
//    {
//        int ret = -1;
//        char * lpErrMsg = NULL;
//        QString strSql("");
//        if("*" == strType)
//        {
//            strSql.sprintf("select count(*) num from %s where Date Between '%s' and '%s'", \
//                DATABASE_TABLE_NAME, strStartDate, strEndDate);
//        }
//        else
//        {
//            strSql.sprintf("select count(*) num from %s where Type = '%s' and Date Between '%s' and '%s'", \
//                DATABASE_TABLE_NAME, strType, strStartDate, strEndDate);
//        }
//        LOGINFO("[SQL] [%s]", strSql);

//        ret = sqlite3_exec(m_lpDB, strSql, CallBack_CountType, NULL, &lpErrMsg);

//        if(SQLITE_OK != ret)
//        {
            
//            LOGERROR("select count(%s) fail. [Event] FatalError(). error code : %d, error describe : %s", strType, ret, lpErrMsg);
//        }
//        else
//        {
//            rst = g_lCount;
//            LOGINFO("[success] [count = %d]", rst);
//        }
//    }

//    LOGINFO("[CountTypeSync]--------------------------[end]");

//    return rst;
//}

//QString CMTSL::SumTypeMoneySync(QString strStartDate, QString strEndDate, QString strType)
//{
//    LOGINFO("[SumTypeMoneySync]--------------------------[start]");

//    g_Money = 0;

//    double money = 0;
//    QString strMoney("");

//	if (!m_bOpened)////数据库没有打开
//	{
//		LOGERROR("%s函数失败，数据库没有打开", __FUNCTION__);
//		return "";
//	}

//    if("" == strStartDate || "" == strEndDate || "" == strType)
//    {
        
//        LOGERROR("%s, input param error.[Event] FatalError(). \
//                        strStartDate = %s, strEndDate = %s, strType = %s", \
//                        __FUNCTION__, strStartDate, strEndDate, strType);
//    }
//    else
//    {
//        int ret = -1;
//        char * lpErrMsg = NULL;
//        QString strSql("");
//        strSql.sprintf("select sum(Money) sum_money from %s where Type = '%s' and Date Between '%s' and '%s'", \
//            DATABASE_TABLE_NAME, strType, strStartDate, strEndDate);
//        LOGINFO("[SQL] [%s]", strSql);

//        ret = sqlite3_exec(m_lpDB, strSql, CallBack_SumMoney, NULL, &lpErrMsg);

//        if(SQLITE_OK != ret)
//        {
            
//            LOGERROR("select sum(%s) fail. [Event] FatalError(). error code : %d, error describe : %s", strType, ret, lpErrMsg);
//        }
//        else
//        {
//            money = g_Money;
//            strMoney.sprintf("%.2f", money);
//            LOGINFO("[success] [money = %s]", strMoney);
//        }
//    }
//    LOGINFO("[SumTypeMoneySync]--------------------------[end]");

//    return strMoney;
//}



//////////返回每一项值为 笔数：金额：失败笔数
//////////总共八项为银行卡小额取款，银行卡大额取款，存折取款，银行卡小额存款，银行卡大额存款，无卡无折小额存款，无卡无折大额存款，对公业务存款
//QString CMTSL::SumDataSync(QString strStartDate, QString strEndDate)
//{
//	LOGINFO("[SumDataSync]--------------------------[start]");

//	if (!m_bOpened)////数据库没有打开
//	{
//		LOGERROR("%s函数失败，数据库没有打开", __FUNCTION__);
//		return "";
//	}

//	if("" == strStartDate || "" == strEndDate )
//	{

//		LOGERROR("%s, input param error.[Event] FatalError().strStartDate = %s, strEndDate = %s",__FUNCTION__, strStartDate, strEndDate);
//	}
//	else
//	{
//		g_i=0;
//        gSumData.clear();
//        gRowDataTmp.clear();
//		int ret = -1;
//		char * lpErrMsg = NULL;
//		int i=0;
//		for (i=0;i<39;i++)//24
//		{
//            QString strSql("");
//            QString strType("");
//			if (0 <= i && i < 3)
//			{
//				strType="SCWD";
//			}
//			else if (3 <= i && i < 6)
//			{
//				strType="BCWD";
//			}
//			else if (6 <= i && i < 9)
//			{
//				strType="BOOKCWD";
//			}
//			else if (9 <= i && i < 12)
//			{
//				strType="SDEP";
//			}
//			else if (12 <= i && i < 15)
//			{
//				strType="BDEP";
//			}
//			else if (15 <= i && i < 18)
//			{
//				strType="SNOCARDDEP";
//			}
//			else if (18 <= i && i < 21)
//			{
//				strType="BNOCARDDEP";
//			}
//			else if (21<= i && i < 24)
//			{
//				strType="BUSINESSDEP";
//			}
//			else if (24<= i && i < 27)
//			{
//				strType="WXDELETE";//微信销户
//			}
//			else if (27<= i && i < 30)
//			{
//				strType="TFDELETE";//转账销户
//			}
//			else if (30<= i && i < 33)
//			{
//				strType="InLineFlag";//行内转账
//			}
//			else if (33<= i && i < 36)
//			{
//				strType="OutLineFlag";//行外转账
//			}
//			else if (36<= i && i < 39)
//			{
//				strType="TRANSFERCANCEL";//转账撤销
//			}

//			if (i%3 == 0)
//			{

//                strSql.sprintf("select count(*) num from %s where Type = '%s' and Date Between '%s' and '%s'", \
//                    DATABASE_TABLE_NAME,strType,strStartDate,strEndDate);
//			}
//			else if (i%3 == 2 )
//			{
//                strSql.sprintf("select count(*) num from %s where Type =  '%s' and Result = 'FAIL' and Date Between '%s' and '%s'", \
//                    DATABASE_TABLE_NAME,strType,strStartDate,strEndDate);
//			}
//			else
//			{
//				if (i<24)
//				{
//                    strSql.sprintf("select sum(Money) sum_money from %s where Type =  '%s' and Date Between '%s' and '%s'", \
//                        DATABASE_TABLE_NAME,strType,strStartDate,strEndDate);
//				}
//				else
//				{
//                    strSql.sprintf("select sum(Money) sum_money from %s where Type =  '%s' and Result = 'SUCCESS' and Date Between '%s' and '%s'", \
//                        DATABASE_TABLE_NAME,strType,strStartDate,strEndDate);
//				}
				
//			}

//			//LOGINFO("[SQL] [%s]", strSql);

//            ret = sqlite3_exec(m_lpDB, strSql, CallBack_SumData, NULL, &lpErrMsg);
//			if(SQLITE_OK != ret)
//			{

//				LOGERROR("select fail. [Event] FatalError(). error code : %d, error describe : %s", ret, lpErrMsg);
//			}

//			int j=0;
//			if (8 == i || 23 == i || 29 == i || 35 == i || 38 == i)
//			{
//				for (j=0;j<3;j++)
//				{
//					strSql = "";
//					if (j == 0)
//					{
//						switch(i)
//						{
//						case 8:
//							{
//                                strSql.sprintf("select count(*) num from %s where Type IN ( 'SCWD', 'BCWD', 'BOOKCWD' ) and Date Between '%s' and '%s'", \
//                                    DATABASE_TABLE_NAME,strStartDate,strEndDate);
//								break;
//							}
//						case 23:
//							{
//                                strSql.sprintf("select count(*) num from %s where Type IN ( 'SDEP', 'BDEP', 'SNOCARDDEP', 'BNOCARDDEP', 'BUSINESSDEP' ) and Date Between '%s' and '%s'", \
//                                    DATABASE_TABLE_NAME,strStartDate,strEndDate);
//								break;
//							}
//						case 29:
//							{
//                                strSql.sprintf("select count(*) num from %s where Type IN ( 'WXDELETE', 'TFDELETE' ) and Date Between '%s' and '%s'", \
//                                    DATABASE_TABLE_NAME,strStartDate,strEndDate);
//								break;
//							}
//						case 35:
//							{
//                                strSql.sprintf("select count(*) num from %s where Type IN ( 'InLineFlag', 'OutLineFlag' ) and Date Between '%s' and '%s'", \
//                                    DATABASE_TABLE_NAME,strStartDate,strEndDate);
//								break;
//							}
//						case 38:
//							{
//                                strSql.sprintf("select count(*) num from %s where Type IN ( 'SCWD', 'BCWD', 'BOOKCWD', 'SDEP', 'BDEP', 'SNOCARDDEP', 'BNOCARDDEP', 'BUSINESSDEP', 'WXDELETE', 'TFDELETE', 'InLineFlag', 'OutLineFlag', 'TRANSFERCANCEL' ) and Date Between '%s' and '%s'", \
//                                    DATABASE_TABLE_NAME,strStartDate,strEndDate);
//								break;
//							}
//						default:
//							{
//								break;
//							}
//						}
						
//					}
//					else if (j == 1)
//					{
//						switch(i)
//						{
//						case 8:
//							{
//                                strSql.sprintf("select sum(Money) sum_money from %s where Type IN ( 'SCWD', 'BCWD', 'BOOKCWD' ) and Date Between '%s' and '%s'", \
//                                    DATABASE_TABLE_NAME,strStartDate,strEndDate);
//								break;
//							}
//						case 23:
//							{
//                                strSql.sprintf("select sum(Money) sum_money from %s where Type IN ( 'SDEP', 'BDEP', 'SNOCARDDEP', 'BNOCARDDEP', 'BUSINESSDEP' ) and Date Between '%s' and '%s'", \
//                                    DATABASE_TABLE_NAME,strStartDate,strEndDate);
//								break;
//							}
//						case 29:
//							{
//                                strSql.sprintf("select sum(Money) sum_money from %s where Type IN ( 'WXDELETE', 'TFDELETE' ) and Result = 'SUCCESS' and Date Between '%s' and '%s'", \
//                                    DATABASE_TABLE_NAME,strStartDate,strEndDate);
//								break;
//							}
//						case 35:
//							{
//                                strSql.sprintf("select sum(Money) sum_money from %s where Type IN ( 'InLineFlag', 'OutLineFlag' ) and Result = 'SUCCESS' and Date Between '%s' and '%s'", \
//                                    DATABASE_TABLE_NAME,strStartDate,strEndDate);
//								break;
//							}
//						case 38:
//							{
//                                QString strSqlTemp("");
//                                strSql.sprintf("select sum(Money) sum_money from %s where Type IN ( 'SCWD', 'BCWD', 'BOOKCWD', 'SDEP', 'BDEP', 'SNOCARDDEP', 'BNOCARDDEP', 'BUSINESSDEP' ) and Date Between '%s' and '%s'", \
//                                    DATABASE_TABLE_NAME,strStartDate,strEndDate);
//                                strSqlTemp.sprintf("select sum(Money) sum_money from %s where Type IN ( 'WXDELETE', 'TFDELETE', 'InLineFlag', 'OutLineFlag', 'TRANSFERCANCEL' ) and Result = 'SUCCESS' and Date Between '%s' and '%s'", \
//                                    DATABASE_TABLE_NAME,strStartDate,strEndDate);
//                                strSql.sprintf("select (( %s) + ( %s))", strSql, strSqlTemp);
								
//								break;
//							}
//						default:
//							{
//								break;
//							}
//						}
//					}
//					else
//					{
//						switch(i)
//						{
//						case 8:
//							{
//                                strSql.sprintf("select count(*) num from %s where Type IN ( 'SCWD', 'BCWD', 'BOOKCWD' ) and Result = 'FAIL' and Date Between '%s' and '%s'", \
//                                    DATABASE_TABLE_NAME,strStartDate,strEndDate);
//								break;
//							}
//						case 23:
//							{
//                                strSql.sprintf("select count(*) num from %s where Type IN ( 'SDEP', 'BDEP', 'SNOCARDDEP', 'BNOCARDDEP', 'BUSINESSDEP' ) and Result = 'FAIL' and Date Between '%s' and '%s'", \
//                                    DATABASE_TABLE_NAME,strStartDate,strEndDate);
//								break;
//							}
//						case 29:
//							{
//                                strSql.sprintf("select count(*) num from %s where Type IN ( 'WXDELETE', 'TFDELETE' ) and Result = 'FAIL' and Date Between '%s' and '%s'", \
//                                    DATABASE_TABLE_NAME,strStartDate,strEndDate);
//								break;
//							}
//						case 35:
//							{
//                                strSql.sprintf("select count(*) num from %s where Type IN ( 'InLineFlag', 'OutLineFlag' ) and Result = 'FAIL' and Date Between '%s' and '%s'", \
//                                    DATABASE_TABLE_NAME,strStartDate,strEndDate);
//								break;
//							}
//						case 38:
//							{
//                                strSql.sprintf("select count(*) num from %s where Type IN ( 'SCWD', 'BCWD', 'BOOKCWD', 'SDEP', 'BDEP', 'SNOCARDDEP', 'BNOCARDDEP', 'BUSINESSDEP', 'WXDELETE', 'TFDELETE', 'InLineFlag', 'OutLineFlag', 'TRANSFERCANCEL' ) and Result = 'FAIL' and Date Between '%s' and '%s'", \
//                                    DATABASE_TABLE_NAME,strStartDate,strEndDate);
//								break;
//							}
//						default:
//							{
//								break;
//							}
//						}
//					}

//                    ret = sqlite3_exec(m_lpDB, strSql, CallBack_SumData, NULL, &lpErrMsg);
//					if(SQLITE_OK != ret)
//					{

//						LOGERROR("select fail. [Event] FatalError(). error code : %d, error describe : %s", ret, lpErrMsg);
//					}
//				}
//			}
			
//		}

//	}

//    LOGINFO("[SumDataSync]--------------------------[end]%s",gSumData);

//	return gSumData;
//}

void CMTSL::N_UpdateRecord()
{
	std::fstream fn;
	string szBuffer;
	vector<string> vRecord;
	LOGINFO("%s,Start",__FUNCTION__);
	try
	{
		//////读取之前的文件记录保存在向量中
		fn.open(CNT_ACCTFILENAME, std::ios::in);
		if(fn.fail())
		{
			LOGERROR("%s打开交易记录文件失败",__FUNCTION__);
            emit UpdateRecordFailed();
			return;
		}

		while(!fn.eof())
		{
			//每一行数据格式: sTime, sJnlnum, sAccoutNo, sTransType, sTransAmount, sInOutAmount, sRetCode, Reason
            szBuffer.clear();
			getline(fn,szBuffer,'\n');
			if(szBuffer.length()<30) //小于一定长度不是交易数据
				continue;

			szBuffer+="\n";
			vRecord.push_back(szBuffer);
		}

		fn.close();

        szBuffer.clear();
        szBuffer=m_UpdateRecordData.toStdString();
		///////修改向量中最后一个的内容包括
		vRecord.pop_back();////移除最后一个
		vRecord.push_back(szBuffer);

		//////重写文件
		fn.open(CNT_ACCTFILENAME, std::ios::trunc|std::ios::out);
		if(fn.fail())
		{
			LOGERROR("%s打开交易记录文件失败",__FUNCTION__);
            emit UpdateRecordFailed();
			return;
		}

		vector<string>::iterator vItr=vRecord.begin();
		for (;vItr!=vRecord.end();++vItr)
		{
			fn<<*vItr;

		}
		fn.close();

        vRecord.clear();
        m_UpdateRecordData.clear();
	}

    catch (...)
	{
        //e->Delete();
		LOGERROR("%s函数捕捉到错误",__FUNCTION__);
        emit UpdateRecordFailed();
		return;
	}
    emit UpdateRecordComplete();
	LOGINFO("%s函数发送UpdateRecordComplete事件",__FUNCTION__);
}

void CMTSL::N_UpdateCheckRecord()
{
	std::fstream fn;
	string szBuffer;
	vector<string> vRecord;
	LOGINFO("%s,Start",__FUNCTION__);
	try
	{
		//////读取之前的文件记录保存在向量中
        QString str_filename = m_strCheckFilePath+m_strCheckFileName;
        fn.open(Q2S(str_filename), std::ios::in);
		if(fn.fail())
		{
			LOGERROR("%s打开对账记录文件失败",__FUNCTION__);
            emit UpdateCheckFailed();
			return;
		}

		while(!fn.eof())
		{
            szBuffer.clear();
			getline(fn,szBuffer,'\n');
			if(szBuffer.length()<30) //小于一定长度不是交易数据
				continue;
			szBuffer+="\n";
			vRecord.push_back(szBuffer);
		}

		fn.close();

        szBuffer.clear();
        szBuffer=m_UpdateCheckRecordData.toStdString();
		///////修改向量中最后一个的内容包括
		vRecord.pop_back();////移除最后一个
		vRecord.push_back(szBuffer);

		//////重写文件
        fn.open(Q2S(str_filename), std::ios::trunc|std::ios::out);
		if(fn.fail())
		{
			LOGERROR("%s打开对账记录文件失败",__FUNCTION__);
            emit UpdateCheckFailed();
			return;
		}

		vector<string>::iterator vItr=vRecord.begin();
		for (;vItr!=vRecord.end();++vItr)
		{
			fn<<*vItr;

		}
		fn.close();

        vRecord.clear();
        m_UpdateCheckRecordData.clear();
	}

    catch (...)
	{
        //e->Delete();
		LOGERROR("%s函数捕捉到错误",__FUNCTION__);
        emit UpdateCheckFailed();
		return;
	}
    emit UpdateCheckComplete();
	LOGINFO("%s函数发送UpdateCheckComplete事件",__FUNCTION__);
}

//void CMTSL::N_GetDatabasePath()
//{
//    char chFilePath[MAX_PATH]={0};
//    GetModuleFileName(NULL,chFilePath,MAX_PATH);
//    if(chFilePath != NULL)
//    {
//        QString strFilePath(chFilePath);
//        strFilePath = strFilePath.Mid(0,strFilePath.ReverseFind('\\'));
//        strFilePath += QString("\\DB\\") + QString(DATABASE_NAME);
//        m_strDataBasePathName = strFilePath;
//    }
//    else
//    {
//        m_strDataBasePathName = "";
//    }
//}

//得到以sep分割的所有子串
CMTSL::VerStr CMTSL::Token_All(const QString str,const char sep)
{
    VerStr verStr;
    int front=0,back=0;

    if(""==str)
        return verStr;

    do{
        front = str.indexOf(sep,back);
        if(-1==front)
        {
            verStr.push_back(str.mid(back,str.length()-back));
        }
        else
        {
            verStr.push_back(str.mid(back,front-back));

            back=front+1;
        }
    }while(-1!=front);

    return verStr;
}


//GB2312到UTF-8的转换
//QString CMTSL::G2U(const char* gb2312)
//{
//    int len = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);
//    wchar_t* wstr = new wchar_t[len+1];
//    memset(wstr, 0, len+1);
//    MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, len);
//    len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
//    char* str = new char[len+1];
//    memset(str, 0, len+1);
//    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
//    if(wstr) delete[] wstr;
//    QString strRet = str;
//    delete str;
//    return strRet;
//}

void CMTSL::GetConfigPath()
{
    QString strFilePath= g_Helper.GetModuleFileName((void*)&CMTSL::GetInstance);
    if(!strFilePath.isEmpty())
    {
        strFilePath = strFilePath.mid(0,strFilePath.lastIndexOf('/'));
        strFilePath += "/ini/Setup.ini";
        m_strConfigPath = strFilePath;
    }
    else
    {
        m_strConfigPath = "";
    }
}

void CMTSL::N_InitUpCheckFile()
{
	char cUpJnlPath[400]={0};//上传对账文件的路径
	char szValue[50]={0};
    QDateTime EtTime = QDateTime::currentDateTime();
    QString sTime;
    sTime.sprintf("%04d/%02d/%02d", EtTime.date().year(), EtTime.date().month(), EtTime.date().day());
    QString strUpJnlPath=g_Helper.GetPrivateProfileString("MTSL", "UpJnlPath", "", m_strConfigPath);

	short nRet = dllDSTO.D_GetDataString(TRUE, "TERMINALNUM", szValue);	
    m_strCheckFilePath.sprintf("%s",Q2S(strUpJnlPath));
    m_strCheckFileName.sprintf("ATMC_HMSZ_%s_%s.txt",szValue,Q2S(sTime));
	
}


QObject *CreateInstance()
{
    return CMTSL::GetInstance();
}
