#include "StdAfx.h"
#include "WndPost.h"
#include "AppDefine.h"
#include "defineText.h"
#include "DPClient.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// ��ų ��й�? 
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////

void CWndPostItemWarning::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();
	MoveParentCenter();
}

void CWndPostItemWarning::SetString(const char* string) {
	if (CWndEdit * pWndEdit = GetDlgItem<CWndEdit>(WIDC_CONTEXT)) {
		pWndEdit->SetString(string);
		pWndEdit->EnableWindow(FALSE);
	}
}

BOOL CWndPostItemWarning::Initialize(CWndBase * pWndParent, DWORD) {
	LPWNDAPPLET lpWndApplet = m_resMng.GetAt(APP_QUEITMWARNING);
	CRect rect{ CPoint(0, 0), lpWndApplet->size };

	return CWndNeuz::Create(WBS_MOVE | WBS_SOUND | WBS_CAPTION | WBS_MODAL, rect, pWndParent, APP_QUEITMWARNING);
}

BOOL CWndPostItemWarning::OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) {
	if (nID == WIDC_BTN_YES || message == EN_RETURN) {
		g_DPlay.SendQueryGetMailItem(m_nMailIndex);
		Destroy();
	} else if (nID == WIDC_BTN_NO) {
		Destroy();
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}


///////////////////////////////////////////////////////////////////////////////

CWndPost::~CWndPost() 
{ 
	CloseMailRequestingBox();
} 

void CWndPost::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	EnableWindow( FALSE );

	//	���⿡ �ڵ��ϸ� �˴ϴ�
	CWndTabCtrl * pWndTabCtrl = GetDlgItem<CWndTabCtrl>(WIDC_TABCTRL1);
	CRect rect{ CPoint(5, 0), GetClientRect().BottomRight() };

	Windows::DestroyIfOpened(APP_BAG_EX);

	m_PostTabSend.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_POST_SEND );
	m_PostTabReceive.Create( WBS_CHILD | WBS_NODRAWFRAME, rect, pWndTabCtrl, APP_POST_RECEIVE );

	pWndTabCtrl->InsertItem(&m_PostTabSend, prj.GetText(TID_APP_POST_SEND));
	pWndTabCtrl->InsertItem(&m_PostTabReceive, prj.GetText(TID_APP_POST_RECEIVE));
	pWndTabCtrl->SetCurSel(1);

	MoveParentCenter();	

	// ������ ���� ���? ��û
	g_DPlay.SendQueryMailBox();	

	CloseMailRequestingBox();
	m_pWndMailRequestingBox = new CWndMailRequestingBox;
	m_pWndMailRequestingBox->Initialize();
}

BOOL CWndPost::Initialize(CWndBase * pWndParent, DWORD) {
	return CWndNeuz::InitDialog(APP_POST, pWndParent, 0, CPoint(0, 0));
}

BOOL CWndPost::OnChildNotify(UINT message, UINT nID, LRESULT * pLResult) {
	if (message == WNM_SELCHANGE) {
		CWndTabCtrl * pWndTabCtrl = GetDlgItem<CWndTabCtrl>(WIDC_TABCTRL1);

		if (pWndTabCtrl->GetSelectedTab() == &m_PostTabReceive) {
			g_DPlay.SendQueryMailBox();
		}
	}

	return CWndNeuz::OnChildNotify(message, nID, pLResult);
}

void CWndPost::CloseMailRequestingBox() {
	SAFE_DELETE(m_pWndMailRequestingBox);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
// CWndPostSendMessageBox
//////////////////////////////////////////////////////////////////////////////
class CWndPostSendMessageBox : public CWndMessageBox {
public: 
	CWndText	m_wndText;
	
	BYTE		m_nItem;
	short		m_nItemNum;
	int			m_nGold;
	char		m_szReceiver[MAX_NAME];
	char		m_szTitle[MAX_MAILTITLE];	
	char		m_szText[MAX_MAILTEXT];		
	void		SetValue( BYTE nItem, short nItemNum, const char* lpszReceiver, int nGold, const char* lpszTitle, const char* lpszText );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = 0 );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
}; 

BOOL CWndPostSendMessageBox::Initialize(CWndBase * pWndParent, DWORD) {
	CString str;
	str.Format(prj.GetText(TID_MAIL_SEND_CONFIRM), MAX_KEEP_MAX_DAY);

	return CWndMessageBox::Initialize(str, pWndParent, MB_OKCANCEL);
}

void CWndPostSendMessageBox::SetValue( BYTE nItem, short nItemNum, const char* lpszReceiver, int nGold, const char* lpszTitle, const char* lpszText )
{
	m_nItem		= nItem;
	m_nItemNum	= nItemNum;
	strcpy( m_szReceiver, lpszReceiver );
	strcpy( m_szTitle, lpszTitle );
	strcpy( m_szText, lpszText );
	m_nGold		= nGold;
}

