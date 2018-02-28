/*@create by:  grb
*@time: 2016��10��07��
*/
; (function () {
    var InputID = document.getElementById("InputNum-input");
    var Inputdata = "";
	var CallResponse = App.Cntl.ProcessOnce( function(Response){
        //TO DO:
        Clearup();
        //Entry the flows control process.
        App.Cntl.ProcessDriven(Response);
    });
    var Initialize = function () {
        //@initialize scope start
        ButtonDisable();
        document.getElementById("InputPwd-error").innerHTML = "";
        App.Timer.TimeoutDisposal(TimeoutCallBack);
        App.Plugin.Keyboard.show("9", "PageSubject", "KeyboardDiv9");
        KeyboardClick();
        InputID.focus();
        ButtonEnable();
        App.Plugin.Voices.play("voi_44");
    }();//Page Entry

    //@User ocde scope start
    //������������
    function Reinput() {
        Inputdata = "";
        document.getElementById("InputNum-input").innerText = Inputdata;
        ButtonEnable();
    }

    function ButtonDisable() {
        document.getElementById('Exit').disabled = true;
        document.getElementById('OK').disabled = true;
		document.getElementById('Back').disabled = true;
    }

    function ButtonEnable() {
        document.getElementById('Exit').disabled = false;
        document.getElementById('OK').disabled = false;
		document.getElementById('Back').disabled = false;
    }

    document.getElementById('Exit').onclick = function () {
        ButtonDisable();
        top.ErrorInfo = top.API.PromptList.No2;
        return CallResponse('Exit');
    }
	
	document.getElementById('Back').onclick = function () {
        ButtonDisable();
        return CallResponse('Back');
    }

    document.getElementById('OK').onclick = function () {
        ButtonDisable();
		if(Inputdata == "" || Inputdata.length != 11 || Inputdata.substring(0, 1) != "1" ){
			document.getElementById("InputPwd-error").innerHTML = "�ֻ�������������,����������";
			document.getElementById('InputPwd-error').style.display = "block";
			Reinput();
		}else{
            top.API.displayMessage("�ֻ����룺" + Inputdata);  //���ֻ������¼��jslog
            top.API.phoneNum = Inputdata;
            var arrPhoneNum = new Array(Inputdata);
            top.Dat.SetDataSync(top.API.ContentDataTag, top.API.ContentDataType, arrPhoneNum);
			top.Jnl.PrintSync("PhoneNum");//���ֻ������¼��JNL	

            // ȡ�����λ��־λ
            top.API.resetFlag = top.API.depErrFlag ? true : false;

            switch(top.API.CashInfo.Dealtype){
                case 'InLineFlag':
                    return CallResponse('InLineFlag');
                    break;
                case 'OutLineFlag':
                    return CallResponse('OutLineFlag');
                    break;
                case '�޿����۴��':
                    if( !top.API.gbAmountType ){ // �����
                        return CallResponse('LARGEAMOUNT');
                    }else{
                        return CallResponse('OK');
                    }
                    break;
                case 'CancelAccount':
                    if( top.API.gTransactiontype == "WXACCDELETE" ){
                        return CallResponse('CancelAccountByWX');
                    }else{
                        return CallResponse('OK');
                    }
                    break;
                default:
                    return CallResponse('OK');
                    break;
            }
			// return CallResponse('OK');			
		}
    }
    function KeyboardClick() {
        document.getElementById("InputPwd-error").innerHTML = "";
        var arrKeyboardKey = document.getElementsByName("Name_Keyboard");
        for (var i = 0; i < arrKeyboardKey.length; i++) {
            var keyEvent = arrKeyboardKey[i];
            keyEvent.onclick = function (e) {
                InputID.focus();
                App.InputEdit.getCurPosition(InputID);
                if ('�˸�' == this.innerText) {					
					App.InputEdit.getInput(InputID , 1 ,"BS");
                    Inputdata = InputID.value;	
                } else if ('���' == this.innerText) {					
					InputID.value = "";
                    Inputdata = "";													
                } else {					
                    if (Inputdata.length < 11) {
                        App.InputEdit.getInput(InputID , 0 , this.innerText);
					    Inputdata = InputID.value;
                    }                    
                }
            }
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
        //TO DO:
        App.Plugin.Voices.del();
        App.Timer.ClearTime();
    }
})();
