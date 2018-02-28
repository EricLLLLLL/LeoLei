/**
 * Created by Administrator on 2017/1/6.
 */
(function(){
window.JSUSApi=function(obj){

    //event handler
	this.addEvent = function(event,handler){
		displayMessage(obj.id + "->attachEvent(" + event+ ")");
		return obj.attachEvent(event,handler);
	};
	this.removeEvent = function(event, handler){
		displayMessage(obj.id + "->detachEvent(" + event+ ")");
		return obj.detachEvent(event,handler);
	};

    this.RegisterMessage = function () {
        displayMessage(obj.id + "->RegisterMessage()");
        return obj.RegisterMessage();
    };

    this.UnRegisterMessage = function () {
        displayMessage(obj.id + "->UnRegisterMessage()");
        return obj.UnRegisterMessage();
    };
    
};
})();