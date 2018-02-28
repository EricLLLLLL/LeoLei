;
(function () {
    var Inputdata = '',
        TextAreaDom = document.getElementById("postScript"),
        PayeeNameDom = document.getElementById("PayeeName"),
        PayeeBankDom = document.getElementById("PayeeBank"),
        TransferType = top.API.CashInfo.Dealtype, // 行内转账 InLineFlag，行外转账 OutLineFlag
        InputDomArray = new Array(),
        bFirstKey = true,
        showFlag = 0,//键盘0不显示 1 显示
        currentId = '';
    CallResponse = App.Cntl.ProcessOnce(function (Response) {
        //TO DO:
        top.Siu.SetPinPadLight('OFF');
        Clearup();
        //Entry the flows control process.
        App.Cntl.ProcessDriven(Response);
    }),
        Initialize = function () {
            EventLogin();
            //@initialize scope start
            App.Timer.TimeoutDisposal(TimeoutCallBack);
            showInfo();

            top.Pin.GetData(0, false, "0,1,2,3,4,5,6,7,8,9,ENTER,CLEAR,CANCEL", "CANCEL", -1);
            top.Siu.SetPinPadLight('CONTINUOUS');

            if( TransferType == "InLineFlag" ){
                App.Plugin.Voices.play("voi_51");
            }else{
                App.Plugin.Voices.play("voi_52");
            }
        }(); //Page Entry

    function ButtonDisable() {
        document.getElementById('Exit').disabled = true;
        document.getElementById('Back').disabled = true;
        document.getElementById('OK').disabled = true;
    }

    function ButtonEnable() {
        document.getElementById('Back').disabled = false;
        document.getElementById('Exit').disabled = false;
        document.getElementById('OK').disabled = false;
    }

    //@User ocde scope start
    document.getElementById('Exit').onclick = function () {
        top.API.displayMessage("点击Exit");
        ButtonDisable();
        top.ErrorInfo = top.API.PromptList.No2;
        if (showFlag == 1) {
            //隐藏输入法
            $("#MAIN",parent.document).height(1024).removeClass("main-sroll"); // 调整页面结构高度，隐藏滚动条
            top.Ime.HideIME('');
        }
        return CallResponse('Exit');
    };

    document.getElementById('Back').onclick = function () {
        top.API.displayMessage("点击Back");
        ButtonDisable();
        if (showFlag == 1) {
            //隐藏输入法
            $("#MAIN",parent.document).height(1024).removeClass("main-sroll");
            top.Ime.HideIME('');
        }
        return CallResponse('Back');
    };

    document.getElementById('OK').onclick = function () {
        top.API.displayMessage("点击OK");
        if (showFlag == 1) {
            //隐藏输入法
            $("#MAIN",parent.document).height(1024).removeClass("main-sroll");
            top.Ime.HideIME('');
        }
        var tmpChar = TextAreaDom.innerText;
        var strlen = 0;
        for (var i = 0; i < tmpChar.length; i++) {
            if (tmpChar.charCodeAt(i) > 255) {
                strlen += 2;
            } else {
                strlen++
            }
        }
        var radioValue = $("input[type='radio']:checked").val();
        var PayeeName = PayeeNameDom.value;
        var PayeeBank = PayeeBankDom.value;

        if (Inputdata == '') {
            $('#TransferTip').text('转账金额不能为空，请输入转账金额！');
        } else if (radioValue == null) {
            $('#TransferTip').text('转账方式不能为空，请选择转账方式！');
        } else if (strlen > 39) {
            $('#TransferTip').text('输入附言字数超限，请重新输入！');
        } else if (TransferType == "OutLineFlag" &&( PayeeName == null || PayeeName == "")) {
            $('#TransferTip').text('收款方户名不能为空，请输入收款方户名！');
        } else {
            top.API.CommentSelect = $("#CommentSelect").find("option:selected").text();
            top.API.postScript = $("#postScript").text();
            top.API.PayeeName = PayeeName;
            top.API.PayeeBank = PayeeBank;
            top.API.TransferMoney = Inputdata;
            top.API.transferWay = radioValue;
            top.API.displayMessage("CommentSelect:" + top.API.CommentSelect + ";postScript:" + top.API.postScript + ";TransferMoney:" + Inputdata);
            
            top.API.MONEYLIMIT = parseInt(top.API.TransferMoney) >= 50000 ? "0" : "1";
            top.Dat.SetDataSync("MONEYLIMIT", "STRING", [top.API.MONEYLIMIT]); // 设置大小额标识符 0大额 1小额

            top.Dat.SetDataSync("TRANSFERWAY", "STRING", [top.API.transferWay]); // 设置转账方式标识 0实时 1普通 2次日

            ButtonDisable();
            // top.API.gTransactiontype = 'Transfer';
        
            if( TransferType == "InLineFlag" ){
                return CallResponse('InLineFlag');
            }else{
                top.API.gTransactiontype = 'QRYFEE'; // 行外转账查手续费
                return CallResponse('OutLineFlag');
            }
        }
    };

    //@User code scope end
    function showInfo() {
        var INCUSTOMERNAME = top.Dat.GetDataSync("INCUSTOMERNAME", "STRING").toArray()[0];
        $('#PayeeAccount').val(top.API.PayeeAccount); // 收款方账号
        $('#PayeeName').val( TransferType == "InLineFlag" ? INCUSTOMERNAME : "" ); // 收款方户名
        $('#PayeeBank').val( TransferType == "InLineFlag" ? "中国农业银行" : top.API.BankName ); // 收款方行方
    }

    function onKeyPressed(key, keyCode) {
        var tmpInputdata = "";
        $('#TransferTip').text('');
        if (((0 == key || 00 == key) && bFirstKey == true) && Inputdata.length < 9) {
            top.API.displayMessage("第一个数字不能为0");
        } else if (0 <= key || key <= 9 || 00 == key) {
            tmpInputdata = Inputdata;
            tmpInputdata += key;
            if (parseInt(tmpInputdata) > 1000000 ) {
                $('#TransferTip').text('输入的金额不能大于100万！');
            } else {
                Inputdata = tmpInputdata;
                bFirstKey = false;
            }
            top.API.displayMessage("onKeyPressed-----" + tmpInputdata);
            $('#TransferMoney').val(top.InsertChar(Inputdata, 3, ','));
        } else if (key == "CLEAR") {
            top.API.displayMessage("onKeyPressed-----CLEAR");
            ReInput();
        } else if (key == "CANCEL") {
            top.API.displayMessage("onKeyPressed-----CANCEL");
            document.getElementById('Exit').onclick();
        } else if (key == "ENTER") {
            top.API.displayMessage("onKeyPressed-----ENTER");
            // document.getElementById('OK').onclick();
        }
    }

    function ReInput() {
        ButtonEnable();
        bFirstKey = true;
        Inputdata = "";
        $('#TransferMoney').val(Inputdata);
    }

    // 调整页面结构样式,调出滚动条
    function changeLaout(){
        $("#MAIN",parent.document).height(1400).addClass("main-sroll");
        $('#PageContent').scrollTop( $('#PageContent').height() );
        $('#PageId').scrollTop( 1400 );
        $('body').scrollTop( 1400 );
        $('html').scrollTop( 1400 );
    }

    TextAreaDom.onclick = function () {
        changeLaout();
        currentId = TextAreaDom.id;
        //点击 textarea框 调出输入法键盘
        //document.getElementById("Check-tip").style.display = "none";

        App.Plugin.ImeHM.getCurPosition(currentId);
        showFlag = 1;
        //显示某种格式的输入法
        top.Ime.ShowIME('0', '', '');
    };

    PayeeNameDom.onclick = function () {
        if( TransferType == "OutLineFlag"){
            changeLaout();
            currentId = PayeeNameDom.id;
            //点击 textarea框 调出输入法键盘
            //document.getElementById("Check-tip").style.display = "none";

            App.Plugin.ImeHM.getCurPosition(currentId);
            showFlag = 1;
            //显示某种格式的输入法
            top.Ime.ShowIME('0', '', '');
        }
        
    };

    // PayeeBankDom.onclick = function () {
    //     currentId = PayeeBankDom.id;
    //     //点击 textarea框 调出输入法键盘
    //     //document.getElementById("Check-tip").style.display = "none";

    //     App.Plugin.ImeHM.getCurPosition(currentId);
    //     showFlag = 1;
    //     //显示某种格式的输入法
    //     top.Ime.ShowIME('0', '', '');
    // };


    //点击页面其他处 隐藏键盘
    document.onclick = function (e) {
        //点击 textarea框 调去输入法键盘
        e = e || window.event;
        var dom = e.srcElement || e.target;
        if (dom.id != currentId && showFlag == 1) {
            $("#MAIN",parent.document).height(1024).removeClass("main-sroll");
            top.Ime.HideIME('');
        }

    };

    function onDeviceError() {
        top.API.displayMessage("onDeviceError触发");
        top.ErrorInfo = top.API.PromptList.No4;
        return CallResponse("Exit");
    }


    function onShowIMECompleted() {
        showFlag = 1;
    }

    function onHideIMECompleted() {
        showFlag = 0;
    }

    function onInputResult(type, value) {
        if (value == "CONFIRM" || value == "CANCEL") {
            $("#MAIN",parent.document).height(1024).removeClass("main-sroll");
            top.Ime.HideIME('');
        } else {
            App.Plugin.ImeHM.getInput(currentId, type, value);
        }
    }


    //Register the event
    function EventLogin() {
        //键盘事件
        top.Pin.KeyPressed.connect(onKeyPressed);
        top.Pin.DeviceError.connect(onDeviceError);

        //输入法事件
        top.Ime.ShowIMECompleted.connect(onShowIMECompleted);
        top.Ime.HideIMECompleted.connect(onHideIMECompleted);
        top.Ime.InputResult.connect(onInputResult);

    }

    function EventLogout() {
        top.Pin.KeyPressed.disconnect(onKeyPressed);
        top.Pin.DeviceError.disconnect(onDeviceError);

        //输入法事件
        top.Ime.ShowIMECompleted.disconnect(onShowIMECompleted);
        top.Ime.HideIMECompleted.disconnect(onHideIMECompleted);
        top.Ime.InputResult.disconnect(onInputResult);

    }

    //Countdown function
    function TimeoutCallBack() {
        top.ErrorInfo = top.API.PromptList.No3;
        $("#MAIN",parent.document).height(1024).removeClass("main-sroll");
        top.Ime.HideIME('');
        return CallResponse('TimeOut');
    }

    //remove all event handler
    function Clearup() {
        //TO DO:
        EventLogout();
        top.Pin.CancelGetData();
        App.Plugin.Voices.del();
        App.Timer.ClearTime();
    }
})();