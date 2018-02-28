/*@create by:  tsxiong
*@time: 2016年03月20日
*/
; (function () {
    var sFingerData = ""; 
    var strShowData = "员工号: 1234500001"; 
    var CurSelctRadioId = "TypeNo1";

    var TellerNo = "";
    var TellerNoType = 0;

    var tellerInfoLabels;

    var nReCompare = 0;
    var bIdentify = false;

    var AuthorType = "1";
    var CurFingerData = new Array();
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
        App.Timer.TimeoutDisposal(TimeoutCallBack);
        document.getElementById('err-tip').innerText = "请按指纹进行审核";
        top.Fpi.Identify(-1);
        bIdentify = true;
		AuthorType = top.Dat.GetPrivateProfileSync("MFPI", "bLocalPower", "1", "C:\\TCR\\Middle\\ini\\Setup.ini");		
		document.getElementById('DivTellers').innerHTML = "";
		ShowInfo();
		top.Dat.GetPersistentData("TELLERNO", "STRING");  //获取柜员号  add by hj
        ButtonEnable();
        App.Plugin.Voices.play("voi_13");
        
    }();//Page Entry

    //@User ocde scope start
    function ButtonDisable() {
        document.getElementById('Left_4').disabled = true;
        document.getElementById('Right_4').disabled = true;
        document.getElementById('ReAuthor').disabled = true;
    }

    function ButtonEnable() {
        document.getElementById('Left_4').disabled = false;
        document.getElementById('Right_4').disabled = false;
        document.getElementById('ReAuthor').disabled = false;
    }
	
	
    function ShowInfo() {
		var strName = '*';
        strName += top.API.gCustomerName.substr(1, (top.API.gCustomerName.length - 1));
        document.getElementById('tName').innerHTML = strName; 
		if(top.API.CashInfo.Dealtype == "CWD取款" || top.API.CashInfo.Dealtype == "存折取款"){
			document.getElementById('CheckIdRes').style.display = "block";
			document.getElementById('Check-tip').innerHTML = top.API.gnCWDMoney;
			document.getElementById('CheckNameRes').style.display = "block";
			if(top.API.gbOrderCWD){
				var strMoneyTip = "当笔业务为预约取款,当前客户取款金额为"+top.API.CashInfo.strTransAmount+"元";
				document.getElementById('ResName').innerHTML = strMoneyTip;
			}else{
				var strMoneyTip = "当前客户取款金额为"+top.API.CashInfo.strTransAmount+"元";
				document.getElementById('ResName').innerHTML = strMoneyTip;
			}
		}
    }
    document.getElementById('Left_4').onclick = function () {
        ButtonDisable();
        if (bIdentify) {
            top.Fpi.CancelIdentify();
        }
        top.ErrorInfo = top.API.PromptList.No2;
        top.API.gOldTellerNo = CurSelctRadioId;
        return CallResponse('Exit');
    }

    document.getElementById('Right_4').onclick = function () {
        top.API.displayMessage("点击<确认>");
        return CallResponse('OK');
    }
    
    document.getElementById('ReAuthor').onclick = function () {
        top.API.gOldTellerNo = CurSelctRadioId;
        if (bIdentify) {
            top.Fpi.CancelIdentify();
        }
        return CallResponse('ReAuthor');
    }


    function xmlParser(xmlStr) {
        if (window.DOMParser) {
            return (new window.DOMParser()).parseFromString(xmlStr, "text/xml");
        } else if (window.ActiveXObject) {
            var xmlDoc = new ActiveXObject("Microsoft.XMLDOM");
            xmlDoc.async = false;
            xmlDoc.loadXML(xmlStr);
            return xmlDoc;
        } else {
            throw new Error("No XML parser found");
        }
    }

    function FingerCheck(sUserCode, sToken)
    { 
		var IsSinglebeta = top.Dat.GetPrivateProfileSync("AssConfig", "IsSinglebeta", "0", top.API.gIniFileName);
        if (IsSinglebeta == "2") {
			top.API.displayMessage("FingerCheck JUMP");
			if (top.API.CashInfo.Dealtype == "CWD取款" || top.API.CashInfo.Dealtype == "存折取款") { //add by grb for 存折取款
				top.API.gTransactiontype = "CWD"
			} else if (top.API.CashInfo.Dealtype == "无卡无折存款"  || top.API.CashInfo.Dealtype == "存折存款") {
				top.API.gTransactiontype = "NOCARDDEP";
				if (top.API.gCardOrBookBank == 1) {
					top.API.gTransactiontype = "DEP";
				}
			} else if (top.API.CashInfo.Dealtype == "对公存款") {
				top.API.gTransactiontype = "BUSSINESSDEP";
			} else if (top.API.CashInfo.Dealtype == "DEP存款") {
				top.API.gTransactiontype = "DEP";
			}
			return CallResponse('OK');
		}
        var xmlhttp = null;
        if (window.ActiveXObject) {
            xmlhttp = new ActiveXObject("Microsoft.XMLHTTP");
        }
        else if (window.XMLHttpRequest) {
            xmlhttp = new XMLHttpRequest();
        }
        
    	var URL = "http://10.235.143.59:8080/usap/authentication?ProcessCode=108&ClientUnique=DECQK_41&AuthenticationType=3&UserCode="+ sUserCode + "&Token=" + sToken + "&ProductCode=TECHSHINO";
    	//var URL = "http://10.232.48.83:88/usap/authentication?ProcessCode=108&ClientUnique=NEWBOS&AuthenticationType=3&UserCode="+ sUserCode + "&Token=" + sToken + "&ProductCode=TECHSHINO";
        xmlhttp.open("GET", URL, true); //异步
        xmlhttp.setRequestHeader("Content-Type", "text/xml")
        xmlhttp.onreadystatechange = function () {
            if (xmlhttp.readyState == 4) {
                if (xmlhttp.status == 200) {
                    var xmlDoc = xmlParser(xmlhttp.responseText);
                    var sPrecessCode = xmlDoc.getElementsByTagName("KSSOResponse")[0].firstElementChild.firstChild.nodeValue;
                    var sBusinessStatus = xmlDoc.getElementsByTagName("KSSOResponse")[0].lastElementChild.textContent;
                    if ("SSO_7" != sBusinessStatus) {
                        bIdentify = false;
                        document.getElementById('ReAuthor').style.display = "block";
                        var tip = "授权验证失败！请点击“重新审核”按钮进行重新审核";
                        tip = strShowData + "授权验证失败(" + sBusinessStatus + ")！请点击“重新审核”按钮进行重新审核";
                        document.getElementById('err-tip').innerText = tip;
                        return;
                    }
                    else {
                        top.API.displayMessage("FingerCheck OK");
                        if (top.API.CashInfo.Dealtype == "CWD取款" || top.API.CashInfo.Dealtype == "存折取款") { //add by grb for 存折取款
                            top.API.gTransactiontype = "CWD"
                        } else if (top.API.CashInfo.Dealtype == "无卡无折存款"  || top.API.CashInfo.Dealtype == "存折存款") {
							top.API.gTransactiontype = "NOCARDDEP";
							if (top.API.gCardOrBookBank == 1) {
								top.API.gTransactiontype = "DEP";
							}
						} else if (top.API.CashInfo.Dealtype == "对公存款") {
                            top.API.gTransactiontype = "BUSSINESSDEP";
                        } else if (top.API.CashInfo.Dealtype == "DEP存款") {
                            top.API.gTransactiontype = "DEP";
                        }
                        return CallResponse('OK');
                    }
                }
            }
        }
        xmlhttp.send(null);
    }


    function onDatGetPersistentDataComplete(DataName, DataType, DataValue) { //add by art
        var arrDataValue = DataValue.toArray();
        if ('TELLERNO' == DataName) {
            var tellersObj = eval('(' + arrDataValue.toString() + ')');
            var tellersInfo = tellersObj.TelInfo;
            if (tellersInfo.length == 0) {//无柜员数据
                document.getElementById('err-tip').innerText = "无柜员数据";
            }
            else {
                var tHtml;
                var strType = "";

				
				   var table = document.createElement("table");
				   table.style.textAlign = "center";
                var tr =  document.createElement("tr");
                //todo
                if (tellersInfo.length > 10) {
                    //  style=" overflow:scroll; width:665px; height:400px;"
                    //overflow-y:auto;overflow-x:hidden;
                    var scroller = document.getElementById("DivTellers");
                    scroller.style.overflow = "auto";
                    scroller.style.width = "540px";
                    scroller.style.height = "212px";
					//border:1px solid red;"
					// document.getElementById("DivTellers").style.border = "1px solid greenyellow";
                }


             
                for (var i = 0; i < tellersInfo.length; i++) {
                    if (AuthorType == "1") {
                        strType = "BOEING柜员号"
                    } else {
                        if (tellersInfo[i].teltype == "00") {
                            strType = "员工号"
                        } else if (tellersInfo[i].teltype == "01") {
                            strType = "身份证号"
                        }
                        else if (tellersInfo[i].teltype == "02") {
                            strType = "ABIS柜员号"
                        }
                        else if (tellersInfo[i].teltype == "03") {
                            strType = "BOEING柜员号"
                        }
                    }

                    var td0 = document.createElement("td0");
                    var tdivObj = document.createElement('Newdiv');
                    tdivObj.className = 'TellerDiv';
                    tdivObj.innerHTML = "<input type='button' id='TypeNo" + (i + 1)
                        + "' name ='TypeNo' value='' fingervalue='"
                        + tellersInfo[i].fingerinfo
                        + "' class='RadioSelect'/><label name='labelTellerInfo'><label name='tellersType' for='TypeNo"
                        + (i + 1) + "'>" + strType + "</label>: <label name='tellersNo' for='TypeNo"
                        + (i + 1) + "'>" + tellersInfo[i].telno + "</label></label>";

                    td0.appendChild(tdivObj);
                    tr.appendChild(td0);

                    if ((i + 1) % 2 == 0) {
                         //table.appendChild(tr);
                         tr = document.createElement("tr");;
                    }
					
					table.appendChild(tr);
                }

                document.getElementById("DivTellers").appendChild(table);
                tellerInfoLabels = document.getElementsByName("labelTellerInfo");
                var ATypes = document.getElementsByName("TypeNo");
                typesClick(ATypes);
                document.getElementById(top.API.gOldTellerNo).click();
                funDivTop();
            }
        }
    }

    function onDatGetPersistentDataError(DataName, ErrorCode) {
        if ('TELLERNO' == DataName) {//mod by hj
            document.getElementById('err-tip').innerText = "获取柜员号数据失败！";
        }
    }

    function funDivTop() {
        var radios = document.getElementsByName("TypeNo");
        //控制页面样式
        if (radios.length <= 4) {
            document.getElementById("DivFingerTip").style.top = "300px";
        } else if (radios.length > 4 && radios.length <= 6) {
            document.getElementById("DivFingerTip").style.top = "330px";
        }
        else if (radios.length > 6 && radios.length <= 8) {
            document.getElementById("DivFingerTip").style.top = "360px";
        }
        else {
            document.getElementById("DivFingerTip").style.top = "380px";
        }
    }
    //控制单选按钮图片
    function typesClick(typebtns) {
        for (var i = 0; i < typebtns.length; i++) {
            (function () {
                var p = i;
                typebtns[p].onclick = function () {
                    for (var j = 0; j < typebtns.length; j++) {
                        typebtns[j].style.backgroundImage = "url('Framework/style/Graphics/radio_bg.png')";
                    }
                    typebtns[p].style.backgroundImage = "url('Framework/style/Graphics/radio_click.png')";
                    CurSelctRadioId = typebtns[p].id;

                    CurFingerData[0] = typebtns[p].getAttribute("fingervalue");
                    strShowData = tellerInfoLabels[p].innerText.replace(/\s+/g, "");
					TellerNo = typebtns[p].getAttribute("fingervalue");
                    
                }
            })();
        }
    }

    function onFpiIdentifyComplete(data) {
        bIdentify = false;
        var IsSinglebeta = top.Dat.GetPrivateProfileSync("AssConfig", "IsSinglebeta", "0", top.API.gIniFileName);
        if (IsSinglebeta == "2") {
			top.API.displayMessage("FingerCheck JUMP");
			if (top.API.CashInfo.Dealtype == "CWD取款" || top.API.CashInfo.Dealtype == "存折取款") { //add by grb for 存折取款
				top.API.gTransactiontype = "CWD"
			} else if (top.API.CashInfo.Dealtype == "无卡无折存款"  || top.API.CashInfo.Dealtype == "存折存款") {
				top.API.gTransactiontype = "NOCARDDEP";
				if (top.API.gCardOrBookBank == 1) {
					top.API.gTransactiontype = "DEP";
				}
			} else if (top.API.CashInfo.Dealtype == "对公存款") {
				top.API.gTransactiontype = "BUSSINESSDEP";
			} else if (top.API.CashInfo.Dealtype == "DEP存款") {
				top.API.gTransactiontype = "DEP";
			}
			return CallResponse('OK');
		}
        if (AuthorType == "1") {//本地授权 比对指纹
			
			if (top.API.CashInfo.Dealtype == "CWD取款" || top.API.CashInfo.Dealtype == "存折取款") { //add by grb for 存折取款
                top.API.gTransactiontype = "CWD"
            } else if (top.API.CashInfo.Dealtype == "无卡无折存款"  || top.API.CashInfo.Dealtype == "存折存款") {
				top.API.gTransactiontype = "NOCARDDEP";
				if (top.API.gCardOrBookBank == 1) {
					top.API.gTransactiontype = "DEP";
				}
			} else if (top.API.CashInfo.Dealtype == "对公存款") {
                top.API.gTransactiontype = "BUSSINESSDEP";
            } else if (top.API.CashInfo.Dealtype == "DEP存款") {
                top.API.gTransactiontype = "DEP";
            }
			return CallResponse('OK');		
        }
		if (AuthorType == "2") {
            sFingerData = data;
            FingerCheck(TellerNo, sFingerData);
            return;
        }
    }

    function onFpiTimeout() {
        top.API.displayMessage("onFpiTimeout is done");
        return CallResponse('Exit');
    }

    function onFpiDeviceError() {
        top.API.displayMessage("onFpiDeviceError is done");
		if (!bIdentify) {
            top.Fpi.Identify(-1);
			bIdentify=true;
        }
    }

    function onFpiIdentifyFailed() {
        top.API.displayMessage("onFpiIdentifyFailed is done");
        document.getElementById('ReAuthor').style.display = "block";
        bIdentify = false;
        var tip = strShowData + "审核失败！请点击“重新审核”按钮进行重新审核";
        document.getElementById('err-tip').innerText = tip;
    }

    function onFpiMatchComplete() {
		if (top.API.CashInfo.Dealtype == "CWD取款" || top.API.CashInfo.Dealtype == "存折取款") { //add by grb for 存折取款
            top.API.gTransactiontype = "CWD"
        } else if (top.API.CashInfo.Dealtype == "无卡无折存款"  || top.API.CashInfo.Dealtype == "存折存款") {
			top.API.gTransactiontype = "NOCARDDEP";
			if (top.API.gCardOrBookBank == 1) {
				top.API.gTransactiontype = "DEP";
			}
		} else if (top.API.CashInfo.Dealtype == "对公存款") {
            top.API.gTransactiontype = "BUSSINESSDEP";
        } else if (top.API.CashInfo.Dealtype == "DEP存款") {
            top.API.gTransactiontype = "DEP";
        }
        return CallResponse('OK');
    }

    function onFpiMatchFailed() {
        document.getElementById('ReAuthor').style.display = "block";
        bIdentify = false;
        var tip ="授权失败！请点击“重新审核”按钮进行重新审核";
        document.getElementById('err-tip').innerText = tip;
    }
    //Register the event
    function EventLogin() {
        top.Fpi.IdentifyComplete.connect(onFpiIdentifyComplete);
        top.Fpi.Timeout.connect(onFpiTimeout);
        top.Fpi.DeviceError.connect(onFpiDeviceError);
        top.Fpi.IdentifyFailed.connect(onFpiIdentifyFailed);
        top.Fpi.MatchComplete.connect(onFpiMatchComplete);
        top.Fpi.MatchFailed.connect(onFpiMatchFailed);

        top.Dat.GetPersistentDataComplete.connect(onDatGetPersistentDataComplete); //add by art
        top.Dat.GetPersistentDataError.connect(onDatGetPersistentDataError);  //add by art
    }

    function EventLogout() {
        top.Fpi.IdentifyComplete.disconnect(onFpiIdentifyComplete);
        top.Fpi.Timeout.disconnect(onFpiTimeout);
        top.Fpi.DeviceError.disconnect(onFpiDeviceError);
        top.Fpi.IdentifyFailed.disconnect(onFpiIdentifyFailed);
        top.Fpi.MatchComplete.disconnect(onFpiMatchComplete);
        top.Fpi.MatchFailed.disconnect(onFpiMatchFailed);

        top.Dat.GetPersistentDataComplete.disconnect(onDatGetPersistentDataComplete); //add by art
        top.Dat.GetPersistentDataError.disconnect(onDatGetPersistentDataError); //add by art
    }

    //Countdown function
    function TimeoutCallBack() {
        if (bIdentify) {
            top.Fpi.CancelIdentify();
        }
        top.ErrorInfo = top.API.PromptList.No3;
        return CallResponse('TimeOut');
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
