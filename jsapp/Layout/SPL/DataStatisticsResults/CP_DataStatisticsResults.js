;(function () {
    var DSArray = new Array();
    var datas = null;//tr����

    var Initialize = function () {
        ButtonDisable();
        DSArray = top.API.gDSResultData;
        //���tr����
        datas = document.getElementsByName("data");
        FillingData();
        ButtonEnable();
    }();//Page Entry

    //@User ocde scope start
    document.getElementById('Exit').onclick = function () {
        ButtonDisable();
        return CallResponse('Exit');
    };
    function ButtonDisable() {
        document.getElementById('Exit').disabled = true;
    }

    function ButtonEnable() {
        document.getElementById('Exit').disabled = false;
    }

    //@User code scope end
    function FillingData() {
        //������
        var LineData = new Array();

        //���ʱ��
        document.getElementById("timeScope").innerText = top.API.gDSTimeScope;

        //����ͳ�����ݿ�ʼ
        var CwdCounts = 0; 
        var CwdMoney = 0;
        var CwdFailCounts = 0;

        var DepCounts = 0;
        var DepMoney = 0;
        var DepFailCounts = 0;
        //���ͳ�Ƴ������������
        for (var i = 0 ; i<datas.length; i++){
            LineData = DSArray[i].split(":");
            for(var j = 0 ; j<3 ; j++){
                datas[i].getElementsByTagName("td")[j+1].innerText = LineData[j];
            }
            if(i>=3){
                DepCounts += parseInt(LineData[0]);
                DepMoney += parseInt(LineData[1]);
                DepFailCounts += parseInt(LineData[2]);

            }else{
                CwdCounts += parseInt(LineData[0]);
                CwdMoney += parseInt(LineData[1]);
                CwdFailCounts += parseInt(LineData[2]);
            }
        }


        var Counts = CwdCounts+DepCounts;
        var Money = CwdMoney+DepMoney;
        var FailCounts = CwdFailCounts+DepFailCounts;
        //����ͳ�����ݽ���

        //������������
        var cwd = document.getElementById("cwd").getElementsByTagName("td");
        var dep = document.getElementById("dep").getElementsByTagName("td");
        var total = document.getElementById("total").getElementsByTagName("td");

        cwd[1].innerText = CwdCounts;
        cwd[2].innerText = CwdMoney;
        cwd[3].innerText = CwdFailCounts;

        dep[1].innerText = DepCounts;
        dep[2].innerText = DepMoney;
        dep[3].innerText = DepFailCounts;

        total[1].innerText = Counts;
        total[2].innerText = Money;
        total[3].innerText = FailCounts;
    }

    function CallResponse(Response) {
        App.Cntl.ProcessDriven(Response);
    }
})();
