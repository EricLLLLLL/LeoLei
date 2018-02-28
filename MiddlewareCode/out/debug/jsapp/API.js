(function() {
	window.API = {
        Initialize: function () {
			if (typeof qt != 'undefined') {
		  		if (typeof qt.webChannelTransport != 'undefined') {
		    		new QWebChannel(qt.webChannelTransport,function(channel){
		    		this.Idc=channel.objects.IDC;
		    		this.Dat=channel.objects.DAT;
		    		alert("init success.");
		    		
		    		});
		    	}else {
		    		alert("qt.webChannelTransport off");
		    	}
			}else {
				alert("qt off");
			}
		},

		OpenDevice: function () {
            //this.Jnl.WriteLogSync("START OpenDevice", 1);
           // this.Idc.ServiceName = "IDC";
            var iRetCode=this.Idc.OpenConnection();
            alert(iRetCode);
        },

        CloseDevice: function (){
        	this.Idc.CloseConnection();
        },

	}
})();
