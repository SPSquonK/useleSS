#pragma once
#include "Ar.h"

class CEnvironment
{
public:
	static	CEnvironment*	GetInstance();
	
	BOOL	GetEnvironmentEffect()	{	return m_bEffect;	}
	void	SetEnvironmentEffect( BOOL bEffect )	{	m_bEffect = bEffect;	}

#ifdef __WORLDSERVER
	BOOL	LoadScript();

	int		GetSeason();
#endif // __WORLDSERVER

#ifdef __CLIENT
	void	SetSeason( int nSeason );
	int		GetSeason()		{	return m_nSeason;	}
#endif // __CLIENT

private:
#ifdef __WORLDSERVER
	int		m_aEnvironmentEffect[LANG_MAX][12];
#endif // __WORLDSERVER

#ifdef __CLIENT
	int		m_nSeason;
#endif // __CLIENT

	BOOL	m_bEffect;
	CTimer	m_tEffectTime;
public:
//	Constructions
	CEnvironment();
	~CEnvironment();
	friend CAr & operator<<(CAr & ar, const CEnvironment & self);
	friend CAr & operator>>(CAr & ar, CEnvironment & self);

#ifdef __CORESERVER
public:
	BOOL	m_Authority;
public:
	HANDLE	m_hWorker;
	HANDLE	m_hCloseWorker;
	
public:
	BOOL	CreateWorkers( void );
	void	CloseWorkers( void );
	static	UINT	_Worker( LPVOID pParam );
	void	Worker( void );
#endif // __CORESERVER
};

