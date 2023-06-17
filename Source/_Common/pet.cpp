#include "stdafx.h"
#include "pet.h"
#include "defineobj.h"

#ifdef __WORLDSERVER
#include "user.h"
#include "dpsrvr.h"
#include "definetext.h"
#endif	// __WORLDSERVER
#ifdef __CLIENT
#include "definetext.h"
#endif

#include "xutil.h"

static constexpr int nDefaultLife = 1;

CPetProperty::CPetProperty()
{
	memset( (void*)m_aPetAvailParam, 0, sizeof(PETAVAILPARAM) * PK_MAX );
}

const CPetProperty::PETPENALTY * CPetProperty::GetPenalty(BYTE nLevel) const {
	if (nLevel < PL_MAX) {
		return &m_levelInfos[nLevel].penalty;
	} else {
		return nullptr;
	}
}

const CPetProperty::PETAVAILPARAM * CPetProperty::GetAvailParam(BYTE nKind) const {
	if (nKind >= PK_TIGER && nKind < PK_MAX) {
		return &m_aPetAvailParam[nKind];
	} else {
		return nullptr;
	}
}

BYTE	CPetProperty::GetLevelupAvailLevel( BYTE wLevel ) {
	if (wLevel >= PL_MAX) {
		return 0;
	}

	const LevelInfo & info = m_levelInfos[wLevel];

	DWORD dwTotal	= 0;
	DWORD dwRandom	= xRandom( 1, 10001 );	// 1 ~ 10000
	for (int i = 0; i <= info.availMax; i++) {
		dwTotal += info.availProb[i];
		if (dwTotal >= dwRandom) {
			return i + 1;
		}
	}
	return 0;
}

WORD CPetProperty::GetFeedEnergy(DWORD dwCost, int nIndex) const {
	if (nIndex < 0 || nIndex > 1)
		return 0;

	for (const FEEDENERGY & feedEnergy : m_aFeedEnergy[nIndex]) {
		if (dwCost >= feedEnergy.dwCostMin && dwCost <= feedEnergy.dwCostMax) {
			return (WORD)xRandom(feedEnergy.wEnergyMin, feedEnergy.wEnergyMax + 1);	// wEnergyMin ~ wEnergyMax
		}
	}

	return 0;
}

BYTE	CPetProperty::Hatch( void )
{
	DWORD dwTotal	= 0;
	DWORD dwRandom	= xRandom( 1, 10001 );	// 1 - 10000
	for( int i = 0; i < PK_MAX; i++ )
	{
		dwTotal	+= m_aPetAvailParam[i].m_dwProbability;
		if( dwTotal >= dwRandom )
			return i;
	}
	return 0;	// error
}

DWORD	CPetProperty::GetIncrementExp( BYTE nLevel )
{
	if( nLevel >= PL_D && nLevel <= PL_A )
		return m_levelInfos[nLevel-1].incrementExp;
	return 0;
}

WORD CPetProperty::GetMaxEnergy( BYTE nLevel )
{
	if( nLevel >= PL_D && nLevel <= PL_S )
		return m_levelInfos[nLevel-1].maxEnergy;
	return 1;	// 0 나누기 방지
}

WORD CPetProperty::GetAddLife( void )
{
	DWORD dwTotal	= 0;
	DWORD dwRandom	= xRandom( 1, 101 );	// 1 ~ 100
	for( DWORD i = 0; i < m_awAddLifeProbability.size(); i++ )
	{
		dwTotal	+= m_awAddLifeProbability[i];
		if( dwTotal >= dwRandom )
			return	( (WORD)( i + 1 ) );
	}
	return 1;
}

CPetProperty* CPetProperty::GetInstance( void )
{
	static	CPetProperty	sPetProperty;
	return &sPetProperty;
}

