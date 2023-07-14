#ifndef __DPCERTIFIED_H__
#define	__DPCERTIFIED_H__

#include "dpmng.h"
#include "msghdr.h"
#include "misc.h"
#include "ListedServer.h"

class CDPCertified : public CDPMng,
	public DPMngFeatures::SendPacketNone<CDPCertified>,
	public DPMngFeatures::PacketHandler<CDPCertified>
{
private:
	bool   m_fConn;
	bool   m_bRecvSvrList;
	CTimer m_timer;
	LONG   m_lError;					// protocol error code 


public:
	CListedServers m_servers;

public:
//	Constructions
	CDPCertified();

//	Overrides
	virtual void	SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpId );
	virtual void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID dpId );

//	Operations
	[[nodiscard]] LONG GetNetError()  const noexcept { return m_lError; }
	[[nodiscard]] LONG GetErrorCode() const noexcept { return m_lError; }
	[[nodiscard]] bool IsConnected()  const noexcept { return m_fConn; }
	[[nodiscard]] LPCTSTR GetServerName(int nServerIndex) const;
	void	Ping();
	void	SendNewAccount( LPCSTR lpszAccount, LPCSTR lpszpw );
	void	SendCloseExistingConnection( const char* lpszAccount, const char* lpszpw );
	void	SendCertify();

private:
	[[nodiscard]] bool CheckNofityDisconnected() const noexcept;

private:
	// Handlers
	void	OnSrvrList( CAr & ar );
	void	OnError( CAr & ar );
	void	OnErrorString( CAr & ar );
	void	OnKeepAlive( CAr & ar );
};

inline void CDPCertified::Ping() {
	if (m_timer.IsTimeOut()) {
		m_timer.Reset();
		SendPacket<PACKETTYPE_PING>();
	}
}

extern CDPCertified g_dpCertified;

#endif	// __DPCERTIFIED_H__