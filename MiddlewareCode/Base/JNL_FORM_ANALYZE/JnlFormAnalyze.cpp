#include "JnlFormAnalyze.h"
#include <fstream>
#include <sstream>
#include <regex>


CJnlFormAnalyze::CJnlFormAnalyze(void) 
    : m_strFilePathName("")
    , m_bPosition(false)
    , m_bSize(false)
    , m_bClass(false)
    , m_bValue(false)
{
}


CJnlFormAnalyze::~CJnlFormAnalyze(void)
{
}


CJnlFormAnalyze::CJnlFormAnalyze(std::string strFilePathName) 
    : m_strFilePathName(strFilePathName)
    , m_bPosition(false)
    , m_bSize(false)
    , m_bClass(false)
    , m_bValue(false)
{
}


void CJnlFormAnalyze::SetFilePathName(std::string strFilePathName)
{
    m_strFilePathName = strFilePathName;
}


int CJnlFormAnalyze::Analyze(strArrArr &formInfo)
{
    m_vecFile.clear();
    m_ArrArrForm.clear();
    m_ErrInfo.clear();

    int ret = N_ReadFile();             //读取文件
    if(SUCCESS == ret)
    {
        ret = N_WordCheck();            //词法检测
        if(SUCCESS == ret)
        {
            ret = N_GrammarCheck();     //语法检测
            if(SUCCESS == ret)
            {
                formInfo = m_ArrArrForm;
            }
        }
    }

    return ret;
}


int CJnlFormAnalyze::N_GrammarCheck()
{
    int ret = SUCCESS;
    m_it_begin_form = m_vecFile.begin();
    m_it_end_form = m_it_begin_form;

    do{
        ret = S_Start(m_it_begin_form);
    }while(m_it_begin_form != m_vecFile.end());

    return ret;
}


int CJnlFormAnalyze::S_Start(strArr::iterator it)
{
    int ret = SUCCESS;

    strArr::iterator it1 = it;
    std::stringstream ss;
    std::string str;
    for( it1 = it;it1 != m_vecFile.end();++it1)
    {
        if(!N_IsSpace(*it1) && !N_IsAnnotation(*it1))
        {
            ss.str("");         //清空
            ss.clear();
            ss<<*it1;

            while(ss>>str)
            {
                if(JNL_FORM == str)
                {
                    return S_Form(it1);
                }
            }
        }
    }

    if(0 == m_ArrArrForm.size())
    {
        m_ErrInfo.push_back(ERROR_INFO(-1, "没有找到合格的 FORM"));
        ret = NOT_FIND_FORM;
    }
    else
    {
        ret = SUCCESS;
    }

    m_it_begin_form = m_vecFile.end();

    return ret;
}


int CJnlFormAnalyze::S_Form(strArr::iterator it)
{
    m_it_begin_form = it;

    strArr::iterator it1 = it;
    std::stringstream ss;
    std::string str;
    for( it1 = it + 1;it1 < m_vecFile.end();++it1)
    {
        if(!N_IsSpace(*it1) && !N_IsAnnotation(*it1))
        {
            ss.str("");         //清空
            ss.clear();
            ss<<*it1;

            while(ss>>str)
            {
                if(JNL_BEGIN == str)
                {
                    return S_Form_Begin(it1);
                }
                else if(JNL_FIELD == str || JNL_END == str)
                {
                    m_ErrInfo.push_back(ERROR_INFO(it1 - m_vecFile.begin(), "之前缺少 BEGIN 关键字"));
                    return S_Start(it1);
                }
                else if(JNL_FORM == str)
                {
                    m_ErrInfo.push_back(ERROR_INFO(it1 - m_vecFile.begin(), "FORM 嵌套 FORM"));
                    m_it_begin_form = it1;
                }
            }
        }
    }

    m_it_begin_form = m_vecFile.end();

    m_ErrInfo.push_back(ERROR_INFO(-1, "异常结束"));

    return ABNORMAL_END;
}


