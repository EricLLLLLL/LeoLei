/*@create by:  tsxiong
*@time: 2016��03��20��
*/
; (function(){
	var CallResponse = App.Cntl.ProcessOnce( function(Response){
        App.Cntl.ProcessDriven( Response );
    });
    var Initialize = function() {    
    }();//Page Entry
    function ButtonDisable() {
        document.getElementById('Exit').disabled = true;
        document.getElementById('InputA').disabled = true;
        document.getElementById('InputB').disabled = true;
        document.getElementById('InputC').disabled = true;
        document.getElementById('InitEPP').disabled = true;
        document.getElementById('Setkey').disabled = true;
        document.getElementById('DownloadKey').disabled = true;
    }

    function ButtonEnable() {
        document.getElementById('Exit').disabled = false;
        document.getElementById('InputA').disabled = false;
        document.getElementById('InputB').disabled = false;
        document.getElementById('InputC').disabled = false;
        document.getElementById('InitEPP').disabled = false;
        document.getElementById('Setkey').disabled = false;
        document.getElementById('DownloadKey').disabled = false;
    }
   //@User ocde scope start
    document.getElementById('InputA').onclick = function(){
         top.API.displayMessage("���������ԿA,��Ӧ<InputAMaster>"); 
         ButtonDisable();
         return CallResponse('InputAMaster');
    }

     document.getElementById('InputB').onclick = function () {
         top.API.displayMessage("���������ԿB,��Ӧ<InputBMaster>");
         ButtonDisable();
         return CallResponse('InputBMaster');
     }

     document.getElementById('InputC').onclick = function(){
         top.API.displayMessage("���������ԿC,��Ӧ<InputCMaster>"); 
         ButtonDisable();
         return CallResponse('InputCMaster');
    }

    document.getElementById('InitEPP').onclick = function(){
         top.API.displayMessage("�����ʼ��EPP����,��Ӧ<InitEPP>"); 
         ButtonDisable();
         return CallResponse('InitEpp');
    }

     document.getElementById('Setkey').onclick = function () {
         top.API.displayMessage("����趨����Կ,��Ӧ<SetMasterKey>");
         ButtonDisable();
         return CallResponse('SetMasterKey');
     }

     document.getElementById('DownloadKey').onclick = function () {
         top.API.displayMessage("����趨����Կ,��Ӧ<DownloadKey>");
         ButtonDisable();
         return CallResponse('DownloadKey');
     }

      document.getElementById('Exit').onclick = function(){
         top.API.displayMessage("��Ӧ<Exit>"); 
         ButtonDisable();
         return CallResponse('Exit');
    }
   

})();
