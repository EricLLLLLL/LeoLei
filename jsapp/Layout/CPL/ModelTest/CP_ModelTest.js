; (function(){
    var CallResponse = App.Cntl.ProcessOnce (function (Response) { 
    //TO DO:
         Clearup();
        //Entry the flows control process.
          App.Cntl.ProcessDriven( Response );
        }),
       Initialize = function() {
        document.getElementById('PageTitle').innerText = '';
        EventLogin();
      //@initialize scope start
        top.API.Crd.AcceptAndReadTracks('2,3', 20000); 
         
        //
        App.Timer.TimeoutDisposal(TimeoutCallBack);
      }();//Page Entry

    // 控制按钮是否可点击 需要控制的按钮的id
    var BtnIdArr = ['Exit', 'OK'];
    BtnEnable(BtnIdArr); // 可以点击
    BtnDisable(BtnIdArr); // 失效 不能点击 






   //@User ocde scope start
    document.getElementById('Exit').onclick = function(){

         return CallResponse('Exit');
    }

    document.getElementById('OK').onclick = function(){
      
         return CallResponse('OK');
    }
   
   //@User code scope end 

    //event handler
    function onCardInserted(){

    }
    //event handler
    function onCardAccepted(){
       return CallResponse('CardAccepted');
   }   
   
    //Register the event
    function EventLogin() {
      //top.Crd.CardInserted.connect(onCardInserted);        
    }
  
    function EventLogout() {
      //top.Crd.CardInserted.disconnect(onCardInserted);      
    }

       //Countdown function
    function TimeoutCallBack() {
        
        return CallResponse('TimeOut');
     }
      //remove all event handler
    function Clearup(){
      //TO DO:
    EventLogout();
      App.Timer.ClearTime();
    }
})();
