///////////////////////////
//		Create by art
//////////////////////////

#include "stdafx.h"
#include "dllDRCV.h"

CDllDRCV::CDllDRCV(void)
{
//	m_hModule = NULL;
}

CDllDRCV::~CDllDRCV(void)
{
 	//if (m_hModule != NULL) {
 	//	::FreeLibrary((HINSTANCE)m_hModule);
 	//}
}

void CDllDRCV::Load()
{
//	if (NULL == m_hModule)
	{
		//m_hModule = ::LoadLibrary("DRCV.dll");
        //if (m_hModule == NULL) {
			//MessageBox(NULL,"LoadLibary DRCV.dll Error","Error",MB_OK);
        //}
	}

}


long CDllDRCV::ParseData(const char *pszDealType, const char *pszBuffer, const long nBufLen)
{
	Load();
	long nRet;



	return nRet;
}
