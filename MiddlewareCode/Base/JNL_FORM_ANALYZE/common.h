#pragma once

#include <vector>
#include <string>
typedef std::vector<std::string> strArr;
typedef std::vector<strArr> strArrArr;


//FORM 关键字
#define JNL_FORM                "FORM"
#define JNL_FIELD               "FIELD"
#define JNL_BEGIN               "BEGIN"
#define JNL_END                 "END"
#define JNL_POSITION            "POSITION"
#define JNL_SIZE                "SIZE"
#define JNL_CLASS               "CLASS"
#define JNL_CLASS_STATIC        "STATIC"
#define JNL_CLASS_OPTIONAL      "OPTIONAL"
#define JNL_VALUE               "VALUE"
#define JNL_HORIZONTAL          "HORIZONTAL"
#define JNL_HORIZONTAL_LEFT     "LEFT"
#define JNL_HORIZONTAL_CENTER   "CENTER"
#define JNL_HORIZONTAL_RIGHT    "RIGHT"


//正则表达式
#define REGEX_PATTERN_FORM          "\\s*(FORM|FIELD)\\s+\"(\\w+)\"\\s*(//.*)?"
#define REGEX_PATTERN_BEGIN         "\\s*(BEGIN|END)\\s*(//.*)?"
#define REGEX_PATTERN_POSITION      "\\s*(POSITION)\\s+(\\d{1,3})(\\s*,\\s*)(\\d{1,3})\\s*(//.*)?"
#define REGEX_PATTERN_SIZE          "\\s*(SIZE)\\s+(\\d{1,3})\\s*(//.*)?"
#define REGEX_PATTERN_CLASS         "\\s*(CLASS)\\s+(STATIC|OPTIONAL)\\s*(//.*)?"
#define REGEX_PATTERN_VALUE         "\\s*(VALUE)\\s+\"(.+)\"\\s*(//.*)?"
#define REGEX_PATTERN_HORIZONTAL    "\\s*(HORIZONTAL)\\s+(LEFT|CENTER|RIGHT)\\s*(//.*)?"


//错误码
#define SUCCESS                      0      //成功
#define OPEN_FILE_FAIL              -1      //打开文件失败                  
#define INVALID_STATEMENT           -2      //不可识别的语句
#define NOT_FIND_FORM               -3      //没有找到合格的 FORM
#define ABNORMAL_END                -4      //异常结束
#define NOT_FIND_VALUE              -5      //没有找到相应的字段名
