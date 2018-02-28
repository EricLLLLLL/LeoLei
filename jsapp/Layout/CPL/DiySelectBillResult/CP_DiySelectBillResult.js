/*@create by:  tsxiong
*@time: 2016��03��20��
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
        var arrTransactionResult = new Array("���׳ɹ�");
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
        top.API.displayMessage("���<�˳�>��ť,��Ӧ<Exit>");
        ButtonDisable();
    return CallResponse('Exit');
    }

    document.getElementById('OK').onclick = function () {
        top.API.displayMessage("���<ȷ��>��ť,��Ӧ<OK>");
        ButtonDisable();
        initPtr();
        //return CallResponse('OK');
    }

    //@User ocde scope start
    function initPtr() {
        top.API.displayMessage("����Function<initPtr()>");
        top.Ptr.Print("ExChangeMoney_Print_szABC", "",top.API.gPrintTimeOut);
    }

    //@User code scope end 
        

    //event handler
    function onPrintComplete() {
        top.API.displayMessage("onPrintComplete����,��Ӧ<OK>");
        top.Siu.SetReceiptPrinterLight('SLOW');
        App.Plugin.Voices.play("voi_8");
        return CallResponse("OK");
    }
    //event handler
    function onPrintEjected() {
        top.API.displayMessage("onPrintEjected����");
        return CallResponse("OK");    
    }
    //event handler
    function onEjectTimeout() {
        top.API.displayMessage("onEjectTimeout����,��Ӧ<TimeOut>");
        top.ErrorInfo = "��ӡʧ�ܣ����׽�����";
        return CallResponse("OK");
    }
    //event handler
    function onDeviceError() {
        top.API.displayMessage("onDeviceError����");
        top.ErrorInfo = top.API.PromptList.No4;
        return CallResponse("Exit");
    }

     function onPrintFailed() {
        top.API.displayMessage("ononPrintFailed����");
        top.ErrorInfo = top.API.PromptList.No4;
        return CallResponse("Exit");
    }
    //event handler
    function onPrintTaken() {
        top.API.displayMessage("onPrintTaken����,��Ӧ<OK>");
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
        top.API.displayMessage("ҳ�泬ʱ,��Ӧ<TimeOut>");
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