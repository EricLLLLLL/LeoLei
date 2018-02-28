#ifndef MSYS_H
#define MSYS_H

#include "msys_global.h"
#include <QObject>
#include "win_type_def.h"
#define MSYS_MSG_SHUTDOWN	10
#define MSYS_MSG_REBOOT		11

#define MAX_MODULE_NUM		40			// 模块最多个数
#define DECLARE_HANDLE(name) struct name##__{int unused;}; typedef struct name##__ *name
DECLARE_HANDLE            (HWND);

class MSYSSHARED_EXPORT CMSYS : public QObject
{
    Q_OBJECT

    Q_PROPERTY(BOOL IsBusying READ GetIsBusying)

public:
    CMSYS();
    BOOL GetIsBusying(void);
signals:
    //信号函数，类似之前的事件回调函数
    void ServiceOpened();
    void ServiceClosed();
    void OpenFrontPageComplete();
    void OpenManagePageComplete();
public slots:
    void SetIsBusying(BOOL newVal);
    SHORT Reset(void);
    SHORT ShutDown(void);
    SHORT Reboot(void);
    SHORT OpenService(void);
    SHORT CloseService(void);
    SHORT OpenFrontPage(void);
    SHORT OpenManagePage(void);
    SHORT StartPollingStatusCheckSync();
	SHORT UpTransFile(void);

	SHORT InitialSync(void);
	QString GetPartsStatusSync(void);
    QString PossibleTransactionSync(void);
	SHORT ClearSync(SHORT ClearDiffer);
	SHORT ItemClearSync(SHORT Item);
	SHORT DataSetSync(SHORT Item, LONG Value);
	LONG DataGetSync(SHORT Item);
	QString InfoGetSync(SHORT Item);
private:
	BOOL SendMsg2HMSETUP(int nMsgType);
	BOOL GetIEWindowHwnd();
	BOOL ShowIEWindow(HWND hwnd);
	BOOL HideIEWindow(HWND hwnd);
	HWND m_oplHwnd;
	HWND m_splHwnd;
};
extern "C" {
MSYSSHARED_EXPORT QObject* CreateInstance(void);
}
#endif // MSYS_H