BOOL CPetProperty::LoadScript( LPCTSTR szFile )
{
	CScript s;
	if( s.Load( szFile ) == FALSE )
		return FALSE;

	s.GetToken();	// subject or FINISHED
	while( s.tok != FINISHED )
	{
		if( s.Token == _T( "AvailDestParam" ) )
		{
			// 	DST_STR
			int	nKind	= 0;
			s.GetToken();	// {
			DWORD dwDstParam	= s.GetNumber();
			while( *s.token != '}' )
			{
				ASSERT( nKind < PK_MAX );
				m_aPetAvailParam[nKind].dwDstParam	= dwDstParam;
				for( int i = 0; i < MAX_PET_AVAIL_LEVEL; i++ )
					m_aPetAvailParam[nKind].m_anParam[i]	= s.GetNumber();
				m_aPetAvailParam[nKind].m_dwItemId	= s.GetNumber();
				m_aPetAvailParam[nKind].m_adwIndex[0]	= s.GetNumber();
				m_aPetAvailParam[nKind].m_adwIndex[1]	= s.GetNumber();
				m_aPetAvailParam[nKind].m_adwIndex[2]	= s.GetNumber();
				m_aPetAvailParam[nKind++].m_dwProbability	= s.GetNumber();
				dwDstParam	= s.GetNumber();
			}
		}
		else if( s.Token == _T( "LevelupAvail" ) )
		{
			LoadLevelupAvail(s);
		}
		else if( s.Token == _T( "FeedEnergy" ) )
		{
			int nIndex	= s.GetNumber();
			ASSERT( nIndex >= 0 && nIndex <= 1 );

			auto * pArr	= &m_aFeedEnergy[nIndex];
			// 	1	575	7	14
			FEEDENERGY	fe;
			s.GetToken();	// {
			fe.dwCostMin	= s.GetNumber();
			while( *s.token != '}' )
			{
				fe.dwCostMax	= s.GetNumber();
				fe.wEnergyMin	= s.GetNumber();
				fe.wEnergyMax	= s.GetNumber();
				pArr->push_back( fe );	// add
				fe.dwCostMin	= s.GetNumber();
			}
		}
		else if( s.Token == _T( "IncrementExp" ) )
		{
			//	100	// 0.1%	// 100000분의 1 기준
			int	nLevel	= 0;
			s.GetToken();	// {
			DWORD	dwIncrementExp	= s.GetNumber();
			while( *s.token != '}' )
			{
				ASSERT( nLevel < PL_S );
				m_levelInfos[nLevel++].incrementExp	= dwIncrementExp;
				dwIncrementExp	= s.GetNumber();
			}
		}
		else if( s.Token == _T( "MaxEnergy" ) )
		{
			//	250	// D	// 최대 기력
			int	nLevel	= 0;
			s.GetToken();	// {
			DWORD	dwMaxEnergy	= s.GetNumber();
			while( *s.token != '}' )
			{
				ASSERT( nLevel <= PL_S );
				m_levelInfos[nLevel++].maxEnergy = (WORD)( dwMaxEnergy );
				dwMaxEnergy		= s.GetNumber();
			}
		}
		else if( s.Token == _T( "AddLifeProbability" ) )
		{
			// 수명 회복액 확률	// 추가 될 경우 합이 100이 되도록 확장
			// 	50	// +1
			s.GetToken();	// {
			WORD wProbability	= s.GetNumber();
			while( *s.token != '}' )
			{
				m_awAddLifeProbability.push_back( wProbability );
				wProbability	= s.GetNumber();
			}
		}
		else if( s.Token == _T( "Penalty" ) )
		{
			// Pet 소환 후 해제 시 D~A레벨에선 경험치를 잃게 되며,
			// 최소 0%에서는 더 이상 감소하지 않는다. 
			// S레벨에선 HP가 감소하며, 최소 5P 이하로는 더 이상 감소하지 않는다. 
			s.GetToken();	// {
			FLOAT fExp	= s.GetFloat();
			int nLevel	= PL_D;
			while( *s.token != '}' )
			{
				m_levelInfos[nLevel].penalty.fExp		= fExp;
				m_levelInfos[nLevel].penalty.wEnergy	= (WORD)s.GetNumber();
				nLevel++;
				fExp	= s.GetFloat();
			}
		}
		s.GetToken();
	}

	return TRUE;
}

void CPetProperty::LoadLevelupAvail(CScript & s) {
	// 	10000	0	0	0	0	0	0	0	0
	int	nLevel	= (int)PL_D;
	s.GetToken();	// {{
	DWORD	dwProbability	= s.GetNumber();
	while( *s.token != '}' )
	{
		m_levelInfos[nLevel].availProb[0]	= dwProbability;
		for( int i = 1; i < MAX_PET_AVAIL_LEVEL; i++ )
		{
			m_levelInfos[nLevel].availProb[i]	= s.GetNumber();
			if( m_levelInfos[nLevel].availProb[i] > 0 )
				m_levelInfos[nLevel].availMax = i;
		}
		nLevel++;
		dwProbability	= s.GetNumber();
	}
}

