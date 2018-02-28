// Last updated at 2015/12/9 16:54:04

#include "stdafx.h"
#include "mtcp.h"
#include "win_type_def.h"
#include "Helper/helper.h"
#include "easytrace.h"
#include <arpa/inet.h>
#include "AlarmLib.h"

#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>
#include <QtXml/QDomNodeList>

const int BUF_SIZE = 1024 * 5;
const int MAX_TIMEOUT = 120 * 1000;//120S

CMTCP* CMTCP::m_pInstance = NULL;

CMTCP::CMTCP(QObject* pParent):QObject(pParent)
{
	m_iRetry = 1;
	m_strIP = "127.0.0.1";
	m_Port = 3014;

	GetConfigPath();//得到配置文件的路径

	char buf[128] = {0};
	//从配置文件中获取IP地址和端口号
    CHelper help;
    m_strIP = help.GetPrivateProfileString("MTCP","HOSTIP","127.0.0.1",m_strConfigPath);

	memset(buf,0,128);
    m_strPassbookIP = help.GetPrivateProfileString("MTCP","PASSBOOKHOSTIP","127.0.0.1",m_strConfigPath);

    m_PassbookPort = help.GetPrivateProfileInt("MTCP","PASSBOOKHOSTPORT",18999,m_strConfigPath);
    m_Port = help.GetPrivateProfileInt("MTCP","HOSTPORT",3014,m_strConfigPath);

    LOGINFO("MTCP模块加载!!");
    LOGINFO("HOSTIP = %s , HOSTPORT = %d",Q2S(m_strIP),m_Port);

}

CMTCP::~CMTCP()
{
    shutdown(m_sHost,SHUT_RDWR);


    LOGINFO("MTCP模块卸载!!");
}

CMTCP *CMTCP::GetInstance()
{
    if(m_pInstance == NULL)
    {
        m_pInstance = new CMTCP();
    }

    return m_pInstance;
}

bool CMTCP::GetMasterDataQueryBuf(char *pszBuf, long *pnBufLen)
{
	const char* pp = "000339441093        130AUTO                                                           <ap><MsgType>01</MsgType><ReqTime>085231</ReqTime><ReqDate>20160922</ReqDate><ReqId>13011734</ReqId><Tulip><ABIS><帐号省市代码>44</帐号省市代码><客户帐号>069700460200127</客户帐号><凭证号></凭证号><客户密码></客户密码><查询范围></查询范围></ABIS></Tulip></ap>";
	strcpy(pszBuf,pp);
	*pnBufLen = strlen(pszBuf);
	return true;
}
bool CMTCP::GetPassbookBuf(char *pszBuf, long *pnBufLen)
{
	const char* pp = "000583441073        130AUTO                                                           <ap><MsgType>01</MsgType><ReqTime>085231</ReqTime><ReqDate>20160922</ReqDate><ReqId>13011734</ReqId><Tulip><ABIS><省市代码>44</省市代码><客户帐号>069700460200127</客户帐号><客户密码>FFFFFFFF</客户密码><凭证种类>62</凭证种类><凭证号>8130943</凭证号></ABIS><票据类><凭证批次>51</凭证批次></票据类><身份资料><证件类型>110001</证件类型><证件号码>440111196206180603</证件号码></身份资料><标识号码><联名支付密码></联名支付密码></标识号码><联名证件类型></联名证件类型><联名证件号码></联名证件号码></Tulip></ap>";
	strcpy(pszBuf,pp);
	*pnBufLen = strlen(pszBuf);
	return true;
}

void CMTCP::CompositionData(const QString& TransactionType)
{
    LOGINFO("%s,IN,TransactionType = %s",__FUNCTION__,Q2S(TransactionType));

	m_TranscationType = TransactionType;

    QString str("");

	char bufMsg[BUF_SIZE]={0};
	char bufMac[BUF_SIZE]={0};
	char bufMsgAsc[BUF_SIZE]={0};
	char bufMacAsc[BUF_SIZE]={0};

	long lenMsg=0,lenMac=0;
	if ("FillPassbook" == m_TranscationType)
	{
		GetPassbookBuf(bufMsg,&lenMsg);
		str = "00";
        m_Data = QString(bufMsg);

        emit CompositionDataCompleted(str);
		LOGINFO("存折补登报文组包数据成功,事件：CompositionDataCompleted。");
	} else if ("MasterDataQuery" == m_TranscationType)
	{
		GetMasterDataQueryBuf(bufMsg,&lenMsg);
		str = "00";
        m_Data = QString(bufMsg);

        emit CompositionDataCompleted(str);
		LOGINFO("存折主档查询报文组包成功,事件：CompositionDataCompleted。");
	}
	else
	{
        int ret = m_DSend.PackageData(TransactionType.toStdString().c_str(),bufMsg,&lenMsg,bufMac,&lenMac);

		if(0 == ret)
		{
			BCDToASCII((LPBYTE)bufMacAsc,(LPBYTE)bufMac,2*lenMac);
			BCDToASCII((LPBYTE)bufMsgAsc,(LPBYTE)bufMsg,2*lenMsg);

            str = QString(bufMacAsc);
            m_Data = QString(bufMsgAsc);

            emit CompositionDataCompleted(str);
			LOGINFO("组包数据成功,事件：CompositionDataCompleted。");
		}
		else
		{
            emit CompositionDataFail();
			LOGERROR("组包数据失败。");
            Alarm("M0040001");
		}
	}

	LOGINFO("%s,OUT",__FUNCTION__);
}


