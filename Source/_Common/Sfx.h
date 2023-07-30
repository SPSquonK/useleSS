#pragma once

#ifndef __CLIENT
static_assert(false, "Sfx.h may only be included in client")
#endif

#include "light.h"
class CSfxGenMoveMark;
extern CSfxGenMoveMark* g_pMoveMark;

#include "mempooler.h"

class CSfxModel;

/// 효과(파티클, 발사체)를 처리하는 base class
class CSfx  : public CCtrl
{
protected:
	CSfxModel m_SfxObj2; // 연속 효과 : 사용하던지 말던지  
	CSfxModel m_SfxObj3; // 연속 효과 : 사용하던지 말던지 

	DWORD	m_dwSkill;		// 스킬발사체면 스킬 아이디를..
	
public:
	int m_nSec;
	int m_nFrame;

	CSfxModel* m_pSfxObj; // 이펙트 모델 포인터
	OBJID       m_idSrc;    // 쏜놈 ID
	OBJID       m_idDest;   // 맞을놈 ID
	D3DXVECTOR3 m_vPosDest; // 맞을 위치

	CLight  m_light;
	int		m_idSfxHit;
	int		m_nMagicPower;	// 충전 단계

	CSfx();
	CSfx( int nIndex, const OBJID idSrc, const D3DXVECTOR3& vPosSrc, const OBJID idDest, const D3DXVECTOR3& vPosDest );
	virtual ~CSfx();

	void	SetSkill( DWORD dwSkill )
	{
		m_dwSkill = dwSkill;
	}
	DWORD	GetSkill()
	{
		return m_dwSkill;
	}
	
	void Process(); // 각 프레임마다 위치이동, 애니메이션 등 처리
	void DamageToTarget( int nMaxDmgCnt = 1 ); // 목표물에 맞았을때 데미지를 준다
	BOOL SetIndex( DWORD dwIndex, BOOL bInitProp = FALSE );

	virtual	int SetSfx( int nIndex, D3DXVECTOR3 vPosSrc, OBJID idSrc, D3DXVECTOR3 vPosDest, OBJID idDest, int nSec = 0 ); // 사용할 SFX 지정
	virtual void ShootSfx( float fAngXZ, float fAngY, float fSpeed ) {}
	virtual void SetPartLink( int nPart ) {}
	virtual void Render(); // 렌더

	/* Remove the LPDIRECT3DDEVICE9 parameter */
	virtual void Render(LPDIRECT3DDEVICE9) final = delete;
};


inline CSfx * CObj::ToSfx() {
	if (GetType() != OT_SFX) return nullptr;
	return static_cast<CSfx *>(this);
}

inline const CSfx * CObj::ToSfx() const {
	if (GetType() != OT_SFX) return nullptr;
	return static_cast<const CSfx *>(this);
}

// 쏘고 날아가서 맞고 터지는 일반적인 이펙트
class CSfxShoot : public CSfx
{
public:
	BOOL	m_bHit; // 명중했는지의 여부
	BOOL	m_bDir;
	DWORD	m_dwSndHit;		// 맞았을때 소리.
	DWORD	m_dwSfxHit;		// 폭발 이펙트
	FLOAT	m_fHitScale;	// 폭발씬 스케일링.
	
	CSfxShoot();
	
	void	SetHitSfx( LPCTSTR szFileName, OBJID idSrc, OBJID idDst, FLOAT fScale = 1.0f );	// 맞았을때 이펙트 등록
	void	SetHitSfx( DWORD dwIndex, OBJID idSrc, OBJID idDst, FLOAT fScale = 1.0f );	// 맞았을때 이펙트 등록
	void	SetDir( BOOL bDir );		// 방향성있는 sfx 인가.
	void	SetSndHit( DWORD dwSndHit ) { m_dwSndHit = dwSndHit; }
	
	void Process() override;
	void Render() override;
};


// CSfx랑 똑같고 idSrc의 회전방향까지 똑같은거.
class CSfxRotate : public CSfx
{
public:
	void Process() override;
	void Render() override;
};

class CSfxDuelParty : public CSfx
{
public:
	int	m_nType = 0; // 0:듀얼  1:전쟁.

	void DeleteSfx(CMover * pEnemy);

	void Process() override;
	void Render() override;
};

