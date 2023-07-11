#pragma once

#include <d3dx9.h>
#include "file.h"
#include <functional>

#define		MAX_VS_BONE		28		// The maximum number of skeleton registers available in VS.

struct BONE {
	BONE *			m_pParent;    // Parent node

	int					m_nParentIdx; // Parent index
	char				m_szName[32]; // Bode name
	D3DXMATRIX	m_mTM;        // original TM
	D3DXMATRIX	m_mInverseTM; // pre-converted inverseTM
	D3DXMATRIX	m_mLocalTM;   // pretransformed local TM
};

#define MAX_MDL_EVENT		8	// #event xx

//
// 본셋트의 구조
class CBones final
{
public:
	int		m_nID;				// 본의 고유 ID(파일내에 있다)
	char	m_szName[64];		// 본의 파일명 - 나중에 이건 빠진다.
	int		m_nMaxBone;			// 뼈대 갯수
	std::unique_ptr<BONE[]> m_pBones;			// 뼈대 리스트
	int		m_bSendVS;			// 본개수가 MAX_VS_BONE보다 적은지...

	D3DXMATRIX	m_mLocalRH;		// 오른손 무기의 RHand로부터의 상대TM
	D3DXMATRIX	m_mLocalLH;		// 오른손 무기의 LHand로부터의 상대TM
	D3DXMATRIX	m_mLocalShield;	// 방패의 LForeArm으로부터의 상대TM
	D3DXMATRIX	m_mLocalKnuckle;	// 너클.
	int		m_nRHandIdx;		// 오른손 인덱스
	int		m_nLHandIdx;		// 왼손 인덱스
	int		m_nRArmIdx;			// 오른손 팔뚝
	int		m_nLArmIdx;			// 왼손 팔뚝 인덱스.
	D3DXVECTOR3	m_vEvent[MAX_MDL_EVENT];	
	int		m_nEventParentIdx[MAX_MDL_EVENT];	// 이벤트좌표들 부모인덱스.
	
public:
	CBones();

	BONE * GetBone(int nIdx) { return &m_pBones[nIdx]; }

	int		LoadBone( LPCTSTR szFileName );		// 실제로 본 파일을 읽음
};



// read only bone database
class CBonesMng final {
private:
	std::map<std::string, std::unique_ptr<CBones>, std::less<>> m_mapBones;

public:
	// Bone file is read and loaded into memory.
	CBones * LoadBone(LPCTSTR szFileName);
};

extern CBonesMng		g_BonesMng;


//////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////
////////////////
////////////////
////////////////
////////////////
////////////////
////////////////
////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
#define		MA_HIT			(0x00000001)		// 타점
#define		MA_SOUND		(0x00000002)		// 소리
#define		MA_QUAKE		(0x00000004)		// 지진.
struct TM_ANIMATION
{
//	D3DXMATRIX			m_mAniTM;
	D3DXQUATERNION		m_qRot;
	D3DXVECTOR3			m_vPos;
};

// 모션은 본과 밀접한 관계가 있다.  본과 크기가 같고, 같은 본에서 나온 모션이 아니면 오동작 할 수가있다.
// 캐릭터 동작하나에 대한 클래스
struct	BONE_FRAME
{
	TM_ANIMATION	*m_pFrame;
	D3DXMATRIX		m_mLocalTM;
};

// 각 프레임의 속성들
struct	MOTION_ATTR
{
	DWORD	m_dwAttr;
//	char	m_szFileName[32];
	int		m_nSndID;		// 사운드 리소스 아이디
	float	m_fFrame;		// 소숫점 단위의 정확한 프레임 번호 
};

