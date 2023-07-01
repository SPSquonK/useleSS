#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndManager.h"
#include "WndSecretRoom.h"
#include "playerdata.h"
#include "SecretRoom.h"
#include "DPClient.h"
#include "WndParty.h"

#include "Tax.h"

#include "PlayerLineup.h"


//////////////////////////////////////////////////////////////////////////
// 비밀의 방 참가자 구성
//////////////////////////////////////////////////////////////////////////

void CWndSecretRoomSelection::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();

	ReplaceListBox<PlayerLineup, PlayerLineup::SimpleDisplayer  >(WIDC_LISTBOX1);
	ReplaceListBox<PlayerLineup, PlayerLineup::NumberedDisplayer>(WIDC_LISTBOX2);

	MoveParentCenter();

	ResetLineup({});
}

BOOL CWndSecretRoomSelection::Initialize(CWndBase * pWndParent, DWORD) {
	return CWndNeuz::InitDialog(APP_SECRETROOM_SELECTION, pWndParent, 0, CPoint(0, 0));
}

BOOL CWndSecretRoomSelection::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_BUTTON1 ) // 출전자 등록
	{
		const PlayerLineup::RuleSet ruleSet{
			.maxSelect = static_cast<size_t>(CSecretRoomMng::GetInstance()->m_nMaxGuildMemberNum),
			.minimumLevel = static_cast<unsigned int>(CSecretRoomMng::GetInstance()->m_nMinGuildMemberNum)
		};

		const auto result = PlayerLineup::DoubleListManager(
			GetDlgItem<CWndTListBox<PlayerLineup, PlayerLineup::SimpleDisplayer  >>(WIDC_LISTBOX1),
			GetDlgItem<CWndTListBox<PlayerLineup, PlayerLineup::NumberedDisplayer>>(WIDC_LISTBOX2)
		).ToSelect(ruleSet);

		switch (result) {
			using enum PlayerLineup::SelectReturn;
			case FullLineup:
			{
				CString str;
				str.Format(prj.GetText(TID_GAME_SECRETROOM_SELECTION_MAX), CSecretRoomMng::GetInstance()->m_nMaxGuildMemberNum);
				g_WndMng.OpenMessageBox(str);
				break;
			}
			case NotAMember:
				g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_SECRETROOM_NOT_GUILD_MEMBER));	//비밀의 방에 참가하는 길드의 맴버가 아닙니다.			
				break;
			case TooLowLevel:
				g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_SECRETROOM_LIMIT_LEVEL)); //출전자 등록은 레벨 30이상이 되어야 합니다.
				break;
			case AlreadyInLineup:
				g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_SECRETROOM_ALREADY_ENTRY)); //이미 등록되어 있습니다. 다시 등록해주세요.
				break;
		}		
	}
	else if( nID == WIDC_BUTTON2 ) // 출전자 취소
	{
		auto * pWndLineup = GetDlgItem<CWndTListBox<PlayerLineup, PlayerLineup::NumberedDisplayer>>(WIDC_LISTBOX2);
		PlayerLineup * selected = pWndLineup->GetCurSelItem();
		if (selected) {
			CGuild * pGuild = g_pPlayer->GetGuild();
			CGuildMember * pGuildMemberl = pGuild->GetMember(selected->playerId);
			
			if (pGuildMemberl->m_nMemberLv == GUD_MASTER) {
				g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_SECRETROOM_DONTREMOVE_GUILDMASTER));
			} else {
				PlayerLineup::DoubleListManager(
					GetDlgItem<CWndTListBox<PlayerLineup, PlayerLineup::SimpleDisplayer  >>(WIDC_LISTBOX1),
					pWndLineup
				).ToGuild();
			}
		}
	}
	else if( nID == WIDC_RESET )
	{
		CWndSecretRoomSelectionResetConfirm* pBox = new CWndSecretRoomSelectionResetConfirm;
		g_WndMng.OpenCustomBox( "", pBox );
	}
	else if( nID == WIDC_FINISH )
	{
		auto * pWndLineup = GetDlgItem<CWndTListBox<PlayerLineup, PlayerLineup::NumberedDisplayer>>(WIDC_LISTBOX2);
		size_t nbInLineup = pWndLineup->GetSize();

		if( CSecretRoomMng::GetInstance()->m_nMinGuildMemberNum > (int)(nbInLineup) )
		{
			CString str;
			str.Format( prj.GetText(TID_GAME_SECRETROOM_MIN_PLAYER), CSecretRoomMng::GetInstance()->m_nMinGuildMemberNum );
			g_WndMng.OpenMessageBox( str );
			return FALSE;
		}

		if(nbInLineup == 0 )
		{
			g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_SECRETROOM_HAVENOT_PLAYER) ); //출전자가 없습니다. 출전자를 선택해주세요.
			return FALSE;
		}

		if(CGuild * pGuild = g_pPlayer->GetGuild())
		{
			bool hasGuildMaster = false;

			std::vector<u_long> lineup;
			for (int i = 0; i < pWndLineup->GetSize(); ++i) {
				const PlayerLineup & pl = (*pWndLineup)[i];

				hasGuildMaster = hasGuildMaster
					|| (pGuild->GetMember(pl.playerId)->m_nMemberLv == GUD_MASTER);

				lineup.emplace_back(pl.playerId);
			}

			if(hasGuildMaster)
			{
				g_DPlay.SendSecretRoomLineUpMember(lineup);
				Destroy();
			}
			else
			{
				g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_SECRETROOM_HAVENOT_MASTER) ); //출전자 명단에 길드마스터나 킹핀이 존재하지 않습니다.
				return FALSE;
			}
		}
	}
	else if( nID == WIDC_CLOSE )
	{
		Destroy();
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}


