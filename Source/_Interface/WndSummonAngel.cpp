#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "WndManager.h"
#include "WndSummonAngel.h"

#include "dpclient.h"



////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CWndSummonAngel
////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CWndSummonAngel::OnDestroy() {
	if (CWndInventory * m_pWndInventory = GetWndBase<CWndInventory>(APP_INVENTORY)) {
		m_pWndInventory->m_wndItemCtrl.SetDieFlag(FALSE);
	}
}

void CWndSummonAngel::OnDraw( C2DRender* p2DRender ) 
{ 
	//Render Icon
	if (const ItemProp * const pItemProp = prj.GetItemProp(II_GEN_MAT_ORICHALCUM01)) {
		CTexture * pTexture = CWndBase::m_textureMng.AddTexture( MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
		if(pTexture != NULL)
			pTexture->Render( p2DRender, CPoint( 30, 218 ) );
	}

	if (const ItemProp * pItemProp = prj.GetItemProp(II_GEN_MAT_MOONSTONE)) {
		CTexture * pTexture = CWndBase::m_textureMng.AddTexture( MakePath( DIR_ITEM, pItemProp->szIcon), 0xffff00ff );
		if(pTexture != NULL)
			pTexture->Render( p2DRender, CPoint( 30, 306 ) );
	}

	// '%'
	int y = 206;
	int nNext = 24;
	DWORD dwColor = 0xff2e70a9;
	CD3DFont* pOldFont = p2DRender->GetFont();
	p2DRender->SetFont( CWndBase::m_Theme.m_pFontText );
	p2DRender->TextOut( 415, y, "%", dwColor); y += nNext;
	p2DRender->TextOut( 415, y, "%", dwColor); y += nNext;
	p2DRender->TextOut( 415, y, "%", dwColor); y += nNext;
	p2DRender->SetFont( pOldFont );
} 

void CWndSummonAngel::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	//Ctrl Initialize.
	m_pText = GetDlgItem<CWndText>(WIDC_TEXT1);

	m_pStatic[0] = GetDlgItem<CWndStatic>(WIDC_STATIC_RED);
	m_pStatic[1] = GetDlgItem<CWndStatic>(WIDC_STATIC_BLUE);
	m_pStatic[2] = GetDlgItem<CWndStatic>(WIDC_STATIC_GREEN);
	SummonRateRefresh();
	
	int StaticID[/* 20 */] = {WIDC_STATIC11, WIDC_STATIC12, WIDC_STATIC13, WIDC_STATIC14, WIDC_STATIC15, WIDC_STATIC16,
						WIDC_STATIC17, WIDC_STATIC18, WIDC_STATIC19, WIDC_STATIC20, WIDC_STATIC21, WIDC_STATIC22,
						WIDC_STATIC23, WIDC_STATIC24, WIDC_STATIC25, WIDC_STATIC26, WIDC_STATIC27, WIDC_STATIC28,
						WIDC_STATIC29, WIDC_STATIC30};

	static_assert(MaxSlotPerItem * 2 == (sizeof(StaticID) / sizeof(StaticID[0])));

	for (unsigned int i = 0; i != MaxSlotPerItem; ++i) {
		m_oriReceivers[i] .Create(0, GetWndCtrl(StaticID[i                 ])->rect, this, StartOffsetWidcSlots                  + i);
		m_moonReceivers[i].Create(0, GetWndCtrl(StaticID[i + MaxSlotPerItem])->rect, this, StartOffsetWidcSlots + MaxSlotPerItem + i);
	}
	
	//Text Setting
	GetDlgItem(WIDC_STATIC3)->SetTitle(prj.GetText(TID_GAME_REDANGEL_RATE));
	GetDlgItem(WIDC_STATIC4)->SetTitle(prj.GetText(TID_GAME_BLUEANGEL_RATE));
	GetDlgItem(WIDC_STATIC5)->SetTitle(prj.GetText(TID_GAME_GREENANGEL_RATE));

	GetDlgItem(WIDC_STATIC6)->SetTitle(prj.GetText(TID_GAME_WHITEANGEL_INFO1));
	GetDlgItem(WIDC_STATIC1)->SetTitle(prj.GetText(TID_GAME_WHITEANGEL_INFO2));
	GetDlgItem(WIDC_STATIC7)->SetTitle(prj.GetText(TID_GAME_WHITEANGEL_INFO3));

	//France Button Image
	CWndButton * pButton = GetDlgItem<CWndButton>(WIDC_START);
	if(::GetLanguage() == LANG_FRE)
		pButton->SetTexture( MakePath( DIR_THEME, _T( "ButOk2.bmp" ) ), TRUE);

	//Window Position
	CWndInventory * m_pWndInventory = GetWndBase<CWndInventory>(APP_INVENTORY);
	CRect rectInventory;
	if(m_pWndInventory != NULL)
	{
		m_pWndInventory->m_wndItemCtrl.SetDieFlag(TRUE);
		rectInventory = m_pWndInventory->GetWindowRect( TRUE );
	}
	CPoint ptInventory = rectInventory.TopLeft();
	CPoint ptMove;
	
	CRect rect = GetWindowRect( TRUE );
	
	if( ptInventory.x > rect.Width() / 2 )
		ptMove = ptInventory - CPoint( rect.Width(), 0 );
	else
		ptMove = ptInventory + CPoint( rectInventory.Width(), 0 );
	
	Move( ptMove );
}

