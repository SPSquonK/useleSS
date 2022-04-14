#include "StdAfx.h"
#include "Piercing.h"


CPiercing::CPiercing()
{
}

CPiercing::~CPiercing()
{
}

CPiercing& CPiercing::operator=( const CPiercing & piercing )
{
	m_vPiercing.assign( piercing.m_vPiercing.begin(), piercing.m_vPiercing.end() );
	m_vUltimatePiercing.assign( piercing.m_vUltimatePiercing.begin(), piercing.m_vUltimatePiercing.end() );
	m_vtmPetVis.assign( piercing.m_vtmPetVis.begin(), piercing.m_vtmPetVis.end() );
	return *this;
}

void CPiercing::Clear( void )
{
	m_vPiercing.clear();
	m_vUltimatePiercing.clear();
	m_vtmPetVis.clear();
}

void CPiercing::Serialize( CAr & ar )
{
	if( ar.IsStoring() )
	{
		ar << GetPiercingSize();
		for( int i = 0; i < GetPiercingSize(); i++ )
			ar << GetPiercingItem( i );
		ar << GetUltimatePiercingSize();
		for( int i = 0; i < GetUltimatePiercingSize(); i++ )
			ar << GetUltimatePiercingItem( i );
		ar << m_vtmPetVis.size();
		for( int i=0; i<(int)( m_vtmPetVis.size() ); i++ )
			ar << GetVisKeepTime( i ) - time_null();
	}
	else
	{
		m_vPiercing.clear();
		size_t nSize;
		ar >> nSize;
		SetPiercingSize( nSize );
		for( size_t i = 0; i < nSize; i++ )
		{
			DWORD dwItem;
			ar >> dwItem;
			SetPiercingItem( i, dwItem );
		}
		m_vUltimatePiercing.clear();
		ar >> nSize;
		SetUltimatePiercingSize( nSize );
		for( int i = 0; i < nSize; i++ )
		{
			DWORD dwItem;
			ar >> dwItem;
			SetUltimatePiercingItem( i, dwItem );
		}
		ar >> nSize;
		SetVisKeepTimeSize( nSize );
		for( int i=0; i<nSize; i++ )
		{
			time_t tmTemp;
			ar >> tmTemp;
			SetVisKeepTime( i, tmTemp + time_null() );
		}
	}
}

void CPiercing::SetPiercingSize( int nSize )
{
	if( nSize > MAX_PIERCING_WEAPON )
		nSize = MAX_PIERCING_WEAPON;

	m_vPiercing.resize( nSize, 0 );
}

int CPiercing::GetPiercingSize() const
{
	return m_vPiercing.size();
}

void CPiercing::SetPiercingItem( int nth, DWORD dwItem )
{
	if( nth >= GetPiercingSize() )
		return;
	
	m_vPiercing[nth]	= dwItem;
}

DWORD CPiercing::GetPiercingItem( int nth ) const
{
	if( nth >= GetPiercingSize()  )
		return 0;
	
	return m_vPiercing[nth];
}

BOOL CPiercing::IsPiercedItem()
{
	if( GetPiercingSize() > 0
		|| GetUltimatePiercingSize() > 0 
		)
		return TRUE;

	return FALSE;
}

void CPiercing::SetUltimatePiercingSize( int nSize )
{
	if( nSize > MAX_PIERCING_ULTIMATE )
		nSize = MAX_PIERCING_ULTIMATE;

	m_vUltimatePiercing.resize( nSize, 0 );
}

int CPiercing::GetUltimatePiercingSize()
{
	return m_vUltimatePiercing.size();
}

void CPiercing::SetUltimatePiercingItem( int nth, DWORD dwItem )
{
	if( nth >= GetUltimatePiercingSize() )
		return;

	m_vUltimatePiercing[nth]	= dwItem;
}

DWORD CPiercing::GetUltimatePiercingItem( int nth )
{
	if( nth >= GetUltimatePiercingSize()  )
		return 0;

	return m_vUltimatePiercing[nth];
}

void CPiercing::SetVisKeepTime( int nth, time_t tmKeep )
{
	if( nth >= GetPiercingSize() || nth >= (int)( m_vtmPetVis.size() ) )
		return;

	m_vtmPetVis[nth] = tmKeep;
}

time_t CPiercing::GetVisKeepTime( int nth )
{
	if( nth >= GetPiercingSize() || nth >= (int)( m_vtmPetVis.size() ) )
		return 0;

	return m_vtmPetVis[nth];
}
