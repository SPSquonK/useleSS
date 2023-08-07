#ifndef	__OBJECT3D_H__
#define	__OBJECT3D_H__

#include <d3dx9.h>
#include "Material.h"
#include "Bone.h"
#include <span>

#define D3DFVF_SKINVERTEX_BUMP (D3DFVF_XYZB3 | D3DFVF_LASTBETA_UBYTE4 | D3DFVF_NORMAL | D3DFVF_NORMAL | D3DFVF_TEX1)
#define D3DFVF_SKINVERTEX (D3DFVF_XYZB3 | D3DFVF_LASTBETA_UBYTE4 | D3DFVF_NORMAL | D3DFVF_TEX1)
#define D3DFVF_NORMALVERTEX_BUMP (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_NORMAL | D3DFVF_TEX1)
#define D3DFVF_NORMALVERTEX (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)

// Mesh Type
enum GMTYPE
{
	GMT_ERROR = -1,
	GMT_NORMAL,
	GMT_SKIN,
	GMT_BONE
};

// Vertex Buffer Type
enum VBTYPE
{
	VBT_ERROR = -1,
	VBT_NORMAL,
	VBT_BUMP,
};

struct SKINVERTEX
{
    D3DXVECTOR3 position;	// The 3D position for the vertex
	FLOAT		w1, w2;		// vertex weight
	DWORD		matIdx;
    D3DXVECTOR3 normal;		// The surface normal for the vertex
    FLOAT       tu, tv;		// The texture coordinates
};

#ifdef __YENV
struct SKINVERTEX_BUMP
{
    D3DXVECTOR3 position;	// The 3D position for the vertex
	FLOAT		w1, w2;		// vertex weight
	DWORD		matIdx;
    D3DXVECTOR3 normal;		// The surface normal for the vertex
	D3DXVECTOR3 tangent;	
	FLOAT       tu, tv;		// The texture coordinates
};
#endif //__YENV

struct NORMALVERTEX
{
    D3DXVECTOR3 position;	// The 3D position for the vertex
    D3DXVECTOR3 normal;		// The surface normal for the vertex
    FLOAT       tu, tv;		// The texture coordinates
};

#ifdef __YENV
struct NORMALVERTEX_BUMP
{
    D3DXVECTOR3 position;	// The 3D position for the vertex
    D3DXVECTOR3 normal;		// The surface normal for the vertex
	D3DXVECTOR3 tangent;	
	FLOAT       tu, tv;		// The texture coordinates
};
#endif //__YENV

struct	MATERIAL_BLOCK
{
	int		m_nStartVertex;			// 버텍스 버퍼에서의 시작 인덱스
	int		m_nPrimitiveCount;		// 프리미티브(삼각형)갯수
	int		m_nTextureID;			// 텍스쳐 번호
	DWORD	m_dwEffect;
	int		m_nAmount;		// 투명도
	int		m_nMaxUseBone;
	int		m_UseBone[CBones::MAX_VS_BONE];
};

//
// 버텍스와 면정보를 갖는 메쉬형태의 데이타 구조
// Geometry Object
//
struct GMOBJECT		// base object
{
	GMTYPE	m_Type;					// GMOBJECT Type
	int			m_nVertexSize;		// sizeof(FVF)
	DWORD		m_dwFVF;			// FVF 포맷
	int			m_nMaxUseBone;		
	int			m_UseBone[ CBones::MAX_VS_BONE ];
	int			m_nID;
	int			m_bOpacity;
	int			m_bBump;
	int			m_bLight;
	int			m_nMaxVertexList;			// read
	int			m_nMaxFaceList;				// read
	int			m_nMaxMtrlBlk;				// read
	int			m_nMaxMaterial;				// read
	GMTYPE		m_ParentType;				// 부모의 타입
	int			m_nParentIdx;				// read		// 스킨일땐 본의 인덱스가 되고 일반일땐 GMOBJECT의 인덱스
	GMOBJECT*	m_pParent;					// 부모, 바이페드를 부모로 가진 링크메시는 이걸안쓴다.
	D3DXMATRIX		m_mLocalTM;			// read
	D3DXMATRIX		m_mInverseTM;			// read 미리 변환된 인버스TM 

	D3DXVECTOR3		*m_pVertexList;			// read 각 관절에 대해 Local좌표가 들어감

