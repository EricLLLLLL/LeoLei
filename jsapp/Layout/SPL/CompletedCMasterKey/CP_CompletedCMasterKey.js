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
        document.getElementById('OK').disabled = true;
    }
    document.getElementById("OK").onclick = function(){
         ButtonDisable();
         return CallResponse("OK");
    }

})();