class CSfxSetItem : public CSfx
{
public:
	~CSfxSetItem();
	void Process() override;
	void Render() override;
};


class CSfxCollect : public CSfx {
public:
	void Process() override;
	void Render() override;
};

// 클락워크 보스 레이저 충전.
class CSfxClockWorksCharge : public CSfx {
public:
	void Process() override;
	void Render() override;
};

// 클락워크 보스 왼팔에서 발사할때 나오는 이펙트.
class CSfxClockWorksCannon : public CSfx {
public:
	void Process() override;
	void Render() override;
};

// 머쉬무트 보스 레이저 충전.
class CSfxMushmootCharge : public CSfx {
public:
	int	m_nEventPos;
	void Process() override;
	void Render() override;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 일반 이펙트 : 통상 데미지
class CSfxGenNormalDmg01 : public CSfx
{
public:
	void Process() override;
	void Render() override;
public:
#ifndef __VM_0820
#ifndef __MEM_TRACE
	static	MemPooler<CSfxGenNormalDmg01>* m_pPool;
	void*	operator new( size_t nSize )	{	return CSfxGenNormalDmg01::m_pPool->Alloc();	}
	void*	operator new( size_t nSize, LPCSTR lpszFileName, int nLine )	{	return CSfxGenNormalDmg01::m_pPool->Alloc();	}
	void	operator delete( void* lpMem )	{	CSfxGenNormalDmg01::m_pPool->Free( (CSfxGenNormalDmg01*)lpMem );	}
	void	operator delete( void* lpMem, LPCSTR lpszFileName, int nLine )	{	CSfxGenNormalDmg01::m_pPool->Free( (CSfxGenNormalDmg01*)lpMem );	}
#endif	// __MEM_TRACE
#endif	// __VM_0820
};

// 극단스킬 이펙트 : 스트레칭01
class CSfxTroStretching01 : public CSfx {
public:
	void Process() override;
	void Render() override;
};
// 극단스킬 이펙트 : 스트레칭02
class CSfxTroStretching02 : public CSfx {
public:
	void Process() override;
	void Render() override;
};
// 극단스킬 이펙트 : 집중공격
class CSfxTroBlitz : public CSfx {
public:
	void Process() override;
	void Render() override;
};

// 일반 이펙트 : 아직 안씀
class CSfxGenSuperDmg01 : public CSfx {
public:
	void Process() override;
	void Render() override;
};
// 일반 이펙트 : 아직 안씀
class CSfxGenSkillDmg01 : public CSfx {
public:
	void Process() override;
	void Render() override;
};
// 일반 이펙트 : 아직 안씀
class CSfxGenMonsterDmg01 : public CSfx {
public:
	void Process() override;
	void Render() override;
};
// 일반 이펙트 : 부활
class CSfxGenRestoration01 : public CSfx {
public:
	void Process() override;
	void Render() override;
};
// 일반 이펙트 : 치료
class CSfxGenCure : public CSfx {
public:
	void Process() override;
	void Render() override;
};
// 일반 이펙트 : 강화
class CSfxGenIncrease01 : public CSfx {
public:
	void Process() override;
	void Render() override;
};
// 일반 이펙트 : 레벨업
class CSfxGenLevelUp : public CSfx {
public:
	void Process() override;
	void Render() override;
};
// 일반 이펙트 : 로그인
class CSfxGenLogin : public CSfx {
public:
	void Process() override;
	void Render() override;
};
// 일반 이펙트 : 워프
class CSfxGenWarp : public CSfx {
public:
	void Process() override;
	void Render() override;
};
// 일반 이펙트 : 사망
class CSfxGenPcDie : public CSfx {
public:
	void Process() override;
	void Render() override;
};
// 일반 이펙트 : 몹 리스폰
class CSfxGenMonsterSpawn : public CSfx {
public:
	void Process() override;
	void Render() override;
};
// 일반 이펙트 : 이동위치
class CSfxGenMoveMark: public CSfx
{
public:
	void Process() override;
	void Render() override;

public:
#ifndef __VM_0820
#ifndef __MEM_TRACE
	static MemPooler<CSfxGenMoveMark>*	m_pPool;
	void*	operator new( size_t nSize )	{	return CSfxGenMoveMark::m_pPool->Alloc();	}
	void*	operator new( size_t nSize, LPCSTR lpszFileName, int nLine )	{	return CSfxGenMoveMark::m_pPool->Alloc();	}
	void	operator delete( void* lpMem )	{	CSfxGenMoveMark::m_pPool->Free( (CSfxGenMoveMark*)lpMem );	}
	void	operator delete( void* lpMem, LPCSTR lpszFileName, int nLine )	{	CSfxGenMoveMark::m_pPool->Free( (CSfxGenMoveMark*)lpMem );	}
#endif	// __MEM_TRACE
#endif	// __VM_0820
};
// 일반 이펙트 : 물의 파장
class CSfxGenWaterCircle: public CSfx
{
public:
	void Process() override;
	void Render() override;

public:
#ifndef __VM_0820
#ifndef __MEM_TRACE
	static MemPooler<CSfxGenWaterCircle>*	m_pPool;
	void*	operator new( size_t nSize )	{	return CSfxGenWaterCircle::m_pPool->Alloc();	}
	void*	operator new( size_t nSize, LPCSTR lpszFileName, int nLine )	{	return CSfxGenWaterCircle::m_pPool->Alloc();	}
	void	operator delete( void* lpMem )	{	CSfxGenWaterCircle::m_pPool->Free( (CSfxGenWaterCircle*)lpMem );	}
	void	operator delete( void* lpMem, LPCSTR lpszFileName, int nLine )	{	CSfxGenWaterCircle::m_pPool->Free( (CSfxGenWaterCircle*)lpMem );	}
#endif	// __MEM_TRACE
#endif	// __VM_0820
};

// 일반 이펙트 : 물의 파장
class CSfxGenRainCircle: public CSfx
{
public:
	void Process() override;
	void Render() override;

public:
#ifndef __VM_0820
#ifndef __MEM_TRACE
	static MemPooler<CSfxGenRainCircle>*	m_pPool;
	void*	operator new( size_t nSize )	{	return CSfxGenRainCircle::m_pPool->Alloc();	}
	void*	operator new( size_t nSize, LPCSTR lpszFileName, int nLine )	{	return CSfxGenRainCircle::m_pPool->Alloc();	}
	void	operator delete( void* lpMem )	{	CSfxGenRainCircle::m_pPool->Free( (CSfxGenRainCircle*)lpMem );	}
	void	operator delete( void* lpMem, LPCSTR lpszFileName, int nLine )	{	CSfxGenRainCircle::m_pPool->Free( (CSfxGenRainCircle*)lpMem );	}
#endif	// __MEM_TRACE
#endif	// __VM_0820
};

// 일반 이펙트 : 물 튀기기
class CSfxGenWaterCrown : public CSfx {
public:
	void Process() override;
	void Render() override;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



class CSfxMagicMiAtk1 : public CSfxShoot
{
public:
	FLOAT m_fCenter;		// 타겟의 좌표.y 보정치.
	FLOAT m_fRadiusXZ;		// 타겟의 XZ평면의 반지름.
	