BOOL CWndPostSendMessageBox::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) {
	if (nID == 1) {
		if (g_pPlayer) {
			g_DPlay.SendQueryPostMail(m_nItem, m_nItemNum, m_szReceiver, m_nGold, m_szTitle, m_szText);

			if (CWndPost * pWndPost = g_WndMng.GetWndBase<CWndPost>(APP_POST)) {
				pWndPost->m_PostTabSend.ClearData();
			}
		}

		Destroy();
	} else if (nID == 2) {
		Destroy();
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

//////////////////////////////////////////////////////////////////////////////

CWndPostSend::~CWndPostSend() 
{ 
	ClearData();
} 

void CWndPostSend::ClearData()
{
	CItemElem * pItemElem = g_pPlayer->m_Inventory.GetAtId(m_nItem);
	if (pItemElem) {
		if (!g_pPlayer->m_vtInfo.IsTrading(pItemElem)) {
			pItemElem->SetExtra(0);
		}
	}
	
	m_nItem		= 0xff;		
	m_nCost		= 0;
	m_nCount	= 0;
}

void CWndPostSend::SetReceive(const char * pchar) {
	CWndComboBox * pWndCombo = GetDlgItem <CWndComboBox>(WIDC_COMBOBOX1);
	pWndCombo->SetString(pchar);
}

void CWndPostSend::SetTitle(const char * pchar) {
	CWndEdit * pWndEdit1 = GetDlgItem<CWndEdit>(WIDC_EDIT2);
	pWndEdit1->SetString(pchar);
}

void CWndPostSend::SetText(const char * pchar) {
	CWndEdit * pWndEdit1 = (CWndEdit *)GetDlgItem(WIDC_EDIT3);
	pWndEdit1->SetString(pchar);
}

void CWndPostSend::OnDraw( C2DRender* p2DRender ) 
{ 
	if (m_nItem == 0xFF) return;

	CItemElem * pItemElem = g_pPlayer->m_Inventory.GetAtId(m_nItem);
	if (!pItemElem) return;
		
	LPWNDCTRL pCustom = GetWndCtrl( WIDC_STATIC3 );
			
	pItemElem->GetTexture()->Render( p2DRender, pCustom->rect.TopLeft(), 255 );

	if( pItemElem->GetProp()->dwPackMax > 1 && pItemElem->GetExtra() > 0 )
	{
		CD3DFont* pOldFont = p2DRender->GetFont(); 
		p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndTitle ); 
				
		TCHAR szTemp[32];
		_stprintf( szTemp, "%d", pItemElem->GetExtra() );
		p2DRender->TextOut( pCustom->rect.right-13,  pCustom->rect.bottom-13 , szTemp, 0xff0000ff );
		p2DRender->TextOut( pCustom->rect.right-14,  pCustom->rect.bottom-14 , szTemp, 0xffb0b0f0 );

		p2DRender->SetFont( pOldFont );				
	}	
			
	CRect hitrect = pCustom->rect;
	CPoint point = GetMousePoint();
	if( hitrect.PtInRect( point ) )
	{
		CPoint point2 = point;
		ClientToScreen( &point2 );
		ClientToScreen( &hitrect );
				
		g_WndMng.PutToolTip_Item( pItemElem, point2, &hitrect );
	}
}

void CWndPostSend::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	// Setup WIDC_EDIT3
	CWndEdit * pNotice = GetDlgItem<CWndEdit>(WIDC_EDIT3);
	pNotice->AddWndStyle( EBS_WANTRETURN );
	pNotice->AddWndStyle( EBS_AUTOVSCROLL );

	if (LPWNDCTRL pWndCtrl = GetWndCtrl(WIDC_EDIT3)) {
		pNotice->SetWndRect(pWndCtrl->rect);
	}

	// Tab Stop
	CWndComboBox* pWndCombo = GetDlgItem<CWndComboBox>( WIDC_COMBOBOX1 );
	pWndCombo->SetTabStop( TRUE );

	GetDlgItem<CWndEdit>(WIDC_EDIT2)->SetTabStop( TRUE );
	pNotice->SetTabStop( TRUE );

	// Fill the list of players
	for (const u_long idPlayer : g_WndMng.m_RTMessenger | std::views::keys) {
		pWndCombo->AddString(CPlayerDataCenter::GetInstance()->GetPlayerString(idPlayer));
	}

	// Non korean adjustements
	if( ::GetLanguage() != LANG_KOR )
	{
		CWndStatic* pStatic2	= (CWndStatic *)GetDlgItem( WIDC_STATIC2 );
		CRect rect	= pStatic2->GetWndRect();
		rect.left	-= 14;
		rect.right	-= 14;
		pStatic2->SetWndRect( rect, TRUE );

		CWndEdit* pEdit4	= (CWndEdit*)GetDlgItem( WIDC_EDIT4 );
		rect	= pEdit4->GetWndRect();
		rect.left	-= 14;
		rect.right	+= 10;
		pEdit4->SetWndRect( rect, TRUE );
	}

	pWndCombo->SetFocus();
}

BOOL CWndPostSend::Initialize(CWndBase * pWndParent, DWORD) {
	return CWndNeuz::InitDialog(APP_POST_SEND, pWndParent, 0, CPoint(0, 0));
}

void CWndPostSend::SetItemId(BYTE nId) {
	if (nId != m_nItem && m_nItem != 0xff) {
		CItemElem * pItemElem = g_pPlayer->m_Inventory.GetAtId(m_nItem);
		if (pItemElem) {
			pItemElem->SetExtra(0);
		}
	}

	m_nItem = nId;
}

