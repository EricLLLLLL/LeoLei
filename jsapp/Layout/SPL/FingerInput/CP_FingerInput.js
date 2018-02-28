; (function () {
var bError =false;
	var CallResponse = App.Cntl.ProcessOnce( function(Response){
        //TO DO:
        Clearup();
        //Entry the flows control process.
        App.Cntl.ProcessDriven(Response);
    });
    var Initialize = function () {
        ButtonDisable();
        EventLogin();
        document.getElementById("PromptIcon3").style.backgroundImage = "url('Framework/style/Graphics/box_ico_4.png')";
        top.Fpi.AcquireData(-1);
        ButtonEnable();
    }();//Page Entry

    function ButtonDisable() {
        document.getElementById('Exit').disabled = true;
    }

    function ButtonEnable() {
        document.getElementById('Exit').disabled = false;
    }
    //@User ocde scope start
    document.getElementById('Exit').onclick = function () {
        ButtonDisable();
	if(!bError){
        top.Fpi.CancelIdentify();
	}
        return CallResponse('Exit');
    }

    function onDataAcquired(data) {
        top.API.displayMessage("onDataAcquired is done");
        var fingerData=data.toArray();
        if ((fingerData[0] != null) && (fingerData[0] != "") && (fingerData[0] != undefined)) {
			{
                document.getElementById("PromptIcon3").style.backgroundImage = "url('Framework/style/Graphics/box_ico_3.png')";
				return CallResponse('OK');
            }
        }
        
    }
    function onFpiTimeout() {
        top.API.displayMessage("onFpiTimeout is done");
		document.getElementById("FingerTipDiv").innerHTML="Ö¸ÎÆÂ¼Èë³¬Ê± ! !";
		bError = true;
    }
    function onFpiDeviceError() {
        top.API.displayMessage("onFpiDeviceError is done");
		document.getElementById("FingerTipDiv").innerHTML="Ö¸ÎÆÂ¼ÈëÊ§°Ü ! !";
		bError = true;      
    }
    function onFingerMoved() {
        top.API.displayMessage("onFingerMoved is done");    
    }
    function onDataAcquireFailed() {
        top.API.displayMessage("DataAcquireFailed is done");
		document.getElementById("FingerTipDiv").innerHTML="Ö¸ÎÆÂ¼ÈëÊ§°Ü ! !";
		bError = true;
    }
    function onAcquireDataCancelled() {
        top.API.displayMessage("AcquireDataCancelled is done");
		document.getElementById("FingerTipDiv").innerHTML="Ö¸ÎÆÂ¼ÈëÊ§°Ü ! !";
		bError = true;  
    }

    //Register the event
    function EventLogin() {
        top.Fpi.DataAcquired.connect(onDataAcquired);
        top.Fpi.Timeout.connect(onFpiTimeout);
        top.Fpi.DeviceError.connect(onFpiDeviceError);
        top.Fpi.FingerMoved.connect(onFingerMoved);
        top.Fpi.DataAcquireFailed.connect(onDataAcquireFailed);
        top.Fpi.AcquireDataCancelled.connect(onAcquireDataCancelled);
    }

    function EventLogout() {
        top.Fpi.DataAcquired.disconnect(onDataAcquired);
        top.Fpi.Timeout.disconnect(onFpiTimeout);
        top.Fpi.DeviceError.disconnect(onFpiDeviceError);
        top.Fpi.FingerMoved.disconnect(onFingerMoved);
        top.Fpi.DataAcquireFailed.disconnect(onDataAcquireFailed);
        top.Fpi.AcquireDataCancelled.disconnect(onAcquireDataCancelled);
    }

    //Countdown function
    function TimeoutCallBack() {
        return CallResponse('TimeOut');
    }

    //remove all event handler
    function Clearup() {
        EventLogout();
    }
})();
