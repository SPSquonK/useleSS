#pragma once

struct TicketProp {
	DWORD	dwWorldId;
	D3DXVECTOR3	vPos;
};

#ifdef __AZRIA_1023
struct LayerStruct {
	DWORD dwWorldId;
	int nExpand;
};

class CLayerProperty final {
public:
	BOOL LoadScript();
	[[nodiscard]] int GetExpanedLayer(DWORD dwWorldId) const;
private:
	std::vector<LayerStruct> m_vLayers;
};
#endif	// __AZRIA_1023

class CTicketProperty final {
public:
	[[nodiscard]] const TicketProp * GetTicketProp(DWORD dwItemId) const;
	[[nodiscard]] BOOL IsTarget(DWORD dwWorldId) const;
	BOOL LoadScript();
#ifdef __AZRIA_1023
	[[nodiscard]] int GetExpanedLayer(DWORD dwWorldId) const {
		return m_lp.GetExpanedLayer( dwWorldId );
	}
#endif	// __AZRIA_1023
private:
	std::map<DWORD, TicketProp>	m_mapTicketProp;
#ifdef __AZRIA_1023
	CLayerProperty m_lp;
#endif // __AZRIA_1023
};

extern CTicketProperty g_ticketProperties;
