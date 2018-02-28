//
//���ܺ����ļ�
//
//�ж�����ʱ���Ƿ���Ч����ʽYYYY-MM-DD-HH-MM-SS
function CheckDateTimeS(str) {
    var reg = /^(\d+)-(\d{1,2})-(\d{1,2})-(\d{1,2})-(\d{1,2})-(\d{1,2})$/;
    var r = str.match(reg);
    if (r == null) return false;
    r[2] = r[2] - 1;
    var d = new Date(r[1], r[2], r[3], r[4], r[5], r[6]);
    if (d.getFullYear() != r[1]) return false;
    if (d.getMonth() != r[2]) return false;
    if (d.getDate() != r[3]) return false;
    if (d.getHours() != r[4]) return false;
    if (d.getMinutes() != r[5]) return false;
    if (d.getSeconds() != r[6]) return false;
    return true;
}
//�ж�����ʱ���Ƿ���Ч����ʽYYYY-MM-DD-HH-MM
function CheckDateTime(str) {
    var reg = /^(\d+)-(\d{1,2})-(\d{1,2})-(\d{1,2})-(\d{1,2})$/;
    var r = str.match(reg);
    if (r == null) return false;
    r[2] = r[2] - 1;
    var d = new Date(r[1], r[2], r[3], r[4], r[5]);
    if (d.getFullYear() != r[1]) return false;
    if (d.getMonth() != r[2]) return false;
    if (d.getDate() != r[3]) return false;
    if (d.getHours() != r[4]) return false;
    if (d.getMinutes() != r[5]) return false;
    return true;
}
//�ж������Ƿ���Ч����ʽYYYY-MM-DD
function CheckDate(str) {
    var reg = /^(\d+)-(\d{1,2})-(\d{1,2})$/;
    var r = str.match(reg);
    if (r == null) return false;
    r[2] = r[2] - 1;
    var d = new Date(r[1], r[2], r[3]);
    if (d.getFullYear() != r[1]) return false;
    if (d.getMonth() != r[2]) return false;
    if (d.getDate() != r[3]) return false;
    return true;
}

function getHtmlFileName() {
    var a = location.href;
    var b = a.split("/");
    var c = b.slice(b.length - 1, b.length).toString(String).split(".");
    return c.slice(0, 1);
}

function GetDate12byte() {
    var Datetime = new Date();
    return Datetime.dateToString() + Datetime.timeToString();
}

//�������ܣ��ַ���תHex
function stringToHex(str) {
    var hexArray = new Array();
    for (var index = 0; index < str.length; index = index + 2) {
        var i = index / 2;
        hexArray[i] = parseInt(str.substr(index, 2), 16);
    }
    return hexArray;
}

//�������ܣ��ַ����������Ϊ�ַ�����
function stringXORstring(stringA, stringB, stringC, stringLen) {
    var DesString = '';
    var tmpDesString = '';
    for (var index = 0; index < stringLen; index++) {
        tmpDesString = parseInt(stringA.substr(index, 1), 16) ^ parseInt(stringB.substr(index, 1), 16) ^ parseInt(stringC.substr(index, 1), 16);
        if (tmpDesString < 10) {
            DesString = DesString + String.fromCharCode(tmpDesString + 48);
        } else {
            DesString = DesString + String.fromCharCode(tmpDesString + 55);
        }
    }
    return DesString;
}

//��ȡ��ҳ�ɼ������ document.body.offsetWidth (�������ߵĿ�)
function getWebWidth(percent) {
    if (arguments.length != 0)
        return parseInt(document.body.offsetWidth * percent);
    else
        return document.body.offsetWidth;
}
//��ȡ��ҳ�ɼ�����ߣ� document.body.offsetHeight (�������ߵĸ�)
function getWebHeight(percent) {
    if (arguments.length != 0)
        return parseInt(document.body.offsetHeight * percent);
    else
        return document.body.offsetHeight;
}


//��ȡ�����ĺ�������
function getFnName(fn) {
    var tmp = fn.toString();
    var re = /function\s*(\w*)/i;
    var matches = re.exec(tmp);
    return matches[1];
}

