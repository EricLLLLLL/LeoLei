//////////////////////////////////////////////////////////////////////////////
//文件名        ：KeyMng.cpp
//创建者        ：张利军
//创建时间      ：2014-12-24 11:03:37
//功能描述      ：密钥管理模块
//
//修改履历      ：2014-12-24  张利军  新建
//
//////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "KeyMng.h"

#include "easytrace.h"
#include <QFile>


CKeyMng::CKeyMng()
{

	m_iMaxCount      = 0;
	m_iSubMaxCount   = 0;
	m_lpKeyExist     = NULL;
	//m_lppSubKeyExist = NULL;
	memset(m_lppSubKeyExist,0,sizeof(m_lppSubKeyExist));
	m_FileName       = "";
	m_bIsInitOk      = false;
}
CKeyMng::~CKeyMng()
{
	Uninit();
}
// 初始化
void CKeyMng::Initial(QString strFilename, int iMaxKey, int iMaxSubKey)
{
	if (m_bIsInitOk )
	{
		return ;
	}
	m_FileName = strFilename;
	m_iMaxCount    = iMaxKey;
	m_iSubMaxCount = iMaxSubKey;
	if (0 < m_iMaxCount)
	{
		m_lpKeyExist = new bool[m_iMaxCount];
		memset(m_lpKeyExist, 0, m_iMaxCount);
		/*if (0 < m_iSubMaxCount)
		{
			m_lppSubKeyExist = new bool*[m_iMaxCount];
			for (int i=0; i<m_iMaxCount; i++)
			{
				m_lppSubKeyExist[i] = new bool[m_iSubMaxCount];
				memset(m_lppSubKeyExist[i], 0, m_iSubMaxCount);
			}
		}*/
	}
	ReadKeyFromFile();
	m_bIsInitOk = true;
}
// 反初始化
void CKeyMng::Uninit()
{
	LOGINFO("%s,IN",__FUNCTION__);

	m_KeyMap.clear();

	LOGINFO("Map 清空。");

	if (0 < m_iMaxCount)
	{
		delete []m_lpKeyExist;
		m_lpKeyExist = NULL;
		
		LOGINFO("m_lpKeyExist 清空。");

		/*if (0 < m_iMaxCount && 0 < m_iSubMaxCount)
		{
			for (int i=0; i<m_iMaxCount; i++)
			{
				WriteLog("i = %d,m_iMaxCount = %d,m_iSubMaxCount = %d,m_lppSubKeyExist[i] = %d",i,m_iMaxCount,m_iSubMaxCount,m_lppSubKeyExist[i]);

				delete [] m_lppSubKeyExist[i];
				m_lppSubKeyExist[i] = NULL;
			}

			WriteLog("m_lppSubKeyExist[i] 清空。");

			delete []m_lppSubKeyExist;
			m_lppSubKeyExist = NULL;

			WriteLog("m_lppSubKeyExist清空。");
		}*/
	}

	LOGINFO("%s,OUT",__FUNCTION__);
}
// 密钥文件的反序列化
void CKeyMng::ReadKeyFromFile()
{
    int pos = m_FileName.lastIndexOf('\\');
    QString folder = m_FileName.left(pos);
    //int res_dir = SHCreateDirectoryEx(0,folder,0);
    QDir dir;
    dir.mkpath(folder);
//	if (ERROR_PATH_NOT_FOUND == res_dir
//		|| ERROR_FILENAME_EXCED_RANGE == res_dir)
//	{
//		;
//	}

    QFile file(m_FileName);
    if (file.open(QIODevice::ReadOnly))
    {
        while (!file.atEnd())
        {
            int nlen = sizeof(stSingleKey);
            void *buf = new BYTE[nlen];
            memset(buf, 0x00, nlen);
            file.read((char *)buf, nlen);
            stSingleKey *key = (stSingleKey *)buf;
            m_KeyMap.insert(map<QString, stSingleKey>::value_type(key->m_Name, *key));
            SetKeyExist(key, true);
            delete []buf;
        }
        file.close();
    }
    else
    {
        //LOGINFO("%s文件不存在：%s",__FUNCTION__,m_FileName);
    }

    /*
	CFile file;
	if (file.Open(m_FileName,CFile::modeRead))
	{
		file.SeekToBegin();
		while (1)
		{
			if (file.GetPosition() == file.GetLength())
			{
				break;
			}
			int len = sizeof(stSingleKey);
			void *buf = new BYTE[len];
			memset(buf,0,len);
			file.Read(buf,len);
			stSingleKey *key = (stSingleKey*)buf;

            m_KeyMap.insert(map<QString,stSingleKey>::value_type(key->m_Name, *key));

			SetKeyExist(key, true);

			delete []buf;
		}
		file.Close();
	}
	else
	{
		LOGINFO("%s文件不存在：%s",__FUNCTION__,m_FileName);
    }*/
}
// 删除单个密钥
void CKeyMng::ClearKey(QString keyname)
{
	if (m_KeyMap.size() >0 )
	{
        map<QString, stSingleKey>::iterator ikey = m_KeyMap.find(keyname);
		if (ikey != m_KeyMap.end())
		{
			SetKeyExist(GetKey(keyname), false);
			m_KeyMap.erase(ikey);

//			::DeleteFile(m_FileName);
            QFile::remove(m_FileName);
            for(map<QString, stSingleKey>::iterator ikey = m_KeyMap.begin();
				ikey != m_KeyMap.end();
				ikey++)
            {
                /*CFile file;
				if (file.Open(m_FileName,CFile::modeCreate|CFile::modeWrite|CFile::modeRead|CFile::modeNoTruncate))
				{
					file.SeekToEnd();
					file.Write(&(ikey->second),sizeof(stSingleKey));
					file.Close();
                }*/
                QFile file(m_FileName);
                if (file.open(QIODevice::WriteOnly | QIODevice::Append))
                {
                    file.write((const char *)&(ikey->second), sizeof(stSingleKey));
                    file.flush();
                    file.close();
                }
			}
		}
	}
}
// 删除所有密钥
void CKeyMng::ClearKeys()
{
	m_KeyMap.clear();
//	DeleteFile(m_FileName);
    QFile::remove(m_FileName);
	if (0 < m_iMaxCount)
	{
		memset(m_lpKeyExist, 0, m_iMaxCount);
		if (0 < m_iSubMaxCount)
		{
			for (int i=0; i<m_iMaxCount; i++)
			{
				memset(m_lppSubKeyExist[i], 0, m_iSubMaxCount);
			}
		}
	}
}

