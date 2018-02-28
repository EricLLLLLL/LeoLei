#ifndef DJNL_H
#define DJNL_H

//#include <AFX.h>
#include "mjnl.h"
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

extern DSTOField Content[];
extern DSTOField CashInBox[];
extern DSTOField CashOutBox1[];
extern DSTOField CashOutBox2[];
extern DSTOField CashOutBox3[];
extern DSTOField WithdrawCorrect[];
extern DSTOField ApplyWorkParam[];
extern DSTOField BeforeSendDisposal[];
extern DSTOField QueryAccInfo[];
extern DSTOField AddCashNote[];
extern DSTOField MachineSettle[];
extern DSTOField ExchangeMoney[];
extern DSTOField Transaction[];
extern DSTOField CaptureCardAction[];
extern DSTOField InsertCardAction[];
extern DSTOField ReadCardAction[];
extern DSTOField AccountAction[];
extern DSTOField BusinessAccountAction[];
extern DSTOField PassbookAction[];
extern DSTOField TakeCardAction[];
extern DSTOField ErrorEjectCardAction[];
extern DSTOField PhoneNum[];
extern DSTOField TfrComplete[];
#endif