///----------start-------------
///��jsԭʼ����������չ��������
///----------start-------------


//��ȡ��ǰ�����ַ��� 
Date.prototype.dateToString = function (separator) {
    var s = separator || '';
    var AddZero = ['00', '01', '02', '03', '04', '05', '06', '07', '08', '09'];
    return this.getFullYear() + s + (AddZero[this.getMonth() + 1] || (this.getMonth() + 1)) + s + (AddZero[this.getDate()] || this.getDate());
};
//��ȡ��ʱ�����ַ��� 
Date.prototype.timeToString = function (separator) {
    var s = separator || '';
    var AddZero = ['00', '01', '02', '03', '04', '05', '06', '07', '08', '09'];
    return (AddZero[this.getHours()] || this.getHours()) + s + (AddZero[this.getMinutes()] || this.getMinutes()) + s + (AddZero[this.getSeconds()] || this.getSeconds());
};
//����������չ����bind/*���ie8��չ*/
if (!Function.prototype.bind) {
    Function.prototype.bind = function (obj) {
        var aArgs = Array.prototype.slice.call(arguments, 1),
            fToBind = this,
            fNOP = function () {},
            fBound = function () {
                return fToBind.apply(this instanceof fNOP && obj ? this : obj, aArgs.concat(Array.prototype.slice.call(arguments)));
            };
        fNOP.prototype = this.prototype;
        fBound.prototype = new fNOP();
        return fBound;
    }
}

