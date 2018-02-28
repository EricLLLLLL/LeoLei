/*@create by:  tsxiong
*@time: 2016��03��20��
*/
; (function () {
    var CashAmount = 0;
    var CashTotal = 0;
    var typeSelect = 0;
	var CallResponse = App.Cntl.ProcessOnce( function(Response){
        //TO DO:
        Clearup();
        //Entry the flows control process.
        App.Cntl.ProcessDriven(Response);
    });
    var Initialize = function () {
        App.Timer.TimeoutDisposal(TimeoutCallBack);
        ButtonDisable();
        EventLogin();
        top.Cim.StoreEscrowedCash(top.API.gStoreEscrowedCashTimeOut);
        ButtonEnable();
    }();//Page Entry

    //@User ocde scope start
    function ButtonDisable() {
        top.API.displayMessage("ButtonDisable");
        document.getElementById('Exit').disabled = true;
    }
    function ButtonEnable() {
        top.API.displayMessage("ButtonEnable");
        document.getElementById('Exit').disabled = false;
    }
    //��ʾ�볮����
    function showInfo() {
        top.API.displayMessage("��ʾ�볮����");
        document.getElementById("CinfoDiv").style.display = "block";
        document.getElementById("CinfoTip").style.display = "block";
        document.getElementById("CinfoTakeTip").style.display = "none";

        var objGet = top.Dat.GetDataSync(top.API.cashintotalTag, top.API.cashintotalType);
        top.API.displayMessage("GetDataSync CASHINTOTAL Return:" + objGet.toArray());
        if (null == objGet) {
            top.API.displayMessage("GetDataSync CASHINTOTAL objGet = null");
        }
        else {
            arrGet = objGet.toArray();
            top.API.displayMessage("�볮��Ϣ�� " + objGet.toArray());
        }
        var  Denomination100 = "";
        var  Denomination50 = "";
        var  Denomination20 = "";
        var  Denomination10 = "";
        var i = 0;
        var strDenomination = new Array();
        for (i = 0; i < arrGet.length; i++) {
            strDenomination = arrGet[i].split(":");
            if (strDenomination[0] == "0") {
                CashAmount=0;
                break;
            }
            if (strDenomination[0] == "100") {
                Denomination100 = strDenomination[1];
            }
            if (strDenomination[0] == "50") {
                Denomination50 = strDenomination[1];
            }
            if (strDenomination[0] == "20") {
                Denomination20 = strDenomination[1];
            }
            if (strDenomination[0] == "10") {
                Denomination10 = strDenomination[1];
            }
        }
        document.getElementById("CinfoDiv").style.display = "block";
        document.getElementById("CinfoTip").style.display = "block";
        document.getElementById("CinfoTakeTip").style.display = "none";
        if (CashAmount == 0) {
            document.getElementById('tCash').value = "CNY0";
            document.getElementById('T100').value = "0";
            document.getElementById('T50').value = "0";
            document.getElementById('T20').value = "0";
            document.getElementById('T10').value = "0";
        } else {
            var showmoney = "CNY" + CashAmount;
            document.getElementById('tCash').value = showmoney;
            document.getElementById('T100').value = Denomination100;
            document.getElementById('T50').value = Denomination50;
            document.getElementById('T20').value = Denomination20;
            document.getElementById('T10').value = Denomination10;
        }
    }

    document.getElementById('Exit').onclick = function () {
        ButtonDisable();
        top.API.displayMessage("���Exit��ť,��Ӧ<Exit>");
        return CallResponse('Exit');
    }


    //@User code scope end 

    //event handler
    function onEscrowedCashStored(arrUnitResult, bRecycle) {
        top.API.displayMessage("����onEscrowedCashStored");
        var Recycle = 0;
        var UnitResult = arrUnitResult.toArray();
        var num;
        var ArrMixResult = new Array(0, 0, 0, 0, 0);
        for (var i = 0; i < UnitResult.length; i++) {
            num = UnitResult[i].substr(0, 1) - 1;
            ArrMixResult[num] = parseInt(UnitResult[i].substr(2, UnitResult[i].length - 2), 10);
        }
        top.API.displayMessage("arrUnitResult[0]=" + ArrMixResult[0] + ";arrUnitResult[1]=" + ArrMixResult[1] + ";arrUnitResult[2]=" + ArrMixResult[2] + ";arrUnitResult[3]=" + ArrMixResult[3] + ";arrUnitResult[4]=" + ArrMixResult[4]);
        for (var i = 0; i < 5; i++) {
            CashAmount = CashAmount + (ArrMixResult[i] * top.API.CashInfo.arrUnitCurrency[i]);
            CashTotal = CashTotal + ArrMixResult[i];
        }
        showInfo();
    }

    function onCashTaken() {
        top.API.displayMessage("����onCashTaken");
    }

    function onTimeout() {
        top.API.displayMessage("����onTimeout");
        return CallResponse('Exit');
    }

    function onDeviceError() {
        top.API.displayMessage("����onDeviceError");
        return CallResponse("Exit");
    }

    function onEscrowedCashStoreFailed() {
        top.API.displayMessage("onEscrowedCashStoreFailed");
    }


    //Register the event
    function EventLogin() {
        top.Cim.EscrowedCashStored.connect(onEscrowedCashStored);
        top.Cim.EscrowedCashStoreFailed.connect(onEscrowedCashStoreFailed);
        top.Cim.CashTaken.connect(onCashTaken);
        top.Cim.Timeout.connect(onTimeout);
        top.Cim.DeviceError.connect(onDeviceError);
    }

    function EventLogout() {
        top.Cim.EscrowedCashStored.disconnect(onEscrowedCashStored);
        top.Cim.EscrowedCashStoreFailed.disconnect(onEscrowedCashStoreFailed);
        top.Cim.CashTaken.disconnect(onCashTaken);
        top.Cim.Timeout.disconnect(onTimeout);
        top.Cim.DeviceError.disconnect(onDeviceError);
    }

    //Countdown function
    function TimeoutCallBack() {
        top.API.displayMessage("��ʱ��Ӧ");
        top.ErrorInfo = top.API.PromptList.No3;
        return CallResponse("Exit");
    }
    //Page Return
    
    //remove all event handler
    function Clearup() {
        //TO DO:
        EventLogout();
        App.Timer.ClearTime();
    }
})();
