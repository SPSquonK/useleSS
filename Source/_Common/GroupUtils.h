#pragma once

#include <boost/container/small_vector.hpp>

#include "party.h"
#include "guild.h"


namespace useless_util {

	template<typename Range>
	struct FilterValidObjView {
		// TODO: requires range dereference to CUser

	private:
		Range range;

	public:
		FilterValidObjView(Range range) : range(range) {}

		struct Sentinel {};

		struct Iterator {
			using UnderlyingIteratorCurrent = decltype(std::declval<Range>().begin());
			using UnderlyingIteratorEnd = decltype(std::declval<Range>().end());

		private:
			UnderlyingIteratorCurrent iterator;
			UnderlyingIteratorEnd     end;

		public:
			Iterator(UnderlyingIteratorCurrent begin, UnderlyingIteratorEnd end)
				: iterator(begin), end(end) {
				GoToValidOrEnd();
			}

			CUser * operator*() const { return *iterator; }

			Iterator & operator++() {
				++iterator;
				GoToValidOrEnd();
				return *this;
			}

			[[nodiscard]] bool operator==(Sentinel) const { return IsFinished(); }

		private:
			[[nodiscard]] bool IsFinished() const { return iterator == end; }

			void GoToValidOrEnd() {
				while (!IsFinished() && !IsValidObj(*iterator)) {
					++iterator;
				}
			}
		};

		[[nodiscard]] Iterator begin() { return Iterator(range.begin(), range.end()); }
		[[nodiscard]] Sentinel end() { return Sentinel{}; }
	};


	struct FilterValidObjAdaptor {
		template<typename Range>
		[[nodiscard]] friend auto operator|(Range range, FilterValidObjAdaptor) {
			return FilterValidObjView(range);
		}
	};
}

static constexpr useless_util::FilterValidObjAdaptor FilterValidObjs = useless_util::FilterValidObjAdaptor{};

namespace useless_util {
	struct AllMembersImplem {
		friend auto operator|(const CParty & party, AllMembersImplem) {
			return std::views::counted(party.m_aMember, party.GetSizeofMember())
				| std::views::transform([](const PartyMember & pm) { return prj.GetUserByID(pm.m_uPlayerId); })
				| FilterValidObjs;
		}

		friend auto operator|(const CGuild & guild, AllMembersImplem) {
			return guild.m_mapPMember
				| std::views::values
				| std::views::transform([](const CGuildMember * pGM) { return prj.GetUserByID(pGM->m_idPlayer); })
				| FilterValidObjs;
		}

		friend auto operator|(const CParty * pParty, AllMembersImplem) {
			return *pParty | AllMembersImplem{};
		}

		friend auto operator|(const CGuild * pGuild, AllMembersImplem) {
			return *pGuild | AllMembersImplem{};
		}
	};
}

static constexpr auto AllMembers = useless_util::AllMembersImplem{};
