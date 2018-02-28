/*@module:API
 *@create by:  hyhua
 *@time: 2015年11月20日
 ****************************************
 *@功能描述:
 *加载各硬件模块。
 *
 ***************************************
 */


(function () {
    //the module type
    window.API = {
        initialize: function () {
            top.JsFilename = "";
            top.ErrorInfo = "交易成功!";
            top.FirstOpenFlag = true; //是否为第一次打开
            data.call(this); //将JSData内定义的全局变量设为top.API
            global.call(this); //将JSGlobal内定义的全局变量设为top.API
           // displayMessage("Api Initialize...");
            //创建硬件对象
           new QWebChannel(qt.webChannelTransport, function(channel){
			//硬件对象
            			top.Crd = channel.objects.IDC;
            			top.Cim = ;
            			top.Cdm = ;
            			top.Fpi = ;
            			top.Pin = ;
            			top.Ptr = ;
            			top.Idr = ;
            			top.Scr = ;
            			top.Siu = ;
            			//非硬件硬件对象
            			top.Jnl = ;
            			top.Tcp = ;
            			top.Sys = ;
            			top.Tsl = ;
            			top.Dat = channel.objects.DAT;
            			top.Ime = ;
            			//农行对象
            			top.Ass = ;
            			top.Rpm = ;
            			top.Fck = ;
            			top.CashInfo = new JSCashInfo();
            
            			
            			top.flag = true;
            			//alert("init success");		
            		});
            /*
            if (typeof qt != 'undefined'){
            if(typeof qt.webChannelTransport != 'undefined'){
            		new QWebChannel(qt.webChannelTransport, function(channel){

            			top.Crd = channel.objects.IDC;
            			alert("init success");
            			
            		});
            }else{
            		alert("qt.webChannelTransport off");
            	}
            }else{
            	alert("qt off");
            }*/
        },
        
        OpenDevice : function(){
        	top.Crd.OpenConnection();
        },
        
      
        
        displayMessage: function (strMsg) {
            console.log(strMsg);
        }
        
    };
})();
 
