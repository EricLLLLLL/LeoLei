///////////////////////////
//		Create by art
//////////////////////////

#pragma once

#include <QObject>
#include <QString>
#include "easytrace.h"
#include "dllDSTO.h"
#include "win_type_def.h"
#include "dsnd_global.h"

enum TRADE_TYPE
{ //create by art
	enTradeType_NONE=0,
	enTradeType_INQ=1, 
	enTradeType_CWD,
	enTradeType_DEP,
	enTradeType_CWC,
	enTradeType_QRYCUST,
	enTradeType_APPLYWORKPARAM,
	enTradeType_NOCARDDEP,
	enTradeType_QRYHOSTAMOUNT,  //8  查询主机现金箱余额 
	enTradeType_PWD,  //改密 
	enTradeType_QRYACCOUNTSTATE,  //查询账户状态 
	enTradeType_QRYBUSSINESSACCOUNT,  //11 查询对公账户状态 
	enTradeType_BUSSINESSDEP,  //12 对公存款交易
	enTradeType_QRYCWDMONEY,  //13  查询客户已取金额
	enTradeType_QRYACCOUNTTYPE, // 14 查询卡类型
	enTradeType_QRYACCOUNTCANCELLATION,  //15  销户查询 2017.9.21.1
	enTradeType_ACCOUNTCANCELLATION,	 //16  销户
};


class DSNDSHARED_EXPORT CDSND : public QObject
{
    Q_OBJECT
public:
    CDSND(QObject* pParent=NULL);
    ~CDSND(void);

    static CDSND *GetInstance(void);

protected:

public:
	long PackageData(const char *pszDealType, char *pszBuffer, long *pnBufLen, char *pszMacData, long *pnMacDataLen); //create by art

private:
	CDllDSTO dllDSTO;

    static CDSND *m_pInstance;

private:

	void AscToBcd(unsigned char *BcdData,char *AscData,const int AscDataLen);
	void BcdToAsc(char *AscData,unsigned char *BcdData,const int AcsDataLen);
	void SetBitMap(char *BitMap,short FiledsNum, bool IsSet = true);

	short GetDataString(BOOL bPersistent, char *pszDataName, char *pszValue);
	short GetDataLong(BOOL bPersistent, char *pszDataName, long *plongValue);
	short InitDataString(BOOL bPersistent, char *pszDataName, char *pszNewValue);

	int CreateMsg(const TRADE_TYPE enType, char *pszOutData,int &nOutDataLen,char *pszOutMacData,int &nOutMacDataLen); //create by art

};

extern "C" {
    DSNDSHARED_EXPORT QObject* CreateInstance();
}

