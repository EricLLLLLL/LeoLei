// Last updated at 2016/7/20 9:23:24

#include "stdafx.h"
#include "mjnl.h"
#include "DJNL.h"

#define MJNL_JSLOGFILENAMEPREFIX "JSLOG"

CMJNL* CMJNL::m_pInstance = NULL;

CMJNL::CMJNL()
{
	m_pFile = NULL;
    N_GetConfigPath();
	N_InitFrom();
	LOGINFO("MJNL 模块加载");
}

CMJNL::~CMJNL()
{
	LOGINFO("MJNL 模块卸载");
}

CMJNL *CMJNL::GetInstance()
{
    if(m_pInstance == NULL)
    {
        m_pInstance = new CMJNL();
    }
    return m_pInstance;
}
// 以下处理函数
SHORT CMJNL::N_InitFrom(void)
{
    int ret = m_JnlFormFormat.Translate(Q2S(m_strFormPathName));
    if(CJnlFormFormat::NG == ret)
    {
        LOGERROR("没有找到合格的 FORM ");
        Alarm("M0010000");
    }
    std::string strErrInfo = m_JnlFormFormat.GetErrInfo();
    if("" != strErrInfo)
    {
        LOGERROR("FROM 错误信息有：\n%s", strErrInfo.c_str());
        Alarm("M0010000");
    }
    return 0;
}

SHORT CMJNL::PrintSync(QString strJnlFromName)
{
    LOGINFO("strJnlType = %s", strJnlFromName.toStdString().c_str());

    QString strValues;
	N_AddTime(strJnlFromName, strValues);
    N_GetFieldValue(strJnlFromName, strValues);
    std::string strFormat = m_JnlFormFormat.Format(Q2S(strJnlFromName), Q2S(strValues));
    LOGINFO("strFormat:%s", strFormat.c_str());
    LOGINFO("strJnlFromName:%s", strJnlFromName.toStdString().c_str());
    LOGINFO("strValues:%s", strValues.toStdString().c_str());
	if (strFormat.empty())
	{
        LOGERROR("未找到FROMNAME：\n%s", strJnlFromName.toStdString().c_str());
        Alarm("M0010003");
		return -1;
	}
	if (strJnlFromName == "MachineSettle")
	{
		//加上异常交易打印
		N_AddMachineSettleAcct(strFormat);
	}
	else if (strJnlFromName == "MachineSettle_HaveReDep")
	{
		//加上带有回存交易的异常交易打印
		N_AddMachineSettleAcct_haveReDep(strFormat);
	}
	N_WriteElectJnl("%s\n", strFormat.c_str());
    return 0;
}

void CMJNL::N_GetConfigPath()
{
    //char chFilePath[MAX_PATH] = {0};
    QString strFilePath = QCoreApplication::applicationDirPath()+"/lib/";
   // char *chFilePath = QchFilePath.toStdString().c_str();
    //applicationDirPath(NULL, chFilePath, MAX_PATH);
    if(!strFilePath.isEmpty())
    {
        strFilePath = strFilePath.mid(0, strFilePath.lastIndexOf('/'));
        strFilePath += "/ini/Setup.ini";

        char buff[MAX_PATH] = {0};
        m_strFormPathName = g_Helper.GetPrivateProfileString("MJNL", "JnlFormPathName", "", strFilePath);
       // m_strFormPathName = buff;
        
        memset(buff, 0, MAX_PATH);
        m_strJnlPath = g_Helper.GetPrivateProfileString("MJNL", "JnlPath", "", strFilePath);
        //m_strJnlPath = buff;

		memset(buff, 0, MAX_PATH);
        m_JsLogPath = g_Helper.GetPrivateProfileString("MJNL", "JsLogPath", "", strFilePath);
        //m_JsLogPath = buff;

        m_JnlSaveDays = g_Helper.GetPrivateProfileInt("MJNL", "JnlSaveDays", 365, strFilePath);

        m_JsLogSaveDays = g_Helper.GetPrivateProfileInt("MJNL", "JsLogSaveDays", 365, strFilePath);
    }
}

