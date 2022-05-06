#pragma once

struct TicketProp {
	DWORD	dwWorldId;
	D3DXVECTOR3	vPos;
};

class CTicketProperty final {
public:
	[[nodiscard]] const TicketProp * GetTicketProp(DWORD dwItemId) const;
	[[nodiscard]] BOOL IsTarget(DWORD dwWorldId) const;
	BOOL LoadScript();
	[[nodiscard]] unsigned int GetExpandedLayer(DWORD dwWorldId) const;
private:
	std::map<DWORD, TicketProp>	m_mapTicketProp;
	std::map<DWORD, unsigned int> m_expandedLayers;
};

extern CTicketProperty g_ticketProperties;
