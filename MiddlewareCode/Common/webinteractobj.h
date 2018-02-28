#ifndef WEBINTERACTOBJ_H
#define WEBINTERACTOBJ_H

#include <wswnd.h>
#include <apictrl.h>

class CWebInteractObj : public CWSWnd
{
    Q_OBJECT

    //属性
    //Q_PROPERTY(QString ServiceName READ GetServiceName WRITE SetServiceName)
    //Q_PROPERTY(QString StDetailedDeviceStatus READ GetStDetailedDeviceStatus)

public:
    CWebInteractObj(const QString& strDBusObjName);

    ~CWebInteractObj();

    //设置逻辑名
    virtual void SetServiceName(QString strServiceName) = 0;
    //QString GetServiceName();

    virtual QString GetStDetailedDeviceStatus() = 0;

public slots:
    //方法，JS可直接调用

    //打开连接
    virtual short OpenConnection(void)=0;
    //关闭连接
    virtual short CloseConnection(void)=0;

signals:
    //信号函数，类似之前的事件回调函数
    void ConnectionOpened(void);
    void OpenFailed(void);
    void ConnectionClosed(void);
    void CloseFailed(void);
    void DeviceError(void);
    void Timeout(void);
    void StatusChanged(const QString& PropertyName, const QString& OldValue, const QString& NewValue);

protected:
    QString m_strServiceName;
    CApiCtrl m_apiCtrl;
};

#endif // WEBINTERACTOBJ_H
