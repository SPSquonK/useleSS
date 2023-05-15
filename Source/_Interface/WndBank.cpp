#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndBank.h"

#include "WndManager.h"
#include "DPClient.h"
#include "MsgHdr.h"


/****************************************************
  WndId : APP_BANK - BANK
  CtrlId : WIDC_INVENTORY - 
  CtrlId : WIDC_COST - 0
****************************************************/

CWndBank::~CWndBank() {
	g_DPlay.SendCloseBankWnd();
	SAFE_DELETE( g_WndMng.m_pWndTradeGold );
} 

void CWndBank::OnDraw( C2DRender* p2DRender ) 
{ 
	static constexpr int nIndex[3] = { WIDC_CUSTOM4, WIDC_CUSTOM5, WIDC_CUSTOM6 };
	CString str;
	
	for (int i = 0 ; i < 3 ; ++i) {
		str.Format(_T("%d"), g_pPlayer->m_dwGoldBank[i]);
		m_slots[i].pCost->SetTitle( str );
		
		if (!m_slots[i].bUse) {
			LPWNDCTRL lpWndCtrl = GetWndCtrl( nIndex[i] );
			m_pTexture->Render( p2DRender, lpWndCtrl->rect.TopLeft(), 50 );
		}
	}
} 

void CWndBank::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	if(GetWndBase( APP_BAG_EX )) GetWndBase( APP_BAG_EX )->Destroy();
	if( g_WndMng.m_pWndTrade || g_WndMng.m_pWndShop || g_WndMng.m_pWndGuildBank || g_WndMng.GetWndVendorBase() )
	{
		Destroy();
		return;
	}

	static constexpr int WIDCBanks[3] = { WIDC_BANK_0, WIDC_BANK_1, WIDC_BANK_3 };
	static constexpr int WIDCBankGo[3] = { WIDC_BANKGO_0, WIDC_BANKGO_1, WIDC_BANKGO_2 };
	static constexpr int WIDCBankGold[3] = { WIDC_BANKGOLD_0, WIDC_BANKGOLD_1, WIDC_BANKGOLD_2 };

	LPCTSTR pszText = prj.GetText(TID_GAME_ITEM);

	for (int i = 0; i != 3; ++i) {
		BankSlot & slot = m_slots[i];

		slot.pTabCtrl = GetDlgItem<CWndTabCtrl>(WIDCBanks[i]);

		slot.wndItemCtrl.Create(WLVS_ICON, CRect(0, 0, 250, 250), slot.pTabCtrl, 11);
		slot.wndItemCtrl.InitItem(&g_pPlayer->m_Bank[i], APP_COMMON_BANK);

		slot.pTabCtrl->InsertItem(&slot.wndItemCtrl, pszText);

		LPWNDCTRL lpWndCtrl = GetWndCtrl(WIDCBankGo[i]);
		slot.wndGold.Create("g", WBS_NODRAWFRAME, lpWndCtrl->rect, this, WIDCBankGo[i]);
		slot.wndGold.AddWndStyle(WBS_NODRAWFRAME);

		slot.pCost = GetDlgItem<CWndStatic>(WIDCBankGold[i]);
	}

	ReSetBank();
	
	m_slots[0].rectItemCtrl = m_slots[0].pTabCtrl->m_rectClient + CRect(-3, 2, -18, -20);
	m_slots[1].rectItemCtrl = m_slots[1].pTabCtrl->m_rectClient + CRect(-3, 2, -19, -20);
	m_slots[2].rectItemCtrl = m_slots[2].pTabCtrl->m_rectClient + CRect(-3, 2, -20, -20);

	CWndBase * pWndInventory = GetWndBase( APP_INVENTORY );
	CRect rectInventory = pWndInventory->GetWindowRect( TRUE );
	CPoint ptInventory = rectInventory.TopLeft();
	CPoint ptMove;

	if( ptInventory.x > m_pWndRoot->GetWndRect().Width() / 2 )
		ptMove = ptInventory - CPoint( (int)( rectInventory.Width() * 2.525 ), 0 );
	else
		ptMove = ptInventory + CPoint( rectInventory.Width(), 0 );
	Move( ptMove );

	m_pTexture = m_textureMng.AddTexture( m_pApp->m_pd3dDevice,  MakePath( DIR_THEME, "WndCommonBankNotUse.tga" ), 0xffff00ff );
} 

