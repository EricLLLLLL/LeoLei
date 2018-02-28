/**
 * @anther: LeoLei
 * @Time: 20170915
 */
;(function () {
    var TransType = null,
        strErrMsg = "",
        strCallResponse = 'Error',
        JnlNum = null,//定义获取交易流水号变量
        bNextPageFlag = false,
        bTimeOutStart = false,
        timeoutFlag = false,
        Check = "",//判断是否提示错误信息
        CallResponse = App.Cntl.ProcessOnce(function (Response) {
            //TO DO:
            AfterSendDisposal();
            Clearup();
            //Entry the flows control process.
            App.Cntl.ProcessDriven(Response);
        }),
        Initialize = function () {
            EventLogin();
            App.Plugin.Voices.play("voi_28");
            App.Timer.TimeoutDisposal(TimeoutCallBack);
            top.ErrorInfo = top.API.PromptList.No1;
            SetCashUnitInfoToTCP();
            //进交易之前设置，交易类型
            TransType = top.API.gTransactiontype;
           // NoTcpForData(TransType);
            var arrTRANSACTIONTYPE = new Array(TransType);
            top.Dat.SetDataSync(top.API.transactiontypeTag, top.API.transactiontypeType, arrTRANSACTIONTYPE);
            top.Dat.GetPersistentData(top.JnlnumTag, top.JnlnumType);
        }();//Page Entry


    function NoTcpForData(transType) {
        var ResponseCode;
        switch
            (transType) {
            case
            'QueryTransInfo'
            :
                top.API.gResponsecode = '00';
                top.API.PayeeName = '和小美';
                top.API.PayeeBank = '农业银行';
                ResponseCode = 'QueryTransInfo_OK';
                break;
            case
            'Transfer'
            :
                top.API.gResponsecode = '00';
                top.API.TransferPoundage = '5.00'
                ResponseCode = 'TransferComplete_OK';
                break;
            case
            'QueryCancelInfo'
            :
                top.API.TrasferCancelList = [{
                    TransferTime: '20170801',
                    PayeeName: '研发部',
                    PayeeAccount: '3465756756354345',
                    Currency: '人民币',
                    TransferMoney: '343455'
                }, {
                    TransferTime: '20170801',
                    PayeeName: '财务部',
                    PayeeAccount: '345642346456',
                    Currency: '人民币',
                    TransferMoney: '4454'
                }, {
                    TransferTime: '20170801',
                    PayeeName: '人力资源部',
                    PayeeAccount: '3453456756756',
                    Currency: '人民币',
                    TransferMoney: '8000000'
                }, {
                    TransferTime: '20170801',
                    PayeeName: '产品部',
                    PayeeAccount: '23423423234421',
                    Currency: '人民币',
                    TransferMoney: '7890000'
                }, {
                    TransferTime: '20170801',
                    PayeeName: '测试组',
                    PayeeAccount: '23423423234421',
                    Currency: '人民币',
                    TransferMoney: '7890000'
                }, {
                    TransferTime: '20170801',
                    PayeeName: '应用开发',
                    PayeeAccount: '23423423234421',
                    Currency: '人民币',
                    TransferMoney: '7890000'
                }, {
                    TransferTime: '20170801',
                    PayeeName: '平台开发',
                    PayeeAccount: '23423423234421',
                    Currency: '人民币',
                    TransferMoney: '7890000'
                }, {
                    TransferTime: '20170801',
                    PayeeName: '售后',
                    PayeeAccount: '23423423234421',
                    Currency: '人民币',
                    TransferMoney: '7890000'
                }];
                ResponseCode = 'QueryCancelInfo_OK';
                break;
            case
            'TransferCancel'
            :
                top.API.gResponsecode = '00';
                // top.API.TransferPoundage = '5.00'
                ResponseCode = 'TransferCancel_OK';
                break;
            case
            'INQC'
            :
                top.API.gResponsecode = '00';
                top.API.CancellationAccountList = ['5444322354566533','7754334233214776','689993323372','32223111233321111'];
                //top.API.PayeeBank = '农业银行';
                ResponseCode = 'QRYACCOUNTCANCEL_OK';
                break;
            case
            'ACCDELETE'
            :
                top.API.gResponsecode = '00';
               // top.API.TrasferCancelList = [5444322354566533,7754334233214776,689993323372,32223111233321111];
                //top.API.PayeeBank = '农业银行';
                ResponseCode = 'ACCOUNTCANCELLTION_OK';
                break;
        }
        return CallResponse(ResponseCode);
    }


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
        var arrTransactionResult = new Array("通讯失败");
        //top.API.gTakeCardAndPrint = true;
        top.Dat.SetDataSync("TRANSACTIONRESULT", "STRING", arrTransactionResult);
        switch (TransType) {
            case "INQC":
                SetTransKind("销户查询");
                break;
            case "WXACCDELETE":
                SetTransKind("微信销户");
                break;
            case "ACCDELETE":
                SetTransKind("销户");
                break;
            case "PeriodicCurrentAccount":
                SetTransKind("定期转活期");
                break;
            case "QRYEXCHANGE":
                SetTransKind("查询定活互转账户信息");
                break;
            case "CURRENTACCOUNTTOSAVINGS":
                SetTransKind("活转定");
                break;
            case "SAVINGSTOCURRENTACCOUNT":
                SetTransKind("定转活");
                break;
            case "QRYINCUSTNAME":
                SetTransKind("行内转账查询户名");
                break;
            case "QRYFEE":
                SetTransKind("行外转账查手续费");
                break;
            case "QRYTFSCANCEL":
                SetTransKind("转账撤销查询");
                break;
            case "TRANSFERCANCEL":
                SetTransKind("转账撤销操作");
                break;
            case "TRANSFERACTS":
                SetTransKind("转账汇款");
                break;
            default:
                top.API.displayMessage("交易类型错误,TransType=" + TransType);
                break;
        }
        top.Jnl.PrintSync("BeforeSendDisposal");
    }
    
    function AfterSendDisposal() {
        if (Check == "00") {
            var tmpstrErrMsg = "交易成功";
            top.API.gTakeCardAndPrint = false;
        } else {
            var tmpstrErrMsg = "交易失败";
            top.API.gTakeCardAndPrint = true; //退卡页面打印凭条
        }
        var arrTransactionResult = new Array(tmpstrErrMsg);
        top.Dat.SetDataSync("TRANSACTIONRESULT", "STRING", arrTransactionResult);
        
        // switch (TransType){
        //     case "INQC":
        //         //top.Jnl.PrintSync("Transaction");
        //         break;
        //     // case "ACCDELETE":
        //     //     top.Jnl.PrintSync("SalesComplete");
        //     //     break;
        //     // case "PeriodicCurrentAccount":
        //     //     top.Jnl.PrintSync("RTCComplete");
        //     //     break;
        //     // case "CURRENTACCOUNTTOSAVINGS":
        //     //     top.Jnl.PrintSync("CTRComplete");
        //     //     break;
        //     // case "SAVINGSTOCURRENTACCOUNT":
        //     //     top.Jnl.PrintSync("RTCComplete");
        //     //     break;
        //     default:
        //         top.API.displayMessage("交易类型错误,TransType=" + TransType);
        //         break;
        // }
    }

    function SetTransKind(TransKindType) {
        var arrTransKindType = new Array(TransKindType);
        top.Dat.SetDataSync(top.API.TransKindTag, top.API.TransKindType, arrTransKindType);
    }

    function onDatGetPersistentDataComplete(DataName, DataType, DataValue) {
        if ('JNLNUM' == DataName) {
            var arrDataValue = DataValue.toArray();
            JnlNum = arrDataValue[0] + 1;
            if (JnlNum.toString().length === 7) {
                JnlNum = 0;
            }
            //设置交易流水号
            var arrJnlNum = new Array();
            arrJnlNum[0] = JnlNum;
            var nRet1 = top.Dat.SetPersistentData(top.JnlnumTag, top.JnlnumType, arrJnlNum);
        }
    }

    function onDatGetPersistentDataError(DataName, ErrorCode) {
        top.API.displayMessage("DatGetPersistentDataError" + DataName + ",ErrorCode=" + ErrorCode);
        bNextPageFlag = false;
        TradeCompleted();
    }

    function onDatSetPersistentDataComplete(DataName) {
        if ('JNLNUM' == DataName) {
            BeforeSendDisposal();
            top.Tcp.CompositionData(TransType);
        }
    }

    function onDatSetPersistentDataError(DataName, ErrorCode) {
        top.API.displayMessage("onDatSetPersistentDataError is done,DataName=" + DataName + ",ErrorCode=" + ErrorCode);
        bNextPageFlag = false;
        TradeCompleted();
    }


    function onCompositionDataCompleted(arrData) {
        var objArrData = arrData.toArray();
        top.Pin.GenerateMAC(objArrData, "MACKEY", '', 0);
    }


    // function onCompositionDataFail() {
        function onCompositionDataFail() {
            top.API.displayMessage("onCompositionDataFail is done");
            strErrMsg = "通讯失败，交易结束";
            WriteAcctFileAfterTCP("AT", "");
            bNextPageFlag = false;
            TradeCompleted();
        }
    // }


    function onMACGenerated(MacData) {
        top.API.displayMessage("onMACGenerated is done");
        var objMacData = MacData.toArray();
        top.Tcp.SendToHost(objMacData, 60000);
    }

    //event handler
    function onDeviceError() {
        top.API.displayMessage('键盘触发事件：onDeviceError()');
        strErrMsg = "系统故障";
        bNextPageFlag = false;
        TradeCompleted();
    }

    //event handler
    function onCryptFailed() {
        top.API.displayMessage('键盘加解密失败：onCryptFailed');
        strErrMsg = "系统故障";
        bNextPageFlag = false;
        TradeCompleted();
    }


    function onTcpSendFailed() {
        top.API.displayMessage("onTcpSendFailed is done");
        strErrMsg = "通讯失败，交易结束";
        WriteAcctFileAfterTCP("AT", ""); //add by art for 写交易记录文件
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
        // top.API.displayMessage("tmpResponse=" + TransType);
        switch (Check) {
            case '00':
                bNextPageFlag = true;
                //例子
                if (TransType == "INQC") {
                    var objGet3 = top.Dat.GetDataSync("ACCOUNTLIST", "STRING");
                    if (null == objGet3 || objGet3.toArray()[0].indexOf("A") == -1) {
                        top.API.displayMessage("GetDataSync ACCOUNTLIST objGet = null");
                        strErrMsg = "未查询出可销户账号！如有疑问请咨询工作人员！"
                    } else {
                        var ACCOUNTLIST = objGet3.toArray()[0].split('A');
                        top.API.CancellationAccountList = ACCOUNTLIST;
                        bSuccess = true;
                        strErrMsg = "交易成功";
                    }
                // } else if (TransType == "ACCDELETE") {
                //     var objGet3 = top.Dat.GetDataSync("ACCDELETERES", "STRING");
                //     top.API.displayMessage("objGet3=" + objGet3);
                //     if (null == objGet3) {
                //         top.API.displayMessage("GetDataSync ACCDELETERES objGet = null");
                //     } else {
                //         var tmpResponse = objGet3.toArray()[0].substr(0, 2);
                //         top.API.displayMessage("tmpResponse=" + tmpResponse);
                //         if (tmpResponse == "00") {
                //             bSuccess = true;
                //             strErrMsg = "交易成功";
                //         } else if (tmpResponse == "02" || tmpResponse == "08") {
                //             bSuccess = true;
                //             strErrMsg = "交易成功";
                //         } else {
                //             bNextPageFlag = false;
                //             strErrMsg = "该银行卡暂不支持交易";
                //         }
                //     }
                } else {
                    bSuccess = true;
                    strErrMsg = "交易成功";
                }
                break;
            case '01':
                strErrMsg = "该银行卡暂不支持交易";//"查询发卡银行";
                break;
            case '04':
                strErrMsg = "银行卡异常,即将回收";//"没收卡";
                break;
            case '12':
                strErrMsg = "暂不能办理此项业务";//"无效交易";
                break;
            case '13':
                strErrMsg = "交易金额不符合业务系统限制";//"无效金额，大于100";
                break;
            case '14':
                strErrMsg = "此账户存在异常，请到柜台办理业务";//"无效卡号";
                break;
            case '15':
                strErrMsg = "该银行卡暂不支持交易";//"无此发卡方";
                break;
            case '33':
                strErrMsg = "卡已过期,即将回收";//"过期的卡";
                break;
            case '38':
                strErrMsg = "密码错误超次数";//"超过允许的PIN试输入（没收卡）";
                break;
            case '40':
                strErrMsg = "系统不支持该业务，请到柜台办理业务";//销户时返回40一般是卡折合一，需要到柜台办理;
                break;
            case '41':
                strErrMsg = "账户已冻结,即将回收";//"挂失卡（没收卡）";
                break;
            case '43':
                strErrMsg = "非法账户,即将回收";//"被窃卡（没收卡）";
                break;
            case '51':
                strErrMsg = "余额不足";//"资金不足";
                break;
            case '54':
                strErrMsg = "卡已过期,即将回收";//"过期的卡";
                break;
            case '55':
                strErrMsg = "您输入的密码不正确";
                break;
            case '61':
                strErrMsg = "取款金额超限";//"资金不足";
                break;
            case '65':
                strErrMsg = "已超当日可取款次数";//"超出取现次数限制";
                break;
            case '75':
                strErrMsg = "密码错误超次数";//"允许的输入PIN次数超限";
                break;
            case '81':
			    strErrMsg = "响应超时，请联系工作人员核实账务"; // 转账出现异常提示
				break;
            case '82':
                strErrMsg = "存在未销子账户，请到柜台办理";//"有未销子账户，拒绝交易";
                break;
            case '83':
                strErrMsg = "用户无权限操作解决方案";//"用户无权限操作解决方案";
                break;
            case '84':
                strErrMsg = "组件柜员分配错误";//"组件柜员分配错误";
                break;
            case '85':
                strErrMsg = "存折超出取款额度";//"存折超出取款额度"，不可再取;
                break;
            case '86':
                strErrMsg = "用户现金箱无此币种";//"用户现金箱无此币种";
                break;
            case '87':
                strErrMsg = "ARQC校验失败";//"相片校验不过（同号换卡多此类错误）";
                break;
            case '88':
                strErrMsg = "当前交易存折为对账折，请联系银行!";//"对账折账户";
                break;
            case '89':
                strErrMsg = "无效终端";//"无效终端";
                break;
            case '90':
            case '91':
            case '96':
            case '30':
            default:
                strErrMsg = "交易失败,请联系银行!";//"交易异常";
                break;
        }
        ;
        if (!bSuccess) {
            bNextPageFlag = false;
        }
        WriteAcctFileAfterTCP("", "");    //add by art for 写交易记录文件
        TradeCompleted();
    }

    function onTcpTimeout() {
        top.API.displayMessage("onTcpTimeout is done,TransType=" + TransType);
        if (!timeoutFlag) {
            timeoutFlag = true;
            WriteAcctFileAfterTCP("AT", ""); //add by art for 写交易记录文件
            strErrMsg = "通讯超时，交易结束";
            bNextPageFlag = false;
            TradeCompleted();
            // setTimeout(function(){
            //     return CallResponse("Exit");
            // },10000);
            // App.Timer.SetPageTimeout(8);
            // return CallResponse("TimeOut");
        }
    }

    function onAnalysisFailed() {
        top.API.displayMessage("onAnalysisFailed is done");
        WriteAcctFileAfterTCP("AT", "");
        strErrMsg = "报文解析失败，交易结束";

        bNextPageFlag = false;
        TradeCompleted();

    }


    /**
     * 写异常记录文件
     * @param Reason 原因
     * @param InOutAmount
     * @constructor
     */
    function WriteAcctFileAfterTCP(Reason, InOutAmount) {
		if(TransType == 'ACCDELETE'){
			var sRetCode = top.API.gResponsecode;
			// add by zwh for 只记录销户失败的记录 because 销户时要记录转账金额，但销户未成功时无法获取
			if(sRetCode != "00"){
				var sAccoutNo = top.Dat.GetDataSync('CARDNO','STRING').toArray()[0];
				var sTransNo = top.Dat.GetDataSync('TFRCARDNO', 'STRING').toArray()[0];
				var sTransAmount = "0.00";
				
				top.API.displayMessage("保存销户失败信息到本地文件");
				top.Tsl.HandleRecordFileSync(top.API.MTSL_WRITECLOSEDCORD, sAccoutNo + ", " + sTransNo + ", " + sTransAmount + ", " + sRetCode + ", " + ["销户失败"]);
			}
		} 
		
		
    }

    /**
     *
     * @describe 交易完成之后设置跳转的标志
     */
    function TradeCompleted() {
        if (bNextPageFlag) {
            var arrTransactionResult = new Array("交易成功");
            //top.API.gTakeCardAndPrint = false;
            top.Dat.SetDataSync("TRANSACTIONRESULT", "STRING", arrTransactionResult);
        }
        switch (TransType) {
            case 'INQC':
                if (bNextPageFlag == true) {
                    strCallResponse = "QRYACCOUNTCANCEL_OK";
                } else {
                    strCallResponse = "QRYACCOUNTCANCEL_NG";
                }
                break;
            case 'WXACCDELETE':
                if (bNextPageFlag == true) {
                    strCallResponse = "WXCancelAccount_OK";
                } else {
                    strCallResponse = "WXCancelAccount_NG";
                }
                break;
            case 'ACCDELETE':
                if (bNextPageFlag == true) {
                    strCallResponse = "ACCOUNTCANCELLTION_OK";
                } else {
                    strCallResponse = "ACCOUNTCANCELLTION_NG";
                }
                break;
            case 'QRYEXCHANGE':
                if (bNextPageFlag == true) {
                    strCallResponse = top.API.CashInfo.Dealtype == "CurrentToRegular" ? "QueryCurrentAccount_OK" : "QueryRegularAccount_OK";
                } else {
                    strCallResponse = top.API.CashInfo.Dealtype == "CurrentToRegular" ? "QueryCurrentAccount_NG" : "QueryRegularAccount_NG";
                }
                break;
            case 'QRYEXECUTERATE':
                if (bNextPageFlag == true) {
                    strCallResponse = "QueryInterestRate_OK";
                } else {
                    strCallResponse = "QueryInterestRate_NG";
                }
                break;
            case 'CURRENTACCOUNTTOSAVINGS':
                if (bNextPageFlag == true) {
                    strCallResponse = "CurrentToRegular_OK";
                } else {
                    strCallResponse = "CurrentToRegular_NG";
                }
                break;
            case 'SAVINGSTOCURRENTACCOUNT':
                if (bNextPageFlag == true) {
                    strCallResponse = "RegularToCurrent_OK";
                } else {
                    strCallResponse = "RegularToCurrent_NG";
                }
                break;
            case 'QRYINCUSTNAME':
                if (bNextPageFlag == true) {
                    strCallResponse = "QRYCUSTNAME_OK";
                } else {
                    strCallResponse = "QRYCUSTNAME_NG";
                }
                break;
            case 'QRYFEE':
                if (bNextPageFlag == true) {
                    strCallResponse = "QueryCharge_OK";
                } else {
                    strCallResponse = "QueryCharge_NG";
                }
                break;
            case 'TRANSFERACTS':
                if (bNextPageFlag == true) {
                    strCallResponse = top.API.MONEYLIMIT == "0" ? "TransferComplete_OK" : "SmallComplete_OK";
                } else {
                    if( top.API.CashInfo.Dealtype == "OutLineFlag" ){
                        if( top.API.MONEYLIMIT == "0" ){
                            top.API.MONEYLIMIT = "1"; // 大额转小额
                            top.Dat.SetDataSync("MONEYLIMIT", "STRING", [top.API.MONEYLIMIT]); // 设置大小额标识符 0大额 1小额
                            top.API.TRANSFERTIMES = "2"; // 大额转小额，第二次发送报文
                            top.Dat.SetDataSync("TRANSFERTIMES", "STRING", [top.API.TRANSFERTIMES]); // 设置第一次转账标识 1第一次 2第二次
                            bNextPageFlag = true;
                            strCallResponse = "TransferComplete_NG";
                        }else{
                            strCallResponse = "SmallComplete_NG";
                        }
                    }else{
                        strCallResponse = "SmallComplete_NG";
                    }      
                }
                break;
            case 'QRYTFSCANCEL':
                if (bNextPageFlag == true) {
                    strCallResponse = "QueryCancelInfo_OK";
                } else {
                    strCallResponse = "QueryCancelInfo_NG";
                }
                break;
            case 'TRANSFERCANCEL':
                if (bNextPageFlag == true) {
                    strCallResponse = "TransferCancel_OK";
                } else {
                    strCallResponse = "TransferCancel_NG";
                }
                break;
            default:
                top.API.displayMessage("交易类型错误,TransType=" + TransType);
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

        // top.API.displayMessage("bNextPageFlag: "+bNextPageFlag+"---"+"strCallResponse: "+strCallResponse);
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

    //Register the event
    function EventLogin() {
        top.Dat.GetPersistentDataComplete.connect(onDatGetPersistentDataComplete);
        top.Dat.GetPersistentDataError.connect(onDatGetPersistentDataError);
        top.Dat.SetPersistentDataComplete.connect(onDatSetPersistentDataComplete);
        top.Dat.SetPersistentDataError.connect(onDatSetPersistentDataError);

        top.Tcp.CompositionDataCompleted.connect(onCompositionDataCompleted);
        top.Tcp.CompositionDataFail.connect(onCompositionDataFail);
        top.Tcp.SendFailed.connect(onTcpSendFailed);
        top.Tcp.OnRecved.connect(onTcpOnRecved);
        top.Tcp.Timeout.connect(onTcpTimeout);
        top.Tcp.AnalysisFailed.connect(onAnalysisFailed);

        top.Pin.MACGenerated.connect(onMACGenerated);
        top.Pin.CryptFailed.connect(onCryptFailed);
        top.Pin.DeviceError.connect(onDeviceError);

    }

    function EventLogout() {
        top.Dat.GetPersistentDataComplete.disconnect(onDatGetPersistentDataComplete);
        top.Dat.GetPersistentDataError.disconnect(onDatGetPersistentDataError);
        top.Dat.SetPersistentDataComplete.disconnect(onDatSetPersistentDataComplete);
        top.Dat.SetPersistentDataError.disconnect(onDatSetPersistentDataError);

        top.Tcp.CompositionDataCompleted.disconnect(onCompositionDataCompleted);
        top.Tcp.CompositionDataFail.disconnect(onCompositionDataFail);
        top.Tcp.SendFailed.disconnect(onTcpSendFailed);
        top.Tcp.OnRecved.disconnect(onTcpOnRecved);
        top.Tcp.Timeout.disconnect(onTcpTimeout);
        top.Tcp.AnalysisFailed.disconnect(onAnalysisFailed);

        top.Pin.MACGenerated.disconnect(onMACGenerated);
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
            strErrMsg = "通讯超时，交易结束";
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

    //remove all event handler
    function Clearup() {
        //TO DO:
        EventLogout();
        App.Plugin.Voices.del();
        App.Timer.ClearTime();
    }
})();
