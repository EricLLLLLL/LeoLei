/***********************************************
  *@CopyRight：和美（深圳）信息技术股份有限公司
  *@date:2018-01-29 15:01:43
  *@author:xiaoyp
  *@brief:

  用于程序使用的公共定义
***********************************************/
#ifndef COMMON_DEF_H
#define COMMON_DEF_H
#include <QString>
#include <QDir>
#include <QCoreApplication>

static const QString SQLITE_DB_CONNECTION = "TCRDbConnection"; //数据库连接名
static const QString DB_DATABASE_NAME = "/DB/TCR.db3"; //数据库名

static const QString CONST_INSTALL_DIR = QDir::homePath()+"/"; //安装目录，类似之前的C盘
static const QString OUT_LIB_DIR = "/lib/"; //生成动态库的目录
static const QString DATA_DIR = "/../Data/"; //Data目录

static const QString CONST_SETUPINIFILE ="/lib/ini/Setup.ini"; //相对HMSetup应用程序路径
static const QString CONST_ErrorCode_CFG = "/lib/ini/ErrorCode.ini";


//SP配置文件路径
static const QString hm_crm_limit_amount = "/.hm_crm_limit_amount";


//EasyLog模块
#define _MAX_FNAME  256
#define EXTENSION           ".log"
#define EXPIRATION          "30"
#define CONFIGNAME          "EasyLog.ini"
//end


#endif // COMMON_DEF_H
