#include "stdafx.h"
#include "post.h"
#include <afxdisp.h>

#ifdef	__CLIENT
#include "DPClient.h"
#endif	//__CLIENT

u_long	CMail::s_nMail	= 0;

#ifdef __DBSERVER
#include "dptrans.h"
#endif	// __DBSERVER

CMail::CMail()
{
	m_nMail	= 0;
	m_idSender	= 0;
	m_pItemElem	= NULL;
	m_tmCreate	= 0;
	m_byRead	= FALSE;
	*m_szTitle	= '\0';
	*m_szText	= '\0';
	m_nGold		= 0;
}

CMail::CMail(u_long idSender, CItemElem* pItemElem, int nGold,
	const char* szTitle, const char* szText
) {
	m_nMail	= 0;
	m_idSender	= idSender;
	m_pItemElem	= pItemElem;
	m_tmCreate	= time_null();
	m_byRead	= FALSE;
	lstrcpy( m_szTitle, szTitle );
	lstrcpy( m_szText, szText );
	m_nGold		= 0;
}

CMail::~CMail()
{
	Clear();
}

void CMail::Clear( void )
{
	SAFE_DELETE( m_pItemElem );
}

std::pair<int, DWORD> CMail::GetMailInfo() const {
	const CTime BaseTime = m_tmCreate;
	const CTime CurrentTime = CTime::GetCurrentTime();
	
	const CTimeSpan ts = CurrentTime - BaseTime;

	const int nGap = (int)( (MAX_KEEP_MAX_DAY+1) - ts.GetDays() );
	const DWORD dwKeepingTime = (DWORD)( (MAX_KEEP_MAX_DAY*24) - ts.GetTotalHours() );

	return std::make_pair(nGap, dwKeepingTime);
}

CAr & operator<<(CAr & ar, const CMail & mail) {
	ar << mail.m_nMail << mail.m_idSender;
	if (mail.m_pItemElem) {
		ar << true;
		ar << *mail.m_pItemElem;
	} else {
		ar << false;
	}
	ar << mail.m_nGold;
	ar << time_null() - mail.m_tmCreate;

	ar << mail.m_byRead;
	ar << mail.m_szTitle << mail.m_szText;

	return ar;
}

CAr & operator>>(CAr & ar, CMail & mail) {
	ar >> mail.m_nMail >> mail.m_idSender;

	bool byItemElem; ar >> byItemElem;
	if (byItemElem) {
		mail.m_pItemElem = new CItemElem;
		ar >> *mail.m_pItemElem;
	}
	ar >> mail.m_nGold;
	
	time_t tm; ar >> tm;
	mail.m_tmCreate = time_null() - tm;
	
	ar >> mail.m_byRead;
	ar >> mail.m_szTitle >> mail.m_szText;

	return ar;
}

CMailBox::~CMailBox()
{
	Clear();
}

#ifdef __CLIENT
CMailBox*	CMailBox::GetInstance( void )
{
	static CMailBox	sMailBox;
	return &sMailBox;
}
#endif

void CMailBox::Clear() {
	for (CMail * pMail : m_mails) {
#ifdef __DBSERVER
		if (m_pPost)
			m_pPost->m_mapMail4Proc.erase(pMail->m_nMail);
#endif	// __DBSERVER
		SAFE_DELETE(pMail);
	}
	m_mails.clear();
}

u_long CMailBox::AddMail( CMail* pMail )
{
	if( 0 == pMail->m_nMail )
		pMail->m_nMail	= ++CMail::s_nMail;
	else
		CMail::s_nMail	= pMail->m_nMail;

	// 康: POST: m_nMail이 같은 메일이 이미 없는지 확인해야 한다.
	// 이미 있다면 별도의 예외 처리를 하자.

	m_mails.push_back( pMail );
#ifdef __DBSERVER
	if (m_pPost) {
		const bool bResult = m_pPost->m_mapMail4Proc.emplace(pMail->m_nMail, std::make_pair(this, pMail)).second;
		if (!bResult) {
			Error("AddMail Failed - nMail : %d, idSender : %d", pMail->m_nMail, pMail->m_idSender);
		}
	}
#endif	// __DBSERVER
	return pMail->m_nMail;
}

