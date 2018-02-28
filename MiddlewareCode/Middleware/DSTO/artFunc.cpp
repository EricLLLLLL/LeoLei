///////////////////////////
//		Create by art
//////////////////////////

#include "stdafx.h"
#include "artFunc.h"
#include <string>


// 转换 {0x00, 0x0A, 0x15, 0x2F}形式的16进制数组为 "00A152F" 形式的字符串
void ByteArray2String( LPBYTE lpDest, LPBYTE lpSrc, WORD wSize)
{
    char Number[] = "0123456789ABCDEF"; 

    for( int i = 0; i < wSize; i++ )
    {
        *lpDest++ = Number[lpSrc[i] >> 4];
        *lpDest++ = Number[lpSrc[i] & 0x0f];
    }
}

// 转换字符串如 00A152F 为 { 0x00, 0x0A, 0x15, 0x2F }的byte数组
void String2ByteArray( LPBYTE lpDest, LPBYTE lpSrc, WORD wSize)
{
    int High, Low;

    if( wSize % 2 ){
        return;
    }

    for( int i = 0; i < wSize; i += 2 ){
        if( lpSrc[i] >= '0' && lpSrc[i] <= '9' )
            High = lpSrc[i] - '0';
        else if( lpSrc[i] >= 'A' && lpSrc[i] <= 'F' )
            High = lpSrc[i] - 'A' + 10;
        else if( lpSrc[i] >= 'a' && lpSrc[i] <= 'f' )
            High = lpSrc[i] - 'a' + 10;
        else {
            return;
        }
        if( lpSrc[i+1] >= '0' && lpSrc[i+1] <= '9' )
            Low = lpSrc[i+1]-'0';
        else if( lpSrc[i+1] >= 'A' && lpSrc[i+1] <= 'F' )
            Low = lpSrc[i+1] - 'A' + 10;
        else if( lpSrc[i+1] >= 'a' && lpSrc[i+1] <= 'f' )
            Low = lpSrc[i+1] - 'a' + 10;
        else {
            return;
        }
        *lpDest++ = BYTE(High * 16 + Low);
    }
}

// 转换字符串数组为以逗号分隔的字符串
void StrArray2String(QString& strDest, const QStringList& strArraySrc, WORD wArraySize)
{
    for (int i = 0; i < wArraySize; ++i)
    {
        strDest += strArraySrc[i];

        if (wArraySize - 1 != i)
        {
            strDest += ",";
        }
    }
}

void LongArray2String(QString& strDest, LPLONG longArray, WORD wLongSize)
{
    for (int i = 0; i < wLongSize; ++i)
    {
        if (i == 0)
        {
            strDest.sprintf("%d", longArray[i]);
        }
        else
        {
            strDest.sprintf("%s,%d", Q2S(strDest), longArray[i]);
        }
    }
}

void String2LongArray(LPLONG longArrayDest, WORD* pwLongSize, QString strArraySrc)
{
    QStringList strArr = strArraySrc.split(",");
    int iIndex = 0;
    for(; iIndex < strArr.count(); iIndex++)
        {
        longArrayDest[iIndex] = atol(Q2S(strArr[iIndex]));
    }
    *pwLongSize=iIndex;
}

/*
void GetDatabase(char *pszDB)
{
        std::string sPath = "";
        char szCurrentPathName[MAX_PATH]={0};
        GetModuleFileName(NULL,szCurrentPathName,MAX_PATH);
        sPath += szCurrentPathName;
        sPath = sPath.substr(0,sPath.rfind("\\"));
        sPath += "\\DB\\TCR.db3";

        sprintf(pszDB, sPath.c_str());
}*/
