(function () {

    window.ImeHM = function () {
        var cursurPosition = 0;
        var inputObj;
        var inputData;
        //����궨λ����������λ��
        this.cursorEnd = function (inputId) {
            inputObj = document.getElementById(inputId);
            inputObj.focus();
            var r = inputObj.createTextRange();
            r.moveStart('character', inputObj.value.length);
            r.collapse(true);
            r.select();
        }
        //��ȡ��ǰ�������λ��
        this.getCurPosition = function (inputId) {
            inputObj = document.getElementById(inputId);
            var range = document.selection.createRange();
            range.moveStart("character", -inputObj.value.length);
            cursurPosition = range.text.length;
        };

        this.getInput = function (inputId, type, value) {
            inputObj = document.getElementById(inputId);
            inputData = document.getElementById(inputId).value;
            //���ܼ�  CLEAR BS CANCEL CONFIRM
            if (type == 1) {
                if (value == "CLEAR") {
                    inputObj.value = "";
                }
                if (value == "BS") {
                    bs(inputObj);
                }
            }
            //ֵ��
            if (type == 0) {
                //inputData += value;
                //inputObj.value = inputData;
                insertCharacter(inputObj, value);
            }

        };

        // �����ַ�
        var insertCharacter = function (inputObj, character) {
            inputData = inputObj.value;
            var textlen = inputObj.value.length;
            var str = inputData.substring(0, cursurPosition) + character + inputData.substring(cursurPosition, inputData.length);//insert_flg(inputData, 'M', cursurPosition);
            inputObj.value = str;
            if (cursurPosition <= textlen) {
                cursurPosition = cursurPosition + 1;
            }
            setCaretPosition(inputObj, cursurPosition);
        }

        //���ù��λ��
        var setCaretPosition = function (ctrl, pos) {
            //���ù��λ�ú���
            if (ctrl.setSelectionRange) {
                ctrl.focus();
                ctrl.setSelectionRange(pos, pos);
            } else if (ctrl.createTextRange) {
                var range = ctrl.createTextRange();
                range.collapse(true);
                range.moveEnd('character', pos);
                range.moveStart('character', pos);
                range.select();
            }
        }

        //�˸�
        var bs = function (inputObj) {
            inputData = inputObj.value;
            var textlen = inputObj.value.length;
			if(textlen==1){
				inputObj.value = "";
			}else{
				inputData = inputData.substring(0, cursurPosition - 1) + inputData.substring(cursurPosition, inputData.length);
				inputObj.value = inputData;
			}
            if (cursurPosition > 0) {
                cursurPosition = cursurPosition - 1;
            }
            setCaretPosition(inputObj, cursurPosition);
        }

    }
})();

