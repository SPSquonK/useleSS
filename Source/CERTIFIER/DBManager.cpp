#include "stdafx.h"
#include "dbmanager.h"
#include <chrono>
#include <mutex>
#include <string_view>

// Disable mutex related warnings
#pragma warning(disable: 26111)

#include "afxinet.h"
#include "dpcertifier.h"
#include "dpaccountclient.h"
#include "IpAddressRecentFailChecker.h"
#include "..\Resource\Lang.h"

/////////////////////////////////////////////////////////////////////////////////////////////////
// global
CDbManager	g_DbManager;

/////////////////////////////////////////////////////////////////////////////////////////////////

CDbManager::~CDbManager() {
	// End all workers through completition port
	for (auto & worker : m_workers) {
		if (worker.ioCompletionPort) {
			PostQueuedCompletionStatus(worker.ioCompletionPort, 0, NULL, NULL);
			CLOSE_HANDLE(worker.ioCompletionPort);
		}
	}

	// Waiting for actual stop is performed by the destructor of std::jthread
}

BOOL CDbManager::CreateDbWorkers()
{
	shared_ptr_timed_mutex loadedDatabase = std::make_shared<std::timed_mutex>();
	loadedDatabase->lock();

	for (auto & [thread, ioCompletionPort] : m_workers) {
		ioCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
		ASSERT(ioCompletionPort);
		
		thread = std::jthread(DbWorkerThread, ioCompletionPort, loadedDatabase);
		
		// Wait for the thread to unlock the mutex
		using namespace std::chrono_literals;
		if (!loadedDatabase->try_lock_for(10s)) {
			OutputDebugString("CERTIFIER.EXE\t// TIMEOUT\t// ODBC");
			return FALSE;
		}
	}

	loadedDatabase->unlock();

	return TRUE;
}

StaticString<256> CDbManager::DBQryAccount(const DB_OVERLAPPED_PLUS & pData) {
	char lpAddr[16] = { 0, };
	g_dpCertifier.GetPlayerAddr(pData.dpId, lpAddr);

	StaticString<256> qryAccount;
	if (GetLanguage() == LANG_TWN) {
		qryAccount.Format("LOGIN_STR @iaccount='%s', @ipassword='%s', @isession='%s', @i_IPAddress='%s'",
			pData.AccountInfo.szAccount, pData.AccountInfo.szPassword, pData.AccountInfo.szSessionPwd, lpAddr
		);
	} else {
		qryAccount.Format("LOGIN_STR '%s', '%s', '%s'",
			pData.AccountInfo.szAccount, pData.AccountInfo.szPassword, lpAddr
		);
	}

	return qryAccount;
}

// AccountFlag�� ��´�.
// f18 - ��񿡼� ��� ����Ÿ 
//       �±�: 2 - �̼�����, 4 - �̵�� ����, 8 - ��ϵ� ���� 
//       �׿�: 0 - �̼�����, 1 - ���� 

BYTE CDbManager::GetAccountFlag( int f18, LPCTSTR szAccount )
{
	BYTE cb18 = (BYTE)f18;
	BYTE cbAccountFlag = 0x00;

	if( ::GetLanguage() == LANG_THA )
	{
		if( cb18 & 0x04 )
			cbAccountFlag |= ACCOUNT_FLAG_UNREGISTER18;		// �̵�� ����
		else if( cb18 & 0x08 )
			cbAccountFlag |= ACCOUNT_FLAG_18;
	}
	else
	{
		if( cb18 & 0x01 )
			cbAccountFlag |= ACCOUNT_FLAG_18;
	}

	if (IsEveSchoolAccount(szAccount)) {
		cbAccountFlag |= ACCOUNT_FLAG_SCHOOLEVENT;
	}

//	char szBuffer[256];
//	sprintf( szBuffer, "%s - f18:%02x cbAccountFlag:%02x\n", szAccount, cb18, cbAccountFlag );
//	OutputDebugString( szBuffer );

	return cbAccountFlag;
}

