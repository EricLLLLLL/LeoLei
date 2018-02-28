(function(){
window.JSApiPtr = function(ServiceName, InstanceName, obj){
	//inherit the base calss attribute
	JSDevApi.call(this, ServiceName, InstanceName, obj);

	this.Print = function(formname, fieldvalues, timeout){
		displayMessage(obj.id + "->Print(" + formname + "," + fieldvalues + "," + timeout + ")");
		return obj.Print(formname, fieldvalues, timeout);
	};

	this.ControlMedia = function(mediaaction, timeout){
		displayMessage(obj.id + "->ControlMedia(" + mediaaction + "," + timeout + ")");
		return obj.ControlMedia(mediaaction, timeout);
	};

	this.GetFormNamesSync = function(){
		displayMessage(obj.id + "->GetFormNamesSync()");
		return obj.GetFormNamesSync();
	};

	this.UpdateFormInfoSync = function(formname){
		displayMessage(obj.id + "->UpdateFormInfoSync(" + formname + ")");
		return obj.UpdateFormInfoSync(formname);
	};

	this.GetMediaNamesSync = function(){
		displayMessage(obj.id + "->GetMediaNamesSync()");
		return obj.GetMediaNamesSync();
	};

	this.UpdateMediaInfoSync = function(medianame){
		displayMessage(obj.id + "->UpdateMediaInfoSync(" + medianame + ")");
		return obj.UpdateMediaInfoSync(medianame);
	};

	this.PrintFile = function(filename, timeout, deletefile){
		displayMessage(obj.id + "->PrintFile(" + filename + "," + timeout + "," + deletefile + ")");
		return obj.PrintFile(filename, timeout, deletefile);
	};

	this.Reset = function(resetaction, timeout){
		displayMessage(obj.id + "->Reset(" + resetaction + "," + timeout + ")");
		return obj.Reset(resetaction, timeout);
	};

	this.StInkStatus = function(){
		return obj.StInkStatus;
	};

	this.StMediaStatus = function(){
		return obj.StMediaStatus;
	};

	this.StPaperStatus = function(){
		return obj.StPaperStatus;
	};

	this.CpCanEject = function(){
		return obj.CpCanEject;
	};

	this.CpCanCapture = function(){
		return obj.CpCanCapture;
	};

	this.CpCanStack = function(){
		return obj.CpCanStack;
	};

	this.CpCanDetectMediaTaken = function(){
		return obj.CpCanDetectMediaTaken;
	};

	this.FormName = function(){
		return obj.FormName;
	};

	this.FormBase = function(){
		return obj.FormBase;
	};

	this.FormUnitX = function(){
		return obj.FormUnitX;
	};

	this.FormUnitY = function(){
		return obj.FormUnitY;
	};

	this.FormWidth = function(){
		return obj.FormWidth;
	};

	this.FormHeight = function(){
		return obj.FormHeight;
	};

	this.FormAlignment = function(){
		return obj.FormAlignment;
	};

	this.FormOrientation = function(){
		return obj.FormOrientation;
	};

	this.FormOffsetX = function(){
		return obj.FormOffsetX;
	};

	this.FormOffsetY = function(){
		return obj.FormOffsetY;
	};

	this.FormVersionMajor = function(){
		return obj.FormVersionMajor;
	};

	this.FormVersionMinor = function(){
		return obj.FormVersionMinor;
	};

	this.FormUserPrompt = function(){
		return obj.FormUserPrompt;
	};

	this.FormFields = function(){
		return obj.FormFields;
	};

	this.FormFieldsIndexCount = function(){
		return obj.FormFieldsIndexCount;
	};

	this.FormFieldsType = function(){
		return obj.FormFieldsType;
	};

	this.FormFieldsClass = function(){
		return obj.FormFieldsClass;
	};

	this.FormFieldsAccess = function(){
		return obj.FormFieldsAccess;
	};

	this.FormFieldsOverflow = function(){
		return obj.FormFieldsOverflow;
	};

	this.FormFieldsInitialValue = function(){
		return obj.FormFieldsInitialValue;
	};

	this.FormFieldsFormat = function(){
		return obj.FormFieldsFormat;
	};

	this.MediaName = function(){
		return obj.MediaName;
	};

	this.MediaType = function(){
		return obj.MediaType;
	};

	this.MediaBase = function(){
		return obj.MediaBase;
	};

	this.MediaUnitX = function(){
		return obj.MediaUnitX;
	};

	this.MediaUnitY = function(){
		return obj.MediaUnitY;
	};

	this.MediaSizeWidth = function(){
		return obj.MediaSizeWidth;
	};

	this.MediaSizeHeight = function(){
		return obj.MediaSizeHeight;
	};

	this.MediaPageCount = function(){
		return obj.MediaPageCount;
	};

	this.MediaLineCount = function(){
		return obj.MediaLineCount;
	};

	this.MediaPrintAreaX = function(){
		return obj.MediaPrintAreaX;
	};

	this.MediaPrintAreaY = function(){
		return obj.MediaPrintAreaY;
	};

	this.MediaPrintAreaWidth = function(){
		return obj.MediaPrintAreaWidth;
	};

	this.MediaPrintAreaHeight = function(){
		return obj.MediaPrintAreaHeight;
	};

	this.MediaRestrictedAreaX = function(){
		return obj.MediaRestrictedAreaX;
	};

	this.MediaRestrictedAreaY = function(){
		return obj.MediaRestrictedAreaY;
	};

	this.MediaRestrictedAreaWidth = function(){
		return obj.MediaRestrictedAreaWidth;
	};

	this.MediaRestrictedAreaHeight = function(){
		return obj.MediaRestrictedAreaHeight;
	};

	this.MediaStagger = function(){
		return obj.MediaStagger;
	};

	this.MediaFoldType = function(){
		return obj.MediaFoldType;
	};

};
})();

