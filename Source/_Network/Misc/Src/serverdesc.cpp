#include "stdafx.h"
#include "ServerDesc.h"
#include "Data.h"
#include "Landscape.h"

CServerDesc::CServerDesc()
{
	m_uIdofMulti	= 0;
	m_uKey	= 0;
	*m_szAddr	= '\0';
//	memset( m_szAddr, 0, sizeof(m_szAddr) );
}

CServerDesc::~CServerDesc()
{
	CJurisdiction* pJurisdiction;
	while( m_lspJurisdiction.size() > 0 )
	{
		pJurisdiction	= m_lspJurisdiction.front();
		m_lspJurisdiction.pop_front();
		safe_delete( pJurisdiction );
	}
}

BOOL CServerDesc::IsUnderJurisdiction(const DWORD dwWorldID, const D3DVECTOR & vPos) const {
	int x	= (int)( vPos.x / MPU / MAP_SIZE );
	int z	= (int)( vPos.z / MPU / MAP_SIZE );

	return IsIntersected(dwWorldID);
}

BOOL CServerDesc::IsIntersected(const DWORD dwWorldID) const {
	const auto i = std::find_if(
		m_lspJurisdiction.begin(), m_lspJurisdiction.end(),
		[dwWorldID](const CJurisdiction * const juridiction) {
			return juridiction->m_dwWorldID == dwWorldID;
		}
	);

	return (i != m_lspJurisdiction.end()) ? TRUE : FALSE;
}

CAr & operator<<(CAr & ar, const CServerDesc & self) {
	ar << self.m_uKey;
	ar << static_cast<std::uint32_t>(self.m_lspJurisdiction.size());
	for (const auto & juridiction : self.m_lspJurisdiction) {
		ar << juridiction->m_dwWorldID;
		ar << juridiction->m_rect;
		ar << juridiction->m_wLeft;
		ar << juridiction->m_wRight;
	}
	ar.WriteString(self.m_szAddr);
	return ar;
}

CAr & operator>>(CAr & ar, CServerDesc & self) {
	u_long uKey; ar >> uKey;
	self.SetKey(uKey);

	std::uint32_t nSize;
	ar >> nSize;
	for (std::uint32_t i = 0; i < nSize; i++) {
		CJurisdiction * pJurisdiction = new CJurisdiction;
		ar >> pJurisdiction->m_dwWorldID;
		ar >> pJurisdiction->m_rect;
		ar >> pJurisdiction->m_wLeft;
		ar >> pJurisdiction->m_wRight;
		self.m_lspJurisdiction.push_back(pJurisdiction);
	}
	ar.ReadString(self.m_szAddr);

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