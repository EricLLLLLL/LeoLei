/*@create by:  tsxiong
*@time: 2016��03��20��
*/
; (function () {
    var TransType = null;
	var CallResponse = App.Cntl.ProcessOnce( function(Response){
        //TO DO:
        Clearup();
        //Entry the flows control process.
        App.Cntl.ProcessDriven(Response);
    });
    var Initialize = function () {
        //EventLogin();
        //@initialize scope start
        ButtonDisable();
        App.Timer.TimeoutDisposal(TimeoutCallBack);
        document.getElementById("Logo-Err").style.display = "block";
        document.getElementById("tip_label").innerText = top.ErrorInfo;
        top.ErrorInfo = top.API.PromptList.No6;
        ButtonEnable();
    } (); //Page Entry

    //@User ocde scope start
    function ButtonDisable() {
        top.API.displayMessage("ButtonDisable");
        document.getElementById('Exit').disabled = true;
        document.getElementById('Back').disabled = true;
    }

    function ButtonEnable() {
        top.API.displayMessage("ButtonEnable");
        document.getElementById('Exit').disabled = false;
        document.getElementById('Back').disabled = false;
    }

    document.getElementById('Exit').onclick = function () {
        top.API.displayMessage("���<�˳�>��ť,��Ӧ<Exit>");
        ButtonDisable();
        return CallResponse('Exit');
    }

    document.getElementById('Back').onclick = function () {
        top.API.displayMessage("���<���°���>��ť,��Ӧ<Back>");
        ButtonDisable();
        return CallResponse('Back');
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
        App.Timer.ClearTime();
    }
})();
