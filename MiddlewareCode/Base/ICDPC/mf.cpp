#include "mf.h"
#include "QString"
#include "QDateTime"
int MFile::mfinit()
{
    QString filelog;
    QString strDate = QDateTime::currentDateTime().toString("yyyyMMdd");
    filelog.sprintf("C:\\DATA\\ICDPC\\%s_log.txt",Q2S(strDate));//	sprintf(tmpBuf,"SBODBC.LOG.%s.txt",);
    fp=fopen(Q2S(filelog),"a");
	if(fp==NULL)
	return 1;
	return 0;
}
int MFile::mfwrite(char* content)
{
    QDateTime tStart = QDateTime::currentDateTime();
    QString strTime = tStart.toString("yyyyMMdd HH:mm:ss");
    QString nstr;
    nstr.sprintf("[%s] %s\r\n",Q2S(strTime),content);
	if(fp)
	{
        fwrite(Q2S(nstr),1,nstr.length(),fp);
		fflush(fp);			
	}
	return 0;
}
int MFile::mfclose()
{
	if(fp)
	{
		fclose(fp);
	}		
	return 0;
}
