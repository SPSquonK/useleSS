#ifndef __DPCERTIFIER_H__
#define	__DPCERTIFIER_H__

#include "dpmng.h"
#include "msghdr.h"
#include "misc.h"
#include <map>

#undef	theClass
#define theClass	CDPCertifier
#undef theParameters
#define theParameters	CAr & ar, DPID, LPBYTE, u_long

// 초당 접속자를 제한한다.
class CLoginLimit
{
private:
	int		m_nLimitation       = INT_MAX;
	DWORD	m_dwTick            = 0;
	int		m_nRequestPerSecond = 0;

public:
	bool	Check();
	void	SetLimitation(int nLimitation) {
		m_nLimitation = nLimitation;
	}
};

class CDPCertifier : public CDPMng
{
public:
	CLoginLimit		m_limit;
	DWORD			m_dwSizeofServerset;
	SERVER_DESC		m_aServerset[128];
	char	m_szVer[32];
#ifdef __SECURITY_0628
	char	m_szResVer[100];
#endif	// __SECURITY_0628
	std::map<u_long, SERVER_DESC *>	m_2ServersetPtr;
public:
//	Constructions
	CDPCertifier();
	virtual	~CDPCertifier();
// Operations
	virtual void	SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpid );
	virtual void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpid );
#ifdef __GPAUTH_01
#ifdef __GPAUTH_02
#ifdef __EUROPE_0514
	void	SendServerList( DPID dpid, DWORD dwAuthKey, BYTE cbAccountFlag, long lTimeSpan, const char* szGPotatoNo, const char* szCheck, const char* szBak );
#else	// __EUROPE_0514
	void	SendServerList( DPID dpid, DWORD dwAuthKey, BYTE cbAccountFlag, long lTimeSpan, const char* szGPotatoNo, const char* szCheck );
#endif	// __EUROPE_0514
#else	// __GPAUTH_02
	void	SendServerList( DPID dpid, DWORD dwAuthKey, BYTE cbAccountFlag, long lTimeSpan, const char* szGPotatoNo );
#endif	// __GPAUTH_02
#else	// __GPAUTH_01
	void	SendServerList( DPID dpid, DWORD dwAuthKey, BYTE cbAccountFlag, long lTimeSpan );
#endif	// __GPAUTH_01
	void	SendError( LONG lError, DPID dpid );
#ifdef __GPAUTH
	void	SendErrorString( const char* szError, DPID dpid );
#endif	// __GPAUTH
	void	SendHdr( DWORD dwHdr, DPID dpid );
	void	SetLimitation( int nLimit );

	USES_PFNENTRIES;

	// Handlers
	void	OnAddConnection( DPID dpid );
	void	OnRemoveConnection( DPID dpid );
	void	OnCertify( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize );
	void	OnPing( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize );
	void	OnCloseExistingConnection( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize );
	void	OnKeepAlive( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize );
	void	OnError( CAr & ar, DPID dpid, LPBYTE lpBuf, u_long uBufSize );
};

extern CDPCertifier g_dpCertifier;

inline void CDPCertifier::SendHdr( DWORD dwHdr, DPID dpid )
{
	BEFORESEND( ar, dwHdr );
	SEND( ar, this, dpid );
}

#endif	// __DPCERTIFIER_H__