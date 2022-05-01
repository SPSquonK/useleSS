#pragma once

#include <array>
#include <vector>
#include <optional>
#include <ranges>
#include "SingleDst.h"

#define	MAX_RANDOM_OPTION	3

#define _AWAKE_SAFE_FLAG	0x2000000000000000

#define _AWAKE_OLD_VALUE	1
#define _AWAKE_NEW_VALUE	2

class CRandomOptionProperty {
public:
	struct AdjData {
		short	nAdj;
		DWORD	dwProb;

		AdjData(const short n, const DWORD dw) : nAdj(n), dwProb(dw) {}
	};

	struct RANDOM_OPTION {
		int nDst = 0;
		int nProb = 0;
		std::vector<AdjData> aAdjData;
	};

public:
	// Added system pet awakening and eating pet awakening
	enum	{	eAwakening, eBlessing,	eSystemPet, eEatPet, eMaxRandomOptionKind	};		// Random option type

	enum	{	eAwakeningExtension	= 3	};	// 0: hand, 1: foot, 2: head	// Awakening Expansion
	
	BOOL	LoadScript( LPCTSTR szFile );
	void	LoadScriptBlock( CScript& s, int nRandomOptionKind );

	// Returns the i-th random option parameter and value, otherwise returns FALSE
	std::optional<SINGLE_DST> GetParam(__int64 nRandomOptItemId, int i);

	auto GetParams(const CItemElem & itemElem) {
		const __int64 randomOpt = GetRandomOptItemId(itemElem);
		const int size = GetRandomOptionSize(randomOpt);

		SmallDstList list;
		for (int i = 0; i != size; ++i) {
			const auto opt = GetParam(randomOpt, i);
			if (opt) {
				list.emplace_back(opt.value());
			}
		}
		return list;
	}

	[[nodiscard]] __int64 GetRandomOptItemId(const CItemElem & itemElem) const;
	
	// Returns the number of given random option parameters
	int		GetRandomOptionSize( __int64 nRandomOptItemId );

	int		GetViewRandomOptionSize( __int64 n64RandomOptItemId );

	// Returns the type of random option according to the mounting part
	int		GetRandomOptionKind( CItemElem* pItemElem );

	// give random option
	BOOL	GenRandomOption( __int64* pnRandomOptItemId, int nRandomOptionKind, int nParts, bool bDecreaseFlag = false );

	// Reset random option
	void	InitializeRandomOption( __int64* pnRandomOptItemId );

	// Set the corresponding parameter after the existing random option - Called by GenRandomOption.
	void	SetParam( __int64* pnRandomOptItemId, int nDst, int nAdj );
private:
	int		DetermineRandomOptionSize( int nRandomOptionKind );
	RANDOM_OPTION*	DetermineRandomOptionDst( int nRandomOptionKind, int nParts );

	short DetermineRandomOptionAdj( RANDOM_OPTION* pRandomOption, bool bDecreaseAdj = false );

	int		GetRandomOptionKindIndex( int nRandomOptionKind, int nParts );
	int		GetUpperProbability( int iRandomOptionKindIndex );
	void	AwakeningExtension( void );		// Awakening Blessing Table Expansion
private:
	int		m_anRandomOptionProb[eMaxRandomOptionKind][MAX_RANDOM_OPTION];
	// Basic + Extended
	std::array<std::vector<RANDOM_OPTION>, eMaxRandomOptionKind + eAwakeningExtension> m_aRandomOption;

public:
	bool	IsCheckedSafeFlag( __int64 n64RandomeOption );
	void	SetSafeFlag( __int64* pn64RandomOption );
	void	ResetSafeFlag( __int64* pn64RandomOption );
};


extern CRandomOptionProperty g_xRandomOptionProperty;
