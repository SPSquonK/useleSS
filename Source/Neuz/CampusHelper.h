#pragma once

#include "Campus.h"

class CCampusHelper  
{
public:
	CCampusHelper();
	~CCampusHelper();

	static CCampusHelper* GetInstance();

	CCampus*	GetCampus()	{	return m_pCampus;	}

	void	Clear();
	void	OnUpdateCampus( CAr & ar );
	void	OnRemoveCampus( CAr & ar );
	void	OnUpdateCampusPoint( CAr & ar );

private:
	CCampus* m_pCampus;
};