	int				*m_pPhysiqueVertex;		// read 피지크, 각 버텍스 피지x크들이 참조하는 본 ID
	MATERIAL_BLOCK	*m_pMtrlBlk;			// read
	LPDIRECT3DTEXTURE9	*m_pMtrlBlkTexture;		// 매터리얼 블럭내 텍스쳐포인터

#ifdef __YENV
	VBTYPE		m_VBType;
	LPDIRECT3DTEXTURE9	*m_pNormalTexture;		// 노말맵(범프용)
	LPDIRECT3DTEXTURE9	*m_pNoSpecTexture;		// 노스팩큘러맵(범프용)
#endif //__YENV
	
	TM_ANIMATION	*m_pFrame;				// 애니메이션이 있으면 프레임수만큼 있다

	D3DXVECTOR3		m_vBBMin, m_vBBMax;			// read bound box min, max

	struct TextureName { char strBitMapFileName[64]; };
	int				m_bMaterial;				// MES파일에 매터리얼이 있었는지?.  저장을 위해서 백업 받아둠
	TextureName		m_MaterialAry[16];			// MES에 저장되어 있던 Material정보.  Save때나 다시 매터리얼을 셋팅 해야할 필요가 있을때 사용

	// void*의 VB를 쓸땐 항상 캐스팅을 할것.
//	NORMALVERTEX	*m_ptempVB;
	void			*m_pVB;			// Indexed용 버텍스 버퍼 - 버텍스 리스트와 다른점은 좌표는 같으나 TU,TV가 다른 버텍스들을 따로 빼낸것이다. matIdx는 가지고 있지 않다.

	WORD			*m_pIB;			// 인덱스 버퍼, m_pIIndexBuffer의 복사본.
	WORD			*m_pIIB;		// m_pVB가 어느 m_pVertexList에 대응되는지에 대한 인덱스, m_nMaxIVB, 스키닝에 쓰임.  버텍스 버퍼와 버텍스리스트의 크기가 다르기때문에 이런게 필요하다.
	int				m_nMaxVB;
	int				m_nMaxIB;

	// 스킨형 오브젝트일때는 VB는 NULL이다. 외부지정VB를 따로 써야 한다.
	LPDIRECT3DVERTEXBUFFER9		m_pd3d_VB;		// Indexed용 VB
	LPDIRECT3DINDEXBUFFER9		m_pd3d_IB;		// Indexed용 IB

};

//
// 3D 오브젝트
// 인간, 몬스터, 나무, 회전관람차 등.....
// Object3D는 GMOBJECT들로써 구성된다.  클래스내에선 편의상 Object로 부른다.
// o3d파일 이다.
// 3D 오브젝트 데이타의 최소단위.
// 
//
/*
여러가지 유형
* 스킨,스킨,스킨,일반,일반,일반
. 스킨들은 한셋트의 본에만 영향을 받음.
. 일반들은 각자 최종업데이트용 매트릭스를 가짐.

스킨
. 본에만 영향 받음.

일반,일반,일반,일반,일반
. 각자 매트릭스를 가짐.

*/

#define		MAX_GROUP		3

#define		LOD_HIGH	0
#define		LOD_MEDIUM	1
#define		LOD_LOW		2

struct LOD_GROUP
{
	int			m_nMaxObject;
	GMOBJECT*	m_pObject;		// sizeof(GMOBJECT) * m_nMaxObject
	// 계층구조 애니메이션후 갱신된 매트릭스들이 담길 임시버퍼 m_nMaxObject만큼 생성. 
	// 만약 스킨만 있는 오브젝트라면 이것은 생성되지 않는다.
	D3DXMATRIX	*_mUpdate;
};

class CObject3D
{
private:
	LOD_GROUP		*m_pGroup;		// 현재 선택된 LOD그룹
	LOD_GROUP		m_Group[ MAX_GROUP ];		// LOD 그룹
	D3DXMATRIX	*m_pmExternBone;	// 외부에서 지정하는 본 포인터.
	D3DXMATRIX	*m_pmExternBoneInv;	// 외부에서 지정하는 본 Inv 포인터.
	
