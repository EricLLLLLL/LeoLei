/*@create by:  tsxiong
*@time: 2016��03��20��
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
		//modify by tsx ��ȡ�Ƿ����������˲�
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
        top.API.displayMessage("���<�˳�>");
        top.ErrorInfo = top.API.PromptList.No2;
        ButtonDisable();
        return CallResponse('Exit');
    }
    document.getElementById('OK').onclick = function () {
        top.API.displayMessage("���<ȷ��>");
        top.ErrorInfo = top.API.PromptList.No1;
        ButtonDisable();
        if (top.API.gReadIdCardFlag == 2) {
            return CallResponse('Agent');
        } else {
            return CallResponse('OK');
        }
		
		/*
        //add by hj����ȡ�����֤���ȷ�Ϻ󣬰�ȷ�ϼ���ʾȡ�����֤��ȡ�����֤��ȷ�ϼ�����ȡ������
        if (top.API.CashInfo.Dealtype == "����ȡ��" && bTipFlag) {
            ButtonDisable();
            if (top.API.gbAmountType == 2) {
                //����ȡ������ת
                return CallResponse('OK');
            } else {
                return CallResponse('CWC');
            }
        }
        else if (top.API.CashInfo.Dealtype == "����ȡ��" && bTipFlag == false) {
            //showTakeIDCard();
        //} else {
            ButtonDisable();
            return CallResponse('OK');
        }*/ //modify by tsx;

    }

    //add by hj ��ʾȡ�����֤
    function showTakeIDCard() {
        document.getElementById('PageTitle').innerHTML = "��ȡ�����֤";
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
        //�����Ƚ�
        var crstr = "";//���������˲���
        var crFlag = "";//�����־ 00  01 ...
        if ((top.ErrorInfo).indexOf('|') > 0) {
            var arrayResult = (top.ErrorInfo).split("|");
            crstr = arrayResult[1];
            crFlag = arrayResult[0];
        } else {
            crstr = top.ErrorInfo;
            crFlag = "no";
        }
		
        if ((top.API.gIdName) != tmpName) {
            top.API.gCheckResult = crFlag + "|" + crstr + "|�뻧����һ��";
        } else {
            top.API.gCheckResult = crFlag + "|" + crstr + "|�뻧��һ��";
        }
        //var strName = '*';
        //strName += tmpName.substr(1, (tmpName.length - 1));
        //document.getElementById('tName').innerHTML = strName;

        //��ʾ���֤�����˲��� color
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
        if (top.API.CashInfo.Dealtype == "����ȡ��" && top.API.gbAmountType == true) {
            top.ErrorInfo = "������ʧ�ܣ������˳���";
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
