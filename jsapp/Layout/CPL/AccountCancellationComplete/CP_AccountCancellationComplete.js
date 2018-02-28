/*@create by:  liaolei
 *@time: 2016��03��20��
 */
;(function () {
    var CallResponse = App.Cntl.ProcessOnce(function (Response) {
        Clearup();
        App.Cntl.ProcessDriven(Response);
    });
    var Initialize = function () {
        App.Timer.TimeoutDisposal(TimeoutCallBack);
        setData();
		SaveTheCloseRecord();
        top.Ptr.Print("Receipt_Sales_szABC", "", top.API.gPrintTimeOut);
    }(); //Page Entry
    function ButtonDisable() {
        document.getElementById('Exit').disabled = true;
    }

    function ButtonEnable() {
        document.getElementById('Exit').disabled = false;
    }


    document.getElementById("Exit").onclick = function () {
        ButtonDisable();
        top.ErrorInfo = top.API.PromptList.No2;
        return CallResponse("Exit");
    }

    function setData(){
        var CARDNO = top.Dat.GetDataSync("CARDNO", "STRING").toArray()[0],
            TRANSAMOUNT = top.Dat.GetDataSync("TRANSAMOUNT", "STRING").toArray()[0],
            INTEREST = top.Dat.GetDataSync("INTEREST", "STRING").toArray()[0],
            TAXABLE = top.Dat.GetDataSync("TAXABLE", "STRING").toArray()[0],
            TAXRATE = top.Dat.GetDataSync("TAXRATE", "STRING").toArray()[0],
            TAX = top.Dat.GetDataSync("TAX", "STRING").toArray()[0],
            TOTAL = top.Dat.GetDataSync("TOTAL", "STRING").toArray()[0],
            CUSTOMERNAME = top.Dat.GetDataSync("CUSTOMERNAME", "STRING").toArray()[0];

        $("#CARDNO").text(top.changeCardNum(CARDNO));
        $("#TRANSAMOUNT").text(TRANSAMOUNT);
        $("#INTEREST").text(INTEREST);
        $("#TAXABLE").text(TAXABLE);
        $("#TAXRATE").text(TAXRATE);
        $("#TAX").text(TAX);
        $("#TOTAL").text(TOTAL);
        $("#CUSTOMERNAME").text(top.changeName(CUSTOMERNAME));

        top.Dat.SetDataSync("DEALTYPE", "STRING", ["��������"]);
    }
	
	function SaveTheCloseRecord(){
		// ��������¼���浽����
        var sAccoutNo = top.Dat.GetDataSync("CARDNO", "STRING").toArray()[0]; // ����
        var sTransNo = top.Dat.GetDataSync("TFRCARDNO", "STRING").toArray()[0]; // ת���˻�
        var sTransAmount = top.Dat.GetDataSync("TOTAL", "STRING").toArray()[0]; // ���
        var sRetCode = top.API.gResponsecode; // ��̨����ֵ
		
		top.API.displayMessage("����������Ϣ�������ļ�");
		top.Tsl.HandleRecordFileSync(top.API.MTSL_WRITECLOSEDCORD, sAccoutNo + ", " + sTransNo + ", " + sTransAmount + ", " + sRetCode + ", " + ["�����ɹ�"]);
	}


    //Countdown function
    function TimeoutCallBack() {
        top.ErrorInfo = top.API.PromptList.No3;
        return CallResponse("TimeOut");
    }

    //Page Return

    //remove all event handler
    function Clearup() {
        App.Timer.ClearTime();
    }
})();