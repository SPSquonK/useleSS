#include "stdafx.h"
#include "defineText.h"
#include "AppDefine.h"
#include "DialogMsg.h"
#include "WndField.h"
#include "WndManager.h"
#include "WndRegVend.h"

#include "DPClient.h"

#define TASKBAR_HEIGHT 48

#define ICON_SIZE    32
#define SKILL_SIZE   28

DWORD   POINT_APPLET_X ;
DWORD   POINT_APPLET_Y ;
DWORD   POINT_ITEM_X   ;
DWORD   POINT_ITEM_Y   ;
DWORD   POINT_QUEUE_X  ;
DWORD   POINT_QUEUE_Y  ;

#define RECT_APPLET CRect( POINT_APPLET_X, POINT_APPLET_Y, POINT_APPLET_X + m_nMaxSlotApplet * ICON_SIZE , POINT_APPLET_Y + ICON_SIZE  )
#define RECT_ITEM   CRect( POINT_ITEM_X  , POINT_ITEM_Y  , POINT_ITEM_X   + MAX_SLOT_ITEM   * ICON_SIZE , POINT_ITEM_Y   + ICON_SIZE  )
#define RECT_QUEUE  CRect( POINT_QUEUE_X , POINT_QUEUE_Y , POINT_QUEUE_X  + MAX_SLOT_QUEUE  * SKILL_SIZE, POINT_QUEUE_Y  + SKILL_SIZE )

#define POINT_APPLET CPoint( POINT_APPLET_X, POINT_APPLET_Y )
#define POINT_ITEM   CPoint( POINT_ITEM_X  , POINT_ITEM_Y   )
#define POINT_QUEUE  CPoint( POINT_QUEUE_X , POINT_QUEUE_Y  )

void CWndTaskBar::UpdateAllTaskbarTexture() {
	// Ensure has texture and no invalid item shortcut
	const auto EnsureCorrectness = [&](SHORTCUT & shortcut) {
		SetTaskBarTexture(shortcut);

		if (!g_pPlayer) return;

		if (shortcut.m_dwShortcut == ShortcutType::Item) {
			CItemElem * pItemBase = g_pPlayer->GetItemId(shortcut.m_dwId);
			if (pItemBase && pItemBase->GetProp()->dwPackMax > 1) {
				shortcut.m_dwItemId = pItemBase->m_dwItemId;
			}

			if (!pItemBase) {
				shortcut.Empty();
			}
		}
	};

	for (SHORTCUT & shortcut : m_aSlotApplet) {
		EnsureCorrectness(shortcut);
	}

	for (auto & bar : m_aSlotItem) {
		for (SHORTCUT & shortcut : bar) {
			EnsureCorrectness(shortcut);
		}
	}

	for (SHORTCUT & shortcut : m_aSlotQueue) {
		EnsureCorrectness(shortcut);
	}

	// Update m_nCurQueueNum
	for (m_nCurQueueNum = 0; m_nCurQueueNum != m_aSlotQueue.size(); ++m_nCurQueueNum) {
		if (m_aSlotQueue[m_nCurQueueNum].m_dwShortcut != ShortcutType::Skill) {
			break;
		}
	}
}

void CWndTaskBar::SetTaskBarTexture( SHORTCUT & shortcut )
{
	if( shortcut.m_dwShortcut == ShortcutType::Applet )
	{
		AppletFunc* pAppletFunc = g_WndMng.GetAppletFunc( shortcut.m_dwId );
		if( pAppletFunc )
		{
			shortcut.m_pTexture = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice,  MakePath( DIR_ICON, pAppletFunc->m_pszIconName ), 0xffff00ff );
		} else
		{
#ifndef __BS_CONSOLE
			Error( "CWndTaskBar::SetTaskBarTexture : %s %d", g_pPlayer->GetName(), shortcut.m_dwId );
#endif
		}
	}
	else if( shortcut.m_dwShortcut == ShortcutType::Item)
	{
		CItemElem * pItemBase = g_pPlayer->GetItemId( shortcut.m_dwId );
		if( pItemBase )
			shortcut.m_pTexture	= pItemBase->GetTexture();
	}
	else if (shortcut.m_dwShortcut == ShortcutType::PartySkill) {
		ItemProp * pProp = prj.GetPartySkill(shortcut.m_dwId);
		shortcut.m_pTexture = m_textureMng.AddTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, pProp->szIcon/*pItemBase->GetProp()->szIcon*/), 0xffff00ff);
	}
	else if ( shortcut.m_dwShortcut == ShortcutType::Skill)
	{
		ItemProp* pSkillProp = prj.m_aPropSkill.GetAt(shortcut.m_dwId);
		if( pSkillProp )
			shortcut.m_pTexture = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ICON, pSkillProp->szIcon ), 0xffff00ff );
	}
	else if ( shortcut.m_dwShortcut == ShortcutType::Lord)
	{
		CCLord* pLord									= CCLord::Instance();
		CLordSkillComponentExecutable* pComponent		= pLord->GetSkills()->GetSkill(shortcut.m_dwId);	
		if(pComponent) shortcut.m_pTexture							= pComponent->GetTexture();
	}
	else if ( shortcut.m_dwShortcut == ShortcutType::Motion)
	{
		MotionProp* pMotionProp = prj.GetMotionProp( shortcut.m_dwId );
		if(pMotionProp)			//061206 ma	8차에 들어갈 모션관리를 위해 버전 추가	propMotion.txt
		{
			pMotionProp->pTexture = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ICON, pMotionProp->szIconName ), 0xffff00ff );
			shortcut.m_pTexture = pMotionProp->pTexture;
		}
	}
	else if( shortcut.m_dwShortcut == ShortcutType::Chat)
	{
		shortcut.m_pTexture	= m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ICON, "icon_MacroChat.dds" ), 0xffff00ff );
	}
	else if( shortcut.m_dwShortcut == ShortcutType::Emoticon)
	{
		if( shortcut.m_dwId >= 0 && shortcut.m_dwId < MAX_EMOTICON_NUM  )
		{
			TCHAR buffer[MAX_SHORTCUT_STRING] = { 0 };
			_tcscat( buffer, "/" );
			_tcscat( buffer, g_DialogMsg.m_EmiticonCmd[shortcut.m_dwId].m_szCommand );
			_tcscpy( shortcut.m_szString, buffer );
			shortcut.m_pTexture =	g_DialogMsg.m_texEmoticonUser.GetAt(shortcut.m_dwId);
		}
	}
	else if( shortcut.m_dwShortcut == ShortcutType::SkillFun)
	{
		shortcut.m_pTexture	= m_pTexture;
	}
}

/*
* 액션 포인트
. 액션 포인트 변수 CWndTaskBar::m_nActionPoint
. 스킬바에 반투명 사각형 그리기
. 액션포인트 시리얼라이즈
. 패턴공격의 2번째 공격을 사용시에는 액션포인트 증가 1점씩 - 100이 한계
. 액션 포인트에 따른 쉐이드 그리기 - 누적포인트에따라 다르다.
. 스킬사용시 단계별로 액션포인트 소모량 다르게.

*/

CWndTaskBar::CWndTaskBar()
{
	m_nCurQueueNum = 0;
	m_nPosition = TASKBAR_BOTTOM;
	memset( &m_aSlotSkill, 0, sizeof( m_aSlotSkill ) );

	m_pSelectShortcut = NULL;
	m_bStartTimeBar = FALSE;
	m_nSkillBar = 0;
	m_nUsedSkillQueue = 0;
	m_nExecute = 0;
	m_nSlotIndex = 0;
	m_nActionPoint = 0;
	m_dwHighAlpha = 0;
}

std::array<SHORTCUT, MAX_SLOT_ITEM> & CWndTaskBar::m_paSlotItem() {
	if (m_nSlotIndex < 0 && m_nSlotIndex >= MAX_SLOT_ITEM) {
		m_nSlotIndex = 0;
	}

	return m_aSlotItem[m_nSlotIndex];
}

const std::array<SHORTCUT, MAX_SLOT_ITEM> & CWndTaskBar::m_paSlotItem() const {
	if (m_nSlotIndex < 0 && m_nSlotIndex >= MAX_SLOT_ITEM) {
		return m_aSlotItem[0];
	}

	return m_aSlotItem[m_nSlotIndex];
}

void CWndTaskBar::PutTooTip( LPSHORTCUT pShortcut, CPoint point, CRect* pRect )
{
	ClientToScreen( &point );
	ClientToScreen( pRect );
	if( pShortcut->m_dwShortcut == ShortcutType::Applet)
	{
		AppletFunc* pAppletFunc = g_WndMng.GetAppletFunc( pShortcut->m_dwId ); 
		if( pAppletFunc )
		{
			
			CEditString strEdit = pAppletFunc->m_pAppletDesc;
			if( pAppletFunc->m_cHotkey != 0 )
			{
				strEdit.AddString( "\n" );
				CString string;
				string.Format( "[%s %c]", prj.GetText( TID_GAME_TOOLTIP_HOTKEY ), pAppletFunc->m_cHotkey );
				strEdit.AddString( string, 0xff0000ff );
			}

			g_toolTip.PutToolTip( pShortcut->m_dwId, strEdit, *pRect, point, 0 );
		}
	}
	else	
	if( pShortcut->m_dwShortcut == ShortcutType::Chat)
	{
		g_toolTip.PutToolTip( 10000, pShortcut->m_szString, *pRect, point, 0 );
	}
	else
	if( pShortcut->m_dwShortcut == ShortcutType::Emoticon)
	{
		g_toolTip.PutToolTip( 10000, pShortcut->m_szString, *pRect, point, 0 );
	}
	else	
	if( pShortcut->m_dwShortcut == ShortcutType::Item)
	{
		g_WndMng.PutToolTip_Item( 0, pShortcut->m_dwId, point, pRect );
	}
	else if (pShortcut->m_dwShortcut == ShortcutType::PartySkill) {
		g_WndMng.PutToolTip_Troupe(pShortcut->m_dwId, point, pRect);
	} else if( pShortcut->m_dwShortcut == ShortcutType::Skill) {
		const SKILL * const lpSkill = g_pPlayer->GetSkill(pShortcut->m_dwId);
		g_WndMng.PutToolTip_Skill( lpSkill->dwSkill, lpSkill ? lpSkill->dwLevel : 0, point, pRect );
	}
	else	
	if( pShortcut->m_dwShortcut == ShortcutType::Lord)
	{
		CCLord*							pLord		= CCLord::Instance();
		CLordSkillComponentExecutable*	pComponent	= pLord->GetSkills()->GetSkill(pShortcut->m_dwId);
		CString							string, str;
		CEditString						strEdit;

		if(!pComponent) return;

		string.Format( "#b#cff2fbe6d%s#nb#nc\n%s", pComponent->GetName(), pComponent->GetDesc());
		if(pComponent->GetTick() != 0)
		{
			str.Format( prj.GetText(TID_TOOLTIP_COOLTIME), pComponent->GetTick(), 0);
			string += "\n";
			string += str;
		}
	
		strEdit.SetParsingString( string );
		g_toolTip.PutToolTip(10000, strEdit, *pRect, point, 0);
	}
	else	
	if( pShortcut->m_dwShortcut == ShortcutType::Motion)
	{
		MotionProp* pMotionProp = prj.GetMotionProp( pShortcut->m_dwId );
		if(!pMotionProp)		//061206 ma	8차에 들어갈 모션관리를 위해 버전 추가	propMotion.txt
			return;

		CString string = pMotionProp->szDesc;
		
		if( pMotionProp->dwID == MOT_BASE_CHEER )
		{
			if( g_pPlayer )
			{
				CString str;
				str.Format( prj.GetText(TID_CHEER_MESSAGE1), g_pPlayer->m_nCheerPoint );

				string += "\n\n";
				string += str;

				if( g_pPlayer->m_nCheerPoint < MAX_CHEERPOINT )
				{
					CTimeSpan ct( ((g_pPlayer->m_dwTickCheer - GetTickCount()) / 1000) );		// 남은시간을 초단위로 변환해서 넘겨줌

					if( ct.GetMinutes() <= 0 )
					{
						str.Format( prj.GetText(TID_PK_LIMIT_SECOND), ct.GetSeconds() );
					}
					else
					{
						str.Format( prj.GetText(TID_PK_LIMIT_MINUTE), ct.GetMinutes() );
					}
					
					string += "\n";
					string += (prj.GetText(TID_CHEER_MESSAGE2)+str);
				}
			}
		}

		CEditString strEdit;
		strEdit.SetParsingString( string );
		g_toolTip.PutToolTip( pShortcut->m_dwId , strEdit, *pRect, point, 0 );
	}
	else
	if( pShortcut->m_dwShortcut == ShortcutType::SkillFun)
	{
		strcpy( pShortcut->m_szString, prj.GetText(TID_GAME_SKILLSHORTCUT) );
		g_toolTip.PutToolTip( 10000, pShortcut->m_szString, *pRect, point, 0 );
	}
		
}

