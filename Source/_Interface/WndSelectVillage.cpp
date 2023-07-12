#include "stdafx.h"
#include "resData.h"
#include "WndSelectVillage.h"
#include "dpclient.h"
#include "definetext.h"




///////////////////////////////////////////////////////////////////////////
// CReturnScrollMsgBox
//////////////////////////////////////////////////////////////////////////////


BOOL CReturnScrollMsgBox::Initialize( CWndBase* pWndParent )
{
	// 아이템 사용 후 4시간 안에 저장된 위치로 돌아오지 않으면 아이템의 효력은 
	//사라집니다. 아이템을 사용하시겠습니까?
	return CWndMessageBox::Initialize(
		prj.GetText(TID_GAME_RETURN_USEITEM),
		pWndParent,
		MB_OKCANCEL
	);
}

BOOL CReturnScrollMsgBox::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
 	switch( nID )
	{
	case IDOK:
		{
			SAFE_DELETE( g_WndMng.m_pWndSelectVillage );
			g_WndMng.m_pWndSelectVillage = new CWndSelectVillage;
			g_WndMng.m_pWndSelectVillage->Initialize();
			Destroy();
		}
		break;
	case IDCANCEL:
		Destroy();
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult );  //??
} 


/*
  WndId : APP_COMM_SELECTVIL
  CtrlId : WIDC_STATIC1
  CtrlId : WIDC_OK 
  CtrlId : WIDC_CANCEL 
  CtrlId : WIDC_EDIT_CHANGENAME 
*/

void CWndSelectVillage::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CWndButton* pWndButton[ 3 ];
	
	pWndButton[ 0 ] = (CWndButton*)GetDlgItem( WIDC_RADIO_FLARIS );
	pWndButton[ 1 ] = (CWndButton*)GetDlgItem( WIDC_RADIO_SAINTM );
	pWndButton[ 2 ] = (CWndButton*)GetDlgItem( WIDC_RADIO_DARKON );
	pWndButton[ 0 ]->SetGroup( TRUE );
	pWndButton[ 0 ]->SetCheck( TRUE );

	MoveParentCenter();
} 

BOOL CWndSelectVillage::Initialize( CWndBase* pWndParent )
{ 
	return CWndNeuz::InitDialog( APP_COMM_SELECTVIL, pWndParent, 0, CPoint( 0, 0 ) );
} 

BOOL CWndSelectVillage::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) {
	switch (nID) {
		case WIDC_BUTTON_OK: {
			using Choice = std::pair<int, UINT>;
			static constexpr auto choices = {
				Choice(0, WIDC_RADIO_FLARIS),
				Choice(1, WIDC_RADIO_SAINTM),
				Choice(2, WIDC_RADIO_DARKON)
			};

			for (const auto [i, widgetId] : choices) {
				CWndButton * pButton = GetDlgItem<CWndButton>(widgetId);
				if (pButton->GetCheck()) {
					g_DPlay.SendReturnScroll(i);
					break;
				}
			}

			Destroy();
			break;
		}
		case WIDC_BUTTON_CANCEL: {
			Destroy();
			break;
		}
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