BOOL CWndPostSend::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CWndBase* pWndFrame =  pShortcut->m_pFromWnd->GetFrameWnd();
	
	// �������� �κ��丮���� �Դ°�?
	if( !(pShortcut->m_dwShortcut == ShortcutType::Item) && !(pWndFrame->GetWndId() == APP_INVENTORY) )
		return FALSE;

	if( pShortcut->m_dwData == 0 )
	{
		SetForbid( TRUE );		
		return FALSE;
	}
	
	if( g_pPlayer->m_Inventory.IsEquip( pShortcut->m_dwId ) )
	{
		g_WndMng.PutString(TID_GAME_EQUIPPUT);
		SetForbid( TRUE );
		return FALSE;
	}
	
	CItemElem* pItemElem = g_pPlayer->m_Inventory.GetAt( pShortcut->m_dwIndex );
	
	if(pItemElem == NULL)
		return FALSE;

	LPWNDCTRL pCustom = NULL;
	pCustom = GetWndCtrl( WIDC_STATIC3 );
	
	// ������( ���?, ���ⱸ )
	if( PtInRect(&(pCustom->rect), point) )
	{
		SetItemId((BYTE)(pItemElem->m_dwObjId));

		CWndTradeGold::Create<SHORTCUT::Source::Inventory>(
			{ pItemElem->m_dwObjId },
			[](auto source, int quantity) {
				if (!g_WndMng.m_pWndPost) return;

				CItemElem * pItemBase = g_pPlayer->m_Inventory.GetAtId(source.itemPos);
				if (!pItemBase) {
					g_WndMng.m_pWndPost->m_PostTabSend.SetItemId(0xff);
					g_WndMng.m_pWndPost->m_PostTabSend.SetCount(0);
					return;
				}

				pItemBase->SetExtra(quantity);
				g_WndMng.m_pWndPost->m_PostTabSend.SetItemId(pItemBase->m_dwObjId);
				g_WndMng.m_pWndPost->m_PostTabSend.SetCount(quantity);
			},
			{
				.onCancel = []() {
					if (!g_WndMng.m_pWndPost) return;
					g_WndMng.m_pWndPost->m_PostTabSend.SetItemId(0xff);
					g_WndMng.m_pWndPost->m_PostTabSend.SetCount(0);
				}
			}
		);

		if (const ItemProp * pItemProp = pItemElem->GetProp()) {
			GetDlgItem<CWndEdit>(WIDC_EDIT2)->SetString(pItemProp->szName);
		}
	}		
			
	return TRUE;
}

void CWndPostSend::OnRButtonUp(UINT, CPoint point) {
	LPWNDCTRL pCustom = GetWndCtrl(WIDC_STATIC3);

	if (pCustom->rect.PtInRect(point)) {
		CItemElem * pItemElem = g_pPlayer->m_Inventory.GetAtId(m_nItem);
		if (pItemElem) {
			pItemElem->SetExtra(0);
		}

		m_nItem = 0xff;
	}
}


BOOL CWndPostSend::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
	case WIDC_SEND:	
		{
			LPCTSTR szReceiver = GetDlgItem<CWndEdit>(WIDC_COMBOBOX1)->GetString();
			LPCTSTR szTitle    = GetDlgItem<CWndEdit>(WIDC_EDIT2)->GetString();
			LPCTSTR szText     = GetDlgItem<CWndEdit>(WIDC_EDIT3)->GetString();
			LPCTSTR szMoney    = GetDlgItem<CWndEdit>(WIDC_EDIT4)->GetString();
			
			// �����? �޴»����? ������ ������
			if (strlen(szReceiver) <= 0 || strlen(szTitle) <= 0) {
				return FALSE;
			}
			
			// �����۰� ���? �ΰ��� �ϳ��� ��� ������
			if (m_nItem == NULL_ID && strlen(szMoney) <= 0) {
				return FALSE;
			}

			// �ݾ׶��� ���ڰ� �ƴϸ� ������
			int nlen = strlen(szMoney);
			const bool bisdigit = std::all_of(szMoney, szMoney + nlen, isdigit2);
			
			if (!bisdigit) {
				//g_WndMng.OpenMessageBoxUpper( prj.GetText(TID_GAME_GUILDONLYNUMBER) );
				return FALSE;
			}

			__int64 liGold	= _atoi64(szMoney);
			if( liGold > 1000000000 )
			{
				g_WndMng.OpenMessageBoxUpper( prj.GetText(TID_GAME_LIMITPENYA) );
				return FALSE;				
			}
			
			if (strlen(szReceiver) < 3 || strlen(szReceiver) > 16) {
				g_WndMng.OpenMessageBoxUpper(prj.GetText(TID_DIAG_0057));
				return FALSE;
			}

			if( strlen(szText) > ( MAX_MAILTEXT - 1 ) ) 
			{
				CString str;
				str.Format( prj.GetText(TID_GAME_MAILBOX_TEXT_MAX), MAX_MAILTEXT );
				g_WndMng.OpenMessageBoxUpper( str );
				return FALSE;				
			}

			if( strlen(szTitle) > ( MAX_MAILTITLE - 1 ) )
			{
				CString str;
				str.Format( prj.GetText(TID_GAME_MAILBOX_TITLE_MAX), MAX_MAILTITLE );
				g_WndMng.OpenMessageBoxUpper( str );
				return FALSE;				
			}

			CWndPostSendMessageBox* pBox = new CWndPostSendMessageBox;
			g_WndMng.OpenCustomBox( "", pBox );
				
			CItemElem* pItemElem = g_pPlayer->m_Inventory.GetAtId( m_nItem );
				
			if (pItemElem) {
				pBox->SetValue(m_nItem, pItemElem->GetExtra(), szReceiver, atoi(szMoney), szTitle, szText);
			} else {
				pBox->SetValue(-1, 0                         , szReceiver, atoi(szMoney), szTitle, szText);
			}
				
			break;
		}
	}
		
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 


///////////////////////////////////////////////////////////////////////////////////////////////////////

void CWndPostRead::ClearData()
{
	m_nMailIndex = -1;
	m_bDrag = FALSE;
}

