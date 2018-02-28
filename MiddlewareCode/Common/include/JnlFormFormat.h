#pragma once
#include <string>
#include <vector>
#include <QtCore/qglobal.h>
#ifdef JNL_FORM_ANALYZE_LIBRARY
#define JNL_FORM_ANALYZE_LIB Q_DECL_EXPORT
#else
#define JNL_FORM_ANALYZE_LIB Q_DECL_IMPORT
#endif


class JNL_FORM_ANALYZE_LIB CJnlFormFormat
{
public:
    enum {
        NG = -1,
        OK = 0
    };
public:
    CJnlFormFormat(void);
    virtual ~CJnlFormFormat(void);

    //
    //功能：Form 解析。
    //输入参数：[in] strFileName 文件路径名。
    //返回值：OK 找到了合格的 FORM ，不代表 FORM 解析没有错误。
    //        NG 没有找到合格的 FORM 。
    int Translate(const std::string strFilePathName);

    //
    //功能：根据指定 FORM 格式化字符串。
    //输入参数：[in] strFormName FORM 名。
    //          [in] strDataIn 输入字符串。以键值对的形式传入并以逗号分隔（如:"CardNo = 123123132, Money = 500"）
    //返回值：格式化后的字符串。
    std::string Format(std::string strFormName, std::string strDataIn);

    //
    //功能：得到 FORM 解析的错误信息。
    //输入参数：无。
    //返回值：FORM 解析的错误信息。
    std::string GetErrInfo();
private:
    bool N_IsExist(const class CJnlForm *lp_Form);        //是否存在同名的 FORM
    class CJnlFormAnalyze* m_lpFormAnalyze;               //FORM 解析对象
    std::vector<class CJnlForm*> m_ArrForm;               //文件中所有合格form
};

