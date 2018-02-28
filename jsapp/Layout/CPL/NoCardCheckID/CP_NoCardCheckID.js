/*@create by:  tsxiong
*@time: 2016年03月20日
*/
; (function () {
    var DeviceErrTimes = 0;
    var ReadIdCardFlag;
	var CallResponse = App.Cntl.ProcessOnce( function(Response){
        //TO DO:
        top.Siu.SetScannerLight('OFF');
        Clearup();
        //Entry the flows control process.
        App.Cntl.ProcessDriven(Response);
    });
    var Initialize = function () {
        EventLogin();
        App.Timer.TimeoutDisposal(TimeoutCallBack);
        ButtonDisable();
		top.Idr.AcceptAndRead(-1);        
        top.Siu.SetScannerLight('QUICK');
        ButtonEnable();
        App.Plugin.Voices.play("voi_5");
    }();//Page Entry

    //@User ocde scope start
    function ButtonDisable() {
        document.getElementById('Exit').disabled = true;
    }

    function ButtonEnable() {
        document.getElementById('Exit').disabled = false;
    }

    document.getElementById('Exit').onclick = function () {
        ButtonDisable();
        top.Idr.CancelAccept();
        top.ErrorInfo = top.API.PromptList.No2;
        return CallResponse('Exit');
    }

    
    function onCardInserted() {
        //alert("onCardInserted");
    }
    //event handler
    function onCardAccepted(info) {
        ButtonDisable();
        var arrIDNO = new Array();
        arrIDNO = info.split(",");
        if (arrIDNO[0] == "") {
            DeviceErrTimes++;
            if (DeviceErrTimes == 10) {
                top.ErrorInfo = top.API.PromptList.No7;
                return CallResponse('Exit');
            } else {
                document.getElementById('err-tip').innerText = "读取身份证信息失败，请重新感应！";
                ButtonEnable();
            }
        } else {
			var sDate = top.GetDate12byte().substring(0, 8);
			if(parseInt(sDate) - parseInt(arrIDNO[3]) < 160000){
				document.getElementById('err-tip').innerText = "年龄未满16周岁，请更换其他证件！";
                ButtonEnable();
			}else if(parseInt(arrIDNO[8]) < parseInt(sDate)){
				document.getElementById('err-tip').innerText = "该身份证已过期，请更换其他证件！";
                ButtonEnable();
			}else{
				top.API.gIdName = arrIDNO[0];
				top.API.gIdNumber = arrIDNO[4];
				document.getElementById('err-tip').innerText = "正在联网核查，请稍候！";
				top.Dat.GetPersistentData("IDCHECKURL", "STRING");    
			}
            
        }
    }

    function onCardTaken() {
		document.getElementById('err-tip').innerText ="";
		document.getElementById('AgentErrTip').style.display="none";
		top.Idr.AcceptAndRead(-1);
    }
    function onTimeout() {
        top.API.displayMessage("onTimeout");
    }
    function onDeviceError() {
        top.API.displayMessage("onDeviceError");
        if (top.Idr.StDetailedDeviceStatus() != "ONLINE") {
            top.ErrorInfo = top.API.PromptList.No7;
        } else {
            DeviceErrTimes++;
            if (DeviceErrTimes == 10) {
                top.ErrorInfo = "读取二代证信息失败，交易结束";
                return CallResponse('Exit');
            } else {
                document.getElementById('err-tip').innerText = "读取身份证信息失败，请重新感应！";
            }
        }

    }

    function onCardAcceptFailed() {
        top.API.displayMessage("onCardAcceptFailed");
        if (top.Idr.StDetailedDeviceStatus() != "ONLINE") {
            top.ErrorInfo = top.API.PromptList.No7;
        } else {
            DeviceErrTimes++;
            if (DeviceErrTimes == 10) {
                top.ErrorInfo = "读取二代证信息失败，交易结束";
                return CallResponse('Exit');
            } else {
                document.getElementById('err-tip').innerText = "读取身份证信息失败，请重新感应！";
            }
        }

    }

    function onCardInvalid() {
        top.API.displayMessage("onCardInvalid");
        DeviceErrTimes++;
        if (DeviceErrTimes == 10) {
            top.ErrorInfo = "读取二代证信息失败，交易结束";
            return CallResponse('Exit');
        } else {
            document.getElementById('err-tip').innerText = "读取身份证信息失败，请重新感应！";
        }
    }

	    /********************************************************************************************************/    
    //永久数据模块
    function onDatGetPersistentDataComplete(DataName, DataType, DataValue) {
        var arrDataValue = DataValue.toArray();
        if ('IDCHECKURL' == DataName) {
            URL = arrDataValue[0];
            if (URL == "") {
                top.ErrorInfo = "联网核查异常！";
                return CallResponse('Exit');
            }else{
				var strParam = top.API.gIdName + "," + top.API.gIdNumber + "," + "" + "," + URL;
				if(!top.CheckInfo(strParam)){
					top.API.displayMessage("联网核查失败！");
					top.ErrorInfo = "联网核查失败！";
					return CallResponse('Exit');
				}else{
					top.API.displayMessage("联网核查成功！");					
					return CallResponse('OK');
				}                
            }            
        }   
    }

    function onDatGetPersistentDataError(DataName, ErrorCode) {
        top.ErrorInfo = "联网核查异常！";
                return CallResponse('Exit');
    }
    //Register the event
    function EventLogin() {
        top.Idr.CardInserted.connect(onCardInserted);
        top.Idr.CardAccepted.connect(onCardAccepted);
        top.Idr.CardAcceptFailed.connect(onCardAcceptFailed);
        top.Idr.CardTaken.connect(onCardTaken);
        top.Idr.DeviceError.connect(onDeviceError);
        top.Idr.Timeout.connect(onTimeout);
        top.Idr.CardInvalid.connect(onCardInvalid);
		top.Dat.GetPersistentDataComplete.connect(onDatGetPersistentDataComplete);
        top.Dat.GetPersistentDataError.connect(onDatGetPersistentDataError);
    }

    function EventLogout() {
        top.Idr.CardInserted.disconnect(onCardInserted);
        top.Idr.CardAccepted.disconnect(onCardAccepted);
        top.Idr.CardAcceptFailed.disconnect(onCardAcceptFailed);
        top.Idr.CardTaken.disconnect(onCardTaken);
        top.Idr.Timeout.disconnect(onTimeout);
        top.Idr.DeviceError.disconnect(onDeviceError);
        top.Idr.CardInvalid.disconnect(onCardInvalid);
		top.Dat.GetPersistentDataComplete.disconnect(onDatGetPersistentDataComplete);
        top.Dat.GetPersistentDataError.disconnect(onDatGetPersistentDataError);
    }

    //Countdown function
    function TimeoutCallBack() {
        top.Idr.CancelAccept();
        top.ErrorInfo = top.API.PromptList.No3;
        return CallResponse('TimeOut');
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
