/*@create by:  tsxiong
*@time: 2016��03��20��
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
        top.API.displayMessage('��ʼ��PUCurrentCount()');
        top.Cdm.PUCurrentCount();
        top.API.displayMessage('������PUCurrentCount()');
        top.API.displayMessage('��ʼ��CUNoteValue()');
        top.Cdm.CUNoteValue();
        top.API.displayMessage('������CUNoteValue()');
        top.API.displayMessage('��ʼ��CUType()');
        top.Cdm.CUType();
        top.API.displayMessage('������CUType()');
        top.Cim.StatisticsData();
        var t=window.setTimeout(PrintData,1000);
    }
   //@User code scope end 
   function PrintData() {
        var arrTransKindType = new Array("��ӡͳ������");
        var nRet = top.Dat.SetDataSync(top.API.TransKindTag, top.API.TransKindType, arrTransKindType);
        top.API.displayMessage("SetDataSync TransKindType Return:" + nRet);
        top.Jnl.PrintSync("MachineSettle");
        top.Ptr.Print("SUBTOTAL_szABC_haveReDep", "",top.API.gPrintTimeOut); 
   }
    //event handler
    function onPrintComplete() {
        top.API.displayMessage("onPrintComplete����");
        return CallResponse("OK");           
    }
    //event handler
    function onPrintEjected() {
        top.API.displayMessage("onPrintEjected����");
    }
    //event handler
    function onEjectTimeout() {
        top.API.displayMessage("onEjectTimeout����,��Ӧ<TimeOut>");
    }
    //event handler
    function onDeviceError() {
        top.API.displayMessage("onDeviceError����");
        return CallResponse("Exit");
    }
     //event handler
    function onPrintFailed() {
        top.API.displayMessage("onPrintFailed����");
        return CallResponse("Exit");
    }
	function onPrintTaken() {
        top.API.displayMessage("onPrintTaken����");
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
