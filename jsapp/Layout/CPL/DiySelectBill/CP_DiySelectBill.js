/*@create by:  tsxiong
*@time: 2016��03��20��
*/
; (function () {
    var bExit = false;
    //���屣������䳮Ʊ������
    var strCash = new Array("0","0","0","0","0","0");
    var nCashArray = new Array(0,0,0,0,0,0);
    var ArrBool = new Array(0,0,0,0,0,0); //����ֵ�Ƿ����
    var RemainCount = 0; 
    var nCash = 0;
    var inputId = 0;
    var Cash = document.getElementsByName("Cash");
    var CashMoney = document.getElementById("CashMoney");
    var ErrorTip = document.getElementById("ErrorTip");
    var ModBtn = document.getElementsByName("BtnJian");
    var AddBtn = document.getElementsByName("BtnJia");
    //���浱ǰ������־λ
    var InputFlag = 1;
	var lRetAuthorized;//�޶�
	var CallResponse = App.Cntl.ProcessOnce( function(Response){
        //TO DO:
        Clearup();
        //Entry the flows control process.
        App.Cntl.ProcessDriven(Response);
    });
    var Initialize = function () {
        //@initialize scope start    
        ButtonDisable();
        EventLogin();
        App.Timer.TimeoutDisposal(TimeoutCallBack);
        RemainCount = top.Cdm.RemainCount()+15;/////zyf mod ����15�����㳮ȡ���ٽ�ֵʱ�н���������ʹ����ʧ�ܣ��˷���ֻ�ܽ��͸���
        GetUnitInfo();                    
        top.Pin.GetData(0, false, "0,1,2,3,4,5,6,7,8,9,ENTER,CLEAR,CANCEL", "CANCEL", -1);
		lRetAuthorized = top.Sys.DataGetSync(top.API.MTRN_AUTHORIZEDAMOUNTRULE);
        top.API.displayMessage("MTRN_AUTHORIZEDAMOUNTRULE,��Ȩ�޶�,lRetAuthorized=" + lRetAuthorized);
        ButtonEnable();
		App.Plugin.Voices.play("voi_39");
    } (); //Page Entry

    function ButtonDisable() {
        document.getElementById('Exit').disabled = true;
        document.getElementById('OK').disabled = true;
    }

    function ButtonEnable() {
        document.getElementById('Exit').disabled = false;
        document.getElementById('OK').disabled = false;
    }

    function GetUnitInfo() {
        var i;        
        for (i = 0; i < top.API.CashInfo.nCountOfUnits; i++) {
            if ((top.API.CashInfo.arrUnitCurrency[i] == 100) && (top.API.CashInfo.arrUnitRemain[i] > RemainCount)) {
                ArrBool[0] = 1;
            } else if ((top.API.CashInfo.arrUnitCurrency[i] == 50) && (top.API.CashInfo.arrUnitRemain[i] > RemainCount)) {
                ArrBool[1] = 1;
            } else if ((top.API.CashInfo.arrUnitCurrency[i] == 20) && (top.API.CashInfo.arrUnitRemain[i] > RemainCount)) {
                ArrBool[2] = 1;
            } else if ((top.API.CashInfo.arrUnitCurrency[i] == 10) && (top.API.CashInfo.arrUnitRemain[i] > RemainCount)) {
                ArrBool[3] = 1;
            } else if ((top.API.CashInfo.arrUnitCurrency[i] == 5) && (top.API.CashInfo.arrUnitRemain[i] > RemainCount)) {
                ArrBool[4] = 1;
            } else if ((top.API.CashInfo.arrUnitCurrency[i] == 1) && (top.API.CashInfo.arrUnitRemain[i] > RemainCount)) {
                ArrBool[5] = 1;
            }
        }
        for (i = 0; i < ArrBool.length; i++) {
            if (ArrBool[i] == 0) {
                Cash[i].style.backgroundPositionY = "0px";                
                AddBtn[i].style.backgroundImage = "url('Framework/style/Graphics/None-jia.png')";                
                AddBtn[i].disabled = true;
            }
			ModBtn[i].style.backgroundImage = "url('Framework/style/Graphics/None-jian.png')";
			ModBtn[i].disabled = true;
        }
    }

    function CheckRemainMoney() {        
        var i = 0;        
        var bRet100 = false;
        var bRet50 = false;
        var bRet20 = false;
        var bRet10 = false;
        var bRet5 = false;
        var bRet1 = false;
        var arrMixCash = new Array();
		var tmpnCashArray = new Array();
		for ( i = 0; i < nCashArray.length; i++) {
			tmpnCashArray[i] = nCashArray[i];
		}
		var arrRemainCurrencyCash = new Array(0,0,0,0,0,0);
        for ( i = 0; i < top.API.CashInfo.nCountOfUnits; i++) {
            if (top.API.CashInfo.arrUnitCurrency[i] == 0) {
                arrMixCash[i] = 0;
                continue;
            }else{
                if (top.API.CashInfo.arrUnitCurrency[i] == 100) {
					//arrRemainCurrencyCash[0] += top.API.CashInfo.arrUnitRemain[i] - RemainCount;
					var tmpNum = top.API.CashInfo.arrUnitRemain[i] - RemainCount;
					if(tmpNum > 0){
						arrRemainCurrencyCash[0] += tmpNum;
					}
                    if ((top.API.CashInfo.arrUnitRemain[i] - tmpnCashArray[0]) < RemainCount) {
                        if(top.API.CashInfo.arrUnitRemain[i] > RemainCount){
							tmpnCashArray[0] = tmpnCashArray[0] - top.API.CashInfo.arrUnitRemain[i] + RemainCount;
							arrMixCash[i] = top.API.CashInfo.arrUnitRemain[i] - RemainCount;  
						}else{
							arrMixCash[i] = 0;  
						}                     
                    }else{
                        arrMixCash[i] = tmpnCashArray[0];
						tmpnCashArray[0]=0;
                        bRet100 = true;
                    }
                } else if (top.API.CashInfo.arrUnitCurrency[i] == 50) {
					var tmpNum = top.API.CashInfo.arrUnitRemain[i] - RemainCount;
					if(tmpNum > 0){
						arrRemainCurrencyCash[1] += tmpNum;
					}
                    if ((top.API.CashInfo.arrUnitRemain[i] - tmpnCashArray[1]) < RemainCount) {
                        if(top.API.CashInfo.arrUnitRemain[i] > RemainCount){
							tmpnCashArray[1] = tmpnCashArray[1] - top.API.CashInfo.arrUnitRemain[i] + RemainCount;
							arrMixCash[i] = top.API.CashInfo.arrUnitRemain[i] - RemainCount;  
						}else{
							arrMixCash[i] = 0;  
						}                       
                    }else{
                        arrMixCash[i] = tmpnCashArray[1];
                        tmpnCashArray[1] = 0;
                        bRet50 = true;
                    }
                } else if (top.API.CashInfo.arrUnitCurrency[i] == 20) {
					var tmpNum = top.API.CashInfo.arrUnitRemain[i] - RemainCount;
					if(tmpNum > 0){
						arrRemainCurrencyCash[2] += tmpNum;
					}
                    if ((top.API.CashInfo.arrUnitRemain[i] - tmpnCashArray[2]) < RemainCount) {
                        if(top.API.CashInfo.arrUnitRemain[i] > RemainCount){
							tmpnCashArray[2] = tmpnCashArray[2] - top.API.CashInfo.arrUnitRemain[i] + RemainCount;
							arrMixCash[i] = top.API.CashInfo.arrUnitRemain[i] - RemainCount;  
						}else{
							arrMixCash[i] = 0;  
						}                      
                    }else{
                        arrMixCash[i] = tmpnCashArray[2];
                        tmpnCashArray[2] = 0;
                        bRet20 = true;
                    }
                } else if (top.API.CashInfo.arrUnitCurrency[i] == 10) {
					var tmpNum = top.API.CashInfo.arrUnitRemain[i] - RemainCount;
					if(tmpNum > 0){
						arrRemainCurrencyCash[3] += tmpNum;
					}
                    if ((top.API.CashInfo.arrUnitRemain[i] - tmpnCashArray[3]) < RemainCount) {
                        if(top.API.CashInfo.arrUnitRemain[i] > RemainCount){
							tmpnCashArray[3] = tmpnCashArray[3] - top.API.CashInfo.arrUnitRemain[i] + RemainCount;
							arrMixCash[i] = top.API.CashInfo.arrUnitRemain[i] - RemainCount;  
						}else{
							arrMixCash[i] = 0;  
						}                      
                    }else{
                        arrMixCash[i] = tmpnCashArray[3];
                        tmpnCashArray[3] = 0;
                        bRet10 = true;
                    }
                } else if (top.API.CashInfo.arrUnitCurrency[i] == 5) {
					var tmpNum = top.API.CashInfo.arrUnitRemain[i] - RemainCount;
					if(tmpNum > 0){
						arrRemainCurrencyCash[4] += tmpNum;
					}
                    if ((top.API.CashInfo.arrUnitRemain[i] - tmpnCashArray[4]) < RemainCount) {
                        if(top.API.CashInfo.arrUnitRemain[i] > RemainCount){
							tmpnCashArray[4] = tmpnCashArray[4] - top.API.CashInfo.arrUnitRemain[i] + RemainCount;
							arrMixCash[i] = top.API.CashInfo.arrUnitRemain[i] - RemainCount;  
						}else{
							arrMixCash[i] = 0;  
						}                       
                    }else{
                        arrMixCash[i] = tmpnCashArray[4];
                        tmpnCashArray[4] = 0;
                        bRet5 = true;
                    }
                } else if (top.API.CashInfo.arrUnitCurrency[i] == 1) {
					var tmpNum = top.API.CashInfo.arrUnitRemain[i] - RemainCount;
					if(tmpNum > 0){
						arrRemainCurrencyCash[5] += tmpNum;
					}
                    if ((top.API.CashInfo.arrUnitRemain[i] - tmpnCashArray[5]) < RemainCount) {
                        if(top.API.CashInfo.arrUnitRemain[i] > RemainCount){
							tmpnCashArray[5] = tmpnCashArray[5] - top.API.CashInfo.arrUnitRemain[i] + RemainCount;
							arrMixCash[i] = top.API.CashInfo.arrUnitRemain[i] - RemainCount;  
						}else{
							arrMixCash[i] = 0;  
						}                      
                    }else{
                        arrMixCash[i] = tmpnCashArray[5];
                        tmpnCashArray[5] = 0;
                        bRet1 = true;
                    }
                } 
            }     
        }
        if (!bRet100 && (tmpnCashArray[0] > 0)){             
            ErrorTip.innerText = "100Ԫ�������㣬��ǰ��ȡ" + arrRemainCurrencyCash[0] + "�ţ������·���";   
            return false;           
        }
        if (!bRet50 && (tmpnCashArray[1] > 0)){
            ErrorTip.innerText = "50Ԫ�������㣬��ǰ��ȡ" + arrRemainCurrencyCash[1] + "�ţ������·���";   
            return false;           
        }
        if (!bRet20 && (tmpnCashArray[2] > 0)){
            ErrorTip.innerText = "20Ԫ�������㣬��ǰ��ȡ" + arrRemainCurrencyCash[2] + "�ţ������·���";   
            return false;           
        }
        if (!bRet10 && (tmpnCashArray[3] > 0)){
            ErrorTip.innerText = "10Ԫ�������㣬��ǰ��ȡ" + arrRemainCurrencyCash[3] + "�ţ������·���";   
            return false;           
        }
        if (!bRet5 && (tmpnCashArray[4] > 0)){
            ErrorTip.innerText = "5Ԫ�������㣬��ǰ��ȡ" + arrRemainCurrencyCash[4] + "�ţ������·���";   
            return false;           
        }
        if (!bRet1  && (tmpnCashArray[5] > 0)){
            ErrorTip.innerText = "1Ԫ�������㣬��ǰ��ȡ" + arrRemainCurrencyCash[5] + "�ţ������·���";   
            return false;           
        }
        var i = 0;
        for ( i = 0; i < top.API.CashInfo.nCountOfUnits; i++) {
            top.API.CashInfo.arrSelfMixDispenset[i] = arrMixCash[i];
        }
        top.API.CashInfo.strTransAmount = nCash.toString();
        var tmp = top.API.CashInfo.strTransAmount + "00";
        var arrINPUTMONEY = new Array(tmp);
        top.Dat.SetDataSync(top.API.transamountTag, top.API.transamountType, arrINPUTMONEY);
        
        return true;           
    }

    function StringtoInt() {        
        var i = 0;
        for ( i = 0;  i < strCash.length;  i++) {
            if (strCash[i] != "") {
                nCashArray[i] = parseInt(strCash[i], 10);
            }
        }
    }

    function RefreshData() {
        var i = 0;
        StringtoInt();
        for ( i = 0; i < nCashArray.length; i++) {
            if (nCashArray[i] == 0) {
                ModBtn[i].style.backgroundImage = "url('Framework/style/Graphics/None-jian.png')";
                ModBtn[i].disabled = true;
            }else{
                ModBtn[i].style.backgroundImage = "url('Framework/style/Graphics/jian.png')";
                ModBtn[i].disabled = false;
            }
        }
        nCash =nCashArray[0] * 100 + nCashArray[1] * 50 + nCashArray[2] * 20 + nCashArray[3] * 10 + nCashArray[4] * 5 + nCashArray[5];
        CashMoney.innerText = nCash.toString();        
    }

    //�༭�����¼�
    for (var j = 0; j < Cash.length; j++) {
		(function () {
        var inpt = Cash[j];
        var p = j;
        inpt.onclick = function (e) {
            if (ArrBool[p] == 1) {
                ClearTip();
                ButtonEnable();
                inputId = p;
            }
        }
		})();
    }
    //���Ű�ť����¼�
    for (var j = 0; j < ModBtn.length; j++) {
		(function () {
        var inpt = ModBtn[j];
        var p = j;
        inpt.onclick = function (e) {
            ClearTip();
            ButtonEnable();
            if ( nCashArray[p] > 0) {
				inputId = p;
                nCashArray[p]-- ;
                strCash[p] = nCashArray[p].toString();
                Cash[p].innerText = strCash[p];
				Cash[inputId].focus();
                RefreshData(); 
				//alert("-");
            }
			//alert(inputId);
        }
		})();
    }
    //�ӺŰ�ť����¼�
    for (var j = 0; j < AddBtn.length; j++) {
		(function () {
        var inpt = AddBtn[j];
        var p = j;
        inpt.onclick = function (e) {
            ClearTip();
            ButtonEnable();
            if ( nCashArray[p] < 9999) {
				inputId = p;
                nCashArray[p]++ ;
                strCash[p] = nCashArray[p].toString();
                Cash[p].innerText = strCash[p];
				Cash[inputId].focus();
                RefreshData();  
				//alert("+");
            }  
			//alert(inputId)
        }
		})();
    }

    function ClearTip() {
        ErrorTip.innerText = ""; 
    }

    //�����ǰ�����
    function onClearNum() {
        Cash[inputId].innerText = '0';
        strCash[inputId] = '0';
        Cash[inputId].focus();
        RefreshData();
    }
    function onKeyPressed(key, keyCode) {
        ClearTip();
        if (key == "ENTER") {
            document.getElementById('OK').onclick();
        } else if (key == "CLEAR") {
            onClearNum();
        } else if (key == "CANCEL") {
            document.getElementById('Exit').onclick();
        } else {
			
            if (strCash[inputId] == "0") {
                strCash[inputId] = "";
            }
            if (strCash[inputId].length < 4){
                strCash[inputId] += key;
                Cash[inputId].innerText = strCash[inputId];
            }                
        }
        RefreshData();        
    }

    document.getElementById('OK').onclick = function () {
        RefreshData();
        if (nCash > 0) {
            if (CheckRemainMoney()) {                
                var UnitMaxAmount = lRetAuthorized;
                if ((nCash + top.API.gnCWDMoney) >= UnitMaxAmount) {
					var CWDMoney = 0;
					if(top.API.CashInfo.Dealtype == "����ȡ��"){
						CWDMoney = 10000;
					}else if(top.API.CashInfo.Dealtype == "CWDȡ��"){
						CWDMoney = 200000;
					}
					if ((nCash + top.API.gnCWDMoney) > CWDMoney && top.API.gbOrderCWD == false) {
						ErrorTip.innerText = "���˿ͻ����ۼ�ȡ���޶�Ϊ"+CWDMoney+"Ԫ�����ı���ȡ�����ѳ���ʣ���ȡ�ֶ�ȣ�����������";
					}else{
						var tmpTslType = "";
						if (top.API.CashInfo.Dealtype == "CWDȡ��") {
							ButtonDisable();
							//���÷��ͱ��ĵı�־λ����ת                
							top.API.gTransactiontype = "CWD";                
							top.Pin.CancelGetData();
							top.API.gTslChooseJnlType = "0108";
							tmpTslType = "BCWD";
							TslFunction(tmpTslType);
							if(nCash < 50000){
								return CallResponse('NeedCheck');
							}else{
								return CallResponse('LargeDeposit');
							}
							return CallResponse('LargeDeposit');
						} else if (top.API.CashInfo.Dealtype == "����ȡ��") {
							ErrorTip.innerText = "����ȡ��ɳ���10000Ԫ�������·���"; 
						} 
					}               
                }else{
                    var tmpTslType = "";
					ButtonDisable();
					//���÷��ͱ��ĵı�־λ����ת                
					top.API.gTransactiontype = "CWD";                
					top.Pin.CancelGetData();
					top.API.gTslChooseJnlType = "0108";
                    if (top.API.CashInfo.Dealtype == "CWDȡ��") {
                        tmpTslType = "SCWD";
						TslFunction(tmpTslType);  
						if(top.API.gbOrderCWD == true){
							return CallResponse('PassBookCWD');
						}else{
							return CallResponse('OK');
						}
                    } else if (top.API.CashInfo.Dealtype == "����ȡ��") {
                        tmpTslType = "BOOKCWD";
						TslFunction(tmpTslType);  
						return CallResponse('PassBookCWD');
                    }                     
                }
            }
        } else {
            ErrorTip.innerText = "�ܽ���Ϊ0��"; 
        }
    }

    document.getElementById('Exit').onclick = function () {
        ButtonDisable();
        top.ErrorInfo = top.API.PromptList.No2;
        top.Pin.CancelGetData();
        return CallResponse("Exit");
    }



    function onDeviceError() {
        top.ErrorInfo = top.API.PromptList.No4;
        return CallResponse("Exit");
    }
    function TslFunction(type){
 		var myData = top.GetDate12byte();
 		top.API.gTslDate = myData.substr(0,8);
        top.API.gTslTime = myData.substr(8,6);
        top.API.gTslChooseType= type;
 	}
    //@User code scope end 

    //Register the event
    function EventLogin() {
        top.Pin.KeyPressed.connect(onKeyPressed);
        top.Pin.DeviceError.connect(onDeviceError);
    }

    function EventLogout() {
        top.Pin.KeyPressed.disconnect(onKeyPressed);
        top.Pin.DeviceError.disconnect(onDeviceError);
    }


    //Countdown function
    function TimeoutCallBack() {

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
