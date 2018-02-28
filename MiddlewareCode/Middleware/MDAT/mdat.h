/***********************************************
  *@CopyRight：和美（深圳）信息技术股份有限公司
  *@date:2018-01-22 20:44:07
  *@author:xiaoyp
  *@brief:
    应用层数据库模块

***********************************************/
#ifndef MDAT_H
#define MDAT_H

#include "mdat_global.h"
#include <QObject>
#include "win_type_def.h"
#include "dllDSTO.h"

class MDATSHARED_EXPORT CMDAT : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool IsGetDataSyncOK READ GetIsGetDataSyncOK)

public:
    CMDAT(QObject* pParent=NULL);
    ~CMDAT();

    static CMDAT *GetInstance(void);

    // 属性
    bool GetIsGetDataSyncOK(void);

public slots:
    //方法，JS可直接调用
    short AddPersistentData(const QString& DataName,const QString& DataType, const QString& DataValue);
    short GetPersistentData(const QString& DataName,const QString& DataType);
    short SetPersistentData(QString DataName, QString DataType, QString NewDataValue);
    short TimedLock(LONG Timeout);
    QString GetDataSync(QString DataName, QString DataType);
    short SetDataSync(QString DataName, QString DataType, QString NewDataValue);
    short InitDatasSync();
    QString GetPersistentDataSync(QString DataName, QString DataType);
    short SetPersistentDataSync(QString DataName, QString DataType, QString NewDataValue);
    QString GetPrivateProfileSync(QString SectionName, QString KeyName, QString Default, QString FileName);
    short WritePrivateProfileSync(QString SectionName, QString KeyName, QString Value, QString FileName);
    // add by zengwh for 深圳农行三期需求生成json文件供应用使用
    short GetBankCodeJsonData(void);
    short GetProcCodeJsonData(void);
    // add by zengwh for 深圳农行三期需求，微信销户需要MD5值以及二维码图片
    short GetQRCodePic(void);
    short GetMD5Data(void);

signals:
    //信号函数，类似之前的事件回调函数
    void AddPersistentDataComplete(QString DataName);
    void GetPersistentDataComplete(QString DataName, QString DataType, QString DataValue);
    void SetPersistentDataComplete(QString DataName);
    void AddPersistentDataError(QString DataName, short ErrorCode);
    void GetPersistentDataError(QString DataName, short ErrorCode);
    void SetPersistentDataError(QString DataName, short ErrorCode);

private:
    static CMDAT* m_pInstance;
    bool m_IsGetDataSyncOK;
    CDllDSTO dllDSTO;
};

extern "C" {
    MDATSHARED_EXPORT QObject* CreateInstance();
}
#endif // MDAT_H