#ifdef __CLIENT
DWORD CPetProperty::GetTIdOfLevel(PETLEVEL petLevel) {
	switch (petLevel) {
		case PL_EGG: return TID_GAME_PETGRADE_E;
		case PL_D:   return TID_GAME_PETGRADE_D;
		case PL_C:   return TID_GAME_PETGRADE_C;
		case PL_B:   return TID_GAME_PETGRADE_B;
		case PL_A:   return TID_GAME_PETGRADE_A;
		case PL_S:   return TID_GAME_PETGRADE_S;
		default:     return 0;
	}
}

DWORD CPetProperty::GetTIdOfDst(const SINGLE_DST & dst, bool shortenHpMax) {
	if (shortenHpMax && dst.nDst == DST_HP_MAX) {
		return CWndMgr::GetDSTStringId(DST_HP);
	}
	return CWndMgr::GetDSTStringId(dst.nDst);
}

#endif

////////////////////////////////////////////////////////////////////////////////

CAr & operator<<(CAr & ar, const CPet & pet) {
	ar << pet.m_nKind;
	ar << pet.m_nLevel;
	ar << pet.m_dwExp;
	ar << pet.m_wEnergy;
	ar << pet.m_wLife;
	ar << pet.m_anAvailLevel;
	ar.WriteString(pet.m_szName);
	return ar;
}

CAr & operator>>(CAr & ar, CPet & pet) {
	ar >> pet.m_nKind;
	ar >> pet.m_nLevel;
	ar >> pet.m_dwExp;
	ar >> pet.m_wEnergy;
	ar >> pet.m_wLife;
	ar >> pet.m_anAvailLevel;
	ar.ReadString(pet.m_szName, MAX_PET_NAME);
	return ar;
}

void CPet::SetEnergy( WORD wEnergy )
{
#ifdef __DBSERVER
	m_wEnergy	= wEnergy;		// trans서버에서는 pet.inc를 읽지 않는다.
#else	// __DBSERVER
	WORD wMaxEnergy		= GetMaxEnergy();
	m_wEnergy	= std::max(wEnergy, wMaxEnergy);
#endif	// __DBSERVER
}

void CPet::SetExp( DWORD dwExp )
{
	m_dwExp		= dwExp > (DWORD)( GetMaxExp() ) ? GetMaxExp(): dwExp;
}

WORD CPet::GetLife( void )
{
	return ( m_nLevel == PL_EGG )? 0: m_wLife;
}

void CPet::SetAvailLevel( BYTE nLevel,  BYTE nAvailLevel )
{
	if( nLevel >= PL_D && nLevel <= PL_S )
		m_anAvailLevel[nLevel-1]	= nAvailLevel;
}

BYTE CPet::GetAvailLevel( BYTE nLevel )
{
	if( nLevel >= PL_D && nLevel <= PL_S )
		return m_anAvailLevel[nLevel-1];
	return 0;
}

SINGLE_DST CPet::GetAvailDestParam() const {
	const CPetProperty::PETAVAILPARAM * pAvailParam = CPetProperty::GetInstance()->GetAvailParam(m_nKind);
	if (!pAvailParam) return { 0, 0 };

	int dst = static_cast<int>(pAvailParam->dwDstParam);
	int nParam = 0;
	for( int i = PL_D; i <= m_nLevel; i++ )
		nParam += pAvailParam->m_anParam[m_anAvailLevel[i-1] - 1];

	return SINGLE_DST{ dst, nParam };
}

DWORD CPet::GetIndex() const {
	if( m_nLevel == PL_EGG )
		return MI_PET_EGG;

	const CPetProperty::PETAVAILPARAM * pPetAvailParam = CPetProperty::GetInstance()->GetAvailParam(m_nKind);
	if (m_nLevel > PL_EGG && m_nLevel < PL_B) {
		return pPetAvailParam->m_adwIndex[0];
	} else if (m_nLevel < PL_S) {
		return pPetAvailParam->m_adwIndex[1];
	} else {
		return pPetAvailParam->m_adwIndex[2];
	}
}

void CPet::InitEgg( void )
{
	SetLife( nDefaultLife );
	SetEnergy( GetMaxEnergy() );
}

