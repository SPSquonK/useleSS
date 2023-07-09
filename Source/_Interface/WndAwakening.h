#pragma once 

#include "WndSqKComponents.h"

class CWndAwakening final : public CWndNeuz {
public:
	class CAwakenableItemReceiver : public CWndItemReceiver {
	public:
		bool CanReceiveItem(const CItemElem & itemElem, bool verbose) override;
	};

private:
	static constexpr UINT WIDC_Receiver = 900;
	CAwakenableItemReceiver m_receiver;

public:
	BOOL Initialize(CWndBase * pWndParent = nullptr);
	BOOL OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) override;
	void OnInitialUpdate() override;
}; 

class CWndSelectAwakeCase final : public CWndNeuz {
public:
	static constexpr DWORD AWAKE_KEEP_TIME = SEC(60);

	CWndSelectAwakeCase(BYTE byObjID, DWORD dwSerialNum, __int64 n64NewOption);

	BOOL Initialize( CWndBase* pWndParent = nullptr ); 
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
