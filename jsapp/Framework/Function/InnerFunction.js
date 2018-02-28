
(function () {
    window.InnerFunction = function () {

        var CallResponse = function (Response) {
            //Entry the flows control process.
            App.Cntl.ProcessDriven(Response);
        };
        this.CF_InputCounterInit = function () {
            return CallResponse("OK");
        };
        this.CF_InputCounterPlus = function () {

            return CallResponse("OK");
        };
        this.CF_CheckPrintStatus = function () {
            if (top.Ptr.bDeviceStatus) {
                return CallResponse("OK");
            } else {
                return CallResponse("NoPrint");
            }
        };

		this.CF_CheckCardType = function () {
            if (top.API.gATMORTCR == "TCR") {
                return CallResponse("OK");
            } else {
                return CallResponse("ATMType");
            }

        };
        this.CF_RefreshCashInfo = function () {
            top.API.CashInfo.arrUnitRemain = top.API.GetUnitInfo(top.Cdm.PUCurrentCount().toArray());
            return CallResponse("OK");
        };
        this.CF_Communicate = function () {
            return CallResponse(top.API.gTransactiontype);
        };
        this.CF_CheckStatus = function () {		
			App.Timer.ClearIntervalTime();
			App.Timer.ClearTime();
            top.ErrorInfo = "1";
           // top.Dat.InitDatasSync();
           // top.API.clearGlobalData();//初始化全局变量
		//	top.Sys.SetIsBusying(false);//供总行监控判断
            if (top.FirstOpenFlag) {
                return CallResponse("OpenDevice");
            }            
            if ("NOTPRESENT" != top.Crd.StMediaStatus()) {
                top.Crd.Eject(-1);
				top.Jnl.PrintSync("ErrorEjectCardAction"); 				
				return CallResponse("OffLine");
            }
	    App.Plugin.Wait.show();
            return CallResponse("OnLine");
        };
        this.CF_AdminStatus = function () {
            if (top.API.AdminStatus == 0) {
                return CallResponse("MainAdmin");
            } else if (top.API.AdminStatus == 1) {
                return CallResponse("GeneralAdmin");
            }
        };
        this.CF_IsCheckIDCardAboutTheAmount = function () {
            if (top.API.gbAmountType == false) {
                //选的是大额，已经核查过了
                return CallResponse("NoNeedIDCard");
            } else {
                var strTransAmountTmp = top.API.CashInfo.strTransAmount;
                var nTransAmount = parseInt(strTransAmountTmp);
                if (nTransAmount >= top.gAmountQuota) {
                    top.API.gCheckInfoFlag = true;
                    return CallResponse("NeedIDCard");
                } else {
                    return CallResponse("NoNeedIDCard");
                }
            }
        };

        this.CF_NeedQueryName = function () {   
            top.API.gTransactiontype = "QRYCUSTNAME";
            return CallResponse("OK");
        };
		
		this.CF_NeedQueryAccountType = function () {  
			if(top.API.gCardOrBookBank == 1){
				top.API.gTransactiontype = "QRYACCOUNTTYPE";
				return CallResponse("OK");
			}else{
				return CallResponse("Exit");
			}
            
        };

        this.CF_DataInitBeforeContinue = function () {
            top.API.gbContinueTransFlag = true;
            top.ErrorInfo = top.API.PromptList.No1;
            var arrCashAmount = new Array();
            arrCashAmount[0] = "0";
            top.Dat.SetDataSync(top.API.transamountTag, top.API.transamountType, arrCashAmount);
            var arrTransactionResult = new Array();
            arrTransactionResult[0] = "";
            top.Dat.SetDataSync("TRANSACTIONRESULT", "STRING", arrTransactionResult);
            var arrAUTHCODE = new Array();
            arrAUTHCODE[0] = "";
            top.Dat.SetDataSync("AUTHCODE", "STRING", arrAUTHCODE);
            var arrPOUNDAGE = new Array();
            arrPOUNDAGE[0] = "";
            top.Dat.SetDataSync("POUNDAGE", "STRING", arrPOUNDAGE);
            var arrSYSREFNUM = new Array();
            arrSYSREFNUM[0] = "";
            top.Dat.SetDataSync("SYSREFNUM", "STRING", arrSYSREFNUM);
            // 交易类型
            var arrDEALTYPE = new Array();
            arrDEALTYPE[0] = "";
            top.Dat.SetDataSync("DEALTYPE", "STRING", arrDEALTYPE);
            // 转入账号
            var arrTFRCARDNO = new Array();
            arrTFRCARDNO[0] = "";
            top.Dat.SetDataSync("TFRCARDNO", "STRING", arrTFRCARDNO);
            // 备注
            var arrCOMMENTS = new Array();
            arrCOMMENTS[0] = "";
            top.Dat.SetDataSync("COMMENTS", "STRING", arrCOMMENTS);

            top.API.ContinueTransClearGlobalData();
            //获取最新钞箱余额
            top.API.CashInfo.arrUnitRemain = top.API.GetUnitInfo(top.Cdm.PUCurrentCount().toArray());
            var strTemp = top.API.CashInfo.strCardNum;
            top.API.CashInfo.InitData();
            top.API.CashInfo.strCardNum = strTemp;

            var strCardType = "MAGCARD";
            var objGet2 = top.Dat.GetDataSync(top.API.cardtypeTag, top.API.cardtypeType);
            if (null == objGet2) {
                top.API.displayMessage("GetDataSync CARDTYPE objGet2 = null");
            }
            else {
                var arrGet = objGet2.toArray();
                strCardType = arrGet[0];
            }
            top.API.gTransStatus = top.Sys.PossibleTransactionSync();
            
            var FallBackCard = 0;
            FallBackCard = top.Dat.GetDataSync(top.API.FallBackCardTag, top.API.FallBackCardType);
			var FallBackCardTmp=FallBackCard.toArray();
            if ("CHIPCARD" == strCardType && parseInt(FallBackCardTmp[0]) != 1) {
               return CallResponse("CHIPCARDcontinue");
            } else {
                return CallResponse("OK");
            }
        };
        this.CF_IsContinueTransAfterPWD = function () {
			return CallResponse("OK");
        };
    };

})();;
