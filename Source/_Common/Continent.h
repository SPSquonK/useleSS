#pragma once

#define min_cont(x,y) (x < y ? x : y)
#define max_cont(x,y) (x > y ? x : y)

#include "ContinentDef.h"


class CContinent  
{
public:
	CContinent();
	virtual ~CContinent();

	static CContinent*	GetInstance( void );
#ifndef __DBSERVER	
	BYTE GetContinent( CMover* pMover );	// 현재 캐릭터가 위치하고 있는 대륙
	BYTE GetArea( CMover* pMover );			// 현재 캐릭터가 위치하고 있는 대륙의 지역(동부, 서부...)
	static BOOL	IsValidObj( CMover* pMover );
#endif // !__DBSERVER
	CString GetContinentName( BYTE nCont ); 
#ifdef __WORLDSERVER
	D3DXVECTOR3 GetRevivalPos( BYTE nContinent );
#endif // __WORLDSERVER

#ifdef __CLIENT
	int	GetMapNo( CMover* pMover );
	int	GetMapNo( const D3DXVECTOR3& vPos );		//gmpbigsun : const 
#endif // __CLIENT

	int GetContinentIndex( const char* continentName );

#ifdef __IMPROVE_MAP_SYSTEM
public:
	BYTE GetContinent( const D3DXVECTOR3& vPos );

private:
#else // __IMPROVE_MAP_SYSTEM
private:
	BYTE GetContinent( const D3DXVECTOR3& vPos );
#endif // __IMPROVE_MAP_SYSTEM
	BYTE GetArea( BYTE nCont );

private:
	void Init();
	BOOL Point_In_Poly(std::vector<CPoint> vecMap, CPoint test_point );
	std::map< BYTE, std::vector<CPoint> > m_MapCont;
	std::map< BYTE, std::vector<CPoint> > m_MapContTown;
public:
	BYTE GetTown( const D3DXVECTOR3& vPos );		//world pos로 get town id!
};
