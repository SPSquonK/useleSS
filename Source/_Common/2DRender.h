#pragma once

#include <exception>

#ifdef __CLIENT
#include "mempooler.h"
#endif
#ifndef __WORLDSERVER
#include "EditString.h"
#endif	// __CLEINT


#include "xUtil.h"
#include <memory>
#include <set>

class CRectClip : public CRect
{
public:
	CRectClip() { }
	CRectClip(int l,int t,int r,int b) : CRect(l,t,r,b) { }
	CRectClip(const RECT& srcRect) : CRect(srcRect) { }
	CRectClip(LPCRECT lpSrcRect) : CRect(lpSrcRect) { }
	CRectClip(POINT point,SIZE size) : CRect(point,size) { }
	CRectClip(POINT topLeft,POINT bottomRight) : CRect(topLeft,bottomRight) { }

	BOOL Clipping(CRect& rect) const;

	BOOL RectLapRect(CRect rect) const;
	BOOL RectInRect (CRect rect) const;
};

struct DRAWVERTEX
{
	D3DXVECTOR3 vec;
	FLOAT rhw;
	DWORD color;
};
struct TEXTUREVERTEX
{
	D3DXVECTOR3 vec;
	FLOAT rhw, u, v;
};
struct TEXTUREVERTEX2
{
	D3DXVECTOR3 vec;
	FLOAT rhw;
	DWORD color; 
	FLOAT u, v;
};
#define D3DFVF_DRAWVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE/*|D3DFVF_TEX1*/)	// FVF가 맞지않아 경고가 떠서 지웠음.-xuzhu-
#define D3DFVF_TEXTUREVERTEX (D3DFVF_XYZRHW|D3DFVF_TEX1)
#define D3DFVF_TEXTUREVERTEX2 (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)

inline void SetTextureVertex( TEXTUREVERTEX* pVertices, FLOAT x, FLOAT y, FLOAT u, FLOAT v )
{
	pVertices->vec = D3DXVECTOR3( x - 0.5f, y - 0.5f, 0 );
	pVertices->rhw   = 1.0f;
	pVertices->u     = u;
	pVertices->v     = v;
}
inline void SetTextureVertex2( TEXTUREVERTEX2* pVertices, FLOAT x, FLOAT y, FLOAT u, FLOAT v, DWORD dwColor )
{
	pVertices->vec = D3DXVECTOR3( x - 0.5f, y - 0.5f, 0 );
	pVertices->rhw   = 1.0f;
	pVertices->u     = u;
	pVertices->v     = v;
	pVertices->color = dwColor;
}

class CTexture;
class CTexturePack;


class C2DRender
{
	LPDIRECT3DVERTEXBUFFER9 m_pVBRect;
	LPDIRECT3DVERTEXBUFFER9 m_pVBFillRect;
	LPDIRECT3DVERTEXBUFFER9 m_pVBRoundRect;
	LPDIRECT3DVERTEXBUFFER9 m_pVBFillTriangle;
	LPDIRECT3DVERTEXBUFFER9 m_pVBTriangle;
	LPDIRECT3DVERTEXBUFFER9 m_pVBLine;
	LPDIRECT3DVERTEXBUFFER9 m_pVBPixel;
	
public:
	LPDIRECT3DDEVICE9 m_pd3dDevice; // The D3D rendering device
	DWORD      m_dwTextColor;
	CD3DFont*  m_pFont;
 	CPoint     m_ptOrigin ; // 뷰포트 시작 지점 
	CRectClip  m_clipRect;

	C2DRender(); 
	~C2DRender();

	CD3DFont* GetFont() { return m_pFont; }
	void SetFont( CD3DFont* pFont ) { m_pFont = pFont; }

	// 뷰포트 관련 
	CPoint GetViewportOrg() const { return m_ptOrigin; }
	void SetViewportOrg(POINT pt) { m_ptOrigin = pt; }
	void SetViewportOrg(int x,int y) { m_ptOrigin = CPoint(x,y); }
	void SetViewport(CRect rect);
	void SetViewport(int nLeft,int nTop,int nRight,int nBottom);

