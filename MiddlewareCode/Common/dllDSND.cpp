///////////////////////////
//		Create by art
//////////////////////////

#include "stdafx.h"
#include "dllDSND.h"

CDllDSND::CDllDSND(void)
{
//	m_hModule = NULL;
}

CDllDSND::~CDllDSND(void)
{
 	//if (m_hModule != NULL) {
 	//	::FreeLibrary((HINSTANCE)m_hModule);
 	//}
}

void CDllDSND::Load()
{
//	if (NULL == m_hModule)
	{
		//m_hModule = ::LoadLibrary("DSND.dll");
        //if (m_hModule == NULL) {
			//MessageBox(NULL,"LoadLibary DSND.dll Error","Error",MB_OK);
        //}
	}

}


long CDllDSND::PackageData(const char *pszDealType, char *pszBuffer ,long *pnBufLen, char *pszMacData, long *pnMacDataLen)
{
	Load();
	long nRet;

//

	return nRet;
}
