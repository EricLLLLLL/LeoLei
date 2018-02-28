; (function(){
    var Initialize = function() {
	  //EventLogin();
    //@initialize scope start    
    
    //
    App.Timer.TimeoutDisposal(TimeoutCallBack);
    }();//Page Entry

    //@User ocde scope start
    document.getElementById("Exit").onclick = function(){

         return CallResponse("Exit");
    }

    document.getElementById("OK").onclick = function(){
      
         return CallResponse("OK");
    }
   
    //@User code scope end 

    //event handler
    
   
    //Register the event
    function EventLogin() {
      //top.Crd.CardInserted.connect(onCardInserted);        
    }

    function EventLogout() {
      // top.Crd.CardInserted.disconnect(onCardInserted);        
    }

    //Countdown function
    function TimeoutCallBack() {
        
      return CallResponse("TimeOut");
    }
       //Page Return
    function  CallResponse ( Response ) { 
      //TO DO:
      Clearup();
      //Entry the flows control process.
      App.Cntl.ProcessDriven( Response );
    }
      //remove all event handler
    function Clearup(){
      //TO DO:
	  //EventLogout();
      App.Timer.ClearTime();
    }
})();