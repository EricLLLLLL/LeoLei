; (function(){
	
	var TextAreaDom = document.getElementById("postScript");
    var showFlag = 0; //0����ʾ  ������뷨״̬
    var currentID = "";//��ǰ�������input��id
    var TextData = "";
	var CallResponse = App.Cntl.ProcessOnce( function(Response){
        //TO DO:
        Clearup();
        //Entry the flows control process.
        App.Cntl.ProcessDriven( Response );
    });
    var Initialize = function() {
        EventLogin();
        //@initialize scope start
		top.API.gBUSSINESSDEPINFO = "";
        currentID = "postScript";
        //ҳ�����ʱ����궨λ�����ʹ���ý���
        App.Plugin.ImeHM.cursorEnd(currentID);
        ButtonEnable();
        App.Timer.TimeoutDisposal(TimeoutCallBack);

    }();//Page Entry

    //@User ocde scope start
    document.getElementById("Back").onclick = function(){
        ButtonDisable();
        if (showFlag == 1) {
            //�������뷨
            top.Ime.HideIME('');
        }
        return CallResponse("Back");
    };
	
	document.getElementById("Exit").onclick = function(){
        ButtonDisable();
        if (showFlag == 1) {
            //�������뷨
            top.Ime.HideIME('');
        }
        return CallResponse("Exit");
    };

    document.getElementById("OK").onclick = function(){
        if (showFlag == 1) {
            //�������뷨
            top.Ime.HideIME('');
        }
		var tmpChar = TextAreaDom.innerText;
		var strlen = 0;
		for(var i=0;i<tmpChar.length;i++){
			if(tmpChar.charCodeAt(i)>255){
				strlen+=2;
			}else{
				strlen++
			}
		}
		if(strlen > 39){
			document.getElementById("Check-tip").style.display = "block";
		}else{
			top.API.gBUSSINESSDEPINFO = TextAreaDom.innerText;
			var tempCOMMENTS = new Array(top.API.gBUSSINESSDEPINFO);
            top.Dat.SetDataSync("COMMENTS", "STRING", tempCOMMENTS);
			ButtonDisable();
			return CallResponse("OK");
		}        
    };
    function ButtonDisable() {
        document.getElementById('Back').disabled = true;
		document.getElementById('Exit').disabled = true;
        document.getElementById('OK').disabled = true;
    }

    function ButtonEnable() {
        document.getElementById('Back').disabled = false;
		document.getElementById('Exit').disabled = false;
        document.getElementById('OK').disabled = false;
    };

    
    TextAreaDom.onclick = function(){
		//��� textarea�� �������뷨����
		document.getElementById("Check-tip").style.display = "none";
		App.Plugin.ImeHM.getCurPosition(currentID);
        showFlag = 1;
        //��ʾĳ�ָ�ʽ�����뷨
		top.Ime.ShowIME('0', '', '');
    };
    //���ҳ�������� ���ؼ���
    document.onclick = function (e) {
    //��� textarea�� ��ȥ���뷨����
        e = e || window.event;
        var dom = e.srcElement || e.target;
        if (dom.id != "postScript"&& showFlag == 1) {
            top.Ime.HideIME('');
        }
    }

    //@User code scope end

    //event handler

    function onShowIMECompleted() {
        showFlag = 1;
    }
    function onHideIMECompleted() {
        showFlag = 0;
    }
    function onInputResult(type, value) {
        if (value == "CONFIRM" || value == "CANCEL") {
            top.Ime.HideIME('');
        } else {
            App.Plugin.ImeHM.getInput(currentID, type, value);
        }
    }
    //Register the event
    function EventLogin() {
        top.Ime.ShowIMECompleted.connect(onShowIMECompleted);
        top.Ime.HideIMECompleted.connect(onHideIMECompleted);
		top.Ime.InputResult.connect(onInputResult);
		
    }
    function EventLogout() {
        top.Ime.ShowIMECompleted.disconnect(onShowIMECompleted);
        top.Ime.HideIMECompleted.disconnect(onHideIMECompleted);
		top.Ime.InputResult.disconnect(onInputResult);

    }
    //Countdown function
    function TimeoutCallBack() {
        top.API.displayMessage("TimeoutCallBack����");
        var strErrMsg = "������ʱ�����׽�����";
        var ErrMsg = new Array(strErrMsg);
        var nRet = top.Dat.SetDataSync(top.API.errmsgTag, top.API.errmsgType, ErrMsg);
        top.API.displayMessage("SetDataSync ERRMSG Return:" + nRet);
        if (showFlag == 1) {
            top.Ime.HideIME('');
        }
        return CallResponse('TimeOut');
    };
    //Page Return
    
    //remove all event handler
    function Clearup(){
        //TO DO:
        EventLogout();
        App.Timer.ClearTime();
    }
})();