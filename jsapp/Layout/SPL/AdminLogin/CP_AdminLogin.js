// JavaScript Document
/*@create by:  tsxiong
 *@time: 2016��03��20��
 */
;(function () {
    var bPinOpen = false;
    var clickEnter = false;
    var bGetData = false;
    var AdminObj;
    var AdminInfo;
    var User = document.getElementById("UserInput");
    var Password = document.getElementById("PswInput");
    User.focus();
    var Element;
    var user_tip = document.getElementById("user_tip");
    var password_tip = document.getElementById("password_tip");
    var CallResponse = App.Cntl.ProcessOnce(function (Response) {
        //TO DO:
        Clearup();
        //Entry the flows control process.
        App.Cntl.ProcessDriven(Response);
    });
    var Initialize = function () {
        EventLogin();
        App.Plugin.Keyboard.show("2", "PageSubject", "KeyboardDiv");
        document.getElementById("KeyboardKey_set").id = "KeyboardKey_login";
        Element = User;
    }(); //Page Entry

    //��������¼�
    User.onclick = function () {
        if (!bGetData) {
            top.Dat.GetPersistentData("ADMININFO", "STRING");
        }
        Element = User;
        App.InputEdit.getCurPosition(Element);
    }

    Password.onclick = function () {
        if (!bGetData) {
            top.Dat.GetPersistentData("ADMININFO", "STRING");
        }
        Element = Password;
        App.InputEdit.getCurPosition(Element);
    }

    var oKeyboardKey = document.getElementsByTagName("span");
    for (var i = 0; i < oKeyboardKey.length; i++) {
        var keyEvent = oKeyboardKey[i];
        keyEvent.onclick = function (e) {
            ClearTip();
            if ('�˸�' == this.innerText) {
                App.InputEdit.getInput(Element, 1, "BS");
            } else if ('���' == this.innerText) {
                App.InputEdit.getInput(Element, 1, "CLEAR");
            } else {
                if (Element.value.length < 6) {
                    App.InputEdit.getInput(Element, 0, this.innerText);
                }
            }
        }
    }

    function ClearTip() {
        user_tip.innerText = "";
        password_tip.innerText = "";
    }

    document.getElementById('KeyboardKey_login').onclick = function () {
        if ((User.value == "") || (Password.value == "")) {
            if (User.value == "") {
                user_tip.innerText = "*�˺Ų���Ϊ��";
            }
            if (Password.value == "") {
                password_tip.innerText = "*��¼���벻��Ϊ��";
            }
        } else {
            var bOk = false;
            for (var i = 0; i < AdminInfo.length; i++) {
                if ((User.value == AdminInfo[i].user) && (Password.value == AdminInfo[i].pw)) {
                    bOk = true;
                }
            }
            if (bOk) {
                var ErrTip = "����Ա��" + User.value + "����¼�ɹ�";
                top.Jnl.PrintSync("AdminLoginSuccess");
                if (User.value == "00") {
                    top.API.AdminStatus = 0;
                } else {
                    top.API.AdminStatus = 1;
                }
                top.Sys.SetIsMaintain(true);
                return CallResponse('OK');
            } else {
                var ErrTip = "����Ա��" + User.value + "����¼ʧ��";
                top.Jnl.PrintSync("AdminLoginFail");
                user_tip.innerText = "";
                password_tip.innerText = "*�û��������������";
            }
        }
    }

    function onDatGetPersistentDataComplete(DataName, DataType, DataValue) {
        if ('ADMININFO' == DataName) {
            bGetData = true;
            var arrDataValue = DataValue.toArray();
            top.API.AdminInfoObjStr = arrDataValue.toString();
            AdminObj = eval('(' + top.API.AdminInfoObjStr + ')');
            AdminInfo = AdminObj.AdminInfo;
        }
    }

    function onDatGetPersistentDataError(DataName, ErrorCode) {
        if ('ADMININFO' == DataName) {
            top.API.displayMessage("onDatGetPersistentDataError is done,DataName=" + DataName + ",ErrorCode=" + ErrorCode);
        }
    }

    function EventLogin() {
        top.Dat.GetPersistentDataComplete.connect(onDatGetPersistentDataComplete);
        top.Dat.GetPersistentDataError.connect(onDatGetPersistentDataError);
    }

    function EventLogout() {
        top.Dat.GetPersistentDataComplete.disconnect(onDatGetPersistentDataComplete);
        top.Dat.GetPersistentDataError.disconnect(onDatGetPersistentDataError);
    }

    document.getElementById('Exit').onclick = function () {
        top.Sys.OpenService();
        top.Sys.OpenFrontPage();
        return CallResponse('Exit');
    }
    //Countdown function
    function TimeoutCallBack() {
        return CallResponse('TimeOut');

    }

    //remove all event handler
    function Clearup() {
        //TO DO:
        EventLogout();
    }
})();