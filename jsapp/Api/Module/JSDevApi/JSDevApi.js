(function(){
window.JSDevApi = function(ServiceName, InstanceName, obj){
	var object = obj;
	var ServiceName = ServiceName;
	var InstanceName = InstanceName;
	this.bOpenDevice = false;
	this.bDeviceStatus = false;
	this.LastEventData;
	this.StDeviceStatus;

	//event handler
	this.addEvent = function(event,handler){
		displayMessage(obj.id + "->attachEvent(" + event+ ")");
		return obj.attachEvent(event,handler);
	};
	this.removeEvent = function(event, handler){
		displayMessage(obj.id + "->detachEvent(" + event+ ")");
		return obj.detachEvent(event,handler);
	};

	this.openSuccessful =  function(){
		this.bOpenDevice = true;
	}

	this.RegisterMessage = function(){
	displayMessage(obj.id + "->RegisterMessage()");
	return obj.RegisterMessage();
	};

	this.UnRegisterMessage = function(){
	displayMessage(obj.id + "->UnRegisterMessage()");
	return obj.UnRegisterMessage();
	};

	this.CloseConnection = function(){
		displayMessage(obj.id + "->CloseConnection()");
		return obj.CloseConnection();
	};

	this.OpenConnection = function(){
		displayMessage(obj.id + "->OpenConnection()");
		return obj.OpenConnection();
	};

	this.SetServiceName = function(){
		obj.ServiceName = ServiceName;
	};

	this.StDetailedDeviceStatus = function(){
		return obj.StDetailedDeviceStatus;
	};

};
})();