	BOOL ResizeRectVB( CRect* pRect, DWORD dwColorLT, DWORD dwColorRT, DWORD dwColorLB, DWORD dwColorRB, LPDIRECT3DVERTEXBUFFER9 pVB );
	BOOL RenderRect  ( CRect rect, DWORD dwColorLT, DWORD dwColorRT, DWORD dwColorLB, DWORD dwColorRB );
	BOOL RenderRect  ( CRect rect, DWORD dwColor ) { return RenderRect( rect, dwColor, dwColor, dwColor, dwColor); }
	BOOL RenderResizeRect( CRect rect, int nThick );

	BOOL ResizeFillRectVB( CRect* pRect, DWORD dwColorLT, DWORD dwColorRT, DWORD dwColorLB, DWORD dwColorRB, LPDIRECT3DVERTEXBUFFER9 pVB, LPDIRECT3DTEXTURE9 m_pTexture = NULL );
	BOOL RenderFillRect  ( CRect rect, DWORD dwColorLT, DWORD dwColorRT, DWORD dwColorLB, DWORD dwColorRB, LPDIRECT3DTEXTURE9 m_pTexture = NULL );
	BOOL RenderFillRect  ( CRect rect, DWORD dwColor, LPDIRECT3DTEXTURE9 m_pTexture = NULL ) { return RenderFillRect( rect, dwColor, dwColor, dwColor, dwColor, m_pTexture); }

	BOOL ResizeRoundRectVB( CRect* pRect, DWORD dwColorLT, DWORD dwColorRT, DWORD dwColorLB, DWORD dwColorRB, LPDIRECT3DVERTEXBUFFER9 pVB, LPDIRECT3DTEXTURE9 m_pTexture = NULL );
	BOOL RenderRoundRect  ( CRect rect, DWORD dwColorLT, DWORD dwColorRT, DWORD dwColorLB, DWORD dwColorRB, LPDIRECT3DTEXTURE9 m_pTexture = NULL );
	BOOL RenderRoundRect  ( CRect rect, DWORD dwColor, LPDIRECT3DTEXTURE9 m_pTexture = NULL ) { return RenderRoundRect( rect, dwColor, dwColor, dwColor, dwColor, m_pTexture ); }

	BOOL RenderLine( CPoint pt1, CPoint pt2, DWORD dwColor1, DWORD dwColor2 );
	BOOL RenderLine( CPoint pt1, CPoint pt2, DWORD dwColor ) { return RenderLine(pt1,pt2,dwColor,dwColor); }

	BOOL RenderTriangle( CPoint pt1, CPoint pt2, CPoint pt3, DWORD dwColor1, DWORD dwColor2, DWORD dwColor3 );
	BOOL RenderTriangle( CPoint pt1, CPoint pt2, CPoint pt3, DWORD dwColor ) { return RenderTriangle( pt1, pt2, pt3, dwColor, dwColor, dwColor ); }

	BOOL RenderFillTriangle( CPoint pt1, CPoint pt2, CPoint pt3, DWORD dwColor1, DWORD dwColor2, DWORD dwColor3 );
	BOOL RenderFillTriangle( CPoint pt1, CPoint pt2, CPoint pt3, DWORD dwColor ) { return RenderFillTriangle( pt1, pt2, pt3, dwColor, dwColor, dwColor ); }

	BOOL RenderTextureEx( CPoint pt, CPoint pt2, CTexture* pTexture, DWORD dwBlendFactorAlhpa, FLOAT fScaleX , FLOAT fScaleY, BOOL bAnti = TRUE );
	BOOL RenderTexture( CPoint pt, CTexture* pTexture, DWORD dwBlendFactorAlhpa = 255, FLOAT fScaleX=1.0 , FLOAT fScaleY=1.0 );
	BOOL RenderTextureRotate( CPoint pt, CTexture* pTexture, DWORD dwBlendFactorAlhpa, FLOAT fScaleX , FLOAT fScaleY, FLOAT fRadian );
	
	//added by gmpbigsun: 회전축 변경 
	BOOL RenderTextureRotate( CPoint pt, CTexture* pTexture, DWORD dwBlendFactorAlhpa, FLOAT fRadian, BOOL bCenter, FLOAT fScaleX, FLOAT fScaleY );
	
	BOOL RenderTexture2( CPoint pt, CTexture* pTexture, FLOAT fScaleX = 1.0, FLOAT fScaleY = 1.0, D3DCOLOR coolor = 0xffffffff );
	BOOL RenderTextureEx2( CPoint pt, CPoint pt2, CTexture* pTexture, DWORD dwBlendFactorAlhpa, FLOAT fScaleX , FLOAT fScaleY, D3DCOLOR color );
	BOOL RenderTextureColor( CPoint pt, CTexture* pTexture, FLOAT fScaleX , FLOAT fScaleY, D3DCOLOR color );
		
