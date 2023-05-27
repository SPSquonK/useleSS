#pragma once

#include <vector>
#include <utility>

struct PARTICLE {
	D3DXVECTOR3 m_vPos;       // Current position
	D3DXVECTOR3 m_vVel;       // Current velocity

	D3DXCOLOR   m_clrDiffuse; // Initial diffuse color
	D3DXCOLOR   m_clrFade;    // Faded diffuse color
	FLOAT       m_fFade;      // Fade progression
	FLOAT		m_fGroundY;

	PARTICLE * m_pNext;      // Next particle in list
};

class CParticles final {
private:
	PARTICLE * m_pPool;

	int		m_nType;
	DWORD	m_dwBase;
	DWORD	m_dwFlush;
	DWORD	m_dwDiscard;
	
	// Intrusive single linked lists
	PARTICLE * m_pParticles;
	PARTICLE * m_pParticlesFree;
	
  // Geometry
  LPDIRECT3DVERTEXBUFFER9 m_pVB;

public:
	bool	m_bActive;
	bool	m_bGravity;
	FLOAT	m_fSize;
	LPDIRECT3DTEXTURE9 m_pParticleTexture;
	
	CParticles();
	CParticles(const CParticles &) = delete;
	CParticles & operator=(const CParticles &) = delete;
	~CParticles();

	void Create( DWORD dwFlush, DWORD dwDiscard, int nType );
	
	HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice, LPCTSTR szFileName );
	HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
  HRESULT InvalidateDeviceObjects();
	
	HRESULT CreateParticle( int nType, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vVel, FLOAT fGroundY );
	HRESULT Update( void );
		
  HRESULT Render( LPDIRECT3DDEVICE9 pd3dDevice );

	[[nodiscard]] static std::pair<float, const char *> GetParticleTypeInfo(int nType);
};

class CParticleMng final {
	static constexpr int MAX_PARTICLE_TYPE = 32; // 최대 파티클 종류.
	LPDIRECT3DDEVICE9 m_pd3dDevice;
	CParticles m_Particles[ MAX_PARTICLE_TYPE ];

public:
	HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice ) {
		m_pd3dDevice = pd3dDevice;
		return S_OK;
	}
	HRESULT RestoreDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
  HRESULT InvalidateDeviceObjects();

	CParticles * CreateParticle( int nType, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vVel, FLOAT fGroundY );
		
	void	Process();
	void	Render( LPDIRECT3DDEVICE9 pd3dDevice );
};

extern CParticleMng	g_ParticleMng;

