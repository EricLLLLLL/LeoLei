#include "JnlForm.h"
#include <sstream>
#include <regex>


/////////////////////////////////////////////////////////////////////////////////////////////////
//class: CJnlForm
/////////////////////////////////////////////////////////////////////////////////////////////////

CJnlForm::CJnlForm(void) 
    : m_strFormName("")
{
}


CJnlForm::~CJnlForm(void)
{
}

int CJnlForm::Translate(const strArr & saFormInfo)
{
    int ret = SUCCESS;

    m_Fields.clear();

    std::stringstream ss;
    std::string str("");

    for(strArr::const_iterator cit = saFormInfo.begin();cit != saFormInfo.end();cit++)
    {
        ss.str("");
        ss.clear();

        ss<<*cit;
        while(ss>>str)
        {
            if(JNL_FORM == str)
            {
                ret = N_GetFormName(*cit);
                break;
            }
            else if(JNL_FIELD == str)
            {
                strArr::const_iterator cit_begin = cit;
                strArr::const_iterator cit_end = cit_begin;

                //得到 Field 区间
                for(++cit;cit != saFormInfo.end();cit++)
                {
                    ss.str("");
                    ss.clear();

                    ss<<*cit;
                    ss>>str;
                    if(JNL_END == str)
                    {
                        cit_end = cit;
                        break;
                    }
                }

                ret = N_GetField(strArr(cit_begin, cit_end + 1));
                break;
            }
        }
    }

    std::sort(m_Fields.begin(), m_Fields.end());

    return ret;
}


std::string CJnlForm::Format(std::string strDataIn)
{
    std::string strRet("");

    int maxCol = 0;

    for(std::vector<CJnlField>::iterator it = m_Fields.begin();it != m_Fields.end();it++)
    {
        if(maxCol < (it->GetPosCol() + it->GetSize()))
        {
            maxCol = it->GetPosCol() + it->GetSize();
        }
    }

    std::string* lp_str = nullptr;
    int iRow = m_Fields.rbegin()->GetPosRow() + 1;

    lp_str = new std::string[iRow];

    for(int i = 0;i < iRow;i++)
    {
        lp_str[i] = std::string(maxCol, 0x20);
    }

    for(std::vector<CJnlField>::iterator it = m_Fields.begin();it != m_Fields.end();it++)
    {
        it->Format(strDataIn, lp_str[it->GetPosRow()]);
    }

    for(int i = 0;i < iRow - 1;i++)
    {
        strRet += lp_str[i] + "\n";
    }
    strRet += lp_str[iRow - 1];

    delete[] lp_str;

    return strRet;
}


int CJnlForm::N_GetFormName(const std::string str)
{
    int ret = INVALID_STATEMENT;

    const std::regex pattern_form(REGEX_PATTERN_FORM);
    std::match_results<std::string::const_iterator> result;
    bool bMatch = std::regex_match(str,result,pattern_form);
    if(bMatch)
    {
        m_strFormName = result[2];
        ret = SUCCESS;
    }

    return ret;
}


int CJnlForm::N_GetField(const strArr & saFieldInfo)
{
    int ret = SUCCESS;

    CJnlField field;

    ret = field.Translate(saFieldInfo);
    if(SUCCESS == ret)
    {
        if(!N_IsExist(field))
        {
            m_Fields.push_back(field);
        }
    }

    return ret;
}


bool CJnlForm::N_IsExist(const CJnlField &f)
{
    for(std::vector<CJnlField>::const_iterator cit = m_Fields.begin();cit != m_Fields.end();cit++)
    {
        if(f == *cit)
        {
            return true;
        }
    }
    return false;
}


bool operator==(const CJnlForm &j1, const CJnlForm &j2)
{
    return (j1.m_strFormName == j2.m_strFormName);
}


std::string CJnlForm::GetFormName() const
{
    return m_strFormName;
}



/////////////////////////////////////////////////////////////////////////////////////////////////
//class: CJnlField
/////////////////////////////////////////////////////////////////////////////////////////////////

CJnlField::CJnlField(void)
    : m_strFieldName("")
    , m_strHorizontal(JNL_HORIZONTAL_LEFT)
    , m_strValue("")
    , m_strClass("")
    , m_Size(-1)
    , m_Pos_Row(-1)
    , m_Pos_Col(-1)
{
}


CJnlField::~CJnlField(void)
{
}


int CJnlField::Translate(const strArr & saFieldInfo)
{
    int ret = SUCCESS;

    std::stringstream ss;
    std::string str("");

    for(strArr::const_iterator cit = saFieldInfo.begin();cit != saFieldInfo.end();cit++)
    {
        ss.clear();
        ss.str("");

        ss<<*cit;
        while(ss>>str)
        {
            if(JNL_FIELD == str)
            {
                ret = N_GetFieldName(*cit);
                break;
            }
            else if(JNL_POSITION == str)
            {
                ret = N_GetPosition(*cit);
                break;
            }
            else if(JNL_SIZE == str)
            {
                ret = N_GetSize(*cit);
                break;
            }
            else if(JNL_CLASS == str)
            {
                ret = N_GetClass(*cit);
                break;
            }
            else if(JNL_VALUE == str)
            {
                ret = N_GetValue(*cit);
                break;
            }
            else if(JNL_HORIZONTAL == str)
            {
                ret = N_GetHorizontal(*cit);
                break;
            }
        }
    }

    return ret;
}


