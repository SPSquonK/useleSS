#ifndef __COLLECTING_H__
#define	__COLLECTING_H__

typedef	struct	_COLLECTING_ITEM
{
	DWORD	dwItemId = 0;
	DWORD	dwProb = 0;
}	COLLECTING_ITEM;

class CCollectingProperty
{
public:
	CCollectingProperty();
	virtual	~CCollectingProperty();
	static	CCollectingProperty*	GetInstance();
	BOOL	LoadScript( LPCTSTR szFile );

	int		GetCool( int nAbilityOption );
	int		GetMaxBattery( void )	{	return m_nMaxBattery;	}
	DWORD	GetItem( void );
	int		GetEnchantProbability( int nAbilityOption );
	int		GetMaxCollectorLevel( void )	{	return m_anEnchant.size();	}

#ifdef __WORLDSERVER
	std::vector<int>*	GetEnchantProbabilityPtr( void )	{	return &m_anEnchant;		}
	std::vector<COLLECTING_ITEM>*	GetItemPtr( void )	{	return &m_aItem;	}
#endif	// __WORLDSERVER

private:
	std::vector<int>	m_anCool;
	std::vector<int>	m_anEnchant;
	std::vector<COLLECTING_ITEM>	m_aItem;
	int		m_nMaxBattery;
};
#endif	// __COLLECTING_H__