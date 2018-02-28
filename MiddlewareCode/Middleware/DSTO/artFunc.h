#pragma once
#include "win_type_def.h"

//原类型有: CHAR, BYTE,	LONG, STRING, BYTEARRAY, LONGARRAY, STRINGARRAY

void ByteArray2String( LPBYTE lpDest, LPBYTE lpSrc, WORD wSize);
void String2ByteArray( LPBYTE lpDest, LPBYTE lpSrc, WORD wSize);
void StrArray2String(QString& strDest, const QStringList& strArraySrc, WORD wArraySize);
void LongArray2String(QString& strDest, LPLONG longArray, WORD wLongSize);
void String2LongArray(LPLONG longArrayDest, WORD* pwLongSize, QString strArraySrc);
//void GetDatabase(char *pszDB);