int CJnlFormAnalyze::S_Form_Begin(strArr::iterator it)
{
    strArr::iterator it1 = it;
    std::stringstream ss;
    std::string str;
    for( it1 = it + 1;it1 < m_vecFile.end();++it1)
    {
        if(!N_IsSpace(*it1) && !N_IsAnnotation(*it1))
        {
            ss.str("");         //清空
            ss.clear();
            ss<<*it1;

            while(ss>>str)
            {
                if(JNL_FORM == str)
                {
                    m_ErrInfo.push_back(ERROR_INFO(it1 - m_vecFile.begin(), "FORM 嵌套 FORM"));
                    return S_Form(it1);
                }
                else if(JNL_FIELD == str)
                {
                    return S_Form_Begin_Field(it1);
                }
                else if(JNL_BEGIN == str)
                {
                    m_ErrInfo.push_back(ERROR_INFO(it1 - m_vecFile.begin(), "BEGIN 关键字重复"));
                    return S_Start(it1);
                }
                else if(JNL_END == str)
                {
                    return S_End(it1);
                }
            }
        }
    }

    m_it_begin_form = m_vecFile.end();

    m_ErrInfo.push_back(ERROR_INFO(-1, "异常结束"));

    return ABNORMAL_END;
}


int CJnlFormAnalyze::S_Form_Begin_Field(strArr::iterator it)
{
    strArr::iterator it1 = it;
    std::stringstream ss;
    std::string str;
    for( it1 = it + 1;it1 < m_vecFile.end();++it1)
    {
        if(!N_IsSpace(*it1) && !N_IsAnnotation(*it1))
        {
            ss.str("");         //清空
            ss.clear();
            ss<<*it1;

            while(ss>>str)
            {
                if(JNL_FORM == str)
                {
                    m_ErrInfo.push_back(ERROR_INFO(it1 - m_vecFile.begin(), "FORM 嵌套 FORM"));
                    return S_Form(it1);
                }
                else if(JNL_FIELD == str)
                {
                    m_ErrInfo.push_back(ERROR_INFO(it1 - m_vecFile.begin(), "FIELD 嵌套 FIELD"));
                    return S_Start(it1);
                }
                else if(JNL_BEGIN == str)
                {
                    return S_Form_Begin_Field_Begin(it1);
                }
                else if(JNL_END == str)
                {
                    m_ErrInfo.push_back(ERROR_INFO(it1 - m_vecFile.begin(), "之前缺少 BEGIN 关键字"));
                    return S_Start(it1);
                }
            }
        }
    }

    m_it_begin_form = m_vecFile.end();

    m_ErrInfo.push_back(ERROR_INFO(-1, "异常结束"));

    return ABNORMAL_END;
}


int CJnlFormAnalyze::S_Form_Begin_Field_Begin(strArr::iterator it)
{
    m_bPosition = false;
    m_bSize = false;
    m_bClass = false;
    m_bValue = false;

    strArr::iterator it1 = it;
    std::stringstream ss;
    std::string str;
    for( it1 = it + 1;it1 < m_vecFile.end();++it1)
    {
        if(!N_IsSpace(*it1) && !N_IsAnnotation(*it1))
        {
            ss.str("");         //清空
            ss.clear();
            ss<<*it1;

            while(ss>>str)
            {
                if(JNL_FORM == str)
                {
                    m_ErrInfo.push_back(ERROR_INFO(it1 - m_vecFile.begin(), "FORM 嵌套 FORM"));
                    return S_Form(it1);
                }
                else if(JNL_FIELD == str)
                {
                    m_ErrInfo.push_back(ERROR_INFO(it1 - m_vecFile.begin(), "FIELD 嵌套 FIELD"));
                    return S_Start(it1);
                }
                else if(JNL_BEGIN == str)
                {
                    m_ErrInfo.push_back(ERROR_INFO(it1 - m_vecFile.begin(), "BEGIN 关键字重复"));
                    return S_Start(it1);
                }
                else if(JNL_END == str)
                {
                    return S_Form_Begin_Field_Begin_End(it1);
                }
                else if(JNL_POSITION == str)
                {
                    return S_Position(it1);
                }
                else if(JNL_SIZE == str)
                {
                    return S_Size(it1);
                }
                else if(JNL_CLASS == str)
                {
                    return S_Class(it1);
                }
                else if(JNL_VALUE == str)
                {
                    return S_Value(it1);
                }
            }
        }
    }

    m_it_begin_form = m_vecFile.end();

    m_ErrInfo.push_back(ERROR_INFO(-1, "异常结束"));

    return ABNORMAL_END;
}