void CWndSecretRoomSelection::ResetLineup(std::span<const u_long> lineup) {
	PlayerLineup::DoubleListManager(
		GetDlgItem<CWndTListBox<PlayerLineup, PlayerLineup::SimpleDisplayer  >>(WIDC_LISTBOX1),
		GetDlgItem<CWndTListBox<PlayerLineup, PlayerLineup::NumberedDisplayer>>(WIDC_LISTBOX2)
	).Reset(lineup);
}

//////////////////////////////////////////////////////////////////////////
// 비밀의 방 신청하기
//////////////////////////////////////////////////////////////////////////

CWndSecretRoomOffer::CWndSecretRoomOffer() 
{
	m_dwMinGold    = 0;
	m_dwBackupGold = 0;
	m_dwReqGold    = 0;
}

CWndSecretRoomOffer::~CWndSecretRoomOffer() 
{
}

void CWndSecretRoomOffer::OnDraw( C2DRender* p2DRender ) 
{	
}

void CWndSecretRoomOffer::EnableAccept( BOOL bFlag )
{
	CWndButton* pWndButton = (CWndButton*)GetDlgItem( WIDC_ACCEPT );

	if( pWndButton )
	{
		pWndButton->EnableWindow( bFlag );
	}
}

void CWndSecretRoomOffer::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	CWndText::SetupDescription(
		GetDlgItem<CWndText>(WIDC_TEXT1),
		_T("SecretRoomDesc.inc")
	);
	
	MoveParentCenter();
} 

BOOL CWndSecretRoomOffer::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( APP_SECRETROOM_OFFER, pWndParent, 0, CPoint( 0, 0 ) );
} 

BOOL CWndSecretRoomOffer::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndSecretRoomOffer::OnSize( UINT nType, int cx, int cy )
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndSecretRoomOffer::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

void CWndSecretRoomOffer::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

void CWndSecretRoomOffer::SetGold( DWORD nCost )
{
	CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );

	if( pWndEdit )
	{
		CString str;
		str.Format( "%d", nCost );
		pWndEdit->SetString(str);
	}				
}

BOOL CWndSecretRoomOffer::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_ACCEPT )
	{
		if( g_pPlayer )
		{
			CWndEdit* pWndEdit = (CWndEdit*)GetDlgItem( WIDC_EDIT1 );
			
			DWORD nCost;
			CString str = pWndEdit->GetString();
			nCost = atoi( str );

			if( m_dwReqGold != 0 )
			{
				if( nCost <= m_dwBackupGold )
				{
					g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GUILDCOMBAT1TO1_MORE_CURRENT_REQUEST) ); //기존 페냐보다 더 많은 금액으로 신청을 하시기 바랍니다.
					return FALSE;
				}
				
			}
			else
			{
				if( nCost < m_dwMinGold )
				{
					g_WndMng.OpenMessageBox( prj.GetText(TID_GAME_GUILDCOMBAT1TO1_LIMIT_MIN) ); //최소금액보다 더 많은 페냐로 신청하시기 바랍니다.
					return FALSE;
				}
			}

			CWndSecretRoomOfferMessageBox* pMsg = new CWndSecretRoomOfferMessageBox;
			if( pMsg )
			{
				g_WndMng.OpenCustomBox( "", pMsg, this );
				CString str;

				if( m_dwReqGold == 0 )
				{
					str.Format( prj.GetText(TID_GAME_GUILDCOMBAT1TO1_MORE_REQUEST), 0, nCost ); //기존에 신청된 %d페냐에서 추가로 %d페냐를 신청하겠습니까?
				}
				else
				{
					str.Format( prj.GetText(TID_GAME_GUILDCOMBAT1TO1_MORE_REQUEST), m_dwBackupGold, nCost-m_dwBackupGold ); //기존에 신청된 %d페냐에서 추가로 %d페냐를 신청하겠습니까?
				}

				pMsg->SetValue( str, nCost );
			}
		}
	}
	else if( nID == WIDC_CLOSE )
	{
		Destroy();
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

//////////////////////////////////////////////////////////////////////////
// 비밀의 방 세율 변경 창
//////////////////////////////////////////////////////////////////////////
CWndSecretRoomChangeTaxRate::CWndSecretRoomChangeTaxRate()
{
	m_nDefaultSalesTax = 0;
	m_nDefaultPurchaseTax = 0;
	m_nChangeSalesTax = 0;
	m_nChangePurchaseTax = 0;
	m_nMinTax = 0;
	m_nMaxTax = 0;
	m_nCont = 0;
}

CWndSecretRoomChangeTaxRate::~CWndSecretRoomChangeTaxRate()
{

}

void CWndSecretRoomChangeTaxRate::OnDraw( C2DRender* p2DRender ) 
{	
}

void CWndSecretRoomChangeTaxRate::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	CWndStatic* pStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC4 );
	CString strTex;
	strTex.Format("%d%%", m_nDefaultSalesTax);
	pStatic->SetTitle(strTex);

	pStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC6 );
	strTex.Format("%d%%", m_nDefaultPurchaseTax);
	pStatic->SetTitle(strTex);
	
	pStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC5 );
	strTex.Format("%d%%", m_nChangeSalesTax);
	pStatic->SetTitle(strTex);

	pStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC7 );
	strTex.Format("%d%%", m_nChangePurchaseTax);
	pStatic->SetTitle(strTex);

	// 세율 변경 안내 설정
	CWndText::SetupDescription(
		GetDlgItem<CWndText>(WIDC_TEXT1),
		_T("SecretRoomChangeTaxRate.inc")
	);

	MoveParentCenter();
} 

