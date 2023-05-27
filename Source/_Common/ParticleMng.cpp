#include "stdafx.h"
#include "ParticleMng.h"

struct POINTVERTEX {
	D3DXVECTOR3 v;
	D3DCOLOR    color;

	static constexpr DWORD FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;
};


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------

CParticles::CParticles() {
	m_pPool = nullptr;

	m_nType = 0;
	m_dwBase = 0;
	m_dwFlush = 0;
	m_dwDiscard = 0;
	
	m_pParticles = nullptr;
	m_pParticlesFree = nullptr;

	m_pVB = nullptr;

	m_bActive = FALSE;
	m_bGravity = false;
	m_fSize = 0.3f;
	m_pParticleTexture = nullptr;
}


//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
CParticles::~CParticles() {
	InvalidateDeviceObjects();
	SAFE_RELEASE(m_pParticleTexture);
	SAFE_DELETE_ARRAY(m_pPool);
}

void CParticles::Create( DWORD dwFlush, DWORD dwDiscard, int nType ) {
	assert(!m_bActive);

	m_bActive   = true;	
	m_dwBase    = dwDiscard;
	m_dwFlush   = dwFlush;
	m_dwDiscard = dwDiscard;
	
	m_nType = nType;
	
	m_pPool = new PARTICLE[dwDiscard];
	memset(m_pPool, 0, sizeof(PARTICLE) * dwDiscard);

	for (DWORD i = 0; i < dwDiscard; ++i) {
		m_pPool[i].m_pNext = m_pParticlesFree;
		m_pParticlesFree = &m_pPool[i];
	}
}



HRESULT CParticles::InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR szFileName )
{
	if (!m_bActive)	return S_OK;
				
	// Create the texture using D3DX
	HRESULT hr = LoadTextureFromRes( pd3dDevice, MakePath( DIR_MODELTEX, szFileName ),
									   D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, 
									   D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 
									   D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 0, NULL, NULL, &m_pParticleTexture );
	
	if (hr == E_FAIL) {
		Error("%s 읽기 실패", MakePath(DIR_MODELTEX, szFileName));
		m_bActive = false;
	}

	return hr;
}