int CMJNL::N_GetFieldValue(QString FormName, QString &FieldValues)
{
	int itForm;
	for(itForm=0; itForm<FormNum; itForm++)
	{
		if(FormName == dstoForm[itForm].FormName)
			break;
	}

	if(itForm < FormNum)
	{
		for(int i=0; i<dstoForm[itForm].FieldNum; i++)
		{
            QString sValue;
			long lValue = 0;
			LPBYTE lpbyteValue = NULL;

			short sRet;
			if(dstoForm[itForm].lpdstoField[i].DataType == "STRING")
			{
				sRet = N_GetDataFromDSTO(dstoForm[itForm].lpdstoField[i].bPersistent, dstoForm[itForm].lpdstoField[i].FieldName, sValue);
			}
			else if(dstoForm[itForm].lpdstoField[i].DataType == "LONG")
			{
				sRet = N_GetDataFromDSTO(dstoForm[itForm].lpdstoField[i].bPersistent, dstoForm[itForm].lpdstoField[i].FieldName, lValue);
			}
			else if(dstoForm[itForm].lpdstoField[i].DataType == "LPBYTE")
			{
				sRet = N_GetDataFromDSTO(dstoForm[itForm].lpdstoField[i].bPersistent, dstoForm[itForm].lpdstoField[i].FieldName, lpbyteValue);
			}

                QString FieldValue;
				if(dstoForm[itForm].lpdstoField[i].DataType == "STRING")
				{
					FieldValue = sValue;
				}
				else if(dstoForm[itForm].lpdstoField[i].DataType == "LONG")
				{
					if(dstoForm[itForm].lpdstoField[i].FieldName=="JNLNUM" || dstoForm[itForm].lpdstoField[i].FieldName=="CWCJNLNUM")
                        //FieldValue.sprintf("%06d", lValue);
                        FieldValue.sprintf("%06d",lValue);
					else
                        //FieldValue.sprintf("%d", lValue);
                        FieldValue.sprintf("%d",lValue);
				}
				else if(dstoForm[itForm].lpdstoField[i].DataType == "LPBYTE")
				{
                    //FieldValue.sprintf("%c",lpbyteValue);
                    FieldValue = US2Q(lpbyteValue);
                    //FieldValue = (lpbyteValue);
				}

				//修改样式
				N_ChangeShowStyle(dstoForm[itForm].FormName, dstoForm[itForm].lpdstoField[i].FieldName, dstoForm[itForm].lpdstoField[i].SizeX, FieldValue);

				FieldValues += dstoForm[itForm].lpdstoField[i].FieldName;
				FieldValues += "=";
				FieldValues += FieldValue;

				if(i+1 < dstoForm[itForm].FieldNum)
				{
					FieldValues += ",";
				}
		}
	}
	FieldValues += ",";
	FieldValues += N_CountBoxAount(FormName); //计算各钞箱金额
	return 0;
}

int CMJNL::N_AddTime(QString FormName, QString &FieldValues)
{
    //QTime CurTime = QTime::currentTime();
    QDateTime EtTime = QDateTime::currentDateTime();
    QString time;
    time.sprintf("Time=%04d/%02d/%02d %02d:%02d:%02d,", EtTime.date().year(), EtTime.date().month(), EtTime.date().day()
        , EtTime.time().hour(), EtTime.time().minute(), EtTime.time().second());
	FieldValues += time;
	return 0;
}

