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


#if defined(__WORLDSERVER) || defined(__CLIENT)

// propJob.inc
bool Project::Jobs::LoadPropJob(LPCTSTR lpszFileName) {
	CScript scanner;
	if (!scanner.Load(lpszFileName, FALSE)) {
		return false;
	}

	while (true) {
		int nJob = scanner.GetNumber();
		if (scanner.tok == FINISHED) break;

		JobProp & pProperty = prop[nJob];
		pProperty.fAttackSpeed = scanner.GetFloat();
		pProperty.fFactorMaxHP = scanner.GetFloat();
		pProperty.fFactorMaxMP = scanner.GetFloat();
		pProperty.fFactorMaxFP = scanner.GetFloat();
		pProperty.fFactorDef = scanner.GetFloat();
		pProperty.fFactorHPRecovery = scanner.GetFloat();
		pProperty.fFactorMPRecovery = scanner.GetFloat();
		pProperty.fFactorFPRecovery = scanner.GetFloat();
		pProperty.fMeleeSWD = scanner.GetFloat();
		pProperty.fMeleeAXE = scanner.GetFloat();
		pProperty.fMeleeSTAFF = scanner.GetFloat();
		pProperty.fMeleeSTICK = scanner.GetFloat();
		pProperty.fMeleeKNUCKLE = scanner.GetFloat();
		pProperty.fMagicWAND = scanner.GetFloat();
		pProperty.fBlocking = scanner.GetFloat();
		pProperty.fMeleeYOYO = scanner.GetFloat();
		pProperty.fCritical = scanner.GetFloat();
	}

	return true;
}

void Project::Jobs::LoadJobInfo(CScript & script) {
	// Read data
	int id = script.GetNumber();
	while (*script.token != '}') {
		script.GetToken();
		_tcscpy(info[id].szName, script.token);
		info[id].dwJobBase = script.GetNumber();
		info[id].dwJobType = script.GetNumber();
		id = script.GetNumber();
	}

	if (!InfoIsConsistent()) {
		Error("etc.inc: Job Info are bad");
	}
}

bool Project::Jobs::InfoIsConsistent() const {
	for (int i = 0; i != MAX_JOB; ++i) {
		const auto line = GetAllJobsOfLine(i);
		const DWORD type = info[i].dwJobType;

		const size_t expectedSize =
			type == JTYPE_BASE ? 1 :
			type == JTYPE_EXPERT ? 2 :
			type == JTYPE_PRO ? 3 :
			type == JTYPE_MASTER ? 4 :
			type == JTYPE_HERO ? 5 :
			type == JTYPE_LEGEND_HERO ? 6 : 0;

		if (line.size() != expectedSize) return false;
	}

	return true;
}

boost::container::small_vector<DWORD, 6> Project::Jobs::GetAllJobsOfLine(DWORD jobId) const {
	boost::container::small_vector<DWORD, 6> jobs;

	while (jobId != JOB_VAGRANT) {
		const auto it = std::ranges::find(jobs, jobId);
		if (it != jobs.end()) break;

		jobs.insert(jobs.begin(), jobId);

		jobId = info[jobId].dwJobBase;
	}

	jobs.insert(jobs.begin(), JOB_VAGRANT);

	return jobs;
};

Project::ProJob Project::Jobs::GetProJob(DWORD jobId) const {
	if (jobId == NULL_ID || jobId == JOB_VAGRANT) return ProJob::Vagrant;

	while (true) {
		switch (jobId) {
			case JOB_VAGRANT:   return ProJob::Vagrant;
			case JOB_ACROBAT:   return ProJob::Acrobat;
			case JOB_ASSIST:    return ProJob::Assist;
			case JOB_MERCENARY: return ProJob::Mercenary;
			case JOB_MAGICIAN:  return ProJob::Magician;
		}

		jobId = info[jobId].dwJobBase;
	}
}

const JobProp * Project::Jobs::GetJobProp(int nIndex) const {
	if (nIndex < 0 || nIndex >= MAX_JOB) {
		LPCTSTR szErr = Error("CProject::GetJobProp with job id=%d", nIndex);
		return NULL;
	}
	return &prop[nIndex];
}

#endif
