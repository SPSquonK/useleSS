#pragma once

#include <array>
#include <span>
#include "defineitem.h"
#include "SingleDst.h"

#ifdef __CLIENT
#include "..\_AIInterface\AIInterface.h"
#endif	// __CLIENT


enum	PETLEVEL	{	PL_EGG,	PL_D,	PL_C,	PL_B,	PL_A,	PL_S,	PL_MAX,	};

#define	MAX_PET_AVAIL_LEVEL	9
#define	MAX_ADD_LIFE	5

#define	MAX_PET_SHAPE	3

class CProject;
class CScript;

class CPetProperty final {
public:
	struct FEEDENERGY {
		DWORD	dwCostMin;
		DWORD	dwCostMax;
		WORD	wEnergyMin;
		WORD	wEnergyMax;
	};

	struct PETPENALTY {
		FLOAT	fExp = 0.0f;
		WORD	wEnergy = 0;
	};

	struct PETAVAILPARAM {
		DWORD	dwDstParam;
		DWORD	m_anParam[MAX_PET_AVAIL_LEVEL];	
		DWORD	m_dwItemId;	// Pet item ID
		DWORD	m_adwIndex[MAX_PET_SHAPE];	// Pet object index
		DWORD	m_dwProbability;	// Hatching probability
	};

	CPetProperty();

	[[nodiscard]] const PETAVAILPARAM * GetAvailParam(BYTE nKind) const;
	BYTE	GetLevelupAvailLevel( BYTE wLevel );		// Returns a random ability level for the given pet level
	[[nodiscard]] WORD GetFeedEnergy(DWORD dwCost, int nIndex = 0) const;		// Generate random food number according to price
	DWORD	GetIncrementExp( BYTE nLevel );	// Experience gained per minute
	WORD	GetMaxEnergy( BYTE nLevel );
	WORD	GetAddLife();		// Add life by life recovery liquid

	static	CPetProperty*	GetInstance( void );
	BOOL	LoadScript( LPCTSTR szFile );
	void LoadLevelupAvail(CScript & script);
	BYTE	Hatch( void );
	[[nodiscard]] const PETPENALTY * GetPenalty(BYTE nLevel) const;

#ifdef __CLIENT
	[[nodiscard]] static DWORD GetTIdOfLevel(PETLEVEL petLevel);
	[[nodiscard]] static DWORD GetTIdOfDst(const SINGLE_DST & dst, bool shortenHpMax = false);
#endif

	friend CProject;

private:
	struct LevelInfo {
		std::array<DWORD, MAX_PET_AVAIL_LEVEL> availProb {0, };
		BYTE availMax      = 0;
		DWORD incrementExp = 0;
		WORD maxEnergy     = 0;
		PETPENALTY penalty;
	};

	PETAVAILPARAM	m_aPetAvailParam[PK_MAX];
	std::array<LevelInfo, PL_MAX> m_levelInfos;
	std::vector<FEEDENERGY>	m_aFeedEnergy[2];	// Amount of food according to loot price when making food
	std::vector<WORD>	m_awAddLifeProbability;
};

#define	MAX_PET_LIFE	99
#define	MAX_PET_EGG_EXP		(DWORD)50000
#define	MAX_PET_EXP		(DWORD)100000

#define	PF_PET	0
#define	PF_PET_LEVEL_UP	1
#define	PF_PET_LEVEL_DOWN	2
#define	PF_PET_GET_AVAIL	3

#define	PETLOGTYPE_CALL	0
#define	PETLOGTYPE_LEVELUP	1
#define	PETLOGTYPE_RELEASE	2
#define	PETLOGTYPE_DEATH		4
#define	PETLOGTYPE_FEED	5
#define	PETLOGTYPE_MIRACLE	6
#define	PETLOGTYPE_MISTAKE	7
#define	PETLOGTYPE_LIFE	8

#define	MAX_PET_NAME_FMT	33	// 16 * 2 + 1
#define	MAX_PET_NAME	17 // 8 korean characters

class CPet final {
private:
//	Operations
	friend CAr & operator<<(CAr & ar, const CPet & pet);
	friend CAr & operator>>(CAr & ar, CPet & pet);

public:
//	Attributions
	void	SetAvailLevel( BYTE nLevel, BYTE nAvailLevel );

	[[nodiscard]] DWORD GetIndex() const;
	BYTE	GetAvailLevel( BYTE nLevel );

	[[nodiscard]] SINGLE_DST GetAvailDestParam() const;
	BYTE	GetKind( void )	{	return m_nKind;	}
	void	SetKind( BYTE nKind )	{	m_nKind	= nKind;	}
	BYTE	GetLevel( void )	{	return m_nLevel;	}
	[[nodiscard]] PETLEVEL GetPetLevel() const { return static_cast<PETLEVEL>(m_nLevel); }
	void	IncLevel( void )		{	m_nLevel++;	}
	void	SetLevel( BYTE nLevel )	{	m_nLevel	= nLevel;	}
	DWORD	GetExp( void )	{	return m_dwExp;		}
	void	SetExp( DWORD dwExp );
	WORD	GetLife( void );
	void	SetLife( WORD wLife )	{	m_wLife	= wLife;	}
	WORD	GetEnergy( void )	{	return m_wEnergy;	}
	void	SetEnergy( WORD wEnergy );
	WORD	GetMaxEnergy( void )	{	return CPetProperty::GetInstance()->GetMaxEnergy( m_nLevel );	}
	int		GetEnergyPercent( void )	{	return GetEnergy() * 100 / GetMaxEnergy();	}
	int		GetExpPercent( void )	{	return( GetExp() * 100 / GetMaxExp() );		}
	int		GetMaxExp( void )	{	return ( m_nLevel == PL_EGG? MAX_PET_EGG_EXP: MAX_PET_EXP );	}
	void	SetName( const char* szName );
	const char*	GetName()	{	return m_szName;	}
	BOOL	HasName()	{	return strlen( m_szName ) > 0;	}
	void	InitEgg( void );
	DWORD	GetItemId( void )
		{
			if( m_nLevel == PL_EGG )
				return II_PET_EGG;
			return( CPetProperty::GetInstance()->GetAvailParam( m_nKind )->m_dwItemId );
		}

private:
	BYTE	m_nKind = 0;	// 0~6
	BYTE	m_nLevel = PL_EGG;	// Pet level : e		// e = 0, d = 1, c = 2, b = 3, a = 4, s = 5
	DWORD	m_dwExp = 0;
	WORD	m_wEnergy = 0;
	WORD	m_wLife = 0;	// Life: 0 ~ 99 // Default: 1 // Remove object when death with 0 life
	std::array<BYTE, PL_MAX> m_anAvailLevel = { 0, 0, 0, 0, 0, 0 };	// Stats
	char	m_szName[MAX_PET_NAME] = "";
};

