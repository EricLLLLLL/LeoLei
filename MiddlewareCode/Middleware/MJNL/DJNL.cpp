#include "stdafx.h"
#include "DJNL.h"

DSTOField ApplyWorkParam[]={
	{"JNLNUM",true,"LONG",0},
	{"SUBBANKNUM",true,"STRING",0},
	{"TERMINALNUM",true,"STRING",0},
	{"RESPONSECODE",false,"STRING",0},
	{"TRANSACTIONRESULT",false,"STRING",0},
};

DSTOField BeforeSendDisposal[]={
	{"JNLNUM",true,"LONG",0},
	{"TRANSKIND",false,"STRING",0},
};

DSTOField Transaction[]={
	{"RESPONSECODE",false,"STRING",0},
	{"TRANSACTIONRESULT",false,"STRING",0},
};

DSTOField AccountAction[]={
	{"CARDNO",false,"STRING",0},
};
DSTOField BusinessAccountAction[]={
	{"CARDNO",false,"STRING",0},
};
DSTOField PassbookAction[]={
	{"CARDNO",false,"STRING",0},
};
DSTOField ErrorEjectCardAction[]={
	{"CARDNO",false,"STRING",0},
};
DSTOField CaptureCardAction[]={
	{"CARDNO",false,"STRING",0},
};
DSTOField InsertCardAction[]={
	{"CARDNO",false,"STRING",0},
};
DSTOField ReadCardAction[]={
	{"CARDNO",false,"STRING",0},
};
DSTOField TakeCardAction[]={
	{"CARDNO",false,"STRING",0},
};

DSTOField CashOutBox1[]={
	{"RESPONSECODE",false,"STRING",0},
	{"TRANSACTIONRESULT",false,"STRING",0},
	{"TRANSAMOUNT",false,"STRING",0},
	{"POUNDAGE",false,"STRING", 0},
};

DSTOField CashOutBox2[]={
	{"SINGLEDISPENSEMONEY",false,"STRING", 0},
};

DSTOField CashOutBox3[]={
	{"CONTENTDATA",false,"STRING", 0},
};

DSTOField Content[]={
	{"CONTENTDATA",false,"STRING", 0},
};

DSTOField CashInBox[]={
	{"RESPONSECODE",false,"STRING",0},
	{"TRANSACTIONRESULT",false,"STRING",0},
	{"TRANSAMOUNT",false,"STRING",0},
	{"POUNDAGE",false,"STRING",0},
	{"CONTENTDATA",false,"STRING", 0},
};

DSTOField AddCashNote[]={          
	{"JNLNUM",true,"LONG",0},
	{"SUBBANKNUM",true,"STRING",0},
	{"TERMINALNUM",true,"STRING",0},
	{"BOX1TYPEFMT",true,"STRING",6},
	{"BOX2TYPEFMT",true,"STRING",6},
	{"BOX3TYPEFMT",true,"STRING",6},
	{"BOX4TYPEFMT",true,"STRING",6},
	{"BOX5TYPEFMT",true,"STRING",6},
	{"BOX6TYPEFMT",true,"STRING",6},
	{"BOX7TYPEFMT",true,"STRING",6},
	{"BOX1VALUEFMT",true,"STRING",6},
	{"BOX2VALUEFMT",true,"STRING",6},
	{"BOX3VALUEFMT",true,"STRING",6},
	{"BOX4VALUEFMT",true,"STRING",6},
	{"BOX5VALUEFMT",true,"STRING",6},
	{"BOX6VALUEFMT",true,"STRING",6},
	{"BOX7VALUEFMT",true,"STRING",6},
	{"ENCASEBILLS1",true,"STRING",6},
	{"ENCASEBILLS2",true,"STRING",6},
	{"ENCASEBILLS3",true,"STRING",6},
	{"ENCASEBILLS4",true,"STRING",6},
	{"ENCASEBILLS5",true,"STRING",6},
	{"ENCASEBILLS6",true,"STRING",6},
	{"ENCASEBILLS7",true,"STRING",6}
};

