#pragma once

#if !defined(__WORLDSERVER) && !defined(__CLIENT)
static_assert(false, "worldmng.h included in another project than World or Client")
#endif

#include <vector>
#include <memory>

typedef struct tagWORLD
{
	TCHAR	m_szFileName[128];
	TCHAR	m_szWorldName[128];
	DWORD	m_dwId;
	BOOL	IsValid()	{	return( m_szFileName[0] != '\0' );	}
}
WORLD,* LPWORLD;

#define MAX_WORLD	256

class CWorld;
class CMover;

class CWorldMng final {
public:
#ifdef __CLIENT
	std::unique_ptr<CWorld> m_currentWorld;
#endif

#ifdef __WORLDSERVER	//	server	--------------------------------------------
public:
	std::vector<std::unique_ptr<CWorld>> m_worlds;

	void		Free();
	CWorld*		GetWorld( DWORD dwWorldID );
	void		Add( CJurisdiction* pJurisdiction );
	void		ReadObject();
	void		Process();
	[[nodiscard]] bool HasSomeoneGoingTo(DWORD dwWorldId, int nLayer) const;
#ifdef __LAYER_1015
	BOOL		AddObj( CObj* pObj, DWORD dwWorldID, BOOL bAddItToGlobalId, int nLayer );
#else	// __LAYER_1015
	BOOL		AddObj( CObj* pObj, DWORD dwWorldID, BOOL bAddItToGlobalId = FALSE );
#endif	// __LAYER_1015
	bool PreremoveObj(const OBJID objid);
	CObj*		PregetObj( OBJID objid );
	u_long		Respawn();
	[[nodiscard]] DWORD GetObjCount() const noexcept;

	CRegionElemArray	m_aRevivalPos;
	CRegionElemArray	m_aRevivalRgn;	
#else	// __WORLDSERVER	//	not server	------------------------------------

public:
//	Constructions
//	Operations
	void	Free();
	CWorld*		operator () () {	return	m_currentWorld.get(); }
	CWorld*		Get()	{	return	m_currentWorld.get(); }
	CWorld*		Open( LPDIRECT3DDEVICE9 pd3dDevice, LPCSTR lpszWorld );
	CWorld*		Open( LPDIRECT3DDEVICE9 pd3dDevice, OBJID idWorld );
	void DestroyCurrentWorld();
#endif	// __WOLDSERVER	--------------------------------------------------------

//	common	--------------------------------------------------------------------
private:
	int		m_nSize;
	void	LoadRevivalPos( DWORD dwWorldId, LPCTSTR lpszWorld );		// as loading script, it is called

#ifdef __WORLDSERVER
public:
	REGIONELEM * GetRevivalPosChao( DWORD dwWorldId, LPCTSTR sKey );
	REGIONELEM * GetNearRevivalPosChao( DWORD dwWorldId, const D3DXVECTOR3 & vPos );
	REGIONELEM * GetRevivalPos( DWORD dwWorldId, LPCTSTR sKey );
	REGIONELEM * GetNearRevivalPos( DWORD dwWorldId, const D3DXVECTOR3 & vPos );

	const REGIONELEM * GetRevival(CMover * pUser);
	const REGIONELEM * GetRevival(const CWorld & world, const D3DXVECTOR3 & vPos, bool isChaotic);
#endif	// __WORLDSERVER
public:

	CFixedArray< WORLD > m_aWorld;
//	Constructions
	CWorldMng();
	virtual	~CWorldMng();
//	Operations
	LPWORLD	GetWorldStruct( OBJID idWorld )	{ return (LPWORLD)m_aWorld.GetAt( idWorld ); }
	BOOL	LoadScript( LPCTSTR lpszFileName );
	void LoadAllMoverDialog();

#ifdef __MAP_SECURITY
	void	AddMapCheckInfo( const char* szFileName );
private:
	BOOL	SetMapCheck( const char* szFileName );
#ifdef __CLIENT
	std::vector<std::string> m_vecstrFileName;
public:
	void	CheckMapKey();
#endif // __CLIENT

#ifdef __WORLDSERVER
	std::map<std::string, std::string>	m_mapMapKey;
public:
	void	CheckMapKey( CUser* pUser, const char* szFileName, const char* szMapKey );
#endif // __WORLDSERVER

#endif // __MAP_SECURITY
/*--------------------------------------------------------------------------------*/
};

#ifdef __WORLDSERVER
extern CWorldMng g_WorldMng;
#endif