//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
HRESULT CParticles::RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if (!m_bActive)			return S_OK;
	if( m_pParticleTexture == NULL )	return S_OK;
    HRESULT hr;

    // Create a vertex buffer for the particle system.  The size of this buffer
    // does not relate to the number of particles that exist.  Rather, the
    // buffer is used as a communication channel with the device.. we fill in 
    // a bit, and tell the device to draw.  While the device is drawing, we
    // fill in the next bit using NOOVERWRITE.  We continue doing this until 
    // we run out of vertex buffer space, and are forced to DISCARD the buffer
    // and start over at the beginning.

    if(FAILED(hr = pd3dDevice->CreateVertexBuffer( m_dwDiscard * 
		sizeof(POINTVERTEX), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY | D3DUSAGE_POINTS, 
        POINTVERTEX::FVF, D3DPOOL_DEFAULT, &m_pVB, NULL )))
	{
        return E_FAIL;
	}

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------
HRESULT CParticles::InvalidateDeviceObjects()
{
    SAFE_RELEASE( m_pVB );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name:
// Desc:
//-----------------------------------------------------------------------------

HRESULT CParticles::Update(void)
{
	if (!m_bActive)	return S_OK;
	if (m_pParticleTexture == NULL)	return S_OK;

	PARTICLE ** ppParticle = &m_pParticles;

	while (*ppParticle) {
		PARTICLE * pParticle = *ppParticle;

		// Calculate new position
		pParticle->m_fFade -= 0.015f;

		pParticle->m_vPos += pParticle->m_vVel;
		if (m_bGravity)
			pParticle->m_vVel.y -= 0.005f;		// 중력.

		if (pParticle->m_fFade < 0.0f)
			pParticle->m_fFade = 0;

		// Kill old particles
		// 희미해져 사라졌을때나 바닥에 떨어졌을때는 삭제됨.
		if (pParticle->m_fFade <= 0 || (m_bGravity && pParticle->m_vPos.y < pParticle->m_fGroundY))
		{
			// Kill particle
			*ppParticle = pParticle->m_pNext;
			pParticle->m_pNext = m_pParticlesFree;
			m_pParticlesFree = pParticle;
		} else {
			ppParticle = &pParticle->m_pNext;
		}
	}


	return S_OK;
}


// 파티클 객체 하나를 생성한다.
// vPos : 생성되는 시작위치
// vVel : 날아갈 방향.
HRESULT CParticles::CreateParticle( int nType, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vVel, FLOAT fGroundY )
{
	if (!m_bActive)	return S_OK;
	if( m_pParticleTexture == NULL )	return S_OK;

	// Find free particle
	if (m_pParticlesFree == nullptr) return E_FAIL;

	
	PARTICLE * pParticle = m_pParticlesFree;
	m_pParticlesFree = pParticle->m_pNext;

	pParticle->m_pNext = m_pParticles;
	m_pParticles = pParticle;
	
  // Emit new particle
	
	// 최초 위치 세팅
  pParticle->m_vPos = vPos;
	// 속도벡터 생성.
	pParticle->m_vVel = vVel;
	
	if (m_nType == 0) {
		pParticle->m_clrDiffuse = D3DXCOLOR( 1.0f , 0.5f , 0.5f , 1.0f);
		pParticle->m_clrFade    = D3DXCOLOR( 1.0f , 0.25f, 0.25f, 1.0f);
	} else if(m_nType < 13) {
		pParticle->m_clrDiffuse = D3DXCOLOR( 0.35f, 0.35f, 1.00f, 1.0f );
		pParticle->m_clrFade    = D3DXCOLOR( 0.0f , 0.0f , 0.0f , 1.0f );
	} else {
		pParticle->m_clrDiffuse = D3DXCOLOR( 1.0f , 1.0f , 1.0f , 1.0f );
		pParticle->m_clrFade    = D3DXCOLOR( 0.0f , 0.0f , 0.0f , 1.0f );
	}

	pParticle->m_fFade    = 1.0f;
	pParticle->m_fGroundY	= fGroundY;
	
	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Renders the particle system using either pointsprites (if supported)
//       or using 4 vertices per particle
//-----------------------------------------------------------------------------
HRESULT CParticles::Render(LPDIRECT3DDEVICE9 pd3dDevice)
{
	if (!m_bActive)	return S_OK;
	if (m_pParticleTexture == nullptr)	return S_OK;
	if (m_pParticles == nullptr) return S_OK;

	D3DXMATRIX mWorld;
	D3DXMatrixIdentity(&mWorld);
	pd3dDevice->SetTransform(D3DTS_WORLD, &mWorld);		// Set World Transform 

	// Draw particles
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
	pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	pd3dDevice->SetRenderState(D3DRS_AMBIENT, 0xffffffff);
	//	pd3dDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_FLAT );

		//
	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

	pd3dDevice->SetTexture(0, m_pParticleTexture);

	HRESULT hr;

	// Set the render states for using point sprites
	pd3dDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_POINTSCALEENABLE, TRUE);
	pd3dDevice->SetRenderState(D3DRS_POINTSIZE, FtoDW(m_fSize));

	pd3dDevice->SetRenderState(D3DRS_POINTSIZE_MIN, FtoDW(1.00f));
	pd3dDevice->SetRenderState(D3DRS_POINTSCALE_A, FtoDW(0.00f));
	pd3dDevice->SetRenderState(D3DRS_POINTSCALE_B, FtoDW(0.00f));
	pd3dDevice->SetRenderState(D3DRS_POINTSCALE_C, FtoDW(1.00f));

	// Set up the vertex buffer to be rendered
	pd3dDevice->SetStreamSource(0, m_pVB, 0, sizeof(POINTVERTEX));
	pd3dDevice->SetVertexShader(NULL);
	pd3dDevice->SetVertexDeclaration(NULL);
	pd3dDevice->SetFVF(POINTVERTEX::FVF);

	POINTVERTEX * pVertices;
	DWORD        dwNumParticlesToRender = 0;



	// Lock the vertex buffer.  We fill the vertex buffer in small
	// chunks, using D3DLOCK_NOOVERWRITE.  When we are done filling
	// each chunk, we call DrawPrim, and lock the next chunk.  When
	// we run out of space in the vertex buffer, we start over at
	// the beginning, using D3DLOCK_DISCARD.

	m_dwBase += m_dwFlush;

	if (m_dwBase >= m_dwDiscard)
		m_dwBase = 0;

	if (FAILED(hr = m_pVB->Lock(m_dwBase * sizeof(POINTVERTEX), m_dwFlush * sizeof(POINTVERTEX),
		(void **)&pVertices, m_dwBase ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD)))
	{
		return hr;
	}


	// 파티클 리스트를 따라 파티클을 렌더링한다.
	PARTICLE * pParticle = m_pParticles;
	while (pParticle) {
		D3DXVECTOR3 vPos = pParticle->m_vPos;
		D3DXVECTOR3 vVel = pParticle->m_vVel;

		D3DXCOLOR clrDiffuse;
		D3DXColorLerp(&clrDiffuse, &pParticle->m_clrFade, &pParticle->m_clrDiffuse, pParticle->m_fFade);
		DWORD dwDiffuse = (DWORD)clrDiffuse;

		// Render each particle a bunch of times to get a blurring effect
		pVertices->v = vPos;
		pVertices->color = dwDiffuse;
		pVertices++;

		if (++dwNumParticlesToRender == m_dwFlush) {
			// Done filling this chunk of the vertex buffer.  Lets unlock and
			// draw this portion so we can begin filling the next chunk.

			m_pVB->Unlock();

			if (FAILED(hr = pd3dDevice->DrawPrimitive(D3DPT_POINTLIST, m_dwBase, dwNumParticlesToRender)))
				return hr;

			// Lock the next chunk of the vertex buffer.  If we are at the 
			// end of the vertex buffer, DISCARD the vertex buffer and start
			// at the beginning.  Otherwise, specify NOOVERWRITE, so we can
			// continue filling the VB while the previous chunk is drawing.
			m_dwBase += m_dwFlush;

			if (m_dwBase >= m_dwDiscard)
				m_dwBase = 0;

			if (FAILED(hr = m_pVB->Lock(m_dwBase * sizeof(POINTVERTEX), m_dwFlush * sizeof(POINTVERTEX),
				(void **)&pVertices, m_dwBase ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD)))
			{
				return hr;
			}

			dwNumParticlesToRender = 0;
		}

		pParticle = pParticle->m_pNext;
	}

	// Unlock the vertex buffer
	m_pVB->Unlock();

	// Render any remaining particles
	if (dwNumParticlesToRender)		// 위에서 512개씩 출력해주고 남은 파티클을 여기서 마저 다 그려줌.
	{
		if (FAILED(hr = pd3dDevice->DrawPrimitive(D3DPT_POINTLIST, m_dwBase, dwNumParticlesToRender)))
			return hr;
	}

	// Reset render states
	pd3dDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);
	pd3dDevice->SetRenderState(D3DRS_POINTSCALEENABLE , FALSE);

	pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE     , TRUE);
	pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE , FALSE);

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//						class CParticleMng

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
CParticleMng	g_ParticleMng;