void CDbManager::OnCertifyQueryOK( CQuery & query, LPDB_OVERLAPPED_PLUS pData, const char* szCheck )
{
	BOOL bGPotatoAuth	= ::GetLanguage() == LANG_FRE; // ::GetLanguage() == LANG_GER || ::GetLanguage() == LANG_FRE;
	int n18	= 1;
	if( !bGPotatoAuth )
		n18	= query.GetInt( "f18" );
	BYTE cbAccountFlag	= GetAccountFlag( n18, pData->AccountInfo.szAccount );
	if( ::GetLanguage() == LANG_THA )
	{
		if( (cbAccountFlag & ACCOUNT_FLAG_18) == 0x00 )
		{
			CTime cur = CTime::GetCurrentTime();
			if( cur.GetHour() >= 22 || cur.GetHour() < 6 ) 
			{
				g_dpCertifier.SendError( ERROR_TOO_LATE_PLAY, pData->dpId );
				return;
			}
		}
	}


	int fCheck = 0;

	char lpAddr[16]	= { 0, };
	g_dpCertifier.GetPlayerAddr( pData->dpId, lpAddr );				

	g_dpAccountClient.SendAddAccount(pData->dpId,
		{
			.lpAddr = lpAddr,
			.lpszAccount = pData->AccountInfo.szAccount,
			.cbAccountFlag = cbAccountFlag,
			.fCheck = fCheck,
#ifdef __GPAUTH_02
			.szCheck = szCheck,
#ifdef __EUROPE_0514
			.szBak = pData->AccountInfo.szBak,
			.dwPCBangClass = pData->AccountInfo.dwPCBangClass
#endif	// __EUROPE_0514
#endif	// __GPAUTH_02
		});
}

void CDbManager::Certify( CQuery & query, LPDB_OVERLAPPED_PLUS pData, IpAddressRecentFailChecker & accountMgr )
{
	ACCOUNT_CHECK result = ACCOUNT_CHECK::Ok;
	if( pData->dwIP )
	{
		result = accountMgr.Check( pData->dwIP );
		switch( result )
		{
			case ACCOUNT_CHECK::x1TimeError:
				g_dpCertifier.SendError(ERROR_15SEC_PREVENT, pData->dpId);
				return;
			case ACCOUNT_CHECK::x3TimeError:
				g_dpCertifier.SendError(ERROR_15MIN_PREVENT, pData->dpId);
				return;
		}
	}

	query.Clear();

	const StaticString<256> szQuery = DBQryAccount(*pData);

	int nCode = ERROR_CERT_GENERAL;		

	if( query.Exec( szQuery ) )
	{
		if( query.Fetch() )
		{
			int nError	= query.GetInt( "fError" );
			switch( nError )
			{
			case 0:
				pData->AccountInfo.dwPCBangClass = query.GetInt( "fPCZone" );
				if( pData->dwIP )
					accountMgr.SetError(false);
#ifdef __EUROPE_0514
				lstrcpy( pData->AccountInfo.szBak, pData->AccountInfo.szAccount );
#endif	// __EUROPE_0514
				OnCertifyQueryOK( query, pData );
				return;
			case 1:	// ��ȣƲ��
				if( pData->dwIP )
					accountMgr.SetError(true);
				nCode = ERROR_FLYFF_PASSWORD;
				break;
			case 3:	// ���������̰ų� ����ȭ �ʰ�
				nCode = ERROR_BLOCKGOLD_ACCOUNT;				
				break;
			case 4:	// �Ǹ������� ���������� �����մϴ� www.flyff.com���� �������ֽʽÿ�
				nCode = ERROR_FLYFF_AUTH;				
				break;
			case 5: // �������� 12�� �̻� �̿밡 �̹Ƿ� ���������� �Ҽ� �����ϴ�.
				nCode = ERROR_FLYFF_PERMIT;
				break;
			case 6: // 14�� �̸� ������ �е��� �θ�� ���Ǽ��� �����ּž� ���� ������ �����մϴ�. www.flyff.com ���� �����ϼż� Ȯ���� �ּ���.
				nCode = ERROR_FLYFF_NEED_AGREEMENT;
				break;
			case 7:	// WebŻ���� ȸ��
				nCode = ERROR_FLYFF_NO_MEMBERSHIP;
				break;
			case 9:	// �ǽð� ������ �۾� ����
				nCode = ERROR_FLYFF_DB_JOB_ING;
				break;
			case 91:
				nCode	= ERROR_FLYFF_EXPIRED_SESSION_PASSWORD;
				break;
			default: 
				nCode = ERROR_FLYFF_ACCOUNT;				
				break;
			}
		}
	}

	g_dpCertifier.SendError( nCode, pData->dpId );
}