void CWndPostRead::OnDraw( C2DRender* p2DRender ) 
{ 
	if( m_nMailIndex == -1 )
		return;

	CMailBox* pMailBox	= CMailBox::GetInstance();	
	CMailBox& mailbox = *pMailBox;
	
	if( !mailbox[m_nMailIndex] )
		return;
	
	CItemElem* m_pItemElem = mailbox[m_nMailIndex]->m_pItemElem;

	if( m_pItemElem && m_pItemElem->GetTexture() )
	{
		LPWNDCTRL pCustom = NULL;
		pCustom = GetWndCtrl( WIDC_STATIC3 );

		CRect hitrect = pCustom->rect;
		CPoint point = GetMousePoint();
		if( hitrect.PtInRect( point ) )
		{
			CPoint point2 = point;
			ClientToScreen( &point2 );
			ClientToScreen( &hitrect );
			
			p2DRender->RenderRoundRect( pCustom->rect, D3DCOLOR_XRGB( 150, 0, 0 ) );
			g_WndMng.PutToolTip_Item( m_pItemElem, point2, &hitrect );
		}

		m_pItemElem->GetTexture()->Render( p2DRender, pCustom->rect.TopLeft(), 255 );

		if( m_pItemElem->GetProp()->dwPackMax > 1 )
		{
			CD3DFont* pOldFont = p2DRender->GetFont(); 
			p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndTitle ); 
			
			TCHAR szTemp[32];
			_stprintf( szTemp, "%d", m_pItemElem->m_nItemNum  );
			CSize size	= p2DRender->m_pFont->GetTextExtent( szTemp );
			p2DRender->TextOut( pCustom->rect.right-13,  pCustom->rect.bottom-13 , szTemp, 0xff0000ff );
			p2DRender->TextOut( pCustom->rect.right-14,  pCustom->rect.bottom-14 , szTemp, 0xffb0b0f0 );

			p2DRender->SetFont( pOldFont );				
		}	
	}

	{
		LPWNDCTRL pCustom = NULL;
		pCustom = GetWndCtrl( WIDC_STATIC4 );
		CRect hitrect = pCustom->rect;
		CPoint point = GetMousePoint();
		if( hitrect.PtInRect( point ) && mailbox[m_nMailIndex]->m_nGold > 0 )
		{
			CPoint point2 = point;
			ClientToScreen( &point2 );
			ClientToScreen( &hitrect );
			
			p2DRender->RenderRoundRect( pCustom->rect, D3DCOLOR_XRGB( 150, 0, 0 ) );
		}
	}
} 
void CWndPostRead::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CWndEdit* pWndEdit1	= (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
	CWndEdit* pWndEdit2	= (CWndEdit*)GetDlgItem( WIDC_EDIT2 );
	CWndEdit* pWndEdit3	= (CWndEdit*)GetDlgItem( WIDC_EDIT3 );
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );

	CRect rect	= pWndEdit3->GetWndRect();
	rect.right	+= 32;
	pWndEdit3->SetWndRect( rect, TRUE );

	

	pWndEdit1->EnableWindow(FALSE);
	pWndEdit2->EnableWindow(FALSE);
	pWndEdit3->EnableWindow(FALSE);

	CWndButton * pWndButton = (CWndButton *)GetDlgItem(WIDC_BUTTON3);
	pWndButton->SetFocus();
	MoveParentCenter();
	
	LPWNDCTRL lpWndCtrl = GetWndCtrl( WIDC_STATIC4 );
	m_wndGold.Create( "g", WBS_NODRAWFRAME, lpWndCtrl->rect, this, WIDC_STATIC4 );
	m_wndGold.AddWndStyle( WBS_NODRAWFRAME );
} 

// ó�� �� �Լ��� �θ��� ������ ������.
BOOL CWndPostRead::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	LPWNDAPPLET lpWndApplet = m_resMng.GetAt ( APP_POST_READ );
	CRect rect = CRect( 0, 0, lpWndApplet->size.cx, lpWndApplet->size.cy );
	
	return CWndNeuz::Create( WBS_MOVE | WBS_SOUND | WBS_CAPTION | WBS_MODAL, rect, pWndParent, APP_POST_READ ); 
} 

void CWndPostRead::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
	m_bDrag = FALSE;
} 
void CWndPostRead::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
	if( m_nMailIndex == -1 )
		return;
	
	CMailBox* pMailBox	= CMailBox::GetInstance();	
	CMailBox& mailbox = *pMailBox;
	
	CMail* pMail = mailbox[m_nMailIndex];
	
	if( !pMail )
		return;	
	
	LPWNDCTRL pCustom = GetWndCtrl( WIDC_STATIC3 );
	
	// ������( ���?, ���ⱸ )
	if (pCustom->rect.PtInRect(point)) {
		m_bDrag = TRUE;
	}
} 

