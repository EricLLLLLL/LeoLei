/*@create by: Gni
 *@time: 2017��12��13��
 */ 
;(function () {
    var CallResponse = App.Cntl.ProcessOnce(function (Response) {
        Clearup();
        App.Cntl.ProcessDriven(Response);
    });
    var Initialize = function () {
        App.Timer.TimeoutDisposal(TimeoutCallBack);
        getData();
        SaveTheCloseRecord();
    }(); //Page Entry
    function ButtonDisable() {
        document.getElementById('Exit').disabled = true;
    }

    function ButtonEnable() {
        document.getElementById('Exit').disabled = false;
    }


    document.getElementById("Exit").onclick = function () {
        ButtonDisable();
        top.ErrorInfo = top.API.PromptList.No2;
        return CallResponse("Exit");
    }

    // ��ά������
    function getData(){
        // var qrcodeUrl = "https://wx.abcchina.com/sz/wxbank/accd/qmall/qrType/1003/deviceNo/13018963/serialNo/ATM171116000000001/timestamp/20171116090123/md5/HGXC526HUVX";
        var qrcodeUrl = top.Dat.GetDataSync("WXACCDELETERESULT", "STRING").toArray()[0];
        var qrcodeArr = $.trim(qrcodeUrl).split("/");
        console.log("��ȡ�����ݣ�  "+JSON.stringify(qrcodeArr));  // ["https:","","wx.abcchina.com","sz","wxbank","accd","qmall","qrType","1003","deviceNo","13018963","serialNo","ATM171116000000001","timestamp","20171116090123","md5","HGXC526HUVX"]

        // ��ȡ��Ӧֵ
        var qrType = qrcodeArr[ $.inArray("qrType",qrcodeArr) + 1 ],
            deviceNo = qrcodeArr[ $.inArray("deviceNo",qrcodeArr) + 1 ],
            serialNo = qrcodeArr[ $.inArray("serialNo",qrcodeArr) + 1 ],
            timestamp = qrcodeArr[ $.inArray("timestamp",qrcodeArr) + 1 ];
        // top.API.displayMessage("�ָ�����ֵ��  "+"qrType:"+qrType+",deviceNo:"+deviceNo+",serialNo:"+serialNo+",timestamp:"+timestamp);

        // set data
        top.Dat.SetDataSync("qrType", "STRING", [qrType]);
        top.Dat.SetDataSync("deviceNo", "STRING", [deviceNo]);
        top.Dat.SetDataSync("serialNo", "STRING", [serialNo]);
        top.Dat.SetDataSync("timestamp", "STRING", [timestamp]);

        // ��ȡ����md5ֵ�Ƿ�ɹ�
        var nRet = top.Dat.GetMD5Data(); // 0 �ɹ�
        top.API.displayMessage("��ȡMD5ֵ�ɹ�����־:"+nRet);

        if( nRet == 0 ){

            qrcodeArr[ $.inArray("md5",qrcodeArr) + 1 ] = top.Dat.GetDataSync("MD5VALUE", "STRING").toArray()[0]; // ����MD5ֵ

            // console.log( $.inArray("md5",qrcodeArr) );
            // top.API.displayMessage("��ȡ�µ�MD5ֵ:"+ qrcodeArr[ $.inArray("md5",qrcodeArr) + 1 ] );
            // console.log(JSON.stringify(qrcodeArr));

            qrcodeUrl = qrcodeArr.join("/");
            // console.log("���ɵĶ�ά���ַ��"+qrcodeUrl);

            var qrcode = new QRCode(document.getElementById("qrcode"), {
                width : 260,
                height : 260
            });

            qrcode.makeCode( qrcodeUrl ); // ���ɶ�ά��

            var qrcodeImgUrl = document.getElementById("qrcode").childNodes[0].toDataURL("image/png");  // ��ȡ��ά��ͼƬ����
            var imgData = qrcodeImgUrl.split("base64,")[1];
            top.Dat.SetDataSync("PICBASE64", "STRING", [imgData]); // �������ɵĶ�ά��ͼƬsrc�������ڴ棬��ƾ��ʹ��
            top.Dat.GetQRCodePic(); // �м���ӿڣ����ļ������ɶ�ά��ͼƬ
            top.Dat.SetDataSync("COMMENTS", "STRING", ["C:\\TCR\\jsapp\\Framework\\QRCode.bmp"]);

        }else{
            $(".qrcode-wrap").hide();
            $(".errTip").html("��ά���ȡʧ�ܣ�����ϵ���й�����Ա��");
            top.Dat.SetDataSync("PICBASE64", "STRING", [""]); // �������ɵĶ�ά��ͼƬsrc�������ڴ棬��ƾ��ʹ��
            top.Dat.SetDataSync("COMMENTS", "STRING", [""]);
        }
        // ��ӡƾ��
        top.Dat.SetDataSync("DEALTYPE", "STRING", ["΢������"]);
        setTimeout(function(){
            top.Ptr.Print("Receipt_SalesForWX_szABC", "", top.API.gPrintTimeOut);
        },5000); // ����ͼƬ�ļ���Ҫ��ʱ
        
    }
    function SaveTheCloseRecord(){
        // ��������¼���浽����
        var sAccoutNo = top.Dat.GetDataSync("CARDNO", "STRING").toArray()[0]; // ����
        var sTransNo = "��"; // ת���˻�
        var sTransAmount = "��"; // ���
        var sRetCode = top.API.gResponsecode; // ��̨����ֵ
        
        top.API.displayMessage("����������Ϣ�������ļ�");
        top.Tsl.HandleRecordFileSync(top.API.MTSL_WRITECLOSEDCORD, sAccoutNo + ", " + sTransNo + ", " + sTransAmount + ", " + sRetCode + ", " + ["΢������"]);
    }


    //Countdown function
    function TimeoutCallBack() {
        top.ErrorInfo = top.API.PromptList.No3;
        return CallResponse("TimeOut");
    }

    //Page Return

    //remove all event handler
    function Clearup() {
        App.Timer.ClearTime();
    }

})();