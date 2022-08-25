#pragma once

#include <map>
#include <ranges>
#include <optional>

/**
 * Double map Id <-> defined identifiers with prefix "look back"
 *
 * Ideas: SquonK, Seb
 * Implementation: SquonK
 * 2021-04 / 2022-08
 * 
 * https://gist.github.com/SPSquonK/e48cb0243bc9d65ddaeaed77ba95adb1
 * Released under the Boost License
 */
class CScriptDefines final {
	std::map<CString, int> m_strToIds;
	std::map<CString, std::multimap<int, CString>> m_prefixToIdsToStr;

public:
	bool Insert(const CString & token, int id) {
		const bool r = m_strToIds.try_emplace(token, id).second;
		if (!r) return false;

		for (auto & [prefix, map] : m_prefixToIdsToStr) {
			if (token.Find(prefix) == 0) {
				map.emplace(id, token);
			}
		}

		return true;
	}

	[[nodiscard]] std::optional<int> Find(const char * string) const {
		const auto it = m_strToIds.find(string);
		if (it == m_strToIds.end()) return std::nullopt;
		return it->second;
	}

	[[nodiscard]] const CString * Lookup(int id, const CString & prefix) {
		const auto & index = GetOrBuild(prefix);

		const auto it = index.find(id);
		if (it == index.end()) {
			return nullptr;
		} else {
			return &it->second;
		}
	}

	const std::multimap<int, CString> & GetOrBuild(const CString & prefix) {
		const auto it = m_prefixToIdsToStr.find(prefix);
		if (it != m_prefixToIdsToStr.end()) return it->second;

		auto & result = m_prefixToIdsToStr[prefix];

		for (const auto & [name, id] : m_strToIds) {
			if (name.Find(prefix) == 0) {
				result.emplace(id, name);
			}
		}
		
		return result;
	}

	/*
	 * Unit test code: Uncomment and call this function to test the behaviour. This function should
	 * always return true.
	 * Commentated to make the compiler's work easier
public:
	static bool Test_ExtractAllWithPrefix() {
		CDefines defines;
		defines.Insert("Lama", 1);
		defines.Insert("Chat", 2);
		defines.Insert("Chien", 3);
		const auto OneTest = [&](const char * prefix, size_t expectedNumber) {
			const auto r = defines.GetOrBuild(prefix);
			if (r.size() != expectedNumber) {
				std::cout << prefix << " - Expected: " << expectedNumber << " ; Found: " << r.size() << '\n';
				return false;
			}
			std::cout << prefix << ':';
			for (const auto & [_, str] : r) std::cout << ' ' << str;
			std::cout << '\n';
			return true;
		};
		return OneTest("Ch", 2)
			&& OneTest("Cha", 1)
			&& OneTest("Lam", 1)
			&& OneTest("Can", 0);
	}
	*/
};