//Unicode -> ASCIIת��
function UnicodeToASCII(str) {
    var strArray = new Array(); //���unicode�ַ�
    if (str == '') {
        return;
    }
    var text = "";
    var firstPosition = 0;
    var lastPosition = 0;
    var flag = false;
    for (var i = 0; i < str.length; i++) {
        var ch = str.charAt(i); //ȡ�ַ�
        if (ch == '&') { //unicode begin
            flag = true;
            firstPosition = i;
        } else if (ch == ';') {
            lastPosition = i;
            strArray[strArray.length] = str.substring(firstPosition, lastPosition + 1);
            var s = str.substring(firstPosition, lastPosition + 1);
            //alert(str.substring(firstPosition,lastPosition+1));
            //alert(firstPosition + "==" + lastPosition);
            text += String.fromCharCode(s.replace(/[&#;]/g, ''));
            flag = false;
        } else {
            if (flag) {

            } else { //��Unicode�ַ�����
                strArray[strArray.length] = ch;
                text += ch;
            }
        }
    }
    // for(var i=0;i<strArray.length;i++){
    //  alert(strArray[i]);
    // }
    return text;
}

//ת������Ҵ�д�����ʽ   
function cmycurd(num) {
    var str1 = '��Ҽ��������½��ƾ�'; //0-9����Ӧ�ĺ���   
    var str2 = '��Ǫ��ʰ��Ǫ��ʰ��Ǫ��ʰԪ�Ƿ�'; //����λ����Ӧ�ĺ���   
    var str3; //��ԭnumֵ��ȡ����ֵ   
    var str4; //���ֵ��ַ�����ʽ   
    var str5 = ''; //����Ҵ�д�����ʽ   
    var i; //ѭ������   
    var j; //num��ֵ����100���ַ�������   
    var ch1; //���ֵĺ������   
    var ch2; //����λ�ĺ��ֶ���   
    var nzero = 0; //����������������ֵ�Ǽ���   
    num = num.replace(",", ""); //�Ѷ����滻  
    num = Math.abs(num).toFixed(2); //��numȡ����ֵ����������ȡ2λС��   
    str4 = (num * 100).toFixed(0).toString(); //��num��100��ת�����ַ�����ʽ   
    j = str4.length; //�ҳ����λ   
    str2 = str2.substr(15 - j); //ȡ����Ӧλ����str2��ֵ���磺200.55,jΪ5����str2=��ʰԪ�Ƿ�   

    //ѭ��ȡ��ÿһλ��Ҫת����ֵ   
    for (i = 0; i < j; i++) {
        str3 = str4.substr(i, 1); //ȡ����ת����ĳһλ��ֵ   
        if (i != (j - 3) && i != (j - 7) && i != (j - 11) && i != (j - 15)) { //����ȡλ����ΪԪ�����ڡ������ϵ�����ʱ   
            if (str3 == '0') {
                ch1 = '';
                ch2 = '';
                nzero = nzero + 1;
            } else {
                if (str3 != '0' && nzero != 0) {
                    ch1 = '��' + str1.substr(str3 * 1, 1);
                    ch2 = str2.substr(i, 1);
                    nzero = 0;
                } else {
                    ch1 = str1.substr(str3 * 1, 1);
                    ch2 = str2.substr(i, 1);
                    nzero = 0;
                }
            }
        } else { //��λ�����ڣ��ڣ���Ԫλ�ȹؼ�λ   
            if (str3 != '0' && nzero != 0) {
                ch1 = "��" + str1.substr(str3 * 1, 1);
                ch2 = str2.substr(i, 1);
                nzero = 0;
            } else {
                if (str3 != '0' && nzero == 0) {
                    ch1 = str1.substr(str3 * 1, 1);
                    ch2 = str2.substr(i, 1);
                    nzero = 0;
                } else {
                    if (str3 == '0' && nzero >= 3) {
                        ch1 = '';
                        ch2 = '';
                        nzero = nzero + 1;
                    } else {
                        if (j >= 11) {
                            ch1 = '';
                            nzero = nzero + 1;
                        } else {
                            ch1 = '';
                            ch2 = str2.substr(i, 1);
                            nzero = nzero + 1;
                        }
                    }
                }
            }
        }
        if (i == (j - 11) || i == (j - 3)) { //�����λ����λ��Ԫλ�������д��   
            ch2 = str2.substr(i, 1);
        }
        str5 = str5 + ch1 + ch2;

        if (i == j - 1 && str3 == '0') { //���һλ���֣�Ϊ0ʱ�����ϡ�����   
            //str5 = str5 + '��';
        }
    }
    if (num == 0) {
        //str5 = '��Ԫ��';
        str5 = '';
    }
    return str5;
}
//ÿ��Num�����ַ�Char
function InsertChar(InData, Num, Char) {
    var OutData = "";
    var nlength = InData.length
    var nRslt = Math.floor(nlength / Num);
    var ResetNum = nlength % Num;
    if (ResetNum == 0) {
        nRslt--;
        ResetNum = Num;
    }
    OutData = InData.substr(0, ResetNum)
    while (nRslt > 0) {
        OutData += Char + InData.substr((nlength - nRslt * Num), Num)
        nRslt--;
    }
    return OutData;
};

// �������� by Gni
function changeName(name) {
    name = name.split("");
    name.splice(0, 1, "*");
    name = name.join("");
    // alert(name);
    return name;
};
// ���ο��� by Gni
function changeCardNum(cardNum) {
    cardNum = cardNum.split("");
    cardNum.splice(cardNum.length - 5, 4, "****");
    cardNum = cardNum.join("");
    // alert(cardNum);
    return cardNum;
};

/**���ư�ť�Ƿ���Ե�� �ķ��� B */

var BtnIdArr = []; // �洢 ��Ҫ���Ƶİ�ťid������   ���Ա��浽ȫ�ֱ�����ȡ��JSGlobal
// ���Ե��
this.BtnEnable = function (BtnIdArr) {
    for (let index = 0; index < BtnIdArr.length; index++) {
        const element = BtnIdArr[index];
        document.getElementById(element).disabled = false;
    }
};
// ���� ��� ʧЧ
function BtnDisable(BtnIdArr) {
    for (let index = 0; index < BtnIdArr.length; index++) {
        const element = BtnIdArr[index];
        document.getElementById(element).disabled = true;
    }
};
/*
// ��ǰ js ҳ��Ĵ��� 
BtnIdArr=['DEP','PWD','CWD','OrderCWD','INQ','Exit','Transfer'];  //��Ҫ���Ƶİ�ť��id
BtnEnable (BtnIdArr);  // ���Ե��
BtnDisable (BtnIdArr);// ʧЧ ���ܵ��
*/
/**���ư�ť�Ƿ���Ե�� �ķ��� E */