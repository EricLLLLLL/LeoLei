// JavaScript Document
/*@create by:  tsxiong
*@time: 2016年03月20日
*/
;(function () {
var User = document.getElementById("UserInput");
var Password = document.getElementById("PswInput");
var ComfirmInput = document.getElementById("ComfirmInput");
User.focus();
var Element;
var span_tip = document.getElementById("span_tip");
var SafePsw = "";
var bGetPswCompleted = false;

	var CallResponse = App.Cntl.ProcessOnce( function(Response){
        //TO DO:
        Clearup();
        //Entry the flows control process.
        App.Cntl.ProcessDriven(Response);
    });
    var Initialize = function () {
        EventLogin();
        top.Dat.GetPersistentData("SAFEPASSWORD", "STRING");
        App.Plugin.Keyboard.show("2", "PageSubject", "KeyboardDiv");
        Element = User;
    } (); //Page Entry

    //@User ocde scope start  
    document.getElementById('Exit').onclick = function () {
        document.getElementById('Exit').disabled = true;
        return CallResponse('Exit');
    }
    //输入框点击事件
    User.onclick = function () {
        Element = User;
        App.InputEdit.getCurPosition(Element);
    }

    Password.onclick = function () {
        Element = Password;
        App.InputEdit.getCurPosition(Element);
    }

    ComfirmInput.onclick = function () {
        Element = ComfirmInput;
        App.InputEdit.getCurPosition(Element);
    }
    var oKeyboardKey = document.getElementsByName("Name_Keyboard");
    for (var i = 0; i < oKeyboardKey.length; i++) {
        var keyEvent = oKeyboardKey[i];
        keyEvent.onclick = function (e) {
            ClearTip();
            if ('退格' == this.innerText) {
                App.InputEdit.getInput(Element, 1, "BS");
            } else if ('清除' == this.innerText) {
                App.InputEdit.getInput(Element, 1, "CLEAR");
            } else {
                if (Element.value.length < 6) {
                    App.InputEdit.getInput(Element, 0, this.innerText);
                }
            }
        }
    }

    function ClearTip() {
        span_tip.innerText = "";
    }

    document.getElementById('KeyboardKey_set').onclick = function () {
        if (bGetPswCompleted) {
            if ((User.value == "") || (Password.value == "") || (ComfirmInput.value == "")) {
                span_tip.innerText = "输入信息不能为空！";
            } else {
                if (User.value != SafePsw) {
                        span_tip.innerText = "原始密码输入错误！";
                } else if (PswInput.value != ComfirmInput.value){
                        span_tip.innerText = "两次密码不一致！";
                }else{
                    var arrTotalFlag = new Array(PswInput.value);
                    nRet1 = top.Dat.SetPersistentData("SAFEPASSWORD", "STRING", arrTotalFlag);
                    top.API.displayMessage('交易流水号：SetPersistentData SAFEPASSWORD Return:' + nRet1);
                }
            }
        }
    }

        function onDatGetPersistentDataComplete(DataName, DataType, DataValue) {
        if ('SAFEPASSWORD' == DataName) {
			var arrDataValue = DataValue.toArray();
			top.API.displayMessage("onDatGetPersistentDataComplete is done,DataName=" + DataName + ",DataType=" + DataType + ",DataValue=" + arrDataValue);
			SafePsw = DataValue.toArray()[0];
            bGetPswCompleted = true;
		}		
    }

    function onDatGetPersistentDataError(DataName, ErrorCode) {
        top.API.displayMessage("onDatSetPersistentDataError is done,DataName=" + DataName + ",ErrorCode=" + ErrorCode);
        var arrParam = new Array("000000");
        top.Dat.AddPersistentData("SAFEPASSWORD", "STRING", arrParam);
    }

    function onAddPersistentDataComplete(DataName) {
        top.API.displayMessage("onAddPersistentDataComplete is done,DataName=" + DataName);
        if ('SAFEPASSWORD' == DataName) {
			SafePsw = '000000';
            bGetPswCompleted = true;
		}	
    }

    function onAddPersistentDataError(DataName, ErrorCode) {
        top.API.displayMessage("onAddPersistentDataError is done,DataName=" + DataName + ",ErrorCode=" + ErrorCode);
    }

    function onDatSetPersistentDataComplete(DataName) {
		top.API.displayMessage("onDatSetPersistentDataComplete is done,DataName=" + DataName);
        top.ErrorInfo = "修改信息管理员密码成功！";
		return CallResponse("OK");
	}

	function onDatSetPersistentDataError(DataName, ErrorCode) {
		top.API.displayMessage("onDatSetPersistentDataError is done,DataName=" + DataName + ",ErrorCode=" + ErrorCode);
	}
    function EventLogin() {
        top.Dat.GetPersistentDataComplete.connect(onDatGetPersistentDataComplete);	
		top.Dat.GetPersistentDataError.connect(onDatGetPersistentDataError);
		top.Dat.AddPersistentDataComplete.connect(onAddPersistentDataComplete);
		top.Dat.AddPersistentDataError.connect(onAddPersistentDataError);
        top.Dat.SetPersistentDataComplete.connect(onDatSetPersistentDataComplete);
		top.Dat.SetPersistentDataError.connect(onDatSetPersistentDataError);
    }

    function EventLogout() {
        top.Dat.GetPersistentDataComplete.disconnect(onDatGetPersistentDataComplete);	
		top.Dat.GetPersistentDataError.disconnect(onDatGetPersistentDataError);
        top.Dat.AddPersistentDataComplete.disconnect(onAddPersistentDataComplete);
		top.Dat.AddPersistentDataError.disconnect(onAddPersistentDataError);
        top.Dat.SetPersistentDataComplete.disconnect(onDatSetPersistentDataComplete);
		top.Dat.SetPersistentDataError.disconnect(onDatSetPersistentDataError);
    }


    //remove all event handler
    function Clearup() {
        EventLogout();
    }
})();