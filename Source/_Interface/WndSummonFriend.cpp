#include "stdafx.h"
#include "resData.h"
#include "WndSummonFriend.h"
#include "dpclient.h"
#include "definetext.h"
#include "party.h"

#include "playerdata.h"



/****************************************************
  WndId : APP_MESSAGEBOX_SUMMONFRINED - 소환
  CtrlId : WIDC_STATIC1 - Static
  CtrlId : WIDC_OK - Button
  CtrlId : WIDC_CANCEL - Button
  CtrlId : WIDC_STATIC2 - Static
****************************************************/

void CWndSummonFriendMsg::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	m_objid = 0;
	m_dwData = 0;

	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndSummonFriendMsg::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( APP_MESSAGEBOX_SUMMONFRINED, pWndParent, 0, CPoint( 0, 0 ) );
} 
void CWndSummonFriendMsg::SetData( OBJID objid, DWORD dwData, char* szName, char* szWorldName )
{
	m_objid = objid;
	m_dwData = dwData;
	strcpy( m_szName, szName );
	
	CWndText * pWndText = GetDlgItem<CWndText>(WIDC_TEXT1);
	CString strTemp;
	strTemp.Format(_T( prj.GetText(TID_DIAG_SUMMONFRIEND_CONFIRM) ),m_szName, szWorldName ); 
	pWndText->SetString( strTemp );		
}

BOOL CWndSummonFriendMsg::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
		case WIDC_OK:
			{
				g_DPlay.SendSummonFriendConfirm( m_objid, m_dwData );
				Destroy();
			}
			break;
		case WIDC_CANCEL:
			{
				g_DPlay.SendSummonFriendCancel( m_objid, m_dwData );
				Destroy();
			}
			break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

/****************************************************
  WndId : APP_SUMMON_FRIEND - 소환
  CtrlId : WIDC_EDIT1 - Edit
  CtrlId : WIDC_STATIC1 - 이 름 :
  CtrlId : WIDC_STATIC2 - 소환할 캐릭터 이름을 입력하세요
  CtrlId : WIDC_OK - Button
  CtrlId : WIDC_CANCEL - Button
****************************************************/

void CWndSummonFriend::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	m_dwData	= 0;

	CWndComboBox* pWndCombo		= (CWndComboBox*)GetDlgItem( WIDC_COMBOBOX1 );

	for (const auto & [idPlayer, pFriend] : g_WndMng.m_RTMessenger) {
		if( pFriend.dwState != FriendStatus::OFFLINE && !pFriend.bBlock	)
			pWndCombo->AddString( CPlayerDataCenter::GetInstance()->GetPlayerString( idPlayer ) );
	}
	pWndCombo->SetFocus();

	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndSummonFriend::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( APP_SUMMON_FRIEND, pWndParent, 0, CPoint( 0, 0 ) );
} 
void CWndSummonFriend::SetData( WORD wId, WORD wReset )
{
	m_dwData	= MAKELONG( wId, wReset );
}

BOOL CWndSummonFriend::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
	case WIDC_OK:
		{
			CWndComboBox* pWndCombo		= (CWndComboBox*)GetDlgItem( WIDC_COMBOBOX1 );
			CString string	= pWndCombo->GetString();
			if( 3 <= string.GetLength() && string.GetLength() < MAX_NAME )
			{
				if( 0 != strcmp( g_pPlayer->GetName(), string ) )
				{
					WORD wId	= LOWORD( m_dwData );
					WORD wMode	= HIWORD( m_dwData );
					CItemElem* pItemElem = g_pPlayer->GetItemId( wId );
					if( pItemElem )
					{
						g_DPlay.SendSummonFriend( m_dwData, (LPSTR)(LPCSTR)string );
					}
					else 
					{
						ItemProp* pItemProp = prj.GetItemProp( II_SYS_SYS_SCR_FRIENDSUMMON_A );
						if( pItemProp )
						{
							char lpString[260]	= { 0, };
							sprintf( lpString, prj.GetText( TID_ERROR_SUMMONFRIEND_NOITEM ), pItemProp->szName, string );
							g_WndMng.PutString( lpString, NULL, prj.GetTextColor( TID_ERROR_SUMMONFRIEND_NOITEM ) );
						}					
					}
				}
				else
				{
					g_WndMng.PutString(TID_GAME_SUMMON_FRIEND_MY_NOUSE);
				}
				Destroy();
			}
		}
		break;
	case WIDC_CANCEL:
		{
			Destroy();
		}
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

/****************************************************
  WndId : APP_MESSAGEBOX_SUMMONPARTY - 극단소환
  CtrlId : WIDC_TEXT1 - 
  CtrlId : WIDC_OK - Button
  CtrlId : WIDC_CANCEL - Button
****************************************************/

