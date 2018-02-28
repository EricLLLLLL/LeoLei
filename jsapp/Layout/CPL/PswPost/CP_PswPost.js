/*@create by:  tsxiong
*@time: 2016��03��20��
*/
; (function () {
    var TransType = null;
    var keyFlag = 0; //���PINKEY��MACKEY��־λ��1��PINKEY   2��MACKEY
    var NextPageFlag = 0; //��ˮ��ת��־λ��1��OK   2��TradeFailed
    var Pinkey = "";
    var MACkey = "";
    var strErrMsg = "���׳ɹ�";
    var Check = "";
    var JnlNum = null; //�����ȡ������ˮ�ű���
    var bReInput = false;
    var timeoutFlag = false; //�Ƿ��ѳ�ʱ��ҳ�泬ʱ��ͨѶ��ʱ����־λ
	var CallResponse = App.Cntl.ProcessOnce( function(Response){
        //TO DO:
        Clearup();
        //Entry the flows control process.
        App.Cntl.ProcessDriven(Response);
    });
    var Initialize = function () {
        EventLogin();
        App.Plugin.Voices.play("voi_28");
        App.Timer.TimeoutDisposal(TimeoutCallBack);
        top.ErrorInfo = top.API.PromptList.No1;
        SetCashUnitInfoToTCP();

        TransType = "PWD";
		top.API.gTransactiontype = "PWD"
        var arrOldPsw = new Array(top.API.gOldPinBlock);
        var arrNewPsw = new Array(top.API.gNewPinBlock);
        //top.API.gCardno = strCardNo;
        var nRet;
        nRet = top.Dat.SetDataSync("OLDPINBLOCK", "STRING", arrOldPsw);
        top.API.displayMessage("SetDataSync arrOldPsw Return:" + nRet); 
        nRet = top.Dat.SetDataSync("NEWPINBLOCK", "STRING", arrNewPsw);
        top.API.displayMessage("SetDataSync arrNewPsw Return:" + nRet);
        //��ȡ��ǰ������ˮ�ţ���ȡ����ֵ��1���õ�JnlNum������
        var nRet1 = top.Dat.GetPersistentData(top.JnlnumTag, top.JnlnumType);
        top.API.displayMessage("GetPersistentData JNLNUM Return:" + nRet1);
    } (); //Page Entry

    //@User ocde scope start

    //@User code scope end 
    function SetCashUnitInfoToTCP() {
        var strCounts = "";
        var strCurrency = "";
        var strInfo = "";
        for (i = 0; i < top.API.CashInfo.nCountOfUnits; i++) {
            switch (top.API.CashInfo.arrUnitCurrency[i]) {
                case 100:
                    strCurrency = "15650100";
                    break;
                case 50:
                    strCurrency = "15650050";
                    break;
                case 20:
                    strCurrency = "15650020";
                    break;
                case 10:
                    strCurrency = "15650010";
                    break;
                case 5:
                    strCurrency = "15650005";
                    break;
                case 1:
                    strCurrency = "15650001";
                    break;
                default:
                    strCurrency = "00000000";
                    break;
            }
            switch (top.API.CashInfo.arrUnitRemain[i].toString().length) {
                case 1:
                    strCounts = "000" + top.API.CashInfo.arrUnitRemain[i].toString();
                    break;
                case 2:
                    strCounts = "00" + top.API.CashInfo.arrUnitRemain[i].toString();
                    break;
                case 3:
                    strCounts = "0" + top.API.CashInfo.arrUnitRemain[i].toString();
                    break;
                case 4:
                    strCounts = top.API.CashInfo.arrUnitRemain[i].toString();
                    break;
                default:
                    strCounts = "0000";
                    break;
            }
            strInfo = strInfo + strCurrency + strCounts;
        }
        var arrDeviceState = new Array(strInfo);
        var nRet = top.Dat.SetDataSync("DEVICESTATE", "STRING", arrDeviceState);
        top.API.displayMessage("SetDataSync DEVICESTATE Return:" + nRet);
    }
    //event handler
    /********************************************************************************************************/
    //FlowControl
    function TradeFailed() {
        top.API.displayMessage('TradeFailed()');
        if (strErrMsg != "���׳ɹ�") {
            top.ErrorInfo = strErrMsg + "!";
        }
        /*if (bReInput) {
        var ReInputFlag = new Array();
        ReInputFlag[0] = 1;
        var nRet = top.Dat.SetDataSync(top.API.inputpasswordtimesTag, top.API.inputpasswordtimesType, ReInputFlag);
        top.API.displayMessage("SetDataSync ReInputFlag Return:" + nRet);
        }
        if (TransType == "CWC")
        {
        return CallResponse("CWC");
        }else{
        return CallResponse("TradeFailed");
        }  
        */
        return CallResponse("Exit");
    }

    function JournalPrint() {
        if (NextPageFlag == 1) {
            return CallResponse("OK");
        }

        if (NextPageFlag == 2) {
            TradeFailed();
        }
        /*var tmpstrErrMsg = "����ʧ��";
        if (Check == "00") {
        tmpstrErrMsg = "���׳ɹ�";
        }
        switch (TransType) {
        case "DEP":
        var i;                   
        //��ȡ������
        var Poundage = "0.00";
        var objGet3= top.Dat.GetDataSync(top.API.poundageTag, top.API.poundageType);                    
        if (null == objGet3) {
        top.API.displayMessage("GetDataSync poundage objGet = null");
        }
        else {                        
        top.API.displayMessage("�����ѣ� " + objGet3.toArray());
        if (objGet3.toArray()[0].length > 2) {
        Poundage = objGet3.toArray()[0].substr(0,(objGet3.toArray()[0].length-2));
        Poundage  += ".";
        Poundage  += objGet3.toArray()[0].substr((objGet3.toArray()[0].length-2),2);
        }else if (objGet3.toArray()[0].length == 2) {
        Poundage = "0."+ objGet3.toArray()[0];
        }else if (objGet3.toArray()[0].length == 1) {
        Poundage = "0.0"+ objGet3.toArray()[0];
        }
        }
        top.API.gPoundage = Poundage;
        //��ȡ�����
        var Amount = top.API.CashInfo.strTransAmount + ".00";
        top.Jnl.PrintSync("CashInBox", "JnlNum=" + JnlNum + ", TransKind=�����, RetCode=" + Check
        + ", ProResult=" + tmpstrErrMsg + ", AmountMoney=" + Amount + ", Poundage=" + Poundage 
        + ", RMB100=" + top.API.CashInfo.arrCurrencyCashIn[0] + ", RMB50=" + top.API.CashInfo.arrCurrencyCashIn[1] 
        + ", RMB20=" + top.API.CashInfo.arrCurrencyCashIn[2] + ", RMB10=" + top.API.CashInfo.arrCurrencyCashIn[3] 
        + ", RMB5=" + top.API.CashInfo.arrCurrencyCashIn[4] + ", RMB1=" + top.API.CashInfo.arrCurrencyCashIn[5] 
        + ", iBOX1=" + top.API.CashInfo.arrAcceptorCount[0] + ", iBOX2=" + top.API.CashInfo.arrAcceptorCount[1] 
        + ", iBOX3=" + top.API.CashInfo.arrAcceptorCount[2] + ", iBOX4=" + top.API.CashInfo.arrAcceptorCount[3] 
        + ", iBOX5=" + top.API.CashInfo.arrAcceptorCount[4] + ", iBOX6=" + top.API.CashInfo.arrAcceptorCount[5] 
        + ", iBOX7=" + top.API.CashInfo.arrAcceptorCount[6]);
        break;
        case "CWD":    
        var i;       
        //��ȡ������
        var Poundage = "0.00";
        var objGet3= top.Dat.GetDataSync(top.API.poundageTag, top.API.poundageType);                    
        if (null == objGet3) {
        top.API.displayMessage("GetDataSync poundage objGet = null");
        }
        else {                        
        top.API.displayMessage("�����ѣ� " + objGet3.toArray());
        if (objGet3.toArray()[0].length > 2) {
        Poundage = objGet3.toArray()[0].substr(0,(objGet3.toArray()[0].length-2));
        Poundage  += ".";
        Poundage  += objGet3.toArray()[0].substr((objGet3.toArray()[0].length-2),2);
        }else if (objGet3.toArray()[0].length == 2) {
        Poundage = "0."+ objGet3.toArray()[0];
        }else if (objGet3.toArray()[0].length == 1) {
        Poundage = "0.0"+ objGet3.toArray()[0];
        }
        }
        top.API.gPoundage = Poundage;
        //��ȡȡ���ܶ�
        var Amount = top.API.CashInfo.strTransAmount + ".00";
        top.Jnl.PrintSync("CashOutBox1", "JnlNum=" + JnlNum + ", TransKind=��ȡ��, RetCode=" + Check + ", ProResult=" + tmpstrErrMsg + ", AmountMoney=" + Amount + ", Poundage=" + Poundage);
        break;
        case "INQ":
        top.Jnl.PrintSync("CardQuery", "JnlNum=" + JnlNum + ", TransKind=����ѯ, RetCode=" + Check + ", ProResult=" + tmpstrErrMsg);
        break;
        case "QRYCUSTNAME":
        top.Jnl.PrintSync("QueryAccInfo", "JnlNum=" + JnlNum + ", TransKind=�黧��, RetCode=" + Check + ", ProResult=" + tmpstrErrMsg);
        break;
        case "CWC":
        top.Jnl.PrintSync("WithdrawCorrect", "JnlNum=" + JnlNum + ", TransKind=ȡ�����, RetCode=" + Check + ", ProResult=" + tmpstrErrMsg);
        break;
        default:
        if (NextPageFlag == 1) {
        if (top.API.CashInfo.Dealtype == "����ȡ��") {
        return CallResponse("EXCWD");
        }else{
        return CallResponse("OK");
        }                        
        }
        if (NextPageFlag == 2) {
        TradeFailed();
        }
        break;
        }*/
    }
    /********************************************************************************************************/
    //��� 
    function onCompositionDataCompleted(arrData) {
        top.API.displayMessage("onCompositionDataCompleted is done");
        var objArrData = arrData.toArray();
        top.Pin.GenerateMAC(objArrData, "MACKEY", '', 0);
    }

    function onCompositionDataFail() {
        top.API.displayMessage("onCompositionDataFail is done");
        strErrMsg = "ͨѶʧ�ܣ����׽���";
        NextPageFlag = 2;
        JournalPrint();
    }

    function onMACGenerated(MacData) {
        top.API.displayMessage("onMACGenerated is done");
        var objMacData = MacData.toArray();
        top.Tcp.SendToHost(objMacData, 60000);
    }
    /********************************************************************************************************/
    //TCPģ��    
    function onTcpSendFailed() {
        top.API.displayMessage("onTcpSendFailed is done");
        strErrMsg = "ͨѶʧ�ܣ����׽���";
        NextPageFlag = 2;
        JournalPrint();
    }

    function onTcpOnRecved(tmpCheck) {
        Check = tmpCheck;
        top.API.gResponsecode = tmpCheck;
        top.API.displayMessage("onTcpOnRecved is done,Check:" + Check);
        var bSuccess = false;
        //var ErrMsg = null;
        //var nRet;
        switch (Check) {
            case '00':

                /*if (CheckKey()) {
                DownPinKey();
                } else {
                NextPageFlag = 1;
                JournalPrint();
                }*/               
                bSuccess = true;
                strErrMsg = "���׳ɹ�";
				NextPageFlag = 1;
                JournalPrint();                
                break;
            case '01':
                strErrMsg = "�������δ�����,����ϵ����"; //"��ѯ��������";
                break;
            case '04':
                strErrMsg = "���п��쳣,��������"; //"û�տ�";
                break;
            case '12':
                strErrMsg = "�ݲ��ܰ������ҵ��"; //"��Ч����";
                break;
            case '13':
                strErrMsg = "��Ч�Ľ��׽��"; //"��Ч���";
                break;
            case '14':
                strErrMsg = "�ÿ���֧��"; //"��Ч����";
                break;
            case '15':
                strErrMsg = "����ȡ��,���뷢������ϵ"; //"�޴˷�����";
                break;
            case '30':
                strErrMsg = "�������δ�����,����ϵ����!"; //"���ױ��Ĵ�";
                break;
            case '33':
                strErrMsg = "���ѹ���,��������"; //"���ڵĿ�";
                break;
            case '38':
                strErrMsg = "������󳬴���"; //"���������PIN�����루û�տ���";
                break;
            case '41':
                strErrMsg = "�˻��Ѷ���,��������"; //"��ʧ����û�տ���";
                break;
            case '43':
                strErrMsg = "�Ƿ��˻�,��������"; //"���Կ���û�տ���";
                break;
            case '51':
                strErrMsg = "����"; //"�ʽ���";
                break;
            case '54':
                strErrMsg = "���ѹ���"; //"���ڵĿ�";
                break;
            case '55':
                bReInput = true;
                strErrMsg = "����������벻��ȷ";
                break;
            case '65':
                strErrMsg = "�ѳ����տ�ȡ�����"; //"����ȡ�ִ�������";
                break;
            case '75':
                strErrMsg = "������󳬴���"; //"���������PIN��������";
                break;
            case '89':
                strErrMsg = "��Ч�ն�"; //"��Ч�ն�";
                break;
            case '90':
                strErrMsg = "�������δ�����"; //"�����л����ڴ���";
                break;
            case '91':
                strErrMsg = "�������δ�����,����ϵ����"; //"�������򽻻����Ĳ��ܲ���";
                break;
            case '96':
                strErrMsg = "�������δ�����,����ϵ����"; //"ϵͳ�쳣��ʧЧ";
                break;
            default:
                strErrMsg = "�������δ�����,����ϵ����"; //"�����쳣";
                break;
        };
        if (!bSuccess) {
            NextPageFlag = 2;
            JournalPrint();
        }
    }

    function onTcpTimeout() {
        top.API.displayMessage("onTcpTimeout is done,TransType=" + TransType);
        if (!timeoutFlag) {
            timeoutFlag = true;
            strErrMsg = "ͨѶ��ʱ�����׽���";
            if (TransType == "CWD") {
                var arrCWCFlag = new Array();
                arrCWCFlag[0] = 1;
                var nRet1 = top.Dat.SetPersistentData(top.API.cwcflagTag, top.API.cwcflagType, arrCWCFlag);
                top.API.displayMessage('������־λ��SetPersistentData CWCFLAG Return:' + nRet1);
            } else {
                NextPageFlag = 2;
                JournalPrint();
            }
        }
    }

    function onAnalysisFailed() {
        top.API.displayMessage("onAnalysisFailed is done,TransType=" + TransType);
        strErrMsg = "���Ľ���ʧ�ܣ����׽���";
        if (TransType == "CWD") {
            var arrCWCFlag = new Array();
            arrCWCFlag[0] = 1;
            var nRet1 = top.Dat.SetPersistentData(top.API.cwcflagTag, top.API.cwcflagType, arrCWCFlag);
            top.API.displayMessage('������־λ��SetPersistentData CWCFLAG Return:' + nRet1);
        } else {
            NextPageFlag = 2;
            JournalPrint();
        }

    }

    /********************************************************************************************************/
    //����Ƿ���Ҫ�������ع�����Կ
    function CheckKey() {
        var objGetPinKey = top.Dat.GetDataSync("PINKEY", "STRING");
        var objGetMACKey = top.Dat.GetDataSync("MACKEY", "STRING");
        if (null == objGetPinKey || null == objGetMACKey) {
            top.API.displayMessage("GetDataSync WorKKey objGet = null");
            return false;
        }
        else {
            top.API.displayMessage("GetDataSync objGetPinKey Return:" + objGetPinKey.toArray());
            top.API.displayMessage("GetDataSync objGetMACKey Return:" + objGetMACKey.toArray());
            var arrGetPinKey = objGetPinKey.toArray();
            var arrGetMACKey = objGetMACKey.toArray();
            PinKey = arrGetPinKey[0];
            MACKey = arrGetMACKey[0];
            if (PinKey == "" || MACKey == "") {
                return false;
            } else {
                return true;
            }
        }
    }

    function DownPinKey() {
        top.API.displayMessage("����PINKEY");
        keyFlag = 1;
        var HexWorkKey = top.stringToHex(PinKey);
        var tmphexArray = new Array();
        top.Pin.ExtendedLoadEncryptedKey("PINKEY", HexWorkKey, "MasterKey", "CRYPT,FUNCTION,MACING,KEYENCKEY", tmphexArray);
    }

    function DownMACKey() {
        top.API.displayMessage("����MACKEY");
        keyFlag = 2;
        var HexWorkKey = top.stringToHex(MACKey);
        var tmphexArray = new Array();
        top.Pin.ExtendedLoadEncryptedKey("MACKEY", HexWorkKey, "MasterKey", "CRYPT,MACING", tmphexArray);
    }

    function onKeyLoaded() {
        top.API.displayMessage('�����¼���onKeyLoaded()');
        if (keyFlag == 1) {
            top.API.displayMessage('����PINKEY�ɹ�');
            DownMACKey();
        } else {
            top.API.displayMessage('����MACKEY�ɹ�');
            NextPageFlag = 1;
            JournalPrint();
        }
    }

    //event handler
    function onDeviceError() {
        top.API.displayMessage('���̴����¼���onDeviceError()');
        strErrMsg = "ϵͳ����";
        NextPageFlag = 2;
        JournalPrint();
    }

     function onKeyLoadFailed() {
        top.API.displayMessage('���̴����¼���onKeyLoadFailed()');
        strErrMsg = "ϵͳ����";
        NextPageFlag = 2;
        JournalPrint();
    }
    /********************************************************************************************************/
    //��������ģ��
    function onDatGetPersistentDataComplete(DataName, DataType, DataValue) {
        var arrDataValue = DataValue.toArray();
        top.API.displayMessage("onDatGetPersistentDataComplete is done,DataName=" + DataName + ",DataType=" + DataType + ",DataValue=" + arrDataValue[0]);
        if ('JNLNUM' == DataName) {
            JnlNum = arrDataValue[0] + 1;
            if (JnlNum.toString().length === 7) {
                JnlNum = 0;
            }
            //���ý�����ˮ��
            var arrJnlNum = new Array();
            arrJnlNum[0] = JnlNum;
            nRet1 = top.Dat.SetPersistentData(top.JnlnumTag, top.JnlnumType, arrJnlNum);
            top.API.displayMessage('������ˮ�ţ�SetPersistentData JNLNUM Return:' + nRet1 + '��JNLNUM = ' + JnlNum);
        }
    }

    function onDatGetPersistentDataError(DataName, ErrorCode) {
        top.API.displayMessage("onDatGetPersistentDataError is done,DataName=" + DataName + ",ErrorCode=" + ErrorCode);
        NextPageFlag = 2;
        JournalPrint();
    }

    function onDatSetPersistentDataComplete(DataName) {
        top.API.displayMessage("onDatSetPersistentDataComplete is done,DataName=" + DataName);
        top.Tcp.CompositionData(TransType);
        /*var nRet;  
        if ('JNLNUM' == DataName) {
        if (TransType == "CWD") {
        //���ó�����ˮ��
        var arrCWCJNLNUM = new Array();
        arrCWCJNLNUM[0] = JnlNum;
        nRet = top.Dat.SetPersistentData(top.API.cwcjnlnumTag, top.API.cwcjnlnumType, arrCWCJNLNUM);
        top.API.displayMessage('������ˮ�ţ�SetPersistentData CWCJNLNUM Return:' + nRet1);
        }else{
        top.Tcp.CompositionData(TransType);
        }
        }
        //�޸Ļ�ȡ�������ݳɹ���־λ
        if ('CWCJNLNUM' == DataName) {
        top.Tcp.CompositionData(TransType);
        }

        if ('CWCFLAG' == DataName) {
        if(TransType == "CWD"){
        //���ó���ԭ��
        var arrCWCREASON = new Array();
        arrCWCREASON[0] = 1;
        nRet = top.Dat.SetPersistentData(top.API.cwcreasonTag, top.API.cwcreasonType, arrCWCREASON);
        top.API.displayMessage('����ԭ��SetPersistentData CWCREASON Return:' + nRet);
        }    
        }
        if ('CWCREASON' == DataName) {                      
        NextPageFlag = 2;
        JournalPrint();
        }     */
    }

    function onDatSetPersistentDataError(DataName, ErrorCode) {
        top.API.displayMessage("onDatSetPersistentDataError is done,DataName=" + DataName + ",ErrorCode=" + ErrorCode);
        NextPageFlag = 2;
        JournalPrint();
    }
    /********************************************************************************************************/
    //JNLģ��
    function onJnlPrintComplete() {
        top.API.displayMessage("onJnlPrintComplete is done");
        if (NextPageFlag == 1) {
            if (TransType == "CWC") {
                return CallResponse("CWC");
            } else {
                if ((TransType == "DEP") || (TransType == "CWD")) {
                    var arrTransactionResult = new Array("���׳ɹ�");
                    var nRet = top.Dat.SetDataSync("TRANSACTIONRESULT", "STRING", arrTransactionResult);
                    top.API.displayMessage("SetDataSync TRANSACTIONRESULT Return:" + nRet);
                }
                if (top.API.gMixSelfCWD) {
                    return CallResponse("EXCWD");
                } else {
                    return CallResponse("OK");
                }
            }
        }
        if (NextPageFlag == 2) {
            TradeFailed();
        }
        // return CallResponse('OK');
    }

    function onJnlDeviceError() {
        top.API.displayMessage("onJnlDeviceError is done");
        onJnlPrintComplete();
    }

    function onJnlTimeout() {
        top.API.displayMessage("onJnlTimeout is done");
        onJnlPrintComplete();
    }

    /********************************************************************************************************/
    //Register the event
    function EventLogin() {
        top.Tcp.SendFailed.connect(onTcpSendFailed);
        top.Tcp.OnRecved.connect(onTcpOnRecved);
        top.Tcp.CompositionDataCompleted.connect(onCompositionDataCompleted);
        top.Tcp.CompositionDataFail.connect(onCompositionDataFail);
        top.Dat.GetPersistentDataComplete.connect(onDatGetPersistentDataComplete);
        top.Dat.GetPersistentDataError.connect(onDatGetPersistentDataError);
        top.Dat.SetPersistentDataComplete.connect(onDatSetPersistentDataComplete);
        top.Dat.SetPersistentDataError.connect(onDatSetPersistentDataError);
        top.Pin.MACGenerated.connect(onMACGenerated);
        top.Tcp.Timeout.connect(onTcpTimeout);
        top.Tcp.AnalysisFailed.connect(onAnalysisFailed);

        top.Pin.KeyLoaded.connect(onKeyLoaded);
        top.Pin.KeyLoadFailed.connect(onKeyLoadFailed);
        top.Pin.DeviceError.connect(onDeviceError);

    }

    function EventLogout() {
        top.Tcp.SendFailed.disconnect(onTcpSendFailed);
        top.Tcp.OnRecved.disconnect(onTcpOnRecved);
        top.Tcp.CompositionDataCompleted.disconnect(onCompositionDataCompleted);
        top.Tcp.CompositionDataFail.disconnect(onCompositionDataFail);
        top.Dat.GetPersistentDataComplete.disconnect(onDatGetPersistentDataComplete);
        top.Dat.GetPersistentDataError.disconnect(onDatGetPersistentDataError);
        top.Dat.SetPersistentDataComplete.disconnect(onDatSetPersistentDataComplete);
        top.Dat.SetPersistentDataError.disconnect(onDatSetPersistentDataError)
        top.Pin.MACGenerated.disconnect(onMACGenerated);
        top.Tcp.Timeout.disconnect(onTcpTimeout);
        top.Tcp.AnalysisFailed.disconnect(onAnalysisFailed);

        top.Pin.KeyLoaded.disconnect(onKeyLoaded);
        top.Pin.KeyLoadFailed.disconnect(onKeyLoadFailed);
        top.Pin.DeviceError.disconnect(onDeviceError);

    }

    //Countdown function
    function TimeoutCallBack() {
        top.API.displayMessage("TimeoutCallBack is done,TransType=" + TransType);
        if (!timeoutFlag) {
            timeoutFlag = true;
            strErrMsg = "ͨѶ��ʱ�����׽���";
            if (TransType == "CWD") {
                var arrCWCFlag = new Array();
                arrCWCFlag[0] = 1;
                var nRet1 = top.Dat.SetPersistentData(top.API.cwcflagTag, top.API.cwcflagType, arrCWCFlag);
                top.API.displayMessage('������־λ��SetPersistentData CWCFLAG Return:' + nRet1);
            } else {
                NextPageFlag = 2;
                JournalPrint();
            }
        }
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