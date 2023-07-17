#include "stdafx.h"
#include "couple.h"


BOOL CCoupleTestCase::Test()
{
	/*
	// Load
	CCoupleProperty* ptr	= CCoupleProperty::Instance();
	BOOL bResult	= ptr->Initialize();
	ASSERT( bResult );

	CCouple	c( 1, 2 );
	for( int nLevel = 1; nLevel < CCouple::eMaxLevel; nLevel++ )
	{
		ASSERT( c.GetLevel() == nLevel );
		int nExperience	= ptr->GetExperienceRequired( c.GetLevel() );
		c.AddExperience( nExperience );
	}
	return FALSE;
	*/
	/*
	// Level
	for( int nLevel = 1; nLevel <= CCouple::eMaxLevel; nLevel++ )
		TRACE( "Level: %d = Exp: %d\n", nLevel, ptr->GetTotalExperience( nLevel ) );
	// Item
	for( nLevel = 1; nLevel <= CCouple::eMaxLevel; nLevel++ )
	{
		std::vector<COUPLE_ITEM>& vItems	= ptr->GetItems( nLevel );
		TRACE( "Level: %d: Item\n", nLevel );
		for( int i = 0; i < vItems.size(); i++ )
			TRACE( "\tnItem: %d, nFlags: %d, nLife: %d, nNum: %d\n", vItems[i].nItem, vItems[i].nFlags, vItems[i].nLife, vItems[i].nNum );
	}
	for( nLevel = 1; nLevel <= CCouple::eMaxLevel; nLevel++ )
	{
		std::vector<int>& vSkills	= ptr->GetSkill( nLevel );
		TRACE( "Level: %d: Skill\n", nLevel );
		for( int i = 0; i < vSkills.size(); i++ )
			TRACE( "\tSkill: %d\n", vSkills[i] );
	}
	u_long idFirst	= 1;
	u_long idSecond	= 2;
	u_long idInvalid	= 0;

	CCoupleMgr* pMgr	= new CCoupleMgr;
	pMgr->Couple( idFirst, idSecond );
	ASSERT( pMgr->GetCount() == 1 );
	pMgr->Decouple( idFirst );
	ASSERT( pMgr->GetCount() == 0 );
	pMgr->Couple( idFirst, idSecond );
	ASSERT( pMgr->GetCount() == 1 );
	pMgr->Decouple( idSecond );
	ASSERT( pMgr->GetCount() == 0 );
	pMgr->Couple( idFirst, idSecond );
	ASSERT( pMgr->GetCount() == 1 );
	BOOL bResult	= pMgr->Decouple( idInvalid );
	ASSERT( bResult == FALSE );
	ASSERT( pMgr->GetCount() == 1 );
	CCouple* pCoupleByFirst	= pMgr->GetCouple( idFirst );
	ASSERT( pCoupleByFirst );
	CCouple* pCoupleBySecond	= pMgr->GetCouple( idSecond );
	ASSERT( pCoupleBySecond );
	ASSERT( pCoupleByFirst == pCoupleBySecond );
	CCouple* pCouple	= pCoupleByFirst;
	ASSERT( pCouple->GetLevel() == 1 );
	pCouple->AddExperience( 1 );
	ASSERT( pCouple->GetExperience() == 1 );
	pCouple->AddExperience( 1 );
	ASSERT( pCouple->GetExperience() == 2 );
	pMgr->OnTimer();
	ASSERT( pCouple->GetExperience() == 3 );
*/
	return TRUE;
}

////////////////////////////////////////////////////////////
CCouple::CCouple()
:
m_nExperience( 0 ),
m_idFirst( 0 ),
m_idSecond( 0 ),
m_nLevel( 1 )
{
}

CCouple::CCouple( u_long idFirst, u_long idSecond )
:
m_nExperience( 0 ),
m_idFirst( idFirst ),
m_idSecond( idSecond ),
m_nLevel( 1 )
{
}

BOOL CCouple::AddExperience( int nExperience )
{
	int nLevel	= GetLevel();
	if( nLevel == eMaxLevel )
		return FALSE;
	m_nExperience	+= nExperience;
	return nLevel != GetLevel( TRUE );
}