int CJnlFormAnalyze::S_Position(strArr::iterator it)
{
    m_bPosition = true;

    strArr::iterator it1 = it;
    std::stringstream ss;
    std::string str;
    for( it1 = it + 1;it1 < m_vecFile.end();++it1)
    {
        if(!N_IsSpace(*it1) && !N_IsAnnotation(*it1))
        {
            ss.str("");         //清空
            ss.clear();
            ss<<*it1;

            while(ss>>str)
            {
                if(JNL_FORM == str)
                {
                    m_ErrInfo.push_back(ERROR_INFO(it1 - m_vecFile.begin(), "FORM 嵌套 FORM"));
                    return S_Form(it1);
                }
                else if(JNL_FIELD == str)
                {
                    m_ErrInfo.push_back(ERROR_INFO(it1 - m_vecFile.begin(), "FIELD 嵌套 FIELD"));
                    return S_Start(it1);
                }
                else if(JNL_BEGIN == str)
                {
                    m_ErrInfo.push_back(ERROR_INFO(it1 - m_vecFile.begin(), "BEGIN 关键字重复"));
                    return S_Start(it1);
                }
                else if(JNL_END == str)
                {
                    if(!m_bPosition)
                    {
                        m_ErrInfo.push_back(ERROR_INFO(it1 - m_vecFile.begin(), "缺少 POSITION 关键字"));
                        return S_Start(it1);
                    }
                    if(!m_bSize)
                    {
                        m_ErrInfo.push_back(ERROR_INFO(it1 - m_vecFile.begin(), "缺少 SIZE 关键字"));
                        return S_Start(it1);
                    }
                    if(!m_bClass)
                    {
                        m_ErrInfo.push_back(ERROR_INFO(it1 - m_vecFile.begin(), "缺少 CLASS 关键字"));
                        return S_Start(it1);
                    }
                    if(!m_bValue)
                    {
                        m_ErrInfo.push_back(ERROR_INFO(it1 - m_vecFile.begin(), "缺少 VALUE 关键字"));
                        return S_Start(it1);
                    }

                    return S_Form_Begin_Field_Begin_End(it1);
                }
                else if(JNL_POSITION == str)
                {
                    m_ErrInfo.push_back(ERROR_INFO(it1 - m_vecFile.begin(), "POSITION 关键字重复"));
                    return S_Position(it1);
                }
                else if(JNL_SIZE == str)
                {
                    return S_Size(it1);
                }
                else if(JNL_CLASS == str)
                {
                    return S_Class(it1);
                }
                else if(JNL_VALUE == str)
                {
                    return S_Value(it1);
                }
            }
        }
    }

    m_it_begin_form = m_vecFile.end();

    m_ErrInfo.push_back(ERROR_INFO(-1, "异常结束"));

    return ABNORMAL_END;
}