BOOL CWndSecretRoomChangeTaxRate::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( APP_SECRETROOM_TAXRATE_CHANGE, pWndParent, 0, CPoint( 0, 0 ) );
}

BOOL CWndSecretRoomChangeTaxRate::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
	switch(nID)
	{
		case WIDC_PLUS_STAX:
			{
				m_nChangeSalesTax++;
				if(m_nChangeSalesTax > m_nMaxTax)
					m_nChangeSalesTax = m_nMaxTax;
			}
			break;
		case WIDC_MINUS_STAX:
			{
				m_nChangeSalesTax--;
				if(m_nChangeSalesTax < m_nMinTax)
					m_nChangeSalesTax = m_nMinTax;
			}
			break;
		case WIDC_PLUS_PTAX:
			{
				m_nChangePurchaseTax++;
				if(m_nChangePurchaseTax > m_nMaxTax)
					m_nChangePurchaseTax = m_nMaxTax;
			}
			break;
		case WIDC_MINUS_PTAX:
			{
				m_nChangePurchaseTax--;
				if(m_nChangePurchaseTax < m_nMinTax)
					m_nChangePurchaseTax = m_nMinTax;
			}
			break;
		case WIDC_OK:
			{
				CWndSecretRoomChangeTaxRateMsgBox* pMsg = new CWndSecretRoomChangeTaxRateMsgBox;
				if( pMsg )
				{
					CString strMsg;
					g_WndMng.OpenCustomBox( "", pMsg, this );
					strMsg.Format( prj.GetText(TID_GAME_SECRETROOM_CHANGETEX), m_nChangeSalesTax, m_nChangePurchaseTax );
					pMsg->SetValue( strMsg,	m_nChangeSalesTax, m_nChangePurchaseTax, m_nCont );
				}
			}
			break;
	}

	if(nID == WIDC_PLUS_STAX || nID == WIDC_MINUS_STAX)
	{
		CWndStatic* pStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC5 );
		CString strTex;
		strTex.Format("%d%%", m_nChangeSalesTax);
		pStatic->SetTitle(strTex);
	}
	else if(nID == WIDC_PLUS_PTAX || nID == WIDC_MINUS_PTAX)
	{
		CWndStatic* pStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC7 );
		CString strTex;
		strTex.Format("%d%%", m_nChangePurchaseTax);
		pStatic->SetTitle(strTex);
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

void CWndSecretRoomChangeTaxRate::SetDefaultTax(int nMinTax, int nMaxTax, BYTE nCont)
{
	m_nCont = nCont;
	m_nDefaultSalesTax		= (int)( CTax::GetInstance()->GetSalesTaxRate( nCont ) * 100 );
	m_nDefaultPurchaseTax	= (int)( CTax::GetInstance()->GetSalesTaxRate( nCont ) * 100 );
	
	if(m_nDefaultSalesTax < nMinTax)
		m_nChangeSalesTax = nMinTax;
	else
		m_nChangeSalesTax = m_nDefaultSalesTax;

	if(m_nDefaultPurchaseTax < nMinTax)
		m_nChangePurchaseTax = nMinTax;
	else
		m_nChangePurchaseTax = m_nDefaultPurchaseTax;
	
	m_nMinTax = nMinTax;
	m_nMaxTax = nMaxTax;
}

//////////////////////////////////////////////////////////////////////////
// 비밀의 방 세율 확인 창
//////////////////////////////////////////////////////////////////////////
CWndSecretRoomCheckTaxRate::CWndSecretRoomCheckTaxRate()
{
	m_nSalesTax = 0;
	m_nPurchaseTax = 0;
	m_dwGuildId = 0;
}

CWndSecretRoomCheckTaxRate::~CWndSecretRoomCheckTaxRate()
{

}

void CWndSecretRoomCheckTaxRate::OnDraw( C2DRender* p2DRender ) 
{	
}