u_long CCouple::GetPartner( u_long idPlayer )
{
	ASSERT( HasPlayer( idPlayer ) );
	return m_idFirst == idPlayer? m_idSecond: m_idFirst;
}

void CCouple::OnTimer()
{
	AddExperience( 1 );		//
}

CAr & operator<<(CAr & ar, const CCouple & self) {
	ar << self.m_nExperience << self.m_idFirst << self.m_idSecond;
	return ar;
}

CAr & operator>>(CAr & ar, CCouple & self) {
	ar >> self.m_nExperience >> self.m_idFirst >> self.m_idSecond;
	self.GetLevel(TRUE);
	return ar;
}

int CCouple::GetLevel( BOOL bCalc )
{
	if( bCalc )
		m_nLevel	= CCoupleProperty::Instance()->GetLevel( GetExperience() );
	return m_nLevel;
}
////////////////////////////////////////////////////////////

void CCoupleMgr::Clear() {
	m_mapPlayers.clear();
	m_vCouples.clear();
}

CCouple * CCoupleMgr::GetCouple(u_long idPlayer) {
	const auto i = m_mapPlayers.find(idPlayer);
	return i != m_mapPlayers.end() ? i->second : nullptr;
}

void CCoupleMgr::Couple(u_long idFirst, u_long idSecond) {
	Couple(std::make_unique<CCouple>(idFirst, idSecond));
}

bool CCoupleMgr::Decouple(const u_long idFirst) {
	const auto i = std::find_if(m_vCouples.begin(), m_vCouples.end(),
		[idFirst](auto & pCouple) { return pCouple->HasPlayer(idFirst); }
	);

	if (i == m_vCouples.end()) return false;

	m_mapPlayers.erase(idFirst);
	m_mapPlayers.erase((*i)->GetPartner(idFirst));
	m_vCouples.erase(i);
	return true;
}

void CCoupleMgr::OnTimer() {
	for (auto & pCouple : m_vCouples) {
		pCouple->OnTimer();
	}
}

CAr & operator<<(CAr & ar, const CCoupleMgr & self) {
	ar << self.m_vCouples.size();
	for (const auto & pCouple : self.m_vCouples) {
		ar << *pCouple;
	}

	return ar;
}

CAr & operator>>(CAr & ar, CCoupleMgr & self) {
	self.Clear();
	size_t nSize;
	ar >> nSize;
	for (size_t i = 0; i < nSize; i++) {
		auto pCouple = std::make_unique<CCouple>();
		ar >> *pCouple;
		self.Couple(std::move(pCouple));
	}

	return ar;
}

void CCoupleMgr::Couple( std::unique_ptr<CCouple> pCouple_ )
{
	auto & pCouple = m_vCouples.emplace_back(std::move(pCouple_));
	bool bResult	= m_mapPlayers.emplace( pCouple->GetFirst(), pCouple.get() ).second;
	ASSERT( bResult );
	bResult	= m_mapPlayers.emplace( pCouple->GetSecond(), pCouple.get() ).second;
	ASSERT( bResult );
}

CCoupleProperty::CCoupleProperty()
{
}

CCoupleProperty::~CCoupleProperty()
{
	m_vExp.clear();
	m_vItems.clear();
	m_vSkillKinds.clear();
	m_vSkills.clear();
}

BOOL CCoupleProperty::Initialize()
{
	CScript s;
	if( s.Load( "couple.inc" ) == FALSE )
		return FALSE;
	s.GetToken();
	while( s.tok != FINISHED )
	{
		if( s.Token == _T( "Level" ) )
			LoadLevel( s );
		else if( s.Token == _T( "Item" ) )
			LoadItem( s );
		else if( s.Token == _T( "SkillKind" ) )
			LoadSkillKind( s );
		else if( s.Token == _T( "SkillLevel" ) )
			LoadSkillLevel( s );
		s.GetToken();
	}
	return TRUE;
}

