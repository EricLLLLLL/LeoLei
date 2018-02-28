/*@create by:  LeoLei
*@time: 2017��09��05��
*/
; (function () {
	var CallResponse = App.Cntl.ProcessOnce( function(Response){
        //TO DO:
        top.Siu.SetReceiptPrinterLight('OFF');
        top.ErrorInfo = top.API.PromptList.No4;
        Clearup();
        App.Cntl.ProcessDriven(Response);
    });
    var Initialize = function () {
        App.Timer.TimeoutDisposal(TimeoutCallBack);
		RecordTsl();
		CheckAcceptStatus();//���Ŀǰ���ģ��AcceptStatus
        initPtr();
    }(); //Page Entry
 
    //@User ocde scope start
	function RecordTsl() {
		//д��TslLog
        if(top.API.gTslFlag && top.API.gTslResult != "SUCCESS"){
			top.API.gTslFlag = false;
        	//������|ʱ��|����|��ˮ��|��������|���|���֤��|���֤ͷ��·��|���׽����
        	var TslLog = top.API.gTslDate;
        	TslLog += "|"+top.API.gTslTime;
        	TslLog += "|"+top.API.gCardno;
        	TslLog += "|"+top.API.gTslJnlNum;
        	TslLog += "|"+top.API.gTslChooseType;
        	TslLog += "|"+top.API.gTslMoneyCount;
        	TslLog += "|"+top.API.gIdNumber;
        	TslLog += "|"+top.API.gIdCardpic;
        	TslLog += "|"+top.API.gTslResult;
        	top.Tsl.AddTransLogSync(TslLog);   
            //�ն˺ţ�8λ�����������ڣ�8λ��������ʱ�䣨6λ�����������ͣ�4λ��0107�����0108����ȡ���
            //�ʺţ�19λ�������׽�10λ������λС��λ�����豸��ˮ�ţ�6λ�����豸��ˮ���κţ�6λ����
            //��̨�����루2λ������̨������ˮ�ţ�12λ�����豸����״̬��2λ��00�����׳ɹ���01�����쳣���ף����쳣״̬���ͣ�4λ��
            var strUpJnl = top.API.gTerminalID;  
            strUpJnl += "|!"+top.API.gTslDate;  
            strUpJnl += "|!"+top.API.gTslTime;  
            strUpJnl += "|!"+top.API.gTslChooseJnlType;  
            strUpJnl += "|!"+top.API.gCardno; 
            strUpJnl += "|!"+top.API.gTslMoneyCount.replace(".","");  
            strUpJnl += "|!"+top.API.gTslJnlNum;  
            strUpJnl += "|!"+top.API.gTslJnlBtn;  
            strUpJnl += "|!"+top.API.gTslResponsecode;  
            strUpJnl += "|!"+top.API.gTslSysrefnum;
            strUpJnl += "|!01|!����ʧ��";
           // top.Tsl.CreateUpJnlFile(strUpJnl); //
            top.Tsl.HandleRecordFileSync(top.API.MTSL_WRITECHECKTRANSRECORD, strUpJnl);
        }
	}
    function initPtr() {
		App.Plugin.Voices.play("voi_8");
		// var objGet1 = top.Dat.GetDataSync("COMMENTS", "STRING");
		// var arrGet1 = objGet1.toArray();
        //
		// var objGet = top.Dat.GetDataSync("TRANSACTIONRESULT", "STRING");
		// if (null == objGet) {
		// 	top.API.displayMessage("GetDataSync TRANSACTIONRESULT objGet = null");
		// }else {
			//var arrGet =  objGet.toArray();
			// if(arrGet[0]!= "ʧ��"){
			// 	document.getElementById("tip_label").innerHTML = arrGet[0] +"<br/>"+arrGet1[0];
			// }else{
			// 	document.getElementById("tip_label").innerHTML = arrGet1[0];
			// }
			$('#tip_label').text('�����ϴ�������Ϣ�����Ժ�');
		// }
		document.getElementById("tip_label").style.display = "block";
		if (top.Ptr.bDeviceStatus && top.API.gNoPtrSerFlag==false) {
			top.Ptr.Print("ReceiptCash_Print_szABC", "",top.API.gPrintTimeOut); 
		}
        //return CallResponse("Exit");
	}
	
	function CheckAcceptStatus() {
		//���Ŀǰ���ģ��AcceptStatus
        var strAcceptStatus = top.Cim.LastAcceptStatus();
        top.API.displayMessage("LastAcceptStatus=" + strAcceptStatus);
        if (strAcceptStatus == "ACTIVE") {
			top.API.displayMessage("ִ��CashInEnd");
			//ִ��CashInEnd
            top.Cim.StoreEscrowedCash(top.API.gStoreEscrowedCashTimeOut);
        } 
	}
    //Countdown function
    function TimeoutCallBack() {
        return CallResponse("TimeOut");
    }
    //Page Return
    
    //remove all event handler
    function Clearup() {
        App.Plugin.Voices.del();
        App.Timer.ClearTime();
    }
})();