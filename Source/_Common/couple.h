#ifndef __COUPLE_H
#define	__COUPLE_H

#include "testcase.h"

class CCoupleTestCase : public ITestCase
{
public:
	CCoupleTestCase()	{}
	virtual	~CCoupleTestCase()	{}
	virtual	BOOL	Test();
};

class CCouple
{
public:
	enum	{	eMaxLevel	= 21,	};
	CCouple();
	CCouple( u_long idFirst, u_long idSecond );
	virtual	~CCouple()	{}
	int		GetExperience()		{	return m_nExperience;	}
	BOOL	AddExperience( int nExperience );
	int		GetLevel( BOOL bCalc = FALSE );
	u_long	GetPartner( u_long idPlayer );
	BOOL	HasPlayer( u_long idPlayer )	{	return m_idFirst == idPlayer || m_idSecond == idPlayer;		}
	void	OnTimer();
	friend CAr & operator<<(CAr & ar, const CCouple & self);
	friend CAr & operator>>(CAr & ar, CCouple & self);
	u_long	GetFirst()	{	return m_idFirst;	}
	u_long	GetSecond()		{	return m_idSecond;	}
private:
	int		m_nExperience;
	int		m_nLevel;
	u_long	m_idFirst;
	u_long	m_idSecond;
};

typedef	struct	_COUPLE_ITEM
{
	int	nItem;
	int nSex;
	int	nFlags;
	int nLife;
	int nNum;
	_COUPLE_ITEM() : nItem( 0 ), nSex( 0 ), nFlags( 0 ), nLife( 0 ), nNum( 0 )		{}
	_COUPLE_ITEM( int i, int s, int f, int l, int n ) : nItem( i ), nSex( s ), nFlags( f ), nLife( l ), nNum( n )		{}
}	COUPLE_ITEM, *PCOUPLE_ITEM;

typedef std::vector<int>		VE;
typedef std::vector<COUPLE_ITEM>		VCI;
typedef	std::vector<VCI>		VVCI;
typedef	std::vector<int>		VSK;
typedef std::vector<int>		VS;
typedef std::vector<VS>		VVS;

typedef	std::map<u_long, CCouple*>	MPC;
typedef	std::vector<CCouple*>	VCOUPLE;

class CCoupleProperty
{
public:
	CCoupleProperty();
	virtual	~CCoupleProperty();
	static	CCoupleProperty*	Instance();
	VCI&	GetItems( int nLevel );
	VS&		GetSkill( int nLevel );
	int		GetLevel( int nExperience );
	BOOL	Initialize();
	int		GetTotalExperience( int nLevel );
	int		GetExperienceRequired( int nLevel );
	float	GetExperienceRate( int nLevel, int nExperience );
	VSK&	GetSKillKinds() { return m_vSkillKinds; };
private:
	void	LoadLevel( CScript & s );
	void	LoadItem( CScript & s );
	void	LoadSkillKind( CScript & s );
	void	LoadSkillLevel( CScript & s );
	void	AddItem( int nLevel, const COUPLE_ITEM & ci );
private:
	VE		m_vExp;
	VVCI	m_vItems;
	VSK		m_vSkillKinds;
	VVS		m_vSkills;
};

class CCoupleMgr
{
public:
	CCoupleMgr();
	virtual	~CCoupleMgr();
	void	Couple( u_long idFirst, u_long idSecond );
	void	Couple( CCouple* pCouple );
	size_t	GetCount()	{	return m_vCouples.size();	}
	BOOL	Decouple( u_long idPlayer );
	CCouple*	GetCouple( u_long idPlayer );
	BOOL	IsCouple( u_long idFirst, u_long idSecond );
	friend CAr & operator<<(CAr & ar, const CCoupleMgr & self);
	friend CAr & operator>>(CAr & ar, CCoupleMgr & self);
	void	OnTimer();
private:
	void	Clear();
private:
	MPC		m_mapPlayers;
	VCOUPLE		m_vCouples;
};
#endif	// __COUPLE_H