//
// 파티클 하나 생성.
//
CParticles *CParticleMng::CreateParticle( int nType, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vVel, FLOAT fGroundY )
{
	if (nType < 0 || nType >= MAX_PARTICLE_TYPE) {
		return nullptr;
	}

	CParticles * pParticles = &m_Particles[nType];

	if (!pParticles->m_bActive) { 	// 생성시킨적이 없는 파티클이면 최초 생성.
		pParticles->Create( 512, 512, nType );

		const auto [fSize, szFileName] = CParticles::GetParticleTypeInfo(nType);
		pParticles->m_fSize = fSize;
		pParticles->InitDeviceObjects( m_pd3dDevice, szFileName );		// 파티클 텍스쳐 로딩.
		pParticles->RestoreDeviceObjects( m_pd3dDevice );
	}

	pParticles->CreateParticle( nType, vPos, vVel, fGroundY );		// nType의 파티클 하나 생성.

	return pParticles;
}

std::pair<float, const char *> CParticles::GetParticleTypeInfo(const int nType) {
	switch (nType) {
		case 0:  return { 0.3f, "Sfx_ItemPatical01.dds"    };
		case 1:  return { 0.2f, "etc_Particle2.bmp"        };
		case 2:  return { 0.5f, "etc_Particle11.bmp"       };
		case 3:  return { 0.3f, "etc_Particle11.bmp"       };
		case 4:  return { 0.2f, "etc_Particle11.bmp"       };
		case 5:  return { 0.3f, "Sfx_ItemPatical06-01.dds" };
		case 6:  return { 0.3f, "Sfx_ItemPatical06-02.dds" };
		case 7:  return { 0.3f, "Sfx_ItemPatical06-03.dds" };
		case 8:  return { 0.3f, "Sfx_ItemPatical06-04.dds" };
		case 9:  return { 0.3f, "Sfx_ItemPatical06-05.dds" };
		case 10: return { 0.5f, "etc_ParticleCloud01.bmp"  };
		case 11: return { 0.3f, "etc_ParticleCloud01.bmp"  };
		case 12: return { 0.2f, "etc_ParticleCloud01.bmp"  };
		// blue
		case 13: return { 0.15f, "etc_Particle11.bmp" };
		case 14: return { 0.1f , "etc_Particle11.bmp" };
		case 15: return { 0.07f, "etc_Particle11.bmp" };
		// red
		case 16: return { 0.15f, "etc_Particle12.bmp" };
		case 17: return { 0.1f , "etc_Particle12.bmp" };
		case 18: return { 0.07f, "etc_Particle12.bmp" };
		// white
		case 19: return { 0.15f, "etc_Particle13.bmp" };
		case 20: return { 0.1f , "etc_Particle13.bmp" };
		case 21: return { 0.07f, "etc_Particle13.bmp" };
		// green
		case 22: return { 0.15f, "etc_Particle14.bmp" };
		case 23: return { 0.1f , "etc_Particle14.bmp" };
		case 24: return { 0.07f, "etc_Particle14.bmp" };
		// default
		default: return { 0.3f , "etc_Particle2.bmp" };
	}
}

void CParticleMng::Process() {
	for (CParticles & particles : m_Particles) {
		particles.Update();
	}
}

void CParticleMng::Render( LPDIRECT3DDEVICE9 pd3dDevice ) {
	for (CParticles & particles : m_Particles) {
		particles.Render(pd3dDevice);
	}
}

HRESULT CParticleMng::RestoreDeviceObjects(LPDIRECT3DDEVICE9 pd3dDevice) {
	for (CParticles & particles : m_Particles) {
		particles.RestoreDeviceObjects(pd3dDevice);
	}

	return S_OK;
}

HRESULT CParticleMng::InvalidateDeviceObjects() {
	for (CParticles & particles : m_Particles) {
		particles.InvalidateDeviceObjects();
	}

	return S_OK;
}
