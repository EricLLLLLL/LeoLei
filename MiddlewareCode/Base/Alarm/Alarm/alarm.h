#ifndef ALARM_H
#define ALARM_H
using namespace std;
#include "alarm_global.h"
#include "win_type_def.h"
#include <qstring.h>
#include <QTime>
#include <qdir.h>
#include <qlogging.h>


#define _MAX_FNAME  256 /* max. length of file name component */
#define ALARM_LOG_NAME  "Alarm.txt"

class ALARMSHARED_EXPORT Alarm
{

public:
    Alarm();
	virtual BOOL InitInstance();
};

#endif // ALARM_H
