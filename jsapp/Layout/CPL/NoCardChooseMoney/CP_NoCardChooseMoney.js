/*@create by:  tsxiong
 *@time: 2016��03��20��
 */
;(function () {
    var LargeOrNot = 0,
        lRetAuthorized,
        lRetBigTranLimit,
        unitMaxmoney,
        SCMSet = [],//supportMoneySet
        noPassbookCWDMoney = top.API.noPassbookCWDMoney, // �����ۼƽ��
        noPassbookLimit = 200000 - parseFloat(noPassbookCWDMoney), // ���ո������۴��ɴ���(�ۼƽ��ܳ���20w)
        bSetSupportCountMoney,
        CallResponse = App.Cntl.ProcessOnce(function (Response) {
            //TO DO:
            Clearup();
            App.Cntl.ProcessDriven(Response);
        });
    var Initialize = function () {
        EventLogin();
        lRetAuthorized = top.Sys.DataGetSync(top.API.MTRN_AUTHORIZEDAMOUNTRULE); // ������Ȩ�޶�
        lRetBigTranLimit = top.Sys.DataGetSync(top.API.MTRN_TRANLIMITAMOUNTREAL); // �����޶�
        ButtonDisable();
        ShowInfo();
        App.Timer.TimeoutDisposal(TimeoutCallBack);
        ButtonEnable();
        bSetSupportCountMoney = top.Dat.GetPrivateProfileSync("TransactionConfig", "bSetSupportCountMoney", "0", top.API.gIniFileName);
        if (bSetSupportCountMoney == '1') {
            SupportCountMoney();
        }else{
            top.Cim.SetSupportValue(['100','50','20','10','5','1']);
            top.API.gSupportValue = ['100','50','20','10','5','1'];
        }

        top.API.displayMessage("noPassbookCWDMoney--"+noPassbookCWDMoney+ ";noPassbookLimit--"+noPassbookLimit+";");
        top.API.displayMessage("lRetAuthorized--"+lRetAuthorized+ ";lRetBigTranLimit--"+lRetBigTranLimit+";");
        App.Plugin.Voices.play("voi_10");
    }();//Page Entry

    function GetUnitInfo(arrParam) {
        var arrCurrentCount = new Array();
        var arrSplite = new Array();
        var arrReturnInfo = new Array();
        arrCurrentCount = arrParam;
        for (i = 0; i < arrCurrentCount.length; i++) {
            arrSplite = arrCurrentCount[i].split(":");
            arrReturnInfo[i] = arrSplite[1];
        }
        return arrReturnInfo;
    }

    //���ݳ����Ƿ�����������ж��Ƿ񿪷������ֵ��
    function SupportCountMoney() {
        var arrUnitRemain = GetUnitInfo(top.API.gArrUnitRemain);
        var arrUnitStatus = GetUnitInfo(top.API.gArrUnitStatus);
        top.API.displayMessage("arrUnitRemain-------" + arrUnitRemain);
        top.API.displayMessage("arrUnitStatus-------" + arrUnitStatus);
        var arr = new Array();
        var amountOfMoney = ['100', '50', '20', '10', '5', '1'];
        for (var i = 0; i < arrUnitRemain.length; i++) {
            for (var j = 0; j < amountOfMoney.length; j++) {
                if (arrUnitRemain[i] == amountOfMoney[j]) {
                    if (arrUnitStatus[i] != 'FULL' && arrUnitStatus[i] != 'MISSING') {
                        arr.push(amountOfMoney[j]);
                    }
                }
            }
        }

        //ȥ��
        for (var i = 0; i < arr.length; i++) {
            if (SCMSet.indexOf(arr[i]) == -1) {
                SCMSet.push(arr[i]);
            }
        }

        if (top.API.gSupportValue.sort().toString() != SCMSet.sort().toString()) {
            top.API.gSupportValue = SCMSet;
            top.API.displayMessage("SCMSet-------" + SCMSet);
            top.Cim.SetSupportValue(SCMSet);
        } else {
            SCMSet.sort(function (a, b) {
                return b - a;
            });

            //��ӱ�����ǰ��֧��������ʾ
            var tempSupportValue = SCMSet;
            var innerText = '������ǰ֧�ֵ�����У�';
            if (tempSupportValue.length != 0) {
                for (var value in tempSupportValue) {
                    innerText += (tempSupportValue[value] + 'Ԫ ');
                }

            } else {
                innerText = '';
            }
            $('#SupportValue').text(innerText);
        }
    }

    //@User ocde scope start
    function ShowInfo() {
        var TransStatus = top.Sys.PossibleTransactionSync();
        var arrTransStatus = TransStatus.split(",");
        var BIG_CARD_flag = parseInt(arrTransStatus[10]);
        if (BIG_CARD_flag != 1) {
            document.getElementById('MoneyBtn2').style.display = "none";
        }
        if (lRetBigTranLimit <= 0) {
            document.getElementById('MoneyBtn2').style.display = "none";
            document.getElementById('MoneyBtn5').style.display = "none";
        }

        // add by Gni �ж��Ƿ񻹿��Խ��д����
        if( top.API.CWDType == "passbook" ){
            if( noPassbookLimit < parseInt(lRetAuthorized) ){
                $("#MoneyBtn2").hide();
                $("#CinfoTip").hide();
            }
            document.getElementById('LimitTodayMoney').innerHTML = noPassbookLimit.toString();
        }else{
            document.getElementById('LimitTodayMoneyTip').style.display = "none";
        }

        var unitMaxmoney = top.Sys.DataGetSync(top.API.MTRN_REMAINDEPSITAMOUT); // ����ʣ��ɴ���
        top.API.displayMessage("ʣ�����ܽ��,Maxmoney=" + unitMaxmoney);

        var iniRet = top.Dat.GetPrivateProfileSync("BankConfig", "Edition", "2", top.API.gIniFileName);
        

        var nAuthorized = parseInt(lRetAuthorized) / 10000;
        $('#MLimit2').text(nAuthorized);
        $('#MLimit3').text(nAuthorized);
        $('#MLimit4').text(nAuthorized);
        $('#MLimit5').text(nAuthorized);
        $('#MLimit6').text(nAuthorized);
        $('#MLimit7').text(nAuthorized);
        $('#MLimit8').text(nAuthorized);

        if (top.API.CashInfo.Dealtype == "���۴��" || top.API.CashInfo.Dealtype == "DEP���") {
            document.getElementById('Back').style.display = "block";
        } else {
            document.getElementById('Back').style.display = "none";
        }
        var strName = '';
        strName += '*';
        strName += top.API.gCustomerName.substr(1, (top.API.gCustomerName.length - 1));
        if (top.API.CashInfo.Dealtype == "�Թ����") {
            document.getElementById('tName').value = top.API.gCustomerName;
        } else {
            document.getElementById('tName').value = strName;
        }
        document.getElementById('tCardNo').value = top.API.gCardno;
    }

    function ButtonDisable() {
        document.getElementById('Exit').disabled = true;
        document.getElementById('MoneyBtn2').disabled = true;//���
        document.getElementById('MoneyBtn5').disabled = true;//С��
        document.getElementById('Back').disabled = true;
    }

    function ButtonEnable() {
        document.getElementById('Exit').disabled = false;
        document.getElementById('MoneyBtn2').disabled = false;
        document.getElementById('MoneyBtn5').disabled = false;
        document.getElementById('Back').disabled = false;
    }

    document.getElementById('Back').onclick = function () {
        ButtonDisable();
        return CallResponse('Back');
    }

    document.getElementById('Exit').onclick = function () {
        ButtonDisable();
        top.ErrorInfo = top.API.PromptList.No2;
        return CallResponse('Exit');
    }

    document.getElementById('MoneyBtn2').onclick = function () {
        top.API.displayMessage("��������");
        top.API.gbAmountType = false;
        var tmpTslType = "";
        if (top.API.CashInfo.Dealtype == "DEP���") {
            tmpTslType = "BDEP";
        } else if (top.API.CashInfo.Dealtype == "���۴��") {
            tmpTslType = "BBOOKDEP";
        } else if (top.API.CashInfo.Dealtype == "�޿����۴��" || top.API.CashInfo.Dealtype == "���۴��") {
            tmpTslType = "BNOCARDDEP";
        }
        TslFunction(tmpTslType);
        top.API.gTslChooseJnlType = "0107";
        var bSetAvailableAmount =  top.Dat.GetPrivateProfileSync("TransactionConfig", "bSetAvailableAmount", "0", top.API.gIniFileName);
        if(bSetAvailableAmount == '1') {
            if (lRetBigTranLimit > unitMaxmoney) {
                lRetBigTranLimit = unitMaxmoney;
            }
        }

        // add by Gni �������۴��ɴ������
        if( top.API.CWDType == "passbook" ){
            if ( lRetBigTranLimit > noPassbookLimit ) {
                lRetBigTranLimit = noPassbookLimit;
            }
        }
        top.API.displayMessage("�ɴ������lRetBigTranLimit--"+lRetBigTranLimit);

        var arrTransactionResult;
        arrTransactionResult = new Array(lRetBigTranLimit.toString());
        top.API.gChooseMoney = lRetBigTranLimit;
        top.Dat.SetDataSync("CASHINMAXAMOUNT", "STRING", arrTransactionResult);
        ButtonDisable();
        return CallResponse('LargeDeposit');
    }

    document.getElementById('MoneyBtn5').onclick = function () {
        top.API.displayMessage("�����ͨ���");
        top.API.gbAmountType = true;
        var tmpTslType = "";
        if (top.API.CashInfo.Dealtype == "DEP���") {
            tmpTslType = "SDEP";
        } else if (top.API.CashInfo.Dealtype == "���۴��") {
            tmpTslType = "SBOOKDEP";
        } else if (top.API.CashInfo.Dealtype == "�޿����۴��" || top.API.CashInfo.Dealtype == "���۴��") {
            tmpTslType = "SNOCARDDEP";
        }
        TslFunction(tmpTslType);
        top.API.gTslChooseJnlType = "0107";
        var SetLimit = (lRetBigTranLimit <= lRetAuthorized - 100) ? lRetBigTranLimit : lRetAuthorized - 100;
        var bSetAvailableAmount =  top.Dat.GetPrivateProfileSync("TransactionConfig", "bSetAvailableAmount", "0", top.API.gIniFileName);
        if(bSetAvailableAmount == '0') {
                SetLimit = lRetAuthorized - 100;
        }

        // add by Gni �������۴��ɴ������
        if( top.API.CWDType == "passbook" ){
            if ( SetLimit > noPassbookLimit ) {
                SetLimit = noPassbookLimit;
            }
        }
        top.API.displayMessage("�ɴ������SetLimit--"+SetLimit);

        top.API.displayMessage("SetLimit=" + SetLimit);
        var arrTransactionResult;
        arrTransactionResult = new Array(SetLimit.toString());
        top.API.gChooseMoney = SetLimit;
        top.Dat.SetDataSync("CASHINMAXAMOUNT", "STRING", arrTransactionResult);
        ButtonDisable();
        return CallResponse('OK');
    }

    function onSetSupportValueCompleted() {
        SCMSet.sort(function (a, b) {
            return b - a;
        });

        //��ӱ�����ǰ��֧��������ʾ
        var tempSupportValue = SCMSet;
        var innerText = '������ǰ֧�ֵ�����У�';
        if (tempSupportValue.length != 0) {
            for (var value in tempSupportValue) {
                innerText += (tempSupportValue[value] + 'Ԫ ');
            }

        } else {
            innerText = '';
        }
        $('#SupportValue').text(innerText);
        top.API.displayMessage("onSetSupportValueCompleted");
    }

    function onSetSupportValueFailed() {
        top.API.gSupportValue = [];
        top.API.displayMessage("onSetSupportValueFailed");
    }


    function EventLogin() {
        top.Cim.SetSupportValueCompleted.connect(onSetSupportValueCompleted);
        top.Cim.SetSupportValueFailed.connect(onSetSupportValueFailed);
    }

    function EventLogout() {
        top.Cim.SetSupportValueCompleted.disconnect(onSetSupportValueCompleted);
        top.Cim.SetSupportValueFailed.disconnect(onSetSupportValueFailed);
    }


    //@User code scope end 
    //��������ʱ��ͽ�������
    function TslFunction(type) {
        var myData = top.GetDate12byte();
        top.API.gTslDate = myData.substr(0, 8);
        top.API.gTslTime = myData.substr(8, 6);
        top.API.gTslChooseType = type;
    }

    //Countdown function
    function TimeoutCallBack() {
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
