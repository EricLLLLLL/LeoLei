#ifndef __YUAN_LOG_H__
#define __YUAN_LOG_H__

#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <vector>

#include <shlwapi.h>
#pragma comment(lib,"shlwapi.lib")
#include <Windows.h>
#pragma comment(lib, "version.lib")

#define LOG_BUF_LEN		512					//��־�ַ�����������				
#define LOG_FILE_LEN	64					//��־�ļ�������

int		WriteLog(char *fmt, ...);
int		HMCreateDirectory(TCHAR* szPath);
void	HMPrintVersion();
void	HMGetErrorText(const int hr, TCHAR *szOut);
void    SetSPName(char* chName);

#endif