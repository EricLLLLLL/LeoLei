;
(function () {
    var Inputdata = '',
        TextAreaDom = document.getElementById("postScript"),
        PayeeNameDom = document.getElementById("PayeeName"),
        PayeeBankDom = document.getElementById("PayeeBank"),
        TransferType = top.API.CashInfo.Dealtype, // ����ת�� InLineFlag������ת�� OutLineFlag
        InputDomArray = new Array(),
        bFirstKey = true,
        showFlag = 0,//����0����ʾ 1 ��ʾ
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
        top.API.displayMessage("���Exit");
        ButtonDisable();
        top.ErrorInfo = top.API.PromptList.No2;
        if (showFlag == 1) {
            //�������뷨
            $("#MAIN",parent.document).height(1024).removeClass("main-sroll"); // ����ҳ��ṹ�߶ȣ����ع�����
            top.Ime.HideIME('');
        }
        return CallResponse('Exit');
    };

    document.getElementById('Back').onclick = function () {
        top.API.displayMessage("���Back");
        ButtonDisable();
        if (showFlag == 1) {
            //�������뷨
            $("#MAIN",parent.document).height(1024).removeClass("main-sroll");
            top.Ime.HideIME('');
        }
        return CallResponse('Back');
    };

    document.getElementById('OK').onclick = function () {
        top.API.displayMessage("���OK");
        if (showFlag == 1) {
            //�������뷨
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
            $('#TransferTip').text('ת�˽���Ϊ�գ�������ת�˽�');
        } else if (radioValue == null) {
            $('#TransferTip').text('ת�˷�ʽ����Ϊ�գ���ѡ��ת�˷�ʽ��');
        } else if (strlen > 39) {
            $('#TransferTip').text('���븽���������ޣ����������룡');
        } else if (TransferType == "OutLineFlag" &&( PayeeName == null || PayeeName == "")) {
            $('#TransferTip').text('�տ��������Ϊ�գ��������տ������');
        } else {
            top.API.CommentSelect = $("#CommentSelect").find("option:selected").text();
            top.API.postScript = $("#postScript").text();
            top.API.PayeeName = PayeeName;
            top.API.PayeeBank = PayeeBank;
            top.API.TransferMoney = Inputdata;
            top.API.transferWay = radioValue;
            top.API.displayMessage("CommentSelect:" + top.API.CommentSelect + ";postScript:" + top.API.postScript + ";TransferMoney:" + Inputdata);
            
            top.API.MONEYLIMIT = parseInt(top.API.TransferMoney) >= 50000 ? "0" : "1";
            top.Dat.SetDataSync("MONEYLIMIT", "STRING", [top.API.MONEYLIMIT]); // ���ô�С���ʶ�� 0��� 1С��

            top.Dat.SetDataSync("TRANSFERWAY", "STRING", [top.API.transferWay]); // ����ת�˷�ʽ��ʶ 0ʵʱ 1��ͨ 2����

            ButtonDisable();
            // top.API.gTransactiontype = 'Transfer';
        
            if( TransferType == "InLineFlag" ){
                return CallResponse('InLineFlag');
            }else{
                top.API.gTransactiontype = 'QRYFEE'; // ����ת�˲�������
                return CallResponse('OutLineFlag');
            }
        }
    };

    //@User code scope end
    function showInfo() {
        var INCUSTOMERNAME = top.Dat.GetDataSync("INCUSTOMERNAME", "STRING").toArray()[0];
        $('#PayeeAccount').val(top.API.PayeeAccount); // �տ�˺�
        $('#PayeeName').val( TransferType == "InLineFlag" ? INCUSTOMERNAME : "" ); // �տ����
        $('#PayeeBank').val( TransferType == "InLineFlag" ? "�й�ũҵ����" : top.API.BankName ); // �տ�з�
    }

    function onKeyPressed(key, keyCode) {
        var tmpInputdata = "";
        $('#TransferTip').text('');
        if (((0 == key || 00 == key) && bFirstKey == true) && Inputdata.length < 9) {
            top.API.displayMessage("��һ�����ֲ���Ϊ0");
        } else if (0 <= key || key <= 9 || 00 == key) {
            tmpInputdata = Inputdata;
            tmpInputdata += key;
            if (parseInt(tmpInputdata) > 1000000 ) {
                $('#TransferTip').text('����Ľ��ܴ���100��');
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

    // ����ҳ��ṹ��ʽ,����������
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
        //��� textarea�� �������뷨����
        //document.getElementById("Check-tip").style.display = "none";

        App.Plugin.ImeHM.getCurPosition(currentId);
        showFlag = 1;
        //��ʾĳ�ָ�ʽ�����뷨
        top.Ime.ShowIME('0', '', '');
    };

    PayeeNameDom.onclick = function () {
        if( TransferType == "OutLineFlag"){
            changeLaout();
            currentId = PayeeNameDom.id;
            //��� textarea�� �������뷨����
            //document.getElementById("Check-tip").style.display = "none";

            App.Plugin.ImeHM.getCurPosition(currentId);
            showFlag = 1;
            //��ʾĳ�ָ�ʽ�����뷨
            top.Ime.ShowIME('0', '', '');
        }
        
    };

    // PayeeBankDom.onclick = function () {
    //     currentId = PayeeBankDom.id;
    //     //��� textarea�� �������뷨����
    //     //document.getElementById("Check-tip").style.display = "none";

    //     App.Plugin.ImeHM.getCurPosition(currentId);
    //     showFlag = 1;
    //     //��ʾĳ�ָ�ʽ�����뷨
    //     top.Ime.ShowIME('0', '', '');
    // };


    //���ҳ�������� ���ؼ���
    document.onclick = function (e) {
        //��� textarea�� ��ȥ���뷨����
        e = e || window.event;
        var dom = e.srcElement || e.target;
        if (dom.id != currentId && showFlag == 1) {
            $("#MAIN",parent.document).height(1024).removeClass("main-sroll");
            top.Ime.HideIME('');
        }

    };

    function onDeviceError() {
        top.API.displayMessage("onDeviceError����");
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
        //�����¼�
        top.Pin.KeyPressed.connect(onKeyPressed);
        top.Pin.DeviceError.connect(onDeviceError);

        //���뷨�¼�
        top.Ime.ShowIMECompleted.connect(onShowIMECompleted);
        top.Ime.HideIMECompleted.connect(onHideIMECompleted);
        top.Ime.InputResult.connect(onInputResult);

    }

    function EventLogout() {
        top.Pin.KeyPressed.disconnect(onKeyPressed);
        top.Pin.DeviceError.disconnect(onDeviceError);

        //���뷨�¼�
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