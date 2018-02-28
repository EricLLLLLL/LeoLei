#include "stdafx.h"
#include "DRPT.h"


DSTOField ReceiptCash_Print_szABC[]={
	{"Date",false,"STRING",12},
	{"Time",false,"STRING",8},
	{"CARDNO",false,"STRING",20},
	{"SUBBANKNUM",true,"STRING",22},
	{"TERMINALNUM",true,"STRING",20},
	{"DEALTYPE",false,"STRING",22},
	{"AUTHCODE",false,"STRING",20},
	{"TRANSAMOUNT",false,"STRING",22},
	{"POUNDAGE",false,"STRING",20},
	{"TFRCARDNO",false,"STRING",22},
	{"SYSREFNUM",false,"STRING",20},
	{"JNLNUM",true,"LONG",15},
	{"RESPONSECODE",false,"STRING",15},
	{"ICSERIALNUM",false,"STRING",18},
	{"TRANSACTIONRESULT",false,"STRING",15},
	{"COMMENTS",false,"STRING",42}
};

DSTOField RejectCash_Print_szABC[]={
	{"Date",false,"STRING",12},
	{"Time",false,"STRING",8},
	{"CARDNO",false,"STRING",20},
	{"SUBBANKNUM",true,"STRING",22},
	{"TERMINALNUM",true,"STRING",20},
	{"DEALTYPE",false,"STRING",22},
	{"COMMENTS",false,"STRING",42}
};

DSTOField TOTAL_szABC[]={ //modify by art for 双室钞箱,加6、7号箱,每钞箱数据宽度改为5
	{"Date",false,"STRING",12},
	{"Time",false,"STRING",8},
	{"SUBBANKNUM",true,"STRING",22},
	{"TERMINALNUM",true,"STRING",20},
	{"BOX1TYPEFMT",true,"STRING",5},
	{"BOX2TYPEFMT",true,"STRING",5},
	{"BOX3TYPEFMT",true,"STRING",5},
	{"BOX4TYPEFMT",true,"STRING",5},
	{"BOX5TYPEFMT",true,"STRING",5},
	{"BOX6TYPEFMT",true,"STRING",5}, 
	{"BOX7TYPEFMT",true,"STRING",5},
	{"BOX1VALUEFMT",true,"STRING",5},
	{"BOX2VALUEFMT",true,"STRING",5},
	{"BOX3VALUEFMT",true,"STRING",5},
	{"BOX4VALUEFMT",true,"STRING",5},
	{"BOX5VALUEFMT",true,"STRING",5},
	{"BOX6VALUEFMT",true,"STRING",5},
	{"BOX7VALUEFMT",true,"STRING",5},
	{"ENCASEBILLS1",true,"STRING",5},
	{"ENCASEBILLS2",true,"STRING",5},
	{"ENCASEBILLS3",true,"STRING",5},
	{"ENCASEBILLS4",true,"STRING",5},
	{"ENCASEBILLS5",true,"STRING",5},
	{"ENCASEBILLS6",true,"STRING",5},
	{"ENCASEBILLS7",true,"STRING",5},
	{"BOX1NOTENUMBERFMT",true,"STRING",5},
	{"BOX2NOTENUMBERFMT",true,"STRING",5},
	{"BOX3NOTENUMBERFMT",true,"STRING",5},
	{"BOX4NOTENUMBERFMT",true,"STRING",5},
	{"BOX5NOTENUMBERFMT",true,"STRING",5},
	{"BOX6NOTENUMBERFMT",true,"STRING",5},
	{"BOX7NOTENUMBERFMT",true,"STRING",5},
	{"ATMC_BOX1DISPENSEFMT",true,"STRING",5},
	{"ATMC_BOX2DISPENSEFMT",true,"STRING",5},
	{"ATMC_BOX3DISPENSEFMT",true,"STRING",5},
	{"ATMC_BOX4DISPENSEFMT",true,"STRING",5},
	{"ATMC_BOX5DISPENSEFMT",true,"STRING",5},
	{"ATMC_BOX6DISPENSEFMT",true,"STRING",5},
	{"ATMC_BOX7DISPENSEFMT",true,"STRING",5},
	{"ATMC_BOX1DISPENSERJFMT",true,"STRING",5},
	{"ATMC_BOX2DISPENSERJFMT",true,"STRING",5},
	{"ATMC_BOX3DISPENSERJFMT",true,"STRING",5},
	{"ATMC_BOX4DISPENSERJFMT",true,"STRING",5},
	{"ATMC_BOX5DISPENSERJFMT",true,"STRING",5},
	{"ATMC_BOX6DISPENSERJFMT",true,"STRING",5},
	{"ATMC_BOX7DISPENSERJFMT",true,"STRING",5},
	{"ATMC_BOX1DEPOSITFMT",true,"STRING",5},
	{"ATMC_BOX2DEPOSITFMT",true,"STRING",5},
	{"ATMC_BOX3DEPOSITFMT",true,"STRING",5},
	{"ATMC_BOX4DEPOSITFMT",true,"STRING",5},
	{"ATMC_BOX5DEPOSITFMT",true,"STRING",5},
	{"ATMC_BOX6DEPOSITFMT",true,"STRING",5},
	{"ATMC_BOX7DEPOSITFMT",true,"STRING",5},
	{"HOSTAMOUNT",false,"STRING",20},			//add by art for 主机现金箱余额
	//add by art for 现金交易统计和异常交易记录 和吞卡记录 begin
	{"DEPCOUNT",false,"STRING",6},
	{"CWDCOUNT",false,"STRING",6},
	{"DEPAMOUNT",false,"STRING",20},
	{"CWDAMOUNT",false,"STRING",20},
	{"ALLERRTRANS",false,"STRING",40}, 
	{"CARDRETAINRECORD",false,"STRING",40}
	//add by art for 现金交易统计和异常交易记录 和吞卡记录 end
};

