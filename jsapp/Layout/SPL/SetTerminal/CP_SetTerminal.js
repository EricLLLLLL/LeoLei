/*@create by:  tsxiong
*@time: 2016��03��20��
*/
; (function () {
	var CallResponse = App.Cntl.ProcessOnce( function(Response){
        //TO DO:
        Clearup();
        //Entry the flows control process.
        App.Cntl.ProcessDriven(Response);
    });
    var Initialize = function () {
        ButtonDisable();
        EventLogin();
        //@initialize scope start  
        App.Plugin.Keyboard.show("3", "KeysDiv", "Keyboards");
        funKeyInput();
        top.Dat.GetPersistentData(top.API.telleridTag, top.API.telleridType);
        ButtonEnable();
    }();//Page Entry
    function ButtonDisable() {
        document.getElementById('Exit').disabled = true;
        document.getElementById('OK').disabled = true;
    }

    function ButtonEnable() {
        document.getElementById('Exit').disabled = false;
        document.getElementById('OK').disabled = false;
    }

    //�������
    var strTellerNo = "";
    var strDeviceNo = "";
    var strBranchNo = "";
    var TellerNo = document.getElementById("TellerNoInput");
    var DeviceNo = document.getElementById("DeviceNoInput");
    var BranchNo = document.getElementById("BranchNoInput");
    var InputFlag = 0;
    TellerNo.focus();

    var oKeyboardKeyInput = document.getElementsByTagName("input");
    for (var j = 0; j < oKeyboardKeyInput.length; j++) {
        var inpt = oKeyboardKeyInput[j];
        inpt.onclick = function (e) {
            inputId = document.activeElement.id;
            if (inputId == "TellerNoInput") {
                InputFlag = 0;
            } else if (inputId == "DeviceNoInput") {
                InputFlag = 1;
            } else if (inputId == "BranchNoInput") {
                InputFlag = 2;
            }
        }
    }


    function funKeyInput() {
        var oKeyboardKey = document.getElementsByName("Name_Keyboard");
        for (var i = 0; i < oKeyboardKey.length; i++) {
            var keyEvent = oKeyboardKey[i];
            keyEvent.onclick = function (e) {
                if (this.innerText == "���") {
                    onClearNum();
                } else if (this.innerText == "Сд") {
                    document.getElementById("KeysDiv").removeChild(document.getElementById("Keyboards"));
                    App.Plugin.Keyboard.show("6", "KeysDiv", "Keyboards");
                    keyflag = 1;
                    funKeyInput();
                } else if (this.innerText == "��д") {
                    document.getElementById("KeysDiv").removeChild(document.getElementById("Keyboards"));
                    App.Plugin.Keyboard.show("3", "KeysDiv", "Keyboards");
                    keyflag = 1;
                    funKeyInput();
                } else {
                    if (InputFlag == 0) {
                        if (strTellerNo.length < 10) {
                            strTellerNo += this.innerText;
                            TellerNo.innerText = strTellerNo;

                        }
                        TellerNo.focus();
                    } else if (InputFlag == 1) {
                        if (strDeviceNo.length < 10) {
                            strDeviceNo += this.innerText;
                            DeviceNo.innerText = strDeviceNo;
                        }
                        DeviceNo.focus();
                    } else if (InputFlag == 2) {
                        if (strBranchNo.length < 10) {
                            strBranchNo += this.innerText;
                            BranchNo.innerText = strBranchNo;

                        }
                        BranchNo.focus();
                    }
                }
            }
        }
    }


    function onClearNum() {
        if (InputFlag == 0) {
            TellerNo.innerText = '';
            strTellerNo = '';
            TellerNo.focus();
        } else if (InputFlag == 1) {
            DeviceNo.innerText = '';
            strDeviceNo = '';
            DeviceNo.focus();
        } else if (InputFlag == 2) {
            BranchNo.innerText = '';
            strBranchNo = '';
            BranchNo.focus();
        }
    }


    //@User ocde scope start
    document.getElementById('OK').onclick = function () {
        if (strTellerNo == "") {
            document.getElementById("tipdiv").innerText = "����Ĺ�Ա�Ų���Ϊ�գ�";
        } else if (strDeviceNo == "") {
            document.getElementById("tipdiv").innerText = "������豸��Ų���Ϊ�գ�";
        } else if (strBranchNo == "") {
            document.getElementById("tipdiv").innerText = "����������кŲ���Ϊ�գ�";
        } else {
            var inputvalue1 = new Array(strTellerNo);
            top.API.displayMessage('��Ա�ţ�SetPersistentData TELLERID =' + inputvalue1);
            var nRet1 = top.Dat.SetPersistentData(top.API.telleridTag, top.API.telleridType, inputvalue1);
            top.API.displayMessage('��Ա�ţ�SetPersistentData TELLERID Return:' + nRet1);
        }
    }

    document.getElementById('Exit').onclick = function () {
        ButtonDisable();
        return CallResponse('Exit');
    }

    function onDatGetPersistentDataComplete(DataName, DataType, DataValue) {
        var arrGet = DataValue.toArray();
        var showinfo = arrGet[0];
        top.API.displayMessage("onDatGetPersistentDataComplete is done,DataName=" + DataName + ",DataType=" + DataType + ",DataValue=" + showinfo);
        if ('TELLERID' == DataName) {
            TellerNo.innerText = showinfo;
            strTellerNo = TellerNo.value;
            var nRet2 = top.Dat.GetPersistentData(top.API.terminalnumTag, top.API.terminalnumType);
            top.API.displayMessage("�ն˺ţ�GetPersistentData TERMINALNUM Return:" + nRet2);
        }
        if ('TERMINALNUM' == DataName) {
            DeviceNo.innerText = showinfo;
            strDeviceNo = DeviceNo.value;
            var nRet3 = top.Dat.GetPersistentData(top.API.subbanknumTag, top.API.subbanknumType);
            top.API.displayMessage("�����кţ�GetPersistentData SUBBANKNUM Return:" + nRet3);
        }
        if ('SUBBANKNUM' == DataName) {
            BranchNo.innerText = showinfo;
            strBranchNo = BranchNo.value;
        }
    }

    function onDatGetPersistentDataError(DataName, ErrorCode) {
        top.API.displayMessage("onDatGetPersistentDataError is done,DataName=" + DataName + ",ErrorCode=" + ErrorCode);
        alert("��ȡʧ��");
        if ('TELLERID' == DataName) {
            var nRet2 = top.Dat.GetPersistentData(top.API.terminalnumTag, top.API.terminalnumType);
            top.API.displayMessage("�ն˺ţ�GetPersistentData TERMINALNUM Return:" + nRet2);
        }
        if ('TERMINALNUM' == DataName) {
            var nRet3 = top.Dat.GetPersistentData(top.API.subbanknumTag, top.API.subbanknumType);
            top.API.displayMessage("�����кţ�GetPersistentData SUBBANKNUM Return:" + nRet3);
        }
    }

    function onDatSetPersistentDataComplete(DataName) {
        top.API.displayMessage("onDatSetPersistentDataComplete is done,DataName=" + DataName);
        if ('TELLERID' == DataName) {
            var inputvalue2 = new Array(strDeviceNo);
            top.API.displayMessage('�ն˺ţ�SetPersistentData TERMINALNUM =' + inputvalue2);
            var nRet2 = top.Dat.SetPersistentData(top.API.terminalnumTag, top.API.terminalnumType, inputvalue2);
            top.API.displayMessage('�ն˺ţ�SetPersistentData TERMINALNUM Return:' + nRet2);
        }
        if ('TERMINALNUM' == DataName) {
            var inputvalue3 = new Array(strBranchNo);
            top.API.displayMessage('�����кţ�SetPersistentData SUBBANKNUM =' + inputvalue3);
            var nRet3 = top.Dat.SetPersistentData(top.API.subbanknumTag, top.API.subbanknumType, inputvalue3);
            top.API.displayMessage('�����кţ�SetPersistentData SUBBANKNUM Return:' + nRet3);
        }
        if ('SUBBANKNUM' == DataName) {
            top.API.gSubBankNum = strBranchNo + "-" + strDeviceNo + "�Ż�";
            return CallResponse('OK');
        }
    }

    function onDatSetPersistentDataError(DataName, ErrorCode) {
        top.API.displayMessage("onDatSetPersistentDataError is done,DataName=" + DataName + ",ErrorCode=" + ErrorCode);
        alert("�趨ʧ��,�������趨��");
    }

    //@User code scope end 

    //Register the event
    function EventLogin() {
        top.Dat.GetPersistentDataComplete.connect(onDatGetPersistentDataComplete);
        top.Dat.GetPersistentDataError.connect(onDatGetPersistentDataError);
        top.Dat.SetPersistentDataComplete.connect(onDatSetPersistentDataComplete);
        top.Dat.SetPersistentDataError.connect(onDatSetPersistentDataError);
    }

    function EventLogout() {
        top.Dat.GetPersistentDataComplete.disconnect(onDatGetPersistentDataComplete);
        top.Dat.GetPersistentDataError.disconnect(onDatGetPersistentDataError);
        top.Dat.SetPersistentDataComplete.disconnect(onDatSetPersistentDataComplete);
        top.Dat.SetPersistentDataError.disconnect(onDatSetPersistentDataError);
    }

    function Clearup() {
        EventLogout();
    }
})();