void CMJNL::N_ChangeShowStyle(QString FormName, QString FieldName, int SizeX, QString &FieldValue)
{
	LOGINFO("函数：%s",__FUNCTION__);
	if(FormName == "MachineSettle" || FormName == "MachineSettle_HaveReDep")
	{
        if((FieldName == "ATMC_DISPENSEAMOUNT" || FieldName == "ATMC_DEPOSITAMOUNT") && FieldValue.length())
		{
            int AMOUNT = FieldValue.toInt();

			AMOUNT = AMOUNT / 100;  //把金额单位从“分”转换为“元”

            FieldValue.sprintf("%d.00", AMOUNT);
		}
        if(FieldName == "BOX1VALUEFMT" && FieldValue.length())
		{
			//钞箱1 纸币面额
			BOXVALUEFMT[0] = FieldValue;
		}
        if(FieldName == "BOX2VALUEFMT" && FieldValue.length())
		{
			//钞箱2 纸币面额
			BOXVALUEFMT[1] = FieldValue;
		}
        if(FieldName == "BOX3VALUEFMT" && FieldValue.length())
		{
			//钞箱3 纸币面额
			BOXVALUEFMT[2] = FieldValue;
		}
        if(FieldName == "BOX4VALUEFMT" && FieldValue.length())
		{
			//钞箱4 纸币面额
			BOXVALUEFMT[3] = FieldValue;
		}
        if(FieldName == "BOX5VALUEFMT" && FieldValue.length())
		{
			//钞箱5 纸币面额
			BOXVALUEFMT[4] = FieldValue;
		}
		//add by art for 双室钞箱 begin
        if(FieldName == "BOX6VALUEFMT" && FieldValue.length())
		{
			//钞箱6 纸币面额
			BOXVALUEFMT[5] = FieldValue;
		}
        if(FieldName == "BOX7VALUEFMT" && FieldValue.length())
		{
			//钞箱7 纸币面额
			BOXVALUEFMT[6] = FieldValue;
		}


        if(FieldName == "BOX1NOTENUMBERFMT" && FieldValue.length())
		{
			//钞箱1 剩余张数
			NOTENUM[0] = FieldValue;
		}
        if(FieldName == "BOX2NOTENUMBERFMT" && FieldValue.length())
		{
			//钞箱2 剩余张数
			NOTENUM[1] = FieldValue;
		}
        if(FieldName == "BOX3NOTENUMBERFMT" && FieldValue.length())
		{
			//钞箱3 剩余张数
			NOTENUM[2] = FieldValue;
		}
        if(FieldName == "BOX4NOTENUMBERFMT" && FieldValue.length())
		{
			//钞箱4 剩余张数
			NOTENUM[3] = FieldValue;
		}
        if(FieldName == "BOX5NOTENUMBERFMT" && FieldValue.length())
		{
			//钞箱5 剩余张数
			NOTENUM[4] = FieldValue;
		}
        if(FieldName == "BOX6NOTENUMBERFMT" && FieldValue.length())
		{
			//钞箱6 剩余张数
			NOTENUM[5] = FieldValue;
		}
        if(FieldName == "BOX7NOTENUMBERFMT" && FieldValue.length())
		{
			//钞箱7 剩余张数
			NOTENUM[6] = FieldValue;
		}
	}


	if(FormName == "AddCashNote")
	{
        if(FieldName == "BOX1VALUEFMT" && FieldValue.length())
		{
			//钞箱1 纸币面额
			BOXVALUEFMT[0] = FieldValue;
		}
        if(FieldName == "BOX2VALUEFMT" && FieldValue.length())
		{
			//钞箱2 纸币面额
			BOXVALUEFMT[1] = FieldValue;
		}
        if(FieldName == "BOX3VALUEFMT" && FieldValue.length())
		{
			//钞箱3 纸币面额
			BOXVALUEFMT[2] = FieldValue;
		}
        if(FieldName == "BOX4VALUEFMT" && FieldValue.length())
		{
			//钞箱4 纸币面额
			BOXVALUEFMT[3] = FieldValue;
		}
        if(FieldName == "BOX5VALUEFMT" && FieldValue.length())
		{
			//钞箱5 纸币面额
			BOXVALUEFMT[4] = FieldValue;
		}
        if(FieldName == "BOX6VALUEFMT" && FieldValue.length())
		{
			//钞箱6 纸币面额
			BOXVALUEFMT[5] = FieldValue;
		}
        if(FieldName == "BOX7VALUEFMT" && FieldValue.length())
		{
			//钞箱7 纸币面额
			BOXVALUEFMT[6] = FieldValue;
		}

        if(FieldName == "ENCASEBILLS1" && FieldValue.length())
		{
			//钞箱1 加钞张数
			NOTENUM[0] = FieldValue;
		}
        if(FieldName == "ENCASEBILLS2" && FieldValue.length())
		{
			//钞箱2 加钞张数
			NOTENUM[1] = FieldValue;
		}
        if(FieldName == "ENCASEBILLS3" && FieldValue.length())
		{
			//钞箱3 加钞张数
			NOTENUM[2] = FieldValue;
		}
        if(FieldName == "ENCASEBILLS4" && FieldValue.length())
		{
			//钞箱4 加钞张数
			NOTENUM[3] = FieldValue;
		}
        if(FieldName == "ENCASEBILLS5" && FieldValue.length())
		{
			//钞箱5 加钞张数
			NOTENUM[4] = FieldValue;
		}
        if(FieldName == "ENCASEBILLS6" && FieldValue.length())
		{
			//钞箱6 加钞张数
			NOTENUM[5] = FieldValue;
		}
        if(FieldName == "ENCASEBILLS7" && FieldValue.length())
		{
			//钞箱7 加钞张数
			NOTENUM[6] = FieldValue;
		}
	}

	if(FormName == "CashInBox" || FormName == "CashOutBox1")
	{
        if (FieldName == "TRANSAMOUNT" && FieldValue.length())
		{
            int TRANSAMOUNT = FieldValue.toInt(); //这里是以分为单位,无小数点
			int nYuan = TRANSAMOUNT /  100;
			int nFen = TRANSAMOUNT %  100;
            FieldValue.sprintf("%d.%02d", nYuan, nFen);
		}
        if(FieldName == "POUNDAGE" && FieldValue.length())
		{
            int POUNDAGE = FieldValue.toInt();
			int nYuan = POUNDAGE /  100;
			int nFen = POUNDAGE %  100;
            FieldValue.sprintf("%d.%02d", nYuan, nFen);
		}
	}
	if (FormName == "TfrComplete")
	{
        if (FieldName == "TRANSAMOUNT" && FieldValue.length())
		{
            int TRANSAMOUNT = FieldValue.toInt(); //这里是以分为单位,无小数点
			int nYuan = TRANSAMOUNT /  100;
			int nFen = TRANSAMOUNT %  100;
            FieldValue.sprintf("%d.%02d", nYuan, nFen);
		}
	}

	if (FormName == "TransferComplete")
	{
        if (FieldName == "TRANSAMOUNT" && FieldValue.length())
		{
            int TRANSAMOUNT = FieldValue.toInt(); //这里是以分为单位,无小数点
			int nYuan = TRANSAMOUNT /  100;
			int nFen = TRANSAMOUNT %  100;
            FieldValue.sprintf("%d.%02d", nYuan, nFen);
		}
	}

	if (FormName == "CTRComplete")
	{
        if (FieldName == "TRANSAMOUNT" && FieldValue.length())
		{
            int TRANSAMOUNT = FieldValue.toInt();//这里是以分为单位,无小数点
			int nYuan = TRANSAMOUNT /  100;
			int nFen = TRANSAMOUNT %  100;
            FieldValue.sprintf("%d.%02d", nYuan, nFen);
		}

        if ((FieldName == "DEPOSIT" || FieldName == "AGREEMENT" )&& FieldValue.length())
		{

            if (!QString::compare(FieldValue,"M003"))
			{
                FieldValue.sprintf("三个月");
			}
            else if (!QString::compare(FieldValue,"M006"))
			{
                FieldValue.sprintf("六个月");
			}
            else if (!QString::compare(FieldValue,"Y001"))
			{
                FieldValue.sprintf("一年");
			}
            else if (!QString::compare(FieldValue,"Y002"))
			{
                FieldValue.sprintf("两年");
			}
            else if (!QString::compare(FieldValue,"Y003"))
			{
                FieldValue.sprintf("三年");
			}
            else if (!QString::compare(FieldValue,"Y005"))
			{
                FieldValue.sprintf("五年");
			}
		}

        if (FieldName == "DEALTYPEFLAG" && FieldValue.length())
		{
            int iTransType = FieldValue.toInt();
			if (1 == iTransType)
			{
                FieldValue.sprintf("预约转存");
			}
			else if(2 == iTransType)
			{
                FieldValue.sprintf("不预约转存");
			}
		}
	}

	if (FormName == "DepErrComplete")
	{
        if (FieldName == "RESETRESULTS" /*&& FieldValue.length()*/)
		{
            LOGINFO("FieldValue:%s", Q2S(FieldValue));  // 后期删除
			if (FieldValue == "1")
			{
                FieldValue.sprintf("复位成功");
			}
			else
			{
                FieldValue.sprintf("复位失败");
			}
		}
	}

	if (FormName == "SalesComplete")
	{
        /*if (FieldName == "TRANSAMOUNT" && FieldValue.length())
		{
            int TRANSAMOUNT = FieldValue.toInt();

			int nYuan = TRANSAMOUNT /  100;
			int nFen = TRANSAMOUNT %  100;
            FieldValue.sprintf("%d.%02d", nYuan, nFen);
		}*/

        if(FieldName == "POUNDAGE" && FieldValue.length())
		{
            int POUNDAGE = FieldValue.toInt();

			int nYuan = POUNDAGE /  100;
			int nFen = POUNDAGE %  100;
            FieldValue.sprintf("%d.%02d", nYuan, nFen);
		}
	}
}