void CDbManager::CloseExistingConnection( CQuery & query, LPDB_OVERLAPPED_PLUS pData )
{
	query.Clear();

	const StaticString<256> szQuery = DBQryAccount(*pData);

	if( query.Exec( szQuery ) )
	{
		if( query.Fetch() )
		{
			if( 0 == query.GetInt( "fError" ) )
				g_dpAccountClient.SendCloseExistingConnection( pData->AccountInfo.szAccount );
		}
	}
}

void DbWorkerThread(HANDLE hIOCP, shared_ptr_timed_mutex mutex) {
	IpAddressRecentFailChecker mgr;

	CQuery query;
	if (!query.Connect(3, "useless_account", "account", g_DbManager.m_szLoginPWD)) {
		AfxMessageBox("Error : Not Connect useless_account DB");
	}

	mutex->unlock();
	mutex.reset();

	DWORD dwBytesTransferred	= 0;
	LPDWORD lpCompletionKey		= NULL;
	LPDB_OVERLAPPED_PLUS pData	= NULL;

	while (true) {
		const BOOL fOk = GetQueuedCompletionStatus( hIOCP,
										 &dwBytesTransferred,
										(LPDWORD)&lpCompletionKey,
										(LPOVERLAPPED*)&pData,
										INFINITE );

		if( fOk == FALSE ) 
		{	
			ASSERT( 0 );				// fatal error
			continue;
		}

		if( dwBytesTransferred == 0 )	// terminate
			return;
		
		switch( pData->nQueryMode )
		{
			case CERTIFY:
				g_DbManager.Certify( query, pData, mgr );
				break;
			case CLOSE_EXISTING_CONNECTION:
				g_DbManager.CloseExistingConnection( query, pData );
				break;
		}

		g_DbManager.DeAlloc(pData);
	}
}

BOOL CDbManager::LoadEveSchoolAccount( void )
{
	CScanner s;
	if( s.Load( "EveSchoolAccount.txt" ) )
	{
		s.GetToken();
		while( s.tok != FINISHED )
		{
			m_eveSchoolAccount.insert( (const char*)s.Token );
			s.GetToken();
		}
		return TRUE;
	}
	return FALSE;
}

bool CDbManager::IsEveSchoolAccount(const char * const pszAccount) const {
	return m_eveSchoolAccount.contains(pszAccount);
}

void CDbManager::PostQ(DB_OVERLAPPED_PLUS * pData) {
	std::string_view accountSv = pData->AccountInfo.szAccount;
	const size_t nKey = std::hash<std::string_view>()(accountSv);
	const size_t nIOCP = nKey % DEFAULT_DB_WORKER_THREAD_NUM;
	::PostQueuedCompletionStatus(m_workers[nIOCP].ioCompletionPort, 1, NULL, (LPOVERLAPPED)pData);
}


void CDbManager::SQLAddAccount( CQuery & query, char* szAccount, char* szPassword, BOOL bGM )
{
	query.Clear();
	char szSQL[100]	= { 0,};

	char chAuth	= ( bGM? 'P': 'F' );
	sprintf( szSQL, "uspAddAccount '%s', '%s', '%c'", szAccount, szPassword, chAuth );

	if( !query.Exec( szSQL ) )
	{
		// error
	}
}