#ifdef __CLIENT

class CAIEgg: public CAIInterface
{
private:
	OBJID m_idOwner;
	int		m_nState;
	void MoveToDst(	OBJID idTarget );
	void MoveToDst(	D3DXVECTOR3 vDst );
	BOOL  MoveProcessIdle( const AIMSG & msg );
	BOOL  MoveProcessRage( const AIMSG & msg );
	void Init( void );
	void Destroy( void );
public:
	CAIEgg( CObj* pObj, OBJID idOwner );
	virtual ~CAIEgg();

	virtual void InitAI();

	int		GetState( void )	{	return m_nState;	}
	OBJID	GetOwnerId( void )	{	return m_idOwner;	}

	BOOL	StateInit( const AIMSG & msg );
	BOOL	StateIdle( const AIMSG & msg );
	BOOL	StateRage( const AIMSG & msg );

	DECLARE_AISTATE()
};
#endif	// __CLIENT


// Individual item elements required for conversion
struct TransformStuffComponent {
	static constexpr bool Archivable = true;
	int		nItem;
	short	nNum;

	TransformStuffComponent(int nItem = 0, short nNum = 0)
		: nItem(nItem), nNum(nNum) {
	}
};

// Set of items required for conversion
class CTransformStuff final {
public:
	explicit CTransformStuff(int nTransform = 0);

	void	AddComponent( int nItem, short nNum );		// Add required item element
	friend CAr & operator<<(CAr & ar, const CTransformStuff & self);
	friend CAr & operator>>(CAr & ar, CTransformStuff & self);
	
	[[nodiscard]] int GetTransform() const  { return m_nTransform; }	// Return the conversion type
	[[nodiscard]] std::span<      TransformStuffComponent> GetSpan()        { return m_vComponents; }
	[[nodiscard]] std::span<const TransformStuffComponent> GetSpan() const { return m_vComponents; }

#ifdef __WORLDSERVER
	// Remove conversion material from user
	void RemoveItem(CUser * pUser) const;
#endif

private:
	int	m_nTransform;	// conversion type
	std::vector<TransformStuffComponent> m_vComponents;	// set of necessary items
};

#ifdef __WORLDSERVER

// Manage all conversions
class CTransformItemProperty final {
	public:
	
	class ITransformer {
	public:
		virtual	~ITransformer()	= default;
		[[nodiscard]] virtual bool IsValidItem(CItemElem * itemElem) const = 0;
	};

	// egg conversion class
	class CTransformerEgg : public ITransformer {
	public:
		[[nodiscard]] bool IsValidItem(CItemElem * itemElem) const override;
	};

	// Conversion Result Item Element
	// Using a deformed circular pattern
	struct TransformItemElement {
		std::unique_ptr<CItemElem> pItem;
		std::uint32_t nProb;
	};

	// It is an element that signifies one concrete transformation.
	class CTransformItemComponent final {
	private:
		static constexpr DWORD eMaxProb = 1000000;

	public:
		CTransformItemComponent( int nTransform );

		void AddElement(std::unique_ptr<CItemElem> item, std::uint32_t prob);
		void	SetStuffSize(u_int nStuffSize) { m_nStuffSize = nStuffSize; }

		[[nodiscard]] u_int	GetStuffSize() const { return m_nStuffSize; }
		[[nodiscard]] int GetTransform() const { return m_nTransform; }
		[[nodiscard]] CItemElem * GetItem() const;
	private:
		int m_nTransform;
		u_int	m_nStuffSize = 0;
		std::uint32_t m_nTotalProb = 0;
		std::vector<TransformItemElement>	m_vTransformItemElements;
	};

public:

	static CTransformItemProperty * Instance();
	
	u_int	GetStuffSize(int nTransform) const;
	CItemElem * GetItem(int nTransform) const;
	BOOL	LoadScript(const char * szFile);

	void Transform(CUser *pUser, const CTransformStuff & stuff) const;
private:
	std::unique_ptr<CItemElem> CreateItemGeneric(CScript & s);
	std::unique_ptr<CItemElem> CreateItemPet(CScript & s);

	const CTransformItemComponent * GetComponent(int nTransform) const;
	
	// Convert user's item
	[[nodiscard]] bool IsValidStuff(CUser * pUser, const CTransformStuff & stuff, const ITransformer & transfomer) const;

	// Creates a conversion result item for the user
	void CreateItem(CUser * pUser, int nTransform) const;


private:
	std::map<int, std::unique_ptr<CTransformItemComponent>>	m_mapComponents;

	CTransformerEgg transformer0Egg;
};

#endif	// __WORLDSERVER

