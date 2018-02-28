(function(){
window.JSApiCim = function(ServiceName, InstanceName, obj){
	//inherit the base calss attribute
	JSDevApi.call(this, ServiceName, InstanceName, obj);

	this.AcceptCash = function(timeout){
		displayMessage(obj.id + "->AcceptCash(" + timeout + ")");
		return obj.AcceptCash(timeout);
	};

	this.CancelAcceptCash = function(){
		displayMessage(obj.id + "->CancelAcceptCash()");
		return obj.CancelAcceptCash();
	};

	this.StoreEscrowedCash = function(timeout){
		displayMessage(obj.id + "->StoreEscrowedCash(" + timeout + ")");
		return obj.StoreEscrowedCash(timeout);
	};

	this.EjectEscrowedCash = function(timeout){
		displayMessage(obj.id + "->EjectEscrowedCash(" + timeout + ")");
		return obj.EjectEscrowedCash(timeout);
	};

	this.OpenShutter = function(timeout){
		displayMessage(obj.id + "->OpenShutter(" + timeout + ")");
		return obj.OpenShutter(timeout);
	};

	this.CloseShutter = function(timeout){
		displayMessage(obj.id + "->CloseShutter(" + timeout + ")");
		return obj.CloseShutter(timeout);
	};

	this.Reset = function(retractarea, id, timeout){
		displayMessage(obj.id + "->Reset(" + retractarea + "," + id + "," + timeout + ")");
		return obj.Reset(retractarea, id, timeout);
	};

	this.GetLastAcceptedAmountSync = function(currency, validity){
		displayMessage(obj.id + "->GetLastAcceptedAmountSync(" + currency + "," + validity + ")");
		return obj.GetLastAcceptedAmountSync(currency, validity);
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

	this.SetUnitTypeSync = function(type){
		displayMessage(obj.id + "->SetUnitTypeSync(" + type + ")");
		return obj.SetUnitTypeSync(type);
	};

	this.SetSupportValue = function(value){
		displayMessage(obj.id + "->SetSupportValue(" + value + ")");
		return obj.SetSupportValue(value);
	};

	this.PrepareForAcceptCash = function(){
		displayMessage(obj.id + "->PrepareForAcceptCash()");
		return obj.PrepareForAcceptCash();
	};

	this.StatisticsData = function(){
		displayMessage(obj.id + "->StatisticsData()");
		return obj.StatisticsData();
	};

	this.StEscrowStatus = function(){
		return obj.StEscrowStatus;
	};

	this.StShutterStatus = function(){
		return obj.StShutterStatus;
	};

	this.StTransportStatus = function(){
		return obj.StTransportStatus;
	};

	this.StInputStatus = function(){
		return obj.StInputStatus;
	};

	this.StAcceptorStatus = function(){
		return obj.StAcceptorStatus;
	};

	this.StSafeDoorStatus = function(){
		return obj.StSafeDoorStatus;
	};

	this.CpCanEscrow = function(){
		return obj.CpCanEscrow;
	};

	this.CpShutterControlSupported = function(){
		return obj.CpShutterControlSupported;
	};

	this.CpMaxAcceptItems = function(){
		return obj.CpMaxAcceptItems;
	};

	this.CpCanDetectCashInserted = function(){
		return obj.CpCanDetectCashInserted;
	};

	this.CpCanDetectCashTaken = function(){
		return obj.CpCanDetectCashTaken;
	};

	this.CpRetractAreas = function(){
		return obj.CpRetractAreas;
	};

	this.CpExchangeTypes = function(){
		return obj.CpExchangeTypes;
	};

	this.LastAcceptStatus = function(){
		return obj.LastAcceptStatus;
	};

	this.NumOfRefused = function(){
		return obj.NumOfRefused;
	};

    this.CUStatus = function(){
        return obj.CUStatus;
    };

};
})();

