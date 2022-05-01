#include "stdafx.h"
#include "randomoption.h"

CRandomOptionProperty g_xRandomOptionProperty;

void CRandomOptionProperty::LoadScriptBlock( CScript & s, int nRandomOptionKind )
{
	int nTotalProb1		= 0;
	for( int i = 0; i < MAX_RANDOM_OPTION; i++ )	// 3	// Probability of how many random options will be given
	{
		m_anRandomOptionProb[nRandomOptionKind][i]	= s.GetNumber() + nTotalProb1;
		nTotalProb1	= m_anRandomOptionProb[nRandomOptionKind][i];
	}
	s.GetToken();	// {
	int nTotalProb2	= 0;
	RANDOM_OPTION	ro;
	ro.nDst		= s.GetNumber();
	while( *s.token != '}' )
	{
		ASSERT( ro.nDst <= 0xFF );	// 255	// 8
		ro.nProb	= s.GetNumber() + nTotalProb2;
		nTotalProb2		= ro.nProb;
		s.GetToken();	// {
		short	nAdj	= s.GetNumber();
		DWORD dwTotalProb3	= 0;
		while( *s.token != '}' )
		{
			DWORD dwProb	= s.GetNumber() + dwTotalProb3;
			dwTotalProb3	= dwProb;
			ASSERT( abs( nAdj ) <= 0x1FF );		// 511	// 1 | 9
			ro.aAdjData.push_back( AdjData( nAdj, dwProb ) );
			nAdj	= s.GetNumber();
		}
		if( dwTotalProb3 != 1000000000 )
			Error( "Awakening: nDst = %d, dwTotalProb = %d", ro.nDst, dwTotalProb3 );
		m_aRandomOption[nRandomOptionKind].push_back( ro );
		ro.aAdjData.clear();
		ro.nDst		= s.GetNumber();
	}
}

BOOL CRandomOptionProperty::LoadScript( LPCTSTR szFile )
{
	CScript s;
	if( s.Load( szFile ) == FALSE )
		return FALSE;

	s.GetToken();	// subject or FINISHED
	while( s.tok != FINISHED )
	{
		if( s.Token == _T( "Awakening" ) )
			LoadScriptBlock( s, eAwakening );
		else if( s.Token == _T( "Blessing" ) )
			LoadScriptBlock( s, eBlessing );
		else if( s.Token == _T( "SystemPet" ) )
			LoadScriptBlock( s, eSystemPet );
		else if( s.Token == _T( "EatPet" ) )
			LoadScriptBlock( s, eEatPet );
		s.GetToken();
	}
	AwakeningExtension();	// Table expansion to add awakening and blessing parts
	return TRUE;
}

void	CRandomOptionProperty::SetParam( __int64* pnRandomOptItemId, int nDst, int nAdj )
{
	int nSize	= GetRandomOptionSize( *pnRandomOptItemId );
	ASSERT( nSize < MAX_RANDOM_OPTION );

	__int64 nRandomOptionItemId		= nDst;
	nRandomOptionItemId		= nRandomOptionItemId << 10;		// 2 | 8 | 10 | 8 | 10 | 8 | 10 | 8
	//	2 : 

	//	1 :
	//	7 :
	//	10 :

	//	8 :

	if( nAdj < 0 )
	{
		nRandomOptionItemId	|= 0x0200;	// Most significant bit 1 if negative
		nRandomOptionItemId	= nRandomOptionItemId | ( -nAdj );
	}
	else
		nRandomOptionItemId	= nRandomOptionItemId | nAdj;

	nRandomOptionItemId	= nRandomOptionItemId << ( 8 + 18 * nSize );	// shift to that position
	*pnRandomOptItemId	|= nRandomOptionItemId;		// Save result value
}

int CRandomOptionProperty::GetRandomOptionKind( CItemElem* pItemElem )
{
	ItemProp* pProp		= pItemElem->GetProp();
	switch( pProp->dwParts )
	{
		case PARTS_UPPER_BODY:
		case PARTS_RWEAPON:
		case PARTS_SHIELD:
		case PARTS_HAND:
		case PARTS_FOOT:
		case PARTS_CAP:
			return static_cast<int>( eAwakening );
		case PARTS_CLOTH:
		case PARTS_CLOAK:
		case PARTS_HAT:
		case PARTS_GLOVE:
		case PARTS_BOOTS:
			return static_cast<int>( eBlessing );
		default:
			{
				if( pProp->dwItemKind3 == IK3_EGG && pItemElem->m_pPet && pItemElem->m_pPet->GetLevel() >= PL_C )
					return static_cast<int>( eSystemPet );
				else if( pProp->dwItemKind3 == IK3_PET )
					return static_cast<int>( eEatPet );
				break;
			}
	}
	return -1;
}

