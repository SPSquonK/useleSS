#include "stdafx.h"
#include "ModelMng.h"

static constexpr std::array<const TCHAR *, MAX_OBJTYPE> g_szRoot = std::to_array({
	"obj",
	"ani",
	"ctrl",
	"sfx",
	"item",
	"mvr",
	"region",
	"obj",		// ship
	"path"
});


CModelMng::~CModelMng() {
	for (CFixedArray<MODELELEM> & apModelElem : m_aaModelElem) {
		for (MODELELEM & pModelElem : apModelElem) {
			SAFE_DELETE_ARRAY(pModelElem.m_apszMotion);
		}
	}
}

MODELELEM * CModelMng::GetModelElem(const DWORD dwType, const DWORD dwIndex) {
	MODELELEM * const pElem = m_aaModelElem[dwType].GetAt(dwIndex);
	if (pElem) return pElem;

	if (dwType == OT_ITEM) {
		// Items with no model default appearance
		return m_aaModelElem[OT_ITEM].GetAt(II_ARM_M_VAG_HELMET02);
	}

	Error("GetModelElem - out of range: type=%lu, size=%d, index=%lu", dwType, m_aaModelElem[dwType].GetSize(), dwIndex);
	return nullptr;
}

void CModelMng::MakeModelName( TCHAR* pszModelName, DWORD dwType, DWORD dwIndex )
{
	MODELELEM * lpModelElem = GetModelElem(dwType, dwIndex);
	if (!lpModelElem) {
		Error("MakeModelNae : dwType=%d dwIndex=%d", dwType, dwIndex);
		throw std::runtime_error("MakeModelName Error");
	}

	if( lpModelElem->m_dwModelType == MODELTYPE_BILLBOARD )
	{
		_tcscpy( pszModelName, lpModelElem->m_szName );
		return;
	}
	if( dwType == OT_SFX )
	{
		if( _tcschr( lpModelElem->m_szName, _T( '_' ) ) == NULL )
		{
			_tcscpy( pszModelName, g_szRoot[ dwType ] );
			_tcscat( pszModelName, "_" );
			_tcscat( pszModelName, lpModelElem->m_szName );
		}
		else
			_tcscpy( pszModelName, lpModelElem->m_szName );
	}
	else
	{
		_tcscpy( pszModelName, g_szRoot[ dwType ] );
		_tcscat( pszModelName, "_" );
		_tcscat( pszModelName, lpModelElem->m_szName );
	}
	if( lpModelElem->m_dwModelType != MODELTYPE_SFX )
		_tcscat( pszModelName, _T( ".o3d" ) );
}
void CModelMng::MakePartsName( TCHAR* pszPartsName, LPCTSTR lpszRootName, DWORD dwIndex, int nSex )
{
	MODELELEM * lpModelElem = GetModelElem( OT_ITEM, dwIndex );
	_tcscpy( pszPartsName, lpszRootName );
	_tcscat( pszPartsName, _T( "_" ) );
	if( nSex == SEX_SEXLESS || nSex == -1 )
		_tcscat( pszPartsName, lpModelElem->m_szPart );
	else
	{
		TCHAR* pszPart2 = strchr( lpModelElem->m_szPart, '/' );
		if( pszPart2 == NULL )
			_tcscat( pszPartsName, lpModelElem->m_szPart );
		else
		{
			TCHAR szPartName[ 128 ];
			if( nSex == SEX_MALE )
			{
				_tcscpy( szPartName, lpModelElem->m_szPart );
				szPartName[ (int)pszPart2 - (int)lpModelElem->m_szPart ] = 0;
				_tcscat( pszPartsName, szPartName );
			}
			else
			{
				_tcscat( pszPartsName, pszPart2 + 1 );
			}
		}
	}
	_tcscat( pszPartsName, _T( ".o3d" ) );
}

void MODELELEM::MakeMotionName( TCHAR* pszMotionName, DWORD dwMotion ) const {
	// Set to MTI_STAND (stop state) when out of range
	if (static_cast<int>(dwMotion) >= m_nMax || dwMotion == NULL_ID) {
		dwMotion = MTI_STAND;
	}

	_tcscpy( pszMotionName, g_szRoot[ m_dwType ] );
	_tcscat( pszMotionName, "_" );

	LPCTSTR lpszMotion = &m_apszMotion[dwMotion * MotionNameLength];
	if( _tcschr( lpszMotion, '_' ) == NULL )
	{
		_tcscat( pszMotionName, m_szName );
		_tcscat( pszMotionName, "_" );
	}

	// Forced setting to MTI_STAND (stop state) in case of blank
	if( lpszMotion[0] == '\0')
	{
		dwMotion = MTI_STAND;
		lpszMotion = &m_apszMotion[MTI_STAND * MotionNameLength];
	}

	_tcscat( pszMotionName, lpszMotion );
	_tcscat( pszMotionName, ".ani" );
}

