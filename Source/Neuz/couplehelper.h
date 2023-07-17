#pragma once 

#include "ar.h"
#include "couple.h"

class CCoupleHelper final {
private:
	CCoupleHelper() = default;
public:
	static CCoupleHelper Instance;

	void	Clear() { m_pCouple = nullptr; }
	BOOL	Initialize();
	void	OnCouple( CAr & ar );
	void	OnProposeResult( CAr & ar );
	void	OnCoupleResult( CAr & ar );
	void	OnDecoupleResult();
	void	OnAddCoupleExperience( CAr & ar );
	CCouple*	GetCouple()		{	return m_pCouple.get();	}
private:
	std::unique_ptr<CCouple>	m_pCouple;
};
