/*@create by:  tsxiong
*@time: 2016年03月20日
*/
; (function () {
    var nReCompare = 0;
    var bIdentify = false;
    var bTipFlag = false;
	var CallResponse = App.Cntl.ProcessOnce( function(Response){
        //TO DO:
        Clearup();
        //Entry the flows control process.
        App.Cntl.ProcessDriven(Response);
    });
    var Initialize = function () {
		//modify by tsx 获取是否跳过联网核查
		var iniRet = top.Dat.GetPrivateProfileSync("TestConfig", "NetCheckIsContinue", "0", top.API.gIniFileName);  
		if(iniRet == 1){
			return CallResponse('OK');	
		}else{
        ButtonDisable();
        EventLogin();
			//@initialize scope start
        ShowInfo();
        ButtonEnable();
		}
        //
    }(); //Page Entry

    //@User ocde scope start
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

    document.getElementById('Exit').onclick = function () {
        top.API.displayMessage("点击<退出>");
        top.ErrorInfo = top.API.PromptList.No2;
        ButtonDisable();
        return CallResponse('Exit');
    }
    document.getElementById('OK').onclick = function () {
        top.API.displayMessage("点击<确认>");
        top.ErrorInfo = top.API.PromptList.No1;
        ButtonDisable();
        if (top.API.gReadIdCardFlag == 2) {
            return CallResponse('Agent');
        } else {
            return CallResponse('OK');
        }
		
		/*
        //add by hj存折取款身份证审核确认后，按确认键提示取走身份证，取走身份证后确认键进入取款流程
        if (top.API.CashInfo.Dealtype == "存折取款" && bTipFlag) {
            ButtonDisable();
            if (top.API.gbAmountType == 2) {
                //存折取款大额跳转
                return CallResponse('OK');
            } else {
                return CallResponse('CWC');
            }
        }
        else if (top.API.CashInfo.Dealtype == "存折取款" && bTipFlag == false) {
            //showTakeIDCard();
        //} else {
            ButtonDisable();
            return CallResponse('OK');
        }*/ //modify by tsx;

    }

    //add by hj 提示取走身份证
    function showTakeIDCard() {
        document.getElementById('PageTitle').innerHTML = "请取走身份证";
        document.getElementById('Exit').style.display = "none";
        document.getElementById('DivContent').style.display = "none";
        document.getElementById('DivTakeIDcard').style.display = "block";
        document.getElementById('OK').style.display = "block";
        bTipFlag = true;
        App.Plugin.Voices.play("voi_35");
    }
    //@User code scope end 

    function ShowInfo() {
        document.getElementById('CheckIdCardpic').setAttribute("src", top.API.gCheckIdCardpic);
        document.getElementById('IdCardpic').setAttribute("src", top.API.gIdCardpic);
        document.getElementById('Name').innerText = top.API.gIdName;
        document.getElementById('Nation').innerText = top.API.gIdNation;
        document.getElementById('Sex').innerText = top.API.gIdSex;
        document.getElementById('Number').innerText = top.API.gIdNumber;
        document.getElementById('CardNo').innerText = top.API.gCardno;

        //var strName = '';
        var tmpName = top.API.gCustomerName;
        //户名比较
        var crstr = "";//保存联网核查结果
        var crFlag = "";//结果标志 00  01 ...
        if ((top.ErrorInfo).indexOf('|') > 0) {
            var arrayResult = (top.ErrorInfo).split("|");
            crstr = arrayResult[1];
            crFlag = arrayResult[0];
        } else {
            crstr = top.ErrorInfo;
            crFlag = "no";
        }
		
        if ((top.API.gIdName) != tmpName) {
            top.API.gCheckResult = crFlag + "|" + crstr + "|与户名不一致";
        } else {
            top.API.gCheckResult = crFlag + "|" + crstr + "|与户名一致";
        }
        //var strName = '*';
        //strName += tmpName.substr(1, (tmpName.length - 1));
        //document.getElementById('tName').innerHTML = strName;

        //显示身份证联网核查结果 color
        document.getElementById('Check-tip').innerText = crstr;
        if (crFlag == "00") {
            document.getElementById('Check-tip').style.color = "green";
        } else {
            document.getElementById('Check-tip').style.color = "red";
        }
        App.Timer.TimeoutDisposal(TimeoutCallBack);
        //add by hj end
    }
    //event handler


    //Register the event
    function EventLogin() {

    }

    function EventLogout() {

    }

    //Countdown function
    function TimeoutCallBack() {
        if (top.API.CashInfo.Dealtype == "存折取款" && top.API.gbAmountType == true) {
            top.ErrorInfo = "身份审核失败，交易退出！";
        } else {
            top.ErrorInfo = top.API.PromptList.No3;
        }
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