void CWndBank::ReSetBank() {
	for (int i = 0 ; i < 3 ; ++i) {
		BankSlot & slot = m_slots[i];

		if( g_pPlayer->m_nSlot == i ) {
			slot.bUse = true;
		} else if( g_pPlayer->m_idPlayerBank[i] == 0 || !g_pPlayer->IsCommBank() ) {
			slot.wndItemCtrl.SetScrollBarVisible( FALSE );
			slot.wndItemCtrl.EnableWindow( FALSE );
			slot.wndGold.EnableWindow( FALSE );
			slot.bUse = false;
		} else {
			slot.wndItemCtrl.SetScrollBarVisible( TRUE );
			slot.wndItemCtrl.EnableWindow( TRUE );
			slot.wndGold.EnableWindow( TRUE );
			slot.bUse = true;
		}

		slot.rectGold = slot.pCost->m_rectClient;
	}	
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndBank::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	if( g_pPlayer == NULL )
		return FALSE;
	CWndGuideSystem* pWndGuide = (CWndGuideSystem*)GetWndBase( APP_GUIDE );
	if(pWndGuide && pWndGuide->IsVisible()) 
	{
		if(pWndGuide->m_CurrentGuide.m_nVicCondition == OPEN_WINDOW && pWndGuide->m_CurrentGuide.m_nInput == APP_COMMON_BANK)
			pWndGuide->m_Condition.nOpenedWindowID = 1;
	}
	return CWndNeuz::InitDialog( APP_COMMON_BANK, pWndParent, 0, CPoint( 0, 0 ) );
} 

BYTE CWndBank::GetPosOfItemCtrl(const CWndBase * pWnd, BYTE defaultValue) const {
	for (BYTE i = 0; i != 3; ++i) {
		if (pWnd == &(m_slots[i].wndItemCtrl)) {
			return i;
		}
	}

	return defaultValue;
}

BYTE CWndBank::GetPosOfGold(const CWndBase * pWnd, BYTE defaultValue) const {
	for (BYTE i = 0; i != 3; ++i) {
		if (pWnd == &(m_slots[i].wndGold)) {
			return i;
		}
	}

	return defaultValue;
}