int CJnlField::Format(const std::string strDataIn, std::string &strDataOut)
{
    int ret = SUCCESS;

    std::string str(m_Size,0x20);
    std::string strValue("");

    if(JNL_CLASS_STATIC == m_strClass)
    {
        strValue = m_strValue;
    }
    else
    {
        //std::string strPatt = ".*" + m_strValue + "\\s*" + "=\\s*([^,]+)(,.*)?";
        std::string strPatt = "(" + m_strValue + "|.+,\\s*" + m_strValue + ")\\s*" + "=\\s*([^,]+)(,.*)?";
        const std::regex patt(strPatt);
        std::match_results<std::string::const_iterator> result;
        bool bMatch = std::regex_match(strDataIn, result, patt);
        if(bMatch)
        {
            strValue = result[2];
        }
        else
        {
            return NOT_FIND_VALUE;
        }
    }

    if(strValue.length() > m_Size)              //字符串长度大于指定域的大小
    {
        strValue = strValue.substr(0,m_Size);   //截断
    }

    if(JNL_HORIZONTAL_LEFT == m_strHorizontal)
    {
        str.replace(0, strValue.length(), strValue);
    }
    else if(JNL_HORIZONTAL_CENTER == m_strHorizontal)
    {
        int pos = (str.length() - strValue.length())/2;
        str.replace(pos, strValue.length(), strValue);
    }
    else
    {
        str.replace(str.length() - strValue.length(), strValue.length(), strValue);
    }

    strDataOut.replace(m_Pos_Col, str.length(), str);

    return ret;
}


int CJnlField::N_GetFieldName(const std::string str)
{
    int ret = INVALID_STATEMENT;

    const std::regex pattern_form(REGEX_PATTERN_FORM);
    std::match_results<std::string::const_iterator> result;
    bool bMatch = std::regex_match(str,result,pattern_form);
    if(bMatch)
    {
        m_strFieldName = result[2];
        ret = SUCCESS;
    }

    return ret;
}


int CJnlField::N_GetPosition(const std::string str)
{
    int ret = INVALID_STATEMENT;
    std::string strRow("");
    std::string strCol("");

    const std::regex pattern_position(REGEX_PATTERN_POSITION);
    std::match_results<std::string::const_iterator> result;
    bool bMatch = std::regex_match(str, result, pattern_position);
    if(bMatch)
    {
        strRow = result[2];
        strCol = result[4];

        m_Pos_Row = atoi(strRow.c_str());
        m_Pos_Col = atoi(strCol.c_str());
        ret = SUCCESS;
    }

    return ret;
}


int CJnlField::N_GetSize(const std::string str)
{
    int ret = INVALID_STATEMENT;
    std::string strSize("");

    const std::regex pattern_size(REGEX_PATTERN_SIZE);
    std::match_results<std::string::const_iterator> result;
    bool bMatch = std::regex_match(str, result, pattern_size);
    if(bMatch)
    {
        strSize = result[2];
        m_Size = atoi(strSize.c_str());
        ret = SUCCESS;
    }

    return ret;
}


int CJnlField::N_GetClass(const std::string str)
{
    int ret = INVALID_STATEMENT;

    const std::regex pattern_class(REGEX_PATTERN_CLASS);
    std::match_results<std::string::const_iterator> result;
    bool bMatch = std::regex_match(str, result, pattern_class);
    if(bMatch)
    {
        m_strClass = result[2];
        ret = SUCCESS;
    }

    return ret;
}


int CJnlField::N_GetValue(const std::string str)
{
    int ret = INVALID_STATEMENT;

    const std::regex pattern_value(REGEX_PATTERN_VALUE);
    std::match_results<std::string::const_iterator> result;
    bool bMatch = std::regex_match(str, result, pattern_value);
    if(bMatch)
    {
        m_strValue = result[2];
        ret = SUCCESS;
    }

    return ret;
}


int CJnlField::N_GetHorizontal(const std::string str)
{
    int ret = INVALID_STATEMENT;

    const std::regex pattern_horizontal(REGEX_PATTERN_HORIZONTAL);
    std::match_results<std::string::const_iterator> result;
    bool bMatch = std::regex_match(str, result, pattern_horizontal);
    if(bMatch)
    {
        m_strHorizontal = result[2];
        ret = SUCCESS;
    }

    return ret;
}


bool operator==(const CJnlField &j1, const CJnlField &j2)
{
    return (j1.m_strFieldName == j2.m_strFieldName);
}


bool operator<(const CJnlField &j1, const CJnlField &j2)
{
    if(j1.m_Pos_Row < j2.m_Pos_Row)
    {
        return true;
    }
    else if(j1.m_Pos_Row == j2.m_Pos_Row)
    {
        return (j1.m_Pos_Col < j2.m_Pos_Col);
    }
    else 
    {
        return false;
    }
}


int CJnlField::GetPosRow() const
{
    return m_Pos_Row;
}


int CJnlField::GetPosCol() const
{
    return m_Pos_Col;
}


int CJnlField::GetSize() const
{
    return m_Size;
}
