#if !defined(AFX_CTRL_H__A23AD198_7E45_465B_B646_7956A4AE179F__INCLUDED_)
#define AFX_CTRL_H__A23AD198_7E45_465B_B646_7956A4AE179F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Obj.h"
#include "ProjectCmn.h"

#ifdef __WORLDSERVER
	class		CUser;
	class		CCtrl;
#endif	// __WORLDSERVER

struct		CtrlProp;
class		CWorld;
class       CSfx;
class		CShip;

/// 배경 오브젝트를 제외한 모든 오브젝트의 base class
class CCtrl : public CObj
{
public:
	CCtrl();
	virtual ~CCtrl();

#ifdef __BS_SAFE_WORLD_DELETE
public:
#else 
protected:
#endif //__BS_SAFE_WORLD_DELETE
	OBJID			m_objid;			/// 객체 아이디
	CShip*			m_pIAObjLink;		/// 비공정 관련 - this가 어느IA오브젝트에 연결이 되었는가. 나중에 CShip*은 CIAObj* 형으로 대체되어야 한다.
	D3DXMATRIX		m_mInvTM;			/// 비공정 관련 - m_matWorld의 역행렬. pIAObjLink와 곱해서 로컬행렬을 얻음.

public:

#ifdef __WORLDSERVER
	std::optional<CtrlSpawnInfo> m_spawnerInfo = std::nullopt;
	std::map< DWORD, CUser* > m_2pc;			/// 주변 플레이어 맵 
	int				m_nOldCenter[MAX_LINKLEVEL];	/// 링크맵 계산용 
#endif	// __WORLDSERVER

public:
	virtual BOOL	GetSkillProp( ItemProp **ppSkillProp, AddSkillProp **ppAddSkillProp, int nSkill, DWORD dwLevel, LPCTSTR szErr );
	virtual int		SendDamage( DWORD dwAtkFlag, OBJID idAttacker, int nParam = 0, BOOL bTarget = TRUE ) { return 0; }
	virtual int		SendDamageForce( DWORD dwAtkFlags, OBJID idSender, int nParam = 0, BOOL bTarget = TRUE ) { return 0; }	// 강공격
	virtual void	AddItToGlobalId();
	virtual	void	RemoveItFromGlobalId();
	virtual void	Process();
	virtual void	Render();
	virtual BOOL	Read( CFileIO* pFile );
	virtual	void	Serialize( CAr & ar );

	void			SetId( OBJID objid )	{ m_objid = objid;  }
	[[nodiscard]] OBJID GetId() const noexcept { return m_objid; }
	int				TargetSelecter( CCtrl *pTarget );
	int				ShootSkill( CCtrl *pTarget, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp );			// 발사형태의 스킬의 발사체 생성
	void			CreateSkillSfx( CCtrl *pTarget, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp );		// 스킬시전시 클라에서 생성되어야 할 sfx
	int				ApplyDamage( CCtrl *pSrc, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, int nParam, BOOL bTarget ) ;
	BOOL			IsPossibleApplySkill( CCtrl *pSrc, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp );
	void			ApplySkill( CCtrl *pSrc, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, bool bIgnoreProb = false, int nParam = 0, BOOL bOnlyDmg = FALSE, BOOL bTarget = TRUE );		// 타겟하나당 실제 적용되어야 하는 효과
	int				DoApplySkill( CCtrl *pTarget, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, bool bIgnoreProp = false, int nParam = 0, BOOL bOnlyDmg = FALSE, BOOL bControl = FALSE );		// 스킬을 타겟중심으로 적용
	void			ApplySkillRegion( const D3DXVECTOR3 &vPos, int nApplyType, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, bool bIgnoreProp = false, BOOL bOnlyDmg = FALSE, CCtrl* pCenter = NULL, BOOL bControl = FALSE );	// vPos를 중심으로 스킬을 적용
	void			ApplySkillAround( CCtrl *pSrc, int nApplyType, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, bool bIgnoreProp = false, BOOL bOnlyDmg = FALSE, FLOAT fRangeCustom = 0.0f, BOOL bControl = FALSE );		// 스킬을 this를 중심으로 주변에 적용.
	void			ApplySkillLine( int nApplyType, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, float fWidth, float fHeight, float fDepth, bool bIgnoreProp = false, BOOL bControl = FALSE );
	int				ApplySkillAroundTroupe( u_long idParty, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, bool bIgnoreProp = false );		// idParty 파티 대상으로 스킬효과를 적용.
	BOOL			ApplySkillHardCoding( CCtrl *pSrc, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp );
	void			DoActiveSkill( DWORD dwSkill, int nLevel, CCtrl *pTarget, bool bIgnoreProb = false, BOOL bControl = FALSE );		// ActiveSkill발동
	CShip*			GetIAObjLink()			{ return m_pIAObjLink; }
	void			SetIAObjLink( CShip *pIA );
	D3DXMATRIX*		GetInvTM()				{ return &m_mInvTM; }

#ifdef __WORLDSERVER
	void			PCSetAt( OBJID objid, CCtrl* pCtrl );	
	BOOL			PCRemoveKey( OBJID objid );	
	void RemoveItFromPcView();
	[[nodiscard]] bool IsNearPC(const OBJID objid) const noexcept { return m_2pc.contains(objid); }
	[[nodiscard]] bool IsNearPC(const CUser * const pUser) const;

	void SetSpawner(CtrlSpawnInfo ctrlSpawnInfo) { m_spawnerInfo = ctrlSpawnInfo; }
	BOOL ProcessDeleteRespawn();
	void RefreshSpawn(BOOL bActiveAttack);
#else
	void			CreateYoyoSkill( CSfx* pSfx, CCtrl *pTarget, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp );
#endif	//__WORLDSERVER

private:
	int				DoApplySkillEx( CCtrl *pTarget, ItemProp *pSkillProp, AddSkillProp *pAddSkillProp, bool bIgnoreProp, int nParam, BOOL bOnlyDmg, BOOL bControl );
};

#ifdef __WORLDSERVER

inline void CCtrl::PCSetAt( OBJID objid, CCtrl* pCtrl )	
{	
	m_2pc[ objid ] = (CUser *)pCtrl;
}

inline BOOL CCtrl::PCRemoveKey( OBJID objid )	
{	
	return m_2pc.erase( objid ) > 0;
}

#endif	// __WORLDSERVER


#endif // !defined(AFX_CTRL_H__A23AD198_7E45_465B_B646_7956A4AE179F__INCLUDED_)