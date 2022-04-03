#pragma once

#include "d3dx9math.h"

namespace D3DXR {
	inline D3DXVECTOR3 Cross(const D3DXVECTOR3 & p1, const D3DXVECTOR3 & p2) {
		D3DXVECTOR3 result;
		D3DXVec3Cross(&result, &p1, &p2);
		return result;
	}

	inline D3DXVECTOR3 TransformCoord(const D3DXVECTOR3 & pV, const D3DXMATRIX & pM) {
		D3DXVECTOR3 result;
		D3DXVec3TransformCoord(&result, &pV, &pM);
		return result;
	}

	inline D3DXVECTOR3 LerpWith0(const D3DXVECTOR3 & v, const float x) {
		const auto zero = D3DXVECTOR3(0.f, 0.f, 0.f);
		D3DXVECTOR3 result;
		D3DXVec3Lerp(&result, &zero, &v, x);
		return result;
	}

	inline FLOAT Length(const D3DXVECTOR3 & pV) {
		return D3DXVec3Length(&pV);
	}

	inline FLOAT LengthSq(const D3DXVECTOR3 & pV) {
		return D3DXVec3LengthSq(&pV);
	}

	inline D3DXMATRIX LookAtLH(const D3DXVECTOR3 & pEye, const D3DXVECTOR3 & pAt, const D3DXVECTOR3 & pUp) {
		D3DXMATRIX result;
		D3DXMatrixLookAtLH(&result, &pEye, &pAt, &pUp);
		return result;
	}

	inline D3DXMATRIX LookAtLH010(const D3DXVECTOR3 & pEye, const D3DXVECTOR3 & pAt) {
		return LookAtLH(pEye, pAt, D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	}
}
