/*@create by:  tsxiong
*@time: 2016��03��20��
*/
; (function () {
	var CallResponse = App.Cntl.ProcessOnce( function(Response){
        //TO DO:
        Clearup();
        App.Cntl.ProcessDriven(Response);
    });
    var Initialize = function () {
        App.Timer.TimeoutDisposal(TimeoutCallBack);
    }(); //Page Entry

    function ButtonDisable() {
        top.API.displayMessage("ButtonDisable");
        document.getElementById('Exit').disabled = true;
    }

    function ButtonEnable() {
        top.API.displayMessage("ButtonEnable");
        document.getElementById('Exit').disabled = false;
    }


	document.getElementById("Exit").onclick = function(){
        top.API.displayMessage("���Exit��ť,��Ӧ<Exit>");
        ButtonDisable();
        top.ErrorInfo = top.API.PromptList.No2;
		return CallResponse("Exit");
	}

    //Countdown function
    function TimeoutCallBack() {
        top.API.displayMessage("ҳ�泬ʱ,��Ӧ<TimeOut>");
        top.ErrorInfo = top.API.PromptList.No3;
        return CallResponse("Exit");
    }
    //Page Return
    
    //remove all event handler
    function Clearup() {
        //TO DO:
        App.Timer.ClearTime();
    }
})();