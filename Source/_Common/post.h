#pragma once

#include <map>
#include <list>


#define	MAX_MAILTITLE	32
#define	MAX_MAILTEXT	256

#define MAX_KEEP_MAX_DAY		15  // 최대 보관일수 15일
#define MAX_KEEP_BASIC_DAY		7	// 기본 보관일수 7일

class CItemElem;
class CMailBox;

class CMail final {
public:
	static u_long s_nMail;

	CMail();
	CMail(u_long idSender, CItemElem * pItemElem, int nGold,
		const char * szTitle, const char * szText
	);
	~CMail();

	enum	{	mail,	item,	gold,	read	};

	void Clear();
	[[nodiscard]] std::pair<int, DWORD> GetMailInfo() const;

	friend CAr & operator<<(CAr & ar, const CMail & mail);
	friend CAr & operator>>(CAr & ar, CMail & mail);
public:
	u_long	m_nMail;
	u_long	m_idSender;
	CItemElem*	m_pItemElem;
	DWORD	m_nGold;
	time_t	m_tmCreate;
	BYTE	m_byRead;
	char	m_szTitle[MAX_MAILTITLE];
	char	m_szText[MAX_MAILTEXT];
};

#ifdef __DBSERVER
class CPost;
#endif	// __DBSERVER

class CMailBox final {
public:
	static constexpr size_t SoftMaxMail = 50;
	std::vector<CMail *> m_mails;

	explicit CMailBox(u_long idReceiver = 0) : m_idReceiver(idReceiver) {}
	CMailBox(const CMailBox &) = delete;
	CMailBox & operator=(const CMailBox &) = delete;
	~CMailBox();

	u_long	AddMail( CMail* pMail );
	friend CAr & operator<<(CAr & ar, const CMailBox & mailBox);
	friend CAr & operator>>(CAr & ar, CMailBox & mailBox);

#ifdef __DBSERVER
	void	WriteMailContent( CAr & ar );
#endif	// __DBSERVER
#ifdef __WORLDSERVER
	void ReadMailContent(CAr & ar);
#endif	// __WORLDSERVER

	CMail*	GetMail( u_long nMail );
	BOOL	RemoveMail( u_long nMail );
	BOOL	RemoveMailItem( u_long nMail );
	BOOL	RemoveMailGold( u_long nMail );
	BOOL	ReadMail( u_long nMail );
	[[nodiscard]] bool IsStampedMailExists() const;
	void	Clear();
#ifdef __DBSERVER
	void	SetPost( CPost* pPost )	{	m_pPost	= pPost;	}
#endif	// __DBSERVER

#ifdef __CLIENT
	static	CMailBox*	GetInstance( void );

	[[nodiscard]] CMail * operator[](int nIndex) {
		if (nIndex < 0 || std::cmp_greater_equal(nIndex, m_mails.size())) {
			return nullptr;
		}

		return m_mails[nIndex];
	}

	[[nodiscard]] int GetSize() const {
		return static_cast<int>(m_mails.size());
	}
#endif

private:
	std::vector<CMail *>::iterator Find( u_long nMail );
#ifdef __DBSERVER
	CPost*	m_pPost = nullptr;
#endif	// __DBSERVER

public:
	u_long	m_idReceiver;
#ifdef __WORLDSERVER
	enum class BoxStatus { nodata = 0, read = 1, data = 2 };
	BoxStatus m_nStatus = BoxStatus::nodata;
#endif	// __WORLDSERVER
};

#if defined(__DBSERVER) || defined(__WORLDSERVER)
class CPost final {
public:
	void Clear();
//	Operations
	u_long	AddMail( u_long idReceiver, CMail* pMail );
	CMailBox*	GetMailBox( u_long idReceiver );
	BOOL	AddMailBox( CMailBox* pMailBox );
#ifdef __DBSERVER
	std::map< u_long, std::pair<CMailBox *, CMail *>> m_mapMail4Proc;
	CMclCritSec	m_csPost;
	void	Process();
#endif	// __DBSERVER

	static CPost * GetInstance();
private:
	std::map<u_long, std::unique_ptr<CMailBox>> m_mapMailBox;

public:
	// Post structure sending =
	// - List of mailboxes
	// - List of mails with only ID / read / time
	// Not using directly operator<</>> because it would give the wrong idea
	// that using these sends everything

	struct Structure {
		CPost * post;
	};
#ifdef __WORLDSERVER
	friend CAr & operator>>(CAr & ar, const Structure & structure);
#endif
#ifdef __DBSERVER
	friend CAr & operator<<(CAr & ar, const Structure & structure);
#endif

	[[nodiscard]] Structure AsStructure() { return Structure{ this }; }
};

#endif