void CCoupleProperty::LoadLevel( CScript & s )
{
	s.GetToken();	// {
	int nExp	= s.GetNumber();
	while( *s.token != '}' )
	{
#ifndef __MAINSERVER
		nExp	/= 100;	// 정식 서버가 아니라면 테스트를 위해서 시간을 1/100로 줄인다.
#endif	// __MAINSERVER
		m_vExp.push_back( nExp );
		nExp	= s.GetNumber();
	}
	m_vItems.resize( m_vExp.size() );
	m_vSkills.resize( m_vExp.size() );
}

void CCoupleProperty::LoadItem( CScript & s )
{
	ASSERT( m_vItems.size() > 0 );
	s.GetToken();	// {
	int nLevel	= s.GetNumber();
	while( *s.token != '}' )
	{
		ASSERT( nLevel <= (int)( m_vItems.size() ) );
		s.GetToken();	// {
		int nItem	= s.GetNumber();
		while( *s.token != '}' )
		{
			int	nSex	= s.GetNumber();
			int nFlags	= s.GetNumber();
			int nLife	= s.GetNumber();
			int nNum	= s.GetNumber();
			AddItem( nLevel, COUPLE_ITEM( nItem, nSex, nFlags, nLife, nNum ) );
			nItem	= s.GetNumber();
		}
		nLevel	= s.GetNumber();
	}
}

std::vector<COUPLE_ITEM> & CCoupleProperty::GetItems( int nLevel )
{
	ASSERT( nLevel <= (int)( m_vItems.size() ) );
	return m_vItems[nLevel-1];
}

void CCoupleProperty::AddItem(int nLevel, const COUPLE_ITEM & ci) {
	GetItems(nLevel).emplace_back(ci);
}

void CCoupleProperty::LoadSkillKind( CScript & s )
{
	s.GetToken();	// {
	int nSkill	= s.GetNumber();
	while( *s.token != '}' )
	{
		m_vSkillKinds.push_back( nSkill );
		nSkill	= s.GetNumber();
	}
}

void CCoupleProperty::LoadSkillLevel( CScript & s )
{
	ASSERT( m_vSkillKinds.size() > 0 );
	ASSERT( m_vSkills.size() > 0 );
	s.GetToken();	// {
	int nLevel	= s.GetNumber();
	while( *s.token != '}' )
	{
		for( int i = 0; i < (int)( m_vSkillKinds.size() ); i++ )
		{
			int nSkillLevel	= s.GetNumber();
			int nSkill	= 0;
			if( nSkillLevel > 0 )
				nSkill	= m_vSkillKinds[i] + nSkillLevel - 1;
			m_vSkills[nLevel-1].push_back( nSkill );
		}
		nLevel	= s.GetNumber();
	}
	for( int i = 1; i <(int)( m_vSkills.size() ); i++ )
	{
		if( m_vSkills[i].empty() )
			m_vSkills[i].assign( m_vSkills[i-1].begin(), m_vSkills[i-1].end() );
	}
}

int CCoupleProperty::GetLevel( int nExperience )
{
	for( int i = 0; i <(int)( m_vExp.size() ); i++ )
	{
		if( nExperience < m_vExp[i] )
			return i;
	}
	return 1;
}

std::vector<int> & CCoupleProperty::GetSkill( int nLevel )
{
	ASSERT( nLevel <= (int)( m_vSkills.size() ) );
	return m_vSkills[nLevel-1];
}

CCoupleProperty* CCoupleProperty::Instance()
{
	static CCoupleProperty sCoupleProperty;
	return &sCoupleProperty;
}

int CCoupleProperty::GetTotalExperience( int nLevel )
{
	ASSERT( nLevel <= (int)( m_vExp.size() ) );
	return m_vExp[nLevel-1];
}

int CCoupleProperty::GetExperienceRequired( int nLevel )
{
	if ( nLevel == CCouple::eMaxLevel )
		return 0;
	return GetTotalExperience( nLevel + 1 ) - GetTotalExperience( nLevel );
}

float	CCoupleProperty::GetExperienceRate( int nLevel, int nExperience )
{
	int nTotal	= GetExperienceRequired( nLevel );
	if( nTotal == 0 )
		return 0.0F;

	return static_cast<float>( nExperience - GetTotalExperience( nLevel ) ) / static_cast<float>( nTotal );
}

