#pragma once

#include "ProjectCmn.h"
#include "FlyFFTypes.h"

#include <vector>
#include <map>
#include <exception>
#include "Mymap.h"
#include "EventLua.h"
#include "GuildCombat1to1.h"


#define	MAX_WORLD	256


#ifdef __S1108_BACK_END_SYSTEM
#define MAX_RESPAWN 1024
#define MAX_MONSTER_PROP 1024
typedef struct	_MONSTER_RESPAWN
{
	char	szMonsterName[32];
	int		nRespawnIndex;
	D3DXVECTOR3	vPos;
	int		nQuantity;
	int		nAggressive;
	int		nRange;
	int		nTime;
	_MONSTER_RESPAWN()
	{	
		szMonsterName[0] = '\0';
		vPos = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
		nQuantity = 0;
		nAggressive = 0;
		nRange = 0;
		nTime = 0;
		nRespawnIndex = 0;
	}
}
MONSTER_RESPAWN, *PMONSTER_RESPAWN;

typedef struct _MONSTER_PROP
{
	char	szMonsterName[32];
	int		nHitPoint;
	int		nAttackPower;
	int		nDefence;
	int		nExp;
	int		nItemDrop;
	int		nPenya;
	_MONSTER_PROP()
	{
		szMonsterName[0] = '\0';
		nHitPoint = 0;
		nAttackPower = 0;
		nDefence = 0;
		nExp = 0;
		nItemDrop = 0;
		nPenya = 0;
	}
}
MONSTER_PROP, *PMONSTER_PROP;
#endif // __S1108_BACK_END_SYSTEM

class CScript;

class CProject
{
public:
	CProject();
	virtual	~CProject();

private:
	std::map<std::string, DWORD>	m_mapII;

public:
	ItemProp*	GetItemProp( LPCTSTR lpszItem );
	CFixedArray<ItemProp>	m_aPropSkill;
	CFixedArray<ItemProp>	m_aPropItem  ;
	int						m_nMoverPropSize;
	MoverProp*				m_pPropMover;	// m_aPropMover배열에 메모리 침범이 있어서 수정함.04.10.14
	CFixedArray< tagColorText >	m_colorText;

	CEventLua	m_EventLua;

	CGuildCombat1to1Mng m_GuildCombat1to1;

	Project::Jobs jobs;

	EXPPARTY	m_aExpParty[MAX_PARTYLEVEL];
	EXPCHARACTER m_aExpCharacter[MAX_EXPCHARACTER];

	FLOAT m_fItemDropRate;			// 몬스터 아이템 드롭률
	FLOAT m_fGoldDropRate;			// 몬스터 페냐 드롭률
	FLOAT m_fMonsterExpRate;		// 몬스터 경험치룰
	FLOAT m_fMonsterHitRate;		// 몬스터 공격률
	FLOAT m_fShopCost;				// 상점가격
#ifdef __ITEM_REMOVE_LIST
	DWORD	m_dwConvMode;
#endif // __ITEM_REMOVE_LIST
	
#ifdef __S1108_BACK_END_SYSTEM
	FLOAT m_fMonsterRebirthRate;	// 몬스터 리스폰률
	FLOAT m_fMonsterHitpointRate;	// 몬스터 생명력률
	FLOAT m_fMonsterAggressiveRate; // 선공몬스터률
	FLOAT m_fMonsterRespawnRate;	// 몬스터 리스폰률
	BOOL  m_bBaseGameSetting;		// 게임셑팅 완료
	BOOL  m_bBackEndSystem;
	MONSTER_RESPAWN	m_aMonsterRespawn[MAX_RESPAWN];		// Respawn
	MONSTER_RESPAWN n_aAddRespawn[MAX_RESPAWN];
	int				m_nAddMonsterRespawnSize;
	UINT			m_nMonsterRespawnSize;			// Respawn Size
	MONSTER_PROP	m_aMonsterProp[MAX_MONSTER_PROP];	// Monster Prop
	MONSTER_PROP	m_aAddProp[MAX_MONSTER_PROP];	// Monster Prop
	char			m_aRemoveProp[MAX_MONSTER_PROP][32];	// Monster Prop
	int				m_nAddMonsterPropSize;
	int				m_nRemoveMonsterPropSize;
	UINT			m_nMonsterPropSize;		// Monster Prop Size
	char			m_chBackEndSystemTime[15];
	char			m_chBackEndSystemChatTime[15];
	char			m_chGMChat[10][256];
#endif // __S1108_BACK_END_SYSTEM

	BOOL			m_bItemUpdate;		// 
	TCHAR	m_apszWorld[MAX_WORLD][64];
	TCHAR	m_apszWorldName[MAX_WORLD][64];
	std::map<std::string, std::vector<D3DXVECTOR3>> m_mapBeginPos;

#ifdef __RULE_0615
	CNameValider nameValider;
#endif	// __RULE_0615

public:
	void	LoadDefines();
	void	LoadPreFiles();
	void	LoadStrings();
	BOOL	OpenProject( LPCTSTR lpszFileName );
	void	LoadBeginPos( void );
	std::optional<D3DXVECTOR3> GetRandomBeginPos(DWORD dwWorldID) const;
	BOOL	LoadPropMover( LPCTSTR lpszFileName );
	BOOL	LoadPropItem( LPCTSTR lpszFileName, CFixedArray<ItemProp>* apObjProp );
	BOOL	LoadDefOfWorld( LPCTSTR lpszFileName );
	BOOL	LoadText( LPCTSTR lpszFileName );
	LPCTSTR GetText( DWORD dwIndex ) 
	{ 
		if( m_colorText.GetAt( dwIndex ) == NULL )
			return "";
		return m_colorText.GetAt( dwIndex )->lpszData; 
	}
	DWORD GetTextColor( DWORD dwIndex ) 
	{ 
		if( m_colorText.GetAt( dwIndex ) == NULL )
			return 0;
		return m_colorText.GetAt( dwIndex )->dwColor; 
	}

	CString GetLangScript( CScript& script );

#if defined(__DBSERVER) || defined(__VPW)
	BOOL	LoadExpTable( LPCTSTR lpszFileName );
#endif	// __DBSERVER
	ItemProp*	GetItemProp( int nIndex )	{	return m_aPropItem.GetAt( nIndex );	}
	MoverProp*  GetMoverProp( int nIndex ) 
	{ 
		if( nIndex < 0 || nIndex >= m_nMoverPropSize )	
		{
			LPCTSTR szErr = Error( "CProject::GetMoverProp out of range. %d %d", nIndex, m_nMoverPropSize );
			TRACE( szErr );
			return NULL;
		}
		if( m_pPropMover[nIndex].dwID )
			return m_pPropMover + nIndex;
		return NULL;
	}

	ItemProp * GetSkillProp(int nIndex) {
		return m_aPropSkill.GetAt(static_cast<DWORD>(nIndex));
	}
#ifdef __ITEM_REMOVE_LIST
	void SetConvMode( DWORD dwMode );
	void ResetConvMode( DWORD dwMode );
	BOOL IsConvMode( DWORD dwMode );
#endif // __ITEM_REMOVE_LIST
};

inline ItemProp* CProject::GetItemProp( LPCTSTR lpszItem )
{
	const auto i	= m_mapII.find( lpszItem );
	if( i != m_mapII.end() )
		return GetItemProp( i->second );
	return NULL;
}
