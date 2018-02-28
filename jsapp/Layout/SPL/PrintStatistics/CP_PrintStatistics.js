/*@create by:  tsxiong
*@time: 2016年03月20日
*/
; (function(){
	var CallResponse = App.Cntl.ProcessOnce( function(Response){
    //TO DO:
        Clearup();
        //Entry the flows control process.
        App.Cntl.ProcessDriven( Response );
    });
    var Initialize = function() {
    EventLogin();
     SetExchangePrintData();
 } (); //Page Entry

   //@User ocde scope start
   function SetExchangePrintData() {
        top.API.displayMessage('开始：PUCurrentCount()');
        top.Cdm.PUCurrentCount();
        top.API.displayMessage('结束：PUCurrentCount()');
        top.API.displayMessage('开始：CUNoteValue()');
        top.Cdm.CUNoteValue();
        top.API.displayMessage('结束：CUNoteValue()');
        top.API.displayMessage('开始：CUType()');
        top.Cdm.CUType();
        top.API.displayMessage('结束：CUType()');
        top.Cim.StatisticsData();
        var t=window.setTimeout(PrintData,1000);
    }
   //@User code scope end 
   function PrintData() {
        var arrTransKindType = new Array("打印统计数据");
        var nRet = top.Dat.SetDataSync(top.API.TransKindTag, top.API.TransKindType, arrTransKindType);
        top.API.displayMessage("SetDataSync TransKindType Return:" + nRet);
        top.Jnl.PrintSync("MachineSettle");
        top.Ptr.Print("SUBTOTAL_szABC_haveReDep", "",top.API.gPrintTimeOut); 
   }
    //event handler
    function onPrintComplete() {
        top.API.displayMessage("onPrintComplete触发");
        return CallResponse("OK");           
    }
    //event handler
    function onPrintEjected() {
        top.API.displayMessage("onPrintEjected触发");
    }
    //event handler
    function onEjectTimeout() {
        top.API.displayMessage("onEjectTimeout触发,响应<TimeOut>");
    }
    //event handler
    function onDeviceError() {
        top.API.displayMessage("onDeviceError触发");
        return CallResponse("Exit");
    }
     //event handler
    function onPrintFailed() {
        top.API.displayMessage("onPrintFailed触发");
        return CallResponse("Exit");
    }
	function onPrintTaken() {
        top.API.displayMessage("onPrintTaken触发");
        //return CallResponse("Exit");
    }
    //Register the event
    function EventLogin() {
        top.Ptr.PrintComplete.connect(onPrintComplete);
        top.Ptr.PrintFailed.connect(onPrintFailed);
        top.Ptr.PrintEjected.connect(onPrintEjected);
        top.Ptr.Timeout.connect(onEjectTimeout);
        top.Ptr.DeviceError.connect(onDeviceError);
        top.Ptr.PrintTaken.connect(onPrintTaken);
    }

    function EventLogout() {
        top.Ptr.PrintComplete.disconnect(onPrintComplete);
        top.Ptr.PrintFailed.disconnect(onPrintFailed);
        top.Ptr.PrintEjected.disconnect(onPrintEjected);
        top.Ptr.Timeout.disconnect(onEjectTimeout);
        top.Ptr.DeviceError.disconnect(onDeviceError);
        top.Ptr.PrintTaken.disconnect(onPrintTaken);
    }


      //remove all event handler
    function Clearup(){
		EventLogout();
    }
})();
