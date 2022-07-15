#include "StdAfx.h"
#include "ProjectJob.h"



void Project::Jobs::LoadSkills(CFixedArray<ItemProp> & aPropSkill) {
	skills.fill({});

	for (int i = 1; i < aPropSkill.GetSize(); i++) {
		ItemProp * pItemProp = aPropSkill.GetAt(i);
		if (!pItemProp) continue;

		if (pItemProp->dwItemKind1 == JTYPE_COMMON) continue;
		if (pItemProp->dwItemKind2 == NULL_ID) continue;

		skills[pItemProp->dwItemKind2].emplace_back(pItemProp);
	}

	for (auto & skillsPerJob : skills) {
		std::ranges::sort(skillsPerJob,
			[](const ItemProp * lhs, const ItemProp * rhs) {
				return lhs->dwReqDisLV < rhs->dwReqDisLV;
			});
	}
}


bool Project::Jobs::LoadJobItem(LPCTSTR lpszFileName) {
	CScript script;
	if (!script.Load(lpszFileName)) return false;
	script.GetToken();
	
	items.fill({});
		
	while (script.tok != FINISHED) {
		if (script.Token == _T("job")) {
			int nJob = script.GetNumber();
			script.GetToken(); // {
			script.GetToken(); // sex

			while (script.Token != _T("}")) {
				if (script.Token == _T("sex")) {
					const int nSex = script.GetNumber();
					script.GetToken(); // {
					
					DWORD nItem = script.GetNumber();
					while (script.Token != _T("}")) {
						items[nJob].adwMale[nSex].emplace_back(nItem);
						nItem = script.GetNumber();
					}
				}
				if (script.Token == _T("item")) {
					// beginitem -> never used both in v15 and v21
					script.GetNumber(); // dwItemType
					script.GetNumber(); // dwItemId
					script.GetNumber(); // dwNum
				}
				script.GetToken();
			}
		}
		script.GetToken();
	}

	return true;
}

