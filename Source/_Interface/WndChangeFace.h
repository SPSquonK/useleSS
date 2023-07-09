#pragma once

#include "WndSqKComponents.h"

class CWndChangeFace final : public CWndNeuz 
{ 
public: 
	int m_nSelectFace;
	CWndChangeFace(); 
	~CWndChangeFace(); 

	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 
	virtual BOOL OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ); 
	virtual void OnSize( UINT nType, int cx, int cy ); 
	virtual void OnLButtonUp( UINT nFlags, CPoint point ); 
	virtual void OnLButtonDown( UINT nFlags, CPoint point ); 
}; 

class CWndChangeSex final : public CWndNeuz 
{ 
private:
	DWORD	m_dwItemId;
	DWORD	m_dwObjId;
public:
	std::unique_ptr<CModelObject> m_pModel;
	void	SetData( DWORD dwItemId, DWORD dwObjId )
	{	m_dwItemId	= dwItemId;	m_dwObjId	= dwObjId;	}
	
public: 
	int m_nSelectFace;
	CWndChangeSex(); 

	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 

private:
	void UpdateModelParts();
}; 

class CWndItemTransy final : public CWndNeuz 
{ 
public:
	static constexpr UINT WIDC_Receiver = 501;

	class CWndEquipementReceiver : public CWndItemReceiver {
	public:
		bool CanReceiveItem(const CItemElem & itemElem, bool verbose) override;
	};

	CWndEquipementReceiver m_itemReceiver;
	CWndItemDisplayer      m_itemDisplayer;
	CItemElem * m_scroll = nullptr;

	void Init( CItemElem* pItemElem );

	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual	void OnInitialUpdate(); 
	virtual void OnDestroy( void );
}; 