	int			m_nMaxBone;			// 이오브젝트가 사용하는 뼈대의 개수. 외부에서 뼈대를 지정할때도 이 개수와 맞는지 확인하자.
	D3DXMATRIX	*m_pBaseBone;		// o3d내에 들어있는 디폴트 뼈대 셋트. 이미계산 끝난 matWorld리스트가 들어있다. 뼈대를 쓰지 않는다면 이것은 NULL이다.
	D3DXMATRIX	*m_pBaseBoneInv;	// BaseBone의 Inverse. 이것은 destroy하지 앟는다.
	CMotion		*m_pMotion;			// o3d에 본애니메이션이 포함되어 있다면 할당됨.
	
	int		m_nMaxEvent;

	int		m_nTextureEx;			// 몇번 텍스쳐 세트로 렌더 해야하는가.
	MOTION_ATTR		*m_pAttr;			// 프레임크기만큼 할당.  각 프레임속성.
	
public:
	D3DXVECTOR3	m_vEvent[CBones::MAX_MDL_EVENT];
	int		m_bSendVS;
	int		m_bLOD;						// LOD데이타가 있는가?
	int		m_nID;						// 오브젝트의 고유 아이디
	int		m_nHavePhysique;			// 피지크 오브젝트를 가지고 있는가.
	char	m_szFileName[64];			// 오브젝트 파일 명
	GMOBJECT	m_CollObject;	// 충돌용 메시
	
	// 검광지점은 인덱스로는 안되고 실제 버텍스 위치를 기록해야 할것이다.
	D3DXVECTOR3		m_vBBMin, m_vBBMax;			// read bound box min, max
	D3DXVECTOR3		m_vForce1, m_vForce2;		// 검광 시작점,끝점
	D3DXVECTOR3		m_vForce3, m_vForce4;			// 추가지점 시작점,끝점

	int		m_nMaxFace;					// 오브젝트들의 폴리곤 갯수 총합
	int		m_nNoTexture;				// 텍스쳐를 사용하지 않음.
	int		m_nNoEffect;				// 상태셋팅을 건드리지 않음.
	
	// 만약 자체 키프레임 애니메이션이 있을경우 사용된다.
	float		m_fPerSlerp;
	int			m_nMaxFrame;

	int		m_nUseCnt;		// 몇번 공유되어 사용되었는가.	

	FLOAT	m_fAmbient[3];

#ifdef __BS_EFFECT_LUA
	DWORD m_dwEffect_EX;	// Render effect 확장

	struct MTE_DATA	// Multi Texturing Effect
	{
		MTE_DATA() { _pTex[ 0 ] = NULL; _pTex[ 1 ] = NULL; _kUV[0] = D3DXVECTOR3( 0.f, 0.f, 0.f ); _kUV[1] = D3DXVECTOR3( 0.f, 0.f, 0.f ); }
		LPDIRECT3DTEXTURE9 _pTex[2];
		D3DXVECTOR3 _kUV[2];
	};
	MTE_DATA* m_pMteData;
#endif //__BS_EFFECT_LUA
	
private:
	void	Init( void );
	void	Destroy( void );

	int		LoadGMObject( CResFile *file, GMOBJECT *pObject );
	int		LoadTMAni	( CResFile *file, GMOBJECT *pObject );
	int		SaveGMObject( FILE *fp, GMOBJECT *pObject );
	int		SaveTMAni	( FILE *fp, GMOBJECT *pObject );

public:

#ifdef __YENV
	LPDIRECT3DVERTEXDECLARATION9 m_pNormalDecl;
	HRESULT						ExtractBuffers( int nType, LPDIRECT3DVERTEXBUFFER9 *ppd3d_VB, GMOBJECT *pObject );
	LPDIRECT3DTEXTURE9			CreateNormalMap( int nType, LPDIRECT3DTEXTURE9* pTexture, LPCTSTR strFileName, LPCTSTR szPath = NULL );
#endif //__YENV
	
	CObject3D();
	~CObject3D();

