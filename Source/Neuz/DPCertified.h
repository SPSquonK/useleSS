#ifndef __DPCERTIFIED_H__
#define	__DPCERTIFIED_H__

#include "dpmng.h"
#include "msghdr.h"
#include "misc.h"
#include "ListedServer.h"

#undef	theClass
#define theClass	CDPCertified
#undef theParameters
#define theParameters	CAr & ar, DPID

class CDPCertified : public CDPMng
{
private:
	BOOL	m_fConn;
	CTimer	m_timer;
	LONG	m_lError;					// protocol error code 

public:
	CListedServers m_servers;

public:
//	Constructions
	CDPCertified();
	virtual	~CDPCertified();

//	Overrides
	virtual void	SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpId );
	virtual void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpId );

//	Operations
	LONG	GetNetError();
	LONG	GetErrorCode()	{ return m_lError; }
//	void	Destroy( BOOL bDestroy );
	BOOL	IsDestroyed( void );
	BOOL	IsConnected( void );
	void	Ping( void );
	void	SendNewAccount( LPCSTR lpszAccount, LPCSTR lpszpw );
	[[nodiscard]] LPCTSTR GetServerName( int nServerIndex ) const;
	void	SendHdr( DWORD dwHdr );
	void	SendCloseExistingConnection( const char* lpszAccount, const char* lpszpw );
	BOOL	CheckNofityDisconnected();
	void	SendCertify();

	USES_PFNENTRIES;

	// Handlers
	void	OnSrvrList( CAr & ar, DPID );
	void	OnError( CAr & ar, DPID dpid );
#ifdef __GPAUTH
	void	OnErrorString( CAr & ar, DPID dpid );
#endif	// __GPAUTH
	void	OnKeepAlive( CAr & ar, DPID );
};

inline void CDPCertified::SendHdr( DWORD dwHdr )
{
	BEFORESEND( ar, dwHdr );
	SEND( ar, this, DPID_SERVERPLAYER );
}
inline void CDPCertified::Ping( void )
{
	if( m_timer.IsTimeOut() ) 
	{
		m_timer.Reset();
		SendHdr( PACKETTYPE_PING );
	}
}

inline BOOL CDPCertified::IsConnected( void )
{	
	return m_fConn;	
}

extern CDPCertified g_dpCertified;

#endif	// __DPCERTIFIED_H__