	CSfxMagicMiAtk1();
	void Process() override;
	void Render() override;
};


// 완드는 모으기 공격 단계별로 4가지
// 일반 이펙트 : 완드1
class CSfxItemWandAtk1 : public CSfxShoot
{
public:
	FLOAT m_fCenter;		// 타겟의 좌표.y 보정치.
	FLOAT m_fRadiusXZ;		// 타겟의 XZ평면의 반지름.

	CSfxItemWandAtk1();
	void Process() override;
	void Render() override;
};

class CSfxItemRangeAtk1 : public CSfxShoot
{
public:
	CTailEffectBelt*	m_pTail = nullptr;		// 꼬랑지 이펙트	
	FLOAT			m_fOriLen;
	D3DXVECTOR3		m_v3SrcPos;
	FLOAT m_fCenter;		// 타겟의 좌표.y 보정치.
	FLOAT m_fRadiusXZ;		// 타겟의 XZ평면의 반지름.

	virtual ~CSfxItemRangeAtk1();
	void Process() override;
	void Render() override;
};

// 아래로 떨어지는 화살 이펙트
class CSfxItemRangeAtk1_Allow : public CSfxShoot
{
public:
	FLOAT	m_fSpeed;
	CSfxItemRangeAtk1_Allow();
	void Process() override;
	void Render() override;
};

// CSfxItemRangeAtk1_Allow생성 관리 - 실제 사용되어지는 부분
class CSfxItemRangeAtk1_AllowRain : public CSfxShoot
{
public:
	int		m_nCount;
	CSfxItemRangeAtk1_AllowRain();
	void Process() override;
	void Render() override;
};

// CSfxItemRangeAtk1_Allow생성 관리 - 실제 사용되어지는 부분
class CSfxItemRangeAtk1_Stone : public CSfxShoot
{
public:
	FLOAT	m_fSpeed;
	CSfxItemRangeAtk1_Stone();
	void Process() override;
	void Render() override;
};


class CSfxItemRangeAtk1_StoneRain : public CSfxShoot
{
public:
	int		m_nCount;
	CSfxItemRangeAtk1_StoneRain();
	void Process() override;
	void Render() override;
};

class CSfxItemRangeAtk_JunkBow : public CSfxShoot
{
public:
	CTailEffectBelt*	m_pTail[3];		// 꼬랑지 이펙트	
	FLOAT			m_fOriLen;
	D3DXVECTOR3		m_v3SrcPos;

