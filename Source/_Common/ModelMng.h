#pragma once

#include "2DRender.h"
#include <array>

struct MODELELEM {
	DWORD m_dwType;
	DWORD m_dwIndex;
	TCHAR m_szName [48];
	int		m_nMax;
	TCHAR* m_apszMotion; // array of 32 * m_nMax TCHARs
	DWORD m_dwModelType;
	TCHAR m_szPart[48]; 
	FLOAT m_fScale;
	LPDIRECT3DTEXTURE9 m_pTexture;
	int m_nTextureEx;			// Texture set number 0 to 7

	BYTE  m_bMark;
	BYTE  m_bFly      : 1;
	BYTE  m_dwDistant : 2;
	BYTE  m_bPick     : 1; // Is it an object that is subject to mouse picking when moving a character?
	BYTE  m_bUsed     : 1;
	BYTE  m_bTrans    : 1; // Is it an object that becomes translucent when covering a character?
	BYTE  m_bShadow   : 1;
	BYTE  m_bReserved : 1;
	BYTE m_bRenderFlag : 1;

	[[nodiscard]] TCHAR * GetMotion(int i) {
		if (i < 0 || i >= m_nMax) {
			Error("MODELELEM : out of range %d", i);
			i = 0;
		}
		return m_apszMotion ? &m_apszMotion[i * 32] : nullptr;
	}
};

class CModel;
class CModelObject;

// A sqktd::maybe_owned_ptr that is not enforced by anything.
struct ModelPointerWithOwnershipInfo {
	CModel * pModel;
	bool isOwnedByCaller;

	explicit(false) ModelPointerWithOwnershipInfo(decltype(nullptr))
		: pModel(nullptr), isOwnedByCaller(false) {
	}

	ModelPointerWithOwnershipInfo(CModel * pModel, bool isOwned)
		: pModel(pModel), isOwnedByCaller(isOwned) {}

	operator CModel * () { return pModel; }

	template<typename T>
	operator T * () { return (T *)(pModel); }

	template<typename T>
	T * DynamicCast() { return dynamic_cast<T *>(pModel); }
};

class CModelMng final {
public:
	std::map<std::string, CModelObject *> m_mapFileToMesh;
	std::array<CFixedArray<MODELELEM>, MAX_OBJTYPE> m_aaModelElem;

public:
	CModelMng() = default;
	CModelMng(const CModelMng &) = delete;
	CModelMng & operator=(const CModelMng &) = delete;
	~CModelMng();

	[[nodiscard]] MODELELEM * GetModelElem(DWORD dwType, DWORD dwIndex);
	void MakeModelName( TCHAR* pszModelName, DWORD dwType, DWORD dwIndex );
	void MakeMotionName( TCHAR* pszMotionName, DWORD dwType, DWORD dwIndex, DWORD dwMotion );
	void MakePartsName( TCHAR* pszPartsName, LPCTSTR lpszRootName, DWORD dwIndex, int nSex = SEX_SEXLESS );

	BOOL    LoadMotion( CModel* pModel, DWORD dwType, DWORD dwIndex, DWORD dwMotion );
	ModelPointerWithOwnershipInfo LoadModel(LPDIRECT3DDEVICE9 pd3dDevice, int nType, int nIndex, BOOL bParts = FALSE);
	ModelPointerWithOwnershipInfo LoadModel( LPDIRECT3DDEVICE9 pd3dDevice, TCHAR* lpszFileName, MODELELEM * lpModelElem, int nType, BOOL bParts = FALSE );

	BOOL LoadScript( LPCTSTR lpszFileName );

	// D3DX initialization related
	HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
	HRESULT RestoreDeviceObjects(LPDIRECT3DDEVICE9 pd3dDevice);
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();
};

