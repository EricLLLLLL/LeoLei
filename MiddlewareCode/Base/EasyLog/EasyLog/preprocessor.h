#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <string>
#include "win_type_def.h"
#include "QString"

class CPreprocessor
{
public:
    CPreprocessor(void);
    ~CPreprocessor(void);

    void CreateConfiguration();
    void GetConfiguration();
    void AssignPathFile( char* , char*,char* );
private:
    std::string m_dateTime;

    char m_szDirPath[256];
    char m_szPathConfig[256];
    char m_szExpiration[10];
    QString m_strLogDir;
};

#endif // PREPROCESSOR_H
