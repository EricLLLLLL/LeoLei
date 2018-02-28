/*@create by:  tsxiong
*@time: 2016��03��20��
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
                document.getElementById('err-tip').innerText = "��ȡ���֤��Ϣʧ�ܣ������¸�Ӧ��";
                ButtonEnable();
            }
        } else {
			var sDate = top.GetDate12byte().substring(0, 8);
			if(parseInt(sDate) - parseInt(arrIDNO[3]) < 160000){
				document.getElementById('err-tip').innerText = "����δ��16���꣬���������֤����";
                ButtonEnable();
			}else if(parseInt(arrIDNO[8]) < parseInt(sDate)){
				document.getElementById('err-tip').innerText = "�����֤�ѹ��ڣ����������֤����";
                ButtonEnable();
			}else{
				top.API.gIdName = arrIDNO[0];
				top.API.gIdNumber = arrIDNO[4];
				document.getElementById('err-tip').innerText = "���������˲飬���Ժ�";
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
                top.ErrorInfo = "��ȡ����֤��Ϣʧ�ܣ����׽���";
                return CallResponse('Exit');
            } else {
                document.getElementById('err-tip').innerText = "��ȡ���֤��Ϣʧ�ܣ������¸�Ӧ��";
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
                top.ErrorInfo = "��ȡ����֤��Ϣʧ�ܣ����׽���";
                return CallResponse('Exit');
            } else {
                document.getElementById('err-tip').innerText = "��ȡ���֤��Ϣʧ�ܣ������¸�Ӧ��";
            }
        }

    }

    function onCardInvalid() {
        top.API.displayMessage("onCardInvalid");
        DeviceErrTimes++;
        if (DeviceErrTimes == 10) {
            top.ErrorInfo = "��ȡ����֤��Ϣʧ�ܣ����׽���";
            return CallResponse('Exit');
        } else {
            document.getElementById('err-tip').innerText = "��ȡ���֤��Ϣʧ�ܣ������¸�Ӧ��";
        }
    }

	    /********************************************************************************************************/    
    //��������ģ��
    function onDatGetPersistentDataComplete(DataName, DataType, DataValue) {
        var arrDataValue = DataValue.toArray();
        if ('IDCHECKURL' == DataName) {
            URL = arrDataValue[0];
            if (URL == "") {
                top.ErrorInfo = "�����˲��쳣��";
                return CallResponse('Exit');
            }else{
				var strParam = top.API.gIdName + "," + top.API.gIdNumber + "," + "" + "," + URL;
				if(!top.CheckInfo(strParam)){
					top.API.displayMessage("�����˲�ʧ�ܣ�");
					top.ErrorInfo = "�����˲�ʧ�ܣ�";
					return CallResponse('Exit');
				}else{
					top.API.displayMessage("�����˲�ɹ���");					
					return CallResponse('OK');
				}                
            }            
        }   
    }

    function onDatGetPersistentDataError(DataName, ErrorCode) {
        top.ErrorInfo = "�����˲��쳣��";
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
