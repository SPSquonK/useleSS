// CampusDBCtrl.h: interface for the CCampusDBCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAMPUSDBCTRL_H__D25D11CF_A207_47BA_A35B_5AD4D63DAAB9__INCLUDED_)
#define AFX_CAMPUSDBCTRL_H__D25D11CF_A207_47BA_A35B_5AD4D63DAAB9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "dbcontroller.h"
#include "Campus.h"
#include <memory>

//////////////////////////////////////////////////////////////////////
// CCampusDBCtrl Construction/Destruction
//////////////////////////////////////////////////////////////////////

enum
{
	CAMPUS_LOAD,
	CAMPUS_SEND,
	CAMPUS_ADD_MEMBER,
	CAMPUS_REMOVE_MEMBER,
	CAMPUS_UPDATE_POINT,
};

class CCampusDBCtrl : public CDbController
{
public:
	
	CCampusDBCtrl(CCampusMng & campusMng) : m_campusMng(campusMng) {}

	virtual void Handler( LPDB_OVERLAPPED_PLUS pov, DWORD dwCompletionKey );

private:
	void	CreateLogQuery();

	void	AddCampusMember( CAr & ar );
	void	RemoveCampusMember( CAr & ar );
	void	UpdateCampusPoint( CAr & ar );

	void	LoadAllCampus();
	void	SendAllCampus( DPID dpId );
	void	InsertCampus( u_long idCampus );
	void	DeleteCampus( u_long idCampus );
	void	InsertCampusMember( u_long idCampus, u_long idPlayer, CampusRole nMemberLv );
	void	DeleteCampusMember( u_long idPlayer, CampusRole nMemberLv );
	int		UpdateCampusPoint( u_long idPlayer, int nCampusPoint );
	void	UpdateCampusId( u_long idPlayer, u_long idCampus );

	void	LogUpdateCampusMember( u_long idCampus, u_long idMaster, u_long idPupil, char chState );
	void	LogUpdateCampusPoint( u_long idPlayer, int nPrevPoint, int nCurrPoint, char chState );

	std::unique_ptr<CQuery> m_pLogQuery = nullptr;
	CCampusMng & m_campusMng;
};

//////////////////////////////////////////////////////////////////////
// CCampusHelper Construction/Destruction
//////////////////////////////////////////////////////////////////////

class CCampusHelper
{
public:
	CCampusHelper();
	~CCampusHelper();
	static CCampusHelper* GetInstance();

	BOOL PostRequest( int nQuery, BYTE* lpBuf = NULL, int nBufSize = 0, DWORD dwCompletionKey = 0 )
	{ return m_CampusDBCtrl.PostRequest( nQuery, lpBuf, nBufSize, dwCompletionKey );	}

	void	Serialize( CAr & ar )	{ m_pCampusMng.Serialize( ar );	} // only operator<< is used
	void RemovePlayerFromCampus(u_long playerId);

private:
	CCampusMng 		m_pCampusMng;
	CCampusDBCtrl	m_CampusDBCtrl;
};

#endif // !defined(AFX_CAMPUSDBCTRL_H__D25D11CF_A207_47BA_A35B_5AD4D63DAAB9__INCLUDED_)
