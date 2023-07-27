#ifndef __LORD_SKILL_H__
#define	__LORD_SKILL_H__

#include "ar.h"
#include <vector>

#ifdef __CLIENT
class CTexture;
#endif	// __CLIENT

class CLordSkillComponent
{
public:
	enum	{	eMaxName	= 32,	eMaxDesc	= 128, eMaxIcon	= 64	};
public:
	CLordSkillComponent(int nId, CScript & script);
	CLordSkillComponent(const CLordSkillComponent &) = delete;
	CLordSkillComponent & operator=(const CLordSkillComponent &) = delete;
	virtual	~CLordSkillComponent() = default;
public:
	[[nodiscard]] int GetId() const noexcept { return m_nId; }
	int		GetCooltime( void )		{	return m_nCooltime;		}
	int		GetItem( void )		{	return m_nItem;		}
	const	char*	GetName( void )	const		{	return m_szName;	}
	const	char*	GetDesc( void )	const	{	return m_szDesc;	}
	const	char*	GetIcon( void )	const	{	return	m_szIcon;	}
	BOOL	IsPassive( void )	{	return m_bPassive;	}
	int		GetTargetType( void )	{	return m_nTargetType;	}
	int		GetTick( void )		{	return m_nTick;		};
	void	SetTick( int nTick )	{	m_nTick		= nTick;	}
#ifdef __CLIENT
	CTexture*	GetTexture( void );
	DWORD	GetSrcSfx( void )	{	return m_dwSrcSfx;	}
	DWORD	GetDstSfx( void )	{	return m_dwDstSfx;	}
#endif	// __CLIENT
#ifndef __DBSERVER
	FLOAT	GetRange( void )	{	return m_fRange;	}
#endif	// __DBSERVER

	int		Tick( void );

private:
	int		m_nId;
	int		m_nCooltime;
	int		m_nTick;
	int		m_nItem;
	char	m_szName[eMaxName];
	char	m_szDesc[eMaxDesc];
	char	m_szIcon[eMaxIcon];
	BOOL	m_bPassive;
	int		m_nTargetType;
	DWORD	m_dwSrcSfx;
	DWORD	m_dwDstSfx;
	FLOAT	m_fRange;
#ifdef __CLIENT
	CTexture*	m_pTexture = nullptr;
#endif	// __CLIENT
};

class CLordSkillComponentExecutable :
	public CLordSkillComponent
{
public:
	CLordSkillComponentExecutable(int nId, CScript & script)
		: CLordSkillComponent(nId, script) {}

	virtual	~CLordSkillComponentExecutable()	{}
protected:
	void	Use()	{	SetTick( GetCooltime() );	}
public:
	virtual	void	Execute( u_long idPlayer, u_long idTarget, VOID* pParam )	{	Use();	}
#ifdef __WORLDSERVER
	virtual	int		IsExecutable( CUser* pUser, const char* szTarget, u_long & idTarget );
	int	IsTeleportable( CUser* pUser, CUser* pTarget );
	void	PutSkillLog( CUser* pUser );
#endif	// __WORLDSERVER
};

////////////////////////////////////////////////////////////////////////////////
class CLord;
class CLordSkill {
public:
	CLordSkill(CLord * pLord);
	virtual ~CLordSkill() = default;
	void Clear() { m_vComponents.clear(); }
	virtual std::unique_ptr<CLordSkillComponentExecutable> CreateSkillComponent(CScript & script);

	CLordSkillComponentExecutable * GetSkill(int nId);

	BOOL	Initialize( const char* szFile );
	void	Reset();

	CAr & WriteTick(CAr & ar) const;
	CAr & ReadTick(CAr & ar);

protected:
	CLord*	m_pLord;
	std::vector<std::unique_ptr<CLordSkillComponentExecutable>> m_vComponents;
};
#endif	// __LORD_SKILL_H__