#ifdef __DBSERVER
void CMailBox::WriteMailContent(CAr & ar) {
	ar << static_cast<std::uint32_t>(m_mails.size());

	for (CMail * pMail : m_mails) {
		ar << pMail->m_nMail << *pMail;
	}
}
#endif	// __DBSERVER

#ifdef __WORLDSERVER
void CMailBox::ReadMailContent(CAr & ar) {
	std::uint32_t nSize; ar >> nSize;
	
	for (std::uint32_t i = 0; i < nSize; ++i) {
		u_long nMail; ar >> nMail;
		CMail* pMail = GetMail( nMail );
		if (pMail) {
			pMail->Clear();
			ar >> *pMail;
		} else {
			CMail * pNewMail = new CMail();
			pNewMail->Clear();
			ar >> *pNewMail;
			m_mails.push_back(pNewMail);
			Error("CMailBox::ReadMailContent - Create NewMail. nMail : %d, Sender : %07d", nMail, pNewMail->m_idSender);
		}
	}

	m_nStatus	= CMailBox::BoxStatus::data;
}
#endif	// __WORLDSERVER

CAr & operator<<(CAr & ar, const CMailBox & mailBox) {
	ar << mailBox.m_idReceiver;

	ar << (std::uint32_t)mailBox.m_mails.size();
	for (const CMail * pMail : mailBox.m_mails) {
		ar << *pMail;
	}

	return ar;
}

CAr & operator>>(CAr & ar, CMailBox & mailBox) {
	mailBox.Clear();
	ar >> mailBox.m_idReceiver;

	std::uint32_t nSize; ar >> nSize;

#ifdef __CLIENT
	if (g_WndMng.m_bWaitRequestMail && nSize <= 0) {
		g_DPlay.SendQueryMailBox();
	}
#endif

	for (std::uint32_t i = 0; i < nSize; i++) {
		CMail * pMail = new CMail;
		ar >> *pMail;
		mailBox.AddMail(pMail);
	}

	return ar;
}

std::vector<CMail *>::iterator CMailBox::Find(const u_long nMail) {
	return std::find_if(m_mails.begin(), m_mails.end(),
		[nMail](CMail * pMail) { return pMail->m_nMail == nMail; }
	);
}

CMail* CMailBox::GetMail(const u_long nMail) {
	const auto i = Find( nMail );
	if (i != m_mails.end()) {
		return *i;
	}

#ifdef __CLIENT
	Error( _T( "CMailBox::GetMail failed!!!!" ) );
#endif
	return nullptr;
}

BOOL CMailBox::RemoveMail( u_long nMail )
{
	auto i = Find( nMail );
	if( i != m_mails.end() )
	{
#ifdef __DBSERVER
		if( m_pPost )
			m_pPost->m_mapMail4Proc.erase( (*i)->m_nMail );
#endif	// __DBSERVER
		SAFE_DELETE( *i );
		m_mails.erase( i );

		return TRUE;
	}

#ifdef __CLIENT
		Error( _T( "CMailBox::RemoveMail Failed nMail:%d" ), nMail );
#endif
	return FALSE;
}

BOOL CMailBox::RemoveMailItem( u_long nMail )
{
	CMail* pMail	= GetMail( nMail );
	if( pMail && pMail->m_pItemElem )
	{
		SAFE_DELETE( pMail->m_pItemElem );
		return TRUE;
	}

#ifdef __CLIENT
		Error( _T( "CMailBox::RemoveMailItem Failed nMail:%d" ), nMail );
#endif
	return FALSE;
}

BOOL CMailBox::RemoveMailGold( u_long nMail )
{
	CMail* pMail	= GetMail( nMail );
	if( pMail && pMail->m_nGold > 0 )
	{
		pMail->m_nGold	= 0;
		return TRUE;
	}
#ifdef __CLIENT
		Error( _T( "CMailBox::RemoveMailGold Failed nMail:%d" ), nMail );
#endif
	return FALSE;
}

BOOL CMailBox::ReadMail( u_long nMail )
{
	CMail* pMail	= GetMail( nMail );
	if( pMail ) {
		pMail->m_byRead	= TRUE;
		return TRUE;
	}

#ifdef __CLIENT
		Error( _T( "CMailBox::ReadMail Failed nMail:%d" ), nMail );
#endif
	return FALSE;
}

