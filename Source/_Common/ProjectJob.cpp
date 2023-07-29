#include "StdAfx.h"
#include "ProjectJob.h"



void Project::Jobs::LoadSkills(CFixedArray<ItemProp> & aPropSkill) {
	skills.fill({});

	for (const ItemProp & pItemProp : aPropSkill) {
		if (pItemProp.dwItemKind1 == JTYPE_COMMON) continue;
		if (pItemProp.dwItemKind2 == NULL_ID) continue;

		skills[pItemProp.dwItemKind2].emplace_back(&pItemProp);
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
		pProperty.AttackSpeed = scanner.GetFloat();
		pProperty.FactorMaxHP = scanner.GetFloat();
		pProperty.FactorMaxMP = scanner.GetFloat();
		pProperty.FactorMaxFP = scanner.GetFloat();
		pProperty.FactorDef = scanner.GetFloat();
		pProperty.FactorHPRecovery = scanner.GetFloat();
		pProperty.FactorMPRecovery = scanner.GetFloat();
		pProperty.FactorFPRecovery = scanner.GetFloat();
		pProperty.MeleeSWD = scanner.GetFloat();
		pProperty.MeleeAXE = scanner.GetFloat();
		pProperty.MeleeSTAFF = scanner.GetFloat();
		pProperty.MeleeSTICK = scanner.GetFloat();
		pProperty.MeleeKNUCKLE = scanner.GetFloat();
		pProperty.MagicWAND = scanner.GetFloat();
		pProperty.Blocking = scanner.GetFloat();
		pProperty.MeleeYOYO = scanner.GetFloat();
		pProperty.Critical = scanner.GetFloat();
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

#ifdef __CLIENT

Project::Jobs::PlayerDataIcon::PlayerDataIcon(DWORD jobId, int level) {
	if (prj.jobs.info[jobId].dwJobType == JTYPE_MASTER) {
		if (level < 70) master = 27;
		else if (level < 80) master = 28;
		else if (level < 90) master = 29;
		else if (level < 100) master = 30;
		else if (level < 110) master = 31;
		else master = 32;
	} else if (prj.jobs.info[jobId].dwJobType >= JTYPE_HERO) {
		master = 33;
	} else {
		master = 0;
	}

	switch (jobId) {
		case JOB_VAGRANT:
			job = 14;
			break;
		case JOB_MERCENARY:
			job = 15;
			break;
		case JOB_ACROBAT:
			job = 16;
			break;
		case JOB_ASSIST:
			job = 17;
			break;
		case JOB_MAGICIAN:
			job = 18;
			break;
		case JOB_KNIGHT:
		case JOB_KNIGHT_MASTER:
		case JOB_KNIGHT_HERO:
		case JOB_LORDTEMPLER_HERO:
			job = 19;
			break;
		case JOB_BLADE:
		case JOB_BLADE_MASTER:
		case JOB_BLADE_HERO:
		case JOB_STORMBLADE_HERO:
			job = 20;
			break;
		case JOB_JESTER:
		case JOB_JESTER_MASTER:
		case JOB_JESTER_HERO:
		case JOB_WINDLURKER_HERO:
			job = 21;
			break;
		case JOB_RANGER:
		case JOB_RANGER_MASTER:
		case JOB_RANGER_HERO:
		case JOB_CRACKSHOOTER_HERO:
			job = 22;
			break;
		case JOB_RINGMASTER:
		case JOB_RINGMASTER_MASTER:
		case JOB_RINGMASTER_HERO:
		case JOB_FLORIST_HERO:
			job = 23;
			break;
		case JOB_BILLPOSTER:
		case JOB_BILLPOSTER_MASTER:
		case JOB_BILLPOSTER_HERO:
		case JOB_FORCEMASTER_HERO:
			job = 24;
			break;
		case JOB_PSYCHIKEEPER:
		case JOB_PSYCHIKEEPER_MASTER:
		case JOB_PSYCHIKEEPER_HERO:
		case JOB_MENTALIST_HERO:
			job = 25;
			break;
		case JOB_ELEMENTOR:
		case JOB_ELEMENTOR_MASTER:
		case JOB_ELEMENTOR_HERO:
		case JOB_ELEMENTORLORD_HERO:
			job = 26;
			break;
		default:
			job = 13;
			break;
	}
}

#endif
