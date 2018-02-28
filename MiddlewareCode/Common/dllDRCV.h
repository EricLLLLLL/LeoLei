///////////////////////////
//		Create by art
//////////////////////////

#pragma once

#include "win_type_def.h"

class CDllDRCV
{
public:
	CDllDRCV(void);
	~CDllDRCV(void);
public:
	long ParseData(const char *pszDealType, const char *pszBuffer, const long nBufLen);

private:
	void Load();          
};