DSTOField MachineSettle[]={          
	{"JNLNUM",true,"LONG",0},
	{"TRANSKIND",false,"STRING",0},
	{"SUBBANKNUM",true,"STRING",0},
	{"TERMINALNUM",true,"STRING",0},
	{"BOX1TYPEFMT",true,"STRING",6},
	{"BOX2TYPEFMT",true,"STRING",6},
	{"BOX3TYPEFMT",true,"STRING",6},
	{"BOX4TYPEFMT",true,"STRING",6},
	{"BOX5TYPEFMT",true,"STRING",6},
	{"BOX6TYPEFMT",true,"STRING",6},
	{"BOX7TYPEFMT",true,"STRING",6},
	{"BOX1VALUEFMT",true,"STRING",6},
	{"BOX2VALUEFMT",true,"STRING",6},
	{"BOX3VALUEFMT",true,"STRING",6},
	{"BOX4VALUEFMT",true,"STRING",6},
	{"BOX5VALUEFMT",true,"STRING",6},
	{"BOX6VALUEFMT",true,"STRING",6},
	{"BOX7VALUEFMT",true,"STRING",6},
	{"ENCASEBILLS1",true,"STRING",6},
	{"ENCASEBILLS2",true,"STRING",6},
	{"ENCASEBILLS3",true,"STRING",6},
	{"ENCASEBILLS4",true,"STRING",6},
	{"ENCASEBILLS5",true,"STRING",6},
	{"ENCASEBILLS6",true,"STRING",6},
	{"ENCASEBILLS7",true,"STRING",6},
	{"BOX1NOTENUMBERFMT",true,"STRING",6},
	{"BOX2NOTENUMBERFMT",true,"STRING",6},
	{"BOX3NOTENUMBERFMT",true,"STRING",6},
	{"BOX4NOTENUMBERFMT",true,"STRING",6},
	{"BOX5NOTENUMBERFMT",true,"STRING",6},
	{"BOX6NOTENUMBERFMT",true,"STRING",6},
	{"BOX7NOTENUMBERFMT",true,"STRING",6},
	{"ATMC_BOX1DISPENSEFMT",true,"STRING",6},
	{"ATMC_BOX2DISPENSEFMT",true,"STRING",6},
	{"ATMC_BOX3DISPENSEFMT",true,"STRING",6},
	{"ATMC_BOX4DISPENSEFMT",true,"STRING",6},
	{"ATMC_BOX5DISPENSEFMT",true,"STRING",6},
	{"ATMC_BOX6DISPENSEFMT",true,"STRING",6},
	{"ATMC_BOX7DISPENSEFMT",true,"STRING",6},
	{"ATMC_BOX1DISPENSERJFMT",true,"STRING",6},
	{"ATMC_BOX2DISPENSERJFMT",true,"STRING",6},
	{"ATMC_BOX3DISPENSERJFMT",true,"STRING",6},
	{"ATMC_BOX4DISPENSERJFMT",true,"STRING",6},
	{"ATMC_BOX5DISPENSERJFMT",true,"STRING",6},
	{"ATMC_BOX6DISPENSERJFMT",true,"STRING",6},
	{"ATMC_BOX7DISPENSERJFMT",true,"STRING",6},
	{"ATMC_BOX1DEPOSITFMT",true,"STRING",6},
	{"ATMC_BOX2DEPOSITFMT",true,"STRING",6},
	{"ATMC_BOX3DEPOSITFMT",true,"STRING",6},
	{"ATMC_BOX4DEPOSITFMT",true,"STRING",6},
	{"ATMC_BOX5DEPOSITFMT",true,"STRING",6},
	{"ATMC_BOX6DEPOSITFMT",true,"STRING",6},
	{"ATMC_BOX7DEPOSITFMT",true,"STRING",6},
	{"HOSTAMOUNT",false,"STRING",20},//主机现金箱余额
	{"DEPCOUNT",false,"STRING",6},
	{"CWDCOUNT",false,"STRING",6},
	{"DEPAMOUNT",false,"STRING",15},
	{"CWDAMOUNT",false,"STRING",15},
};

