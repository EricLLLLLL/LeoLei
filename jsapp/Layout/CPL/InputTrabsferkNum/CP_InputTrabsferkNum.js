/*@create by:  tsxiong
 *@time: 2016年03月20日
 */
; (function () {
    var $TipError = $("#TipError");
    var CallResponse = App.Cntl.ProcessOnce( function(Response){
        //TO DO:
        Clearup();
        //Entry the flows control process.
        App.Cntl.ProcessDriven(Response);
    });
    var InputID = document.getElementById("AccountInput");
    var CardNoValue = "";
    var Initialize = function () {
        //@initialize scope start
        ButtonDisable();
        App.Timer.TimeoutDisposal(TimeoutCallBack);

        // 销户流程默认提示
        if( top.API.CashInfo.Dealtype == 'CancelAccount' ){
            $TipError.html("转入账户需为农行借记卡或存折！");
        }

        App.Plugin.Keyboard.show("9", "PageSubject", "KeyboardDiv9");
        KeyboardClick();
        InputID.focus();
        App.Plugin.Voices.play("voi_45");
        ButtonEnable();
    }();//Page Entry

    //@User ocde scope start
    function ButtonDisable() {
        document.getElementById('Exit').disabled = true;
        document.getElementById('OK').disabled = true;
    }

    function ButtonEnable() {
        document.getElementById('Exit').disabled = false;
        document.getElementById('OK').disabled = false;
    }

    document.getElementById('Exit').onclick = function () {
        ButtonDisable();
        top.ErrorInfo = top.API.PromptList.No2;
        return CallResponse('Exit');
    }

    document.getElementById('OK').onclick = function () {
        ButtonDisable();
        var tmpCardNoValue = CardNoValue;
        while(tmpCardNoValue.indexOf(" ") != -1)
        {
            tmpCardNoValue = tmpCardNoValue.replace(" ","");
        }
        if( top.API.CashInfo.Dealtype == 'CancelAccount' && top.API.CancelCardNo == tmpCardNoValue ){
            ButtonEnable();
            $TipError.html("转入账号不能为待销户账号，请重新输入！");
            InputID.value = "";
            CardNoValue = "";

        }else if(tmpCardNoValue.length > 0){
            // if( tmpCardNoValue.length == 16 || tmpCardNoValue.length == 19 || tmpCardNoValue.length == 17 ){
                //add by tsx 应用根据位数判断存折还是卡号。卡号走大额存款，存折走ATM存款
                top.API.gCardOrBookBank = 2;
                if (tmpCardNoValue.length == 19 || tmpCardNoValue.length == 16) {
                    top.API.gCardOrBookBank = 1;
                }
                if (tmpCardNoValue.substr(0,2) == "41"){
                    tmpCardNoValue = tmpCardNoValue.substr(2,tmpCardNoValue.length-2);
                }
                top.API.PayeeAccount = tmpCardNoValue;
                if( top.API.CashInfo.Dealtype == 'CancelAccount'){
                    top.API.gCardno = tmpCardNoValue;
                    top.API.gTransactiontype = "QRYCUSTNAME";
                    var arrCardNo = new Array(top.API.gCardno);
                    top.Dat.SetDataSync(top.API.cardnoTag, top.API.cardnoType, arrCardNo);
                }else if( top.API.CashInfo.Dealtype == "InLineFlag") {
                    // 行内转账查询
                    top.API.gCardno = tmpCardNoValue;
                    top.API.gTransactiontype = "QRYINCUSTNAME";
                    var arrCardNo = new Array(top.API.gCardno);
                    top.Dat.SetDataSync("TFRCARDNO", "STRING", arrCardNo);
                }else if( top.API.CashInfo.Dealtype == "OutLineFlag") {
                    // 行外转账
                    top.API.gCardno = tmpCardNoValue;
                    // top.API.gTransactiontype = "QRYINCUSTNAME";
                    var arrCardNo = new Array(top.API.gCardno);
                    top.Dat.SetDataSync("TFRCARDNO", "STRING", arrCardNo);
                }else {
                    top.API.gTransactiontype = "QueryTransInfo";
                    // var arrCardNo = new Array(tmpCardNoValue);
                    // // TODO 设置到数据库的要是转账账号字段，cardnoTag是无卡无折输入的
                    // top.Dat.SetDataSync('TFRCARDNO', top.API.cardnoType, arrCardNo);
                }
                //top.Jnl.PrintSync("AccountAction");
                return CallResponse('OK');
            }else{
            	ButtonEnable();
                $TipError.html("转入账号格式不正确，请重新输入！");
            }
            
        // }else{
        //     ButtonEnable();
        //     $TipError.html("输入账号不能为空!");
        // }
    }
    function KeyboardClick() {
        var arrKeyboardKey = document.getElementsByName("Name_Keyboard");
        for (var i = 0; i < arrKeyboardKey.length; i++) {
            var keyEvent = arrKeyboardKey[i];
            keyEvent.onclick = function (e) {
                InputID.focus();
                App.InputEdit.getCurPosition(InputID);
                if ('退格' == this.innerText) {
                    App.InputEdit.getInput(InputID , 1 ,"BS");
                    CardNoValue = InputID.value;
                    if(CardNoValue.length == 5 || CardNoValue.length == 10 || CardNoValue.length == 15 || CardNoValue.length == 20)
                    {
                        App.InputEdit.getInput(InputID , 1 ,"BS");
                        CardNoValue = InputID.value;
                    }
                } else if ('清除' == this.innerText) {
                    InputID.value = "";
                    CardNoValue = "";
                } else {
                    if (CardNoValue.length < 24) {
                        if(CardNoValue.length == 4 || CardNoValue.length == 9 || CardNoValue.length == 14 || CardNoValue.length == 19)
                        {
                            App.InputEdit.getInput(InputID , 0 ," ");
                            CardNoValue = InputID.value;
                        }
                        App.InputEdit.getInput(InputID , 0 , this.innerText);
                        CardNoValue = InputID.value;
                    }
                }
            }
        }
    }


    //Countdown function
    function TimeoutCallBack() {
        top.ErrorInfo = top.API.PromptList.No3;
        return CallResponse("TimeOut");
    }
    //Page Return

    //remove all event handler
    function Clearup() {
        //TO DO:
        App.Plugin.Voices.del();
        App.Timer.ClearTime();
    }
})();
