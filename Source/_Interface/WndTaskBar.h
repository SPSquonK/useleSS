#pragma once

#include "Taskbar.h"

//////////////////////////////////////////////////////////////////////////////////////
// 작업 윈도 
// 메뉴 버튼, 명령 아이콘, 단축 아이콘, 활성화 기능 등등
//
class CWndTaskBar : public CWndNeuz, public CTaskbar
{
	DWORD m_dwHighAlpha;

public:
	CWndMenu       m_menuShortcut;
	CTexturePack   m_texPack;
	CWndButton     m_wndMenu;
	CWndButton     m_wndQuickList;
	CWndButton     m_wndQuickListUp;
	CWndButton     m_wndQuickListDn;

	std::array<SHORTCUT, MAX_SLOT_ITEM> & m_paSlotItem();
	const std::array<SHORTCUT, MAX_SLOT_ITEM> & m_paSlotItem() const;
	LPSHORTCUT     m_pSelectShortcut;
	SHORTCUT       m_aSlotSkill;
	CTexture*      m_pTexture;
	int            m_nMaxSlotApplet;
	BOOL           m_bStartTimeBar;
	int            m_nUsedSkillQueue;
	int            m_nPosition;
	int            m_nSkillBar;
	int            m_nCurQueueNum;
	int            m_nCurQueue;
	CTimer         m_timer;
	int			   m_nExecute;		// 0: 실행중이지 않음 1:실행대기중(스킬쓰러 달려가는중) 2:실행중.
	OBJID		   m_idTarget;		// 스킬사용대상.
	int            m_nSlotIndex;
	
	void RenderLordCollTime( CPoint pt, DWORD dwSkillId, C2DRender* p2DRender );
	void RenderCollTime( CPoint pt, DWORD dwSkillId, C2DRender* p2DRender );
	void UpdateItem();
	void PutTooTip( LPSHORTCUT pShortcut, CPoint point, CRect* pRect );
	void SetItemSlot( int nSlot );
	BOOL CheckAddSkill( int nSkillStyleSrc, int nQueueDest );
	BOOL UseSkillQueue( CCtrl* pTargetObj );
	LPSKILL GetCurrentSkillQueue();
	BOOL SetSkillQueue( int nIndex, DWORD dwType, DWORD dwId, CTexture* pTexture = NULL );
	BOOL SetShortcut( int nIndex, ShortcutType dwShortcut, DWORD dwType, DWORD dwId, CTexture* pTexture = NULL, int nWhere = 0 );
	void OnEndSkill( void );
	void OnCancelSkill( void );
	LPSHORTCUT Select( CPoint point );
	void RemoveSkillQueue( int nIndex, BOOL bSend = TRUE );
	void UpdateAllTaskbarTexture();
	

	CWndTaskBar();
//	virtual CItem* GetFocusItem() { return NULL; }
	virtual void OnDraw(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	// message
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase );
	virtual void OnSize(UINT nType, int cx, int cy);
	virtual void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void OnRButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnDropIcon( LPSHORTCUT pShortcut, CPoint point = 0 );
	virtual BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);
	void PaintFrame(C2DRender * p2DRender) override { CWndBase::PaintFrame(p2DRender); }
	BOOL OnEraseBkgnd(C2DRender * p2DRender) override { return TRUE; }
	virtual BOOL Process();
	virtual void OnMouseWndSurface( CPoint point );
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	
	HRESULT	RestoreDeviceObjects();
	HRESULT InvalidateDeviceObjects();
	HRESULT DeleteDeviceObjects();
protected:
	void RenderOutLineLamp(int x, int y, int num, DWORD size);
	bool RenderShortcut(C2DRender * p2DRender, const SHORTCUT & shortcut, CPoint point, bool drawLetter);

	void FindNewStackForShortcut(std::optional<int> where, int i);
	void SetTaskBarTexture(SHORTCUT & shortcut);
};
//////////////////////////////////////////////////////////////////////////////////////
// 태스크바의 매뉴 
//
class CWndTaskMenu : public CWndMenu //Neuz
{
public:
	// static void SetTexture( CWndButton* pWndButton );
//	virtual CItem* GetFocusItem() { return NULL; }
	static CWndButton * MakeButton(CWndMenu * pWndMenu, UINT nFlags, UINT nIDNewItem, LPCTSTR lpszNewItem);
	virtual void OnDraw(C2DRender* p2DRender);
	virtual void PaintFrame(C2DRender* p2DRender);
	virtual BOOL OnEraseBkgnd(C2DRender* p2DRender);
	virtual	void OnInitialUpdate();
	virtual BOOL Initialize(CWndBase* pWndParent = NULL,DWORD dwWndId = 0);
	// message
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase = NULL );
	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	virtual void OnLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnChildNotify(UINT message,UINT nID,LRESULT* pLResult);
	virtual BOOL Process();
	virtual void OnKillFocus(CWndBase* pNewWnd);
private:
	class FolderAdder {
	private:
		CWndMenu * m_self;
	public:
		FolderAdder(CWndMenu * self) : m_self(self) {}
		
		FolderAdder AddApplet(DWORD appId, DWORD textId) const {
			CWndTaskMenu::MakeButton(m_self, 0, appId, prj.GetText(textId));
			return *this;
		}
	};

	void AddApplet(DWORD appId, DWORD textId);
	FolderAdder AddFolder(DWORD textId);

	std::vector<std::unique_ptr<CWndMenu>> m_menus;

};

