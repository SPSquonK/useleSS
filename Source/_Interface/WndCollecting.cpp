#include "stdafx.h"
#include "resData.h"
#include "WndCollecting.h"
#include "Collecting.h"
#include "DPClient.h"
#include "DefineObj.h"
#include "defineText.h"


/****************************************************
  WndId : APP_COLLECTING - Applet
  CtrlId : WIDC_BUTTON1 - Start
  CtrlId : WIDC_STATIC1 - Bettery
  CtrlId : WIDC_STATIC2 - Level
  CtrlId : WIDC_CUSTOM1 - Custom
  CtrlId : WIDC_STATIC3 - 0
  CtrlId : WIDC_STATIC4 - /  5
  CtrlId : WIDC_STATIC5 - 0
  CtrlId : WIDC_STATIC6 - / 1600
****************************************************/

CCollectingWnd::CCollectingWnd() 
{ 

	m_bIsCollecting = false;
	m_pVBGauge	= NULL;
	m_pTexGauEmptyNormal = NULL;
	m_pTexGauFillNormal = NULL;
	m_pTexBatt = m_pTexLevel = NULL;

} 



void CCollectingWnd::OnDraw( C2DRender* p2DRender ) 
{ 
    if(!g_pPlayer) return;
	LPWNDCTRL wndCtrlPic1 = GetWndCtrl( WIDC_PIC1 );
	LPWNDCTRL wndCtrlPic2 = GetWndCtrl( WIDC_PIC2 );

	if (m_pTexBatt) {
		m_pTexBatt->Render(p2DRender, wndCtrlPic2->rect.TopLeft());
	}

	if (m_pTexLevel) {
		m_pTexLevel->Render(p2DRender, wndCtrlPic1->rect.TopLeft());
	}

	CItemElem* pCollector = g_pPlayer->GetCollector();

	if(pCollector)
	{
		LPWNDCTRL	lpGauge   = GetWndCtrl( WIDC_GAUGE );
		int 		nWidthClient = lpGauge->rect.Width();
		int			nMax = CCollectingProperty::GetInstance()->GetMaxBattery();
		float		fRatio = (float)pCollector->m_nHitPoint / (float)nMax; 
		int			nBtryRemain = pCollector->m_nHitPoint;

		if(nBtryRemain < 0)  nBtryRemain = 0;

		int nWidth = (int)( nWidthClient * fRatio );
		wsprintf(m_pbufText, "%d / %d (%d%%)", nBtryRemain, nMax, (int)(fRatio * 100));

		if(nWidth >= 2)
		{
			CRect rectTemp = lpGauge->rect;
			rectTemp.right = rectTemp.left + nWidth;
			ClientToScreen( rectTemp );

			m_Theme.MakeGaugeVertex( &rectTemp, D3DCOLOR_ARGB( 128, 0, 0, 255 ), m_pVBGauge, m_pTexGauFillNormal );
			m_Theme.RenderGauge( m_pVBGauge, m_pTexGauEmptyNormal );	
		}
	}
	
	

	LPWNDCTRL lpGauge   = GetWndCtrl( WIDC_GAUGE );
	DWORD dwColor = D3DCOLOR_ARGB(255, 0, 0, 255 );
	
	p2DRender->TextOut( lpGauge->rect.left + 5, lpGauge->rect.top, m_pbufText, dwColor, 0x00000000 );
}
 
void CCollectingWnd::OnDestroy()
{
}

HRESULT CCollectingWnd::RestoreDeviceObjects()
{
	CWndBase::RestoreDeviceObjects();
	if(!m_pVBGauge)
	m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBGauge, NULL );
	
	return S_OK;
}

HRESULT CCollectingWnd::InvalidateDeviceObjects()
{
	CWndBase::InvalidateDeviceObjects();
	SAFE_RELEASE( m_pVBGauge );
	return S_OK;
}

HRESULT CCollectingWnd::DeleteDeviceObjects()
{
	CWndBase::DeleteDeviceObjects();
	return InvalidateDeviceObjects();
}

