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



CWndPost::~CWndPost() 
{ 
	CloseMailRequestingBox();
} 

void CWndPost::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	EnableWindow( FALSE );

	//	���⿡ �ڵ��ϸ� �˴ϴ�
	CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );
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
// ó�� �� �Լ��� �θ��� ������ ������.
BOOL CWndPost::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	return CWndNeuz::InitDialog( dwWndId, pWndParent, 0, CPoint( 0, 0 ) );
} 

BOOL CWndPost::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( message == WNM_SELCHANGE )
	{
		CWndTabCtrl* pWndTabCtrl = (CWndTabCtrl*)GetDlgItem( WIDC_TABCTRL1 );

		if (pWndTabCtrl->GetSelectedTab() == &m_PostTabReceive) {
			g_DPlay.SendQueryMailBox();
		}
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndPost::CloseMailRequestingBox() {
	SAFE_DELETE(m_pWndMailRequestingBox);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
// CWndPostSendMessageBox
//////////////////////////////////////////////////////////////////////////////
class CWndPostSendMessageBox : public CWndMessageBox
{ 
public: 
	CWndText	m_wndText;
	
	BYTE		m_nItem;
	short		m_nItemNum;
	int			m_nGold;
	char		m_szReceiver[MAX_NAME];
	char		m_szTitle[MAX_MAILTITLE];	
	char		m_szText[MAX_MAILTEXT];		
	void		SetValue( BYTE nItem, short nItemNum, char* lpszReceiver, int nGold, char* lpszTitle, char* lpszText );
	virtual BOOL Initialize( CWndBase* pWndParent = NULL, DWORD dwWndId = 0 );
	virtual BOOL OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ); 
}; 

BOOL CWndPostSendMessageBox::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	CString str;
	str.Format( prj.GetText(TID_MAIL_SEND_CONFIRM), MAX_KEEP_MAX_DAY );
	
	return CWndMessageBox::Initialize( str, 
		pWndParent, 
		MB_OKCANCEL );	
}

void CWndPostSendMessageBox::SetValue( BYTE nItem, short nItemNum, char* lpszReceiver, int nGold, char* lpszTitle, char* lpszText )
{
	m_nItem		= nItem;
	m_nItemNum	= nItemNum;
	strcpy( m_szReceiver, lpszReceiver );
	strcpy( m_szTitle, lpszTitle );
	strcpy( m_szText, lpszText );
	m_nGold		= nGold;
}