int CJnlFormAnalyze::S_Size(strArr::iterator it)
{
    m_bSize = true;

    strArr::iterator it1 = it;
    std::stringstream ss;
    std::string str;
    for( it1 = it + 1;it1 < m_vecFile.end();++it1)
    {
        if(!N_IsSpace(*it1) && !N_IsAnnotation(*it1))
        {
            ss.str("");         //清空
            ss.clear();
            ss<<*it1;

            while(ss>>str)
            {
                if(JNL_FORM == str)
                {
                    m_ErrInfo.push_back(ERROR_INFO(it1 - m_vecFile.begin(), "FORM 嵌套 FORM"));
                    return S_Form(it1);
                }
                else if(JNL_FIELD == str)
                {
                    m_ErrInfo.push_back(ERROR_INFO(it1 - m_vecFile.begin(), "FIELD 嵌套 FIELD"));
                    return S_Start(it1);
                }
                else if(JNL_BEGIN == str)
                {
                    m_ErrInfo.push_back(ERROR_INFO(it1 - m_vecFile.begin(), "BEGIN 关键字重复"));
                    return S_Start(it1);
                }
                else if(JNL_END == str)
                {
                    if(!m_bPosition)
                    {
                        m_ErrInfo.push_back(ERROR_INFO(it1 - m_vecFile.begin(), "缺少 POSITION 关键字"));
                        return S_Start(it1);
                    }
                    if(!m_bSize)
                    {
                        m_ErrInfo.push_back(ERROR_INFO(it1 - m_vecFile.begin(), "缺少 SIZE 关键字"));
                        return S_Start(it1);
                    }
                    if(!m_bClass)
                    {
                        m_ErrInfo.push_back(ERROR_INFO(it1 - m_vecFile.begin(), "缺少 CLASS 关键字"));
                        return S_Start(it1);
                    }
                    if(!m_bValue)
                    {
                        m_ErrInfo.push_back(ERROR_INFO(it1 - m_vecFile.begin(), "缺少 VALUE 关键字"));
                        return S_Start(it1);
                    }

                    return S_Form_Begin_Field_Begin_End(it1);
                }
                else if(JNL_POSITION == str)
                {
                    return S_Position(it1);
                }
                else if(JNL_SIZE == str)
                {
                    m_ErrInfo.push_back(ERROR_INFO(it1 - m_vecFile.begin(), "SIZE 关键字重复"));
                    return S_Size(it1);
                }
                else if(JNL_CLASS == str)
                {
                    return S_Class(it1);
                }
                else if(JNL_VALUE == str)
                {
                    return S_Value(it1);
                }
            }
        }
    }

    m_it_begin_form = m_vecFile.end();

    m_ErrInfo.push_back(ERROR_INFO(-1, "异常结束"));

    return ABNORMAL_END;
}


int CJnlFormAnalyze::S_Class(strArr::iterator it)
{
    m_bClass = true;

    strArr::iterator it1 = it;
    std::stringstream ss;
    std::string str;
    for( it1 = it + 1;it1 < m_vecFile.end();++it1)
    {
        if(!N_IsSpace(*it1) && !N_IsAnnotation(*it1))
        {
            ss.str("");         //清空
            ss.clear();
            ss<<*it1;

            while(ss>>str)
            {
                if(JNL_FORM == str)
                {
                    m_ErrInfo.push_back(ERROR_INFO(it1 - m_vecFile.begin(), "FORM 嵌套 FORM"));
                    return S_Form(it1);
                }
                else if(JNL_FIELD == str)
                {
                    m_ErrInfo.push_back(ERROR_INFO(it1 - m_vecFile.begin(), "FIELD 嵌套 FIELD"));
                    return S_Start(it1);
                }
                else if(JNL_BEGIN == str)
                {
                    m_ErrInfo.push_back(ERROR_INFO(it1 - m_vecFile.begin(), "BEGIN 关键字重复"));
                    return S_Start(it1);
                }
                else if(JNL_END == str)
                {
                    if(!m_bPosition)
                    {
                        m_ErrInfo.push_back(ERROR_INFO(it1 - m_vecFile.begin(), "缺少 POSITION 关键字"));
                        return S_Start(it1);
                    }
                    if(!m_bSize)
                    {
                        m_ErrInfo.push_back(ERROR_INFO(it1 - m_vecFile.begin(), "缺少 SIZE 关键字"));
                        return S_Start(it1);
                    }
                    if(!m_bClass)
                    {
                        m_ErrInfo.push_back(ERROR_INFO(it1 - m_vecFile.begin(), "缺少 CLASS 关键字"));
                        return S_Start(it1);
                    }
                    if(!m_bValue)
                    {
                        m_ErrInfo.push_back(ERROR_INFO(it1 - m_vecFile.begin(), "缺少 VALUE 关键字"));
                        return S_Start(it1);
                    }

                    return S_Form_Begin_Field_Begin_End(it1);
                }
                else if(JNL_POSITION == str)
                {
                    return S_Position(it1);
                }
                else if(JNL_SIZE == str)
                {
                    return S_Size(it1);
                }
                else if(JNL_CLASS == str)
                {
                    m_ErrInfo.push_back(ERROR_INFO(it1 - m_vecFile.begin(), "CLASS 关键字重复"));
                    return S_Class(it1);
                }
                else if(JNL_VALUE == str)
                {
                    return S_Value(it1);
                }
            }
        }
    }

    m_it_begin_form = m_vecFile.end();

    m_ErrInfo.push_back(ERROR_INFO(-1, "异常结束"));

    return ABNORMAL_END;
}


