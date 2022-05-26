#include "StdAfx.h"
#include "groupquest.h"


namespace GroupQuest {

	void QuestElem::SetGetItemState() {
		nProcess = GroupQuest::ProcessState::GetItem;
		dwEndTime = GetTickCount() + MIN(20);
		ns = nf = 0;
		nState = 0;
		objidWormon = NULL_ID;
	}

	bool CQuestProcessor::IsOutOfRange(int nQuestId, const char * caller) const {
		if (nQuestId < 0 || nQuestId >= m_pElem.size()) {
			Error(__FUNCTION__"(%d, %s): the quest can not exist", nQuestId, caller);
			return false;
		}

		return true;
	}

	void CQuestProcessor::_SetQuest(int nQuestId, int nState, int ns, int nf, u_long idGuild, OBJID objidWormon) {
		if (IsOutOfRange(nQuestId, __FUNCTION__)) return;

		const GUILDQUESTPROP * pProp = prj.GetGuildQuestProp(nQuestId);
		if (!pProp) {
			Error(__FUNCTION__"(): the quest %d has no prop", nQuestId);
			return;
		}

		TRACE("SET_QUEST, %d, %d, %d\n", nQuestId, idGuild, objidWormon);

		GroupQuest::QuestElem * pElem = &m_pElem[nQuestId];
		pElem->nId = nQuestId;
		pElem->nState = nState;
		pElem->idGroup = idGuild;

		pElem->dwEndTime = GetTickCount() + MIN(60);

		pElem->nProcess = GroupQuest::ProcessState::Wormon;
		pElem->ns = ns;
		pElem->nf = nf;
		pElem->objidWormon = objidWormon;
		pElem->nCount = 0;
	}

	GroupQuest::QuestElem * CQuestProcessor::GetQuest(int nQuestId) {
		if (IsOutOfRange(nQuestId, __FUNCTION__)) return nullptr;
		if (m_pElem[nQuestId].nId == -1) return nullptr;

		return &m_pElem[nQuestId];
	}

	void CQuestProcessor::RemoveQuest(int nQuestId) {
		if (IsOutOfRange(nQuestId, __FUNCTION__)) return;

		TRACE("REMOVE_GUILD_QUEST, %d\n", nQuestId);
		m_pElem[nQuestId] = GroupQuest::QuestElem();
	}

	bool CQuestProcessor::IsQuesting(int nQuestId) const noexcept {
		if (IsOutOfRange(nQuestId, __FUNCTION__)) return false;
		return m_pElem[nQuestId].nId != -1;
	}

	int CQuestProcessor::PtInQuestRect(DWORD dwWorldId, const D3DXVECTOR3 & vPos) const {
		const POINT point = { (int)vPos.x, (int)vPos.z };

		const auto it = std::ranges::find_if(m_rects, [&](const QuestRect & rect) {
			return rect.dwWorldId == dwWorldId && rect.rect.PtInRect(point);
			});

		if (it == m_rects.end()) return -1;
		return static_cast<int>(it - m_rects.begin());
	}


	const CRect * CQuestProcessor::GetQuestRect(const int nId) const {
		const auto it = std::ranges::find_if(m_rects, [&](const QuestRect & rect) {
			return rect.nId == nId;
			});

		return (it != m_rects.end()) ? &it->rect : nullptr;
	}

	const QuestRect * CQuestProcessor::GetQuestQuestRect(const int nId) const {
		const auto it = std::ranges::find_if(m_rects, [&](const QuestRect & rect) {
			return rect.nId == nId;
			});

		return (it != m_rects.end()) ? &*it : nullptr;
	}

}