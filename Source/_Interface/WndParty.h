#pragma once

class CWndParty;

class CWndPartyQuick : public CWndNeuz {
public:
	static constexpr size_t MaxPartyMember = 8;

	CWndButton * m_pBtnParty = nullptr;
	std::array<CWndStatic *, MaxPartyMember> m_pWndMemberStatic;
	CTexture m_texGauEmptyNormal;
	CTexture m_texGauFillNormal;

	LPDIRECT3DVERTEXBUFFER9 m_pVBGauge = nullptr;

	size_t m_MemberCount = 0;
	CWndParty * m_pWndParty = nullptr;
	CMover * m_pFocusMember = nullptr;
public:

	CWndPartyQuick();
	~CWndPartyQuick();

	virtual BOOL Initialize(CWndBase * pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult);
	virtual void OnDraw(C2DRender * p2DRender);
	virtual	void OnInitialUpdate();
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();

	void SetActiveMember(size_t MemberNum);

	static void ReduceSize(CString & str, int length);
	static CString FormatPlayerName(int level, DWORD dwJob, const char * name);
};

class CWndPartyConfirm : public CWndNeuz {
public:
	u_long m_uLeader = 0;
	TCHAR m_szLeaderName[MAX_NAME];

	CWndPartyConfirm(u_long uLeader, const TCHAR * szLeaderName);

	virtual BOOL Initialize(CWndBase * pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult);
	virtual	void OnInitialUpdate();
};

class CWndPartyChangeName : public CWndNeuz {
public:
	TCHAR	m_sParty[33];

	virtual BOOL Initialize(CWndBase * pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult);
	virtual	void OnInitialUpdate();

private:
	void OnSendName();
};

class CWndPartyChangeTroup : public CWndNeuz {
public:
	CWndPartyChangeName * m_WndPartyChangeName = nullptr;
	~CWndPartyChangeTroup();

	virtual BOOL Initialize(CWndBase * pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult);
	virtual	void OnInitialUpdate();
};

class CWndPartyLeaveConfirm : public CWndNeuz {
public:
	u_long uLeaveId = 0;

	void SetLeaveId(u_long uidPlayer);
public:
	virtual BOOL Initialize(CWndBase * pWndParent = NULL, DWORD nType = MB_OK);
	virtual BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult);
	virtual	void OnInitialUpdate();
};

class CWndPartyInfo : public CWndNeuz 
{ 
public:
	struct PlayerInfo {
		const char * name;
		int level;
		DWORD levelMode;
		int job;
		LPCTSTR jobName;
	};

public: 
	int m_nSelected = -1; // 현재 파티창에서 선택된 놈 인덱스 (위에서부터 zero base)
	~CWndPartyInfo(); 

	CTexture m_texGauEmptyNormal ;
	CTexture m_texGauFillNormal  ;
	LPDIRECT3DVERTEXBUFFER9 m_pVBGauge = nullptr;
	
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();
	
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 

private:

	static PlayerInfo GetPlayerInfo(u_long playerId, CMover * pObjMember);
}; 

class CWndPartySkill : public CWndNeuz 
{ 
public: 
	std::array<CTexture *, 10> m_atexSkill;
	int m_nSkillSelect = -1;
	
	CWndPartySkill(); 

	virtual void OnMouseWndSurface( CPoint point );
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDblClk( UINT nFlags, CPoint point);

private:
	int GetCurrentlyHoveredSkill(CPoint point) /* const */;
}; 

class CWndParty : public CWndNeuz 
{ 
public:
	CWndButton* m_pWndLeave          = nullptr;
	CWndButton* m_pWndChange         = nullptr;
	CWndButton* m_pWndTransfer       = nullptr;
	CWndButton* m_pBtnPartyQuick     = nullptr;
	CWndPartyQuick* m_pWndPartyQuick = nullptr;
	
public: 
	CWndPartyInfo  m_wndPartyInfo;
	CWndPartySkill m_wndPartySkill;
	CWndPartyChangeTroup* m_WndPartyChangeTroup = nullptr;
	
	~CWndParty(); 

	virtual void SerializeRegInfo( CAr& ar, DWORD& dwVersion );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 

private:
	void OnLeave();
	void OnChangeLeader();
}; 