void CWndSecretRoomCheckTaxRate::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	// 세율 및 점령길드 설정
	BYTE nCont = CTax::GetInstance()->GetContinent( g_pPlayer );

	m_nSalesTax		= (int)( CTax::GetInstance()->GetSalesTaxRate( g_pPlayer ) * 100 );
	m_nPurchaseTax	= (int)( CTax::GetInstance()->GetPurchaseTaxRate( g_pPlayer ) * 100 );
	__TAXINFO* taxInfo = CTax::GetInstance()->GetTaxInfo( nCont );
	if( taxInfo )
		m_dwGuildId = taxInfo->dwId;
	else
		m_dwGuildId = NULL_ID;

	CWndStatic* pStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC3 );
	CString strTex;
	strTex.Format("%d%%", m_nSalesTax);
	pStatic->SetTitle(strTex);

	pStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC4 );
	strTex.Format("%d%%", m_nPurchaseTax);
	pStatic->SetTitle(strTex);

	CGuild *pGuild = g_GuildMng.GetGuild( m_dwGuildId );
		
	CString strFormat;
	if(pGuild)
	{
		strFormat = strings::CStringMaxSize(pGuild->m_szGuild, 16);
		pStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC8 );
		pStatic->SetTitle(strFormat);

		strFormat = strings::CStringMaxSize(CPlayerDataCenter::GetInstance()->GetPlayerString( pGuild->m_idMaster ), 16);

		pStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC9 );
		pStatic->SetTitle(strFormat);
	}
	else
	{
		pStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC8 );
		pStatic->SetTitle(prj.GetText(TID_GAME_SECRETROOM_TEX_NOGUILD));
		pStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC9 );
		pStatic->SetTitle(prj.GetText(TID_GAME_SECRETROOM_TEX_NOGUILD));
	}

	// 세율 확인 안내 설정
	CWndText::SetupDescription(
		GetDlgItem<CWndText>(WIDC_TEXT1),
		_T("SecretRoomCheckTaxRate.inc")
	);

	MoveParentCenter();
} 

BOOL CWndSecretRoomCheckTaxRate::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( APP_SECRETROOM_TAXRATE_CHECK, pWndParent, 0, CPoint( 0, 0 ) );
}

BOOL CWndSecretRoomCheckTaxRate::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
	if(nID == WIDC_OK)
		Destroy();

	return CWndNeuz::OnChildNotify( message, nID, pLResult );
}

//////////////////////////////////////////////////////////////////////////
// 비밀의 방 참가자 구성 확인 창
//////////////////////////////////////////////////////////////////////////

BOOL CWndSecretRoomSelectionResetConfirm::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	return CWndMessageBox::Initialize( prj.GetText(TID_GAME_SECRETROOM_REMAKE_MAKEUP), //명단작성을 다시 하시겠습니까?
		pWndParent, 
		MB_OKCANCEL );
}

BOOL CWndSecretRoomSelectionResetConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( nID )
	{
		case IDOK:
			{
				CWndSecretRoomSelection *pWndGuildSecretRoomSelection = (CWndSecretRoomSelection*)g_WndMng.GetWndBase( APP_SECRETROOM_SELECTION );

				if( pWndGuildSecretRoomSelection )
					pWndGuildSecretRoomSelection->ResetLineup();

				Destroy();
			}
			break;
		case IDCANCEL:
			Destroy();
			break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

//////////////////////////////////////////////////////////////////////////
// 비밀의 방 참가 길드 현황
//////////////////////////////////////////////////////////////////////////
CWndSecretRoomOfferState::CWndSecretRoomOfferState()
{ 
	Init( 0 );

	m_tEndTime = 0;
	m_tCurrentTime = 0;
}

CWndSecretRoomOfferState::~CWndSecretRoomOfferState() 
{ 
} 

void CWndSecretRoomOfferState::Init( time_t lTime )
{
}

void CWndSecretRoomOfferState::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();
	MoveParentCenter();
}

void CWndSecretRoomOfferState::InsertTitle( const char szTitle[] )
{
	CString strTitle;
	strTitle.Format( "%s - %s", GetTitle(), szTitle );
	SetTitle( strTitle );
}

BOOL CWndSecretRoomOfferState::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{
	return CWndNeuz::InitDialog( APP_SECRETROOM_OFFERSTATE, pWndParent, 0, CPoint( 0, 0 ) );
} 

BOOL CWndSecretRoomOfferState::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndSecretRoomOfferState::OnSize( UINT nType, int cx, int cy )
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndSecretRoomOfferState::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

// 선택된 인덱스를 얻는다.
int CWndSecretRoomOfferState::GetSelectIndex( const CPoint& point )
{
	return -1;
}

void CWndSecretRoomOfferState::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

void CWndSecretRoomOfferState::OnLButtonDblClk( UINT nFlags, CPoint point)
{
}

void CWndSecretRoomOfferState::OnRButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

void CWndSecretRoomOfferState::OnRButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

BOOL CWndSecretRoomOfferState::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_CLOSE )
	{
		Destroy();
	}
	else
	if( nID == WIDC_BUTTON1 )
	{
		Destroy();
		g_DPlay.SendSecretRoomTenderOpenWnd();
	}
	else
	if( nID == WIDC_BUTTON2 )
	{
		g_DPlay.SendSecretRoomTenderView();
	}
	
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndSecretRoomOfferState::InsertGuild( const char szGuild[], const char szName[], int nNum )
{
	CWndListBox* pWndList = (CWndListBox*)GetDlgItem( WIDC_LISTBOX1 );
	
	CString str;
	str.Format( "No.%.2d		%s", nNum, szGuild );
	pWndList->AddString(str);
}

