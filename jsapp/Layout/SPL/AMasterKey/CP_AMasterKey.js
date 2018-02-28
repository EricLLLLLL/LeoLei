/*@create by:  tsxiong
*@time: 2016��03��20��
*/
; (function(){
	var strMasterKey = "";
	var CallResponse = App.Cntl.ProcessOnce( function(Response){
        Clearup();
        App.Cntl.ProcessDriven( Response );
    });
    var Initialize = function() {              
        ButtonDisable();
        EventLogin();
		top.API.gMasterkeyA = "";
        App.Plugin.Keyboard.show("1", "PageSubject", "KeyboardDiv");
        ButtonEnable();
    }();//Page Entry
    function ButtonDisable() {
        document.getElementById('OK').disabled = true;
        document.getElementById('Exit').disabled = true;
    }

    function ButtonEnable() {
        document.getElementById('OK').disabled = false;
        document.getElementById('Exit').disabled = false;
    }
   //ȫ�ֱ�����inputId���༭��ID��
   var InputA = document.getElementById("FirstInput");
   var InputB = document.getElementById("SecondInput");
   var AMasterKey1 = "";
   var AMasterKey2 = "";
   
   InputA.focus();//����궨���ڵ�һ���༭��

    function onClearNum(){      
        InputA.innerText = '';                   
        InputB.innerText = '';                   
        AMasterKey1 = '';
        AMasterKey2 = '';
        InputA.focus();
    }

    function getLinePos(pInputId) {
        var range;
        var cursurPosition;
        pInputId.focus();
        range = document.selection.createRange();
        range.moveStart("character", -pInputId.value.length);
        cursurPosition = range.text.length;
        return cursurPosition;
    }     
	
    //������������ʾ�ڵ�ǰ�༭����
    var oKeyboardKey = document.getElementsByTagName("span");
    for (var i = 0; i < oKeyboardKey.length; i++) {
        var keyEvent = oKeyboardKey[i];
        keyEvent.onclick = function (e) {            
            if ('�˸�' == this.innerText) {                    
                if ((AMasterKey1.length < 17) && (AMasterKey2.length == 0) && (AMasterKey1.length > 0)) {                        
                    AMasterKey1 = AMasterKey1.substr(0, (AMasterKey1.length-1));                        
                    InputA.innerText = AMasterKey1;
                }else if(AMasterKey2.length > 0){                    
                    AMasterKey2 = AMasterKey2.substr(0, (AMasterKey2.length-1));                    
                    InputB.innerText = AMasterKey2;                   
                }                    
		     }else if ('���' == this.innerText) {
                onClearNum();
		     }else {
                document.getElementById("ErrorTip").style.display="none";
                if (AMasterKey1.length < 16) {
                        AMasterKey1  += this.innerText;
                    document.getElementById("FirstInput").innerText = AMasterKey1;           			    
                }else if(AMasterKey2.length < 16){
                    AMasterKey2  += this.innerText;
                    document.getElementById("SecondInput").innerText = AMasterKey2;  
                }
              }
                
            }
        }
    //�˳���ť����Ӧ�����¼�
    document.getElementById('Exit').onclick = function(){   
        ButtonDisable();   
        return CallResponse('Exit');
    }

    document.getElementById('OK').onclick = function(){   
        ButtonDisable();   
        top.API.gMasterkeyA = strMasterKey;
        return CallResponse('OK');
    }
    document.getElementById('KeyboardKey_set').onclick = function(){
		strMasterKey= AMasterKey1+AMasterKey2;       
        if (strMasterKey.length == 32) { 
            var HexMasterKey=top.stringToHex(strMasterKey);
            var strKeyname='MasterKey';
            var tmphexArray = new Array(0);
            top.Pin.ExtendedLoadKey("MasterKey",HexMasterKey,"CRYPT,FUNCTION,MACING,KEYENCKEY",tmphexArray);   
        }else{
            onClearNum();
            document.getElementById("ErrorTip").style.display="inline";
		}
    } 
    //@User code scope end 

    //event handler
    function onKeyLoaded(){
        top.API.displayMessage('����ԿA���سɹ�');
        //��ȡУ��ֵ
        var objGetKVC = top.Dat.GetDataSync("KVC", "STRING");
        if (objGetKVC == "" || null == objGetKVC) {
            top.API.displayMessage("GetDataSync KVC  objGetKVC = null ��ȡʧ��");
            document.getElementById("CheckCode").innerText = "У��ֵ��ȡʧ��";
        }
        else {
            top.API.displayMessage("GetDataSync DEALTYPE Return:" + objGetKVC.toArray());
            document.getElementById("CheckCode").innerText = objGetKVC.toArray()[0];
        }        
        document.getElementById("OK").style.display = "block"; 
        ButtonEnable(); 
    }

   //event handler
	function onDeviceError(){
		 return CallResponse("Exit");
	}
	function onKeyLoadFailed(){
		 return CallResponse("Exit");
	}
   
    //Register the event
    function EventLogin() {
		top.Pin.KeyLoadFailed.connect(onKeyLoadFailed);
        top.Pin.KeyLoaded.connect(onKeyLoaded);
		top.Pin.DeviceError.connect(onDeviceError );
    }

    function EventLogout() {
		top.Pin.KeyLoadFailed.disconnect(onKeyLoadFailed);
        top.Pin.KeyLoaded.disconnect(onKeyLoaded);
        top.Pin.DeviceError.disconnect(onDeviceError );     
    }


      //remove all event handler
    function Clearup(){
        EventLogout();
    }
})();
