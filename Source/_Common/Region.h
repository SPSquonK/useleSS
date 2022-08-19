#pragma once

#include <type_traits>

#define MAX_REGIONDESC 256
#define MAX_REGIONTITLE 256


struct REGIONELEM
{
	static constexpr bool Archivable = true;
	FLOAT         m_fTeleAngle;
	D3DXVECTOR3   m_vTeleWorld;  
	D3DXVECTOR3   m_vPos;  
	TCHAR         m_szName[ 32];
	TCHAR         m_szKey[ 32];	
	DWORD         m_dwIdTeleWorld; // 
	TCHAR         m_szSound[ 32];
	TCHAR         m_szScript[ 32];
	DWORD         m_dwAttribute;
	CRect         m_rect;
	bool          m_bInside;
	char          m_cDescSize;

	TCHAR         m_szDesc [ MAX_REGIONDESC  ];
	TCHAR         m_szTitle[ MAX_REGIONTITLE ];
	union
	{
		bool          m_bDirectMusic;
		bool          m_bUnit; // structure type에서 unit인가?
	};

	bool          m_bPlayDirectMusic;
	bool          m_bTargetKey;

	union
	{
		DWORD		  m_dwStructure; // structure type 
		DWORD         m_dwIdMusic;
	};
	DWORD	m_dwWorldId;
	DWORD	m_dwIndex;
	
	u_long	m_uItemId;
	UINT	m_uiItemCount;
	UINT	m_uiMinLevel;
	UINT	m_uiMaxLevel;
	int		m_iQuest;
	int		m_iQuestFlag;
	int		m_iJob;
	int		m_iGender;
	BOOL	m_bCheckParty;
	BOOL	m_bCheckGuild;
	BOOL	m_bChaoKey;
};

/*----------------------------------------*/
class CRegionElemArray
{
private:
	static constexpr size_t MAX_REGIONELEM = 256;
	DWORD	m_cbRegionElem;
	REGIONELEM	m_aRegionElem[MAX_REGIONELEM];
public:
//	Constructions
	CRegionElemArray()	{	m_cbRegionElem	= 0;	}
	~CRegionElemArray()		{}
//	Operations
	int	GetSize( void )	{	return m_cbRegionElem;	}
	void	AddTail( const REGIONELEM * lpRegionElem );
	REGIONELEM *	GetAt( int nIndex );

	template<typename Func>
	requires (std::is_invocable_r_v<bool, Func, const REGIONELEM &>)
	[[nodiscard]] const REGIONELEM * FindAny(Func && predicate) const {
		for (DWORD i = 0; i != m_cbRegionElem; ++i) {
			if (predicate(m_aRegionElem[i])) {
				return &m_aRegionElem[i];
			}
		}

		return nullptr;
	}

	template<typename Func>
	requires (std::is_invocable_r_v<bool, Func, const REGIONELEM &>)
	[[nodiscard]] const REGIONELEM * FindClosest(const D3DXVECTOR3 & position, Func && predicate) const {
		const REGIONELEM * result = nullptr;
		long resultDistance = 0; // Initialized to shut up the compiler

		for (DWORD i = 0; i != m_cbRegionElem; ++i) {
			if (predicate(m_aRegionElem[i])) {
				const D3DXVECTOR3 & diff = position - m_aRegionElem[i].m_vPos;
				const long distance = static_cast<long>(D3DXVec3LengthSq(&diff));
				if (!result || distance < resultDistance) {
					result = &m_aRegionElem[i];
					resultDistance = distance;
				}
			}
		}

		return result;
	}
};

inline void CRegionElemArray::AddTail( const REGIONELEM * lpRegionElem )
{
	ASSERT( m_cbRegionElem < MAX_REGIONELEM );
	memcpy( &m_aRegionElem[m_cbRegionElem++], lpRegionElem, sizeof(REGIONELEM) );
}
inline REGIONELEM * CRegionElemArray::GetAt( int nIndex )
{
	if( nIndex >= 0 && nIndex < MAX_REGIONELEM )
		return &m_aRegionElem[nIndex];
	return NULL;
}

