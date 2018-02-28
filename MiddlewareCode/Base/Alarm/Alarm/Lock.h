#pragma once
#include <qmutex.h>

class CLock
{
public:
    CLock(QMutex* cs)
	{
		m_pCs = cs;
        m_pCs->lock();
	}
	~CLock(void){
        m_pCs->unlock();
	}

protected:
    QMutex* m_pCs;
};
