#pragma once

#include "DPMng.h"
#include "MsgHdr.h"

#undef	theClass
#define theClass	CDPLoginSrvr
#undef theParameters
#define theParameters	CAr & ar, DPID

typedef	struct	tagCACHESTRUCT
{
	char	lpAddr[16];
	u_long	uCount;
}
CACHESTRUCT, *LPCACHESTRUCT;
#define	MAX_CACHE_SIZE	4

class CDPLoginSrvr : public CDPMng,
	public DPMngFeatures::BroadcastPacketNone<CDPLoginSrvr>
{
private:
	CACHESTRUCT		m_aCache[MAX_CACHE_SIZE];
	int		m_nSizeOfCache;
public:
	void	AddCahce( const char* lpCacheAddr );
	int		CacheIn( void );	// cache addr
	void	CacheOut( int nIndexOfCache );
	void	SendCacheAddr( int nIndexOfCache, DPID dpid );

public:
//	Constructions
	CDPLoginSrvr();
	virtual	~CDPLoginSrvr();
	
//	Operations
	virtual void	SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
	virtual void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );

	void	SendError( LONG lError, DPID dpid );
	void	SendHdr( DWORD dwHdr, DPID dpid );

	USES_PFNENTRIES;
//	Handlers
	void	OnAddConnection( CAr & ar, DPID dpid );
	void	OnRemoveConnection( DPID dpid );
	void	OnPreJoin( CAr & ar, DPID dpid );
	void	OnPing( CAr & ar, DPID dpid );
	void	OnQueryTickCount( CAr & ar, DPID dpid );

	void	SendNumPadId( DPID dpid );
public:
	void	SendLoginProtect( BOOL bLogin, DPID dpid );
};

extern CDPLoginSrvr g_dpLoginSrvr;
