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
	LPDIRECT3DTEXTURE9 m_pTexture;	// 4바이트 정렬을 위해서 옮김.-xuzhu-
	int m_nTextureEx;			// 텍스쳐 셋 번호 0 ~ 7

	BYTE  m_bMark;
	BYTE  m_bFly      : 1;
	BYTE  m_dwDistant : 2;
	BYTE  m_bPick     : 1; // 캐릭터 이동시 마우스 피킹에 걸리는 오브젝트인가?
	BYTE  m_bUsed     : 1;
	BYTE  m_bTrans    : 1; // 캐릭터를 가릴 시 반투명이 되는 오브젝트인가?
	BYTE  m_bShadow   : 1;
	BYTE  m_bReserved : 1;
	BYTE m_bRenderFlag : 1;

	TCHAR * GetMotion(int i) {
		if (i < 0 || i >= m_nMax) {
			Error("MODELELEM : out of range %d", i);
			i = 0;
		}
		return m_apszMotion ? &m_apszMotion[i * 32] : nullptr;
	}

};

class CModel;
class CModelObject;

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
	CModel* LoadModel( LPDIRECT3DDEVICE9 pd3dDevice, int nType, int nIndex, BOOL bParts = FALSE );
	CModel* LoadModel( LPDIRECT3DDEVICE9 pd3dDevice, TCHAR* lpszFileName, MODELELEM * lpModelElem, int nType, BOOL bParts = FALSE ); //int nModelType, LPCTSTR lpszTexture = NULL );

	BOOL LoadScript( LPCTSTR lpszFileName );

	// dx 초기화 관련 
	HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
	HRESULT RestoreDeviceObjects(LPDIRECT3DDEVICE9 pd3dDevice);
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();
};
/////////////////////////////////////////////////////////////////////////////

