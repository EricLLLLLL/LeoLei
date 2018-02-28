;
(function () {
    var capitalMoney,//��дת�˽��
        PayeeBank =  top.API.PayeeBank,
        TransferMoney =  top.API.TransferMoney,
        PayeeName =  top.API.PayeeName,
        PayeeAccount = top.API.PayeeAccount,
        Dealtype =  top.API.CashInfo.Dealtype, // ����ת�� InLineFlag������ת�� OutLineFlag
        transferWay = '',
    CallResponse = App.Cntl.ProcessOnce(function (Response) {
            //TO DO:
            Clearup();
            //Entry the flows control process.
            App.Cntl.ProcessDriven(Response);
        }),
        Initialize = function () {
            EventLogin();
            ButtonDisable();
            //@initialize scope start
            showInfo();
            //Ϊ��ƾ����������
            setInfoForJnl();
            ButtonEnable();
            App.Timer.TimeoutDisposal(TimeoutCallBack);
        }(); //Page Entry

    function showInfo() {
        capitalMoney = top.cmycurd(TransferMoney);
        $('#PayeeAccount').text(PayeeAccount);
        $('#PayeeName').text(PayeeName);
        $('#PayeeBank').text(PayeeBank);
        $('#TransferMoney').text(TransferMoney);
        $('#TransferMoney2').text(capitalMoney);
        top.API.TransferPoundage = Dealtype == "InLineFlag" ? "0.00" : $.trim(top.Dat.GetDataSync("POUNDAGE", "STRING").toArray()[0]);
        switch (top.API.transferWay) {
            case '0':
                transferWay = 'ʵʱת��';
                break;
            case '1':
                transferWay = '��ͨת��';
                break;
            case '2':
                transferWay = '����ת��';
                break;
        }
        $('#transferWay').text(transferWay);
        var comment;
        if (top.API.postScript != '') {
            comment = top.API.CommentSelect + ',' + top.API.postScript;
        } else {
            comment = top.API.CommentSelect;
        }
        $('#transferComment').text(comment);
        $('#transferPoundage').text( top.API.TransferPoundage );
        // top.Dat.SetDataSync('POUNDAGE','STRING',[top.API.TransferPoundage]);//������
    }

    function setInfoForJnl() {
        top.Dat.SetDataSync('DEALTYPE','STRING',[Dealtype]);//��������
        top.Dat.SetDataSync('TREANSMETHOD','STRING',[transferWay]);
        top.Dat.SetDataSync('TFRBANK','STRING',[PayeeBank]);//�տ���
        top.Dat.SetDataSync('TFRCARDNO','STRING',[PayeeAccount]);//�տ����˺�
        top.Dat.SetDataSync('TFRCUSTNAME','STRING',[PayeeName]); //�տ�������
        top.Dat.SetDataSync('REMITTANCE','STRING',[capitalMoney]);//������д
		var tempMoney = TransferMoney + '00';
        top.Dat.SetDataSync('TRANSAMOUNT','STRING',[tempMoney]);//�����
    }

    //@User ocde scope start

    function ButtonDisable() {
        document.getElementById('Exit').disabled = true;
        document.getElementById('Back').disabled = true;
        document.getElementById('OK').disabled = true;
    }

    function ButtonEnable() {
        document.getElementById('Exit').disabled = false;
        document.getElementById('Back').disabled = false;
        document.getElementById('OK').disabled = false;
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

    document.getElementById('OK').onclick = function () {
        ButtonDisable();
        top.API.gTransactiontype = 'TRANSFERACTS'; // ת�˻��
        top.Dat.SetDataSync("TRANSFERTIMES", "STRING", [top.API.TRANSFERTIMES]); // ���õ�һ��ת�˱�ʶ 1��һ�� 2�ڶ���
        top.Dat.SetDataSync("TFRCARDNO", "STRING", [top.API.PayeeAccount]); // ����ת�뿨����
        top.Dat.SetDataSync("INCUSTOMERNAME", "STRING", [PayeeName]); // ����ת�뻧��

        if( top.Dat.GetDataSync("MONEYLIMIT", "STRING").toArray() == 0 ){
            // ���������˲�
            return CallResponse('MaxMoneyFlag');
        }else{
            return CallResponse('MinMoneyFlag');
        }
    };

    //@User code scope end


    //Register the event
    function EventLogin() {

    }

    function EventLogout() {

    }

    //Countdown function
    function TimeoutCallBack() {
        top.ErrorInfo = top.API.PromptList.No3;
        return CallResponse('TimeOut');
    }

    //remove all event handler
    function Clearup() {
        //TO DO:
        EventLogout();
        App.Timer.ClearTime();
    }
})();