BOOL CWndPostSendMessageBox::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	if( nID == 1 )
	{
		if( g_pPlayer )
		{
			g_DPlay.SendQueryPostMail( m_nItem, m_nItemNum, m_szReceiver, m_nGold, m_szTitle, m_szText );			
			
			CWndPost * pWndPost = (CWndPost *)g_WndMng.GetWndBase( APP_POST );
			
			if( pWndPost )
			{
				pWndPost->m_PostTabSend.ClearData();
			}
		}
		
		Destroy();
	}
	else if( nID == 2 )
	{
		Destroy();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 



CWndPostSend::~CWndPostSend() 
{ 
	ClearData();
} 

void CWndPostSend::ClearData()
{
	CItemElem * pItemElem = g_pPlayer->m_Inventory.GetAtId( m_nItem );
	
	if(pItemElem)
	{
		// Ʈ���̵忡 �ɸ� �������� ����
		if( !g_pPlayer->m_vtInfo.IsTrading( pItemElem ) )
			pItemElem->SetExtra( 0 );
	}
	
	m_nItem		= 0xff;		
	m_nCost		= 0;
	m_nCount	= 0;
	/*
	CWndEdit* pWndEdit2	= (CWndEdit*)GetDlgItem( WIDC_EDIT2 );
	CWndEdit* pWndEdit3	= (CWndEdit*)GetDlgItem( WIDC_EDIT3 );
	CWndEdit* pWndEdit4	= (CWndEdit*)GetDlgItem( WIDC_EDIT4 );
	
	pWndEdit2->SetString("");
	pWndEdit3->SetString("");
	pWndEdit4->SetString("");
	*/
}

void CWndPostSend::SetReceive( const char* pchar )
{
	CWndComboBox* pWndCombo = (CWndComboBox*)GetDlgItem( WIDC_COMBOBOX1 );	
	pWndCombo->SetString( pchar );
}
void CWndPostSend::SetTitle( const char* pchar )
{
	CWndEdit* pWndEdit1	= (CWndEdit*)GetDlgItem( WIDC_EDIT2 );
	
	pWndEdit1->SetString( pchar );
}
void CWndPostSend::SetText( const char* pchar )
{
	CWndEdit* pWndEdit1	= (CWndEdit*)GetDlgItem( WIDC_EDIT3 );
	
	pWndEdit1->SetString( pchar );
}
void CWndPostSend::OnDraw( C2DRender* p2DRender ) 
{ 
	if( m_nItem != 0xff )
	{
		CItemElem* pItemElem = g_pPlayer->m_Inventory.GetAtId( m_nItem );
		
		if( pItemElem )
		{
			LPWNDCTRL pCustom = NULL;
			pCustom = GetWndCtrl( WIDC_STATIC3 );
			
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
			
			//*
			CRect hitrect = pCustom->rect;
			CPoint point = GetMousePoint();
			if( hitrect.PtInRect( point ) )
			{
				CPoint point2 = point;
				ClientToScreen( &point2 );
				ClientToScreen( &hitrect );
				
				g_WndMng.PutToolTip_Item( pItemElem, point2, &hitrect );
			}
			/**/
		}
	}
} 
void CWndPostSend::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 

	CWndEdit* pNotice = (CWndEdit*) GetDlgItem( WIDC_EDIT3 );
	pNotice->AddWndStyle( EBS_WANTRETURN );
	pNotice->AddWndStyle( EBS_AUTOVSCROLL );

	// �Ʒ��� �����? �������̴�. CWndEdit�� SetWndRect���� ��ũ�ѹ��� visible���¿� ���� �����ϰ� ����
	// �θ�Ŭ������ CWndText�� OnInitialUpdate ȣ��Ǿ�? ��ũ�ѹ��� visible���¸� �����Ѵ�. �׷��� SetWndRect�� ��ȣ��....�Ѥ�;;;; �������� �����ϰ� ���Ҵ�.
	// �������ϴ� �޽��� �޼��� â�� ���? OnSize���� SetWndRect�� ȣ���ؼ� ���������� �����ߴ� ��.
	LPWNDCTRL pWndCtrl = GetWndCtrl( WIDC_EDIT3 );
	if(pWndCtrl)
		pNotice->SetWndRect(pWndCtrl->rect);

	CWndComboBox* pWndCombo = (CWndComboBox*)GetDlgItem( WIDC_COMBOBOX1 );
	CWndEdit* pWndEdit2	= (CWndEdit*)GetDlgItem( WIDC_EDIT2 );
	CWndEdit* pWndEdit3	= (CWndEdit*)GetDlgItem( WIDC_EDIT3 );
	
	pWndCombo->SetTabStop( TRUE );
	pWndEdit2->SetTabStop( TRUE );
	pWndEdit3->SetTabStop( TRUE );


	for (const u_long idPlayer : g_WndMng.m_RTMessenger | std::views::keys) {
		pWndCombo->AddString(CPlayerDataCenter::GetInstance()->GetPlayerString(idPlayer));
	}

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
// ó�� �� �Լ��� �θ��� ������ ������.
BOOL CWndPostSend::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	return CWndNeuz::InitDialog( APP_POST_SEND, pWndParent, 0, CPoint( 0, 0 ) );
} 