QString CMJNL::N_CountBoxAount(QString FormName)
{
	LOGINFO("函数：%s",__FUNCTION__);
    QString FieldValues;

	if(FormName == "AddCashNote" || FormName == "MachineSettle")
	{
		int AllboxCount = 0; //钞箱总金额
		//计算钞箱金额
		for(int i=0; i < 7; i++) //modify by art for 双室钞箱
		{
            int NoteVALUEFMT = BOXVALUEFMT[i].toInt();//纸币面额
            int NoteNum = NOTENUM[i].toInt(); //纸币张

			int Amount = NoteNum * NoteVALUEFMT;  //金额

			// 计算回收箱总金额
			if( (i+1 == 4) && NoteNum > 0 ) //回收箱且张数大于0
			{			
				LONG nABAmount=0;
				N_GetDataFromDSTO(false,"ABAMOUNT", nABAmount);
				Amount=nABAmount;
			}
            BOXAMOUNT[i].sprintf("%d.00", Amount);

			AllboxCount += Amount; //钞箱总金额

            QString FieldName;
            FieldName.sprintf("%s%d%s", "BOX",i+1,"AMOUNT");
			FieldValues += FieldName;
			FieldValues += "=";
			FieldValues += BOXAMOUNT[i];
			FieldValues += ",";

            LOGINFO("函数：%s，ValueName = %s FieldValues=%s",__FUNCTION__, Q2S(FieldName), Q2S(BOXAMOUNT[i]));
		}
        ALLBOXNOTEAMOUNT.sprintf("%d.00", AllboxCount);
		FieldValues += "ALLBOXNOTEAMOUNT";
		FieldValues += "=";
		FieldValues += ALLBOXNOTEAMOUNT;
		FieldValues += ",";
        LOGINFO("函数：%s，ValueName = %s FieldValues=%s",__FUNCTION__, "ALLBOXNOTEAMOUNT", Q2S(ALLBOXNOTEAMOUNT));
	}
	return FieldValues;
}

