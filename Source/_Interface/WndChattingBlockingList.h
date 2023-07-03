#ifndef __WND_CHATTING_BLOCKING_LIST_H__
#define __WND_CHATTING_BLOCKING_LIST_H__

#ifdef __YS_CHATTING_BLOCKING_SYSTEM
#ifdef __CLIENT

class CWndChattingBlockingList : public CWndNeuz
{
public:
	CWndChattingBlockingList( void );
	~CWndChattingBlockingList( void );

public:
	BOOL Initialize( CWndBase* pWndParent = nullptr ) override;
	virtual	void OnInitialUpdate( void );

public:
	void UpdateInformation( void );
};

#endif // __CLIENT
#endif // __YS_CHATTING_BLOCKING_SYSTEM

#endif // __WND_CHATTING_BLOCKING_LIST_H__