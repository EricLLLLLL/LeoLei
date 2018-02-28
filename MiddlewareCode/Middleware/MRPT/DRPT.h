#ifndef DPRT_H
#define DPRT_H

//#include <AFX.h>
#include "mrpt.h"
#include "QString"
typedef struct{
	
    QString FieldName;
	BOOL bPersistent;
    QString DataType;
	int SizeX;
}DSTOField;
	
typedef struct{
	
    QString FormName;
	DSTOField* lpdstoField;
	int FieldNum;
}DSTOForm;


extern int FormNum;
extern DSTOForm dstoForm[];

extern DSTOField ReceiptCash_Print_szABC[];
extern DSTOField TOTAL_szABC[];
extern DSTOField SUBTOTAL_szABC[];
extern DSTOField ReceiptBillLoadingCountSet_szABC[];
extern DSTOField ExChangeMoney_Print_szABC[];
extern DSTOField RejectCash_Print_szABC[];
extern DSTOField Receipt_ReDEPCash_szABC[];
extern DSTOField Receipt_Trans_szABC[];
extern DSTOField Receipt_CTR_szABC[];
extern DSTOField Receipt_RTC_szABC[];
extern DSTOField TOTAL_szABC_haveReDep[];
extern DSTOField SUBTOTAL_szABC_haveReDep[];
extern DSTOField Receipt_Sales_szABC[];
extern DSTOField Receipt_SalesForWX_szABC[];

#endif
