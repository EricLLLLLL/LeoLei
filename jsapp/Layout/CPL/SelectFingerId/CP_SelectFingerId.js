
/*@create by:  tsxiong
*@time: 2016��03��20��
*/
; (function () {
    var AdminObj;
    var TelInfo;
    var myselect = document.getElementById("cusId");
	var CallResponse = App.Cntl.ProcessOnce( function(Response){
        //TO DO:
        Clearup();
        //Entry the flows control process.
        App.Cntl.ProcessDriven(Response);
    });
    var Initialize = function () {
        EventLogin();
        //@initialize scope start
        App.Timer.TimeoutDisposal(TimeoutCallBack);
        ButtonDisable();
        var nRet1 = top.Dat.GetPersistentData("TELLERNO", "STRING");  //��ȡ��Ա��
        top.API.displayMessage("��Ա�ţ�GetPersistentData TELLERNO Return:" + nRet1);  //��ȡ��Ա���Ƿ�ɹ�
        ButtonEnable();
    } (); //Page Entry

    //@User ocde scope start
    function ButtonDisable() {
        top.API.displayMessage("ButtonDisable");
        document.getElementById('OK').disabled = true;
        document.getElementById('Exit').disabled = true;
    }

    function ButtonEnable() {
        top.API.displayMessage("ButtonEnable");
        document.getElementById('OK').disabled = false;
        document.getElementById('Exit').disabled = false;
    }

    function ShowOpt(){
        var opt = "";
        for (var i = 0; i < TelInfo.length; i++) {
            opt = opt + "<option value='"+TelInfo[i].telno+"'>"+TelInfo[i].telno+"</option>";
        }
        myselect.innerHTML = opt;
    }

    document.getElementById('Exit').onclick = function () {
        ButtonDisable();
        return CallResponse('Exit');
    }

    document.getElementById('OK').onclick = function () {
        ButtonDisable();
        var Index = myselect.selectedIndex;
		top.API.RemoteID = myselect.options[Index].text;
        return CallResponse('OK');
    }
    //@User code scope end 

    //event handler
    function onDatGetPersistentDataComplete(DataName, DataType, DataValue) {
        var arrDataValue = DataValue.toArray();
        top.API.displayMessage("onDatGetPersistentDataComplete is done,DataName=" + DataName + ",DataType=" + DataType + ",DataValue=" + arrDataValue);
        //top.API.TelInfoObjStr = arrDataValue.toString();
       // top.API.displayMessage("top.API.TelInfoObjStr =" + top.API.TelInfoObjStr);
        AdminObj = eval('(' + arrDataValue.toString() + ')');
        TelInfo = AdminObj.TelInfo;
		if(TelInfo.length == 0){
			top.ErrorInfo = "�������Ա��Ȩ";
			//return CallResponse('Exit');
		}
        ShowOpt();
    }

    function onDatGetPersistentDataError(DataName, ErrorCode) {
        top.API.displayMessage("onDatSetPersistentDataError is done,DataName=" + DataName + ",ErrorCode=" + ErrorCode);
        top.ErrorInfo = "�������Ա��Ȩ";
        return CallResponse('Exit');
    }
    //Register the event
    function EventLogin() {
        top.Dat.GetPersistentDataComplete.connect(onDatGetPersistentDataComplete);
        top.Dat.GetPersistentDataError.connect(onDatGetPersistentDataError);
    }

    function EventLogout() {
        top.Dat.GetPersistentDataComplete.disconnect(onDatGetPersistentDataComplete);
        top.Dat.GetPersistentDataError.disconnect(onDatGetPersistentDataError);
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
        App.Timer.ClearTime();
    }
})();
