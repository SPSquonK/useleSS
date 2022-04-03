#ifndef __COUPLEHELPER_H
#define	__COUPLEHELPER_H

#include "ar.h"
#include "couple.h"

class CCoupleHelper
{
private:
	CCoupleHelper();
public:
	virtual	~CCoupleHelper();
	static	CCoupleHelper*	Instance();
	void	Clear();
	BOOL	Initialize();
	void	OnCouple( CAr & ar );
	void	OnProposeResult( CAr & ar );
	void	OnCoupleResult( CAr & ar );
	void	OnDecoupleResult();
	void	OnAddCoupleExperience( CAr & ar );
	CCouple*	GetCouple()		{	return m_pCouple;	}
private:
	CCouple*	m_pCouple;
};
#endif	// __COUPLEHELPER_H