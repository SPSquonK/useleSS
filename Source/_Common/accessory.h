#pragma once

#include <ranges>

#define	MAX_AAO		20

struct SINGLE_DST {
	int	nDst;
	int	nAdj;
};

template<typename T>
concept MultipleDsts = std::ranges::range<T> && std::same_as<std::ranges::range_value_t<T>, SINGLE_DST>;


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

	vector<SINGLE_DST>*	GetDst( DWORD dwItemId, int nAbilityOption );
private:
	DWORD	m_adwProbability[MAX_AAO];
	map<DWORD, vector<SINGLE_DST>*>	m_mapAccessory;
};
