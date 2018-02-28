(function () {
    window.JSCashInfo = function () {
        this.arrUnitRemain = new Array(); //���������
        this.arrUnitName = new Array();   //����������
        this.arrUnitCurrency = new Array(); //��������ֵ
        this.arrMixDispense = new Array(); //����ȡ����̳����䳮��¼
        this.arrDispenseCount = new Array(); //����ȡ����̳�������ļ�¼
        this.arrSelfMixDispenset = new Array(); //�����䳮�����������¼
        this.arrDispensetRJCount = new Array(); //������RJ��Ŀ
        this.arrAcceptorCount = new Array(); //���������볮��Ŀ
        this.arrCurrencyCashIn = new Array(6); //����ֵ�볮����100,50,20,10,5,1
        this.strTransAmount = "0";
        this.nCountOfUnits = 0;
        this.bINQ = true;
        this.bCWD = false;
        this.bDEP = false;
        this.bExchange = false;
        this.bCASH = false;
        this.bCashFull = true;
        this.bCashEmpty = true;
		this.b100CashEmpty = true;
		this.bOtherCashEmpty = true;///��������Ƿ����Ԥ������
        this.strCardNum = "";
        this.Dealtype = ""; 
        this.InitData = function () {
            var i = 0;
            for (i = 0; i < this.nCountOfUnits; i++) {
                this.arrMixDispense[i] = 0;
                this.arrDispenseCount[i] = 0;
                this.arrSelfMixDispenset[i] = 0;
                this.arrDispensetRJCount[i] = 0;
                this.arrAcceptorCount[i] = 0;
            }
            for (i = 0; i < this.arrCurrencyCashIn.length; i++) {
                this.arrCurrencyCashIn[i] = "0";
            }
            this.strTransAmount = "0";
            this.strCardNum = "";
            this.Dealtype = "";
        };
        this.SpliceArr = function () {
            this.arrUnitRemain.splice(0, this.arrUnitRemain.length);
            this.arrUnitCurrency.splice(0, this.arrUnitCurrency.length);
            this.arrMixDispense.splice(0, this.arrMixDispense.length);
            this.arrDispenseCount.splice(0, this.arrDispenseCount.length);
            this.arrSelfMixDispenset.splice(0, this.arrSelfMixDispenset.length);
            this.arrDispensetRJCount.splice(0, this.arrDispensetRJCount.length);
            this.arrAcceptorCount.splice(0, this.arrAcceptorCount.length);
            this.arrCurrencyCashIn.splice(0, this.arrCurrencyCashIn.length);
        };
    };
})();

