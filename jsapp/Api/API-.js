/*@module:API
 *@create by:  hyhua
 *@time: 2015��11��20��
 ****************************************
 *@��������:
 *���ظ�Ӳ��ģ�顣
 *
 ***************************************
 */


(function () {
    //the module type
    window.API = {
        initialize: function () {
            top.JsFilename = "";
            top.ErrorInfo = "���׳ɹ�!";
            top.FirstOpenFlag = true; //�Ƿ�Ϊ��һ�δ�
            data.call(this); //��JSData�ڶ����ȫ�ֱ�����Ϊtop.API
            global.call(this); //��JSGlobal�ڶ����ȫ�ֱ�����Ϊtop.API
           // displayMessage("Api Initialize...");
            //����Ӳ������
           new QWebChannel(qt.webChannelTransport, function(channel){
			//Ӳ������
            			top.Crd = channel.objects.IDC;
            			top.Cim = ;
            			top.Cdm = ;
            			top.Fpi = ;
            			top.Pin = ;
            			top.Ptr = ;
            			top.Idr = ;
            			top.Scr = ;
            			top.Siu = ;
            			//��Ӳ��Ӳ������
            			top.Jnl = ;
            			top.Tcp = ;
            			top.Sys = ;
            			top.Tsl = ;
            			top.Dat = channel.objects.DAT;
            			top.Ime = ;
            			//ũ�ж���
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
 
