#pragma once
#include <map>
#include "common.h"

class CJnlFormAnalyze
{
public:
    struct ERROR_INFO
    {
        int m_iRow;                                       //错误出现的行号
        std::string m_strInfo;                            //具体错误信息
        ERROR_INFO() : m_iRow(-1),m_strInfo("") {}
        ERROR_INFO(int i, std::string str) {m_iRow = i;m_strInfo = str;}
    };

    typedef std::vector<ERROR_INFO> errArr;
public:
    CJnlFormAnalyze(void);
    CJnlFormAnalyze(std::string strFilePathName);
    ~CJnlFormAnalyze(void);

    void SetFilePathName(std::string strFilePathName);       //设置 FORM 路径名
    int Analyze(strArrArr &formInfo);                        //解析 FORM （词法检测和语法检查）
    std::string GetErrorInfo();                              //得到错误信息
private:
    int N_ReadFile();                                        //读文件，将文件内容保存在m_vecFile中
    int N_WordCheck();                                       //词法检测
    int N_GrammarCheck();                                    //语法检测
    bool N_IsAnnotation(std::string str);                    //str语句是否是注释
    bool N_IsSpace(std::string str);                         //str语句是否是空白字符串

    int S_Start(strArr::iterator it);
    int S_Form(strArr::iterator it);
    int S_Form_Begin(strArr::iterator it);
    int S_Form_Begin_Field(strArr::iterator it);
    int S_Form_Begin_Field_Begin(strArr::iterator it);
    int S_Form_Begin_Field_Begin_End(strArr::iterator it);
    int S_End(strArr::iterator it);
    int S_Position(strArr::iterator it);
    int S_Size(strArr::iterator it);
    int S_Class(strArr::iterator it);
    int S_Value(strArr::iterator it);

    int M_Form(std::string str);
    int M_Field(std::string str);
    int M_Begin(std::string str);
    int M_End(std::string str);
    int M_Position(std::string str);
    int M_Size(std::string str);
    int M_Class(std::string str);
    int M_Value(std::string str);
    int M_Horizontal(std::string str);
private:
    std::string m_strFilePathName;          //文件路径名

    strArr::iterator m_it_begin_form;
    strArr::iterator m_it_end_form;
    strArr m_vecFile;                       //form文件内容
    strArrArr m_ArrArrForm;                 //所有合法的form文件内容

    errArr m_ErrInfo;                       //错误信息

    bool m_bPosition;                       //是否有 POSITION 关键字
    bool m_bSize;                           //是否有 SIZE 关键字
    bool m_bClass;                          //是否有 CLASS 关键字
    bool m_bValue;                          //是否有 VALUE 关键字
};

