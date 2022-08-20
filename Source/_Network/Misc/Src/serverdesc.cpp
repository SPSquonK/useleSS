#include "stdafx.h"
#include <algorithm>
#include "ServerDesc.h"
#include "Data.h"
#include "Landscape.h"


CAr & operator<<(CAr & ar, const CServerDesc & self) {
	ar << self.m_uKey;
	ar << static_cast<std::uint32_t>(self.m_lspJurisdiction.size());
	for (const WorldId & jurisdiction : self.m_lspJurisdiction) {
		ar << jurisdiction;
	}
	ar << self.m_szAddr;
	return ar;
}

CAr & operator>>(CAr & ar, CServerDesc & self) {
	u_long uKey; ar >> uKey;
	self.SetKey(uKey);

	self.m_lspJurisdiction.clear();
	std::uint32_t nSize;
	ar >> nSize;
	for (std::uint32_t i = 0; i < nSize; i++) {
		WorldId pJurisdiction; ar >> pJurisdiction;
		self.m_lspJurisdiction.emplace(pJurisdiction);
	}

	ar >> self.m_szAddr;

	return ar;
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/
CServerDescArray::CServerDescArray()
{
#ifndef __STL_0402
	SetSize( 16, 16, 8 );	
#endif	// __STL_0402
}

CServerDescArray::~CServerDescArray()
{
	Free();
}

void CServerDescArray::Free( void )
{
	CMclAutoLock Lock( m_AddRemoveLock );

#ifdef __STL_0402
	for( auto i = begin(); i != end(); ++i )
	{
		CServerDesc* pServer	= i->second;
		SAFE_DELETE( pServer );
	}
	clear();
#else	// __STL_0402
	CMyBucket<CServerDesc*>* pBucket	= GetFirstActive();
	while( pBucket )
	{
		safe_delete( pBucket->m_value );
		pBucket		= pBucket->pNext;
	}
	ClearActiveList();
#endif	// __STL_0402
}

CServerDesc* CServerDescArray::GetAt( ULONG uKey )
{
#ifdef __STL_0402
	CServerDescArray::iterator i = find( uKey );
	if( i != end() )
		return i->second;
	return NULL;
#else	// __STL_0402
	CServerDesc* pDesc;
	if( Lookup( uKey, pDesc ) )
		return pDesc;
	return NULL;
#endif	// __STL_0402
}