/*@create by:  tsxiong
 *@time: 2016��03��20��
 */
;(function () {
    var bTimeOut = false;
    var cOutMoneyThisTime = 0;//���γ������
    var nOutMoneyAllTime = 0; //�ѳ����
    var nTakenMoneyAllTime = 0; //�����߽��
    var DispenseMoney; //δ�����
    var AllMoney = 0; //��Ҫ�������ܽ��
    var arrRJCount = new Array(); //��ʼRJ����
    var nCompleteOrError = false;   //��ʶ��ǰҳ�������ʽ��false��ʧ�ܣ�true���ɹ���

    var ArrMixResult = new Array(); //�䳮�������
    var bFirstMix = true;

    var nCashUnitErrRetry = 0;
    var strPRESENTRESULT = "ʧ��";

    var tmpCurrentInfo = new Array();
    var CurrentInfo = new Array();//ʵ�ʳ�������
    var rCurrentInfo = new Array();//����������
    var responseFlag = null;//��ת��־
    var partCwcFlag = false;//�Ƿ���벿������
    var partCwcJNL = 0;
    var shutterOpenFlag = false; // add by Gni �Ƿ񿪳���
    var saveBackFlag = false; // add by Gni �Ƿ�ش棨��Կ���ʱ��ȡһ���ֿ����жϣ�
    var CallResponse = App.Cntl.ProcessOnce(function (Response) {
        SetJnl();
        Clearup();
        //Entry the flows control process.
        App.Cntl.ProcessDriven(Response);
    });
    var Initialize = function () {
        ButtonDisable();
        App.Timer.TimeoutDisposal(TimeoutCallBack);
        EventLogin();
        InitArray();

        arrRJCount = top.API.GetUnitInfo(top.Cdm.PURejectCount().toArray());

        AllMoney = parseInt(top.API.CashInfo.strTransAmount);
        top.API.displayMessage("��Ҫ�������ܽ��:" + AllMoney);
        DispenseMoney = AllMoney;
        top.Cdm.SetDispenseAmount(DispenseMoney);
        var arrTransactionResult = new Array("����ʧ��");
        top.API.gTakeCardAndPrint = true;
        top.Dat.SetDataSync("TRANSACTIONRESULT", "STRING", arrTransactionResult);
        top.API.displayMessage("Mix���: " + DispenseMoney);
        top.Cdm.Mix(DispenseMoney, "CNY", "1");
        ButtonEnable();
        App.Plugin.Voices.play("voi_14");


    }();//Page Entry
    function ButtonDisable() {
        document.getElementById('OK').disabled = true;
        document.getElementById('Exit').disabled = true;
    }

    function ButtonEnable() {
        document.getElementById('OK').disabled = false;
        document.getElementById('Exit').disabled = false;
    }

    function InitArray() {
        var i = 0;
        for (i = 0; i < top.API.CashInfo.nCountOfUnits; i++) {
            arrRJCount[i] = 0;
            tmpCurrentInfo[i] = 0;
            CurrentInfo[i] = 0;
            rCurrentInfo[i] = 0;
        }
    }

    function ShowLoading() {
        document.getElementById("CinfoMoney").style.display = "none";
        document.getElementById("CinfoTip").style.display = "none";
        $('#IsCwdPart').css('display', 'none');
        document.getElementById("TipDiv").innerText = "���ڳ��������Ժ�...";
        document.getElementById("AD").style.display = "block";
    }

    function showinfo() {
        $('#IsCwdPart').css('display', 'none');
        top.API.displayMessage("�ܶ�=" + AllMoney + ";�ѳ�:" + nOutMoneyAllTime);
        document.getElementById("AD").style.display = "none";
        document.getElementById("CinfoMoney").style.display = "block";
        document.getElementById("CinfoTip").style.display = "block";
        document.getElementById("outMoney").value = nOutMoneyAllTime;
        document.getElementById("RemainMoney").value = AllMoney - nOutMoneyAllTime;
        document.getElementById("TipDiv").innerText = "��ȡ�����ĳ�Ʊ";
        document.getElementById("CurOutMoney").innerText = cOutMoneyThisTime;
    }

    function ShowFailedInfo() {
        //top.API.displayMessage("�Ƿ�ش�saveBackFlag:" + saveBackFlag);

        saveBackFlag = shutterOpenFlag ? true : false; // ���ÿ���������ֿ������ش� by Gni

        $('#AD').css('display', 'none');
        $('#CinfoTip').css('display', 'none');
        $('#TipDiv').text('����ʧ�ܣ���˶��ѳ���Ʊ��');
        $('#CinfoMoney').css('display', 'block');
        $('#outMoney').val(nTakenMoneyAllTime);
        $('#RemainMoney').val(AllMoney - nTakenMoneyAllTime);
        $("#CurOutMoney").html(cOutMoneyThisTime);
        $('#Exit').css('display', 'block');

        //ȡ���������
        var CWCSupport = top.Dat.GetPrivateProfileSync("TransactionConfig", "CWCSupport", "0", top.API.gIniFileName);

        //ȡ����ش�����
        // var SetPartCwcFlag = top.Dat.GetPrivateProfileSync("TransactionConfig", "SetPartCwcFlag", "0", top.API.gIniFileName);

        //ע�⣡ ���ȡ���������δ���������򿨳��ش湦��Ҳ������
        if( top.API.SaveBack ){  // �Ƿ������п��������ش�
            if (CWCSupport == '1' && top.API.gbSAVEBACK_DEAL) {
                // if (cOutMoneyThisTime != 0 || saveBackFlag) {
                if (saveBackFlag) {
                    $('#TipDiv').css('display', 'none');
                    $('#CinfoMoney').css('display', 'none');
                    $("#IsCwdPart").css('display', 'block');
                    var nNotTakenMoneyAllTime = AllMoney - nTakenMoneyAllTime;
                    $('#cOutMoneyThisTime').text(cOutMoneyThisTime);
                    $('#nTakenMoneyAllTime').text(nTakenMoneyAllTime);
                    $('#nNotTakenMoneyAllTime').text(nNotTakenMoneyAllTime);
                    $('#OK').css('display', 'block');
                    $('#OK').text('ͬ��');
                    $('#Exit').text('��ͬ��');
                    SetPartCwc();
                }
            }
        }
        
        top.API.displayMessage("before---UpdateRecord");
        nCompleteOrError = false;
        top.Tsl.UpdateRecord(top.API.gCardno + ", " + "CWD" + ", " + top.API.CashInfo.strTransAmount + ", " + nTakenMoneyAllTime + ", " + top.API.gResponsecode + ", " + "PF");
        top.API.displayMessage("after---UpdateRecord");
        // upLoadTSL();
        App.Timer.SetPageTimeout(60);
    }

    function upLoadTSL() {

        if (top.API.gTslFlag) {
            //������|ʱ��|����|��ˮ��|��������|���|���֤��|���֤ͷ��·��|���׽����TSL���ݿ���־
            top.API.gTslFlag = false;
            var TslLog = top.API.gTslDate;
            TslLog += "|" + top.API.gTslTime;
            TslLog += "|" + top.API.gCardno;
            TslLog += "|" + top.API.gTslJnlNum;
            TslLog += "|" + top.API.gTslChooseType;
            TslLog += "|" + top.API.gTslMoneyCount;
            TslLog += "|" + top.API.gIdNumber;
            TslLog += "|" + top.API.gIdCardpic;
            TslLog += "|" + top.API.gTslResult;
            top.Tsl.AddTransLogSync(TslLog); //CreateUpJnlFile
            //�ն˺ţ�8λ�����������ڣ�8λ��������ʱ�䣨6λ�����������ͣ�4λ��0107�����0108����ȡ���
            //�ʺţ�19λ�������׽�10λ������λС��λ�����豸��ˮ�ţ�6λ�����豸��ˮ���κţ�6λ����
            //��̨�����루2λ������̨������ˮ�ţ�12λ�����豸����״̬��2λ��00�����׳ɹ���01�����쳣���ף����쳣״̬���ͣ�4λ��
            var strUpJnl = top.API.gTerminalID;
            strUpJnl += "|!" + top.API.gTslDate;
            strUpJnl += "|!" + top.API.gTslTime;
            strUpJnl += "|!" + top.API.gTslChooseJnlType;
            strUpJnl += "|!" + top.API.gCardno;
            strUpJnl += "|!" + top.API.gTslMoneyCount.replace(".", "");
            strUpJnl += "|!" + top.API.gTslJnlNum;
            strUpJnl += "|!" + top.API.gTslJnlBtn;
            strUpJnl += "|!" + top.API.gTslResponsecode;
            strUpJnl += "|!" + top.API.gTslSysrefnum;
            strUpJnl += "|!00|!���׳ɹ�";
            top.Tsl.HandleRecordFileSync(top.API.MTSL_WRITECHECKTRANSRECORD, strUpJnl);
            //top.Tsl.HandleRecordFileSync(0, strUpJnl);

            /*top.Tsl.HandleRecordFileSync(top.API.MTSL_WRITETRANSRECORD, top.API.gCardno +
                ", " + "CWD" + ", " + top.API.CashInfo.strTransAmount + ", " + nTakenMoneyAllTime +
                ", " + top.API.gResponsecode + ", " + "PF");*/

            // ���ûش���
            // top.API.CashInfo.strTransAmount = top.API.CashInfo.strTransAmount - nTakenMoneyAllTime;
            // top.API.displayMessage("top.API.CashInfo.strTransAmount1=" + top.API.CashInfo.strTransAmount);
        }
        /*function upLoadTSL() {

            //������|ʱ��|����|��ˮ��|��������|���|���֤��|���֤ͷ��·��|���׽����TSL���ݿ���־
            var TslLog = top.API.gTslDate;
            TslLog += "|" + top.API.gTslTime;
            TslLog += "|" + top.API.gCardno;
            TslLog += "|" + top.API.gTslJnlNum;
            TslLog += "|" + top.API.gTslChooseType;
            TslLog += "|" + top.API.gTslMoneyCount;
            TslLog += "|" + top.API.gIdNumber;
            TslLog += "|" + top.API.gIdCardpic;
            TslLog += "|" + top.API.gTslResult;
            top.Tsl.AddTransLogSync(TslLog); //CreateUpJnlFile
            //�ն˺ţ�8λ�����������ڣ�8λ��������ʱ�䣨6λ�����������ͣ�4λ��0107�����0108����ȡ���
            //�ʺţ�19λ�������׽�10λ������λС��λ�����豸��ˮ�ţ�6λ�����豸��ˮ���κţ�6λ����
            //��̨�����루2λ������̨������ˮ�ţ�12λ�����豸����״̬��2λ��00�����׳ɹ���01�����쳣���ף����쳣״̬���ͣ�4λ��
            var strUpJnl = top.API.gTerminalID;
            strUpJnl += "|!" + top.API.gTslDate;
            strUpJnl += "|!" + top.API.gTslTime;
            strUpJnl += "|!" + top.API.gTslChooseJnlType;
            strUpJnl += "|!" + top.API.gCardno;
            strUpJnl += "|!" + top.API.gTslMoneyCount.replace(".", "");
            strUpJnl += "|!" + top.API.gTslJnlNum;
            strUpJnl += "|!" + top.API.gTslJnlBtn;
            strUpJnl += "|!" + top.API.gTslResponsecode;
            strUpJnl += "|!" + top.API.gTslSysrefnum;
            strUpJnl += "|!00|!���׳ɹ�";
            top.Tsl.HandleRecordFileSync(top.API.MTSL_WRITECHECKTRANSRECORD, strUpJnl);

            top.Tsl.HandleRecordFileSync(top.API.MTSL_WRITETRANSRECORD, top.API.gCardno +
                ", " + "CWD" + ", " + top.API.CashInfo.strTransAmount +
                ", " + nTakenMoneyAllTime + ", " +
                top.API.gResponsecode + ", " + "PF")

            // ���ûش���
            // top.API.CashInfo.strTransAmount = top.API.CashInfo.strTransAmount - nTakenMoneyAllTime;
            top.API.displayMessage("top.API.CashInfo.strTransAmount2=" + top.API.CashInfo.strTransAmount);

        }*/
    }

    function SetPartCwc() {
        // nOutMoneyAllTime �ѳ����
        top.API.displayMessage("AllMoney: " + AllMoney + "-----nTakenMoneyAllTime:" + nTakenMoneyAllTime + '----nOutMoneyAllTime:' + nOutMoneyAllTime);
        var partCwcMoney = AllMoney - nTakenMoneyAllTime;
        top.API.displayMessage("partCwcMoney:" + partCwcMoney);
        if (partCwcMoney > 0 && nOutMoneyAllTime != 0) {
            partCwcFlag = true;
        }
        if (partCwcFlag) {
            //������ˮ��
            top.API.needPrintReDEPCash = true;
            top.Dat.GetPersistentData(top.JnlnumTag, top.JnlnumType);
            //�жϳ�������(�������)
            if (top.API.CashInfo.Dealtype == "CWDȡ��") {
                top.API.CashInfo.Dealtype = "REDEP";
                var arrDealType = new Array(top.API.CashInfo.Dealtype);
                top.Dat.SetDataSync(top.API.dealtypeTag, top.API.dealtypeType, arrDealType);
                top.Sys.DataSetSync(top.API.MTRN_TRANSACTIONDIFFER, top.API.MTRN_DEPOSIT_CARD);
                top.API.gTransactiontype = "DEP";
            } else if (top.API.CashInfo.Dealtype == "����ȡ��") {
                top.API.CashInfo.Dealtype = "REDEP";
                top.API.gATMORTCR = "TCR";
                top.API.gTransactiontype = "NOCARDDEP";
                var arrDealType = new Array(top.API.CashInfo.Dealtype);
                top.Dat.SetDataSync(top.API.dealtypeTag, top.API.dealtypeType, arrDealType);
                top.Sys.DataSetSync(top.API.MTRN_TRANSACTIONDIFFER, top.API.MTRN_DEPOSIT_PB);
            }
            //����Ϊ�������
            //top.API.gTransactiontype = "DEP";
            //���ó���(���)���
            var tmp = partCwcMoney + "00";
            var arrCashAmount = new Array(tmp);
            top.Dat.SetDataSync(top.API.transamountTag, top.API.transamountType, arrCashAmount);

            //for �ش�ƾ��
            top.Dat.SetDataSync("DISPENAMOUNT", "STRING", new Array(AllMoney + '00'));  //�ܽ��

            top.Dat.SetDataSync("TAKENAMOUNT", "STRING", new Array(nTakenMoneyAllTime + '00'));  //�ѽ��

            //������ת��־
            responseFlag = 'partCwc';
        }
    }

    function ShowSuccessInfo() {
        ButtonEnable();
        top.API.displayMessage("�����ɹ����ܶ�=" + AllMoney + ";�ѳ�:" + nOutMoneyAllTime);
        nCompleteOrError = true;
        document.getElementById("CnextTip").innerHTML = "������ϣ�����<ȷ��>��ť";
        document.getElementById("OK").style.display = "block";

        top.Tsl.UpdateRecord(top.API.gCardno + ", " + "CWD" + ", " + top.API.CashInfo.strTransAmount + ", " + nOutMoneyAllTime + ", " + top.API.gResponsecode + ", " + "");
        App.Timer.SetPageTimeout(60);
    }

    //�����䳮������г������䷽ʽ���
    function StartDispense() {
        var nUnits = 0;
        top.API.displayMessage("StartDispense,DispenseMoney: " + DispenseMoney);
        var UnitsNotes = 0;
        var ArrDispense = new Array();
        for (nUnits = 0; nUnits < top.API.CashInfo.nCountOfUnits; nUnits++) {
            UnitsNotes += ArrMixResult[nUnits];
            ArrDispense[nUnits] = 0;
        }
        if (bFirstMix) {
            top.API.displayMessage("��1��Dispense");
            top.API.displayMessage("������=" + UnitsNotes);
            bFirstMix = false;
            top.API.gDispenseCounts = UnitsNotes;//�����δ�ӡ���ֺ���Ҫ��¼������
        }
        top.API.displayMessage("�䳮���=" + ArrMixResult);
        if (UnitsNotes <= 100) {
            top.Cdm.Dispense(0, ArrMixResult, "CNY", "0");
        } else {
            var nRoundCount = 100;
            for (nUnits = 0; nUnits < top.API.CashInfo.nCountOfUnits; nUnits++) {
                if (ArrMixResult[nUnits] != 0) {
                    if (nRoundCount > 0) {
                        if (ArrMixResult[nUnits] > nRoundCount) {
                            ArrDispense[nUnits] = nRoundCount;
                            nRoundCount = nRoundCount - ArrDispense[nUnits];
                            top.API.displayMessage("����" + (nUnits + 1) + "�䳮����=" + ArrDispense[nUnits]);
                        } else {
                            ArrDispense[nUnits] = ArrMixResult[nUnits];
                            nRoundCount = nRoundCount - ArrDispense[nUnits];
                            top.API.displayMessage("����" + (nUnits + 1) + "�䳮����=" + ArrDispense[nUnits]);
                            continue;//Ϊ��ȷ������������Ϊ100�š�
                        }
                    }
                    top.API.displayMessage("���θ�����������:" + ArrDispense);
                    top.Cdm.Dispense(0, ArrDispense, "CNY", "0");
                    break;
                }
            }
        }
        for (nUnits = 0; nUnits < top.API.CashInfo.nCountOfUnits; nUnits++) {
            ArrMixResult[nUnits] = 0;
        }
    }


    function VoicesPlay() {
        App.Plugin.Voices.play("voi_7");
    }

    //@User code scope end 

    //event handler
    function onCashDispensed(info, Amount) {
        top.API.displayMessage("onCashDispensed����");
        cOutMoneyThisTime = Amount;
        nOutMoneyAllTime += Amount;
        var arrInfo = new Array();
        arrInfo = info.toArray();
        var nUnits = 0;
        for (nUnits = 0; nUnits < top.API.CashInfo.nCountOfUnits; nUnits++) {
            CurrentInfo[nUnits] += parseInt(arrInfo[nUnits]);
        }
        DispenseMoney = AllMoney - nOutMoneyAllTime;

        var strAmount = cOutMoneyThisTime.toString();//���ν��
        var arrTmp = new Array(strAmount);
        top.Dat.SetDataSync("SINGLEDISPENSEMONEY", "STRING", arrTmp);
        top.Jnl.PrintSync("CashOutBox2");//��ӡ���γ������


        var cdmStatus = top.Cdm.StDetailedDeviceStatus();
        top.API.displayMessage("cdmStatus=" + cdmStatus);
        if (cOutMoneyThisTime == 0 && AllMoney == DispenseMoney && (cdmStatus == "HARDWAREERROR" || cdmStatus == "OFFLINE")) {
            top.ErrorInfo = top.API.PromptList.No4;
            var arrCWCFlag = new Array();
            arrCWCFlag[0] = 1;
            top.Dat.SetPersistentData(top.API.cwcflagTag, top.API.cwcflagType, arrCWCFlag);
        } else {
            showinfo();
			top.API.displayMessage("OutputStatus: " + top.Cdm.StOutputStatus());
            top.Cdm.OpenShutter(top.API.gOpenShutterTimeOut);
			if(top.Cdm.StOutputStatus() == "EMPTY"){
				setTimeout(function(){
				top.Cdm.CloseShutter(top.API.gCloseShutterTimeOut);},10000);
			}
        }
    }

    function onNotDispensable() {
        top.ErrorInfo = top.API.PromptList.No4;
        if (nOutMoneyAllTime == 0) {
            var arrCWCFlag = new Array();
            arrCWCFlag[0] = 1;
            top.Dat.SetPersistentData(top.API.cwcflagTag, top.API.cwcflagType, arrCWCFlag);
        } else {
            showinfo();
            ShowFailedInfo();
        }
    }

    function onMixFailed() {
        top.ErrorInfo = top.API.PromptList.No4;
        if (nOutMoneyAllTime == 0) {
            var arrCWCFlag = new Array();
            arrCWCFlag[0] = 1;
            top.Dat.SetPersistentData(top.API.cwcflagTag, top.API.cwcflagType, arrCWCFlag);
        } else {
            showinfo();
            ShowFailedInfo();
        }
    }

    function onDeviceError() {
        top.ErrorInfo = top.API.PromptList.No4;
        if (nOutMoneyAllTime == 0) {
            //�޸ĳ�����־
            var arrCWCFlag = new Array();
            arrCWCFlag[0] = 1;
            top.Dat.SetPersistentData(top.API.cwcflagTag, top.API.cwcflagType, arrCWCFlag);
        } else {
            showinfo();
            ShowFailedInfo();
        }
    }

    function onMixComplete(MixResult) {
        ArrMixResult = MixResult.toArray();
        var i = 0;
        var bStartDispense = true;
        for (i = 0; i < ArrMixResult.length; i++) {
            if ((ArrMixResult[i] != 0) && (top.API.CashInfo.arrUnitCurrency[i] != 100 )) {
                bStartDispense = false;
                break;
            }
        }
        if (bStartDispense) {
            StartDispense();
        } else {
            top.API.displayMessage("Mix������ش����㳮");
            showinfo();
            ShowFailedInfo();
        }
    }

    function onCashDispenseFailed(info, Amount) {
        top.API.displayMessage("onCashDispenseFailed ,���������" + Amount);
        var arrInfo = new Array();
        arrInfo = info.toArray();
        var nUnits = 0;
        for (nUnits = 0; nUnits < top.API.CashInfo.nCountOfUnits; nUnits++) {
            CurrentInfo[nUnits] += parseInt(arrInfo[nUnits]);
        }
        var cdmStatus = top.Cdm.StDetailedDeviceStatus();
        top.API.displayMessage("cdmStatus=" + cdmStatus);
        cOutMoneyThisTime = Amount;
        nOutMoneyAllTime += Amount;
        DispenseMoney = AllMoney - nOutMoneyAllTime;

        var strAmount = cOutMoneyThisTime.toString();//���ν��
        var arrTmp = new Array(strAmount);
        top.Dat.SetDataSync("SINGLEDISPENSEMONEY", "STRING", arrTmp);
        top.Jnl.PrintSync("CashOutBox2");//��ӡ���γ������


        if (nCashUnitErrRetry > 3 || (cdmStatus == "HARDWAREERROR" || cdmStatus == "OFFLINE")) {
            top.ErrorInfo = top.API.PromptList.No4;
            if (nOutMoneyAllTime == 0) {
                //�޸ĳ�����־
                var arrCWCFlag = new Array();
                arrCWCFlag[0] = 1;
                top.Dat.SetPersistentData(top.API.cwcflagTag, top.API.cwcflagType, arrCWCFlag);
            } else {
                //showinfo();
                ShowFailedInfo();
            }
        } else {
            showinfo();
            nCashUnitErrRetry++;
            var tmp1 = top.Cdm.StOutputStatus();
            if (tmp1 == "EMPTY") {
                top.API.displayMessage("CashUnitError��Mix,���=" + DispenseMoney);
                top.Cdm.Mix(DispenseMoney, "CNY", "1");
                ShowLoading();
            } else {
                top.Cdm.OpenShutter(top.API.gOpenShutterTimeOut);
            }
        }
    }

    //*****************ȡ����г�����*************************
    function onOutPositionNotEmpty() {
        top.API.displayMessage("���� onOutPositionNotEmpty�¼�");
        top.ErrorInfo = top.API.PromptList.No4;
        showinfo();
        ShowFailedInfo();
    }

    function onDispenseAmountIncorrect() {
        top.ErrorInfo = top.API.PromptList.No4;
        showinfo();
        ShowFailedInfo();
    }

    function onCashTaken() {
        App.Timer.SetPageTimeout(180);
        if (bTimeOut) {
            top.API.displayMessage("TimeoutCallBack�Ѿ�����,onCashTakenֱ��return");
            return;
        }
        nTakenMoneyAllTime += cOutMoneyThisTime;
        App.Timer.ClearIntervalTime();
        top.Cdm.CloseShutter(top.API.gCloseShutterTimeOut);
    }

    function onShutterOpened() {
        top.API.displayMessage("onShutterOpened����,��ʾ�ͻ����߳�Ʊ");
        shutterOpenFlag = true; // ���ÿ������������Ϊ��ȡ�� by Gni
        App.Timer.SetIntervalDisposal(VoicesPlay, 12000);
        App.Timer.SetPageTimeout(180);
    }

    function onShutterClosed() {
        top.API.displayMessage("onShutterClosed����");
        if (AllMoney > nOutMoneyAllTime) {
            DispenseMoney = AllMoney - nOutMoneyAllTime;
            top.Cdm.Mix(DispenseMoney, "CNY", "1");
            ShowLoading();
        } else if (AllMoney == nOutMoneyAllTime) {
            ShowSuccessInfo();
        } else {
            ShowFailedInfo();
        }
    }

    function onShutterOpenFailed() {
        top.API.displayMessage("onShutterOpenFailed����");
        top.ErrorInfo = top.API.PromptList.No4;
        if (nOutMoneyAllTime == 0) {
            //�޸ĳ�����־
            var arrCWCFlag = new Array();
            arrCWCFlag[0] = 1;
            top.Dat.SetPersistentData(top.API.cwcflagTag, top.API.cwcflagType, arrCWCFlag);
        } else {
            showinfo();
            ShowFailedInfo();
        }
    }

    function onShutterCloseFailed() {
        top.API.displayMessage("onShutterCloseFailed����");
        top.ErrorInfo = top.API.PromptList.No4;
        if (nOutMoneyAllTime == 0) {
            //�޸ĳ�����־
            var arrCWCFlag = new Array();
            arrCWCFlag[0] = 1;
            top.Dat.SetPersistentData(top.API.cwcflagTag, top.API.cwcflagType, arrCWCFlag);
        } else {
            showinfo();
            ShowFailedInfo();
        }
    }

    function onSubDispenseOk(info, Amount) {
        top.API.displayMessage("onSubDispenseOk����");
        nOutMoneyAllTime += Amount;
        cOutMoneyThisTime = Amount;//�����ѳ����
        if (nOutMoneyAllTime != AllMoney) {
            showinfo();
        }
        App.Timer.SetIntervalDisposal(VoicesPlay, 12000);
        App.Timer.SetPageTimeout(180);
    }

    /********************************************************************************************************/
    //��������ģ��
    function onDatSetPersistentDataComplete(DataName) {
        if ('CWCFLAG' == DataName) {
            //���ó���ԭ��
            var arrCWCREASON = new Array();
            arrCWCREASON[0] = 2;
            top.Dat.SetPersistentData(top.API.cwcreasonTag, top.API.cwcreasonType, arrCWCREASON);
        }
        if ('CWCREASON' == DataName) {
            showinfo();
            ShowFailedInfo();
        }
    }

    function onDatSetPersistentDataError(DataName, ErrorCode) {
        top.ErrorInfo = top.API.PromptList.No5;
        showinfo();
        ShowFailedInfo();
    }


    function onDatGetPersistentDataComplete(DataName, DataType, DataValue) {
        var arrDataValue = DataValue.toArray();
        var JnlNum = 0;
        if ('JNLNUM' == DataName) {
            JnlNum = arrDataValue[0] + 1;
            if (JnlNum.toString().length === 7) {
                JnlNum = 0;
            }
            partCwcJNL = JnlNum;
            //���ý�����ˮ��
            var arrJnlNum = new Array();
            arrJnlNum[0] = JnlNum;
            top.Dat.SetPersistentData(top.JnlnumTag, top.JnlnumType, arrJnlNum);
        }
        //������һ��ȡ�����ˮ��
        top.Dat.SetDataSync("JNLNUM2", "LONG", arrDataValue);
    }

    function onDatGetPersistentDataError(DataName, ErrorCode) {
        top.API.displayMessage("onDatGetPersistentDataError is done,DataName=" + DataName + ",ErrorCode=" + ErrorCode);
        top.ErrorInfo = top.API.PromptList.No4;
        return CallResponse('Exit');
    }

    //******************************************************************************************************

    function SetJnl() {
        var i = 0;
        if (nCompleteOrError) {
            strPRESENTRESULT = "�ɹ�";
        }
        for (i = 0; i < top.API.CashInfo.nCountOfUnits; i++) {
            tmpCurrentInfo[i] = top.API.CashInfo.arrUnitRemain[i];
        }
        top.API.CashInfo.arrUnitRemain = top.API.GetUnitInfo(top.Cdm.PUCurrentCount().toArray());
        //��ȡRJ��Ϣ
        var NewarrRJCount = new Array();
        NewarrRJCount = top.API.GetUnitInfo(top.Cdm.PURejectCount().toArray());
        //
        var nRJCount = 0;
        for (i = 0; i < top.API.CashInfo.nCountOfUnits; i++) {
            rCurrentInfo[i] = NewarrRJCount[i] - arrRJCount[i];
            CurrentInfo[i] += rCurrentInfo[i];
            nRJCount += rCurrentInfo[i];
        }
        CurrentInfo[3] = 0 - nRJCount;
        top.API.displayMessage("ʵ�ʳ�������=" + CurrentInfo);
        top.API.displayMessage("����������=" + rCurrentInfo);
        if (typeof CurrentInfo[5] == 'undefined') {
            CurrentInfo[5] = 0;
        }
        if (typeof CurrentInfo[6] == 'undefined') {
            CurrentInfo[6] = 0;
        }
        if (typeof  rCurrentInfo[5] == 'undefined') {
            rCurrentInfo[5] = 0;
        }
        if (typeof  rCurrentInfo[6] == 'undefined') {
            rCurrentInfo[6] = 0;
        }
        var strAmount = nOutMoneyAllTime.toString();//�ܽ��
        var strJNLData = ", oBOX1=" + CurrentInfo[0] + ", oBOX2=" + CurrentInfo[1] + ", oBOX3=" + CurrentInfo[2]
            + ", oBOX4=" + CurrentInfo[3] + ", oBOX5=" + CurrentInfo[4] + ", oBOX6=" + CurrentInfo[5] + ", oBOX7="
            + CurrentInfo[6] + ", rBOX1=" + rCurrentInfo[0] + ", rBOX2=" + rCurrentInfo[1] + ", rBOX3=" + rCurrentInfo[2]
            + ", rBOX4=" + rCurrentInfo[3] + ", rBOX5=" + rCurrentInfo[4] + ", rBOX6=" + rCurrentInfo[5] + ", rBOX7=" + rCurrentInfo[6]
            + ",PRESENTRESULT=" + strPRESENTRESULT + ",PRESENTAMOUNT=" + strAmount;

        var arrCashOutBoxData = new Array(strJNLData);
        top.Dat.SetDataSync(top.API.ContentDataTag, top.API.ContentDataType, arrCashOutBoxData);
        top.Jnl.PrintSync("CashOutBox3");

        if('partCwc' == responseFlag){
            top.Jnl.PrintSync("selectReDep");
        }
    }

    document.getElementById('OK').onclick = function () {
        ButtonDisable();
        if (!partCwcFlag) {
            responseFlag = "OK";
            upLoadTSL();
            top.Jnl.PrintSync("CwdComplete");
        }else if(saveBackFlag && top.API.gbSAVEBACK_DEAL){
            top.API.CashInfo.strTransAmount = top.API.CashInfo.strTransAmount - nTakenMoneyAllTime;
			
			var arrTmp = new Array("2");
			top.Dat.SetDataSync("DEPFLAG", "STRING", arrTmp);//�����48��Ϊ1��ȡ��ش�Ϊ2
        }
        //��ӡȡ������������˵��쳣��ƾ��
        return CallResponse(responseFlag);
    }
    document.getElementById('Exit').onclick = function () {
        ButtonDisable();
        responseFlag = 'Exit';
        top.API.needPrintReDEPCash = false;
        top.API.displayMessage("Exit------responseFlag:" + responseFlag)
        return CallResponse(responseFlag);
    }

    //**************************************************************
    //Register the event
    function EventLogin() {
        top.Dat.GetPersistentDataComplete.connect(onDatGetPersistentDataComplete);
        top.Dat.GetPersistentDataError.connect(onDatGetPersistentDataError);
        top.Dat.SetPersistentDataComplete.connect(onDatSetPersistentDataComplete);
        top.Dat.SetPersistentDataError.connect(onDatSetPersistentDataError);
        /////
        top.Cdm.MixComplete.connect(onMixComplete);
        top.Cdm.MixFailed.connect(onMixFailed);
        top.Cdm.CashDispensed.connect(onCashDispensed);
        top.Cdm.CashTaken.connect(onCashTaken);
        top.Cdm.NotDispensable.connect(onNotDispensable);
        top.Cdm.CashDispenseFailed.connect(onCashDispenseFailed);
        top.Cdm.DeviceError.connect(onDeviceError);
        top.Cdm.DispenseAmountIncorrect.connect(onDispenseAmountIncorrect);
        top.Cdm.OutPositionNotEmpty.connect(onOutPositionNotEmpty);
        top.Cdm.SubDispenseOk.connect(onSubDispenseOk);

        //Door
        top.Cdm.ShutterOpened.connect(onShutterOpened);
        top.Cdm.ShutterOpenFailed.connect(onShutterOpenFailed);
        top.Cdm.ShutterClosed.connect(onShutterClosed);
        top.Cdm.ShutterCloseFailed.connect(onShutterCloseFailed);

    }

    function EventLogout() {
        top.Dat.GetPersistentDataComplete.disconnect(onDatGetPersistentDataComplete);
        top.Dat.GetPersistentDataError.disconnect(onDatGetPersistentDataError);
        top.Dat.SetPersistentDataComplete.disconnect(onDatSetPersistentDataComplete);
        top.Dat.SetPersistentDataError.disconnect(onDatSetPersistentDataError);
        ///
        top.Cdm.MixComplete.disconnect(onMixComplete);
        top.Cdm.MixFailed.disconnect(onMixFailed);
        top.Cdm.CashDispensed.disconnect(onCashDispensed);
        top.Cdm.CashTaken.disconnect(onCashTaken);
        top.Cdm.NotDispensable.disconnect(onNotDispensable);
        top.Cdm.CashDispenseFailed.disconnect(onCashDispenseFailed);
        top.Cdm.DeviceError.disconnect(onDeviceError);
        top.Cdm.DispenseAmountIncorrect.disconnect(onDispenseAmountIncorrect);
        top.Cdm.OutPositionNotEmpty.disconnect(onOutPositionNotEmpty);
        top.Cdm.SubDispenseOk.disconnect(onSubDispenseOk);
        //Door
        top.Cdm.ShutterOpened.disconnect(onShutterOpened);
        top.Cdm.ShutterOpenFailed.disconnect(onShutterOpenFailed);
        top.Cdm.ShutterClosed.disconnect(onShutterClosed);
        top.Cdm.ShutterCloseFailed.disconnect(onShutterCloseFailed);
    }

    //Countdown function
    function TimeoutCallBack() {
        bTimeOut = true;
        ButtonDisable();
        responseFlag = 'TimeOut';
        top.API.displayMessage("TimeoutCallBack----responseFlag:" + responseFlag);
        var tmp1 = top.Cdm.StOutputStatus();
        var tmp2 = top.Cim.StInputStatus();
        if (tmp1 != "EMPTY" || tmp2 != "EMPTY") {
            top.API.displayMessage("�ͻ�δȡ��");
            strPRESENTRESULT = "�ͻ�δȡ��"
            top.ErrorInfo = top.API.PromptList.No3;
            var arrTransactionResult = new Array("�ͻ�δȡ��");
            top.API.gTakeCardAndPrint = true;
            top.Dat.SetDataSync("TRANSACTIONRESULT", "STRING", arrTransactionResult);
            return CallResponse(responseFlag);
        } else {
            if (nCompleteOrError) {
                return CallResponse('OK');
            } else {
                top.ErrorInfo = top.API.PromptList.No3;
                return CallResponse(responseFlag);
            }
        }
    }

    //Page Return

    //remove all event handler
    function Clearup() {
        //TO DO:
        EventLogout();
        top.Cdm.CloseShutter(top.API.gCloseShutterTimeOut);
        App.Timer.ClearIntervalTime();
        App.Plugin.Voices.del();
        App.Timer.ClearTime();
    }
})();
