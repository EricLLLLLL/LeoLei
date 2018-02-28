(function(){

window.Waiting = function(){

    this.show = function(){
    	   //get the page width and height
          var sWidth = document.body.scrollWidth;
          var sHeight = document.body.scrollHeight;

          var wHeight = document.documentElement.clientHeight;
          sHeight = wHeight;
          

          var oMask = document.createElement("div");
              oMask.id = "mask";
              oMask.style.height = sHeight + "px";
              oMask.style.width = sWidth + "px";
              document.body.appendChild(oMask);

          var  oWait = document.createElement("div");
              oWait.id = "wait";
              //oWait.innerHTML="<div class='waitCon'><div id='inform'></div><div id='ShowTime'></div></div>";
              oWait.innerHTML="<div class='waitCon'></div></div>";
              document.body.appendChild(oWait);

           var dHeight = oWait.offsetHeight;
           var dWidth = oWait.offsetWidth;

              oWait.style.left = sWidth/2-dWidth/2+"px";
              oWait.style.top = wHeight/2-dHeight/2+"px";
             // document.getElementById("inform").innerHTML = "You got it";
             // document.getElementById("ShowTime").innerHTML = "100";
    },
        this.showNoBankCard = function(){
            //get the page width and height
            var sWidth = document.body.scrollWidth;
            var sHeight = document.body.scrollHeight;

            var wHeight = document.documentElement.clientHeight;
            sHeight = wHeight;


            var oMask = document.createElement("div");
            oMask.id = "mask";
            oMask.style.height = sHeight + "px";
            oMask.style.width = sWidth + "px";
            document.body.appendChild(oMask);

            var  oWait = document.createElement("div");
            oWait.id = "wait";
            //oWait.innerHTML="<div class='waitCon'><div id='inform'></div><div id='ShowTime'></div></div>";
            oWait.innerHTML="<div class='NoBankCard'></div></div>";
            document.body.appendChild(oWait);

            var dHeight = oWait.offsetHeight;
            var dWidth = oWait.offsetWidth;

            oWait.style.left = sWidth/2-dWidth/2+"px";
            oWait.style.top = wHeight/2-dHeight/2+"px";
            // document.getElementById("inform").innerHTML = "You got it";
            // document.getElementById("ShowTime").innerHTML = "100";
        },

    this.disappear = function() {
    	var oWait = document.getElementById("wait");
    	var oMask = document.getElementById("mask");
    	if( oWait != null ){
       	    document.body.removeChild(oWait);
    	}
    	if( oMask != null ){
    		document.body.removeChild(oMask);
    	}
    }
};

})();

