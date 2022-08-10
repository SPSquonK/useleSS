#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndSealChar.h"
#include "MsgHdr.h"
#include "DPClient.h"

///////////////////////////////////////////////////////////////////////////////

void CWndSealChar::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	
	CWndText* pWndText = GetDlgItem<CWndText>( WIDC_TEXT1 );
	LPCTSTR strTemp = prj.GetText(TID_DIAG_SEAL_CHAR_TEXT01); // 메시지 바꾸려면 이걸 바꾸시오
	pWndText->SetString( strTemp );

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( ( rectRoot.right - rectWindow.Width() ) / 2, 70 );
	Move( point );	
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndSealChar::Initialize(CWndBase * pWndParent, DWORD) {
	return CWndNeuz::InitDialog(APP_SEAL_CHAR, pWndParent, 0, CPoint(0, 0));
}

BOOL CWndSealChar::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	BOOL bResult = FALSE;
	if( nID == WIDC_OK ) 
	{
		g_DPlay.SendSealChar(  );		// 케릭봉인신청
		bResult = TRUE;
	}
	// 반드시!!! TRUE로 해서 메모리에서 날리자!!
	if( bResult )
		Destroy( /*TRUE*/ );
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 


CWndSealCharSelect::CWndSealCharSelect() 
{ 
	memset( m_szSrc1, 0, sizeof(m_szSrc1) );
	m_idSrc1 = NULL_ID;
	memset( m_szSrc2, 0, sizeof(m_szSrc2) );
	m_idSrc2 = NULL_ID;
	m_lPlayerSlot1 = -1;
	m_lPlayerSlot2 = -1;
	m_sCount = 0;
}

void CWndSealCharSelect::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );

	LPCTSTR strTemp = _T(prj.GetText(TID_DIAG_SEAL_CHAR_SELECT_TEXT01)); // 메시지 바꾸려면 이걸 바꾸시오
	pWndText->SetString( strTemp );

	CWndButton* pWndButton[ 2 ];
	
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_RADIO1 );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_RADIO2 );

	pWndButton[ 0 ]->SetGroup( TRUE );
	pWndButton[ 0 ]->SetCheck( TRUE );



	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( ( rectRoot.right - rectWindow.Width() ) / 2, 70 );
	Move( point );	
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndSealCharSelect::Initialize(CWndBase * pWndParent, DWORD) {
	return CWndNeuz::InitDialog(APP_SEAL_CHAR_SELECT, pWndParent, 0, CPoint(0, 0));
}

BOOL CWndSealCharSelect::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	BOOL bResult = FALSE;
	if( nID == WIDC_OK ) 
	{
		UINT nRadios[2] = { WIDC_RADIO1, WIDC_RADIO2 };
		int i = NULL;
		for( ; i<2; ++i )
		{
			CWndButton* pButton = (CWndButton* )GetDlgItem( nRadios[i] );
			if( pButton->GetCheck() )
				break;
		}

		if( (( i == 0 ) && (m_idSrc1 < 1 ) ) || (( i == 1 ) && (m_idSrc2 < 1 ) ) )
			bResult = FALSE;
		else
		{
			CWndSealCharSend* pWndCWndSealCharSend = (CWndSealCharSend*)g_WndMng.GetWndBase( APP_SEAL_CHAR_SEND );
			if( !pWndCWndSealCharSend )
			{
				pWndCWndSealCharSend = new CWndSealCharSend;
				pWndCWndSealCharSend->Initialize( &g_WndMng );
			}

			if(i == 0)
				pWndCWndSealCharSend->SetData(m_idSrc1,m_szSrc1);
			else if(i == 1)
				pWndCWndSealCharSend->SetData(m_idSrc2,m_szSrc2);

			bResult = TRUE;
		}
	}
	// 반드시!!! TRUE로 해서 메모리에서 날리자!!
	if( bResult )
		Destroy( /*TRUE*/ );
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndSealCharSelect::SetData( short sCount,LONG lPlayerSolt1,LONG lPlayerSolt2,u_long uPlayerID1,u_long uPlayerID2, char* szName1, char* szName2 )
{
	strcpy( m_szSrc1, szName1 );
	strcpy( m_szSrc2, szName2 );

	m_idSrc1 = uPlayerID1;
	m_idSrc2 = uPlayerID2;
	m_lPlayerSlot1 = lPlayerSolt1;
	m_lPlayerSlot2 = lPlayerSolt2;
	m_sCount = sCount;

	CWndButton* pWndButton[ 2 ];
	
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_RADIO1 );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_RADIO2 );

	pWndButton[ 0 ]->SetTitle( m_szSrc1 );
	pWndButton[ 1 ]->SetTitle( m_szSrc2 );
}


