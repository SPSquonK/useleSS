#include "stdafx.h"
#include <algorithm>
#include "ticket.h"
#include "sqktd/algorithm.h"
#include <rapidjson/document.h>

CTicketProperty g_ticketProperties;

bool CTicketProperty::IsTarget(DWORD dwWorldId) const {
	return std::ranges::any_of(m_mapTicketProp,
		[dwWorldId](const auto & pair) {
			return pair.second.dwWorldId == dwWorldId;
		}
	);
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

			const auto maybeWorld = CScript::GetDefineNumOpt(ticketObj["world"].GetString());
			if (!maybeWorld) throw std::exception("Unknown world");

			const DWORD worldId = maybeWorld.value();

			const auto & positionJson = ticketObj["position"].GetObject();
			const D3DXVECTOR3 position(
				positionJson["x"].GetFloat(),
				positionJson["y"].GetFloat(),
				positionJson["z"].GetFloat()
			);

			const unsigned int nExpand = ([](const auto & ticketObj) -> unsigned int {
				if (ticketObj.HasMember("nExpand")) {
					return ticketObj["nExpand"].GetUint();
				} else {
					return 0;
				}
			})(ticketObj);
			
			std::vector<DWORD> items;
			for (const auto & itemJson : ticketObj["items"].GetArray()) {
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

		return TRUE;
	} catch (std::exception e) {
		Error(__FUNCTION__"(): Invalid ticket.json file - %s", e.what());
		return FALSE;
	}
}
