/*@create by:  tsxiong
*@time: 2016年03月20日
*/
; (function () {
    var bAccepted = false; //标志存款是否完成
    var bTaken = false;  //标志拒钞是否拿走
    var bClearTime = false;
	var nLastAcceptedAmount = -1;//当笔CashIn入钞总金额
	var strCallResponse = "";
	var bError = false;
	var CallResponse = App.Cntl.ProcessOnce( function(Response){
        //TO DO:
        SetJnl();
        Clearup();
        App.Cntl.ProcessDriven(Response);
    });
    var Initialize = function () {
        App.Timer.TimeoutDisposal(TimeoutCallBack);
        EventLogin();        
        top.Cim.AcceptCash(-1);
        App.Plugin.Voices.play("voi_16");
    }();//Page Entry

    //event handler     
    function onCashInserted() {
        top.API.displayMessage("开始吸钞");        
    }

	function SetCurrencyCashIn(TotalItems){
		var i = 0;
		var strDenomination = new Array();
		for (i = 0; i < TotalItems.toArray().length; i++) {
			strDenomination = TotalItems.toArray()[i].split(":");
			if (strDenomination[0] == "0") {
				break;
			}
			if (strDenomination[0] == "100") {
				top.API.CashInfo.arrCurrencyCashIn[0] = strDenomination[1];
			}
			if (strDenomination[0] == "50") {
				top.API.CashInfo.arrCurrencyCashIn[1] = strDenomination[1];
			}
			if (strDenomination[0] == "20") {
				top.API.CashInfo.arrCurrencyCashIn[2] = strDenomination[1];
			}
			if (strDenomination[0] == "10") {
				top.API.CashInfo.arrCurrencyCashIn[3] = strDenomination[1];
			}
			if (strDenomination[0] == "5") {
				top.API.CashInfo.arrCurrencyCashIn[4] = strDenomination[1];
			}
			if (strDenomination[0] == "1") {
				top.API.CashInfo.arrCurrencyCashIn[5] = strDenomination[1];
			}
		}
		return;
	}
	function ReCordFile(){
		var nAllRefusedNum=top.Cim.NumOfRefused(); 
		var nThisTimeRefusedNum=nAllRefusedNum-top.API.gOldCimRefusedNums; 
		top.API.gOldCimRefusedNums=nAllRefusedNum; 
		
        var Lcount = top.API.gLcount;
        top.API.displayMessage(" 验钞次数==" + Lcount);		
		var bRecordFile = true;
		if((Lcount == 1) && (nLastAcceptedAmount == 0)){
			bRecordFile = false;
		}		
			
		if (Lcount == 1) {
			if(bRecordFile){
				top.Tsl.HandleRecordFileSync(top.API.MTSL_WRITETRANSRECORD, top.API.gCardno 
									+ ", DEP, " + top.API.CashInfo.strTransAmount 
									+ ", " + top.API.CashInfo.strTransAmount + ", , ");
				var strJNLData = "第" + Lcount + "次放钞：100:" +
											top.API.CashInfo.arrCurrencyCashIn[0] + " 50:" + 
											top.API.CashInfo.arrCurrencyCashIn[1] + " 20:" + 
											top.API.CashInfo.arrCurrencyCashIn[2] + " 10:" + 
											top.API.CashInfo.arrCurrencyCashIn[3] + " 5:" + 
											top.API.CashInfo.arrCurrencyCashIn[4] + " 1:" + 
											top.API.CashInfo.arrCurrencyCashIn[5] + " RJ:" + 
											nThisTimeRefusedNum; 
				var arrCashOutBoxData = new Array(strJNLData);
				top.Dat.SetDataSync(top.API.ContentDataTag, top.API.ContentDataType, arrCashOutBoxData);
				top.Jnl.PrintSync("Content");
			}
		} else {
			var TotalItemsArr = new Array(0,0,0,0,0,0);
			var i=0;
			for (i = 0; i < top.API.garrTotalItems.length; i++) {					
				TotalItemsArr[i] = parseInt(top.API.CashInfo.arrCurrencyCashIn[i]) - top.API.garrTotalItems[i];					
			}
			top.Tsl.UpdateRecord(top.API.gCardno + ", DEP, " + top.API.CashInfo.strTransAmount + ", " 
									+ top.API.CashInfo.strTransAmount + ", , ");								
			top.API.displayMessage("当前(不是第一次)放钞信息：---Tcount100=" + TotalItemsArr[0] + "---Tcount50=" + 
													TotalItemsArr[1] + "---Tcount20=" + TotalItemsArr[2] + "---Tcount10=" + 
													TotalItemsArr[3] + "---Tcount5=" + TotalItemsArr[4] + "---Tcount1=" + 
													TotalItemsArr[5]+ "---RJNum=" + nThisTimeRefusedNum+ "---");  
			var strJNLData = "第" + Lcount + "次放钞：100:" + TotalItemsArr[0] + " 50:" + 
											TotalItemsArr[1] + " 20:" + TotalItemsArr[2] + " 10:" + TotalItemsArr[3]+ " 5:" + TotalItemsArr[4] + 
											" 1:" + TotalItemsArr[5] + " RJ:" + nThisTimeRefusedNum; 
			var arrCashOutBoxData = new Array(strJNLData);
			top.Dat.SetDataSync(top.API.ContentDataTag, top.API.ContentDataType, arrCashOutBoxData);
			top.Jnl.PrintSync("Content");
		}
		if (bRecordFile) {
			for (i = 0; i < top.API.garrTotalItems.length; i++) {					
				top.API.garrTotalItems[i] = parseInt(top.API.CashInfo.arrCurrencyCashIn[i]);					
			}
			top.API.gLcount = Lcount + 1;
		}				
	}
	
	function SetComments(){
		var arrTransactionResult = new Array("失败");
		top.Dat.SetDataSync("TRANSACTIONRESULT", "STRING", arrTransactionResult);
		top.API.gTakeCardAndPrint = true; //退卡页面打印凭条
		var arrComments = new Array("设备故障，请联系银行工作人员");
		top.Dat.SetDataSync("COMMENTS", "STRING", arrComments);
	}
    function onCashAccepted(TotalItems, PartRefused) {
        top.API.displayMessage("存入钞箱信息：" + TotalItems.toArray() + "; 是否有拒钞：" + PartRefused);    
		var Lcount = top.API.gLcount;
        top.API.displayMessage(" 验钞次数==" + Lcount);
		nLastAcceptedAmount = top.Cim.GetLastAcceptedAmountSync("CNY", "VALID");
		if(nLastAcceptedAmount < 0){
			top.API.displayMessage("top.Cim.GetLastAcceptedAmountSync异常，top.Cim.GetLastAcceptedAmountSync = " + nLastAcceptedAmount);
			top.Cim.StoreEscrowedCash(top.API.gStoreEscrowedCashTimeOut);
			top.ErrorInfo = top.API.PromptList.No4;
			SetComments();
			if (Lcount == 1) {
				top.Tsl.HandleRecordFileSync(top.API.MTSL_WRITETRANSRECORD,top.API.gCardno + ", DEP, 0, 0, , SF");
			}else{
				top.Tsl.UpdateRecord(top.API.gCardno + ", DEP, 0, 0, , SF");
			}
			strCallResponse = "Exit";
			//return CallResponse("Exit");
		}
		top.API.CashInfo.strTransAmount = nLastAcceptedAmount.toString();
		var tmp = top.API.CashInfo.strTransAmount +"00";
		var arrCashAmount = new Array(tmp);
		top.Dat.SetDataSync(top.API.transamountTag, top.API.transamountType, arrCashAmount);		
		SetCurrencyCashIn(TotalItems);		
		ReCordFile();	
		
		var cimStatus = top.Cim.StDetailedDeviceStatus();
		var DoorStatus = top.Cim.StSafeDoorStatus();
		top.API.gbPartCashIn = (cimStatus == "HARDWAREERROR" || cimStatus == "OFFLINE" || DoorStatus == "OPEN");
		if (top.API.gbPartCashIn) { //验钞时卡钞
			ErrorSituation();
		}else{
			strCallResponse = "OK";			
		}
		top.API.displayMessage("出钞口状态：" + top.Cdm.StOutputStatus() + ";是否有拒钞：" + PartRefused);
		if (top.Cdm.StOutputStatus() == "NOTEMPTY" && PartRefused) {
			top.Cdm.OpenShutter(top.API.gOpenShutterTimeOut);	
		}else if(top.Cdm.StOutputStatus() == "EMPTY" && PartRefused){
			top.Cdm.OpenShutter(top.API.gOpenShutterTimeOut);	
			setTimeout(function(){
			top.Cdm.CloseShutter(top.API.gCloseShutterTimeOut);},10000);                            
		} else {
			return NextPage();
		}	
    }
	
	function ErrorSituation(){
		bError = true;
		top.Jnl.PrintSync("CashInError");
		var iniRet = top.Dat.GetPrivateProfileSync("TransactionConfig", "PartCashSaveSupport", "1", top.API.gIniFileName);
		if( "1" == iniRet){ 
			top.API.displayMessage("交易金额："+ top.API.CashInfo.strTransAmount);
			if(parseInt(top.API.CashInfo.strTransAmount) > 0){
				if (top.API.CashInfo.Dealtype == "DEP存款"){
				   top.API.gTransactiontype = "DEP";
				}else if(top.API.CashInfo.Dealtype == "无卡无折存款" || top.API.CashInfo.Dealtype == "存折存款"){
					top.API.gTransactiontype = "NOCARDDEP";
					if (top.API.gCardOrBookBank == 1) {
						top.API.gTransactiontype = "DEP";
					}
				}else if(top.API.CashInfo.Dealtype == "对公存款"){
					top.API.gTransactiontype = "BUSSINESSDEP";
				}				
				strCallResponse = "DirectAccount";
			}else{
				top.ErrorInfo = top.API.PromptList.No4;
				SetComments();
				strCallResponse = "Exit";
			}
		}else{			
			top.ErrorInfo = top.API.PromptList.No4;
			SetComments();
			strCallResponse = "Exit";           
		}
	}
	
	function NextPage() {
		if(strCallResponse == ""){
			top.API.displayMessage("strCallResponse为空");
			return 0;
		}
		Clearup();
		if(bError){
			top.Cim.StoreEscrowedCash(top.API.gStoreEscrowedCashTimeOut);
		}
		return CallResponse(strCallResponse);
	}
    function VoicesPlay() {       
	    App.Plugin.Voices.play("voi_7");
    }
   
    function onCashTaken() {
        App.Timer.ClearIntervalTime();
        top.Cdm.CloseShutter(top.API.gCloseShutterTimeOut);		
    }

    function onDeviceError() {
		top.API.displayMessage("onDeviceError触发");
        ErrorSituation();
		NextPage();
    }
	function onShutterOpened(){
		top.API.displayMessage("onShutterOpened触发,提示客户拿走钞票");
		App.Timer.SetIntervalDisposal(VoicesPlay, 12000);
	}	
	function onShutterClosed(){
		top.API.displayMessage("onShutterClosed触发");
		NextPage();
	}
	function onShutterOpenFailed(){
		top.API.displayMessage("onShutterOpenFailed触发");
		ErrorSituation();
		NextPage();		
	}	
	function onShutterCloseFailed(){
		top.API.displayMessage("onShutterCloseFailed触发");
		ErrorSituation();
		NextPage();
	}
	
    function SetJnl() {
        var i;
        var tmpCurrentInfo = new Array();
        for ( i = 0; i < top.API.CashInfo.nCountOfUnits; i++) {
            tmpCurrentInfo[i] = top.API.CashInfo.arrUnitRemain[i];
        }
        top.API.CashInfo.arrUnitRemain = top.API.GetUnitInfo(top.Cdm.PUCurrentCount().toArray());
        for ( i = 0; i < top.API.CashInfo.nCountOfUnits; i++) {
            top.API.CashInfo.arrAcceptorCount[i] += (tmpCurrentInfo[i]>top.API.CashInfo.arrUnitRemain[i])?(tmpCurrentInfo[i]-top.API.CashInfo.arrUnitRemain[i]):(top.API.CashInfo.arrUnitRemain[i]-tmpCurrentInfo[i]);
        }        
    }
    //Register the event
    function EventLogin() {
        top.Cim.CashInserted.connect(onCashInserted);
        top.Cim.CashAccepted.connect(onCashAccepted);
        top.Cim.CashTaken.connect(onCashTaken);
        top.Cim.DeviceError.connect(onDeviceError);
		//Door
		top.Cdm.ShutterOpened.connect(onShutterOpened);
		top.Cdm.ShutterOpenFailed.connect(onShutterOpenFailed);
		top.Cdm.ShutterClosed.connect(onShutterClosed);
		top.Cdm.ShutterCloseFailed.connect(onShutterCloseFailed);
    }

    function EventLogout() {
        top.Cim.CashInserted.disconnect(onCashInserted);
        top.Cim.CashAccepted.disconnect(onCashAccepted);
        top.Cim.CashTaken.disconnect(onCashTaken);
        top.Cim.DeviceError.disconnect(onDeviceError);
		//Door
		top.Cdm.ShutterOpened.disconnect(onShutterOpened);
		top.Cdm.ShutterOpenFailed.disconnect(onShutterOpenFailed);
		top.Cdm.ShutterClosed.disconnect(onShutterClosed);
		top.Cdm.ShutterCloseFailed.disconnect(onShutterCloseFailed);
    }

    //Countdown function
    function TimeoutCallBack() {
        top.ErrorInfo = top.API.PromptList.No3;
		strCallResponse = "TimeOut";  
		top.API.gTakeCardAndPrint = true; // 超时打印凭条
		top.Dat.SetDataSync("TRANSACTIONRESULT", "STRING", ["失败"]);
		top.Dat.SetDataSync("COMMENTS", "STRING", ["交易超时"]); // 备注
		bError = true;
		NextPage();
    }
    //Page Return
    
    //remove all event handler
    function Clearup() {
        //TO DO:
        EventLogout();
		top.Cdm.CloseShutter(top.API.gCloseShutterTimeOut);		
        App.Timer.ClearIntervalTime();
        App.Plugin.Voices.del();
		App.Timer.ClearTime();
    }
})();