	const int	GetMaxEvent( )		{ return m_nMaxEvent; }
	GMOBJECT*	GetGMOBJECT( int nIdx = 0 ) { return &m_pGroup->m_pObject[ nIdx ]; }
	int		GetMaxObject( void ) { return m_pGroup->m_nMaxObject; }
	int		GetMaxFrame( void ) { return m_nMaxFrame; }
	int		GetMaxBone( void )	{ return m_nMaxBone; }
	D3DXMATRIX *GetBaseBone( void ) { return m_pBaseBone; }
	void	SetExternBone( D3DXMATRIX *pmBone, D3DXMATRIX *pmBoneInv ) { if(m_pBaseBone == NULL) { m_pmExternBone = pmBone; m_pmExternBoneInv = pmBoneInv; } }	// 스킨만이 m_pBaseBone을 이미 할당하고 있다.
//	int		Find( int nID );
	void	SetTexture( LPCTSTR szTexture );		// 수동으로 텍스쳐를 지정함.
	void	SetTexture( LPDIRECT3DTEXTURE9 pTexture );
	void	SetTextureEx( GMOBJECT *pObj, int nNumEx );
	void	SetTextureEx( int nNumEx )
	{
		for( int i = 0; i < m_pGroup->m_nMaxObject; i ++ )
			SetTextureEx( &(m_pGroup->m_pObject[i]), nNumEx );		
	}
	void	LoadTextureEx( int nNumEx, GMOBJECT *pObj, std::span<LPDIRECT3DTEXTURE9, 16> pmMaterial );
	void	ChangeTexture( LPCTSTR szSrc, LPCTSTR szDest );
	void	SetBone( D3DXMATRIX *pmBone ) { m_pmExternBone = pmBone; }
	int		GetMaxVertex( void );
	int		GetMaxFace( void );
	int		GetMaxMtrlBlk( void );
	BOOL	IsHaveCollObj( void ) { return (m_CollObject.m_Type != GMT_ERROR)? TRUE : FALSE; }
	BOOL	IsUseExternBone( void ) { if( m_nHavePhysique && m_pBaseBone == NULL ) return TRUE; else return FALSE; }	// 스킨오브젝트이고 자체 본을 가지고 있지 않다. 이것은 외장본을 사용하는 스킨이다
	void	SetLOD( int nLevel ) 
	{ 
		if( nLevel < 0 || nLevel >= MAX_GROUP )
		{
			Error( "SetLOD : lv=%d, %s", nLevel, m_szFileName );
			nLevel = 0;
		}
		if( m_bLOD == 0 ) nLevel = 0;	// LOD데이타가 없다면 항상 High레벨로만..
		m_pGroup = &m_Group[nLevel]; 
	}	// 0:high,  1:medium,   2:low

private:
	template<DWORD MA_FLAG>
	const MOTION_ATTR * GetMotionAttr(float fNumFrm) const {
		if (!m_pAttr) return nullptr;
		const MOTION_ATTR * pAttr = &m_pAttr[static_cast<int>(fNumFrm)];
		if (pAttr->m_dwAttr & MA_FLAG)
			if (pAttr->m_fFrame == fNumFrm)		// 속성은 8.0에 있는데 fNumFrm==8.5 로 들어왔을때 정확하게 비교하기 위해 필요.
				return pAttr;
		return nullptr;
	}

public:
	[[nodiscard]] bool IsAttrHit(float fNumFrm) const {
		return GetMotionAttr<MA_HIT>(fNumFrm);
	}

	[[nodiscard]] std::optional<int> IsAttrSound(float fNumFrm) const {
		const MOTION_ATTR * pAttr = GetMotionAttr<MA_SOUND>(fNumFrm);
		return pAttr && pAttr->m_nSndID > 0 ? std::optional(pAttr->m_nSndID) : std::nullopt;
	}

	[[nodiscard]] bool IsAttrQuake(float fNumFrm) const {
		return GetMotionAttr<MA_QUAKE>(fNumFrm);
	}
	
	HRESULT	CreateDeviceBuffer( GMOBJECT *pObject, LPDIRECT3DVERTEXBUFFER9 *ppd3d_VB, D3DPOOL pool = D3DPOOL_MANAGED );
	HRESULT SendVertexBuffer( GMOBJECT *pObj, LPDIRECT3DVERTEXBUFFER9 pd3d_VB );
	HRESULT SendIndexBuffer( GMOBJECT *pObj );
	int		LoadObject( LPCTSTR szFileName );
static 	BOOL IsAnimateFile( LPCTSTR szFileName );		// szFileName이 애니메이션이 있냐?
	int		SaveObject( LPCTSTR szFileName );
	void	ClearNormal( void );

