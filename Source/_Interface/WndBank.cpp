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

	if( ptInventory.x > g_WndMng.GetWndRect().Width() / 2 )
		ptMove = ptInventory - CPoint( (int)( rectInventory.Width() * 2.525 ), 0 );
	else
		ptMove = ptInventory + CPoint( rectInventory.Width(), 0 );
	Move( ptMove );

	m_pTexture = m_textureMng.AddTexture( MakePath( DIR_THEME, "WndCommonBankNotUse.tga" ), 0xffff00ff );
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
				if( lpShortcut->m_dwData != 0 )
				{
					CWndItemCtrl* pWndItemCtrl = (CWndItemCtrl*)lpShortcut->m_pFromWnd;
					UINT SelectCount = pWndItemCtrl->GetSelectedCount();
					if (SelectCount != 1) {
						g_WndMng.PutString(TID_GAME_EQUIPPUT);
					} else {
						CWndTradeGold::Create<SHORTCUT::Source::Inventory>(
							{ lpShortcut->m_dwId },
							[nSlot](auto source, int quantity) {
								g_DPlay.SendPutItemBank(nSlot, source.itemPos, quantity);
							}
						);
					}
				}
				else
				{
					CWndTradeGold::Create<SHORTCUT::Source::Penya>(
						{},
						[nSlot](auto, int quantity) {
							g_DPlay.SendPutGoldBank(nSlot, quantity);
						}
					);
				}
			}
			else if( pWndFrame->GetWndId() == APP_COMMON_BANK )
			{
				BYTE nPutSolt;
			
				if( lpShortcut->m_dwData != 0 )
				{
					nPutSolt = GetPosOfItemCtrl(pWndPut);

					CWndItemCtrl* pWndItemCtrl = (CWndItemCtrl*)lpShortcut->m_pFromWnd;
					UINT SelectCount = pWndItemCtrl->GetSelectedCount();
					if ( SelectCount != 1) {
						g_WndMng.PutString(TID_GAME_EQUIPPUT);
					} else {
						CWndTradeGold::Create<SHORTCUT::Source::Bank>(
							{ nPutSolt, lpShortcut->m_dwId },
							[nSlot](auto source, int quantity) {
								g_DPlay.SendPutItemBankToBank(source.slot, nSlot, source.itemPos, quantity);
							}
						);
					}
				}
				else
				{
					nPutSolt = GetPosOfGold(pWndPut);

					CWndTradeGold::Create<SHORTCUT::Source::BankPenya>(
						{ nPutSolt },
						[nSlot](auto source, int quantity) {
							g_DPlay.SendPutGoldBankToBank(source.slot, nSlot, quantity);
						}
					);
				}
			}
		}
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

//---------------------------------------------------------------------------------------------------------------------------

bool CWndBankPassword::IsValidPassword(const char * text) {
	size_t size = std::strlen(text);
	if (size != 4) return false;

	for (size_t i = 0; i != 4; ++i) {
		if (!isdigit2(text[i])) {
			return false;
		}
	}

	return true;
}

void CWndBankPassword::CopyPassword(char * buffer, const char * text) {
	std::strncpy(buffer, text, 5);
	buffer[4] = '\0';
}


//---------------------------------------------------------------------------------------------------------------------------
// 패스워드 확인창
//---------------------------------------------------------------------------------------------------------------------------

void CWndConfirmBank::OnInitialUpdate() { 
	CWndNeuz::OnInitialUpdate(); 
	
	CWndEdit * m_pPass = GetDlgItem<CWndEdit>( WIDC_EDIT1 );
	if (m_pPass) {
		m_pPass->AddWndStyle(EBS_PASSWORD);
		m_pPass->SetFocus();
	}
	
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndConfirmBank::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( APP_CONFIRM_BANK, pWndParent, 0, CPoint( 0, 0 ) );
}

BOOL CWndConfirmBank::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_OK || message == EN_RETURN )
	{
		CWndEdit * m_pPass = GetDlgItem<CWndEdit>(WIDC_EDIT1);
		if (m_pPass) {
			char szPass[5];
			CWndBankPassword::CopyPassword(szPass, m_pPass->GetString());

			if (CWndBankPassword::IsValidPassword(szPass)) {
				g_DPlay.SendPacket<PACKETTYPE_CONFIRMBANK, char[5], OBJID>(szPass, m_dwId);
			} else {
				m_pPass->SetString("");
				g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0025)));
			}
		}
	} else if( nID == WIDC_CHANGE_PASS ) {
		g_WndMng.m_pWndBankPassword = new CWndBankPassword(true, m_dwId);
		g_WndMng.m_pWndBankPassword->Initialize( NULL );	

		Destroy();
	} else if( nID == WIDC_CANCEL ) {
		Destroy();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 



//---------------------------------------------------------------------------------------------------------------------------
// 패스워드 변경창
//---------------------------------------------------------------------------------------------------------------------------

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

		if(!m_needCurrentPassword)
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
			char szLastPass[5]; CopyPassword(szLastPass, m_pLastPass->GetString());
			char szNewPass[5]; CopyPassword(szNewPass, m_pNewPass->GetString());
			const char * szConfirmPass = m_pConfirmPass->GetString();

			if (!IsValidPassword(szLastPass)
				|| !IsValidPassword(szNewPass)
				|| !IsValidPassword(szConfirmPass)) {

				if (!m_needCurrentPassword) {
					m_pLastPass->SetString("0000");
				} else {
					m_pLastPass->SetString("");
				}
				m_pNewPass->SetString("");
				m_pConfirmPass->SetString("");
				g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0025)));
			} else {
				if (0 == strcmp(szNewPass, szConfirmPass)) {
					g_DPlay.SendPacket<PACKETTYPE_CHANGEBANKPASS, char[5], char[5], OBJID>(
						szLastPass, szNewPass, m_dwId
					);
				} else {
					m_pNewPass->SetString("");
					m_pConfirmPass->SetString("");
					g_WndMng.OpenMessageBox(_T(prj.GetText(TID_DIAG_0022)));
				}
			}
		}
	}
	else if( nID == WIDC_CANCEL )
	{
		Destroy();
	}
	else if( !m_needCurrentPassword && message == 0 && nID == WIDC_EDIT1 )
	{
		CWndBase *m_pNewPass = GetDlgItem( WIDC_EDIT2 );
		m_pNewPass->SetFocus();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}
