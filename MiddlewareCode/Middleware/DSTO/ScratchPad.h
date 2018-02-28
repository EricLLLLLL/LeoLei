///////////////////////////
//		Create by art
//////////////////////////

#pragma once

#include "artFunc.h"
#include "easytrace.h"

using namespace std;
#include <map>
#include <string>

typedef map<string, string> SCRATCHMAP;

class CScratchPad
{
public:
	CScratchPad(void);
	~CScratchPad(void);

protected:

public:
	static SCRATCHMAP m_ScratchPad;
	//short F_CreateMainWnd(HWND* hWnd);

	short SetData(char *pszDataName, char *pszDataType, char *pszNewValue);
	short GetData(char *pszDataName, char *pszDataType, char *pszValue);

	short InitDatas();
	short ClearData();///////针对继续交易，清除需要清除的数据
private:
	static SCRATCHMAP m_ScratchPadInit;
	static SCRATCHMAP m_ScratchPadClear;
	short LoadInitValues();
	short LoadClearValues();
	static bool m_bLoaded;
	static bool m_bCleared;
};
