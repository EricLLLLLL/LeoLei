(function(){
window.JSApiIdr = function(ServiceName, InstanceName, obj){
	//inherit the base calss attribute
	JSDevApi.call(this, ServiceName, InstanceName, obj);

	this.AcceptAndRead = function(timeout){
		displayMessage(obj.id + "->AcceptAndRead(" + timeout + ")");
		return obj.AcceptAndRead(timeout);
	};

	this.Eject = function(timeout){
		displayMessage(obj.id + "->Eject(" + timeout + ")");
		return obj.Eject(timeout);
	};

	this.Capture = function(timeout){
		displayMessage(obj.id + "->Capture(" + timeout + ")");
		return obj.Capture(timeout);
	};

	this.CancelAccept = function(){
		displayMessage(obj.id + "->CancelAccept()");
		return obj.CancelAccept();
	};

	this.CancelWait = function(){
		displayMessage(obj.id + "->CancelWait()");
		return obj.CancelWait();
	};

	this.Reset = function(resetaction){
		displayMessage(obj.id + "->Reset(" + resetaction + ")");
		return obj.Reset(resetaction);
	};

	this.StMediaStatus = function(){
		return obj.StMediaStatus;
	};

	this.CpCanEject = function(){
		return obj.CpCanEject;
	};

	this.CpCanCapture = function(){
		return obj.CpCanCapture;
	};

	this.Name = function(){
		return obj.Name;
	};

	this.Sex = function(){
		return obj.Sex;
	};

	this.Nation = function(){
		return obj.Nation;
	};

	this.Birthday = function(){
		return obj.Birthday;
	};

	this.Address = function(){
		return obj.Address;
	};

	this.Number = function(){
		return obj.Number;
	};

	this.Department = function(){
		return obj.Department;
	};

	this.StartTime = function(){
		return obj.StartTime;
	};

	this.EndTime = function(){
		return obj.EndTime;
	};

	this.PortraitFilePath = function(){
		return obj.PortraitFilePath;
	};

	this.FrontFilePath = function(){
		return obj.FrontFilePath;
	};

	this.SetFrontFilePath = function(newval){
		obj.FrontFilePath = newval;
	};

	this.BackFilePath = function(){
		return obj.BackFilePath;
	};

	this.SetBackFilePath = function(newval){
		obj.BackFilePath = newval;
	};

};
})();

