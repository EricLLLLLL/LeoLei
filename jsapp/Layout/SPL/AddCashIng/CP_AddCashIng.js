/*@create by:  tsxiong
*@time: 2016年03月20日
*/
; (function () {
    var ChangeNum = new Array();
	var CallResponse = App.Cntl.ProcessOnce( function(Response){
        //TO DO:
        Clearup();
        //Entry the flows control process.
        App.Cntl.ProcessDriven(Response);
    });
    var Initialize = function () {
        EventLogin();
		top.API.gAddNoteSuccess = false;
        for (i= 0; i < top.API.CashInfo.nCountOfUnits; i++) {
            ChangeNum[i] = top.API.CashInfo.arrUnitName[i] + ":" + top.API.CashInfo.arrUnitRemain[i].toString();          
        }
        top.API.displayMessage('ChangeNum=' + ChangeNum);
        top.Cim.StartExchange();
    } (); //Page Entry


    function onExchangeInitiated() {
        var nRet = top.Cim.SetUnitCountSync(ChangeNum);
        if (nRet == 0) {
            top.Cim.EndExchange();
        } else {
            return CallResponse("OK");
        }
    }

    function onExchangeCompleted() {
        //获取当前钞箱信息--钞箱余额
		var arrCurrentCount = new Array();
		var arrSplite = new Array();
		var arrReturnInfo = new Array();
		arrCurrentCount = top.Cdm.PUCurrentCount().toArray();
		for (i = 0; i < arrCurrentCount.length; i++) {
			arrSplite = arrCurrentCount[i].split(":");
			top.API.CashInfo.arrUnitName[i] = arrSplite[0];
			top.API.CashInfo.arrUnitRemain[i] = arrSplite[1];
		}
        //获取当前钞箱信息--钞箱面值
        top.API.CashInfo.arrUnitCurrency = top.API.GetUnitInfo(top.Cdm.CUNoteValue().toArray());
        //获取当前钞箱信息--钞箱个数
        top.API.CashInfo.nCountOfUnits = top.API.CashInfo.arrUnitRemain.length;
		top.API.gAddNoteSuccess = true;
        return CallResponse("OK");
    }

    function onDeviceError() {
        top.API.displayMessage('触发事件：onDeviceError()');
        return CallResponse("OK");
    }

    function onStartExchangeFailed() {
        top.API.displayMessage('触发事件：StartExchangeFailed()');
        return CallResponse("OK");
    }

    function onEndExchangeFailed() {
        top.API.displayMessage('触发事件：EndExchangeFailed()');
        return CallResponse("OK");
    }

    //@User code scope end 

    //Register the event
    function EventLogin() {
        top.Cim.StartExchangeCompleted.connect(onExchangeInitiated);
        top.Cim.StartExchangeFailed.connect(onStartExchangeFailed);
        top.Cim.DeviceError.connect(onDeviceError);
        top.Cim.EndExchangeCompleted.connect(onExchangeCompleted);
        top.Cim.EndExchangeFailed.connect(onEndExchangeFailed);
    }

    function EventLogout() {
        top.Cim.StartExchangeCompleted.disconnect(onExchangeInitiated);
        top.Cim.StartExchangeFailed.disconnect(onStartExchangeFailed);
        top.Cim.DeviceError.disconnect(onDeviceError);
        top.Cim.EndExchangeCompleted.disconnect(onExchangeCompleted);
        top.Cim.CompositionDataFail.disconnect(onEndExchangeFailed);
    }

    
    //remove all event handler
    function Clearup() {
        //TO DO:
        EventLogout();
    }
})();
