/*@create by:  tsxiong
*@time: 2016��03��20��
*/
; (function () {
	var CallResponse = App.Cntl.ProcessOnce( function(Response){
        //TO DO:
        Clearup();
        //Entry the flows control process.
        App.Cntl.ProcessDriven(Response);
    });
    var Initialize = function () {
        ButtonDisable();
        EventLogin();
        //@initialize scope start 
        App.Plugin.Keyboard.show("2", "PageSubject", "KeyboardDiv");
        App.Timer.TimeoutDisposal(TimeoutCallBack,ButtonDisable);
        //��ȡ�������ݲ���
        var nRet1 = top.Dat.GetPersistentData(top.API.hostipTag,top.API.hostipType);
        top.API.displayMessage("P������IP��ַ: GetPersistentData  HOSTIP Return:"+nRet1);
        ButtonEnable();
    }();//Page Entry
    function ButtonDisable() {
        top.API.displayMessage("ButtonDisable");
        document.getElementById('Exit').disabled = true;
    }

    function ButtonEnable() {
        top.API.displayMessage("ButtonEnable");
        document.getElementById('Exit').disabled = false;
    }
    //�������
    var strHostIP1 = "";
    var strHostIP2 = "";
    var strHostIP3 = "";
    var strHostIP4 = "";
    var strHostPort = "";
    var strMonitorHostIP1 = "";
    var strMonitorHostIP2 = "";
    var strMonitorHostIP3 = "";
    var strMonitorHostIP4 = "";
    var strMonitorHostPort = "";
    var strMonitorType = "";
    var HostIP1 = document.getElementById("HostIP1Input");
    var HostIP2 = document.getElementById("HostIP2Input");
    var HostIP3 = document.getElementById("HostIP3Input");
    var HostIP4 = document.getElementById("HostIP4Input");
    var HostPort = document.getElementById("HostPortInput");
    var MonitorHostIP1 = document.getElementById("MonitorHostIP1Input");
    var MonitorHostIP2 = document.getElementById("MonitorHostIP2Input");
    var MonitorHostIP3 = document.getElementById("MonitorHostIP3Input");
    var MonitorHostIP4 = document.getElementById("MonitorHostIP4Input");
    var MonitorHostPort = document.getElementById("MonitorHostPortInput");
    var MonitorType = document.getElementById("MonitorTypeInput");
    var InputFlag = 1;
    HostIP1.focus();

    var oKeyboardKeyInput = document.getElementsByTagName("input");
    for (var j = 0; j < oKeyboardKeyInput.length; j++) {
        var inpt = oKeyboardKeyInput[j];
        inpt.onclick = function (e) {
            inputId = document.activeElement.id;
            if (inputId == "HostIP1Input") {
                InputFlag = 1;
            } else if (inputId == "HostIP2Input") {
                InputFlag = 2;
            } else if (inputId == "HostIP3Input") {
                InputFlag = 3;
            } else if (inputId == "HostIP4Input") {
                InputFlag = 4;
            } else if (inputId == "HostPortInput") {
                InputFlag = 5;
            } else if (inputId == "MonitorHostIP1Input") {
                InputFlag = 6;
            } else if (inputId == "MonitorHostIP2Input") {
                InputFlag = 7;
            } else if (inputId == "MonitorHostIP3Input") {
                InputFlag = 8;
            } else if (inputId == "MonitorHostIP4Input") {
                InputFlag = 9;
            } else if (inputId == "MonitorHostPortInput") {
                InputFlag = 10;
            } else if (inputId == "MonitorTypeInput") {
                InputFlag = 11;
            }
        }
    }

    function onClearNum() {
        if (InputFlag == 1) {
            HostIP1.innerText = '';
            strHostIP1 = '';
            HostIP1.focus();
        } else if (InputFlag == 2) {
            HostIP2.innerText = '';
            strHostIP2 = '';
            HostIP2.focus();
        } else if (InputFlag == 3) {
            HostIP3.innerText = '';
            strHostIP3 = '';
            HostIP3.focus();
        } else if (InputFlag == 4) {
            HostIP4.innerText = '';
            strHostIP4 = '';
            HostIP4.focus();
        } else if (InputFlag == 5) {
            HostPort.innerText = '';
            strHostPort = '';
            HostPort.focus();
        } else if (InputFlag == 6) {
            MonitorHostIP1.innerText = '';
            strMonitorHostIP1 = '';
            MonitorHostIP1.focus();
        } else if (InputFlag == 7) {
            MonitorHostIP2.innerText = '';
            strMonitorHostIP2 = '';
            MonitorHostIP2.focus();
        } else if (InputFlag == 8) {
            MonitorHostIP3.innerText = '';
            strMonitorHostIP3 = '';
            MonitorHostIP3.focus();
        } else if (InputFlag == 9) {
            MonitorHostIP4.innerText = '';
            strMonitorHostIP4 = '';
            MonitorHostIP4.focus();
        } else if (InputFlag == 10) {
            MonitorHostPort.innerText = '';
            strMonitorHostPort = '';
            MonitorHostPort.focus();
        } else if (InputFlag == 11) {
            MonitorType.innerText = '';
            strMonitorType = '';
            MonitorType.focus();
        }
    }

    var oKeyboardKey = document.getElementsByTagName("span");
    for (var i = 0; i < oKeyboardKey.length; i++) {
        var keyEvent = oKeyboardKey[i];
        keyEvent.onclick = function (e) {
            if ('�˸�' == this.innerText) {
                if (InputFlag == 1) {
                    if (strHostIP1.length != 0) {
                        strHostIP1 = strHostIP1.substr(0, (strHostIP1.length - 1));
                        HostIP1.innerText = strHostIP1;
                        HostIP1.focus();
                    }
                } else if (InputFlag == 2) {
                    if (strHostIP2.length != 0) {
                        strHostIP2 = strHostIP2.substr(0, (strHostIP2.length - 1));
                        HostIP2.innerText = strHostIP2;
                        HostIP2.focus();
                    }
                } else if (InputFlag == 3) {
                    if (strHostIP3.length != 0) {
                        strHostIP3 = strHostIP3.substr(0, (strHostIP3.length - 1));
                        HostIP3.innerText = strHostIP3;
                        HostIP3.focus();
                    }
                } else if (InputFlag == 4) {
                    if (strHostIP4.length != 0) {
                        strHostIP4 = strHostIP4.substr(0, (strHostIP4.length - 1));
                        HostIP4.innerText = strHostIP4;
                        HostIP4.focus();
                    }
                } else if (InputFlag == 5) {
                    if (strHostPort.length != 0) {
                        strHostPort = strHostPort.substr(0, (strHostPort.length - 1));
                        HostPort.innerText = strHostPort;
                        HostPort.focus();
                    }
                } else if (InputFlag == 6) {
                    if (strMonitorHostIP1.length != 0) {
                        strMonitorHostIP1 = strMonitorHostIP1.substr(0, (strMonitorHostIP1.length - 1));
                        MonitorHostIP1.innerText = strMonitorHostIP1;
                        MonitorHostIP1.focus();
                    }
                } else if (InputFlag == 7) {
                    if (strMonitorHostIP2.length != 0) {
                        strMonitorHostIP2 = strMonitorHostIP2.substr(0, (strMonitorHostIP2.length - 1));
                        MonitorHostIP2.innerText = strMonitorHostIP2;
                        MonitorHostIP2.focus();
                    }
                } else if (InputFlag == 8) {
                    if (strMonitorHostIP3.length != 0) {
                        strMonitorHostIP3 = strMonitorHostIP3.substr(0, (strMonitorHostIP3.length - 1));
                        MonitorHostIP3.innerText = strMonitorHostIP3;
                        MonitorHostIP3.focus();
                    }
                } else if (InputFlag == 9) {
                    if (strMonitorHostIP4.length != 0) {
                        strMonitorHostIP4 = strMonitorHostIP4.substr(0, (strMonitorHostIP4.length - 1));
                        MonitorHostIP4.innerText = strMonitorHostIP4;
                        MonitorHostIP4.focus();
                    }
                } else if (InputFlag == 10) {
                    if (strMonitorHostPort.length != 0) {
                        strMonitorHostPort = strMonitorHostPort.substr(0, (strMonitorHostPort.length - 1));
                        MonitorHostPort.innerText = strMonitorHostPort;
                        MonitorHostPort.focus();
                    }
                } else if (InputFlag == 11) {
                    if (strMonitorType.length != 0) {
                        strMonitorType = strMonitorType.substr(0, (strMonitorType.length - 1));
                        MonitorType.innerText = strMonitorType;
                        MonitorType.focus();
                    }
                }
            } else if ('���' == this.innerText) {
                onClearNum();
            } else {
                if (InputFlag == 1) {
                    //if (strHostIP1.length < 6) {                          
                    strHostIP1 += this.innerText;
                    HostIP1.innerText = strHostIP1;
                    //  }                
                } else if (InputFlag == 2) {
                    // if (strPsw.length < 6) {                          
                    strHostIP2 += this.innerText;
                    HostIP2.innerText = strHostIP2;
                    //  }                                   
                } else if (InputFlag == 3) {
                    strHostIP3 += this.innerText;
                    HostIP3.innerText = strHostIP3;
                } else if (InputFlag == 4) {
                    strHostIP4 += this.innerText;
                    HostIP4.innerText = strHostIP4;
                } else if (InputFlag == 5) {
                    strHostPort += this.innerText;
                    HostPort.innerText = strHostPort;
                } else if (InputFlag == 6) {
                    //if (strMonitorHostIP1.length < 6) {                          
                    strMonitorHostIP1 += this.innerText;
                    MonitorHostIP1.innerText = strMonitorHostIP1;
                    //  }                
                } else if (InputFlag == 7) {
                    // if (strPsw.length < 6) {                          
                    strMonitorHostIP2 += this.innerText;
                    MonitorHostIP2.innerText = strMonitorHostIP2;
                    //  }                                   
                } else if (InputFlag == 8) {
                    strMonitorHostIP3 += this.innerText;
                    MonitorHostIP3.innerText = strMonitorHostIP3;
                } else if (InputFlag == 9) {
                    strMonitorHostIP4 += this.innerText;
                    MonitorHostIP4.innerText = strMonitorHostIP4;
                } else if (InputFlag == 10) {
                    strMonitorHostPort += this.innerText;
                    MonitorHostPort.innerText = strMonitorHostPort;
                } else if (InputFlag == 11) {
                    //if (strMonitorHostIP1.length < 6) {                          
                    strMonitorType += this.innerText;
                    MonitorType.innerText = strMonitorType;
                    //  }                
                }
            }

        }
    }

    document.getElementById('KeyboardKey_set').onclick = function () {
        if ( (strHostIP1 == "" ) || (strHostIP2 == "") || (strHostIP3 == "") || (strHostIP4 == "") || (strMonitorHostIP1 == "" ) || (strMonitorHostIP2 == "") || (strMonitorHostIP3 == "") || (strMonitorHostIP4 == "")) {
                document.getElementById("tipdiv").innerText = "�����IP��ַ����Ϊ�գ�";
        }else if ((strHostPort == "") || (strMonitorHostPort == "")) {
                 document.getElementById("tipdiv").innerText ="����Ķ˿ڲ���Ϊ�գ�";      
        }else if (strMonitorType == "") {
                 document.getElementById("tipdiv").innerText ="����ļ������ģʽ����Ϊ�գ�";                        
        }else if ((strHostIP1 > 255) || (strHostIP2 >  255) || (strHostIP3 >  255) || (strHostIP4 >  255) || (strMonitorHostIP1 > 255) || (strMonitorHostIP2 > 255) || (strMonitorHostIP3 > 255) || (strMonitorHostIP4 > 255)){
                 document.getElementById("tipdiv").innerText ="IP��ַ�������";
        }else if ((strHostPort > 65535) || (strMonitorHostPort > 65535) || (strHostPort < 1) || (strMonitorHostPort < 1)){
                 document.getElementById("tipdiv").innerText = "�˿��������";
        }else if (strMonitorType >1) {
                 document.getElementById("tipdiv").innerText = "�������ģʽ�������"
        }else{
                var strHostIP = strHostIP1 +"." +strHostIP2 + "." +strHostIP3 +"." +strHostIP4;
				var inputvalue1 = new Array(strHostIP);
		        top.API.displayMessage('HOSTIP='+inputvalue1); 
				var nRet1 = top.Dat.SetPersistentData(top.API.hostipTag, top.API.hostipType, inputvalue1); 
		        top.API.displayMessage('SetPersistentData HOSTIP Return:'+nRet1);   
                                      
                //return CallResponse('OK');
         }
    }

    document.getElementById('Exit').onclick = function () {
        ButtonDisable();
        return CallResponse('Exit');
    }

    function onDatGetPersistentDataComplete(DataName, DataType, DataValue) {
        var arrGet = DataValue.toArray();
        top.API.displayMessage("onDatGetPersistentDataComplete is done,DataName="+DataName+",DataType="+DataType+",DataValue="+arrGet[0]);
        var showinfo = arrGet[0];
        if ('HOSTIP' == DataName) {
			top.API.displayMessage("DataName=HOSTIP");
			var strIP = showinfo.split(".");
            for (var i=0; i<4;i++) {
                var displayid = "HostIP"+(i+1)+"Input";
                document.getElementById(displayid).innerText= strIP [i]; 
                document.getElementById(displayid).value= strIP [i]; 
			}
			strHostIP1 = HostIP1.value;
			strHostIP2 = HostIP2.value;
			strHostIP3 = HostIP3.value;
			strHostIP4 = HostIP4.value;
			var nRet2 = top.Dat.GetPersistentData(top.API.hostportTag, top.API.hostportType);
			top.API.displayMessage("P�������˿�: GetPersistentData HOSTPORT  Return:"+nRet2);
        }
        if ('HOSTPORT' == DataName) {
			HostPort.innerText = showinfo;
			strHostPort = HostPort.value;
			var nRet3 = top.Dat.GetPersistentData(top.API.monitorserveripTag, top.API.monitorserveripType);
			top.API.displayMessage("�������IP��ַ: GetPersistentData MONITORSERVERIP Return:"+nRet3);
        }
		if('MONITORSERVERIP' == DataName){
			top.API.displayMessage("DataName=MONITORSERVERIP");
			var strIP = showinfo.split(".");
			for (var i=0; i<4;i++) {
				var displayid = "MonitorHostIP"+(i+1)+"Input";
				document.getElementById(displayid).innerText= strIP [i]; 
				document.getElementById(displayid).value= strIP [i]; 
			}
			strMonitorHostIP1 = MonitorHostIP1.value;
			strMonitorHostIP2 = MonitorHostIP2.value;
			strMonitorHostIP3 = MonitorHostIP3.value;
			strMonitorHostIP4 = MonitorHostIP4.value;
			var nRet4 = top.Dat.GetPersistentData(top.API.monitorserverportTag, top.API.monitorserverportType);
			top.API.displayMessage("��������˿�: GetPersistentData MONITORSERVERPORT Return:"+nRet4);
        } 				
        if ('MONITORSERVERPORT' == DataName) {
                MonitorHostPort.innerText = showinfo;
                strMonitorHostPort = MonitorHostPort.value;
                var nRet5 = top.Dat.GetPersistentData(top.API.monitorservermodeTag, top.API.monitorservermodeType);
                top.API.displayMessage("����������ӷ�ʽ: GetPersistentData  MONITORSERVERMODE Return:"+nRet5);		
        }
        if ('MONITORSERVERMODE' == DataName) {
                MonitorType.innerText = showinfo;
               strMonitorType = MonitorType.value;
        }
	}

    function onDatGetPersistentDataError(DataName, ErrorCode) {
         top.API.displayMessage("onDatGetPersistentDataError is done,DataName="+DataName+",ErrorCode="+ErrorCode);
         //alert("��ȡʧ��");
		 if ('HOSTIP' == DataName){
		     var nRet2 = top.Dat.GetPersistentData(top.API.hostportTag, top.API.hostportType);
                top.API.displayMessage("P�������˿�: GetPersistentData HOSTPORT  Return:"+nRet2); 
         }
         if('HOSTPORT' == DataName){
             var nRet3 = top.Dat.GetPersistentData(top.API.monitorserveripTag, top.API.monitorserveripType);
                top.API.displayMessage("�������IP��ַ: GetPersistentData MONITORSERVERIP Return:"+nRet3);	
         }
         if('MONITORSERVERIP' == DataName){
             var nRet4 = top.Dat.GetPersistentData(top.API.monitorserverportTag, top.API.monitorserverportType);
                top.API.displayMessage("��������˿�: GetPersistentData MONITORSERVERPORT Return:"+nRet4);			
        }
        if ('MONITORSERVERPORT' == DataName){ 
            var nRet5 = top.Dat.GetPersistentData(top.API.monitorservermodeTag, top.API.monitorservermodeType);
                top.API.displayMessage("����������ӷ�ʽ: GetPersistentData  MONITORSERVERMODE Return:"+nRet5); 
         }      
    }
	
    function onDatSetPersistentDataComplete(DataName) {
        top.API.displayMessage("onDatSetPersistentDataComplete is done,DataName="+DataName);
        if ('HOSTIP' == DataName){
                var inputvalue2 = new Array(strHostPort);
				top.API.displayMessage('HOSTPORT='+inputvalue2); 
				var nRet2 = top.Dat.SetPersistentData(top.API.hostportTag, top.API.hostportType, inputvalue2);
		        top.API.displayMessage('SetPersistentData HOSTPORT Return:'+nRet2);   
         }
         if('HOSTPORT' == DataName){
                var strMonitorHostIP = strMonitorHostIP1 +"." +strMonitorHostIP2 + "." +strMonitorHostIP3 +"." +strMonitorHostIP4;
		        var inputvalue3 = new Array(strMonitorHostIP);  
				top.API.displayMessage('MONITORSERVERIP='+inputvalue3); 
				var nRet3 = top.Dat.SetPersistentData(top.API.monitorserveripTag, top.API.monitorserveripType, inputvalue3);
				top.API.displayMessage('SetPersistentData MONITORSERVERIP Return:'+nRet3);  	
         }
         if('MONITORSERVERIP' == DataName){
                var inputvalue4 = new Array(strMonitorHostPort);
				top.API.displayMessage('MONITORSERVERPORT='+inputvalue4); 
				var nRet4 = top.Dat.SetPersistentData(top.API.monitorserverportTag, top.API.monitorserverportType, inputvalue4);
				top.API.displayMessage('SetPersistentData MONITORSERVERPORT Return:'+nRet4);  			
        }
        if ('MONITORSERVERPORT' == DataName){ 
                var inputvalue5 = new Array(strMonitorType);
			    top.API.displayMessage('MONITORSERVERMODE='+inputvalue5);   
			    var nRet5 = top.Dat.SetPersistentData(top.API.monitorservermodeTag, top.API.monitorservermodeType, inputvalue5);
		        top.API.displayMessage('SetPersistentData MONITORSERVERMODE Return:'+nRet5);    
                }    
      if ('MONITORSERVERMODE' == DataName){
                return CallResponse('OK');
      }
    }
	
    function onDatSetPersistentDataError(DataName, ErrorCode) {
        top.API.displayMessage("onDatSetPersistentDataError is done,DataName="+DataName+",ErrorCode="+ErrorCode);
		//alert("�趨ʧ��,�������趨��");
        //return CallResponse('OK');
    }

    //Register the event
    function EventLogin() {
        top.Dat.GetPersistentDataComplete.connect(onDatGetPersistentDataComplete);
        top.Dat.GetPersistentDataError.connect(onDatGetPersistentDataError);
        top.Dat.SetPersistentDataComplete.connect(onDatSetPersistentDataComplete);
        top.Dat.SetPersistentDataError.connect(onDatSetPersistentDataError);
    }

    function EventLogout() {
        top.Dat.GetPersistentDataComplete.disconnect(onDatGetPersistentDataComplete);
        top.Dat.GetPersistentDataError.disconnect(onDatGetPersistentDataError);
        top.Dat.SetPersistentDataComplete.disconnect(onDatSetPersistentDataComplete);
        top.Dat.SetPersistentDataError.disconnect(onDatSetPersistentDataError);
    }

    //Countdown function
    function TimeoutCallBack() {

        return CallResponse('TimeOut');
    }
    //Page Return
    
    //remove all event handler
    function Clearup() {
        //TO DO:
        EventLogout();
        App.Timer.ClearTime();
    }
})();
