/*@create by:  tsxiong
 *@time: 2016��03��20��
 */
;
(function() {
    var DeviceErrTimes = 0;
    var ReadIdCardFlag;
    var CallResponse = App.Cntl.ProcessOnce(function(Response) {
        //TO DO:
        top.Idr.CancelAccept();
        top.Siu.SetScannerLight('OFF');
        Clearup();
        App.Cntl.ProcessDriven(Response);
    });
    var Initialize = function() {
        EventLogin();

        // test by Gni
        // top.API.gIdNumber = "432503199109287690";
        // top.Dat.SetDataSync('IDNUMBER','STRING', [top.API.gIdNumber]);
        
        // $("#Tip").show();
        // App.Plugin.Voices.play("voi_50");
        //App.Timer.SetPageTimeout(7);
        // setTimeout(function(){
        //     return CallResponse('OK');  
        // },5000);
        
        //@initialize scope start
        App.Timer.TimeoutDisposal(TimeoutCallBack);
        ButtonDisable();
        if (top.API.CashInfo.Dealtype == "�޿����۴��") {
            document.getElementById("MLimit2").innerText = "1";
        } else if (top.API.CashInfo.Dealtype == "CancelAccount"){
            PeriodicCA();
        }
        ReadIdCardFlag = top.API.gReadIdCardFlag;
        if (ReadIdCardFlag == 2) {
            document.getElementById("lab_title").innerText = "�뽫���������֤�ڷ���ָ��λ��";
            document.getElementById("AgentTip").style.display = "block";
            document.getElementById("OK").style.display = "block";
        } else {
            if (top.API.CashInfo.Dealtype == "�޿����۴��" ||
                top.API.CashInfo.Dealtype == "TransferCancellation" ||
                top.API.CashInfo.Dealtype == "TransferRemittance") {
                document.getElementById("lab_title").innerText = "�뽫���֤�ڷ���ָ��λ��";
            } else if(top.API.CashInfo.Dealtype == "CancelAccount") {
                document.getElementById("lab_title").innerText = "�뽫���֤�ڷ���ָ��λ��";
                document.getElementById("lab_tip").innerText = "����������ͷ";
            } else {
                document.getElementById("lab_title").innerText = "�뽫���֤�ڷ���ָ��λ��";
            }
            top.Idr.AcceptAndRead(-1);
        }
        top.Siu.SetScannerLight('QUICK');
        ButtonEnable();
        App.Plugin.Voices.play("voi_5");
    }(); //Page Entry

    //@User ocde scope start
    function ButtonDisable() {
        document.getElementById('Exit').disabled = true;
        document.getElementById('OK').disabled = true;
        document.getElementById('Back').disabled = true;
    }

    function ButtonEnable() {
        document.getElementById('Exit').disabled = false;
        document.getElementById('OK').disabled = false;
        document.getElementById('Back').disabled = false;
    }

    document.getElementById('Exit').onclick = function() {
        ButtonDisable();
        top.ErrorInfo = top.API.PromptList.No2;
        return CallResponse('Exit');
    }

    document.getElementById('Back').onclick = function() {
        ButtonDisable();
        return CallResponse('Back');
    }

    document.getElementById('OK').onclick = function() {
        top.Idr.AcceptAndRead(-1);
        document.getElementById("OK").style.display = "none";
    }

    // ��ת������ ---- Gni
    function PeriodicCA(){
        $("#TipDiv").hide();
    }

    //@User code scope end 

    //event handler
    function onCardInserted() {
        document.getElementById('err-tip').innerText = "";
        document.getElementById('AgentErrTip').style.display = "none";
        //alert("onCardInserted");
    }

    //event handler
    function onCardAccepted(info) {
        /*info=
         info[0]xxx
         ,info[1]��
         ,info[2]����
         ,info[3]19930929
         ,info[4]450xxxxxxxxxxxxxxx
         ,info[5]�㶫����
         ,info[6]�����ַ־�
         ,info[7]20130820
         ,info[8]20230820
         ,info[9]D:\DATA\HeadImage.bmp
         ,info[10]D:\DATA\FrontImage.jpg
         ,info[11]D:\DATA\BackImage.jpg*/
        ButtonDisable();
        top.API.gstrIdCardInfo = info;
        var arrIDNO = new Array();
        arrIDNO = info.split(",");
        top.API.gIdCardpic = arrIDNO[9];
        top.API.gIdName = arrIDNO[0].toString().replace(/\s+/g, "");
        top.API.gIdNation = arrIDNO[2];
        top.API.gIdSex = arrIDNO[1];
        top.API.gIdNumber = arrIDNO[4];

        if (arrIDNO[0] == "") {
            DeviceErrTimes++;
            if (DeviceErrTimes == 10) {
                top.ErrorInfo = top.API.PromptList.No7;
                return CallResponse('Exit');
            } else {
                document.getElementById('err-tip').innerText = "��ȡ���֤��Ϣʧ�ܣ������¸�Ӧ��";
                ButtonEnable();
            }
        } else {
            var sDate = top.GetDate12byte().substring(0, 8);
            if (parseInt(sDate) - parseInt(arrIDNO[3]) < 160000) {
                document.getElementById('err-tip').innerText = "����δ��16���꣬���������֤����";
                ButtonEnable();
            } else if (parseInt(arrIDNO[8]) < parseInt(sDate)) {
                document.getElementById('err-tip').innerText = "�����֤�ѹ��ڣ����������֤����";
                ButtonEnable();
            } else {
                if (ReadIdCardFlag == 1) { //�����˰���
                    //������������ˢ�������֤���Ƿ��жϻ���һ��
                    var IsIDCardNameJudge = top.Dat.GetPrivateProfileSync("TestConfig", "IsIDCardNameJudge", "1", top.API.gIniFileName);
                    if ("0" == IsIDCardNameJudge) {
                        top.API.gIdNumberForRisk = top.API.gIdNumber;

                        $("#Tip").show();
                        App.Plugin.Voices.play("voi_50");
                        App.Timer.SetPageTimeout(7);
                        setTimeout(function(){
                            return CallResponse('OK');  
                        },5000);

                    } else {
                        if (top.API.gIdName == top.API.gCustomerName) {
                            top.API.gIdNumberForRisk = top.API.gIdNumber;
                            
                            $("#Tip").show();
                            App.Plugin.Voices.play("voi_50");
                            App.Timer.SetPageTimeout(7);
                            setTimeout(function(){
                                return CallResponse('OK');  
                            },5000);

                        } else {
                            document.getElementById('AgentErrTip').style.display = "block";
                            ButtonEnable();
                            return;
                        }
                    }
                } else if (ReadIdCardFlag == 2) { //�����˰���ҵ��,���������֤��ȡ�ɹ�
                    return CallResponse('Audit');
                } else { //���˰���
                    if (top.API.gIdName == top.API.gCustomerName ||
                        top.API.CashInfo.Dealtype == "�޿����۴��" ||
                        // top.API.CashInfo.Dealtype == "TransferCancellation" ||
                        top.API.CashInfo.Dealtype == "TransferRemittance" ||
                        top.API.CashInfo.Dealtype == "CancelAccount") { //�������֤��ȡ�ɹ�
                        top.API.gIdNumberForRisk = top.API.gIdNumber;  
                        top.Dat.SetDataSync('IDNUMBER','STRING', [top.API.gIdNumber]);

                        $("#Tip").show();
                        App.Plugin.Voices.play("voi_50");
                        App.Timer.SetPageTimeout(7);
                        setTimeout(function(){
                            return CallResponse('OK');  
                        },5000);

                    } else {
                        var IsSameName = top.Dat.GetPrivateProfileSync("AssConfig", "IsSameName", "0", top.API.gIniFileName);
                        if (IsSameName == "0") {
                            top.API.gIdNumberForRisk = top.API.gIdNumber;
                            top.API.gIdName = top.API.gCustomerName;

                            $("#Tip").show();
                            App.Plugin.Voices.play("voi_50");
                            App.Timer.SetPageTimeout(7);
                            setTimeout(function(){
                                return CallResponse("OK");
                            },5000);

                        } else {
                            document.getElementById('AgentErrTip').style.display = "block";
                            ButtonEnable();
                            return;
                        }
                    }
                }
            }

        }
    }

    function onCardTaken() {
        document.getElementById('err-tip').innerText = "";
        document.getElementById('AgentErrTip').style.display = "none";
        top.Idr.AcceptAndRead(-1);
    }

    function onTimeout() {
        top.API.displayMessage("onTimeout");
    }

    function onDeviceError() {
        if (top.Idr.StDetailedDeviceStatus() != "ONLINE") {
            top.ErrorInfo = top.API.PromptList.No7;
        } else {
            DeviceErrTimes++;
            if (DeviceErrTimes == 10) {
                top.ErrorInfo = "��ȡ����֤��Ϣʧ�ܣ����׽���";
                return CallResponse('Exit');
            } else {
                document.getElementById('err-tip').innerText = "��ȡ���֤��Ϣʧ�ܣ������¸�Ӧ��";
            }
        }

    }

    function onCardAcceptFailed() {
        top.API.displayMessage("onCardAcceptFailed");
        if (top.Idr.StDetailedDeviceStatus() != "ONLINE") {
            top.ErrorInfo = top.API.PromptList.No7;
        } else {
            DeviceErrTimes++;
            if (DeviceErrTimes == 10) {
                top.ErrorInfo = "��ȡ����֤��Ϣʧ�ܣ����׽���";
                return CallResponse('Exit');
            } else {
                document.getElementById('err-tip').innerText = "��ȡ���֤��Ϣʧ�ܣ������¸�Ӧ��";
            }
        }

    }

    function onCardInvalid() {
        DeviceErrTimes++;
        if (DeviceErrTimes == 10) {
            top.ErrorInfo = "��ȡ����֤��Ϣʧ�ܣ����׽���";
            return CallResponse('Exit');
        } else {
            document.getElementById('err-tip').innerText = "��ȡ���֤��Ϣʧ�ܣ������¸�Ӧ��";
        }
    }

    //Register the event
    function EventLogin() {
        top.Idr.CardInserted.connect(onCardInserted);
        top.Idr.CardAccepted.connect(onCardAccepted);
        top.Idr.CardAcceptFailed.connect(onCardAcceptFailed);
        top.Idr.CardTaken.connect(onCardTaken);
        top.Idr.DeviceError.connect(onDeviceError);
        top.Idr.Timeout.connect(onTimeout);
        top.Idr.CardInvalid.connect(onCardInvalid);
    }

    function EventLogout() {
        top.Idr.CardInserted.disconnect(onCardInserted);
        top.Idr.CardAccepted.disconnect(onCardAccepted);
        top.Idr.CardAcceptFailed.disconnect(onCardAcceptFailed);
        top.Idr.CardTaken.disconnect(onCardTaken);
        top.Idr.Timeout.disconnect(onTimeout);
        top.Idr.DeviceError.disconnect(onDeviceError);
        top.Idr.CardInvalid.disconnect(onCardInvalid);
    }

    //Countdown function
    function TimeoutCallBack() {
        top.ErrorInfo = top.API.PromptList.No3;
        return CallResponse('TimeOut');
    }

    //Page Return

    //remove all event handler
    function Clearup() {
        //TO DO:
        EventLogout();
        App.Plugin.Voices.del();
        App.Timer.ClearTime();
    }
})();