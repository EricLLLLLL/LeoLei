/*@create by:  tsxiong
 *@time: 2016年03月20日
 *@modify by: LeoLei
 *@modify time :2016年09月07日
 */
;(function () {
    var Inputdata = "";
    var bFirstKey = true;
    var EnterKey = false;
    var lRetAuthorized;
    var lRetBigTranLimit;
    var CallResponse = App.Cntl.ProcessOnce(function (Response) {
        //TO DO:
        top.Siu.SetPinPadLight('OFF');
        Clearup();
        App.Cntl.ProcessDriven(Response);
    });
    var Initialize = function () {
        //SYS 判断是否可取零钱
        top.API.gMixSelfCWD = false;//初始化零钞标志
        top.API.gbAmountType = true;
        if (top.API.IsOntherCashCWD()) {
            document.getElementById('EXCWD').style.display = "block";
        }
        lRetAuthorized = top.Sys.DataGetSync(top.API.MTRN_AUTHORIZEDAMOUNTRULE);
        top.API.displayMessage("MTRN_AUTHORIZEDAMOUNTRULE,授权限额,lRetAuthorized=" + lRetAuthorized);
        //考虑零钞限额
        var tmplRetBigTranLimit = top.Dat.GetDataSync("POSSIBLEDISPENSE100AMOUNT", "LONG").toArray()[0];
        var TransStatus = top.Sys.PossibleTransactionSync();
        var arrTransStatus = TransStatus.split(",");
        var BIG_CARD_flag = parseInt(arrTransStatus[10]);
        if (BIG_CARD_flag != 1) {
            lRetBigTranLimit = (tmplRetBigTranLimit <= 50000) ? tmplRetBigTranLimit : 49999;
        } else {
            lRetBigTranLimit = tmplRetBigTranLimit;
        }
        top.API.displayMessage("MTRN_TRANLIMITAMOUNTREAL,交易限额,lRetBigTranLimit=" + lRetBigTranLimit);

        ShowInfo();  //add by grb for 存折取款
        App.Timer.TimeoutDisposal(TimeoutCallBack);
        ButtonDisable();
        EventLogin();
        //@initialize scope start
        top.Pin.GetData(0, false, "0,1,2,3,4,5,6,7,8,9,ENTER,CLEAR,CANCEL", "CANCEL", -1);
        top.Siu.SetPinPadLight('CONTINUOUS');
        ButtonEnable();
        App.Plugin.Voices.play("voi_12");
    }();//Page Entry

    //@User ocde scope start
    //重新输入金额
    function Reinput() {
        ButtonEnable();
        EnterKey = false;
        bFirstKey = true;
        Inputdata = "";
        document.getElementById("NumMoney").innerText = Inputdata;
        document.getElementById("CNMoney").innerText = Inputdata;
    }

    function ButtonDisable() {
        document.getElementById('Normal_l1').disabled = true;
        document.getElementById('Normal_l2').disabled = true;
        document.getElementById('Normal_l3').disabled = true;
        document.getElementById('Normal_r1').disabled = true;
        document.getElementById('Normal_r2').disabled = true;
        document.getElementById('Normal_r3').disabled = true;
        document.getElementById('Exit').disabled = true;
        document.getElementById('OK').disabled = true;
        document.getElementById('Back').disabled = true;
        document.getElementById('EXCWD').disabled = true;

    }

    function ButtonEnable() {
        document.getElementById('Normal_l1').disabled = false;
        document.getElementById('Normal_l2').disabled = false;
        document.getElementById('Normal_l3').disabled = false;
        document.getElementById('Normal_r1').disabled = false;
        document.getElementById('Normal_r2').disabled = false;
        document.getElementById('Normal_r3').disabled = false;
        document.getElementById('Exit').disabled = false;
        document.getElementById('OK').disabled = false;
        document.getElementById('Back').disabled = false;
        document.getElementById('EXCWD').disabled = false;
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

    document.getElementById('EXCWD').onclick = function () {
        top.API.gMixSelfCWD = true;
        ButtonDisable();
        return CallResponse('EXCWD');
    }


    document.getElementById('OK').onclick = function () {
        top.API.displayMessage("点击OK按钮");
        EnterKey = true;
        document.getElementById('ErrTip').InnerText = "";
        if (Inputdata == "") {
            document.getElementById("ErrTip").innerText = "输入不能为空";
            Reinput();
        } else {
            var UnitMaxAmount = lRetBigTranLimit;
            var nAmount = parseInt(Inputdata, 10);
            if (UnitMaxAmount < nAmount || (nAmount % 100 != 0)) {
                document.getElementById("ErrTip").innerText = "超出设备可取金额";
                Reinput();
            } else {
                var CWDMoney = 0;
                if (top.API.CashInfo.Dealtype == "存折取款") {
                    var nRet = top.Sys.DataSetSync(top.API.MTRN_TRANSACTIONDIFFER, top.API.MTRN_WITHDRAW_PB);
                    top.API.displayMessage("Trn DataSetSync MTRN_TRANSACTIONDIFFER MTRN_WITHDRAW_PB 折取款 Return:" + nRet);
                    CWDMoney = parseInt(top.Sys.DataGetSync(top.API.MTRN_TRANLIMITAMOUNTRULE).toString());
                } else if (top.API.CashInfo.Dealtype == "CWD取款") {
                    var nRet = top.Sys.DataSetSync(top.API.MTRN_TRANSACTIONDIFFER, top.API.MTRN_WITHDRAW_CARD);
                    top.API.displayMessage("Trn DataSetSync MTRN_TRANSACTIONDIFFER MTRN_WITHDRAW_CARD 卡取款 Return:" + nRet);
                    CWDMoney = parseInt(top.Sys.DataGetSync(top.API.MTRN_TRANLIMITAMOUNTRULE).toString());
                }
                if ((nAmount + top.API.gnCWDMoney) > CWDMoney && top.API.gbOrderCWD == false) {
                    document.getElementById("ErrTip").innerText = "个人客户日累计取款限额为" + CWDMoney + "元，您的本次取款金额已超出剩余可取现额度，请重新输入";
                    Reinput();
                } else {
                    if ((nAmount + top.API.gnCWDMoney) < lRetAuthorized || top.API.CashInfo.Dealtype == "存折取款" ) {
                        top.API.gbAmountType = true;
                    } else {
                        top.API.gbAmountType = false;  // 大额
                    }
                    SetDealData();
                }
            }
        }
    }

    function SetDealData() {
        top.API.CashInfo.strTransAmount = Inputdata;
        var tmp = Inputdata + "00";
        var arrINPUTMONEY = new Array(tmp);
        top.Dat.SetDataSync(top.API.transamountTag, top.API.transamountType, arrINPUTMONEY);
        top.API.gTransactiontype = "CWD";
        if (top.API.gbAmountType == true) {
            if (top.API.CashInfo.Dealtype == "存折取款") {
                TslFunction("BOOKCWD");
                top.API.gTslChooseJnlType = "0108";
                return CallResponse('PassBookCWD');
            } else {
                TslFunction("SCWD");
                top.API.gTslChooseJnlType = "0108";
                if (top.API.gbOrderCWD == true) {
                    return CallResponse('PassBookCWD');
                } else {
                    return CallResponse('OK');
                }
            }
        } else {
            TslFunction("BCWD");
            top.API.gTslChooseJnlType = "0108";
            var nAmount = parseInt(Inputdata, 10);
            if (Inputdata < 50000) {
                return CallResponse('NeedCheck');
            } else {
                return CallResponse('LargeDeposit');
            }
        }

    }

    document.getElementById('Normal_l1').onclick = function () {
        top.API.displayMessage("点击1万元/100按钮");
        ButtonDisable();
        document.getElementById('ErrTip').InnerText = "";
        if (top.API.CashInfo.Dealtype == "存折取款") {
            Inputdata = "100";
        } else {
            Inputdata = "10000";
        }
        document.getElementById("NumMoney").innerText = top.InsertChar(Inputdata, 3, ',');
        document.getElementById("CNMoney").innerText = top.cmycurd(Inputdata);
        EnterKey = true;
        document.getElementById('OK').onclick();
    }
    document.getElementById('Normal_l2').onclick = function () {
        top.API.displayMessage("点击2万元/500按钮");
        ButtonDisable();
        document.getElementById('ErrTip').InnerText = "";
        if (top.API.CashInfo.Dealtype == "存折取款") {
            Inputdata = "500";
        } else {
            Inputdata = "20000";
        }
        document.getElementById("NumMoney").innerText = top.InsertChar(Inputdata, 3, ',');
        document.getElementById("CNMoney").innerText = top.cmycurd(Inputdata);
        EnterKey = true;
        document.getElementById('OK').onclick();
    }
    document.getElementById('Normal_l3').onclick = function () {
        top.API.displayMessage("点击3万元/1000按钮");
        ButtonDisable();
        document.getElementById('ErrTip').InnerText = "";
        if (top.API.CashInfo.Dealtype == "存折取款") {
            Inputdata = "1000";
        } else {
            Inputdata = "30000";
        }
        document.getElementById("NumMoney").innerText = top.InsertChar(Inputdata, 3, ',');
        document.getElementById("CNMoney").innerText = top.cmycurd(Inputdata);
        EnterKey = true;
        document.getElementById('OK').onclick();
    }
    document.getElementById('Normal_r1').onclick = function () {
        top.API.displayMessage("点击4万元/2000按钮");
        ButtonDisable();
        document.getElementById('ErrTip').InnerText = "";
        if (top.API.CashInfo.Dealtype == "存折取款") {
            Inputdata = "2000";
        } else {
            Inputdata = "40000";
        }
        document.getElementById("NumMoney").innerText = top.InsertChar(Inputdata, 3, ',');
        document.getElementById("CNMoney").innerText = top.cmycurd(Inputdata);
        EnterKey = true;
        document.getElementById('OK').onclick();
    }
    document.getElementById('Normal_r2').onclick = function () {
        top.API.displayMessage("点击5万元/5000按钮");
        ButtonDisable();
        document.getElementById('ErrTip').InnerText = "";
        if (top.API.CashInfo.Dealtype == "存折取款") {
            Inputdata = "5000";
        } else {
            Inputdata = "50000";
        }
        document.getElementById("NumMoney").innerText = top.InsertChar(Inputdata, 3, ',');
        document.getElementById("CNMoney").innerText = top.cmycurd(Inputdata);
        EnterKey = true;
        document.getElementById('OK').onclick();
    }
    document.getElementById('Normal_r3').onclick = function () {
        top.API.displayMessage("点击10万元/10000按钮");
        ButtonDisable();
        document.getElementById('ErrTip').InnerText = "";
        if (top.API.CashInfo.Dealtype == "存折取款") {
            Inputdata = "10000";
        } else {
            Inputdata = "100000";
        }
        document.getElementById("NumMoney").innerText = top.InsertChar(Inputdata, 3, ',');
        document.getElementById("CNMoney").innerText = top.cmycurd(Inputdata);
        EnterKey = true;
        document.getElementById('OK').onclick();
    }

    function ShowInfo() {
        if (top.API.CashInfo.Dealtype == "存折取款") {
            if (lRetBigTranLimit >= 10000) {
                document.getElementById("Normal_r3").style.display = "block";
            }
            if (lRetBigTranLimit >= 5000) {
                document.getElementById("Normal_r2").style.display = "block";
            }
            if (lRetBigTranLimit >= 2000) {
                document.getElementById("Normal_r1").style.display = "block";
            }
            if (lRetBigTranLimit >= 1000) {
                document.getElementById("Normal_l3").style.display = "block";
            }
            if (lRetBigTranLimit >= 500) {
                document.getElementById("Normal_l2").style.display = "block";
            }
            if (lRetBigTranLimit >= 100) {
                document.getElementById("Normal_l1").style.display = "block";
            }
            document.getElementById("Normal_l1").innerText = "100";
            document.getElementById("Normal_l2").innerText = "500";
            document.getElementById("Normal_l3").innerText = "1000";
            document.getElementById("Normal_r1").innerText = "2000";
            document.getElementById("Normal_r2").innerText = "5000";
            document.getElementById("Normal_r3").innerText = "10000";
        } else {
            if (lRetBigTranLimit >= 100000) {
                document.getElementById("Normal_r3").style.display = "block";
            }
            if (lRetBigTranLimit >= 50000) {
                document.getElementById("Normal_r2").style.display = "block";
            }
            if (lRetBigTranLimit >= 40000) {
                document.getElementById("Normal_r1").style.display = "block";
            }
            if (lRetBigTranLimit >= 30000) {
                document.getElementById("Normal_l3").style.display = "block";
            }
            if (lRetBigTranLimit >= 20000) {
                document.getElementById("Normal_l2").style.display = "block";
            }
            if (lRetBigTranLimit >= 10000) {
                document.getElementById("Normal_l1").style.display = "block";
            }
            document.getElementById("Normal_l1").innerText = "1万";
            document.getElementById("Normal_l2").innerText = "2万";
            document.getElementById("Normal_l3").innerText = "3万";
            document.getElementById("Normal_r1").innerText = "4万";
            document.getElementById("Normal_r2").innerText = "5万";
            document.getElementById("Normal_r3").innerText = "10万";
        }
    }


    //@User code scope end 
    function onKeyPressed(key, keyCode) {
        var tmpInputdata = "";
        document.getElementById('ErrTip').innerText = "";
        if (((0 == key || 00 == key) && bFirstKey == true) && Inputdata.length < 9) {
            top.API.displayMessage("第一个数字不能为0");
        } else if (0 <= key || key <= 9 || 00 == key) {
            tmpInputdata = Inputdata;
            tmpInputdata += key;
            if (tmpInputdata.length == 7) {
                document.getElementById('ErrTip').innerText = "";
            } else if ((parseInt(tmpInputdata, 10) > 10000) && (top.API.CashInfo.Dealtype == "存折取款")) {
                document.getElementById('ErrTip').innerText = "存折取款不可大于10000元";
            } else {
                Inputdata = tmpInputdata;
                bFirstKey = false;
            }
            document.getElementById("NumMoney").innerText = top.InsertChar(Inputdata, 3, ',');
            document.getElementById("CNMoney").innerText = top.cmycurd(Inputdata);
        } else if (key == "CLEAR") {
            Reinput();
        } else if (key == "CANCEL") {
            document.getElementById('Exit').onclick();
        } else if (key == "ENTER") {
            if (!EnterKey) {
                EnterKey = true;
                document.getElementById('OK').onclick();
            }
        }
    }


    //event handler
    function onDeviceError() {
        top.API.displayMessage("onDeviceError触发");
        top.ErrorInfo = top.API.PromptList.No4;
        return CallResponse("Exit");
    }

    function TslFunction(type) {
        var myData = top.GetDate12byte();
        top.API.gTslDate = myData.substr(0, 8);
        top.API.gTslTime = myData.substr(8, 6);
        top.API.gTslChooseType = type;
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
        top.Jnl.PrintSync("PageTimeOut");
        top.ErrorInfo = top.API.PromptList.No3;
        return CallResponse('TimeOut');
    }

    //Page Return

    //remove all event handler
    function Clearup() {
        //TO DO:
        EventLogout();
        top.Pin.CancelGetData();
        App.Plugin.Voices.del();
        App.Timer.ClearTime();
    }
})();