void CWndSummonAngel::SetQuestText(const CHAR * szChar) {
	if (m_pText != NULL) {
		m_pText->m_string.AddParsingString(szChar);
		m_pText->ResetString();
	}
}

// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CWndSummonAngel::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
	// Daisy에서 설정한 리소스로 윈도를 연다.
	return CWndNeuz::InitDialog( APP_SUMMON_ANGEL, pWndParent, 0, CPoint( 0, 0 ) );
} 

void CWndSummonAngel::SetDieFromInventory(CItemElem & pItemElem) {
	if (ItemProps::IsOrichalcum(pItemElem)) {
		CWndItemReceiver::TryReceiveIn(pItemElem, m_oriReceivers);
	} else if (ItemProps::IsMoonstone(pItemElem)) {
		CWndItemReceiver::TryReceiveIn(pItemElem, m_moonReceivers);
	}
}

void CWndSummonAngel::ReFreshAll() {
	ForEachReceiver([](auto & receiver) { receiver.ResetItemWithNotify(); });

	m_nOrichalcum = 0;
	m_nMoonstone = 0;
	m_nowStarting = false;
	SummonRateRefresh();
}

BOOL CWndSummonAngel::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if (nID == WIDC_RESET) {
		ReFreshAll();
	} else if (nID == WIDC_START) {
		if(	m_nOrichalcum + m_nMoonstone < 3 ) {
			g_WndMng.OpenMessageBox( prj.GetText( TID_GAME_SUMMONANGEL_ERROR2 ) );
		} else if (!m_nowStarting) {
			std::vector<ItemPos> sentItems;
			m_nowStarting = true;

			ForEachReceiver([&](auto & receiver) {
				if (CItemElem * item = receiver.GetItem()) {
					sentItems.emplace_back(item->m_dwObjId);
				}
				});

			//Angel Create
			g_DPlay.SendCreateAngel(sentItems);
			ReFreshAll();
		}
	} else if (nID == WIDC_CANCEL) {
		Destroy();
	} else if (nID >= StartOffsetWidcSlots && nID < StartOffsetWidcSlots + 2 * MaxSlotPerItem) {
		OnChangedItems();
	}

	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
}

void CWndSummonAngel::OnChangedItems() {

	m_nOrichalcum = 0;
	m_nMoonstone  = 0;

	for (const auto & oriReceiver : m_oriReceivers) {
		if (oriReceiver.GetItem()) {
			++m_nOrichalcum;
		}
	}

	for (const auto & moonReceiver : m_moonReceivers) {
		if (moonReceiver.GetItem()) {
			++m_nMoonstone;
		}
	}

	SummonRateRefresh();
}

void CWndSummonAngel::SummonRateRefresh()
{
	m_GreenAngelRate = (m_nOrichalcum * 1.0f) + (m_nMoonstone * 1.0f);
	m_BlueAngelRate = m_GreenAngelRate * 2.0f;
	m_RedAngelRate = 100.0f - ( m_GreenAngelRate + m_BlueAngelRate );

	if(m_pStatic[0] && m_pStatic[1] && m_pStatic[2])
	{
		CString tempString;
		tempString.Format("%0.1f", m_RedAngelRate);
		m_pStatic[0]->SetTitle(tempString);
		tempString.Format("%0.1f", m_BlueAngelRate);
		m_pStatic[1]->SetTitle(tempString);
		tempString.Format("%0.1f", m_GreenAngelRate);
		m_pStatic[2]->SetTitle(tempString);
	}
}