DSTOField SUBTOTAL_szABC[]={ //modify by art for 双室钞箱,加6、7号箱,每钞箱数据宽度改为5
	{"Date",false,"STRING",12},
	{"Time",false,"STRING",8},
	{"SUBBANKNUM",true,"STRING",22},
	{"TERMINALNUM",true,"STRING",20},
	{"BOX1TYPEFMT",true,"STRING",5},
	{"BOX2TYPEFMT",true,"STRING",5},
	{"BOX3TYPEFMT",true,"STRING",5},
	{"BOX4TYPEFMT",true,"STRING",5},
	{"BOX5TYPEFMT",true,"STRING",5},
	{"BOX6TYPEFMT",true,"STRING",5},
	{"BOX7TYPEFMT",true,"STRING",5},
	{"BOX1VALUEFMT",true,"STRING",5},
	{"BOX2VALUEFMT",true,"STRING",5},
	{"BOX3VALUEFMT",true,"STRING",5},
	{"BOX4VALUEFMT",true,"STRING",5},
	{"BOX5VALUEFMT",true,"STRING",5},
	{"BOX6VALUEFMT",true,"STRING",5},
	{"BOX7VALUEFMT",true,"STRING",5},
	{"ENCASEBILLS1",true,"STRING",5},
	{"ENCASEBILLS2",true,"STRING",5},
	{"ENCASEBILLS3",true,"STRING",5},
	{"ENCASEBILLS4",true,"STRING",5},
	{"ENCASEBILLS5",true,"STRING",5},
	{"ENCASEBILLS6",true,"STRING",5},
	{"ENCASEBILLS7",true,"STRING",5},
	{"BOX1NOTENUMBERFMT",true,"STRING",5},
	{"BOX2NOTENUMBERFMT",true,"STRING",5},
	{"BOX3NOTENUMBERFMT",true,"STRING",5},
	{"BOX4NOTENUMBERFMT",true,"STRING",5},
	{"BOX5NOTENUMBERFMT",true,"STRING",5},
	{"BOX6NOTENUMBERFMT",true,"STRING",5},
	{"BOX7NOTENUMBERFMT",true,"STRING",5},
	{"ATMC_BOX1DISPENSEFMT",true,"STRING",5},
	{"ATMC_BOX2DISPENSEFMT",true,"STRING",5},
	{"ATMC_BOX3DISPENSEFMT",true,"STRING",5},
	{"ATMC_BOX4DISPENSEFMT",true,"STRING",5},
	{"ATMC_BOX5DISPENSEFMT",true,"STRING",5},
	{"ATMC_BOX6DISPENSEFMT",true,"STRING",5},
	{"ATMC_BOX7DISPENSEFMT",true,"STRING",5},
	{"ATMC_BOX1DISPENSERJFMT",true,"STRING",5},
	{"ATMC_BOX2DISPENSERJFMT",true,"STRING",5},
	{"ATMC_BOX3DISPENSERJFMT",true,"STRING",5},
	{"ATMC_BOX4DISPENSERJFMT",true,"STRING",5},
	{"ATMC_BOX5DISPENSERJFMT",true,"STRING",5},
	{"ATMC_BOX6DISPENSERJFMT",true,"STRING",5},
	{"ATMC_BOX7DISPENSERJFMT",true,"STRING",5},
	{"ATMC_BOX1DEPOSITFMT",true,"STRING",5},
	{"ATMC_BOX2DEPOSITFMT",true,"STRING",5},
	{"ATMC_BOX3DEPOSITFMT",true,"STRING",5},
	{"ATMC_BOX4DEPOSITFMT",true,"STRING",5},
	{"ATMC_BOX5DEPOSITFMT",true,"STRING",5},
	{"ATMC_BOX6DEPOSITFMT",true,"STRING",5},
	{"ATMC_BOX7DEPOSITFMT",true,"STRING",5},
	{"HOSTAMOUNT",false,"STRING",20},			//add by art for 主机现金箱余额
	//add by art for 现金交易统计和异常交易记录 和吞卡记录 begin
	{"DEPCOUNT",false,"STRING",6},
	{"CWDCOUNT",false,"STRING",6},
	{"DEPAMOUNT",false,"STRING",20},
	{"CWDAMOUNT",false,"STRING",20},
	{"ALLERRTRANS",false,"STRING",40}, 
	{"CARDRETAINRECORD",false,"STRING",40} 
	//add by art for 现金交易统计和异常交易记录 和吞卡记录 end
};