void CWndSecretRoomOfferState::OnDraw( C2DRender* p2DRender ) 
{ 
	if( m_tCurrentTime > 0 )
	{
		CWndStatic* pStatic	= (CWndStatic *)GetDlgItem( WIDC_STATIC2 );

		CString str1;
		CString str2;
		CString str3;
		CString str4;
		CString str;
		
		str1.Format(prj.GetText(TID_PK_LIMIT_DAY), static_cast<int>(m_ct.GetDays()) );
		str2.Format(prj.GetText(TID_PK_LIMIT_HOUR), m_ct.GetHours() );
		str3.Format(prj.GetText(TID_PK_LIMIT_MINUTE), m_ct.GetMinutes() );
		str4.Format(prj.GetText(TID_PK_LIMIT_SECOND), m_ct.GetSeconds() );

		str.Format( "%s %s %s %s", str1, str2, str3, str4 );
		pStatic->SetTitle(str);
	}
} 

void CWndSecretRoomOfferState::SetRate( int nRate )
{
	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC5 );
	
	CString str;
	str.Format( "%d", nRate );
	
	if( nRate == -1 )
		str = " ";

	pWndStatic->SetTitle( str );
	if( 8 < nRate )
		pWndStatic->m_dwColor = 0xFFFF0000;	
}

BOOL CWndSecretRoomOfferState::Process() 
{
	m_tCurrentTime = ( m_tEndTime - time_null() );
	
	if( m_tEndTime && m_tCurrentTime > 0 )
	{
		m_ct = m_tCurrentTime;//long)(m_dwCurrentTime / 1000.0f) );
	}
	
	return TRUE;
}

void CWndSecretRoomOfferState::SetGold( int nGold )
{
	CWndStatic* pWndStatic = (CWndStatic*)GetDlgItem( WIDC_STATIC6 );
	pWndStatic->AddWndStyle(WSS_MONEY);	

	CString str;
	str.Format( "%d", nGold );
	pWndStatic->SetTitle( str );	
}

//////////////////////////////////////////////////////////////////////////
// 비밀의 방 입찰 확인 창
//////////////////////////////////////////////////////////////////////////

BOOL CWndSecretRoomOfferMessageBox::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	return CWndMessageBox::Initialize( "", pWndParent, MB_OKCANCEL );	
}

void CWndSecretRoomOfferMessageBox::SetValue( CString str, DWORD nCost )
{
	m_wndText.SetString( str );
	m_nCost = nCost;	
}

BOOL CWndSecretRoomOfferMessageBox::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( nID )
	{
		case IDOK:
			g_DPlay.SendSecretRoomTender( m_nCost );
			SAFE_DELETE(g_WndMng.m_pWndSecretRoomOffer);
			Destroy();
			break;
		case IDCANCEL:
			Destroy();
			break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

//////////////////////////////////////////////////////////////////////////
// 비밀의 방 메세지 창
//////////////////////////////////////////////////////////////////////////

CWndSecretRoomInfoMsgBox::CWndSecretRoomInfoMsgBox()
{
}

CWndSecretRoomInfoMsgBox::~CWndSecretRoomInfoMsgBox()
{
}

BOOL CWndSecretRoomInfoMsgBox::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	return CWndNeuz::InitDialog( APP_SECRETROOM_MSG, pWndParent, 0, CPoint( 0, 0 ) );
}

BOOL CWndSecretRoomInfoMsgBox::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( nID )
	{
	case WIDC_YES:
		{
			g_DPlay.SendSecretRoomTeleportToNPC();
			Destroy();
		}
		break;
	case WIDC_NO:
		{
			Destroy();
		}
	case 10000:
		{
			Destroy();
		}
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndSecretRoomInfoMsgBox::SetString(const CHAR* szChar )
{
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );

	pWndText->m_string.AddParsingString( szChar  );
	pWndText->ResetString();	
}

void CWndSecretRoomInfoMsgBox::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();
	MoveParentCenter();
}

//////////////////////////////////////////////////////////////////////////
// 비밀의 방 세율 변경 확인 창
//////////////////////////////////////////////////////////////////////////

BOOL CWndSecretRoomChangeTaxRateMsgBox::Initialize( CWndBase* pWndParent, DWORD dwWndId )
{
	return CWndMessageBox::Initialize( "", pWndParent, MB_OKCANCEL );	
}

void CWndSecretRoomChangeTaxRateMsgBox::SetValue( CString str, int nSalesTax, int nPurchaseTax, BYTE nCont )
{
	m_wndText.SetString( str );
	m_nSalesTax = nSalesTax;
	m_nPurchaseTax = nPurchaseTax;
	m_nCont = nCont;
}

BOOL CWndSecretRoomChangeTaxRateMsgBox::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult )
{
	switch( nID )
	{
		case IDOK:
			g_DPlay.SendTaxRate(m_nCont, m_nSalesTax, m_nPurchaseTax);
			SAFE_DELETE(g_WndMng.m_pWndSecretRoomChangeTaxRate);
			Destroy();
			break;
		case IDCANCEL:
			Destroy();
			break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

//////////////////////////////////////////////////////////////////////////
// 비밀의 방 취소
//////////////////////////////////////////////////////////////////////////

CWndSecretRoomCancelConfirm::CWndSecretRoomCancelConfirm() 
{
}

CWndSecretRoomCancelConfirm::~CWndSecretRoomCancelConfirm() 
{
}

void CWndSecretRoomCancelConfirm::OnDraw( C2DRender* p2DRender ) 
{
}

void CWndSecretRoomCancelConfirm::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	CWndText* pWndText = (CWndText*)GetDlgItem(WIDC_TEXT1);
	if(pWndText)
	{
		pWndText->SetString(_T( prj.GetText(TID_GAME_SECRETROOM_CANCEL) ), 0xff000000);
		pWndText->ResetString();
	}

	MoveParentCenter();
} 

BOOL CWndSecretRoomCancelConfirm::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( APP_SECRETROOM_CANCEL_CONFIRM, pWndParent, 0, CPoint( 0, 0 ) );
} 