CModel * CModelMng::LoadModel( LPDIRECT3DDEVICE9 pd3dDevice, int nType, int nIndex, BOOL bParts )
{
	MODELELEM * lpModelElem = GetModelElem( nType, nIndex );
	if (lpModelElem == NULL) {
		Error("CModelMng::loadModel mdlObj/mdlDyna - objtype=%d index=%d bpart=%d has no information.", nType, nIndex, bParts);
		return nullptr;
	}

	TCHAR szFileName[ MAX_PATH ];
	MakeModelName( szFileName, nType, nIndex );
	return LoadModel( pd3dDevice, szFileName, lpModelElem, nType, bParts ).ptr; 
}


// Handler for the case where the code expects a non object CModelObject but
// the object is actually owned
//
// Here the chosen behaviour is as follows:
// - If we are the server, produce an error
// - If we are the client and a regular player, produce a error
// - If we are the client and a game master, crash
//
// We want to crash only gamemasters to not inconvinience players
static std::set<std::pair<int, int>> s_actuallyOwnedCModelObjects;
void OnActuallyOwnedCModelObject(int nType, int nIndex) {
	if (s_actuallyOwnedCModelObjects.contains(std::make_pair(nType, nIndex))) return;

	Error("CModelMng::LoadModel(%d, %d) : expected a non owned Model but the model is actually owned", nType, nIndex);

#ifdef __CLIENT
	if (g_pPlayer && g_pPlayer->IsAuthHigher(AUTH_GAMEMASTER)) {
		throw std::exception("A CModelObject is actually owned. Check the Error file");
	}
#endif

	s_actuallyOwnedCModelObjects.emplace(nType, nIndex);
}


template<typename T>
requires (sqktd::IsOneOf<T,
#ifdef __CLIENT
	std::unique_ptr<CSfxModel>,
#endif
	std::unique_ptr<CModelObject>,
	/* not owned */ CModelObject * ,
	sqktd::maybe_owned_ptr<CModelObject>
>)
T CModelMng::LoadModel(LPDIRECT3DDEVICE9 pd3dDevice, int nType, int nIndex, BOOL bParts) {
	MODELELEM * lpModelElem = GetModelElem(nType, nIndex);
	if (lpModelElem == NULL) {
		Error("CModelMng::loadModel mdlObj/mdlDyna - objtype=%d index=%d bpart=%d has no information.", nType, nIndex, bParts);
		return nullptr;
	}

	TCHAR szFileName[MAX_PATH];
	MakeModelName(szFileName, nType, nIndex);
	ModelPtrInfo info = LoadModel(pd3dDevice, szFileName, lpModelElem, nType, bParts);

	if (info.ptr == nullptr) return T(nullptr);

#ifdef __CLIENT
	if constexpr (std::same_as<T, std::unique_ptr<CSfxModel>>) {
		if (info.isOwned && info.type == ModelType::Sfx) {
			return std::unique_ptr<CSfxModel>(static_cast<CSfxModel *>(info.ptr));
		}
	}
#endif
	
	if constexpr (std::same_as<T, std::unique_ptr<CModelObject>>) {
		if (info.isOwned && info.type == ModelType::ModelObject) {
			return std::unique_ptr<CModelObject>(static_cast<CModelObject *>(info.ptr));
		}
	} else if constexpr (std::same_as<T, CModelObject *>) {
		if (info.type == ModelType::ModelObject) {
			if (info.isOwned) {
				OnActuallyOwnedCModelObject(nType, nIndex);
			}

			return static_cast<CModelObject *>(info.ptr);
		}
	} else if constexpr (std::same_as<T, sqktd::maybe_owned_ptr<CModelObject>>) {
		if (info.type == ModelType::ModelObject) {
			CModelObject * ptr = static_cast<CModelObject *>(info.ptr);
			if (info.isOwned) {
				return std::unique_ptr<CModelObject>(ptr);
			} else {
				return sqktd::maybe_owned_ptr<CModelObject>(ptr);
			}
		}
	}

	// TODO: if info.ptr is not null, should we do something? As it is likely a 
	// programing error
	if (info.isOwned && info.ptr) delete info.ptr;
	return T(nullptr);
}