DSTOField ExchangeMoney[]={         
	{"BOX1TYPEFMT",true,"STRING",6},
	{"BOX2TYPEFMT",true,"STRING",6},
	{"BOX3TYPEFMT",true,"STRING",6},
	{"BOX4TYPEFMT",true,"STRING",6},
	{"BOX5TYPEFMT",true,"STRING",6},
	{"BOX6TYPEFMT",true,"STRING",6},
	{"BOX7TYPEFMT",true,"STRING",6},
	{"BOX1VALUEFMT",true,"STRING",6},
	{"BOX2VALUEFMT",true,"STRING",6},
	{"BOX3VALUEFMT",true,"STRING",6},
	{"BOX4VALUEFMT",true,"STRING",6},
	{"BOX5VALUEFMT",true,"STRING",6},
	{"BOX6VALUEFMT",true,"STRING",6},
	{"BOX7VALUEFMT",true,"STRING",6}
};

DSTOField WithdrawCorrect[]={
	{"JNLNUM",true,"LONG",0},
	{"CWCJNLNUM",true,"LONG",0}
};

DSTOField QueryAccInfo[]={
	{"JNLNUM",true,"LONG",0}
};

DSTOField PhoneNum[]={
	{"CONTENTDATA",false,"STRING", 0},
};

DSTOField TfrComplete[]={
	{"RESPONSECODE",false,"STRING", 0}, 
	{"TRANSACTIONRESULT",false,"STRING", 0}, 
	{"TRANSAMOUNT",false,"STRING", 0}, 
	{"TFRCARDNO",false,"STRING", 0}, 
};

// add by zwh for 转账汇款交易相关流水
DSTOField TransferComplete[]={
	{"JNLNUM",true,"LONG",0},
	{"TRANSKIND",false,"STRING",0},
	{"RESPONSECODE",false,"STRING",0},
	{"TRANSACTIONRESULT",false,"STRING",0},
	{"CARDNO",false,"STRING",0},
	{"CUSTOMERNAME",false,"STRING",0},
	{"TFRBANK",false,"STRING",0},
	{"TFRCARDNO",false,"STRING",0},
	{"TFRCUSTNAME",false,"STRING",0},
	{"TRANSAMOUNT",false,"STRING",0},
	{"POUNDAGE",false,"STRING", 0},
};

DSTOField CTRComplete[]={
	{"JNLNUM",true,"LONG",0},
	{"TRANSKIND",false,"STRING",0},
	{"RESPONSECODE",false,"STRING",0},
	{"TRANSACTIONRESULT",false,"STRING",0},
	{"CARDNO",false,"STRING",0},
	{"CUSTOMERNAME",false,"STRING",0},
	{"CURRENCY",false,"STRING",0},
	{"TRANSAMOUNT",false,"STRING",0},
	{"DEPOSIT",false,"STRING",0},
	{"DEALTYPEFLAG",false,"STRING",0},
	{"AGREEMENT",false,"STRING",0},
};

DSTOField RTCComplete[]={
	{"JNLNUM",true,"LONG",0},
	{"TRANSKIND",false,"STRING",0},
	{"RESPONSECODE",false,"STRING",0},
	{"TRANSACTIONRESULT",false,"STRING",0},
	{"CARDNO",false,"STRING",0},
	{"CUSTOMERNAME",false,"STRING",0},
	{"CURRENCY",false,"STRING",0},
	{"WITHDRAWING",false,"STRING",0},
	{"TRANSAMOUNT",false,"STRING",0},
	{"INTEREST",false,"STRING",0},
	{"TOTAL",false,"STRING",0},
};

DSTOField SalesComplete[]={
	{"JNLNUM",true,"LONG",0},
	{"TRANSKIND",false,"STRING",0},
	{"RESPONSECODE",false,"STRING",0},
	{"TRANSACTIONRESULT",false,"STRING",0},
	{"CARDNO",false,"STRING",20},				
	{"CUSTOMERNAME",false,"STRING",20},
	{"DEALTYPE",false,"STRING",22},
	{"TRANSAMOUNT",false,"STRING",20},
	{"INTEREST",false,"STRING",20},
	{"TAXABLE",false,"STRING",20},
	{"TAXRATE",false,"STRING",20},			
	{"TAX",false,"STRING",20},		
	{"TOTAL",false,"STRING",20},		
	{"TRANSFERINACC",false,"STRING",20},	
	{"TFRCUSTNAME",false,"STRING",20},	
	{"POUNDAGE",false,"STRING",20},		
};