void CWndPostSend::SetItemId( BYTE nId )
{ 
	// ���� �������� �־����� Ȯ�� ����Ÿ�� �ʱ�ȭ �Ѵ�.
	if( nId != m_nItem && m_nItem != 0xff )
	{
		CItemElem* pItemElem  = g_pPlayer->m_Inventory.GetAtId( m_nItem );
		if( pItemElem )
		{
			pItemElem->SetExtra( 0 );				
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
		if( pItemElem->m_nItemNum > 1 )
		{ 
			SetItemId( (BYTE)( pItemElem->m_dwObjId ) );

			g_WndMng.m_pWndTradeGold = new CWndTradeGold;
			memcpy( &g_WndMng.m_pWndTradeGold->m_Shortcut, pShortcut, sizeof(SHORTCUT) );
			g_WndMng.m_pWndTradeGold->m_dwGold = pItemElem->m_nItemNum;
			g_WndMng.m_pWndTradeGold->m_nIdWndTo = APP_POST_SEND; // �κ��丮 �ε� ���� �־ Gold�� ��.
			g_WndMng.m_pWndTradeGold->m_pWndBase = this;
			g_WndMng.m_pWndTradeGold->m_nSlot = 0;
			
			g_WndMng.m_pWndTradeGold->Initialize( &g_WndMng, APP_TRADE_GOLD );
			g_WndMng.m_pWndTradeGold->AddWndStyle( WBS_MODAL );
			g_WndMng.m_pWndTradeGold->MoveParentCenter();

			CWndStatic* pStatic	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_STATIC );
			CWndStatic* pStaticCount	= (CWndStatic *)g_WndMng.m_pWndTradeGold->GetDlgItem( WIDC_CONTROL1 );
			CString strMain = prj.GetText( TID_GAME_MOVECOUNT );//"��� �̵��Ͻðڽ��ϱ�?";
			CString strCount = prj.GetText(TID_GAME_NUMCOUNT);//" ���� : ";
			pStatic->m_strTitle = strMain;
			pStaticCount->m_strTitle = strCount;
			g_WndMng.m_pWndTradeGold->SetTitle("");

			if( pItemElem->GetProp() )
			{
				CWndEdit* pWndEdit2	= (CWndEdit*)GetDlgItem( WIDC_EDIT2 );

				if( stricmp( pWndEdit2->GetString(), pItemElem->GetProp()->szName ) != 0 )
					pWndEdit2->SetString(pItemElem->GetProp()->szName);
			}
		}
		else
		{
			SetItemId( (BYTE)( pItemElem->m_dwObjId ) );
			pItemElem->SetExtra( 1 );				

			if( pItemElem->GetProp() )
			{
				CWndEdit* pWndEdit2	= (CWndEdit*)GetDlgItem( WIDC_EDIT2 );

				if( stricmp( pWndEdit2->GetString(), pItemElem->GetProp()->szName ) != 0 )
					pWndEdit2->SetString(pItemElem->GetProp()->szName);
			}			
		}
	}		
			
	return TRUE;
}

void CWndPostSend::OnRButtonUp( UINT nFlags, CPoint point )
{
	LPWNDCTRL pCustom = NULL;
	pCustom = GetWndCtrl( WIDC_STATIC3 );
	
	// ������( ���?, ���ⱸ )
	if( PtInRect(&(pCustom->rect), point) )
	{
		CItemElem* pItemElem = g_pPlayer->m_Inventory.GetAtId( m_nItem );
		
		if(pItemElem)
		{
			pItemElem->SetExtra( 0 );				
		}

		m_nItem = 0xff;	
	}		
}

void CWndPostSend::AdditionalSkinTexture( LPWORD pDest, CSize sizeSurface, D3DFORMAT d3dFormat )
{
	CWndNeuz::AdditionalSkinTexture( pDest, sizeSurface, d3dFormat );
}


#define MAX_BYTE_POST		128

BOOL CWndPostSend::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
	case WIDC_SEND:	
		{
			CWndComboBox* pWndCombo = (CWndComboBox*)GetDlgItem( WIDC_COMBOBOX1 );
			CWndEdit* pWndEdit2	= (CWndEdit*)GetDlgItem( WIDC_EDIT2 );
			CWndEdit* pWndEdit3	= (CWndEdit*)GetDlgItem( WIDC_EDIT3 );
			CWndEdit* pWndEdit4	= (CWndEdit*)GetDlgItem( WIDC_EDIT4 );
			
			LPCTSTR szstr1 = pWndCombo->GetString();
			LPCTSTR szstr2 = pWndEdit2->GetString();
			LPCTSTR szstr3 = pWndEdit3->GetString();
			LPCTSTR szstr4 = pWndEdit4->GetString();
			
			// �����? �޴»����? ������ ������
			if( strlen( szstr1 ) <=0 || strlen( szstr2 ) <=0 )
			{
				return FALSE;
			}
			
			// �����۰� ���? �ΰ��� �ϳ��� ��� ������
			if( m_nItem == NULL_ID && strlen( szstr4 ) <=0 )
			{
				return FALSE;
			}

			// �ݾ׶��� ���ڰ� �ƴϸ� ������
			int nlen = strlen(szstr4);
			BOOL bisdigit = TRUE;

			if( nlen > 0 )
			{
				for( int i = 0 ; i < nlen ; i++ )
				{
					if( (isdigit2( szstr4[i] ) == FALSE) )
					{
						bisdigit = FALSE;
						break;
					}
				}
			}
			
			if( bisdigit == FALSE )
			{
				//g_WndMng.OpenMessageBoxUpper( prj.GetText(TID_GAME_GUILDONLYNUMBER) );
				return FALSE;
			}

			__int64 liGold	= _atoi64(szstr4);
			if( liGold > 1000000000 )
			{
				g_WndMng.OpenMessageBoxUpper( prj.GetText(TID_GAME_LIMITPENYA) );
				return FALSE;				
			}
			
			if( strlen(szstr1) < 3 || strlen(szstr1) > 16 )
			{
				//�̸��� �ʹ� ��ϴ�?. �ٽ� �Է����ֽʽÿ�.
				g_WndMng.OpenMessageBoxUpper( prj.GetText(TID_DIAG_0057) );
				return FALSE;				
			}

			if( strlen(szstr3) > ( MAX_MAILTEXT - 1 ) )		//gmpbigsun: ������ �������� 
			{
				CString str;
				str.Format( prj.GetText(TID_GAME_MAILBOX_TEXT_MAX), MAX_MAILTEXT );
				g_WndMng.OpenMessageBoxUpper( str );
				return FALSE;				
			}

			if( strlen(szstr2) > ( MAX_MAILTITLE - 1 ) )	//gmpbigsun: ������ ��������
			{
				CString str;
				str.Format( prj.GetText(TID_GAME_MAILBOX_TITLE_MAX), MAX_MAILTITLE );
				g_WndMng.OpenMessageBoxUpper( str );
				return FALSE;				
			}

			CWndPostSendMessageBox* pBox = new CWndPostSendMessageBox;

			if( pBox )
			{
				g_WndMng.OpenCustomBox( "", pBox );
				
				CItemElem* pItemElem = g_pPlayer->m_Inventory.GetAtId( m_nItem );
				
				if( pItemElem )
				{
					pBox->SetValue( m_nItem, pItemElem->GetExtra(), (char*)szstr1, atoi(szstr4), (char*)szstr2, (char*)szstr3 );
				}
				else
				{
					pBox->SetValue( -1, 0, (char*)szstr1, atoi(szstr4), (char*)szstr2, (char*)szstr3 );
				}
			}
				
			break;
		}
	}
		
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 


