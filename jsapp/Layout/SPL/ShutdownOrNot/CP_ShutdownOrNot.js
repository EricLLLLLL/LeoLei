/*@create by:  tsxiong
*@time: 2016��03��20��
*/
; (function () {
	var CallResponse = App.Cntl.ProcessOnce( function(Response){
        //TO DO:
        Clearup();
        App.Cntl.ProcessDriven(Response);
    });
    var Initialize = function () {
		EventLogin();
    } (); 
    function ButtonDisable() {
        document.getElementById('Exit').disabled = true;
        document.getElementById('OK').disabled = true;
    }

    function ButtonEnable() {
        document.getElementById('Exit').disabled = false;
        document.getElementById('OK').disabled = false;
    }
    //@User ocde scope start
    document.getElementById("Exit").onclick = function () {
        ButtonDisable();
        return CallResponse("OK");
    }

    document.getElementById("OK").onclick = function () {
        ButtonDisable();
	top.Jnl.PrintSync("ShutDown");
		top.Sys.UpTransFile();
	document.getElementById("tip").innerHTML = "�����ϴ������ļ������Ժ�..."; 
      
    }

    function onUploadFileFailed() {
        top.API.displayMessage("onUploadFileFailed����");
document.getElementById("tip").innerHTML = "���ڹػ������Ժ�...";
		top.Sys.ShutDown(); 
    }
    //event handler
    function onUploadFileSucceed() {
        top.API.displayMessage("onUploadFileSucceed����");
document.getElementById("tip").innerHTML = "���ڹػ������Ժ�...";
		top.Sys.ShutDown(); 
    }

    //Register the event
    function EventLogin() {
		top.Sys.UploadFileFailed.connect(onUploadFileFailed);
        top.Sys.UploadFileSucceed.connect(onUploadFileSucceed);
    }

    function EventLogout() {
		top.Sys.UploadFileFailed.disconnect(onUploadFileFailed);
        top.Sys.UploadFileSucceed.disconnect(onUploadFileSucceed);
    }


    //remove all event handler
    function Clearup() {
        EventLogout();
    }
})();