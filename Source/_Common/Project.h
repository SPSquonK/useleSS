#pragma once

#if !(defined(__WORLDSERVER) || defined(__CLIENT))
static_assert(false, "Project.h was included")
#endif

#include <functional>
#include <memory>
#include <set>
#include <boost/container/small_vector.hpp>
#include "boost/container/static_vector.hpp"
#include <boost/container/flat_map.hpp>
#include <span>
#include "sqktd/static_string.h"
#include "FlyFFTypes.h"

#include "SingleDst.h"

#include "defineNeuz.h"
#include "ModelMng.h"
#include "TerrainMng.h"
#include "ObjMap.h" 
#include "ProjectCmn.h"
#include "Script.h" 
#include "guildquest.h"

#include "partyquest.h"

#ifdef __EVE_MINIGAME
#include "MiniGame.h"
#endif // __EVE_MINIGAME

#include "UltimateWeapon.h"

#ifdef __TRADESYS
#include "Exchange.h"
#endif // __TRADESYS

#if defined(__WORLDSERVER) // __EVENTLUA && __WORLDSERVER
#include "EventLua.h"
#endif // __EVENTLUA && __WORLDSERVER

#ifdef __IMPROVE_MAP_SYSTEM
#ifdef __CLIENT
#include "MapInformationManager.h"
#endif // __CLIENT
#endif // __IMPROVE_MAP_SYSTEM

////////////////////////////////////////////////////////////////////////////////////////////////////
// define  
////////////////////////////////////////////////////////////////////////////////////////////////////

#define		MAX_RESPAWN			1536
#define		MAX_MONSTER_PROP	1024
#define		MAX_GUILDAPPELL		5

////////////////////////////////////////////////////////////////////////////////////////////////////
// extern 
////////////////////////////////////////////////////////////////////////////////////////////////////

extern CString GetLangFileName( int nLang, int nType );

////////////////////////////////////////////////////////////////////////////////////////////////////
// struct
////////////////////////////////////////////////////////////////////////////////////////////////////

#define MAX_QUESTPROPITEM  4
#define MAX_QUESTCONDITEM  64
#define MAX_QUESTREMOVE    12

struct QuestState
{
	TCHAR	m_szDesc[ 512 ];
};

struct QuestGoalData {
	DWORD m_dwGoalIndex = 0;
	FLOAT m_fGoalPositionX = 0.0f;
	FLOAT m_fGoalPositionZ = 0.0f;
	DWORD m_dwGoalTextID = 0;
};

struct QuestPropItem 
{
	char    m_nSex;
	char	m_nType; 
	int     m_nJobOrItem;
	int		m_nItemIdx;
	int		m_nItemNum;
#ifdef __JEFF_11
	int		m_nAbilityOption;
#endif	//__JEFF_11
	BYTE	m_byFlag;
	QuestGoalData m_ItemGoalData;
};

struct QuestProp
{
	QuestId	m_wId;
	TCHAR	m_szTitle[ 64 ];
	TCHAR	m_szNpcName[ 32 ];
	QuestId	m_nHeadQuest;
	CHAR	m_nQuestType;
	bool	m_bNoRemove; // TRUE일 경우 삭제 불가 

	// 시작 - 조건 
	int		m_nParam[ 4 ];
	char    m_nBeginCondPreviousQuestType;
	QuestId	m_anBeginCondPreviousQuest[ 6 ];
	QuestId	m_anBeginCondExclusiveQuest[ 6 ];
	char	m_nBeginCondJob[MAX_JOB];
	char	m_nBeginCondJobNum;
	BYTE	m_nBeginCondLevelMax;
	BYTE	m_nBeginCondLevelMin;
	char	m_nBeginCondParty; 
	char 	m_nBeginCondPartyNumComp; 
	char	m_nBeginCondPartyNum; 
	char	m_nBeginCondPartyLeader; 
	char	m_nBeginCondGuild; 
	char 	m_nBeginCondGuildNumComp; 
	WORD	m_nBeginCondGuildNum; 
	char	m_nBeginCondGuildLeader; 
	QuestPropItem* m_paBeginCondItem; 
	char	m_nBeginCondItemNum;
	char	m_nBeginCondSex; 
	int		m_nBeginCondSkillIdx;
	char	m_nBeginCondSkillLvl;
	QuestPropItem* m_paBeginCondNotItem; 
	char	m_nBeginCondNotItemNum;
	int		m_nBeginCondPKValue;
	int		m_nBeginCondDisguiseMoverIndex;
	// 시작 - 추가 
	int  	m_nBeginSetAddItemIdx[ 4 ];
	char	m_nBeginSetAddItemNum[ 4 ];
	int		m_nBeginSetAddGold;
	int     m_nBeginSetDisguiseMoverIndex;
	int		m_nBeginCondPetLevel;
	int		m_nBeginCondPetExp;
	int		m_nBeginCondTutorialState;
	int		m_nBeginCondTSP;
	// 종료 - 조건  
	int     m_nEndCondLimitTime; //  퀘스트 수행 제한 시간 
	QuestPropItem* m_paEndCondItem; 
	char	m_nEndCondItemNum;
	int     m_nEndCondKillNPCIdx[ 2 ]; // 죽여야할 NPC 인덱스 - 총 2개 
	int     m_nEndCondKillNPCNum[ 2 ]; // 죽여야할 NPC 갯수 - 총 2개    
	QuestGoalData m_KillNPCGoalData[ 2 ]; // 죽여야 할 NPC 목표 데이터
	DWORD   m_dwEndCondPatrolWorld; // 정찰해야될 맵 
	DWORD	m_dwPatrolDestinationID;	// 정찰 목적지 이름을 찾기 위한 ID
	QuestGoalData m_PatrolWorldGoalData; // 정찰해야 할 지역 목표 데이터
	CRect   m_rectEndCondPatrol; // 정찰해야될 장소 영역 
	CHAR    m_szEndCondCharacter[64]; // 퀘스트를 완수를 판단할 캐릭터 키(NULL이면 자신)
	QuestGoalData m_MeetCharacterGoalData; // 퀘스트 완수를 판단할 캐릭터 목표 데이터
	CHAR*   m_lpszEndCondMultiCharacter; 
	int		m_nEndCondSkillIdx;
	char	m_nEndCondSkillLvl;
	QuestPropItem* m_paEndCondOneItem;
	char	m_nEndCondOneItemNum;
	int		m_nEndCondGold;
	BYTE	m_nEndCondLevelMin;	
	BYTE	m_nEndCondLevelMax;
	int		m_nEndCondExpPercentMin;	
	int		m_nEndCondExpPercentMax;