///////////////////////////////////////////////////////////////////////////////////////////////////////
CWndPostRead::CWndPostRead():m_nMailIndex(-1), m_bDrag(0), m_pWndPostItemWarning(0)
{ 
	m_pDeleteConfirm = NULL;
} 
CWndPostRead::~CWndPostRead() 
{ 
	ClearData();
//	SAFE_DELETE(m_pWndPostItemWarning);
} 

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

	CWndButton* pWndButton = (CWndButton*)GetDlgItem( WIDC_BUTTON3 );
	

	pWndEdit1->EnableWindow(FALSE);
	pWndEdit2->EnableWindow(FALSE);
	pWndEdit3->EnableWindow(FALSE);
//	pWndText->EnableWindow(FALSE);

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

BOOL CWndPostRead::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 
void CWndPostRead::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
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
	
	LPWNDCTRL pCustom = NULL;
	pCustom = GetWndCtrl( WIDC_STATIC3 );
	
	// ������( ���?, ���ⱸ )
	if( PtInRect(&(pCustom->rect), point) )
	{
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

	CWndEdit* pWndEdit1	= (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
	CWndEdit* pWndEdit2	= (CWndEdit*)GetDlgItem( WIDC_EDIT2 );
	CWndEdit* pWndEdit3	= (CWndEdit*)GetDlgItem( WIDC_EDIT3 );
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );

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
			m_GlobalShortcut.m_dwData     = (DWORD) pMail->m_pItemElem;
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

	LPWNDCTRL pCustom = NULL;
	pCustom = GetWndCtrl( WIDC_STATIC3 );
	
	// ������( ���?, ���ⱸ )
	if( PtInRect(&(pCustom->rect), point) )
	{
		MailReceiveItem();
	}
	
	pCustom = GetWndCtrl( WIDC_STATIC4 );

	if( PtInRect(&(pCustom->rect), point) )
	{
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
			FLOAT fCustody = 0.0f;
			FLOAT fPay = 0.0f;
			fCustody = (FLOAT)( (FLOAT)( MAX_KEEP_MAX_DAY - MAX_KEEP_BASIC_DAY - nDay ) / (FLOAT)( MAX_KEEP_MAX_DAY - MAX_KEEP_BASIC_DAY ) );

			m_bDrag = FALSE;
//			SAFE_DELETE(m_pWndPostItemWarning);
			m_pWndPostItemWarning = new CWndPostItemWarning;
			m_pWndPostItemWarning->Initialize( this, 0 );
			m_pWndPostItemWarning->SetIndex( pMail->m_nMail );

			fPay = pMail->m_pItemElem->GetCost() * fCustody;
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
				pWndPost->m_PostTabSend.SetTitle(str.GetBuffer(0));
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
	
	if( m_nMax > 0 )
	{
		m_wndScrollBar.SetScrollRange( 0, m_nMax );
		if( m_nMax < MAX_MAIL_LIST_PER_PAGE )
			m_wndScrollBar.SetScrollPage( m_nMax );
		else
			m_wndScrollBar.SetScrollPage( MAX_MAIL_LIST_PER_PAGE );
	}
	else
	{
		m_wndScrollBar.SetScrollRange( 0, 1 );
		m_wndScrollBar.SetScrollPage( 1 );
	}	
}
void CWndPostReceive::OnDraw( C2DRender* p2DRender ) 
{ 
	const DWORD dwColor = D3DCOLOR_XRGB(0, 0, 0);
	int	sx, sy;
	
	sx = 9;
	sy = 30;
	
	CRect rc( sx, 5, sx+310, 7 ); 	
	rc += CPoint( 0, 20 );
	
	int nBase = m_wndScrollBar.GetScrollPos();
	if( nBase < 0 )
	{
		nBase = 0;
		TRACE("aa\n");
	}
	
	TCHAR szTemp[32];

	CMailBox* pMailBox	= CMailBox::GetInstance();	
	CMailBox& mailbox = *pMailBox;

	p2DRender->RenderTexture( CPoint(sx+4, sy - 4), &m_Texture[0], 150  );

	// �Ⱓ ���� ��ȭ
	D3DXVECTOR2 v2_1 = D3DXVECTOR2( 255, 0 );
	D3DXVECTOR2 v2_2 = D3DXVECTOR2( 0, 150 );
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

	COleDateTime dt;
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
				dt = mailbox[i]->m_tmCreate;
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
#ifdef __FIX_WND_1109
		m_wndPostRead->Initialize( this );
#else	// __FIX_WND_1109
		m_wndPostRead->Initialize();
#endif	// __FIX_WND_1109
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
	
	if( m_nMax > 0 )
	{
		m_wndScrollBar.SetScrollRange( 0, m_nMax );
		if( m_nMax < MAX_MAIL_LIST_PER_PAGE )
			m_wndScrollBar.SetScrollPage( m_nMax );
		else
			m_wndScrollBar.SetScrollPage( MAX_MAIL_LIST_PER_PAGE );
	}
	else
	{
		m_wndScrollBar.SetScrollRange( 0, 1 );
		m_wndScrollBar.SetScrollPage( 1 );
	}
	
	m_wndScrollBar.AddWndStyle( WBS_DOCKING );
	m_wndScrollBar.Create( WBS_VERT, rect, this, 1000 );
	
	// ������ �߾����� �ű��? �κ�.
	MoveParentCenter();
	
	m_Texture[0].LoadTexture(g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "WndPostTable.tga" ), 0xffff00ff );	
	m_Texture[1].LoadTexture(g_Neuz.m_pd3dDevice, MakePath( DIR_THEME, "WndNotUse.tga" ), 0xffff00ff );	
	m_Texture[2].LoadTexture(g_Neuz.m_pd3dDevice, MakePath( "item\\", "itm_GolGolSeed.dds" ), 0xffff00ff );	
	
} 
// ó�� �� �Լ��� �θ��� ������ ������.
BOOL CWndPostReceive::Initialize( CWndBase* pWndParent, DWORD dwWndId ) 
{ 
	return CWndNeuz::InitDialog( APP_POST_RECEIVE, pWndParent, 0, CPoint( 0, 0 ) );
} 

void CWndPostReceive::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
	if( m_nMax <= 0 )
		return;
	
	int nSelect = GetSelectIndex( point );
	if( nSelect != -1 )
		m_nSelect = nSelect;	
} 
BOOL CWndPostReceive::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

BOOL CWndPostReceive::OnMouseWheel( UINT nFlags, short zDelta, CPoint pt )
{
	if( zDelta < 0 )
	{
		if( m_wndScrollBar.GetMaxScrollPos() - m_wndScrollBar.GetScrollPage() > m_wndScrollBar.GetScrollPos() )
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetScrollPos()+1 );
		else
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetMaxScrollPos() - m_wndScrollBar.GetScrollPage() );
	}
	else
	{
		if( m_wndScrollBar.GetMinScrollPos() < m_wndScrollBar.GetScrollPos() )
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetScrollPos()-1 );
		else
			m_wndScrollBar.SetScrollPos( m_wndScrollBar.GetMinScrollPos() );
	}
	
	return TRUE;
}