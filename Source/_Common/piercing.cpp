#include "StdAfx.h"
#include "Piercing.h"

void CPiercing::Clear( void )
{
	m_vPiercing.clear();
	m_vUltimatePiercing.clear();
	m_vtmPetVis.clear();
}

CAr & operator<<(CAr & ar, const CPiercing & self) {
	ar << self.GetPiercingSize();
	for (int i = 0; i < self.GetPiercingSize(); i++)
		ar << self.GetPiercingItem(i);
	ar << self.GetUltimatePiercingSize();
	for (int i = 0; i < self.GetUltimatePiercingSize(); i++)
		ar << self.GetUltimatePiercingItem(i);
	ar << self.m_vtmPetVis.size();
	for (int i = 0; i < (int)(self.m_vtmPetVis.size()); i++)
		ar << self.GetVisKeepTime(i) - time_null();

	return ar;
}

CAr & operator>>(CAr & ar, CPiercing & self) {
	self.m_vPiercing.clear();
	size_t nSize;
	ar >> nSize;
	self.SetPiercingSize(nSize);
	for (size_t i = 0; i < nSize; i++) {
		DWORD dwItem;
		ar >> dwItem;
		self.SetPiercingItem(i, dwItem);
	}
	self.m_vUltimatePiercing.clear();
	ar >> nSize;
	self.SetUltimatePiercingSize(nSize);
	for (int i = 0; i < nSize; i++) {
		DWORD dwItem;
		ar >> dwItem;
		self.SetUltimatePiercingItem(i, dwItem);
	}
	ar >> nSize;
	self.SetVisKeepTimeSize(nSize);
	for (int i = 0; i < nSize; i++) {
		time_t tmTemp;
		ar >> tmTemp;
		self.SetVisKeepTime(i, tmTemp + time_null());
	}

	return ar;
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

int CPiercing::GetUltimatePiercingSize() const
{
	return m_vUltimatePiercing.size();
}

void CPiercing::SetUltimatePiercingItem( int nth, DWORD dwItem )
{
	if( nth >= GetUltimatePiercingSize() )
		return;

	m_vUltimatePiercing[nth]	= dwItem;
}

DWORD CPiercing::GetUltimatePiercingItem( int nth ) const
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

time_t CPiercing::GetVisKeepTime( int nth ) const
{
	if( nth >= GetPiercingSize() || nth >= (int)( m_vtmPetVis.size() ) )
		return 0;

	return m_vtmPetVis[nth];
}
