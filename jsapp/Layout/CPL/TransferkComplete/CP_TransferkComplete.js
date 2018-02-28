/*@create by:  liaolei
 *@time: 2016年03月20日
 */
;(function () {
    var CallResponse = App.Cntl.ProcessOnce(function (Response) {
        Clearup();
        App.Cntl.ProcessDriven(Response);
    });
    var Initialize = function () {
        ButtonDisable();
        setData();
        App.Timer.TimeoutDisposal(TimeoutCallBack);
        ButtonEnable();
    }(); //Page Entry
    function ButtonDisable() {
        document.getElementById('Exit').disabled = true;
        document.getElementById('Continue').disabled = true;
        document.getElementById('Print').disabled = true;
    }

    function ButtonEnable() {
        document.getElementById('Exit').disabled = false;
        document.getElementById('Continue').disabled = false;
        document.getElementById('Print').disabled = false;
    }


    document.getElementById("Exit").onclick = function () {
        ButtonDisable();
        top.ErrorInfo = top.API.PromptList.No2;
        return CallResponse("Exit");
    }

    document.getElementById("Continue").onclick = function () {
        ButtonDisable();
        // top.API.gNoPtrSerFlag = true;
        return CallResponse("NeedInitData");

    }
    document.getElementById("Print").onclick = function () {
        // document.getElementById('Print').display = "none";
        $("#Print").hide();
        setPrintForm();
    }

    function setData(){
        $("#Print").hide();
        if(top.Ptr.bDeviceStatus && top.API.gNoPtrSerFlag == false){
            $("#Print").show();
        }

        // 设置数据
        switch (top.API.CashInfo.Dealtype){
            case "PeriodicCurrentAccount":
                top.Dat.SetDataSync("CURRENCY", "STRING", ["人民币"]); // 币种
                break;
            case "CurrentToRegular":
                top.Dat.SetDataSync("CURRENCY", "STRING", ["人民币"]); // 币种
                top.Dat.SetDataSync("WITHDRAWING", "STRING", ["全部提取"]); // 支取方式
                break;
            case "InLineFlag":
                top.Dat.SetDataSync("DEALTYPE", "STRING", ["行内转账"]);
                top.Dat.SetDataSync('POUNDAGE','STRING',["0.00"]);//手续费
                var comment;
                if (top.API.postScript != '') {
                    comment = top.API.CommentSelect + ' ' + top.API.postScript;
                } else {
                    comment = top.API.CommentSelect;
                }
                top.Dat.SetDataSync('COMMENTS','STRING',[comment]);//附言

                break;
            case "OutLineFlag":
                top.Dat.SetDataSync("DEALTYPE", "STRING", ["行外转账"]);
                top.Dat.SetDataSync('POUNDAGE','STRING',[top.API.TransferPoundage]);//手续费
                var comment;
                if (top.API.postScript != '') {
                    comment = top.API.CommentSelect + ' ' + top.API.postScript;
                } else {
                    comment = top.API.CommentSelect;
                }
                top.Dat.SetDataSync('COMMENTS','STRING',[comment]);//附言

                break;
            case "TransferCancellation":
                top.Dat.SetDataSync("DEALTYPE", "STRING", ["转账撤销"]);
                break;
            default:
                break;
        }

        // 打印流水
        switch (top.API.CashInfo.Dealtype){
            case "PeriodicCurrentAccount":
                top.Jnl.PrintSync("RTCComplete");
                break;
            case "CurrentToRegular":
                top.Jnl.PrintSync("CTRComplete");
                break;
            case "InLineFlag":
            case "OutLineFlag":
                top.Jnl.PrintSync("TransferComplete");
                break;
            case "TransferCancellation":
                top.Jnl.PrintSync("selectRevocation");
                break;
            default:
                break;
        }
    }

    function setPrintForm(){

        // 打印凭条
        switch (top.API.CashInfo.Dealtype){
            case "PeriodicCurrentAccount":
                top.Ptr.Print("Receipt_RTC_szABC", "", top.API.gPrintTimeOut);
                break;
            case "CurrentToRegular":
                top.Ptr.Print("Receipt_CTR_szABC", "", top.API.gPrintTimeOut);
                break;
            case "InLineFlag":
            case "OutLineFlag":
                top.Ptr.Print("Receipt_Trans_szABC", "", top.API.gPrintTimeOut);
                break;
            case "TransferCancellation":
                top.Ptr.Print("ReceiptCash_Print_szABC", "", top.API.gPrintTimeOut);
                break;
            default:
                break;
        }

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
})
();