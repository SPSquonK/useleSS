#include "StdAfx.h"
#include "WndManager.h"
#include "defineText.h"

namespace WndMgr {





	void CTooltipBuilder::PutEquipItemText(CEditString & pEdit) const {
		CString strEdit;
		strEdit.Format("[ %s ]\n", prj.GetText(TID_GAME_EQUIPED_ITEM));
		pEdit.AddString(strEdit, 0xff000000, ESSTY_BOLD);
	}
}
