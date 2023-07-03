#pragma once

class CWndSelectCh final : public CWndNeuz {
private:
	int m_nItemId;
	unsigned int m_nChCount;

public: 
	CWndSelectCh(int nItemId, unsigned int nChCount); 

	BOOL Initialize( CWndBase* pWndParent = nullptr ) override; 
	BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) override;
	void OnInitialUpdate() override;
};