int CJnlFormAnalyze::S_Value(strArr::iterator it)
{
    m_bValue = true;

    strArr::iterator it1 = it;
    std::stringstream ss;
    std::string str;
    for( it1 = it + 1;it1 < m_vecFile.end();++it1)
    {
        if(!N_IsSpace(*it1) && !N_IsAnnotation(*it1))
        {
            ss.str("");         //清空
            ss.clear();
            ss<<*it1;

            while(ss>>str)
            {
                if(JNL_FORM == str)
                {
                    m_ErrInfo.push_back(ERROR_INFO(it1 - m_vecFile.begin(), "FORM 嵌套 FORM"));
                    return S_Form(it1);
                }
                else if(JNL_FIELD == str)
                {
                    m_ErrInfo.push_back(ERROR_INFO(it1 - m_vecFile.begin(), "FIELD 嵌套 FIELD"));
                    return S_Start(it1);
                }
                else if(JNL_BEGIN == str)
                {
                    m_ErrInfo.push_back(ERROR_INFO(it1 - m_vecFile.begin(), "BEGIN 关键字重复"));
                    return S_Start(it1);
                }
                else if(JNL_END == str)
                {
                    if(!m_bPosition)
                    {
                        m_ErrInfo.push_back(ERROR_INFO(it1 - m_vecFile.begin(), "缺少 POSITION 关键字"));
                        return S_Start(it1);
                    }
                    if(!m_bSize)
                    {
                        m_ErrInfo.push_back(ERROR_INFO(it1 - m_vecFile.begin(), "缺少 SIZE 关键字"));
                        return S_Start(it1);
                    }
                    if(!m_bClass)
                    {
                        m_ErrInfo.push_back(ERROR_INFO(it1 - m_vecFile.begin(), "缺少 CLASS 关键字"));
                        return S_Start(it1);
                    }
                    if(!m_bValue)
                    {
                        m_ErrInfo.push_back(ERROR_INFO(it1 - m_vecFile.begin(), "缺少 VALUE 关键字"));
                        return S_Start(it1);
                    }

                    return S_Form_Begin_Field_Begin_End(it1);
                }
                else if(JNL_POSITION == str)
                {
                    return S_Position(it1);
                }
                else if(JNL_SIZE == str)
                {
                    return S_Size(it1);
                }
                else if(JNL_CLASS == str)
                {
                    return S_Class(it1);
                }
                else if(JNL_VALUE == str)
                {
                    m_ErrInfo.push_back(ERROR_INFO(it1 - m_vecFile.begin(), "VALUE 关键字重复"));
                    return S_Value(it1);
                }
            }
        }
    }

    m_it_begin_form = m_vecFile.end();

    m_ErrInfo.push_back(ERROR_INFO(-1, "异常结束"));

    return ABNORMAL_END;
}


