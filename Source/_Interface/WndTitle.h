#pragma once

DECLARE_WNDMESSAGEBOX( CWndConnectingBox ) 
DECLARE_WNDMESSAGEBOX( CWndCharBlockBox )
DECLARE_WNDMESSAGEBOX( CWndAllCharBlockBox )

#ifdef __NPKCRYPT
#include "npkcrypt.h"
#endif	// __NPKCRYPT
#include "Wnd2ndPassword.h"

class CWndLogin : public CWndNeuz
{
	//CTexture   m_Texture;
public:
#ifdef __NPKCRYPT
	HKCRYPT		m_hKCrypt;
#endif	// __NPKCRYPT

	BOOL m_bDisconnect;

	CWndEdit   m_wndAccount     ;
	CWndEdit   m_wndPassword    ;
	CWndButton m_wndSaveAccount ;
	CWndButton m_wndSavePassword;
	CWndButton m_wndFindAccount ;
	
	CWndButton m_wndLogin    ;
	CWndButton m_wndRegist   ;
	CWndButton m_wndQuit     ;

	void Connected( long lTimeSpan );

	CWndLogin();
	virtual ~CWndLogin();
	virtual	void OnInitialUpdate();
	BOOL Initialize(CWndBase * pWndParent = nullptr) override;
	virtual BOOL Process ();
	// message
	virtual BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);

#ifdef __CON_AUTO_LOGIN
	void SetAccountAndPassword( const CString& account, const CString& pass );
#endif
};

#include "ListedServer.h"
class CWndSelectServer : public CWndNeuz {
public:
	std::vector<CString> m_vecStrBanner;
	DWORD		 m_dwChangeBannerTime;
	LPIMAGE		 m_atexPannel;

	CWndButton   m_wndBack;
	CWndButton   m_wndAccept;

	CWndSelectServer();
	~CWndSelectServer() override;
	void OnInitialUpdate() override;
	BOOL Initialize( CWndBase* pWndParent = nullptr ) override;
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
	static void DisplayChannels(CWndListBox & listBox, const std::span<CListedServers::Channel> & channels);

	BOOL Process() override;
	void AfterSkinTexture(LPWORD pDest, CSize size, D3DFORMAT d3dFormat = D3DFMT_A4R4G4B4);
	void Connected();
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) override;

};


class CWndDeleteChar : public CWndNeuz {
public:
	BOOL Initialize( CWndBase* pWndParent = nullptr ) override;
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
	void DeletePlayer(int nSelect, LPCTSTR szNo);
	virtual void AdditionalSkinTexture(LPWORD pDest, CSize sizeSurface, D3DFORMAT d3dFormat = D3DFMT_A4R4G4B4);
};


class CWndSelectChar : public CWndNeuz {
public:
	CWndText m_wndText1;
	CWndText m_wndText2;
	CWndText m_wndText3;

	CWndDeleteChar * m_pWndDeleteChar;
	CWnd2ndPassword * m_pWnd2ndPassword;
	static int m_nSelectCharacter;
	CRect m_aRect[MAX_CHARACTER_LIST];
	DWORD m_dwMotion[MAX_CHARACTER_LIST];
	std::unique_ptr<CModelObject> m_pBipedMesh[MAX_CHARACTER_LIST];
	BOOL m_bDisconnect;

private:
	BOOL m_CreateApply; //서버통합 관련 특정 기간 캐릭터 생성 금지.

public:
	CWndSelectChar();
	~CWndSelectChar() override;

	void DeleteCharacter();
	void UpdateCharacter();
	void Connected();
	void OnDestroyChildWnd(CWndBase * pWndChild);

	void SelectCharacter(int i);

	BOOL Process() override;
	void OnDraw(C2DRender * p2DRender) override;
	void OnInitialUpdate() override;
	BOOL Initialize( CWndBase* pWndParent = nullptr ) override;
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
	void OnLButtonUp(UINT nFlags, CPoint point) override;

	HRESULT InitDeviceObjects() override;
	HRESULT RestoreDeviceObjects() override;
	HRESULT InvalidateDeviceObjects() override;
	HRESULT DeleteDeviceObjects() override;
	void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) override;

private:
	BOOL SetMotion(CModelObject * pModel, DWORD dwMotion, int nLoop, DWORD dwOption);
};


class CWndCreateChar : public CWndNeuz {
public:
	CWndButton   m_wndAccept;
	CModelObject * m_pModel;
	PLAYER m_Player;

	CWndCreateChar();
	~CWndCreateChar() override;
	void OnDraw(C2DRender * p2DRender) override;
	void OnInitialUpdate() override;
	BOOL Initialize( CWndBase* pWndParent = nullptr ) override;
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;

	void SetSex(int nSex);
	void Connected();
	virtual HRESULT InitDeviceObjects();
	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();

	enum { MAX_2ND_PASSWORD_NUMBER = 4 };
};
