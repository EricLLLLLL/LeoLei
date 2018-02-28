/*@create by:  tsxiong
 *@time: 2016年03月20日
 */
;(function () {
    var bSPL1 = false;
    var strCardNo = "";
    var timeoutID;
    var CallResponse = App.Cntl.ProcessOnce(function (Response) {
        if (Response == "OffLine") {
          //  top.Crd.CancelAccept();
          //  top.Scr.CancelAccept();
          //  top.Sys.SetIsBusying(false);
        } else {
            //top.Sys.SetIsBusying(true);
        }
       // top.Siu.SetCardReaderLight('OFF');
        window.clearTimeout(timeoutID);
        Clearup();
        App.Cntl.ProcessDriven(Response);
    });
    var Initialize = function () {
        //@initialize scope start
        ButtonDisable();
      //  top.API.gArrUnitRemain = top.Cdm.CUNoteValue().toArray();
       // top.API.gArrUnitStatus = top.Cim.CUStatus().toArray();
       // top.Sys.ItemClearSync(top.API.MTRN_TRANSACTIONDIFFER);
        App.Plugin.Advert.InsertAdv("AdvParentDiv");
       // EventLogin();
        //top.API.CashInfo.arrUnitRemain = top.API.GetUnitInfo(top.Cdm.PUCurrentCount().toArray());
        //top.API.CashInfo.InitData();
        document.getElementById("version").innerText = "版本号: ABC_ShenZhen_TCR_AP_" + "V010000";
       // var arrCwdDealtypeType = new Array("CARD"); //新增数据库初始值
       // top.Dat.SetDataSync(top.API.CWDDealTypeTag, top.API.CWDDealTypeType, arrCwdDealtypeType);
      //  setSupportDepValue();
        ButtonEnable();
        $("#Business7").show();
     //   refreshTitleAdv();
    }(); //Page Entry


    // 设置存款限额到sp中
    function setSupportDepValue(){
        var unitMaxmoney = top.Sys.DataGetSync(top.API.MTRN_REMAINDEPSITAMOUT);
        var arrTransactionResult= new Array(''+unitMaxmoney);
        top.Dat.SetDataSync("CASHINMAXAMOUNT", "STRING", arrTransactionResult);
    }

    //@User ocde scope start
    function ButtonDisable() {
        document.getElementById('Business5').disabled = true;
        document.getElementById('Business6').disabled = true;
        document.getElementById('Business7').disabled = true;
        document.getElementById('Business4').disabled = true;
    }

    function ButtonEnable() {
        document.getElementById('Business5').disabled = false;
        document.getElementById('Business6').disabled = false;
        document.getElementById('Business7').disabled = false;
        document.getElementById('Business4').disabled = false;

    }

    function refreshTitleAdv() {
        var bRet = top.API.CheckDeviceStatus();
        App.Plugin.Wait.disappear();
        if (!bRet) {
            return CallResponse('OffLine');
        } else {
            top.API.displayMessage("卡取款:" + top.API.gbCARDCWD_DEAL +
                ",卡存款:" + top.API.gbCARDDEP_DEAL +
                ",存折取款:" + top.API.gbPBCWD_DEAL +
                ",存折存款:" + top.API.gbPBDEP_DEAL +
                ",零钞兑换:" + top.API.gbEXCHANGE_DEAL +
                ",卡折销户:" + top.API.gbACCDELETE_DEAL +
                ",无卡无折存款:" + top.API.gbNOCARDDEP_DEAL +
                ",对公存款:" + top.API.gbBUSINESSDEP_DEAL +
                ",转账汇款:" + top.API.gbTRANSFER_DEAL +
                ",转账撤销:" + top.API.gbTRANSFERCANCEL_DEAL +
                ",定转活:" + top.API.gbPCA_DEAL +
                ",活转定:" + top.API.gbCTR_DEAL +
                ",卡钞回存:" + top.API.gbSAVEBACK_DEAL);
            var strTip = "";
            if (!top.API.gbCARDCWD_DEAL && !top.API.gbCARDDEP_DEAL && !top.API.gbPBCWD_DEAL
                && !top.API.gbPBDEP_DEAL && !top.API.gbNOCARDDEP_DEAL && !top.API.gbBUSINESSDEP_DEAL) {
                return CallResponse('OffLine');
            } else {
                if (!top.API.gbCARDDEP_DEAL && !top.API.gbPBDEP_DEAL
                    && !top.API.gbNOCARDDEP_DEAL && !top.API.gbBUSINESSDEP_DEAL) {
                    strTip = strTip + "/存款";
                } else {
                    if (!top.API.gbCARDDEP_DEAL) {
                        strTip = strTip + "/卡存款";
                    }
                    if (!top.API.gbPBDEP_DEAL) {
                        strTip = strTip + "/折存款";
                    }
                    if (!top.API.gbNOCARDDEP_DEAL) {
                        strTip = strTip + "/无卡无折存款";
                    }
                    if (!top.API.gbBUSINESSDEP_DEAL) {
                        strTip = strTip + "/对公存款";
                    }
                }
                if (!top.API.gbCARDCWD_DEAL && !top.API.gbPBCWD_DEAL) {
                    strTip = strTip + "/取款";
                } else {
                    if (!top.API.gbCARDCWD_DEAL) {
                        strTip = strTip + "/卡取款";
                    }
                    if (!top.API.gbPBCWD_DEAL) {
                        strTip = strTip + "/存折取款";
                    }
                }
            }
            if (strTip != "") {
                strTip = strTip.substring(1, strTip.length);
                document.getElementById("moveTilte").style.display = "block";
                document.getElementById("moveTilte").innerHTML = "本机暂不提供" + strTip + "服务";
            } else {
                document.getElementById("moveTilte").innerHTML = "";
                document.getElementById("moveTilte").style.display = "none";
            }

            // 对公存款
            if (top.API.gbBUSINESSDEP_DEAL) {
                $("#Business7").show();
            } else {
                $("#Business7").hide();
            }

            // 无卡无折
            if (top.API.gbNOCARDDEP_DEAL) {
                $("#Business5").show();
            } else {
                $("#Business5").hide();
            }
            // 零钞
            if (top.API.gbEXCHANGE_DEAL) {
                $("#Business4").show();
            } else {
                $("#Business4").hide();
            }
            // 卡折销户
            var PtrPaperStatus = top.Ptr.StPaperStatus();
            if (top.API.gbACCDELETE_DEAL && (PtrPaperStatus == "FULL" || PtrPaperStatus == "LOW")) { // 凭条有纸时显示
                $("#Business6").show();
            } else {
                $("#Business6").hide();
            }
        }
        if (top.API.gbCARDCWD_DEAL || top.API.gbCARDDEP_DEAL) {
            top.Crd.AcceptAndChipInitialise('AcceptAndChipInitialise', -1);
            top.Siu.SetCardReaderLight('SLOW');
        }

        if (top.API.gbPBDEP_DEAL || top.API.gbPBCWD_DEAL) {
            top.Scr.AcceptAndReadAvailableTracks('1,2,3', -1);
        }
    }

    function ChangebSPL1() {
        //TO DO:
        bSPL1 = false;
    }

    document.getElementById("SPL1").onclick = function () {
        bSPL1 = true;
        var t = window.setTimeout(ChangebSPL1, 1000);
    }
    document.getElementById("SPL2").onclick = function () {
        if (bSPL1) {
            bSPL1 = false;
            top.Sys.OpenManagePage();
            top.Jnl.PrintSync("AdminOpenSpl");
            return CallResponse('OffLine');
        }
        ;
    }

    // 零钞兑换
    document.getElementById("Business4").onclick = function(){
        ButtonDisable();
        top.Jnl.PrintSync("SelectExchange");
        top.API.CashInfo.Dealtype = "零钞兑换";
        var arrDealType = new Array(top.API.CashInfo.Dealtype);
        var nRet = top.Dat.SetDataSync(top.API.dealtypeTag, top.API.dealtypeType, arrDealType);
        nRet = top.Sys.DataSetSync(top.API.MTRN_TRANSACTIONDIFFER, top.API.MTRN_NOCARDDEPOSIT);
        
        top.Crd.CancelAccept();
        top.Scr.CancelAccept();
        CheckPtrPaper("Exchange");
    }

    // 卡折销户
    document.getElementById("Business6").onclick = function () {
        ButtonDisable();
		top.Jnl.PrintSync("selectSales");
        top.API.CashInfo.Dealtype = "CancelAccount";  // 销户
        var arrDealType = new Array(top.API.CashInfo.Dealtype);
        top.Dat.SetDataSync(top.API.dealtypeTag, top.API.dealtypeType, arrDealType);
        top.API.gTransactiontype = 'INQC';
        top.Crd.CancelAccept();
        top.Scr.CancelAccept();
        CheckPtrPaper("LossOfHouseholds");
    };

    //无卡无折存款
    document.getElementById("Business5").onclick = function () {
        ButtonDisable();
        top.Jnl.PrintSync("SelectNoCardDep");
        top.API.CashInfo.Dealtype = "无卡无折存款";
        var arrDealType = new Array(top.API.CashInfo.Dealtype);
        top.Dat.SetDataSync(top.API.dealtypeTag, top.API.dealtypeType, arrDealType);
        top.Dat.SetDataSync("QRYCWDMONEYFLAG", "STRING", ["1"]);//无卡无折存款为"1";
        top.Sys.DataSetSync(top.API.MTRN_TRANSACTIONDIFFER, top.API.MTRN_NOCARDDEPOSIT);
        top.Crd.CancelAccept();
        top.Scr.CancelAccept();
        CheckPtrPaper("NoCardDep");
    }

    //对公存款
    document.getElementById("Business7").onclick = function () {
        ButtonDisable();
        top.Jnl.PrintSync("SelectBusinessDep");
        top.API.CashInfo.Dealtype = "对公存款";
        var arrDealType = new Array(top.API.CashInfo.Dealtype);
        var nRet = top.Dat.SetDataSync(top.API.dealtypeTag, top.API.dealtypeType, arrDealType);
        nRet = top.Sys.DataSetSync(top.API.MTRN_TRANSACTIONDIFFER, top.API.MTRN_NOCARDDEPOSIT);
        top.Crd.CancelAccept();
        top.Scr.CancelAccept();
        CheckPtrPaper("BusinessDep");
    }

    function CheckPtrPaper(Response) {
        var PtrPaperStatus = top.Ptr.StPaperStatus();
        if (top.Ptr.bDeviceStatus && (PtrPaperStatus == "FULL" || PtrPaperStatus == "LOW")) {
            return CallResponse(Response);
        } else {
            return CallResponse("NoPrint");
        }
    }

    //@User code scope end
    function onStatusChanged(PropertyName, OldValue, NewValue) {
        var arrCurrentStatus = new Array();
        arrCurrentStatus = NewValue.toArray();
        top.API.displayMessage(PropertyName + ":CurrentStatus=" + arrCurrentStatus[0]);
        refreshTitleAdv();
    }

    function onSafeDoorOpened() {
        top.API.displayMessage("SafeDoorOpened");
        refreshTitleAdv();
    }

    function onSafeDoorClosed() {
        top.API.displayMessage("SafeDoorClosed");
        refreshTitleAdv();
    }

    //event handler
    function onCrdDeviceError() {
        top.API.displayMessage("onCrdDeviceError");
        top.Siu.SetCardReaderLight('OFF');
        refreshTitleAdv();
    }

    function onScrDeviceError() {
        top.API.displayMessage("onScrDeviceError");
        refreshTitleAdv();
    }

    function onScrCardAcceptFailed() {
        top.API.displayMessage("onScrCardAcceptFailed");
        refreshTitleAdv();
    }


    function onChipDataReceived(Token, Bytes) {
        top.API.CashInfo.Dealtype = "INQ";
        var arrBalanceInquiryType = new Array("ACCTYPE");
        top.Dat.SetDataSync(top.API.BalanceInquiryTag, top.API.BalanceInquiryType, arrBalanceInquiryType);
        return CallResponse('CardInsert');
    }

    function onCardTaken() {
        top.API.displayMessage("onCardTaken触发");
        refreshTitleAdv();
    }

    function onCardInserted() {
        top.API.displayMessage("CardInserted");
        ButtonDisable();
        top.Scr.CancelAccept();
    }

    //---------------存折事件--------------
    function onScrCardAccepted() {
        ButtonDisable();
        top.Crd.CancelAccept();
        var strTrack2 = top.Scr.Track2();
        var strTrack3 = top.Scr.Track3();
        if (strTrack2 == "" || strTrack3 == "") {
            App.Plugin.Wait.showNoBankCard();
            timeoutID = window.setTimeout(function () {
                App.Plugin.Wait.disappear();
                refreshTitleAdv();
            }, 3000);
        } else {
            var arrTrack2 = new Array();
            arrTrack2 = strTrack2.split("=");
            if (arrTrack2.length == 2) {
                strCardNo = arrTrack2[0];
            } else {
                var num = arrTrack2.length - 1;
                strCardNo = arrTrack2[num];
            }
            SetCardNo();
        }
    }

    function SetCardNo() {
        if (strCardNo.toString().length > 15 || strCardNo.toString().length == 0) {
            App.Plugin.Wait.showNoBankCard();
            timeoutID = window.setTimeout(function () {
                App.Plugin.Wait.disappear();
                refreshTitleAdv();
            }, 3000);
            return;
        } else {
            var arrCardNo = new Array(strCardNo);
            top.API.gCardno = strCardNo;
            top.Dat.SetDataSync(top.API.cardnoTag, top.API.cardnoType, arrCardNo);
            top.Jnl.PrintSync("PassbookAction");
            top.API.CashInfo.Dealtype = "存折业务";
            var arrDealType = new Array(top.API.CashInfo.Dealtype);
            top.Dat.SetDataSync(top.API.dealtypeTag, top.API.dealtypeType, arrDealType);
            CheckPtrPaper("BookBank");
        }
    }

    function onScrCardInvalid() {
        top.API.displayMessage("CardInvalid");
        App.Plugin.Wait.showNoBankCard();
        timeoutID = window.setTimeout(function () {
            App.Plugin.Wait.disappear();
        }, 3000);
    }

    //---------------存折事件--------------

    //Register the event
    function EventLogin() {
        top.Crd.StatusChanged.connect(onStatusChanged);
        top.Pin.StatusChanged.connect(onStatusChanged);
        top.Ptr.StatusChanged.connect(onStatusChanged);
        top.Cim.StatusChanged.connect(onStatusChanged);
        top.Fpi.StatusChanged.connect(onStatusChanged);
        top.Idr.StatusChanged.connect(onStatusChanged);
        top.Scr.StatusChanged.connect(onStatusChanged);
        //CRD
        top.Crd.ChipDataReceived.connect(onChipDataReceived);
        top.Crd.CardTaken.connect(onCardTaken);
        top.Crd.CardInserted.connect(onCardInserted);
        top.Crd.DeviceError.connect(onCrdDeviceError);
        //SCR  
        top.Scr.CardAccepted.connect(onScrCardAccepted);
        top.Scr.CardAcceptFailed.connect(onScrCardAcceptFailed);
        top.Scr.CardInvalid.connect(onScrCardInvalid);
        top.Scr.CardTaken.connect(onCardTaken);
        top.Scr.DeviceError.connect(onScrDeviceError);
        //保险柜门开关事件
        top.Cim.SafeDoorClosed.connect(onSafeDoorClosed);
        top.Cim.SafeDoorOpened.connect(onSafeDoorOpened);
    }

    function EventLogout() {
        top.Crd.StatusChanged.disconnect(onStatusChanged);
        top.Pin.StatusChanged.disconnect(onStatusChanged);
        top.Ptr.StatusChanged.disconnect(onStatusChanged);
        top.Cim.StatusChanged.disconnect(onStatusChanged);
        top.Fpi.StatusChanged.disconnect(onStatusChanged);
        top.Idr.StatusChanged.disconnect(onStatusChanged);
        top.Scr.StatusChanged.disconnect(onStatusChanged);
        //CRD
        top.Crd.ChipDataReceived.disconnect(onChipDataReceived);
        top.Crd.DeviceError.disconnect(onCrdDeviceError);
        top.Crd.CardTaken.disconnect(onCardTaken);
        top.Crd.CardInserted.disconnect(onCardInserted);
        //SCR  
        top.Scr.CardAccepted.disconnect(onScrCardAccepted);
        top.Scr.CardAcceptFailed.disconnect(onScrCardAcceptFailed);
        top.Scr.CardInvalid.disconnect(onScrCardInvalid);
        top.Scr.CardTaken.disconnect(onCardTaken);
        top.Scr.DeviceError.disconnect(onScrDeviceError);
        //保险柜门开关事件
        top.Cim.SafeDoorClosed.disconnect(onSafeDoorClosed);
        top.Cim.SafeDoorOpened.disconnect(onSafeDoorOpened);
    }

    //remove all event handler
    function Clearup() {
        App.Plugin.Advert.removeAdv();
        EventLogout();
        //TO DO:
    }

})();