#ifdef __CLIENT
enum
{
	STATE_INIT	= 1,
	STATE_IDLE,
	STATE_RAGE
};

BEGIN_AISTATE_MAP( CAIEgg, CAIInterface )

	ON_STATE( STATE_INIT, &CAIEgg::StateInit )
	ON_STATE( STATE_IDLE, &CAIEgg::StateIdle )
	ON_STATE( STATE_RAGE, &CAIEgg::StateRage )

END_AISTATE_MAP()

#define		PETSTATE_IDLE		0
#define		PETSTATE_TRACE		1

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CAIEgg::CAIEgg( CObj* pObj, OBJID idOwner ) : CAIInterface( pObj )
{
	Init();
	m_idOwner	= idOwner;
}

CAIEgg::~CAIEgg()
{
	Destroy();
}

void CAIEgg::Init( void )
{
	m_idOwner = NULL_ID;
	m_nState = PETSTATE_IDLE;
}

void CAIEgg::Destroy( void )
{
	Init();
}

void CAIEgg::InitAI()
{
	PostAIMsg( AIMSG_SETSTATE, STATE_IDLE ); 
}

void CAIEgg::MoveToDst(	D3DXVECTOR3 vDst )
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();
	pMover->SetDestPos( vDst, true, FALSE );
	pMover->m_nCorr		= -1;
}

void CAIEgg::MoveToDst(	OBJID idTarget )
{
	CMover* pMover = GetMover();
	if( pMover->GetDestId() == idTarget )
		return;
	pMover->SetDestObj( idTarget );
}

BOOL CAIEgg::StateInit( const AIMSG & msg )
{
	return TRUE;
}

BOOL CAIEgg::MoveProcessIdle( const AIMSG & msg )
{
//	static	CTimer	tmTrace( 333.33F );
	
	CMover *pMover = GetMover();
	CMover *pOwner = prj.GetMover( m_idOwner ); 
	CWorld *pWorld = GetWorld();
	MoverProp *pProp = pMover->GetProp();

	// 데미지 상태거나 죽었을 경우 이동 처리 수행 않음 
	if( pMover->IsDie() || (pMover->m_pActMover->GetState() & OBJSTA_DMG_FLY_ALL) )
		return FALSE;

	// 주인님과의 거리가 멀어지면 주인님쪽으로 달려간다.
	if( m_nState == PETSTATE_IDLE )
	{
		FLOAT fAngXZ	= pOwner->GetAngle();
		FLOAT fAngH		= pOwner->GetAngleX();

		D3DXVECTOR3 vPos;
//		AngleToVector( &vPos, fAngXZ, -fAngH, 1.0f );
//		vPos	+= pOwner->GetPos();
		vPos	= pOwner->GetPos();
		vPos.y	= pOwner->GetWorld()->GetLandHeight( pOwner->GetPos() );

		D3DXVECTOR3 vPos1, vPos2;
		fAngXZ	= pOwner->GetAngle();
		fAngH  = pOwner->GetAngleX();
		fAngXZ -= 90.0f;
		if( fAngXZ < 0 )
			fAngXZ += 360.0f;
		AngleToVector( &vPos1, fAngXZ, -fAngH, 1.0f );
		vPos1 += vPos;
		fAngXZ	= pOwner->GetAngle();
		fAngH  = pOwner->GetAngleX();
		fAngXZ += 90.0f;
		if( fAngXZ > 360.0f )
			fAngXZ -= 360.0f;
		AngleToVector( &vPos2, fAngXZ, -fAngH, 1.0f );
		vPos2	+= vPos;
		
		D3DXVECTOR3 v1	= vPos1 - pMover->GetPos();
		D3DXVECTOR3 v2	= vPos2 - pMover->GetPos();
		double d1	= (double)D3DXVec3LengthSq( &v1 );
		double d2	= (double)D3DXVec3LengthSq( &v2 );
		if( d1 < d2 )
			vPos	= vPos1;
		else
			vPos	= vPos2;
		D3DXVECTOR3 vDist = vPos - pMover->GetPos();
		vDist.y	= 0;
		FLOAT fDistSq = D3DXVec3LengthSq( &vDist );
		if( fDistSq >1.0f )
		{
			MoveToDst( vPos );
//			m_nState = PETSTATE_TRACE;
		}
	}
	/*
	else if( m_nState == PETSTATE_TRACE )
	{
//		if( pOwner->IsRangeObj( pMover, 0 ) == TRUE )	// 주인님 곁으로 다가갔다.
//		if( pMover->IsEmptyDestPos() )
//		if( tmTrace.Over() )
//		{
//			tmTrace.Reset();
			m_nState = PETSTATE_IDLE;	// 대기상태로 전환
//			pMover->SendActMsg( OBJMSG_STOP );	// 모션 정지
//			pMover->ClearDest();	// 이동목표 클리어.
//		}
	}
	*/
	return TRUE;
}