	int		m_nEndCondPetLevel;
	int		m_nEndCondPetExp;

	int		m_nEndCondDisguiseMoverIndex; // 변신 
	char	m_nEndCondParty; // 파티 여부 (솔로,파티,길드)  
	char 	m_nEndCondPartyNumComp; 
	WORD 	m_nEndCondPartyNum; 
	char	m_nEndCondPartyLeader;
	char	m_nEndCondGuild; 
	char 	m_nEndCondGuildNumComp; 
	WORD	m_nEndCondGuildNum; 
	char	m_nEndCondGuildLeader; 
	BYTE	m_nEndCondState; // 스테이트 여부  
	BYTE    m_nEndCondCompleteQuestOper; // 0 = or, 1 = and
	QuestId	m_nEndCondCompleteQuest[ 6 ]; // 완료 퀘스트 여부. 시나리오 퀘스트에 필요 
	CHAR    m_szEndCondDlgCharKey[ 64 ];
	QuestGoalData m_DialogCharacterGoalData; // 대화해야 할 캐릭터 목표 데이터
	CHAR    m_szEndCondDlgAddKey[ 64 ];
	CHAR    m_szPatrolZoneName[ 64 ];
	int		m_nEndCondTSP;
	// EndDialog 대화시 지급할 아이템(일종의 보상)
	int		m_nDlgRewardItemIdx[ 4 ];
	int		m_nDlgRewardItemNum[ 4 ];
	// 종료 - 제거 
	int		m_nEndRemoveItemIdx[ 8 ];
	int		m_nEndRemoveItemNum[ 8 ];
	int		m_nEndRemoveGold;
	int		m_anEndRemoveQuest[ MAX_QUESTREMOVE ];
	int		m_nEndRemoveTSP;
	// 보상 
	QuestPropItem* m_paEndRewardItem; 
	int		m_nEndRewardItemNum;
	int		m_nEndRewardGoldMin;
	int		m_nEndRewardGoldMax;
	int		m_nEndRewardExpMin;
	int		m_nEndRewardExpMax;
	int		m_nEndRewardPKValueMin;
	int		m_nEndRewardPKValueMax;
	int		m_nEndRewardTeleport;
	D3DXVECTOR3		m_nEndRewardTeleportPos;
	BOOL	m_bEndRewardPetLevelup;
	int		m_nEndRewardSkillPoint;
	bool    m_bEndRewardHide; // 보상 목록을 보일지 여부
	bool    m_bRepeat;
	int		m_nEndRewardTSP;

#if defined( __WORLDSERVER ) 
	CHAR*		m_apQuestDialog[ 32 ];
#endif
	QuestState*	m_questState[ 16 ];

	[[nodiscard]] static const QuestProp * Get(const QuestId questId);
};

struct FILTER {
	TCHAR	m_szSrc[ 64 ];
	TCHAR	m_szDst[ 64 ];
};


struct STRUCTURE {
	TCHAR	szName[ 32 ];
	TCHAR	szEName[ 32 ];
};

struct GUILD_APPELL {
	TCHAR	szName[ 32 ];
	TCHAR	szEName[ 32 ];
};

struct FXPCHARACTER {
	DWORD	dwFxp;
	DWORD	dwFxp2;
};

struct ATKSTYLE {
	DWORD	dwParam1;
	DWORD dwParam2;
};



struct MotionProp
{
	DWORD	dwID;   
	DWORD	dwAction;
	DWORD	dwMotion;
	DWORD	dwPlay;				// 0:1play    1:cont		2:loop
	DWORD	dwRequireLv;		// 여구 레벨 
	DWORD	dwRequireExp;		// 요구 경험치 
	TCHAR	szName[ 32 ];		// 표시 단어 
	TCHAR	szRoot[ 32 ];		// 루트 명칭 
	TCHAR	szLink[ 128];		// 링크 단어 
	TCHAR	szIconName[ 32 ];	// 아이콘 파일 네임 
	TCHAR	szDesc[ 128 ];		// 설명 
	CTexture*	pTexture;
};