void CWndTaskBar::OnMouseWndSurface( CPoint point )
{
	CRect rect = CRect( POINT_APPLET_X, POINT_APPLET_Y, POINT_APPLET_X + ICON_SIZE, POINT_APPLET_Y + ICON_SIZE );
	for( int i = 0; i < m_nMaxSlotApplet; i++ )
	{
		LPSHORTCUT lpShortcut = &m_aSlotApplet[ i ] ;
		if( !lpShortcut->IsEmpty() && rect.PtInRect( point) )
		{
			PutTooTip( lpShortcut, point, &rect );
		}
		rect += CPoint( ICON_SIZE, 0 );
	}

	rect = CRect( POINT_ITEM_X, POINT_ITEM_Y, POINT_ITEM_X + ICON_SIZE, POINT_ITEM_Y + ICON_SIZE );
	for( int i = 0; i < MAX_SLOT_ITEM; i++ )
	{
		LPSHORTCUT lpShortcut = &m_paSlotItem()[i];
		if( !lpShortcut->IsEmpty() && rect.PtInRect( point)  )
		{
			PutTooTip( lpShortcut, point,&rect );
		}
		rect += CPoint( ICON_SIZE, 0 );
	}
	rect = CRect( POINT_QUEUE_X, POINT_QUEUE_Y, POINT_QUEUE_X + SKILL_SIZE, POINT_QUEUE_Y + SKILL_SIZE );
	for( int i = 0; i < MAX_SLOT_QUEUE; i++ )
	{
		LPSHORTCUT lpShortcut = &m_aSlotQueue[ i ] ;
		if( !lpShortcut->IsEmpty() && rect.PtInRect( point)  )
		{
			PutTooTip( lpShortcut, point,&rect );
		}
		rect += CPoint( SKILL_SIZE, 0 );
	}

	rect = CRect( POINT_QUEUE_X - ICON_SIZE - 5, POINT_QUEUE_Y, (POINT_QUEUE_X - ICON_SIZE - 5) + 32, POINT_QUEUE_Y + 32);

	if( rect.PtInRect( point) )
	{
		SHORTCUT Shortcut;
		Shortcut.m_dwShortcut = ShortcutType::SkillFun;
		PutTooTip( &Shortcut, point,&rect );
	}
}

static void DRAW_HOTKEY(C2DRender * p2DRender, CPoint point, char cHotkey) {
	CString string;
	string.Format( "%c", cHotkey );
	p2DRender->TextOut( point.x - 0 + 2, point.y - 0 - 4, string, 0xffffffff );
}

void CWndTaskBar::OnDraw( C2DRender* p2DRender )
{
	if( NULL == g_pPlayer )
		return;

	DWORD dwCur = g_tmCurrent;
	CRect rect = GetWindowRect();
	CRect rectSkillQueue;

	CD3DFont* pOldFont = p2DRender->GetFont(); 
	p2DRender->SetFont( CWndBase::m_Theme.m_pFontWndTitle ); 
		
	if( m_nPosition == TASKBAR_TOP || m_nPosition == TASKBAR_BOTTOM )
	{
		 rectSkillQueue.SetRect( rect.Width() - 32 * 4 - 6, 0, rect.Width(), rect.Height() );
	}
	else
	if( m_nPosition == TASKBAR_LEFT || m_nPosition == TASKBAR_RIGHT )
	{
		rectSkillQueue.SetRect( 0, rect.Height() - 32 * 4 - 6, rect.Width(), rect.Height() );
	}

	// 드래그 하이라이트 슬롯 표시
	if( m_GlobalShortcut.IsEmpty() == FALSE )
	{
		DWORD dwColor;
		if( m_dwHighAlpha < 128 )
			dwColor = D3DCOLOR_ARGB( m_dwHighAlpha, 0, 0, 255 );
		else
			dwColor = D3DCOLOR_ARGB( 128 - ( m_dwHighAlpha - 128 ), 0, 0, 255 );

		LPWNDCTRL pApplet = GetWndCtrl( WIDC_CUSTOM1 );
		LPWNDCTRL pItem   = GetWndCtrl( WIDC_CUSTOM2 );
		LPWNDCTRL pSkill  = GetWndCtrl( WIDC_CUSTOM3 );

		CRect rectApplet = pApplet->rect;
		CRect rectItem = pItem->rect;
		CRect rectSkill = pSkill->rect;

		rectApplet.top = 42;
		rectApplet.bottom = 47;
		rectApplet.left -= 6;
		rectApplet.right += 6;
		rectItem.top = 42;
		rectItem.bottom = 47;
		rectItem.left -= 6;
		rectItem.right += 6;
		rectSkill.top = 42;
		rectSkill.bottom = 47;
		rectSkill.left -= 3;
		rectSkill.right += 4;
		
		switch( m_GlobalShortcut.m_dwShortcut )
		{
			case ShortcutType::Applet:
			case ShortcutType::Item:
			case ShortcutType::Motion:
				p2DRender->RenderFillRect( rectApplet, dwColor );
				p2DRender->RenderFillRect( rectItem, dwColor );
				break;
			case ShortcutType::Skill:
				{
					const DWORD dwSkill = m_GlobalShortcut.m_dwId;
					DWORD dwComboStyleSrc = prj.GetSkillProp( dwSkill )->dwComboStyle;
					if( dwComboStyleSrc == CT_STEP  || dwComboStyleSrc == CT_GENERAL  )		// 스텝이나 제네럴이면 합격
						p2DRender->RenderFillRect( rectItem, dwColor );		
					p2DRender->RenderFillRect( rectSkill, dwColor );
				}
				break;
			case ShortcutType::PartySkill:
				p2DRender->RenderFillRect( rectItem, dwColor );
				break;
		}		
	}

	// 애플렛 아이콘 출력
	CPoint point = POINT_APPLET;
	for( int i = 0; i < m_nMaxSlotApplet; i++ )
	{
		SHORTCUT & lpShortcut = m_aSlotApplet[i];
		const bool isMissingItem = RenderShortcut(p2DRender, lpShortcut, point, true);

		if (isMissingItem) {
			FindNewStackForShortcut(std::nullopt, i);
		}

		point += CPoint( ICON_SIZE, 0 );
	}
	// 아이템 아이콘 출력 
	point = POINT_ITEM;
	for( int i = 0; i < MAX_SLOT_ITEM; i++ )
	{
		const SHORTCUT & lpShortcut = m_paSlotItem()[i];
		RenderShortcut(p2DRender, lpShortcut, point, false);

		// hotkey 출력 
		CHAR cHotkey = ( i == 9 ) ? '0' : '1' + i;
		CPoint ptHotkey( point.x + 8, point.y - 9 );

		point += CPoint( ICON_SIZE, 0 );
	}
	
	point = CPoint( POINT_QUEUE_X - ICON_SIZE - 5, POINT_QUEUE_Y );
	p2DRender->RenderTexture( point, m_pTexture );

	// 스킬 큐 
	point = POINT_QUEUE;
	for( int i = 0; i < m_nCurQueueNum; i++ )
	{
		LPSHORTCUT lpShortcut = &m_aSlotQueue[ i ] ;
		if( !lpShortcut->IsEmpty() && lpShortcut->m_pTexture)
		{
			p2DRender->RenderTexture( point, lpShortcut->m_pTexture );

//				LPSKILL pSkill = g_pPlayer->GetSkill( lpShortcut->m_dwType, lpShortcut->m_dwId );
//				if( g_pPlayer->m_nReUseDelay[ lpShortcut->m_dwId ] > 0 )
//					p2DRender->TextOut( point.x, point.y, g_pPlayer->m_nReUseDelay[ lpShortcut->m_dwId ] );
		}

		point += CPoint( SKILL_SIZE, 0 );
	}

	
	if( !( g_pPlayer->IsSMMode( SM_ACTPOINT ) ) )
	{

		point = POINT_QUEUE;
		point.x += SKILL_SIZE;	// 스킬 2번째칸부터 쉐이드된다.
		{
			int nAP2Size;	// 액션포인트를 하이라이트양으로 변환한값.
			if( m_nActionPoint >= LV4MAXAP )	// LV4MAXAP를 넘어서면 5단계 시작.
				nAP2Size = (SKILL_SIZE * 3) + (((m_nActionPoint - LV4MAXAP) * SKILL_SIZE) / (LV5MAXAP - LV4MAXAP));
			else if( m_nActionPoint >= LV3MAXAP )
				nAP2Size = (SKILL_SIZE * 2) + (((m_nActionPoint - LV3MAXAP) * SKILL_SIZE) / (LV4MAXAP - LV3MAXAP));
			else if( m_nActionPoint >= LV2MAXAP )
				nAP2Size = (SKILL_SIZE * 1) + (((m_nActionPoint - LV2MAXAP) * SKILL_SIZE) / (LV3MAXAP - LV2MAXAP));
			else 
				nAP2Size = (m_nActionPoint * SKILL_SIZE) / LV2MAXAP;
			
			p2DRender->m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
			p2DRender->m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
			point.x += nAP2Size;
			nAP2Size = (SKILL_SIZE * 4) - nAP2Size;
			p2DRender->RenderFillRect( CRect(point.x, point.y,  point.x + nAP2Size, point.y + SKILL_SIZE + 2), 
										D3DCOLOR_ARGB( 0x7f, 0, 0, 0 ) );
			
			if( m_nCurQueueNum > 0 )
			{
				int show = 1;
				
				if( m_nActionPoint >= LV2MAXAP )
					show = 2;
				if( m_nActionPoint >= LV3MAXAP )
					show = 3;
				if( m_nActionPoint >= LV4MAXAP )
					show = 4;
				if( m_nActionPoint >= LV5MAXAP )
					show = 5;
				
				if( m_nCurQueueNum <= show )
					show = m_nCurQueueNum;
				
				CPoint Point = CPoint( POINT_QUEUE_X, POINT_QUEUE_Y+1 );

				RenderOutLineLamp( POINT_QUEUE_X, POINT_QUEUE_Y+1, show, SKILL_SIZE );
			}
		}
	}

	CWndButton* pWndButton = (CWndButton*)GetDlgItem( WIDC_UP );
	rect = pWndButton->GetWindowRect( TRUE );
	rect.top = rect.bottom - 2;
	p2DRender->TextOut( rect.left, rect.top, m_nSlotIndex + 1, 0xffffffff );
	p2DRender->SetFont( pOldFont );
}

