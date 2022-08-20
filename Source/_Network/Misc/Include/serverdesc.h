#ifndef __SERVER_DESC_H__
#define __SERVER_DESC_H__

#pragma once

#include "Ar.h"
#include "MyMap.h"

class CJurisdiction {
public:
	DWORD	m_dwWorldID = NULL_ID;
};

#define NULL_POS_ATTR	(short)-32768



class CServerDesc
{
public:
	u_long	m_uIdofMulti;
private:
	u_long	m_uKey;
public:
	char	m_szAddr[16];
	std::list<CJurisdiction*>	m_lspJurisdiction;

public:
//	Constructions
	CServerDesc();
	virtual	~CServerDesc();

//	Operations
	[[nodiscard]] bool IsIntersected(DWORD dwWorldID) const;

//	Attributes
#ifdef __S8_SERVER_PORT
	void	SetKey( u_long uKey )	{	m_uKey	= uKey;		m_uIdofMulti	= uKey % 100;	}
#else // __S8_SERVER_PORT
	void	SetKey( u_long uKey )	{	m_uKey	= uKey;		m_uIdofMulti	= uKey / 100;	}
#endif // __S8_SERVER_PORT
	u_long	GetKey( void )	{	return m_uKey;	}
	u_long	GetIdofMulti( void )	{	return	m_uIdofMulti;	}	

	friend CAr & operator<<(CAr & ar, const CServerDesc & self);
	friend CAr & operator>>(CAr & ar, CServerDesc & self);
};

class CServerDescArray : public std::map<u_long, CServerDesc*>
{
public:
#ifdef __STL_0402
	CMclCritSec		m_AddRemoveLock;
#endif	// __STL_0402

public:
	// Constructions
	CServerDescArray();
	virtual	~CServerDescArray();
	
	void Free( void );

	// Operations
	CServerDesc*	GetAt( ULONG uKey );
};

#endif	// __SERVER_DESC_H__