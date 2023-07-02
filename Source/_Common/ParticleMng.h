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
	
	HRESULT InitDeviceObjects( LPCTSTR szFileName );
	HRESULT RestoreDeviceObjects( );
  HRESULT InvalidateDeviceObjects();
	
	HRESULT CreateParticle( int nType, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vVel, FLOAT fGroundY );
	HRESULT Update( void );
		
  HRESULT Render( );

	[[nodiscard]] static std::pair<float, const char *> GetParticleTypeInfo(int nType);
};

class CParticleMng final {
	static constexpr int MAX_PARTICLE_TYPE = 32; // 최대 파티클 종류.
	CParticles m_Particles[ MAX_PARTICLE_TYPE ];

public:
	HRESULT InitDeviceObjects( ) {
		return S_OK;
	}
	HRESULT RestoreDeviceObjects( );
  HRESULT InvalidateDeviceObjects();

	CParticles * CreateParticle( int nType, const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vVel, FLOAT fGroundY );
		
	void	Process();
	void	Render( );
};

extern CParticleMng	g_ParticleMng;

