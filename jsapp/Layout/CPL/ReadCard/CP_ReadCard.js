/*@create by:  tsxiong
*@time: 2016年03月20日
*/
; (function () {
    var strCardNo = "";
	var CallResponse = App.Cntl.ProcessOnce( function(Response){
        //TO DO:
        Clearup();
        App.Cntl.ProcessDriven(Response);
    });
    var Initialize = function () {         
        EventLogin();
        App.Plugin.Voices.play("voi_15");
        //@initialize scope start
        App.Timer.TimeoutDisposal(TimeoutCallBack);
        var strCardType = "MAGCARD";
        var objGet = top.Dat.GetDataSync(top.API.cardtypeTag, top.API.cardtypeType);
        if (null == objGet) {
            top.API.displayMessage("GetDataSync CARDTYPE objGet = null");
        }
        else {            
            var arrGet = objGet.toArray();
            strCardType = arrGet[0];
        }
        if ("CHIPCARD" === strCardType) {            
            document.getElementById('PageTag').innerHTML = '正在读取IC卡，请稍候...';
            top.Crd.PBOCGetICInfo(40000);//待修正  超时时间需要考虑页面超时临界值
        } else {
            top.Crd.AcceptAndReadAvailableTracks('2,3', 40000);//待修正  超时时间需要考虑页面超时临界值
        }        
    }();//Page Entry

    //@User ocde scope start
	//top.Crd.PBOCGetICInfo触发该事件
	function onGetICInfoCompleted() {
        top.API.displayMessage("onGetICInfoCompleted");
        strCardNo = top.Crd.CardNumber();
        SetCardNo();        
        document.getElementById('PageTag').innerHTML = '正在认证IC卡数据，请稍候...';
        top.Crd.PBOCReadIcTLV(50000);//读芯片卡数据（55域）
    }
	//top.Crd.PBOCReadIcTLV触发该事件
	function onReadIcTLVCompleted(Info) {
        top.API.displayMessage("ReadIcTLVCompleted");
        var arrICCardData = new Array(Info);
        var nRet = top.Dat.SetDataSync(top.API.iccarddataTag, top.API.iccarddataType, arrICCardData);
		ContinueTrans();
        
    }
	function ContinueTrans() {
		var PtrPaperStatus = top.Ptr.StPaperStatus();
        if (top.Ptr.bDeviceStatus && (PtrPaperStatus == "FULL" || PtrPaperStatus == "LOW")) {
            if (top.API.gbContinueTransFlag === true) {
				return CallResponse("CHIPCARDcontinue");
			}else{
				return CallResponse("OK");
			}
        } else {
            return CallResponse("NoPrint");
        }
	}
    function SetCardNo() {
        var arrCardNo = new Array(strCardNo);
        top.API.gCardno = strCardNo;
        var nRet = top.Dat.SetDataSync(top.API.cardnoTag, top.API.cardnoType, arrCardNo);
        top.Jnl.PrintSync("ReadCardAction");
    }

    //@User code scope end 

    //event handler
    function onCardInserted() {
        top.API.displayMessage("onCardInserted触发");
    }
    //event handler
    function onCardAccepted() {
        top.API.displayMessage("onCardAccepted触发");        
		var strTrack2 = top.Crd.Track2();
		var strTrack3 = top.Crd.Track3();
		if ("" === strTrack2 && "" === strTrack3) {
			top.API.displayMessage(top.API.PromptList.No10);
			top.ErrorInfo = top.API.PromptList.No10;
			return CallResponse("Exit");
		} else {
			var arrTrack2 = new Array();
			arrTrack2 = strTrack2.split("=");
			strCardNo = arrTrack2[0];
		}
		SetCardNo();
		ContinueTrans();     
    }

    function onCardInvalid() {
        top.API.displayMessage(top.API.PromptList.No7);
		top.ErrorInfo = top.API.PromptList.No7;
		return CallResponse("Exit");
    }
    function onChipInvalid() {
        top.API.displayMessage(top.API.PromptList.No7);
		top.ErrorInfo = top.API.PromptList.No7;
		return CallResponse("Exit");
    }

  function onCardAcceptFailed() {
        top.API.displayMessage("onCardAcceptFailed");
        top.ErrorInfo = top.API.PromptList.No7;
        return CallResponse('Exit');
    }


     function onChipPowerFailed() {
        top.API.displayMessage("onChipPowerFailed");
        top.ErrorInfo = top.API.PromptList.No7;
        return CallResponse('Exit');
    }



	function onGetICInfoFailed() {
        top.API.displayMessage("onGetICInfoFailed");
        top.ErrorInfo = top.API.PromptList.No7;
        return CallResponse('Exit');
    }

    function onReadIcTLVFailed() {
        top.API.displayMessage("onReadIcTLVFailed");
        top.ErrorInfo = top.API.PromptList.No7;
        return CallResponse('Exit');
    }

     function onDeviceError() {
        top.API.displayMessage("onDeviceError");
        top.ErrorInfo = top.API.PromptList.No4;
        return CallResponse('Exit');
    }

    

    //Register the event
    function EventLogin() {
        top.Crd.CardInserted.connect(onCardInserted);
        top.Crd.CardAccepted.connect(onCardAccepted);
        top.Crd.CardInvalid.connect(onCardInvalid);
        top.Crd.DeviceError.connect(onDeviceError);
        top.Crd.GetICInfoCompleted.connect(onGetICInfoCompleted);
        top.Crd.ReadIcTLVCompleted.connect(onReadIcTLVCompleted);
		//待修正  PBOC失败事件
        top.Crd.ChipInvalid.connect(onChipInvalid);
        top.Crd.CardAcceptFailed.connect(onCardAcceptFailed);
        top.Crd.ChipPowerFailed.connect(onChipPowerFailed);
        top.Crd.GetICInfoFailed.connect(onGetICInfoFailed);
        top.Crd.ReadIcTLVFailed.connect(onReadIcTLVFailed);

    }

    function EventLogout() {
        top.Crd.CardInserted.disconnect(onCardInserted);
        top.Crd.CardAccepted.disconnect(onCardAccepted);
        top.Crd.CardInvalid.disconnect(onCardInvalid);
        top.Crd.DeviceError.disconnect(onDeviceError);
        top.Crd.GetICInfoCompleted.disconnect(onGetICInfoCompleted);
        top.Crd.ReadIcTLVCompleted.disconnect(onReadIcTLVCompleted);
        top.Crd.ChipInvalid.disconnect(onChipInvalid);
        top.Crd.CardAcceptFailed.disconnect(onCardAcceptFailed);
        top.Crd.ChipPowerFailed.disconnect(onChipPowerFailed);
        top.Crd.GetICInfoFailed.disconnect(onGetICInfoFailed);
        top.Crd.ReadIcTLVFailed.disconnect(onReadIcTLVFailed);
    }

    //Countdown function
    function TimeoutCallBack() {
        top.ErrorInfo = top.API.PromptList.No3;
        return CallResponse("TimeOut");
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