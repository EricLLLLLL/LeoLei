/*@create by:  tsxiong
*@time: 2016��03��20��
*/
; (function () {
	var CallResponse = App.Cntl.ProcessOnce( function(Response){
        //TO DO:
        Clearup();
        //Entry the flows control process.
        App.Cntl.ProcessDriven(Response);
    });
    var Initialize = function () {
        App.Timer.TimeoutDisposal(TimeoutCallBack);
        EventLogin();
        initPbk();
    }(); //Page Entry
    //@User ocde scope start
    function initPbk() {
        top.API.displayMessage("����Function<initPbk()>");
        // var field1 = "data1=20160920|abstract1=֧��|detail1=-1000.00|balance1=2000.00|operator1=001";
        // var field2 = "data2=20160922|abstract2=����|detail2=+1000.00|balance2=3000.00|operator2=012";
        // var field3 = "data3=20160928|abstract3=֧��|detail3=-1500.00|balance3=1500.00|operator3=009";
    
        // var fields = field1 + "|" + field2 + "|" + field3;
        // top.API.Pbk.Print("TestPassbookForm", fields);
        var FillPassbook;
        var objFillPassbook = top.Dat.GetDataSync("FillPassbook", "STRING");
        if (null == objFillPassbook) {
            top.API.displayMessage("GetDataSync FillPassbook objGet = null");
        }
        else {
            top.API.displayMessage("GetDataSync objFillPassbook Return:" + objFillPassbook.toArray());
            var arrFillPassbook = objFillPassbook.toArray();
            FillPassbook = arrFillPassbook[0];
        }
		FillPassbook = "";
        top.API.Pbk.Print("PassbookForm", FillPassbook);
    }

    //@User code scope end 

    //event handler
    function onPrintComplete() {
        top.API.displayMessage("onPrintComplete����,��Ӧ<OK>");
        return CallResponse("OK");           
    }
    //event handler
    function onPrintEjected() {
        top.API.displayMessage("onPrintEjected����");
    }
    //event handler
    function onEjectTimeout() {
        top.API.displayMessage("onEjectTimeout����,��Ӧ<TimeOut>");
        top.ErrorInfo = "��ӡʧ�ܣ����׽�����";
        return CallResponse("TimeOut");
    }
    //event handler
    function onDeviceError() {
        top.API.displayMessage("onDeviceError����");
        top.ErrorInfo = top.API.PromptList.No4;
        return CallResponse("Exit");
    }
    //event handler
    function onPrintTaken() {
        top.API.displayMessage("onPrintTaken����,��Ӧ<OK>");
        return CallResponse("OK");
    }
    //Register the event
    function EventLogin() {
        top.Pbk.PrintComplete.connect(onPrintComplete);
        top.Pbk.PrintEjected.connect(onPrintEjected);
        top.Pbk.Timeout.connect(onEjectTimeout);
        top.Pbk.DeviceError.connect(onDeviceError);
        top.Pbk.PrintTaken.connect(onPrintTaken);
    }

    function EventLogout() {
        top.Pbk.PrintComplete.disconnect(onPrintComplete);
        top.Pbk.PrintEjected.disconnect(onPrintEjected);
        top.Pbk.Timeout.disconnect(onEjectTimeout);
        top.Pbk.DeviceError.disconnect(onDeviceError);
        top.Pbk.PrintTaken.disconnect(onPrintTaken);
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