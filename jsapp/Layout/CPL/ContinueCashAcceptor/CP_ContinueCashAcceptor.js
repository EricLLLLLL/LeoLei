/*@create by:  tsxiong
*@time: 2016年03月20日
*/
; (function () {
    var nTransAmount;
	var CallResponse = App.Cntl.ProcessOnce( function(Response){
        //TO DO:
        Clearup();
        App.Cntl.ProcessDriven(Response);
    });
    var Initialize = function () {
        App.Timer.TimeoutDisposal(TimeoutCallBack);
        ButtonDisable();
        nTransAmount = top.Cim.GetLastAcceptedAmountSync("CNY", "VALID");
		if (nTransAmount > 0) {
			document.getElementById('Back').style.display = "none";
			document.getElementById('Exit').style.display = "none";
		} else {
			document.getElementById('Back').style.display = "block";
			document.getElementById('Exit').style.display = "block";
		}
		ButtonEnable();
        App.Plugin.Voices.play("voi_6");
    }();//Page Entry

    //@User ocde scope start
    function ButtonDisable() {
        document.getElementById('Back').disabled = true;
		document.getElementById('Exit').disabled = true;
        document.getElementById('OK').disabled = true;
		document.getElementById('End').disabled = true;
    }
    function ButtonEnable() {
        document.getElementById('Back').disabled = false;
		document.getElementById('Exit').disabled = false;
        document.getElementById('OK').disabled = false;
		document.getElementById('End').disabled = false;
    }
    document.getElementById('OK').onclick = function () {
        ButtonDisable();
		var tmp = top.Cim.StInputStatus();
        if (tmp == "EMPTY") {
			ButtonEnable();
			//汉字提示            
            App.Plugin.Voices.play("voi_46");//新增语音
        }else{
			return CallResponse('OK');
		}
    }
	
	document.getElementById('End').onclick = function () {
		ButtonDisable();
		return CallResponse('EndPutMoney');
    }
    document.getElementById('Exit').onclick = function () {
		ButtonDisable();
        top.ErrorInfo = top.API.PromptList.No2;
		top.Cim.StoreEscrowedCash(top.API.gStoreEscrowedCashTimeOut);
		return CallResponse('Exit');
    }
	
	document.getElementById('Back').onclick = function () {
		ButtonDisable();
        top.ErrorInfo = top.API.PromptList.No2;
		top.Cim.StoreEscrowedCash(top.API.gStoreEscrowedCashTimeOut);
		return CallResponse('Back');
    }


    function TimeoutCallBack() {
        top.ErrorInfo = top.API.PromptList.No3;
		return CallResponse('TimeOut');        
    }
    //Page Return
    
    //remove all event handler
    function Clearup() {
        App.Plugin.Voices.del();
        App.Timer.ClearTime();
    }
})();