void CCollectingWnd::OnInitialUpdate() 
{ 
	CWndNeuz::OnInitialUpdate(); 
	// 여기에 코딩하세요
	if(!g_pPlayer) return;
	CItemElem* pCollector = g_pPlayer->GetCollector();

	if(pCollector)
	{

		LPWNDCTRL lpWndCtrl1 	= GetWndCtrl( WIDC_STATIC1 );
		LPWNDCTRL lpWndCtrl2 	= GetWndCtrl( WIDC_STATIC2 );
		LPWNDCTRL	lpGauge		= GetWndCtrl( WIDC_GAUGE );
		int 		nWidthClient= lpGauge->rect.Width();
		int			nMax		= CCollectingProperty::GetInstance()->GetMaxBattery();
		int			nRatio		= pCollector->m_nHitPoint / nMax; 
		int			nWidth		= nWidthClient * nRatio;
		int			nBtryRemain = pCollector->m_nHitPoint;

		if(nBtryRemain < 0)  nBtryRemain = 0;
		m_BetteryRect	= lpWndCtrl1->rect;
		m_LevelRect		= lpWndCtrl2->rect;
		
		if(!m_pVBGauge)
			m_pd3dDevice->CreateVertexBuffer( sizeof( TEXTUREVERTEX2 ) * 3 * 6, D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_TEXTUREVERTEX2, D3DPOOL_DEFAULT, &m_pVBGauge, NULL );

		m_pTexGauEmptyNormal = CWndBase::m_textureMng.AddTexture( MakePath( DIR_THEME, "GauEmptyNormal.bmp" ), 0xffff00ff );
		m_pTexGauFillNormal  = CWndBase::m_textureMng.AddTexture( MakePath( DIR_THEME, "GauEmptyNormal.bmp" ), 0xffff00ff );

		wsprintf(m_pbufText, "%d / %d (%d%%)", nBtryRemain, nMax, nRatio * 100);
		
		CRect rectTemp = lpGauge->rect;
		rectTemp.right = rectTemp.left + nWidth;
		ClientToScreen( rectTemp );

		char		szTemp[64];
		memset(szTemp, 0, 64);
		sprintf(szTemp, "%d  /  5", pCollector->GetAbilityOption());

		CWndStatic * pLevel = (CWndStatic *)GetDlgItem(WIDC_STATIC3);
		pLevel->SetTitle(szTemp);

		m_pTexBatt  = CWndBase::m_textureMng.AddTexture( MakePath( DIR_ICON, _T( "Icon_ColBattery.tga" )), 0xffff00ff );
		m_pTexLevel = CWndBase::m_textureMng.AddTexture( MakePath( DIR_ICON, _T( "Icon_ColLevel.tga" )), 0xffff00ff );
		
	}

	// 윈도를 중앙으로 옮기는 부분.
	CRect rectRoot = m_pWndRoot->GetLayoutRect();
	CRect rectWindow = GetWindowRect();
	CPoint point( rectRoot.right - rectWindow.Width(), 110 );
	Move( point );
	
} 
// 처음 이 함수를 부르면 윈도가 열린다.
BOOL CCollectingWnd::Initialize( CWndBase* pWndParent, DWORD /*dwWndId*/ ) 
{ 
		// Daisy에서 설정한 리소스로 윈도를 연다.
		return CWndNeuz::InitDialog( APP_COLLECTING, pWndParent, 0, CPoint( 0, 0 ) );
} 



void  CCollectingWnd::Update()
{
	if(!g_pPlayer) return;
	
	CItemElem* pCollector = g_pPlayer->GetCollector();
	if (!pCollector) return;

	char szTemp[64];
	sprintf(szTemp, "%d  /  5", pCollector->GetAbilityOption());

	CWndBase * pLevel = GetDlgItem(WIDC_STATIC3);
	pLevel->SetTitle(szTemp);
}

BOOL CCollectingWnd::Process()
{
	return TRUE;
}


void CCollectingWnd::SetButtonCaption(bool bIsStart) {
	CWndBase * pWndButton = GetDlgItem(WIDC_BUTTON1);

	if (bIsStart) {
		pWndButton->SetTexture(MakePath(DIR_THEME, _T("ButtStop.bmp")), TRUE);
	} else if (::GetLanguage() == LANG_FRE) {
		pWndButton->SetTexture(MakePath(DIR_THEME, _T("ButOk2.bmp")), TRUE);
	} else {
		pWndButton->SetTexture(MakePath(DIR_THEME, _T("ButtStart.bmp")), TRUE);
	}
}

BOOL CCollectingWnd::OnChildNotify( UINT message, UINT nID, LRESULT* pLResult ) 
{ 
	if(!g_pPlayer) return FALSE;
	switch(nID)
	{
		case WIDC_BUTTON1 :
			m_bIsCollecting = !m_bIsCollecting;
			if(m_bIsCollecting)
			{
				if(!g_pPlayer->m_pActMover->IsFly())	
					g_DPlay.SendQueryStartCollecting();
				else
					g_WndMng.PutString( prj.GetText(TID_GAME_COLLECT_FLY), NULL, 0xffff0000 );
			}
			else				
				g_DPlay.SendQueryStopCollecting();
			break;
	};
	return CWndNeuz::OnChildNotify( message, nID, pLResult ); 
} 


void CCollectingWnd::OnMouseWndSurface(CPoint point) {
	
	if (CRect DrawRect = m_BetteryRect; DrawRect.PtInRect(point)) {
		CPoint point2 = point;
		ClientToScreen(&point2);
		ClientToScreen(&DrawRect);

		CString strText = prj.GetText(TID_GAME_COLLECTOR_BETTERY_INFO);
		g_toolTip.PutToolTip(100000, strText, DrawRect, point2);
	}

	if (CRect DrawRect = m_LevelRect; DrawRect.PtInRect(point)) {
		CPoint point2 = point;
		ClientToScreen(&point2);
		ClientToScreen(&DrawRect);

		CString strText = prj.GetText(TID_GAME_COLLECTOR_LEVEL_INFO);
		g_toolTip.PutToolTip(100000, strText, DrawRect, point2);
	}
}
