#ifndef _DCS_MUTEX_H_
#define _DCS_MUTEX_H_

class CDCSMutex
{
public:
	CDCSMutex() 
	{
#ifdef WIN32
		::InitializeCriticalSection(&m_cs);
#else
		pthread_mutex_init(&m_mutex, 0);
#endif		
	}

	~CDCSMutex()
	{
#ifdef WIN32
		::DeleteCriticalSection(&m_cs);
#else
		pthread_mutex_destroy(&m_mutex);
#endif		
	}

	void Lock()
	{
#ifdef WIN32
		::EnterCriticalSection(&m_cs);
#else
		pthread_mutex_lock(&m_mutex);
#endif	
	}

	void Unlock()
	{
#ifdef WIN32
		::LeaveCriticalSection(&m_cs);
#else
		pthread_mutex_unlock(&m_mutex);
#endif	
	}

private:
	CDCSMutex(const CDCSMutex&);
	CDCSMutex& operator=(const CDCSMutex&);

private:
#ifdef WIN32
	CRITICAL_SECTION 	m_cs;
#else
	pthread_mutex_t 	m_mutex;
#endif
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

class CDCSMutexAutoLocker 
{	
public:
	CDCSMutexAutoLocker(CDCSMutex* cs)
		: m_cs(cs)
	{
		if (m_cs) {
			m_cs->Lock();
		}
	}

	~CDCSMutexAutoLocker()
	{
		if (m_cs) {
			m_cs->Unlock();
		}
	}

private:
	CDCSMutex*		m_cs;
};

#endif // #ifdef _DCS_MUTEX_H_
