/*@create by:  tsxiong
 *@time: 2016��03��20��
 */
;(function () {
    var TransType = null;
    var bNextPageFlag = false; //��ˮ��ת��־λ��1��OK   2��TradeFailed
    var strErrMsg = "ͨѶʧ�ܣ����׽�����";
    var Check = "";
    var JnlNum = null;//�����ȡ������ˮ�ű���
    var timeoutFlag = false;//�Ƿ��ѳ�ʱ��ҳ�泬ʱ��ͨѶ��ʱ����־λ
    var NGtimeId = null; //��ʾ������Ϣ��ʱ��
    var strCallResponse = "Error";//������ת����
    var bTimeOutStart = false;//�ж��Ƿ���ʾ������Ϣ
    var CallResponse = App.Cntl.ProcessOnce(function (Response) {
        //TO DO:
        AfterSendDisposal();
        Clearup();
        App.Cntl.ProcessDriven(Response);
    });
    var Initialize = function () {
        EventLogin();
        top.API.gResponsecode = '';
        //���������ĵ�ʱ������������ʾ
        if (top.API.gTransactiontype == "QRYCUSTNAME") {
            document.getElementById('Tipid').innerText = "���ڲ�ѯ�����˻���Ϣ...";
            App.Plugin.Voices.play("voi_34");
        }
        else {
            App.Plugin.Voices.play("voi_28");
        }
        //@initialize scope start
        App.Timer.TimeoutDisposal(TimeoutCallBack);
        top.ErrorInfo = top.API.PromptList.No1;
        SetCashUnitInfoToTCP();
        TransType = top.API.gTransactiontype;
        var arrTRANSACTIONTYPE = new Array(TransType);
        top.Dat.SetDataSync(top.API.transactiontypeTag, top.API.transactiontypeType, arrTRANSACTIONTYPE);
        top.Dat.GetPersistentData(top.JnlnumTag, top.JnlnumType);

    }();//Page Entry

    //@User ocde scope start

    //@User code scope end 
    function SetCashUnitInfoToTCP() {
        var strCounts = "";
        var strCurrency = "";
        var strInfo = "";
        for (i = 0; i < top.API.CashInfo.nCountOfUnits; i++) {
            switch (top.API.CashInfo.arrUnitCurrency[i]) {
                case 100:
                    strCurrency = "15650100";
                    break;
                case 50:
                    strCurrency = "15650050";
                    break;
                case 20:
                    strCurrency = "15650020";
                    break;
                case 10:
                    strCurrency = "15650010";
                    break;
                case 5:
                    strCurrency = "15650005";
                    break;
                case 1:
                    strCurrency = "15650001";
                    break;
                default:
                    strCurrency = "00000000";
                    break;
            }
            switch (top.API.CashInfo.arrUnitRemain[i].toString().length) {
                case 1:
                    strCounts = "000" + top.API.CashInfo.arrUnitRemain[i].toString();
                    break;
                case 2:
                    strCounts = "00" + top.API.CashInfo.arrUnitRemain[i].toString();
                    break;
                case 3:
                    strCounts = "0" + top.API.CashInfo.arrUnitRemain[i].toString();
                    break;
                case 4:
                    strCounts = top.API.CashInfo.arrUnitRemain[i].toString();
                    break;
                default:
                    strCounts = "0000";
                    break;
            }
            strInfo = strInfo + strCurrency + strCounts;
        }
        var arrDeviceState = new Array(strInfo);
        top.Dat.SetDataSync("DEVICESTATE", "STRING", arrDeviceState);
    }

    function BeforeSendDisposal() {
        var arrTransactionResult = new Array("ͨѶʧ��");
        top.API.gTakeCardAndPrint = true;
        top.Dat.SetDataSync("TRANSACTIONRESULT", "STRING", arrTransactionResult);
        switch (TransType) {
            case "DEP":
                SetTransKind("�����");
                break;
            case "NOCARDDEP":
                if (top.API.CashInfo.Dealtype == "���۴��") {
                    SetTransKind("���۴��");
                } else {
                    SetTransKind("�޿����۴��");
                }
                break;
            case "BUSSINESSDEP":
                SetTransKind("�Թ����");
                var arrTRANSACTIONTYPE = new Array(top.API.gBUSSINESSDEPINFO);
                top.Dat.SetDataSync("BUSSINESSDEPINFO", "STRING", arrTRANSACTIONTYPE);
                break;
            case "CWD":
                if (top.API.CashInfo.Dealtype == "����ȡ��") {
                    SetTransKind("����ȡ��");
                } else {
                    SetTransKind("��ȡ��");
                }
                break;
            case "INQ":
                if (top.API.CashInfo.Dealtype == "����ҵ��") {
                    SetTransKind("�۲�ѯ");
                } else {
                    SetTransKind("����ѯ");
                }
                break;
            case "QRYCUSTNAME":
                SetTransKind("������ѯ");
                break;
            case "CWC":
                SetTransKind("ȡ�����");
                break;
            case "QRYBUSSINESSACCOUNT":
                SetTransKind("�Թ��˻���ѯ");
                break;
            case "QRYCWDMONEY":
                SetTransKind("�ۼƽ���ѯ");
                break;
            case "QRYACCOUNTTYPE":
                SetTransKind("�˻����Ͳ�ѯ");
                break;
            default:
                top.API.displayMessage("�������ʹ���,TransType=" + TransType);
                break;
        }
        top.Jnl.PrintSync("BeforeSendDisposal");
    }

    function TradeCompleted() {
        if (bNextPageFlag) {
            var arrTransactionResult = new Array("���׳ɹ�");
            top.API.gTakeCardAndPrint = false;
            top.Dat.SetDataSync("TRANSACTIONRESULT", "STRING", arrTransactionResult);
        }
        switch (TransType) {
            case 'CWC':
                if (bNextPageFlag == true) {
                    strCallResponse = "CWC_OK";
                } else {
                    strCallResponse = "CWC_NG";
                }
                break;
            case 'BUSSINESSDEP':
            case 'NOCARDDEP':
            case 'DEP':
                if (top.API.gbPartCashIn) {
                    if (bNextPageFlag == true) {
                        var Amount = top.API.CashInfo.strTransAmount + ".00";
                        top.API.partCwcTip = true;
                        arrTransactionResult = new Array("�����˽��:" + Amount);
                        var arrComments = new Array("����δ���˽�����ϵ���й�����Ա");
                    } else {
                        top.API.partCwcTip = false;
                        arrTransactionResult = new Array("ʧ��");
                        var arrComments = new Array("�豸���ϣ�����ϵ���й�����Ա");
                    }
                    top.API.gTakeCardAndPrint = true;
                    top.Dat.SetDataSync("TRANSACTIONRESULT", "STRING", arrTransactionResult);
                    top.Dat.SetDataSync("COMMENTS", "STRING", arrComments);
                    var bSetDepErrorReset = top.Dat.GetPrivateProfileSync("TransactionConfig", "bSetDepErrorReset", "0", top.API.gIniFileName);
                    if (bSetDepErrorReset == '1') {
                        strCallResponse = 'CashinErrorReset';
                    } else if (bSetDepErrorReset == '2') {
                        strCallResponse = 'DirectAccount';
                    }
                } else {
                    if (bNextPageFlag == true) {
                        strCallResponse = "DEP_OK";
                    } else {
                        var arrTransactionResult = new Array("�泮ʧ��");
                        top.API.gTakeCardAndPrint = true;
                        top.Dat.SetDataSync("TRANSACTIONRESULT", "STRING", arrTransactionResult);
                        strCallResponse = "DEP_NG";
                    }
                }
                break;
            case 'CWD':
                if (bNextPageFlag == true) {

                    if (top.API.gMixSelfCWD) {
                        strCallResponse = "EXCWD";
                    } else {
                        strCallResponse = "CWD_OK";
                    }
                } else {
                    strCallResponse = "CWD_NG";
                }
                break;
            case 'QRYCUSTNAME':
                if (bNextPageFlag == true) {
                    top.API.gTransStatus = top.Sys.PossibleTransactionSync();
                    var objGet1 = top.Dat.GetDataSync(top.API.customernameTag, top.API.customernameType);
                    if (null == objGet1) {
                        top.API.gCustomerName = "";
                    } else {
                        top.API.gCustomerName = objGet1.toArray()[0].replace(/\s+/g, "");
                    }
                    strCallResponse = "QRYCUSTNAME_OK";
                } else {
                    strCallResponse = "QRYCUSTNAME_NG";
                }
                break;
            case 'INQ':
                if (bNextPageFlag == true) {
                    GetBalanceInfo();
                    strCallResponse = "INQ_OK";
                } else {
                    strCallResponse = "INQ_NG";
                }
                break;
            case 'QRYACCOUNTSTATE':
                if (bNextPageFlag == true) {
                    strCallResponse = "QRYACCOUNTSTATE_OK";
                } else {
                    strCallResponse = "QRYACCOUNTSTATE_NG";
                }
                break;
            case 'QRYBUSSINESSACCOUNT':
                if (bNextPageFlag == true) {
                    top.API.gTransStatus = top.Sys.PossibleTransactionSync();
                    var objGet1 = top.Dat.GetDataSync(top.API.customernameTag, top.API.customernameType);
                    if (null == objGet1) {
                        top.API.gCustomerName = "";
                    } else {
                        top.API.gCustomerName = objGet1.toArray()[0].replace(/\s+/g, "");
                    }
                    strCallResponse = "QRYBUSSINESSACCOUNT_OK";
                } else {
                    strCallResponse = "QRYBUSSINESSACCOUNT_NG";
                }
                break;
            case 'QRYCWDMONEY':
                if (bNextPageFlag == true) {
                    strCallResponse = "QRYCWDMONEY_OK";
                    top.API.gTransactiontype = "QRYCUSTNAME"; // �黧��
                } else {
                    strCallResponse = "QRYCWDMONEY_NG";
                }
                break;
            case 'QRYACCOUNTTYPE':
                if (bNextPageFlag == true) {
                    strCallResponse = "QRYACCOUNTTYPE_OK";
                } else {
                    strCallResponse = "QRYACCOUNTTYPE_NG";
                }
                break;
            default:
                top.API.displayMessage("�������ʹ���,TransType=" + TransType);
                break;
        }
        if (top.API.gResponsecode == "55") {
            strCallResponse = "ReInputPsw";
        }
        if (top.API.gResponsecode == "04" || top.API.gResponsecode == "33" ||
            top.API.gResponsecode == "41" || top.API.gResponsecode == "43" ||
            top.API.gResponsecode == "07" || top.API.gResponsecode == "34" ||
            top.API.gResponsecode == "35" || top.API.gResponsecode == "36" ||
            top.API.gResponsecode == "37" || top.API.gResponsecode == "67") {
            strCallResponse = "Capture";
        }
        if (bNextPageFlag == false) {
            top.ErrorInfo = top.API.PromptList.No6;
            if (timeoutFlag) {
                strCallResponse = "TimeOut";
            }
            document.getElementById('Loading').style.display = "none";
            document.getElementById('Tipdiv').style.display = "block";
            document.getElementById('tip_label').innerText = strErrMsg;
            bTimeOutStart = true;
            App.Timer.ClearTime();
            App.Timer.SetPageTimeout(8);
            App.Timer.TimeoutDisposal(TimeoutCallBack);
        } else {
            return CallResponse(strCallResponse);
        }

    }

    function GetBalanceInfo() {
        var strBalance;
        var objGet3 = top.Dat.GetDataSync(top.API.currentbalanceTag, top.API.currentbalanceType);
        if (null == objGet3) {
            top.API.displayMessage("GetDataSync CURRENTBALANCE objGet = null");
            top.API.gCURRENTBALANCE = "";
        }
        else {
            if (objGet3.toArray()[0].length > 2) {
                strBalance = objGet3.toArray()[0].substr(0, (objGet3.toArray()[0].length - 2));
                strBalance += ".";
                strBalance += objGet3.toArray()[0].substr((objGet3.toArray()[0].length - 2), 2);
            } else if (objGet3.toArray()[0].length == 2) {
                strBalance = "0." + objGet3.toArray()[0];
            } else if (objGet3.toArray()[0].length == 1) {
                strBalance = "0.0" + objGet3.toArray()[0];
            }
            top.API.gCURRENTBALANCE = strBalance;
        }
        var strAvailableBalance;
        var objGet4 = top.Dat.GetDataSync(top.API.availablebalanceTag, top.API.availablebalanceType);
        if (null == objGet4) {
            top.API.displayMessage("GetDataSync AVAILABLEBALANCE objGet = null");
            top.API.gAVAILABLEBALANCE = "";
        }
        else {
            if (objGet4.toArray()[0].length > 2) {
                strAvailableBalance = objGet4.toArray()[0].substr(0, (objGet4.toArray()[0].length - 2));
                strAvailableBalance += ".";
                strAvailableBalance += objGet4.toArray()[0].substr((objGet4.toArray()[0].length - 2), 2);
            } else if (objGet4.toArray()[0].length == 2) {
                strAvailableBalance = "0." + objGet4.toArray()[0];
            } else if (objGet4.toArray()[0].length == 1) {
                strAvailableBalance = "0.0" + objGet4.toArray()[0];
            }
            top.API.gAVAILABLEBALANCE = strAvailableBalance;
        }
    }

    function AfterSendDisposal() {
        if (Check == "00") {
            var tmpstrErrMsg = "���׳ɹ�";
            top.API.gTakeCardAndPrint = false;
        } else {
            var tmpstrErrMsg = "����ʧ��";
            top.API.gTakeCardAndPrint = true; //�˿�ҳ���ӡƾ��
        }
        var arrTransactionResult = new Array(tmpstrErrMsg);
        top.Dat.SetDataSync("TRANSACTIONRESULT", "STRING", arrTransactionResult);

        switch (TransType) {
            case "DEP":
                //��ȡ�����
                var Amount = top.API.CashInfo.strTransAmount + ".00";
                top.API.gTslMoneyCount = Amount;
                top.API.gTslJnlNum = JnlNum;
                top.API.gTslFlag = true;
                top.API.gTslResponsecode = top.API.gResponsecode;
                //��ȡ��̨���ؽ�����
                objGet3 = top.Dat.GetDataSync("SYSREFNUM", "STRING");
                if (null == objGet3) {
                    top.API.displayMessage("GetDataSync poundage objGet = null");
                    top.API.gTslSysrefnum = "";
                }
                else {
                    top.API.displayMessage("��̨���ؽ����룺 " + objGet3.toArray());
                    top.API.gTslSysrefnum = objGet3.toArray();
                }
                SetJnlCashInBoxData();
                top.Jnl.PrintSync("CashInBox");
                break;
            case "NOCARDDEP":
                //��ȡ�����
                var Amount = top.API.CashInfo.strTransAmount + ".00";
                top.API.gTslMoneyCount = Amount;
                top.API.gTslJnlNum = JnlNum;
                top.API.gTslFlag = true;
                top.API.gTslResponsecode = top.API.gResponsecode;
                //��ȡ��̨���ؽ�����
                objGet3 = top.Dat.GetDataSync("SYSREFNUM", "STRING");
                if (null == objGet3) {
                    top.API.displayMessage("GetDataSync poundage objGet = null");
                    top.API.gTslSysrefnum = "";
                }
                else {
                    top.API.displayMessage("��̨���ؽ����룺 " + objGet3.toArray());
                    top.API.gTslSysrefnum = objGet3.toArray();
                }
                SetJnlCashInBoxData();
                top.Jnl.PrintSync("CashInBox");
                break;
            case "BUSSINESSDEP":
                //��ȡ�����
                var Amount = top.API.CashInfo.strTransAmount + ".00";
                top.API.gTslMoneyCount = Amount;
                top.API.gTslJnlNum = JnlNum;
                top.API.gTslFlag = true;
                top.API.gTslResponsecode = top.API.gResponsecode;
                //��ȡ��̨���ؽ�����
                objGet3 = top.Dat.GetDataSync("SYSREFNUM", "STRING");
                if (null == objGet3) {
                    top.API.displayMessage("GetDataSync poundage objGet = null");
                    top.API.gTslSysrefnum = "";
                }
                else {
                    top.API.displayMessage("��̨���ؽ����룺 " + objGet3.toArray());
                    top.API.gTslSysrefnum = objGet3.toArray();
                }
                SetJnlCashInBoxData();
                top.Jnl.PrintSync("CashInBox");
                break;
            case "CWD":
                //��ȡȡ���ܶ�
                var Amount = top.API.CashInfo.strTransAmount + ".00";
                top.API.gTslMoneyCount = Amount;
                top.API.gTslJnlNum = JnlNum;
                top.API.gTslFlag = true;
                top.API.gTslResponsecode = top.API.gResponsecode;
                //��ȡ��̨���ؽ�����
                objGet3 = top.Dat.GetDataSync("SYSREFNUM", "STRING");
                if (null == objGet3) {
                    top.API.displayMessage("GetDataSync poundage objGet = null");
                    top.API.gTslSysrefnum = "";
                }
                else {
                    top.API.displayMessage("��̨���ؽ����룺 " + objGet3.toArray());
                    top.API.gTslSysrefnum = objGet3.toArray();
                }
                top.Jnl.PrintSync("CashOutBox1");
                break;
            case "INQ":
                top.Jnl.PrintSync("Transaction");
                break;
            case "QRYCUSTNAME":
                top.Jnl.PrintSync("Transaction");
                break;
            case "QRYBUSSINESSACCOUNT":
                top.Jnl.PrintSync("Transaction");
                break;
            case "QRYCWDMONEY":
                top.Jnl.PrintSync("Transaction");
                break;
            case "QRYACCOUNTTYPE":
                top.Jnl.PrintSync("Transaction");
                break;
            case "CWC":
                top.Jnl.PrintSync("Transaction");
                break;
            default:
                top.API.displayMessage("�������ʹ���,TransType=" + TransType);
                break;
        }
    }

    //event handler
    /********************************************************************************************************/
    //��� 
    function onCompositionDataCompleted(arrData) {
        var objArrData = arrData.toArray();
        top.Pin.GenerateMAC(objArrData, "MACKEY", '', 0);
    }

    function onCompositionDataFail() {
        top.API.displayMessage("onCompositionDataFail is done");
        strErrMsg = "ͨѶʧ�ܣ����׽���";
        WriteAcctFileAfterTCP("AT", ""); //add by art for д���׼�¼�ļ�
        bNextPageFlag = false;
        TradeCompleted();
    }

    function onMACGenerated(MacData) {
        top.API.displayMessage("onMACGenerated is done");
        var objMacData = MacData.toArray();
        top.Tcp.SendToHost(objMacData, 60000);
    }

    /********************************************************************************************************/
    //TCPģ��    
    function onTcpSendFailed() {
        top.API.displayMessage("onTcpSendFailed is done");
        strErrMsg = "ͨѶʧ�ܣ����׽���";
        WriteAcctFileAfterTCP("AT", ""); //add by art for д���׼�¼�ļ�
        bNextPageFlag = false;
        TradeCompleted();
    }

    function onTcpOnRecved(tmpCheck) {
        Check = tmpCheck;
        top.API.gResponsecode = tmpCheck;
        top.API.displayMessage("onTcpOnRecved is done,CheckCode:" + Check);
        var bSuccess = false;
        var ErrMsg = null;
        var nRet;
        switch (Check) {
            case '00':
                bNextPageFlag = true;
                if (TransType == "INQ") {
                    var objGet3 = top.Dat.GetDataSync("RESPONSECARDTYPE", "STRING");
                    if (null == objGet3) {
                        top.API.displayMessage("GetDataSync RESPONSECARDTYPE objGet = null");
                    } else {
                        var tmpResponse = objGet3.toArray()[0].substr(0, 2);
                        if (tmpResponse == "01") {
                            bSuccess = true;
                            strErrMsg = "���׳ɹ�";
                            top.API.gATMORTCR = "TCR";
                            var arrATMORTCR = new Array("TCR");
                            var nRet = top.Dat.SetDataSync("ATMORTCR", "STRING", arrATMORTCR);

                            top.API.SaveBack = true; // �����������п����������ش� 
                            
                        } else if (tmpResponse == "02" || tmpResponse == "08") {
                            bSuccess = true;
                            strErrMsg = "���׳ɹ�";
                            top.API.gATMORTCR = "ATM";
                            var arrATMORTCR = new Array("ATM");
                            var nRet = top.Dat.SetDataSync("ATMORTCR", "STRING", arrATMORTCR);
                        } else {
                            bNextPageFlag = false;
                            strErrMsg = "�����п��ݲ�֧�ֽ���";
                        }
                    }
                } else if (TransType == "QRYCWDMONEY") {
                    var objGet3 = top.Dat.GetDataSync("QRYCWDMONEY", "STRING");
                    if (null == objGet3) {
                        top.API.displayMessage("GetDataSync QRYCWDMONEY objGet = null");
                    }
                    else {
                        if( top.API.CWDType == "passbook" ){
                            var tmpResponse = objGet3.toArray()[0];
                            var noPassbookCWDMoney = 200000; //�����ۼƽ��д��
                            if (parseInt(tmpResponse) < noPassbookCWDMoney) {
                                top.API.noPassbookCWDMoney = parseInt(tmpResponse);
                                bSuccess = true;
                                strErrMsg = "���׳ɹ�";
                            } else {
                                bNextPageFlag = false;
                                strErrMsg = "���˿ͻ��ۼ����۴���޶�Ϊ20���������Ѵ����Ѵ�20������������ѯ��";
                            }
                        }else{
                            var tmpResponse = objGet3.toArray()[0];
                            var CWDMoney = 200000;//�����ۼƽ��д��
                            if (parseInt(tmpResponse) < CWDMoney || top.API.gbOrderCWD == true) {
                                top.API.gnCWDMoney = parseInt(tmpResponse);
                                bSuccess = true;
                                strErrMsg = "���׳ɹ�";
                            } else {
                                bNextPageFlag = false;
                                strErrMsg = "���˿ͻ��ۼ�ȡ���޶�Ϊ20����������ȡ����Ѵ�20����������ԤԼȡ�";
                            }
                        }
                        
                    }
                } else if (TransType == "QRYBUSSINESSACCOUNT") {
                    var objGet1 = top.Dat.GetDataSync(top.API.customernameTag, top.API.customernameType);
                    var arrGet1 = objGet1.toArray();
                    top.API.gCustomerName = arrGet1[0].toString().replace(/\s+/g, "");
                    if (top.API.gCustomerName != "") {
                        bSuccess = true;
                        strErrMsg = "���׳ɹ�";
                    } else {
                        bSuccess = false;
                        strErrMsg = "���˺��ݲ��ܰ������ҵ��";
                    }
                } else if (TransType == "QRYACCOUNTTYPE") {
                    var objGet1 = top.Dat.GetDataSync("QRYACCOUNTTYPE", "STRING");
                    var arrGet1 = objGet1.toArray();
                    var strQryAccountType = arrGet1[0].toString().replace(/\s+/g, "");
                    if (strQryAccountType == "00") {
                        bSuccess = true;
                        strErrMsg = "���׳ɹ�";
                    } else if (strQryAccountType == "01") {
                        bSuccess = false;
                        strErrMsg = "���˺��ݲ��ܰ������ҵ��";
                    } else if (strQryAccountType == "02") {
                        bSuccess = false;
                        strErrMsg = "�ö����˺��ݲ��ܰ������ҵ��";
                    } else {
                        bSuccess = false;
                        strErrMsg = "���˺��ݲ��ܰ������ҵ��";
                    }
                } else {
                    bSuccess = true;
                    strErrMsg = "���׳ɹ�";
                }
                break;
            case '01':
                strErrMsg = "�����п��ݲ�֧�ֽ���";//"��ѯ��������";
                break;
            case '04':
                strErrMsg = "���п��쳣,��������";//"û�տ�";
                break;
            case '12':
                strErrMsg = "�ݲ��ܰ������ҵ��";//"��Ч����";
                break;
            case '13':
                strErrMsg = "��Ч�Ľ��׽��";//"��Ч���";
                break;
            case '14':
                strErrMsg = "�����п��ݲ�֧�ֽ���";//"��Ч����";
                break;
            case '15':
                strErrMsg = "�����п��ݲ�֧�ֽ���";//"�޴˷�����";
                break;
            case '33':
                strErrMsg = "���ѹ���,��������";//"���ڵĿ�";
                break;
            case '38':
                strErrMsg = "������󳬴���";//"���������PIN�����루û�տ���";
                break;
            case '41':
                strErrMsg = "�˻��Ѷ���,��������";//"��ʧ����û�տ���";
                break;
            case '43':
                strErrMsg = "�Ƿ��˻�,��������";//"���Կ���û�տ���";
                break;
            case '51':
                strErrMsg = "����";//"�ʽ���";
                break;
            case '54':
                strErrMsg = "���ѹ���,��������";//"���ڵĿ�";
                break;
            case '55':
                strErrMsg = "����������벻��ȷ";
                break;
            case '61':
                strErrMsg = "ȡ�����";//"�ʽ���";
                break;
            case '65':
                strErrMsg = "�ѳ����տ�ȡ�����";//"����ȡ�ִ�������";
                break;
            case '75':
                strErrMsg = "������󳬴���";//"���������PIN��������";
                break;
            case '82':
                strErrMsg = "����δ�����˻����뵽��̨����";//"��δ�����˻����ܾ�����";
                break;
            case '83':
                strErrMsg = "�û���Ȩ�޲����������";//"�û���Ȩ�޲����������";
                break;
            case '84':
                strErrMsg = "�����Ա�������";//"�����Ա�������";
                break;
            case '85':
                strErrMsg = "���۳���ȡ����";//"���۳���ȡ����"��������ȡ;
                break;
            case '86':
                strErrMsg = "�û��ֽ����޴˱���";//"�û��ֽ����޴˱���";
                break;
            case '87':
                strErrMsg = "ARQCУ��ʧ��";//"��ƬУ�鲻����ͬ�Ż�����������";
                break;
            case '88':
                strErrMsg = "��ǰ���״���Ϊ�����ۣ�����ϵ����!";//"�������˻�";
                break;
            case '89':
                strErrMsg = "��Ч�ն�";//"��Ч�ն�";
                break;
            case '90':
            case '91':
            case '96':
            case '30':
            default:
                strErrMsg = "����ʧ��,����ϵ����!";//"�����쳣";
                break;
        }
        ;
        if (!bSuccess) {
            bNextPageFlag = false;
        }
        WriteAcctFileAfterTCP("", "");	//add by art for д���׼�¼�ļ�
        TradeCompleted();
    }

    function onTcpTimeout() {
        top.API.displayMessage("onTcpTimeout is done,TransType=" + TransType);
        if (!timeoutFlag) {
            timeoutFlag = true;
            WriteAcctFileAfterTCP("AT", ""); //add by art for д���׼�¼�ļ�
            strErrMsg = "ͨѶ��ʱ�����׽���";
            if (TransType == "CWD") {
                var arrCWCFlag = new Array();
                arrCWCFlag[0] = 1;
                top.Dat.SetPersistentData(top.API.cwcflagTag, top.API.cwcflagType, arrCWCFlag);
            } else {
                bNextPageFlag = false;
                TradeCompleted();
            }
        }
    }

    function onAnalysisFailed() {
        top.API.displayMessage("onAnalysisFailed is done");
        WriteAcctFileAfterTCP("AT", "");
        strErrMsg = "���Ľ���ʧ�ܣ����׽���";
        if (TransType == "CWD") {
            var arrCWCFlag = new Array();
            arrCWCFlag[0] = 1;
            top.Dat.SetPersistentData(top.API.cwcflagTag, top.API.cwcflagType, arrCWCFlag);
        } else {
            bNextPageFlag = false;
            TradeCompleted();
        }
    }

    //event handler
    function onDeviceError() {
        top.API.displayMessage('���̴����¼���onDeviceError()');
        strErrMsg = "ϵͳ����";
        bNextPageFlag = false;
        TradeCompleted();
    }

    //event handler
    function onCryptFailed() {
        top.API.displayMessage('���̼ӽ���ʧ�ܣ�onCryptFailed');
        strErrMsg = "ϵͳ����";
        bNextPageFlag = false;
        TradeCompleted();
    }

    /********************************************************************************************************/
    //��������ģ��
    function onDatGetPersistentDataComplete(DataName, DataType, DataValue) {
        var arrDataValue = DataValue.toArray();
        if ('JNLNUM' == DataName) {
            if (TransType == "DEP" || TransType == "NOCARDDEP" || TransType == "BUSSINESSDEP") {
                JnlNum = arrDataValue[0];
                BeforeSendDisposal();
                top.Tcp.CompositionData(TransType);
            } else {
                JnlNum = arrDataValue[0] + 1;
                if (JnlNum.toString().length === 7) {
                    JnlNum = 0;
                }
                //���ý�����ˮ��
                var arrJnlNum = new Array();
                arrJnlNum[0] = JnlNum;
                var nRet1 = top.Dat.SetPersistentData(top.JnlnumTag, top.JnlnumType, arrJnlNum);
            }
        }
    }

    function onDatGetPersistentDataError(DataName, ErrorCode) {
        top.API.displayMessage("DatGetPersistentDataError" + DataName + ",ErrorCode=" + ErrorCode);
        bNextPageFlag = false;
        TradeCompleted();
    }

    function onDatSetPersistentDataComplete(DataName) {
        if ('JNLNUM' == DataName) {
            if (TransType == "CWD") {
                //���ó�����ˮ��
                var arrCWCJNLNUM = new Array();
                arrCWCJNLNUM[0] = JnlNum;
                //ȡ��׳ɹ�֮����Ҫ���������ˮ��
                BeforeSendDisposal();
                top.Dat.SetPersistentData(top.API.cwcjnlnumTag, top.API.cwcjnlnumType, arrCWCJNLNUM);
                top.Dat.SetDataSync(top.API.cwcjnlnumTag, top.API.cwcjnlnumType, arrCWCJNLNUM);
            } else {
                BeforeSendDisposal();
                top.Tcp.CompositionData(TransType);
            }
        }
        //�޸Ļ�ȡ�������ݳɹ���־λ
        if ('CWCJNLNUM' == DataName) {
            top.Tcp.CompositionData(TransType);
        }
        if ('CWCFLAG' == DataName) {
            if (TransType == "CWD") {
                //���ó���ԭ��
                var arrCWCREASON = new Array();
                arrCWCREASON[0] = 1;
                nRet = top.Dat.SetPersistentData(top.API.cwcreasonTag, top.API.cwcreasonType, arrCWCREASON);
                top.Dat.SetDataSync(top.API.cwcreasonTag, top.API.cwcreasonType, arrCWCREASON);
            }
        }
        if ('CWCREASON' == DataName) {
            bNextPageFlag = false;
            TradeCompleted();
        }
    }

    function onDatSetPersistentDataError(DataName, ErrorCode) {
        top.API.displayMessage("onDatSetPersistentDataError is done,DataName=" + DataName + ",ErrorCode=" + ErrorCode);
        bNextPageFlag = false;
        TradeCompleted();
    }

    /********************************************************************************************************/
    //Register the event
    function EventLogin() {
        top.Tcp.SendFailed.connect(onTcpSendFailed);
        top.Tcp.OnRecved.connect(onTcpOnRecved);
        top.Tcp.CompositionDataCompleted.connect(onCompositionDataCompleted);
        top.Tcp.CompositionDataFail.connect(onCompositionDataFail);
        top.Dat.GetPersistentDataComplete.connect(onDatGetPersistentDataComplete);
        top.Dat.GetPersistentDataError.connect(onDatGetPersistentDataError);
        top.Dat.SetPersistentDataComplete.connect(onDatSetPersistentDataComplete);
        top.Dat.SetPersistentDataError.connect(onDatSetPersistentDataError);
        top.Pin.MACGenerated.connect(onMACGenerated);
        top.Tcp.Timeout.connect(onTcpTimeout);
        top.Tcp.AnalysisFailed.connect(onAnalysisFailed);
        top.Pin.CryptFailed.connect(onCryptFailed);

        top.Pin.DeviceError.connect(onDeviceError);
    }

    function EventLogout() {
        top.Tcp.SendFailed.disconnect(onTcpSendFailed);
        top.Tcp.OnRecved.disconnect(onTcpOnRecved);
        top.Tcp.CompositionDataCompleted.disconnect(onCompositionDataCompleted);
        top.Tcp.CompositionDataFail.disconnect(onCompositionDataFail);
        top.Dat.GetPersistentDataComplete.disconnect(onDatGetPersistentDataComplete);
        top.Dat.GetPersistentDataError.disconnect(onDatGetPersistentDataError);
        top.Dat.SetPersistentDataComplete.disconnect(onDatSetPersistentDataComplete);
        top.Dat.SetPersistentDataError.disconnect(onDatSetPersistentDataError)
        top.Pin.MACGenerated.disconnect(onMACGenerated);
        top.Tcp.Timeout.disconnect(onTcpTimeout);
        top.Tcp.AnalysisFailed.disconnect(onAnalysisFailed);
        top.Pin.CryptFailed.disconnect(onCryptFailed);
        top.Pin.DeviceError.disconnect(onDeviceError);

    }

    //Countdown function
    function TimeoutCallBack() {
        if (bTimeOutStart) {
            return CallResponse(strCallResponse);
        }
        if (!timeoutFlag) {
            timeoutFlag = true;
            strErrMsg = "ͨѶ��ʱ�����׽���";
            if (TransType == "CWD") {
                var arrCWCFlag = new Array();
                arrCWCFlag[0] = 1;
                var nRet1 = top.Dat.SetPersistentData(top.API.cwcflagTag, top.API.cwcflagType, arrCWCFlag);
            } else {
                bNextPageFlag = false;
                TradeCompleted();
            }
        }
    }

    //Page Return

    //remove all event handler
    function Clearup() {
        //TO DO:
        EventLogout();
        App.Plugin.Voices.del();
        App.Timer.ClearTime();
    }

    function WriteAcctFileAfterTCP(Reason, InOutAmount) {
        var sRetCode = top.API.gResponsecode;
        var sTransAmount = top.API.CashInfo.strTransAmount;// + ".00"
        var sAccoutNo = top.API.gCardno;
        var sInOutAmount = InOutAmount;
        var sTransType = "";

        switch (top.API.gTransactiontype) {
            case "DEP":
            case "NOCARDDEP":
            case "BUSSINESSDEP":
                sTransType = "DEP";
                break;
            case "CWD":
                sTransType = "CWD";
                break;
            default:
                break;
        }
        if (("" == sTransType) || ("CWD" == sTransType && sRetCode == "55")) {
            return;
        }
        if ("CWD" == sTransType && sRetCode == "00") {
            Reason = "PF";
            top.Tsl.HandleRecordFileSync(top.API.MTSL_WRITETRANSRECORD, sAccoutNo + ", " + sTransType + ", " + sTransAmount + ", " + sInOutAmount + ", " + sRetCode + ", " + Reason);
            return;
        }
        if (top.API.gbPartCashIn && "DEP" == sTransType) {
            Reason += "PARTIN";
        }
        if ("DEP" == sTransType) {
            if( top.API.needPrintReDEPCash == true){
                    top.Tsl.HandleRecordFileSync(top.API.MTSL_WRITETRANSRECORD,top.API.gCardno + ", " + "DEP" + ", " +
                        top.API.CashInfo.strTransAmount + ", " + top.API.CashInfo.strTransAmount +
                        ", " + sRetCode + ", " + "REP");
            }else{
                top.Tsl.UpdateRecord(top.API.gCardno + ", " + "DEP" + ", " +
                    top.API.CashInfo.strTransAmount + ", " + top.API.CashInfo.strTransAmount +
                    ", " + sRetCode + ", " + Reason);
            }
        } else {
            top.Tsl.HandleRecordFileSync(top.API.MTSL_WRITETRANSRECORD, sAccoutNo + ", " + sTransType + ", " + sTransAmount + ", " + sInOutAmount + ", " + sRetCode + ", " + Reason);
        }
    }

    function SetTransKind(TransKindType) {
        var arrTransKindType = new Array(TransKindType);
        top.Dat.SetDataSync(top.API.TransKindTag, top.API.TransKindType, arrTransKindType);

    };
    function SetJnlCashInBoxData() {
        var CurrentInfo5 = top.API.CashInfo.arrAcceptorCount[5]
        if (typeof CurrentInfo5 == 'undefined') {
            CurrentInfo5 = 0;
        }
        var CurrentInfo6 = top.API.CashInfo.arrAcceptorCount[6]
        if (typeof CurrentInfo6 == 'undefined') {
            CurrentInfo6 = 0;
        }
        var strJNLData = ", RMB100=" + top.API.CashInfo.arrCurrencyCashIn[0] + ", RMB50=" + top.API.CashInfo.arrCurrencyCashIn[1]
            + ", RMB20=" + top.API.CashInfo.arrCurrencyCashIn[2] + ", RMB10=" + top.API.CashInfo.arrCurrencyCashIn[3]
            + ", RMB5=" + top.API.CashInfo.arrCurrencyCashIn[4] + ", RMB1=" + top.API.CashInfo.arrCurrencyCashIn[5]
            + ", iBOX1=" + top.API.CashInfo.arrAcceptorCount[0] + ", iBOX2=" + top.API.CashInfo.arrAcceptorCount[1]
            + ", iBOX3=" + top.API.CashInfo.arrAcceptorCount[2] + ", iBOX4=" + top.API.CashInfo.arrAcceptorCount[3]
            + ", iBOX5=" + top.API.CashInfo.arrAcceptorCount[4] + ", iBOX6=" + CurrentInfo5
            + ", iBOX7=" + CurrentInfo6;
        top.API.displayMessage("strJNLData=" + strJNLData);
        var arrCashOutBoxData = new Array(strJNLData);
        top.Dat.SetDataSync(top.API.ContentDataTag, top.API.ContentDataType, arrCashOutBoxData);
    };
})();
