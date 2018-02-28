/*@create by:  tsxiong
 *@time: 2016\C4\EA03\D4\C220\C8\D5
 */
;
(function () {
    var keyFlag = 0; //\B1\E6\B1\F0PINKEY\A1\A2MACKEY\B1\EA־λ\A3\BB1\A3\BAPINKEY   2\A3\BAMACKEY
    var Pinkey = "";
    var MACkey = "";
    var nConnectTimes = 1;
    var JnlNum = 0; //\BC\C7\C1\F7ˮ,\BD\BB\D2\D7\C1\F7ˮ\BA\C5
    var Check = ""; //记流水,交易响应码
    var strMsg = ""; //\BC\C7\C1\F7ˮ,\BD\BB\D2\D7\D0\C5Ϣ
    var bJnl = true; //\CAǷ\F1\B5\DAһ\B4\CE\CD\EA\B3\C9Jnl
    var CallResponse = App.Cntl.ProcessOnce(function (Response) {
        //TO DO:
        Clearup();
        //Entry the flows control process.
        App.Cntl.ProcessDriven(Response);
    });
    var Initialize = function () {
        //EventLogin();

        App.Timer.TimeoutDisposal(TimeoutCallBack);
        if (top.FirstOpenFlag) {
            //alert("API.OpenDevice");
            top.API.OpenDevice();
        } else {
            InitialDevice();
        }
        //top.Sys.SetMainProcessSync();
    }(); //Page Entry

    //@User ocde scope start
    function CheckOpenStatus() {
        InitialDevice();
        /*
            if (top.Crd.bOpenDevice && top.Pin.bOpenDevice
                && top.Ptr.bOpenDevice && top.Idr.bOpenDevice && top.Fpi.bOpenDevice
                 && top.Cim.bOpenDevice && top.Cdm.bOpenDevice && top.Siu.bOpenDevice && top.Scr.bOpenDevice) {
                top.FirstOpenFlag = false; //\D2Ѿ\ADȫ\B2\BF\B4򿪹\FD
                InitialDevice(); //\BD\F8\D0г\F5ʼ\BB\AF
            }*/
    }

    function InitialDevice() {
        return CallResponse("OnLine");
        top.API.CheckDeviceStatus(); //\BC\EC\B2\E2\B8\F7\B8\F6\B2\BF\BC\FE״̬
        top.API.gVersion = top.Dat.GetPrivateProfileSync("Version", "APVer", "", "c:\\TCR\\HMAP_Ver.ini");
        top.API.gOrderCWDFlag = top.Dat.GetPrivateProfileSync("TransactionConfig", "bOrderCWD", "0", top.API.gIniFileName);
        var tmp1 = top.Cdm.StOutputStatus();
        var tmp2 = top.Cim.StInputStatus();
        if (tmp1 != "EMPTY" || tmp2 != "EMPTY") {
            top.API.displayMessage("\B3\F6\B3\AE\BF\DAInputOutputStatus=" + tmp1 + "\C8볮\BF\DAInputOutputStatus=" + tmp2);

            return CallResponse("OffLine");
        }

        if (top.Pin.bDeviceStatus) {
            if (top.Pin.KeyIsValidSync("MasterKey")) {
                top.API.displayMessage("\D6\F7\C3\DCԿ\B4\E6\D4\DA");
                if ("NOTPRESENT" != top.Crd.StMediaStatus()) { //\B4\FD\D0\DE\D5\FD4
                    top.Crd.Eject(-1);
                    top.Jnl.PrintSync("ErrorEjectCardAction");
                    return CallResponse("OffLine");
                }
                top.API.CashInfo.bCASH = true;
                //modify by tsx  \C1\D9ʱ\B7\BD\B0\B8 \BB\F1ȡ3\B4γ\AE\CF\E4\D0\C5Ϣ
                for (var i = 0; i < 3; i++) {
                    var tmpPUCurrentCount = top.Cdm.PUCurrentCount();
                    var tmpCUNoteValue = top.Cdm.CUNoteValue();
                    if (tmpPUCurrentCount != null && tmpCUNoteValue != null) {
                        //\BB\F1ȡ\B5\B1ǰ\B3\AE\CF\E4\D0\C5Ϣ--\B3\AE\CF\E4\D3\E0\C1\BF
                        top.API.CashInfo.arrUnitRemain = top.API.GetUnitInfo(tmpPUCurrentCount.toArray());
                        //\BB\F1ȡ\B5\B1ǰ\B3\AE\CF\E4\D0\C5Ϣ--\B3\AE\CF\E4\C3\E6ֵ
                        top.API.CashInfo.arrUnitCurrency = top.API.GetUnitInfo(tmpCUNoteValue.toArray());
                        //\BB\F1ȡ\B5\B1ǰ\B3\AE\CF\E4\D0\C5Ϣ--\B3\AE\CF\E4\B8\F6\CA\FD
                        top.API.CashInfo.nCountOfUnits = top.API.CashInfo.arrUnitRemain.length;
                        break;
                    } else if (i == 2) {
                        top.API.displayMessage("\BB\F1ȡ\B3\AE\CF\E4\D0\C5Ϣʧ\B0ܣ\A1");

                        return CallResponse("OffLine");
                    }
                }
                top.API.CashInfo.nCountOfUnits = top.API.CashInfo.arrUnitCurrency.length;
                if (top.API.CashInfo.nCountOfUnits < 5) {
                    return CallResponse("OffLine");
                }
                //\BC\EC\B2\E2Ŀǰ\B4\E6\BF\EEģ\BF\E9AcceptStatus
                var strAcceptStatus = top.Cim.LastAcceptStatus();
                top.API.displayMessage("LastAcceptStatus=" + strAcceptStatus);
                if (strAcceptStatus == "ACTIVE") {
                    top.API.displayMessage("ִ\D0\D0CashInEnd");
                    //ִ\D0\D0CashInEnd
                    top.Cim.StoreEscrowedCash(top.API.gStoreEscrowedCashTimeOut);
                }
                //\BB\F1ȡ\C7\E5\BB\FA\B1\EA־
                var nRet1 = top.Dat.GetPersistentData("TOTALADDOK", "LONG");
            } else {
                top.Jnl.PrintSync("MasterKeyError");

                return CallResponse("OffLine");
            }
        } else {
            //\D0\DE\D5\FD7--  \BC\D3form
            top.API.displayMessage("\BC\FC\C5̴\F2\BF\AA\D2쳣\A3\ACֹͣ\B7\FE\CE\F1"); //\BC\FC\C5̴\F2\BF\AA\D2쳣\A3\ACֹͣ\B7\FE\CE\F1

            return CallResponse("OffLine");
        }
    }
    //@User code scope end
    //Open START
    function onCrdConnectionOpened() {
        CheckOpenStatus();
    }
    /*
    function onCrdConnectionOpened() {
        top.API.displayMessage("onCrdConnectionOpened is done");
        top.Crd.bOpenDevice = true;
        CheckOpenStatus();

    }*/

    function onPinConnectionOpened() {
        top.API.displayMessage("onPinConnectionOpened is done");
        top.Pin.bOpenDevice = true;
        CheckOpenStatus();
    }

    function onPtrConnectionOpened() {
        top.API.displayMessage("onPtrConnectionOpened is done");
        top.Ptr.bOpenDevice = true;
        CheckOpenStatus();
    }

    function onCimConnectionOpened() {
        top.API.displayMessage("onCimConnectionOpened is done");
        top.Cim.bOpenDevice = true;
        CheckOpenStatus();
    }


    function onIdrConnectionOpened() {
        top.API.displayMessage("onIdrConnectionOpened is done"); //,ִ\D0\D0<top.Idr.Reset(RETRACT, 4)>    
        top.Idr.bOpenDevice = true;
        CheckOpenStatus();
    }

    function onFpiConnectionOpened() {
        top.API.displayMessage("onFpiConnectionOpened is done"); //,ִ\D0\D0<top.Fpi.Reset(RETRACT, 4)>  
        top.Fpi.bOpenDevice = true;
        CheckOpenStatus();

    }

    function onCdmConnectionOpened() {
        top.API.displayMessage("onCdmConnectionOpened is done"); //,ִ\D0\D0<top.Cim.Reset(RETRACT, 4)>
        top.Cdm.bOpenDevice = true;
        CheckOpenStatus();
    }

    function onScrConnectionOpened() {
        top.API.displayMessage("onScrConnectionOpened is done");
        top.Scr.bOpenDevice = true;
        CheckOpenStatus();
    }


    //\B2\BB\D7\F6״̬\B8ı\E4\CA¼\FE\B4\A6\C0\ED
    function onSiuConnectionOpened() {
        top.API.displayMessage("onSiuConnectionOpened is done");
        top.Siu.bOpenDevice = true;
        CheckOpenStatus();
    }

    //Open END
    //DeviceError START
    function onCrdOpenFailed() {
        top.API.displayMessage("onCrdOpenFailed is done");
        top.Fpi.bOpenDevice = true;
        CheckOpenStatus();
    }

    function onPtrOpenFailed() {
        top.API.displayMessage("onPtrOpenFailed is done");
        top.Ptr.bOpenDevice = true;
        CheckOpenStatus();
    }

    function onCimOpenFailed() {
        top.API.displayMessage("onCimOpenFailed is done");
        top.Cim.bOpenDevice = true;
        CheckOpenStatus();
    }

    function onCdmOpenFailed() {
        top.API.displayMessage("onCdmOpenFailed is done");
        top.Cdm.bOpenDevice = true;
        CheckOpenStatus();
    }

    function onFpiOpenFailed() {
        top.API.displayMessage("onFpiOpenFailed is done");
        top.Fpi.bOpenDevice = true;
        CheckOpenStatus();
    }

    function onIdrOpenFailed() {
        top.API.displayMessage("onIdrOpenFailed is done");
        top.Idr.bOpenDevice = true;
        CheckOpenStatus();
    }

    function onSiuOpenFailed() {
        top.API.displayMessage("onSiuOpenFailed is done");
        top.Siu.bOpenDevice = true;
        CheckOpenStatus();
    }

    function onScrOpenFailed() {
        top.API.displayMessage("onScrOpenFailed is done");
        top.Scr.bOpenDevice = true;
        CheckOpenStatus();
    }

    function onPinOpenFailed() {
        top.API.displayMessage("onScrOpenFailed is done");
        top.Pin.bOpenDevice = true;
        CheckOpenStatus();
    }

    /********************************************************************************************************/
    //\D3\C0\BE\C3\CA\FD\BE\DDģ\BF\E9
    function onDatGetPersistentDataComplete(DataName, DataType, DataValue) {
        var arrDataValue = DataValue.toArray();
        var nRet1 = -1;
        top.API.displayMessage("onDatGetPersistentDataComplete is done,DataName=" + DataName + ",DataType=" + DataType + ",DataValue=" + arrDataValue[0]);
        if ('TOTALADDOK' == DataName) {
            if (arrDataValue[0] == 0) {
                //\D0\DE\D5\FD   \BC\D3\C8\EB\C1\F7ˮ  \CC\ED\BC\D3form
                top.API.displayMessage("δ\D7\F6\C7\E5\BB\FA\BCӳ\AE\B2\D9\D7\F7");
                top.Sys.SetIsMaintain(true);
                return CallResponse("OffLine");
            } else {
                top.Dat.GetPersistentData("TERMINALNUM", "STRING");
            }
        }
        if ('TERMINALNUM' == DataName) {
            top.API.gTerminalID = arrDataValue[0];
            //\BB\F1ȡ\BD\BB\D2\D7\C1\F7ˮ\BA\C5
            top.Dat.GetPersistentData(top.JnlnumTag, top.JnlnumType);
        }
        if ('JNLNUM' == DataName) {
            JnlNum = arrDataValue[0] + 1;
            if (JnlNum.toString().length === 7) {
                JnlNum = 0;
            }
            //\C9\E8\D6ý\BB\D2\D7\C1\F7ˮ\BA\C5
            var arrJnlNum = new Array();
            arrJnlNum[0] = JnlNum;
            top.Dat.SetPersistentData(top.JnlnumTag, top.JnlnumType, arrJnlNum);
        }
    }

    function onDatGetPersistentDataError(DataName, ErrorCode) {
        top.API.displayMessage("onDatGetPersistentDataError is done,DataName=" + DataName + ",ErrorCode=" + ErrorCode);
        top.API.displayMessage("δ\D7\F6\C7\E5\BB\FA\BCӳ\AE\B2\D9\D7\F7");
        return CallResponse("OffLine");
    }

    function onDatSetPersistentDataComplete(DataName) {
        top.API.displayMessage("onDatSetPersistentDataComplete is done,DataName=" + DataName);
        if ('JNLNUM' == DataName) {
            //\B4\FD\D0\DE\D5\FD \CC\ED\BC\D3\C1\F7ˮ
            var arrTransType = new Array("APPLYWORKPARAM");
            top.API.displayMessage("Start APPLYWORKPARAM");
            top.Tcp.CompositionData(arrTransType); //\BD\F8\D0д\A6\C0\ED\B9\A4\D7\F7\B2\CE\CA\FD\B1\A8\CE\C4
        }
    }

    function onDatSetPersistentDataError(DataName, ErrorCode) {
        top.API.displayMessage("onDatSetPersistentDataError is done,DataName=" + DataName + ",ErrorCode=" + ErrorCode);
        return CallResponse("OffLine");
    }
    /********************************************************************************************************/
    //TCPģ\BF\E9
    function onCompositionDataCompleted() {
        top.API.displayMessage("onCompositionDataCompleted is done");
        var objArrData = new Array();
        top.Tcp.SendToHost(objArrData, 60000);
    }

    function onTcpOnRecved(tmpCheck) {
        Check = tmpCheck;
        top.API.displayMessage("onTcpOnRecved is done,Check:" + Check);
        if (Check == "00" && CheckKey()) {
            PrintJnl("\C9\EA\C7빤\D7\F7\B2\CE\CA\FD\B3ɹ\A6");
            DownPinKey();
        } else {
            PrintJnl("\C9\EA\C7빤\D7\F7\B2\CE\CA\FD\D2쳣");
            return CallResponse("OffLine");
        }
    }

    function PrintJnl(strData) {
        var arrTransactionResult = new Array(strData);
        top.Dat.SetDataSync("TRANSACTIONRESULT", "STRING", arrTransactionResult);
        top.Jnl.PrintSync("ApplyWorkParam");
    }

    function onTcpSendFailed() {
        top.API.displayMessage("onTcpSendFailed is done");
        App.Timer.SetIntervalDisposal(onCompositionDataCompleted, 15000);
        if (nConnectTimes == 3) {
            top.API.displayMessage("\CD\F8\C2\E7\C1\AC\BDӳ\ACʱ\C8\FD\B4\CE");
            return CallResponse("OffLine");
        } else {
            nConnectTimes++;
        }
    }

    function onTcpTimeout() {
        top.API.displayMessage("onTcpTimeout is done");
        App.Timer.SetIntervalDisposal(onCompositionDataCompleted, 15000);
        if (nConnectTimes == 3) {
            top.API.displayMessage("\CD\F8\C2\E7\C1\AC\BDӳ\ACʱ\C8\FD\B4\CE");
            return CallResponse("OffLine");
        } else {
            nConnectTimes++;
        }
    }
    /********************************************************************************************************/
    //PINģ\BF\E9
    function CheckKey() {
        var objGetPinKey = top.Dat.GetDataSync(top.API.pinkeyTag, top.API.pinkeyType);
        var objGetMACKey = top.Dat.GetDataSync(top.API.mackeyTag, top.API.mackeyType);
        if (null == objGetPinKey || null == objGetMACKey) {
            top.API.displayMessage("GetDataSync WorKKey objGet = null");
            return false;
        } else {
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
        top.API.displayMessage("\CF\C2\D4\D8PINKEY");
        keyFlag = 1;
        var HexWorkKey = top.stringToHex(PinKey);
        var tmphexArray = new Array();
        top.Pin.ExtendedLoadEncryptedKey("PINKEY", HexWorkKey, "MasterKey", "CRYPT,FUNCTION,MACING,KEYENCKEY", tmphexArray);
    }

    function DownMACKey() {
        top.API.displayMessage("\CF\C2\D4\D8MACKEY");
        keyFlag = 2;
        var HexWorkKey = top.stringToHex(MACKey);
        var tmphexArray = new Array();
        top.Pin.ExtendedLoadEncryptedKey("MACKEY", HexWorkKey, "MasterKey", "CRYPT,MACING", tmphexArray);
    }

    function onKeyLoaded() {
        if (keyFlag == 1) {
            top.API.displayMessage('\CF\C2\D4\D8PINKEY\B3ɹ\A6');
            DownMACKey();
        } else {
            top.API.displayMessage('\CF\C2\D4\D8MACKEY\B3ɹ\A6');
            top.Jnl.PrintSync("OpenAppSuccess");
            return CallResponse("OnLine");
        }
    }

    function onDeviceError() {
        top.API.displayMessage('\BC\FC\C5̴\A5\B7\A2\CA¼\FE\A3\BAonDeviceError()');
        return CallResponse("OffLine");
    }

    function onKeyLoadFailed() {
        top.API.displayMessage('\BC\FC\C5̴\A5\B7\A2\CA¼\FE\A3\BAonKeyLoadFailed()');
        return CallResponse("OffLine");
    }

    function onCryptFailed() {
        top.API.displayMessage('\BC\FC\C5̼ӽ\E2\C3\DCʧ\B0ܣ\BAonCryptFailed');
        return CallResponse("OffLine");
    }

    function onServiceClosed() {
        top.API.displayMessage('\B4\A5\B7\A2\CA¼\FE\A3\BAonServiceClosed()');
        return CallResponse("OffLine");
    }
    //Register the event
    function EventLogin() {
        top.Crd.ConnectionOpened.connect(onCrdConnectionOpened);
        top.Pin.ConnectionOpened.connect(onPinConnectionOpened);
        top.Ptr.ConnectionOpened.connect(onPtrConnectionOpened);
        top.Cim.ConnectionOpened.connect(onCimConnectionOpened);
        top.Cdm.ConnectionOpened.connect(onCdmConnectionOpened);
        top.Idr.ConnectionOpened.connect(onIdrConnectionOpened);
        top.Fpi.ConnectionOpened.connect(onFpiConnectionOpened);
        top.Siu.ConnectionOpened.connect(onSiuConnectionOpened);
        top.Scr.ConnectionOpened.connect(onScrConnectionOpened);
        ////
        top.Crd.OpenFailed.connect(onCrdOpenFailed);
        top.Ptr.OpenFailed.connect(onPtrOpenFailed);
        top.Cim.OpenFailed.connect(onCimOpenFailed);
        top.Cdm.OpenFailed.connect(onCdmOpenFailed);
        top.Idr.OpenFailed.connect(onIdrOpenFailed);
        top.Fpi.OpenFailed.connect(onFpiOpenFailed);
        top.Siu.OpenFailed.connect(onSiuOpenFailed);
        top.Scr.OpenFailed.connect(onScrOpenFailed);
        top.Pin.OpenFailed.connect(onPinOpenFailed);
        ////------全部切换
        top.Tcp.SendFailed.connect(onTcpSendFailed);
        top.Tcp.OnRecved.connect(onTcpOnRecved);
        top.Tcp.CompositionDataCompleted.connect(onCompositionDataCompleted);
        top.Tcp.Timeout.connect(onTcpTimeout);
        ////
        top.Pin.KeyLoaded.connect(onKeyLoaded);
        top.Pin.KeyLoadFailed.connect(onKeyLoadFailed);
        top.Pin.CryptFailed.connect(onCryptFailed);
        top.Pin.DeviceError.connect(onDeviceError);
        ////
        top.Dat.GetPersistentDataComplete.connect(onDatGetPersistentDataComplete);
        top.Dat.GetPersistentDataError.connect(onDatGetPersistentDataError);
        top.Dat.SetPersistentDataComplete.connect(onDatSetPersistentDataComplete);
        top.Dat.SetPersistentDataError.connect(onDatSetPersistentDataError)
        ////
    }

    function EventLogout() {
        top.Crd.ConnectionOpened.disconnect(onCrdConnectionOpened);
        top.Pin.ConnectionOpened.disconnect(onPinConnectionOpened);
        top.Ptr.ConnectionOpened.disconnect(onPtrConnectionOpened);
        top.Cim.ConnectionOpened.disconnect(onCimConnectionOpened);
        top.Cdm.ConnectionOpened.disconnect(onCdmConnectionOpened);
        top.Idr.ConnectionOpened.disconnect(onIdrConnectionOpened);
        top.Fpi.ConnectionOpened.disconnect(onFpiConnectionOpened);
        top.Scr.ConnectionOpened.disconnect(onScrConnectionOpened);
        top.Siu.ConnectionOpened.disconnect(onSiuConnectionOpened);
        ////
        top.Crd.OpenFailed.disconnect(onCrdOpenFailed);
        top.Ptr.OpenFailed.disconnect(onPtrOpenFailed);
        top.Cim.OpenFailed.disconnect(onCimOpenFailed);
        top.Cdm.OpenFailed.disconnect(onCdmOpenFailed);
        top.Idr.OpenFailed.disconnect(onIdrOpenFailed);
        top.Fpi.OpenFailed.disconnect(onFpiOpenFailed);
        top.Siu.OpenFailed.disconnect(onSiuOpenFailed);
        top.Scr.OpenFailed.disconnect(onScrOpenFailed);
        top.Pin.OpenFailed.disconnect(onPinOpenFailed);
        ////-----ok
        top.Tcp.SendFailed.disconnect(onTcpSendFailed);
        top.Tcp.OnRecved.disconnect(onTcpOnRecved);
        top.Tcp.CompositionDataCompleted.disconnect(onCompositionDataCompleted);
        top.Tcp.Timeout.disconnect(onTcpTimeout);
        ////
        top.Pin.KeyLoaded.disconnect(onKeyLoaded);
        top.Pin.KeyLoadFailed.disconnect(onKeyLoadFailed);
        top.Pin.CryptFailed.disconnect(onCryptFailed);
        top.Pin.DeviceError.disconnect(onDeviceError);
        ////
        top.Dat.GetPersistentDataComplete.disconnect(onDatGetPersistentDataComplete);
        top.Dat.GetPersistentDataError.disconnect(onDatGetPersistentDataError);
        top.Dat.SetPersistentDataComplete.disconnect(onDatSetPersistentDataComplete);
        top.Dat.SetPersistentDataError.disconnect(onDatSetPersistentDataError);
        ////
    }
    //Page Return
    function TimeoutCallBack() {
        top.API.displayMessage("\B3\ACʱ\CA¼\FE\B4\A5\B7\A2");

        return CallResponse("OffLine");
    }


    //remove all event handler
    function Clearup() {
        //EventLogout();
        App.Timer.ClearTime();
        App.Timer.ClearIntervalTime();
        //TO DO:
    };
})();