	FLOAT m_fCenter;		// 타겟의 좌표.y 보정치.
	FLOAT m_fRadiusXZ;		// 타겟의 XZ평면의 반지름.
	CSfxItemRangeAtk_JunkBow();
	virtual ~CSfxItemRangeAtk_JunkBow();
	void Process() override;
	void Render() override;
};

D3DXVECTOR3	SplineSlerp(D3DXVECTOR3 * v1, D3DXVECTOR3 * v2, D3DXVECTOR3 * v3, D3DXVECTOR3 * v4, float fSlerp);

class CSfxItemYoyoAtk : public CSfxShoot
{
public:
	int				m_nMaxSpline;
	CTailEffectModel*	m_pTail;		// 꼬랑지 이펙트	
	D3DXVECTOR3		m_aSpline[30];
	int				m_nStep;
	int				m_nType;
	int				m_nCount;
	
	FLOAT			m_fMaxLength;
	D3DXVECTOR3		m_v3SrcPos;
	D3DXVECTOR3		m_vDelta;
	int				m_nDelayCount;
	
	

	
	FLOAT m_fCenter;		// 타겟의 좌표.y 보정치.
	FLOAT m_fRadiusXZ;		// 타겟의 XZ평면의 반지름.

	void  MakePath(int nType);
	
	CSfxItemYoyoAtk();
	virtual ~CSfxItemYoyoAtk();
	void Process() override;
	void Render() override;
};


// 일직선으로 발사하는 장거리 탄환류.
class CSfxAtkStraight : public CSfxShoot
{
	D3DXVECTOR3 m_vDelta;			// 이동 증가량.
	DWORD		m_dwExplosion;		// 맞고난후 생성될 폭발 아이디.
public:
	
	CSfxAtkStraight();
	void ShootSfx( float fAngXZ, float fAngH, float fSpeed, DWORD dwExplosion );		// 발사체의 이동증가량을 선계산함. 
	
	void Process() override;
	void Render() override;
};

class CSfxItemWandAtkAir : public CSfxShoot
{
	D3DXVECTOR3		m_vDelta;		// 관성.
public:
	
	BOOL m_bHit; // 명중했는지의 여부
	CSfxItemWandAtkAir();
	int SetSfx( int nIndex, D3DXVECTOR3 vPosSrc, OBJID idSrc, D3DXVECTOR3 vPosDest, OBJID idDest, int nSec = 0 ) override; // 사용할 SFX 지정
	void ShootSfx( float fAngXZ, float fAngY, float fSpeed ) override;

	void Process() override;
	void Render() override;
};


// 일반 이펙트 : 완드2
class CSfxItemWandAtk2 : public CSfxShoot
{
public:
	
	BOOL m_bHit; // 명중했는지의 여부
	CSfxItemWandAtk2();

	void Process() override;
	void Render() override;
};
// 일반 이펙트 : 완드3
class CSfxItemWandAtk3 : public CSfxShoot
{
public:
	
	BOOL m_bHit; // 명중했는지의 여부
	CSfxItemWandAtk3();
	void Process() override;
	void Render() override;
};
// 일반 이펙트 : 완드4
class CSfxItemWandAtk4 : public CSfxShoot
{
public:
	
