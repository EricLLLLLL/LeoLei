#include "tracetestdll1.h"
#include "thread"
#include "easytrace.h"
#include "QEventLoop"
#include "QTimer"
using namespace std;

TraceTestDll1::TraceTestDll1()
{

}


void _ThreadProc()
{
  while(1){
      LOGINFO("%s","This is the CUserDll2App trace which was written by easylog.-----------1");
      QEventLoop eventloop;
      QTimer::singleShot(100, &eventloop, SLOT(quit()));
      eventloop.exec();
       break;
  }
}

void _ThreadProc2()
{
  while(1){
      LOGINFO("%s","This is the CUserDll2App trace which was written by easylog.-----------2");
      QEventLoop eventloop;
      QTimer::singleShot(100, &eventloop, SLOT(quit()));
      eventloop.exec();
      break;
  }
}

long userLog2()
{
    std::thread t1(_ThreadProc);
    std::thread t2(_ThreadProc2);
    t1.join();
    t2.join();

    LOGINFO("%s","This is the CUserDll2App trace which was written by easylog.");
    LOGDEBUG("%s","This is the CUserDll2App trace which was written by easylog.");
    LOGERROR("%s","This is the CUserDll2App trace which was written by easylog.");
    LOGINFO("%s","This is the CUserDll2App trace which was written by easylog.");
    LOGINFO("%s","This is the CUserDll2App trace which was written by easylog.");



    QEventLoop eventloop;
    QTimer::singleShot(2000, &eventloop, SLOT(quit()));
    eventloop.exec();
    return 1;
}