HRESULT CWndSummonPartyMsg::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	m_Texture.DeleteDeviceObjects();
	return TRUE;
}
void CWndSummonPartyMsg::OnDraw( C2DRender* p2DRender ) 
{ 
	const int sx = 8;
	const int sy = 120;
	p2DRender->RenderTexture( CPoint(sx+4, sy - 4), &m_Texture, 255  );
} 
void CWndSummonPartyMsg::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	ItemProp* pItemProp = prj.GetItemProp( II_SYS_SYS_SCR_PARTYSUMMON );
	if( pItemProp )
		m_Texture.LoadTexture(g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, pItemProp->szIcon ), 0xffff00ff );

	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndSummonPartyMsg::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( APP_MESSAGEBOX_SUMMONPARTY, pWndParent, 0, CPoint( 0, 0 ) );
} 

BOOL CWndSummonPartyMsg::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) { 
	switch (nID) {
		case WIDC_OK:
			Destroy();
			break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
void CWndSummonPartyMsg::SetData( OBJID objid, DWORD dwData, const char* szWorldName )
{
	g_Neuz.m_dwSummonPartyObjid = objid;
	g_Neuz.m_dwSummonPartyData = dwData;
	strcpy( g_Neuz.m_szSummonPartyWorldName, szWorldName );
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	CString strTemp;
	strTemp.Format( _T( prj.GetText(TID_DIAG_SUMMONPARTY_CONFIRM) ), g_Party.m_sParty ); 
	pWndText->SetString( strTemp );		

	ItemProp* pItemProp = prj.GetItemProp( II_SYS_SYS_SCR_PARTYSUMMON );
	if( pItemProp )
	{
		CWndText * pWndText1 = (CWndText *)GetDlgItem(WIDC_TEXT2);
		strTemp.Format( _T( prj.GetText(TID_DIAG_SUMMONPARTY_CONFIRM_USE) ), pItemProp->szName ); 
		pWndText1->SetString( strTemp );		
	}
}
/****************************************************
  WndId : APP_SUMMON_PARTY - Applet
  CtrlId : WIDC_TEXT1 - 
  CtrlId : WIDC_CANCEL - Button
  CtrlId : WIDC_OK - Button
****************************************************/

void CWndSummonParty::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	m_dwData	= 0;

	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndSummonParty::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( APP_SUMMON_PARTY, pWndParent, 0, CPoint( 0, 0 ) );
} 

BOOL CWndSummonParty::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
	case WIDC_OK:
		{
			WORD wId	= LOWORD( m_dwData );
			WORD wMode	= HIWORD( m_dwData );
			CItemElem* pItemElem = g_pPlayer->GetItemId( wId );
			if( pItemElem )
			{
				g_DPlay.SendSummonParty( m_dwData );
			}
			else 
			{
				ItemProp* pItemProp = prj.GetItemProp( II_SYS_SYS_SCR_PARTYSUMMON );
				if( pItemProp )
				{
					char lpString[260]	= { 0, };
					sprintf( lpString, prj.GetText( TID_ERROR_SUMMONPARTY_NOITEM ), pItemProp->szName );
					g_WndMng.PutString( lpString, NULL, prj.GetTextColor( TID_ERROR_SUMMONPARTY_NOITEM ) );
				}					
			}
			Destroy();
		}
		break;
	case WIDC_CANCEL:
		{
			Destroy();
		}
		break;
	}
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
void CWndSummonParty::SetData( WORD wId, WORD wReset )
{
	m_dwData	= MAKELONG( wId, wReset );

	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	CString strTemp;
	strTemp.Format( _T( prj.GetText(TID_GAME_SUMMONPARTY) ), g_pPlayer->GetWorld()->m_szWorldName ); 
	pWndText->SetString( strTemp );		
	
}

/****************************************************
WndId : APP_SUMMON_PARTY_USE - 극단소환
CtrlId : WIDC_TEXT1 - 
CtrlId : WIDC_OK - Button
CtrlId : WIDC_CANCEL - Button
****************************************************/

void CWndSummonPartyUse::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	CWndText* pWndText = (CWndText*)GetDlgItem( WIDC_TEXT1 );
	CString strMsg;
	strMsg.Format( prj.GetText(TID_DIAG_SUMMONPARTY_USE) , g_Neuz.m_szSummonPartyWorldName );
	pWndText->SetString( strMsg );		
	
	// 윈도를 중앙으로 옮기는 부분.
	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndSummonPartyUse::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( APP_SUMMON_PARTY_USE, pWndParent, 0, CPoint( 0, 0 ) );
} 

BOOL CWndSummonPartyUse::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	switch( nID )
	{
	case WIDC_OK:
		{
			g_DPlay.SendSummonPartyConfirm( g_Neuz.m_dwSummonPartyObjid, g_Neuz.m_dwSummonPartyData );
			Destroy();
		}
		break;
	case WIDC_CANCEL:
		{
			Destroy();
		}
	}				
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 
