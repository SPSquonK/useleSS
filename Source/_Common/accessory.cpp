#include "stdafx.h"
#include "accessory.h"

CAccessoryProperty g_AccessoryProperty;

bool CAccessoryProperty::LoadScript(LPCTSTR szFile) {
	CScript s;
	if (!s.Load(szFile)) return false;

	s.GetToken();	// subject or FINISHED
	while (s.tok != FINISHED) {
		if (s.Token == _T("Probability"))	// 아이템 각성
		{
			const auto probabilities = s.GetNumbers<DWORD>('}');
			ChangeProbabilities(probabilities);
		} else if (s.Token == _T("Accessory")) {
			s.GetToken();	// {
			DWORD dwItemId = s.GetNumber();
			while (*s.token != '}') {
				s.GetToken();	// {
				std::array<std::vector<SINGLE_DST>, MAX_AAO + 1> pVector;

				int nAbilityOption = s.GetNumber();
				while (*s.token != '}') {
					ASSERT(nAbilityOption >= 0 && nAbilityOption <= MAX_AAO);
					s.GetToken();	// {
					SINGLE_DST dst;
					dst.nDst = s.GetNumber();
					while (*s.token != '}') {
						dst.nAdj = s.GetNumber();
						pVector[nAbilityOption].push_back(dst);
						dst.nDst = s.GetNumber();
					}
					nAbilityOption = s.GetNumber();
				}
				bool b = m_mapAccessory.emplace(dwItemId, std::move(pVector)).second;
				ASSERT(b);
				dwItemId = s.GetNumber();
			}
		}
		s.GetToken();	//
	}

	return true;
}

void CAccessoryProperty::ChangeProbabilities(const std::vector<DWORD> & probabilities) {
	if (probabilities.size() < MAX_AAO) {
		m_adwProbability.fill(0);
		std::ranges::copy(probabilities, m_adwProbability.begin());
	} else {
		Error(__FUNCTION__"(): %lu >= %d probabilities passed", probabilities.size(), MAX_AAO);
		std::copy(probabilities.begin(), probabilities.begin() + MAX_AAO, m_adwProbability.begin());
	}
}
