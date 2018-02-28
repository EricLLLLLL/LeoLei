; (function(){
    var CallResponse = App.Cntl.ProcessOnce (function (Response) { 
            //TO DO:
            Clearup();
            //Entry the flows control process.
            App.Cntl.ProcessDriven( Response );
        }),
        Initialize = function() {
            EventLogin();
            //@initialize scope start
             
            //
            App.Timer.TimeoutDisposal(TimeoutCallBack);
        }();//Page Entry

    //@User ocde scope start
    document.getElementById('Exit').onclick = function(){
        top.ErrorInfo = top.API.PromptList.No2;
        return CallResponse('Exit');
    }

    document.getElementById('Back').onclick = function(){
        return CallResponse('Back');
    }

    // 行内转账
    document.getElementById('ChooseInBank').onclick = function(){
        // top.API.gTransactiontype = "QRYCUSTNAME";  // 查询户名
        top.API.CashInfo.Dealtype = 'InLineFlag';
        top.Dat.SetDataSync("INOROUT", "STRING", ["1"]);
        // top.Dat.SetDataSync("DEALTYPE", "STRING", ["行内转账"]);
        top.Dat.SetDataSync('TFRBANK','STRING',["中国农业银行"]);//收款行
        return CallResponse('ChooseInBank');
    }

    // 行外转账
    document.getElementById('ChooseOutBank').onclick = function(){
        // top.API.gTransactiontype = "OutLineFlag";
        top.API.CashInfo.Dealtype = 'OutLineFlag';
        top.Dat.SetDataSync("INOROUT", "STRING", ["2"]);
        // top.Dat.SetDataSync("DEALTYPE", "STRING", ["行外转账"]);
        return CallResponse('ChooseOutBank');
    }
   
    //@User code scope end 

    //event handler  
   
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
    function Clearup(){
        //TO DO:
        EventLogout();
        App.Timer.ClearTime();
    }
})();
