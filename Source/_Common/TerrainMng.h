#pragma once

#include <vector>
#include <optional>

struct TERRAIN {
	LPDIRECT3DTEXTURE9 m_pTexture  = nullptr;
	TCHAR m_szTextureFileName[128] = "";
};

struct WaterTexList {
	static constexpr float FrameAdvance = 0.15f;
	std::vector<DWORD> terrainIds;
	float currentFrame = 0.0f;
	
	void Advance();
	[[nodiscard]] std::optional<DWORD> GetTerrainId() const {
		if (terrainIds.size() == 0) return std::nullopt;
		return terrainIds[static_cast<size_t>(currentFrame)];
	}
};

class CTerrainMng
{
//	int m_nSize;
	std::vector<TERRAIN> m_terrains;
	TERRAIN m_defaultTerrain;

public:
	int				m_nWaterFrame = 0;
	WaterTexList	*m_pWaterIndexList = nullptr;

	~CTerrainMng();

	// int GetSize() { return m_nSize; }

	BOOL LoadTexture( DWORD dwId );
	[[nodiscard]] TERRAIN * GetTerrain(DWORD dwId) {
		if (dwId >= m_terrains.size()) return &m_defaultTerrain;
		return &m_terrains[dwId];
	}
	BOOL LoadScript( LPCTSTR lpszFileName );

	HRESULT DeleteDeviceObjects();
};