int CMJNL::N_AddMachineSettleAcct(std::string &strFormat)
{
	//现金交易统计和异常交易记录
	char szErrors[10*1024]={0};
	dllDSTO.D_GetDataString(FALSE, "ALLERRTRANS", szErrors);
	strFormat.append("\r\n************现金异常交易记录*********** \r\n");
	strFormat.append(szErrors);

	// 吞卡记录
	memset(szErrors, 0x00, sizeof(szErrors));
	dllDSTO.D_GetDataString(FALSE, "CARDRETAINRECORD", szErrors);
	strFormat.append("\r\n************吞卡记录*********** \r\n");
	strFormat.append(szErrors);
	strFormat.append("\r\n");
	return 0;
}

int CMJNL::N_AddMachineSettleAcct_haveReDep(std::string &strFormat)
{
	//现金交易统计和异常交易记录
	char szErrors[10*1024]={0};
	dllDSTO.D_GetDataString(FALSE, "ALLERRTRANS", szErrors);
	strFormat.append("\r\n************现金异常交易记录*********** \r\n");
	strFormat.append(szErrors);

	// 吞卡记录
	memset(szErrors, 0x00, sizeof(szErrors));
	dllDSTO.D_GetDataString(FALSE, "CARDRETAINRECORD", szErrors);
	strFormat.append("\r\n************吞卡记录*********** \r\n");
	strFormat.append(szErrors);
	strFormat.append("\r\n");

	// 回存记录
	memset(szErrors, 0x00, sizeof(szErrors));
	dllDSTO.D_GetDataString(FALSE, "REDEPAMOUNT", szErrors);
	strFormat.append("\r\n**********回存交易记录********* \r\n");
	strFormat.append(szErrors);
	strFormat.append("\r\n");

	// 销户记录
	memset(szErrors, 0x00, sizeof(szErrors));
	dllDSTO.D_GetDataString(FALSE, "ALLCLOSETRANS", szErrors);
	strFormat.append("\r\n**********销户交易记录********* \r\n");
	strFormat.append(szErrors);
	strFormat.append("\r\n");

	return 0;
}