BOOL CWndSecretRoomCancelConfirm::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndSecretRoomCancelConfirm::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndSecretRoomCancelConfirm::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

void CWndSecretRoomCancelConfirm::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

BOOL CWndSecretRoomCancelConfirm::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_YES )
	{
		if( g_pPlayer )
			g_DPlay.SendSecretRoomTenderCancelReturn();
		Destroy( TRUE );
	}
	else if( nID == WIDC_NO )
		Destroy( TRUE );

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

//////////////////////////////////////////////////////////////////////////
// 비밀의 방 안내 창
//////////////////////////////////////////////////////////////////////////
CWndSecretRoomBoard::CWndSecretRoomBoard() 
{
}

CWndSecretRoomBoard::~CWndSecretRoomBoard() 
{
}

void CWndSecretRoomBoard::OnDraw( C2DRender* p2DRender ) 
{
}

void CWndSecretRoomBoard::OnInitialUpdate() {
	CWndNeuz::OnInitialUpdate();
	MoveParentCenter();
}

BOOL CWndSecretRoomBoard::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	return CWndNeuz::InitDialog( APP_SECRETROOM_BOARD, pWndParent, 0, CPoint( 0, 0 ) );
} 

BOOL CWndSecretRoomBoard::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase ) 
{ 
	return CWndNeuz::OnCommand( nID, dwMessage, pWndBase ); 
} 

void CWndSecretRoomBoard::OnSize( UINT nType, int cx, int cy ) \
{ 
	CWndNeuz::OnSize( nType, cx, cy ); 
} 

void CWndSecretRoomBoard::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
} 

void CWndSecretRoomBoard::OnLButtonDown( UINT nFlags, CPoint point ) 
{ 
} 

BOOL CWndSecretRoomBoard::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_BUTTON1 )
	{
		Destroy();
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

void CWndSecretRoomBoard::SetString()
{
	CWndText::SetupDescription(GetDlgItem<CWndText>(WIDC_TEXT1), _T("SecretRoomBoard.inc"));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndSecretRoomQuick
////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool prMemberLevelDesc(u_long player1, u_long player2)
{
	bool rtn_val = false;

	CGuildMember* pMember1 = g_pPlayer->GetGuild()->GetMember(player1);
	CGuildMember* pMember2 = g_pPlayer->GetGuild()->GetMember(player2);
	
	if(pMember1 && pMember2)
	{
		PlayerData* pPlayerData1 = CPlayerDataCenter::GetInstance()->GetPlayerData(pMember1->m_idPlayer);
		PlayerData* pPlayerData2 = CPlayerDataCenter::GetInstance()->GetPlayerData(pMember2->m_idPlayer);

		if(pPlayerData1 && pPlayerData2)
		{
			if(pPlayerData1->data.nLevel < pPlayerData2->data.nLevel)
				rtn_val = true;
			else if(pPlayerData1->data.nLevel == pPlayerData2->data.nLevel)
			{
				if(pPlayerData1->data.nJob > pPlayerData2->data.nJob)
					rtn_val = true;
			}
		}
	}

	return rtn_val;
}

CWndSecretRoomQuick::CWndSecretRoomQuick() 
{ 
	if(g_pPlayer->GetGuild())
		m_vecGuildMemberId.push_back(g_pPlayer->GetGuild()->m_idMaster);

	m_StaticID[0] = WIDC_STATIC_MEM1;
	m_StaticID[1] = WIDC_STATIC_MEM2;
	m_StaticID[2] = WIDC_STATIC_MEM3;
	m_StaticID[3] = WIDC_STATIC_MEM4;
	m_StaticID[4] = WIDC_STATIC_MEM5;
	m_StaticID[5] = WIDC_STATIC_MEM6;
	m_StaticID[6] = WIDC_STATIC_MEM7;
	m_StaticID[7] = WIDC_STATIC_MEM8;
	m_StaticID[8] = WIDC_STATIC_MEM9;
	m_StaticID[9] = WIDC_STATIC_MEM10;
	m_StaticID[10] = WIDC_STATIC_MEM11;

	m_pWndMemberStatic.fill(nullptr);
} 

CWndSecretRoomQuick::~CWndSecretRoomQuick() 
{ 
	DeleteDeviceObjects();
} 

HRESULT CWndSecretRoomQuick::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	if( m_pVBGauge == NULL )
		return m_pApp->m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBGauge, NULL );
	return S_OK;
}
HRESULT CWndSecretRoomQuick::InvalidateDeviceObjects() {
	CWndBase::InvalidateDeviceObjects();
	SAFE_RELEASE(m_pVBGauge);
	return S_OK;
}
HRESULT CWndSecretRoomQuick::DeleteDeviceObjects() {
	CWndBase::DeleteDeviceObjects();
	SAFE_RELEASE(m_pVBGauge);
	return S_OK;
}

