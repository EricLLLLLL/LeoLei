/*@create by:  tsxiong
*@time: 2016��03��20��
*/
; (function () {
	var CallResponse = App.Cntl.ProcessOnce( function(Response){
        //TO DO:
        top.Siu.SetReceiptPrinterLight('OFF');
        Clearup();
        App.Cntl.ProcessDriven(Response);
    });
    var Initialize = function () {
        App.Timer.TimeoutDisposal(TimeoutCallBack);
        top.API.gShowPrintButton = false;
        EventLogin();
        initPtr();
    }(); //Page Entry

    //@User ocde scope start
    function initPtr() {
		top.Siu.SetReceiptPrinterLight('SLOW');
		if (top.API.gbPartCashIn) { //�鳮ʱ����
			if (!top.API.gbPartCashInEnd) { 
				var arrComments = new Array("�豸���ϣ�����ϵ���й�����Ա");
				top.Dat.SetDataSync("COMMENTS", "STRING", arrComments);
			}else{
				var arrTransactionResult  = new Array("��������");
				top.Dat.SetDataSync("TRANSACTIONRESULT", "STRING", arrTransactionResult);
				top.API.displayMessage("SetDataSync TRANSACTIONRESULT Return:" + nRet);
				top.Dat.SetDataSync("COMMENTS", "STRING", arrComments);
			}
		}
		top.Ptr.Print("ReceiptCash_Print_szABC", "",top.API.gPrintTimeOut);
		
        App.Plugin.Voices.play("voi_8"); 
    }

    //@User code scope end 

    //event handler
    function onPrintComplete() {
        if (top.API.gbPartCashIn || top.API.CashInfo.Dealtype == "�޿����۴��"
                || top.API.CashInfo.Dealtype == "�Թ����" || top.API.CashInfo.Dealtype == "����ȡ��") { //�鳮ʱ����
	        return CallResponse("Exit");           
	    }else{
	        return CallResponse("OK");           
	    }        
    }
    
    //event handler
    function onEjectTimeout() {
        top.ErrorInfo = "��ӡʧ�ܣ����׽�����";
        return CallResponse("Exit");
    }
    //event handler
    function onDeviceError() {
        top.API.displayMessage("onDeviceError����");
        top.ErrorInfo = top.API.PromptList.No4;
        return CallResponse("Exit");
    }
     function onPrintFailed() {
        top.API.displayMessage("onPrintFailed����");
        top.ErrorInfo = top.API.PromptList.No4;
        return CallResponse("Exit");
    }
    //event handler
    function onPrintTaken() {
        if (top.API.gbPartCashIn || top.API.CashInfo.Dealtype == "�޿����۴��"
                 || top.API.CashInfo.Dealtype == "�Թ����" || top.API.CashInfo.Dealtype == "����ȡ��") { //�鳮ʱ����
            return CallResponse("Exit");  
        } else {
            return CallResponse("OK");           
        }
    }
    //Register the event
    function EventLogin() {
        top.Ptr.PrintComplete.connect(onPrintComplete);
        top.Ptr.PrintFailed.connect(onPrintFailed);
        top.Ptr.DeviceError.connect(onDeviceError);
        top.Ptr.PrintTaken.connect(onPrintTaken);
    }

    function EventLogout() {
        top.Ptr.PrintComplete.disconnect(onPrintComplete);
        top.Ptr.PrintFailed.disconnect(onPrintFailed);
        top.Ptr.DeviceError.disconnect(onDeviceError);
        top.Ptr.PrintTaken.disconnect(onPrintTaken);
    }

    //Countdown function
    function TimeoutCallBack() {
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