CWndSealCharSend::CWndSealCharSend() 
{ 
	memset( m_szSrc1, 0, sizeof(m_szSrc1) );
	m_idSrc1 = NULL_ID;
} 

void CWndSealCharSend::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	
	CWndText* pWndText = GetDlgItem<CWndText>( WIDC_TEXT1 );
	CString strTemp;

	strTemp.Format( _T( prj.GetText(TID_DIAG_SEAL_CHAR_SEND_TEXT01) ), m_szSrc1); // 메시지 바꾸려면 이걸 바꾸시오
	pWndText->SetString( strTemp );


	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( ( rectRoot.right - rectWindow.Width() ) / 2, 70 );
	Move( point );	
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndSealCharSend::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( APP_SEAL_CHAR_SEND, pWndParent, 0, CPoint( 0, 0 ) );
}

BOOL CWndSealCharSend::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	BOOL bResult = TRUE;
	if( nID == WIDC_YES ) 
	{

		g_DPlay.SendSealCharConm( m_idSrc1 );		// 케릭봉인신청
		bResult = TRUE;
	}
	// 반드시!!! TRUE로 해서 메모리에서 날리자!!
	if( bResult )
		Destroy( /*TRUE*/ );
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndSealCharSend::SetData( u_long uPlayerID1, char* szName1 )
{
	strcpy( m_szSrc1, szName1 );

	m_idSrc1 = uPlayerID1;
	CWndText* pWndText = GetDlgItem<CWndText>( WIDC_TEXT1 );

	CString strTemp;
	strTemp.Format( _T( prj.GetText(TID_DIAG_SEAL_CHAR_SEND_TEXT01) ), m_szSrc1); // 메시지 바꾸려면 이걸 바꾸시오
	pWndText->SetString( strTemp );
}


///////////////////////////////////////////////////////////////////////////////

void CWndSealCharSet::OpenOrResetWindow(DWORD scrollPosition) {
	if (!g_WndMng.m_pWndSealCharSet) {
		g_WndMng.m_pWndSealCharSet = new CWndSealCharSet;
		g_WndMng.m_pWndSealCharSet->Initialize(&g_WndMng);
	}

	g_WndMng.m_pWndSealCharSet->m_scrollPos = scrollPosition;
}

void CWndSealCharSet::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();

	CWndText * pWndText = GetDlgItem<CWndText>(WIDC_TEXT1);
	LPCTSTR strTemp = _T(prj.GetText(TID_DIAG_SEAL_CHAR_SET_TEXT01));
	pWndText->SetString(strTemp);

	Move70();
}

BOOL CWndSealCharSet::Initialize(CWndBase * pWndParent, DWORD) {
	return CWndNeuz::InitDialog(APP_SEAL_CHAR_SET, pWndParent, 0, CPoint(0, 0));
}

BOOL CWndSealCharSet::OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) {
	if (nID == WIDC_YES) {
		g_DPlay.SendPacket<PACKETTYPE_SEALCHARGET_REQ, DWORD>(m_scrollPos);
		Destroy();
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}
