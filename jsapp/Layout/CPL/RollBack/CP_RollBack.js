/*@create by:  tsxiong
*@time: 2016��03��20��
*/
; (function () {
    var nPlanOutMoneyThisTime = 0;  //������Ҫ�����Ľ��
    var DispenseMoney;
    var nMixOrDispense = 0;   //��ʶ��ǰִ�е��������NotDispensable����Ӧ��0��Mix��1��Dispense��
    var nCompleteOrError = false;   //��ʶ��ǰҳ�������ʽ��false��ʧ�ܣ�true���ɹ���
    var AllMoney;
    var bEndflag = false;
    var bCompletedflag = false;
    var bTakenflag = false;
    var bShowNext = false;
    var ArrMixResult = new Array();
    var ArrDispense = new Array();
    var bTakenTimerClear = true;

    var timeId = null;
    var btimeup = false;
	var CallResponse = App.Cntl.ProcessOnce( function(Response){
        //TO DO:
        Clearup();
            if (top.API.gResponsecode == "04" || top.API.gResponsecode == "33" ||
                top.API.gResponsecode == "41" || top.API.gResponsecode == "43" ||
                top.API.gResponsecode == "07" || top.API.gResponsecode == "34" ||
                top.API.gResponsecode == "35" || top.API.gResponsecode == "36" ||
                top.API.gResponsecode == "37" || top.API.gResponsecode == "67") {
                Response = "Capture";
            }
        
        //if ((!nCompleteOrError) && (bTakenTimerClear)) {
            // top.ErrorInfo = top.API.PromptList.No8;
            top.ErrorInfo = top.API.PromptList.No2;
        //}
        App.Cntl.ProcessDriven(Response);
    });
    var Initialize = function () {
        App.Timer.TimeoutDisposal(TimeoutCallBack);
        ButtonDisable();
        EventLogin();
        App.Plugin.Voices.play("voi_20");
        if ("0" == top.API.CashInfo.strTransAmount) {
			nCompleteOrError = true;
			//top.ErrorInfo = top.API.PromptList.No3;
			return CallResponse('OK');
	    }
        var arrTransactionResult = new Array("�泮ʧ��");
        var nRet = top.Dat.SetDataSync("TRANSACTIONRESULT", "STRING", arrTransactionResult);
        top.API.displayMessage("SetDataSync TRANSACTIONRESULT Return:" + nRet);
        return CallResponse('OK');//���˳����˳����˳�
        InitArray();
        //@initialize scope start	
        DispenseMoney = parseInt(top.API.CashInfo.strTransAmount);
        DispenseMoney = DispenseMoney - 0;
        AllMoney = DispenseMoney;
        top.API.displayMessage("����Mix,DispenseMoney=" + DispenseMoney);
        nMixOrDispense = 0;
        //top.Cdm.Mix(DispenseMoney, "CNY", "2");
        ButtonEnable();

    }();//Page Entry
    function InitArray() {
        var i = 0;
        for ( i = 0; i < top.API.CashInfo.nCountOfUnits; i++) {
            ArrMixResult[i] = 0; 
            ArrDispense[i] = 0; 
        }
    }
    function OnlyDispense() {
        top.API.displayMessage("ֱ�ӽ���Dispense");
        var objGet = top.Dat.GetDataSync(top.API.cashinboxresultTag, top.API.cashinboxresultType);
        if (null == objGet) {
            top.API.displayMessage("GetDataSync CASHINBOXRESULT objGet = null");
        }
        else {
            top.API.displayMessage("GetDataSync CASHINBOXRESULT Return:" + objGet.toArray());
            var arrGet = objGet.toArray();
            var num;
            for (var i = 0; i < arrGet.length; i++) {
                num = arrGet[i].substr(0, 1) - 1;
                ArrDispense[num] = parseInt(arrGet[i].substr(2, arrGet[i].length - 2), 10);
            }
            for (i = 0; i < ArrDispense.length; i++) {
                ArrMixResult[i] = ArrDispense[i];
            }
            top.API.displayMessage("MixResult[0]=" + ArrMixResult[0] + ";MixResult[1]=" + ArrMixResult[1] + ";MixResult[2]=" + ArrMixResult[2] + ";MixResult[3]=" + ArrMixResult[3] + ";MixResult[4]=" + ArrMixResult[4]);
            MulDispense();
        }
    }

    function MixAndDispense() {
        top.API.displayMessage("����MixAndDispense,DispenseMoney=" + DispenseMoney);
        nMixOrDispense = 0;
        top.Cdm.Mix(DispenseMoney, "CNY", "1");
    }
    function showinfo() {
        top.API.displayMessage("�ܶ�=" + AllMoney + ";δ��:" + DispenseMoney);
        var tmpString = "Content=ȡ��������˳���" + (AllMoney - DispenseMoney).toString() + "Ԫ  ʣ�ࣺ" + DispenseMoney.toString() + "Ԫ��";
        top.Jnl.PrintSync("DepCancel");
        document.getElementById("Loading").style.display = "none";
        document.getElementById("CinfoMoney").style.display = "block";
        document.getElementById("CinfoTip").style.display = "block";
        document.getElementById("outMoney").value = AllMoney - DispenseMoney;
        document.getElementById("RemainMoney").value = DispenseMoney;
        document.getElementById("PageTitle").innerText = "��ȡ�����ĳ�Ʊ";
    }
    function ShowFailedInfo() {
        top.API.displayMessage("�ܶ�=" + AllMoney + ";δ��:" + DispenseMoney);
        document.getElementById("PageTitle").innerText = "����ʧ�ܣ���˶��ѳ���Ʊ��";
        document.getElementById("CnextTip").style.display = "none";
        document.getElementById("CinfoTip").style.display = "none";
        document.getElementById("Exit").style.display = "block";
        if (AllMoney == DispenseMoney) {        
            VoicesPlay();
            timeId = setInterval(VoicesPlay, 12000);
            btimeup = true;
            bTakenTimerClear = false;
        }
        App.Timer.SetPageTimeout(60);
        App.Timer.TimeoutDisposal(TimeoutCallBack);
    }
    function ShowSuccessInfo() {
        top.API.displayMessage("�ܶ�=" + AllMoney + ";δ��:" + DispenseMoney);
        nCompleteOrError = true;
        document.getElementById("CnextTip").innerHTML = "������ϣ�����<ȷ��>��ť";
        document.getElementById("OK").style.display = "block";
        App.Timer.SetPageTimeout(60);
        App.Timer.TimeoutDisposal(TimeoutCallBack);
    }

    function OutMoney() {
        top.API.displayMessage("showinfo,DispenseMoney=" + DispenseMoney);
        if (bCompletedflag) {
            top.API.displayMessage("����OutMoney,DispenseMoney=" + DispenseMoney);
            nMixOrDispense = 0;
            top.Cdm.Mix(DispenseMoney, "CNY", "1");
            // MulDispense();
        }
    }
    //@User ocde scope start
    function MulDispense() {
        top.API.displayMessage("MulDispense����,DispenseMoney=" + DispenseMoney);
        var nUnits = 0;
        var ArrDispense = new Array();
        var UnitsNotes = 0;
        for ( nUnits = 0; nUnits < top.API.CashInfo.nCountOfUnits; nUnits++) {
            UnitsNotes += ArrMixResult[nUnits];
            ArrDispense[nUnits] = 0;
        } 
        top.API.displayMessage("������=" + UnitsNotes);
        top.API.displayMessage("��ֵ=" + top.API.CashInfo.arrUnitCurrency);
        top.API.displayMessage("ArrMixResult=" + ArrMixResult);
        if (UnitsNotes <= 100) {
            InitFlag();
            nPlanOutMoneyThisTime = DispenseMoney;
            nMixOrDispense = 1;
            top.Cdm.Dispense(0, ArrMixResult, "CNY", "0");
            top.API.displayMessage("����ArrMixResult����");
            DispenseMoney = 0;
            bEndflag = true;
            for (nUnits = 0; nUnits < top.API.CashInfo.nCountOfUnits; nUnits++) {
                ArrMixResult[nUnits] = 0;
            }
            IsEnd();
        } else {
            var nRoundCount = 100;
            nPlanOutMoneyThisTime = DispenseMoney;
            for (nUnits = 0; nUnits < top.API.CashInfo.nCountOfUnits; nUnits++) {
                if (ArrMixResult[nUnits] != 0) {
                    if (nRoundCount > 0) {
                        if (ArrMixResult[nUnits] > nRoundCount) {
                            ArrDispense[nUnits] = nRoundCount;
                            ArrMixResult[nUnits] = ArrMixResult[nUnits] - ArrDispense[nUnits];
                            nRoundCount = nRoundCount - ArrDispense[nUnits];
                            DispenseMoney = DispenseMoney - top.API.CashInfo.arrUnitCurrency[nUnits] * ArrDispense[nUnits];
                            top.API.displayMessage("ArrMixResult[nUnits] > nRoundCount������" + (nUnits + 1) + "�䳮����=" + ArrDispense[nUnits]);
                        } else {
                            ArrDispense[nUnits] = ArrMixResult[nUnits];
                            ArrMixResult[nUnits] = ArrMixResult[nUnits] - ArrDispense[nUnits];
                            nRoundCount = nRoundCount - ArrDispense[nUnits];
                            DispenseMoney = DispenseMoney - top.API.CashInfo.arrUnitCurrency[nUnits] * ArrDispense[nUnits];
                            top.API.displayMessage("ArrMixResult[nUnits] <= nRoundCount����" + (nUnits + 1) + "�䳮����=" + ArrDispense[nUnits]);
                            continue;
                        };
                    } else { };
                    InitFlag();
                    nPlanOutMoneyThisTime = nPlanOutMoneyThisTime - DispenseMoney;
                    nMixOrDispense = 1;
                    top.Cdm.Dispense(0, ArrDispense, "CNY", "0");
                    top.API.displayMessage("����:ArrDispense=" + ArrDispense);
                    IsEnd();
                    break;
                }
            }
        }
    }

    function InitFlag() {
        bCompletedflag = false;
        bTakenflag = false;
        bShowNext = false;
    }

    function IsEnd() {
        var nUnits = 0;
        var bReturn = true;
        for (nUnits = 0; nUnits < top.API.CashInfo.nCountOfUnits; nUnits++) {
            if (ArrMixResult[nUnits] != 0) {
                bReturn = false;
            }
        }
        if (bReturn) {
            DispenseMoney = 0;
            bEndflag = true;
            top.API.displayMessage("���г���������");
        } 
        return bReturn;
    }
    //@User code scope end 

    //event handler
    function onCashDispensed(info) {
        top.API.displayMessage("onCashDispensed����");
        var arrInfo = new Array();
        arrInfo = info.toArray();
        var nUnits = 0;
        var nAmountThisTime = 0;
        for (nUnits = 0; nUnits < top.API.CashInfo.nCountOfUnits; nUnits++) {            
            nAmountThisTime = nAmountThisTime + (arrInfo[nUnits] * top.API.CashInfo.arrUnitCurrency[nUnits]);
        }
        DispenseMoney = DispenseMoney + nPlanOutMoneyThisTime - nAmountThisTime;
        if (nAmountThisTime == 0) {
            showinfo();
            ShowFailedInfo();
        }else{
            bCompletedflag = true;
            showinfo();
            if (bTakenflag) {//��������ȫ��bTakenflagΪfalse��
                OutMoney();
            }else {
                if (bShowNext) {
                    ShowSuccessInfo();
                }else{  
                    top.API.displayMessage("��ʾ�ͻ����߳�Ʊ");                  
                    VoicesPlay();
                    timeId = setInterval(VoicesPlay, 12000);
                    btimeup = true;
                    App.Timer.SetPageTimeout(60);
                    bTakenTimerClear = false;
                    App.Timer.TimeoutDisposal(TimeoutCallBack);
                }
            }
        }
    }

    function VoicesPlay() {
        var tmp1 = top.Cdm.StOutputStatus();
        var tmp2= top.Cim.StInputStatus();
        if (tmp1 != "EMPTY" || tmp2 != "EMPTY") {
            top.API.displayMessage("������InputOutputStatus="+tmp1 + "�볮��InputOutputStatus="+tmp2);
           App.Plugin.Voices.play("voi_7");
        }
    }

    function onNotDispensable() {
        if (0 === nMixOrDispense) {
            top.API.displayMessage("Mix�����onNotDispensable����,DispenseMoney=" + DispenseMoney + ";nPlanOutMoneyThisTime=" + nPlanOutMoneyThisTime);
        } else {
            top.API.displayMessage("Dispense�����onNotDispensable����,DispenseMoney=" + DispenseMoney + ";nPlanOutMoneyThisTime=" + nPlanOutMoneyThisTime);
            DispenseMoney = DispenseMoney + nPlanOutMoneyThisTime - 0;
        }
        showinfo();
        ShowFailedInfo();
    }

    function onDeviceError() {
        if (0 === nMixOrDispense) {
            top.API.displayMessage("Mix�����onDeviceError����,DispenseMoney=" + DispenseMoney + ";nPlanOutMoneyThisTime=" + nPlanOutMoneyThisTime);
        } else {
            top.API.displayMessage("Dispense�����onDeviceError����,DispenseMoney=" + DispenseMoney + ";nPlanOutMoneyThisTime=" + nPlanOutMoneyThisTime);
            DispenseMoney = DispenseMoney + nPlanOutMoneyThisTime - 0;
        }
        showinfo();
        ShowFailedInfo();
    }
    function onMixComplete(MixResult) {
        top.API.displayMessage("Mix:" + DispenseMoney + ";onMixComplete����");
        ArrMixResult = MixResult.toArray();
        top.API.displayMessage("MixResult=" + ArrMixResult);
        MulDispense();
    }
    //event handler
    function onCashUnitError(info) {
        top.API.displayMessage("onCashUnitError����");
        var arrInfo = new Array();
        arrInfo = info.toArray();
        var nUnits = 0;
        var nAmountThisTime = 0;
        for (nUnits = 0; nUnits < top.API.CashInfo.nCountOfUnits; nUnits++) {
            nAmountThisTime = nAmountThisTime + (arrInfo[nUnits] * top.API.CashInfo.arrUnitCurrency[nUnits]);
        }
        DispenseMoney = DispenseMoney + nPlanOutMoneyThisTime - nAmountThisTime;
        showinfo();
        ShowFailedInfo();
    }

    function onCashTaken() {
        top.API.displayMessage("onCashTaken����");
        if (btimeup) {
            btimeup = false;
            window.clearInterval(timeId);
        }
        if (!bTakenTimerClear) {
            App.Timer.ClearTime();
            bTakenTimerClear = true;
        }
        if (!bEndflag) {
            top.API.displayMessage("��������");
            bTakenflag = true;
            OutMoney();
        } else {
            top.API.displayMessage("�������");
            if (bCompletedflag) {
                ShowSuccessInfo();
            }
            bShowNext = true;
            //return CallResponse("OK");
        }
    }

    function ButtonDisable() {
        top.API.displayMessage("ButtonDisable");
        document.getElementById('OK').disabled = true;
        document.getElementById('Exit').disabled = true;
    }

    function ButtonEnable() {
        top.API.displayMessage("ButtonEnable");
        document.getElementById('OK').disabled = false;
        document.getElementById('Exit').disabled = false;
    }

    document.getElementById('OK').onclick = function () {
        top.API.displayMessage("���OK��ť,��Ӧ<OK>");
        ButtonDisable();
        //add by hj 
        if (top.API.gCheckInfoFlag && top.API.gAuthorRefuse!=""&&top.Ptr.bDeviceStatus && top.API.gNoPtrSerFlag==false) {
            return CallResponse('Print');
        } else {
            return CallResponse('OK');
        }
    }

    document.getElementById('Exit').onclick = function () {
        top.API.displayMessage("���Exit��ť,��Ӧ<Exit>");
        ButtonDisable();
        if (top.API.gCheckInfoFlag && top.API.gAuthorRefuse != "" && top.Ptr.bDeviceStatus && top.API.gNoPtrSerFlag==false) {
            return CallResponse('Print');
        } else {
            return CallResponse('Exit');
        }
    }

    //Register the event
    function EventLogin() {
        top.Cdm.MixComplete.connect(onMixComplete);
        top.Cdm.CashDispensed.connect(onCashDispensed);
        top.Cdm.CashTaken.connect(onCashTaken);
        top.Cdm.NotDispensable.connect(onNotDispensable);
        top.Cdm.CashUnitError.connect(onCashUnitError);
        top.Cdm.DeviceError.connect(onDeviceError);
    }

    function EventLogout() {
        top.Cdm.MixComplete.disconnect(onMixComplete);
        top.Cdm.CashDispensed.disconnect(onCashDispensed);
        top.Cdm.CashTaken.disconnect(onCashTaken);
        top.Cdm.NotDispensable.disconnect(onNotDispensable);
        top.Cdm.CashUnitError.disconnect(onCashUnitError);
        top.Cdm.DeviceError.disconnect(onDeviceError);
    }

    //Countdown function
    function TimeoutCallBack() {
        top.API.displayMessage("��ʱ�¼�����");
        ButtonDisable();
        if (!bTakenTimerClear) {
            top.API.displayMessage("�ͻ�δȡ��");
            top.ErrorInfo = top.API.PromptList.No3;
            var arrTransactionResult  = new Array("�ͻ�δȡ��");
            var nRet = top.Dat.SetDataSync("TRANSACTIONRESULT", "STRING", arrTransactionResult);
            top.API.displayMessage("SetDataSync TRANSACTIONRESULT Return:" + nRet);
             var arrBSERVICE = new Array();
            arrBSERVICE[0] = 0;
            nRet = top.Dat.SetDataSync(top.API.servicestateTag, top.API.servicestateType, arrBSERVICE);
            top.API.displayMessage("SetDataSync SERVICESTATE Return:" + nRet);
            if (top.API.gCheckInfoFlag && top.API.gAuthorRefuse != "" && top.Ptr.bDeviceStatus && top.API.gNoPtrSerFlag==false) {
                return CallResponse('Print');
            } else {
                return CallResponse('Capture');
            }
        }else{
            if (nCompleteOrError) {
                if (top.API.gCheckInfoFlag && top.API.gAuthorRefuse != "" && top.Ptr.bDeviceStatus && top.API.gNoPtrSerFlag==false) {
                    return CallResponse('Print');
                } else {
                    return CallResponse('OK');
                }
            } else {
                top.ErrorInfo = top.API.PromptList.No3;
                if (top.API.gCheckInfoFlag && top.API.gAuthorRefuse != "" && top.Ptr.bDeviceStatus && top.API.gNoPtrSerFlag==false) {
                    return CallResponse('Print');
                } else {
                    return CallResponse('TimeOut');
                }
            }
        }
    }
    //Page Return
    
    //remove all event handler
    function Clearup() {
        //TO DO:
        if (btimeup) {
            btimeup = false;
            window.clearInterval(timeId);
        }
        App.Plugin.Voices.del();
        EventLogout();
        App.Timer.ClearTime();
    }
})();