//写文件
int CMJNL::N_WriteElectJnl(const char *pInfo, ...)
{
	if (NULL == pInfo)
	{
		return -1;
	}

	int len = 0;
	char* buffer = nullptr;
    //va_list  args;

    len = S2Q(pInfo).length() + 1;
    buffer = new char[len * sizeof(char)];
    memcpy(buffer,pInfo,len);

    //va_start(args, pInfo);
    //len = _vscprintf(pInfo, args) + 1;
    //buffer = new char[len * sizeof(char)];
    //vsprintf_s(buffer, len, pInfo, args);
    //va_end(args);

	try
	{
		static bool  bDeletedJnl = false;
        QDateTime EtTime = QDateTime::currentDateTime();
		int iYear, iMonth, iDay;  
        iYear = EtTime.date().year();
        iMonth = EtTime.date().month();
        iDay = EtTime.date().day();
        //CFileFind cFindfile;
        if (!QDir(m_strJnlPath).exists())
		{
            if (!QDir(m_strJnlPath).mkpath(m_strJnlPath))
			{
                LOGERROR("SHCreateDirectoryEx fail, nRet = -1");
                Alarm("M0010001");
                return -1;
			}
		}else{
			if(!bDeletedJnl){   
				N_ElectDeleteOldFile();
				bDeletedJnl = true;  
			}
		}

        QString strJnlPath;
        if (m_strJnlPath.length() == m_strJnlPath.lastIndexOf('\\') + 1)
		{
            strJnlPath = m_strJnlPath + QDateTime::currentDateTime().toString("yyyyMMdd")+".txt";
		}
		else
		{
            strJnlPath = m_strJnlPath + "\\" + QDateTime::currentDateTime().toString("yyyyMMdd")+".txt";
		}


		if(!m_pFile)//若文件流没有打开，则重新打开
		{
            m_pFile = fopen(Q2S(strJnlPath),"a");
			if(!m_pFile)
			{
				LOGERROR("M_WriteElectJnl fopen fail！");
                Alarm("M0010001");
				if (buffer)
				{
					delete[] buffer;
					buffer = nullptr;
				}
				return -2;
			}
		}
		//写日志信息到文件流
		fprintf(m_pFile, "%s", buffer);
	}
	catch (...)
	{

	}
	fflush(m_pFile);
	if (buffer)
	{
		delete[] buffer;
		buffer = nullptr;
	}
	if(m_pFile)
	{
		fclose(m_pFile);
		m_pFile = NULL;
	}
	return 0;
}