DSTOField ReceiptBillLoadingCountSet_szABC[]={ //modify by art for 双室钞箱,加6、7号箱,每钞箱数据宽度改为5
	{"Date",false,"STRING",12},
	{"Time",false,"STRING",8},
	{"SUBBANKNUM",true,"STRING",22},
	{"TERMINALNUM",true,"STRING",20},
	{"BOX1TYPEFMT",true,"STRING",5},
	{"BOX2TYPEFMT",true,"STRING",5},
	{"BOX3TYPEFMT",true,"STRING",5},
	{"BOX4TYPEFMT",true,"STRING",5},
	{"BOX5TYPEFMT",true,"STRING",5},
	{"BOX6TYPEFMT",true,"STRING",5},
	{"BOX7TYPEFMT",true,"STRING",5},
	{"BOX1VALUEFMT",true,"STRING",5},
	{"BOX2VALUEFMT",true,"STRING",5},
	{"BOX3VALUEFMT",true,"STRING",5},
	{"BOX4VALUEFMT",true,"STRING",5},
	{"BOX5VALUEFMT",true,"STRING",5},
	{"BOX6VALUEFMT",true,"STRING",5},
	{"BOX7VALUEFMT",true,"STRING",5},
	{"ENCASEBILLS1",true,"STRING",5},
	{"ENCASEBILLS2",true,"STRING",5},
	{"ENCASEBILLS3",true,"STRING",5},
	{"ENCASEBILLS4",true,"STRING",5},
	{"ENCASEBILLS5",true,"STRING",5},
	{"ENCASEBILLS6",true,"STRING",5},
	{"ENCASEBILLS7",true,"STRING",5}
};

