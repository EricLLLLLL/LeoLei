(function () {
    window.global = function () {
        this.gTransactiontype = "";
        this.gCardno = ""; //����
        this.gPoundage = "";
        this.gResponsecode = "";
        this.gMasterkeyA = "";
        this.gMasterkeyB = "";
        this.gMasterkeyC = "";
        this.gIdCardpic = ""; //���֤ȫ·��
        this.gCheckIdCardpic = "";
        this.gIdName = ""; //���֤����
        this.gCustomerName = ""; //���п�����
        this.gIdNation = "";
        this.gIdSex = "";
        this.gIdNumber = ""; //���֤����
        this.gChooseMoney = 0;
        this.gOldPinBlock = "";
        this.gNewPinBlock = "";
        this.gbAmountType = 0; //0:Ĭ��ֵ   1:С��   2:���
        this.gDispenseCounts = 0;
        this.gAcceptCounts = 0;
        this.gPrintFSN = false;
        this.gSubBankNum = "";
        this.gDevStatus = "";
        this.AdminStatus = 0; //0Ϊ������Ա 00
        this.gTslChooseType = ""; //�������� �Թ���BUSINESSDEP�� ����ȡ�BOOKCWD�� ����CWD�� С���DEP�� ����NOCARDDEP
        this.gTslJnlNum = ""; //��ˮ��
        this.gTslMoneyCount = ""; //���
        this.gTslResult = "FAIL"; //���׽�� Ĭ��Ϊʧ�ܡ�
        this.gTslDate = "";
        this.gTslTime = "";
        this.gTslFlag = false;
        this.gTslChooseJnlType = ""; //������ˮ�������ͣ�0107�����0108����ȡ��
        this.gTslSysrefnum = ""; //��̨������ˮ��
        this.gTslJnlBtn = ""; //�豸��ˮ���κ�
        this.gTslFailType = ""; //�쳣״̬���ͣ�4λ��
        this.gTslResponsecode = "";
        this.gstrIdCardInfo = ""; //�����Ϣ�����ַ�����Ϣ
        this.gIniFileName = "c:\\TCR\\jsapp\\ini\\TCR_CUSTOMIZE.INI"; //JS�����ļ�·��
        this.CreateBusiListHead = "";
        this.CreateBusiListUrl = "";
        this.CreateBusiListParam = "";
        this.CreateTaskHead = "";
        this.CreateTaskUrl = "";
        this.CreateTaskParam = "";
        this.RemoteID = "";
        this.gAmountQuota = 0;

        //add by hj
        this.gReadIdCardFlag = 0; //��������֤ҳ���־λ 0�������׵�һ�����֤��1�����ֿ��˱������֤��2�������������֤
        this.gCheckInfoFlag = false; //��Ҫ�����֤��־
        this.gAuthorRefuse = ""; //��Ȩ�ܾ�ԭ��

        this.gLTotalItems = ""; //��һ���鳮���볮����Ϣ
        this.gLcount = "1"; //��һ���鳮����
        this.gCheckResult = ""; //�����˲���
        this.gOldTellerRadioId = "TypeNo1"; //��˲�ͨ�����˳�ʱ�����ԭ��Ա��,Ĭ�ϵ�һ����
        this.gIniFileName = "c:\\TCR\\jsapp\\ini\\TCR_CUSTOMIZE.INI"; //��������·��

        this.gOldCimRefusedNums = 0; //add by art for ��ˮ��Ӿܳ�RJ����: �����鳮��ǰ�����оܳ�������������)
        this.gbContinueTransFlag = false; //�������ױ�ʶ
        this.gbPartCashIn = false; //�鳮�����볮������

        this.gNoPtrSerFlag = false; //��ƾ�������־λ

        this.gbPartCashInEnd = false;
        this.gbStartServiceOrNot = true; //add by tsx  �жϹ���Ա�Ƿ��������
        this.gTerminalID = ""; //�ն˺ţ��ò��������ʼ������opendevice��Troubleҳ���ȡ��

        this.gPHOTOIMAGURL = ""; //add by tsx  ����ͼƬ·����
        this.gTransStatus = "";
        this.gCardOrBookBank = 2; //add by tsx Ӧ�ø���λ���жϴ��ۻ��ǿ��š������ߴ���������ATM��� ���п���1  ���ۣ�2��
        this.gReINQ = false; //add by tsx �ж�INQ����
        this.gMixSelfCWD = false; //add by tsx ����ȡ���־λ
        this.gIdNumberForRisk = ""//add by tsx ��ѯ���յȼ������֤
        this.gATMORTCR = ""//add by tsx �ж��Ƿ�Ϊ���ǿ�
        this.gShowPrintButton = true; //add by tsx �ж��Ƿ���Ҫ��ʾ��ӡƾ����ť
        //add by grb for �������� begin
        this.MTRN_NO = 0
        this.MTRN_NG = 0
        this.MTRN_OFF = 0
        this.MTRN_OK = 1
        this.MTRN_YES = 1
        this.MTRN_NEED = 1
        this.MTRN_ON = 1
        this.MTRN_CHANGED = 2

        //Mark of card type
        this.MTRN_PB = 260
        this.MTRN_NOCARD = 261
        this.MTRN_CARD = 262
        this.MTRN_ICCARD = 263
        this.MTRN_ICNG = 264
        this.MTRN_CASHINNG = 265
        this.MTRN_CASHOUTNG = 266
        this.MTRN_ABFUL = 267
        this.MTRN_IGNORE = 268

        //Transaction differ start
        this.MTRN_INQUIRYBALANCE = 100
        this.MTRN_TRANSFERACNTINQ = 101
        this.MTRN_PASSWORDCHANGE = 102
        this.MTRN_WITHDRAW_CARD = 103
        this.MTRN_WITHDRAW_PB = 104
        this.MTRN_DEPOSIT_CARD = 105
        this.MTRN_DEPOSIT_PB = 106
        this.MTRN_NOCARDDEPOSITACNTINQ = 107
        this.MTRN_NOCARDDEPOSIT = 108
        //Transaction differ end

        this.MTRN_TRNDIFFERNAME = 10
        this.MTRN_CANCELTRNNAME = 11
        this.MTRN_TRNDIFFERABBREVIATION = 12
        this.MTRN_CANCELTRNABBREVIATION = 13
        this.MTRN_ICTRNTYPE = 14
        this.MTRN_ECSERVICE = 15
        this.MTRN_TRNMSG = 16
        this.MTRN_CANCELMSG = 17
        this.MTRN_JNLFWDFMT = 18
        this.MTRN_CANCELJNLFWDFMT = 19
        this.MTRN_STATISTICALTYPE = 20
        this.MTRN_CANCELSTATISTICALTYPE = 21
        this.MTRN_TRNDIFFERNAMES = 23
        this.MTRN_CANCELTRNNAMES = 24
        this.MTRN_TRNDIFFERABBREVIATIONS = 25
        this.MTRN_CANCELTRNABBREVIATIONS = 26
        this.MTRN_TRNNAMES = 27
        this.MTRN_TRNSPOSSIBILITY = 28
        this.MTRN_POSSIBLETRNNAMES = 29
        this.MTRN_POSSIBLETRN = 30
        this.MTRN_POSSIBLETRNBK = 31
        this.MTRN_TRANSACTIONDIFFER = 32
        this.MTRN_TRANLIMITAMOUNTREAL = 33
        this.MTRN_TRANLIMITAMOUNTRULE = 35
        this.MTRN_AUTHORIZEDAMOUNTRULE = 36
        this.MTRN_REMAINDEPSITAMOUT = 37
        //add by grb for �������� end

        //д���׼�¼
        this.MTSL_WRITETRANSRECORD = 0   //д���׼�¼0---y
        this.MTSL_WRITERETAINCARDRECORD = 1   //д�̿���¼1---y
        this.MTSL_WRITECHECKTRANSRECORD = 2   //д���˼�¼2
        this.MTSL_READTRANSRECORD = 3   //�����׼�¼3
        //this.MTSL_READRETAINCARDRECORD = 4   //���̿���¼4---��2��
        this.MTSL_RENAMERECORD = 4   //���������׺��̿���¼5--y

        this.clearGlobalData = function () {
            this.gTransactiontype = "";
            this.gCardno = "";
            this.gPoundage = "";
            this.gResponsecode = "";
            this.gMasterkeyA = "";
            this.gMasterkeyB = "";
            this.gMasterkeyC = "";
            this.gIdCardpic = "";
            this.gCheckIdCardpic = "";
            this.gIdName = "";
            this.gIdNation = "";
            this.gIdSex = "";
            this.gIdNumber = "";
            this.gChooseMoney = 0;
            this.gOldPinBlock = "";
            this.gNewPinBlock = "";
            this.gbAmountType = 0;
            this.gDispenseCounts = 0;
            this.gPrintFSN = false;
            this.gAcceptCounts = 0;
            this.gMACKEY = false;
            this.gSubBankNum = "";
            this.gDevStatus = "";
            this.gTslChooseType = ""; //��������
            this.gTslJnlNum = ""; //��ˮ��
            this.gTslMoneyCount = ""; //���
            this.gTslResult = "FAIL"; //���׽��
            this.gTslDate = "";
            this.gTslTime = "";
            this.gTslFlag = false;
            this.gTslChooseJnlType = ""; //������ˮ�������ͣ�0107�����0108����ȡ��
            this.gTslSysrefnum = ""; //��̨������ˮ��
            this.gTslJnlBtn = ""; //�豸��ˮ���κ�
            this.gTslFailType = ""; //�쳣״̬���ͣ�4λ��
            this.gTslResponsecode = "";
            this.gCustomerName = "";
            this.gstrIdCardInfo = "";
            this.CreateBusiListHead = "";
            this.CreateBusiListUrl = "";
            this.CreateBusiListParam = "";
            this.CreateTaskHead = "";
            this.CreateTaskUrl = "";
            this.CreateTaskParam = "";
            this.RemoteID = "";
            this.gAmountQuota = 0;

            this.gCheckInfoFlag = false;
            this.gAuthorRefuse = false;

            this.gLTotalItems = ""; //��һ���鳮���볮����Ϣ
            this.gLcount = "1"; //��һ���鳮����
            this.gCheckResult = ""; //�����˲���
            this.gOldTellerNo = "TypeNo1";

            this.gOldCimRefusedNums = 0; //add by art for ��ˮ��Ӿܳ�RJ����: �����鳮��ǰ�����оܳ�������������)			
            this.gbContinueTransFlag = false; //�������ױ�ʶ
            this.gbPartCashIn = false; //�鳮�����볮������

            this.gNoPtrSerFlag = false; //��ƾ�������־λ

            this.gbPartCashInEnd = false;

            this.gTransStatus = "";
            this.gReadIdCardFlag = 0;
            this.gCardOrBookBank = 2; //add by tsx Ӧ�ø���λ���жϴ��ۻ��ǿ��š������ߴ���������ATM��� ���п���1  ���ۣ�2��
            this.gReINQ = false; //add by tsx �ж�INQ����
            this.gMixSelfCWD = false; //add by tsx ����ȡ���־λ
            this.gIdNumberForRisk = ""//add by tsx ��ѯ���յȼ������֤
            this.gATMORTCR = ""//add by tsx �ж��Ƿ�Ϊ���ǿ�
            this.gShowPrintButton = true; //add by tsx �ж��Ƿ���Ҫ��ʾ��ӡƾ����ť

        };
        this.ContinueTransClearGlobalData = function () {
            this.gTransactiontype = "";
            this.gPoundage = "";
            this.gResponsecode = "";
            this.gIdCardpic = "";
            this.gCheckIdCardpic = "";
            this.gIdName = "";
            this.gIdNation = "";
            this.gIdSex = "";
            this.gIdNumber = "";
            this.gChooseMoney = 0;
            this.gOldPinBlock = "";
            this.gNewPinBlock = "";
            this.gbAmountType = 0;
            this.gDispenseCounts = 0;
            this.gPrintFSN = false;
            this.gAcceptCounts = 0;
            this.gAmountQuota = 0;

            this.gCheckInfoFlag = false;
            this.gAuthorRefuse = false;

            this.gLTotalItems = ""; //��һ���鳮���볮����Ϣ
            this.gLcount = "1"; //��һ���鳮����
            this.gCheckResult = ""; //�����˲���
            this.gOldTellerNo = "TypeNo1";

            this.gOldCimRefusedNums = 0; //add by art for ��ˮ��Ӿܳ�RJ����: �����鳮��ǰ�����оܳ�������������)          
            this.gbPartCashIn = false; //�鳮�����볮������
            this.gbPartCashInEnd = false;
            this.gReadIdCardFlag = 0;
            this.gPHOTOIMAGURL = "";
            this.gReINQ = false; //add by tsx �ж�INQ����
            this.gMixSelfCWD = false; //add by tsx ����ȡ���־λ
        };
    };
})();

