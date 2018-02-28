/*@create by:  tsxiong
*@time: 2016��03��20��
*/
; (function () {
    var BService;
    var bReset = false;
	var bCimReset = false;
    var bCdmReset = false;
    var bCrdReset = false;
    var bFpiReset = false;
    var bIdrReset = false;
    var bPinReset = false;
    var timeoutId = null;
    var CallResponse = App.Cntl.ProcessOnce( function(Response){
        //TO DO:
        Clearup();
        //Entry the flows control process.
        App.Cntl.ProcessDriven(Response);
    });
    var Initialize = function () {
        ButtonDisable();
        EventLogin();
        showstatusinfo();     
		onCheckExchangTime();   
		if (top.API.gOutService == false) {
            document.getElementById('Service').value = "�رշ���";
            document.getElementById('SuperVise').style.display = "none";
        } else {
            document.getElementById('Service').value = "��������";
			document.getElementById('SuperVise').style.display = "inline";
        }
        ButtonEnable();
    }();//Page Entry
    function ButtonDisable() {
        document.getElementById('Service').disabled = true;
        document.getElementById('Reset').disabled = true;
        document.getElementById('Shutdown').disabled = true;
        document.getElementById('Reboot').disabled = true;
        document.getElementById('SuperVise').disabled = true;
        document.getElementById('ExitImg').disabled = true;
    }

    function ButtonEnable() {
        document.getElementById('Service').disabled = false;
        document.getElementById('Reset').disabled = false;
        document.getElementById('Shutdown').disabled = false;
        document.getElementById('Reboot').disabled = false;
        document.getElementById('SuperVise').disabled = false;
        document.getElementById('ExitImg').disabled = false;
    }
	function showinfo(){
		if(!bReset){
			top.API.gTransStatus = top.Sys.PossibleTransactionSync();        
			top.API.gPartsStatus = top.Sys.GetPartsStatusSync();
			showstatusinfo();
		}		
	}
    //@User ocde scope start
    function showstatusinfo() {
        if (top.API.gOutService) {
            document.getElementById('SuperVise').style.display = "inline";
            document.getElementById('Service').value = "��������";
        } else {
            document.getElementById('SuperVise').style.display = "none";
            document.getElementById('Service').value = "�رշ���";
        }
        showstatus();
        showmoney();
        showFault();
    }
	
    //��ʾ����
    function showFault(){
		var ErrorCode = "00000(00)";
		var ErrorCodeInfo = top.Sys.InfoGetSync(38);
		top.API.displayMessage("ErrorCodeInfo="+ErrorCodeInfo);
		if(ErrorCodeInfo != ""){
			var arrErrorCodeInfo = ErrorCodeInfo.split("|");
			var ErrorUintName = arrErrorCodeInfo[0];			
			var ErrorInfo = arrErrorCodeInfo[2];
			ErrorCode = arrErrorCodeInfo[1];
			document.getElementById('faultContent').innerText = ErrorUintName + ErrorInfo;
			document.getElementById('normalInfo').style.display = "none";
		}else{
			document.getElementById('faultContent').innerText = "��";
			document.getElementById('normalInfo').style.display = "block";
		}
		document.getElementById('DevStatus').innerText = ErrorCode;
    }


    function showmoney() {
        top.API.displayMessage("showmoney()");
        top.API.displayMessage("ֽ������top.API.CashInfo.arrUnitRemain" + top.API.CashInfo.arrUnitRemain);
        var cashboxid = "";
        var moneynumid = "";
        var Currencyid = "";
        var UintNameid = "";
        var Currency ="";
        var ClsCashBoxNum = "";
        var nAllMoney = 0;
        var i = 0;
        var j = 0;
        var nRej = 0;
        var bTmp = false;
        var tmpArray = new Array();
        for (i = 0; i < top.API.CashInfo.nCountOfUnits; i++) {
            tmpArray[i] = top.API.CashInfo.arrUnitRemain[i];            
        }

        for (i = 1; i < (top.API.CashInfo.nCountOfUnits + 1) ; i++) {
            cashboxid = "CashBox" + i.toString();
            UintNameid = "UintName" + i.toString();
            moneynumid = "MoneyNum" + i.toString();
            ClsCashBoxNum = "ClsCashBox" + i.toString();
           Currencyid = "CurrencyNo" + i.toString();
            document.getElementById(ClsCashBoxNum).style.display = "block";
           switch (top.API.CashInfo.arrUnitCurrency[i-1]) {
                case 100:
                    Currency = '100';                    
                    break;
                case 50:
                    Currency = '050';
                    break;
                case 20:
                    Currency = '020';
                    break;
                case 10:
                    Currency = '010';
                    break;
                case 5:
                    Currency = '005';
                    break;
                case 1:
                    Currency = '001';
                    break;
                default:
                   Currency = '000';
                    break;
                }
            nAllMoney += tmpArray[i - 1] * parseInt(Currency);
           document.getElementById(Currencyid).innerText = Currency;
           document.getElementById(UintNameid).innerText = top.API.CashInfo.arrUnitName[i-1];
            if (tmpArray[i - 1] == 0) {
                document.getElementById(cashboxid).style.backgroundImage = "url('Framework/style/Graphics/btn/cxzt0.png')";
            }
            else if (tmpArray[i - 1] <= 600) {
                document.getElementById(cashboxid).style.backgroundImage = "url('Framework/style/Graphics/btn/cxzt1.png')";
            }
            else if (tmpArray[i - 1] <= 1200) {
                document.getElementById(cashboxid).style.backgroundImage = "url('Framework/style/Graphics/btn/cxzt2.png')";
            }
            else if (tmpArray[i - 1] <= 1800) {
                document.getElementById(cashboxid).style.backgroundImage = "url('Framework/style/Graphics/btn/cxzt3.png')";
            }
            else if (tmpArray[i - 1] <= 2400) {
                document.getElementById(cashboxid).style.backgroundImage = "url('Framework/style/Graphics/btn/cxzt4.png')";
            }
            else {
                document.getElementById(cashboxid).style.backgroundImage = "url('Framework/style/Graphics/btn/cxzt5.png')";
            }
            document.getElementById(moneynumid).innerText = tmpArray[i - 1];
        }
            document.getElementById('AllMoney').innerText = nAllMoney.toString() + ".00Ԫ";
    }

    function showstatus() {
        top.API.displayMessage("showstatus()");
		//��ȡ������״̬
		top.API.CheckDeviceStatus();
		var doorstatus = top.Cim.StSafeDoorStatus();
        top.API.displayMessage("top.Cim.StSafeDoorStatus==" + doorstatus);
        if (doorstatus == "OPEN") {
            document.getElementById("WTip").style.display = "block";            
        }else{
			document.getElementById("WTip").style.display = "none";   
		}
		if (top.API.gTransStatus == "-1") {
            top.API.displayMessage("-------��ȡ������״̬ʧ��------");
        } else {
			/*
			TransStatus[0]=�˻�����ѯ
			TransStatus[1]=������ѯ
			TransStatus[2]=����
			TransStatus[3]=��ȡ��
			TransStatus[4]=��ȡ��
			TransStatus[5]=�����
			TransStatus[6]=�۴��
			TransStatus[7]=�޿����۴���ѯ
			TransStatus[8]=�޿����۴��
			TransStatus[9]=�㳮�һ�
			TransStatus[10]=����
            TransStatus[11]=�Թ����
            TransStatus[12]=��������
            TransStatus[13]=ת�˻��
            TransStatus[14]=ת�˳���
            TransStatus[15]=����ת����
            TransStatus[16]=����ת����           
            TransStatus[17]=�����ش�			
			*/
            var arrTransStatus = top.API.gTransStatus.split(",");
			if (parseInt(arrTransStatus[0]) == 1) {
				document.getElementById("TradeIcon1").style.backgroundImage = "url('Framework/style/Graphics/btn/icon1.png')";
			} else {
				document.getElementById("TradeIcon1").style.backgroundImage = "url('Framework/style/Graphics/btn/icon2.png')";
			}        
			if (parseInt(arrTransStatus[3]) == 1) {
				document.getElementById("TradeIcon2").style.backgroundImage = "url('Framework/style/Graphics/btn/icon1.png')";
			} else {
				document.getElementById("TradeIcon2").style.backgroundImage = "url('Framework/style/Graphics/btn/icon2.png')";
			}
			if (parseInt(arrTransStatus[5]) == 1) {
				document.getElementById("TradeIcon3").style.backgroundImage = "url('Framework/style/Graphics/btn/icon1.png')";
			} else {
				document.getElementById("TradeIcon3").style.backgroundImage = "url('Framework/style/Graphics/btn/icon2.png')";
			}
			if (parseInt(arrTransStatus[4]) == 1) {
				document.getElementById("TradeIcon4").style.backgroundImage = "url('Framework/style/Graphics/btn/icon1.png')";
			} else {
				document.getElementById("TradeIcon4").style.backgroundImage = "url('Framework/style/Graphics/btn/icon2.png')";
			}
            if (parseInt(arrTransStatus[9]) == 1) {
                document.getElementById("TradeIcon5").style.backgroundImage = "url('Framework/style/Graphics/btn/icon1.png')";
            } else {
                document.getElementById("TradeIcon5").style.backgroundImage = "url('Framework/style/Graphics/btn/icon2.png')";
            }
            if (parseInt(arrTransStatus[12]) == 1) {
                document.getElementById("TradeIcon6").style.backgroundImage = "url('Framework/style/Graphics/btn/icon1.png')";
            } else {
                document.getElementById("TradeIcon6").style.backgroundImage = "url('Framework/style/Graphics/btn/icon2.png')";
            }
            if (parseInt(arrTransStatus[13]) == 1) {
                document.getElementById("TradeIcon7").style.backgroundImage = "url('Framework/style/Graphics/btn/icon1.png')";
            } else {
                document.getElementById("TradeIcon7").style.backgroundImage = "url('Framework/style/Graphics/btn/icon2.png')";
            }
            if (parseInt(arrTransStatus[14]) == 1) {
                document.getElementById("TradeIcon8").style.backgroundImage = "url('Framework/style/Graphics/btn/icon1.png')";
            } else {
                document.getElementById("TradeIcon8").style.backgroundImage = "url('Framework/style/Graphics/btn/icon2.png')";
            }
            if (parseInt(arrTransStatus[15]) == 1) {
                document.getElementById("TradeIcon9").style.backgroundImage = "url('Framework/style/Graphics/btn/icon1.png')";
            } else {
                document.getElementById("TradeIcon9").style.backgroundImage = "url('Framework/style/Graphics/btn/icon2.png')";
            }
            if (parseInt(arrTransStatus[16]) == 1) {
                document.getElementById("TradeIcon10").style.backgroundImage = "url('Framework/style/Graphics/btn/icon1.png')";
            } else {
                document.getElementById("TradeIcon10").style.backgroundImage = "url('Framework/style/Graphics/btn/icon2.png')";
            }
            if (parseInt(arrTransStatus[17]) == 1) {
                document.getElementById("TradeIcon11").style.backgroundImage = "url('Framework/style/Graphics/btn/icon1.png')";
            } else {
                document.getElementById("TradeIcon11").style.backgroundImage = "url('Framework/style/Graphics/btn/icon2.png')";
            }
		}
		
		if (top.Cim.bDeviceStatus && top.Cdm.bDeviceStatus) {
			document.getElementById("CASH").style.backgroundImage = "url('Framework/style/Graphics/btn/icon1.png')";
		}else {
			document.getElementById("CASH").style.backgroundImage = "url('Framework/style/Graphics/btn/icon2.png')";
		}
		if (top.Crd.bDeviceStatus) {
			document.getElementById("CRD").style.backgroundImage = "url('Framework/style/Graphics/btn/icon1.png')";
		}else {
			document.getElementById("CRD").style.backgroundImage = "url('Framework/style/Graphics/btn/icon2.png')";
		}
		if (top.Fpi.bDeviceStatus) {
			document.getElementById("FPI").style.backgroundImage = "url('Framework/style/Graphics/btn/icon1.png')";
		}else {
			document.getElementById("FPI").style.backgroundImage = "url('Framework/style/Graphics/btn/icon2.png')";
		}
		if (top.Idr.bDeviceStatus) {
			document.getElementById("IDR").style.backgroundImage = "url('Framework/style/Graphics/btn/icon1.png')";
		}else {
			document.getElementById("IDR").style.backgroundImage = "url('Framework/style/Graphics/btn/icon2.png')";
		}
		if (top.Pin.bDeviceStatus && (top.Pin.KeyIsValidSync("MasterKey"))) {
			document.getElementById("PIN").style.backgroundImage = "url('Framework/style/Graphics/btn/icon1.png')";
		}else {
			document.getElementById("PIN").style.backgroundImage = "url('Framework/style/Graphics/btn/icon2.png')";    
		}
		if (top.Ptr.bDeviceStatus) {
			document.getElementById("PTR").style.backgroundImage = "url('Framework/style/Graphics/btn/icon1.png')";
			document.getElementById("JNL").style.backgroundImage = "url('Framework/style/Graphics/btn/icon1.png')";
		} else {
			document.getElementById("PTR").style.backgroundImage = "url('Framework/style/Graphics/btn/icon2.png')";
			document.getElementById("JNL").style.backgroundImage = "url('Framework/style/Graphics/btn/icon2.png')";
		}
		if (top.Scr.bDeviceStatus) {
			document.getElementById("SCR").style.backgroundImage = "url('Framework/style/Graphics/btn/icon1.png')";
		}else {
			document.getElementById("SCR").style.backgroundImage = "url('Framework/style/Graphics/btn/icon2.png')";
		}
		        
    }

    document.getElementById('Service').onclick = function () {
		top.API.gOutService = !top.API.gOutService;
        if (top.API.gOutService == false) {
            document.getElementById('Service').value = "�رշ���";
            document.getElementById('SuperVise').style.display = "none";
        } else {
            document.getElementById('Service').value = "��������";
			document.getElementById('SuperVise').style.display = "inline";
        }
    }

    document.getElementById('SuperVise').onclick = function () {
        return CallResponse('OK');
    }

    document.getElementById('Shutdown').onclick = function () {
		return CallResponse("Shutdown");
    }

    document.getElementById('Reboot').onclick = function () {
		return CallResponse("Reboot");
    }

    document.getElementById('Reset').onclick = function () {
        top.Jnl.PrintSync("Reset");
		ServiceReset();
    }

    document.getElementById('ExitImg').onclick = function () {
        top.Jnl.PrintSync("Exit");
        ButtonDisable();
        if (top.API.gOutService == false) {
            top.Sys.OpenService();
        }
        top.Sys.OpenFrontPage();
        top.Sys.SetIsMaintain(false);
        return CallResponse('Exit');
    }

    //@User code scope end 
    function ServiceReset() {
        top.API.displayMessage("��ʼReset");
		bReset = true;
		bCdmReset = false;
		bCrdReset = false;
		bFpiReset = false;
		bIdrReset = false;
		bPinReset = false;
		top.Sys.Reset();
		App.Plugin.Wait.show();
        top.Cdm.Reset("RETRACT", 0,top.API.gResetTimeout);
        top.Crd.Reset("RETRACT", top.API.gResetTimeout);
        top.Fpi.Reset(top.API.gResetTimeout);
        top.Idr.Reset("NOACTION");
        top.Pin.Reset(top.API.gResetTimeout);
        timeoutId = setTimeout(function(){
            App.Plugin.Wait.disappear();
        },120000);
    }

    function onCheckStatus() {
		top.API.gTransStatus = top.Sys.PossibleTransactionSync();        
		top.API.gPartsStatus = top.Sys.GetPartsStatusSync();
        showinfo();
    }

    function onResetStatus() {
        if (bCdmReset && bCrdReset && bFpiReset && bIdrReset && bPinReset) {
            clearTimeout(timeoutId);
            bCdmReset = false;
            bCrdReset = false;
            bFpiReset = false;
            bIdrReset = false;
            bPinReset = false;
            App.Plugin.Wait.disappear();
			bReset = false;
			if(top.API.AdminStatus == 1 && top.Cdm.StOutputStatus() == "NOTEMPTY"){
				top.Cdm.OpenShutter(top.API.gOpenShutterTimeOut);	
			}else{
				showinfo();
			}
        }
    }
    //door
	function onShutterOpened(){
		top.API.displayMessage("onShutterOpened����");
	}	
	function onCashTaken() {
        App.Timer.ClearIntervalTime();
        top.Cdm.CloseShutter(top.API.gCloseShutterTimeOut);		
    }
	function onShutterClosed(){
		top.API.displayMessage("onShutterClosed����");
		showinfo();
	}
	function onShutterOpenFailed(){
		top.API.displayMessage("onShutterOpenFailed����");
		showinfo();		
	}	
	function onShutterCloseFailed(){
		top.API.displayMessage("onShutterCloseFailed����");
		showinfo();
	}
    //RESET COMPLETE
    function onCimResetComplete() {
        top.API.displayMessage("onCimResetComplete is done");
        bCimReset = true;
        onResetStatus();
    }
    function onCdmResetComplete() {
        top.API.displayMessage("onCdmResetComplete is done");
        bCdmReset = true;
        onResetStatus();
    }
    function onCrdResetComplete() {
        top.API.displayMessage("onCrdResetComplete is done");
        bCrdReset = true;
        onResetStatus();
    }

     function onCrdResetFailed() {
        top.API.displayMessage("onCrdResetFailed is done");
        bCrdReset = true;
        onResetStatus();
    }

    function onFpiResetComplete() {
        top.API.displayMessage("onFpiResetComplete is done");
        bFpiReset = true;
        onResetStatus();
    }
    function onIdrResetComplete() {
        top.API.displayMessage("onIdrResetComplete is done");
        bIdrReset = true;
        onResetStatus();
    }

    function onJnlResetComplete() {
        top.API.displayMessage("onJnlResetComplete is done");
    }

    function onPinResetComplete() {
        top.API.displayMessage("onPinResetComplete is done");
        bPinReset = true;
        onResetStatus();
    }

    function onPtrResetComplete() {
        top.API.displayMessage("onPtrResetComplete is done");
    }

    function onTcpResetComplete() {
        top.API.displayMessage("onTcpResetComplete is done");
    }

    function onCrdResetFailed() {
        top.API.displayMessage("onCrdResetFailed is done");
        bCrdReset = true;
        onResetStatus();
    }

    function onPinResetFailed() {
        top.API.displayMessage("onPinResetFailed is done");
        bPinReset = true;
        onResetStatus();
    }


    function onCimResetFailed() {
        top.API.displayMessage("onCimResetFailed is done");
        bCimReset = true;
        onResetStatus();
    }
    function onCdmResetFailed() {
        top.API.displayMessage("onCdmResetFailed is done");
        bCdmReset = true;
        onResetStatus();
    }

    function onFpiResetFailed() {
        top.API.displayMessage("onFpiResetFailed is done");
        bFpiReset = true;
        onResetStatus();
    }
    function onIdrResetFailed() {
        top.API.displayMessage("onIdrResetFailed is done");
        bIdrReset = true;
        onResetStatus();
    }

     function onPtrResetFailed() {
        top.API.displayMessage("onPtrResetFailed is done");
        bCrdReset = true;
        onResetStatus();
    }
    //DeviceError START
    function onCrdDeviceError() {
        top.API.displayMessage("onCrdDeviceError is done");
        bCrdReset = true;
        onResetStatus();
    }

    function onPinDeviceError() {
        top.API.displayMessage("onPinDeviceError is done");
        bPinReset = true;
        onResetStatus();
    }


    function onCimDeviceError() {
        top.API.displayMessage("onCimDeviceError is done");
        bCimReset = true;
        onResetStatus();
    }
    function onCdmDeviceError() {
        top.API.displayMessage("onCdmDeviceError is done");
        bCdmReset = true;
        onResetStatus();
    }

    function onFpiDeviceError() {
        top.API.displayMessage("onFpiDeviceError is done");
        bFpiReset = true;
        onResetStatus();
    }
    function onIdrDeviceError() {
        top.API.displayMessage("onIdrDeviceError is done");
        bIdrReset = true;
        onResetStatus();
    }
    //CHANGE STATUS
    function onCdmStatusChanged(PropertyName, OldValue, NewValue) {
        top.API.displayMessage("onCdmStatusChanged()����");
        showinfo();
    }

    function onCrdStatusChanged(PropertyName, OldValue, NewValue) {
        top.API.displayMessage("onCrdStatusChanged()����");
        showinfo();
    }

    function onFpiStatusChanged(PropertyName, OldValue, NewValue) {
        top.API.displayMessage("onFpiStatusChanged()����");
        showinfo();
    }

    function onIdrStatusChanged(PropertyName, OldValue, NewValue) {
        top.API.displayMessage("onIdrStatusChanged()����");
        showinfo();
    }

    function onJnlStatusChanged(PropertyName, OldValue, NewValue) {
        top.API.displayMessage("onJnlStatusChanged()����");
        showinfo();
    }

    function onPinStatusChanged(PropertyName, OldValue, NewValue) {
        top.API.displayMessage("onPinStatusChanged()����");
        showinfo();
    }

    function onPtrStatusChanged(PropertyName, OldValue, NewValue) {
        top.API.displayMessage("onPtrStatusChanged()����");
        showinfo();
    }

    function onTcpStatusChanged(PropertyName, OldValue, NewValue) {
        top.API.displayMessage("onTcpStatusChanged()����");
        showinfo();
    }

    function onScrStatusChanged(PropertyName, OldValue, NewValue) {
        top.API.displayMessage("onScrStatusChanged()����");
        showinfo();
    }

    function onSafeDoorOpened() {
        document.getElementById("WTip").style.display = "block";
        showinfo();
    }

    function onSafeDoorClosed() {
        document.getElementById("WTip").style.display = "none";
		showinfo();
    }
	
	// �ж��Ƿ񳬹�7�����ʱ��
	function onCheckExchangTime(){
		var strLastExchangeDate = top.Dat.GetPrivateProfileSync("Exchange", "ExchangeDate", "", top.API.gIniFileName);
		
		if("" == strLastExchangeDate){
			top.API.displayMessage("���ʱ���¼Ϊ��,������ʾ" );
		}
		else{
			var createTime = strLastExchangeDate.substring(0,10);
			var time = new Date().getTime() - new Date(createTime.replace(/-/g,"/")).getTime();
			var days = parseInt(time/(1000*60*60*24));
			if(days>=7){
				top.API.displayMessage("�����ϴ�����ѳ���7��");
				alert("�����������");
			}
		}
	}
    //Register the event
    function EventLogin() {
        //RESET COMPLETE
        top.Cim.ResetComplete.connect(onCimResetComplete);
        top.Cim.ResetFailed.connect(onCimResetFailed);
        top.Cdm.ResetComplete.connect(onCdmResetComplete);
        top.Cdm.ResetFailed.connect(onCdmResetFailed);
        top.Crd.ResetComplete.connect(onCrdResetComplete);
        top.Crd.ResetFailed.connect(onCrdResetFailed);
        top.Fpi.ResetComplete.connect(onFpiResetComplete);
        top.Fpi.ResetFailed.connect(onFpiResetFailed);
        top.Idr.ResetComplete.connect(onIdrResetComplete);
        top.Idr.ResetFailed.connect(onIdrResetFailed);
        top.Jnl.ResetComplete.connect(onJnlResetComplete);
        top.Pin.ResetComplete.connect(onPinResetComplete);
        top.Pin.ResetFailed.connect(onPinResetFailed);
        top.Ptr.ResetComplete.connect(onPtrResetComplete);
        top.Ptr.ResetFailed.connect(onPtrResetFailed);
        top.Tcp.ResetComplete.connect(onTcpResetComplete);
        //CHANGE STATUS
        top.Cdm.StatusChanged.connect(onCdmStatusChanged);
        top.Crd.StatusChanged.connect(onCrdStatusChanged);
        top.Fpi.StatusChanged.connect(onFpiStatusChanged);
        top.Idr.StatusChanged.connect(onIdrStatusChanged);
        top.Jnl.StatusChanged.connect(onJnlStatusChanged);
        top.Pin.StatusChanged.connect(onPinStatusChanged);
        top.Ptr.StatusChanged.connect(onPtrStatusChanged);
        top.Tcp.StatusChanged.connect(onTcpStatusChanged);
        top.Scr.StatusChanged.connect(onScrStatusChanged);
        top.Sys.CheckStatus.connect(onCheckStatus);
        //DEVICEERROE          
        top.Crd.DeviceError.connect(onCrdDeviceError);
        top.Pin.DeviceError.connect(onPinDeviceError);
        top.Cim.DeviceError.connect(onCimDeviceError);
        top.Cdm.DeviceError.connect(onCdmDeviceError);
        top.Idr.DeviceError.connect(onIdrDeviceError);
        top.Fpi.DeviceError.connect(onFpiDeviceError);

        //���չ��ſ����¼�
        top.Cim.SafeDoorClosed.connect(onSafeDoorClosed);
        top.Cim.SafeDoorOpened.connect(onSafeDoorOpened);
		//Door
		top.Cdm.ShutterOpened.connect(onShutterOpened);
		top.Cdm.ShutterOpenFailed.connect(onShutterOpenFailed);
		top.Cdm.ShutterClosed.connect(onShutterClosed);
		top.Cdm.ShutterCloseFailed.connect(onShutterCloseFailed);
		
        top.Cdm.CashTaken.connect(onCashTaken);
    }

    function EventLogout() {
        top.Cim.ResetComplete.disconnect(onCimResetComplete);
        top.Cim.ResetFailed.disconnect(onCimResetFailed);
        top.Cdm.ResetComplete.disconnect(onCdmResetComplete);
        top.Cdm.ResetFailed.disconnect(onCdmResetFailed);
        top.Crd.ResetComplete.disconnect(onCrdResetComplete);
        top.Crd.ResetFailed.disconnect(onCrdResetFailed);
        top.Fpi.ResetComplete.disconnect(onFpiResetComplete);
        top.Fpi.ResetFailed.disconnect(onFpiResetFailed);
        top.Idr.ResetComplete.disconnect(onIdrResetComplete);
        top.Idr.ResetFailed.disconnect(onIdrResetFailed);
        top.Jnl.ResetComplete.disconnect(onJnlResetComplete);
        top.Pin.ResetComplete.disconnect(onPinResetComplete);
        top.Pin.ResetFailed.disconnect(onPinResetFailed);
        top.Ptr.ResetComplete.disconnect(onPtrResetComplete);
        top.Ptr.ResetFailed.disconnect(onPtrResetFailed);
        top.Tcp.ResetComplete.disconnect(onTcpResetComplete);
        //
        top.Crd.StatusChanged.disconnect(onCrdStatusChanged);
        top.Pin.StatusChanged.disconnect(onPinStatusChanged);
        top.Ptr.StatusChanged.disconnect(onPtrStatusChanged);
        top.Cdm.StatusChanged.disconnect(onCdmStatusChanged);
        top.Fpi.StatusChanged.disconnect(onFpiStatusChanged);
        top.Idr.StatusChanged.disconnect(onIdrStatusChanged);
        top.Jnl.StatusChanged.disconnect(onJnlStatusChanged);
        top.Tcp.StatusChanged.disconnect(onTcpStatusChanged);
        top.Scr.StatusChanged.disconnect(onScrStatusChanged);
        top.Sys.CheckStatus.disconnect(onCheckStatus);
        //DEVICEERROE          
        top.Crd.DeviceError.disconnect(onCrdDeviceError);
        top.Pin.DeviceError.disconnect(onPinDeviceError);
        top.Cim.DeviceError.disconnect(onCimDeviceError);
        top.Cdm.DeviceError.disconnect(onCdmDeviceError);
        top.Idr.DeviceError.disconnect(onIdrDeviceError);
        top.Fpi.DeviceError.disconnect(onFpiDeviceError);
        //���չ��ſ����¼�
        top.Cim.SafeDoorClosed.disconnect(onSafeDoorClosed);
        top.Cim.SafeDoorOpened.disconnect(onSafeDoorOpened);
		//Door
		top.Cdm.ShutterOpened.disconnect(onShutterOpened);
		top.Cdm.ShutterOpenFailed.disconnect(onShutterOpenFailed);
		top.Cdm.ShutterClosed.disconnect(onShutterClosed);
		top.Cdm.ShutterCloseFailed.disconnect(onShutterCloseFailed);
		
        top.Cdm.CashTaken.disconnect(onCashTaken);
    }
    //remove all event handler
    function Clearup() {
        EventLogout();
    }
})();
