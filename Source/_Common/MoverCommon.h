#pragma once

#include "FlyFFTypes.h"
#include <boost/container/static_vector.hpp>

namespace MoverSub {
	struct SkinMeshs {
		static constexpr bool Archivable = true;

		std::uint8_t skinSet  = 0;
		std::uint8_t face     = 0;
		std::uint8_t hairMesh = 0;
		std::uint8_t headMesh = 0;
	};

	struct Quests {
		boost::container::static_vector<QUEST, MAX_QUEST> current;
		boost::container::static_vector<QuestId, MAX_COMPLETE_QUEST> completed;
		boost::container::static_vector<QuestId, MAX_CHECKED_QUEST> checked;

		struct ById {
			QuestId questId;
			ById(QuestId questId) : questId(questId) {}

			[[nodiscard]] constexpr bool operator()(const QUEST & quest) const noexcept {
				return quest.m_wId == questId;
			}
		};

#if defined(__WORLDSERVER) || defined(__CLIENT)

	public:
		// bool IsCheckedQuestID(BYTE questId) const;
		void RemoveQuest(QuestId questId);
		void Clear();
		QUEST * FindQuest(QuestId questId);
		[[nodiscard]] bool IsCompletedQuest(QuestId questId) const;
#endif
	};
}

CAr & operator<<(CAr & ar, const MoverSub::Quests & quests);
CAr & operator>>(CAr & ar, MoverSub::Quests & quests);
