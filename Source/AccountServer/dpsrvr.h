#pragma once

#include "dpmng.h"
#include "msghdr.h"
#include "misc.h"
#include <map>

#undef	theClass
#define theClass	CDPSrvr_AccToCert
#undef theParameters
#define theParameters	CAr & ar, DPID, DPID

typedef std::map<std::string, int>	STRING2INT;

class CDPSrvr_AccToCert : public CDPMng<CBuffer>
{
public:
	static constexpr size_t MAX_IP = 10240;

	bool	m_bCheckAddr;		// 접속하는 account의 address를 검사해야 하는가?
	int		m_nMaxConn;
	BOOL	m_bReloadPro;
	CMclCritSec		m_csAddrPmttd;
	char	m_sAddrPmttd[128][16];
	int		m_nSizeofAddrPmttd;
	

	CMclCritSec		m_csIPCut;
	STRING2INT m_sIPCut;
	int		m_nIPCut[MAX_IP][3];
	int		m_nSizeofIPCut;

	DWORD	m_dwSizeofServerset;
	SERVER_DESC		m_aServerset[128];
	std::map<u_long, SERVER_DESC *>	m_2ServersetPtr;
	char	m_szVer[32];
#ifdef __SECURITY_0628
	char	m_szResVer[100];
#endif	// __SECURITY_0628

public:
//	Constructions
	CDPSrvr_AccToCert();
//	Overrides
	virtual void	SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
	virtual void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
// Operations
	void	CloseExistingConnection( LPCTSTR lpszAccount, LONG lError );
	bool	LoadAddrPmttd( LPCTSTR lpszFileName );
	BOOL	LoadIPCut( LPCSTR lpszFileName );
	void	DestroyPlayer( DPID dpid1, DPID dpid2 );

	void	OnAddConnection( DPID dpid );
	void	OnRemoveConnection( DPID dpid );

	void	SendServersetList( DPID dpid );
	void	SendPlayerCount( u_long uId, long lCount );

	BOOL	EnableServer( DWORD dwParent, DWORD dwID, long lEnable );
	void	SendEnableServer( u_long uId, long lEnable );
	
	struct OnAfterCheckingParams {
		BYTE f;
		LPCTSTR lpszAccount;
		DWORD dwAuthKey;
		BYTE cbAccountFlag;
		long lTimeSpan;
#ifdef __GPAUTH_02
		const char * szCheck;
#ifdef __EUROPE_0514
		const char * szBak;
#endif
#endif
	};

	void OnAfterChecking(DPID dpid1, DPID dpid2, const OnAfterCheckingParams & params);
	BOOL	IsABClass( LPCSTR lpszIP );
	void	GetABCClasstoString( LPCSTR lpszIP, char * lpABClass, int &nCClass );
	void	InitIPCut( void );
//	Handlers
	USES_PFNENTRIES;
	void	OnAddAccount( CAr & ar, DPID dpid1, DPID dpid2 );
	void	OnRemoveAccount( CAr & ar, DPID dpid1, DPID dpid2 );
	void	OnPing( CAr & ar, DPID dpid1, DPID dpid2 );
	void	OnCloseExistingConnection( CAr & ar, DPID dpid1, DPID dpid2 );
};

extern CDPSrvr_AccToCert g_dpSrvr;