void CMTCP::SendToHost(const QString& MAC, LONG Timeout)
{
    LOGINFO("%s,MAC = %s,IN",__FUNCTION__,MAC.toStdString().c_str());

	Timeout = (-1==Timeout) ? MAX_TIMEOUT:Timeout;
	m_Timeout = Timeout;

	if(Timeout < 0)
	{
		//FatalError();
		LOGERROR("%s,参数错误",__FUNCTION__);
        Alarm("M0040002");
		return;
	}

	char bufSend[BUF_SIZE] = {0};
	char szTmp[BUF_SIZE] = {0};
	char bufSendAsc[BUF_SIZE] = {0};
	int bufLen = 0;
	int ret = 0;

	//创建套接字 IPPROTO_TCP
	m_sHost = socket(AF_INET,SOCK_STREAM,0);
    if(0 > m_sHost)
	{
        emit SendFailed();
		LOGERROR("创建套接字失败！");
        Alarm("M0040003");
		return;
	}
    bzero((char *) &m_servAddr, sizeof(sockaddr_in));
	m_servAddr.sin_family = AF_INET;


	if ("MasterDataQuery" == m_TranscationType || "FillPassbook" == m_TranscationType)
	{
        m_servAddr.sin_addr.s_addr = inet_addr(m_strPassbookIP.toStdString().c_str());
		m_servAddr.sin_port = htons(m_PassbookPort); 
	}
	else
	{
        m_servAddr.sin_addr.s_addr = inet_addr(m_strIP.toStdString().c_str());
		m_servAddr.sin_port = htons(m_Port); 
	}

	//连接服务器
    ret = ::connect(m_sHost,(struct sockaddr*)&m_servAddr,sizeof(sockaddr_in));

    if(-1 == ret)
	{
        emit SendFailed();
		LOGERROR("连接服务器失败！");
        Alarm("M0040003");
		return;
	}

	ZeroMemory(bufSend,BUF_SIZE);

    QString str("");

    if(MAC == "00" || MAC.isEmpty())
		str = m_Data;
	else
		str = m_Data + MAC;

    int len = str.length();
	if ("MasterDataQuery" == m_TranscationType || "FillPassbook" == m_TranscationType)
        memcpy(bufSend, str.toStdString().c_str(), len);
	else
        ASCIIToBCD((LPBYTE)bufSend,(LPBYTE)str.toStdString().c_str(),len);

	EncryptTrace((LPBYTE)szTmp, (LPBYTE)bufSend, len); //报文加密打印
	BCDToASCII((LPBYTE)bufSendAsc,(LPBYTE)szTmp,len);
    LOGINFO("SEND,Type = %s,Length = %d\n%s",m_TranscationType.toStdString().c_str(),len,FormatData(bufSendAsc).toStdString().c_str());

	//设置发送超时时间
	int nTimeout = Timeout/2;
	setsockopt(m_sHost,SOL_SOCKET,SO_SNDTIMEO,(char *)&nTimeout,sizeof(int));
	//向服务器发送数据
	if ("MasterDataQuery" == m_TranscationType || "FillPassbook" == m_TranscationType)
		ret = send(m_sHost,bufSend,len,0);
	else
		ret = send(m_sHost,bufSend,len/2,0);

    if(-1 == ret)
	{
        emit SendFailed();
		LOGERROR("向服务器发送数据失败！");
        Alarm("M0040003");
		return;
	}

    ::pthread_create(&m_pThread, NULL, ThreadProc, this);

	LOGINFO("%s,OUT",__FUNCTION__);
}
bool CMTCP::AnalysisRecvPassbookBuf(char *pszBuf, int *pnBufLen)
{
    /*
	TiXmlDocument* doc = new TiXmlDocument();
	if (!doc)
	{
		return false;
	}
	doc->Parse(pszBuf);
	for (;true;)
	{
		TiXmlNode* rootnode = doc->FirstChild("ap");
		if (!rootnode)
			break;
		TiXmlNode* node03 = rootnode->FirstChild("Tulip")->FirstChild("数量类")->FirstChild("总记录数");
		if (!node03)
			break;
		TiXmlElement* pElement = node03->ToElement();
		int nCount = atoi(pElement->GetText());
		if (0 >= nCount)
		{
			break;
		}
		else
		{
			TiXmlNode* fcnode = rootnode->FirstChild("Tulip")->FirstChild("fc");
			if (!fcnode)
				break;
			for (int i=1;i<=nCount;i++)
			{
                QString strD = "";
                QString strRow = fcnode->FirstChild("行码")->ToElement()->GetText();
                QString strT;
                strT = QString("data%1=").arg(strRow);
                //strT.Format("data%s=",strRow);
				strD += strT;
				strD += fcnode->FirstChild("交易日期")->ToElement()->GetText();
				strD += "|";
                strT = QString("abstract%1=").arg(strRow);
                //strT.Format("abstract%s=",strRow);
				strD += strT;
				strD += fcnode->FirstChild("摘要")->ToElement()->GetText();
				strD += "|";
                strT = QString("detail%1=").arg(strRow);
                //strT.Format("detail%s=",strRow);
				strD += strT;
				strD += fcnode->FirstChild("交易金额")->ToElement()->GetText();
				strD += "|";
                strT = QString("balance%1=").arg(strRow);
                //strT.Format("balance%s=",strRow);
				strD += strT;
				strD += fcnode->FirstChild("交易后余额")->ToElement()->GetText();
				strD += "|";
                strT = QString("operator%1=").arg(strRow);
                //strT.Format("operator%s=",strRow);
				strD += strT;
				strD += fcnode->FirstChild("交易柜员")->ToElement()->GetText();

				strT = fcnode->FirstChild("页码")->ToElement()->GetText();
				strT = "FillPassbook" + strT;

				char szDataGet[2400] = {0};
                m_DSTO.D_GetDataString(false,strT.toStdString().c_str(),szDataGet);
				if (0 < strlen(szDataGet))
				{
					strD = "|"+strD;
					strD = szDataGet+strD;
				}
                m_DSTO.D_SetDataString(false,strT.toStdString().c_str(),strD.toStdString().c_str());

				fcnode = fcnode->NextSibling("fc");
				if (!fcnode)
					break;
			}
			break;
		}
		break;
	}
	delete doc;
    doc = NULL;*/
	return true;
}

