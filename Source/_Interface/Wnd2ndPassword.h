#ifndef __WND_2ND_PASSWORD__
#define __WND_2ND_PASSWORD__

#if defined( __CLIENT )
class CWnd2ndPassword final : public CWndNeuz
{
public:
	CWnd2ndPassword( void );
	~CWnd2ndPassword( void );

public:
	BOOL Initialize( CWndBase* pWndParent = nullptr );
	virtual	void OnInitialUpdate( void );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult );

public:
	void SetInformation( u_long idNumberPad, int nSelectCharacter = -1 );
	void InsertPassword( int nPasswordNumber );
	void DeletePassword( void );
	void ResetNumberpad( u_long idNumberPad );

private:
	enum { TABLE_NUMBER_X = 10, TABLE_NUMBER_Y = 1000 };

private:
	int m_nSelectCharacter;
};
#endif // defined( __IMPROVE_QUEST_INTERFACE ) && defined( __CLIENT )

#endif // __WND_2ND_PASSWORD__



















