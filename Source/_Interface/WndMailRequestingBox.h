#pragma once

#ifdef __CLIENT
class CWndMailRequestingBox final : public CWndNeuz
{
public:
	enum { REQUEST_BUTTON_WAITING_TIME = 5 };
	enum { DESTRUCTION_TIME = 3 };

public:
	CWndMailRequestingBox( void );
	~CWndMailRequestingBox( void );

public:
	BOOL Initialize( CWndBase* pWndParent = nullptr );
	virtual	void OnInitialUpdate( void );
	BOOL Process( void );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );

private:
	DWORD m_timeWaitingRequest;
	int m_nDestroyCounter;
};
#endif // __CLIENT

