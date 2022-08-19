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
class CRegionElemArray final {
private:
	// In V21, Sanpres has 12 regions which is the second max.
	// Madrigal, the max, have 148.
	static constexpr size_t SoftMaxRegions = 24;
	boost::container::small_vector<REGIONELEM, SoftMaxRegions> m_elems;
public:
	[[nodiscard]] std::span<      REGIONELEM> AsSpan()       { return m_elems; }
	[[nodiscard]] std::span<const REGIONELEM> AsSpan() const { return m_elems; }

	void Add(const REGIONELEM & lpRegionElem) {
		m_elems.emplace_back(lpRegionElem);
	}

	template<typename Func>
	requires (std::is_invocable_r_v<bool, Func, const REGIONELEM &>)
	[[nodiscard]] const REGIONELEM * FindAny(Func && predicate) const {
		for (const REGIONELEM & elem : m_elems) {
			if (predicate(elem)) {
				return &elem;
			}
		}

		return nullptr;
	}

	template<typename Func>
	requires (std::is_invocable_r_v<bool, Func, const REGIONELEM &>)
	[[nodiscard]] const REGIONELEM * FindClosest(const D3DXVECTOR3 & position, Func && predicate) const {
		const REGIONELEM * result = nullptr;
		long resultDistance = 0; // Initialized to shut up the compiler

		for (const REGIONELEM & elem : m_elems) {
			if (predicate(elem)) {
				const D3DXVECTOR3 & diff = position - elem.m_vPos;
				const long distance = static_cast<long>(D3DXVec3LengthSq(&diff));
				if (!result || distance < resultDistance) {
					result = &elem;
					resultDistance = distance;
				}
			}
		}

		return result;
	}
};
