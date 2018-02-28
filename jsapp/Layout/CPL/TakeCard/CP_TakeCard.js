/*@create by:  tsxiong
 *@time: 2016��03��20��
 */
;(function () {
    var bHaveCard = true;
    var warnTimer = 0;
    var soundObj;
    var CallResponse = App.Cntl.ProcessOnce(function (Response) {
        //TO DO:
        ClearWarning();
        top.Siu.SetCardReaderLight('OFF');
        Clearup();
        App.Cntl.ProcessDriven(Response);
    });
    var Initialize = function () {
        EventLogin();
        //д��TslLog
        if (top.API.gTslFlag && top.API.gTslResult != "SUCCESS") {
            //������|ʱ��|����|��ˮ��|��������|���|���֤��|���֤ͷ��·��|���׽����
            var TslLog = top.API.gTslDate;
            TslLog += "|" + top.API.gTslTime;
            TslLog += "|" + top.API.gCardno;
            TslLog += "|" + top.API.gTslJnlNum;
            TslLog += "|" + top.API.gTslChooseType;
            TslLog += "|" + top.API.gTslMoneyCount;
            TslLog += "|" + top.API.gIdNumber;
            TslLog += "|" + top.API.gIdCardpic;
            TslLog += "|" + top.API.gTslResult;
            top.Tsl.AddTransLogSync(TslLog);
            //�ն˺ţ�8λ�����������ڣ�8λ��������ʱ�䣨6λ�����������ͣ�4λ��0107�����0108����ȡ���
            //�ʺţ�19λ�������׽�10λ������λС��λ�����豸��ˮ�ţ�6λ�����豸��ˮ���κţ�6λ����
            //��̨�����루2λ������̨������ˮ�ţ�12λ�����豸����״̬��2λ��00�����׳ɹ���01�����쳣���ף����쳣״̬���ͣ�4λ��
            var strUpJnl = top.API.gTerminalID;
            strUpJnl += "|!" + top.API.gTslDate;
            strUpJnl += "|!" + top.API.gTslTime;
            strUpJnl += "|!" + top.API.gTslChooseJnlType;
            strUpJnl += "|!" + top.API.gCardno;
            strUpJnl += "|!" + top.API.gTslMoneyCount.replace(".", "");
            strUpJnl += "|!" + top.API.gTslJnlNum;
            strUpJnl += "|!" + top.API.gTslJnlBtn;
            strUpJnl += "|!" + top.API.gTslResponsecode;
            strUpJnl += "|!" + top.API.gTslSysrefnum;
            strUpJnl += "|!01|!����ʧ��";
            // top.Tsl.CreateUpJnlFile(strUpJnl); //
            top.Tsl.HandleRecordFileSync(top.API.MTSL_WRITECHECKTRANSRECORD, strUpJnl);
        }
        //@initialize scope start
        document.getElementById("tip_label").innerText = top.ErrorInfo;
        document.getElementById("tip_label1").style.display = "block";
        document.getElementById("tip_button").style.display = "block";

        // ����ƾ���ֶ�
        switch (top.API.CashInfo.Dealtype){
            case "PeriodicCurrentAccount":
            case "CurrentToRegular":
                top.Dat.SetDataSync("DEALTYPE", "STRING", ["���ת"]);
                setPrintForm();
                break;
            case "InLineFlag":
                top.Dat.SetDataSync("DEALTYPE", "STRING", ["����ת��"]);
                setPrintForm();
                break;
            case "OutLineFlag":
                top.Dat.SetDataSync("DEALTYPE", "STRING", ["����ת��"]);
                setPrintForm();
                break;
            case "TransferCancellation":
                top.Dat.SetDataSync("DEALTYPE", "STRING", ["ת�˳���"]);
                setPrintForm();
                break;
            default:
                top.API.displayMessage(top.API.gTakeCardAndPrint);
                if (top.API.needPrintReDEPCash && top.Ptr.bDeviceStatus && top.API.gNoPtrSerFlag == false) {
                    top.Ptr.Print("Receipt_ReDEPCash_szABC", "", top.API.gPrintTimeOut);
                    top.API.needPrintReDEPCash = false;
                }else if (top.API.gTakeCardAndPrint == true) {
                    if (!top.API.gbPartCashIn && top.Ptr.bDeviceStatus && top.API.gNoPtrSerFlag == false) {
                        top.Ptr.Print("ReceiptCash_Print_szABC", "", top.API.gPrintTimeOut);//comment by grb
                    }
                }
                break;
        }
        if (top.ErrorInfo == top.API.PromptList.No1) {
            document.getElementById("tip_button").style.backgroundImage = "url('Framework/style/Graphics/box_ico_3.png')";
        } else {
            document.getElementById("tip_button").style.backgroundImage = "url('Framework/style/Graphics/box_ico_2.png')";
        }
        
        if ("PRESENT" == top.Crd.StMediaStatus()) {
            PlayVoice(bHaveCard);
            warnTimer = window.setTimeout(function () {
                playWarning()
            }, 15000);
            App.Timer.SetPageTimeout(60);
            App.Timer.TimeoutDisposal(TimeoutCallBack);
            top.Crd.Eject(top.API.gEjectCardTimeOut);
            document.getElementById("tip_label2").style.display = "block";
        } else {
            bHaveCard = false;
            PlayVoice(bHaveCard);
            var iniRet = top.Dat.GetPrivateProfileSync("TransactionConfig", "CWCSupport", "0", top.API.gIniFileName);
            if ((top.API.CashInfo.Dealtype == "����ȡ��") && iniRet == 1) {
                top.Dat.GetPersistentData(top.API.cwcflagTag, top.API.cwcflagType);
            } else {
                App.Timer.SetPageTimeout(8);
                App.Timer.TimeoutDisposal(TimeoutCallBack);
            }
        }
        //��ȡ���׽��

    }();//Page Entry
    //��������
    function playWarning() {
        top.API.displayMessage("15���δȡ����������ʾ");
        var obj = document.createElement("bgsound");
        obj.id = "ModuleSound";
        obj.src = "Framework/Plugin/Voice/Sound/Forget_Beep_New.wav";
        obj.autoplay = true;
        obj.loop = "-1";
        soundObj = obj;
        document.body.appendChild(soundObj);

    }

    function ClearWarning() {
        if (warnTimer != 0) {
            window.clearTimeout(warnTimer);
            if (soundObj != null) {
                document.body.removeChild(soundObj);
            }
            warnTimer = 0;
        }
    }

    // ת�˽���ʧ�ܹ���ƾ��
    function setPrintForm(){

        top.Dat.SetDataSync("POUNDAGE", "STRING", ["0.00"]);
        if (top.API.gTakeCardAndPrint == true) {
            if (top.Ptr.bDeviceStatus && top.API.gNoPtrSerFlag == false) {
                top.Ptr.Print("ReceiptCash_Print_szABC", "", top.API.gPrintTimeOut);// ��ӡƾ��
            }
        }
        

    }

    function PlayVoice(bCard) {
        switch (top.ErrorInfo) {
            case top.API.PromptList.No1:
                if (bCard) {
                    App.Plugin.Voices.play("voi_29");
                } else {
                    App.Plugin.Voices.play("voi_33");
                }
                break;
            case top.API.PromptList.No2:
                if (bCard) {
                    App.Plugin.Voices.play("voi_19");
                } else {
                    App.Plugin.Voices.play("voi_30");
                }
                break;
            case top.API.PromptList.No3:
                if (bCard) {
                    App.Plugin.Voices.play("voi_17");
                } else {
                    App.Plugin.Voices.play("voi_18");
                }
                break;
            case top.API.PromptList.No4:
                if (bCard) {
                    App.Plugin.Voices.play("voi_23");
                } else {
                    App.Plugin.Voices.play("voi_23");
                }
                break;
            case top.API.PromptList.No5:
                if (bCard) {
                    App.Plugin.Voices.play("voi_25");
                } else {
                    App.Plugin.Voices.play("voi_24");
                }
                break;
            case top.API.PromptList.No6:
                if (bCard) {
                    App.Plugin.Voices.play("voi_2");
                }
                break;
            case top.API.PromptList.No7:
                if (bCard) {
                    App.Plugin.Voices.play("voi_31");
                }
                break;
            case top.API.PromptList.No8:
                if (bCard) {
                    //App.Plugin.Voices.play("voi_32");
                }
                break;
            default:
                App.Plugin.Voices.play("voi_2");
                break;
        }
    }


    function onCardEjected() {
        top.Siu.SetCardReaderLight('SLOW');
    }

    function onCardTaken() {
        top.Jnl.PrintSync("TakeCardAction");
        ClearWarning();
        //�ж��Ƿ����
        var iniRet = top.Dat.GetPrivateProfileSync("TransactionConfig", "CWCSupport", "0", top.API.gIniFileName);
        if (top.API.CashInfo.Dealtype == "CWDȡ��" && (iniRet == 1)) {
            bHaveCard = false;
            top.Dat.GetPersistentData(top.API.cwcflagTag, top.API.cwcflagType);
        } else {
            return CallResponse("Exit");
        }
    }

    function onDeviceError() {
        top.API.displayMessage("DeviceError");
        return CallResponse("Exit");
    }

    function onCardEjectFailed() {
        top.API.displayMessage("onCardEjectFailed");
        top.ErrorInfo = top.API.PromptList.No4;
        return CallResponse('Exit');
    }

    function onDatGetPersistentDataComplete(DataName, DataType, DataValue) {
        if ('CWCFLAG' == DataName) {
            var flag_CWC = DataValue.toArray();
            if (flag_CWC[0] === 1) {
                if (top.API.CashInfo.Dealtype == "CWDȡ��" || top.API.CashInfo.Dealtype == "����ȡ��") {
                    top.API.gTransactiontype = "CWC";
                    //��ת֮ǰ���ó�����־λΪ0
                    var arrCWCFlag = new Array();
                    arrCWCFlag[0] = 0;
                    top.Dat.SetPersistentData(top.API.cwcflagTag, top.API.cwcflagType, arrCWCFlag);
                }
            } else {
                return CallResponse("Exit");
            }
        }
    }

    function onDatGetPersistentDataError(DataName, ErrorCode) {
        return CallResponse("Exit");
    }

    function onDatSetPersistentDataComplete(DataName) {
        if ('CWCFLAG' == DataName) {
            return CallResponse("CWC");
        }
    }

    function onDatSetPersistentDataError(DataName, ErrorCode) {
        top.API.displayMessage("DatSetPersistentDataError");
        return CallResponse("Exit");
    }

    //Register the event
    function EventLogin() {
        top.Crd.CardEjected.connect(onCardEjected);
        top.Crd.CardEjectFailed.connect(onCardEjectFailed);
        top.Crd.CardTaken.connect(onCardTaken);
        top.Crd.DeviceError.connect(onDeviceError);
        top.Dat.GetPersistentDataComplete.connect(onDatGetPersistentDataComplete);
        top.Dat.GetPersistentDataError.connect(onDatGetPersistentDataError);
        top.Dat.SetPersistentDataComplete.connect(onDatSetPersistentDataComplete);
        top.Dat.SetPersistentDataError.connect(onDatSetPersistentDataError);
    }

    function EventLogout() {
        top.Crd.CardEjected.disconnect(onCardEjected);
        top.Crd.CardEjectFailed.disconnect(onCardEjectFailed);
        top.Crd.CardTaken.disconnect(onCardTaken);
        top.Crd.DeviceError.disconnect(onDeviceError);
        top.Dat.GetPersistentDataComplete.disconnect(onDatGetPersistentDataComplete);
        top.Dat.GetPersistentDataError.disconnect(onDatGetPersistentDataError);
        top.Dat.SetPersistentDataComplete.disconnect(onDatSetPersistentDataComplete);
        top.Dat.SetPersistentDataError.disconnect(onDatSetPersistentDataError);
    }

    //Countdown function
    function TimeoutCallBack() {
        if (bHaveCard) {
            top.API.displayMessage("��ʱ�̿�");
            return CallResponse("Capture");
        } else {
            return CallResponse("TimeOut");
        }
    }

    //Page Return

    //remove all event handler
    function Clearup() {
        EventLogout();
        App.Plugin.Voices.del();
        App.Timer.ClearTime();
    }

    //@User ocde scope start

})();