DSTOField SalesComplete2[]={
	{"JNLNUM",true,"LONG",0},
	{"TRANSKIND",false,"STRING",0},
	{"RESPONSECODE",false,"STRING",0},
	{"TRANSACTIONRESULT",false,"STRING",0},
	{"CARDNO",false,"STRING",20},				
	{"CUSTOMERNAME",false,"STRING",20},
	{"TRANSFERINACC",false,"STRING",20},	
};

// add by zwh for 取款回存交易相关流水
DSTOField MachineSettle_HaveReDep[]={          
	{"JNLNUM",true,"LONG",0},
	{"TRANSKIND",false,"STRING",0},
	{"SUBBANKNUM",true,"STRING",0},
	{"TERMINALNUM",true,"STRING",0},
	{"BOX1TYPEFMT",true,"STRING",6},
	{"BOX2TYPEFMT",true,"STRING",6},
	{"BOX3TYPEFMT",true,"STRING",6},
	{"BOX4TYPEFMT",true,"STRING",6},
	{"BOX5TYPEFMT",true,"STRING",6},
	{"BOX6TYPEFMT",true,"STRING",6},
	{"BOX7TYPEFMT",true,"STRING",6},
	{"BOX1VALUEFMT",true,"STRING",6},
	{"BOX2VALUEFMT",true,"STRING",6},
	{"BOX3VALUEFMT",true,"STRING",6},
	{"BOX4VALUEFMT",true,"STRING",6},
	{"BOX5VALUEFMT",true,"STRING",6},
	{"BOX6VALUEFMT",true,"STRING",6},
	{"BOX7VALUEFMT",true,"STRING",6},
	{"ENCASEBILLS1",true,"STRING",6},
	{"ENCASEBILLS2",true,"STRING",6},
	{"ENCASEBILLS3",true,"STRING",6},
	{"ENCASEBILLS4",true,"STRING",6},
	{"ENCASEBILLS5",true,"STRING",6},
	{"ENCASEBILLS6",true,"STRING",6},
	{"ENCASEBILLS7",true,"STRING",6},
	{"BOX1NOTENUMBERFMT",true,"STRING",6},
	{"BOX2NOTENUMBERFMT",true,"STRING",6},
	{"BOX3NOTENUMBERFMT",true,"STRING",6},
	{"BOX4NOTENUMBERFMT",true,"STRING",6},
	{"BOX5NOTENUMBERFMT",true,"STRING",6},
	{"BOX6NOTENUMBERFMT",true,"STRING",6},
	{"BOX7NOTENUMBERFMT",true,"STRING",6},
	{"ATMC_BOX1DISPENSEFMT",true,"STRING",6},
	{"ATMC_BOX2DISPENSEFMT",true,"STRING",6},
	{"ATMC_BOX3DISPENSEFMT",true,"STRING",6},
	{"ATMC_BOX4DISPENSEFMT",true,"STRING",6},
	{"ATMC_BOX5DISPENSEFMT",true,"STRING",6},
	{"ATMC_BOX6DISPENSEFMT",true,"STRING",6},
	{"ATMC_BOX7DISPENSEFMT",true,"STRING",6},
	{"ATMC_BOX1DISPENSERJFMT",true,"STRING",6},
	{"ATMC_BOX2DISPENSERJFMT",true,"STRING",6},
	{"ATMC_BOX3DISPENSERJFMT",true,"STRING",6},
	{"ATMC_BOX4DISPENSERJFMT",true,"STRING",6},
	{"ATMC_BOX5DISPENSERJFMT",true,"STRING",6},
	{"ATMC_BOX6DISPENSERJFMT",true,"STRING",6},
	{"ATMC_BOX7DISPENSERJFMT",true,"STRING",6},
	{"ATMC_BOX1DEPOSITFMT",true,"STRING",6},
	{"ATMC_BOX2DEPOSITFMT",true,"STRING",6},
	{"ATMC_BOX3DEPOSITFMT",true,"STRING",6},
	{"ATMC_BOX4DEPOSITFMT",true,"STRING",6},
	{"ATMC_BOX5DEPOSITFMT",true,"STRING",6},
	{"ATMC_BOX6DEPOSITFMT",true,"STRING",6},
	{"ATMC_BOX7DEPOSITFMT",true,"STRING",6},
	{"HOSTAMOUNT",false,"STRING",20},//主机现金箱余额
	{"DEPCOUNT",false,"STRING",6},
	{"CWDCOUNT",false,"STRING",6},
	{"DEPAMOUNT",false,"STRING",15},
	{"CWDAMOUNT",false,"STRING",15},
	{"ALLCLOSETRANS",false,"STRING",40}
};