__int64 CRandomOptionProperty::GetRandomOptItemId(const CItemElem & itemElem) const {
	return itemElem.GetRandomOptItemId();
}

std::optional<SINGLE_DST> CRandomOptionProperty::GetParam(__int64 nRandomOptItemId, int i) {
	if (i >= MAX_RANDOM_OPTION) return std::nullopt;

	int nRandomOption = static_cast<int>(nRandomOptItemId >> (8 + i * 18));
	int adj = nRandomOption & 0x000001FF;
	if (nRandomOption & 0x00000200) {
		adj = -adj;
	}
	nRandomOption = nRandomOption >> 10;

	int dst = nRandomOption & 0x0000007F;

	if (dst <= 0) return std::nullopt;

	return SINGLE_DST{ dst, adj };
}

int CRandomOptionProperty::GetRandomOptionSize( __int64 nRandomOptItemId )
{
	int nSize	= 0;

	bool	bCheckedSafeFlag = false;
	bCheckedSafeFlag	= IsCheckedSafeFlag( nRandomOptItemId );

	if( bCheckedSafeFlag == true )
	{
		return	nSize;
	}

	__int64 i	= 0x3FFFF << 8;
	for( int j = 0; j < MAX_RANDOM_OPTION; j++ )
	{
		if( nRandomOptItemId & i )
			nSize++;
		else
			return nSize;

		i	= i << 18;
	}

	return nSize;
}


int		CRandomOptionProperty::GetViewRandomOptionSize( __int64 n64RandomOptItemId )
{
	int nSize	= 0;

	__int64 i	= 0x3FFFF << 8;
	for( int j = 0; j < MAX_RANDOM_OPTION; j++ )
	{
		if( n64RandomOptItemId & i )
			nSize++;
		else
			return nSize;

		i	= i << 18;
	}

	return nSize;
}


BOOL	CRandomOptionProperty::GenRandomOption( __int64* pnRandomOptItemId, int nRandomOptionKind, int nParts, bool bDecreaseFlag /* = false */ )
{ 
	if( GetRandomOptionSize( *pnRandomOptItemId )  > 0 )
	{
		return FALSE;
	}

	int nRandomOptionSize	= DetermineRandomOptionSize( nRandomOptionKind );

	int	nDecreaseNumber	= -1;

	if( bDecreaseFlag == true )
	{
		nDecreaseNumber = xRandom( nRandomOptionSize );
	}

	
	while( nRandomOptionSize-- > 0 )
	{
		RANDOM_OPTION* pRandomOption	= DetermineRandomOptionDst( nRandomOptionKind, nParts );
		ASSERT( pRandomOption );


		bool	bDecreaseAdj	= false;

		if( nRandomOptionSize == nDecreaseNumber )
		{
			bDecreaseAdj = true;
		}
		else
		{
			bDecreaseAdj = false;
		}

		short nAdj	= DetermineRandomOptionAdj( pRandomOption, bDecreaseAdj );

		SetParam( pnRandomOptItemId, pRandomOption->nDst, nAdj );
	}
	return TRUE;
}

int	CRandomOptionProperty::DetermineRandomOptionSize( int nRandomOptionKind )
{
	int nUpper	= m_anRandomOptionProb[nRandomOptionKind][MAX_RANDOM_OPTION - 1];
	DWORD dwRand	= xRandom( nUpper );
	for( int i = 0; i < MAX_RANDOM_OPTION; i++ )
	{
		if( (int)( dwRand ) < m_anRandomOptionProb[nRandomOptionKind][i] )
			return i + 1;
	}
	return 0;
}

CRandomOptionProperty::RANDOM_OPTION * CRandomOptionProperty::DetermineRandomOptionDst( int nRandomOptionKind, int nParts )
{
	int iRandomOptionKindIndex	= GetRandomOptionKindIndex( nRandomOptionKind, nParts );
	DWORD dwRand	= xRandom( GetUpperProbability( iRandomOptionKindIndex ) );
	for( DWORD i = 0; i < m_aRandomOption[iRandomOptionKindIndex].size(); i++ )
	{
		RANDOM_OPTION* pRandomOption	= &m_aRandomOption[iRandomOptionKindIndex][i];
		if( (int)( dwRand ) < pRandomOption->nProb )
		{
			return pRandomOption;
		}
	}
	return NULL;
}

short CRandomOptionProperty::DetermineRandomOptionAdj( RANDOM_OPTION* pRandomOption, bool bDecreaseAdj /* = false */ )
{
	ASSERT( pRandomOption );
	DWORD dwRand	= xRandom( 1000000000 );	// 0 - 9999999
	for( DWORD i = 0; i < pRandomOption->aAdjData.size(); i++ )
	{
		AdjData	ad	= pRandomOption->aAdjData[i];
		if( dwRand < ad.dwProb )
		{
			if( bDecreaseAdj == true && i > 0 )
			{
				return	( pRandomOption->aAdjData[i-1].nAdj );
			}
			else
			{
				return ad.nAdj;
			}
		}
	}
	Error( "CRandomOptionProperty.DetermineRandomOptionAdj: 0" );
	return 0;
}


