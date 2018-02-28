;
(function () {
    var Data = top.Dat.GetDataSync("QRYEXCHANGERESULT", "STRING").toArray(), // ��̨��������
    // var Data = ["103600.00             M0031.85000 2017121720180317","264300.00             M0062.85000 2017031720180317","53600.00             Y0031.89000 2017121720180317"], // ģ�ⷵ������
    // var Data = ["103600.00             M0031.85000 2017121720180317264300.00             M0062.85000 201703172018031753600.00             Y0031.89000 2017121720180317"], // ģ�ⷵ������
        // arrData = Data.split("A"),
        accountType = "", // �˻����� (���ڴ����˻��������˻����������˻�)
        gCardno = "", // ����
        startDay = "", // ��������
        currency = "", // ����
        saveTime = "", // ����
        serialNo = "", // �����к�

        CallResponse = App.Cntl.ProcessOnce(function (Response) {
            //TO DO:
            Clearup();
            //Entry the flows control process.
            App.Cntl.ProcessDriven(Response);
        }),
        Initialize = function () {
            ButtonDisable();
            EventLogin();
            // getData(Data);

            if( Data[0] == "02" ){
                $("#AgentErrTip").html("δ��ѯ���������˺ţ�������������ѯ������Ա��");
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
        // var chooseValue = $("input[type='radio']:checked").val();
        top.API.PCAAccount = $("input[type='radio']:checked").attr("serialNo");
        
        top.API.PCABalance = $("input[type='radio']:checked").attr("PCABalance"); // ��ת�����
        top.API.RemittanceFlag = $("input[type='radio']:checked").attr("RemittanceFlag"); // ��ת������

        // top.API.gTransactiontype = 'SAVINGSTOCURRENTACCOUNT';
        top.Dat.SetDataSync("SUBACCOUNT", "STRING", [top.API.PCAAccount]);

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
        var arr = data.split("");

        // serialNo = data.substr(0,4).split("");
        // serialNo = serialNo.splice(0,1,"0").join("");

        serialNo = data.substr(1,3);
        serialNo = "0"+serialNo;


        top.API.displayMessage("serialNo=" + serialNo);
        accountType = "�����˻�";
        gCardno = top.API.gCardno;
        startDay = data.substr(35,8);
        currency = "�����";

        saveTime = data.substr(23,4);
        switch (saveTime) {
            case "M003":
                saveTime = "������";
                break;
            case "M006":
                saveTime = "������";
                break;
            case "Y001":
                saveTime = "һ��";
                break;
            case "Y003":
                saveTime = "����";
                break;
            default:
                saveTime = "����";
                break;
        }
        
        PCABalance = data.substr(4,19); // ��ת�����
        RemittanceFlag = data.substr(27,8); // ��ת������
    }

    function addToTable() {
        //top.API.displayMessage("Data=" + Data);

        var table = $('#accountTable'),
            arrData = [],
            strLen = 51, // ÿһ�����˻����ݳ���
            dataStrLen = Data[0].length, // ��̨�������˻��ܳ���
            arrDataLen = Data[0].length/strLen; // ���˻�����

        // �����˻���Ϣ�ָ�������
        for( var i=0, len=arrDataLen; i<arrDataLen; i++ ){
            arrData[i] = Data[0].substr(i*strLen, strLen);
        }

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
            input.attr('serialNo', serialNo);
            input.attr('PCABalance', PCABalance);
            input.attr('RemittanceFlag', RemittanceFlag);

            var label = $("<label></label>");
            label.attr('for', idAndFor);
            div.append(input);
            input.after(label);
            td1.append(div);

            var td2 = $("<td></td>");
            td2.text(accountType);

            var td3 = $("<td></td>");
            td3.text(gCardno);

            var td4 = $("<td></td>");
            td4.text(startDay);

            var td5 = $("<td></td>");
            td5.text(currency);

            var td6 = $("<td></td>");
            td6.text("��");

            var td7 = $("<td></td>");
            td7.text(saveTime);

            var td8 = $("<td></td>");
            td8.text(PCABalance);

            // var td9 = $("<td></td>");
            // td9.text(PCABalance);


            tr.append(td1);
            td1.after(td2);
            td2.after(td3);
            td3.after(td4);
            td4.after(td5);
            td5.after(td6);
            td6.after(td7);
            td7.after(td8);
            //td7.after(td9);

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