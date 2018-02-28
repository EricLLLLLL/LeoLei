/*@create by:  tsxiong
*@time: 2016Äê03ÔÂ20ÈÕ
*/
; (function(){
	var CallResponse = App.Cntl.ProcessOnce( function(Response){
        Clearup();
        App.Cntl.ProcessDriven( Response );
    });
    var Initialize = function() {     
        EventLogin();
        ButtonDisable();
		top.Tsl.HandleRecordFileSync(top.API.MTSL_RENAMERECORD, "");
		if (top.API.gAddNoteSuccess == true) {
			var arrTotalFlag = new Array();
			arrTotalFlag[0] = 1;
			top.Dat.SetPersistentData("TOTALADDOK", "LONG", arrTotalFlag);
		}else{
			document.getElementById("SuccessedDiv").style.display = "none";
			document.getElementById("FailedDiv").style.display = "block";
			document.getElementById("Exit").style.display = "block";
			ButtonEnable();
		}
    }();//Page Entry
    function ButtonDisable() {
        document.getElementById('Exit').disabled = true;
        document.getElementById('OK').disabled = true;
    }

    function ButtonEnable() {
        document.getElementById('Exit').disabled = false;
        document.getElementById('OK').disabled = false;
    }
   //@User ocde scope start
    document.getElementById('OK').onclick = function(){
        ButtonDisable();
         return CallResponse('OK');
    }

    document.getElementById('Exit').onclick = function(){
        ButtonDisable();
         return CallResponse('Exit');
    }
   //@User code scope end 
        /********************************************************************************************************/
    function onDatSetPersistentDataComplete(DataName) {
        if ('TOTALADDOK' == DataName) {
            top.Jnl.PrintSync("AddCashNote");
			var PtrPaperStatus = top.Ptr.StPaperStatus();
            if (PtrPaperStatus != "FULL" || PtrPaperStatus != "LOW") {
                top.Ptr.Print("ReceiptBillLoadingCountSet_szABC", "",top.API.gPrintTimeOut);
            }
            document.getElementById("OK").style.display = "block";  
        }  
        ButtonEnable();
    }

    function onDatSetPersistentDataError(DataName, ErrorCode) {
        document.getElementById("SuccessedDiv").style.display = "none";
        document.getElementById("FailedDiv").style.display = "block";
        document.getElementById("Exit").style.display = "block";
        ButtonEnable();
    }
    //Register the event
    function EventLogin() {
        top.Dat.SetPersistentDataComplete.connect(onDatSetPersistentDataComplete);
        top.Dat.SetPersistentDataError.connect(onDatSetPersistentDataError);
    }

    function EventLogout() {
        top.Dat.SetPersistentDataComplete.disconnect(onDatSetPersistentDataComplete);
        top.Dat.SetPersistentDataError.disconnect(onDatSetPersistentDataError);
    }
      //Page Return
    
      //remove all event handler
    function Clearup(){
      //TO DO:
      EventLogout();
    }
})();
