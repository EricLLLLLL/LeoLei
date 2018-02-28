(function(){
window.JSApiCrd = function(ServiceName, InstanceName, obj){
	//inherit the base calss attribute
	JSDevApi.call(this, ServiceName, InstanceName, obj);

	this.AcceptAndReadAvailableTracks = function(trackmap, timeout){
		displayMessage(obj.id + "->AcceptAndReadAvailableTracks(" + trackmap + "," + timeout + ")");
		return obj.AcceptAndReadAvailableTracks(trackmap, timeout);
	};

	this.AcceptAndReadTracks = function(trackmap, timeout){
		displayMessage(obj.id + "->AcceptAndReadTracks(" + trackmap + "," + timeout + ")");
		return obj.AcceptAndReadTracks(trackmap, timeout);
	};

	this.CancelAccept = function(){
		displayMessage(obj.id + "->CancelAccept()");
		return obj.CancelAccept();
	};

	this.Eject = function(timeout){
		displayMessage(obj.id + "->Eject(" + timeout + ")");
		return obj.Eject(timeout);
	};

	this.Capture = function(timeout){
		displayMessage(obj.id + "->Capture(" + timeout + ")");
		return obj.Capture(timeout);
	};

	this.AcceptAndChipInitialise = function(token, timeout){
		displayMessage(obj.id + "->AcceptAndChipInitialise(" + token + "," + timeout + ")");
		return obj.AcceptAndChipInitialise(token, timeout);
	};

	this.ChipPower = function(action){
		displayMessage(obj.id + "->ChipPower(" + action + ")");
		return obj.ChipPower(action);
	};

	this.ResetBinCountSync = function(){
		displayMessage(obj.id + "->ResetBinCountSync()");
		return obj.ResetBinCountSync();
	};

	this.Reset = function(resetaction, timeout){
		displayMessage(obj.id + "->Reset(" + resetaction + "," + timeout + ")");
		return obj.Reset(resetaction, timeout);
	};

	this.PBOCGetICInfo = function(timeout){
		displayMessage(obj.id + "->PBOCGetICInfo(" + timeout + ")");
		return obj.PBOCGetICInfo(timeout);
	};

	this.PBOCReadIcTLV = function(timeout){
		displayMessage(obj.id + "->PBOCReadIcTLV(" + timeout + ")");
		return obj.PBOCReadIcTLV(timeout);
	};

	this.CpBinSize = function(){
		return obj.CpBinSize;
	};

	this.CpCanCapture = function(){
		return obj.CpCanCapture;
	};

	this.CpCanEject = function(){
		return obj.CpCanEject;
	};

	this.CpCanReadTrack1 = function(){
		return obj.CpCanReadTrack1;
	};

	this.CpCanReadTrack2 = function(){
		return obj.CpCanReadTrack2;
	};

	this.CpCanReadTrack3 = function(){
		return obj.CpCanReadTrack3;
	};

	this.CpCanReadTrackJIS2 = function(){
		return obj.CpCanReadTrackJIS2;
	};

	this.CpCanWriteTrack1 = function(){
		return obj.CpCanWriteTrack1;
	};

	this.CpCanWriteTrack2 = function(){
		return obj.CpCanWriteTrack2;
	};

	this.CpCanWriteTrack3 = function(){
		return obj.CpCanWriteTrack3;
	};

	this.CpSecurity = function(){
		return obj.CpSecurity;
	};

	this.CpVariant = function(){
		return obj.CpVariant;
	};

	this.CpChipPower = function(){
		return obj.CpChipPower;
	};

	this.CpPowerSaveModeControl = function(){
		return obj.CpPowerSaveModeControl;
	};

	this.CpSupportedGuidelights = function(){
		return obj.CpSupportedGuidelights;
	};

	this.StBinCount = function(){
		return obj.StBinCount;
	};

	this.StBinStatus = function(){
		return obj.StBinStatus;
	};

	this.StMediaStatus = function(){
		return obj.StMediaStatus;
	};

	this.StChipStatus = function(){
		return obj.StChipStatus;
	};

	this.Track1 = function(){
		return obj.Track1;
	};

	this.Track2 = function(){
		return obj.Track2;
	};

	this.Track3 = function(){
		return obj.Track3;
	};

	this.Track1Status = function(){
		return obj.Track1Status;
	};

	this.Track2Status = function(){
		return obj.Track2Status;
	};

	this.Track3Status = function(){
		return obj.Track3Status;
	};

	this.AnswerToReset = function(){
		return obj.AnswerToReset;
	};

	this.CardNumber = function(){
		return obj.CardNumber;
	};

	this.CardSerial = function(){
		return obj.CardSerial;
	};

	this.StartTime = function(){
		return obj.StartTime;
	};

	this.EndTime = function(){
		return obj.EndTime;
	};

	this.Name = function(){
		return obj.Name;
	};

	this.NameExpend = function(){
		return obj.NameExpend;
	};

	this.IDNumber = function(){
		return obj.IDNumber;
	};

	this.IType = function(){
		return obj.IType;
	};

};
})();

