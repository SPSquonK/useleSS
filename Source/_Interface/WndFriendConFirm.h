#pragma once

class CWndFriendConFirm final : public CWndNeuz {
public:
	u_long m_uLeader = 0;
	TCHAR m_szLeaderName[MAX_NAME] = _T("");

public:
	void SetMember(u_long uLeader, const char * szLeadName);

	BOOL Initialize( CWndBase* pWndParent = nullptr );
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
	void OnDraw(C2DRender * p2DRender) override;
	void OnInitialUpdate() override;
};

class CWndAddFriend final : public CWndNeuz 
{ 
public: 
	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 

private:
	void OnOk();
}; 
