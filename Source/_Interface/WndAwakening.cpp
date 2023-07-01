#include "stdafx.h"
#include "resData.h"
#include "WndAwakening.h"
#include "DPClient.h"
#include "WndManager.h"
#include "defineText.h"

#include "randomoption.h"

/****************************************************
  WndId : APP_AWAKENING - 아이템 각성 창
  CtrlId : WIDC_TEXT1 - 
  CtrlId : WIDC_STATIC1 - 
  CtrlId : WIDC_BUTTON1 - Button
****************************************************/

// gmpbigsun ( 10_04_05 ) : CWndAwakening class는 현재 쓰이지 않음

void CWndAwakening::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	// 여기에 코딩하세요
	CWndButton* pButton = GetDlgItem<CWndButton>(WIDC_BUTTON1);

	if(::GetLanguage() == LANG_FRE)
		pButton->SetTexture( MakePath( DIR_THEME, _T( "ButOk2.bmp" ) ), TRUE);

	pButton->EnableWindow(FALSE);

	CWndText * pText = GetDlgItem<CWndText>(WIDC_TEXT1);
	CWndText::SetupDescription(pText, _T("ItemAwakening.inc"));

	LPWNDCTRL wndCtrl = GetWndCtrl(WIDC_STATIC1);
	m_receiver.Create(0, wndCtrl->rect, this, 900);

	MoveParentCenter();
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndAwakening::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( APP_AWAKENING, pWndParent, 0, CPoint( 0, 0 ) );
} 

BOOL CWndAwakening::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) {
	if( nID == WIDC_BUTTON1 )
	{
		//서버로 시작을 알린다.
		if (CItemElem * item = m_receiver.GetItem()) {
			CWndButton * pButton = GetDlgItem<CWndButton>(WIDC_BUTTON1);
			pButton->EnableWindow(FALSE);

			// 서버에 처리 요청하는 함수 호출해야함
			g_DPlay.SendAwakening(item->m_dwObjId);
			Destroy();
		}
	} else if (nID == WIDC_Receiver) {
		GetDlgItem<CWndButton>(WIDC_BUTTON1)->EnableWindow(
			m_receiver.GetItem() ? TRUE : FALSE
		);
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

bool CWndAwakening::CAwakenableItemReceiver::CanReceiveItem(const CItemElem & itemElem, bool verbose) {
	const int nRandomOptionKind = g_xRandomOptionProperty.GetRandomOptionKind(&itemElem);
	if (nRandomOptionKind != CRandomOptionProperty::eAwakening) {
		if (verbose) g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_INVALID_TARGET_ITEM));
		return false;
	}

	if (g_xRandomOptionProperty.GetRandomOptionSize(itemElem.GetRandomOptItemId()) > 0) {
		if (verbose) g_WndMng.OpenMessageBox(prj.GetText(TID_GAME_AWAKE_OR_BLESSEDNESS01));
		return false;
	}

	return true;
}


CWndSelectAwakeCase::CWndSelectAwakeCase(BYTE byObjID, DWORD dwSerialNum, __int64 n64NewOption) {
	m_dwOldTime = g_tmCurrent;

	//server로 전송할 데이터를 유지하고, 아이템 Index를 뽑아서 아이콘을 그려줄 준비
	m_byObjID = byObjID;
	m_dwSerialNum = dwSerialNum;
	m_n64NewOption = n64NewOption;

	if (!g_pPlayer) return;
	
	const ItemProp * const pProp = g_pPlayer->GetItemIdProp(m_byObjID);
	if (pProp) {
		m_dwItemIndex = pProp->dwID;
	}
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndSelectAwakeCase::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( APP_AWAKE_SELECTCASE, pWndParent, 0, CPoint( 0, 0 ) );
} 

void CWndSelectAwakeCase::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	
	MoveParentCenter();

	const ItemProp * pProp = prj.GetItemProp( m_dwItemIndex );
	if (pProp) {
		m_pTexture = CWndBase::m_textureMng.AddTexture(MakePath(DIR_ITEM, pProp->szIcon), 0xffff00ff);
	}
	
	m_pTexGuage = CWndBase::m_textureMng.AddTexture( MakePath( DIR_THEME, "Wndguage.tga"   ), 0xffff00ff );
	if (!m_pTexGuage) {
		Error("CWndSelectAwakeCase::OnInitialUpdate m_pTexGuage(Wndguage.tga) is NULL");
	}

	AddWndStyle(WBS_MODAL);

	RestoreDeviceObjects();
} 


