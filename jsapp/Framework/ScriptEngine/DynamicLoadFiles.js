(function () {

	window.dynamicLoadFiles = function () {

		function enumBrowserType() {
			var Sys = {};
			var ua = navigator.userAgent.toLowerCase();
			var s;
			var strType = "";

			(s = ua.match(/msie ([\d.]+)/)) ? Sys.ie = s[1]:
				(s = ua.match(/firefox\/([\d.]+)/)) ? Sys.firefox = s[1] :
				(s = ua.match(/chrome\/([\d.]+)/)) ? Sys.chrome = s[1] :
				(s = ua.match(/opera.([\d.]+)/)) ? Sys.opera = s[1] :
				(s = ua.match(/version\/([\d.]+).*safari/)) ? Sys.safari = s[1] : 0;

			if (Sys.ie) {
				strType = "IE";
			} else if (Sys.firefox) {
				strType = "Firefox";
			} else if (Sys.chrome) {
				strType = "Chrome";
			} else if (Sys.opera) {
				strType = "Opera";
			} else if (Sys.safari) {
				strType = "Safari";
			}

			return strType;
		}

		this.browserType = enumBrowserType();

		this.parseXml = function (xml) {
			var xmldom = null;

			switch (this.browserType) {
				case 'IE':
					xmldom = new ActiveXObject("Microsoft.XMLDOM");
					xmldom.load(xml);
					if (xmldom.parseError != 0) {
						throw new Error("XML parsing error: " + xmldom.parseError.reason);
					}
					break;
				case 'Firefox':
					xmldom = document.implementation.createDocument("", "", null);
					xmldom.async = false;
					xmldom.load(xml);
					if (xmldom.documentElement.nodeName == "parsererror") {
						throw new Error("XML parsing error: " + xmldom.documentElement.childNodes[0].nodeValue);
					}
					break;
				case 'Chrome':
					var xmlhttp = new XMLHttpRequest();
					xmlhttp.open("GET", xml, false);
					xmlhttp.setRequestHeader('Content-Type', 'text/xml');
					xmlhttp.send();
					xmldom = xmlhttp.responseXML.documentElement;
					// xmldom=xmlhttp.responseXML;
					break;
				default:
					break;
			}

			return xmldom;
		};

		this.css = function (path) {
			if (!path || path.length === 0) {
				throw new Error('argument "path" is required !');
			}
			var head = document.getElementsByTagName('head')[0];
			var link = document.createElement('link');
			link.href = path;
			link.rel = 'stylesheet';
			link.type = 'text/css';
			head.appendChild(link);
		};

		this.js = function (path) {
			if (!path || path.length === 0) {
				throw new Error('argument "path" is required !');
			}
			var head = document.getElementsByTagName('head')[0];
			var script = document.createElement('script');
			script.type = 'text/javascript';
			script.onload = script.onreadystatechange = function () {
				if (!this.readyState || this.readyState === "loaded" || this.readyState === "complete") {
					// Handle memory leak in IE 
					script.onload = script.onreadystatechange = null;
				}
			};
			script.src = path;
			head.appendChild(script);
		};

		this.xml = function (path) {
			var xmldom = null;
			try {
				xmldom = this.parseXml(path);
				var xmldata;

				switch (this.browserType) {
					case 'IE':
						xmldata = xmldom.xml;
						break;
					case 'Firefox':
						var serializer = new XMLSerializer();
						xmldata = serializer.serializeToString(xmldom);
						break;
					case 'Chrome':
						xmldata = xmldom.innerHTML;
						break;
					default:
						break;
				}

				var docObj = document.createElement('Newdiv');
				docObj.className = 'PageDiv';
				docObj.id = 'PageId';
				docObj.innerHTML = xmldata;
				//docObj.style.backgroundImage = "./Framework/style/Graphics/bg.jpg";
				document.body.appendChild(docObj); //???DIV????body??

				//$("#PageId").slideDown(500);

				//取消a链接
				var aTags = document.getElementsByTagName("a");
				for (var i = 0; i < aTags.length; i++) {
					if (aTags[i].hasAttribute("href"))
						aTags[i].removeAttribute("href");
				}

				//后屏管理时间显示
				var oCurrentTime = document.getElementById("LocalTime");
				if (oCurrentTime != null) {
					document.getElementById("LocalTime").innerText = "";
					var showTime = function () {
						var time = new Date();
						oCurrentTime.innerText = time.toLocaleString().replace(/??/, "-").replace(/??/, "-").replace(/??/, "");
					}
					setInterval(showTime, 1000);
				}
				var oSubBankId = document.getElementById("SubBankId");
				if (oSubBankId != null) {
					oSubBankId.innerText = top.API.gSubBankNum;
				}

				// 待机页面时间显示   星期三 13:40  下一行 2018年1月8日
				var oCurrentShowTimeIndex = document.getElementById("ShowTimeIndex");
				if (oCurrentShowTimeIndex != null) {
					document.getElementById("ShowTimeIndex").innerText = "";
					var showTime = function () {
						var time = new Date();
						oCurrentShowTimeIndex.innerText = "星期" + "日一二三四五六".split(/(?!\b)/)[time.getDay()] +' '+time.dateHMToString()+ ' ' +  time.dateYMDToString();
					}
					setInterval(showTime, 1000);
				}
				// 前屏时间显示  2018年1月8日 星期三 13:40
				var oCurrentShowTime = document.getElementById("ShowTime");
				if (oCurrentShowTime != null) {
					document.getElementById("ShowTime").innerText = "";
					var showTime = function () {
						var time = new Date();
						oCurrentShowTime.innerText = time.dateYMDToString() + ' ' +"星期" + "日一二三四五六".split(/(?!\b)/)[time.getDay()]+' ' +time.dateHMToString();
					}
					setInterval(showTime, 1000);
				}
				//获取当前日期字符串  2018年2月7日     by yangjl dateYMDToString
				Date.prototype.dateYMDToString = function (separator) {
					var s = separator || '';
					var AddZero = ['00', '01', '02', '03', '04', '05', '06', '07', '08', '09'];
					return this.getFullYear() + '年' + s + ' ' + +(AddZero[this.getMonth() + 1] || (this.getMonth() + 1)) + '月' + s + (AddZero[this.getDate()] || this.getDate()) + '日';
				};
				//获取当前日期字符串    15:33   by yangjl
				Date.prototype.dateHMToString = function (separator) {
					var s = separator || '';
					var AddZero = ['00', '01', '02', '03', '04', '05', '06', '07', '08', '09'];
					return (AddZero[this.getHours()] || this.getHours()) + ':' + (AddZero[this.getMinutes()] || this.getMinutes());
				};

				// 前屏网点机构号
				var oSubBankId = document.getElementById("numberDot");
				if (oSubBankId != null) {
					oSubBankId.innerText = top.API.gNumberDot;
				};
				// 前屏 设备号
				var oSubBankId = document.getElementById("numberDevice");
				if (oSubBankId != null) {
					oSubBankId.innerText = top.API.gNumberDevice;
				};

				// 错误提示倒计时
				var oCurrentCountDown = document.getElementById("countDown");
				if (oCurrentCountDown != null) {
					document.getElementById("countDown").innerText = "03";
					// setInterval(showTime, 1000);
				}


			} catch (ex) {
				alert(ex.message);
			}
		};

		this.InsertPlgin = function (path, InsertdivId, plginId) {
			var xmldom = null;
			try {
				xmldom = this.parseXml(path);
				var xmldata;

				switch (this.browserType) {
					case 'IE':
						xmldata = xmldom.xml;
						break;
					case 'Firefox':
						var serializer = new XMLSerializer();
						xmldata = serializer.serializeToString(xmldom);
						break;
					case 'Chrome':
						xmldata = xmldom.innerHTML;
						break;
					default:
						break;
				}

				var docObj = document.createElement('div');
				docObj.id = plginId;
				docObj.innerHTML = xmldata;
				//inserting the div which appeared in the html body
				document.getElementById(InsertdivId).appendChild(docObj);
			} catch (ex) {
				alert(ex.message);
			}
		};
	};

})();