bool CWndTaskBar::RenderShortcut(
	C2DRender * p2DRender, const SHORTCUT & shortcut, CPoint point,
	bool drawLetter
) {
	if (shortcut.IsEmpty()) return true;

	if (shortcut.m_pTexture) {
		p2DRender->RenderTexture(point, shortcut.m_pTexture);
	}

	switch (shortcut.m_dwShortcut) {
		case ShortcutType::Item: {
			CItemElem * const pItemBase = g_pPlayer->GetItemId(shortcut.m_dwId);
			if (!pItemBase) return false;

			const ItemProp & itemProp = *pItemBase->GetProp();

			if (itemProp.dwPackMax > 1) {
				TCHAR szTemp[32];
				_stprintf(szTemp, "%d", g_pPlayer ? g_pPlayer->m_Inventory.GetItemCount(pItemBase->m_dwItemId) : 0);
				CSize size = m_p2DRender->m_pFont->GetTextExtent(szTemp);
				p2DRender->TextOut(point.x + 32 - size.cx, point.y + 32 - size.cy, szTemp, 0xff0000ff);
				p2DRender->TextOut(point.x + 31 - size.cx, point.y + 31 - size.cy, szTemp, 0xffb0b0f0);
			}

			if (g_pPlayer) {
				if (const auto cooldown = g_pPlayer->m_cooltimeMgr.GetElapsedTime(itemProp)) {
					RenderRadar(p2DRender, point, cooldown->elapsedTime, cooldown->totalWait);
				}
			}
			break;
		}
		case ShortcutType::Applet: {
			AppletFunc * pAppletFunc = g_WndMng.GetAppletFunc(shortcut.m_dwId);
			if (pAppletFunc && pAppletFunc->m_cHotkey) {
				CPoint ptHotkey(point.x + 8, point.y - 9);
				DRAW_HOTKEY(p2DRender, ptHotkey, pAppletFunc->m_cHotkey);
			}
			break;
		}
		case ShortcutType::Skill:
			RenderCollTime(point, shortcut.m_dwId, p2DRender);
			break;
		case ShortcutType::Lord:
			RenderLordCollTime(point, shortcut.m_dwId, p2DRender);
			break;
		case ShortcutType::Motion: {
			if (shortcut.m_dwId == MOT_BASE_ESCAPE) {
				ItemProp * pItem = prj.GetItemProp(g_AddSMMode.dwSMItemID[SM_ESCAPE]);

				if (pItem && g_pPlayer) {
					CTimeSpan ct(g_pPlayer->m_dwSMTime[SM_ESCAPE]);

					if (ct.GetTotalSeconds()) {
						point.y -= 1;
						point.x += 1;
						RenderRadar(m_p2DRender, point, (DWORD)(pItem->dwCircleTime - ct.GetTotalSeconds()), pItem->dwCircleTime);
					}
				}
			}
		}
	}

	return true;
}

void CWndTaskBar::UpdateItem() {
	if (!g_pPlayer) return;

	for (int i = 0; i < m_paSlotItem().size(); ++i) {
		SHORTCUT & shortcut = m_paSlotItem()[i];
		if (shortcut.m_dwShortcut != ShortcutType::Item) continue;
		
		CItemElem * pItemElem = g_pPlayer->GetItemId(shortcut.m_dwId);

		if (pItemElem) {
			if (shortcut.m_pTexture != pItemElem->GetTexture()) {
				shortcut.m_pTexture = pItemElem->GetTexture();
			}
			continue;
		}

		FindNewStackForShortcut(m_nSlotIndex, i);
	}
}

void CWndTaskBar::FindNewStackForShortcut(std::optional<int> where, int i) {
	if (where && where < 0 || where >= MAX_SLOT_ITEM_COUNT) {
		where = 0;
	}

	SHORTCUT & shortcut = where ? m_aSlotItem[*where][i] : m_aSlotApplet[i];

	const auto SendRemove = [&]() {
		shortcut.Empty();
		if (where) {
			g_DPlay.SendRemoveItemTaskBar(*where, i);
		} else {
			g_DPlay.SendRemoveAppletTaskBar(i);
		}
	};

	CItemElem * pItemElem = g_pPlayer->GetItemId(shortcut.m_dwId);
	if (pItemElem) return;

	const ItemProp * const pItemProp = prj.GetItemProp(shortcut.m_dwItemId);
	if (!pItemProp || pItemProp->dwPackMax <= 1) {
		SendRemove();
		return;
	}

	const DWORD dwId = g_pPlayer->m_Inventory.Find(shortcut.m_dwItemId);
	if (dwId == NULL_ID) {
		SendRemove();
		return;
	}

	shortcut.m_dwId = dwId;
	g_DPlay.SendAddInTaskbar(where, i, shortcut);
}

void CWndTaskBar::OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags )
{
}
void CWndTaskBar::OnInitialUpdate()
{
	CWndNeuz::OnInitialUpdate();

	LPWNDCTRL pApplet = GetWndCtrl( WIDC_CUSTOM1 );
	LPWNDCTRL pItem   = GetWndCtrl( WIDC_CUSTOM2 );
	LPWNDCTRL pSkill  = GetWndCtrl( WIDC_CUSTOM3 );

	POINT_APPLET_X = pApplet->rect.left;
	POINT_APPLET_Y = pApplet->rect.top;
	POINT_ITEM_X   = pItem->rect.left;
	POINT_ITEM_Y   = pItem->rect.top;
	POINT_QUEUE_X  = pSkill->rect.left;
	POINT_QUEUE_Y  = pSkill->rect.top;

	CRect rect( 5, 7, 754, 40 );

	g_WndMng.m_pWndTaskBar = this;

	{
		rect = g_Neuz.GetDeviceRect();
		m_pWndRoot->m_rectLayout = rect;
		switch( m_nPosition )
		{
		case TASKBAR_TOP:
			rect.bottom = TASKBAR_HEIGHT;
			m_pWndRoot->m_rectLayout.top = rect.bottom;
			break;
		case TASKBAR_BOTTOM:
			rect.top = rect.bottom - TASKBAR_HEIGHT;
			m_pWndRoot->m_rectLayout.bottom = rect.top;
			break;
		case TASKBAR_LEFT:
			rect.right = TASKBAR_HEIGHT;
			m_pWndRoot->m_rectLayout.left = rect.right;
			break;
		case TASKBAR_RIGHT:
			rect.left = rect.right - TASKBAR_HEIGHT;
			m_pWndRoot->m_rectLayout.right = rect.left;
			break;
		}
		SetWndRect( rect );
	}
	m_texPack.LoadScript( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, "hotkey.inc" ) );

	m_menuShortcut.CreateMenu( this );	
	m_menuShortcut.AppendMenu( 0, 0 ,_T( "삭제" ) );

	m_pTexture = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice,  MakePath( DIR_ICON, "icon_ActionSkill.dds" ), 0xffff00ff );
	m_aSlotSkill.m_dwShortcut = ShortcutType::SkillFun;
	m_aSlotSkill.m_pTexture   = m_pTexture;
}

HRESULT CWndTaskBar::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
	
#ifdef __YDEBUG
	m_texPack.InvalidateDeviceObjects();
#endif //__YDEBUG
	return S_OK;
}

HRESULT CWndTaskBar::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	
#ifdef __YDEBUG
	m_texPack.DeleteDeviceObjects();
#endif //__YDEBUG
	return S_OK;
}

HRESULT CWndTaskBar::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	
#ifdef __YDEBUG
	m_texPack.RestoreDeviceObjects(m_pApp->m_pd3dDevice);
#endif //__YDEBUG
	
	return 0;
}

BOOL CWndTaskBar::Initialize(CWndBase* pWndParent,DWORD dwWndId)
{
	CRect rect = g_Neuz.GetDeviceRect();
	//m_rectLayout = rect;
/*
	switch( m_nPosition )
	{
	case TASKBAR_TOP:
		rect.bottom = TASKBAR_HEIGHT;
		m_pWndRoot->m_rectLayout.top = rect.bottom;
		break;
	case TASKBAR_BOTTOM:
		rect.top = rect.bottom - TASKBAR_HEIGHT;
		m_pWndRoot->m_rectLayout.bottom = rect.top;
		break;
	case TASKBAR_LEFT:
		break;
	case TASKBAR_RIGHT:
		break;
	}
	*/
	//m_pWndRoot->SetWndRect( rectRoot );

//CMainFrame
	//rect.top = rect.bottom;
	//rect.bottom += TASKBAR_HEIGHT;
//	rect.bottom = CLIENT_HEIGHT;
	//m_strMessage = lpszMessage;
	//m_dwType = nType;
	//SetTitle("메지시 윈도");
//	return CWndBase::Create( 0 | /*WBS_MODAL|*/ WBS_MANAGER | WBS_SOUND | WBS_NOFRAME, rect, &g_WndMng, APP_TASKBAR );
	BOOL bResult;
	if( g_Option.m_nResWidth == 800 )
	{
		m_nMaxSlotApplet = 5;
		bResult = CWndNeuz::InitDialog( APP_TASKBAR800, pWndParent, WBS_MANAGER | WBS_SOUND, CPoint( 0, 0 ) );
	}
	else
	if( g_Option.m_nResWidth == 1024 )
	{
		m_nMaxSlotApplet = 10;
		bResult = CWndNeuz::InitDialog( APP_TASKBAR1024, pWndParent, WBS_MANAGER | WBS_SOUND, CPoint( 0, 0 ) );
	}
	else
	if( g_Option.m_nResWidth == 1280 )
	{
		m_nMaxSlotApplet = MAX_SLOT_APPLET;
		bResult = CWndNeuz::InitDialog( APP_TASKBAR1280, pWndParent, WBS_MANAGER | WBS_SOUND, CPoint( 0, 0 ) );
	}
	else if( g_Option.m_nResWidth == 1360 )
	{
		m_nMaxSlotApplet = MAX_SLOT_APPLET;
		bResult = CWndNeuz::InitDialog( APP_TASKBAR1360W, pWndParent, WBS_MANAGER | WBS_SOUND, CPoint( 0, 0 ) );
	}
	else if( g_Option.m_nResWidth == 1400 )
	{
		m_nMaxSlotApplet = MAX_SLOT_APPLET;
		bResult = CWndNeuz::InitDialog( APP_TASKBAR1400, pWndParent, WBS_MANAGER | WBS_SOUND, CPoint( 0, 0 ) );
	}
	else if( g_Option.m_nResWidth == 1440 )
	{
		m_nMaxSlotApplet = MAX_SLOT_APPLET;
		bResult = CWndNeuz::InitDialog( APP_TASKBAR1440W, pWndParent, WBS_MANAGER | WBS_SOUND, CPoint( 0, 0 ) );
	}
	else if( g_Option.m_nResWidth == 1600 )
	{
		m_nMaxSlotApplet = MAX_SLOT_APPLET;
		bResult = CWndNeuz::InitDialog( APP_TASKBAR1600, pWndParent, WBS_MANAGER | WBS_SOUND, CPoint( 0, 0 ) );
	}
	else if( g_Option.m_nResWidth == 1680 )
	{
		m_nMaxSlotApplet = MAX_SLOT_APPLET;
		bResult = CWndNeuz::InitDialog( APP_TASKBAR1680W, pWndParent, WBS_MANAGER | WBS_SOUND, CPoint( 0, 0 ) );
	}	
	
	DelWndStyle( WBS_MOVE );
	return bResult;
	
}