void CWndSecretRoomQuick::OnDraw( C2DRender* p2DRender ) 
{ 
	// 파티 정보 출력
	if(m_MemberCount <= 0 || m_MemberCount != m_vecGuildMemberId.size() || g_pPlayer->GetGuild() == NULL)
		return;

	int nDrawCount;
	if(m_bMini)
		nDrawCount = 1;
	else
		nDrawCount = m_vecGuildMemberId.size();

	for(int i=0; i<nDrawCount; i++)
	{
		LPWNDCTRL lpWndCtrl = GetWndCtrl( m_StaticID[i] );

		if(lpWndCtrl == NULL)
		{
			Error("SecretRoomQuick Draw i=%d", i);
			continue;
		}

		CRect rect = lpWndCtrl->rect;
		u_long uPlayerid = m_vecGuildMemberId[i];
		
		CGuildMember* pMember = g_pPlayer->GetGuild()->GetMember(uPlayerid);
		if(pMember)
		{
			PlayerData* pPlayerData = CPlayerDataCenter::GetInstance()->GetPlayerData(pMember->m_idPlayer);
			CWndWorld* pWndWorld = (CWndWorld*)g_WndMng.GetWndBase( APP_WORLD );
			CMover* pObjMember = pMember->GetMover();

			CMover* pFocusObjMember = NULL;
			if(m_FocusMemberid > 0)
				pFocusObjMember = g_pPlayer->GetGuild()->GetMember(m_FocusMemberid)->GetMover();

			if(pPlayerData == NULL || pWndWorld == NULL)
				return;

			CString strMember;
			
			if(m_bMini && pFocusObjMember)
				pObjMember = pFocusObjMember;

			if(pFocusObjMember && pFocusObjMember == pObjMember)
				p2DRender->RenderFillRect( rect, 0x60ffff00 );

			// 상태에 따라 색 변경
			DWORD dwColor = 0xff000000;

			if( IsValidObj(pObjMember) )
			{
				if( pObjMember->GetHitPoint() == 0 ) 
					dwColor = 0xffff0000; // 죽은놈
				else if( ((FLOAT)pObjMember->GetHitPoint()) / ((FLOAT)pObjMember->GetMaxHitPoint()) <.1f ) 
					dwColor = 0xffffff00; // HP 10% 이하인놈
				if(i==0) //GuildMaster Color Set
				{
					dwColor = 0xff1fb72d; //굵게 해야함...
				}

				strMember = CWndPartyQuick::FormatPlayerName(pObjMember->GetLevel(), pObjMember->GetJob(), pObjMember->GetName());
			}
			else
			{
				dwColor = 0xff878787; // 디폴트는 주위에 없는놈
				if(pPlayerData->data.uLogin <= 0)
					dwColor = 0xff000000; // 서버에 없는놈

				CString	strTemp2;
				int nLevel	= pPlayerData->data.nLevel;
				int nJob	= pPlayerData->data.nJob;

				strMember = CWndPartyQuick::FormatPlayerName(nLevel, static_cast<DWORD>(nJob), pPlayerData->szPlayer);
			}
			//Member - Level, Name Draw
			//긴 이름은 ... 으로.

			strings::ReduceSize(strMember, 13);

			p2DRender->TextOut( rect.TopLeft().x+5, rect.TopLeft().y+5, strMember, dwColor );

			//Member - Gauge Draw
			rect.TopLeft().y += 18;
			rect.TopLeft().x += 2;
			rect.BottomRight().x -= 2;
			int nWidth	= pObjMember ? pObjMember->GetHitPointPercent( rect.Width() ) : 0;
			CRect rectTemp = rect; 
			rectTemp.right = rectTemp.left + nWidth;
			if( rect.right < rectTemp.right )
				rectTemp.right = rect.right;
			m_pTheme->RenderGauge( p2DRender, &rect, 0xffffffff, m_pVBGauge, &m_texGauEmptyNormal );
			m_pTheme->RenderGauge( p2DRender, &rectTemp, 0x64ff0000, m_pVBGauge, &m_texGauFillNormal );
		}
	}
} 

void CWndSecretRoomQuick::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	for (int i = 0; i < MAX_SECRETROOM_MEMBER; i++)
		m_pWndMemberStatic[i] = GetDlgItem<CWndStatic>(m_StaticID[i]);

	m_texGauEmptyNormal.LoadTexture( MakePath( DIR_THEME, "GauEmptySmall.bmp" ), 0xffff00ff, TRUE );
	m_texGauFillNormal.LoadTexture( MakePath( DIR_THEME, "GauFillSmall.bmp" ), 0xffff00ff, TRUE );
	
	SetActiveMember(m_MemberCount);
	SortMemberList();
	
	const CRect rectRoot = m_pWndRoot->GetLayoutRect();
	const CRect rectWindow = GetWindowRect();
	const CPoint point( rectRoot.right - rectWindow.Width(), 112 + 48 );
	Move( point );
} 

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndSecretRoomQuick::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( APP_SECRETROOM_QUICK, pWndParent, 0, CPoint( 0, 0 ) );
} 