void CWndPostRead::SetValue( int nMailIndex )
{
	m_nMailIndex	= nMailIndex;

	CMailBox* pMailBox	= CMailBox::GetInstance();	
	CMailBox& mailbox = *pMailBox;

	CMail* pMail = mailbox[m_nMailIndex];
				
	if( pMail == NULL )
		return;


	TCHAR szCutTitle[128];

	LPCSTR lpszPlayerString		= CPlayerDataCenter::GetInstance()->GetPlayerString( pMail->m_idSender );
	char lpszPlayer[MAX_PLAYER]		= { 0,};
	if( pMail->m_idSender == 0 )
		lstrcpy( lpszPlayer, "FLYFF" );
	else
		lstrcpy( lpszPlayer, lpszPlayerString );

	memset( szCutTitle, 0, sizeof(szCutTitle) );
	GetStrCut( lpszPlayer, szCutTitle, 16 );
	
	if( GetStrLen( lpszPlayer ) > 16 )
		_tcscat( szCutTitle, "..." );

	CWndEdit * pWndEdit1 = (CWndEdit *)GetDlgItem(WIDC_EDIT1);
	if( lstrlen( lpszPlayer ) > 0 )
		pWndEdit1->SetString(szCutTitle);
	else
		pWndEdit1->SetString("Unknown");
	
	memset( szCutTitle, 0, sizeof(szCutTitle) );
	GetStrCut( pMail->m_szTitle, szCutTitle, 13 );
	
	if( GetStrLen( pMail->m_szTitle ) >= 13 )
	{
		_tcscat( szCutTitle, "..." );
	}

	CWndEdit * pWndEdit2 = (CWndEdit *)GetDlgItem(WIDC_EDIT2);
	CWndText * pWndText = (CWndText *)GetDlgItem(WIDC_TEXT1);
	if( lstrlen( lpszPlayer ) > 0 )
	{
		pWndEdit2->SetString(szCutTitle);
		pWndText->SetString(pMail->m_szText);
	}
	else
	{
		pWndEdit2->SetString( "" );
		pWndText->SetString( "" );
	}

	g_DPlay.SendQueryReadMail( pMail->m_nMail );

	char szbuffer[128] = {0};
	sprintf( szbuffer, "%d", pMail->m_nGold );
	CWndEdit * pWndEdit3 = (CWndEdit *)GetDlgItem(WIDC_EDIT3);
	pWndEdit3->SetString(GetNumberFormatEx(szbuffer));
}

BOOL CWndPostRead::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	return TRUE;
}

void CWndPostRead::OnMouseMove(UINT nFlags, CPoint point )
{
	if( m_bDrag )
	{
		if( m_nMailIndex == -1 )
			return;
		
		CMailBox* pMailBox	= CMailBox::GetInstance();	
		CMailBox& mailbox = *pMailBox;
		
		CMail* pMail = mailbox[m_nMailIndex];
		
		if( pMail && pMail->m_pItemElem && pMail->m_pItemElem->GetProp() )
		{
			m_GlobalShortcut.m_pFromWnd   = this;
			m_GlobalShortcut.m_dwShortcut = ShortcutType::Item;
			m_GlobalShortcut.m_dwIndex    = 0xffffffff;
			m_GlobalShortcut.m_dwId       = pMail->m_pItemElem->m_dwObjId;
			m_GlobalShortcut.m_pTexture   = pMail->m_pItemElem->GetTexture();
			m_GlobalShortcut.m_dwData     = reinterpret_cast<DWORD>(pMail->m_pItemElem);
			_tcscpy( m_GlobalShortcut.m_szString, pMail->m_pItemElem->GetProp()->szName);
		}
	}	
}

