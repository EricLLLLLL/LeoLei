;(function () {
    var bCdmReset = false,//�Ƿ�����cdmReset����
        bResetSuccess = false,//�Ƿ�λ�ɹ�
        setIntervalId = null,
        bResetFailGoOn,
        CallResponse = App.Cntl.ProcessOnce(function (Response) {
            //TO DO:
            top.Sys.ItemClearSync(top.API.MTRN_TRANSACTIONDIFFER);
            Clearup();
            //Entry the flows control process.
            App.Cntl.ProcessDriven(Response);
        }),
        Initialize = function () {
            EventLogin();
            if (top.API.partCwcTip) {
                $('#partCwcTip').text('�������˳ɹ�,���˽�' + top.API.CashInfo.strTransAmount + '.00Ԫ');
            } else {
                $('#partCwcTip').text('��������ʧ��');
            }
            resetFn(); // ��λ
            bResetFailGoOn = top.Dat.GetPrivateProfileSync("TransactionConfig", "bResetFailGoOn", "0", top.API.gIniFileName);
            App.Timer.TimeoutDisposal(TimeoutCallBack);
        }();//Page Entry

    //@User ocde scope start
    document.getElementById('Exit').onclick = function () {
        top.API.displayMessage("���Exit��");
        return CallResponse('Exit');
    }

    document.getElementById('OK').onclick = function () {
        top.API.depErrFlag = true; // ȡ���
        return CallResponse('Exit');
    }

    function resetFn(){
        if( top.API.resetFlag ){
            top.API.displayMessage("���OK����ʼReset��");
            bCdmReset = false;
            App.Plugin.Wait.show();
            top.Cdm.Reset("RETRACT", 0, top.API.gResetTimeout);
            $('#OK').css('display', 'none');   
        }
        top.API.gTakeCardAndPrint = true; //���ܳɹ���񶼴�ӡƾ��
    }

    function onResetStatus() {
        top.API.displayMessage("onResetStatus");
        if (bCdmReset) {
            //clearTimeout(timeoutId);
            //bCimReset = false;
            bCdmReset = false;
            App.Plugin.Wait.disappear();
            if (bResetSuccess && top.Cdm.StOutputStatus() == "NOTEMPTY") {
                top.API.displayMessage("StOutputStatus --- NOTEMPTY");
                $("#Exit").css('display', 'block');
                top.Cdm.OpenShutter(top.API.gOpenShutterTimeOut);
            } else {
                top.API.displayMessage("��λʧ�ܣ�����ϵ���д���");
                $('#warning').css('display', 'block');
                $('#warning').text('��λʧ�ܣ�����ϵ���д���');
                App.Timer.SetPageTimeout(8);
                //return CallResponse('OK');
            }
        }
    }

    //door
    function onShutterOpened() {
        $('#warning').css('display', 'block');
        $('#warning').text('��ע����㷵���Ľ�����������������������ϵ��');
        App.Timer.SetPageTimeout(60);
        setIntervalId = setInterval(function () {
            App.Plugin.Voices.play("voi_7");
        }, 8000);
        top.API.displayMessage("onShutterOpened����");
    }

    function onShutterOpenFailed() {
        top.API.displayMessage("onShutterOpenFailed����");
        //showInfo();
        $('#warning').css('display', 'block');
        $('#warning').text('��λʧ�ܣ�����ϵ���д���');
        App.Timer.SetPageTimeout(8);
        //return CallResponse('OK');
    }

    function onCashTaken() {
        clearInterval(setIntervalId);
        top.Cdm.CloseShutter(top.API.gCloseShutterTimeOut);
    }

    function onShutterClosed() {
        top.API.displayMessage("onShutterClosed����");
    }

    function onShutterCloseFailed() {
        top.API.displayMessage("onShutterCloseFailed����");
    }

    //��λ�¼���ʼ
    // function onCimResetComplete() {
    //     bResetSuccess = true;
    //     top.API.displayMessage("onCimResetComplete is done");
    //     bCimReset = true;
    //     onResetStatus();
    // }
    //
    // function onCimResetFailed() {
    //     top.API.displayMessage("onCimResetFailed is done");
    //     if(bResetFailGoOn == '1'){
    //         bResetSuccess = true;
    //     }
    //     bCimReset = true;
    //     onResetStatus();
    // }

    function onCdmResetComplete() {
        bResetSuccess = true;
        top.API.displayMessage("onCdmResetComplete is done");
        bCdmReset = true;
        onResetStatus();
    }

    function onCdmResetFailed() {
        top.API.displayMessage("onCdmResetFailed is done");
        if (bResetFailGoOn == '1') {
            bResetSuccess = true;
        } else if (bResetFailGoOn = '2') {
            bResetSuccess = false;
        }
        bCdmReset = true;
        onResetStatus();
    }

    // function onCimDeviceError() {
    //     top.API.displayMessage("onCimDeviceError is done");
    //     bCdmReset = true;
    //     onResetStatus();
    // }

    function onCdmDeviceError() {
        top.API.displayMessage("onCdmDeviceError is done");
        bCdmReset = true;
        onResetStatus();
    }

    function EventLogin() {
        //top.Cim.ResetComplete.connect(onCimResetComplete);
        //top.Cim.ResetFailed.connect(onCimResetFailed);
        top.Cdm.ResetComplete.connect(onCdmResetComplete);
        top.Cdm.ResetFailed.connect(onCdmResetFailed);
        //top.Cdm.StatusChanged.connect(onCdmStatusChanged);
        //top.Cim.DeviceError.connect(onCimDeviceError);
        top.Cdm.DeviceError.connect(onCdmDeviceError);

        //door
        top.Cdm.ShutterOpened.connect(onShutterOpened);
        top.Cdm.ShutterOpenFailed.connect(onShutterOpenFailed);
        top.Cdm.ShutterClosed.connect(onShutterClosed);
        top.Cdm.ShutterCloseFailed.connect(onShutterCloseFailed);

        //���չ���
        //top.Cim.SafeDoorClosed.connect(onSafeDoorClosed);
        //top.Cim.SafeDoorOpened.connect(onSafeDoorOpened);

        top.Cdm.CashTaken.connect(onCashTaken);
    }

    function EventLogout() {

        //top.Cim.ResetComplete.disconnect(onCimResetComplete);
        //top.Cim.ResetFailed.disconnect(onCimResetFailed);
        top.Cdm.ResetComplete.disconnect(onCdmResetComplete);
        top.Cdm.ResetFailed.disconnect(onCdmResetFailed);
        //top.Cdm.StatusChanged.disconnect(onCdmStatusChanged);
        //top.Cim.DeviceError.disconnect(onCimDeviceError);
        top.Cdm.DeviceError.disconnect(onCdmDeviceError);

        //door
        top.Cdm.ShutterOpened.disconnect(onShutterOpened);
        top.Cdm.ShutterOpenFailed.disconnect(onShutterOpenFailed);
        top.Cdm.ShutterClosed.disconnect(onShutterClosed);
        top.Cdm.ShutterCloseFailed.disconnect(onShutterCloseFailed);

        //���չ���
        //top.Cim.SafeDoorClosed.disconnect(onSafeDoorClosed);
        //top.Cim.SafeDoorOpened.disconnect(onSafeDoorOpened);

        top.Cdm.CashTaken.disconnect(onCashTaken);
    }

    //Countdown function
    function TimeoutCallBack() {
        return CallResponse('TimeOut');
    }

    //remove all event handler
    function Clearup() {
        //TO DO:
        EventLogout();
        App.Timer.ClearTime();
    }
})();