void CWndTaskBar::SetItemSlot( int nSlot )
{
	if (nSlot >= 0 && nSlot < MAX_SLOT_ITEM_COUNT) {
		m_nSlotIndex = nSlot;
	}
}
BOOL CWndTaskBar::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{
	CWndButton* pWndButton = (CWndButton*) pLResult;
	if( message == WIN_ITEMDROP )
	{
	}
	else
	switch( nID )
	{
		case WIDC_UP:
			m_nSlotIndex--;
			if( m_nSlotIndex < 0 ) m_nSlotIndex = MAX_SLOT_ITEM_COUNT-1;
			break;
		case WIDC_DOWN:
			m_nSlotIndex++;
			if( m_nSlotIndex > MAX_SLOT_ITEM_COUNT-1 ) m_nSlotIndex = 0;
			break;
		case WIDC_BUTTON1: // Menu
		{
			g_WndMng.OpenMenu();
			CRect rect = g_Neuz.GetDeviceRect();
			CRect rectMenu = m_wndMenu.GetScreenRect();
			switch( m_nPosition )
			{
			case TASKBAR_TOP:
				if( g_WndMng.m_pWndMenu )
					g_WndMng.m_pWndMenu->Move( CPoint( 0, TASKBAR_HEIGHT ) );
				break;
			case TASKBAR_BOTTOM:
				if( g_WndMng.m_pWndMenu )
					g_WndMng.m_pWndMenu->Move( CPoint( 0, rect.bottom - TASKBAR_HEIGHT - g_WndMng.m_pWndMenu->GetWndRect().Height() ) );
				break;
			case TASKBAR_LEFT:
				if( g_WndMng.m_pWndMenu )
					g_WndMng.m_pWndMenu->Move( CPoint( TASKBAR_HEIGHT, 0 ) );
				break;
			case TASKBAR_RIGHT:
				if( g_WndMng.m_pWndMenu )
					g_WndMng.m_pWndMenu->Move( CPoint( rect.right - TASKBAR_HEIGHT - g_WndMng.m_pWndMenu->GetWndRect().Width(), 0 ) );
				break;
			}

		}
		default:
			{
				switch( pWndButton->m_shortcut.m_dwIndex )
				{
					case APP_TRADE:		g_DPlay.SendTrade( (CMover*)g_WorldMng.Get()->GetObjFocus() );	break;
					default:
						g_WndMng.ObjectExecutor( &pWndButton->m_shortcut );	
						break;
				}
			}
	}
	return TRUE;
}
BOOL CWndTaskBar::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return TRUE;
}
void CWndTaskBar::OnSize(UINT nType, int cx, int cy)
{
	CWndNeuz::OnSize( nType, cx, cy );
}
void CWndTaskBar::RemoveSkillQueue( int nIndex, BOOL bSend )
{
	if( m_nExecute )	return;		// 스킬큐 실행중엔 빼지지도 않는다.
BACK:
	if( !m_aSlotQueue[ nIndex ].IsEmpty() )
	{
		const ItemProp* pItemProp = g_pPlayer->GetSkill( m_aSlotQueue[ nIndex ].m_dwId )->GetProp();
		DWORD dwComboStyleSrc = pItemProp->dwComboStyle;
		int i = NULL;
		for( i = nIndex; i < MAX_SLOT_QUEUE - 1; i++ )
		{
			memcpy( &m_aSlotQueue[ i ], &m_aSlotQueue[ i + 1 ], sizeof( SHORTCUT ) );
			m_aSlotQueue[ i ].m_dwIndex = i;
		}
		m_aSlotQueue[i].Empty();
		m_nCurQueueNum--;
		if( m_nCurQueueNum < 0 ) 
			m_nCurQueueNum = 0;
		if( CheckAddSkill( dwComboStyleSrc, nIndex /*- 1*/ ) == FALSE )
			goto BACK;
	}
	if( bSend )
		g_DPlay.SendSkillTaskBar( );
	// 스킬을 지우는 곳 서버로 보내야함.	

}
void CWndTaskBar::OnRButtonUp( UINT nFlags, CPoint point )
{
	CRect rect = RECT_APPLET;
	if( rect.PtInRect( point ) )
	{
		point.x -= rect.left;
		point.x /= ICON_SIZE;
		if( FALSE == m_aSlotApplet[ point.x ].IsEmpty() )
		{
#ifdef __MAINSERVER
			if( point.x == 0 && m_aSlotApplet[ point.x ].m_dwId == 400 )
				return;
#endif //__MAINSERVER
			
			m_aSlotApplet[ point.x ].Empty();			
			g_DPlay.SendRemoveAppletTaskBar( (BYTE)( point.x ) );		// 아이템 삭제 서버로 전송
		}
	}
	rect = RECT_ITEM;
	if( rect.PtInRect( point ) )
	{
		point.x -= rect.left;

		point.x /= ICON_SIZE;
		if( FALSE == m_paSlotItem()[point.x].IsEmpty())
		{
			m_paSlotItem()[point.x].Empty();
			g_DPlay.SendRemoveItemTaskBar( (BYTE)( m_nSlotIndex ), (BYTE)( point.x ) );	// 아이템 삭제 서버로 전송			
		}

	}
	rect = RECT_QUEUE;
	if( rect.PtInRect( point ) )
	{
		point.x -= rect.left;
		point.y -= rect.top;
		point.x /= SKILL_SIZE;
		RemoveSkillQueue( point.x );
	}
}

