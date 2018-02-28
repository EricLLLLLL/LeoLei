/*@create by:  tsxiong
 *@time: 2016��03��20��
 */
;(function () {

    // var box = document.getElementsByName("TransRadio"),
    //     arr = null,
    //     BankCardCwdFlag,//���п�ȡ��
    //     PassbookCwdFlag,//����ȡ��
    //     BankCardDepFlag,//���п����
    //     NoCardDepFlag,//�޿����۴��
    //     ZeroExchangeFlag,//�㳮�һ�
    //     CancelAccountFlag,//��������
    //     BlockTradeFlag,//����
    //     CompanyDepFlag,//�Թ����
    //     OrderCWDFlag,//�Թ����

    //     TransferRemittanceFlag,//ת�˻��
    //     TransferCancellationFlag,//ת�˳���
    //     PCAFlag,//����ת����
    //     CTRFlag,//����ת����
    //     SaveBackFlag;//�����ش�

    var $setBtnWrap = $(".set-btn-wrap"),
        TransactionArr = ["Transaction4","Transaction5","Transaction6","Transaction9","Transaction10","Transaction11","Transaction12","TransactionConfig","Transaction13","Transaction14","Transaction15","Transaction16","Transaction17","Transaction18"], // ��������Ҫ��ť���Ƶ���
        Transaction = "Transaction4", // ���ö�ȡ��һ������
        TransactionUrl = "C:\\TCR\\Middle\\ini\\MTransaction.ini"; // �����ļ�·��

	var CallResponse = App.Cntl.ProcessOnce( function(Response){
        App.Cntl.ProcessDriven(Response);
    });
    var Initialize = function () {
        ButtonDisable();
        bulidList();
        btnClick();
        ButtonEnable();
    }();//Page Entry
    function ButtonDisable() {
        document.getElementById('Exit').disabled = true;
        document.getElementById('OK').disabled = true;
    }

    function ButtonEnable() {
        document.getElementById('Exit').disabled = false;
        document.getElementById('OK').disabled = false;
    }

    function bulidList(){

        var $ul = $("<ul></ul>"),
            $li,
            btnFlag = 0, // 0��֧�� 1֧��
            btnName = ""; // ��������

        $.each( TransactionArr,function(i,data){
            Transaction = data;

            if( Transaction == "TransactionConfig" ){
                btnName = "�Թ����";
                btnFlag = top.Dat.GetPrivateProfileSync(Transaction, "bOrderCWD", "0", top.API.gIniFileName);
            }else{
                btnName = top.Dat.GetPrivateProfileSync(Transaction, "TransName", "0", TransactionUrl);
                btnFlag = top.Dat.GetPrivateProfileSync(Transaction, "TransactionSupport", "0", TransactionUrl);
            }
            //console.log("��ȡ������btnName:"+btnName+";btnFlag:"+btnFlag);

            $li = $(['<li>',
                    '<span>' + btnName + '��</span>',
                    '<input class="TranstypeRadio" type="button" flag="' + btnFlag + '"/>',
                    '</li>'
                    ].join(""));
            $ul.append($li);
        });
        $setBtnWrap.append($ul);

        //��ȡ�����־
        // BankCardCwdFlag = top.Dat.GetPrivateProfileSync("Transaction4", "TransactionSupport", "0", "C:\\TCR\\Middle\\ini\\MTransaction.ini");
        // PassbookCwdFlag = top.Dat.GetPrivateProfileSync("Transaction5", "TransactionSupport", "0", "C:\\TCR\\Middle\\ini\\MTransaction.ini");
        // BankCardDepFlag = top.Dat.GetPrivateProfileSync("Transaction6", "TransactionSupport", "0", "C:\\TCR\\Middle\\ini\\MTransaction.ini");
        // NoCardDepFlag = top.Dat.GetPrivateProfileSync("Transaction9", "TransactionSupport", "0", "C:\\TCR\\Middle\\ini\\MTransaction.ini");
        // ZeroExchangeFlag = top.Dat.GetPrivateProfileSync("Transaction10", "TransactionSupport", "0", "C:\\TCR\\Middle\\ini\\MTransaction.ini");
        // BlockTradeFlag = top.Dat.GetPrivateProfileSync("Transaction11", "TransactionSupport", "0", "C:\\TCR\\Middle\\ini\\MTransaction.ini");
        // CompanyDepFlag = top.Dat.GetPrivateProfileSync("Transaction12", "TransactionSupport", "0", "C:\\TCR\\Middle\\ini\\MTransaction.ini");
        // OrderCWDFlag = top.Dat.GetPrivateProfileSync("TransactionConfig", "bOrderCWD", "0", top.API.gIniFileName);
        // CancelAccountFlag = top.Dat.GetPrivateProfileSync("Transaction13", "TransactionSupport", "0", "C:\\TCR\\Middle\\ini\\MTransaction.ini");

        // TransferRemittanceFlag = top.Dat.GetPrivateProfileSync("Transaction14", "TransactionSupport", "0", "C:\\TCR\\Middle\\ini\\MTransaction.ini");
        // TransferCancellationFlag = top.Dat.GetPrivateProfileSync("Transaction15", "TransactionSupport", "0", "C:\\TCR\\Middle\\ini\\MTransaction.ini");
        // PCAFlag = top.Dat.GetPrivateProfileSync("Transaction16", "TransactionSupport", "0", "C:\\TCR\\Middle\\ini\\MTransaction.ini");
        // CTRFlag = top.Dat.GetPrivateProfileSync("Transaction17", "TransactionSupport", "0", "C:\\TCR\\Middle\\ini\\MTransaction.ini");
        // SaveBackFlag = top.Dat.GetPrivateProfileSync("Transaction18", "TransactionSupport", "0", "C:\\TCR\\Middle\\ini\\MTransaction.ini");
        // SaveBackFlag = top.Dat.GetPrivateProfileSync("TransactionConfig", "CWCSupport", "0", "C:\\TCR\\jsapp\\ini\\TCR_CUSTOMIZE.INI");

        // arr = [
        //     {"flag": BankCardCwdFlag},
        //     {"flag": PassbookCwdFlag},
        //     {"flag": BankCardDepFlag},
        //     {"flag": NoCardDepFlag},
        //     {"flag": ZeroExchangeFlag},
        //     {"flag": BlockTradeFlag},
        //     {"flag": CompanyDepFlag},
        //     {"flag": OrderCWDFlag},
        //     {"flag": CancelAccountFlag},
        //     {"flag": TransferRemittanceFlag},
        //     {"flag": TransferCancellationFlag},
        //     {"flag": PCAFlag},
        //     {"flag": CTRFlag},
        //     {"flag": SaveBackFlag}
        // ];
        // for (i = 0; i < box.length; i++) {
        //     if (arr[i].flag == "1") {
        //         box[i].setAttribute("style", "background-image: url('Framework/style/Graphics/btn/btn_on.png')");
        //         box[i].setAttribute("flag", "1");
        //     } else if (arr[i].flag == "0") {
        //         box[i].setAttribute("style", "background-image: url('Framework/style/Graphics/btn/btn_off.png')");
        //         box[i].setAttribute("flag", "0");
        //     }
        // }
        // radioClick(box);
    }

    function btnClick(){

        var btnClickFlag = "0"; // ��ť��ǰ״̬
        $setBtnWrap.on("click",".TranstypeRadio",function(){

            btnClickFlag = $(this).attr("flag") == 0 ? 1 : 0; // 0���1,1���0
            $(this).attr("flag", btnClickFlag );

        });
    }

    function setData(){

        var dataFlag = ""; // ��ťԪ�ص�flag����ֵ������ʱ���������ý������ļ���

        $.each( TransactionArr,function(i,data){
            dataFlag = $(".TranstypeRadio").eq(i).attr("flag");
            if( data == "TransactionConfig" ){
                top.Dat.WritePrivateProfileSync(data, "bOrderCWD", dataFlag, top.API.gIniFileName);
            }else{
                top.Dat.WritePrivateProfileSync(data, "TransactionSupport", dataFlag, TransactionUrl);
            }
            //console.log("��������data:"+data+";dataFlag:"+dataFlag);
        });

    }

    // function radioClick() {
    //     var i = box.length;
    //     var j = 0;
    //     for (j = 0; j < i; j++) {
    //         (function () {
    //             var p = j;
    //             box[p].onclick = function () {
    //                 change(p);
    //             }
    //         })();
    //     }

    //     function change(j) {

    //         top.API.displayMessage("���id=" + box[j].getAttribute("id"));
    //         top.API.displayMessage("���ǰCancelAccountFlag=" + box[j].getAttribute("flag"));
    //         if (box[j].getAttribute("flag") == 0) {
    //             box[j].setAttribute("style", "background-image: url('Framework/style/Graphics/btn/btn_on.png')");
    //             box[j].setAttribute("flag", "1");
    //             arr[j].flag = "1";
    //         } else if (box[j].getAttribute("flag") == 1) {
    //             box[j].setAttribute("style", "background-image: url('Framework/style/Graphics/btn/btn_off.png')");
    //             box[j].setAttribute("flag", "0");
    //             arr[j].flag = "0";
    //         }
    //         top.API.displayMessage("�����CancelAccountFlag=" + box[j].getAttribute("flag"));
    //     }
    // }

    //@User ocde scope start
    document.getElementById('Exit').onclick = function () {
        ButtonDisable();
        return CallResponse('Exit');
    }

    document.getElementById('OK').onclick = function () {
        ButtonDisable();

        setData();
        // top.Dat.WritePrivateProfileSync("Transaction4", "TransactionSupport", arr[0].flag, "C:\\TCR\\Middle\\ini\\MTransaction.ini");
        // top.Dat.WritePrivateProfileSync("Transaction5", "TransactionSupport", arr[1].flag, "C:\\TCR\\Middle\\ini\\MTransaction.ini");
        // top.Dat.WritePrivateProfileSync("Transaction6", "TransactionSupport", arr[2].flag, "C:\\TCR\\Middle\\ini\\MTransaction.ini");
        // top.Dat.WritePrivateProfileSync("Transaction9", "TransactionSupport", arr[3].flag, "C:\\TCR\\Middle\\ini\\MTransaction.ini");
        // top.Dat.WritePrivateProfileSync("Transaction10", "TransactionSupport", arr[4].flag, "C:\\TCR\\Middle\\ini\\MTransaction.ini");
        // top.Dat.WritePrivateProfileSync("Transaction13", "TransactionSupport", arr[8].flag, "C:\\TCR\\Middle\\ini\\MTransaction.ini");
        // top.Dat.WritePrivateProfileSync("Transaction11", "TransactionSupport", arr[5].flag, "C:\\TCR\\Middle\\ini\\MTransaction.ini");
        // top.Dat.WritePrivateProfileSync("Transaction12", "TransactionSupport", arr[6].flag, "C:\\TCR\\Middle\\ini\\MTransaction.ini");
        // top.Dat.WritePrivateProfileSync("TransactionConfig", "bOrderCWD", arr[7].flag, top.API.gIniFileName);
        // top.Dat.WritePrivateProfileSync("Transaction14", "TransactionSupport", arr[9].flag, "C:\\TCR\\Middle\\ini\\MTransaction.ini");
        // top.Dat.WritePrivateProfileSync("Transaction15", "TransactionSupport", arr[10].flag, "C:\\TCR\\Middle\\ini\\MTransaction.ini");
        // top.Dat.WritePrivateProfileSync("Transaction16", "TransactionSupport", arr[11].flag, "C:\\TCR\\Middle\\ini\\MTransaction.ini");
        // top.Dat.WritePrivateProfileSync("Transaction17", "TransactionSupport", arr[12].flag, "C:\\TCR\\Middle\\ini\\MTransaction.ini");
        // top.Dat.WritePrivateProfileSync("Transaction18", "TransactionSupport", arr[13].flag, "C:\\TCR\\Middle\\ini\\MTransaction.ini");
        // top.Dat.WritePrivateProfileSync("TransactionConfig", "CWCSupport", arr[13].flag, "C:\\TCR\\jsapp\\ini\\TCR_CUSTOMIZE.INI");
        return CallResponse('OK');
    }



})();
