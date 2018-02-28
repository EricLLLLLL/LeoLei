/*@create by:  tsxiong
*@time: 2016��03��20��
*/
; (function () {    
	var CallResponse = App.Cntl.ProcessOnce( function(Response){
		EventLogout();
        App.Cntl.ProcessDriven(Response);
    });
    var Initialize = function () {
		EventLogin();
        CheckStatus();
    } (); //Page Entry

    //@User ocde scope start
    function CheckStatus() {
		top.API.CheckDeviceStatus();
		var arrCurrentCount = new Array();
		var arrSplite = new Array();
		var arrReturnInfo = new Array();
		arrCurrentCount = top.Cdm.PUCurrentCount().toArray();
		for (i = 0; i < arrCurrentCount.length; i++) {
			arrSplite = arrCurrentCount[i].split(":");
			top.API.CashInfo.arrUnitName[i] = arrSplite[0];
			top.API.CashInfo.arrUnitRemain[i] = arrSplite[1];
		}
        //��ȡ��ǰ������Ϣ--������ֵ
        top.API.CashInfo.arrUnitCurrency = top.API.GetUnitInfo(top.Cdm.CUNoteValue().toArray());
        //��ȡ��ǰ������Ϣ--�������
        top.API.CashInfo.nCountOfUnits = top.API.CashInfo.arrUnitRemain.length;
        if (top.API.CashInfo.nCountOfUnits < 5) {
            top.API.displayMessage("������Ŀ����nCountOfUnits = " + top.API.CashInfo.nCountOfUnits);
            return CallResponse("UnitError");
			
        }
        if (top.Pin.KeyIsValidSync("MACKEY")) {
            top.API.gMACKEY = true;
        }
        top.API.CashInfo.InitData();
		top.Dat.GetPersistentData("SUBBANKNUM", "STRING");
    }
	//��������ģ��
    function onDatGetPersistentDataComplete(DataName, DataType, DataValue) {
        var arrDataValue = DataValue.toArray();
        if ('SUBBANKNUM' == DataName) {
            top.API.gSubBankNum = arrDataValue[0];
             //��ȡ��Ա��
            top.Dat.GetPersistentData("TERMINALNUM", "STRING");
        }
        if ('TERMINALNUM' == DataName) {
            top.API.gSubBankNum += "-" + arrDataValue[0] + "�Ż�";
            return CallResponse("OK");
        }      
    }

    function onDatGetPersistentDataError(DataName, ErrorCode) {
        top.API.displayMessage("onDatGetPersistentDataError is done,DataName=" + DataName + ",ErrorCode=" + ErrorCode);
        return CallResponse("OK");
    }
	function EventLogin() {
        top.Dat.GetPersistentDataComplete.connect(onDatGetPersistentDataComplete);
        top.Dat.GetPersistentDataError.connect(onDatGetPersistentDataError);
    }

    function EventLogout() {
        top.Dat.GetPersistentDataComplete.disconnect(onDatGetPersistentDataComplete);
        top.Dat.GetPersistentDataError.disconnect(onDatGetPersistentDataError);
    }
})();