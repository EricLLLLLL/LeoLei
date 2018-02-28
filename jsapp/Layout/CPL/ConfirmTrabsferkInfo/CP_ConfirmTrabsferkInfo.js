;
(function () {
    var capitalMoney,//大写转账金额
        PayeeBank =  top.API.PayeeBank,
        TransferMoney =  top.API.TransferMoney,
        PayeeName =  top.API.PayeeName,
        PayeeAccount = top.API.PayeeAccount,
        Dealtype =  top.API.CashInfo.Dealtype, // 行内转账 InLineFlag，行外转账 OutLineFlag
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
            //为打凭条设置数据
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
                transferWay = '实时转账';
                break;
            case '1':
                transferWay = '普通转账';
                break;
            case '2':
                transferWay = '次日转账';
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
        // top.Dat.SetDataSync('POUNDAGE','STRING',[top.API.TransferPoundage]);//手续费
    }

    function setInfoForJnl() {
        top.Dat.SetDataSync('DEALTYPE','STRING',[Dealtype]);//交易类型
        top.Dat.SetDataSync('TREANSMETHOD','STRING',[transferWay]);
        top.Dat.SetDataSync('TFRBANK','STRING',[PayeeBank]);//收款行
        top.Dat.SetDataSync('TFRCARDNO','STRING',[PayeeAccount]);//收款人账号
        top.Dat.SetDataSync('TFRCUSTNAME','STRING',[PayeeName]); //收款人姓名
        top.Dat.SetDataSync('REMITTANCE','STRING',[capitalMoney]);//汇款金额大写
		var tempMoney = TransferMoney + '00';
        top.Dat.SetDataSync('TRANSAMOUNT','STRING',[tempMoney]);//汇款金额
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
        top.API.gTransactiontype = 'TRANSFERACTS'; // 转账汇款
        top.Dat.SetDataSync("TRANSFERTIMES", "STRING", [top.API.TRANSFERTIMES]); // 设置第一次转账标识 1第一次 2第二次
        top.Dat.SetDataSync("TFRCARDNO", "STRING", [top.API.PayeeAccount]); // 设置转入卡卡号
        top.Dat.SetDataSync("INCUSTOMERNAME", "STRING", [PayeeName]); // 设置转入户名

        if( top.Dat.GetDataSync("MONEYLIMIT", "STRING").toArray() == 0 ){
            // 大额，需联网核查
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