void *CMTCP::ThreadProc(LPVOID lpParam)
{
	CMTCP * pTcp = (CMTCP *)lpParam;

	int bufLen = 0; 
	char bufRecv[BUF_SIZE] = {0};
	char bufRecvAsc[BUF_SIZE] = {0};
	char szTmp[BUF_SIZE] = {0};

	//设置接收超时时间
	int nTimeout = pTcp->m_Timeout/2;
	setsockopt(pTcp->m_sHost,SOL_SOCKET,SO_RCVTIMEO,(char *)&nTimeout,sizeof(int));

	bufLen = recv(pTcp->m_sHost,bufRecv,BUF_SIZE,0);//阻塞

    shutdown(pTcp->m_sHost,SHUT_RDWR);//关闭套接字

	if(bufLen > 0)
	{
		LOGINFO("接受到了报文");

		if ("MasterDataQuery" == pTcp->m_TranscationType || "FillPassbook" == pTcp->m_TranscationType)
		{
            LOGINFO("RECV,Type = %s,Length = %d\n%s",pTcp->m_TranscationType.toStdString().c_str(),bufLen,bufRecv);
			//pTcp->AnalysisRecvPassbookBuf(bufRecv+86,&bufLen);
            emit pTcp->OnRecved(pTcp->m_TranscationType);//发送完成事件
		}
		else
		{
			pTcp->EncryptTrace((LPBYTE)szTmp, (LPBYTE)bufRecv, bufLen); //报文数据加密打印
			pTcp->BCDToASCII((LPBYTE)bufRecvAsc,(LPBYTE)szTmp,2*bufLen);
            LOGINFO("RECV,Type = %s,Length = %d\n%s",pTcp->m_TranscationType.toStdString().c_str(),2*bufLen,pTcp->FormatData(bufRecvAsc).toStdString().c_str());

            int ret = pTcp->m_DRecv.ParseData(pTcp->m_TranscationType.toStdString().c_str(),bufRecv,bufLen);//数据解包

			if(ret == 0)
			{
				char chCode[16] = {0};
				pTcp->m_DSTO.D_GetDataString(false,"RESPONSECODE",chCode);

                emit pTcp->OnRecved(chCode);//发送完成事件
				LOGINFO("%s,事件：OnRecved(%s)",__FUNCTION__,chCode);
			}
			else
			{
                emit pTcp->AnalysisFailed();
				LOGERROR("解包数据失败！事件：AnalysisFailed()");
                Alarm("M0040004");
			}
		}
	}
	else
	{
		LOGERROR("SOCKET_ERROR 发送超时事件");
        Alarm("M0040004");
        emit pTcp->Timeout();
	}

    pthread_exit(NULL);//关闭线程内核对象
    pTcp->m_pThread = NULL;

    //return 0;
}