DSTOField ExChangeMoney_Print_szABC[]={
	{"Date",false,"STRING",12},
	{"Time",false,"STRING",8},
	{"SUBBANKNUM",true,"STRING",22},
	{"TERMINALNUM",true,"STRING",20},
	{"EXCHANGEBILLCOUNTIN",false,"STRING",32},
	{"EXCHANGEBILLCOUNTOUT",false,"STRING",32},
	{"TRANSAMOUNT",false,"STRING",23},
	{"TRANSACTIONRESULT",false,"STRING",15}   //add by art
};

// 取款失败金额回存凭条
DSTOField Receipt_ReDEPCash_szABC[]={
	{"Date",false,"STRING",12},					// 交易日期
	{"Time",false,"STRING",8},					// 交易时间
	{"CARDNO",false,"STRING",20},				// 交易卡号
	{"SUBBANKNUM",true,"STRING",22},			// 受理行号
	{"TERMINALNUM",true,"STRING",20},			// 设备编号
	{"DEALTYPE",false,"STRING",22},				// 交易类型
	{"SYSREFNUM",false,"STRING",20},			// 后台返回码
	{"JNLNUM2",false,"LONG",15},				// 取款交易流水号
	{"DISPENAMOUNT",false,"STRING",22},			// 取款金额
	{"TAKENAMOUNT",false,"STRING",22},			// 已取金额
	{"JNLNUM",true,"LONG",15},					// 取款回存流水号
	{"TRANSAMOUNT",false,"STRING",22},			// 回存金额
	{"RESPONSECODE",false,"STRING",15},			// 交易结果
	{"ICSERIALNUM",false,"STRING",18},			// IC卡序列号
	{"TRANSACTIONRESULT",false,"STRING",15},	// 响应码
	{"COMMENTS",false,"STRING",42}				// 备注
};

DSTOField Receipt_Trans_szABC[]={
	{"CARDNO",false,"STRING",20},				// 受理卡号
	{"CUSTOMERNAME",false,"STRING",20},			// 客户姓名
	{"SUBBANKNUM",true,"STRING",22},			// 受理行号
	{"TERMINALNUM",true,"STRING",20},			// 设备编号
	{"DEALTYPE",false,"STRING",22},				// 交易类型
	{"TREANSMETHOD",false,"STRING",22},			// 转存方式
	{"TFRBANK",false,"STRING",40},				// 收款行
	{"TFRCARDNO",false,"STRING",20},			// 收款人账号
	{"TFRCUSTNAME",false,"STRING",20},			// 收款人姓名
	{"TRANSAMOUNT",false,"STRING",22},			// 汇款金额
	{"REMITTANCE",false,"STRING",22},			// 汇款大写金额
	{"Date",false,"STRING",12},					// 交易日期
	{"Time",false,"STRING",8},					// 交易时间
	{"TRANSACTIONRESULT",false,"STRING",15},	// 交易结果
	{"JNLNUM",true,"LONG",12},					// 流水号
	{"SYSREFNUM",false,"STRING",20},			// 后台流水号
	{"RESPONSECODE",false,"STRING",15},			// 响应码
	{"COMMENTS",false,"STRING",42},				// 备注
	{"POUNDAGE",false,"STRING",20}				// 手续费
};

DSTOField Receipt_CTR_szABC[]={
	{"CARDNO",false,"STRING",20},				// 受理卡号
	{"CUSTOMERNAME",false,"STRING",20},			// 客户姓名
	{"SUBBANKNUM",true,"STRING",22},			// 受理行号
	{"TERMINALNUM",true,"STRING",20},			// 设备编号
	{"DEALTYPE",false,"STRING",22},				// 交易类型
	{"CURRENCY",false,"STRING",20},				// 币种
	{"TRANSAMOUNT",false,"STRING",20},			// 转存金额
	{"DEPOSIT",false,"STRING",20},				// 存期
	{"DEALTYPEFLAG",false,"STRING",20},			// 转存方式
	{"AGREEMENT",false,"STRING",20},			// 约定转存存期
	{"Date",false,"STRING",12},					// 交易日期
	{"Time",false,"STRING",8},					// 交易时间
	{"TRANSACTIONRESULT",false,"STRING",15},	// 交易结果
	{"JNLNUM",true,"LONG",12},					// 流水号
	{"SYSREFNUM",false,"STRING",20},			// 后台流水号
	{"RESPONSECODE",false,"STRING",15},			// 响应码
	{"COMMENTS",false,"STRING",42}				// 备注
};