int CJnlFormAnalyze::S_Form_Begin_Field_Begin_End(strArr::iterator it)
{
    strArr::iterator it1 = it;
    std::stringstream ss;
    std::string str;
    for( it1 = it + 1;it1 < m_vecFile.end();++it1)
    {
        if(!N_IsSpace(*it1) && !N_IsAnnotation(*it1))
        {
            ss.str("");         //清空
            ss.clear();
            ss<<*it1;

            while(ss>>str)
            {
                if(JNL_FORM == str)
                {
                    m_ErrInfo.push_back(ERROR_INFO(it1 - m_vecFile.begin(), "FORM 嵌套 FORM"));
                    return S_Form(it1);
                }
                else if(JNL_FIELD == str)
                {
                    return S_Form_Begin_Field(it1);
                }
                else if(JNL_BEGIN == str)
                {
                    m_ErrInfo.push_back(ERROR_INFO(it1 - m_vecFile.begin(), "BEGIN 关键字多余"));
                    return S_Start(it1);
                }
                else if(JNL_END == str)
                {
                    return S_End(it1);
                }
            }
        }
    }

    m_it_begin_form = m_vecFile.end();

    m_ErrInfo.push_back(ERROR_INFO(-1, "异常结束"));

    return ABNORMAL_END;
}


int CJnlFormAnalyze::S_End(strArr::iterator it)
{
    m_it_end_form = ++it;

    strArr sa;
    for(strArr::iterator it_c = m_it_begin_form;it_c != m_it_end_form;it_c++)
    {
        if(!N_IsAnnotation(*it_c))
        {
            sa.push_back(*it_c);
        }
    }
    m_ArrArrForm.push_back(sa);

    if(m_it_end_form != m_vecFile.end())
    {
        m_it_begin_form = m_it_end_form + 1;
    }
    else
    {
        m_it_begin_form = m_it_end_form;
    }

    return SUCCESS;
}


int CJnlFormAnalyze::N_ReadFile()
{
    std::fstream f_in;
    std::string str("");
    f_in.open(m_strFilePathName, std::ios::in);     //打开文件
    if(!f_in)
    {
        m_ErrInfo.push_back(ERROR_INFO(-1, "打开文件失败"));
        return OPEN_FILE_FAIL;                      //打开文件失败
    }

    while(!f_in.eof())
    {
        getline(f_in,str);

        m_vecFile.push_back(str);
    }

    f_in.close();                                   //关闭文件

    return SUCCESS;
}


