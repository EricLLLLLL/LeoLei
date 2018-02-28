/*@create by:  tsxiong
*@time: 2016Äê03ÔÂ20ÈÕ
*/
; (function () {
	var CallResponse = App.Cntl.ProcessOnce( function(Response){
        //TO DO:
        Clearup();
        //Entry the flows control process.
        App.Cntl.ProcessDriven(Response);
    });
    var Initialize = function () {
        EventLogin();
        //@initialize scope start
        App.Timer.TimeoutDisposal(TimeoutCallBack);
        //    top.Tcp.SendToHost("CHECKBALANCE", 60000);
        return CallResponse('OK');
    }();//Page Entry

    //@User ocde scope start
    //document.getElementById('OK').onclick = function(){
    //  
    //     return CallResponse('OK');
    //}

    //@User code scope end 

    //event handler
    function onTcpSendFailed() {
        top.API.displayMessage("onTcpSendFailed is done");
        return CallResponse('TimeOut');
    }
    function onTcpOnRecved(Check) {
        top.API.displayMessage("onTcpOnRecved is done,Check:" + Check);
        if (00 == Check) {
            return CallResponse('OK');
        } else { 
            return CallResponse('TimeOut');
        };
    }
    function onTcpTimeout() {
        top.API.displayMessage("onTcpTimeout is done");
        return CallResponse('TimeOut');
    }

    //Register the event
    function EventLogin() {
        top.Tcp.SendFailed.connect(onTcpSendFailed);
        top.Tcp.OnRecved.connect(onTcpOnRecved);
        top.Tcp.Timeout.connect(onTcpTimeout);
    }

    function EventLogout() {
        top.Tcp.SendFailed.disconnect(onTcpSendFailed);
        top.Tcp.OnRecved.disconnect(onTcpOnRecved);
        top.Tcp.Timeout.disconnect(onTcpTimeout);
    }

    //Countdown function
    function TimeoutCallBack() { 
        return CallResponse('TimeOut');
    }
    //Page Return
    
    //remove all event handler
    function Clearup() {
        //TO DO:
        EventLogout();
        App.Timer.ClearTime();
    }
})();
