#include "stdafx.h"
#include "user.h"
#include "msghdr.h"
#include <ranges>
#ifdef _DEBUG
#include "mytrace.h"
extern	CMyTrace	g_MyTrace;
#endif	// _DEBUG

CCertUserMng g_CertUserMng;

CCertUser::CCertUser(DPID dpid)
	:
	m_dpid(dpid),
	m_bValid(TRUE) {
	m_dwTick = GetTickCount();
	*m_pszAccount = '\0';
}

CCertUserMng::~CCertUserMng() {
	const auto lock = std::lock_guard(m_AddRemoveLock);
	m_users.clear();
}

bool CCertUserMng::AddUser(const DPID dpid) {
	CCertUser * const pUser	= new CCertUser(dpid);

	const auto lock = std::lock_guard(m_AddRemoveLock);

	bool bResult	= m_users.emplace(dpid, pUser).second;
	if( bResult == false ) {
		WriteError( "ADD//0" );
		return false;
	}
#ifdef _DEBUG
	g_MyTrace.Add( CMyTrace::Key( "count" ), FALSE, "// %04d", m_users.size() );
#endif	// _DEBUG
	return true;
}

bool CCertUserMng::RemoveUser(const DPID dpid) {
	const auto lock = std::lock_guard(m_AddRemoveLock);
	auto i	= m_users.find( dpid );
	if( i != m_users.end() )
	{
		m_users.erase( i );
#ifdef _DEBUG
	g_MyTrace.Add( CMyTrace::Key( "count" ), FALSE, "// %04d", m_users.size() );
#endif	// _DEBUG
		return true;
	}
	WriteError( "REMOVE//0" );
	return false;
}

CCertUser * CCertUserMng::GetUser( DPID dpid )
{
	auto i	= m_users.find( dpid );
	if( i != m_users.end() )
		return i->second.get();
	return NULL;
}

void CCertUserMng::ClearDum( CDPMng* pdp )
{
	BEFORESEND( ar, PACKETTYPE_KEEP_ALIVE );

	DWORD dwTick	= GetTickCount() - SEC( 10 );

	const auto lock = std::lock_guard(m_AddRemoveLock);

	for (auto & pUsertmp : m_users | std::views::values) {
		if (pUsertmp->m_dwTick < dwTick) {
			pdp->DestroyPlayer(pUsertmp->m_dpid);
		} else if (pUsertmp->m_bValid == FALSE) {
			pUsertmp->m_bValid = TRUE;
			pdp->DestroyPlayer(pUsertmp->m_dpid);
		} else {
			pUsertmp->m_bValid = FALSE;
			SEND(ar, pdp, pUsertmp->m_dpid);
		}
	}
}

void CCertUserMng::KeepAlive(const DPID dpid) {
	const auto lock = std::lock_guard(m_AddRemoveLock);

	if (CCertUser * const pUser = GetUser(dpid)) {
		pUser->m_bValid = TRUE;
	}
}

bool CCertUserMng::SetAccount(const DPID dpid, const char * const accountName) {
	const auto lock = std::lock_guard(m_AddRemoveLock);

	CCertUser * const pUser = GetUser(dpid);
	if (!pUser) return false;
	
	pUser->SetAccount(accountName);
	return true;
}

void CCertUserMng::PrintUserAccount(const DPID dpid) {
	const auto lock = std::lock_guard(m_AddRemoveLock);

	if (CCertUser * const pUser = GetUser(dpid)) {
		Error("account: %s", pUser->GetAccount());
	}
}

bool CCertUserMng::AccountCheckOk(const DPID dpid, const char * const szBak) {
	const auto lock = std::lock_guard(m_AddRemoveLock);
	CCertUser * pUser = GetUser(dpid);
	if (!pUser) return false;
	
	pUser->m_dwTick = 0xffffffff;

#ifdef __EUROPE_0514
	if (lstrcmp(pUser->GetAccount(), szBak)) {
		Error("CDPAccountClient.OnAddAccount: %s, %s", pUser->GetAccount(), szBak);
		return false;
	}
#endif	// __EUROPE_0514

	return true;
}
