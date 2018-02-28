;(function() {
    var $cardNum  = $(".cardNum"),
        $name     = $(".name"),
        $money    = $(".money"),
        $errTip   = $(".errTip"),
        $saveTime = $(".saveTime"),
        $saveToType = $(".saveToType"),
        $saveToTime = $(".saveToTime"),

        cardNum = top.API.gCardno,  // �˻�
        name    = top.API.gCustomerName,  // ����
        money   = 0;  // ���

    var Inputdata = "";
    var EnterKey = false;
    var bFirstKey = true;
        
    var CallResponse = App.Cntl.ProcessOnce(function(Response) {
        //TO DO:
        Clearup();
        top.Siu.SetPinPadLight('OFF');
        //Entry the flows control process.
        App.Cntl.ProcessDriven(Response);
    }),
    Initialize = function() {
        EventLogin();
        //@initialize scope start

        // ��ʼֵ����
        $cardNum.html( top.changeCardNum(cardNum) ); // ����
        $name.html( top.changeName(name) );  //  ����

        App.Timer.TimeoutDisposal(TimeoutCallBack);
        top.Pin.GetData(0, false, "0,1,2,3,4,5,6,7,8,9,ENTER,CLEAR,CANCEL", "CANCEL", -1);
        top.Siu.SetPinPadLight('CONTINUOUS');
    }(); //Page Entry


    function ButtonDisable() {
        document.getElementById('Exit').disabled = true;
        document.getElementById('OK').disabled = true;

    }

    function ButtonEnable() {
        document.getElementById('Exit').disabled = false;
        document.getElementById('OK').disabled = false;
    }
    //@User ocde scope start
    document.getElementById('Exit').onclick = function() {

        top.ErrorInfo = top.API.PromptList.No2;
        return CallResponse('Exit');
    }

    document.getElementById('OK').onclick = function() {
        
        // return CallResponse('OK');
        top.API.displayMessage("���OK��ť");
        EnterKey = true;
        $errTip.html("");
        if (Inputdata == "") {
            $errTip.html("���׽���Ϊ�գ�");
            Reinput();
        } else {
            getData();
        }
    }

    $saveToType.change(function(){
        if( $saveToType.val() == "1" ){
            $(".saveToTimeWrap").show();
        }else{ $(".saveToTimeWrap").hide(); }
    });

    //@User code scope end

    function getData(){
        if( parseInt(Inputdata) > parseInt(top.API.balance) ){
            $errTip.html("���׽����ڿ��������������룡");
            Reinput();
        }else if( parseInt(Inputdata) < 50 ){
            $errTip.html("���׽���С��50Ԫ�����������룡");
            Reinput();
        }else{
            top.API.CTRMoney = Inputdata;
            // top.API.svaeTime = $(".svaeTime option:selected").text();
            // top.API.saveToType = $(".saveToType option:selected").text();
            // top.API.saveToTime = $(".saveToTime option:selected").text();
            // top.API.svaeTime = $(".saveTime").val();
            // top.API.saveToType = $saveToType.val();
            // top.API.saveToTime = $(".saveToTime").val();

            // [���׽��]����
			var tmp = Inputdata + "00";
			var arrINPUTMONEY = new Array(tmp);

			top.Dat.SetDataSync(top.API.transamountTag, top.API.transamountType, arrINPUTMONEY);
            
            // [����]����
            var arrSAVETIME = $saveTime.val();
            switch ( arrSAVETIME ) {
                case "M006":
                    top.API.saveTime = "������";
                    break;
                case "Y001":
                    top.API.saveTime = "һ��";
                    break;
                case "Y003":
                    top.API.saveTime = "����";
                    break;
                case "Y005":
                    top.API.saveTime = "����";
                    break;
                default:
                    top.API.saveTime = "������";
                    break;
            }
            top.Dat.SetDataSync("DEPOSIT", "STRING", [arrSAVETIME]);

            // [ת������]����
            var arrSAVETOTYPE = $saveToType.val();
            top.API.saveToType = arrSAVETOTYPE == "1" ? "Լ��ת��" : "��Լ��ת��";
            top.Dat.SetDataSync("DEALTYPEFLAG", "STRING", [arrSAVETOTYPE]);

            // [Լ��ת�����]����
            if(arrSAVETOTYPE != "1"){
                var arrSAVETOTIME = "��";
            }else{
                var arrSAVETOTIME = $saveToTime.val();
                switch ( arrSAVETOTIME ) {
                    case "M003":
                        top.API.saveToTime = "������";
                        break;
                    case "M006":
                        top.API.saveToTime = "������";
                        break;
                    case "Y001":
                        top.API.saveToTime = "һ��";
                        break;
                    case "Y003":
                        top.API.saveToTime = "����";
                        break;
                    case "Y005":
                        top.API.saveToTime = "����";
                        break;
                    default:
                        top.API.saveToTime = "";
                        break;
                }
            }
            
            top.Dat.SetDataSync("AGREEMENT", "STRING", [arrSAVETOTIME]);


            // top.API.displayMessage("saveTime="+top.API.saveTime +"--saveToType="+top.API.saveToType+"--saveToTime="+top.API.saveToTime);
            // top.API.displayMessage("balance="+top.API.balance);
            // top.API.displayMessage("Inputdata="+Inputdata);

            
            top.API.gTransactiontype = 'QRYEXECUTERATE';  // ��ѯ��ת������

            return CallResponse('OK');
        }
    }
    //����������
    function Reinput() {
        ButtonEnable();
        EnterKey = false;
        bFirstKey = true;
        Inputdata = "";
        //document.getElementById("CNMoney").innerText = Inputdata;
        $money.val(Inputdata);
    }

    function onKeyPressed(key, keyCode) {
        var tmpInputdata = "";
        $errTip.html("");
        if (((0 == key || 00 == key) && bFirstKey == true) && Inputdata.length < 9) {
            top.API.displayMessage("��һ�����ֲ���Ϊ0");
        } else if (0 <= key || key <= 9 || 00 == key) {
            tmpInputdata = Inputdata;
            tmpInputdata += key;
            // if (tmpInputdata.length == 7) {
            //     $errTip.html("");
            // } else if ((parseInt(tmpInputdata, 10) > 10000) && (top.API.CashInfo.Dealtype == "����ȡ��")) {
            //     $errTip.html("����ȡ��ɴ���10000Ԫ");
            // } else {
                Inputdata = tmpInputdata;
                bFirstKey = false;
            // }
            // document.getElementById("CNMoney").innerText = top.InsertChar(Inputdata, 3, ',');
            $money.val( top.InsertChar(Inputdata, 3, ',') );
            // document.getElementById("CNMoney").innerText = top.cmycurd(Inputdata);
        } else if (key == "CLEAR") {
            Reinput();
        } else if (key == "CANCEL") {
            Reinput();
            // document.getElementById('Exit').onclick();
        } else if (key == "ENTER") {
            if (!EnterKey) {
                EnterKey = true;
                // document.getElementById('OK').onclick();
            }
        }
    }
    function onDeviceError() {
        top.API.displayMessage("onDeviceError����");
        top.ErrorInfo = top.API.PromptList.No4;
        return CallResponse("Exit");
    }


    //Register the event
    function EventLogin() {
        top.Pin.KeyPressed.connect(onKeyPressed);
        top.Pin.DeviceError.connect(onDeviceError);

    }

    function EventLogout() {
        top.Pin.KeyPressed.disconnect(onKeyPressed);
        top.Pin.DeviceError.disconnect(onDeviceError);

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
        top.Pin.CancelGetData();
        App.Timer.ClearTime();
    }
})();