// add by zwh for 存款失败自助取回
DSTOField DepErrComplete[]={ 
	{"RESETRESULTS", false, "STRING", 0},
};

DSTOForm dstoForm[] = {
	{"ApplyWorkParam",ApplyWorkParam, sizeof(ApplyWorkParam)/sizeof(DSTOField)},
	{"BeforeSendDisposal",BeforeSendDisposal, sizeof(BeforeSendDisposal)/sizeof(DSTOField)},
	{"Transaction",Transaction, sizeof(Transaction)/sizeof(DSTOField)},
    {"CaptureCardAction",CaptureCardAction, sizeof(CaptureCardAction)/sizeof(DSTOField)},
	{"InsertCardAction",InsertCardAction, sizeof(InsertCardAction)/sizeof(DSTOField)},
	{"ReadCardAction",ReadCardAction, sizeof(ReadCardAction)/sizeof(DSTOField)},
	{"AccountAction",AccountAction, sizeof(AccountAction)/sizeof(DSTOField)},
	{"BusinessAccountAction",BusinessAccountAction, sizeof(BusinessAccountAction)/sizeof(DSTOField)},
	{"PassbookAction",PassbookAction, sizeof(PassbookAction)/sizeof(DSTOField)},
	{"TakeCardAction",TakeCardAction, sizeof(TakeCardAction)/sizeof(DSTOField)},
	{"ErrorEjectCardAction",ErrorEjectCardAction, sizeof(ErrorEjectCardAction)/sizeof(DSTOField)},
	{"CashOutBox1",CashOutBox1, sizeof(CashOutBox1)/sizeof(DSTOField)},
	{"CashOutBox2",CashOutBox2, sizeof(CashOutBox2)/sizeof(DSTOField)},
	{"CashOutBox3",CashOutBox3, sizeof(CashOutBox3)/sizeof(DSTOField)},
	{"Content",Content, sizeof(Content)/sizeof(DSTOField)},
	{"CashInBox",CashInBox, sizeof(CashInBox)/sizeof(DSTOField)},
	{"ExchangeMoney",ExchangeMoney, sizeof(ExchangeMoney)/sizeof(DSTOField)},
	{"MachineSettle",MachineSettle, sizeof(MachineSettle)/sizeof(DSTOField)},
	{"AddCashNote",AddCashNote, sizeof(AddCashNote)/sizeof(DSTOField)},
	{"WithdrawCorrect",WithdrawCorrect, sizeof(WithdrawCorrect)/sizeof(DSTOField)},
	{"QueryAccInfo",QueryAccInfo, sizeof(QueryAccInfo)/sizeof(DSTOField)},
	{"PhoneNum",PhoneNum, sizeof(PhoneNum)/sizeof(DSTOField)},
	{"TfrComplete",TfrComplete, sizeof(TfrComplete)/sizeof(DSTOField)},
	{"TransferComplete",TransferComplete, sizeof(TransferComplete)/sizeof(DSTOField)},
	{"MachineSettle_HaveReDep", MachineSettle_HaveReDep, sizeof(MachineSettle_HaveReDep)/sizeof(DSTOField)},
	{"CTRComplete", CTRComplete, sizeof(CTRComplete)/sizeof(DSTOField)},
	{"RTCComplete", RTCComplete, sizeof(RTCComplete)/sizeof(DSTOField)},
	{"DepErrComplete", DepErrComplete, sizeof(DepErrComplete)/sizeof(DSTOField)},
	{"SalesComplete", SalesComplete, sizeof(SalesComplete)/sizeof(DSTOField)},
	{"SalesComplete2", SalesComplete2, sizeof(SalesComplete2)/sizeof(DSTOField)},
};

int FormNum = sizeof(dstoForm)/sizeof(DSTOForm);