void CKeyMng::WriteKeyFromSection(stSingleKey &key)
{
    QString secname = key.m_Name;

    QFile file(m_FileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        for (map<QString, stSingleKey>::iterator it = m_KeyMap.begin(); it != m_KeyMap.end(); ++it)
        {
            file.write((const char *)&(it->second), sizeof(stSingleKey));
        }
        file.flush();
        file.close();
    }

 /*
	CFile   file;
	if (file.Open(m_FileName,CFile::modeCreate|CFile::modeWrite|CFile::modeRead|CFile::modeNoTruncate))
	{
		file.SeekToEnd();
		int i = 0;
        for(map<QString, stSingleKey>::iterator ikey = m_KeyMap.begin();
			ikey != m_KeyMap.end();
			ikey++,i++)
		{
// 			if (!ikey->first.Compare(secname))
// 			{
// 				file.Seek(i*sizeof(stSingleKey),CFile::begin);
// 				break;
// 			}
			// 为防止 map的自动排序问题
			file.Seek(i*sizeof(stSingleKey),CFile::begin);
			file.Write(&ikey->second,sizeof(stSingleKey));
		}
		//file.Write(&key,sizeof(stSingleKey));
		file.Close();
    }*/

//	SetFileAttributes(m_FileName,FILE_ATTRIBUTE_HIDDEN);
}

