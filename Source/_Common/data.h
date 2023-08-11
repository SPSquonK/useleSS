#pragma once

#include "debug.h"

static constexpr DWORD NULL_ID	= 0xffffffff;

#include "Container_FixedArray.hpp"

using EXPINTEGER = __int64;
using EXPFLOAT   = double;

#if defined(__WORLDSERVER ) || defined(__CLIENT)
// A struct for the ability to mark a location on click in the navigation.
struct NaviPoint {
	// Emphasize that this location is a location in the world, not a location on the navigator.
	D3DXVECTOR3 Pos = D3DXVECTOR3(0.f, 0.f, 0.f);
	// Time remaining in points
	short		Time = 0;
	DWORD		objid = 0xffffffff;
	std::string      Name = "";
}; 

typedef std::vector<NaviPoint> V_NaviPoint;

#endif


#include "ePropType.h"
