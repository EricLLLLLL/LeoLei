/*@create by:  tsxiong
*@time: 2016��03��20��
*/
; (function () {
    var TransType = null;
    var strCallResponse = "TradeFailed";
	var CallResponse = App.Cntl.ProcessOnce( function(Response){
        //TO DO:
        Clearup();
        //Entry the flows control process.
        App.Cntl.ProcessDriven(Response);
    });
    var Initialize = function () {
        //EventLogin();
        //@initialize scope start
        App.Timer.TimeoutDisposal(TimeoutCallBack);
        TransType = top.API.gTransactiontype;    
        if (((top.ErrorInfo == "ͨѶ��ʱ�����׽���!") || (top.API.gResponsecode == "89")) && (TransType == "DEP"))
        {
            top.ErrorInfo = "ҵ����ʧ��,�������з���ϵ!";
            strCallResponse = "NoRollback";
        }
        if (top.API.gResponsecode == "55")
        {
            strCallResponse = "ReInputPsw";
        }
        if (top.API.gResponsecode == "04" || top.API.gResponsecode == "33" ||
            top.API.gResponsecode == "41" || top.API.gResponsecode == "43" ||
            top.API.gResponsecode == "07" || top.API.gResponsecode == "34" ||
            top.API.gResponsecode == "35" || top.API.gResponsecode == "36" ||
            top.API.gResponsecode == "37" || top.API.gResponsecode == "67"  ) {
            if (TransType != "DEP") {
                strCallResponse = "Capture";
            }
        }
        
        if ((TransType == "DEP") || (TransType == "CWD") ) {
            var arrTransactionResult  = new Array("����ʧ��");
            var nRet = top.Dat.SetDataSync("TRANSACTIONRESULT", "STRING", arrTransactionResult);
            top.API.displayMessage("SetDataSync TRANSACTIONRESULT Return:" + nRet);
        }
        if ((TransType == "CWC")) {
            strCallResponse = "CWC";
        }
        document.getElementById("Logo-Err").style.display = "block";
        document.getElementById("tip_label").innerText = top.ErrorInfo;        
        top.ErrorInfo = top.API.PromptList.No6;
    }();//Page Entry

    //@User ocde scope start
    function ButtonDisable() {
        top.API.displayMessage("ButtonDisable");
        //document.getElementById('OK').disabled = true;
    }

    function ButtonEnable() {
        top.API.displayMessage("ButtonEnable");
        //document.getElementById('OK').disabled = false;
    }



    //@User code scope end 
    //event handler

    //Register the event
    function EventLogin() {
    }

    function EventLogout() {
    }

    //Countdown function
    function TimeoutCallBack() {
        return CallResponse(strCallResponse);
    }
    //Page Return
    
    //remove all event handler
    function Clearup() {
        //TO DO:
        //EventLogout();
        App.Timer.ClearTime();
    }
})();
