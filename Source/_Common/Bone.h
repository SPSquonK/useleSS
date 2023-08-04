#pragma once

#include <d3dx9.h>
#include "file.h"
#include <functional>
#include "sqktd/dynamic_array.hpp"

struct BONE {
	BONE *			m_pParent;    // Parent node

	int					m_nParentIdx; // Parent index
	char				m_szName[32]; // Bode name
	D3DXMATRIX	m_mTM;        // original TM
	D3DXMATRIX	m_mInverseTM; // pre-converted inverseTM
	D3DXMATRIX	m_mLocalTM;   // pretransformed local TM
};



//
// 본셋트의 구조
class CBones final
{
public:
	static constexpr size_t MAX_VS_BONE   = 28; // The maximum number of skeleton registers available in VS.
	static constexpr int    MAX_MDL_EVENT = 8;  // #event xx

	int		m_nID;                            // Unique ID of the bone (located in the file)
	char	m_szName[64];                     // The name of the model's file - this will be removed later.
	sqktd::dynamic_array<BONE> m_pBones;
	int		m_bSendVS;                        // Whether the number of bones is less than MAX_VS_BONE...

	D3DXMATRIX	m_mLocalRH;                 // Relative transformation matrice from RHand of weapon
	D3DXMATRIX	m_mLocalLH;                 // Relative transformation matrice from LHand of weapon
	D3DXMATRIX	m_mLocalShield;             // Relative transformation matrice from left forarm of shields
	D3DXMATRIX	m_mLocalKnuckle;            // Relative transformation matrice from RHand of knuckles
	int		m_nRHandIdx;                      // Index of the right hand
	int		m_nLHandIdx;                      // Index of the left hand
	int		m_nRArmIdx;                       // Index of the right forarm
	int		m_nLArmIdx;                       // Index of the left forarm
	D3DXVECTOR3	m_vEvent[MAX_MDL_EVENT];	
	int		m_nEventParentIdx[MAX_MDL_EVENT];	// Parent indices of event coordinates.
	
public:
	CBones();

	[[nodiscard]] const BONE * GetBone(int nIdx) const { return &m_pBones[nIdx]; }

	int		LoadBone( LPCTSTR szFileName );		// 실제로 본 파일을 읽음
};



// read only bone database
class CBonesMng final {
private:
	std::map<std::string, std::unique_ptr<CBones>, std::less<>> m_mapBones;

public:
	// Bone file is read and loaded into memory.
	const CBones * LoadBone(LPCTSTR szFileName);
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

class CMotion final
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
	BONE_FRAME	*m_pBoneFrame;		// 뼈대갯수 만큼 할당. 로딩이 끝난후 사용하기 좋도록 포인터를 재배치한다.  프레임이 없는곳은 NULL
	BONE		*m_pBoneInfo;		// 뼈대구성정보
	int		m_nMaxEvent;
	D3DXVECTOR3	m_vEvent[4];
	
	CMotion();
	~CMotion();

	//void Init( void );

	
	DWORD	GetAttr( int nNumFrm ) 
	{ 
		return m_pAttr[ nNumFrm ].m_dwAttr; 
	}

private:

	template<DWORD MA_FLAG>
	const MOTION_ATTR * GetMotionAttr(float fOldFrm, float fNumFrm) const {
		const MOTION_ATTR * pAttr = &m_pAttr[static_cast<int>(fNumFrm)];
		if (pAttr->m_dwAttr & MA_FLAG)
			if (fOldFrm < pAttr->m_fFrame && pAttr->m_fFrame <= fNumFrm)	// 이전 프레임이랑 현재 프레임 사이에 타점이 들어있었는가.
				return pAttr;
		return 0;
	}


public:

	[[nodiscard]] bool IsAttrHit(float fOldFrm, float fNumFrm) const {
		return GetMotionAttr<MA_HIT>(fOldFrm, fNumFrm);
	}

	[[nodiscard]] const MOTION_ATTR * IsAttrSound(float fOldFrm, float fNumFrm) const {
		return GetMotionAttr<MA_SOUND>(fOldFrm, fNumFrm);
	}

	[[nodiscard]] DWORD IsAttrQuake(float fOldFrm, float fNumFrm) const {
		const MOTION_ATTR * pAttr = GetMotionAttr<MA_QUAKE>(fOldFrm, fNumFrm);
		return pAttr ? pAttr->m_dwAttr : 0;
	}

	void	SetAttr( float fNumFrm, DWORD dwAttr ) 
	{
		m_pAttr[ (int)fNumFrm ].m_dwAttr |= dwAttr;
		m_pAttr[ (int)fNumFrm ].m_fFrame = fNumFrm;
	}
	void	ResetAttr( int nNumFrm, DWORD dwAttr ) { m_pAttr[ nNumFrm ].m_dwAttr &= (~dwAttr); }
	
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
