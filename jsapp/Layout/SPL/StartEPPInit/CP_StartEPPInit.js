/*@create by:  tsxiong
*@time: 2016年03月20日
*/
; (function(){
	var CallResponse = App.Cntl.ProcessOnce( function(Response){
      //TO DO:
	  Clearup();
	  //Entry the flows control process.
	  App.Cntl.ProcessDriven( Response );
    });
    var Initialize = function() {      
        EventLogin();
        top.Pin.Initialize();
    }();//Page Entry

   //@User ocde scope start

   function onDeviceError(){
       top.API.displayMessage("onDeviceError触发");
       return CallResponse("Exit");
   }   

    function onInitializeFailed(){
       top.API.displayMessage("onInitializeFailed触发");
       return CallResponse("Exit");
   }   
   
   function onInitializeComplete(){
       top.API.displayMessage("onInitializeComplete");
       return CallResponse("OK");
   }
    
    function EventLogin() {
        top.Pin.InitializeComplete.connect(onInitializeComplete);   
        top.Pin.InitializeFailed.connect(onInitializeFailed);    
        top.Pin.DeviceError.connect(onDeviceError);        
    }

    function EventLogout() {
         top.Pin.InitializeComplete.disconnect(onInitializeComplete);  
         top.Pin.InitializeFailed.disconnect(onInitializeFailed);   
         top.Pin.DeviceError.disconnect(onDeviceError);           
    }


    //remove all event handler
    function Clearup(){
	  EventLogout();
    }
})();