void CWndTaskBar::SetShortcut( int nIndex, ShortcutType dwShortcut, DWORD dwType, DWORD dwId, CTexture* pTexture, CTaskbar::BarName nWhere) {
	SHORTCUT & pShortcut = nWhere == CTaskbar::BarName::Applet ? m_aSlotApplet[nIndex] : m_paSlotItem()[nIndex];

	if (!CanAddShortcut(dwShortcut, pShortcut)) {
		g_WndMng.PutString(TID_GAME_MAX_SHORTCUT_CHAT);
		return;
	}

	pShortcut.m_dwShortcut = dwShortcut;
	pShortcut.m_dwIndex = nIndex;
	pShortcut.m_dwId = dwId;
	pShortcut.m_dwUserId = 0;
	pShortcut.m_dwData = nWhere == CTaskbar::BarName::Applet ? 0 : 1;
	strcpy(pShortcut.m_szString, m_GlobalShortcut.m_szString);

	if (dwShortcut == ShortcutType::Lord)
		pShortcut.m_dwId--;

	if (dwShortcut == ShortcutType::Item) {
		CItemElem * pItemBase = g_pPlayer->GetItemId(dwId);
		if (pItemBase && pItemBase->GetProp()->dwPackMax > 1) {
			pShortcut.m_dwItemId = pItemBase->m_dwItemId;
		}
	}

	if (pTexture) {
		pShortcut.m_pTexture = pTexture;
	} else {
		SetTaskBarTexture(pShortcut);
	}

	std::optional<unsigned int> where;
	if (nWhere == CTaskbar::BarName::Applet) where = std::nullopt;
	else where = m_nSlotIndex;

	g_DPlay.SendAddInTaskbar(where, nIndex, pShortcut);
}
BOOL CWndTaskBar::CheckAddSkill( int nSkillStyleSrc, int nQueueDest  )
{
	/*
	START -> FINISH, FG 
	CIRCLE -> START, CIRCLE
	FINISH -> START, CIRCLE
	*/
	DWORD dwComboStyleSrc = nSkillStyleSrc;
	if( nQueueDest == 0 )		// 첫칸에 넣으려 시도 했을경우
	{
		/*
		if( 이미 스킬창에 등록되어 있다 )
		{
			if( 삽입하려는게 CT_GENERAL인가 )
				if( 이미 삽입되어 있는게 CT_GENERAL인가 )
				return TRUE;
			return FALSE;		// CT_GENERAL이외의 것을 첫칸에 삽입하려면 모두 실패.
		} else
		{
			// 아무것도 등록되어 있지 않다.
			// 스텝, 제네럴만 합격.
			if( dwComboStyleSrc == CT_STEP  || dwComboStyleSrc == CT_GENERAL  ) 
				return TRUE;
			return FALSE;	// 그외엔 모두 불합격.
		}
		 */
		if( m_nCurQueueNum == 0 )		// 비어있다
		{
			if( dwComboStyleSrc == CT_STEP  || dwComboStyleSrc == CT_GENERAL  )		// 스텝이나 제네럴이면 합격
				return TRUE;
			return	FALSE;
		} else
		// 뭔가 있다.
		{
			DWORD dwComboStyleDest;
			DWORD dwSkill = m_aSlotQueue[nQueueDest].m_dwId;
				//m_aSlotQueue[ nQueueDest ].m_dwId;	// 넣으려는 칸의 스킬
			if( dwSkill )
				dwComboStyleDest = prj.GetSkillProp( dwSkill )->dwComboStyle;		// 그칸의 스타일
			else 
				dwComboStyleDest = 0;
			if( dwComboStyleSrc == CT_GENERAL && dwComboStyleDest == CT_GENERAL )	// 이미있는것도 제네럴이고 삽입하려는것도 제네럴이면 합격
				return TRUE;
			return FALSE;	// 그외는 모두 첫칸에 삽입할 수 없음.
		}
	}
	else
	if( nQueueDest == 4 )		// 마지막칸에 넣으려고 했을경우
	{
		if( dwComboStyleSrc == CT_STEP )	// 스텝은 첫칸이 아니면 무조건 못넣는다.
			return FALSE;
		DWORD dwSkill = m_aSlotQueue[ nQueueDest - 1 ].m_dwId;
			//m_aSlotQueue[ nQueueDest - 1 ].m_dwId;	// 넣으려는 칸의 앞에있는 스킬
		DWORD dwComboStylePrev;
		if( dwSkill )
			dwComboStylePrev = prj.GetSkillProp( dwSkill )->dwComboStyle;		// 앞칸의 스타일
		else
			dwComboStylePrev = 0;
		if( dwComboStylePrev == CT_FINISH )		// 앞 스킬이 피니쉬면 더이상 넣을 수 없다.
			return FALSE;
		// 서클기술을 넣으려하고 앞칸에 제네럴이면 못넣음.
		if( dwComboStyleSrc == CT_CIRCLE && dwComboStylePrev == CT_GENERAL )
			return FALSE;
		// 피니쉬(암놈)를 넣으려하고 앞칸에 제네럴이면 못넣음.
		if( dwComboStyleSrc == CT_FINISH && dwComboStylePrev == CT_GENERAL )
			return FALSE;
		// 제네럴을 넣으려하고 앞칸이 제네럴이 아니면 못넣음.
		if( dwComboStyleSrc == CT_GENERAL && dwComboStylePrev != CT_GENERAL )
			return FALSE;
		
		return TRUE;	// 그외는 모두 합격
	}
	else
	// 그외 중간에 넣으려 한경우.
	{
		// 둘째칸 이상부터 넣으려고 했을때 처리.
		if( dwComboStyleSrc == CT_STEP )	// 스텝은 첫칸이 아니면 무조건 못넣는다.
			return FALSE;
		DWORD dwComboStylePrev;
		DWORD dwComboStyleDest;
		DWORD dwSkill = m_aSlotQueue[ nQueueDest - 1 ].m_dwId;
			//m_aSlotQueue[ nQueueDest - 1 ].m_dwId;	// 넣으려는 칸의 앞에있는 스킬
		if( dwSkill )
			dwComboStylePrev = prj.GetSkillProp( dwSkill )->dwComboStyle;		// 앞칸의 스타일
		else
			dwComboStylePrev = 0;
		dwSkill = m_aSlotQueue[ nQueueDest ].m_dwId;
		//dwSkill = m_aSlotQueue[ nQueueDest ].m_dwId;	// 넣으려는 칸의 스킬
		if( dwSkill )
			dwComboStyleDest = prj.GetSkillProp( dwSkill )->dwComboStyle;		// 그칸의 스타일
		else
			dwComboStyleDest = 0;
		if( dwComboStylePrev == CT_FINISH )		// 앞 스킬이 피니쉬면 더이상 넣을 수 없다.
			return FALSE;
		// 서클기술을 넣으려하고 앞칸에 제네럴이면 못넣음.
		if( dwComboStyleSrc == CT_CIRCLE && dwComboStylePrev == CT_GENERAL )
			return FALSE;
		// 피니쉬(암놈)를 넣으려하고 앞칸에 제네럴이면 못넣음.
		if( dwComboStyleSrc == CT_FINISH && dwComboStylePrev == CT_GENERAL )
			return FALSE;
		// 피니쉬를 넣으려하고 그칸에 서클이면 못넣음.
		if( dwComboStyleSrc == CT_FINISH && dwComboStyleDest == CT_CIRCLE )
			return FALSE;
		// 피니쉬를 넣으려하고 그칸에 피니쉬이면 못넣음.
		if( dwComboStyleSrc == CT_FINISH && dwComboStyleDest == CT_FINISH )
			return FALSE;
		// 제네럴을 넣으려하고 
		if( dwComboStyleSrc == CT_GENERAL )
		{
			if( dwComboStylePrev != CT_GENERAL )	// 앞칸이 제네럴이 아니면 못넣음.
				return FALSE;
			if( dwComboStyleDest != CT_GENERAL && m_nCurQueueNum > nQueueDest )	// 그칸이 제네럴이 아니면 못넣는데 맨 끝이면 넣을수 있다.
					return FALSE;
		}
		
		return TRUE;	// 그 외는 모두 합격
	}
	return FALSE;
}
BOOL CWndTaskBar::SetSkillQueue( int nIndex, const DWORD skillId, CTexture* pTexture )
{
	if( m_nExecute )		return FALSE;		// 스킬큐 실행중엔 등록 안됨.
	if( m_nCurQueueNum >= 5 )
		return FALSE;

	// m_nCurQueueNum 보다 작은 슬롯에 비어있는게 있다면 오류 상태다. 클리어하자.
	for( int i = 0; i < m_nCurQueueNum; i++ )
	{
		if( m_aSlotQueue[ i ].IsEmpty() )
		{
			for( i = 0; i < m_nCurQueueNum; i++ )
				m_aSlotQueue[ i ].Empty();
			m_nCurQueueNum = 0;
			break;
		}
	}
	 if( nIndex > m_nCurQueueNum )
		 nIndex = m_nCurQueueNum;

	LPSHORTCUT pShortcut = NULL;
	CRect rect = GetWindowRect();
	SKILL * lpSkill = g_pPlayer->GetSkill(skillId);
	const DWORD dwLevel = lpSkill->dwLevel;
	const ItemProp * pSkillProp = prj.GetSkillProp( skillId );
	const DWORD dwComboStyleSrc = pSkillProp->dwComboStyle;

	if( dwLevel <= 0 )
		return FALSE;

	AddSkillProp* pAddSkillProp	= prj.GetAddSkillProp( pSkillProp->dwSubDefine, dwLevel );
	ASSERT( pAddSkillProp );
	if ((int)pAddSkillProp->dwCooldown > 0)	//  쿨타임있는 스킬은 액션슬롯에 못들어감
	{
		g_WndMng.PutString(TID_GAME_SKILLLNOTUSE, pSkillProp->szName);
		return FALSE;
	}

//	DWORD dwComboStyleDest;
//	ItemProp* pItemProp1, * pItemProp2,* pItemProp3; 
	if( nIndex < m_nCurQueueNum ) // insert		// nIndex는 세팅할 칸의 인덱스
	{
		if( !CheckAddSkill( dwComboStyleSrc, nIndex ) )		// Src를 nIndex에 넣어도 되는지 체크
			return FALSE;
		for( int i = m_nCurQueueNum; i > nIndex; i-- )
		{
			memcpy( &m_aSlotQueue[ i ], &m_aSlotQueue[ i - 1 ], sizeof( SHORTCUT ) );
		}
		pShortcut = &m_aSlotQueue[ nIndex ];
		m_nCurQueueNum++;
	}
	else // add
	{
		if( !CheckAddSkill( dwComboStyleSrc,  m_nCurQueueNum ) )	// 제일 마지막 칸에 넣어도 되는지 테스트.
			return FALSE;
		pShortcut = &m_aSlotQueue[ m_nCurQueueNum ];
		nIndex = m_nCurQueueNum;
		m_nCurQueueNum++;
	}
	
	if( pTexture == NULL )
	{
		LPSKILL lpSkill = g_pPlayer->GetSkill( skillId );
		pShortcut->m_pTexture = m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ICON, lpSkill->GetProp()->szIcon), 0xff000000, FALSE );
	}
	else pShortcut->m_pTexture = pTexture;


	pShortcut->m_dwShortcut = ShortcutType::Skill;
	pShortcut->m_dwIndex    = nIndex;//dwIndex; // 스킬 콘트롤에서의 순서 
	pShortcut->m_dwId       = skillId; // 스킬 인덱스 
	pShortcut->m_dwUserId   = 0 ;
	pShortcut->m_dwData     = 2;
	_tcscpy( pShortcut->m_szString, m_GlobalShortcut.m_szString );
	g_DPlay.SendSkillTaskBar();
	// 스킬 등록 여기서 서버로 보내야 함
	return TRUE;
}

