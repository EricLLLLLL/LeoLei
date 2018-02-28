;
(function () {
  var Initialize = function () {
    EventLogin();
    //@initialize scope start   
    // 控制按钮是否可点击 需要控制的按钮的id
    BtnEnable(BtnIdArr); // 可以点击
    BtnDisable(BtnIdArr); // 失效 不能点击 
    // ButtonDisable();
    // ButtonEnable();
    //
    App.Timer.TimeoutDisposal(TimeoutCallBack, ButtonDisable);
  }(); //Page Entry
  var BtnIdArr = ['Exit', 'OK'];
  //@User ocde scope start


  /*    function ButtonDisable(){
          document.getElementById('Exit').disabled = true;
          document.getElementById('OK').disabled = true;
     } 
     
     function ButtonEnable(){
          document.getElementById('Exit').disabled = false;
          document.getElementById('OK').disabled = false;
     }  */

  document.getElementById("Exit").onclick = function () {
    ButtonDisable();
    return CallResponse("Exit");
  }

  document.getElementById("OK").onclick = function () {
    ButtonDisable();
    return CallResponse("OK");
  }

  //@User code scope end 

  //event handler


  //Register the event
  function EventLogin() {
    //top.Crd.CardInserted.connect(onCardInserted);        
  }

  function EventLogout() {
    //top.Crd.CardInserted.disconnect(onCardInserted);      
  }

  //Countdown function
  function TimeoutCallBack() {

    return CallResponse("TimeOut");
  }

  //Page Return
  function CallResponse(Response) {
    //TO DO:
    Clearup();
    //Entry the flows control process.
    App.Cntl.ProcessDriven(Response);
  }
  //remove all event handler
  function Clearup() {
    //TO DO:
    EventLogout();
    App.Timer.ClearTime();
  }
})();