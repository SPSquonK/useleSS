#include "StdAfx.h"
#include "WndManager.h"
#include "defineText.h"


namespace WndMgr {




	void CTooltipBuilder::PutSealChar(const CItemElem & pItemElem, CEditString & pEdit) const {
		
		CString str;
		str.Format("\n%s\n%s", prj.GetText(TID_TOOLTIP_CHARNAME), pItemElem.m_szItemText);
		CString strTemp = str;

		const int	nJob = pItemElem.m_nRepair;
		const int nLevel = static_cast<int>(pItemElem.m_nRepairNumber);

		CString strTemp2;
		if (MAX_PROFESSIONAL <= nJob && nJob < MAX_MASTER)
			strTemp2.Format("%d%s", nLevel, prj.GetText(TID_GAME_TOOLTIP_MARK_MASTER));
		else if (MAX_MASTER <= nJob)
			strTemp2.Format("%d%s", nLevel, prj.GetText(TID_GAME_TOOLTIP_MARK_HERO));
		else
			strTemp2.Format("%d", nLevel);

		str.Format("\n%s%s", prj.GetText(TID_TOOLTIP_CHARLEVEL), strTemp2);
		strTemp += str;

		CString strTemp3;
		strTemp3.Format("%s", prj.m_aJob[nJob].szName);

		str.Format("\n%s%s", prj.GetText(TID_TOOLTIP_CHARJOB), strTemp3);
		strTemp += str;
		str.Format("\n%s", prj.GetText(TID_TOOLTIP_CHARSTAT));
		strTemp += str;


		str.Format("\n%s%d", prj.GetText(TID_TOOLTIP_CHARSTA), (int)pItemElem.GetPiercingItem(0));
		strTemp += str;
		str.Format("\n%s%d", prj.GetText(TID_TOOLTIP_CHARSTR), (int)pItemElem.GetPiercingItem(1));
		strTemp += str;
		str.Format("\n%s%d", prj.GetText(TID_TOOLTIP_CHARDEX), (int)pItemElem.GetPiercingItem(2));
		strTemp += str;
		str.Format("\n%s%d", prj.GetText(TID_TOOLTIP_CHARINT), (int)pItemElem.GetPiercingItem(3));
		strTemp += str;
		str.Format("\n%s%d", prj.GetText(TID_TOOLTIP_CHARPOINT), (int)pItemElem.m_nResistAbilityOption);
		strTemp += str;

		pEdit.AddString(strTemp, D3DCOLOR_XRGB(255, 20, 20));
	}


	void CTooltipBuilder::PutEquipItemText(CEditString & pEdit) const {
		CString strEdit;
		strEdit.Format("[ %s ]\n", prj.GetText(TID_GAME_EQUIPED_ITEM));
		pEdit.AddString(strEdit, 0xff000000, ESSTY_BOLD);
	}
}