BOOL CWndTaskBar::OnDropIcon( LPSHORTCUT pShortcut, CPoint point )
{
	CRect rect = GetWindowRect();
	rect.DeflateRect( 3, 3);
	// 밖으로 버렸을 경우 
	if( pShortcut->m_pFromWnd == this )
	{
		LPSHORTCUT lpShortcut = (LPSHORTCUT) pShortcut->m_dwData;
		// 포인트가 테스크바가 아닌 곳이라면. 의도적으로 숏컷을 지우려고 태스크바 밖에 놓은 것임. 지우자.
		if( GetClientRect().PtInRect( point ) == FALSE )
		{
			// 아이템 삭제 서버로 전송
			if( lpShortcut->m_dwData == 0 )
				g_DPlay.SendRemoveAppletTaskBar( (BYTE)( lpShortcut->m_dwIndex ) );
			else
			if( lpShortcut->m_dwData == 1 )
				g_DPlay.SendRemoveItemTaskBar( m_nSlotIndex, (BYTE)( lpShortcut->m_dwIndex ) );
			else
			if( lpShortcut->m_dwData == 2 )
			{
				RemoveSkillQueue( lpShortcut->m_dwIndex );
				return TRUE;
				//g_DPlay.SendSkillTaskBar( );
			}
			lpShortcut->Empty();
			return TRUE;
		}
	//	return TRUE;
	}
	CWndBase* pWndFrame =  pShortcut->m_pFromWnd->GetFrameWnd();


	if( pWndFrame && ( pWndFrame->GetWndId() != APP_INVENTORY && pWndFrame != g_WndMng.m_pWndTaskBar && pWndFrame->GetWndId() != APP_COMMUNICATION_CHAT && pWndFrame->GetWndId() != APP_SKILL3 && pWndFrame->GetWndId() != APP_MOTION && pWndFrame->GetWndId() != APP_PARTY && pWndFrame->GetWndId() != APP_LORD_SKILL) )
	{
		SetForbid( TRUE );
		return FALSE;
	}
	
	if( pShortcut->m_dwShortcut == ShortcutType::Skill ) // 극단스킬은 안들어감
	{
		SKILL * pSkill = g_pPlayer->GetSkill( pShortcut->m_dwId );
		if( pSkill && (pSkill->dwLevel <= 0 || g_pPlayer->CheckSkill( pSkill->dwSkill ) == FALSE) )
		{
			SetForbid( TRUE );
			return FALSE;
		}
	}
	if( pShortcut->m_dwId == 0 && strlen( pShortcut->m_szString ) < 1
		&& pShortcut->m_dwShortcut != ShortcutType::SkillFun
		&& pShortcut->m_dwShortcut != ShortcutType::Lord)
	{
		SetForbid( TRUE );
		return FALSE;
	}
	// 내부에서 이동되거나, 외부에서 내부로 
	if( m_nPosition == TASKBAR_TOP || m_nPosition == TASKBAR_BOTTOM )
	{
		BOOL bForbid = TRUE;
		CRect rect = RECT_APPLET;
		if( rect.PtInRect( point ) )
		{
			point.x -= rect.left;
			point.y -= rect.top;
			point.x /= ICON_SIZE;
			// 내부에서 이동 
			if( pShortcut->m_pFromWnd == this )
			{
				LPSHORTCUT lpCurShortcut = (LPSHORTCUT) pShortcut->m_dwData;			

				if( &m_aSlotApplet[ point.x ] == lpCurShortcut )
				{
					//SetForbid( TRUE );
					return FALSE;
				}
				// 오리지날이 액션슬롯이다.
				//if( lpCurShortcut->m_dwData == 2 )
				//	RemoveSkillQueue( lpCurShortcut->m_dwIndex );
			}
			if( pShortcut->m_dwShortcut == ShortcutType::Skill)
			{
				const ItemProp * const pProp = prj.GetSkillProp(pShortcut->m_dwId);

				if( pProp->dwComboStyle != CT_STEP && pProp->dwComboStyle != CT_GENERAL )	
				{
					SetForbid( TRUE );
					return FALSE;	// 퀵슬롯에 등록하려는 스킬이 스텝기술이 아닌건 등록 못함.
				}
			}

#ifdef __MAINSERVER
			if( point.x == 0 && m_aSlotApplet[ point.x ].m_dwId == APP_WEBBOX)
				return FALSE;
#endif //__MAINSERVER
				
			SetShortcut( point.x, pShortcut->m_dwShortcut, 0, pShortcut->m_dwId, pShortcut->m_pTexture, CTaskbar::BarName::Applet );
			bForbid = FALSE;
		}
		rect = RECT_ITEM;
//		rect.right += 32;
		if( rect.PtInRect( point ) )
		{
			point.x -= rect.left;
			point.y -= rect.top;
			point.x /= ICON_SIZE;
			// 내부에서 이동 
			if( pShortcut->m_pFromWnd == this )
			{
				LPSHORTCUT lpCurShortcut = (LPSHORTCUT) pShortcut->m_dwData;
				if( &m_aSlotItem[ m_nSlotIndex ][ point.x ] == lpCurShortcut )
					return FALSE;
				// 오리지날이 액션슬롯이다.
				//if( lpCurShortcut->m_dwData == 2 )
				//	RemoveSkillQueue( lpCurShortcut->m_dwIndex );
			}
			if( pShortcut->m_dwShortcut == ShortcutType::Chat)
			{
				CString sChat	= m_GlobalShortcut.m_szString;
				sChat.Replace( "--", "" );
				sChat.Replace( "'", "" );
				sChat.Replace( "/*", "" );
				sChat.Replace( "*/", "" );
				strcpy( m_GlobalShortcut.m_szString, (const char*)sChat );
			}
			else if( pShortcut->m_dwShortcut == ShortcutType::Skill)
			{
				const ItemProp * const pProp = prj.GetSkillProp(pShortcut->m_dwId);

				if( pProp->dwComboStyle != CT_STEP && pProp->dwComboStyle != CT_GENERAL )	
				{
					SetForbid( TRUE );
					return FALSE;	// 퀵슬롯에 등록하려는 스킬이 스텝기술이 아닌건 등록 못함.
				}
			}
			SetShortcut( point.x, pShortcut->m_dwShortcut, 0, pShortcut->m_dwId, pShortcut->m_pTexture, CTaskbar::BarName::Item);
			bForbid = FALSE;
		}
		rect = RECT_QUEUE;
		if( rect.PtInRect( point ) )
		{
			if( pShortcut->m_dwShortcut == ShortcutType::Skill )		// 스킬일경우만 등록
			{
				point.x -= rect.left;
				point.y -= rect.top;
				point.x /= SKILL_SIZE;
				// 내부에서 이동 ; 보통은 아래 부분에서 삭제를 처리하나, 스킬 큐에서의 이동은 삭제후 바로 추가해야하기 때문에 여기서 삭제를 한다.
				if( pShortcut->m_pFromWnd == this )
				{
					LPSHORTCUT lpShortcut = (LPSHORTCUT) pShortcut->m_dwData;
					if( lpShortcut->m_dwData == 2 )
					{
						RemoveSkillQueue( lpShortcut->m_dwIndex, FALSE );
						SetSkillQueue( point.x, pShortcut->m_dwId, pShortcut->m_pTexture );// == FALSE )
						return TRUE;
					}
				}
				if( SetSkillQueue( point.x, pShortcut->m_dwId, pShortcut->m_pTexture ) == FALSE )
					SetForbid( TRUE );
				bForbid = FALSE;
			}
		}
		if( pShortcut->m_pFromWnd != this )
			SetForbid( bForbid );
	}
	// 안으로 버렸을 경우, 또는 안에서 이동했을 경우 오리지날 부분 삭제  
	// 태스크바에 있는 아이템을 슬롯 안의 공백 부분에 드래그했을 경우 
	if( pShortcut->m_pFromWnd == this )
	{
		LPSHORTCUT lpShortcut = (LPSHORTCUT) pShortcut->m_dwData;
		// 아이템 삭제 서버로 전송
		if( lpShortcut->m_dwData == 0 )
			g_DPlay.SendRemoveAppletTaskBar( (BYTE)( lpShortcut->m_dwIndex ) );
		else
		if( lpShortcut->m_dwData == 1 )
			g_DPlay.SendRemoveItemTaskBar( m_nSlotIndex, (BYTE)( lpShortcut->m_dwIndex ) );
		else
		if( lpShortcut->m_dwData == 2 )
		{
			RemoveSkillQueue( lpShortcut->m_dwIndex );
			//g_DPlay.SendSkillTaskBar( );
			return TRUE;
		}
		lpShortcut->Empty();
	}
	return TRUE;
}
LPSHORTCUT CWndTaskBar::Select( CPoint point )
{
	CRect rect = RECT_APPLET;
	LPSHORTCUT pShortcut = NULL;
	if( rect.PtInRect( point ) )
	{
		point.x -= rect.left;
		point.y -= rect.top;
		point.x /= ICON_SIZE;
		pShortcut = &m_aSlotApplet[ point.x ];
		if( !pShortcut->IsEmpty() )
		{
			return pShortcut;
		}
	}
	rect = RECT_ITEM;
	if( rect.PtInRect( point ) )
	{
		point.x -= rect.left;
		point.y -= rect.top;
		point.x /= ICON_SIZE;
		pShortcut = &m_paSlotItem()[point.x];
		if( !pShortcut->IsEmpty() )
			return pShortcut;
	}
	rect = RECT_QUEUE;
	if( rect.PtInRect( point ) )
	{
		point.x -= rect.left;
		point.y -= rect.top;
		point.x /= SKILL_SIZE;
		pShortcut = &m_aSlotQueue[ point.x ];
		if( !pShortcut->IsEmpty() )
			return pShortcut;

	}
	rect = CRect( POINT_QUEUE_X - ICON_SIZE - 5, POINT_QUEUE_Y, (POINT_QUEUE_X - ICON_SIZE - 5) + 32, POINT_QUEUE_Y + 32);
	if( rect.PtInRect( point ) )
	{
		m_aSlotSkill.m_dwShortcut = ShortcutType::SkillFun;

		pShortcut   = &m_aSlotSkill;

		if( !pShortcut->IsEmpty() )
			return pShortcut;
	}

	return NULL;
}
void CWndTaskBar::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_bLButtonDown && IsPush() )
	{
	//PSHORTCUT pShortcut = Select( point );
	//	if( pShortcut && pShortcut == m_pSelectShortcut && !pShortcut->IsEmpty() )
		if( m_pSelectShortcut && !m_pSelectShortcut->IsEmpty() ) //&& m_pSelectShortcut->m_dwShortcut != SHORTCUT_SKILL )
		{
#ifdef __MAINSERVER
			if( m_pSelectShortcut->m_dwId == APP_WEBBOX )
				return;
#endif //__MAINSERVER
			if(m_pSelectShortcut->m_dwShortcut == ShortcutType::Lord)
			{
				m_pSelectShortcut->m_dwId++;
			}
			memcpy( &m_GlobalShortcut, m_pSelectShortcut, sizeof( m_GlobalShortcut ) );
			m_GlobalShortcut.m_pFromWnd = this;
			m_GlobalShortcut.m_dwData = (DWORD)m_pSelectShortcut;
			m_pSelectShortcut = NULL;
		}
	}
	CWndNeuz::OnMouseMove( nFlags, point );
}

void CWndTaskBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(g_WndMng.m_pWndMenu && g_WndMng.m_pWndMenu->IsVisible())
		g_WndMng.m_pWndMenu->SetVisible(FALSE);

	m_pSelectShortcut = Select( point );
	SetCapture();
}
void CWndTaskBar::OnLButtonUp(UINT nFlags, CPoint point)
{
	ReleaseCapture();
	LPSHORTCUT pShortcut = Select( point );
	
	if( pShortcut && pShortcut == m_pSelectShortcut && !pShortcut->IsEmpty() && pShortcut->m_dwData != 2 )
	{
		g_WndMng.ObjectExecutor( pShortcut );
	}

}

LPSKILL CWndTaskBar::GetCurrentSkillQueue()
{
	LPSKILL pSkill = NULL;
	LPSHORTCUT pShortcut = &m_aSlotQueue[ m_nUsedSkillQueue ];

	if( pShortcut && pShortcut->IsEmpty() == FALSE )
		pSkill = g_pPlayer->GetSkill( pShortcut->m_dwId );

	return pSkill;
}
//
// 스킬큐 실행 고!
//
BOOL CWndTaskBar::UseSkillQueue( CCtrl* pTargetObj )
{
	if( m_nExecute == 2 )	return FALSE;	// 이미 스킬바 실행중이라면 다시 들어오지 않음.
	if( g_pPlayer->m_pActMover->IsActAttack() )			return FALSE;		// 이미 공격동작이 이뤄지고 있으면 들어오지 마라.
	if( g_pPlayer->m_pActMover->IsActJump() )			return FALSE;		// 점프중엔 스킬사용 안됨.
//	if( g_pPlayer->m_pActMover->IsActDamage() )			return FALSE;		// 데미지 모션중엔 스킬사용 안됨.
	if( g_pPlayer->m_pActMover->IsFly() )				return FALSE;
//	if( g_pPlayer->m_dwFlag & MVRF_SKILL )		return FALSE;				// 이미 스킬 사용중이면 사용 안됨.
	if( pTargetObj && pTargetObj->GetType() != OT_MOVER )	return FALSE;

	// 오브젝트에 오른쪽 클릭 했을때
	if( pTargetObj )
	{
		if( pTargetObj->GetType() == OT_OBJ )		// 오브젝트가 OT_OBJ(배경)이면 선택 안한걸로 간주함.
			m_idTarget = NULL_ID;
		else
			m_idTarget = ((CCtrl*)pTargetObj)->GetId();		// OT_OBJ가 아니면 아이디를 가져옴
	}
	else
		m_idTarget = NULL_ID;		// 타겟을 선택하지 않았으면 NULL_ID
	CMover *pTargetMover = (CMover*)pTargetObj;		// prj.GetMover( m_idTarget );
	if( IsInvalidObj( pTargetMover ) )		// 거시기한 타겟이었으면 타겟 안한걸로 간주.
		m_idTarget = NULL_ID;

	
	m_nUsedSkillQueue = 0;
	
	LPSHORTCUT pShortcut = &m_aSlotQueue[ m_nUsedSkillQueue ];

	if( pShortcut->IsEmpty() == FALSE )
	{
		LPSKILL pSkill = g_pPlayer->GetSkill( pShortcut->m_dwId );
		if( pSkill == NULL )	return FALSE;
		
		if( g_pPlayer->IsBullet( pSkill->GetProp() ) == FALSE )
			return FALSE;

		m_nExecute = 1;		// 1 스킬바사용 실행대기중 
		// 여기엔 m_idTarget이 NULL_ID가 들어갈수도 있다.
		if( g_pPlayer->CMD_SetUseSkill( m_idTarget, pShortcut->m_dwId, SUT_QUEUESTART ) == 0 )		// 실행할 명령을 셋팅. 이동 + 스킬사용이 합쳐진 명령.
		{
			OnCancelSkill();	// 첫스킬부터 실패했다면 스킬큐 사용을 취소.
		} else
		{	// success
			// 1단계 쓸때는 ap가 소모되지 않는다.
			const ItemProp *pItemProp = g_pPlayer->GetActiveHandItemProp();
			if( pItemProp )
			{   // 손에 들고 있는게 스태프나 치어스틱이 아닐때만 NEXTSKILL_NONE로 변경. 안하면 스킬이 이어지지 않음.
				if( pItemProp->dwItemKind3 != IK3_STAFF && pItemProp->dwItemKind3 != IK3_CHEERSTICK )			
					g_WndMng.m_pWndWorld->SetNextSkill( NEXTSKILL_NONE );	// 2006/06/12 스킬큐를 실행했으니 스킬큐사용 명령 클리어.-xuzhu-
			}

		}
		return TRUE;
	} else
		g_WndMng.m_pWndWorld->SetNextSkill( NEXTSKILL_NONE );	// 액션스킬이 비어있으면 취소.

	return FALSE;
}

