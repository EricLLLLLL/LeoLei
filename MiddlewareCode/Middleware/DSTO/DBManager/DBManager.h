/********************************************************************
	created:	2017/06/16  14:14
	filename: 	f:\ICBCMarketingDesktop\ICBCMarketingDesktop\ICBCMarketingDesktop\DBDataManager.h
	file path:	f:\ICBCMarketingDesktop\ICBCMarketingDesktop\ICBCMarketingDesktop
	author:		xiaoyaping
	
	purpose:	DB相关管理
*********************************************************************/
#pragma once
#include "QObject"
#include "QTimer"
#include "qsqldatabase.h"

class CDBManager:public QObject
{
    Q_OBJECT

public:
    CDBManager(QObject* pParent = NULL);
	~CDBManager();

	static CDBManager* GetInstance();

	static void DestroyInstance();

    //创建数据库连接（内部会打开数据库）
	//strConnectionName连接名（如果创建的连接已存在，则之前存在的会被替换掉）
    //strDataBaseName数据库文件名（存在本地的DB文件，如：xxx.db
	bool CreateDBConnection(const QString& strConnectionName,const QString& strDataBaseName);

    //打开当前数据库
    bool OpenDataBase();

    //关闭当前数据库
    void CloseDataBase();

	//移除数据库连接
    void RemoveDBConnection();

	//DB是否连接，在执行DB sql语句之前应该调用该方法检查
    bool IsDBConnected();

	//某个表格是否存在
	//strTableName 表名
    bool IsTableExist(const QString& strTableName);

	//在某个连接对应的数据库中执行sql语句
	//strSqlQuery  要执行的sql语句，如果sql语句中带有变量，请使用ODBC语法，如："INSERT INTO TABLENAME (name,age) VALUES (?,?)"
	//listValue  对应strSqlQuery字段中的"?,?"
	//stOutQuery  输出的执行结果
    bool DBSqlQuery(const QString& strSqlQuery, const QVariantList& listValue, QSqlQuery& stOutQuery);

    bool DBSqlQueryTransactBegin();

    bool DBSqlQueryTransactCommit();

    QString GetLastError();

private slots:
	void slot_OnTimer();


private:
    bool __DelWithQueryErr(const QSqlError& stSqlError);

private:
	static CDBManager* m_pDBDataManager;
	QTimer m_oTimer;
    QString m_strConnectionName;
};

