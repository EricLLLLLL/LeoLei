#ifndef ALARM_LIB
#define ALARM_LIB
extern "C" long Alarm(const char* pszErrCode);
extern "C" int GetAlarm(char* pszOutput, const int nBuffLen=0);
extern "C" int RenameAlarmFile(void);
#endif
