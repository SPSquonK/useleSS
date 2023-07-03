#ifndef __WND_QUEST_QUICK_INFO_H__
#define __WND_QUEST_QUICK_INFO_H__

#if defined( __CLIENT )
//-----------------------------------------------------------------------------
class CWndQITreeCtrl : public CWndTreeCtrl
{
public:
	CWndQITreeCtrl( void );
	virtual ~CWndQITreeCtrl( void );

public:
	void OnLButtonDown(UINT nFlags, CPoint point) override;
	void OnRButtonDown(UINT nFlags, CPoint point) override;
	void PaintTree(C2DRender * p2DRender, CPoint & pt, TreeElems & ptrArray) override;
};
//-----------------------------------------------------------------------------
class CWndQuestQuickInfo : public CWndNeuz
{
public:
	CWndQuestQuickInfo( void );
	virtual ~CWndQuestQuickInfo( void );

public:
	BOOL Initialize( CWndBase* pWndParent = nullptr ) override;
	virtual	void OnInitialUpdate( void );
	virtual BOOL Process( void );
	virtual	void PaintFrame( C2DRender* p2DRender );
	virtual	void SetWndRect( CRect rectWnd, BOOL bOnSize = TRUE );
	virtual void OnSize( UINT nType, int cx, int cy );

public:
	void SetUpdateTextSwitch( BOOL bUpdateText );
	void ExtendAppletHeight( void );

public:
	BOOL GetActiveGroundClick( void ) const;

private:
	void DeleteTree( void );

private:
	int m_nTreeInformationListSize;
	BOOL m_bOnSurface;
	BOOL m_bActiveGroundClick;
	DWORD m_tmOld;
	BOOL m_bUpdateText;
	CWndQITreeCtrl m_CWndQITreeCtrl;
};
//-----------------------------------------------------------------------------
#endif // defined( __IMPROVE_QUEST_INTERFACE ) && defined( __CLIENT )

#endif // __WND_QUEST_QUICK_INFO_H__