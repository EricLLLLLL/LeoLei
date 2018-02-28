/*@create by:  tsxiong
*@time: 2016��03��20��
*/
; (function () {
    var nExitTime;
	var CallResponse = App.Cntl.ProcessOnce( function(Response){
        //TO DO:
        if ('OK' == Response) {
            //top.Siu.SetCardReaderLight('CONTINUOUS'); //comment test by art
        } else {
            //top.Siu.SetCardReaderLight('OFF'); //comment test by art
        };
        Clearup();
        //Entry the flows control process.
        App.Cntl.ProcessDriven(Response);
    });
    var Initialize = function () {
        App.Timer.TimeoutDisposal(TimeoutCallBack);
		top.ErrorInfo = top.API.PromptList.No2;
        EventLogin();
        //@initialize scope start
        top.Crd.AcceptAndChipInitialise('AcceptAndChipInitialise', 40000);
        App.Plugin.Voices.play("voi_3");
        top.Siu.SetCardReaderLight('SLOW'); //comment test by art
        ButtonEnable();

    }();//Page Entry

    //@User ocde scope start
    function ButtonDisable() {
        top.API.displayMessage("ButtonDisable");
        document.getElementById('Exit').disabled = true;
        document.getElementById('OK').disabled = true;
    }

    function ButtonEnable() {
        top.API.displayMessage("ButtonEnable");
        document.getElementById('Exit').disabled = false;
        document.getElementById('OK').disabled = false;
    }
    //@User code scope end 

    //event handler
    function onCardInserted() {
        top.API.displayMessage("onCardInserted����");

    }
    //event handler
    function onCardAccepted() {
        top.API.displayMessage("onCardAccepted����,��Ӧ<ok>");
        //return CallResponse('OK');
        // top.Crd.Eject(30000); 
        //return CallResponse('CardAccepted');
    }
    function onCardEjected() {
        top.API.displayMessage("onCardEjected����");
    }

    function onCardTaken() {
        top.API.displayMessage("onCardTaken��������Ӧ<Exit>");
    }

    function onCardAcceptCancelled() {
        top.API.displayMessage("onCardAcceptCancelled");
    }

    function onChipDataReceived(Token, Bytes) {
        window.clearTimeout(nExitTime);
        App.Timer.ClearTime();
        top.API.displayMessage("onChipDataReceived Token:" + Token);
        var arrBytes = Bytes.toArray();
        if ("" === arrBytes.toString()) {
            //������
            ChipInvalidReadTracks();
        } else {
            if ('AcceptAndChipInitialise' == Token) {
                //оƬ������ת����������
                var arrCardType = new Array("CHIPCARD");
                var nRet = top.Dat.SetDataSync(top.API.cardtypeTag, top.API.cardtypeType, arrCardType);
                top.API.displayMessage("SetDataSync CARDTYPE Return:" + nRet);

            } 
        };

         //add by grb for �忨���� begin
        top.API.gTransactiontype = "INQ";
        var arrBalanceInquiryType = new Array("ACCTYPE");   //�忨������ѯ
        var nRet33 = top.Dat.SetDataSync(top.API.BalanceInquiryTag, top.API.BalanceInquiryType, arrBalanceInquiryType);
        top.API.displayMessage("SetDataSync BALANCEINQUIRYTYPE Return:" + nRet33);
        //add by grb for �忨���� end

        return CallResponse('OK');
    }

    function ChipInvalidReadTracks() {
        top.API.displayMessage("ChipInvalidReadTracks");
        var arrCardType = new Array("MAGCARD");
        var nRet = top.Dat.SetDataSync(top.API.cardtypeTag, top.API.cardtypeType, arrCardType);
        top.API.displayMessage("SetDataSync CARDTYPE Return:" + nRet);
    }
    function onChipInvalid() {
        top.API.displayMessage("onChipInvalid");
    }
    
    function onCardAcceptFailed() {
        top.API.displayMessage("onCardAcceptFailed");
        top.ErrorInfo = top.API.PromptList.No4;
        return CallResponse('TimeOut');
    }

    function onCardCaptureFailed() {
        top.API.displayMessage("onCardCaptureFailed");
        top.ErrorInfo = top.API.PromptList.No4;
        return CallResponse('TimeOut');
    }

    function onCardEjectFailed() {
        top.API.displayMessage("onCardEjectFailed");
        top.ErrorInfo = top.API.PromptList.No4;
        return CallResponse('TimeOut');
    }

     function onChipPowerFailed() {
        top.API.displayMessage("onChipPowerFailed");
        top.ErrorInfo = top.API.PromptList.No4;
        return CallResponse('TimeOut');
    }

    function onResetFailed() {
        top.API.displayMessage("onResetFailed");
        top.ErrorInfo = top.API.PromptList.No4;
        return CallResponse('TimeOut');
    }

 function onGetICInfoFailed() {
        top.API.displayMessage("onGetICInfoFailed");
        top.ErrorInfo = top.API.PromptList.No4;
        return CallResponse('TimeOut');
    }

    function onReadIcTLVFailed() {
        top.API.displayMessage("onReadIcTLVFailed");
        top.ErrorInfo = top.API.PromptList.No4;
        return CallResponse('TimeOut');
    }

    //Register the event
    function EventLogin() {
        top.Crd.CardAcceptCancelled.connect(onCardAcceptCancelled);
        top.Crd.CardInserted.connect(onCardInserted);
        top.Crd.CardAccepted.connect(onCardAccepted);
        top.Crd.CardEjected.connect(onCardEjected);
        top.Crd.CardTaken.connect(onCardTaken);
        top.Crd.ChipDataReceived.connect(onChipDataReceived);
        top.Crd.ChipInvalid.connect(onChipInvalid);
        top.Crd.CardAcceptFailed.connect(onCardAcceptFailed);
        top.Crd.CardCaptureFailed.connect(onCardCaptureFailed);
        top.Crd.CardEjectFailed.connect(onCardEjectFailed);
        top.Crd.ChipPowerFailed.connect(onChipPowerFailed);
        top.Crd.ResetFailed.connect(onResetFailed);
        top.Crd.GetICInfoFailed.connect(onGetICInfoFailed);
        top.Crd.ReadIcTLVFailed.connect(onReadIcTLVFailed);
    }

    function EventLogout() {
        top.Crd.CardInserted.disconnect(onCardInserted);
        top.Crd.CardAccepted.disconnect(onCardAccepted);
        top.Crd.CardEjected.disconnect(onCardEjected);
        top.Crd.CardTaken.disconnect(onCardTaken);
        top.Crd.CardAcceptCancelled.disconnect(onCardAcceptCancelled);
        top.Crd.ChipDataReceived.disconnect(onChipDataReceived);
        top.Crd.ChipInvalid.disconnect(onChipInvalid);
        top.Crd.CardAcceptFailed.disconnect(onCardAcceptFailed);
        top.Crd.CardCaptureFailed.disconnect(onCardCaptureFailed);
        top.Crd.CardEjectFailed.disconnect(onCardEjectFailed);
        top.Crd.ChipPowerFailed.disconnect(onChipPowerFailed);
        top.Crd.ResetFailed.disconnect(onResetFailed);
        top.Crd.GetICInfoFailed.disconnect(onGetICInfoFailed);
        top.Crd.ReadIcTLVFailed.disconnect(onReadIcTLVFailed);
    }

    //Countdown function
    function TimeoutCallBack() {
        top.API.displayMessage("ҳ�泬ʱ,�������ʱ���һ����뿨,�ȴ�3��");
        top.ErrorInfo = top.API.PromptList.No3;
        top.Crd.CancelAccept();
        nExitTime = window.setTimeout(TimeoutExit, 3000);
    }
    function TimeoutExit() {
        top.API.displayMessage("3���");
        return CallResponse('Exit');
    }
    document.getElementById("Exit").onclick = function () {
        top.API.displayMessage("���Exit��ť,ִ��<top.API.Crd.CancelAccept()>,��Ӧ<Exit>");
        ButtonDisable();
        top.Crd.CancelAccept();
        return CallResponse('Exit');
    }
    //Page Return
    
    //remove all event handler
    function Clearup() {
        //TO DO:
        App.Plugin.Voices.del();
        EventLogout();
        App.Timer.ClearTime();
    }
})();
