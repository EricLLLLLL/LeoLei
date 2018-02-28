/*@create by:  tsxiong
*@time: 2016年03月20日
*/
; (function () {
	var CallResponse = App.Cntl.ProcessOnce( function(Response){
        //TO DO:
        Clearup();
        App.Cntl.ProcessDriven(Response);
    });
	
    var Initialize = function () {
        ButtonDisable();
		App.Timer.TimeoutDisposal(TimeoutCallBack);
		EventLogin();
		top.Cim.PrepareForAcceptCash();		
        App.Plugin.Voices.play("voi_6");
    }();//Page Entry

    //@User ocde scope start
    function ButtonDisable() {
        document.getElementById('Back').disabled = true;
		document.getElementById('Exit').disabled = true;
        document.getElementById('OK').disabled = true;
    }
    function ButtonEnable() {
        document.getElementById('Back').disabled = false;
		document.getElementById('Exit').disabled = false;
        document.getElementById('OK').disabled = false;
    }
    document.getElementById('OK').onclick = function () {
        ButtonDisable();
		var tmp = top.Cim.StInputStatus();
        if (tmp == "EMPTY") {
			ButtonEnable();
			//汉字提示            
            App.Plugin.Voices.play("voi_46");
        }else{
			return CallResponse('OK');
		}        
    }

    document.getElementById('Exit').onclick = function () {
		ButtonDisable();
        top.ErrorInfo = top.API.PromptList.No2;
		top.Cim.StoreEscrowedCash(top.API.gStoreEscrowedCashTimeOut);
		return CallResponse('Exit');
    }
	
	document.getElementById('Back').onclick = function () {
		ButtonDisable();
        top.ErrorInfo = top.API.PromptList.No2;
		top.Cim.StoreEscrowedCash(top.API.gStoreEscrowedCashTimeOut);
		return CallResponse('Back');
    }
    //@User code scope end 

    
	/********************************************************************************************************/
    //CIM模块
	function onDeviceError() {
        top.API.displayMessage("onDeviceError触发");
		top.ErrorInfo = top.API.PromptList.No4;
		return CallResponse('Exit');
    }
	
	function onAcceptCashPrepared() {
        top.API.displayMessage("onAcceptCashPrepared");
		top.Dat.GetPersistentData(top.JnlnumTag, top.JnlnumType);        
    }
	function onAcceptCashAlreadyActive() {
        top.API.displayMessage("onAcceptCashAlreadyActive");
		top.ErrorInfo = top.API.PromptList.No4;
		top.Cim.StoreEscrowedCash(top.API.gStoreEscrowedCashTimeOut);
		return CallResponse('Exit');
    }
	function onAcceptCashPrepareFailed() {
        top.API.displayMessage("onAcceptCashPrepareFailed");
		top.ErrorInfo = top.API.PromptList.No4;
		return CallResponse('Exit');
    }
    /********************************************************************************************************/
    //永久数据模块
    function onDatGetPersistentDataComplete(DataName, DataType, DataValue) {
        var arrDataValue = DataValue.toArray();
        var JnlNum = 0;
        if ('JNLNUM' == DataName) {
            JnlNum = arrDataValue[0] + 1;
            if (JnlNum.toString().length === 7) {
                JnlNum = 0;
            }
            //设置交易流水号
            var arrJnlNum = new Array();
            arrJnlNum[0] = JnlNum;
			top.Dat.SetPersistentData(top.JnlnumTag, top.JnlnumType, arrJnlNum);
        }
    }

    function onDatGetPersistentDataError(DataName, ErrorCode) {
        top.API.displayMessage("onDatGetPersistentDataError is done,DataName=" + DataName + ",ErrorCode=" + ErrorCode);
		top.ErrorInfo = top.API.PromptList.No4;
		return CallResponse('Exit');
    }

    function onDatSetPersistentDataComplete(DataName) {
		ButtonEnable();
        //top.API.displayMessage("onDatSetPersistentDataComplete is done,DataName=" + DataName);   	
    }

    function onDatSetPersistentDataError(DataName, ErrorCode) {
        top.API.displayMessage("onDatSetPersistentDataError is done,DataName=" + DataName + ",ErrorCode=" + ErrorCode);       
		top.ErrorInfo = top.API.PromptList.No4;
		return CallResponse('Exit');
    }
    //Register the event
    function EventLogin() {
        top.Cim.DeviceError.connect(onDeviceError);
		top.Cim.AcceptCashPrepared.connect(onAcceptCashPrepared);
		top.Cim.AcceptCashAlreadyActive.connect(onAcceptCashAlreadyActive);
		top.Cim.AcceptCashPrepareFailed.connect(onAcceptCashPrepareFailed);
        //
        top.Dat.GetPersistentDataComplete.connect(onDatGetPersistentDataComplete);
        top.Dat.GetPersistentDataError.connect(onDatGetPersistentDataError);
        top.Dat.SetPersistentDataComplete.connect(onDatSetPersistentDataComplete);
        top.Dat.SetPersistentDataError.connect(onDatSetPersistentDataError);
    }

    function EventLogout() {
        top.Cim.DeviceError.disconnect(onDeviceError);
		top.Cim.AcceptCashPrepared.disconnect(onAcceptCashPrepared);
		top.Cim.AcceptCashAlreadyActive.disconnect(onAcceptCashAlreadyActive);
		top.Cim.AcceptCashPrepareFailed.disconnect(onAcceptCashPrepareFailed);
        //
        top.Dat.GetPersistentDataComplete.disconnect(onDatGetPersistentDataComplete);
        top.Dat.GetPersistentDataError.disconnect(onDatGetPersistentDataError);
        top.Dat.SetPersistentDataComplete.disconnect(onDatSetPersistentDataComplete);
        top.Dat.SetPersistentDataError.disconnect(onDatSetPersistentDataError);
    }

    //Countdown function
    
    //Page Return
    function TimeoutCallBack() {
        top.ErrorInfo = top.API.PromptList.No3;
	return CallResponse('TimeOut');        
    }
    //remove all event handler
    function Clearup() {
        //TO DO:
        EventLogout();
        App.Plugin.Voices.del();
        App.Timer.ClearTime();
    }
})();
