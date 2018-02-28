(function(){
window.JSApiCdm = function(ServiceName, InstanceName, obj){
	//inherit the base calss attribute
	JSDevApi.call(this, ServiceName, InstanceName, obj);

	this.Mix = function(amount, currency, mixalgorithm){
		displayMessage(obj.id + "->Mix(" + amount + "," + currency + "," + mixalgorithm + ")");
		return obj.Mix(amount, currency, mixalgorithm);
	};

	this.MixAndDispense = function(amount, currency, mixalgorithm){
		displayMessage(obj.id + "->MixAndDispense(" + amount + "," + currency + "," + mixalgorithm + ")");
		return obj.MixAndDispense(amount, currency, mixalgorithm);
	};

	this.Dispense = function(amount, notecounts, currency, mixalgorithm){
		displayMessage(obj.id + "->Dispense(" + amount + "," + notecounts + "," + currency + "," + mixalgorithm + ")");
		return obj.Dispense(amount, notecounts, currency, mixalgorithm);
	};

	this.Present = function(timeout){
		displayMessage(obj.id + "->Present(" + timeout + ")");
		return obj.Present(timeout);
	};

	this.Reset = function(retractarea, id, timeout){
		displayMessage(obj.id + "->Reset(" + retractarea + "," + id + "," + timeout + ")");
		return obj.Reset(retractarea, id, timeout);
	};

	this.SetUnitTypeSync = function(type){
		displayMessage(obj.id + "->SetUnitTypeSync(" + type + ")");
		return obj.SetUnitTypeSync(type);
	};

	this.StartExchange = function(){
		displayMessage(obj.id + "->StartExchange()");
		return obj.StartExchange();
	};

	this.EndExchange = function(){
		displayMessage(obj.id + "->EndExchange()");
		return obj.EndExchange();
	};

	this.SetUnitCountSync = function(unit){
		displayMessage(obj.id + "->SetUnitCountSync(" + unit + ")");
		return obj.SetUnitCountSync(unit);
	};

	this.SetUnitValueSync = function(value){
		displayMessage(obj.id + "->SetUnitValueSync(" + value + ")");
		return obj.SetUnitValueSync(value);
	};

	this.SetDispenseAmount = function(amount){
		displayMessage(obj.id + "->SetDispenseAmount(" + amount + ")");
		return obj.SetDispenseAmount(amount);
	};

	this.OpenShutter = function(timeout){
		displayMessage(obj.id + "->OpenShutter(" + timeout + ")");
		return obj.OpenShutter(timeout);
	};

	this.CloseShutter = function(timeout){
		displayMessage(obj.id + "->CloseShutter(" + timeout + ")");
		return obj.CloseShutter(timeout);
	};

	this.NumLogicalCashUnits = function(){
		return obj.NumLogicalCashUnits;
	};

	this.CUNumber = function(){
		return obj.CUNumber;
	};

	this.CUType = function(){
		return obj.CUType;
	};

	this.CUId = function(){
		return obj.CUId;
	};

	this.CUStatus = function(){
		return obj.CUStatus;
	};

	this.CUCurrentCount = function(){
		return obj.CUCurrentCount;
	};

	this.CUCurrency = function(){
		return obj.CUCurrency;
	};

	this.CUNoteValue = function(){
		return obj.CUNoteValue;
	};

	this.CUInitialCount = function(){
		return obj.CUInitialCount;
	};

	this.CUMaxThreshold = function(){
		return obj.CUMaxThreshold;
	};

	this.CUMinThreshold = function(){
		return obj.CUMinThreshold;
	};

	this.PUId = function(){
		return obj.PUId;
	};

	this.PUCUNumber = function(){
		return obj.PUCUNumber;
	};

	this.PUCurrentCount = function(){
		return obj.PUCurrentCount;
	};

	this.PUCUId = function(){
		return obj.PUCUId;
	};

	this.PUStatus = function(){
		return obj.PUStatus;
	};

	this.PUInitialCount = function(){
		return obj.PUInitialCount;
	};

	this.PURejectCount = function(){
		return obj.PURejectCount;
	};

	this.CpCanRetract = function(){
		return obj.CpCanRetract;
	};

	this.StSafeDoorStatus = function(){
		return obj.StSafeDoorStatus;
	};

	this.StDispenserStatus = function(){
		return obj.StDispenserStatus;
	};

	this.MixNumber = function(){
		return obj.MixNumber;
	};

	this.StStackerStatus = function(){
		return obj.StStackerStatus;
	};

	this.StTransportStatus = function(){
		return obj.StTransportStatus;
	};

	this.StOutputStatus = function(){
		return obj.StOutputStatus;
	};

	this.StShutterStatus = function(){
		return obj.StShutterStatus;
	};

	this.CpCanDetectCashTaken = function(){
		return obj.CpCanDetectCashTaken;
	};

	this.RemainCount = function(){
		return obj.RemainCount;
	};

};
})();