bool CJnlFormAnalyze::N_IsAnnotation(std::string str)
{
    std::stringstream ss;
    ss<<str;
    ss>>str;                                        //清除左边的空格
    if(str.length() > 1 && str[0] == '/' && str[1] == '/')
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool CJnlFormAnalyze::N_IsSpace(std::string str)
{
    std::string strTmp("");
    for(std::size_t i = 0;i<str.length();i++)
    {
        if(' ' != str[i] && '\t' != str[i])
        {
            strTmp += str[i];
        }
    }

    if("" == strTmp)
    {
        return true;
    }
    else
    {
        return false;
    }
}


int CJnlFormAnalyze::N_WordCheck()
{
    int ret_err = SUCCESS;
    int ret = SUCCESS;

    std::stringstream ss;
    std::string str("");
    for(std::size_t i = 0;i<m_vecFile.size();i++)
    {
        if(!N_IsSpace(m_vecFile[i]) && !N_IsAnnotation(m_vecFile[i]))     //不是空行和注释
        {
            ss.str("");
            ss.clear();
            ss<<m_vecFile[i];
            while(ss>>str)
            {
                if(JNL_FORM == str)
                {
                    ret = M_Form(m_vecFile[i]);
                }
                else if(JNL_FIELD == str)
                {
                    ret = M_Field(m_vecFile[i]);
                }
                else if(JNL_BEGIN == str)
                {
                    ret = M_Begin(m_vecFile[i]);
                }
                else if(JNL_END == str)
                {
                    ret = M_End(m_vecFile[i]);
                }
                else if(JNL_POSITION == str)
                {
                    ret = M_Position(m_vecFile[i]);
                }
                else if(JNL_SIZE == str)
                {
                    ret = M_Size(m_vecFile[i]);
                }
                else if(JNL_CLASS == str)
                {
                    ret = M_Class(m_vecFile[i]);
                }
                else if(JNL_VALUE == str)
                {
                    ret = M_Value(m_vecFile[i]);
                }
                else if(JNL_HORIZONTAL == str)
                {
                    ret = M_Horizontal(m_vecFile[i]);
                }
                else
                {
                    ret = INVALID_STATEMENT;
                }
                break;
            }

            if(SUCCESS != ret)
            {
                m_ErrInfo.push_back(ERROR_INFO(i, "不可识别的语句"));
                ret_err = ret;
            }
        }
    }

    return ret_err;
}


int CJnlFormAnalyze::M_Form(std::string str)
{
    int ret = INVALID_STATEMENT;

    const std::regex pattern_form(REGEX_PATTERN_FORM);
    std::match_results<std::string::const_iterator> result;
    bool bMatch = std::regex_match(str,result,pattern_form);
    if(bMatch)
    {
        ret = SUCCESS;
    }

    return ret;
}


int CJnlFormAnalyze::M_Field(std::string str)
{
    return M_Form(str);
}

int CJnlFormAnalyze::M_Begin(std::string str)
{
    int ret = INVALID_STATEMENT;

    const std::regex pattern_begin(REGEX_PATTERN_BEGIN);
    std::match_results<std::string::const_iterator> result;
    bool bMatch = std::regex_match(str, result, pattern_begin);
    if(bMatch)
    {
        ret = SUCCESS;
    }

    return ret;
}


int CJnlFormAnalyze::M_End(std::string str)
{
    return M_Begin(str);
}


int CJnlFormAnalyze::M_Position(std::string str)
{
    int ret = INVALID_STATEMENT;

    const std::regex pattern_position(REGEX_PATTERN_POSITION);
    std::match_results<std::string::const_iterator> result;
    bool bMatch = std::regex_match(str, result, pattern_position);
    if(bMatch)
    {
        ret = SUCCESS;
    }

    return ret;
}


int CJnlFormAnalyze::M_Size(std::string str)
{
    int ret = INVALID_STATEMENT;

    const std::regex pattern_size(REGEX_PATTERN_SIZE);
    std::match_results<std::string::const_iterator> result;
    bool bMatch = std::regex_match(str, result, pattern_size);
    if(bMatch)
    {
        ret = SUCCESS;
    }

    return ret;
}


int CJnlFormAnalyze::M_Class(std::string str)
{
    int ret = INVALID_STATEMENT;

    const std::regex pattern_class(REGEX_PATTERN_CLASS);
    std::match_results<std::string::const_iterator> result;
    bool bMatch = std::regex_match(str, result, pattern_class);
    if(bMatch)
    {
        ret = SUCCESS;
    }

    return ret;
}


int CJnlFormAnalyze::M_Value(std::string str)
{
    int ret = INVALID_STATEMENT;

    const std::regex pattern_value(REGEX_PATTERN_VALUE);
    std::match_results<std::string::const_iterator> result;
    bool bMatch = std::regex_match(str, result, pattern_value);
    if(bMatch)
    {
        ret = SUCCESS;
    }

    return ret;
}


int CJnlFormAnalyze::M_Horizontal(std::string str)
{
    int ret = INVALID_STATEMENT;

    const std::regex pattern_horizontal(REGEX_PATTERN_HORIZONTAL);
    std::match_results<std::string::const_iterator> result;
    bool bMatch = std::regex_match(str, result, pattern_horizontal);
    if(bMatch)
    {
        ret = SUCCESS;
    }

    return ret;
}


std::string CJnlFormAnalyze::GetErrorInfo()
{
    std::stringstream ss;

    for(size_t i = 0;i<m_ErrInfo.size();i++)
    {
        if(-1 == m_ErrInfo[i].m_iRow)
        {
            ss<<i + 1<<" "<<m_ErrInfo[i].m_strInfo<<std::endl;
        }
        else
        {
            ss<<i + 1<<" ["<<m_ErrInfo[i].m_iRow + 1<<"] "<<m_ErrInfo[i].m_strInfo<<std::endl;
        }
    }

    return ss.str();
}