void CWndSecretRoomQuick::OnLButtonUp( UINT nFlags, CPoint point ) 
{ 
	CWndTaskBar* pTaskBar = g_WndMng.m_pWndTaskBar;
	if(g_WndMng.m_pWndWorld->m_bAutoAttack || pTaskBar->m_nExecute != 0)
		return;

	CGuild * guild = g_pPlayer->GetGuild();
	if (!guild) return;
	
	for(int i=0; i<m_MemberCount; i++) 
	{
		const WNDCTRL * lpWndCtrl = GetWndCtrl( m_StaticID[i] );
		const CRect rect = lpWndCtrl->rect;
		if (!rect.PtInRect(point)) continue;

		g_WndMng.m_pWndWorld->m_pSelectRenderObj = NULL;
			
		CGuildMember* pMember = guild->GetMember(m_vecGuildMemberId[i]);
		if (!pMember) return; /* yes return */

		CMover* pMover = pMember->GetMover();
		if( g_pPlayer != pMover ) 
		{
			if( IsValidObj( pMover ) ) 
			{
				g_WorldMng()->SetObjFocus( pMover );
				g_WndMng.m_pWndWorld->m_pRenderTargetObj = NULL;

				m_FocusMemberid = m_vecGuildMemberId[i];
			}
		}
		else
		{
			m_FocusMemberid = 0;
			g_WorldMng()->SetObjFocus( NULL );
		}

		return;
	}
} 

BOOL CWndSecretRoomQuick::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{
	if(nID == 10000)
	{
		if(message == WNM_CLICKED)
		{
			if(m_MemberCount == 1)
			{
				m_bMini = FALSE;
				return TRUE;
			}
			
			CRect rect = GetWindowRect(TRUE);
			if(m_nWndHeight != 0)
			{
				if(rect.bottom - rect.top == m_nWndHeight)
				{
					rect.bottom = rect.top + 80;
					for(int i=1; i<m_MemberCount; i++)
					{
						CWndStatic* lpWndStatic;
						lpWndStatic = (CWndStatic*) GetDlgItem( m_StaticID[i] );
						lpWndStatic->EnableWindow(FALSE);
						lpWndStatic->SetVisible(FALSE);
					}

					m_bMini = TRUE;
				}
				else if(rect.bottom - rect.top == 80)
				{
					rect.bottom = rect.top + m_nWndHeight;
					for(int i=1; i<m_MemberCount; i++)
					{
						CWndStatic* lpWndStatic;
						lpWndStatic = (CWndStatic*) GetDlgItem( m_StaticID[i] );
						lpWndStatic->EnableWindow(TRUE);
						lpWndStatic->SetVisible(TRUE);
					}

					m_bMini = FALSE;
				}

				SetWndRect(rect);
			}

			return TRUE;
		}
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

void CWndSecretRoomQuick::SetActiveMember(int MemberNum)
{
	int i, height;
	CRect rect;
	
	rect = GetWindowRect(TRUE);
	height = 40;

	for(i=0; i<MemberNum; i++)
	{
		CWndStatic* lpWndStatic;
		lpWndStatic = (CWndStatic*) GetDlgItem( m_StaticID[i] );
		lpWndStatic->EnableWindow(TRUE);
		lpWndStatic->SetVisible(TRUE);
		height += 34;
	}
	for(i=MemberNum; i<MAX_SECRETROOM_MEMBER; i++)
	{
		CWndStatic* lpWndStatic;
		lpWndStatic = (CWndStatic*) GetDlgItem( m_StaticID[i] );
		lpWndStatic->EnableWindow(FALSE);
		lpWndStatic->SetVisible(FALSE);
	}
	rect.bottom = rect.top + height;

	SetWndRect(rect);
	rect = GetWindowRect(TRUE);
	m_nWndHeight = rect.bottom - rect.top;

	AdjustWndBase();
}

void CWndSecretRoomQuick::SetGuildMember(u_long uGuildMemberId)
{
	if(m_vecGuildMemberId.size() < MAX_SECRETROOM_MEMBER)
	{
		if(g_pPlayer->GetGuild())
		{
			CGuildMember* pMember = g_pPlayer->GetGuild()->GetMember(uGuildMemberId);
			if(pMember && pMember->m_nMemberLv != GUD_MASTER)
				m_vecGuildMemberId.push_back(uGuildMemberId);
		}
	}
}

void CWndSecretRoomQuick::SortMemberList()
{
	u_long memberList[MAX_SECRETROOM_MEMBER];
	memset( memberList, 0, sizeof(u_long) * MAX_SECRETROOM_MEMBER );

	int nCount = m_vecGuildMemberId.size();
	for(int i=0; i<nCount-1; i++)
		memberList[i] = m_vecGuildMemberId[i+1];

	std::sort( memberList, memberList + nCount, prMemberLevelDesc );

	m_vecGuildMemberId.clear();
	if(g_pPlayer->GetGuild())
		m_vecGuildMemberId.push_back(g_pPlayer->GetGuild()->m_idMaster);

	for( int i=0; i<nCount; i++)
	{
		if(g_pPlayer->GetGuild())
		{
			CGuildMember* pMember = g_pPlayer->GetGuild()->GetMember(memberList[i]);
			if(pMember && pMember->m_nMemberLv != GUD_MASTER)
				m_vecGuildMemberId.push_back(memberList[i]);
		}
	}
}

