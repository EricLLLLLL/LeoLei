/*@create by:  tsxiong
*@time: 2016��03��20��
*/
; (function () {
	var sFingerData=""; //add by art
    var nReCompare = 0;
    var bIdentify = false;
    var RefuseReason = "�����Ϣ����";
	var CallResponse = App.Cntl.ProcessOnce( function(Response){
        //TO DO:
        Clearup();
        //Entry the flows control process.
        App.Cntl.ProcessDriven(Response);
    });
    var Initialize = function () {
        ButtonDisable();
        // EventLogin();
        //@initialize scope start
        //ShowInfo();
        //return CallResponse('OK');
        App.Timer.TimeoutDisposal(TimeoutCallBack);
        // top.Fpi.Identify(-1);
        // bIdentify = true;
        ButtonEnable();
        // App.Plugin.Voices.play("voi_13");
    }();//Page Entry

    document.getElementById('DealTime').onclick = function () {
        document.getElementById('DealTime').style.backgroundImage = "url('Framework/style/Graphics/btn/radio_select.png')";
        document.getElementById('CardNum').style.backgroundImage = "url('Framework/style/Graphics/btn/redio_noselct.png')";
        document.getElementById('JnlNum').style.backgroundImage = "url('Framework/style/Graphics/btn/redio_noselct.png')";
        RefuseReason = "�����Ϣ����";
    }
    document.getElementById('CardNum').onclick = function () {
        document.getElementById('DealTime').style.backgroundImage = "url('Framework/style/Graphics/btn/redio_noselct.png')";
        document.getElementById('CardNum').style.backgroundImage = "url('Framework/style/Graphics/btn/radio_select.png')";
        document.getElementById('JnlNum').style.backgroundImage = "url('Framework/style/Graphics/btn/redio_noselct.png')";
        RefuseReason = "���׽���";
    }
    document.getElementById('JnlNum').onclick = function () {
        document.getElementById('DealTime').style.backgroundImage = "url('Framework/style/Graphics/btn/redio_noselct.png')";
        document.getElementById('CardNum').style.backgroundImage = "url('Framework/style/Graphics/btn/redio_noselct.png')";
        document.getElementById('JnlNum').style.backgroundImage = "url('Framework/style/Graphics/btn/radio_select.png')";
        RefuseReason = "ָ����Ȩʧ��";
    }

    //@User ocde scope start
    function ButtonDisable() {
        top.API.displayMessage("ButtonDisable");
        document.getElementById('Left_3').disabled = true;
        document.getElementById('Right_3').disabled = true;
    }

    function ButtonEnable() {
        top.API.displayMessage("ButtonEnable");
        document.getElementById('Left_3').disabled = false;
        document.getElementById('Right_3').disabled = false;
    }

    document.getElementById('Left_3').onclick = function () {
        top.API.displayMessage("���<�˳�>");
        ButtonDisable();
        return CallResponse('TimeOut');
    }
    document.getElementById('Right_3').onclick = function () {
        top.API.displayMessage("���<ȷ��>"); //tmp by art
        ButtonDisable();
        top.API.gAuthorRefuse = RefuseReason;
        if (top.Ptr.bDeviceStatus && top.API.gNoPtrSerFlag==false) {
            return CallResponse('Exit');
        } else {
            return CallResponse('TimeOut');
        }
    }


    //Countdown function
    function TimeoutCallBack() {
        return CallResponse('TimeOut');
    }
    //Page Return
    
    //remove all event handler
    function Clearup() {
        //TO DO:
        App.Timer.ClearTime();
    }
})();
