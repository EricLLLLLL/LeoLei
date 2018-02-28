(function(){
window.JSApiScr = function(ServiceName, InstanceName, obj){
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

	this.Reset = function(resetaction, timeout){
		displayMessage(obj.id + "->Reset(" + resetaction + "," + timeout + ")");
		return obj.Reset(resetaction, timeout);
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

	this.StMediaStatus = function(){
		return obj.StMediaStatus;
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

};
})();

