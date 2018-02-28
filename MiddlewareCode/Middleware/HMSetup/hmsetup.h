#ifndef HMSETUP_H
#define HMSETUP_H

#include <QWidget>
#include <QWebChannel>

namespace Ui {
class CHMSetup;
}

class QWebEngineView;
class CHMSetup : public QWidget
{
    Q_OBJECT

public:
    explicit CHMSetup(QWidget *parent = 0);
    ~CHMSetup();

    void keyReleaseEvent(QKeyEvent *pEvent);

private:
    void __InitWebObjs();

private:
    Ui::CHMSetup *ui;
    QWebEngineView* m_pWebView;

    typedef QHash<QString,QObject*> HashObj;
    HashObj m_hashWebObjs;

    QWebChannel m_oWebChannel;
};

#endif // HMSETUP_H