int	CRandomOptionProperty::GetRandomOptionKindIndex( int nRandomOptionKind, int nParts )
{	// Returns the index of the table to be referenced when blessing awakening
	//PARTS_HAND: 4, PARTS_FOOT: 5, PARTS_CAP: 6
	// The extended index starts from the last index of the base table.
	if( nRandomOptionKind == eAwakening && ( nParts == PARTS_HAND || nParts == PARTS_FOOT || nParts == PARTS_CAP ) )
		return eMaxRandomOptionKind + nParts - PARTS_HAND;
	return nRandomOptionKind;
}

void CRandomOptionProperty::InitializeRandomOption( __int64* pnRandomOptItemId )
{
	*pnRandomOptItemId	&= 0xC0000000000000FF;
}




void	CRandomOptionProperty::SetSafeFlag( __int64* pn64RandomOption )
{
	if( pn64RandomOption == NULL )
	{
		WriteLog( "pn64RandomOption is NULL in SetSafeFlag function" );
	}
	else
	{
		bool	bCheckedSafeFlag = false;
		bCheckedSafeFlag	= IsCheckedSafeFlag( *pn64RandomOption );

		if( bCheckedSafeFlag == true )
		{
			WriteLog( "bCheckedSafeFlag is already true in SetSafeFlag function." );
		}
		else
		{
			*pn64RandomOption = *pn64RandomOption | _AWAKE_SAFE_FLAG;
		}
	}

	return;
}

void	CRandomOptionProperty::ResetSafeFlag( __int64* pn64RandomOption )
{
	if( pn64RandomOption == NULL )
	{
		WriteLog( "pn64RandomOption is NULL in ResetSafeFlag function" );
	}
	else
	{
		bool	bCheckedSafeFalg = false;
		bCheckedSafeFalg	= IsCheckedSafeFlag( *pn64RandomOption );

		if( bCheckedSafeFalg == true )
		{
			*pn64RandomOption = *pn64RandomOption & ~(_AWAKE_SAFE_FLAG);
		}
		else
		{
			WriteLog( "bCheckedSafeFlag is already false in ResetSafeFlag function." );
		}
	}

	return;
}

bool	CRandomOptionProperty::IsCheckedSafeFlag( __int64 n64RandomeOption )
{
	if( (n64RandomeOption & _AWAKE_SAFE_FLAG) == _AWAKE_SAFE_FLAG )
	{
#ifdef __PROTECT_AWAKE
		return	true;
#endif	
	}

	return	false;
}




void CRandomOptionProperty::AwakeningExtension( void )
{
	// Awakenable properties for each part
	int	anDst[eAwakeningExtension][6]	= {		
		DST_CHR_CHANCECRITICAL, DST_ATKPOWER, DST_SPELL_RATE, DST_ATTACKSPEED, DST_MP_MAX, DST_FP_MAX,
		DST_CRITICAL_BONUS, DST_SPEED, DST_ADJDEF, DST_HP_MAX, DST_MP_MAX, DST_FP_MAX,
		DST_STR, DST_DEX, DST_INT, DST_STA, DST_MP_MAX, DST_FP_MAX	};
	int anTotal[eAwakeningExtension];
	memset( anTotal, 0, sizeof(int) * eAwakeningExtension );

	// To include only the attributes that can be awakened in the corresponding part of the awakening table.
	// Configure after expanding the awakening table
	for( DWORD i = 0; i < m_aRandomOption[eAwakening].size(); i++ )
	{
		RANDOM_OPTION* pRandomOption	= &m_aRandomOption[eAwakening][i];
		int nProb	= ( i == 0?
			m_aRandomOption[eAwakening][i].nProb:
			m_aRandomOption[eAwakening][i].nProb - m_aRandomOption[eAwakening][i-1].nProb );

		for( int j = 0; j < eAwakeningExtension; j++ )
		{
			int iRandomOptionKindIndex	= GetRandomOptionKindIndex( eAwakening, PARTS_HAND + j );
			for( int k = 0; k < 6; k++ )
				if( pRandomOption->nDst == anDst[j][k] )
				{
					RANDOM_OPTION ro( *pRandomOption );
					ro.nProb	= anTotal[j] + nProb;
					anTotal[j]	= ro.nProb;
					m_aRandomOption[iRandomOptionKindIndex].push_back( ro );
				}
		}
	}
}

int	CRandomOptionProperty::GetUpperProbability( int iRandomOptionKindIndex )
{
	int iLast	= m_aRandomOption[iRandomOptionKindIndex].size() - 1;
	return m_aRandomOption[iRandomOptionKindIndex][iLast].nProb;
}