	// 충돌/교차 루틴들.
	int		SlideVectorXZ( D3DXVECTOR3 *pOut, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vEnd , const D3DXMATRIX &mWorld, BOOL bCollObj/* = TRUE*/, int nSlideCnt );
	int		SlideVectorXZ2( D3DXVECTOR3 *pOut, D3DXVECTOR3 *pIntersect, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vEnd , const D3DXMATRIX &mWorld, BOOL bCollObj = TRUE);
	int		SlideVectorUnder( D3DXVECTOR3 *pOut, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vEnd , const D3DXMATRIX &mWorld, D3DXVECTOR3 *pIntersect );
	void	FindTouchTriLine( D3DXVECTOR3 **pTriOut, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vEnd , const D3DXMATRIX &mWorld, FLOAT *pDist, BOOL bCollObj = TRUE );
	D3DXVECTOR3 *IntersectRayTri( const D3DXVECTOR3 &vRayOrig, const D3DXVECTOR3 &vRayDir, const D3DXMATRIX &mWorld, D3DXVECTOR3* pvIntersect, FLOAT* pfDist, BOOL bColl = FALSE );
	BOOL	SimpleTriIntersect(D3DXMATRIX mWorld, GMOBJECT* TargetObj, D3DXMATRIX mTargetWorld);
	void	ComputeInterval(float fVV0,float fVV1,float fVV2,float fD0,float fD1,float fD2,float fD0D1,float fD0D2,float &fA,float &fB,float &fC,float &fX0,float &fX1);
	void	Animate( FLOAT fFrameCurrent, int nNextFrame );
#ifndef __WORLDSERVER
	void    SetState( MATERIAL_BLOCK* pBlock, int nEffect, DWORD dwBlendFactor );
	void    ResetState( MATERIAL_BLOCK* pBlock, int nEffect, DWORD dwBlendFactor );
	void	SetShader( const D3DXMATRIX *mWorld );
	void	Render( LPDIRECT3DVERTEXBUFFER9 *ppd3d_VB, FLOAT fFrameCurrent, int nNextFrame, const D3DXMATRIX *mWorld, int nEffect = 0, DWORD dwBlendFactor = 0xff000000 );
	void	RenderSkin( LPDIRECT3DVERTEXBUFFER9 pd3d_VB, GMOBJECT *pObj, const D3DXMATRIX *mWorld, int nEffect, DWORD dwBlendFactor = 0xff000000 );
	void	RenderNormal( GMOBJECT *pObj, const D3DXMATRIX *mWorld, int nEffect, int nBlendFactor = 255 );
	
	void	RenderBB( const D3DXMATRIX *mWorld );
#ifdef __SHADOW
	void	RenderNormalShadow( GMOBJECT *pObj, const D3DXMATRIX *mWorld, int nBlendFactor );
	void	RenderShadow( LPDIRECT3DVERTEXBUFFER9 *ppd3d_VB, FLOAT fFrameCurrent, int nNextFrame, const D3DXMATRIX *mWorld, int nBlendFactor = 255 );
//	void	RenderShadow( FLOAT fFrameCurrent, int nNextFrame, const D3DXMATRIX *mWorld, int nBlendFactor );
#endif
#endif
	
	HRESULT InitDeviceObjects( ) { return S_OK; }
	HRESULT RestoreDeviceObjects( LPDIRECT3DVERTEXBUFFER9 *ppd3d_VB, D3DPOOL pool = D3DPOOL_MANAGED );
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();
};

/* A collection of CObject3D. It enables the user to load the objects.
The CObject3DMng remains the owner of the CObject3D: users must call the
DeleteObject3D when they stop using a loaded CObject3D.

The same CObject3D instance may be returned multiple times.
*/
class	CObject3DMng final {
private:
	std::map<std::string, std::unique_ptr<CObject3D>, std::less<>> m_mapObject3D;

public:
	~CObject3DMng();

	[[nodiscard]] CObject3D * LoadObject3D(LPCTSTR szFileName);
	void DeleteObject3D(CObject3D * pObject3D);

	void Destroy();
};

extern CObject3DMng		g_Object3DMng;
extern D3DXMATRIX	g_mReflect;

HRESULT		CreateShadowVS( LPCTSTR szFileName );
HRESULT		CreateSkinningVS( LPCTSTR szFileName );
void		DeleteVertexShader( );


void	SetTransformViewProj( const D3DXMATRIX &mViewProj );

D3DXVECTOR3 *GetLastPickTri( void );		// 마지막으로 피킹한 삼각형버텍스의 시작포인터.



#endif // OBJECT3D
