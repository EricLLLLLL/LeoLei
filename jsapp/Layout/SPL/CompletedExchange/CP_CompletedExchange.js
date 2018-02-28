/*@create by:  tsxiong
*@time: 2016年03月20日
*/
; (function () {
	var JnlNum = 0; //记流水,交易流水号
	var Check = ""; //记流水,交易响应码
	var strMsg = ""; //记流水,交易信息		
	var arrTransType;
	
	var ChangeNum = new Array();
	var CallResponse = App.Cntl.ProcessOnce( function(Response){
        //TO DO:
        Clearup();
        //Entry the flows control process.
        App.Cntl.ProcessDriven(Response);
    });
	var Initialize = function () {
		ButtonDisable();
		EventLogin();
		top.Dat.GetPersistentData(top.JnlnumTag, top.JnlnumType);
    } (); //Page Entry
    function ButtonDisable() {
        document.getElementById('OK').disabled = true;
        document.getElementById('Exit').disabled = true;
    }

    function ButtonEnable() {
        document.getElementById('OK').disabled = false;
        document.getElementById('Exit').disabled = false;
    }
	
	function ExchangeStart() {
        top.Cdm.PUCurrentCount();
        top.Cdm.CUNoteValue();
        top.Cdm.CUType();   
        for (i = 0; i < top.API.CashInfo.arrUnitName.length; i++) {
			ChangeNum[i] = top.API.CashInfo.arrUnitName[i] + ":0";
		}
        top.Cim.StartExchange();
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
	
	function ShowFaildInfo(){
		document.getElementById("faildDiv").style.display = "block";
		document.getElementById("ChangeDiv").style.display = "none";
		document.getElementById("Exit").style.display = "block";
	}
    function onExchangeInitiated() {
        var nRet = top.Cim.SetUnitCountSync(ChangeNum);
        if (nRet == 0) {
            var arrTransKindType = new Array("清除统计数据");
            top.Dat.SetDataSync(top.API.TransKindTag, top.API.TransKindType, arrTransKindType);
            top.Jnl.PrintSync("MachineSettle");
            var PtrPaperStatus = top.Ptr.StPaperStatus();
            if (PtrPaperStatus != "FULL" || PtrPaperStatus != "LOW") {
                top.Ptr.Print("TOTAL_szABC_haveReDep", "",top.API.gPrintTimeOut);
            }
            top.Cim.EndExchange();
        } else {
            ShowFaildInfo();
        }
        ButtonEnable();
    }

    function onExchangeCompleted() {
        var arrTotalFlag = new Array();
        arrTotalFlag[0] = 0;
        top.Dat.SetPersistentData("TOTALADDOK", "LONG", arrTotalFlag);
		
		// 保存清机时间
		var strTmp = getNowFormatDate();
		top.API.displayMessage("本次清机时间:" + strTmp);
		top.Dat.WritePrivateProfileSync("Exchange", "ExchangeDate", strTmp, top.API.gIniFileName);
    }

	function getNowFormatDate() {
		var date = new Date();
		var seperator1 = "-";
		var seperator2 = ":";
		var month = date.getMonth() + 1;
		var strDate = date.getDate();
		var strHour = date.getHours();
		var strMin = date.getMinutes();
		var strSec = date.getSeconds();
		
		if (month >= 1 && month <= 9) {
			month = "0" + month;
		}
		
		if (strDate >= 0 && strDate <= 9) {
			strDate = "0" + strDate;
		}
		
		if (strHour >= 0 && strHour <= 9) {
			strHour = "0" + strHour;
		}
		
		if (strMin >= 0 && strMin <= 9) {
			strMin = "0" + strMin;
		}
		
		if (strSec >= 0 && strSec <= 9) {
			strSec = "0" + strSec;
		}
		
		var currentdate = date.getFullYear() + seperator1 + month + seperator1 + strDate + " " + strHour + seperator2 + strMin + seperator2 + strSec;
		return currentdate;
	} 

    function onDeviceError() {
		var strAcceptStatus = top.Cim.LastAcceptStatus();
        top.API.displayMessage("LastAcceptStatus=" + strAcceptStatus);
		if (strAcceptStatus == "ACTIVE") {
			top.API.displayMessage("执行CashInEnd");
            top.Cim.StoreEscrowedCash(top.API.gStoreEscrowedCashTimeOut);
			ExchangeStart();
        } else{
			ButtonEnable();
			ShowFaildInfo();
		}
    }
    /********************************************************************************************************/    
    /********************************************************************************************************/
    //永久数据模块
    function onDatGetPersistentDataComplete(DataName, DataType, DataValue) {
        var arrDataValue = DataValue.toArray();
        if ('JNLNUM' == DataName) {
            JnlNum = arrDataValue[0] + 1;
            if (JnlNum.toString().length === 7) {
                JnlNum = 0;
            }
            var arrJnlNum = new Array();
            arrJnlNum[0] = JnlNum;
            top.Dat.SetPersistentData(top.JnlnumTag, top.JnlnumType, arrJnlNum);
		}
    }

    function onDatGetPersistentDataError(DataName, ErrorCode) {
        ButtonEnable();
		ShowFaildInfo();
    }
    function onDatSetPersistentDataComplete(DataName) {
        if ('JNLNUM' == DataName) {
            var arrTransType = new Array("QRYHOSTAMOUNT");
            top.Tcp.CompositionData(arrTransType); //清机报文
        }
        if ('TOTALADDOK' == DataName) {
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
    		document.getElementById("SuccessedDiv").style.display = "block";
            document.getElementById("ChangeDiv").style.display = "none";
            document.getElementById("OK").style.display = "block"; 
			ButtonEnable();
        }          
    }

    function onDatSetPersistentDataError(DataName, ErrorCode) {
        ButtonEnable();
		ShowFaildInfo();
    }

    //@User code scope end 
	/********************************************************************************************************/
    //组包模块
	function onCompositionDataCompleted(arrData) {
        var objArrData = arrData.toArray();
        top.Pin.GenerateMAC(objArrData, "MACKEY", '', 0);
    }
	function onCompositionDataFail() {
        top.API.displayMessage("CompositionDataFail");
        ExchangeStart();
    }

    function onMACGenerated(MacData) {
        var objMacData = MacData.toArray();
        top.Tcp.SendToHost(objMacData, 60000);
    }
	/********************************************************************************************************/
    //TCP模块
    function onTcpOnRecved(tmpCheck) {
        Check = tmpCheck;
        top.API.displayMessage("Check:" + Check);
        strErrMsg = "";
        switch (Check) {
            case '00':
				ExchangeStart();
                strErrMsg = "交易成功";
                break;
            default:
                top.API.displayMessage("下载工作参数异常");  
				ExchangeStart();
                break;
        }       
    }
	
    function onTcpSendFailed() {
        top.API.displayMessage("onTcpSendFailed is done");
		ExchangeStart();
        
    }
    function onTcpTimeout() {
        top.API.displayMessage("onTcpTimeout is done");
        ExchangeStart();
    }
	function onAnalysisFailed() {
        top.API.displayMessage("onAnalysisFailed");       
		ExchangeStart();
        
    }

    function onStartExchangeFailed() {
        top.API.displayMessage('触发事件：StartExchangeFailed()');
        ButtonEnable();
		ShowFaildInfo();
    }

    function onEndExchangeFailed() {
        top.API.displayMessage('触发事件：EndExchangeFailed()');
        ButtonEnable();
		ShowFaildInfo();
    }
	
    //Register the event
    function EventLogin() {
        top.Cim.StartExchangeCompleted.connect(onExchangeInitiated);
        top.Cim.StartExchangeFailed.connect(onStartExchangeFailed);
        top.Cim.DeviceError.connect(onDeviceError);
        top.Cim.EndExchangeCompleted.connect(onExchangeCompleted);
        top.Cim.EndExchangeFailed.connect(onEndExchangeFailed);
		////
        top.Tcp.SendFailed.connect(onTcpSendFailed);
        top.Tcp.OnRecved.connect(onTcpOnRecved);
        top.Tcp.CompositionDataCompleted.connect(onCompositionDataCompleted);
        top.Tcp.Timeout.connect(onTcpTimeout);
		top.Tcp.CompositionDataFail.connect(onCompositionDataFail);
		top.Tcp.AnalysisFailed.connect(onAnalysisFailed);
		top.Pin.MACGenerated.connect(onMACGenerated);
        ////
        top.Dat.GetPersistentDataComplete.connect(onDatGetPersistentDataComplete);
        top.Dat.GetPersistentDataError.connect(onDatGetPersistentDataError);
        top.Dat.SetPersistentDataComplete.connect(onDatSetPersistentDataComplete);
        top.Dat.SetPersistentDataError.connect(onDatSetPersistentDataError);
    }

    function EventLogout() {
        top.Cim.StartExchangeCompleted.disconnect(onExchangeInitiated);
        top.Cim.StartExchangeFailed.disconnect(onStartExchangeFailed);
        top.Cim.DeviceError.disconnect(onDeviceError);
        top.Cim.EndExchangeCompleted.disconnect(onExchangeCompleted);
        top.Cim.CompositionDataFail.disconnect(onEndExchangeFailed);
        ////
        top.Tcp.SendFailed.disconnect(onTcpSendFailed);
        top.Tcp.OnRecved.disconnect(onTcpOnRecved);
        top.Tcp.CompositionDataCompleted.disconnect(onCompositionDataCompleted);
        top.Tcp.Timeout.disconnect(onTcpTimeout);
		top.Tcp.CompositionDataFail.disconnect(onCompositionDataFail);
		top.Tcp.AnalysisFailed.disconnect(onAnalysisFailed);
        top.Pin.MACGenerated.disconnect(onMACGenerated);
        ////
        top.Dat.GetPersistentDataComplete.disconnect(onDatGetPersistentDataComplete);
        top.Dat.GetPersistentDataError.disconnect(onDatGetPersistentDataError);
        top.Dat.SetPersistentDataComplete.disconnect(onDatSetPersistentDataComplete);
        top.Dat.SetPersistentDataError.disconnect(onDatSetPersistentDataError);
    }

    //remove all event handler
    function Clearup() {
        //TO DO:
        EventLogout();
    }
})();