DSTOField Receipt_RTC_szABC[]={
	{"CARDNO",false,"STRING",20},				// 受理卡号
	{"CUSTOMERNAME",false,"STRING",20},			// 客户姓名
	{"SUBBANKNUM",true,"STRING",22},			// 受理行号
	{"TERMINALNUM",true,"STRING",20},			// 设备编号
	{"DEALTYPE",false,"STRING",22},				// 交易类型
	{"CURRENCY",false,"STRING",20},				// 币种
	{"WITHDRAWING",false,"STRING",20},			// 支取方式
	{"TRANSAMOUNT",false,"STRING",20},			// 本金金额
	{"INTEREST",false,"STRING",20},				// 利息
	{"TOTAL",false,"STRING",20},				// 本息合计
	{"Date",false,"STRING",12},					// 交易日期
	{"Time",false,"STRING",8},					// 交易时间
	{"TRANSACTIONRESULT",false,"STRING",15},	// 交易结果
	{"JNLNUM",true,"LONG",12},					// 流水号
	{"SYSREFNUM",false,"STRING",20},			// 后台流水号
	{"RESPONSECODE",false,"STRING",15},			// 响应码
	{"COMMENTS",false,"STRING",42}				// 备注
};

DSTOField TOTAL_szABC_haveReDep[]={ // add by zwh for 相比之前的FORM，增加回存交易打印
	{"Date",false,"STRING",12},
	{"Time",false,"STRING",8},
	{"SUBBANKNUM",true,"STRING",22},
	{"TERMINALNUM",true,"STRING",20},
	{"BOX1TYPEFMT",true,"STRING",5},
	{"BOX2TYPEFMT",true,"STRING",5},
	{"BOX3TYPEFMT",true,"STRING",5},
	{"BOX4TYPEFMT",true,"STRING",5},
	{"BOX5TYPEFMT",true,"STRING",5},
	{"BOX6TYPEFMT",true,"STRING",5}, 
	{"BOX7TYPEFMT",true,"STRING",5},
	{"BOX1VALUEFMT",true,"STRING",5},
	{"BOX2VALUEFMT",true,"STRING",5},
	{"BOX3VALUEFMT",true,"STRING",5},
	{"BOX4VALUEFMT",true,"STRING",5},
	{"BOX5VALUEFMT",true,"STRING",5},
	{"BOX6VALUEFMT",true,"STRING",5},
	{"BOX7VALUEFMT",true,"STRING",5},
	{"ENCASEBILLS1",true,"STRING",5},
	{"ENCASEBILLS2",true,"STRING",5},
	{"ENCASEBILLS3",true,"STRING",5},
	{"ENCASEBILLS4",true,"STRING",5},
	{"ENCASEBILLS5",true,"STRING",5},
	{"ENCASEBILLS6",true,"STRING",5},
	{"ENCASEBILLS7",true,"STRING",5},
	{"BOX1NOTENUMBERFMT",true,"STRING",5},
	{"BOX2NOTENUMBERFMT",true,"STRING",5},
	{"BOX3NOTENUMBERFMT",true,"STRING",5},
	{"BOX4NOTENUMBERFMT",true,"STRING",5},
	{"BOX5NOTENUMBERFMT",true,"STRING",5},
	{"BOX6NOTENUMBERFMT",true,"STRING",5},
	{"BOX7NOTENUMBERFMT",true,"STRING",5},
	{"ATMC_BOX1DISPENSEFMT",true,"STRING",5},
	{"ATMC_BOX2DISPENSEFMT",true,"STRING",5},
	{"ATMC_BOX3DISPENSEFMT",true,"STRING",5},
	{"ATMC_BOX4DISPENSEFMT",true,"STRING",5},
	{"ATMC_BOX5DISPENSEFMT",true,"STRING",5},
	{"ATMC_BOX6DISPENSEFMT",true,"STRING",5},
	{"ATMC_BOX7DISPENSEFMT",true,"STRING",5},
	{"ATMC_BOX1DISPENSERJFMT",true,"STRING",5},
	{"ATMC_BOX2DISPENSERJFMT",true,"STRING",5},
	{"ATMC_BOX3DISPENSERJFMT",true,"STRING",5},
	{"ATMC_BOX4DISPENSERJFMT",true,"STRING",5},
	{"ATMC_BOX5DISPENSERJFMT",true,"STRING",5},
	{"ATMC_BOX6DISPENSERJFMT",true,"STRING",5},
	{"ATMC_BOX7DISPENSERJFMT",true,"STRING",5},
	{"ATMC_BOX1DEPOSITFMT",true,"STRING",5},
	{"ATMC_BOX2DEPOSITFMT",true,"STRING",5},
	{"ATMC_BOX3DEPOSITFMT",true,"STRING",5},
	{"ATMC_BOX4DEPOSITFMT",true,"STRING",5},
	{"ATMC_BOX5DEPOSITFMT",true,"STRING",5},
	{"ATMC_BOX6DEPOSITFMT",true,"STRING",5},
	{"ATMC_BOX7DEPOSITFMT",true,"STRING",5},
	{"HOSTAMOUNT",false,"STRING",20},			//add by art for 主机现金箱余额
	//add by art for 现金交易统计和异常交易记录、吞卡记录和回存记录 begin
	{"DEPCOUNT",false,"STRING",6},
	{"CWDCOUNT",false,"STRING",6},
	{"DEPAMOUNT",false,"STRING",20},
	{"CWDAMOUNT",false,"STRING",20},
	{"ALLERRTRANS",false,"STRING",40}, 
	{"CARDRETAINRECORD",false,"STRING",40}, 
	{"REDEPAMOUNT",false,"STRING",40},
	{"ALLCLOSETRANS",false,"STRING",40}
	//add by art for 现金交易统计和异常交易记录、吞卡记录和回存记录 end
};

