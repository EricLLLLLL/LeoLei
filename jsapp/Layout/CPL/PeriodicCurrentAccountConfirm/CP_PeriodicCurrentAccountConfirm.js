;
(function() {
    var CallResponse = App.Cntl.ProcessOnce(function(Response) {
            //TO DO:
            Clearup();
            //Entry the flows control process.
            App.Cntl.ProcessDriven(Response);
        }),
        Initialize = function() {
            EventLogin();
            setData();
            //@initialize scope start
            App.Timer.TimeoutDisposal(TimeoutCallBack);
        }(); //Page Entry

    //@User ocde scope start
    document.getElementById('Back').onclick = function() {
        return CallResponse('Back');
    }
    document.getElementById('Exit').onclick = function() {
        top.ErrorInfo = top.API.PromptList.No2;
        return CallResponse('Exit');
    }

    document.getElementById('OK').onclick = function() {
        top.API.gTransactiontype = "SAVINGSTOCURRENTACCOUNT";

        return CallResponse('OK');
    }

    //@User code scope end
    // set data
    function setData(){
        $("#Username").html(top.changeName(top.API.gCustomerName));
        $("#AccountNumber").html(top.changeCardNum(top.API.gCardno));
        $("#Moneytype").html("�����");
        // $("#PaymentMethod").html(top.API.PaymentMethod);
        $("#PrincipalAmount").html(top.API.PCABalance); // ����
        $("#Flag").html("��"); // �����־
        // $("#InterestMoney").html(top.Dat.GetDataSync("INTEREST", "STRING").toArray()[0]); // ��Ϣ
    }


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