BOOL CWndSelectAwakeCase::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if( nID == WIDC_BUTTON1 )
	{
		g_DPlay.SendSelectedAwakeningValue( m_byObjID, m_dwSerialNum, _AWAKE_OLD_VALUE );
		g_WndMng.PutString( GETTEXT( TID_GAME_REGARDLESS_USE03 ) );	//선택됨
		Destroy();
	}
	else if( nID == WIDC_BUTTON2 )
	{
		g_DPlay.SendSelectedAwakeningValue( m_byObjID, m_dwSerialNum, _AWAKE_NEW_VALUE );
		g_WndMng.PutString( GETTEXT( TID_GAME_REGARDLESS_USE03 ) );	//선택됨
		Destroy();
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 

void CWndSelectAwakeCase::OnDraw( C2DRender* p2DRender ) 
{ 
	if( !g_pPlayer )
		return;

	CItemElem* pItemElem = g_pPlayer->GetItemId( m_byObjID );
	if( !pItemElem )
	{
		assert( 0 );
		return;
	}

	m_dwDeltaTime += (g_tmCurrent - m_dwOldTime);
	m_dwOldTime = g_tmCurrent;

	if( m_dwDeltaTime > AWAKE_KEEP_TIME )
	{
		//제한시간이 지나면 현재옵션으로 요청 
		g_DPlay.SendSelectedAwakeningValue( m_byObjID, m_dwSerialNum, _AWAKE_OLD_VALUE );

		g_WndMng.PutString( GETTEXT( TID_GAME_REGARDLESS_USE02 ) );	//선택시간 초과
		
		m_dwDeltaTime = 0;
		Destroy();
	}

	//old
	OutputOptionString( p2DRender, pItemElem );

	//new
	OutputOptionString( p2DRender, pItemElem, TRUE );


	CRect rect = GetWindowRect();
	rect.left += 14;
	rect.top += 174;

	// 시간 / 100 * 360(총길이) / 600 (총시간)
	rect.right = LONG(( rect.left + (AWAKE_KEEP_TIME - m_dwDeltaTime) / 100 ) * 0.6f) ;		//귀찮으니 걍 바의 길이를 최대초로...
	rect.bottom = rect.top + 20;

	m_pTheme->RenderGauge(p2DRender, &rect, 0xffffffff, m_pVertexBufferGauge, m_pTexGuage);

	// draw icon
	LPWNDCTRL wndCtrl = GetWndCtrl( WIDC_STATIC1 );
	assert( wndCtrl );
			
	if (m_pTexture) {
		m_pTexture->Render(p2DRender, CPoint(wndCtrl->rect.left, wndCtrl->rect.top));
	}
} 

extern bool IsDst_Rate(int nDstParam);
extern const char *FindDstString( int nDstParam );
void CWndSelectAwakeCase::OutputOptionString( C2DRender* p2DRender, CItemElem* pItemElem, BOOL bNew )
{
	// 옵션 문자열 그리기
	if( !pItemElem )
		return;

    int nSize = 0;
	if( !bNew )
		nSize = g_xRandomOptionProperty.GetRandomOptionSize( pItemElem->GetRandomOptItemId() );
	else nSize = g_xRandomOptionProperty.GetViewRandomOptionSize( m_n64NewOption );

	__int64 n64Options = 0;
	n64Options = ( bNew ? m_n64NewOption : pItemElem->GetRandomOptItemId() );

	CString str;

	// option
	for( int i = 0; i < nSize; i++ )
	{
		const auto opt = g_xRandomOptionProperty.GetParam(n64Options, i);
		if (!opt) continue;

		const int nDst = opt->nDst;
		const int nAdj = opt->nAdj;

		if( IsDst_Rate( nDst ) )
		{
			if( nDst == DST_ATTACKSPEED )
				str.Format( "\n%s %c%d%% ", FindDstString( nDst ), ( nAdj > 0? '+': '-' ), ::abs( nAdj / 2 / 10 ) );
			else
				str.Format( "\n%s %c%d%%", FindDstString( nDst ), ( nAdj > 0? '+': '-' ), ::abs( nAdj ) );
		}
		else
		{
			str.Format( "\n%s %c%d", FindDstString( nDst ), ( nAdj > 0? '+': '-' ), ::abs( nAdj ) );
		}

		//걍 한줄한줄씩 찍자
		if( bNew )
			p2DRender->TextOut( 70, 92 + i * 20, str, 0xff000000 );
		else p2DRender->TextOut( 70, 20 + i * 20, str, 0xff000000 );
	}
}

HRESULT CWndSelectAwakeCase::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	if( m_pVertexBufferGauge == NULL )
		m_pApp->m_pd3dDevice->CreateVertexBuffer(sizeof(TEXTUREVERTEX2) * 3 * 6, 
												 D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 
												 D3DFVF_TEXTUREVERTEX2, 
												 D3DPOOL_DEFAULT, 
												 &m_pVertexBufferGauge, 
												 NULL);
	assert(m_pVertexBufferGauge != NULL);
	
	return S_OK;
}
HRESULT CWndSelectAwakeCase::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
    SAFE_RELEASE( m_pVertexBufferGauge );
	return S_OK;
}
HRESULT CWndSelectAwakeCase::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	SAFE_RELEASE( m_pVertexBufferGauge );
	return S_OK;
}