	BOOL m_bHit; // 명중했는지의 여부
	CSfxItemWandAtk4();
	void Process() override;
	void Render() override;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 방랑자 스킬 : 오버커터
class CSfxSkillVagOverCutter : public CSfx {
public:
	void Process() override;
	void Render() override;
};
// 방랑자 스킬 : 클린히트
class CSfxSkillVagCleanHit : public CSfx {
public:
	void Process() override;
	void Render() override;
};
// 방랑자 스킬 : 브랜디쉬
class CSfxSkillVagBrandish : public CSfx {
public:
	void Process() override;
	void Render() override;
};

// 머셔너리 스킬 : 킨휠
class CSfxSkillMerKeenWheel : public CSfx {
public:
	void Process() override;
	void Render() override;
};
// 머셔너리 스킬 : 스플매쉬
class CSfxSkillMerSplmash : public CSfx
{
public:
	CSfxSkillMerSplmash();

	void Process() override;
	void Render() override;
};
// 머셔너리 스킬 : 블라인드 사이드
class CSfxSkillMerBlindSide : public CSfx {
public:
	void Process() override;
	void Render() override;
};

// 매지션 주문외기
class CSfxSkillMagFireCasting : public CSfx {
public:
	void Process() override;
	void Render() override;
};
class CSfxSkillMagWindCasting : public CSfx {
public:
	void Process() override;
	void Render() override;
};

// 어시 너클추가..

class CSfxSkillAssBurstcrack : public CSfx {
public:
	void Process() override;
	void Render() override;
};

class CSfxSkillAssTampinghole : public CSfx {
public:
	void Process() override;
	void Render() override;
};

class CSfxNpcDirSteam : public CSfx
{
public:
	BOOL m_fInit = TRUE;

	void Process() override;
	void Render() override;
};

	

// 매지션 스킬 : 스트롱윈드
class CSfxSkillMagStrongWind : public CSfxShoot
{
	float m_fAngle;	
public:
	
	BOOL m_bHit; // 명중했는지의 여부
	CSfxSkillMagStrongWind();

	void Process() override;
	void Render() override;
};
// 매지션 스킬 : 소드윈드
class CSfxSkillMagSwordWind : public CSfxShoot
{
public:
	
	BOOL m_bHit; // 명중했는지의 여부
	CSfxSkillMagSwordWind();
	void Process() override;
	void Render() override;
};
// 매지션 스킬 : 파이어부메랑
class CSfxSkillMagFireBoomerang : public CSfxShoot
{
public:
	
	BOOL m_bHit; // 명중했는지의 여부
	CSfxSkillMagFireBoomerang();
	void Process() override;
	void Render() override;
};
// 매지션 스킬 : 파이어봄버
class CSfxSkillMagFireBomb : public CSfx {
public:
	void Process() override;
	void Render() override;
};
// 매지션 스킬 : 핫에어
class CSfxSkillMagHotAir : public CSfxShoot
{
	int m_nDmgCnt;		// 지속데미지의 데미지 카운트. 데미지 먹일때마다 하나씩 올라간다.
public:
	
	CSfxSkillMagHotAir();
	void Process() override;
	void Render() override;
};



// 매지션 스킬 : 아이스미사일
class CSfxSkillMagIceMissile : public CSfxShoot
{
public:
	BOOL m_bHit; // 명중했는지의 여부
	CSfxSkillMagIceMissile();
	void Process() override;
	void Render() override;
};

// 매지션 스킬 : 라이트닝 볼
class CSfxSkillMagLightningBall : public CSfxShoot
{
public:
	
	BOOL m_bHit; // 명중했는지의 여부
	CSfxSkillMagLightningBall();
	void Process() override;
	void Render() override;
};


// 매지션 스킬 : 스파이크 스톤
class CSfxSkillMagSpikeStone : public CSfxShoot
{
public:
	
