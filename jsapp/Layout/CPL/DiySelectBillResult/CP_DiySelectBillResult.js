/*@create by:  tsxiong
*@time: 2016年03月20日
*/
; (function () {
	var CallResponse = App.Cntl.ProcessOnce( function(Response){
        //TO DO:
        top.Siu.SetReceiptPrinterLight('OFF');
        Clearup();
        //Entry the flows control process.
        App.Cntl.ProcessDriven(Response);
    });
    var Initialize = function () {
        App.Timer.TimeoutDisposal(TimeoutCallBack);
        ButtonDisable();
        var arrTransactionResult = new Array("交易成功");
        top.Dat.SetDataSync("TRANSACTIONRESULT", "STRING", arrTransactionResult);
        top.API.gTakeCardAndPrint = false;
        EventLogin();
        ButtonEnable();
    } (); //Page Entry

    function ButtonDisable() {
        top.API.displayMessage("ButtonDisable");
        document.getElementById('Exit').disabled = true;
        document.getElementById('OK').disabled = true;
    }

    function ButtonEnable() {
    top.API.displayMessage("ButtonEnable");
    document.getElementById('Exit').disabled = false;
    document.getElementById('OK').disabled = false;
    }

    document.getElementById('Exit').onclick = function () {
        top.API.displayMessage("点击<退出>按钮,响应<Exit>");
        ButtonDisable();
    return CallResponse('Exit');
    }

    document.getElementById('OK').onclick = function () {
        top.API.displayMessage("点击<确定>按钮,响应<OK>");
        ButtonDisable();
        initPtr();
        //return CallResponse('OK');
    }

    //@User ocde scope start
    function initPtr() {
        top.API.displayMessage("进入Function<initPtr()>");
        top.Ptr.Print("ExChangeMoney_Print_szABC", "",top.API.gPrintTimeOut);
    }

    //@User code scope end 
        

    //event handler
    function onPrintComplete() {
        top.API.displayMessage("onPrintComplete触发,响应<OK>");
        top.Siu.SetReceiptPrinterLight('SLOW');
        App.Plugin.Voices.play("voi_8");
        return CallResponse("OK");
    }
    //event handler
    function onPrintEjected() {
        top.API.displayMessage("onPrintEjected触发");
        return CallResponse("OK");    
    }
    //event handler
    function onEjectTimeout() {
        top.API.displayMessage("onEjectTimeout触发,响应<TimeOut>");
        top.ErrorInfo = "打印失败，交易结束！";
        return CallResponse("OK");
    }
    //event handler
    function onDeviceError() {
        top.API.displayMessage("onDeviceError触发");
        top.ErrorInfo = top.API.PromptList.No4;
        return CallResponse("Exit");
    }

     function onPrintFailed() {
        top.API.displayMessage("ononPrintFailed触发");
        top.ErrorInfo = top.API.PromptList.No4;
        return CallResponse("Exit");
    }
    //event handler
    function onPrintTaken() {
        top.API.displayMessage("onPrintTaken触发,响应<OK>");
        return CallResponse("OK");
    }
    /********************************************************************************************************/
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

    //Countdown function
    function TimeoutCallBack() {
        top.API.displayMessage("页面超时,响应<TimeOut>");
        top.ErrorInfo = top.API.PromptList.No3;
        return CallResponse("TimeOut");
    }
    //Page Return
    
    //remove all event handler
    function Clearup() {
        //TO DO:
        EventLogout();
        App.Plugin.Voices.del();
        App.Timer.ClearTime();
    }
})();