DSTOField SUBTOTAL_szABC_haveReDep[]={ // add by zwh for 相比之前的FORM，增加回存交易打印
	{"Date",false,"STRING",12},
	{"Time",false,"STRING",8},
	{"SUBBANKNUM",true,"STRING",22},
	{"TERMINALNUM",true,"STRING",20},
	{"BOX1TYPEFMT",true,"STRING",5},
	{"BOX2TYPEFMT",true,"STRING",5},
	{"BOX3TYPEFMT",true,"STRING",5},
	{"BOX4TYPEFMT",true,"STRING",5},
	{"BOX5TYPEFMT",true,"STRING",5},
	{"BOX6TYPEFMT",true,"STRING",5},
	{"BOX7TYPEFMT",true,"STRING",5},
	{"BOX1VALUEFMT",true,"STRING",5},
	{"BOX2VALUEFMT",true,"STRING",5},
	{"BOX3VALUEFMT",true,"STRING",5},
	{"BOX4VALUEFMT",true,"STRING",5},
	{"BOX5VALUEFMT",true,"STRING",5},
	{"BOX6VALUEFMT",true,"STRING",5},
	{"BOX7VALUEFMT",true,"STRING",5},
	{"ENCASEBILLS1",true,"STRING",5},
	{"ENCASEBILLS2",true,"STRING",5},
	{"ENCASEBILLS3",true,"STRING",5},
	{"ENCASEBILLS4",true,"STRING",5},
	{"ENCASEBILLS5",true,"STRING",5},
	{"ENCASEBILLS6",true,"STRING",5},
	{"ENCASEBILLS7",true,"STRING",5},
	{"BOX1NOTENUMBERFMT",true,"STRING",5},
	{"BOX2NOTENUMBERFMT",true,"STRING",5},
	{"BOX3NOTENUMBERFMT",true,"STRING",5},
	{"BOX4NOTENUMBERFMT",true,"STRING",5},
	{"BOX5NOTENUMBERFMT",true,"STRING",5},
	{"BOX6NOTENUMBERFMT",true,"STRING",5},
	{"BOX7NOTENUMBERFMT",true,"STRING",5},
	{"ATMC_BOX1DISPENSEFMT",true,"STRING",5},
	{"ATMC_BOX2DISPENSEFMT",true,"STRING",5},
	{"ATMC_BOX3DISPENSEFMT",true,"STRING",5},
	{"ATMC_BOX4DISPENSEFMT",true,"STRING",5},
	{"ATMC_BOX5DISPENSEFMT",true,"STRING",5},
	{"ATMC_BOX6DISPENSEFMT",true,"STRING",5},
	{"ATMC_BOX7DISPENSEFMT",true,"STRING",5},
	{"ATMC_BOX1DISPENSERJFMT",true,"STRING",5},
	{"ATMC_BOX2DISPENSERJFMT",true,"STRING",5},
	{"ATMC_BOX3DISPENSERJFMT",true,"STRING",5},
	{"ATMC_BOX4DISPENSERJFMT",true,"STRING",5},
	{"ATMC_BOX5DISPENSERJFMT",true,"STRING",5},
	{"ATMC_BOX6DISPENSERJFMT",true,"STRING",5},
	{"ATMC_BOX7DISPENSERJFMT",true,"STRING",5},
	{"ATMC_BOX1DEPOSITFMT",true,"STRING",5},
	{"ATMC_BOX2DEPOSITFMT",true,"STRING",5},
	{"ATMC_BOX3DEPOSITFMT",true,"STRING",5},
	{"ATMC_BOX4DEPOSITFMT",true,"STRING",5},
	{"ATMC_BOX5DEPOSITFMT",true,"STRING",5},
	{"ATMC_BOX6DEPOSITFMT",true,"STRING",5},
	{"ATMC_BOX7DEPOSITFMT",true,"STRING",5},
	{"HOSTAMOUNT",false,"STRING",20},			//add by art for 主机现金箱余额
	//add by art for 现金交易统计和异常交易记录、吞卡记录和回存记录 begin
	{"DEPCOUNT",false,"STRING",6},
	{"CWDCOUNT",false,"STRING",6},
	{"DEPAMOUNT",false,"STRING",20},
	{"CWDAMOUNT",false,"STRING",20},
	{"ALLERRTRANS",false,"STRING",40}, 
	{"CARDRETAINRECORD",false,"STRING",40}, 
	{"REDEPAMOUNT",false,"STRING",40},
	{"ALLCLOSETRANS",false,"STRING",40}
	//add by art for 现金交易统计和异常交易记录、吞卡记录和回存记录 end
};