BOOL CAIEgg::StateIdle( const AIMSG & msg )
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();

	BeginAIHandler( )

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_INIT ) 

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_INIT_TARGETCLEAR )
			
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_PROCESS ) 
		MoveProcessIdle( msg );

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DAMAGE ) 

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DIE ) 
		SendAIMsg( AIMSG_EXIT );

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_COLLISION )

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_ARRIVAL )

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_EXIT )	

	EndAIHandler( )

	return TRUE;
}

BOOL CAIEgg::MoveProcessRage( const AIMSG & msg )
{
	CMover* pMover = GetMover();
	CWorld* pWorld = GetWorld();
	MoverProp *pProp = pMover->GetProp();
	
	// 데미지 상태거나 죽었을 경우 이동 처리 수행 않음 
	if( pMover->IsDie() || ( pMover->m_pActMover->GetState() & OBJSTA_DMG_FLY_ALL ) )
		return FALSE;
	return TRUE;
}

BOOL CAIEgg::StateRage( const AIMSG & msg )
{
	CMover* pMover	= GetMover();
	CWorld* pWorld	= GetWorld();

	if( IsInvalidObj( pMover ) )
		return FALSE;

	BeginAIHandler( )

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_INIT ) 

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_INIT_TARGETCLEAR )		// 타겟을 클리어하고 대기모드로 돌아감.
		
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_PROCESS ) 
		MoveProcessRage( msg );
		
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DAMAGE ) 

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_COLLISION )
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DIE ) 
	
	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_DSTDIE ) 
		SendAIMsg( AIMSG_SETSTATE, STATE_IDLE );

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_BEGINMOVE )

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_ARRIVAL )

	///////////////////////////////////////////////////////////////////////////////////////////////////
	OnMessage( AIMSG_EXIT )	

	EndAIHandler( )
	
	return TRUE;
}
#endif	// __CLIENT


// 알변환 일반적인 아이템 변환에 이용할 수 있도록 확장을 고려하여 만들었으나 아직 구체적이지 않다.
// 비슷한 변환이 추가될 때, 나머지 작업을 하자
CTransformStuff::CTransformStuff()
:
m_nTransform( 0 )
{
}

CTransformStuff::CTransformStuff( int nTransform )
:
m_nTransform( nTransform )
{
}

CTransformStuff::~CTransformStuff()
{
}

void CTransformStuff::AddComponent( int nItem, short nNum )
{
	TransformStuffComponent component( nItem, nNum );
	m_vComponents.push_back( component );
}

CAr & operator<<(CAr & ar, const CTransformStuff & self) {
	ar << self.m_nTransform;

	ar << static_cast<std::uint32_t>(self.m_vComponents.size());
	for (const TransformStuffComponent & component : self.m_vComponents) {
		ar << component;
	}

	return ar;
}

CAr & operator>>(CAr & ar, CTransformStuff & self) {
	ar >> self.m_nTransform;

	std::uint32_t nSize; ar >> nSize;
#ifdef __WORLDSERVER
	if (nSize != CTransformItemProperty::Instance()->GetStuffSize(self.m_nTransform))
		return ar;
#endif	// __WORLDSERVER
	for (std::uint32_t i = 0; i < nSize; i++) {
		TransformStuffComponent component;
		ar >> component;
		self.AddComponent(component.nItem, component.nNum);
	}

	return ar;
}

#ifdef __WORLDSERVER
// 변환 객체
ITransformer::~ITransformer()
{
}

ITransformer* ITransformer::Transformer( int nTransformer )
{
	if( nTransformer == 0 )
		return CTransformerEgg::Instance();
	assert( 0 );
	return NULL;
}

