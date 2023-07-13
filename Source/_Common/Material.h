#pragma once

#include <d3d9.h>

struct MATERIAL {
	D3DMATERIAL9       m_Material;
	LPDIRECT3DTEXTURE9 m_pTexture;
	char               strBitMapFileName[64];
	int  m_nUseCnt;
	BOOL m_bActive;
};

#define		MAX_MATERIAL		2048

class CTextureManager final {
private:
	int		m_nMaxTexture;
	
public:
	MATERIAL	m_pMaterial[ MAX_MATERIAL ];
	CTextureManager();
	~CTextureManager();

	HRESULT DeleteDeviceObjects();

	int		DeleteMaterial( LPDIRECT3DTEXTURE9 pTexture );
	

	[[nodiscard]] D3DMATERIAL9 * GetShadowMaterial() { return &m_pMaterial[0].m_Material; }

	MATERIAL * AddMaterial(LPCTSTR strFileName, LPCTSTR szPath = nullptr);
};

extern CTextureManager		g_TextureMng;
