#pragma once

#include "debug.h"

const DWORD NULL_ID	= 0xffffffff;

#include "Container_FixedArray.hpp"

typedef	__int64	EXPINTEGER;
typedef	double	EXPFLOAT;

#if defined(__WORLDSERVER ) || defined(__CLIENT)
struct NaviPoint //	네비게이션에 클릭으로 위치를 표시하는 기능을 위한 구조체
{
	D3DXVECTOR3 Pos = D3DXVECTOR3(0.f, 0.f, 0.f);		//	이 위치는 네이게이터상의 위치가 아닌 월드상의 위치임을 강조한다.
	short		Time = 0;			//	포인트의 남아있는 시간
	DWORD		objid = 0xffffffff;
	std::string      Name = "";
}; 

typedef std::vector<NaviPoint> V_NaviPoint;

#endif // defined(__WORLDSERVER ) || defined(__CLIENT)

namespace SAI79
{
	// ************************************
	// * 타입 없음.	: 0
	// * 불   속성	: 1
	// * 물   속성	: 2
	// * 전기 속성	: 3
	// * 바람 속성	: 4
	// * 땅   속성	: 5
	// * 이   속성 타입을 ePropType로 설정
	// ************************************
	enum	ePropType 	{ NO_PROP = 0, FIRE, WATER, ELECTRICITY, WIND, EARTH, END_PROP };
} // namespace SAI79

