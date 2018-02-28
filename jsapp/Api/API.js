/*@module:API
 *@create by:  hyhua
 *@time: 2015��11��20��
 ****************************************
 *@��������:
 *���ظ�Ӳ��ģ�顣
 *
 ***************************************
 */


(function () {
	//the module type
	window.API = {
		initialize: function () {
			top.InitFlag = false;
			top.JsFilename = "";
			// top.XmlFilename = "";//by yangjl
			// top.businessTracename = "";//by yangjl
			top.ErrorInfo = "���׳ɹ�!";
			top.FirstOpenFlag = true; //�Ƿ�Ϊ��һ�δ�
			data.call(this); //��JSData�ڶ����ȫ�ֱ�����Ϊtop.API
			global.call(this); //��JSGlobal�ڶ����ȫ�ֱ�����Ϊtop.API
			// displayMessage("Api Initialize...");
			//����Ӳ������

			if (typeof qt != "undefined") {
				if (typeof qt.webChannelTransport != "undefined") {
					new QWebChannel(qt.webChannelTransport, function (channel) {
						top.Crd = channel.objects.MIDC;
						top.Cim = channel.objects.MCIM;
						top.Cdm = channel.objects.MCDM;
						top.Fpi = channel.objects.MFPI;
						top.Pin = channel.objects.MPIN;
						top.Ptr = channel.objects.MRPT;
						top.Idr = channel.objects.MIDR;
						top.Siu = channel.objects.MSIU;
						top.SCr = channel.objects.MSCR;  //buy jilin

						top.Jnl = channel.objects.MJNL;
						top.Tcp = channel.objects.MTCP;
						top.Sys = channel.objects.MSYS;
						top.Tsl = channel.objects.MTSL;
						top.Dat = channel.objects.MDAT;
						top.Ime = channel.objects.MIME;

						top.Ass = channel.objects.MASS;
						top.Rpm = channel.objects.MRPM;
						top.Fck = channel.objects.MFCK;

						//top.CashInfo = new JSCashInfo();

						top.InitFlag = true;
					});
				} else {
					alert("Qt.WebChannelTransport Off");
				}
			} else {
				alert("QT OFF");
			}
		},
		OpenDevice: function () {
			// this.Jnl.WriteLogSync("START OpenDevice", 1);
			top.Jnl.WriteLogSync("START OpenDevice", 1);
			top.Crd.SetServiceName();
			top.Cim.SetServiceName();
			top.Cdm.SetServiceName();
			top.Pin.SetServiceName();
			top.Idr.SetServiceName();
			top.Scr.SetServiceName();
			top.Ptr.SetServiceName();
			top.Siu.SetServiceName();
			top.Fpi.SetServiceName();
			top.Crd.OpenConnection();
			top.Pin.OpenConnection();
			top.Cim.OpenConnection();
			top.Cdm.OpenConnection();
			top.Ptr.OpenConnection();
			top.Idr.OpenConnection();
			top.Fpi.OpenConnection();
			top.Siu.OpenConnection();
			top.Scr.OpenConnection();
		},
		InitOpenFlag: function () { //  false
			// this.Jnl.WriteLogSync("START InitOpenFlag", 1);
			top.Jnl.WriteLogSync("START InitOpenFlag", 1);
			top.Crd.bDeviceStatus = false;
			top.Pin.bDeviceStatus = false;
			top.Cim.bDeviceStatus = false;
			top.Cdm.bDeviceStatus = false;
			top.Ptr.bDeviceStatus = false;
			top.Idr.bDeviceStatus = false;
			top.Fpi.bDeviceStatus = false;
			top.Siu.bDeviceStatus = false;
			top.Scr.bDeviceStatus = false;
			top.API.gbINQ_DEAL = false;
			top.API.gbQRYNAME_DEAL = false;
			top.API.gbPSW_DEAL = false;
			top.API.gbCARDCWD_DEAL = false;
			top.API.gbPBCWD_DEAL = false;
			top.API.gbCARDDEP_DEAL = false;
			top.API.gbPBDEP_DEAL = false;
			top.API.gbNOCARINQ_DEAL = false;
			top.API.gbNOCARDDEP_DEAL = false;
			top.API.gbEXCHANGE_DEAL = false;
			top.API.gbLARGEAMOUNT_DEAL = false;
			top.API.gbBUSINESSDEP_DEAL = false;
			top.API.gbACCDELETE_DEAL = false;
			top.API.gbTRANSFER_DEAL = false;
			top.API.gbTRANSFERCANCEL_DEAL = false;
			top.API.gbPCA_DEAL = false;
			top.API.gbCTR_DEAL = false;
			top.API.gbSAVEBACK_DEAL = false;
			top.API.gbWXCancel_DEAL = false;
		},
		CheckDeviceStatus: function () {
			this.InitOpenFlag();
			top.API.gPartsStatus = top.Sys.GetPartsStatusSync();
			top.API.gTransStatus = top.Sys.PossibleTransactionSync();
			if (top.API.gTransStatus == "-1") {
				top.API.displayMessage("-------��ȡ������״̬ʧ��------");
				return false;
			} else {
				/*
				TransStatus[0]=�˻�����ѯ
				TransStatus[1]=������ѯ
				TransStatus[2]=����
				TransStatus[3]=��ȡ��
				TransStatus[4]=��ȡ��
				TransStatus[5]=�����
				TransStatus[6]=�۴��
				TransStatus[7]=�޿����۴���ѯ
				TransStatus[8]=�޿����۴��
				TransStatus[9]=�㳮�һ�
				TransStatus[10]=����
				TransStatus[11]=�Թ����
				TransStatus[12]=��������
				TransStatus[13]=ת�˻��
				TransStatus[14]=ת�˳���
				TransStatus[15]=����ת����
				TransStatus[16]=����ת����			
				TransStatus[17]=�����ش�			
				TransStatus[18]=΢������			
				*/
				top.API.displayMessage("��ȡ����״̬top.API.gTransStatus=" + top.API.gTransStatus);
				var arrTransStatus = top.API.gTransStatus.split(",");
				if (parseInt(arrTransStatus[0]) == 1) {
					top.API.gbINQ_DEAL = true;
				}
				if (parseInt(arrTransStatus[1]) == 1) {
					top.API.gbQRYNAME_DEAL = true;
				}
				if (parseInt(arrTransStatus[2]) == 1) {
					top.API.gbPSW_DEAL = true;
				}
				if (parseInt(arrTransStatus[3]) == 1) {
					top.API.gbCARDCWD_DEAL = true;
				}
				if (parseInt(arrTransStatus[4]) == 1) {
					top.API.gbPBCWD_DEAL = true;
				}
				if (parseInt(arrTransStatus[5]) == 1) {
					top.API.gbCARDDEP_DEAL = true;
				}
				if (parseInt(arrTransStatus[6]) == 1) {
					top.API.gbPBDEP_DEAL = true;
				}
				if (parseInt(arrTransStatus[7]) == 1) {
					top.API.gbNOCARINQ_DEAL = true;
				}
				if (parseInt(arrTransStatus[8]) == 1) {
					top.API.gbNOCARDDEP_DEAL = true;
				}
				if (parseInt(arrTransStatus[9]) == 1) {
					top.API.gbEXCHANGE_DEAL = true;
				}
				if (parseInt(arrTransStatus[10]) == 1) {
					top.API.gbLARGEAMOUNT_DEAL = true;
				}
				if (parseInt(arrTransStatus[11]) == 1) {
					top.API.gbBUSINESSDEP_DEAL = true;
				}
				if (parseInt(arrTransStatus[12]) == 1) {
					top.API.gbACCDELETE_DEAL = true;
				}
				if (parseInt(arrTransStatus[13]) == 1) {
					top.API.gbTRANSFER_DEAL = true;
				}
				if (parseInt(arrTransStatus[14]) == 1) {
					top.API.gbTRANSFERCANCEL_DEAL = true;
				}
				if (parseInt(arrTransStatus[15]) == 1) {
					top.API.gbPCA_DEAL = true;
				}
				if (parseInt(arrTransStatus[16]) == 1) {
					top.API.gbCTR_DEAL = true;
				}
				if (parseInt(arrTransStatus[17]) == 1) {
					top.API.gbSAVEBACK_DEAL = true;
				}
				if (parseInt(arrTransStatus[18]) == 1) {
					top.API.gbWXCancel_DEAL = true;
				}
			}
			if (top.API.gPartsStatus == "") {
				top.API.displayMessage("-------��ȡ������״̬ʧ��------");
				return false;
			} else {
				/*
				SZABC_ALL = 0,
				SZABC_IDC = 1,
				SZABC_SCR = 2,
				SZABC_IDR = 5,
				SZABC_PIN = 9,
				SZABC_RPT = 12,
				SZABC_CDM = 16,
				SZABC_CIM = 17,
				SZABC_SIU = 19,
				SZABC_FPI = 27,
				SZABC_TNW1 = 32             //������������1
				------------------------------------
				SZABC_NO_DEVICE = '0',      //�޴��豸����
				SZABC_NOMAL = 'Z',          //����
				SZABC_UNNOMAL = '1',        //����
				SZABC_LOW = '2',            //����
				SZABC_EMPTY = '3',          //�����
				SZABC_FULL = '4'            //������
				*/
				if ((top.API.gPartsStatus.substr(16, 1) == "Z") && (top.API.gPartsStatus.substr(17, 1) == "Z")) {
					top.Cim.bDeviceStatus = true;
					top.Cdm.bDeviceStatus = true;
				}
				if (top.API.gPartsStatus.substr(1, 1) == "Z") {
					top.Crd.bDeviceStatus = true;
				}
				if (top.API.gPartsStatus.substr(27, 1) == "Z") {
					top.Fpi.bDeviceStatus = true;
				}
				if (top.API.gPartsStatus.substr(5, 1) == "Z") {
					top.Idr.bDeviceStatus = true;
				}
				if (top.API.gPartsStatus.substr(9, 1) == "Z") {
					top.Pin.bDeviceStatus = true;
				}
				if (top.API.gPartsStatus.substr(12, 1) == "Z") {
					top.Ptr.bDeviceStatus = true;
				}
				if (top.API.gPartsStatus.substr(2, 1) == "Z") {
					top.Scr.bDeviceStatus = true;
				}
			}
			return true;
		},
		GetUnitInfo: function (arrParam) {
			var arrCurrentCount = new Array();
			var arrSplite = new Array();
			var arrReturnInfo = new Array();
			arrCurrentCount = arrParam;
			for (i = 0; i < arrCurrentCount.length; i++) {
				arrSplite = arrCurrentCount[i].split(":");
				arrReturnInfo[i] = parseInt(arrSplite[1]);
			}

			return arrReturnInfo;
		},
		IsOntherCashCWD: function () {
			var bRet = false; //�ж��㳮�Ƿ��ȡ
			for (i = 0; i < top.API.CashInfo.nCountOfUnits; i++) {
				if (top.API.CashInfo.arrUnitRemain[i] > 45 &&
					top.API.CashInfo.arrUnitCurrency[i] != 100 &&
					top.API.CashInfo.arrUnitCurrency[i] != 0) {
					bRet = true;
				}
			}
			return bRet;
		},


		displayMessage: function (strMsg) {
			strMsg = top.JsFilename + "   " + strMsg;
			var mode = arguments[1] ? arguments[1] : 1;
			top.Jnl.WriteLogSync(strMsg, mode);
		},
		


	};
})();