DSTOField Receipt_Sales_szABC[]={
	{"CARDNO",false,"STRING",20},				// 受理卡号
	{"CUSTOMERNAME",false,"STRING",20},			// 客户姓名
	{"SUBBANKNUM",true,"STRING",22},			// 受理行号
	{"TERMINALNUM",true,"STRING",20},			// 设备编号
	{"DEALTYPE",false,"STRING",22},				// 交易类型
	{"TRANSAMOUNT",false,"STRING",20},			// 本金金额
	{"INTEREST",false,"STRING",20},				// 利息
	{"TAXABLE",false,"STRING",20},				// 应税利息
	{"TAXRATE",false,"STRING",20},				// 税率
	{"TAX",false,"STRING",20},					// 税金
	{"TOTAL",false,"STRING",20},				// 本息合计
	{"TFRCARDNO",false,"STRING",20},			// 转入账号
	{"TFRCUSTNAME",false,"STRING",20},			// 转入客户姓名
	{"POUNDAGE",false,"STRING",20},				// 手续费
	{"Date",false,"STRING",12},					// 交易日期
	{"Time",false,"STRING",8},					// 交易时间
	{"TRANSACTIONRESULT",false,"STRING",15},	// 交易结果
	{"JNLNUM",true,"LONG",12},					// 流水号
	{"SYSREFNUM",false,"STRING",20},			// 后台流水号
	{"RESPONSECODE",false,"STRING",15},			// 响应码
	{"COMMENTS",false,"STRING",42}				// 备注
};

