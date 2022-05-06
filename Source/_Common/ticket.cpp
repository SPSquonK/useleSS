#include "stdafx.h"
#include <algorithm>
#include "ticket.h"
#include "sqktd.h"
#include <rapidjson/document.h>

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
	const auto i = m_mapTicketProp.find(dwItemId);
	return i != m_mapTicketProp.end() ? &i->second : nullptr;
}

unsigned int CTicketProperty::GetExpandedLayer(DWORD dwWorldId) const {
	return sqktd::find_in_map(m_expandedLayers, dwWorldId, 0);
}

BOOL CTicketProperty::LoadScript() {
	CScript s;
	if (!s.Load( "ticket.json" )) return FALSE;

	rapidjson::Document document;
	document.Parse(s.m_pProg);

	m_mapTicketProp.clear();

	try {
		for (const auto & ticket_ : document.GetArray()) {
			// == Read JSON
			const auto & ticketObj = ticket_.GetObject();

			const auto maybeWorld = CScript::GetDefineNumOpt(
				ticketObj.FindMember("world")->value.GetString()
			);
			if (!maybeWorld) throw std::exception("Unknown world");

			const DWORD worldId = maybeWorld.value();

			const auto & positionJson = ticketObj.FindMember("position")->value.GetObject();
			const D3DXVECTOR3 position(
				positionJson.FindMember("x")->value.GetFloat(),
				positionJson.FindMember("y")->value.GetFloat(),
				positionJson.FindMember("z")->value.GetFloat()
			);

			const unsigned int nExpand = ([](const auto & ticketObj) -> unsigned int {
				if (ticketObj.HasMember("nExpand")) {
					return ticketObj.FindMember("nExpand")->value.GetUint();
				} else {
					return 0;
				}
			})(ticketObj);
			
			std::vector<DWORD> items;
			for (const auto & itemJson : ticketObj.FindMember("items")->value.GetArray()) {
				const char * itemStr = itemJson.GetString();
				const int id = CScript::GetDefineNum(itemStr);
				if (id == -1) throw std::exception("Item not found");
				items.push_back(static_cast<DWORD>(id));
			}

			// == Add data

			const TicketProp tp{ worldId, position };

			for (const auto itemId : items) {
				const bool ok = m_mapTicketProp.emplace(itemId, tp).second;
				if (!ok) throw std::exception("An item is mapped to multiple tickets");
			}

			const bool ok = m_expandedLayers.emplace(worldId, nExpand).second;
			if (!ok) throw std::exception("A world has several expansions value");
		}
	} catch (std::exception e) {
		Error(__FUNCTION__"(): Invalid ticket.json file - %s", e.what());
		return FALSE;
	}
}
