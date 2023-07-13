#ifndef		__MATERIAL_H__
#define		__MATERIAL_H__

#include <d3d9.h>

struct MATERIAL
{
	D3DMATERIAL9			m_Material;
	LPDIRECT3DTEXTURE9		m_pTexture;		// 텍스쳐메모리 포인터
	char			strBitMapFileName[64];	// 텍스쳐 파일명
	int						m_nUseCnt;		// 같은 텍스쳐가 몇번 사용됐는지 ...
	BOOL					m_bActive;
};

#define		MAX_MATERIAL		2048

class CTextureManager
{
protected:
	int		m_nMaxTexture;
	
public:
	MATERIAL	m_pMaterial[ MAX_MATERIAL ];		// 게임에서 사용하는 모든 매트리얼이 들어간다.
	CTextureManager();
	~CTextureManager();

	HRESULT DeleteDeviceObjects();

	int		DeleteMaterial( LPDIRECT3DTEXTURE9 pTexture );	// pTexture를 사용하는 매터리얼을 찾아 삭제한다.
	
	D3DMATERIAL9			*GetMaterial( int nIdx ) { return &m_pMaterial[ nIdx ].m_Material; }
	LPDIRECT3DTEXTURE9		GetTexture( int nIdx ) { return m_pMaterial[ nIdx ].m_pTexture; }

	MATERIAL	*AddMaterial( LPCTSTR strFileName, LPCTSTR szPath = NULL );		// 매터리얼 하나를 추가하고 그 포인터를 되돌린다.
};

extern CTextureManager		g_TextureMng;


#endif