void ITransformer::Transform( CUser* pUser, CTransformStuff& stuff )
{
	if( !IsValidStuff( pUser, stuff ) )
		return;
	RemoveItem( pUser, stuff );
	CreateItem( pUser, stuff );
}

void ITransformer::RemoveItem( CUser* pUser, CTransformStuff & stuff )
{
	for( DWORD i = 0; i < stuff.GetSize(); i++ )
	{
		TransformStuffComponent* pComponent	= stuff.GetComponent( i );
		CItemElem * pItem = pUser->GetItemId(pComponent->nItem);
		if( pItem )
		{
			g_DPSrvr.PutItemLog( pUser, "X", "transform-removestuff", pItem, pComponent->nNum );
			pUser->RemoveItem( pComponent->nItem, pComponent->nNum );
		}
	}
}

void ITransformer::CreateItem( CUser* pUser, CTransformStuff &stuff )
{
	CItemElem* pItem	= CTransformItemProperty::Instance()->GetItem( stuff.GetTransform() );
	if( pItem && pUser->CreateItem( pItem ) )
	{
		pUser->AddDefinedText( TID_GAME_TRANSFORM_S00 );
		g_DPSrvr.PutCreateItemLog( pUser, pItem, "X", "transform" );
	}
	else
		pUser->AddDefinedText( TID_GAME_TRANSFORM_S01 );
}

BOOL ITransformer::IsValidStuff( CUser* pUser, CTransformStuff & stuff )
{
	// 사용자가 보내온 재료와 실제 필요한 재료 개수가 다르면 부적합 재료
	if( stuff.GetSize() != CTransformItemProperty::Instance()->GetStuffSize( stuff.GetTransform() ) )
		return FALSE;

	// 사용자가 보내온 재료를 사용자가 가지고 있고
	// 사용가능한 상태인지를 검사한다
	for( DWORD i = 0; i < stuff.GetSize(); i++ )
	{
		TransformStuffComponent* pComponent		= stuff.GetComponent( i );
		CItemElem * pItem = pUser->GetItemId(pComponent->nItem);
		if( !pItem || pUser->IsUsing( pItem ) )
			return FALSE;
	}
	return TRUE;
}

CTransformerEgg::CTransformerEgg()
{
}

CTransformerEgg::~CTransformerEgg()
{
}

CTransformerEgg*	CTransformerEgg::Instance( void )
{
	static CTransformerEgg sTransformerEgg;
	return &sTransformerEgg;
}