	BOOL m_bHit; // 명중했는지의 여부
	CSfxSkillMagSpikeStone();
	void Process() override;
	void Render() override;
};


#define MAX_SHOOTWAVE_TAIL		8
// 바닥에 붙어서 날아가는 이펙트.
class CSfxShootWave : public CSfxShoot
{
public:
	BOOL	m_bHit; // 명중했는지의 여부
	BOOL	m_bDir;
	DWORD	m_dwSndHit;		// 맞았을때 소리.
	FLOAT	m_fHitScale;	// 폭발씬 스케일링.
	D3DXVECTOR3 m_vTail[MAX_SHOOTWAVE_TAIL];	// 지나갔던 좌표를 차례로 쌓음.
	int		m_nTailFrame[ MAX_SHOOTWAVE_TAIL ];	// 프레임 번호.

	CSfxShootWave();
	
	void	SetHitSfx( LPCTSTR szFileName, OBJID idSrc, OBJID idDst, FLOAT fScale = 1.0f );	// 맞았을때 이펙트 등록
	void	SetHitSfx( DWORD dwIndex, OBJID idSrc, OBJID idDst, FLOAT fScale = 1.0f );	// 맞았을때 이펙트 등록
	void	SetDir( BOOL bDir );		// 방향성있는 sfx 인가.
	void	SetSndHit( DWORD dwSndHit ) { m_dwSndHit = dwSndHit; }
	
	void Process() override;
	void Render() override;
};


// 고정좌표 기준 범용 sfx
class CSfxFixed : public CSfx
{
	float m_fAngle;
public:
	void Process() override;
	void Render() override;
};

class CSfxPartsLink : public CSfx	// 계속유지되는 이펙트		
{
public:
	BOOL	m_bEndFrame = FALSE;
	int		m_nPartsLink = 0;		// 링크될 위치. 0:오른손 1:왼손
	
	void Process() override;
	void Render() override;
	void SetPartLink(int nPart) override { m_nPartsLink = nPart; }
};

// 파츠에 링크되는 sfx(블레이드의 손에 사용)
class CSfxPartsLinkBlade : public CSfxPartsLink		// 블레이드 파트 링크 이펙 : 밀리 모션이 끝나면 소멸
{
public:
	void Process() override;
};

class CSfxPartsLinkJst : public CSfxPartsLink		// 제스터 파츠 링크 이펙 : 이펙트가 EndFrame이면 소멸
{
public:
	void Process() override;
};



// 파츠에 링크되는 sfx(아크로뱃의 손에 사용)
class CSfxAllowPartsLink : public CSfx
{
public:
	int		m_nPartsLink = 0;		// 링크될 위치. 0:오른손 1:왼손
	
	void Process() override;
	void Render() override;
	void SetPartLink(int nPart) override { m_nPartsLink = nPart; }
};

class CSfxPartsLinkShoulder : public CSfxPartsLink		// 블레이드 파트 링크 이펙 : 밀리 모션이 끝나면 소멸
{
public:
	int				m_nOldAbilityOption;
	D3DXVECTOR3		m_v3Offset;

	CSfxPartsLinkShoulder();
	~CSfxPartsLinkShoulder();

	void Process() override;
	void Render() override;
};

// 아이템 시전 이펙트
class CSfxReady : public CSfx
{
public:
	void Process() override;
};

class CSfxCursor : public CSfx
{
public:
	void Process() override;
};

class CSfxLinkMover : public CSfx			
{
public:
	//gmpbigsun : 무버에 붙어 다니는 일반적인 상황연출용
	void Process() override;
	void Render() override;
};



class CSfxHitParts : public CSfxPartsLink {
public:
	enum class HIT_PARTS {
		HIT_BODY,
		HIT_OVERHEAD,
	};
	//gmpbigsun : For the general situation of being attached to the mover
	CSfxHitParts(const HIT_PARTS eParts = HIT_PARTS::HIT_BODY);
	void Process() override;
	void Render() override;

protected:
	HIT_PARTS m_eHitParts;
	BOOL m_bBuff;
};

class CSfxAttackParts : public CSfxPartsLink {
public:
	enum class HIT_PARTS {
		HIT_BODY,
		HIT_OVERHEAD,
		HIT_RHAND,
	};
	//gmpbigsun : For the general situation of being attached to the mover
	CSfxAttackParts(const HIT_PARTS eParts = HIT_PARTS::HIT_BODY);
	void Process() override;
	void Render() override;

	HIT_PARTS m_eHitParts;
};

// It's the same as CSfx, and the rotation direction of idSrc is the same.
class CSfxRotate_New : public CSfx {
public:
	void Process() override;
	void Render() override;
};
