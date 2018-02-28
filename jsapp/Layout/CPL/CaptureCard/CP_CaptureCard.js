/*@create by:  tsxiong
*@time: 2016年03月20日
*/
; (function(){
	var CallResponse = App.Cntl.ProcessOnce( function(Response){
        Clearup();
        App.Cntl.ProcessDriven( Response );
    });
    var Initialize = function() {
        EventLogin();
        App.Plugin.Voices.play("voi_21");
        var arrDealType = new Array("吞卡(RETAIN CARD)");		
        top.Dat.SetDataSync("COMMENTS", "STRING", arrDealType); 
        top.Ptr.Print("ReceiptCash_Print_szABC", "",top.API.gPrintTimeOut);
        top.Crd.Capture(top.API.gCaptureTimeout);
        top.Jnl.PrintSync("CaptureCardAction");
        top.Tsl.HandleRecordFileSync(top.API.MTSL_WRITERETAINCARDRECORD, top.API.gCardno+ ", "+ "");
		 
        App.Timer.TimeoutDisposal(TimeoutCallBack);
    }();//Page Entry

   //@User ocde scope start
   function onCardCaptured(){
		var iniRet = top.Dat.GetPrivateProfileSync("TransactionConfig", "CWCSupport", "0", top.API.gIniFileName); 
		if (top.API.CashInfo.Dealtype == "CWD取款" && (iniRet == 1)) {
            top.Dat.GetPersistentData(top.API.cwcflagTag, top.API.cwcflagType);
        }else{
            return CallResponse("Exit");
        }
   }
   
   function onCardCaptureFailed(){
		var iniRet = top.Dat.GetPrivateProfileSync("TransactionConfig", "CWCSupport", "0", top.API.gIniFileName); 
		if (top.API.CashInfo.Dealtype == "CWD取款" && (iniRet == 1)) {
            top.Dat.GetPersistentData(top.API.cwcflagTag, top.API.cwcflagType);
        }else{
            return CallResponse("Exit");
        }
   }
    function onDatGetPersistentDataComplete(DataName, DataType, DataValue) {
        var flag_CWC = DataValue.toArray();
        if (flag_CWC[0] == 0) {
            return CallResponse("Exit");
        } else {
            if (top.API.CashInfo.Dealtype == "CWD取款" || top.API.CashInfo.Dealtype == "存折取款") { 
                top.API.gTransactiontype = "CWC"; 
                var arrCWCFlag = new Array();
                arrCWCFlag[0] = 0;
                top.Dat.SetPersistentData(top.API.cwcflagTag, top.API.cwcflagType, arrCWCFlag);
            }
        }
    }

    function onDatGetPersistentDataError(DataName, ErrorCode) {
        top.API.displayMessage("onDatGetPersistentDataError is done,DataName=" + DataName + ",ErrorCode=" + ErrorCode);
        return CallResponse('Exit');
    }
   
    function onDatSetPersistentDataComplete(DataName) {
        if ('CWCFLAG' == DataName) {
           return CallResponse("CWC");
        }
    }

    function onDatSetPersistentDataError(DataName, ErrorCode) {
        top.API.displayMessage("读取失败---onDatSetPersistentDataError is done,DataName=" + DataName + ",ErrorCode=" + ErrorCode);
        return CallResponse('Exit');
    }
   //@User code scope end 
    function EventLogin() {
		top.Crd.CardCaptured.connect(onCardCaptured);
        top.Crd.CardCaptureFailed.connect(onCardCaptureFailed);
        top.Dat.GetPersistentDataComplete.connect(onDatGetPersistentDataComplete);
        top.Dat.GetPersistentDataError.connect(onDatGetPersistentDataError);
        top.Dat.SetPersistentDataComplete.connect(onDatSetPersistentDataComplete);
        top.Dat.SetPersistentDataError.connect(onDatSetPersistentDataError);
    }

    function EventLogout() {
		top.Crd.CardCaptured.disconnect(onCardCaptured);
        top.Crd.CardCaptureFailed.disconnect(onCardCaptureFailed);
        top.Dat.GetPersistentDataComplete.disconnect(onDatGetPersistentDataComplete);
        top.Dat.GetPersistentDataError.disconnect(onDatGetPersistentDataError);
        top.Dat.SetPersistentDataComplete.disconnect(onDatSetPersistentDataComplete);
        top.Dat.SetPersistentDataError.disconnect(onDatSetPersistentDataError);
    }

   
    //Register the event

       //Countdown function
    function TimeoutCallBack() {
		top.API.displayMessage("直接退出");
		return CallResponse("TimeOut");
     }
       //Page Return
    
      //remove all event handler
    function Clearup(){
        EventLogout();
      App.Timer.ClearTime();
      App.Plugin.Voices.del();
    }
})();