class CMotion
{
private:
	TM_ANIMATION	*m_pMotion;			// 뼈대순서대로 각각의 애니메이션이 들어있다.  애니가 없는 뼈대는 LocalTM만 있다.
	MOTION_ATTR		*m_pAttr;			// 프레임크기만큼 할당.  각 프레임속성.
	int		m_nMaxFrame;				// 모션 프레임수
public:
	int		m_nID;						// 모션의 고유 ID(파일내에 있다)
	char	m_szName[64];				// 모션 파일명 - 나중에 이건 빠진다.
	float	m_fPerSlerp;				//
	int		m_nMaxBone;					// 이것은 어디까지나 확인용 변수일뿐이다.  실제 본의 갯수가 필요하다면 CBone에서 읽어야 한다.
	D3DXVECTOR3	m_vPivot;				// 0,0,0(발)기준에서 골반(루트)의 좌표.  0번키의 좌표와 같다.
	BONE_FRAME	*m_pBoneFrame;		// 뼈대갯수 만큼 할당. 로딩이 끝난후 사용하기 좋도록 포인터를 재배치한다.  프레임이 없는곳은 NULL
	BONE		*m_pBoneInfo;		// 뼈대구성정보
	int		m_nMaxEvent;
	D3DXVECTOR3	m_vEvent[4];
	D3DXVECTOR3		*m_pPath;			// path
	
	CMotion();
	~CMotion();

	void Init( void );
	void Destroy( void );

	MOTION_ATTR *GetMotionAttr( int nNumFrm ) { return m_pAttr + nNumFrm; }	
	DWORD	GetAttr( int nNumFrm ) 
	{ 
		return m_pAttr[ nNumFrm ].m_dwAttr; 
	}
//#if 0
	DWORD	IsAttrHit( float fOldFrm, float fNumFrm ) 
	{ 
		MOTION_ATTR		*pAttr = &m_pAttr[ (int)fNumFrm ];
		if( pAttr->m_dwAttr & MA_HIT )
			if( fOldFrm < pAttr->m_fFrame && pAttr->m_fFrame <= fNumFrm )	// 이전 프레임이랑 현재 프레임 사이에 타점이 들어있었는가.
				return pAttr->m_dwAttr;
			return 0;
	}
	MOTION_ATTR *IsAttrSound( float fOldFrm, float fNumFrm )
	{ 
		MOTION_ATTR		*pAttr = &m_pAttr[ (int)fNumFrm ];
		if( pAttr->m_dwAttr & MA_SOUND )
			if( fOldFrm < pAttr->m_fFrame && pAttr->m_fFrame <= fNumFrm )	// 이전 프레임이랑 현재 프레임 사이에 타점이 들어있었는가.
				return pAttr;
			return NULL;
	}
	DWORD	IsAttrQuake( float fOldFrm, float fNumFrm ) 
	{ 
		MOTION_ATTR		*pAttr = &m_pAttr[ (int)fNumFrm ];
		if( pAttr->m_dwAttr & MA_QUAKE )
			if( fOldFrm < pAttr->m_fFrame && pAttr->m_fFrame <= fNumFrm )	// 이전 프레임이랑 현재 프레임 사이에 타점이 들어있었는가.
				return pAttr->m_dwAttr;
			return 0;
	}
	void	SetAttr( float fNumFrm, DWORD dwAttr ) 
	{
		m_pAttr[ (int)fNumFrm ].m_dwAttr |= dwAttr;
		m_pAttr[ (int)fNumFrm ].m_fFrame = fNumFrm;
	}
	void	ResetAttr( int nNumFrm, DWORD dwAttr ) { m_pAttr[ nNumFrm ].m_dwAttr &= (~dwAttr); }
	TM_ANIMATION*	GetFrame( int nNumFrm ) { return &m_pMotion[nNumFrm]; }
	int		GetMaxFrame( void ) { return m_nMaxFrame; }

	int		LoadMotion( LPCTSTR strFileName );			// 파일에서 모션파일을 읽음
	void	ReadTM( CResFile *file, int nNumBone, int nNumFrame );

	void	AnimateBone( D3DXMATRIX *pmUpdateBone, CMotion *pMotionOld, float fFrameCurrent, int nNextFrame, int nFrameOld, BOOL bMotionTrans, float fBlendWeight );
	
};


// read only motion database
class CMotionMng final {
private:
	std::map<std::string, std::unique_ptr<CMotion>, std::less<>> m_mapMotions;

public:
	// Load the motion into memory, or return the already existing motion if
	// already loaded.
	CMotion * LoadMotion(LPCTSTR strFileName);
};

extern CMotionMng		g_MotionMng;
