/*@create by:  tsxiong
*@time: 2016Äê03ÔÂ20ÈÕ
*/
; (function(){
    var bSPL1 = false;
	var CallResponse = App.Cntl.ProcessOnce( function(Response){
    //TO DO:
        Clearup();
        //Entry the flows control process.
        App.Cntl.ProcessDriven( Response );
    });
    var Initialize = function() {
    App.Timer.TimeoutDisposal(TimeoutCallBack);
    EventLogin();
    //@initialize scope start
    }();//Page Entry

   //@User ocde scope start

   //@User code scope end 
    function ChangebSPL1() { 
         //TO DO:
         bSPL1 = false
    }
    document.getElementById("SPL1").onclick = function(){
        bSPL1 = true;
        var t=window.setTimeout(ChangebSPL1,1000);
    }
    document.getElementById("SPL2").onclick = function(){
        if (bSPL1) {
            bSPL1 = false;
			top.Jnl.PrintSync("AdminOpenSpl");
            top.Sys.OpenManagePage();
        };
    }

    //event handler
     function onServiceOpened(){             
        return CallResponse('OnLine');
    }

    function EventLogin() {
        top.Sys.ServiceOpened.connect(onServiceOpened);
    }

    function EventLogout() {
       top.Sys.ServiceOpened.disconnect(onServiceOpened);
    }

       //Countdown function
    function TimeoutCallBack() {
        
        return CallResponse('TimeOut');
     }
       //Page Return
    
      //remove all event handler
    function Clearup(){
      //TO DO:
    EventLogout();
      App.Timer.ClearTime();
    }
})();
