#pragma once

#include <boost/container/static_vector.hpp>

namespace MoverSub {

	struct Quests {
		// TODO: what are quest ids ? WORDs, BYTEs or DWORDs?
		boost::container::static_vector<QUEST, MAX_QUEST> current;
		boost::container::static_vector<BYTE, MAX_COMPLETE_QUEST> completed;
		boost::container::static_vector<BYTE, MAX_CHECKED_QUEST> checked;

		struct QuestSearcherById {
			int questId;
			constexpr QuestSearcherById(int questId) : questId(questId) {}

			[[nodiscard]] constexpr bool operator()(const QUEST & quest) const noexcept {
				return quest.m_wId == questId;
			}
		};

		using ById = QuestSearcherById;

#if defined(__WORLDSERVER) || defined(__CLIENT)

	public:
		// bool IsCheckedQuestID(BYTE questId) const;
		void RemoveQuest(int questId);
		void Clear();
		QUEST * FindQuest(int questId);
		[[nodiscard]] bool IsCompletedQuest(int questId) const;
#endif
	};
}

CAr & operator<<(CAr & ar, const MoverSub::Quests & quests);
CAr & operator>>(CAr & ar, MoverSub::Quests & quests);