	void SetTextColor( DWORD dwColor ) { m_dwTextColor = dwColor; }
	DWORD GetTextColor() { return m_dwTextColor; }
#ifdef __CLIENT
	void TextOut_EditString( int x,int y, CEditString& strEditString, int nPos = 0, int nLines = 0, int nLineSpace = 0 );
#endif
	void TextOut( int x,int y, LPCTSTR pszString, DWORD dwColor = 0xffffffff, DWORD dwShadowColor = 0x00000000 );
	void TextOut( int x,int y, int nValue, DWORD dwColor = 0xffffffff, DWORD dwShadowColor = 0x00000000 );
	void TextOut( int x,int y, FLOAT fXScale, FLOAT fYScale, LPCTSTR pszString, DWORD dwColor = 0xffffffff, DWORD dwShadowColor = 0x00000000 );

	HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
	HRESULT RestoreDeviceObjects(  D3DSURFACE_DESC*  pd3dsdBackBuffer );
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();
};
class CTexture
{
	BOOL m_bAutoFree;
public:
	SIZE  m_sizePitch;
	SIZE  m_size;
	FLOAT m_fuLT, m_fvLT;
	FLOAT m_fuRT, m_fvRT;
	FLOAT m_fuLB, m_fvLB;
	FLOAT m_fuRB, m_fvRB;
	POINT m_ptCenter;
	LPDIRECT3DTEXTURE9 m_pTexture;
#ifdef _DEBUG
	CString m_strFileName;
#endif
	

	CTexture();
	~CTexture();

	BOOL DeleteDeviceObjects();
	void SetAutoFree( BOOL bAuto ) { m_bAutoFree = bAuto; }


	LPDIRECT3DTEXTURE9 GetTexture() { return m_pTexture; }
	BOOL CreateTexture( int nWidth, int nHeight, 
						   UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool = D3DPOOL_DEFAULT );
	BOOL LoadTexture( LPCTSTR pFileName, D3DCOLOR d3dKeyColor, BOOL bMyLoader = FALSE );
	//BOOL LoadTextureFromRes( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR pFileName, D3DCOLOR d3dKeyColor, BOOL bMyLoader = FALSE );
	void Render( C2DRender* p2DRender, CPoint pt, DWORD dwBlendFactorAlhpa = 255 ) {
		p2DRender->RenderTexture( pt, this, dwBlendFactorAlhpa ); 
	}
	void Render( C2DRender* p2DRender, CPoint pt, CPoint pt2, DWORD dwBlendFactorAlhpa = 255, FLOAT fscalX = 1.0, FLOAT fscalY = 1.0 ) {
		p2DRender->RenderTextureEx( pt, pt2, this, dwBlendFactorAlhpa, fscalX, fscalY ); 
	}
	void RenderEx2( C2DRender* p2DRender, CPoint pt, CPoint pt2, DWORD dwBlendFactorAlhpa = 255, FLOAT fscalX = 1.0, FLOAT fscalY = 1.0, D3DCOLOR color = 0 ) {
		p2DRender->RenderTextureEx2( pt, pt2, this, dwBlendFactorAlhpa, fscalX, fscalY, color ); 
	}
	void RenderScal( C2DRender* p2DRender, CPoint pt, DWORD dwBlendFactorAlhpa = 255, FLOAT fscalX = 1.0, FLOAT fscalY = 1.0 ) {
		p2DRender->RenderTexture( pt, this, dwBlendFactorAlhpa , fscalX, fscalY ); 
	}
	void RenderRotate( C2DRender* p2DRender, CPoint pt, FLOAT fRadian, DWORD dwBlendFactorAlhpa = 255, FLOAT fscalX = 1.0, FLOAT fscalY = 1.0 ) {
		p2DRender->RenderTextureRotate( pt, this, dwBlendFactorAlhpa , fscalX, fscalY, fRadian ); 
	}

	//added by gmpbigsun : 회전축 변경가능 ( center or start point )
	void RenderRotate( C2DRender* p2DRender, CPoint pt, FLOAT fRadian, BOOL bCenter, DWORD dwBlendFactorAlhpa = 255, FLOAT fscalX = 1.0, FLOAT fscalY = 1.0 ) {
		p2DRender->RenderTextureRotate( pt, this, dwBlendFactorAlhpa, fRadian, bCenter, fscalX, fscalY ); 
	}
	
