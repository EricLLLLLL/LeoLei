/*@create by:  tsxiong
*@time: 2016��03��20��
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
        // App.Plugin.Voices.play("voi_22");
        ButtonDisable();
        EventLogin();
        App.Timer.TimeoutDisposal(TimeoutCallBack);
        top.API.Pbk.ReadTrack('PassbookForm', 'Track1,Track2,Track3', 60000);
        // top.Scr.AcceptAndReadAvailableTracks('1,2,3', 60000);
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
        top.API.Pbk.CancelReadTrack();
        // top.Scr.CancelAccept();
        return CallResponse("Exit");
    }

    function SetCardNo() {
        var arrCardNo = new Array(strCardNo);
        top.API.gCardno = strCardNo;
        var nRet = top.Dat.SetDataSync(top.API.cardnoTag, top.API.cardnoType, arrCardNo);
        top.API.displayMessage("SetDataSync CARDNO Return:" + nRet);
        top.Jnl.PrintSync("InsertCardAction");
    }

     //event handler
    function onDeviceError() {
        top.API.displayMessage("onDeviceError");
    }

    function onReadTrackComplete(param) {
        top.API.displayMessage("onReadTrackComplete����");
        top.API.displayMessage(param);
        var arrTrack = new Array();
        arrTrack = param.split("|");
		var strTrack2 = arrTrack[1];
        // var strTrack2 = top.Scr.Track2();
        // if ("" === strTrack2) {
        //     top.ErrorInfo = top.API.PromptList.No7;
        //     return CallResponse("Error");
        // } else {
        var arrTrack2 = new Array();
        arrTrack2 = strTrack2.split("=");
        strCardNo = arrTrack2[1];
        //     if (arrTrack2.length == 2) {
        //         strCardNo = arrTrack2[0];
        //     }else{
		      //   var num = arrTrack2.length - 1;
        //         strCardNo = arrTrack2[num];
        //     }
        // }
        SetCardNo();
        return CallResponse('OK');
    }


    //@User code scope end 

    //event handler
    //event handler
    //Register the event
    function EventLogin() {
        top.Pbk.DeviceError.connect(onDeviceError);
        top.Pbk.ReadTrackComplete.connect(onReadTrackComplete);
    }

    function EventLogout() {
        top.Pbk.DeviceError.disconnect(onDeviceError);
        top.Pbk.ReadTrackComplete.disconnect(onReadTrackComplete);
    }

    //Countdown function
    function TimeoutCallBack() {
        top.ErrorInfo = top.API.PromptList.No3;
        // top.Scr.CancelAccept();
        top.API.Pbk.CancelReadTrack();
        return CallResponse('TimeOut');
    }
    //Page Return
    
    //remove all event handler
    function Clearup() {
        //TO DO:
        EventLogout();
        App.Timer.ClearTime();
        // App.Plugin.Voices.del();
    }
})();
