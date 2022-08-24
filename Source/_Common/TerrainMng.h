#pragma once

struct TERRAIN {
	LPDIRECT3DTEXTURE9 m_pTexture  = nullptr;
	TCHAR m_szTextureFileName[128] = "";
};

struct WaterTexList {
	static constexpr float FrameAdvance = 0.15f;
	int  ListCnt = 0;
	int * pList = nullptr;
};

#define MAX_WATER	10

class CTerrainMng
{
//	int m_nSize;
	LPDIRECT3DDEVICE9 m_pd3dDevice;
	std::vector<TERRAIN> m_terrains;
	TERRAIN m_defaultTerrain;

public:
	int				m_nWaterFrame = 0;
	WaterTexList	*m_pWaterIndexList = nullptr;
	FLOAT			m_fWaterFrame[MAX_WATER];

	CTerrainMng();
	~CTerrainMng();

	// int GetSize() { return m_nSize; }

	BOOL LoadTexture( DWORD dwId );
	[[nodiscard]] TERRAIN * GetTerrain(DWORD dwId) {
		if (dwId >= m_terrains.size()) return &m_defaultTerrain;
		return &m_terrains[dwId];
	}
	BOOL LoadScript( LPCTSTR lpszFileName );

	HRESULT InitDeviceObjects(LPDIRECT3DDEVICE9 pd3dDevice);
	HRESULT DeleteDeviceObjects();
};