	void Render2( C2DRender* p2DRender, CPoint pt, D3DCOLOR color, float fscalX = 1.0f, float fscalY = 1.0f ) {
		p2DRender->RenderTexture2( pt, this, fscalX, fscalY, color ); 
	}
	void RenderScal2( C2DRender* p2DRender, CPoint pt, DWORD dwBlendFactorAlhpa = 255, FLOAT fscalX = 1.0, FLOAT fscalY = 1.0, D3DCOLOR color = 0 ) {
		p2DRender->RenderTextureColor( pt, this, fscalX, fscalY, color ); 
	}
};
class CTexturePack
{
public:
	DWORD m_dwNumber;
	CSize m_size;
	LPDIRECT3DTEXTURE9 m_pTexture;
	CTexture* m_ap2DTexture;
 
	CTexturePack();
	~CTexturePack();

	BOOL DeleteDeviceObjects();
	[[nodiscard]] DWORD GetNumber() const noexcept { return m_dwNumber; }
	void MakeVertex( C2DRender* p2DRender, CPoint point, int nIndex, TEXTUREVERTEX** ppVertices );
	void MakeVertex( C2DRender* p2DRender, CPoint point, int nIndex, TEXTUREVERTEX2** ppVertices, DWORD dwColor );
	void Render( TEXTUREVERTEX* pVertices, int nVertexNum );
	void Render( TEXTUREVERTEX2* pVertices, int nVertexNum );

	virtual BOOL LoadScript( LPCTSTR pFileName );

	CTexture* LoadTexture( LPCTSTR pFileName, D3DCOLOR d3dKeyColor );
	CTexture* GetAt( DWORD dwIndex ) {
		return &m_ap2DTexture[ dwIndex ];
	}
	void Render( C2DRender* p2DRender, CPoint pt, DWORD dwIndex, DWORD dwBlendFactorAlhpa = 255, FLOAT fScaleX=1.0f , FLOAT fScaleY=1.0f ) 
	{
		if (dwIndex >= m_dwNumber || (int)dwIndex < 0) {
			Error("CTexturePack::Render : %lu", dwIndex);
			throw std::exception("Trying to render an unexisting texture");
		}
		p2DRender->RenderTexture( pt, &m_ap2DTexture[ dwIndex ], dwBlendFactorAlhpa, fScaleX , fScaleY ); 
	}
};

class CWndBase;

class CTextureMng final {
private:
	std::map<std::string, std::unique_ptr<CTexture>> m_mapTexture;
	std::set<std::string> m_failedTextures;
public:
	CTexture * AddTexture(LPCTSTR pFileName, D3DCOLOR d3dKeyColor, BOOL bMyLoader = FALSE);
	void Clear() { m_mapTexture.clear(); m_failedTextures.clear(); }
};

#ifdef __CLIENT
// 몹, 플레이어 데미지 출력
class CDamageNum final
{
public:
	DWORD m_nFrame = 0;
	DWORD m_nNumber;
	DWORD m_nAttribute;
	D3DXVECTOR3 m_vPos;
	FLOAT	m_fY = 0.0f, m_fDy = 0.0f;
	int		m_nState = 0;
	int		m_nCnt = 0;

	CDamageNum(D3DXVECTOR3 vPos, DWORD nNumber, DWORD nAttribute) {
		m_nNumber = nNumber; m_vPos = vPos; m_nAttribute = nAttribute;
	}

	void Process();
	void Render(CTexturePack *textPackNum);
};

// 데미지 숫자 관리자
class CDamageNumMng final {
	std::vector<CDamageNum> m_damages;
public:
	D3DXMATRIX m_matProj,m_matView,m_matWorld;
	D3DVIEWPORT9 m_viewport;
	CTexturePack m_textPackNum;

	CDamageNumMng() { D3DXMatrixIdentity(&m_matWorld); };
	~CDamageNumMng();

	void DeleteDeviceObjects();
	void LoadTexture();
	void AddNumber(D3DXVECTOR3 vPos,DWORD nNumber,DWORD nAttribute); // 새로운 데미지 표시를 생성시킨다.
	void Process(); // 현재 생성된 데미지 표시들을 처리한다.
	void Render(); // 현재 생성된 데미지 표시들을 출력한다.
};
#endif