SHORT CMJNL::WriteLogSync(QString Buffer, SHORT LogLevel)
{
	static bool  bDeleted = false;
    QDateTime EtTime = QDateTime::currentDateTime();
    int iYear, iMonth, iDay;
    iYear = EtTime.date().year();
    iMonth = EtTime.date().month();
    iDay = EtTime.date().day();
    //CFileFind cFindfile;

    if (!QDir(m_strJnlPath).exists())
    {
        if (!QDir(m_strJnlPath).mkpath(m_strJnlPath))
        {
            LOGERROR("SHCreateDirectoryEx fail, nRet = -1");
            Alarm("M0010001");
            return -1;
        }
    }else{
		if(!bDeleted){   
			N_DeleteOldFile(); 
			bDeleted = true;  
		}
	}

    QFile JnlFile(m_JsLogPath + MJNL_JSLOGFILENAMEPREFIX + QDateTime::currentDateTime().toString("yyyyMMdd")+".txt");
    if (!JnlFile.open(QIODevice::ReadWrite|QIODevice::Append|QIODevice::Truncate))
	{
		LOGERROR("WriteLogSync fopen fail！");
        Alarm("M0010002");
		return -1;
	}

    QTextStream stream(&JnlFile);
    stream.seek(JnlFile.size());
	try
	{
        QString strTime;
        strTime.sprintf("%04d/%02d/%02d %02d:%02d:%02d,", EtTime.date().year(), EtTime.date().month(), EtTime.date().day()
                        , EtTime.time().hour(), EtTime.time().minute(), EtTime.time().second());

        stream << strTime + Buffer + "\n";
	}
    catch (...)
	{
        JnlFile.close();
		return -1;
	}
    JnlFile.close();
	return 0;  
}

SHORT CMJNL::N_DeleteOldFile()
{
    QDateTime EtTime = QDateTime::currentDateTime();
    //CFileFind filefind;
    QDateTime     ctExpireTime;
    QString   CurrentFilename;
    ctExpireTime  =EtTime.addDays(-m_JsLogSaveDays);
    QString sExpireFileName;
    sExpireFileName.sprintf("%04d%02d%02d",ctExpireTime.date().year(),ctExpireTime.date().month(),ctExpireTime.date().day());
    QDir::setCurrent(m_JsLogPath);
    //::SetCurrentDirectory(m_JsLogPath);
    QDir dir(m_JsLogPath);
    QStringList filters;
    filters << "*.txt";//设置过滤类型
    dir.setNameFilters(filters);//设置文件名的过滤
    QFileInfoList list = dir.entryInfoList();
    if(list.length()!=0)
    {
       for (int i = 0; i < list.size(); ++i)
        {
            // compare date
           CurrentFilename = list.at(i).fileName();
           CurrentFilename = CurrentFilename.mid(5, 8);
           if(CurrentFilename <= sExpireFileName)
           {
               CurrentFilename = MJNL_JSLOGFILENAMEPREFIX +CurrentFilename+ ".txt";
               //LPSTR lpTemp = CurrentFilename.GetBuffer(CurrentFilename.length());
               dir.remove(m_JsLogPath + CurrentFilename);
               //CurrentFilename.ReleaseBuffer();
           }
        }
    }
	return 0;
}