void CWndPostRead::OnRButtonUp( UINT nFlags, CPoint point )
{
	if( m_nMailIndex == -1 )
		return;

	CMailBox* pMailBox	= CMailBox::GetInstance();	
	CMailBox& mailbox = *pMailBox;
	
	CMail* pMail = mailbox[m_nMailIndex];
	
	if( !pMail )
		return;	

	// ������( ���?, ���ⱸ )
	if (GetWndCtrl(WIDC_STATIC3)->rect.PtInRect(point)) {
		MailReceiveItem();
	}
	
	if (GetWndCtrl(WIDC_STATIC4)->rect.PtInRect(point)) {
		MailReceiveGold();
	}
}
void CWndPostRead::MailReceiveItem()
{
	// �ش� �������� ���Ͽ��� �����´�.
	if( m_nMailIndex == -1 )
		return;
	
	CMailBox* pMailBox	= CMailBox::GetInstance();	
	CMailBox& mailbox = *pMailBox;
	
	CMail* pMail = mailbox[m_nMailIndex];
				
	if( pMail && pMail->m_pItemElem )
	{
		// �⺻ �����ϼ� ���������� �˻��Ͽ� ������ �ΰ��Ѵ�.
		const auto [nDay, dwTime] = pMail->GetMailInfo();

		// �⺻ �����ϼ��� ������!!!
		if( (MAX_KEEP_MAX_DAY*24) - dwTime > (MAX_KEEP_BASIC_DAY*24) )
		{
			FLOAT fCustody = (FLOAT)( (FLOAT)( MAX_KEEP_MAX_DAY - MAX_KEEP_BASIC_DAY - nDay ) / (FLOAT)( MAX_KEEP_MAX_DAY - MAX_KEEP_BASIC_DAY ) );

			m_bDrag = FALSE;
//			SAFE_DELETE(m_pWndPostItemWarning);
			m_pWndPostItemWarning = new CWndPostItemWarning;
			m_pWndPostItemWarning->Initialize( this, 0 );
			m_pWndPostItemWarning->SetIndex( pMail->m_nMail );

			FLOAT fPay = pMail->m_pItemElem->GetCost() * fCustody;
			if( fPay < 0.0f )
				fPay = 0.0f;

			TCHAR szChar[256] = { 0 };
			sprintf( szChar, prj.GetText(TID_GAME_MAILBOX_KEEP_PAY), MAX_KEEP_BASIC_DAY, (int)fPay );

			m_pWndPostItemWarning->SetString( szChar );
		}
		else
		{
			g_DPlay.SendQueryGetMailItem( pMail->m_nMail );								
		}
	}
}
void CWndPostRead::MailReceiveGold()
{
	// �ش� ��ĸ�? ���Ͽ��� �����´�.
	if( m_nMailIndex == -1 )
		return;
	
	CMailBox* pMailBox	= CMailBox::GetInstance();	
	CMailBox& mailbox = *pMailBox;
	
	CMail* pMail = mailbox[m_nMailIndex];
	
	if( pMail && pMail->m_nMail > 0 && pMail->m_nGold > 0 )
	{
		g_DPlay.SendQueryGetMailGold( pMail->m_nMail );		
		
		CWndEdit* pWndEdit	= (CWndEdit*)GetDlgItem( WIDC_EDIT3 );
		pWndEdit->SetString("0");
	}	
}
BOOL CWndPostRead::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	CMailBox* pMailBox	= CMailBox::GetInstance();	
	CMailBox& mailbox = *pMailBox;
	
	CMail* pMail = mailbox[m_nMailIndex];

	if( !pMail )
		return FALSE;
	
	CWndPost* pWndPost = (CWndPost*)g_WndMng.GetWndBase(APP_POST);

	if( pWndPost == NULL )
		return FALSE;

	switch( nID )
	{
	case WIDC_BUTTON1:	
		{
//			SAFE_DELETE(m_pDeleteConfirm);

			if(pMail->m_pItemElem && pMail->m_pItemElem->m_dwItemId == II_SYS_SYS_SCR_SEALCHARACTER )
				return FALSE;

			m_pDeleteConfirm = new CWndPostDeleteConfirm;

			if( m_pDeleteConfirm )
			{
				m_pDeleteConfirm->Initialize(this);
				m_pDeleteConfirm->SetValue(m_nMailIndex);			
			}
		}
		break;
	case WIDC_BUTTON2:	
		{
			CWndEdit* pWndEdit1	= (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
			CWndEdit* pWndEdit2	= (CWndEdit*)GetDlgItem( WIDC_EDIT2 );
			
			if( pWndEdit1 && pWndEdit2 )
			{
				if( stricmp( pWndEdit1->GetString(), "Unknow" ) == 0 )
				{
					g_WndMng.OpenMessageBoxUpper( prj.GetText(TID_MAIL_UNKNOW) );
					return FALSE;
				}

				pWndPost->m_PostTabSend.SetItemId(0xff);
				pWndPost->m_PostTabSend.SetReceive("");
				pWndPost->m_PostTabSend.SetText("");
				pWndPost->m_PostTabSend.SetTitle("");
				
				CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)pWndPost->GetDlgItem( WIDC_TABCTRL1 );
				pWndTabCtrl->SetCurSel(0);	

				LPCSTR lpszPlayerString		= CPlayerDataCenter::GetInstance()->GetPlayerString( pMail->m_idSender );
				char	lpszPlayer[MAX_PLAYER]	= { 0, };
				if( pMail->m_idSender == 0 )
					lstrcpy( lpszPlayer, "FLYFF" );
				else
					lstrcpy( lpszPlayer, lpszPlayerString );

				pWndPost->m_PostTabSend.SetReceive( (char*)lpszPlayer );

				CString str = "Re:";
				str += pWndEdit2->GetString();
				pWndPost->m_PostTabSend.SetTitle(str.GetString());
				Destroy();
			}
		}
		break;
	case WIDC_BUTTON3:	
		{
			Destroy();
		}
		break;
	}
		
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndPostDeleteConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	CRect rect	= GetClientRect();
	rect.left	+= 8;
	rect.right	-= 4;
	rect.top	+= 8;
	rect.bottom	/= 2;

	CWndText* pWndText = (CWndText*)(GetDlgItem( WIDC_TEXT1 ));
	pWndText->SetString(_T( prj.GetText(TID_MAIL_DELETE_CONFIRM) ));	
	
	MoveParentCenter();
} 

BOOL CWndPostDeleteConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	LPWNDAPPLET lpWndApplet = m_resMng.GetAt ( APP_POST_DELETE_CONFIRM );
	CRect rect = CRect( 0, 0, lpWndApplet->size.cx, lpWndApplet->size.cy );
	
	return CWndNeuz::Create( WBS_MOVE | WBS_SOUND | WBS_CAPTION | WBS_MODAL, rect, pWndParent, APP_POST_DELETE_CONFIRM ); 
} 

BOOL CWndPostDeleteConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_YES )
	{
		CMailBox* pMailBox	= CMailBox::GetInstance();	
		CMailBox& mailbox = *pMailBox;
		
		CMail* pMail = mailbox[m_nIndex];
		
		if( pMail )
		{
			g_DPlay.SendQueryRemoveMail( pMail->m_nMail );						
			Destroy(TRUE);

			CWndPostRead* pWndPost = (CWndPostRead*)g_WndMng.GetWndBase(APP_POST_READ);
			if(pWndPost)
				pWndPost->Destroy();
			
		}
	}
	else if( nID == WIDC_NO )
	{
		Destroy( TRUE );
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}
///////////////////////////////////////////////////////////////////////////////////////////////////////

CWndPostReceive::~CWndPostReceive() 
{ 
#ifdef __FIX_WND_1109
	DeleteDeviceObjects();
#endif	// __FIX_WND_1109
} 
HRESULT CWndPostReceive::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();

	m_Texture[0].DeleteDeviceObjects();
	m_Texture[1].DeleteDeviceObjects();
	m_Texture[2].DeleteDeviceObjects();

#ifdef __FIX_WND_1109
	SAFE_DELETE( m_wndPostRead );
