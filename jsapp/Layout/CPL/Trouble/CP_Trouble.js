/*@create by:  tsxiong
*@time: 2016��03��20��
*/
;(function () {
    var keyFlag = 0; //���PINKEY��MACKEY��־λ��1��PINKEY   2��MACKEY
    var Pinkey = "";
    var MACkey = "";
    var nConnectTimes = 1;
    var bTimeUp = false;
    var timeId;
    var JnlNum = 0; //����ˮ,������ˮ��
    var Check = ""; //����ˮ,������Ӧ��
    var strMsg = ""; //����ˮ,������Ϣ
    var bTrade = false; //�����Ƿ�ɹ���true:�ɹ�
	var CallResponse = App.Cntl.ProcessOnce( function(Response){
        //TO DO:
        Clearup();
        //Entry the flows control process.
        App.Cntl.ProcessDriven(Response);
    });
    var Initialize = function () {
        EventLogin();
        InitialDevice();
    } (); //Page Entry

    function InitialDevice() {
        top.API.CheckDeviceStatus(); //����������״̬

        //note by hj
        /*if (!top.Cim.bDeviceStatus || !top.Cdm.bDeviceStatus) {
            top.API.displayMessage("�ֽ�ģ��״̬�쳣��ֹͣ����");
            top.API.InitOpenFlag();
            return CallResponse("OffLine");
        }*/
		var tmp1 = top.Cdm.StOutputStatus();
        var tmp2= top.Cim.StInputStatus();
        if (tmp1 != "EMPTY" || tmp2 != "EMPTY") {
            top.API.displayMessage("������InputOutputStatus="+tmp1 + "�볮��InputOutputStatus="+tmp2);
            top.API.InitOpenFlag();
            return CallResponse("OffLine");
        }
        if (top.Pin.bDeviceStatus) {// && top.Crd.bDeviceStatus
            if ((top.Pin.KeyIsValidSync("MasterKey"))) {
                top.API.displayMessage("����Կ����");
                if ("NOTPRESENT" != top.Crd.StMediaStatus()) {//�������п����̿�
                    top.Crd.Capture(top.API.gCaptureTimeout);
                }
                top.API.CashInfo.bCASH = true;
                //��ȡ��ǰ������Ϣ
				//modify by tsx  ��ʱ���� ��ȡ3�γ�����Ϣ
				for(var i=0; i<3;i++){
					var tmpPUCurrentCount = top.Cdm.PUCurrentCount();
					var tmpCUNoteValue = top.Cdm.CUNoteValue();
					if(tmpPUCurrentCount != null  && tmpCUNoteValue != null){
						//��ȡ��ǰ������Ϣ--��������
						top.API.CashInfo.arrUnitRemain = tmpPUCurrentCount.toArray();top.API.GetUnitInfo(tmpPUCurrentCount.toArray());
                //��ȡ��ǰ������Ϣ--������ֵ
						top.API.CashInfo.arrUnitCurrency = top.API.GetUnitInfo(tmpCUNoteValue.toArray());
						//��ȡ��ǰ������Ϣ--�������
						top.API.CashInfo.nCountOfUnits = top.API.CashInfo.arrUnitRemain.length;
						break;
					}else if(i == 2){
						top.API.displayMessage("��ȡ������Ϣʧ�ܣ�");
						top.API.InitOpenFlag();
						return CallResponse("OffLine");
					}					
				}
                //��ȡ��ǰ������Ϣ--�������
                top.API.CashInfo.nCountOfUnits = top.API.CashInfo.arrUnitCurrency.length;
                if (top.API.CashInfo.nCountOfUnits < 5) {
					//top.API.CashInfo.nCountOfUnits=5; 
                    top.API.displayMessage("������Ŀ����nCountOfUnits = " + top.API.CashInfo.nCountOfUnits);
                    top.API.InitOpenFlag();
                    return CallResponse("OffLine");
                }
				//���Ŀǰ���ģ��AcceptStatus
				var strAcceptStatus = top.Cim.LastAcceptStatus();
				top.API.displayMessage("LastAcceptStatus=" + strAcceptStatus);
				if (strAcceptStatus == "ACTIVE") {
					top.API.displayMessage("ִ��CashInEnd");
					//ִ��CashInEnd
					top.Cim.StoreEscrowedCash(top.API.gStoreEscrowedCashTimeOut);
				} 
                //��ȡ�����־
                var nRet1 = top.Dat.GetPersistentData("TOTALADDOK", "LONG");
                top.API.displayMessage("GetPersistentData TOTALADDOK Return:" + nRet1);
            } else {
                top.API.displayMessage("����Կ�쳣");
                top.API.InitOpenFlag();
                return CallResponse("OffLine");
            }
        } else {
            top.API.displayMessage("���̴��쳣��ֹͣ����");
            top.API.InitOpenFlag();
            return CallResponse("OffLine");
        }
    }

    /********************************************************************************************************/
    //��������ģ��
    function onDatGetPersistentDataComplete(DataName, DataType, DataValue) {
        var arrDataValue = DataValue.toArray();
        var nRet1 = -1;
        top.API.displayMessage("onDatGetPersistentDataComplete is done,DataName=" + DataName + ",DataType=" + DataType + ",DataValue=" + arrDataValue[0]);
        if ('TOTALADDOK' == DataName) {
            if (arrDataValue[0] == 0) {
                top.API.displayMessage("δ������ӳ�����");
                return CallResponse("OffLine");
            } else {
                //��ȡ����������
                nRet1 = top.Dat.GetPersistentData("POSSIBLETRANSACTION", "STRING");
                top.API.displayMessage("GetPersistentData POSSIBLETRANSACTION Return:" + nRet1);
            }
        }
        if ('POSSIBLETRANSACTION' == DataName) {
            if (arrDataValue[0].substr(0, 1) == "1") {
                top.API.CashInfo.bExchange = true;
            } else {
                top.API.CashInfo.bExchange = false;
            }
            if (arrDataValue[0].substr(1, 1) == "1") {
                top.API.CashInfo.bCWD = true;
            } else {
                top.API.CashInfo.bCWD = false;
            }
            if (arrDataValue[0].substr(2, 1) == "1") {
                top.API.CashInfo.bDEP = true;
            } else {
                top.API.CashInfo.bDEP = false;
            }
            //��ȡ�ն˺�
            nRet1 = top.Dat.GetPersistentData("TERMINALNUM", "STRING");
            top.API.displayMessage("GetPersistentData TERMINALNUM Return:" + nRet1);
        }
        if ('TERMINALNUM' == DataName) {            
            top.API.gTerminalID = arrDataValue[0];
            //��ȡ������ˮ��
            nRet1 = top.Dat.GetPersistentData(top.JnlnumTag, top.JnlnumType);
            top.API.displayMessage("GetPersistentData JNLNUM Return:" + nRet1);
        }
        if ('JNLNUM' == DataName) {
            JnlNum = arrDataValue[0] + 1;
            if (JnlNum.toString().length === 7) {
                JnlNum = 0;
            }
            //���ý�����ˮ��
            var arrJnlNum = new Array();
            arrJnlNum[0] = JnlNum;
            nRet1 = top.Dat.SetPersistentData(top.JnlnumTag, top.JnlnumType, arrJnlNum);
            top.API.displayMessage('��ѯ����--������ˮ�ţ�SetPersistentData JNLNUM Return:' + nRet1 + '��JNLNUM = ' + JnlNum);
        }
    }

    function onDatGetPersistentDataError(DataName, ErrorCode) {
        top.API.displayMessage("onDatGetPersistentDataError is done,DataName=" + DataName + ",ErrorCode=" + ErrorCode);
        top.API.displayMessage("δ������ӳ�����");
        return CallResponse("OffLine");
    }

    function onDatSetPersistentDataComplete(DataName) {
        top.API.displayMessage("onDatSetPersistentDataComplete is done,DataName=" + DataName);
        if ('JNLNUM' == DataName) {
            var arrTransType = new Array("APPLYWORKPARAM");
            top.API.displayMessage("Start APPLYWORKPARAM");
            top.Tcp.CompositionData(arrTransType); //���д�������������
        }
    }

    function onDatSetPersistentDataError(DataName, ErrorCode) {
        top.API.displayMessage("onDatSetPersistentDataError is done,DataName=" + DataName + ",ErrorCode=" + ErrorCode);
        return CallResponse("OffLine");
    }
    /********************************************************************************************************/
    //TCPģ��
    function onCompositionDataCompleted() {
        top.API.displayMessage("onCompositionDataCompleted is done");
        var objArrData = new Array();
        top.Tcp.SendToHost(objArrData, 60000);
    }
    function onTcpOnRecved(tmpCheck) {
        Check = tmpCheck;
        top.API.displayMessage("onTcpOnRecved is done,Check:" + Check);
        strErrMsg = "";
        switch (Check) {
            case '00':
                bTrade = true;
                strErrMsg = "���׳ɹ�";
                break;
            default:
                top.API.displayMessage("���ع��������쳣");
                strErrMsg = "����ʧ��";
                top.API.InitOpenFlag();
                break;
        }
        SendJnl();
    }
    function onTcpSendFailed() {
        top.API.displayMessage("onTcpSendFailed is done");
        App.Timer.SetIntervalDisposal(onCompositionDataCompleted, 15000);
        if (nConnectTimes == 3) {
            top.API.InitOpenFlag();
            top.API.displayMessage("�������ӳ�ʱ����");
            return CallResponse("OffLine");
        } else {
            nConnectTimes++;
        }
    }
    function onTcpTimeout() {
        top.API.displayMessage("onTcpTimeout is done");
        App.Timer.SetIntervalDisposal(onCompositionDataCompleted, 15000);
        if (nConnectTimes == 3) {
            top.API.InitOpenFlag();
            top.API.displayMessage("�������ӳ�ʱ����");
            return CallResponse("OffLine");
        } else {
            nConnectTimes++;
        }
    }
    /********************************************************************************************************/
    //JNLģ��
    function SendJnl() {
        top.Jnl.PrintSync("ApplyWorkParam");
        onJnlPrintComplete();
    }
    function onJnlPrintComplete() {
        top.API.displayMessage("onJnlPrintComplete is done");
        if (CheckKey()) {
            DownPinKey();
        } else {
            top.API.displayMessage("���ع����������׳ɹ�");
            if (bTrade) {
                var arrBSERVICE = new Array();
                arrBSERVICE[0] = 1;
                var nRet = top.Dat.SetDataSync(top.API.servicestateTag, top.API.servicestateType, arrBSERVICE);
                top.API.displayMessage("SetDataSync SERVICESTATE Return:" + nRet);
                return CallResponse("OnLine");
            } else {
                top.API.displayMessage("���ع��������������0");
                return CallResponse("OffLine");
            }
        }
    }

    function onJnlDeviceError() {
        top.API.displayMessage("onJnlDeviceError is done");
        return CallResponse("OffLine");
    }

    function onJnlTimeout() {
        top.API.displayMessage("onJnlTimeout is done");
        return CallResponse("OffLine");
    }

    /********************************************************************************************************/
    //PINģ��
    function CheckKey() {
        var objGetPinKey = top.Dat.GetDataSync(top.API.pinkeyTag, top.API.pinkeyType);
        var objGetMACKey = top.Dat.GetDataSync(top.API.mackeyTag, top.API.mackeyType);
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
            if (bTrade) {
                var arrBSERVICE = new Array();
                arrBSERVICE[0] = 1;
                var nRet = top.Dat.SetDataSync(top.API.servicestateTag, top.API.servicestateType, arrBSERVICE);
                top.API.displayMessage("SetDataSync SERVICESTATE Return:" + nRet);
                return CallResponse("OnLine");
            } else {
                return CallResponse("OffLine");
            }
        }
    }

    function onDeviceError() {
        top.API.displayMessage('���̴����¼���onDeviceError()');
        top.API.InitOpenFlag();
        return CallResponse("OffLine");
    }
    function onKeyLoadFailed() {
        top.API.displayMessage('���̴����¼���onKeyLoadFailed()');
        top.API.InitOpenFlag();
        return CallResponse("OffLine");
    }
    //Register the event
    function EventLogin() {
        top.Tcp.SendFailed.connect(onTcpSendFailed);
        top.Tcp.OnRecved.connect(onTcpOnRecved);
        top.Tcp.CompositionDataCompleted.connect(onCompositionDataCompleted);
        top.Tcp.Timeout.connect(onTcpTimeout);
        ////
        top.Pin.KeyLoaded.connect(onKeyLoaded);
        top.Pin.KeyLoadFailed.connect(onKeyLoadFailed); 
        top.Pin.DeviceError.connect(onDeviceError);
        ////
        top.Dat.GetPersistentDataComplete.connect(onDatGetPersistentDataComplete);
        top.Dat.GetPersistentDataError.connect(onDatGetPersistentDataError);
        top.Dat.SetPersistentDataComplete.connect(onDatSetPersistentDataComplete);
        top.Dat.SetPersistentDataError.connect(onDatSetPersistentDataError)
        ////
    }

    function EventLogout() {
        top.Tcp.SendFailed.disconnect(onTcpSendFailed);
        top.Tcp.OnRecved.disconnect(onTcpOnRecved);
        top.Tcp.CompositionDataCompleted.disconnect(onCompositionDataCompleted);
        top.Tcp.Timeout.disconnect(onTcpTimeout);
        ////
        top.Pin.KeyLoaded.disconnect(onKeyLoaded);
        top.Pin.KeyLoadFailed.disconnect(onKeyLoadFailed); 
        top.Pin.DeviceError.disconnect(onDeviceError);
        ////
        top.Dat.GetPersistentDataComplete.disconnect(onDatGetPersistentDataComplete);
        top.Dat.GetPersistentDataError.disconnect(onDatGetPersistentDataError);
        top.Dat.SetPersistentDataComplete.disconnect(onDatSetPersistentDataComplete);
        top.Dat.SetPersistentDataError.disconnect(onDatSetPersistentDataError)
        ////
    }
    //Page Return
    

    //remove all event handler
    function Clearup() {
        EventLogout();
        App.Timer.ClearIntervalTime();
        //TO DO:
    };
})();