#include "stdafx.h"
#include <d3d9.h>
#include <d3dx9.h>

#include <stdio.h>
#include "material.h"
#include "..\_DirectX\dxutil.h"

#include "HwOption.h"
#include "path.h"
#include "vutil.h"
#include "xutil.h"


CTextureManager		g_TextureMng;

CTextureManager::CTextureManager() {
	memset( m_pMaterial.data(), 0, sizeof(m_pMaterial));

#ifndef __CHERRY
	m_defaultMaterial.Ambient  = D3DCOLORVALUE{ 1.f, 1.f, 1.f, 0.f };
	m_defaultMaterial.Diffuse  = D3DCOLORVALUE{ 1.f, 1.f, 1.f, 0.f };
	m_defaultMaterial.Specular = D3DCOLORVALUE{ 1.f, 1.f, 1.f, 0.f };
	m_defaultMaterial.Emissive = D3DCOLORVALUE{ 0.f, 0.f, 0.f, 0.f };
	m_defaultMaterial.Power    = 0.0f;
#endif
}

CTextureManager::~CTextureManager() {
	DeleteDeviceObjects();
}

HRESULT CTextureManager::DeleteDeviceObjects() {
	for (int i = 0; i < MAX_MATERIAL; i++) {
		if (m_pMaterial[i].m_nUseCnt > 0) {
			SAFE_RELEASE(m_pMaterial[i].m_pTexture);
			m_pMaterial[i].strBitMapFileName[0] = 0;
			m_pMaterial[i].m_nUseCnt = 0;
		}
	}

	return  S_OK;
}

// Find materials that use pTexture and delete them.
// If the texture is shared, look at the usage counter and delete only the one with 1.
bool CTextureManager::DeleteMaterial( LPDIRECT3DTEXTURE9 pTexture )
{
	if( !pTexture )	return false;

	for (ManagedTexture & material : m_pMaterial) {
		if (material.m_nUseCnt == 0) continue;

		if (material.m_pTexture != pTexture) continue;

		if (material.m_nUseCnt == 1) {
			SAFE_RELEASE(material.m_pTexture);
			material.m_nUseCnt = 0;
			material.strBitMapFileName[0] = 0;

			return true;
		} else {
			// SquonK: ??? We do not decrease m_nUseCnt?
			// That means that if a texture is shared at one point, it
			// will never be deleted. Is it intended?
			return false;
		}
	}

	return false;
}

CTextureManager::ManagedTexture * CTextureManager::AddMaterial( LPCTSTR strFileName, LPCTSTR szPath ) {
	// Already in cache?
	auto it = std::find_if(
		m_pMaterial.begin(), m_pMaterial.end(),
		[&](const ManagedTexture & material) {
			return material.m_nUseCnt > 0 && strcmpi(strFileName, material.strBitMapFileName) == 0;
		}
	);

	if (it != m_pMaterial.end()) {
		it->m_nUseCnt += 1;
		return &*it;
	}


	// Load the texture
	char strPath[256];
	if (szPath) {
		std::strcpy(strPath, szPath);
	} else if (g_Option.m_nTextureQuality == 0) {
		std::strcpy(strPath, DIR_MODELTEX);
	} else if (g_Option.m_nTextureQuality == 1) {
		std::strcpy(strPath, DIR_MODELTEXMID);
	} else {
		std::strcpy(strPath, DIR_MODELTEXLOW);
	}

	std::strcat(strPath, strFileName);

	LPDIRECT3DTEXTURE9 pTexture = nullptr;
	if( FAILED( LoadTextureFromRes( strPath, 
			  D3DX_DEFAULT, D3DX_DEFAULT, 4, 0, D3DFMT_UNKNOWN, //D3DFMT_A4R4G4B4, 
			  D3DPOOL_MANAGED,  D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR              , 
			   D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR               , 0x00000000, NULL, NULL, &pTexture ) ) )

	{
		if( !IsEmpty(strFileName) )
			Error( "%s texture bitmap", strPath );
	}

	// Look for an empty slot
	it = std::find_if(
		m_pMaterial.begin(), m_pMaterial.end(),
		[](const ManagedTexture & material) { return material.m_nUseCnt == 0; }
	);

	if (it == m_pMaterial.end()) {
		Error("CTextureManager::AddMaterial : Exceeded the number of textures.");
		return nullptr;
	}

	strcpy( it->strBitMapFileName, strFileName );
	it->m_pTexture = pTexture;
	it->m_nUseCnt = 1;

	return &*it;
}

