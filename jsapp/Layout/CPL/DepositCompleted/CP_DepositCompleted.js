/*@create by:  tsxiong
*@time: 2016��03��20��
*/
; (function () {
    var iniRet = "";
	var CallResponse = App.Cntl.ProcessOnce( function(Response){
        //TO DO:
        Clearup();
        //Entry the flows control process.
        App.Cntl.ProcessDriven(Response);
    });
    var Initialize = function () {
        App.Timer.TimeoutDisposal(TimeoutCallBack);
        ButtonDisable();
        top.API.gTslResult = "SUCCESS";
        if(top.API.gTslFlag){
        	//������|ʱ��|����|��ˮ��|��������|���|���֤��|���֤ͷ��·��|���׽����TSL���ݿ���־
            top.API.gTslFlag = false;
        	var TslLog = top.API.gTslDate;
        	TslLog += "|"+top.API.gTslTime;
        	TslLog += "|"+top.API.gCardno;
        	TslLog += "|"+top.API.gTslJnlNum;
        	TslLog += "|"+top.API.gTslChooseType;
        	TslLog += "|"+top.API.gTslMoneyCount;
        	TslLog += "|"+top.API.gIdNumber;
        	TslLog += "|"+top.API.gIdCardpic;
        	TslLog += "|"+top.API.gTslResult;
        	top.Tsl.AddTransLogSync(TslLog); //CreateUpJnlFile
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
            strUpJnl += "|!00|!���׳ɹ�";
            top.Tsl.HandleRecordFileSync(top.API.MTSL_WRITECHECKTRANSRECORD, strUpJnl);	
            
        }
		if (top.API.CashInfo.Dealtype == "DEP���") {
	    	document.getElementById('INQ').style.display = "block";
		}
        top.Sys.ItemClearSync(top.API.MTRN_TRANSACTIONDIFFER);
        if (top.Ptr.bDeviceStatus && top.API.gNoPtrSerFlag==false && top.API.gShowPrintButton) {
            document.getElementById('Print').style.display = "block";
        }
        iniRet = top.Dat.GetPrivateProfileSync("TransactionConfig", "ContinueTransSupport", "1", top.API.gIniFileName);
        if (top.API.CashInfo.Dealtype == "�޿����۴��" || top.API.gbPartCashIn || iniRet==="0" || top.API.CashInfo.Dealtype == "�Թ����"){
            document.getElementById('Continue').innerHTML = "����";
        } else {
			document.getElementById('OK').style.display = "block";
			if(top.API.CashInfo.Dealtype == "���۴��"){
				document.getElementById('OK').innerHTML = "����";
			}else{
				document.getElementById('OK').innerHTML = "�˿�";
			}           
        }
		
        //��ȡ����ܶ�
        var Amount = top.API.CashInfo.strTransAmount + ".00";
        document.getElementById("Amount").innerText=Amount;
        App.Plugin.Voices.play("voi_33");
        var arrTransactionResult;
        if (top.API.gbPartCashIn) {
            arrTransactionResult = new Array("��������");
			var arrComments = new Array("����δ���˽�����ϵ���й�����Ա");
		    top.Dat.SetDataSync("COMMENTS", "STRING", arrComments);
		    document.getElementById('Print').style.display = "none";
		    document.getElementById('Continue').style.display = "none";
		    top.Ptr.Print("ReceiptCash_Print_szABC", "",top.API.gPrintTimeOut);
        } else{
            arrTransactionResult = new Array("���׳ɹ�");
        }
		top.API.gTakeCardAndPrint = false;
		top.Dat.SetDataSync("TRANSACTIONRESULT", "STRING", arrTransactionResult);
        ButtonEnable();
    }();//Page Entry

    //@User ocde scope start
    function ButtonDisable() {
        document.getElementById('Print').disabled = true;
        document.getElementById('INQ').disabled = true;
        document.getElementById('OK').disabled = true;
        document.getElementById('Continue').disabled = true;
    }

    function ButtonEnable() {
        document.getElementById('Print').disabled = false;
        document.getElementById('INQ').disabled = false;
        document.getElementById('OK').disabled = false;
        document.getElementById('Continue').disabled = false;
    }

    document.getElementById('Print').onclick = function () {
        ButtonDisable();
        top.API.gPrintFSN = false;
		return CallResponse("Print");
	}

    document.getElementById('INQ').onclick = function () {
        ButtonDisable();
		top.API.gTransactiontype = "INQ";
		//����ѯ
        var arrBalanceInquiryType = new Array("NOTYPE");
        top.Dat.SetDataSync(top.API.BalanceInquiryTag, top.API.BalanceInquiryType, arrBalanceInquiryType);
        return CallResponse('INQ');
    }

    document.getElementById('OK').onclick = function () {
        ButtonDisable();
        return CallResponse("OK");
    }
    document.getElementById('Continue').onclick = function () {
        ButtonDisable();
        top.API.gArrUnitRemain = top.Cdm.CUNoteValue().toArray();
        top.API.gArrUnitStatus = top.Cim.CUStatus().toArray();
        //top.API.CashInfo.InitData();
        if (top.API.CashInfo.Dealtype == "�޿����۴��" || top.API.gbPartCashIn || iniRet==="0" || top.API.CashInfo.Dealtype == "�Թ����" ){
            return CallResponse('Exit');
        } else {
            return CallResponse('NeedInitData');
        }
        
    }
    function TimeoutCallBack() {
        top.ErrorInfo = top.API.PromptList.No3;
        return CallResponse("TimeOut");
    }
    //Page Return
    
    //remove all event handler
    function Clearup() {
        App.Plugin.Voices.del();
        App.Timer.ClearTime();
    }
})();