BOOL CWndBank::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
	// 인벤토리에서 Bank로 아이템이 Drop되었다. 어디서 왔는지 확인한후 적절히 대응하라.
	if( message == WIN_ITEMDROP )
	{
		LPSHORTCUT lpShortcut = (LPSHORTCUT)pLResult;
		CWndBase* pWndFrame = lpShortcut->m_pFromWnd->GetFrameWnd();
		CWndBase* pWndTarget = pWndFrame->m_pWndOnMouseMove;
		CWndBase* pWndPut = pWndFrame->m_pCurFocus;

		const BYTE nSlot = GetPosOfItemCtrl(pWndTarget);
				
		if( nID == 11 ) // item
		{
			if( pWndFrame->GetWndId() == APP_INVENTORY )
			{
				SAFE_DELETE( g_WndMng.m_pWndTradeGold );
				if( lpShortcut->m_dwData != 0 )
				{
					CWndItemCtrl* pWndItemCtrl = (CWndItemCtrl*)lpShortcut->m_pFromWnd;
					UINT SelectCount = pWndItemCtrl->GetSelectedCount();
					if( SelectCount != 1)
					{
						g_WndMng.PutString(TID_GAME_EQUIPPUT);
						
					}
					else
					{

						CItemElem* itemElem = (CItemElem*)lpShortcut->m_dwData;
						if( itemElem->m_nItemNum > 1 )
						{ 
							g_WndMng.m_pWndTradeGold = new CWndTradeGold;
							memcpy( &g_WndMng.m_pWndTradeGold->m_Shortcut, pLResult, sizeof(SHORTCUT) );
							g_WndMng.m_pWndTradeGold->m_dwGold = itemElem->m_nItemNum;
							g_WndMng.m_pWndTradeGold->m_nIdWndTo = APP_BANK;
							g_WndMng.m_pWndTradeGold->m_pWndBase = this;
							g_WndMng.m_pWndTradeGold->m_nSlot = nSlot;
							
							g_WndMng.m_pWndTradeGold->Initialize( NULL, APP_TRADE_GOLD );
							g_WndMng.m_pWndTradeGold->MoveParentCenter();
							CWndStatic* pStatic	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_STATIC );
							CWndStatic* pStaticCount	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_CONTROL1 );
							pStatic->m_strTitle = prj.GetText(TID_GAME_MOVECOUNT);
							pStaticCount->m_strTitle = prj.GetText(TID_GAME_NUMCOUNT);
						}
						else
						{
							g_DPlay.SendPutItemBank( nSlot, (BYTE)( lpShortcut->m_dwId ), 1 );
						}
						
					}
				}
				else
				{
					// 페냐 (돈)
					g_WndMng.m_pWndTradeGold = new CWndTradeGold;
					memcpy( &g_WndMng.m_pWndTradeGold->m_Shortcut, pLResult, sizeof(SHORTCUT) );
					g_WndMng.m_pWndTradeGold->m_dwGold = g_pPlayer->GetGold();
					g_WndMng.m_pWndTradeGold->m_nIdWndTo = APP_BANK;
					g_WndMng.m_pWndTradeGold->m_pWndBase = this;
					g_WndMng.m_pWndTradeGold->m_nSlot = nSlot;
					
					g_WndMng.m_pWndTradeGold->Initialize( NULL, APP_TRADE_GOLD );
					g_WndMng.m_pWndTradeGold->MoveParentCenter();
					CWndStatic* pStatic	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_STATIC );
					CWndStatic* pStaticCount	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_CONTROL1 );

					pStatic->m_strTitle = prj.GetText(TID_GAME_MOVEPENYA);
					pStaticCount->m_strTitle = prj.GetText(TID_GAME_PENYACOUNT);
				}
			}
			else if( pWndFrame->GetWndId() == APP_COMMON_BANK )
			{
				BYTE nPutSolt;
			
				SAFE_DELETE( g_WndMng.m_pWndTradeGold );
				if( lpShortcut->m_dwData != 0 )
				{
					nPutSolt = GetPosOfItemCtrl(pWndPut);

					CWndItemCtrl* pWndItemCtrl = (CWndItemCtrl*)lpShortcut->m_pFromWnd;
					UINT SelectCount = pWndItemCtrl->GetSelectedCount();
					if( SelectCount != 1)
					{
						g_WndMng.PutString(TID_GAME_EQUIPPUT);
						
					}
					else
					{

						CItemElem* itemElem = (CItemElem*)lpShortcut->m_dwData;
						if( itemElem->m_nItemNum > 1 )
						{ 
							g_WndMng.m_pWndTradeGold = new CWndTradeGold;
							memcpy( &g_WndMng.m_pWndTradeGold->m_Shortcut, pLResult, sizeof(SHORTCUT) );
							g_WndMng.m_pWndTradeGold->m_dwGold = itemElem->m_nItemNum;
							g_WndMng.m_pWndTradeGold->m_nIdWndTo = APP_COMMON_BANK;
							g_WndMng.m_pWndTradeGold->m_pWndBase = this;
							g_WndMng.m_pWndTradeGold->m_nSlot = nSlot;
							g_WndMng.m_pWndTradeGold->m_nPutSlot = nPutSolt;
							
							g_WndMng.m_pWndTradeGold->Initialize( NULL, APP_TRADE_GOLD );
							g_WndMng.m_pWndTradeGold->MoveParentCenter();
							CWndStatic* pStatic	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_STATIC );
							CWndStatic* pStaticCount	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_CONTROL1 );
							pStatic->m_strTitle = prj.GetText(TID_GAME_MOVECOUNT);
							pStaticCount->m_strTitle = prj.GetText(TID_GAME_NUMCOUNT);
						}
						else
						{
							g_DPlay.SendPutItemBankToBank( nPutSolt, nSlot, (BYTE)( lpShortcut->m_dwId ), 1 );
						}
						
					}
				}
				else
				{
					nPutSolt = GetPosOfGold(pWndPut);
					
					// 페냐 (돈)
					g_WndMng.m_pWndTradeGold = new CWndTradeGold;
					memcpy( &g_WndMng.m_pWndTradeGold->m_Shortcut, pLResult, sizeof(SHORTCUT) );
					g_WndMng.m_pWndTradeGold->m_dwGold = g_pPlayer->m_dwGoldBank[nPutSolt];
					g_WndMng.m_pWndTradeGold->m_nIdWndTo = APP_COMMON_BANK;
					g_WndMng.m_pWndTradeGold->m_pWndBase = this;
					g_WndMng.m_pWndTradeGold->m_nSlot = nSlot;
					g_WndMng.m_pWndTradeGold->m_nPutSlot = nPutSolt;
					
					g_WndMng.m_pWndTradeGold->Initialize( NULL, APP_TRADE_GOLD );
					g_WndMng.m_pWndTradeGold->MoveParentCenter();
					CWndStatic* pStatic	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_STATIC );
					CWndStatic* pStaticCount	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_CONTROL1 );

					pStatic->m_strTitle = prj.GetText(TID_GAME_MOVEPENYA);
					pStaticCount->m_strTitle = prj.GetText(TID_GAME_PENYACOUNT);
				}
			}
		}
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

