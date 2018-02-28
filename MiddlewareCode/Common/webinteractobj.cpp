#include "webinteractobj.h"

CWebInteractObj::CWebInteractObj(const QString& strDBusObjName)
{
    CWSWnd::CreateDBus(strDBusObjName.toStdString().c_str());
    m_apiCtrl.SetDBusName(GetDBusName());
}

CWebInteractObj::~CWebInteractObj()
{
    CWSWnd::DestoryDBus();
}

/*
void CWebInteractObj::SetServiceName(const QString &strServiceName)
{
    m_strServiceName = strServiceName;
}

QString CWebInteractObj::GetServiceName()
{
    return m_strServiceName;
}
*/
