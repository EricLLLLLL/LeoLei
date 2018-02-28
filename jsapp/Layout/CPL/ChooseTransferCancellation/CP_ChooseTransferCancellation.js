;
(function () {
    // var data = top.API.TrasferCancelList,
    var Data = top.Dat.GetDataSync("QRYTFSCANCELRESULT", "STRING").toArray(), // ��̨��������

        TransferTime  = "", // ������ڣ�ԭ�������ڣ�
        OperatingTime = "", // ����ʱ�䣨ԭ����ʱ�䣩
        TransfereAccount = "", // ת���˺�
        TransferName  = "", // ת�뻧��
        TransferMoney = "", // ת�˽��
        SystemFlag    = "", // ϵͳ��ʶ
        originallogno = "", // ԭʼ��־��
        rqstserialnum = "", // ������ˮ��
        paynum = "",// ֧���������

        CallResponse = App.Cntl.ProcessOnce(function (Response) {
            //TO DO:
            Clearup();
            //Entry the flows control process.
            App.Cntl.ProcessDriven(Response);
        }),
        Initialize = function () {
            ButtonDisable();
            EventLogin();

            if( Data == null || Data == "" ){
                $("#AgentErrTip").html("δ��ѯ���ɳ������ף�������������ѯ������Ա��");
            }else{
                addToTable();
            }
            
            App.Timer.TimeoutDisposal(TimeoutCallBack);
            ButtonEnable();
            $("#OK").hide();
        }(); //Page Entry

    //@User ocde scope start
    document.getElementById('Exit').onclick = function () {
        ButtonDisable();
        top.ErrorInfo = top.API.PromptList.No2;
        return CallResponse('Exit');
    };

    document.getElementById('OK').onclick = function () {
        ButtonDisable();

        // ���������ڴ���
        var $radioChecked = $("input[type='radio']:checked"),
            TRANSFERTIME  = $radioChecked.parent().parent().parent("tr").find("td").eq(1).html(), // ������ڣ�ԭ�������ڣ�
            OPERATINGTIME = $radioChecked.parent().parent().parent("tr").find("td").eq(2).html(), // ����ʱ�䣨ԭ����ʱ�䣩
            TRANSFEREACCOUNT = $radioChecked.parent().parent().parent("tr").find("td").eq(3).html(), // ת���˺�
            TRANSFERNAME  = $radioChecked.parent().parent().parent("tr").find("td").eq(4).html(), // ת�뻧��
            TRANSFERMONEY = (parseFloat( $radioChecked.parent().parent().parent("tr").find("td").eq(5).html() ) *100).toString(); // ת�˽��
            RQSTSERIALNUM = $radioChecked.parent().parent().parent("tr").find("td").eq(5).attr("rqstserialnum"), // ������ˮ��
            ORIGINALLOGNO = $radioChecked.parent().parent().parent("tr").find("td").eq(5).attr("originallogno"), // ԭʼ��־��
            PAYNUM = $radioChecked.parent().parent().parent("tr").find("td").eq(5).attr("paynum"), // ֧���������
            SYSTEMFLAG = $radioChecked.parent().parent().parent("tr").find("td").eq(6).html(); // ϵͳ��ʶ

        top.Dat.SetDataSync("TRANSFERTIME", "STRING", [TRANSFERTIME]);
        top.Dat.SetDataSync("OPERATINGTIME", "STRING", [OPERATINGTIME]);
        top.Dat.SetDataSync("TRANSFEREACCOUNT", "STRING", [TRANSFEREACCOUNT]);
        top.Dat.SetDataSync("TFRCARDNO", "STRING", [TRANSFEREACCOUNT]);
        top.Dat.SetDataSync("TRANSFERNAME", "STRING", [TRANSFERNAME]);
        top.Dat.SetDataSync("TRANSAMOUNT", "STRING", [TRANSFERMONEY]);
        top.Dat.SetDataSync("RQSTSERIALNUM", "STRING", [RQSTSERIALNUM]);
        top.Dat.SetDataSync("ORIGINALLOGNO", "STRING", [ORIGINALLOGNO]);
        top.Dat.SetDataSync("PAYNUM", "STRING", [PAYNUM]);
        top.Dat.SetDataSync("SYSTEMFLAG", "STRING", [SYSTEMFLAG]);

        top.API.gTransactiontype = 'TRANSFERCANCEL';

        return CallResponse('OK');
    };

    function ButtonDisable() {
        document.getElementById('Exit').disabled = true;
        document.getElementById('OK').disabled = true;
    }

    function ButtonEnable() {
        document.getElementById('Exit').disabled = false;
        document.getElementById('OK').disabled = false;
    }
    $("input[type='radio']").on("click", function(){
        $("#OK").show();
    })


    //@User code scope end
    // ��������
    function getData(data){
        var arr = data.split("A");

        // TransferTime = data.substr(0,8); // ������ڣ�ԭ�������ڣ�
        // OperatingTime = data.substr(8,6); // ����ʱ�䣨ԭ����ʱ�䣩
        // TransfereAccount = data.substr(14,20); // ת���˺�
        // TransferName = data.substr(34,17); // ת�뻧��
        // TransferMoney = data.substr(51,19); // ת�˽��
        // SystemFlag = data.substr(70,1); // ϵͳ��ʶ
        // originallogno = data.substr(71,9); // ԭʼ��־��
        // rqstserialnum = data.substr(80,22); // ������ˮ��
        // paynum = data.substr(102,8); // ֧���������

        TransferTime  = arr[0]; // ������ڣ�ԭ�������ڣ�
        OperatingTime = arr[1]; // ����ʱ�䣨ԭ����ʱ�䣩
        TransfereAccount = arr[2]; // ת���˺�
        TransferName  = arr[3]; // ת�뻧��
        TransferMoney = arr[4]; // ת�˽��
        SystemFlag    = arr[5]; // ϵͳ��ʶ
        originallogno = arr[6]; // ԭʼ��־��
        rqstserialnum = arr[7]; // ������ˮ��
        paynum = arr[8]; // ֧���������

    }
    function addToTable() {

        var table = $('#accountTable'),
            arrData = Data[0].split("B");
            // strLen = 110, // ÿһ�����˻����ݳ���
            // dataStrLen = Data[0].length, // ��̨�������˻��ܳ���
            // arrDataLen = Data[0].length/strLen; // ���˻�����

        // �����˻���Ϣ�ָ�������
        // for( var i=0, len=arrDataLen; i<arrDataLen; i++ ){
        //     arrData[i] = Data[0].substr(i*strLen, strLen);
        // }

        var table = $('#accountTable');
        for (var o in arrData) {
            getData(arrData[o]);
            var tr = $("<tr></tr>");
            var td1 = $("<td></td>");
            var div = $("<div></div>");
            div.attr('class', 'radio-wrap');

            var input = $("<input />");
            input.attr('type', 'radio');
            var idAndFor = 'radio' + o;
            input.attr('id', idAndFor);
            input.attr('name', 'radio')
            input.attr('value', o);

            var label = $("<label></label>");
            label.attr('for', idAndFor);
            div.append(input);
            input.after(label);
            td1.append(div);

            var td2 = $("<td></td>");
            td2.text(TransferTime);

            var td3 = $("<td></td>");
            td3.text(OperatingTime);

            var td4 = $("<td></td>");
            td4.text(TransfereAccount);

            var td5 = $("<td></td>");
            td5.text(TransferName);

            var td6 = $("<td paynum='"+paynum+"' originallogno='"+originallogno+"' rqstserialnum='"+rqstserialnum+"'></td>");
            td6.text(TransferMoney);

            var td7 = $("<td></td>");
            td7.text(SystemFlag);


            tr.append(td1);
            // tr.append(td2);
            td1.after(td2);
            td2.after(td3);
            td3.after(td4);
            td4.after(td5);
            td5.after(td6);
            td6.after(td7);

            table.append(tr);
        }
    }

    //Register the event
    function EventLogin() {

    }

    function EventLogout() {

    }

    //Countdown function
    function TimeoutCallBack() {
        top.ErrorInfo = top.API.PromptList.No3;
        return CallResponse('TimeOut');
    }

    //remove all event handler
    function Clearup() {
        //TO DO:
        EventLogout();
        App.Timer.ClearTime();
    }
})();