//---------------------------------------------------------------------------------------------------------------------------
// 패스워드 확인창
//---------------------------------------------------------------------------------------------------------------------------
CWndConfirmBank::CWndConfirmBank() 
{ 
	m_dwId	= NULL_ID;
	m_dwItemId	= 0;
} 
CWndConfirmBank::~CWndConfirmBank() 
{ 
} 
void CWndConfirmBank::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndConfirmBank::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	
	CWndEdit *m_pPass   = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
	if( m_pPass )
	{
		m_pPass->AddWndStyle( EBS_PASSWORD );
		m_pPass->SetFocus();
	}
	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndConfirmBank::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( APP_CONFIRM_BANK, pWndParent, 0, CPoint( 0, 0 ) );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndConfirmBank::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndConfirmBank::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndConfirmBank::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndConfirmBank::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndConfirmBank::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndConfirmBank::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_OK || message == EN_RETURN )
	{
		CWndEdit *m_pPass   = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
		if( m_pPass )
		{
			// 여기서 공백이 있는지 혹은 숫자이외의 것이 오는지 확인
			char szPass[10] ={0,};
			strncpy( szPass, m_pPass->GetString(), 10 );
			if( strlen( szPass ) == 4 )
			{
				BOOL bDigit = TRUE;
				for( int i = 0 ; i < 4 ; i++ )
				{
					if( isdigit2( szPass[i] ) == FALSE )
					{
						bDigit = FALSE;	
						break;
					}
				}
				if( bDigit )
				{ 
					g_DPlay.SendConfirmBank( szPass, m_dwId, m_dwItemId );
				} 
				else
				{
					m_pPass->SetString( "" );
					g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0025) ) );
//					g_WndMng.OpenMessageBox( "숫자 4자리로만 넣어야 합니다. 다시 입력해주세요" );
				}
			}
			else
			{
				m_pPass->SetString( "" );
				g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0025) ) );
