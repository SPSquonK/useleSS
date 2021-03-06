#pragma once 

#include "WndSqKComponents.h"

class CWndAwakening : public CWndNeuz {
public:
	class CAwakenableItemReceiver : public CWndItemReceiver {
	public:
		bool CanReceiveItem(const CItemElem & itemElem, bool verbose) override;
	};

private:
	static constexpr UINT WIDC_Receiver = 900;
	CAwakenableItemReceiver m_receiver;

public:
	BOOL Initialize(CWndBase * pWndParent, DWORD) override;
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
	void OnInitialUpdate() override;
}; 

class CWndSelectAwakeCase : public CWndNeuz {
public:
	static constexpr DWORD AWAKE_KEEP_TIME = SEC(60);

	CWndSelectAwakeCase(BYTE byObjID, DWORD dwSerialNum, __int64 n64NewOption);

	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD nType = MB_OK ); 
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
	virtual void OnDraw( C2DRender* p2DRender ); 
	virtual	void OnInitialUpdate(); 

	virtual HRESULT RestoreDeviceObjects();
	virtual HRESULT InvalidateDeviceObjects();
	virtual HRESULT DeleteDeviceObjects();

private:
	void OutputOptionString(C2DRender * p2DRender, CItemElem * pItemElem, BOOL bNew = FALSE);

	DWORD m_dwOldTime;
	DWORD m_dwDeltaTime = 0;

	DWORD m_dwItemIndex = 0;
	CTexture*  m_pTexture = nullptr;

	BYTE m_byObjID = 0;
	DWORD m_dwSerialNum = 0;
	__int64 m_n64NewOption = 0;

	CTexture* m_pTexGuage = nullptr;
	LPDIRECT3DVERTEXBUFFER9 m_pVertexBufferGauge = nullptr;
};
