///////////////////////////
//		Create by art
//////////////////////////

#pragma once

#include "win_type_def.h"

class CDllDSND
{

public:
	CDllDSND(void);
	~CDllDSND(void);
public:
	long PackageData(const char *pszDealType, char *pszBuffer ,long *pnBufLen, char *pszMacData, long *pnMacDataLen);

private:
	void Load();          
};