#ifdef __CLIENT
template std::unique_ptr<CSfxModel>           CModelMng::LoadModel<std::unique_ptr<CSfxModel>          >(LPDIRECT3DDEVICE9 pd3dDevice, int nType, int nIndex, BOOL bParts);
#endif
template std::unique_ptr<CModelObject>        CModelMng::LoadModel<std::unique_ptr<CModelObject>       >(LPDIRECT3DDEVICE9 pd3dDevice, int nType, int nIndex, BOOL bParts);
template CModelObject *                       CModelMng::LoadModel<CModelObject *                      >(LPDIRECT3DDEVICE9 pd3dDevice, int nType, int nIndex, BOOL bParts);
template sqktd::maybe_owned_ptr<CModelObject> CModelMng::LoadModel<sqktd::maybe_owned_ptr<CModelObject>>(LPDIRECT3DDEVICE9 pd3dDevice, int nType, int nIndex, BOOL bParts);

CModelMng::ModelPtrInfo CModelMng::LoadModel( LPDIRECT3DDEVICE9 pd3dDevice, TCHAR* lpszFileName, MODELELEM * lpModelElem, int nType, BOOL bParts )
{
	const int nModelType = lpModelElem->m_dwModelType;


	switch( nModelType )
	{
		case MODELTYPE_SFX: {
#ifndef __WORLDSERVER		// The new version doesn't new sfx in the world.
			CSfxModel * pModel = new CSfxModel;
			pModel->SetModelType(nModelType);
			pModel->SetSfx(lpszFileName);
			pModel->m_pModelElem = lpModelElem;
			pModel->m_pModelElem->m_bUsed = TRUE;
			return ModelPtrInfo{ pModel, ModelType::Sfx, true };
#endif // not World
			return ModelPtrInfo{ nullptr, ModelType::Nullptr, false };
		}

		case MODELTYPE_MESH: {
			auto mapItor = m_mapFileToMesh.find( lpszFileName );
			if( mapItor != m_mapFileToMesh.end() )
			{
				mapItor->second->m_pModelElem->m_bUsed = TRUE;

#ifdef __CSC_EXTEXTURE
				CModelObject * pModel = mapItor->second;
				pModel->SetModelType( nModelType );
				pModel->m_pModelElem = lpModelElem;

				return ModelPtrInfo{ pModel, ModelType::ModelObject, true };
#else
				return ModelPtrInfo{ mapItor->second.get(), ModelType::ModelObject, false };
#endif
			}
			CModelObject * pModel = new CModelObject;
			pModel->SetModelType( nModelType );
			pModel->m_pModelElem = lpModelElem;
			HRESULT hr = pModel->InitDeviceObjects();
			hr = pModel->LoadModel( lpszFileName );
			if( hr == SUCCESS )
			{
				hr = pModel->RestoreDeviceObjects();
			#ifdef _DEBUG
				if( pModel->GetObject3D()->m_nHavePhysique )
					Error( "CModelMng::LoadModel : %s is a dynamic object, but it is set as a static object.", lpszFileName );
			#endif			
				m_mapFileToMesh.emplace(lpszFileName, pModel);
				pModel->m_pModelElem->m_bUsed = TRUE;
				return ModelPtrInfo{ pModel, ModelType::ModelObject, false };
			} else {
				SAFE_DELETE(pModel);
				return ModelPtrInfo{ nullptr, ModelType::Nullptr, false };
			}
		}
		case MODELTYPE_ANIMATED_MESH: {
			CModelObject * pModel = new CModelObject;
			pModel->SetModelType(nModelType);
			pModel->InitDeviceObjects();
			pModel->m_pModelElem = lpModelElem;
			pModel->m_pModelElem->m_bUsed = TRUE;
			TCHAR szFileName[MAX_PATH];
			std::memset(szFileName, 0, sizeof(szFileName)); // memset because we use strncpy
			_tcsncpy(szFileName, lpszFileName, _tcslen(lpszFileName) - 4);	// Remove the .o3d and copy only the file name part
			_tcscat(szFileName, _T(".chr"));
			switch (nType) {
				case OT_ITEM:	// If it is an item, load the outer copy (.chr), if any (eg wings)
				{
					CResFile resFp;
					BOOL bResult = resFp.Open(MakePath(DIR_MODEL, szFileName), "rb");
					if (bResult == TRUE)
						pModel->LoadBone(szFileName);
					break;
				}
				case OT_MOVER:	// If it is a mover, load the external copy (.chr)
				{
					pModel->LoadBone(szFileName);
					break;
				}
			}
			if (bParts == FALSE) {
				if (pModel->LoadModel(lpszFileName) == SUCCESS)  // Read skin
				{
					pModel->RestoreDeviceObjects();
				}
			}

			return ModelPtrInfo{ pModel, ModelType::ModelObject, true };
		}
		default:
			return ModelPtrInfo{ nullptr, ModelType::Nullptr, false };
	}
}

