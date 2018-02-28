/*@create by:  tsxiong
*@time: 2016年03月20日
*/
; (function () {
    var keyFlag = 0; //辨别PINKEY、MACKEY标志位；1：PINKEY   2：MACKEY
    var Pinkey = "";
    var MACkey = "";
    var JnlNum = 0; //记流水,交易流水号
    var Check = ""; //记流水,交易响应码
    var strMsg = ""; //记流水,交易信息		

    var ChangeNum = new Array();
    var BoxNo = new Array();
	var CallResponse = App.Cntl.ProcessOnce( function(Response){
        //TO DO:
        Clearup();
        //Entry the flows control process.
        App.Cntl.ProcessDriven(Response);
    });
    var Initialize = function () {
        ButtonDisable();
        EventLogin();
        //获取交易流水号
        var nRet1 = top.Dat.GetPersistentData(top.JnlnumTag, top.JnlnumType);
        top.API.displayMessage("GetPersistentData JNLNUM Return:" + nRet1);
    } (); //Page Entry
    function ButtonDisable() {
        document.getElementById('OK').disabled = true;
        document.getElementById('Exit').disabled = true;
    }

    function ButtonEnable() {
        document.getElementById('OK').disabled = false;
        document.getElementById('Exit').disabled = false;
    }

    function DownFaild() {
        document.getElementById("faildDiv").style.display = "block";
        document.getElementById("ChangeDiv").style.display = "none";
        document.getElementById("Exit").style.display = "block";
        ButtonEnable();
    }

    function DownSuccessd() {
        document.getElementById("SuccessedDiv").style.display = "block";
        document.getElementById("ChangeDiv").style.display = "none";
        document.getElementById("OK").style.display = "block";
        ButtonEnable();
    }

    //@User ocde scope start
    document.getElementById('OK').onclick = function () {
        ButtonDisable();
        return CallResponse('OK');
    }

    document.getElementById('Exit').onclick = function () {
        ButtonDisable();
        return CallResponse('Exit');
    }
    /********************************************************************************************************/
    //永久数据模块
    function onDatGetPersistentDataComplete(DataName, DataType, DataValue) {
        var arrDataValue = DataValue.toArray();
        var nRet1 = -1;
        top.API.displayMessage("onDatGetPersistentDataComplete is done,DataName=" + DataName + ",DataType=" + DataType + ",DataValue=" + arrDataValue[0]);
        if ('JNLNUM' == DataName) {
            JnlNum = arrDataValue[0] + 1;
            if (JnlNum.toString().length === 7) {
                JnlNum = 0;
            }
            //设置交易流水号
            var arrJnlNum = new Array();
            arrJnlNum[0] = JnlNum;
            nRet1 = top.Dat.SetPersistentData(top.JnlnumTag, top.JnlnumType, arrJnlNum);
            top.API.displayMessage('查询余额报文--交易流水号：SetPersistentData JNLNUM Return:' + nRet1 + '，JNLNUM = ' + JnlNum);
        }
    }

    function onDatGetPersistentDataError(DataName, ErrorCode) {
        top.API.displayMessage("onDatGetPersistentDataError is done,DataName=" + DataName + ",ErrorCode=" + ErrorCode);
        DownFaild();
    }
    function onDatSetPersistentDataComplete(DataName) {
        top.API.displayMessage("onDatSetPersistentDataComplete is done,DataName=" + DataName);
        if ('JNLNUM' == DataName) {
            var arrTransType = new Array("APPLYWORKPARAM");
            top.API.displayMessage("Start APPLYWORKPARAM");
            top.Tcp.CompositionData(arrTransType); //进行处理工作参数报文
        }        
    }

    function onDatSetPersistentDataError(DataName, ErrorCode) {
        top.API.displayMessage("onDatSetPersistentDataError is done,DataName=" + DataName + ",ErrorCode=" + ErrorCode);
        DownFaild();
        ButtonEnable();
    }

    //@User code scope end 
    /********************************************************************************************************/
    //组包模块
    function onCompositionDataCompleted(arrData) {
        top.API.displayMessage("onCompositionDataCompleted is done");
        var objArrData = arrData.toArray();
        top.Tcp.SendToHost(objArrData, 60000);
    }
    function onCompositionDataFail() {
        top.API.displayMessage("onCompositionDataFail is done");
        DownFaild()
    }

    function onMACGenerated(MacData) {
        top.API.displayMessage("onMACGenerated is done");
        var objMacData = MacData.toArray();
        top.Tcp.SendToHost(objMacData, 60000);
    }
    /********************************************************************************************************/
    //TCP模块
    function onTcpOnRecved(tmpCheck) {
        Check = tmpCheck;
        top.API.displayMessage("onTcpOnRecved is done,Check:" + Check);
        switch (Check) {
            case '00':
                if (CheckKey()) {
                    DownPinKey();
                } else {
                    DownFaild();
                }
                break;
            default:
                top.API.displayMessage("下载工作参数异常");
                DownFaild();
                break;
        }
    }

    function onTcpSendFailed() {
        top.API.displayMessage("onTcpSendFailed is done");
        DownFaild();

    }
    function onTcpTimeout() {
        top.API.displayMessage("onTcpTimeout is done");
        DownFaild();
    }
    function onAnalysisFailed() {
        top.API.displayMessage("onAnalysisFailed is done");
        DownFaild();

    }
    /********************************************************************************************************/
    //PIN模块
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
        top.API.displayMessage("下载PINKEY");
        keyFlag = 1;
        var HexWorkKey = top.stringToHex(PinKey);
        var tmphexArray = new Array();
        top.Pin.ExtendedLoadEncryptedKey("PINKEY", HexWorkKey, "MasterKey", "CRYPT,FUNCTION,MACING,KEYENCKEY", tmphexArray);
    }

    function DownMACKey() {
        top.API.displayMessage("下载MACKEY");
        keyFlag = 2;
        var HexWorkKey = top.stringToHex(MACKey);
        var tmphexArray = new Array();
        top.Pin.ExtendedLoadEncryptedKey("MACKEY", HexWorkKey, "MasterKey", "CRYPT,MACING", tmphexArray);
    }

    function onKeyLoaded() {
        top.API.displayMessage('触发事件：onKeyLoaded()');
        if (keyFlag == 1) {
            top.API.displayMessage('下载PINKEY成功');
            DownMACKey();
        } else {
            top.API.displayMessage('下载MACKEY成功');
            DownSuccessd();
        }
    }

    function onDeviceError() {
        top.API.displayMessage('键盘触发事件：onDeviceError()');
        DownFaild();
    }
    function onKeyLoadFailed() {
        top.API.displayMessage('键盘触发事件：onDeviceError()');
        DownFaild();
    }
    /////////////////////////////////////////
    //Register the event
    function EventLogin() {
        top.Tcp.SendFailed.connect(onTcpSendFailed);
        top.Tcp.OnRecved.connect(onTcpOnRecved);
        top.Tcp.CompositionDataCompleted.connect(onCompositionDataCompleted);
        top.Tcp.Timeout.connect(onTcpTimeout);
        top.Tcp.CompositionDataFail.connect(onCompositionDataFail);
        top.Tcp.AnalysisFailed.connect(onAnalysisFailed);
        ////
        top.Pin.MACGenerated.connect(onMACGenerated);
        top.Pin.KeyLoaded.connect(onKeyLoaded);
        top.Pin.KeyLoadFailed.connect(onKeyLoadFailed); 
        top.Pin.DeviceError.connect(onDeviceError);
        ////
        top.Dat.GetPersistentDataComplete.connect(onDatGetPersistentDataComplete);
        top.Dat.GetPersistentDataError.connect(onDatGetPersistentDataError);
        top.Dat.SetPersistentDataComplete.connect(onDatSetPersistentDataComplete);
        top.Dat.SetPersistentDataError.connect(onDatSetPersistentDataError)
    }

    function EventLogout() {
        ////
        top.Tcp.SendFailed.disconnect(onTcpSendFailed);
        top.Tcp.OnRecved.disconnect(onTcpOnRecved);
        top.Tcp.CompositionDataCompleted.disconnect(onCompositionDataCompleted);
        top.Tcp.Timeout.disconnect(onTcpTimeout);
        top.Tcp.CompositionDataFail.disconnect(onCompositionDataFail);
        top.Tcp.AnalysisFailed.disconnect(onAnalysisFailed);
        ////
        top.Pin.MACGenerated.disconnect(onMACGenerated);
        top.Pin.KeyLoaded.disconnect(onKeyLoaded);
        top.Pin.KeyLoadFailed.disconnect(onKeyLoadFailed); 
        top.Pin.DeviceError.disconnect(onDeviceError);
        ////
        top.Dat.GetPersistentDataComplete.disconnect(onDatGetPersistentDataComplete);
        top.Dat.GetPersistentDataError.disconnect(onDatGetPersistentDataError);
        top.Dat.SetPersistentDataComplete.disconnect(onDatSetPersistentDataComplete);
        top.Dat.SetPersistentDataError.disconnect(onDatSetPersistentDataError)
    }
    //remove all event handler
    function Clearup() {
        EventLogout();
    }
})();
