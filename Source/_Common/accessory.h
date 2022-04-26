#pragma once

#include "SingleDst.h"

#define	MAX_AAO		20


class CAccessoryProperty
{
public:
	CAccessoryProperty();
	virtual	~CAccessoryProperty();
	static	CAccessoryProperty*	GetInstance();

	BOOL	LoadScript( LPCTSTR szFile );
	BOOL	IsAccessory( DWORD dwItemId );
	DWORD	GetProbability( int nAbilityOption )	{	return m_adwProbability[nAbilityOption];		}
#ifdef __JEFF_11_3
	LPDWORD		GetProbabilityPtr( void )		{	return	m_adwProbability;	}
#endif	// __JEFF_11_3

	std::vector<SINGLE_DST>*	GetDst( DWORD dwItemId, int nAbilityOption );
private:
	DWORD	m_adwProbability[MAX_AAO];
	std::map<DWORD, std::vector<SINGLE_DST>*>	m_mapAccessory;
};