//void CKeyMng::GetKeysName(CArray<QString,QString&> &arr_keyname, char* excKeyName/* = NULL*/)
void CKeyMng::GetKeysName(QVector<QString> &arr_keyname, char *excKeyName/* = NULL*/)
{
    for(map<QString, stSingleKey>::iterator ikey = m_KeyMap.begin();
		ikey != m_KeyMap.end();
		ikey++)
	{
        QString keyName = (QString)ikey->first;
		if (NULL != excKeyName)
		{
            if (keyName == excKeyName)
            {
                continue;
            }
		}
        //arr_keyname.Add(keyName);
        arr_keyname.push_back(keyName);
	}

}
bool CKeyMng::CheckKey(QString keyname)
{
	return m_KeyMap.find( keyname ) != m_KeyMap.end();
}


int CKeyMng::SaveKey(stSingleKey &key)
{
	int keyno       = key.m_KeyNo;
    QString keyname = key.m_Name;
	if (CheckKey(keyname))
	{
		stSingleKey *savekey = GetKey(keyname);
		SetKeyExist(savekey, false);
		savekey->m_KeyNo = savekey->m_KeyNo;//key.m_KeyNo;
		savekey->m_Use= key.m_Use;
		savekey->m_Length = key.m_Length;
		savekey->m_EncKeyNo = key.m_EncKeyNo;
		SetKeyExist(savekey, true);
	}
	else
	{
		SetKeyExist(&key, true);
        strcpy(key.m_Name, keyname.toStdString().c_str());
        m_KeyMap.insert(map<QString,stSingleKey>::value_type(keyname, key));
        //keyname.ReleaseBuffer();
	}

	if (keyno != -1) //成功获取密钥号
	{
		WriteKeyFromSection(m_KeyMap[keyname]);
	}

	return keyno;
}

stSingleKey* CKeyMng::GetKey(QString keyname)
{
    map<QString, stSingleKey>::iterator ikey = m_KeyMap.find(keyname);

	if (ikey == m_KeyMap.end())
	{
		return NULL;
	}
	return &(ikey->second);
}

//  得到将要分配的密钥号 或者 已有的密钥号
int CKeyMng::GetRightKeyNoByName(QString keyname, stSingleKey* lpkey /*= NULL*/)
{
	int keyno = -1;
	if (CheckKey(keyname))
	{
		stSingleKey *savekey = GetKey(keyname);
		keyno = savekey->m_KeyNo;
	}
	else
	{
		if (NULL == lpkey || 0 == m_iSubMaxCount)
		{// 为主密钥
			keyno = GetRightKeyNo(); // 得到一个合适密钥号	
		}
		else
		{// 工作密钥
			keyno = GetRightSubKeyNo(lpkey->m_KeyNo);
		}			
	}

	return keyno;
}
// 得到将要分配的主密钥号 
int CKeyMng::GetRightKeyNo()
{
	for(int i=0; i<m_iMaxCount; i++)
	{
		if (!m_lpKeyExist[i])
		{
			return i;
		}
	}

	return -1;
}

// 得到将要分配的工作密钥号 
int CKeyMng::GetRightSubKeyNo(int mkeyno)
{
	for (int i=0; i < m_iSubMaxCount; i++)
	{
		if (!m_lppSubKeyExist[mkeyno][i])
		{
			return i;		
		}		
	}
	return -1;
}
// 设置 key是否存在的值
bool CKeyMng::SetKeyExist(stSingleKey* lpkey, bool value)
{
	if (NULL == lpkey) return false;
	
	if (-1 == lpkey->m_EncKeyNo)
	{// 主密钥
		if (lpkey->m_KeyNo < m_iMaxCount)
		{
			m_lpKeyExist[lpkey->m_KeyNo] = value;
		}
		else
		{
			return false;
		}
	}
	else
	{// 工作密钥
		if (lpkey->m_KeyNo < m_iSubMaxCount && lpkey->m_EncKeyNo < m_iMaxCount)
		{
			m_lppSubKeyExist[lpkey->m_EncKeyNo][lpkey->m_KeyNo] = value;
		}
		else
		{
			return false;
		}
	}
	
	return true;
}
