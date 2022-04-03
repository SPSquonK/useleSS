#ifndef		__TERRAINMNG_H__
#define		__TERRAINMNG_H__

typedef struct tagTERRAIN
{
	LPDIRECT3DTEXTURE9 m_pTexture;
	TCHAR m_szTextureFileName[128];
	TCHAR m_szSoundFileName  [128];
	BOOL  m_bBlock;
	DWORD m_dwId;

	BOOL IsValid() { return m_szTextureFileName[0] ? TRUE : FALSE; }

} TERRAIN,* LPTERRAIN;

typedef struct tagWaterTexList
{
	int  ListCnt;
	int* pList;
	FLOAT fWaterFrame;
	tagWaterTexList()
	{
		ListCnt = 0;
		pList = NULL;
		fWaterFrame = 0.1f;
	}
}WaterTexList, *LPWATERTEXLIST;

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
	LPTERRAIN	GetTerrain( DWORD dwId ) { return &m_aTerrain[ dwId ]; }
	BOOL LoadScript( LPCTSTR lpszFileName );

	HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
	HRESULT RestoreDeviceObjects();
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();	
};

#endif