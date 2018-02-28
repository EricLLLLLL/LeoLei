/*@create by:  tsxiong
*@time: 2016年03月20日
*/
; (function () {
	var CallResponse = App.Cntl.ProcessOnce( function(Response){
        Clearup();
        //Entry the flows control process.
        App.Cntl.ProcessDriven(Response);
    });
    var Initialize = function () {
        App.Timer.TimeoutDisposal(TimeoutCallBack);
        ButtonDisable();
        EventLogin();
        showInfo();
        ButtonEnable();
        App.Plugin.Voices.play("voi_9");
    }();//Page Entry

    //@User ocde scope start
    function ButtonDisable() {
        document.getElementById('Exit').disabled = true;
        document.getElementById('ContinueAddMoney').disabled = true;
        document.getElementById('OK').disabled = true;
    }

    function ButtonEnable() {
        document.getElementById('Exit').disabled = false;
        document.getElementById('ContinueAddMoney').disabled = false;
        document.getElementById('OK').disabled = false;
    }
    //显示入钞数据
    function showInfo() {
        var strName = '';
        strName += '*';
        strName += top.API.gCustomerName.substr(1, (top.API.gCustomerName.length - 1));
		
        if(top.API.CashInfo.Dealtype == "对公存款"){
			document.getElementById('tNameTittle').style.marginLeft = "45px";
			document.getElementById('tName').style.width = "272px";
			document.getElementById('tName').value = top.API.gCustomerName;
		}else{
			document.getElementById('tName').value = strName;
		}
        var strTransAmountTmp = parseInt(top.API.CashInfo.strTransAmount);
        if (strTransAmountTmp >= top.API.gChooseMoney && top.API.gChooseMoney != -1) {
            document.getElementById('ContinueAddMoney').style.display = "none";
        }else{
			document.getElementById('ContinueAddMoney').style.display = "block";
		}
		
        for (var i = 0; i < top.API.CashInfo.arrCurrencyCashIn.length; i++) {
            top.API.gAcceptCounts += parseInt(top.API.CashInfo.arrCurrencyCashIn[i]);
        }
		
        document.getElementById('PromptDiv1').style.display = "none";
        document.getElementById('PageSubject').style.display = "block";
		if (strTransAmountTmp > 0) {
			document.getElementById('OK').style.display = "block";
            var showmoney = "CNY " + top.API.CashInfo.strTransAmount + ".00";
            document.getElementById('tCash').value = showmoney;
            document.getElementById('t100').value = top.API.CashInfo.arrCurrencyCashIn[0];
            document.getElementById('t50').value = top.API.CashInfo.arrCurrencyCashIn[1];
            document.getElementById('t20').value = top.API.CashInfo.arrCurrencyCashIn[2];
            document.getElementById('t10').value = top.API.CashInfo.arrCurrencyCashIn[3];
            document.getElementById('t5').value = top.API.CashInfo.arrCurrencyCashIn[4];
            document.getElementById('t1').value = top.API.CashInfo.arrCurrencyCashIn[5];			           
        } else {
			document.getElementById('Exit').style.display = "block";
            document.getElementById('tCash').value = "CNY0.00";
            document.getElementById('t100').value = "0";
            document.getElementById('t50').value = "0";
            document.getElementById('t20').value = "0";
            document.getElementById('t10').value = "0";
            document.getElementById('t5').value = "0";
            document.getElementById('t1').value = "0";
        }
    }
	
    document.getElementById('Exit').onclick = function () {
        ButtonDisable();
        top.ErrorInfo = top.API.PromptList.No2;
        top.API.gOldCimRefusedNums = 0;
		top.Cim.StoreEscrowedCash(top.API.gStoreEscrowedCashTimeOut);
		return CallResponse('Exit');
        
    }

    document.getElementById('ContinueAddMoney').onclick = function () {
        ButtonDisable();
        return CallResponse('ContinueAddMoney');
    }

    document.getElementById('OK').onclick = function () {
		ButtonDisable();
		if (top.API.CashInfo.Dealtype == "DEP存款") {
			top.API.gTransactiontype = "DEP";
		}else if (top.API.CashInfo.Dealtype == "对公存款") {
			top.API.gTransactiontype = "BUSSINESSDEP";
		}else if (top.API.CashInfo.Dealtype == "无卡无折存款" || top.API.CashInfo.Dealtype == "存折存款") {
			top.API.gTransactiontype = "NOCARDDEP";
			if (top.API.gCardOrBookBank == 1) {
				top.API.gTransactiontype = "DEP";
			}
		}
		top.API.gOldCimRefusedNums = 0;

        var strJNLData = "总共放钞：100:" + top.API.CashInfo.arrCurrencyCashIn[0] + " 50:" + top.API.CashInfo.arrCurrencyCashIn[1]
			+ " 20:" + top.API.CashInfo.arrCurrencyCashIn[2] + " 10:" + top.API.CashInfo.arrCurrencyCashIn[3] + " 5:" + top.API.CashInfo.arrCurrencyCashIn[4]
			+ " 1:" + top.API.CashInfo.arrCurrencyCashIn[5] + " RJ:" + top.Cim.NumOfRefused(); 
        var arrCashOutBoxData = new Array(strJNLData);
        top.Dat.SetDataSync(top.API.ContentDataTag, top.API.ContentDataType, arrCashOutBoxData);
		top.Jnl.PrintSync("Content");
		top.Cim.StoreEscrowedCash(top.API.gStoreEscrowedCashTimeOut);
		return CallResponse('OK');
    }

    //@User code scope end 
    function onEscrowedCashStored(arrUnitResult, bRecycle) {
		top.API.displayMessage("EscrowedCashStored");//暂时不对CashInEnd成功进行处理
    }
     function onEscrowedCashStoreFailed() {
        top.API.displayMessage("onEscrowedCashStoreFailed");//暂时不对CashInEnd失败进行处理
    }

    function onDeviceError() {
        top.API.displayMessage("DeviceError");//暂时不对CashInEnd失败进行处理
       
    }

    //Register the event
    function EventLogin() {
        top.Cim.EscrowedCashStored.connect(onEscrowedCashStored);
        top.Cim.EscrowedCashStoreFailed.connect(onEscrowedCashStoreFailed);
        top.Cim.DeviceError.connect(onDeviceError);
    }

    function EventLogout() {
        top.Cim.EscrowedCashStored.disconnect(onEscrowedCashStored);
        top.Cim.EscrowedCashStoreFailed.disconnect(onEscrowedCashStoreFailed);
        top.Cim.DeviceError.disconnect(onDeviceError);
    }

    //Countdown function
    function TimeoutCallBack() {
		if (parseInt(top.API.CashInfo.strTransAmount) > 0) {
			if (top.API.CashInfo.Dealtype == "DEP存款") {
				top.API.gTransactiontype = "DEP";
			}else if (top.API.CashInfo.Dealtype == "对公存款") {
				top.API.gTransactiontype = "BUSSINESSDEP";
			}else if (top.API.CashInfo.Dealtype == "无卡无折存款"  || top.API.CashInfo.Dealtype == "存折存款") {
				top.API.gTransactiontype = "NOCARDDEP";
				if (top.API.gCardOrBookBank == 1) {
					top.API.gTransactiontype = "DEP";
				}
			}
			top.API.gOldCimRefusedNums = 0;
            var strJNLData = "总共放钞：100:" + top.API.CashInfo.arrCurrencyCashIn[0] + " 50:" + top.API.CashInfo.arrCurrencyCashIn[1]
				+ " 20:" + top.API.CashInfo.arrCurrencyCashIn[2] + " 10:" + top.API.CashInfo.arrCurrencyCashIn[3] + " 5:" + top.API.CashInfo.arrCurrencyCashIn[4]
				+ " 1:" + top.API.CashInfo.arrCurrencyCashIn[5] + " RJ:" + top.Cim.NumOfRefused(); 
            var arrCashOutBoxData = new Array(strJNLData);
            top.Dat.SetDataSync(top.API.ContentDataTag, top.API.ContentDataType, arrCashOutBoxData);
		    top.Jnl.PrintSync("Content");

			top.Cim.StoreEscrowedCash(top.API.gStoreEscrowedCashTimeOut);
			return CallResponse('OK');			
		}else{
			top.ErrorInfo = top.API.PromptList.No3;
			top.API.gOldCimRefusedNums = 0;
			top.Cim.StoreEscrowedCash(top.API.gStoreEscrowedCashTimeOut);
			return CallResponse('TimeOut');
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
