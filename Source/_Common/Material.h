#pragma once

#include <d3d9.h>

#define		MAX_MATERIAL		2048

class CTextureManager final {
public:
	struct ManagedTexture {
		LPDIRECT3DTEXTURE9 m_pTexture;
		char               strBitMapFileName[64];
		int  m_nUseCnt;
	};


private:
	D3DMATERIAL9 m_defaultMaterial;

	std::array<ManagedTexture, MAX_MATERIAL> m_pMaterial;

public:
	CTextureManager();
	~CTextureManager();

	HRESULT DeleteDeviceObjects();

	bool DeleteMaterial(LPDIRECT3DTEXTURE9 pTexture);
	

	[[nodiscard]] D3DMATERIAL9 * GetShadowMaterial() { return &m_defaultMaterial; }

	ManagedTexture * AddMaterial(LPCTSTR strFileName, LPCTSTR szPath = nullptr);
};

extern CTextureManager		g_TextureMng;
