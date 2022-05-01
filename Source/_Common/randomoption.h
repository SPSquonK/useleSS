#pragma once

#include <vector>

#define	MAX_RANDOM_OPTION	3

#define _AWAKE_SAFE_FLAG	0x2000000000000000

#define _AWAKE_OLD_VALUE	1
#define _AWAKE_NEW_VALUE	2


typedef	struct	_AdjData
{
	short	nAdj;
	DWORD	dwProb;
	_AdjData( short n, DWORD dw )
	{
		nAdj	= n;
		dwProb	= dw;
	}
}	AdjData;

typedef struct	_RANDOM_OPTION
{
	int		nDst;
	int		nProb;
	std::vector<AdjData>	aAdjData;
	_RANDOM_OPTION()
	{
		nDst	= 0;
		nProb	= 0;
	}
	_RANDOM_OPTION( _RANDOM_OPTION* pRandomOption )
	{
		nDst	= pRandomOption->nDst;
		nProb	= pRandomOption->nProb;
		aAdjData.assign( pRandomOption->aAdjData.begin(), pRandomOption->aAdjData.end() );
	}
}	RANDOM_OPTION;

class CRandomOptionProperty
{
public:
	// Added system pet awakening and eating pet awakening
	enum	{	eAwakening, eBlessing,	eSystemPet, eEatPet, eMaxRandomOptionKind	};		// Random option type

	enum	{	eAwakeningExtension	= 3	};	// 0: hand, 1: foot, 2: head	// Awakening Expansion

	CRandomOptionProperty();
	virtual	~CRandomOptionProperty();
	static	CRandomOptionProperty*	GetInstance();
	
	BOOL	LoadScript( LPCTSTR szFile );
	void	LoadScriptBlock( CScript& s, int nRandomOptionKind );

	// Returns the i-th random option parameter and value, otherwise returns FALSE
	BOOL	GetParam( __int64 nRandomOptItemId, int i, int* pnDst, int* pnAdj );
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
	std::vector<RANDOM_OPTION>	m_aRandomOption[eMaxRandomOptionKind + eAwakeningExtension];


public:
	bool	IsCheckedSafeFlag( __int64 n64RandomeOption );
	void	SetSafeFlag( __int64* pn64RandomOption );
	void	ResetSafeFlag( __int64* pn64RandomOption );
};


#define g_xRandomOptionProperty		CRandomOptionProperty::GetInstance()
