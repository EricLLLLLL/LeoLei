(function(){
window.JSApiPin = function(ServiceName, InstanceName, obj){
	//inherit the base calss attribute
	JSDevApi.call(this, ServiceName, InstanceName, obj);

	this.LoadMasterKey = function(keyname, keyvalue, timeout){
		displayMessage(obj.id + "->LoadMasterKey(" + keyname + "," + keyvalue + "," + timeout + ")");
		return obj.LoadMasterKey(keyname, keyvalue, timeout);
	};

	this.Initialize = function(){
		displayMessage(obj.id + "->Initialize()");
		return obj.Initialize();
	};

	this.LoadWorkKey = function(keyname, keyvalue, enckeyname){
		displayMessage(obj.id + "->LoadWorkKey(" + keyname + "," + keyvalue + "," + enckeyname + ")");
		return obj.LoadWorkKey(keyname, keyvalue, enckeyname);
	};

	this.EncryptECB = function(value, keyname, padchar){
		displayMessage(obj.id + "->EncryptECB(" + value + "," + keyname + "," + padchar + ")");
		return obj.EncryptECB(value, keyname, padchar);
	};

	this.DecryptECB = function(value, keyname, padchar){
		displayMessage(obj.id + "->DecryptECB(" + value + "," + keyname + "," + padchar + ")");
		return obj.DecryptECB(value, keyname, padchar);
	};

	this.EncryptCBC = function(value, keyname, ivname, padchar){
		displayMessage(obj.id + "->EncryptCBC(" + value + "," + keyname + "," + ivname + "," + padchar + ")");
		return obj.EncryptCBC(value, keyname, ivname, padchar);
	};

	this.DecryptCBC = function(value, keyname, ivname, padchar){
		displayMessage(obj.id + "->DecryptCBC(" + value + "," + keyname + "," + ivname + "," + padchar + ")");
		return obj.DecryptCBC(value, keyname, ivname, padchar);
	};

	this.GenerateMAC = function(value, keyname, ivname, padchar){
		displayMessage(obj.id + "->GenerateMAC(" + value + "," + keyname + "," + ivname + "," + padchar + ")");
		return obj.GenerateMAC(value, keyname, ivname, padchar);
	};

	this.GenerateMACClearIV = function(value, keyname, iv, padchar){
		displayMessage(obj.id + "->GenerateMACClearIV(" + value + "," + keyname + "," + iv + "," + padchar + ")");
		return obj.GenerateMACClearIV(value, keyname, iv, padchar);
	};

	this.GetPin = function(minlength, maxlength, autoend, activekeys, terminatorkeys, timeout){
		displayMessage(obj.id + "->GetPin(" + minlength + "," + maxlength + "," + autoend + "," + activekeys + "," + terminatorkeys + "," + timeout + ")");
		return obj.GetPin(minlength, maxlength, autoend, activekeys, terminatorkeys, timeout);
	};

	this.PinBlock = function(format, customerdata, padchar, keyname, xordata, secondkeyname){
		displayMessage(obj.id + "->PinBlock(" + format + "," + customerdata + "," + padchar + "," + keyname + "," + xordata + "," + secondkeyname + ")");
		return obj.PinBlock(format, customerdata, padchar, keyname, xordata, secondkeyname);
	};

	this.CancelGetPin = function(){
		displayMessage(obj.id + "->CancelGetPin()");
		return obj.CancelGetPin();
	};

	this.GetData = function(maxkeys, autoend, activekeys, terminatorkeys, timeout){
		displayMessage(obj.id + "->GetData(" + maxkeys + "," + autoend + "," + activekeys + "," + terminatorkeys + "," + timeout + ")");
		return obj.GetData(maxkeys, autoend, activekeys, terminatorkeys, timeout);
	};

	this.CancelGetData = function(){
		displayMessage(obj.id + "->CancelGetData()");
		return obj.CancelGetData();
	};

	this.GetKeyNamesSync = function(){
		displayMessage(obj.id + "->GetKeyNamesSync()");
		return obj.GetKeyNamesSync();
	};

	this.GetKeyUseFlagsSync = function(keyname){
		displayMessage(obj.id + "->GetKeyUseFlagsSync(" + keyname + ")");
		return obj.GetKeyUseFlagsSync(keyname);
	};

	this.KeyIsValidSync = function(keyname){
		displayMessage(obj.id + "->KeyIsValidSync(" + keyname + ")");
		return obj.KeyIsValidSync(keyname);
	};

	this.ExtendedLoadKey = function(keyname, keyvalue, useflags, identification){
		displayMessage(obj.id + "->ExtendedLoadKey(" + keyname + "," + keyvalue + "," + useflags + "," + identification + ")");
		return obj.ExtendedLoadKey(keyname, keyvalue, useflags, identification);
	};

	this.ExtendedLoadEncryptedKey = function(keyname, keyvalue, enckeyname, useflags, identification){
		displayMessage(obj.id + "->ExtendedLoadEncryptedKey(" + keyname + "," + keyvalue + "," + enckeyname + "," + useflags + "," + identification + ")");
		return obj.ExtendedLoadEncryptedKey(keyname, keyvalue, enckeyname, useflags, identification);
	};

	this.Reset = function(timeout){
		displayMessage(obj.id + "->Reset(" + timeout + ")");
		return obj.Reset(timeout);
	};

	this.CpCanCBC = function(){
		return obj.CpCanCBC;
	};

	this.CpCanECB = function(){
		return obj.CpCanECB;
	};

	this.CpCanRSA = function(){
		return obj.CpCanRSA;
	};

	this.CpCanMAC = function(){
		return obj.CpCanMAC;
	};

	this.CpCanTripleECB = function(){
		return obj.CpCanTripleECB;
	};

	this.CpCanTripleCBC = function(){
		return obj.CpCanTripleCBC;
	};

	this.CpCanTripleCFB = function(){
		return obj.CpCanTripleCFB;
	};

	this.CpCanTripleMAC = function(){
		return obj.CpCanTripleMAC;
	};

	this.CpKeysSupported = function(){
		return obj.CpKeysSupported;
	};

	this.PINLength = function(){
		return obj.PINLength;
	};

	this.CpPINFormatsSupported = function(){
		return obj.CpPINFormatsSupported;
	};

};
})();