bool CMailBox::IsStampedMailExists() const {
	return std::any_of(
		m_mails.begin(), m_mails.end(),
		[](CMail * pMail) { return pMail->m_byRead == FALSE; }
	);
}

#if defined(__DBSERVER) || defined(__WORLDSERVER)

void CPost::Clear( void )
{
	m_mapMailBox.clear();
#ifdef __DBSERVER
#ifdef _DEBUG
	TRACE( "CPost::m_mapMail4Proc.size() = %d\n", m_mapMail4Proc.size() );
#endif	// _DEBUG
	m_mapMail4Proc.clear();
#endif	// __DBSERVER
}

u_long CPost::AddMail( u_long idReceiver, CMail* pMail )
{
	CMailBox* pMailBox	= GetMailBox( idReceiver );
	if( !pMailBox )
	{
		pMailBox	= new CMailBox( idReceiver );
		if( AddMailBox( pMailBox ) == FALSE )
		{
			Error( "AddMailBox Failed - idReceiver : %d", idReceiver );
		}
		else
		{
// 			//	BEGINTEST
// 			Error( "CPost::AddMail [%d]", idReceiver );
		}
	}
	return pMailBox->AddMail( pMail );
}

CMailBox* CPost::GetMailBox( u_long idReceiver )
{
	auto i = m_mapMailBox.find( idReceiver );
	if( i != m_mapMailBox.end() )
		return i->second.get();
	return NULL;
}

BOOL CPost::AddMailBox( CMailBox* pMailBox )
{
#ifdef __DBSERVER
	pMailBox->SetPost( this );
#endif	// __DBSERVER
	return m_mapMailBox.emplace(pMailBox->m_idReceiver, pMailBox).second;
}

#ifdef __DBSERVER
CAr & operator<<(CAr & ar, const CPost::Structure & structure) {
	CPost & post = *structure.post;

	ar << static_cast<std::uint32_t>(post.m_mapMailBox.size());
	for (const auto & [_, pMailBox] : post.m_mapMailBox) {
		ar << pMailBox->m_idReceiver;

		ar << static_cast<std::uint32_t>(pMailBox->m_mails.size());
		for (CMail * pMail : pMailBox->m_mails) {
			ar << pMail->m_nMail;
			ar << time_null() - pMail->m_tmCreate;
			ar << pMail->m_byRead;
		}
	}

	return ar;
}
#endif

#ifdef __WORLDSERVER
CAr & operator>>(CAr & ar, const CPost::Structure & structure) {
	CPost & post = *structure.post;

	std::uint32_t nbMailBoxes; ar >> nbMailBoxes;
	for (std::uint32_t iMailBoxes = 0; iMailBoxes != nbMailBoxes; ++iMailBoxes) {
		CMailBox * pMailBox = new CMailBox();
		ar >> pMailBox->m_idReceiver;

		std::uint32_t nbMails; ar >> nbMails;
		for (std::uint32_t iMails = 0; iMails != nbMails; ++iMails) {
			CMail * pMail = new CMail();
			ar >> pMail->m_nMail;
			
			time_t tm; ar >> tm;
			pMail->m_tmCreate = time_null() - tm;
			ar >> pMail->m_byRead;

			pMailBox->AddMail(pMail);
		}

		post.AddMailBox(pMailBox);
	}

	return ar;
}
#endif

CPost*	CPost::GetInstance( void )
{
	static CPost	sPost;
	return &sPost;
}

#ifdef __DBSERVER
void CPost::Process( void )
{
	CMclAutoLock	Lock( m_csPost );
	CTime t	= CTime::GetCurrentTime() - CTimeSpan( MAX_KEEP_MAX_DAY, 0, 0, 0 );

	std::vector<std::pair<CMailBox *, CMail *>> lpsMail;

	for (const auto & pair : m_mapMail4Proc | std::views::values) {
		if (pair.second->m_tmCreate < t.GetTime()) {
			lpsMail.emplace_back(pair);
		}
	}

	g_DbManager.RemoveMail(lpsMail);
}
#endif	// __DBSERVER

#endif

