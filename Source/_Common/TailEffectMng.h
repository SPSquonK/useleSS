#pragma once

#ifdef __CLIENT

class CTailEffect
{
protected:
	int		m_nType;
	BOOL	m_bActive;
	FLOAT	m_fFadeSpeed;
	int		m_nMaxTail;

public:
	virtual ~CTailEffect() {}

	virtual void Init( void ) { m_nType = 0; m_bActive = FALSE; };
	
	virtual void	Create( int nType, FLOAT fFadeSpeed = 0.030f ) {};
	virtual int		CreateTail( const D3DXVECTOR3 &vPos1, const D3DXVECTOR3 &vPos2 ) { return 0; }
	virtual void	Clear( void ) {};
	int				GetType( void ) { return m_nType; }
	BOOL			IsActive() { return m_bActive; }
	
	virtual HRESULT FrameMove( void ) = 0;
	virtual HRESULT InitDeviceObjects( LPCTSTR szFileName ) = 0;
	virtual HRESULT RestoreDeviceObjects() = 0;
    virtual HRESULT InvalidateDeviceObjects() = 0;
	virtual HRESULT Render() = 0;

	virtual HRESULT ChangeTexture( LPCTSTR szFileName, int nType ) { return S_OK; }
};
	
class CTailEffectBelt : public CTailEffect
{
protected:
	
	struct TAIL
	{
			D3DXVECTOR3 m_vPos1;       // 꼬리 좌표 1
			D3DXVECTOR3 m_vPos2;       // 꼬리 좌표 2
	
			D3DXCOLOR   m_clrDiffuse; // Initial diffuse color
			D3DXCOLOR   m_clrFade;    // Faded diffuse color
			FLOAT       m_fFade;      // Fade progression
	
			TAIL*   m_pNext;      // Next Tail in list
	};

	TAIL	*m_pPool;
	int		m_nPoolPtr;
    DWORD	m_dwBase;
	DWORD	m_dwFlush;
    DWORD	m_dwDiscard;
	
    TAIL* m_pTails;			// 리스트의 현재 포인터.
    TAIL* m_pTailsFree;		// 비어있는 포인터
	
    LPDIRECT3DVERTEXBUFFER9 m_pVB;
	
	virtual void Init( void );
	
public:
	LPDIRECT3DTEXTURE9 m_pTexture;
	
	CTailEffectBelt();
	virtual ~CTailEffectBelt();
	
	virtual void	Create( int nType, FLOAT fFadeSpeed = 0.030f );
	virtual int		CreateTail( const D3DXVECTOR3 &vPos1, const D3DXVECTOR3 &vPos2 );
	virtual void	Clear( void );
	
	virtual HRESULT FrameMove( void );
	
	virtual HRESULT InitDeviceObjects( LPCTSTR szFileName );
	virtual HRESULT RestoreDeviceObjects();
    virtual HRESULT InvalidateDeviceObjects();
	HRESULT ChangeTexture( LPCTSTR szFileName, int nType );
	
	virtual HRESULT Render();
};


class CTailEffectModel : public CTailEffect
{
protected:
	struct TAILMODEL {
		D3DXMATRIX  m_mWorld;
		int			m_nFactor;
	};

	int					m_nMaxTail;
	CModelObject*		m_pModel;
	std::vector<TAILMODEL>	m_vecTail;
	
	virtual void Init( void );
	
public:
	CTailEffectModel();
	~CTailEffectModel() override;
	
	virtual void	Create( int nType, FLOAT fFadeSpeed = 0.030f );
	virtual int		CreateTail( D3DXMATRIX* pTail );
	virtual void	Clear( void );
	
	virtual HRESULT FrameMove( void );
	
	virtual HRESULT InitDeviceObjects( LPCTSTR szFileName );
	virtual HRESULT RestoreDeviceObjects();
    virtual HRESULT InvalidateDeviceObjects();
	
	virtual HRESULT Render();
};

class CTailEffectMng final {
	static constexpr size_t MAX_TAILEFFECT = 32; 		// 최대 파티클 종류.
	
	std::array<CTailEffect *, MAX_TAILEFFECT> m_TailEffects;

public:
	CTailEffectMng();
	CTailEffectMng(const CTailEffectMng &) = delete;
	CTailEffectMng & operator=(const CTailEffectMng &) = delete;
	~CTailEffectMng();

	HRESULT InitDeviceObjects() { return S_OK; }
	HRESULT RestoreDeviceObjects();
	HRESULT InvalidateDeviceObjects();
		
	CTailEffect *AddEffect( LPCTSTR szFileName, int nType, FLOAT fFadeSpeed = 0.030f );
	int			Delete(CTailEffect * pTail);
		
	void	Process( void );
	void	Render();
	
};




extern CTailEffectMng	g_TailEffectMng;

#endif // CLIENT
