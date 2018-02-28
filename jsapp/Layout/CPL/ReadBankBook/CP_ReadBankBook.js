/*@create by:  tsxiong
*@time: 2016年03月20日
*/
; (function () {
    var strCardNo = "";
	var CallResponse = App.Cntl.ProcessOnce( function(Response){
        //TO DO:
        Clearup();
        //Entry the flows control process.
        App.Cntl.ProcessDriven(Response);
    });
    var Initialize = function () {
        //@initialize scope start
        App.Plugin.Voices.play("voi_22");
        ButtonDisable();
        EventLogin();
        App.Timer.TimeoutDisposal(TimeoutCallBack);
        top.Scr.AcceptAndReadAvailableTracks('1,2,3', 60000);
        ButtonEnable();
        //return CallResponse('OK');
    } (); //Page Entry

    //@User ocde scope start
    function ButtonDisable() {
        top.API.displayMessage("ButtonDisable");
        document.getElementById('Exit').disabled = true;
    }

    function ButtonEnable() {
        top.API.displayMessage("ButtonEnable");
        document.getElementById('Exit').disabled = false;

    }
    document.getElementById("Exit").onclick = function () {
        ButtonDisable();
        top.Scr.CancelAccept();
        return CallResponse("Exit");
    }

    function SetCardNo() {
        var arrCardNo = new Array(strCardNo);
        top.API.gCardno = strCardNo;
        var nRet = top.Dat.SetDataSync(top.API.cardnoTag, top.API.cardnoType, arrCardNo);
        top.API.displayMessage("SetDataSync CARDNO Return:" + nRet);
        top.Jnl.PrintSync("PassbookAction");
    }

     //event handler
    function onCardInserted() {
        top.API.displayMessage("This event should not have been there");
    }

    function onCardAccepted() {
        top.API.displayMessage("onCardAccepted触发");
        var strTrack2 = top.Scr.Track2();
        if ("" === strTrack2) {
            top.ErrorInfo = top.API.PromptList.No9;
            return CallResponse("Error");
        } else {
            var arrTrack2 = new Array();
            arrTrack2 = strTrack2.split("=");
            if (arrTrack2.length == 2) {
                strCardNo = arrTrack2[0];
            }else{
		        var num = arrTrack2.length - 1;
                strCardNo = arrTrack2[num];
            }
        }
        SetCardNo();
        return CallResponse('OK');
    }

    function onCardAcceptCancelled() {
        top.API.displayMessage("onCardAcceptCancelled");
        top.ErrorInfo = top.API.PromptList.No2;
        return CallResponse("TimeOut");
    }

    function onCardRead() {
        return CallResponse("TimeOut");
    }

    function onCardInvalid() {
        top.API.displayMessage("onCardInvalid,响应<TimeOut>");
        top.ErrorInfo = top.API.PromptList.No9;
        return CallResponse("TimeOut");
    }

    function onDeviceError() {
        top.API.displayMessage("onDeviceError,响应<TimeOut>");
        top.ErrorInfo = top.API.PromptList.No4;
        return CallResponse("TimeOut");
    }

    function onCardAcceptFailed() {
        top.API.displayMessage("onCardAcceptFailed");        
        refreshTitleAdv();
    }
     function onCardAcceptCancelFailed() {
        top.API.displayMessage("onCardAcceptCancelFailed");        
        refreshTitleAdv();
    }

    //@User code scope end 

    //event handler
    //event handler
    //Register the event
    function EventLogin() {
        top.Scr.CardInserted.connect(onCardInserted);
        top.Scr.CardAccepted.connect(onCardAccepted);
        top.Scr.CardAcceptFailed.connect(onCardAcceptFailed);
        top.Scr.CardAcceptCancelled.connect(onCardAcceptCancelled);
        top.Scr.CardAcceptCancelFailed.connect(onCardAcceptCancelFailed);
        top.Scr.CardRead.connect(onCardRead);
        top.Scr.CardInvalid.connect(onCardInvalid);
        top.Scr.DeviceError.connect(onDeviceError);
    }

    function EventLogout() {
        top.Scr.CardInserted.disconnect(onCardInserted);
        top.Scr.CardAccepted.disconnect(onCardAccepted);
        top.Scr.CardAcceptFailed.disconnect(onCardAcceptFailed);
        top.Scr.CardAcceptCancelled.disconnect(onCardAcceptCancelled);
        top.Scr.CardAcceptCancelFailed.disconnect(onCardAcceptCancelFailed);
        top.Scr.CardRead.disconnect(onCardRead);
        top.Scr.CardInvalid.disconnect(onCardInvalid);
        top.Scr.DeviceError.disconnect(onDeviceError);
    }

    //Countdown function
    function TimeoutCallBack() {
        top.ErrorInfo = top.API.PromptList.No3;
        top.Scr.CancelAccept();
        return CallResponse('TimeOut');
    }
    //Page Return
    
    //remove all event handler
    function Clearup() {
        //TO DO:
        EventLogout();
        App.Timer.ClearTime();
        App.Plugin.Voices.del();
    }
})();