SHORT CMJNL::N_ElectDeleteOldFile()
{
    QDateTime EtTime = QDateTime::currentDateTime();
    //CFileFind filefind;
    QDateTime     ctExpireTime;
    QString   CurrentFilename;
    ctExpireTime  =EtTime.addDays(-m_JnlSaveDays);
    QString sExpireFileName;
    sExpireFileName.sprintf("%04d%02d%02d",ctExpireTime.date().year(),ctExpireTime.date().month(),ctExpireTime.date().day());
    QDir::setCurrent(m_strJnlPath);
    QDir dir(m_strJnlPath);
    QStringList filters;
    filters << "*.TXT";//设置过滤类型
    dir.setNameFilters(filters);//设置文件名的过滤
    QFileInfoList list = dir.entryInfoList();
    if(list.length()!=0)
    {
       for (int i = 0; i < list.size(); ++i)
        {
            // compare date
           CurrentFilename = list.at(i).fileName();
           CurrentFilename = CurrentFilename.left(8);
           if(CurrentFilename <= sExpireFileName)
           {
               CurrentFilename += ".txt";
               dir.remove(m_strJnlPath + CurrentFilename);
           }
        }
    }
	return 0;
}

bool CMJNL::N_GetDataFromDSTO(BOOL bPersistent, QString DataName, QString &Value)
{
	LOGINFO("函数：%s",__FUNCTION__);

	char* sValue = NULL;
	sValue = new char[10*1024];
	if(sValue)
	{
		memset(sValue, 0x00, 10*1024);
        short nRet = dllDSTO.D_GetDataString(bPersistent, Q2S(DataName), sValue);

        Value = S2Q(sValue);
		delete [] sValue;


		if(nRet!=0)
		{
            LOGINFO("函数：%s，ValueName = %s GetValueFalse",__FUNCTION__, Q2S(DataName));
			return false;
		}
	}
	else
	{
		return false;
	}

    LOGINFO("函数：%s，ValueName = %s, bPersistent = %s, Value = %s",__FUNCTION__, Q2S(DataName), ((bPersistent)?("true"):("false")), Q2S(Value));
	return true;
}
bool CMJNL::N_GetDataFromDSTO(BOOL bPersistent, QString DataName, long &Value)
{
	LOGINFO("函数：%s",__FUNCTION__);

    short nRet = dllDSTO.D_GetDataLong(bPersistent, Q2S(DataName), &Value);

	if(nRet!=0)
	{
        LOGINFO("函数：%s，ValueName = %s GetValueFalse",__FUNCTION__, Q2S(DataName));
		return false;
	}

    LOGINFO("函数：%s，ValueName = %s, bPersistent = %s, Value = %d",__FUNCTION__, Q2S(DataName), ((bPersistent)?("true"):("false")),Value);

	return true;
}
bool CMJNL::N_GetDataFromDSTO(BOOL bPersistent, QString DataName, LPBYTE Value)
{
	LOGINFO("函数：%s",__FUNCTION__);

    short nRet = dllDSTO.D_GetDataBytes(bPersistent, Q2S(DataName), Value);

	if(nRet!=0)
	{
        LOGINFO("函数：%s，ValueName = %s GetValueFalse",__FUNCTION__, Q2S(DataName));
		return false;
	}

    LOGINFO("函数：%s，ValueName = %s, bPersistent = %s, Value = %s",__FUNCTION__, Q2S(DataName), ((bPersistent)?("true"):("false")), Value);

	return true;
}

//bool CMJNL::N_CreateFileDirectory(TCHAR* szPath)
//{
//	TCHAR DirName[MAX_PATH];
//	TCHAR* p = szPath;
//	TCHAR* q = DirName;
//	while ( *p )
//	{
//		if ( ('\\' == *p) || ('/' == *p) )
//		{
//			if ( ':' != *(p-1) )
//			{
//				CreateDirectory(DirName, NULL);
//			}
//		}
//		*q++ = *p++;
//		*q = '\0';
//	}
//	CreateDirectory(DirName, NULL);
//	return 0;
//}


QObject *CreateInstance()
{
    return CMJNL::GetInstance();
}
