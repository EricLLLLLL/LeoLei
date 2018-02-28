;(function () {
    var CallResponse = App.Cntl.ProcessOnce( function(Response){
        //TO DO:
        Clearup();
        //Entry the flows control process.
        App.Cntl.ProcessDriven( Response );
    });
    var Initialize = function () {
        ButtonDisable();
        EventLogin();
        ButtonEnable();
    }();//Page Entry

    function ButtonDisable() {
        document.getElementById('backCPL').disabled = true;
    }

    function ButtonEnable() {
        document.getElementById('backCPL').disabled = false;
    }

    //@User ocde scope start

    function ButtonDisable() {
        document.getElementById('backCPL').disabled = true;
    }

    function ButtonEnable() {
        document.getElementById('backCPL').disabled = false;
    }

    document.getElementById('backCPL').onclick = function () {
        top.API.displayMessage("click-----backCPL°´Å¥");
        ButtonDisable();
        top.Sys.OpenFrontPage();
        ButtonEnable();
    };

    //@User code scope end

    //event handler
    function onOpenManagePageComplete() {
        top.API.displayMessage("onOpenManagePageComplete()´¥·¢");
        return CallResponse("OK");
    }
    //Register the event
    function EventLogin() {
        top.Sys.OpenManagePageComplete.connect(onOpenManagePageComplete);
    }

    function EventLogout() {
        top.Sys.OpenManagePageComplete.disconnect(onOpenManagePageComplete);
    }

    function Clearup(){
        //TO DO:
        EventLogout();
       // App.Timer.ClearTime();
    }
    // function CallResponse(Response) {
    //     EventLogout();
    //     App.Cntl.ProcessDriven(Response);
    // }
})();
