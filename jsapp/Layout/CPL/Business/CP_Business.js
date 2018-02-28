/*@create by:  tsxiong
 *@time: 2016��03��20��
 */
;(function () {
    var bSPL1 = false;
    var CallResponse = App.Cntl.ProcessOnce(function (Response) {
        //TO DO:
        Clearup();
        //Entry the flows control process.
        App.Cntl.ProcessDriven(Response);
    });
    var Initialize = function () {
        //@initialize scope start
        App.Timer.TimeoutDisposal(TimeoutCallBack);
        top.API.gShowPrintButton = true;
        App.Plugin.Voices.play("voi_36");
        top.API.gbOrderCWD = false;
        ButtonDisable();
        top.Sys.ItemClearSync(top.API.MTRN_TRANSACTIONDIFFER);
        InitTSLData();
        BtnsContorl();
        ButtonEnable();


    }(); //Page Entry




    function ButtonDisable() {
        document.getElementById('DEP').disabled = true;
        document.getElementById('PWD').disabled = true;
        document.getElementById('CWD').disabled = true;
        document.getElementById('OrderCWD').disabled = true;
        document.getElementById('INQ').disabled = true;
        document.getElementById('Exit').disabled = true;
        document.getElementById('Transfer').disabled = true;
    }

    function ButtonEnable() {
        document.getElementById('DEP').disabled = false;
        document.getElementById('PWD').disabled = false;
        document.getElementById('CWD').disabled = false;
        document.getElementById('OrderCWD').disabled = false;
        document.getElementById('INQ').disabled = false;
        document.getElementById('Exit').disabled = false;
        document.getElementById('Transfer').disabled = false;
    }

    function InitTSLData() {
        top.API.gTslChooseType = ""; //��������
        top.API.gTslJnlNum = ""; //��ˮ��
        top.API.gTslMoneyCount = ""; //���
        top.API.gTslResult = "FAIL"; //���׽�� Ĭ��Ϊʧ�ܡ�
        top.API.gTslDate = "";
        top.API.gTslTime = "";
        top.API.gTslFlag = false;
        top.API.gTslSysrefnum = "";
        top.API.gTslChooseJnlType = ""; //������ˮ�������ͣ�0107�����0108����ȡ��
        top.API.gTslSysrefnum = ""; //��̨������ˮ��
        top.API.gTslJnlBtn = "";//�豸��ˮ���κ�
        top.API.gTslFailType = ""; //�쳣״̬���ͣ�4λ��
        top.API.gTslResponsecode = "";
        top.API.gMixSelfCWD = false;//����ȡ���־λ��ʼ��
    }

    function BtnsContorl() {
        if (!top.API.gbCARDCWD_DEAL) {
            document.getElementById('CWD').style.display = "none";
        } else {
            document.getElementById('CWD').style.display = "block";
        }
        if (!top.API.gbCARDDEP_DEAL) {
            document.getElementById('DEP').style.display = "none";
        } else {
            document.getElementById('DEP').style.display = "block";
        }
        if (!top.API.CashInfo.bINQ || top.API.gATMORTCR == "ATM") {//����ũ�� ���ƿ������ÿ����ܲ�ѯ���
            document.getElementById('INQ').style.display = "none";
        }
        if (top.API.gOrderCWDFlag == "0" || !top.API.gbCARDCWD_DEAL) {
            document.getElementById('OrderCWD').style.display = "none";
        } else {
            document.getElementById('OrderCWD').style.display = "block";
        }
        if ( top.API.gbTRANSFER_DEAL || top.API.gbTRANSFERCANCEL_DEAL || top.API.gbPCA_DEAL || top.API.gbCTR_DEAL ) {
            $("#Transfer").show();
        } else {
            $("#Transfer").hide();
        }
    }

    document.getElementById("Transfer").onclick = function () {
        ButtonDisable();
        //top.Jnl.PrintSync("SelectDEP");
        // top.API.CashInfo.Dealtype = "DEP���";
        // var arrDealType = new Array(top.API.CashInfo.Dealtype);
        // top.Dat.SetDataSync(top.API.dealtypeTag, top.API.dealtypeType, arrDealType);
        // top.Sys.DataSetSync(top.API.MTRN_TRANSACTIONDIFFER, top.API.MTRN_DEPOSIT_CARD);
        //top.API.gTransactiontype = "DEP";
        return CallResponse("Transfer");
    }
    document.getElementById("DEP").onclick = function () {
        ButtonDisable();
        top.Jnl.PrintSync("SelectDEP");
        top.API.CashInfo.Dealtype = "DEP���";
        top.Dat.SetDataSync("DEPFLAG", "STRING", ["1"]);//�����48��Ϊ1��ȡ��ش�Ϊ2
        var arrDealType = new Array(top.API.CashInfo.Dealtype);
        top.Dat.SetDataSync(top.API.dealtypeTag, top.API.dealtypeType, arrDealType);

        top.Sys.DataSetSync(top.API.MTRN_TRANSACTIONDIFFER, top.API.MTRN_DEPOSIT_CARD);
        top.API.gTransactiontype = "DEP";
        return CallResponse("DEP");
    }

    document.getElementById("CWD").onclick = function () {
        ButtonDisable();
        top.Jnl.PrintSync("SelectCARDCWD");
        top.API.CashInfo.Dealtype = "CWDȡ��";
        top.API.gTransactiontype = "QRYCWDMONEY";

        top.Dat.SetDataSync("QRYCWDMONEYFLAG", "STRING", ["2"]);//��ǿ��������ۼ�ȡ��Ϊ"2"
        var arrDealType = new Array(top.API.CashInfo.Dealtype);
        top.Dat.SetDataSync(top.API.dealtypeTag, top.API.dealtypeType, arrDealType);
        top.Sys.DataSetSync(top.API.MTRN_TRANSACTIONDIFFER, top.API.MTRN_WITHDRAW_CARD);
        if (top.API.gbContinueTransFlag === true) {
            return CallResponse("CWDcontinue");
        } else {
            return CallResponse("CWD");
        }
    }

    document.getElementById("OrderCWD").onclick = function () {
        ButtonDisable();
        top.API.gbOrderCWD = true;
        top.Jnl.PrintSync("SelectCARDCWD");
        top.API.CashInfo.Dealtype = "CWDȡ��";
        top.API.gTransactiontype = "QRYCWDMONEY";
        var arrDealType = new Array(top.API.CashInfo.Dealtype);
        top.Dat.SetDataSync(top.API.dealtypeTag, top.API.dealtypeType, arrDealType);
        top.Sys.DataSetSync(top.API.MTRN_TRANSACTIONDIFFER, top.API.MTRN_WITHDRAW_CARD);
        if (top.API.gbContinueTransFlag === true) {
            return CallResponse("CWDcontinue");
        } else {
            return CallResponse("CWD");
        }
    }

    document.getElementById("INQ").onclick = function () {
        ButtonDisable();
        top.Jnl.PrintSync("SelectINQ");
        top.API.CashInfo.Dealtype = "INQ";
        var arrDealType = new Array(top.API.CashInfo.Dealtype);
        top.Dat.SetDataSync(top.API.dealtypeTag, top.API.dealtypeType, arrDealType);
        var arrTRANSACTIONTYPE = new Array("INQ");
        top.API.gTransactiontype = "INQ";
        //����ѯ
        var arrBalanceInquiryType = new Array("NOTYPE");
        top.Dat.SetDataSync(top.API.BalanceInquiryTag, top.API.BalanceInquiryType, arrBalanceInquiryType);
        return CallResponse("INQ");
    }

    document.getElementById("Exit").onclick = function () {
        ButtonDisable();
        top.Jnl.PrintSync("SelectCancel");
        top.ErrorInfo = top.API.PromptList.No2;
        return CallResponse("Exit");
    }

    //@User code scope end
    function TimeoutCallBack() {
        top.Jnl.PrintSync("PageTimeOut");
        top.ErrorInfo = top.API.PromptList.No3;
        return CallResponse('TimeOut');
    }

    //Register the event
    //Page Return


    //remove all event handler
    function Clearup() {
        App.Timer.ClearTime();
        App.Plugin.Voices.del();
        //TO DO:
    }
})();