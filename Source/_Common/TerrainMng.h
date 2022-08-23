#pragma once

struct TERRAIN {
	LPDIRECT3DTEXTURE9 m_pTexture;
	TCHAR m_szTextureFileName[128];
	DWORD m_dwId;
};

struct WaterTexList {
	int  ListCnt = 0;
	int * pList = nullptr;
	FLOAT fWaterFrame = 0.1f;
};

#define MAX_WATER	10
#define MAX_TERRAIN 256

class CTerrainMng
{
	int m_nSize;
	LPDIRECT3DDEVICE9 m_pd3dDevice;
public:
	TERRAIN m_aTerrain[MAX_TERRAIN];
	int				m_nWaterFrame;
	WaterTexList	*m_pWaterIndexList;
	FLOAT			m_fWaterFrame[MAX_WATER];

	CTerrainMng();
	~CTerrainMng();

	int GetSize() { return m_nSize; }

	BOOL LoadTexture( DWORD dwId );
	TERRAIN	* GetTerrain( DWORD dwId ) { return &m_aTerrain[ dwId ]; }
	BOOL LoadScript( LPCTSTR lpszFileName );

	HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
	HRESULT DeleteDeviceObjects();	
};