struct CVendor {
	enum class Type { Penya, RedChip };

	struct CategoryItem : DROPKIND {
		int		m_nItemJob;

		CategoryItem(DWORD nItemkind3, int nItemJob, short nUniqueMin, short nUniqueMax) {
			dwIK3 = nItemkind3;
			nMinUniq = nUniqueMin;
			nMaxUniq = nUniqueMax;
			m_nItemJob = nItemJob;
		}
	};

	struct SingleItem {
		DWORD	m_dwItemId;
		SingleItem(DWORD dwItemId) : m_dwItemId(dwItemId) {}
	};

	/* 0 = penya, 1 = red chip */
	Type m_type = Type::Penya;

	/* Shop name */
	CString			m_venderSlot[4];

	// List of sold items, Only populated for penya items?
	std::vector<CategoryItem>		m_venderItemAry[4];

	// List of sold items, Only populated for red chip items?
	std::vector<SingleItem>   m_venderItemAry2[4];
};

#ifdef __NPC_BUFF
struct NPC_BUFF_SKILL {
	DWORD	dwSkillID;
	DWORD	dwSkillLV;
	int		nMinPlayerLV;
	int		nMaxPlayerLV;
	DWORD	dwSkillTime;
};
#endif // __NPC_BUFF

typedef struct tagCHARACTER
{
	CHAR			m_szKey[ 64 ];
	CString			m_strName;
	CHAR			m_szChar[ 64 ];
	CHAR			m_szDialog[ 64 ];
	CHAR			m_szDlgQuest[ 64 ];		// TODO_raiders: 사용하지 않는다. 제거 
	int				m_nStructure;
	int				m_nEquipNum;
	DWORD			m_dwMoverIdx;
	DWORD			m_dwMusicId;
	DWORD			m_adwEquip[ MAX_HUMAN_PARTS ];
	DWORD			m_dwHairMesh;
	DWORD			m_dwHairColor;
	DWORD			m_dwHeadMesh;
	BOOL			m_abMoverMenu[ MAX_MOVER_MENU ];

	std::vector<std::pair<QuestId, DWORD>> m_srcQuests;
	std::vector<std::pair<QuestId, DWORD>> m_dstQuests;

	void Clear();

	CVendor m_vendor;

#ifdef __NPC_BUFF
	std::vector<NPC_BUFF_SKILL> m_vecNPCBuffSkill;
#endif // __NPC_BUFF
#ifdef __CHIPI_DYO
	std::vector<DWORD>	m_vecdwLanguage;
	BOOL			bOutput;
#endif // __CHIPI_DYO
	DWORD			m_dwWorldId;
	D3DXVECTOR3		m_vPos;
	std::vector<D3DXVECTOR3> m_vecTeleportPos;
} CHARACTER,* LPCHARACTER;


