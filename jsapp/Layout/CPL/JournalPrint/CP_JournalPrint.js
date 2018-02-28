/*@create by:  tsxiong
*@time: 2016年03月20日
*/
; (function () {
    var TransType = null;
	var CallResponse = App.Cntl.ProcessOnce( function(Response){
        //TO DO:
        Clearup();
        //Entry the flows control process.
        App.Cntl.ProcessDriven(Response);
    });
    var Initialize = function () {
        App.Timer.TimeoutDisposal(TimeoutCallBack);
        EventLogin();
        //@initialize scope start
//         var nRet = top.Dat.GetPersistentData(top.JnlnumTag, top.JnlnumType);
//         top.API.displayMessage("GetPersistentData JNLNUM Return:" + nRet);


    }();//Page Entry

    //@User ocde scope start

    //@User code scope end

    //event handler
    function onDatGetPersistentDataComplete(DataName, DataType, DataValue) {
        var JnlNum = DataValue.toArray()[0];
        top.API.displayMessage("onDatGetPersistentDataComplete is done,DataName=" + DataName + ",DataType=" + DataType + ",DataValue=" + JnlNum);
        if ('JNLNUM' == DataName) {
            TransType = top.API.gTransactiontype;
            switch (TransType) {
                case "CWD":
                    var i;
                    //获取钞箱变更信息
                    var tmpCurrentInfo = new Array();
                    var CurrentInfo = new Array();
                    var objGet1 = top.Cdm.PUCurrentCount();
                    tmpCurrentInfo = objGet1.toArray();
                    for ( i = 0; i < tmpCurrentInfo.length; i++) {
                        CurrentInfo[i] = (tmpCurrentInfo[i]>top.API.CashInfo.arrUnitRemain[i])?(tmpCurrentInfo[i]-top.API.CashInfo.arrUnitRemain[i]):(top.API.CashInfo.arrUnitRemain[i]-tmpCurrentInfo[i]);
                    }         
                    top.Jnl.PrintSync("CashOutBox2");
                    break;
                default:
                    break;
            }
        }
    }
    function onDatGetPersistentDataError(DataName, ErrorCode) {
        top.API.displayMessage("onDatGetPersistentDataError is done,DataName=" + DataName + ",ErrorCode=" + ErrorCode);
        return CallResponse('OK');
    }

    function onDatSetPersistentDataComplete(DataName) {
        top.API.displayMessage("onDatSetPersistentDataComplete is done,DataName=" + DataName);
        return CallResponse('OK');
    }

    function onDatSetPersistentDataError(DataName, ErrorCode) {
        top.API.displayMessage("onDatSetPersistentDataError is done,DataName=" + DataName + ",ErrorCode=" + ErrorCode);
        return CallResponse('OK');
    }

    function onPrintComplete() {
        top.API.displayMessage("onPrintComplete is done");
        return CallResponse('OK');
    }

    function onDeviceError() {
        top.API.displayMessage("onDeviceError is done");
        return CallResponse('OK');
    }

    function onTimeout() {
        top.API.displayMessage("onTimeout is done");
        return CallResponse('OK');
    }

 
    //Register the event
    function EventLogin() {
        top.Dat.GetPersistentDataComplete.connect(onDatGetPersistentDataComplete);
        top.Dat.GetPersistentDataError.connect(onDatGetPersistentDataError);
        top.Dat.SetPersistentDataComplete.connect(onDatSetPersistentDataComplete);
        top.Dat.SetPersistentDataError.connect(onDatSetPersistentDataError);
        top.Jnl.PrintComplete.connect(onPrintComplete);
        top.Jnl.DeviceError.connect(onDeviceError);
        top.Jnl.Timeout.connect(onTimeout);
    }
    function EventLogout() {
        top.Dat.GetPersistentDataComplete.disconnect(onDatGetPersistentDataComplete);
        top.Dat.GetPersistentDataError.disconnect(onDatGetPersistentDataError);
        top.Dat.SetPersistentDataComplete.disconnect(onDatSetPersistentDataComplete);
        top.Dat.SetPersistentDataError.disconnect(onDatSetPersistentDataError);
//         top.Jnl.PrintComplete.disconnect(onPrintComplete);
//         top.Jnl.DeviceError.disconnect(onDeviceError);
//         top.Jnl.Timeout.disconnect(onTimeout);
    }
    //Countdown function
    function TimeoutCallBack() {
        return CallResponse('TimeOut');
    }

    //Page Return
    

    //remove all event handler
    function Clearup() {
        //TO DO:
        EventLogout();
        App.Timer.ClearTime();
    }
})();