#endif	// __FIX_WND_1109

	return TRUE;
}
int CWndPostReceive::GetSelectIndex( const CPoint& point )
{
	int nBase = m_wndScrollBar.GetScrollPos();
	int nIndex = (point.y-23) / 40;
	
	if( 0 <= nIndex && nIndex < MAX_MAIL_LIST_PER_PAGE ) 
	{
		int nSelect = nBase + nIndex;
		if( 0 <= nSelect && nSelect < m_nMax )
			return nSelect;
	}
	return -1;
}
void CWndPostReceive::UpdateScroll()
{
	CMailBox* pMailBox	= CMailBox::GetInstance();
	m_nMax = pMailBox->GetSize();
	
	m_wndScrollBar.SetScrollFromSize(m_nMax, MAX_MAIL_LIST_PER_PAGE);
}
void CWndPostReceive::OnDraw( C2DRender* p2DRender ) 
{ 
	static constexpr DWORD dwColor = D3DCOLOR_XRGB(0, 0, 0);
	
	static constexpr int sx = 9;
	int sy = 30;
	
	CRect rc( sx, 5, sx+310, 7 ); 	
	rc += CPoint( 0, 20 );
	
	int nBase = std::max(m_wndScrollBar.GetScrollPos(), 0);
	
	TCHAR szTemp[32];

	CMailBox* pMailBox	= CMailBox::GetInstance();	
	CMailBox& mailbox = *pMailBox;

	p2DRender->RenderTexture( CPoint(sx+4, sy - 4), &m_Texture[0], 150  );

	// �Ⱓ ���� ��ȭ
	const D3DXVECTOR2 v2_1 = D3DXVECTOR2( 255, 0 );
	const D3DXVECTOR2 v2_2 = D3DXVECTOR2( 0, 150 );
	D3DXVECTOR2 v2Result;

	TCHAR szCutTitle[128];
	

	COleDateTime dt;
	CString strDateTime;

	for( int i=nBase; i<nBase + MAX_MAIL_LIST_PER_PAGE; ++i )
	{
		if( i >= m_nMax )	
			break;

		if( mailbox.GetSize() <= i )
			continue;
			
		if( mailbox[i] == NULL ) 
			continue;

		if( m_nSelect >= 0 && i == m_nSelect )
		{
			rc.SetRect( sx+4, sy - 4, sx+240, sy + 36 ); 	
			p2DRender->RenderFillRect( rc , D3DCOLOR_ARGB( 32, 190, 0 , 0 ) );
		}
		
		// �������� ������ ���?
		if( mailbox[i]->m_pItemElem && mailbox[i]->m_pItemElem->m_pTexture )
		{
			p2DRender->RenderTexture( CPoint( sx + 10,  sy+2 ), mailbox[i]->m_pItemElem->m_pTexture );

			if( mailbox[i]->m_pItemElem->GetProp()->dwPackMax > 1 )
			{
				CD3DFont* pOldFont = p2DRender->GetFont(); 
				p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndTitle ); 
				
				_stprintf( szTemp, "%d", mailbox[i]->m_pItemElem->m_nItemNum );
				CSize size	= p2DRender->m_pFont->GetTextExtent( szTemp );
				p2DRender->TextOut( sx+27,  sy+20 , szTemp, 0xff0000ff );
				p2DRender->TextOut( sx+27,  sy+20 , szTemp, 0xffb0b0f0 );
				
				p2DRender->SetFont( pOldFont );			
			}
		}
		else
		// �������� ���� ���� ������ �����? ���?
		if( mailbox[i]->m_pItemElem == NULL && mailbox[i]->m_nGold > 0 )
		{
			p2DRender->RenderTexture( CPoint( sx + 10,  sy+2 ), &m_Texture[2] );
		}

		// ������ �о��ٸ� ���� ǥ��
		if( mailbox[i]->m_byRead )
		{
			p2DRender->RenderTexture( CPoint( sx+10, sy+1 ), &m_Texture[1], 110 );
		}		

		const char* lpszPlayerString	= CPlayerDataCenter::GetInstance()->GetPlayerString( mailbox[i]->m_idSender );
		char lpszPlayer[MAX_PLAYER]	= { 0, };
		if( mailbox[i]->m_idSender == 0 )
			lstrcpy( lpszPlayer, "FLYFF" );
		else
			lstrcpy( lpszPlayer, lpszPlayerString );
		
		memset( szCutTitle, 0, sizeof(szCutTitle) );
		GetStrCut( lpszPlayer, szCutTitle, 16 );
		
		if( GetStrLen( lpszPlayer ) > 16 )
			_tcscat( szCutTitle, "..." );

		if( lstrlen( lpszPlayer ) > 0 )
		{
			p2DRender->TextOut( sx + 60,  sy+3, szCutTitle ,  D3DCOLOR_XRGB( 0, 0, 0 ) );
		}
		else
		{
			p2DRender->TextOut( sx + 60,  sy+3, "Unknown",  D3DCOLOR_XRGB( 0, 0, 0 ) );
		}

		const auto [nDay, dwTime] = mailbox[i]->GetMailInfo();  // �������� �˻�

		CString szDay;

		// �Ϸ� ���� ������ �ð����� ǥ�����ش�.
		if( nDay <= 1 )
		{
			szDay.Format( prj.GetText(TID_PK_LIMIT_HOUR), dwTime );
		}
		else
		{
			szDay.Format( prj.GetText(TID_PK_LIMIT_DAY), nDay );
		}
		
		FLOAT f = (FLOAT)( (FLOAT)nDay / (FLOAT)MAX_KEEP_BASIC_DAY );
		D3DXVec2Lerp( &v2Result, &v2_1, &v2_2, f );

		if( nDay <= 0 )
			p2DRender->TextOut( sx + 190,  sy+3, szDay,  D3DCOLOR_XRGB( (int)v2Result.x, (int)v2Result.y, 0 ) );			
		else
			p2DRender->TextOut( sx + 193,  sy+3, szDay,  D3DCOLOR_XRGB( (int)v2Result.x, (int)v2Result.y, 0 ) );			
		
		memset( szCutTitle, 0, sizeof(szCutTitle) );
		GetStrCut( mailbox[i]->m_szTitle, szCutTitle, 13 );
		
		if( GetStrLen( mailbox[i]->m_szTitle ) >= 13 )
		{
			_tcscat( szCutTitle, "..." );
		}
		
//		if( GetFocusWnd() == this )
		{
			CRect  rect;
			rect.SetRect( sx+4, sy - 4, sx+240, sy + 36 ); 	
			CPoint point	= GetMousePoint();
			if( PtInRect( rect, point ) )
			{
				ClientToScreen( &point );
				ClientToScreen( &rect );
				dt = mailbox[i]->m_tmCreate;
				strDateTime.Format( prj.GetText( TID_GAME_MAIL_RECEIVE_DATE ), dt.Format() );
				g_toolTip.PutToolTip( (DWORD)this, strDateTime, rect, point );
			}		
		}
		
		if( lstrlen( lpszPlayer ) > 0 )
			p2DRender->TextOut( sx + 60,  sy+18, szCutTitle,  D3DCOLOR_XRGB( 0, 0, 190 ) );
		else
			p2DRender->TextOut( sx + 60,  sy+18, "",  D3DCOLOR_XRGB( 0, 0, 190 ) );

		sy += 40;
	}	
} 