HRESULT CModelMng::InitDeviceObjects(LPDIRECT3DDEVICE9 pd3dDevice) {
	return S_OK;
}

HRESULT CModelMng::RestoreDeviceObjects(LPDIRECT3DDEVICE9 pd3dDevice) {
	for (auto & pModel : m_mapFileToMesh | std::views::values) {
		pModel->RestoreDeviceObjects();
	}

	return S_OK;
}

HRESULT CModelMng::InvalidateDeviceObjects() {
	for (auto & pModel : m_mapFileToMesh | std::views::values) {
		pModel->InvalidateDeviceObjects();
	}

	return S_OK;
}

HRESULT CModelMng::DeleteDeviceObjects() {
	for (auto & pModel : m_mapFileToMesh | std::views::values) {
		pModel->DeleteDeviceObjects();
	}
	m_mapFileToMesh.clear();
	return S_OK;
}

BOOL CModelMng::LoadScript(LPCTSTR lpszFileName) {
	CScript script;
	if (!script.Load(lpszFileName, FALSE))
		return FALSE;


	script.GetToken(); // subject or FINISHED
	while( script.tok != FINISHED )
	{
		const UINT iType = static_cast<UINT>(script.GetNumber());

		if (iType >= MAX_OBJTYPE) {
			Error("Tried to load a model with iType = %u >= MAX_OBJTYPE(%d)", iType, MAX_OBJTYPE);
			throw std::exception("Bad model loading");
		}

		CFixedArray<MODELELEM> & apModelElem = m_aaModelElem[iType];
		script.GetToken(); // {
		script.GetToken(); // object name or }
		int nBrace = 1;

		// Read objects in file
		while( nBrace )
		{
			if( *script.token == '}' ) 
			{
				nBrace--;
				if( nBrace > 0 )
				{
					script.GetToken(); // object name or }
					continue;
				}
				if( nBrace == 0 )
					continue;
			}
		#ifdef _DEBUG
			if( sizeof(szObject) <= strlen(script.token) + 1 )
				Error( "%s name is too long. max = %zu", lpszFileName, strlen(script.token) );
		#endif

			TCHAR szObject[48];
			_tcscpy( szObject, script.token ); // Folder or object name

			script.SetMark();
			
			script.GetToken(); // {
			if( *script.token == '{' )
			{
				nBrace++;
				script.GetToken(); // object name or }
			#ifdef _DEBUG
				if( sizeof(szObject) <= strlen(script.token) + 1 )
					Error( "%s name is too long. max = %zu", lpszFileName, strlen(script.token) );
			#endif

				_tcscpy( szObject, script.token );
				continue;
			}

			script.GoMark();
			const UINT iObject = script.GetNumber();
			if( iObject == 0 )
			{
				CString str;
				str.Format( "CModelMng::LoadScript(%d) Motion ID specified as 0 : %s, %s", script.GetLineNum(), szObject, script.token );
				AfxMessageBox( str );
			}

			MODELELEM modelElem;
			ZeroMemory(&modelElem, sizeof(modelElem));
			modelElem.m_dwType = iType;
			modelElem.m_dwIndex = iObject;
		#ifdef _DEBUG
			if( sizeof(modelElem.m_szName) <= strlen(szObject) + 1 )
				Error( "%s string is too long. max = %zu", lpszFileName, strlen(szObject) );
		#endif
			_tcscpy( modelElem.m_szName, szObject );
			modelElem.m_dwModelType = script.GetNumber();
			script.GetToken();
		#ifdef _DEBUG
			if( sizeof(modelElem.m_szPart) <= strlen(script.token) + 1 )
				Error( "%s string is too long. max = %zu", lpszFileName, strlen(script.token) );
		#endif
			_tcscpy( modelElem.m_szPart, script.Token );
			modelElem.m_bFly = script.GetNumber();
			modelElem.m_dwDistant = script.GetNumber();
			modelElem.m_bPick = script.GetNumber();
			modelElem.m_fScale = script.GetFloat();
			modelElem.m_bTrans = script.GetNumber();
			modelElem.m_bShadow = script.GetNumber();			
			modelElem.m_bReserved = 0;
			modelElem.m_nTextureEx = script.GetNumber();
			modelElem.m_bRenderFlag = script.GetNumber();

			script.GetToken(); // object name or { or }
			// Animations
			if( *script.token == '{' )
			{
				script.SetMark();
				script.GetToken(); // motion name or }
				int nMax = 0;
				// Motion list count
				while( *script.token != '}' )
				{
					TCHAR szMotion[48];
				#ifdef _DEBUG
					if( sizeof(szMotion) <= strlen(script.token) + 1 )
						Error( "%s string is too long. max = %d", lpszFileName, strlen(script.token) );
				#endif
					_tcscpy( szMotion, script.token );
					const UINT iMotion = script.GetNumber();
					if( (int)( iMotion ) > nMax )
						nMax = iMotion;
					script.GetToken(); // motion name or }
				}
				nMax++;
				script.GoMark();
				// Actual motion list setting
				script.GetToken(); // motion name or }
				modelElem.m_apszMotion = new TCHAR[ nMax * MODELELEM::MotionNameLength];
				modelElem.m_nMax = nMax;
				ZeroMemory( modelElem.m_apszMotion, sizeof( TCHAR ) * nMax * MODELELEM::MotionNameLength);
				//TRACE( " %s %p\n", modelElem.m_szName, modelElem.m_apszMotion);
				while( *script.token != '}' )
				{
					TCHAR szMotion[MODELELEM::MotionNameLength];
				#ifdef _DEBUG
					if( sizeof(szMotion) <= strlen(script.token) + 1 )
						Error( "%s string is too long. size = %zu / max = %zu", lpszFileName, strlen(script.token), MODELELEM::MotionNameLength);
				#endif
					_tcscpy( szMotion, script.token );
					const UINT iMotion = script.GetNumber();
					TCHAR* lpszMotion = modelElem.GetMotion( iMotion );
					if( lpszMotion[0] )
					{
						CString str;
						str.Format( "CModelMng::LoadScript(%d) %s Motion Duplicate ID : %s", script.GetLineNum(), lpszFileName, lpszMotion );
						AfxMessageBox( str );
					}
					_tcscpy( lpszMotion, szMotion );
					script.GetToken(); // motion name or }
				}
				script.GetToken(); // object name or }
			}
			if( apModelElem.GetAt( iObject ) )
			{
				CString str;
				str.Format( "CModelMng::LoadScript(%d) %s duplicate id : type = %d, idx = %d, name = %s", script.GetLineNum(), lpszFileName, iType, iObject, modelElem.m_szName );
				AfxMessageBox( str );
			}

//////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
			
		#ifdef __WORLDSERVER
			if( iType != OT_SFX )	// Sfx skip from server
		#endif
				apModelElem.SetAtGrow(iObject, modelElem);

		} // while( nBrace )

		script.GetToken(); // type name or }
		apModelElem.Optimize();
	}	// while( script.tok != FINISHED )

	return TRUE;
}


void CModelMng::DestroyUnusedModels() {
	// Destroy unused models.
	// Only static things should come in here. Anything dynamic (using skinning)
	// shouldn't be destroyed here.
	auto itor = m_mapFileToMesh.begin();
	while (itor != m_mapFileToMesh.end()) {
		std::unique_ptr<CModelObject> & pModel = itor->second;
		pModel->DeleteDeviceObjects();
		if (pModel->m_pModelElem->m_bUsed == FALSE && pModel->m_pModelElem->m_dwType != OT_ITEM) {
			itor = prj.m_modelMng.m_mapFileToMesh.erase(itor);
		} else {
			pModel->m_pModelElem->m_bUsed = FALSE;
			++itor;
		}
	}
}
