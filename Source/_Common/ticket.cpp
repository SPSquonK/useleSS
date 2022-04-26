#include "stdafx.h"
#include <algorithm>
#include "ticket.h"

CTicketProperty g_ticketProperties;

BOOL CTicketProperty::IsTarget(DWORD dwWorldId) const {
	const auto it = std::find_if(
		m_mapTicketProp.begin(), m_mapTicketProp.end(),
		[dwWorldId](const auto & pair) {
			return pair.second.dwWorldId == dwWorldId;
		}
	);

	return (it != m_mapTicketProp.end()) ? TRUE : FALSE;
}

const TicketProp * CTicketProperty::GetTicketProp(DWORD dwItemId) const {
	auto i = m_mapTicketProp.find(dwItemId);
	return i != m_mapTicketProp.end() ? &i->second : nullptr;
}


BOOL	CTicketProperty::LoadScript()
{
	CScript s;
	if( s.Load( "ticket.inc" ) == FALSE )
		return FALSE;

	DWORD dwItemId	= s.GetNumber();
	while( s.tok != FINISHED )
	{
		TicketProp	tp;
		tp.dwWorldId	= s.GetNumber();
		tp.vPos.x	= s.GetFloat();
		tp.vPos.y	= s.GetFloat();
		tp.vPos.z	= s.GetFloat();
		bool b	= m_mapTicketProp.emplace( dwItemId, tp ).second;
		ASSERT( b );
		dwItemId	= s.GetNumber();
	}
#ifdef __AZRIA_1023
	return m_lp.LoadScript();
#endif	// __AZRIA_1023
}

#ifdef __AZRIA_1023
BOOL CLayerProperty::LoadScript()
{
	CScript s;
	if( s.Load( "layer.inc" ) == FALSE )
		return FALSE;
	DWORD dwWorldId	= s.GetNumber();
	while( s.tok != FINISHED )
	{
		LayerStruct ls;
		ls.dwWorldId	= dwWorldId;
		ls.nExpand	= s.GetNumber();
		m_vLayers.push_back( ls );
		dwWorldId	= s.GetNumber();
	}
	return TRUE;
}

int CLayerProperty::GetExpanedLayer(DWORD dwWorldId) const {
	const auto it = std::find_if(m_vLayers.begin(), m_vLayers.end(),
		[dwWorldId](const LayerStruct & layerStruct) {
			return layerStruct.dwWorldId == dwWorldId;
		}
	);

	return it != m_vLayers.end() ? it->nExpand : 0;
}
#endif	// __AZRIA_1023