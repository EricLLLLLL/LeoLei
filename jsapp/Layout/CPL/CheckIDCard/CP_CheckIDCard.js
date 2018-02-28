/*@create by:  tsxiong
*@time: 2016��03��20��
*/
; (function () {
    var URL = "";//���Ի�����ַ--http://10.232.53.166:9080/CIIS/services/IdchkService
    var PicBase64 = "";//�����˲�base64
	var strBusinessNo = "";//PimageID
	var strBatchNo = "";//���κ�
	var ClientIp = "";//���ip
	var PictureMD5 = "";//�����˲�ͼ��MD5ֵ
	var CheckPicPath = "";//�����˲�ͼ��·��
	var CertID = "";//�ͻ�ID
	var CheckSystemParams = "";//�����˲��־
	var BusiId = "";//ҵ��ID
	var TaskId = "";//����ID
    var TerminalId = "";//�豸���
    var strHeadParams = ""//Vtsͷ����
    var VtsAddr = 'http://10.191.3.241:8052/VTSWS29/Service.svc/';//Vts�ӿڵ�ַ
	var MachNO = "";//�豸��
	var PointNo = "";//������
	var TradeTypeId = "";//��������0078
	var TellerNo = "";//��Ա��
	var TerminalNo = "";//�豸���
	var bISCreateBusi = false;//�Ƿ�Ϊ��������
	var bISRiskLevel = false;//�Ƿ�Ϊ��ѯ���յȼ�
	var RequestIdType = "";//��������
	var RequestId = "";//ҵ�����κ�
	var MachID = "";
				 
	var arrCardInfo = top.API.gstrIdCardInfo.split(",");
	var CallResponse = App.Cntl.ProcessOnce( function(Response){
        //TO DO:
        if(Response == "OK" && top.API.gReadIdCardFlag == 2){
            Response = "Audit";
        }
        Clearup();
        //Entry the flows control process.
        App.Cntl.ProcessDriven(Response);
    });	
	
    var Initialize = function () {
        EventLogin();
        // Gni test
        // return CallResponse('OK');
        //@initialize scope start
        App.Timer.TimeoutDisposal(TimeoutCallBack);
        var IsSinglebeta = top.Dat.GetPrivateProfileSync("AssConfig", "IsSinglebeta", "0", top.API.gIniFileName);
        if (IsSinglebeta == "1") {
            top.ErrorInfo = "00|������ݺ���������һ�£��Ҵ�����Ƭ";
            return CallResponse("OK");
        }else{
            //InitIdCardInfo();
			GetVTSParams();
			RequestId = GetAndSetBusiNo("RequestId");			
            top.Dat.GetPersistentData("IDCHECKURL", "STRING");      
        }
    } (); //Page Entry

    //@User ocde scope start
	function InitIdCardInfo(){
		if(arrCardInfo[1] == '��'){
			arrCardInfo[1] = '1';
		}else if(arrCardInfo[1] == 'Ů'){
			arrCardInfo[1] = '2';
		}
		arrCardInfo[2].replace("��", "");
		var tmp1 = arrCardInfo[7].substr(0,4)+"-"+ arrCardInfo[7].substr(4,2)+"-" + arrCardInfo[7].substr(6,2);
		arrCardInfo[7] = tmp1;
		var tmp2 = arrCardInfo[8].substr(0,4)+"-"+ arrCardInfo[8].substr(4,2)+"-" + arrCardInfo[8].substr(6,2);
		arrCardInfo[8] = tmp2;
		
	}
	
	function GetVTSParams() {
        var strRet = '';
        var xmldom = new ActiveXObject("Microsoft.XMLDOM");
        xmldom.load('./ini/TCR_Params.xml');
        if (xmldom.parseError != 0){
              throw new Error("XML parsing error: " + xmldom.parseError.reason);
        }
		var Params = xmldom.getElementsByTagName("VTS");
		VtsAddr = Params[0].getAttribute("VtsAddr");
		MachNO = Params[0].getAttribute("MachNO");
		TradeTypeId = Params[0].getAttribute("TradeTypeId");
		TerminalNo = Params[0].getAttribute("TerminalNo");
		TellerNo = Params[0].getAttribute("TellerNo");
		PointNo = Params[0].getAttribute("PointNo");
	}
	
    function GetAndSetBusiNo(keyname) {
        var tmpBusinessNo = top.Dat.GetPrivateProfileSync("AssConfig", keyname, "", top.API.gIniFileName);//"41000";
        var strRet='';
        var sDate = top.GetDate12byte().substring(0, 8);
		var tmpPointNo = PointNo;
		if(keyname == "BusinessNo"){
			if (sDate != tmpBusinessNo.substring(0, 8)) {
				strRet = sDate + tmpPointNo + TellerNo + "0001";
				var nRet = top.Dat.WritePrivateProfileSync("AssConfig", keyname, strRet, top.API.gIniFileName);//"41000";
			}else{
				var tmpStr = "0000";
				var tmpNo = (parseInt(tmpBusinessNo.substring(18,22))+1).toString();
				if (tmpNo.length < 4 && tmpNo != "") {
					tmpNo = tmpStr.substr(0,(4-tmpNo.length)) + tmpNo;
				}
				var NewBusinessNo = sDate + tmpPointNo + TellerNo + tmpNo;
				strRet = NewBusinessNo;
				var nRet = top.Dat.WritePrivateProfileSync("AssConfig", keyname, NewBusinessNo, top.API.gIniFileName);//"41000";
			}
		}else{
			if (sDate != tmpBusinessNo.substring(0, 8)) {
				strRet = sDate + TerminalNo + "0001";
				var nRet = top.Dat.WritePrivateProfileSync("AssConfig", keyname, strRet, top.API.gIniFileName);//"41000";
			}else{
				var tmpStr = "0000";
				var tmpNo = (parseInt(tmpBusinessNo.substring(16,20))+1).toString();
				if (tmpNo.length < 4 && tmpNo != "") {
					tmpNo = tmpStr.substr(0,(4-tmpNo.length)) + tmpNo;
				}
				var NewBusinessNo = sDate + TerminalNo + tmpNo;
				strRet = NewBusinessNo;
				var nRet = top.Dat.WritePrivateProfileSync("AssConfig", keyname, NewBusinessNo, top.API.gIniFileName);//"41000";
			}
		}
        
        return strRet;
    }

    function GetHeadParams() {
        var strRet = "";
		var tmpPointNo = PointNo;
		if(bISRiskLevel){
			bISRiskLevel = false;
			if(PointNo.length > 4){
				tmpPointNo = PointNo.substr(PointNo.length-4,4);
			}
		}
		strRet = ',"MachNO":"'+MachNO+'","PointNo":"'+tmpPointNo+'","TradeTypeId":"'+TradeTypeId+'","TellerNo":"'+TellerNo+'","TerminalNo":"'+TerminalNo+'"';
        return strRet;
    }

    function GetHeadMsg() {
        var strRet = '';        
		strHeadParams = GetHeadParams();
        strRet = '{"RequestId":"' + RequestId + '"'+strHeadParams+'}';
        return strRet;
    }
    function UploadPic(){
        var xmldom = new ActiveXObject("Microsoft.XMLDOM");
        xmldom.load('./ini/TCR_Params.xml');
        if (xmldom.parseError != 0){
              throw new Error("XML parsing error: " + xmldom.parseError.reason);
        }
		var Params = xmldom.getElementsByTagName("ASS");
		var strAssAddr = Params[0].getAttribute("AssAddr");
		var strAppCheck = Params[0].getAttribute("AppCheck");
		var strAppNo = Params[0].getAttribute("AppNo");
		var strUserNo = Params[0].getAttribute("UserNo");
		var strBranchCode = Params[0].getAttribute("BranchCode");
		strBatchNo = Params[0].getAttribute("BatchNo");
				 
		var str = TerminalId + new Date().getTime();//�����豸��+ʱ�������MD5		
		strBusinessNo = str.MD5();	
		top.API.displayMessage("�ϴ�Ӱ��ƽ̨����BusinessNo="+strBusinessNo); 
		top.Ass.OpenConnectionSync(strAssAddr, strAppNo, strAppCheck, strUserNo, strBranchCode, strBusinessNo, strBatchNo);	
        top.Ass.AssUpload(arrCardInfo[10]+","+arrCardInfo[11]+","+CheckPicPath+","+arrCardInfo[9]);
	}
    //����ʶ��ӿ�
    function bFaceCheck(){
		var IsCheckFace = top.Dat.GetPrivateProfileSync("TestConfig", "IsCheckFace", "0", top.API.gIniFileName);
        if (IsCheckFace == "2") {            
            return true;
        }	
		var paramInfo = new Object();
		paramInfo.methodName = "";
		paramInfo.channel = "0408";
		paramInfo.img1 = top.API.gCheckIdCardpic;
		paramInfo.img2 = arrCardInfo[9];
		if (IsCheckFace == "1") {            
            paramInfo.img2 = top.API.gCheckIdCardpic;
        }		
		paramInfo.channelTradingFlowNum = RequestId;
		paramInfo.equipmentNum = TerminalNo;
		paramInfo.organizationNum = "41"+PointNo;
		paramInfo.tellerNum = TellerNo;
		paramInfo.citycode = "41";
		paramInfo.cId = arrCardInfo[4];
		var strParam =JSON.stringify(paramInfo);
		var retMsg = "";	
		var sh_facecheckresult = top.Fck.faceMatch(strParam);
		var jsonResult = eval('('+sh_facecheckresult+')');
        top.API.displayMessage("��������"+jsonResult.result+"\���ƶȽ����"+jsonResult.simResult+"/���ƶȷ�����"+jsonResult.sim+"\ʶ����ˮ�ţ�"+jsonResult.flowCode+"\ʶ��ƽ̨��Ӧ�����"+sh_facecheckresult);
        if (jsonResult.result == "0" && jsonResult.simResult == "1") {
            return true;
        }else{
            return false;
        }
	}

    function StartServer(){
        var bRet = false;
        var nRet = RemoteObj.ServerStart(7701);
		if (nRet == 0) {
            var bStartBCD = op.API.Ass.BcdStartServiceSync('Server','��ʼ��','D:\\PRelease\\API\RemoteVideo\\RemoteVideo_TestServer.exe');
            if (bStartBCD) {
                bRet=true;
                top.API.displayMessage("�򿪱������豸�ɹ�");
            }else{
                top.API.displayMessage("�򿪱������豸ʧ��");
            }
		}else{
            top.API.displayMessage("����Զ����˷���ʧ��");
        }
		return bRet;
	}

    function CallFunc(DealName,bResult,strMsg){
        var tmpHeadMsg='';
        var bSuccess = false;		
		switch (DealName){
			case "FindMachInfo":
				if(bResult){
					top.API.displayMessage("FindMachInfo�ɹ�,strMsg="+strMsg); 
					var arrRetMsg = strMsg.split(",");
                    bSuccess = true;
					MachID = arrRetMsg[0];
					TerminalNo = arrRetMsg[1];
					TellerNo = arrRetMsg[2];
					PointNo = arrRetMsg[3];
					var tmpUrl = VtsAddr+'FindAllParams';				
					tmpHeadMsg = GetHeadMsg();
					top.API.displayMessage("��ʼԶ�����,FindAllParams,tmpHeadMsg="+tmpHeadMsg);
					top.FindAllParams(CallFunc,tmpHeadMsg,tmpUrl);
				}else{
					if(strMsg == ""){
						strMsg = 'ͨѶ���ϣ�����ȡ��';
					}
					top.API.displayMessage("DealName="+DealName+",strMsg="+strMsg); 
				}
				break;
			case "FindAllParams":
				if(bResult){
					var strParam = arrCardInfo[0]+","+arrCardInfo[4]+","+strMsg+","+URL;
					CheckSystemParams = 2;//1Ϊ����  2Ϊ����							
					if(top.CheckInfo(strParam)){
						top.ErrorInfo = top.getStrRetMsg();
						PicBase64 = top.getStrB64();                       
						// top.API.displayMessage("�����˲�ɹ�,strParam= "+strParam); 
						top.API.displayMessage("�����˲�ɹ�"); 
						CheckPicPath = top.Ass.SaveBase64ToFileSync(PicBase64);
						top.API.gCheckIdCardpic = CheckPicPath;
						var strPictureMD5 = top.Ass.GetMD5ByFileSync(CheckPicPath, true);
						PictureMD5 = strPictureMD5.toUpperCase();
                        if (bFaceCheck()) {
                            bSuccess = true;							
                            var tmpUrl = VtsAddr+'QueryCertInfo';
                            tmpHeadMsg = GetHeadMsg();
							if(arrCardInfo[1] == '��'){
								arrCardInfo[1] = '1';
							}else if(arrCardInfo[1] == 'Ů'){
								arrCardInfo[1] = '2';
							}
							arrCardInfo[2] = arrCardInfo[2].replace("��", "");
							var tmp1 = arrCardInfo[3].substr(0,4)+"-"+ arrCardInfo[3].substr(4,2)+"-" + arrCardInfo[3].substr(6,2);
							arrCardInfo[3] = tmp1;
							strParam = arrCardInfo[4]+','+arrCardInfo[1]+','+arrCardInfo[7]+','+arrCardInfo[8]+','+PictureMD5+','+arrCardInfo[3];
							// top.API.displayMessage("֤������ʶ��ɹ�,��ʼQueryCertInfo,strParam= "+strParam); 
							top.API.displayMessage("֤������ʶ��ɹ�,��ʼQueryCertInfo"); 
						    top.QueryCertInfo(CallFunc,tmpHeadMsg,strParam,tmpUrl);
                        }else{
                            strMsg = '֤������ʶ��ʧ�ܣ�����ȡ��';
						    top.API.displayMessage("DealName=FaceCheck,strMsg="+strMsg); 
                        }
					}else{
                        strMsg = '�����˲�ʧ�ܣ�����ȡ��';
						top.API.displayMessage("DealName=CheckInfo,strMsg="+top.getStrRetMsg()); 
					}
				}else{
					if(strMsg == ""){
						strMsg = 'ͨѶ���ϣ�����ȡ��';
					}
					top.API.displayMessage("DealName="+DealName+",strMsg="+strMsg); 
				}
				break;
			case "QueryRiskLevel":
				if(bResult){
					if(strMsg == "20" ||strMsg == "30"){
                        top.ErrorInfo = "�߷��տͻ�!"
						top.API.displayMessage("�߷��տͻ�"); 
					}else{
                        bSuccess = true; 
						top.API.displayMessage("��ѯ�ͻ����յȼ��ɹ�!");
						return CallResponse('OK');                        
					}
				}else{
					if(strMsg == ""){
						strMsg = 'ͨѶ���ϣ�����ȡ��';
					}
					top.API.displayMessage("DealName="+DealName+",strMsg="+strMsg); 
				}
				break;
			case "UpdateCertInfo":
				if(bResult){	
					bSuccess = true;					
					var Date = top.GetDate12byte().substring(0, 4)+"-"+top.GetDate12byte().substring(4, 6)+"-"+top.GetDate12byte().substring(6, 8);
					BusiId = GetAndSetBusiNo("BusinessNo");
					top.API.displayMessage("UpdateCertInfo�ɹ������������Ϣ�Լ�����BusinessNo="+BusiId);
					bISCreateBusi = true;
					top.API.CreateBusiListHead = GetHeadMsg();
					top.API.CreateBusiListUrl = VtsAddr+'CreateBusiList';
					//top.API.CreateBusiListParam = BusiId+','+arrCardInfo[4]+','+arrCardInfo[0]+','+Date+','+CertID;
					top.API.CreateBusiListParam = BusiId+','+top.API.gCardno+','+top.API.gCustomerName+','+Date+','+CertID+','+MachID;			
					top.API.CreateTaskHead = GetHeadMsg();
					top.API.CreateTaskUrl = VtsAddr+'CreateTask';
					top.API.CreateTaskParam = BusiId; 
					var tmpUrl = VtsAddr+'QueryRiskLevel';
					bISRiskLevel = true;
					tmpHeadMsg = GetHeadMsg();
					//top.API.displayMessage("��ѯ�ͻ����յȼ���QueryRiskLevel��top.API.gIdNumberForRisk="+top.API.gIdNumberForRisk);
					top.API.displayMessage("��ѯ�ͻ����յȼ���QueryRiskLevel��");
					top.QueryRiskLevel(CallFunc,tmpHeadMsg,top.API.gIdNumberForRisk,tmpUrl);
				}else{
					if(strMsg == ""){
						strMsg = 'ͨѶ���ϣ�����ȡ��';
					}
					top.API.displayMessage("DealName="+DealName+",strMsg="+strMsg); 
				}
				break;
			case "QueryCertInfo":
				if(bResult){
					top.API.displayMessage("QueryCertInfo�ɹ�,�ͻ�Ϊ����ͻ�"); 
					var arrRetMsg = strMsg.split(",");
                    bSuccess = true;
					//arrRetMsg[2] ="aaa";
					if(PictureMD5 != arrRetMsg[2]){
						bSuccess = true;						
						top.API.displayMessage("MD5ֵ��һ���������½��ͻ�����ʼ�ϴ�Ӱ��ƽ̨��PictureMD5="+PictureMD5+",arrRetMsgMD5="+arrRetMsg[2]); 
						UploadPic();
					}else{
						CertID = arrRetMsg[0];
						strBusinessNo = arrRetMsg[1];
						PictureMD5 = arrRetMsg[2];
						strBatchNo = arrRetMsg[3];
						var retCheckSystem = arrRetMsg[4];
						top.API.displayMessage("retCheckSystem="+retCheckSystem);
						if(CheckSystemParams != retCheckSystem){			
							if(arrCardInfo[1] == '��'){
								arrCardInfo[1] = '1';
							}else if(arrCardInfo[1] == 'Ů'){
								arrCardInfo[1] = '2';
							}
							var strParam = CertID + ',' + arrCardInfo[0] + ',' + arrCardInfo[0] + ',' + arrCardInfo[1] + ',' + CheckSystemParams;
                            tmpHeadMsg = GetHeadMsg();
                            var tmpUrl = VtsAddr+'UpdateCertInfo';
							// top.API.displayMessage("�����˲��־��ͬ����ҪUpdateCertInfo��strParam="+strParam); 
							top.API.displayMessage("�����˲��־��ͬ����ҪUpdateCertInfo"); 
							top.UpdateCertInfo(CallFunc,tmpHeadMsg,strParam,tmpUrl);
						}else{                            
                            var Date = top.GetDate12byte().substring(0, 4)+"-"+top.GetDate12byte().substring(4, 6)+"-"+top.GetDate12byte().substring(6, 8);
                            BusiId = GetAndSetBusiNo("BusinessNo");
							bISCreateBusi = true;
							top.API.displayMessage("���������Ϣ�Լ�����BusinessNo="+BusiId);
                            top.API.CreateBusiListHead = GetHeadMsg();
                            top.API.CreateBusiListUrl = VtsAddr+'CreateBusiList';
		                    //top.API.CreateBusiListParam = BusiId+','+arrCardInfo[4]+','+arrCardInfo[0]+','+Date+','+CertID;
							top.API.CreateBusiListParam = BusiId+','+top.API.gCardno+','+top.API.gCustomerName+','+Date+','+CertID+','+MachID;	
		                    top.API.CreateTaskHead = GetHeadMsg();
		                    top.API.CreateTaskUrl = VtsAddr+'CreateTask';
		                    top.API.CreateTaskParam = BusiId; 
                            var tmpUrl = VtsAddr+'QueryRiskLevel';
							bISRiskLevel = true;
							tmpHeadMsg = GetHeadMsg();
							// top.API.displayMessage("��ѯ�ͻ����յȼ���QueryRiskLevel��top.API.gIdNumberForRisk="+top.API.gIdNumberForRisk);
							top.API.displayMessage("��ѯ�ͻ����յȼ���QueryRiskLevel");
							top.QueryRiskLevel(CallFunc,tmpHeadMsg,top.API.gIdNumberForRisk,tmpUrl);	
						}
					}					
				}else{
					if(strMsg == "InsertCertInfo"){
						top.API.displayMessage("InsertCertInfo");
						top.API.displayMessage("QueryCertInfo�ɹ�,��Ҫ�½��ͻ���Ϣ����ʼ�ϴ�Ӱ��ƽ̨");
						bSuccess = true;
						UploadPic();
					}else{
						if(strMsg == ""){
							strMsg = 'ͨѶ���ϣ�����ȡ��';
						}
						top.API.displayMessage("DealName="+DealName+",strMsg="+strMsg); 
					} 
				}
				break;			
			case "InsertCertInfo":
				if(bResult){
					bSuccess = true;
					CertID = strMsg;					
					var Date = top.GetDate12byte().substring(0, 4)+"-"+top.GetDate12byte().substring(4, 6)+"-"+top.GetDate12byte().substring(6, 8);
                    BusiId = GetAndSetBusiNo("BusinessNo");
					bISCreateBusi = true;
					top.API.displayMessage("InsertCertInfo�ɹ������������Ϣ�Լ�����BusinessNo="+BusiId);
                    top.API.CreateBusiListHead = GetHeadMsg();
					top.API.CreateBusiListUrl = VtsAddr+'CreateBusiList';
					//top.API.CreateBusiListParam = BusiId+','+arrCardInfo[4]+','+arrCardInfo[0]+','+Date+','+CertID;
					top.API.CreateBusiListParam = BusiId+','+top.API.gCardno+','+top.API.gCustomerName+','+Date+','+CertID+','+MachID;	
					top.API.CreateTaskHead = GetHeadMsg();
					top.API.CreateTaskUrl = VtsAddr+'CreateTask';
					top.API.CreateTaskParam = BusiId;   
					var tmpUrl = VtsAddr+'QueryRiskLevel';
					bISRiskLevel = true;
                    tmpHeadMsg = GetHeadMsg();
					// top.API.displayMessage("��ѯ�ͻ����յȼ���QueryRiskLevel��top.API.gIdNumberForRisk="+top.API.gIdNumberForRisk);
					top.API.displayMessage("��ѯ�ͻ����յȼ���QueryRiskLevel");
					top.QueryRiskLevel(CallFunc,tmpHeadMsg,top.API.gIdNumberForRisk,tmpUrl);					
				}else{
					if(strMsg == ""){
						strMsg = 'ͨѶ���ϣ�����ȡ��';
					}
					top.API.displayMessage("DealName="+DealName+",strMsg="+strMsg); 
				}
				break;			
			default:
				break;
		}
        if (!bSuccess) {
            top.API.displayMessage("���ͨѶ���ϣ�");
			top.ErrorInfo = strMsg;
            return CallResponse("Exit");
        }
	}
    /********************************************************************************************************/    
    //�ϴ�Ӱ��ƽ̨�¼�
    function onAssUploadComplete() {
        //top.API.displayMessage("�ϴ��ɹ���"); 		
        var strParam = top.API.gstrIdCardInfo+","+strBusinessNo+","+ PictureMD5+","+ strBatchNo+","+ CheckSystemParams;
        tmpHeadMsg = GetHeadMsg();
        var tmpUrl = VtsAddr+'InsertCertInfo';
		//top.API.displayMessage("�ϴ�Ӱ��ƽ̨�ɹ�����ʼInsertCertInfo��strParam="+strParam+"//tmpHeadMsg="+tmpHeadMsg);
        top.API.displayMessage("�ϴ�Ӱ��ƽ̨�ɹ�����ʼInsertCertInfo��"+"tmpHeadMsg="+tmpHeadMsg);
		top.InsertCertInfo(CallFunc,tmpHeadMsg,strParam,tmpUrl);
    }
    function onAssUploadFail() {
        top.API.displayMessage("�ϴ�ʧ�ܣ�"); 
		top.ErrorInfo = "���ͨѶ���ϣ�";
        return CallResponse("Exit");
    }
    
    //event handler
        /********************************************************************************************************/    
    //��������ģ��
    function onDatGetPersistentDataComplete(DataName, DataType, DataValue) {
        var arrDataValue = DataValue.toArray();
        if ('IDCHECKURL' == DataName) {
            URL = arrDataValue[0];
            if (URL == "") {
                top.API.displayMessage("�����˲��ַΪ�գ�");
                top.ErrorInfo = "�����˲��쳣��";
                return CallResponse('Exit');
            }else{
                //�ж��Ƿ�Ϊ���������̣������������гֿ��˱������֤�����˲飬ֱ�ӵ���CheckInfo
				
                if (top.API.gReadIdCardFlag==1) {
                    var strParam = arrCardInfo[0] + "," + arrCardInfo[4] + "," + "" + "," + URL;
					if(top.CheckInfo(strParam)){
						top.ErrorInfo = top.getStrRetMsg();
						PicBase64 = top.getStrB64();                     	
						// top.API.displayMessage("�����˷��û������֤���������˲�ɹ�,strParam= "+strParam);
						top.API.displayMessage("�����˷��û������֤���������˲�ɹ�");
						CheckPicPath = top.Ass.SaveBase64ToFileSync(PicBase64);
						top.API.gCheckIdCardpic = CheckPicPath;						
                        if (bFaceCheck()) {
                            top.API.gReadIdCardFlag = 2;
							return CallResponse("OK"); 
                        }else{                            
							top.API.displayMessage("֤������ʶ��ʧ�ܣ�����ȡ����");
							top.ErrorInfo = "֤������ʶ��ʧ�ܣ�����ȡ����";
							return CallResponse("Exit"); 						    
                        }
					}else{
                        top.API.displayMessage("�����˷��û������֤�����˲�ʧ�ܣ�");
                        top.ErrorInfo = "�����˲�ʧ�ܣ�";
                        return CallResponse("Exit"); 
					}                   
                } else {
                    //���������֤�����˲飬����־λ�ָ����Ա���������˲���ɻ���������ת��
                    if (top.API.gReadIdCardFlag == 2) {
                        top.API.gReadIdCardFlag = 0;
                    }
					var tmpUrl = VtsAddr+'FindMachInfo';					
					var tmpHeadMsg = '{"RequestId":"' + RequestId + '","MachNo":"'+MachNO+'"}';
					top.API.displayMessage("��ʼԶ�����,FindMachInfo,tmpHeadMsg="+tmpHeadMsg);
					top.FindMachInfo(CallFunc,tmpHeadMsg,tmpUrl);
                }
            }            
        }   
    }
	
    function onDatGetPersistentDataError(DataName, ErrorCode) {
        top.API.displayMessage("onDatGetPersistentDataError is done,DataName=" + DataName + ",ErrorCode=" + ErrorCode);
    }
    //Register the event
    function EventLogin() {
        top.Dat.GetPersistentDataComplete.connect(onDatGetPersistentDataComplete);
        top.Dat.GetPersistentDataError.connect(onDatGetPersistentDataError);
        top.Ass.AssUploadComplete.connect(onAssUploadComplete);
        top.Ass.AssUploadFail.connect(onAssUploadFail);
    }

    function EventLogout() {
        top.Dat.GetPersistentDataComplete.disconnect(onDatGetPersistentDataComplete);
        top.Dat.GetPersistentDataError.disconnect(onDatGetPersistentDataError);
        top.Ass.AssUploadComplete.disconnect(onAssUploadComplete);
        top.Ass.AssUploadFail.disconnect(onAssUploadFail);
    }

    //Countdown function
    function TimeoutCallBack() {
        top.ErrorInfo = top.API.PromptList.No3;
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