//				g_WndMng.OpenMessageBox( "숫자 4자리로만 넣어야 합니다. 다시 입력해주세요" );
			}
		}
	}
	else if( nID == WIDC_CHANGE_PASS )
	{
		g_WndMng.m_pWndBankPassword = new CWndBankPassword;
		g_WndMng.m_pWndBankPassword->SetBankPassword( 1 );
		g_WndMng.m_pWndBankPassword->Initialize( NULL, APP_BANK_PASSWORD );	

		Destroy();
	}
	else if( nID == WIDC_CANCEL )
	{
		Destroy();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 



//---------------------------------------------------------------------------------------------------------------------------
// 패스워드 변경창
//---------------------------------------------------------------------------------------------------------------------------
CWndBankPassword::CWndBankPassword() 
{ 
	m_nFlags = 0;
	m_dwId	= NULL_ID;
	m_dwItemId	= 0;
} 
CWndBankPassword::~CWndBankPassword() 
{ 
} 
void CWndBankPassword::OnDraw( C2DRender* p2DRender ) 
{ 
} 
void CWndBankPassword::SetBankPassword( int nFlags )
{
	m_nFlags = nFlags;
}
void CWndBankPassword::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	CWndEdit *m_pLastPass   = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
	CWndEdit *m_pNewPass = (CWndEdit*)GetDlgItem( WIDC_EDIT2 );
	CWndEdit *m_pConfirmPass = (CWndEdit*)GetDlgItem( WIDC_EDIT3 );

	if( m_pLastPass && m_pNewPass && m_pConfirmPass )
	{	
		m_pLastPass->AddWndStyle( EBS_PASSWORD );
		m_pNewPass->AddWndStyle( EBS_PASSWORD );
		m_pConfirmPass->AddWndStyle( EBS_PASSWORD );

		if( m_nFlags == 0 )
		{
			m_pLastPass->SetString( "0000" );
			m_pNewPass->SetFocus();
		}
		else
		{
			m_pLastPass->SetFocus();
		}
	}

	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndBankPassword::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( APP_BANK_PASSWORD, pWndParent, 0, CPoint( 0, 0 ) );
} 
/*
  직접 윈도를 열때 사용 
BOOL CWndBankPassword::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	CRect rectWindow = m_pWndRoot->GetWindowRect(); 
	CRect rect( 50 ,50, 300, 300 ); 
	SetTitle( _T( "title" ) ); 
	return CWndNeuz::Create( WBS_THICKFRAME | WBS_MOVE | WBS_SOUND | WBS_CAPTION, rect, pWndParent, dwWndId ); 
} 
*/
BOOL CWndBankPassword::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndBankPassword::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 
void CWndBankPassword::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 
void CWndBankPassword::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 
BOOL CWndBankPassword::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_OK || message == EN_RETURN )
	{
		CWndEdit *m_pLastPass   = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
		CWndEdit *m_pNewPass = (CWndEdit*)GetDlgItem( WIDC_EDIT2 );
		CWndEdit *m_pConfirmPass = (CWndEdit*)GetDlgItem( WIDC_EDIT3 );
		if( m_pLastPass && m_pNewPass && m_pConfirmPass )
		{
			// 여기서 공백이 있는지 혹은 숫자이외의 것이 오는지 확인
			char szLastPass[10] ={0,};
			char szNewPass[10] ={0,};
			char szConfirmPass[10] ={0,};
//			strncpy( szLastPass, m_pLastPass->GetString(), 10 );
//			strncpy( szNewPass, m_pNewPass->GetString(), 10 );
//			strncpy( szConfirmPass, m_pConfirmPass->GetString(), 10 );
			strcpy( szLastPass, m_pLastPass->GetString() );
			strcpy( szNewPass, m_pNewPass->GetString() );
			strcpy( szConfirmPass, m_pConfirmPass->GetString() );
			if( strlen( szLastPass ) == 4 && strlen( szNewPass ) == 4 && strlen( szConfirmPass ) == 4)
			{
				if( 0 == strcmp( szNewPass, szConfirmPass ) )
				{
					BOOL bDigit = TRUE;
					for( int i = 0 ; i < 4 ; i++ )
					{
						if( (isdigit2( szLastPass[i] ) == FALSE) || (isdigit2( szNewPass[i] ) == FALSE) || (isdigit2( szConfirmPass[i] ) == FALSE) )
						{
							bDigit = FALSE;
							break;
						}
					}
					if( bDigit )
					{
						g_DPlay.SendPacket<PACKETTYPE_CHANGEBANKPASS,
							char[10], char[10], DWORD, DWORD
						>(szLastPass, szNewPass, m_dwId, m_dwItemId);
					}
					else
					{
						if( m_nFlags == 0 )
						{
							m_pLastPass->SetString( "0000" );
						}
						else
						{
							m_pLastPass->SetString( "" );
						}

						m_pNewPass->SetString( "" );
						m_pConfirmPass->SetString( "" );
						g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0025) ) );
//						g_WndMng.OpenMessageBox( "숫자 4자리로만 넣어야 합니다. 다시 입력해주세요" );
					}					
				}
				else
				{
					m_pNewPass->SetString( "" );
					m_pConfirmPass->SetString( "" );
					g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0022) ) );
//					g_WndMng.OpenMessageBox( "새암호와 암호확인이 다름니다. 다시 입력해주세요" );					
				}
			}
			else
			{
				if( m_nFlags == 0 )
				{
					m_pLastPass->SetString( "0000" );
				}
				else
				{
					m_pLastPass->SetString( "" );
				}
				m_pNewPass->SetString( "" );
				m_pConfirmPass->SetString( "" );
				g_WndMng.OpenMessageBox( _T( prj.GetText(TID_DIAG_0025) ) );
//				g_WndMng.OpenMessageBox( "숫자 4자리로만 넣어야 합니다. 다시 입력해주세요" );
			}
		}
	}
	else if( nID == WIDC_CANCEL )
	{
		Destroy();
	}
	else if( m_nFlags == 0 && message == 0 && nID == WIDC_EDIT1 )
	{
		CWndEdit *m_pNewPass = (CWndEdit*)GetDlgItem( WIDC_EDIT2 );
		m_pNewPass->SetFocus();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}