#include "stdafx.h"
#include "DBManager.h"
#include "QSqlError"
#include "QSqlQuery"
#include "QSqlDriver"
#include "easytrace.h"

CDBManager* CDBManager::m_pDBDataManager = NULL;

CDBManager::CDBManager(QObject* pParent):QObject(pParent)
{
   // connect(&m_oTimer, &QTimer::timeout, this, &CDBManager::slot_OnTimer);
}


CDBManager::~CDBManager()
{
    //m_oTimer.stop();
    CloseDataBase();
    RemoveDBConnection();
}

CDBManager* CDBManager::GetInstance()
{
	if (m_pDBDataManager == NULL)
	{
		m_pDBDataManager = new CDBManager();
	}

	return m_pDBDataManager;
}

void CDBManager::DestroyInstance()
{
	if (m_pDBDataManager)
	{
		delete m_pDBDataManager;
		m_pDBDataManager = NULL;
	}
}

bool CDBManager::CreateDBConnection(const QString& strConnectionName, const QString& strDataBaseName)
{
    /*
	if (QSqlDatabase::connectionNames().empty()) //第一次创建连接时，开启重连机制
	{
		m_oTimer.start(60 * 1000);
    }*/

	QSqlDatabase stOutSqlDataBase = QSqlDatabase::addDatabase("QSQLITE", strConnectionName);
	stOutSqlDataBase.setDatabaseName(strDataBaseName);
	if (!stOutSqlDataBase.open())
	{
        LOGERROR("failed to open database. err:%s",Q2S(stOutSqlDataBase.lastError().text()));
		QSqlDatabase::removeDatabase(strConnectionName);
		return false;
	}
    m_strConnectionName = strConnectionName;

    return true;
}

bool CDBManager::OpenDataBase()
{
    QSqlDatabase stSqlDataBase = QSqlDatabase::database(m_strConnectionName);
    if (!stSqlDataBase.open())
    {
        LOGERROR("failed to open database. err:%s",Q2S(stSqlDataBase.lastError().text()));
        QSqlDatabase::removeDatabase(m_strConnectionName);
        return false;
    }

    return true;
}

void CDBManager::CloseDataBase()
{
    QSqlDatabase stSqlDataBase = QSqlDatabase::database(m_strConnectionName);
    stSqlDataBase.close();
}

void CDBManager::RemoveDBConnection()
{
    QSqlDatabase::removeDatabase(m_strConnectionName);
    m_strConnectionName.clear();

    /*
	if (QSqlDatabase::connectionNames().empty())
	{
		m_oTimer.stop();
    }*/
}

bool CDBManager::IsDBConnected()
{
    if(m_strConnectionName.isEmpty())
    {
        return false;
    }

    QSqlDatabase stDBConn = QSqlDatabase::database(m_strConnectionName,false);
	if (!stDBConn.isValid() || !stDBConn.isOpen())
	{
		return false;
	}

	return true;
}

bool CDBManager::IsTableExist(const QString& strTableName)
{
    QSqlDatabase stDBConn = QSqlDatabase::database(m_strConnectionName,false);
	if (!stDBConn.tables().contains(strTableName,Qt::CaseInsensitive))
	{
		return false;
	}

	return true;
}

bool CDBManager::DBSqlQuery(const QString& strSqlQuery,const QVariantList& listValue, QSqlQuery& stOutQuery)
{
    QSqlDatabase stDB = QSqlDatabase::database(m_strConnectionName);
	if (!stDB.isOpen())
	{
        LOGERROR("%s get DB by connection name failed. ",__FUNCTION__);
		return false;
	}

	QSqlQuery stSqlQuery(stDB);
	stSqlQuery.prepare(strSqlQuery);

    for(int iIndex = 0; iIndex < listValue.count(); iIndex++)
	{
        QVariant varObj = listValue.at(iIndex);
		stSqlQuery.addBindValue(varObj);
	}

    stSqlQuery.setForwardOnly(true);
	if (!stSqlQuery.exec())
	{
        __DelWithQueryErr(stSqlQuery.lastError());
		return false;
	}
	stOutQuery = stSqlQuery;
	
	return true;
}

bool CDBManager::DBSqlQueryTransactBegin()
{
    QSqlDatabase stDB = QSqlDatabase::database(m_strConnectionName);
	if (!stDB.isOpen())
	{
        LOGERROR("%s  get DB by connection name failed. ",__FUNCTION__);
		return false;
	}

	if (!stDB.driver()->hasFeature(QSqlDriver::Transactions))
	{
        LOGERROR("%s  hasn't Transactions feature!",__FUNCTION__);
		return false;
	}

	if (!stDB.transaction()) //启动事务操作
	{
        LOGERROR("transcat start failed!");
		return false;
	}

	//TODO

	//下面执行各种数据库操作
// 	stOutQuery.exec(strSqlQuery);
// 	if (stOutQuery.isSelect() && stOutQuery.isActive())
// 	{
// 		stOutQuery.finish();
// 	}
// 	if (!stDB.commit())
// 	{
// 		qDebug() << stDB.lastError(); //提交
// 		if (!stDB.rollback())
// 			qDebug() << QSqlDatabase::database().lastError(); //回滚
// 	}

	return true;
}

bool CDBManager::DBSqlQueryTransactCommit()
{
    QSqlDatabase stDB = QSqlDatabase::database(m_strConnectionName);
	if (!stDB.isOpen())
	{
        LOGERROR("%s  get DB by connection name failed. ",__FUNCTION__);
		return false;
	}

	bool bSuc = stDB.commit();
	if (!bSuc)
	{
        LOGERROR("commit DB transaction failed.  error:%s" ,Q2S(stDB.lastError().text()));
		if (!stDB.rollback())
		{
            LOGERROR("rollback failed.  error:%s", Q2S(stDB.lastError().text()));
		}
		
		return false;
	}

    return true;
}

QString CDBManager::GetLastError()
{
    QSqlDatabase stDBConn = QSqlDatabase::database(m_strConnectionName);
    return stDBConn.lastError().text();
}

void CDBManager::slot_OnTimer()
{
	QString strQuery = "select 1;";
	QStringList listConnections = QSqlDatabase::connectionNames();
    for(int iIndex = 0; iIndex < listConnections.count();iIndex++)
	{
        QString strConnName = listConnections.at(iIndex);
		QSqlDatabase stDBConn = QSqlDatabase::database(strConnName,false);
		if (!stDBConn.isValid())
		{
            LOGERROR("invalid database connection. This should not happend!  strConnName=%s",Q2S(strConnName));
            RemoveDBConnection();
			continue;
		}

		QSqlQuery stQuery(strQuery, stDBConn);
		if (!stQuery.exec())
		{
            __DelWithQueryErr(stQuery.lastError());
		}
	}
}

bool CDBManager::__DelWithQueryErr(const QSqlError& stSqlError)
{
    QSqlDatabase stDBConn = QSqlDatabase::database(m_strConnectionName,false);
	QSqlError::ErrorType enType = stSqlError.type();
    LOGERROR("%s error:%s",__FUNCTION__,Q2S(stSqlError.text()));
	switch (enType)
	{
	case QSqlError::ConnectionError:
	{
        LOGERROR("DB disconnected! start reconnect!");
		stDBConn.close();
		return stDBConn.open();
	}
	break;

	case QSqlError::StatementError:
	{
		
	}
	break;
	
	case QSqlError::TransactionError:
	{
		return stDBConn.rollback();
	}
	break;

	case QSqlError::UnknownError:
	{

	}
	break;

	default:
		return false;
	}

	return true;
}