typedef struct	_MONSTER_RESPAWN
{
	char		szMonsterName[32];
	int			nRespawnIndex;
	D3DXVECTOR3	vPos;
	int			nQuantity;
	int			nAggressive;
	int			nRange;
	int			nTime;

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

struct DIE_PENALTY {
	int		nLevel = 0;
	int		nValue = 0;
};

struct CHAO_PROPENSITY {
	DWORD	dwPropensityMin = 0;		/// 성향 수치 Min
	DWORD	dwPropensityMax = 0;		/// 성향 수치 Max
	int		nEquipDorpMin = 0;			/// 착용아이템 드롭 개수 Min
	int		nEquipDorpMax = 0;			/// 착용아이템 드롭 개수 Max
	int		nInvenDorpMin = 0;			/// 인벤아이템 드롭 개수 Min
	int		nInvenDorpMax = 0;			/// 인벤아이템 드롭 개수 Max
	int		nShop = 1;					    /// 상점 이용 유/무
	int		nBank = 1;					    /// 창고 이용 유/무
	int		nFly = 1;					      /// 비행 이용 유/무
	int		nGuardAttack = 1;		  	/// 경비병 공격 유/무
	int		nVendor = 1;			    	/// 개인상점 이용 및 개설 유/무
};

struct PK_SETTING {
	DWORD	dwGeneralColor = 0;			/// 색 : 일반(흰색)
	DWORD	dwReadyColor = 0;			/// 색 : 준카오(핑크색)
	DWORD	dwChaoColor = 0;			/// 색 : 카오(레드색)
	int		nGeneralAttack = 0;			/// 일반인을 공격했을시 준카오가 되는 시간
	int		nReadyAttack = 0;			/// 준카오를 공격했을시 준카오가 되는 시간
	int    nLimitLevel = 0;			/// 카오를 할수 있는 레벨
	int		nDiePropensityMin = 0;		/// 죽으면 성향치 깍이는 Min
	int		nDiePropensityMax = 0;		/// 죽으면 성향치 깍이는 Max
	std::map<int, CHAO_PROPENSITY> mapPropensityPenalty;	/// 성향수치 단계별 Penalty
	std::map<int, DWORD> mapLevelExp;					/// 레벨에 따른 경험치
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// enum 
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
// macro and inline 
////////////////////////////////////////////////////////////////////////////////////////////////////

inline void LOG_RANGE( const char* szMsg, int nMin, int nMax, int nIndex )
{
	LPCTSTR szErr = Error( _T( "%s min:%d, max:%d, index:%d" ), szMsg, nMin, nMax, nIndex ); 
	//ADDERRORMSG( szErr ); 
}

inline void LOG_CALLSTACK()
{
#ifdef _DEBUG
	__asm int 3
#endif

#if defined(__INTERNALSERVER)
	int *p = NULL;		// 콜스택을 추적하기 위해 죽인다
	*p = 1;
#endif
}

#define VERIFY_RANGE( nIndex, nMin, nMax, szMsg, result )  \
	do { \
		if( nIndex < (nMin) || nIndex >= (nMax) ) \
		{  \
			LOG_RANGE( szMsg, nMin, nMax, nIndex ); \
			return (result); \
		}  \
	} while (0)

#define VERIFY_RANGE_ASSERT( nIndex, nMin, nMax, szMsg, result )  \
	do { \
		if( nIndex < (nMin) || nIndex >= (nMax) ) \
		{  \
			LOG_RANGE( szMsg, nMin, nMax, nIndex ); \
			LOG_CALLSTACK(); \
			return (result); \
		}  \
	} while (0)


////////////////////////////////////////////////////////////////////////////////////////////////////
// KARMAPROP 
////////////////////////////////////////////////////////////////////////////////////////////////////


#ifdef __WORLDSERVER

#include <boost/container/flat_map.hpp>

class	CGiftboxMan {
public:
	static constexpr DWORD MaxSum = 1000000;
	
	/// If true, boxes with more than MaxSum probabilities
	/// will be rebalanced over MaxSum.
	/// If false, boxes with more than MaxSub probabilities
	/// will make WorldServer crash on load.
	static constexpr bool Rebalance = true;

	struct Item {
		DWORD	dwItem = 0;
		int		nNum = 0;
		BYTE nFlag = 0;
		int nSpan = 0;
		int	nAbilityOption = 0;
	};

	struct PossibleItem {
		Item concreteItem;
		DWORD	adwProbability;

		PossibleItem(Item concreteItem, DWORD	adwProbability)
			: concreteItem(concreteItem), adwProbability(adwProbability) {}
	};

	struct Box {
		std::vector<PossibleItem> items; // Must be ordered by adwProbability

		[[nodiscard]] std::optional<Item> DrawAnItem() const;
		void Rebalance();
	};

	boost::container::flat_map<DWORD, Box>	m_giftBoxes;

public:
	[[nodiscard]] std::optional<Item> Open(DWORD dwGiftBox) const;
	
	[[nodiscard]] bool Load(LPCTSTR lpszFileName);

private:
	[[nodiscard]] bool AddItem(DWORD dwGiftbox, DWORD dwItem, DWORD dwProbability, int nNum, BYTE nFlag = 0, int nSpan = 0, int nAbilityOption = 0);
	void Optimize();
};

extern CGiftboxMan g_GiftboxMan;


class CPackItem final {
public:
	struct PackedItem {
		DWORD dwItem;
		int nAbilityOption;
		int nNum;
	};

	struct PACKITEMELEM {
		int		nSpan = 0;
		std::vector<PackedItem> aItems;
	};

private:
	boost::container::flat_map<DWORD, PACKITEMELEM> m_packs;

public:
	void Load(LPCTSTR lpszFileName) noexcept(false);
	const PACKITEMELEM * Open(DWORD dwPackItem) const;
};

extern CPackItem g_PackItem;

#endif	// __WORLDSERVER


#define	MAX_ITEMAVAIL	32
using ITEMAVAIL = boost::container::static_vector<SINGLE_DST, MAX_ITEMAVAIL>;

class CSetItem final {
public:
	static constexpr size_t SetItemNameSize = 64;
	static constexpr size_t ElemSize = 8;

	struct PartItem { int part; DWORD itemId; };

	struct EquipedDst {
		int nDst;
		int nAdj;
		int anEquiped;
	};

public:
	int		m_nId;
	StaticString<SetItemNameSize> m_pszString;
	boost::container::static_vector<PartItem, ElemSize>       m_components;
	boost::container::small_vector<EquipedDst, MAX_ITEMAVAIL> m_avail;

public:
	CSetItem(const int nId, const char * const pszString)
		: m_nId(nId) {
		m_pszString = pszString;
	}

	bool AddSetItemElem(DWORD dwItemId, int nParts);
	bool AddItemAvail(int nDstParam, int nAdjParam, int nEquiped);
	void SortItemAvail();

	[[nodiscard]] ITEMAVAIL GetItemAvail(int nEquiped, bool bAll) const;
	[[nodiscard]] const char * GetString() const { return m_pszString; }
};

class CSetItemFinder final {
private:
	std::map<DWORD, const CSetItem *>        m_mapItemId;
	std::map<int, std::unique_ptr<CSetItem>> m_mapSetId;
public:
	void AddSetItem(std::unique_ptr<CSetItem> pSetItem);

	const CSetItem * GetSetItem(int nSetItemId) const;
	const CSetItem * GetSetItemByItemId(DWORD dwItemId) const;
};

extern CSetItemFinder g_SetItemFinder;

typedef struct	_SETITEMAVAIL
{
	int		nHitRate;
	int		nBlock;
	int		nMaxHitPointRate;
	int		nAddMagic;
	int		nAdded;
}	SETITEMAVAIL, *PSETITEMAVAIL;

struct PIERCINGAVAIL final {
	static constexpr size_t MAX_PIERCING_DSTPARAM = 32;

	DWORD	dwItemId;
	boost::container::static_vector<SINGLE_DST, MAX_PIERCING_DSTPARAM> params;
};

class CPiercingAvail final {
public:
	static constexpr size_t MAX_PIERCING_MATERIAL = 128;

private:
	std::map<DWORD, size_t> m_itemIdToPosition;
	boost::container::static_vector<PIERCINGAVAIL, MAX_PIERCING_MATERIAL> m_pPiercingAvail;

public:
	bool AddPiercingAvail(DWORD dwItemId, int nDstParam, int nAdjParam);
	[[nodiscard]] const PIERCINGAVAIL * GetPiercingAvail(DWORD dwItemId) const;
};

extern CPiercingAvail g_PiercingAvail;

class CRandomOptItemGen final {
public:
	static constexpr size_t MAXNAME = 32;

	struct RandomOptItem {
		int nId = 0;
		int nLevel = 0;
		StaticString<MAXNAME> pszString;
		DWORD dwProbability = 0;
		ITEMAVAIL ia;
	};

private:
	boost::container::small_vector<RandomOptItem, 256> m_aRandomOptItem;
	std::map<int, int> m_mapid;
	std::array<int, MAX_MONSTER_LEVEL> m_anIndex = {};

public:
	void AddRandomOption(const RandomOptItem & pRandomOptItem);
	void Arrange();

	[[nodiscard]] const RandomOptItem * GetRandomOptItem(int nId) const;
	[[nodiscard]] const char * GetRandomOptItemString(int nId) const;
	[[nodiscard]] int GenRandomOptItem(int nLevel, FLOAT fPenalty, const ItemProp* pItemProp, DWORD dwClass) const;
};

extern CRandomOptItemGen g_RandomOptItemGen;


////////////////////////////////////////////////////////////////////////////////////////////////////
// typedef 
////////////////////////////////////////////////////////////////////////////////////////////////////

typedef std::map<u_long, std::string>	ULONG2STRING;

////////////////////////////////////////////////////////////////////////////////////////////////////
// CProject
////////////////////////////////////////////////////////////////////////////////////////////////////

class CObj;
class CCtrl;
class CSfx;
class CItem;
class CMover;
class CShip;
#ifdef __WORLDSERVER
class CUser;
#endif

class CProject  
{ 
public:
	CProject();
	virtual ~CProject();

private:
	std::map<std::string, DWORD>			m_mapII;
	std::map<std::string, DWORD>			m_mapMVI;
	std::map<std::string, DWORD>			m_mapCtrl;
#ifdef __CLIENT
	CDWordArray					m_aStateQuest; // 스테이트 사용 여부를 체크할 배열 
#endif

#if defined( __CLIENT )
	std::map< int, CString >			m_mapQuestDestination;		// 퀘스트 목적지 설명
	std::map< int, CString >			m_mapPatrolDestination;		// 정찰 목적지 이름
#endif // defined( __IMPROVE_QUEST_INTERFACE ) && defined( __CLIENT )

public:
	static FLOAT				m_fItemDropRate;			// 몬스터 아이템 드롭률
	static FLOAT				m_fGoldDropRate;			// 몬스터 페냐 드롭률
	static FLOAT				m_fMonsterExpRate;			// 몬스터 경험치룰
	static FLOAT				m_fMonsterHitRate;			// 몬스터 공격률
	static FLOAT				m_fSkillExpRate;		
	static DWORD				m_dwVagSP;					// 방랑자 스킬의 레벨업때 필요한 SP포인트
	static DWORD				m_dwExpertSP;				// 1차직업 스킬의 레벨업때 필요한 SP포인트
	static DWORD				m_dwProSP;					// 2차직업 스킬의 레벨업때 필요한 SP포인트
	static DWORD m_dwLegendSP;

	std::map<u_long, CMoverPlayer *>		m_idPlayerToUserPtr;
	CObjMap						m_objmap;
	CModelMng					m_modelMng;
	int							m_nMoverPropSize;
	MoverProp*					m_pPropMover;				// m_aPropMover배열에 메모리 침범이 있어서 수정함.04.10.14
	CFixedArray< ItemProp >		m_aPartySkill;
	CFixedArray< CtrlProp >		m_aPropCtrl;
	CFixedArray< MotionProp >	m_aPropMotion;
	CFixedArray< ItemProp >		m_aPropItem;
	CFixedArray< ItemProp >		m_aPropSkill;
	CFixedArray< AddSkillProp > m_aPropAddSkill;
	CFixedArray< tagColorText >	m_colorText;
	std::map<std::string, std::string>	m_mapAlphaFilter;
	std::map<std::string, std::string>	m_mapNonalphaFilter;
	CFixedArray< QuestProp >	m_aPropQuest ;
	CFixedArray<GUILDQUESTPROP>	m_aPropGuildQuest;
	CMapStringToPtr				m_mapCharacter;
	
	Project::Jobs jobs;

	STRUCTURE					m_aStructure[ MAX_STRUCTURE ];
	GUILD_APPELL				m_aGuildAppell[ MAX_GUILDAPPELL ];
	EXPCHARACTER				m_aExpCharacter[ MAX_EXPCHARACTER ];
	FXPCHARACTER				m_aFxpCharacter[ MAX_FXPCHARACTER ];
	DWORD						m_aExpLPPoint[ MAX_EXPLPPOINT ];
	DWORD						m_aExpSkill[ MAX_EXPSKILL ];
	EXPPARTY					m_aExpParty[MAX_PARTYLEVEL];
	
	std::vector<ItemProp *> m_itemKindAry[MAX_ITEM_KIND3];
	SIZE m_minMaxIdxAry[MAX_ITEM_KIND3][MAX_UNIQUE_SIZE];

	int							m_aExpUpItem[6][11];	// +0 ~ +10까지의 추가능력치.
	DWORD						m_adwExpDropLuck[122][11];
	SETITEMAVAIL				m_aSetItemAvail[11];
	std::map<int, PARTYQUESTPROP>	m_propPartyQuest;
	std::vector< DIE_PENALTY >		m_vecRevivalPenalty;
	std::vector< DIE_PENALTY >		m_vecDecExpPenalty;
	std::vector< DIE_PENALTY >		m_vecLevelDownPenalty;
	PK_SETTING					m_PKSetting;
	std::set<DWORD>					m_setExcept;

#ifdef __CLIENT
	CTerrainMng					m_terrainMng;
	std::map<std::string, std::string, std::less<>> m_mapHelp;
#endif
	
	FLOAT						m_fMonsterRebirthRate;	// 몬스터 리스폰률(시간)
	FLOAT						m_fMonsterHitpointRate;	// 몬스터 생명력률
	FLOAT						m_fMonsterAggressiveRate;	// 선공몬스터률
	FLOAT						m_fMonsterRespawnRate;	// 몬스터 리스폰률(몬스터숫자)
	MONSTER_PROP				m_aMonsterProp[MAX_MONSTER_PROP];	// Monster Prop
	MONSTER_PROP				m_aAddProp[MAX_MONSTER_PROP];	// Monster Prop
	char						m_aRemoveProp[MAX_MONSTER_PROP][32];	// Monster Prop
	int							m_nAddMonsterPropSize;
	int							m_nRemoveMonsterPropSize;
	int							m_nMonsterPropSize;
	char						m_chGMChat[10][256];
	
	int				m_nEnchantLimitLevel[3];
	float			m_fEnchantLevelScal[2][10];

#ifdef __EVE_MINIGAME
	CMiniGame		m_MiniGame;			// 미니게임
#endif // __EVE_MINIGAEM

	CUltimateWeapon	m_UltimateWeapon;
	
#ifdef __TRADESYS
	CExchange m_Exchange;
#endif // __TRADESYS

#if defined(__WORLDSERVER) // __EVENTLUA && __WORLDSERVER
	CEventLua m_EventLua;
#endif // __EVENTLUA && __WORLDSERVER
	
#ifdef __YS_CHATTING_BLOCKING_SYSTEM
#ifdef __CLIENT
	enum { BLOCKING_NUMBER_MAX = 1000 };
	std::set< CString > m_setBlockedUserID;
#endif // __CLIENT
#endif // __YS_CHATTING_BLOCKING_SYSTEM

#ifdef __CLIENT
#ifdef __SHOP_COST_RATE
public:
	float	m_fShopBuyRate;
	float	m_fShopSellRate;
#endif // __SHOP_COST_RATE
#endif // __CLIENT

#ifdef __IMPROVE_MAP_SYSTEM
#ifdef __CLIENT
	CMapInformationManager m_MapInformationManager;
	BOOL m_bMapTransparent;
#endif // __CLIENT
#endif // __IMPROVE_MAP_SYSTEM

public:
	BOOL			LoadPropEnchant( LPCTSTR lpszFileName );
	static void		ReadConstant( CScript& script );
	static BOOL		LoadConstant( LPCTSTR lpszFileName );
	static void		SetGlobal( UINT type, float fValue );

	PSETITEMAVAIL	GetSetItemAvail( int nAbilityOption );
	[[nodiscard]] std::span<const ItemProp * const> GetItemKind3WithRarity(DROPKIND dropKind) const;
	ObjProp*		GetProp( int nType, int nIndex );
	GUILDQUESTPROP*	GetGuildQuestProp( int nQuestId );
	PARTYQUESTPROP*	GetPartyQuestProp( int nQuestId );
	BOOL			IsGuildQuestRegion( const D3DXVECTOR3 & vPos );
	CtrlProp*		GetCtrlProp( int nIndex );
	ItemProp*		GetItemProp( int nIndex ); 
	ItemProp*		GetSkillProp( int nIndex ); 
	MoverProp*		GetMoverProp( int nIndex ) ;
	MoverProp*		GetMoverPropEx( int nIndex );
	DWORD			GetSkillPoint( const ItemProp* pSkillProp ) const;
	ItemProp*		GetItemProp( LPCTSTR lpszItem );
	MoverProp*		GetMoverProp( LPCTSTR lpszMover ); 
	CtrlProp*		GetCtrlProp( LPCTSTR lpszMover );
	MotionProp*		GetMotionProp ( int nIndex ) { return m_aPropMotion.GetAt( nIndex ); }
	int				GetExpUpItem( DWORD dwItemKind3, int nOption ); 
	AddSkillProp*	GetAddSkillProp( DWORD dwSubDefine, DWORD dwLevel );
	AddSkillProp*	GetAddSkillProp( DWORD dwSubDefine );
	ItemProp*		GetPartySkill ( int nIndex ) { return m_aPartySkill.GetAt( nIndex ); }
	void			LoadDefines();
	void			LoadPreFiles();
	void			LoadStrings();
	BOOL			OpenProject( LPCTSTR lpszFileName );
	BOOL			LoadPropMover( LPCTSTR lpszFileName );
	BOOL			LoadPropItem( LPCTSTR lpszFileName, CFixedArray< ItemProp >* apObjProp, LoadPropItemStyle style = LoadPropItemStyle::V15);
	void			OnAfterLoadPropItem();
	BOOL			LoadPropSfx( LPCTSTR lpszFileName );
	BOOL			LoadPropCtrl( LPCTSTR lpszFileName, CFixedArray<CtrlProp>*	apObjProp);
	BOOL			LoadMotionProp( LPCTSTR lpszFileName );
	BOOL			LoadText( LPCTSTR lpszFileName );
	BOOL			LoadExpTable( LPCTSTR lpszFileName );
	BOOL			LoadPropMoverEx_AI_SCAN( LPCTSTR szFileName, CScript &script, int nVal );
	BOOL			LoadPropMoverEx_AI_BATTLE( LPCTSTR szFileName, CScript &script, int nVal );
	BOOL			LoadPropMoverEx_AI_MOVE( LPCTSTR szFileName, CScript &script, int nVal );
	BOOL			LoadPropMoverEx_AI( LPCTSTR szFileName, CScript &script, int nVal );
	BOOL			LoadPropMoverEx( LPCTSTR szFileName );
	CString			GetLangScript( CScript& script );
	BOOL			LoadCharacter( LPCTSTR szFileName );
	BOOL			LoadEtc( LPCTSTR szFileName );
	BOOL			LoadPropAddSkill( LPCTSTR lpszFileName );
	void			InterpretRandomItem( CScript& script );
	BOOL			LoadScriptDiePenalty( LPCTSTR lpszFileName );
	BOOL			LoadScriptPK( LPCTSTR lpszFileName );
	BOOL			LoadPropQuest( LPCTSTR szFileName, BOOL bOptimize = TRUE );
	BOOL			LoadPropGuildQuest( LPCTSTR szFilename );
	BOOL			LoadPropPartyQuest( LPCTSTR szFilename );
	BOOL			LoadDropEvent( LPCTSTR lpszFileName );
	BOOL			LoadPiercingAvail( LPCTSTR lpszFileName );
	DWORD			GetTextColor( DWORD dwIndex ); 
	LPCTSTR			GetText( DWORD dwIndex );
	LPCTSTR			GetGuildAppell( int nAppell );
	CCtrl*			GetCtrl( OBJID objid );
	CItem*			GetItem( OBJID objid );
	CMover*			GetMover( OBJID objid );
	CShip*			GetShip( OBJID objid );
	CMoverPlayer * GetUserByID(u_long idPlayer);
	LPCHARACTER		GetCharacter( LPCTSTR lpStrKey );
	void			ProtectPropMover();
	DWORD			GetLevelExp( int nLevel );
	CHAO_PROPENSITY GetPropensityPenalty( DWORD dwPropensity );
	BOOL			LoadExcept( LPCTSTR lpszFilename );
	void			LoadSkill();


#ifdef __EVE_MINIGAME
	BOOL			LoadMiniGame();
#endif // __EVE_MINIGAME

	BOOL			LoadUltimateWeapon();

#ifdef __WORLDSERVER
	CUser*			GetUser( OBJID objid );
private:
	int			m_nMaxSequence;
public:
	int		GetGuildMaxSeq( void )	{	return m_nMaxSequence;	}
	BOOL		LoadGuideMaxSeq();

#ifdef __JEFF_11_3
	BOOL	LoadServerScript( const char* sFile );
#endif	// __JEFF_11_3
#endif	// __WORLDSERVER

#ifdef __CLIENT
	BOOL			LoadFilter( LPCTSTR lpszFileName );
	BOOL			LoadHelp( LPCTSTR lpszFileName ); 
	[[nodiscard]] LPCTSTR GetHelp(LPCTSTR lpStr) const;
	CSfx*			GetSfx( OBJID objid );
#endif	// __CLIENT

#ifdef __RULE_0615
	CNameValider nameValider;
#endif	// __RULE_0615

#ifdef __OUTPUT_INFO_0803
	void			OutputStore( const char* lpStrKey, CMover* pMover  );
#endif	// __OUTPUT_INFO_0803
	void			OutputDropItem( void );

#ifdef __ADDSKILLPROP0811
	void			OutputSkill( void );
#endif	

	void			AddMonsterProp( MONSTER_PROP MonsterProp );
	void			RemoveMonsterProp( char* lpszMonsterName );

#if defined( __CLIENT )
	BOOL LoadQuestDestination( void );
	const CString& GetQuestDestination( DWORD dwKey ) const;
	BOOL LoadPatrolDestination( void );
	const CString& GetPatrolDestination( DWORD dwKey ) const;
#endif // defined( __IMPROVE_QUEST_INTERFACE ) && defined( __CLIENT )

#ifdef __IMPROVE_MAP_SYSTEM
#ifdef __CLIENT
	BOOL LoadPropMapComboBoxData( const CString& strFileName );
#endif // __CLIENT
#endif // __IMPROVE_MAP_SYSTEM
};

inline DWORD CProject::GetLevelExp( int nLevel )
{
	const auto it = m_PKSetting.mapLevelExp.find( nLevel );
	if( it != m_PKSetting.mapLevelExp.end() )
		return it->second;
	return 4400000;
}
inline CHAO_PROPENSITY CProject::GetPropensityPenalty( DWORD dwPropensity )
{
	auto it = m_PKSetting.mapPropensityPenalty.begin();
	for( ; it != m_PKSetting.mapPropensityPenalty.end() ; it++ )
	{
		CHAO_PROPENSITY Propensity = it->second;
		if( Propensity.dwPropensityMin <= dwPropensity && dwPropensity <= Propensity.dwPropensityMax )
			return Propensity;
	}
	
	it = m_PKSetting.mapPropensityPenalty.find( 8 );	// 못찾으면 마지막 8단계로 보냄 // 꼭 있어야함
	if( it != m_PKSetting.mapPropensityPenalty.end() )
		return it->second;

	CHAO_PROPENSITY Propensity;
	return Propensity;
}

inline ItemProp* CProject::GetItemProp( LPCTSTR lpszItem )
{
	const auto i	= m_mapII.find( lpszItem );
	if( i != m_mapII.end() )
		return GetItemProp( i->second );
	return NULL;
}

inline CtrlProp* CProject::GetCtrlProp( LPCTSTR lpszCtrl )
{
	const auto i	= m_mapCtrl.find( lpszCtrl );
	if( i != m_mapCtrl.end() )
		return (CtrlProp*)GetCtrlProp( i->second );
	return NULL;
}

inline MoverProp* CProject::GetMoverProp( LPCTSTR lpszMover )
{
	const auto i	= m_mapMVI.find( lpszMover );
	if( i != m_mapMVI.end() )
		return GetMoverProp( i->second );
	return NULL;
}

inline CtrlProp* CProject::GetCtrlProp( int nIndex ) 
{ 
	VERIFY_RANGE_ASSERT( nIndex, 0, m_aPropCtrl.GetSize(), "GetCtrlProp range_error", NULL );
	return m_aPropCtrl.GetAt( nIndex ); 
}

inline ItemProp* CProject::GetItemProp( int nIndex ) 
{ 
	VERIFY_RANGE_ASSERT( nIndex, 0, m_aPropItem.GetSize(), "GetItemProp range_error", NULL );
	return m_aPropItem.GetAt( nIndex ); 
}

inline ItemProp* CProject::GetSkillProp( int nIndex ) 
{ 
	VERIFY_RANGE( nIndex, 0, m_aPropSkill.GetSize(), "GetSkillProp range_error", NULL );
	return m_aPropSkill.GetAt( nIndex ); 
}

inline MoverProp* CProject::GetMoverProp( int nIndex ) 
{ 
	VERIFY_RANGE( nIndex, 0, m_nMoverPropSize, "GetMoverProp range_error", NULL );
	return m_pPropMover + nIndex;
}

inline MoverProp* CProject::GetMoverPropEx( int nIndex )
{
	if( nIndex < 0 || nIndex >= m_nMoverPropSize )
		return NULL;
	return GetMoverProp( nIndex );
}

inline DWORD CProject::GetTextColor( DWORD dwIndex ) 
{ 
	VERIFY_RANGE( (int)( dwIndex ), 0, m_colorText.GetSize(), "GetTextColor range_error", 0xFFFFFFFF );
	if( m_colorText.GetAt( dwIndex ) == NULL )
		return 0;
	return m_colorText.GetAt( dwIndex )->dwColor; 
}

inline LPCTSTR CProject::GetText( DWORD dwIndex ) 
{ 
	VERIFY_RANGE( (int)( dwIndex ), 0, m_colorText.GetSize(), "GetText range_error", "error" );
	if( m_colorText.GetAt( dwIndex ) == NULL )
		return "";
	return m_colorText.GetAt( dwIndex )->lpszData; 
}

inline CMoverPlayer * CProject::GetUserByID(const u_long idPlayer) {
	const auto i = m_idPlayerToUserPtr.find( idPlayer );
	return i != m_idPlayerToUserPtr.end() ? i->second : nullptr;
}

inline LPCHARACTER CProject::GetCharacter( LPCTSTR lpStrKey )
{ 
	if( lpStrKey[0] == 0 )	return NULL;
	LPCHARACTER lpCharacter = NULL; 
	TCHAR szStrKeyLwr[ 64 ];
	_tcscpy( szStrKeyLwr, lpStrKey );
	_tcslwr( szStrKeyLwr );
	m_mapCharacter.Lookup( szStrKeyLwr, (void*&)lpCharacter ); 
	return lpCharacter; 
}

extern CProject prj;

#define GETTEXTCOLOR( x )	prj.GetTextColor( x )
#define GETTEXT( x )		prj.GetText( x )
#define GETANIM( x )		prj.GetAnim( x )
#define GETITEM( x )		prj.GetItem( x )
#define GETMOVER( x )		prj.GetMover( x )

inline const QuestProp * QuestProp::Get(const QuestId questId) {
	return prj.m_aPropQuest.GetAt(questId.get());
}
