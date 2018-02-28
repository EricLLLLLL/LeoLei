//////////////////////////////////////////////////////////////////////////////
//文件名        ：KeyMng.h
//创建者        ：张利军
//创建时间      ：2014-12-24 11:02:56
//功能描述      ：密钥管理模块
//
//修改履历      ：2014-12-24  张利军  新建
//
//////////////////////////////////////////////////////////////////////////////
#pragma once

//#include <afxtempl.h>
#include <map>
#include <QString>
#include <QVector>
using namespace std;


//////////////////////////////////////////////////////////////////////////////
//类名          ：m_Length
//创建时间      ：2014-12-24 11:08:59
//功能描述      ：保存单个密钥的结构体
//
//修改履历      ：2014-12-24  张利军  新建
//
//////////////////////////////////////////////////////////////////////////////
struct stSingleKey 
{
	int m_KeyNo;		// 密钥号
	int m_Length;		// 密钥长度
	int m_Use;			// 密钥用途
	int m_EncKeyNo;		// 本密钥的关联主密钥 add yyx
	char m_Name[40];
	stSingleKey()
	{
		m_KeyNo    = -1;
		m_Length   = 0;
		m_Use      = -1;
		m_EncKeyNo = -1;
		memset(m_Name,0,40);
	}
};

//////////////////////////////////////////////////////////////////////////////
//文件名        ：KeyMng.h
//创建者        ：张利军
//创建时间      ：2014-12-24 11:03:59
//功能描述      ：密钥管理类的声明
//
//修改履历      ：2014-12-24  张利军  新建
//
//////////////////////////////////////////////////////////////////////////////
class CKeyMng
{
public:
	CKeyMng(void);
	virtual ~CKeyMng(void);
protected:
    QString m_FileName;      // 保存文件的路径
	int     m_iMaxCount;     // 主密钥的最大数目
	int     m_iSubMaxCount;  // 每个主密钥下的工作密钥的最大数目
	bool*   m_lpKeyExist;    // 记录主密钥的存在状态
	//bool**  m_lppSubKeyExist;// 记录工作密钥的存在状态
	bool  m_lppSubKeyExist[16][16];// 记录工作密钥的存在状态
	bool    m_bIsInitOk;     // 初始化是否完成   

    map<QString, stSingleKey> m_KeyMap; // 保存所有密钥

	void WriteKeyFromSection(stSingleKey &key); // 将一个密钥写进文件

	void ReadKeyFromFile(); // 密钥文件的反序列化

public:
	// 初始化
	// strFileName 数据文件全路径
	// iMaxKey     主密钥的最大数量 iMaxSubKey 主密钥下挂的工作密钥的最大数量
    virtual void Initial(QString strFileName, int iMaxKey, int iMaxSubKey);
	// 反初始化
	virtual void Uninit();

	// 得到 所有的key 的Array
	// excKeyName:例外的密钥名
 //   virtual void GetKeysName(CArray<QString,QString&> &arr_keyname, char* excKeyName = NULL);
    virtual void GetKeysName(QVector<QString> &arr_keyname, char* excKeyName = NULL);
	// 检查是否存在该密钥
	// keyname 密钥名称
    virtual bool CheckKey(QString keyname);
	// 保存密钥
	virtual int  SaveKey(stSingleKey &key);
    virtual stSingleKey* GetKey(QString keyname);

	// 删除单个密钥
    virtual void ClearKey(QString keyname);
	// 删除所有密钥
	virtual void ClearKeys();

	// 得到将要分配的密钥号 或者 已有的密钥号
	// keyname    需要判断的密钥名称
	// lpkey      keyname 的主密钥
    virtual int GetRightKeyNoByName(QString keyname, stSingleKey* lpkey = NULL);

	// 得到key的数量
	inline int GetKeyCount()
	{
		return (int) m_KeyMap.size();
	}

	inline bool ExistSubKey()
	{
		return m_iSubMaxCount>0;
	}
protected:
	// 得到将要分配的主密钥号 
	virtual int GetRightKeyNo();
	// 得到将要分配的工作密钥号 
	// mkeyno 主密钥的寄存器号
	virtual int GetRightSubKeyNo(int mkeyno);

	// 设置 key是否存在的值
	bool SetKeyExist(stSingleKey* lpkey, bool value);

};