BOOL CWndTaskBar::Process( void )
{
	// 툴팁 계속 갱신해야하는것들은 여기서...
	if( m_nExecute )
	{
		if( m_idTarget != NULL_ID )		// m_idTarget이 NULL_ID인경우는 자기 자신에게 쓴경우기땜에 캔슬시켜선 안된다.
		{
			CMover* pObj = prj.GetMover( m_idTarget );
			if( IsInvalidObj(pObj) )						// 실행중에 대상이 없어졌다면 스킬사용을 멈춤.
				OnCancelSkill();
		}
//		if( pObj && pObj->IsDie() )						// 실행중 대상이 죽었으면 스킬바 실행 멈춤.
//			OnCancelSkill();

//		if( (g_pPlayer->m_dwFlag & MVRF_SKILL) == 0 )
//			OnCancelSkill();													// 스킬사용 해제
	}
	// 드래그 하이라이트 슬롯 알파 처리 
	if( m_GlobalShortcut.IsEmpty() == FALSE )
	{
		m_dwHighAlpha += 10;
		if( m_dwHighAlpha >= 256 )
			m_dwHighAlpha = 0;
	}
	// 드래그중에는 핫키를 사용할 수 없음 
	else
	{
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// 핫키 처리 
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		if( g_bKeyTable[ VK_RETURN ] ) 
		{
			g_bKeyTable[ VK_RETURN ] = FALSE;
			CWndBase* pWndBase = GetWndBase( APP_COMMUNICATION_CHAT );
			if( pWndBase == NULL )
			{
				g_WndMng.OpenApplet( APP_COMMUNICATION_CHAT );
				CWndChat* pWndChat = (CWndChat*)GetWndBase( APP_COMMUNICATION_CHAT );
				if( pWndChat != NULL )
				{
					pWndChat->m_wndEdit.SetFocus();
				}
			}
		}
		else
		{
			BOOL bKey = FALSE;

			for (const auto & [dwIdApplet, pApplet] : g_WndMng.m_mapAppletFunc)
			{
				
#ifdef __XKEYEDGE
				bKey = g_bKeyTable[ pApplet->m_cHotkey ];
				if( bKey && !g_bKeyTable2[ pApplet->m_cHotkey ] )
#else
				if( g_bKeyTable[ pApplet->m_cHotkey ] )			//gmpbigsun: 하우징 Y키( 가구창 ) 검사 
#endif
				{					
					g_WndMng.OpenApplet( pApplet->m_dwIdApplet );
#ifndef __XKEYEDGE					
					g_bKeyTable[ pApplet->m_cHotkey ] = FALSE;
#endif					
				}			
#ifdef __XKEYEDGE
				g_bKeyTable2[ pApplet->m_cHotkey ] = bKey;
				bKey = FALSE;
#endif

			}
			
			CWndRegVend* pWndRegVend = (CWndRegVend*)g_WndMng.GetWndBase(APP_VENDOREX_SELL);
			CWndRegVend* pWndVendorBuy = (CWndRegVend*)g_WndMng.GetWndBase(APP_VENDOREX_BUY);
			
			if( !pWndRegVend || !pWndVendorBuy )
			{
				DWORD *dwHotkey;
#ifdef __Y_INTERFACE_VER3				
				DWORD dwHotkey1[10] = { '1', '2', '3', '4', '5', '6', '7', '8', '9', '0' };
				DWORD dwHotkey2[10] = { 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79 };

				if( g_Option.m_nInterface == 2 )
					dwHotkey = dwHotkey1;
				else
					dwHotkey = dwHotkey2;								
#else //__Y_INTERFACE_VER3
				DWORD dwHotkey1[10] = { 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79 };
				dwHotkey = dwHotkey1;
#endif //__Y_INTERFACE_VER3

				for( int i = 0; i < 10; i++ )
				{
					if( g_bKeyTable[ dwHotkey[i] ] && g_bSlotSwitchAboutEquipItem[ i ] == FALSE )
					{
						LPSHORTCUT lpShortcut = &m_paSlotItem()[i];
						if( lpShortcut->m_dwShortcut == ShortcutType::Item )
						{
							if (const ItemProp * props = g_pPlayer->GetItemIdProp(lpShortcut->m_dwId)) {
								const int nPart = props->dwParts;
								if (nPart != -1) {
									g_bSlotSwitchAboutEquipItem[i] = TRUE;
								}
							}
						}
						g_WndMng.ObjectExecutor( lpShortcut );	
						g_bKeyTable[ dwHotkey[i] ] = 0;
					}
				}			
			}
		}
	}

	UpdateItem();
	
	return CWndBase::Process();
}

// 스킬바 사용을 취소(중단)함.
void CWndTaskBar::OnCancelSkill( void )
{
	if( g_pPlayer->m_dwReqFlag & REQ_USESKILL )
		return;

	m_nExecute = 0;
	m_nUsedSkillQueue = 0;
	g_pPlayer->ClearDestObj();
	g_WndMng.m_pWndWorld->SetNextSkill( NEXTSKILL_NONE );
}

// 스킬 동작 끝났을때 호출.
void CWndTaskBar::OnEndSkill( void )
{
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// 태스크 바의 매뉴 처리 윈도  
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

void CWndTaskMenu::OnDraw(C2DRender* p2DRender)
{
	return;
	p2DRender->TextOut( 10, 20,  _T( "MASQUERADE" ),  D3DCOLOR_ARGB( 255, 250, 250, 250 ) );

	CRect rect = GetWindowRect();
	// 테두리 박스 
	DWORD dwColor1t = D3DCOLOR_ARGB( 055,  91, 104, 205 );
	DWORD dwColor2t = D3DCOLOR_ARGB( 055, 116, 128, 220 );
	DWORD dwColor4t = D3DCOLOR_ARGB( 055, 143, 173, 245 );
	DWORD dwColor1b = D3DCOLOR_ARGB( 055,  41, 104, 155 );
	DWORD dwColor2b = D3DCOLOR_ARGB( 055,  66,  78, 170 );
	DWORD dwColor4b = D3DCOLOR_ARGB( 055, 143, 173, 245 );

	DWORD dwColor3t = D3DCOLOR_ARGB( 155, 250, 250, 250 );
	DWORD dwColor3b = D3DCOLOR_ARGB(  55,  00,  00,  00 );
	
	//p2DRender->RenderRoundRect( rect, dwColor1t, dwColor1t, dwColor1b, dwColor1b );
	rect.DeflateRect(1,1);
	//p2DRender->RenderRoundRect( rect, dwColor2t, dwColor2t, dwColor2b, dwColor2b );
//	rect.DeflateRect(1,1);

	CRect rect1 = rect; rect1.bottom = rect1.top + 40;
	CRect rect2 = rect; rect2.top    = rect2.top + 40;

	p2DRender->RenderFillRect( rect1, dwColor3t, dwColor3t, dwColor3b, dwColor3b );
	p2DRender->RenderFillRect( rect2, dwColor3b, dwColor3b, dwColor4b, dwColor4b );

//	p2DRender->RenderLine( CPoint( 10, 5 * 22 + 54 ), CPoint( 140, 5 * 22 + 54 ), dwColor1t );
	// client 영역 칠하기 
	//OnEraseBkgnd(p2DRender);
	//.OffsetRect(-rect.TopLeft());
	//p2DRender->PaintRect( rect,MKHIGHRGB(255>>3,255>>3,255>>3) );
	//p2DRender->TextOut( 5,20,"HP : 100 / 100", dwColor1t );
	//p2DRender->TextOut( 5,35,"MP : 100 / 100", dwColor1t );
}
BOOL CWndTaskMenu::Process()
{
	if(	!CWndBase::m_GlobalShortcut.IsEmpty() )
		return CWndBase::Process();

	if( IsVisible() == FALSE )
		return CWndBase::Process();

	for (auto & pWndButton : m_wndMenuItems) {
		
		if( pWndButton->GetClientRect( TRUE ).PtInRect( m_ptMouse ) )
		{
			// 모두 숨기기 
			if (pWndButton->m_pWndMenu == NULL || pWndButton->m_pWndMenu->IsVisible() == FALSE) {
				for (auto & subButton : m_wndMenuItems) {
					if (subButton->m_pWndMenu) {
						subButton->m_pWndMenu->SetVisibleSub(FALSE);
					}
				}
			}
			// 새 매뉴를 보이고 포커스 주기 
			if( pWndButton->m_pWndMenu )
			{
				if( pWndButton->m_pWndMenu->IsVisible() == FALSE )
				{
					CRect rect = pWndButton->GetScreenRect();
					pWndButton->m_pWndMenu->Move( CPoint( rect.right , rect.top ) );
				}
				// 메뉴의 좌표 지정 
				CRect rcButton = pWndButton->GetScreenRect();
				pWndButton->m_pWndMenu->Move( CPoint( rcButton.right, rcButton.top ) );
				// 그런데 그 메뉴가 화면을 벗어났다면 위치를 수정 
				CRect rcMenu = pWndButton->m_pWndMenu->GetScreenRect();
				CRect rcLayout = m_pWndRoot->GetLayoutRect();
				CPoint pt = rcMenu.TopLeft();
				if( rcMenu.right > rcLayout.right )
				{
					pt.x = rcButton.left - rcMenu.Width();
					//pWndButton->m_pWndMenu->Move( CPoint( rcButton.left - rcMenu.Width(), rcButton.top ) );
				}
				if( rcMenu.bottom > rcLayout.bottom )
				{
					pt.y -= rcMenu.bottom - rcLayout.bottom;
					//pWndButton->m_pWndMenu->Move( CPoint( rcButton.left - rcMenu.Width(), rcButton.top ) );
				}
				pWndButton->m_pWndMenu->Move( pt );
				pWndButton->m_pWndMenu->SetVisible( TRUE );
				pWndButton->m_pWndMenu->SetFocus();
			}
			break;
		}
	}
	return CWndBase::Process();
}

void CWndTaskMenu::OnInitialUpdate()
{
	CWndMenu::OnInitialUpdate();

	if (g_pPlayer->IsAuthHigher(AUTH_GAMEMASTER)) {
		// TODO: it should not be required to pass the TID to AddApplet
		AddApplet(APP_DEBUGINFO, TID_APP_DEBUGINFO);
	}

	AddApplet(APP_STATUS1   , TID_APP_STATUS);
	AddApplet(APP_NAVIGATOR , TID_APP_NAVIGATOR);
	AddApplet(APP_CHARACTER3, TID_APP_CHARACTER);
	AddApplet(APP_INVENTORY , TID_APP_INVENTORY);

#ifndef __TMP_POCKET
	AddApplet(APP_BAG_EX         , TID_APP_BAG_EX);
#endif
	AddApplet(APP_HOUSING        , TID_GAME_HOUSING_BOX);
	AddApplet(APP_VENDOR_REVISION, TID_APP_VENDOR);
	AddApplet(APP_SKILL3         , TID_APP_SKILL);
	AddApplet(APP_QUEST_EX_LIST  , TID_APP_QUEST);
	AddApplet(APP_MOTION         , TID_APP_MOTION);

#ifdef __IMPROVE_MAP_SYSTEM
	AddApplet(APP_MAP_EX         , TID_APP_MAP);
#else // __IMPROVE_MAP_SYSTEM
	AddApplet(APP_MAP            , TID_APP_MAP);
#endif // __IMPROVE_MAP_SYSTEM

	// Note: pWndButton->m_shortcut.m_dwShortcut = SHORTCUT_APPLET; SetTexture(pWndButton);
	// is it required for these applets?
	AddFolder(TID_APP_COMMITEM)
		.AddApplet(APP_WEBBOX, TID_APP_ITEMMALL)
		.AddApplet(APP_COMM_ITEM, TID_APP_ITEMTIME);

	AddFolder(TID_APP_COMMUNICATION)
		.AddApplet(APP_COMMUNICATION_CHAT, TID_APP_COMMUNICATION_CHAT)
		.AddApplet(APP_MESSENGER_, TID_APP_MESSENGER)
		.AddApplet(APP_COUPLE_MAIN, TID_GAME_COUPLE);
	

	AddFolder(TID_APP_COMMUNITY)
		.AddApplet(APP_PARTY, TID_APP_PARTY)
		.AddApplet(APP_GUILD, TID_APP_COMPANY)
#ifdef __GUILDVOTE
		.AddApplet(APP_GUILD_VOTE, TID_GAME_TOOLTIP_GUILDVOTE)
#endif
		;

	AddFolder(TID_APP_INFOMATION)
		.AddApplet(APP_INFO_NOTICE, TID_APP_INFO_NOTICE);

	const auto adderForHelp = AddFolder(TID_APP_HELPER)
		.AddApplet(APP_HELPER_HELP, TID_APP_HELPER_HELP)
		.AddApplet(APP_HELPER_TIP, TID_APP_HELPER_TIP)
		.AddApplet(APP_HELPER_FAQ, TID_APP_HELPER_FAQ)
		.AddApplet(APP_INFOPANG, TID_APP_INFOPANG);

#ifdef __NEW_WEB_BOX
#ifdef __INTERNALSERVER
	adderForHelp.AddApplet(APP_WEBBOX2, TID_GAME_HELPER_WEB_BOX_ICON_TITLE);
#else // __INTERNALSERVER
	if ((GetLanguage() == LANG_ENG && GetSubLanguage() == LANG_SUB_USA)) {
		adderForHelp.AddApplet(APP_WEBBOX2, TID_GAME_HELPER_WEB_BOX_ICON_TITLE);
	}
#endif // __INTERNALSERVER
#endif // __NEW_WEB_BOX

	AddApplet(APP_OPTIONEX, TID_APP_OPTION);
	AddApplet(APP_LOGOUT  , TID_APP_LOGOUT);
	AddApplet(APP_QUIT    , TID_APP_QUIT);

	CWndBase::SetTexture( m_pApp->m_pd3dDevice, MakePath( DIR_THEME, _T( "WndTaskMenu.tga" ) ), TRUE );

	CRect nextRectSurface(10, 50, m_pTexture->m_size.cx - 20, 50 + 20);

	for (auto & pWndButton : m_wndMenuItems) {
		pWndButton->SetWndRect(nextRectSurface);
		nextRectSurface.OffsetRect(CSize(0, 22));
	}

	SetWndRect(CRect(CPoint(0, 0), m_pTexture->m_size));
}

void CWndTaskMenu::AddApplet(DWORD appId, DWORD textId) {
	MakeButton(this, 0, appId, prj.GetText(textId));
}

CWndTaskMenu::FolderAdder CWndTaskMenu::AddFolder(DWORD textId) {
	CWndButton * menuButton = MakeButton(this, 0, 0, prj.GetText(textId));
	menuButton->SetTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, _T("icon_Folder.dds")));

	CWndMenu * menu = new CWndMenu;
	menu->CreateMenu(this);
	m_menus.emplace_back(menu);
	menuButton->SetMenu(menu);
	return FolderAdder(menu);
}

