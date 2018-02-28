/*@create by:  liaolei
 *@time: 2016��03��20��
 */
; (function () {
    var CallResponse = App.Cntl.ProcessOnce( function(Response){
        Clearup();
        App.Cntl.ProcessDriven(Response);
    });
    var Initialize = function () {
        App.Timer.TimeoutDisposal(TimeoutCallBack);
        if( top.API.CashInfo.Dealtype == "CancelAccount" ){
            $("#OK").hide();
            $("#SubjectTip").find("p").html("��ƾ��ֽ���ݲ��ܽ����������ף�");
        }
    }(); //Page Entry
    function ButtonDisable() {
        document.getElementById('Exit').disabled = true;
        document.getElementById('OK').disabled = true;
    }

    function ButtonEnable() {
        document.getElementById('Exit').disabled = false;
        document.getElementById('OK').disabled = false;
    }


    document.getElementById("Exit").onclick = function(){
        ButtonDisable();
        top.ErrorInfo = top.API.PromptList.No2;
        return CallResponse("Exit");
    }

    document.getElementById("OK").onclick = function(){
        ButtonDisable();
        top.API.gNoPtrSerFlag = true;
        if (top.API.CashInfo.Dealtype == "�޿����۴��") {
            return CallResponse("NoCardDep");
        } else if (top.API.CashInfo.Dealtype == "����ҵ��") {
            return CallResponse("BookBank");
        } else if (top.API.CashInfo.Dealtype == "�Թ����") {
            return CallResponse("BusinessDep");
        } else if (top.API.CashInfo.Dealtype == "�㳮�һ�") {
            return CallResponse("Exchange");
        // }else if (top.API.CashInfo.Dealtype == "CancelAccount"){
        //     return CallResponse("CancelAccount");
        } else if(top.API.gbContinueTransFlag === true) {//�жϲ忨���׹����еļ�������			
            return CallResponse("CHIPCARDcontinue");
        }else{
            return CallResponse("OK");
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
})();