#pragma once

#include <vector>
#include <map>

#include "WndWorld.h"

class CWndBuffStatus : public CWndNeuz {
public:
	size_t m_lastSeenSize = 0;

	bool m_verticalDisplay;
public:
	CWndBuffStatus();

	BOOL Initialize(CWndBase * pWndParent = NULL, DWORD nType = MB_OK) override;
	void OnDraw(C2DRender * p2DRender) override;
	void OnInitialUpdate() override;
	void OnLButtonUp(UINT nFlags, CPoint point) override;
	void OnLButtonDown(UINT nFlags, CPoint point) override;
	void OnRButtonDown(UINT nFlags, CPoint point) override;
	void OnLButtonDblClk(UINT nFlags, CPoint point) override;
	void OnRButtonDblClk(UINT nFlags, CPoint point) override;
	void PaintFrame(C2DRender * p2DRender) override;

	void RenderBuffIcon(C2DRender * p2DRender, IBuff * pBuff, CPoint buffPosition, CPoint ptMouse);
	void RenderOptBuffTime(C2DRender * p2DRender, const CPoint & point, DWORD timeLeft, DWORD dwColor);
	void SetBuffIconInfo(bool force);
	[[nodiscard]] bool GetHitTestResult() /* const */;

	class CPointGenerator {
	private:
		int m_verticalAlign;
		CPoint m_point;

	public:
		explicit CPointGenerator(bool verticalAlign) : m_verticalAlign(verticalAlign) {}
		CPoint Next();
	};
};