void CWndPostReceive::OnMouseWndSurface( CPoint point )
{
	int	sx, sy;
	
	sx = 15;
	sy = 30;
	
	CRect rc( sx, 5, sx+310, 7 ); 	
	rc += CPoint( 0, 20 );
	
	int nBase = m_wndScrollBar.GetScrollPos();
	if( nBase < 0 )
	{
		nBase = 0;
		TRACE("aa\n");
	}
	
	CMailBox* pMailBox	= CMailBox::GetInstance();	
	CMailBox& mailbox = *pMailBox;

	CString strDateTime;

	CPoint cpoint	= point;

	for( int i=nBase; i<nBase + MAX_MAIL_LIST_PER_PAGE; ++i )
	{
		if( i >= m_nMax )	
			break;

		if( mailbox.GetSize() <= i )
			continue;
			
		if( mailbox[i] == NULL ) 
			continue;

//		if( GetFocusWnd() == this )
		{			
			rc.SetRect( sx+4, sy - 4, sx+240, sy + 36 ); 	
			if( PtInRect( rc, cpoint ) )
			{
				ClientToScreen( &cpoint );
				ClientToScreen( &rc );
				COleDateTime dt = mailbox[i]->m_tmCreate;
				strDateTime.Format( prj.GetText( TID_GAME_MAIL_RECEIVE_DATE ), dt.Format() );
				g_toolTip.PutToolTip( (DWORD)this, strDateTime, rc, cpoint );
				break;
			}		
		}
		
		sy += 40;
	}	
}

void CWndPostReceive::OnLButtonDblClk( UINT nFlags, CPoint point )
{
	if( m_nMax <= 0 )
		return;

	int nSelect = GetSelectIndex( point );
	if( nSelect == -1 )
		return;
	
	m_nSelect = nSelect;	

	CMailBox* pMailBox	= CMailBox::GetInstance();	
	CMailBox& mailbox = *pMailBox;
	
	if( mailbox[m_nSelect] == NULL )
		return;

#ifdef __FIX_WND_1109
	SAFE_DELETE( m_wndPostRead );
#endif	// __FIX_WND_1109

	m_wndPostRead = new CWndPostRead;

	if( m_wndPostRead )
	{
		m_wndPostRead->Initialize( this );
		m_wndPostRead->SetValue( m_nSelect );
	}
}

void CWndPostReceive::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CRect rect = GetWindowRect();
	rect.top    += 15;
	rect.bottom -= 50;
	rect.left    = rect.right - 40;
	rect.right   -= 30;
	
	m_wndScrollBar.SetScrollFromSize(m_nMax, MAX_MAIL_LIST_PER_PAGE);	
	m_wndScrollBar.AddWndStyle( WBS_DOCKING );
	m_wndScrollBar.Create( WBS_VERT, rect, this, 1000 );
	
	// ������ �߾����� �ű��? �κ�.
	MoveParentCenter();
	
	m_Texture[0].LoadTexture(g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "WndPostTable.tga" ), 0xffff00ff );	
	m_Texture[1].LoadTexture(g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "WndNotUse.tga" ), 0xffff00ff );	
	m_Texture[2].LoadTexture(g_Neuz.m_pd3dDevice, MakePath( "item\\", "itm_GolGolSeed.dds" ), 0xffff00ff );	
	
} 

BOOL CWndPostReceive::Initialize(CWndBase * pWndParent, DWORD) {
	return CWndNeuz::InitDialog(APP_POST_RECEIVE, pWndParent, 0, CPoint(0, 0));
}

void CWndPostReceive::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
	if( m_nMax <= 0 )
		return;
	
	int nSelect = GetSelectIndex( point );
	if( nSelect != -1 )
		m_nSelect = nSelect;	
} 

BOOL CWndPostReceive::OnMouseWheel(UINT, short zDelta, CPoint) {
	m_wndScrollBar.MouseWheel(zDelta);
	return TRUE;
}
