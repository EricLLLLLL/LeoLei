#include "JnlFormFormat.h"
#include "JnlFormAnalyze.h"
#include "JnlForm.h"
#include "common.h"


CJnlFormFormat::CJnlFormFormat(void)
    : m_lpFormAnalyze(nullptr)
{
    if(nullptr == m_lpFormAnalyze)
    {
        m_lpFormAnalyze = new CJnlFormAnalyze();
    }
}


CJnlFormFormat::~CJnlFormFormat(void)
{
    if(m_lpFormAnalyze)
    {
        delete m_lpFormAnalyze;
        m_lpFormAnalyze = nullptr;
    }

    for(std::vector<class CJnlForm*>::iterator it = m_ArrForm.begin();it != m_ArrForm.end();it++)
    {
        delete *it;
    }
    m_ArrForm.clear();
}

int CJnlFormFormat::Translate(const std::string strFilePathName)
{
    for(std::vector<class CJnlForm*>::iterator it = m_ArrForm.begin();it != m_ArrForm.end();it++)
    {
        delete *it;
    }
    m_ArrForm.clear();

    m_lpFormAnalyze->SetFilePathName(strFilePathName);

    strArrArr saaFormInfo;

    int ret = m_lpFormAnalyze->Analyze(saaFormInfo);

    if(SUCCESS == ret)
    {
        for(strArrArr::const_iterator cit = saaFormInfo.begin();cit != saaFormInfo.end();cit++)
        {
            CJnlForm* lp_form = new CJnlForm();
            ret = lp_form->Translate(*cit);
            if(SUCCESS == ret)
            {
                if(!N_IsExist(lp_form))
                {
                    m_ArrForm.push_back(lp_form);
                }
            }
        }
    }

    if(SUCCESS == ret)
    {
        ret = CJnlFormFormat::OK;
    }
    else
    {
        ret = CJnlFormFormat::NG;
    }

    return ret;
}


std::string CJnlFormFormat::Format(std::string strFormName, std::string strDataIn)
{
    std::string strRet("");

    std::vector<CJnlForm *>::iterator it;
    for(it = m_ArrForm.begin();it != m_ArrForm.end();it++)
    {
        if((*it)->GetFormName() == strFormName)
        {
            break;
        }
    }

    if(it == m_ArrForm.end())
    {
        return "";          //没有找到相应的 FORM
    }

    strRet = (*it)->Format(strDataIn);

    return strRet;
}

std::string CJnlFormFormat::GetErrInfo()
{
    return m_lpFormAnalyze->GetErrorInfo();
}


bool CJnlFormFormat::N_IsExist(const CJnlForm *lp_Form)
{
    for(std::vector<CJnlForm *>::const_iterator cit = m_ArrForm.begin();cit != m_ArrForm.end();cit++)
    {
        if(*lp_Form == **cit)
        {
            return true;
        }
    }
    return false;
}
