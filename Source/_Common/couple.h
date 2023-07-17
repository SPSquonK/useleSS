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
	[[nodiscard]] u_long	GetFirst() const	{	return m_idFirst;	}
	[[nodiscard]] u_long	GetSecond() const		{	return m_idSecond;	}
private:
	int		m_nExperience;
	int		m_nLevel;
	u_long	m_idFirst;
	u_long	m_idSecond;
};

struct	COUPLE_ITEM
{
	int	nItem;
	int nSex;
	int	nFlags;
	int nLife;
	int nNum;
	COUPLE_ITEM() : nItem( 0 ), nSex( 0 ), nFlags( 0 ), nLife( 0 ), nNum( 0 )		{}
	COUPLE_ITEM( int i, int s, int f, int l, int n ) : nItem( i ), nSex( s ), nFlags( f ), nLife( l ), nNum( n )		{}
};

typedef std::vector<int>		VE;
typedef std::vector<COUPLE_ITEM>		VCI;
typedef	std::vector<VCI>		VVCI;
typedef	std::vector<int>		VSK;
typedef std::vector<int>		VS;
typedef std::vector<VS>		VVS;

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

class CCoupleMgr final
{
public:
	void	Couple( u_long idFirst, u_long idSecond );
	void	Couple(std::unique_ptr<CCouple> pCouple);
	[[nodiscard]] size_t GetCount() const noexcept { return m_vCouples.size(); }
	bool	Decouple( u_long idPlayer );
	CCouple*	GetCouple( u_long idPlayer );

	friend CAr & operator<<(CAr & ar, const CCoupleMgr & self);
	friend CAr & operator>>(CAr & ar, CCoupleMgr & self);
	void	OnTimer();
private:
	void	Clear();
private:
	std::map<u_long, CCouple *>           m_mapPlayers;
	std::vector<std::unique_ptr<CCouple>> m_vCouples;
};
#endif	// __COUPLE_H