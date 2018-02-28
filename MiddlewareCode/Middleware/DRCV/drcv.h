///////////////////////////
//		Create by art
//////////////////////////

#pragma once

#include "easytrace.h"
#include "dllDSTO.h"
#include "drcv_global.h"
#include <QString>
#include <QObject>

class DRCVSHARED_EXPORT CDRCV : public QObject
{
     Q_OBJECT
public:
    CDRCV(QObject* pParent=NULL);
    ~CDRCV(void);

    static CDRCV *GetInstance(void);

protected:

public:
	long ParseData(const char *pszDealType, const char *pszBuffer, const long nBufLen);

private:
	CDllDSTO dllDSTO;  
    QString m_strConfigPath;

    static CDRCV *m_pInstance;

	void BcdToAsc(char *AscData,unsigned char *BcdData,const int AcsDataLen);
	void AscToBcd(unsigned char *BcdData,char *AscData,const int AscDataLen);

	short SetDataResult(BOOL bPersistent, char *pszDataName, char *pszNewValue);
	
	int IsValidTransType(const char *pszDealType, const char *pszMsgType, const char *pszProcessCode);
};

extern "C" {
    DRCVSHARED_EXPORT QObject* CreateInstance();
}
