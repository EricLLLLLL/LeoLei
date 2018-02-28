(function(){
window.Voice = function(){
	var soundObj = null;
	var xmlDoc = null;
	var urlXml = "./Framework/Plugin/Voice/Voice.xml";
    
    this.init = function(){
        var obj;
	    obj = document.createElement("bgsound");
		//xmlDoc = new ActiveXObject("Microsoft.XMLDOM");
		//xmlDoc.async = false;
		//xmlDoc.load(urlXml);
		try{
			xmlDoc= document.implementation.createDocument("","",null); 
			xmlDoc.async = false;
			xmlDoc.load(urlXml);
		}catch(e){
			var xmlhttp=new window.XMLHttpRequest();  
			 xmlhttp.open("get",urlXml,false);  
			 xmlhttp.send(null);  
			xmlDoc = xmlhttp.responseXML.documentElement; 
		}
		

		obj.id = "ModuleSound";
		obj.src = "";
		obj.autoplay = true;
		soundObj = obj;
		document.body.appendChild(soundObj);
    };

    this.play = function( code ){
        var temp = null;
	    if (/^[0-9a-zA-Z_]+$/.test(code)) {
	        if (xmlDoc != null) {
	            var childList = xmlDoc.documentElement.getElementsByTagName(code);
	            if (childList.length >= 1) {
	                temp = childList[0].getAttribute("path");
	            }
	        }
	    }
	    else {
	        temp = code;
	    }
	    if (soundObj == null){
	        init();
	    }
	    if (temp != null){
	        soundObj.src = temp;
	    }
	    else{
	        soundObj.src = code;
	    }
	    return true;
    };

    this.del = function(){
    	if (soundObj == null)
			init();
		soundObj.src = "";
    };
};

})();
