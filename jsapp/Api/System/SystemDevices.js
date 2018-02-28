(function(){
	
window.SystemDevices = function(ocxId ){
        var xmldom = new ActiveXObject("Microsoft.XMLDOM");
		// SELF
        xmldom.load('./Api/System/SystemDevices.xml');
		// KAL
		//xmldom.load('./Api/System/SystemDevices_KAL.xml');
        if (xmldom.parseError != 0){
              throw new Error("XML parsing error: " + xmldom.parseError.reason);
        }
		var controls = xmldom.getElementsByTagName("control");
		for( var i=0; i< controls.length; i++ ){
			 if(controls[i].getAttributeNode("id").nodeValue == ocxId ){
			     this.OcxObjectName = controls[i].childNodes[0].text;
			     this.OcxClassId = controls[i].childNodes[1].text;
				 break;
			 }
		}
		var devices = xmldom.getElementsByTagName("device");
		for( var i=0; i< devices.length; i++ ){
			 if(devices[i].getAttributeNode("id").nodeValue == ocxId ){
			     this.serviceName = devices[i].childNodes[0].text;
			     this.controlName = devices[i].childNodes[1].text;
				 break;
			 }
		}
};
})();
