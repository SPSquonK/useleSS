#pragma once 

#include "dpmng.h"
#include "msghdr.h"
#include "misc.h"
#include <map>
#include <span>
#include "ListedServer.h"
#include "sqktd/mutexed_object.h"

class CDPCertifier : public CDPMng,
	public DPMngFeatures::PacketHandler<CDPCertifier, DPID, BYTE *, u_long>
{
public:
	sqktd::mutexed_on_write_object<CListedServers> m_servers;

	char	m_szVer[32]     = "";
#ifdef __SECURITY_0628
	char	m_szResVer[100] = "";
#endif	// __SECURITY_0628
public:
//	Constructions
	CDPCertifier();
	~CDPCertifier() override = default;
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


private:
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
