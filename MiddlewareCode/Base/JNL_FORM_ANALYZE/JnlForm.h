#pragma once
#include "common.h"

/////////////////////////////////////////////////////////////////////////////////////////////////
//class: CJnlForm
/////////////////////////////////////////////////////////////////////////////////////////////////

class CJnlField;

class CJnlForm
{
public:
    CJnlForm(void);
    virtual ~CJnlForm(void);

    std::string GetFormName() const;

    int Translate(const strArr & saFormInfo);

    std::string Format(std::string strDataIn);

    friend bool operator==(const CJnlForm &j1, const CJnlForm &j2);
private:
    int N_GetFormName(const std::string str);
    int N_GetField(const strArr & saFieldInfo);
    bool N_IsExist(const CJnlField &f);             //是否存在同名的 FIELD
private:     
    std::string m_strFormName;                      //Form 名
    std::vector<CJnlField> m_Fields;                //Form 所包含的域
};


/////////////////////////////////////////////////////////////////////////////////////////////////
//class: CJnlField
/////////////////////////////////////////////////////////////////////////////////////////////////
class CJnlField
{
public:
    CJnlField(void);
    virtual ~CJnlField(void);
    int GetPosRow() const;
    int GetPosCol() const;
    int GetSize() const;

    int Translate(const strArr & saFieldInfo);
    int Format(const std::string strDataIn, std::string &strDataOut);
    friend bool operator==(const CJnlField &j1, const CJnlField &j2);
    friend bool operator<(const CJnlField &j1, const CJnlField &j2);
private:
    int N_GetFieldName(const std::string str);
    int N_GetPosition(const std::string str);
    int N_GetSize(const std::string str);
    int N_GetClass(const std::string str);
    int N_GetValue(const std::string str);
    int N_GetHorizontal(const std::string str);

    std::string m_strFieldName;         //域名
    std::string m_strClass;             //域的属性（常量或变量）
    std::string m_strValue;             //域的值
    std::string m_strHorizontal;        //对其方式
    int m_Pos_Row;                      //所在行
    int m_Pos_Col;                      //所在列（一个字符算一列）
    int m_Size;                         //域的大小
};