CWndButton* CWndTaskMenu::MakeButton( CWndMenu* pWndMenu, UINT nFlags, UINT nIDNewItem,	LPCTSTR lpszNewItem )
{
	CWndButton* pWndButton = (CWndButton*)pWndMenu->AppendMenu( nFlags, nIDNewItem, lpszNewItem );
	
	const AppletFunc* pAppletFunc = g_WndMng.GetAppletFunc( nIDNewItem ); 
	if (!pAppletFunc) return pWndButton;

	pWndButton->m_cHotkey = pAppletFunc->m_cHotkey;
	CString string = pAppletFunc->m_pAppletDesc;
	if (pAppletFunc->m_cHotkey != 0) {
		string.AppendFormat("\n[%s %c]", pAppletFunc->m_pAppletDesc, prj.GetText(TID_GAME_TOOLTIP_HOTKEY), pAppletFunc->m_cHotkey);
	}
	pWndButton->m_strToolTip = string;
	pWndButton->m_shortcut.m_dwShortcut = ShortcutType::Applet; 
	pWndButton->SetTexture(g_Neuz.m_pd3dDevice, MakePath(DIR_ICON, g_WndMng.GetAppletFunc(pWndButton->GetWndId())->m_pszIconName));
	
	return pWndButton;
}

BOOL CWndTaskMenu::Initialize(CWndBase* pWndParent,DWORD dwWndId)
{
	CreateMenu( &g_WndMng );
	SetVisible(TRUE);
	return TRUE;
}

void CWndTaskMenu::OnKillFocus(CWndBase* pNewWnd)
{
	if(g_WndMng.m_pWndTaskBar != pNewWnd)
		CWndMenu::OnKillFocus(pNewWnd);
}

BOOL CWndTaskMenu::OnCommand( UINT nID, DWORD dwMessage, CWndBase* pWndBase )
{
	return TRUE;
}
BOOL CWndTaskMenu::OnChildNotify(UINT message,UINT nID,LRESULT* pLResult)
{
	if( message == WNM_CLICKED )
		g_WndMng.OpenApplet( nID );
	return TRUE;
}

void CWndTaskMenu::OnLButtonUp(UINT nFlags, CPoint point)
{
}
void CWndTaskMenu::OnLButtonDown(UINT nFlags, CPoint point)
{
}

void CWndTaskMenu::PaintFrame( C2DRender* p2DRender )
{
	CWndBase::PaintFrame( p2DRender );
}
BOOL CWndTaskMenu::OnEraseBkgnd( C2DRender* p2DRender )
{
	return CWndBase::OnEraseBkgnd( p2DRender );
}


void CWndTaskBar::RenderCollTime(CPoint pt, DWORD dwSkillId, C2DRender* p2DRender )
{
	DWORD dwDelay = g_pPlayer->GetReuseDelay( dwSkillId );
	if( dwDelay > 0 )
	{
		SKILL * skill = g_pPlayer->GetSkill(dwSkillId);
		if (!skill) return;

		const ItemProp* pSkillProp	= skill->GetProp();
		ASSERT( pSkillProp );
		AddSkillProp* pAddSkillProp	= prj.GetAddSkillProp( pSkillProp->dwSubDefine, skill->dwLevel );
		ASSERT( pAddSkillProp );
		RenderRadar( p2DRender, pt, 
			            pAddSkillProp->dwCooldown - dwDelay, 
						pAddSkillProp->dwCooldown );	
	}
}

	void CWndTaskBar::RenderLordCollTime( CPoint pt, DWORD dwSkillId, C2DRender* p2DRender )
	{
		CCLord* pLord									= CCLord::Instance();
		CLordSkillComponentExecutable* pComponent		= pLord->GetSkills()->GetSkill(dwSkillId);
		if(!pComponent) return;
		DWORD dwDelay = 0;
		if(pComponent->GetTick() > 0)
			RenderRadar( p2DRender, pt, pComponent->GetCooltime() - pComponent->GetTick(), pComponent->GetCooltime() );	
	}

void CWndTaskBar::RenderOutLineLamp(int x, int y, int num, DWORD size)
{
	CPoint Point = CPoint( x, y );
	LONG   thick = 10;

	D3DXCOLOR dwColorDest2  = D3DCOLOR_ARGB( 0, 40, 0, 255 );

	D3DXCOLOR dwColor  = D3DCOLOR_ARGB( 196, 40, 0, 255 );

	D3DXCOLOR dwColor1 = D3DCOLOR_ARGB( 0, 255, 255, 255 );
	D3DXCOLOR dwColor2 = D3DCOLOR_ARGB( 0, 255, 255, 255 );

	static BOOL  bReverse = FALSE;
	static FLOAT fLerp = 0.0f;

	if( bReverse )
		fLerp-=0.05f;
	else
		fLerp+=0.05f;

	if( fLerp > 1.0f )
	{
		bReverse = TRUE;
		fLerp = 1.0f;
	}
	else
	if( fLerp < 0.0f )
	{
		bReverse = FALSE;
		fLerp = 0.0f;
	}
	
	D3DXColorLerp( &dwColor2, &dwColor, &dwColorDest2, fLerp);
	
	CRect Rect = CRect( 0, 0, (size * num), size );
	
	m_p2DRender->RenderFillRect( CRect( (Point.x+Rect.left)-thick, Point.y+Rect.top, (Point.x+Rect.left), Point.y+Rect.bottom ),
		dwColor1, dwColor2, dwColor1, dwColor2 );
	
	m_p2DRender->RenderFillRect( CRect( (Point.x+Rect.left), (Point.y+Rect.top)-thick, (Point.x+Rect.right), Point.y+Rect.top ),
		dwColor1, dwColor1, dwColor2, dwColor2 );
	
	m_p2DRender->RenderFillRect( CRect( (Point.x+Rect.right), Point.y+Rect.top, (Point.x+Rect.right)+thick, Point.y+Rect.bottom ),
		dwColor2, dwColor1, dwColor2, dwColor1 );
	
	m_p2DRender->RenderFillRect( CRect( (Point.x+Rect.left), (Point.y+Rect.bottom), (Point.x+Rect.right), (Point.y+Rect.bottom)+thick ),
		dwColor2, dwColor2, dwColor1, dwColor1 );
}