DSTOField Receipt_SalesForWX_szABC[]={
	{"CARDNO",false,"STRING",20},				// 受理卡号
	{"CUSTOMERNAME",false,"STRING",20},			// 客户姓名
	{"SUBBANKNUM",true,"STRING",22},			// 受理行号
	{"TERMINALNUM",true,"STRING",20},			// 设备编号
	{"DEALTYPE",false,"STRING",22},				// 交易类型
	{"Date",false,"STRING",12},					// 交易日期
	{"Time",false,"STRING",8},					// 交易时间
	{"TRANSACTIONRESULT",false,"STRING",15},	// 交易结果
	{"JNLNUM",true,"LONG",12},					// 流水号
	{"SYSREFNUM",false,"STRING",20},			// 后台流水号
	{"RESPONSECODE",false,"STRING",15},			// 响应码
	{"COMMENTS",false,"STRING",42}				// 二维码
};

DSTOForm dstoForm[] = {
	{"ReceiptCash_Print_szABC",ReceiptCash_Print_szABC, sizeof(ReceiptCash_Print_szABC)/sizeof(DSTOField)},
	{"RejectCash_Print_szABC",RejectCash_Print_szABC, sizeof(RejectCash_Print_szABC)/sizeof(DSTOField)},
	{"TOTAL_szABC",TOTAL_szABC, sizeof(TOTAL_szABC)/sizeof(DSTOField)},
	{"SUBTOTAL_szABC",SUBTOTAL_szABC, sizeof(SUBTOTAL_szABC)/sizeof(DSTOField)},
	{"ReceiptBillLoadingCountSet_szABC",ReceiptBillLoadingCountSet_szABC, sizeof(ReceiptBillLoadingCountSet_szABC)/sizeof(DSTOField)},
	{"ExChangeMoney_Print_szABC",ExChangeMoney_Print_szABC, sizeof(ExChangeMoney_Print_szABC)/sizeof(DSTOField)},
	{"Receipt_ReDEPCash_szABC",Receipt_ReDEPCash_szABC, sizeof(Receipt_ReDEPCash_szABC)/sizeof(DSTOField)},
	{"Receipt_Trans_szABC",Receipt_Trans_szABC, sizeof(Receipt_Trans_szABC)/sizeof(DSTOField)},
	{"Receipt_CTR_szABC",Receipt_CTR_szABC, sizeof(Receipt_CTR_szABC)/sizeof(DSTOField)},
	{"Receipt_RTC_szABC",Receipt_RTC_szABC, sizeof(Receipt_RTC_szABC)/sizeof(DSTOField)},
	{"TOTAL_szABC_haveReDep",TOTAL_szABC_haveReDep, sizeof(TOTAL_szABC_haveReDep)/sizeof(DSTOField)},
	{"SUBTOTAL_szABC_haveReDep",SUBTOTAL_szABC_haveReDep, sizeof(SUBTOTAL_szABC_haveReDep)/sizeof(DSTOField)},
	{"Receipt_Sales_szABC",Receipt_Sales_szABC, sizeof(Receipt_Sales_szABC)/sizeof(DSTOField)},
	{"Receipt_SalesForWX_szABC",Receipt_SalesForWX_szABC, sizeof(Receipt_SalesForWX_szABC)/sizeof(DSTOField)},
};
int FormNum = sizeof(dstoForm)/sizeof(DSTOForm);
