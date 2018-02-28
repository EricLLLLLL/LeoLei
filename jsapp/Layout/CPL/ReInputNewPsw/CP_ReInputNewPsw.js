/*@create by:  tsxiong
*@time: 2016年03月20日
*/
; (function () {
    var Inputdata = "";
    var Pressing = 0;
    var ExitFlag = 0;
    var CardNo = "";
	var CallResponse = App.Cntl.ProcessOnce( function(Response){
        //TO DO:
        top.Siu.SetPinPadLight('OFF');
        Clearup();
        //Entry the flows control process.
        App.Cntl.ProcessDriven(Response);
    });
    var Initialize = function () {
        App.Timer.TimeoutDisposal(TimeoutCallBack);
        ButtonDisable();
        EventLogin();
        //@initialize scope start    
        top.Pin.GetPin(4, 6, true, "0,1,2,3,4,5,6,7,8,9,ENTER,CLEAR,CANCEL", "ENTER,CANCEL", -1);
        top.Siu.SetPinPadLight('CONTINUOUS');
        document.getElementById("InputPwd-input").focus();
        Pressing = 1;
        document.getElementById("InputPwd-error").innerHTML = "";
        CardNo = top.API.gCardno;
        ButtonEnable();
        App.Plugin.Voices.play("voi_11");
    } (); //Page Entry

    //@User ocde scope start
    //重新输入密码
    function Reinput() {
        top.API.displayMessage("开始重新输入密码");
        top.Pin.GetPin(4, 6, true, "0,1,2,3,4,5,6,7,8,9,ENTER,CLEAR,CANCEL", "ENTER,CANCEL", -1);
        Pressing = 1;
        Inputdata = "";
        document.getElementById("InputPwd-input").innerText = Inputdata;
        ButtonEnable();
    }
    //获取pinblock值
    function GetPinblock() {
        top.API.displayMessage("开始FormatPin");
        var pbCustomerdata = CardNo.substr(CardNo.length - 13, 12);
        var pbFormat = 'ANSI';
        var pbKeyname = 'PINKEY';
        top.Pin.PinBlock(pbFormat, pbCustomerdata, 15, pbKeyname, "", "");
    }

    function ButtonDisable() {
        top.API.displayMessage("ButtonDisable");
        document.getElementById('Exit').disabled = true;
        document.getElementById('OK').disabled = true;
    }

    function ButtonEnable() {
        top.API.displayMessage("ButtonEnable");
        document.getElementById('Exit').disabled = false;
        document.getElementById('OK').disabled = false;
    }

    document.getElementById("Exit").onclick = function () {
        top.API.displayMessage("点击Exit按钮,执行<top.API.Pin.CancelGetPin()>,响应<Exit>");
        ButtonDisable();
        top.Pin.CancelGetPin();
        //return CallResponse("Exit");
    }

    document.getElementById("OK").onclick = function () {
        top.API.displayMessage("点击OK按钮,执行<top.API.Pin.CancelGetPin()>,响应<RePsw>");
        ButtonDisable();
        Pressing = 2;
        top.Pin.CancelGetPin();
    }

    //@User code scope end 

    //event handler
    function onKeyPressed(key, keyCode) {
        top.API.displayMessage("onKeyPressed触发，key=" + key);
        document.getElementById("InputPwd-error").innerHTML = "";
        if ((0 == key || key <= 9) || ('*' == key)) {
            Inputdata += "*";
            document.getElementById("InputPwd-input").innerText = Inputdata;
        } else if (key === "CLEAR") {
            Inputdata = "";
            document.getElementById("InputPwd-input").innerText = Inputdata;
        }
    }
    //event handler
    function onGetPinCompleted() {
        top.API.displayMessage("onGetPinCompleted触发，Inputdata.length=" + Inputdata.length);
        Pressing = 0;
        if (Inputdata.length < 6) {
            document.getElementById("InputPwd-error").innerHTML = "密码有误，请重新输入";
            Reinput();
        } else {            
            document.getElementById("InputPwd-error").innerHTML = "";            
            top.API.displayMessage("开始getPinblock");
            GetPinblock();
        }
    }
    //event handler
    function onTimeout() {
        top.API.displayMessage("onTimeout触发,响应<Exit>");
        top.ErrorInfo = top.API.PromptList.No3;
        return CallResponse("TimeOut");
    }
    //event handler
    function onPinBlockCompleted(Pinblock) {
        //top.API.displayMessage("onPinBlockCompleted触发");
        top.API.displayMessage("onPinBlockCompleted触发,Pinblock=" + Pinblock.toArray());
        var strPinblock = '';
        var hexArray = new Array();
        var ntmp = 0;
        hexArray = Pinblock.toArray();
        for (var index = 0; index < 8; index++) {
			ntmp = parseInt(hexArray[index] % 65535);
			if(ntmp < 16){
				if(ntmp == 0){
					strPinblock += "00";
				}else{
					strPinblock += "0" + ntmp.toString(16);
				}				
			}else{
				strPinblock += ntmp.toString(16);
			}            
        }
        top.API.displayMessage("Pinblock(字符串)=" + strPinblock);
        if (top.API.gNewPinBlock == strPinblock) {
            return CallResponse("OK");
        }else{
            document.getElementById("InputPwd-error").innerHTML = "两次密码不一致";
            Reinput();
        }
    }

    function onGetPinCancelled() {
        top.API.displayMessage("onGetPinCancelled触发");
        if (Pressing == 2) {
            return CallResponse("RePsw");
        } else if (Pressing == 3) {
            top.API.displayMessage("页面超时触发，响应<TimeOut>");
            top.ErrorInfo = top.API.PromptList.No3;
            return CallResponse("TimeOut");
        } else {
            top.API.displayMessage("密码键盘触发，响应<Exit>");
            top.ErrorInfo = top.API.PromptList.No2;
            return CallResponse("Exit");
        }
    }

    function onDeviceError() {
        top.API.displayMessage("onDeviceError触发");
        top.ErrorInfo = top.API.PromptList.No4;
        return CallResponse("Exit");
        //return CallResponse("onDeviceError");
    }

    function onGetPinFailed() {
        top.API.displayMessage("onGetPinFailed触发");
        top.ErrorInfo = top.API.PromptList.No4;
        return CallResponse("Exit");
    }
   function onPinBlockFailed() {
        top.API.displayMessage("onPinBlockFailed触发");
        top.ErrorInfo = top.API.PromptList.No4;
        return CallResponse("Exit");
    }

   function onGetPinCancelFailed() {
        top.API.displayMessage("onGetPinCancelFailed触发");
        top.ErrorInfo = top.API.PromptList.No4;
        return CallResponse("Exit");
    }

    //Register the event
    function EventLogin() {
        top.Pin.KeyPressed.connect(onKeyPressed);
        top.Pin.GetPinCompleted.connect(onGetPinCompleted);
        top.Pin.GetPinFailed.connect(onGetPinFailed);
        top.Pin.DeviceError.connect(onDeviceError);
        top.Pin.GetPinCancelled.connect(onGetPinCancelled);
        top.Pin.GetPinCancelFailed.connect(onGetPinCancelFailed);
        top.Pin.Timeout.connect(onTimeout);
        top.Pin.PinBlockCompleted.connect(onPinBlockCompleted);
        top.Pin.PinBlockFailed.connect(onPinBlockFailed);
    }

    function EventLogout() {
        top.Pin.GetPinCancelled.disconnect(onGetPinCancelled);
        top.Pin.GetPinCancelFailed.disconnect(onGetPinCancelFailed);
        top.Pin.KeyPressed.disconnect(onKeyPressed);
        top.Pin.GetPinCompleted.disconnect(onGetPinCompleted);
        top.Pin.GetPinFailed.disconnect(onGetPinFailed);
        top.Pin.DeviceError.disconnect(onDeviceError);
        top.Pin.Timeout.disconnect(onTimeout);
        top.Pin.PinBlockCompleted.disconnect(onPinBlockCompleted);
        top.Pin.PinBlockFailed.disconnect(onPinBlockFailed);
    }

    //Countdown function
    function TimeoutCallBack() {
        top.API.displayMessage("页面超时,执行<top.API.Pin.CancelGetPin()>,响应<TimeOut>");
        Pressing = 3;
        top.Pin.CancelGetPin();
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