BOOL CTransformerEgg::IsValidStuff( CUser* pUser, CTransformStuff & stuff )
{
	// 기본 재료 조건을 만족하지 않으면 부적합 재료
	if( !ITransformer::IsValidStuff( pUser, stuff ) )
		return FALSE;

	std::set<int> setItems;
	for( DWORD i = 0; i < stuff.GetSize(); i++ )
	{
		TransformStuffComponent* pComponent		= stuff.GetComponent( i );
		// 중복된 재료를 가지고 있다면 조작된 것이다
		if( setItems.insert( pComponent->nItem ).second == false )
			return FALSE;
		CItemElem * pItem = pUser->GetItemId(pComponent->nItem);
		ASSERT( pItem );
		// 알이 아니면 부적합 재료
		if( !pItem->IsEgg() )
			return FALSE;
	}
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
CTransformItemComponent::CTransformItemComponent(const int nTransform)
	: m_nTransform(nTransform) {}

void CTransformItemComponent::AddElement(std::unique_ptr<CItemElem> item, std::uint32_t prob) {
	m_nTotalProb += prob;

	m_vTransformItemElements.emplace_back(
		TransformItemElement{
			.pItem = std::move(item),
			.nProb = m_nTotalProb
		}
	);
}

CItemElem * CTransformItemComponent::GetItem() {
	DWORD nProb = xRandom(0, eMaxProb);
	for (TransformItemElement & transformElement : m_vTransformItemElements) {
		if (nProb < transformElement.nProb) {
			transformElement.pItem->SetSerialNumber();
			return transformElement.pItem.get();
		}
	}
	return nullptr;
}

////////////////////////////////////////////////////////////////////////////////
CTransformItemProperty*	CTransformItemProperty::Instance( void )
{
	static	CTransformItemProperty	sTransformItemProperty;
	return &sTransformItemProperty;
}

BOOL CTransformItemProperty::LoadScript( const char* szFile )
{
	CScript s;
	if( s.Load( szFile ) == FALSE )
		return FALSE;

	int nTransform	= s.GetNumber();		// subject or FINISHED
	while( s.tok != FINISHED )
	{
		CTransformItemComponent * pComponent
			= m_mapComponents.emplace(
				nTransform,
				std::make_unique<CTransformItemComponent>(nTransform)
		).first->second.get();
		
		s.GetToken();	// {
		s.GetToken();	// subject or '}'
		while( *s.token != '}' )
		{
			if( s.Token == _T( "stuff" ) )
			{
				pComponent->SetStuffSize( s.GetNumber() );
			}
			else if( s.Token == _T( "item" ) )
			{
				s.GetToken();	// {
				int nProb	= s.GetNumber();
				while( *s.token != '}' )
				{
					const int nType = s.GetNumber();
					std::unique_ptr<CItemElem> pItem;
					if( nType == TI_GENERIC )
						pItem	= CreateItemGeneric( s );
					else if( nType == TI_PET )
						pItem	= CreateItemPet( s );
					assert( pItem );
					if (pItem) {
						pItem->SetSerialNumber(0);
					}
					pComponent->AddElement( std::move(pItem), nProb );
					nProb	= s.GetNumber();
				}
			}
			s.GetToken();
		}
		nTransform	= s.GetNumber();
	}
	return TRUE;
}

std::unique_ptr<CItemElem> CTransformItemProperty::CreateItemGeneric( CScript & s )
{
	std::unique_ptr<CItemElem> pItem = std::make_unique<CItemElem>();
	pItem->m_dwItemId	= s.GetNumber();
	if (!pItem->GetProp()) {
		Error(__FUNCTION__"(): Item %lu has no props", pItem->m_dwItemId);
		return nullptr;
	}
	pItem->m_nItemNum	= s.GetNumber();
	return pItem;
}

std::unique_ptr<CItemElem> CTransformItemProperty::CreateItemPet( CScript & s )
{
	std::unique_ptr<CItemElem> pItem = std::make_unique<CItemElem>();
	pItem->m_nItemNum	= 1;
	CPet* pPet	= pItem->m_pPet	= new CPet;
	pPet->SetKind( s.GetNumber() );
	s.GetToken();
	pPet->SetLevel( s.Token.GetLength() );
	pItem->m_dwItemId	= pPet->GetItemId();
	BYTE anAvail[PL_MAX - 1]	= { 0,};
	char sAvail[2]	= { 0,};
	for( int i = 0; i < pPet->GetLevel(); i++ )
	{
		sAvail[0]	= s.Token.GetAt( i );
		sAvail[1]	= '\0';
		anAvail[i]	= atoi( sAvail );
		if( anAvail[i] < 1 || anAvail[i] > 9 )
		{
			Error(__FUNCTION__"(): Bad avail %d", anAvail[i]);
			return nullptr;
		}
	}
	for( int i = PL_D; i <= pPet->GetLevel(); i++ )
		pPet->SetAvailLevel( i, anAvail[i-1] );
	for( int i = pPet->GetLevel() + 1; i <= PL_S; i++ )
		pPet->SetAvailLevel( i, 0 );
	pPet->SetLife( s.GetNumber() );
	DWORD dwExp		= (DWORD)( pPet->GetMaxExp() * s.GetFloat() / 100 );
	pPet->SetExp( dwExp );
	pPet->SetEnergy( pPet->GetMaxEnergy() );
	return pItem;
}

CTransformItemComponent * CTransformItemProperty::GetComponent(int nTransform) {
	const auto i = m_mapComponents.find(nTransform);
	return i != m_mapComponents.end() ? i->second.get() : nullptr;
}

u_int CTransformItemProperty::GetStuffSize( int nTransform )
{
	CTransformItemComponent* pComponent	= GetComponent( nTransform );
	if( pComponent )
		return pComponent->GetStuffSize();
	return 0;
}

CItemElem* CTransformItemProperty::GetItem( int nTransform )
{
	CTransformItemComponent* pComponent	= GetComponent( nTransform );
	if( pComponent )
		return pComponent->GetItem();
	return NULL;
}

#endif	// __WORLDSERVER


void CPet::SetName( const char* szName )
{
	strncpy( m_szName, szName, MAX_PET_NAME - 1 );
	m_szName[MAX_PET_NAME-1]	= '\0';
}
