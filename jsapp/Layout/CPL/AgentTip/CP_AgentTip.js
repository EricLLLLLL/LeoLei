/*@create by:  hj
*@time: 2016��11��23��
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
        document.getElementById('Back').disabled = true;
        document.getElementById('Continue').disabled = true;
    }

    function ButtonEnable() {
        top.API.displayMessage("ButtonEnable");
        document.getElementById('Back').disabled = false;
        document.getElementById('Continue').disabled = false;
    }


    document.getElementById("Back").onclick = function () {
        top.API.displayMessage("���Back��ť,��Ӧ<Back>");
        ButtonDisable();
        return CallResponse("Back");
    }

    document.getElementById("Continue").onclick = function () {
        top.API.displayMessage("���Continue��ť,��Ӧ<OK>");
        ButtonDisable();
        top.API.gReadIdCardFlag = 1;
        return CallResponse("OK");
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
        App.Timer.ClearTime();
    }
})();