bool CMTCP::ASCIIToBCD(unsigned char *bcd, const unsigned char *asc, int nLen)
{
	if ( bcd == NULL || asc == NULL )
	{
		return false;
	}

	char *p = new char[nLen + 1];
	memset(p, 0, nLen + 1);
	int	i = 0;
	int j = 0;

	for (i = 0; i < nLen; ++i) 
	{
		if( (asc[i] >= '0') && (asc[i] <= '9') )
		{
			p[i] = asc[i] - '0';
		}
		else if( (asc[i] >= 'a') && (asc[i] <= 'f') )
		{
			p[i] = asc[i] - 'a' + 10;
		}
		else if( (asc[i] >= 'A') && (asc[i] <= 'F') )
		{
			p[i] = asc[i] - 'A' + 10;
		}		
		else 
		{
			p[i] = asc[i] - '0';
		}
	}

	/*两两配对*/
	j = (nLen + nLen % 2) / 2;
	if ( nLen % 2 ) 
	{
		p[nLen + 1] = 0;
	}

	for ( i = 0; i < j; ++i ) 
	{
		bcd[i] = (p[2 * i] & 0x0f) << 4;
		bcd[i] |= (p[2 * i + 1] & 0x0f);
	}

	delete []p;

	return true;
}

bool CMTCP::BCDToASCII(LPBYTE asc, const LPBYTE bcd, int len)
{
	if ( asc == NULL || bcd == NULL )
	{
		return false;
	}

	int	i, j;

	j = (len + len % 2 )/2;
	for ( i = 0; i < j; ++i ) 
	{
		asc[2 * i]		= (bcd[i] >> 4) & 0x0f;
		asc[2 * i + 1]	= bcd[i] & 0x0f;
	}

	for ( i = 0; i < len; ++i )
	{
		if ( asc[i] > 0x09) 
			asc[i] = 'A' + asc[i] - 0x0a;
		else	
			asc[i] += '0';
	}

	asc[len] = 0;

	return true;
}

QString CMTCP::FormatData(QString originalStr)
{
    int iLen = originalStr.length();

	if(originalStr == "" || iLen%2)
		return originalStr;

	char strRet[2048] = {0};
	int index = 0;

	int col = 32;//每行的字符数
	int row = iLen/col + ((iLen%col == 0) ? 0 : 1);//得到行数

	for(int i = 0;i<row;i++)
	{
		for(int j = 0;j<col && (i*col+j) < iLen;j+=2)
		{
            //strRet[index++] = originalStr[i*col+j];
            //strRet[index++] = originalStr[i*col+j+1];
			strRet[index++] = ' ';//空格分隔
			if((j+2)%(col/2) == 0)//每一行分为2段，每段用空格分隔
				strRet[index++] = ' ';
		}
		strRet[index-1] = '\n';//换行
	}
	strRet[index-1] = 0x00;//结束符

	return strRet;
}

bool CMTCP::EncryptTrace(LPBYTE outData, const LPBYTE inData, int len)
{
	if ( outData == NULL || inData == NULL )
	{
		return false;
	}
	memcpy(outData, inData, len);

	int i;                                                                
	for (i=0; i<len; i++)                                                       
	{                                                                         
		outData[i] ^= 0x5C;                                                         
	}                                                                         
	for (i=len-1; i>0; i--)                                                     
	{                                                                         
		outData[i] ^=outData[i-1];                                                        
	}                                                                         
	outData[0] ^= 0xC5;  

	return true;
}

void CMTCP::GetConfigPath()
{
    QString strFilePath = g_Helper.GetModuleFileName((void*)&CMTCP::GetInstance);
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


QObject *CreateInstance()
{
    return CMTCP::GetInstance();
}
