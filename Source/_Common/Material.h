#pragma once

#include <d3d9.h>

struct MaterialessMATERIAL {
	LPDIRECT3DTEXTURE9 m_pTexture;
	char               strBitMapFileName[64];
	int  m_nUseCnt;
	BOOL m_bActive;
};

#define		MAX_MATERIAL		2048

class CTextureManager final {
private:
	int		m_nMaxTexture;
	D3DMATERIAL9 m_defaultMaterial;

public:
	MaterialessMATERIAL	m_pMaterial[ MAX_MATERIAL ];
	CTextureManager();
	~CTextureManager();

	HRESULT DeleteDeviceObjects();

	int		DeleteMaterial( LPDIRECT3DTEXTURE9 pTexture );
	

	[[nodiscard]] D3DMATERIAL9 * GetShadowMaterial() { return &m_defaultMaterial; }

	MaterialessMATERIAL * AddMaterial(LPCTSTR strFileName, LPCTSTR szPath = nullptr);
};

extern CTextureManager		g_TextureMng;
