#pragma once

#if !(defined(__WORLDSERVER) || defined(__CLIENT))
static_assert(false, "WorldLandIterator.hpp may only be included in WorldServer and Neuz")
#endif

///////////////////////////////////////////////////////////////////////////////
//// FOR LAND RANGE


template<LinkType dwLinkType>
inline auto GetLandRange_(
	CWorld * pWorld,
	const D3DXVECTOR3 & vPos,
	int nRange, int nLayer
) {
	struct Empty {
		bool operator==(CWorld::Iterators::Sentinel) { return true; }
		Empty & operator++() { return *this; }
		CObj * operator*() { return nullptr; }
	};

	struct Range {
		// WImpl::LandIterator<dwLinkType> begin_;

		// auto begin() { return begin_; }


		Empty begin() { return Empty{}; }
		CWorld::Iterators::Sentinel end() { return CWorld::Iterators::Sentinel{}; }
	};

	return Range{};
}

#ifdef __WORLDSERVER
#define GetLandRange(pWorld, vPos, nRange, dwLinkType, nLayer) GetLandRange_<dwLinkType>(pWorld, vPos, nRange, nLayer)
#else
#define GetLandRange(pWorld, vPos, nRange, dwLinkType, nLayer) GetLandRange_<dwLinkType>(pWorld, vPos, nRange, 0)
#endif




///////////////////////////////////////////////////////////////////////////////
//// Legacy useless ForLinkMap function

template<LinkType dwLinkType>
void CWorld::ForLinkMap(
	const D3DXVECTOR3 & vPos,
	int nRange, int nLayer,
	auto && consumer
) {
	using ObjSpec =
		std::conditional_t<
			std::is_convertible_v<CObjSpecialization<dwLinkType> *, CObj *>,
			CObjSpecialization<dwLinkType>,
			CObj
		>;
	using ReturnType = std::invoke_result_t<decltype(consumer), ObjSpec *>;
	
	const int _nLinkX = (int)( vPos.x / m_iMPU );
	const int _nLinkZ = (int)( vPos.z / m_iMPU );

#ifdef __WORLDSERVER	
	for (int i = 0; i < m_linkMap.GetMaxLinkLevel(dwLinkType, nLayer); i++) {
		CObj ** _pObjs = m_linkMap.GetObj(dwLinkType, i, nLayer);
		ASSERT(_pObjs);

		const int nWidthLink = m_linkMap.GetLinkWidth(dwLinkType, i, nLayer);
#else // __CLIENT
	for (int i = 0; i < MAX_LINKLEVEL; i++) {
		const int nWidthLink = CLandscape::m_nWidthLinkMap[i];
#endif

		const int nMaxWidth = nWidthLink * m_nLandWidth;
		const int nMaxHeight = nWidthLink * m_nLandHeight;
		const int nUnit = (MAP_SIZE * m_nLandWidth) / nMaxWidth;
		const int d = nUnit * m_iMPU / 2;
		int nX = (_nLinkX / nUnit) * nUnit * m_iMPU;
		int nZ = (_nLinkZ / nUnit) * nUnit * m_iMPU;
		nX = ((int)(vPos.x) - nX > d) ? 1 : 0;
		nZ = ((int)(vPos.z) - nZ > d) ? 1 : 0;

		const int nLinkXMin = std::max(((_nLinkX - nRange) / nUnit) + (nX - 1), 0);
		const int nLinkZMin = std::max(((_nLinkZ - nRange) / nUnit) + (nZ - 1), 0);
		const int nLinkXMax = std::min(((_nLinkX + nRange) / nUnit) + nX, nMaxWidth - 1);
		const int nLinkZMax = std::min(((_nLinkZ + nRange) / nUnit) + nZ, nMaxHeight - 1);

		for (int z = nLinkZMin; z <= nLinkZMax; z++) {
			for (int x = nLinkXMin; x <= nLinkXMax; x++) {
#ifdef __WORLDSERVER
				const int nPos = z * nMaxWidth + x;
				CObj * pObj = _pObjs[nPos];
				int _limit = 1000;
				while (pObj && _limit--) {
#else
				CLandscape * pLand = m_apLand[(z / nWidthLink) * m_nLandWidth + (x / nWidthLink)];
				if (pLand) {
					CObj ** pObjs = pLand->GetObjLink(dwLinkType, i);
					ASSERT(_pObjs);
					const int nPos = (z % nWidthLink) * nWidthLink + (x % nWidthLink);
					CObj * pObj = pObjs[nPos];
					while (pObj) {
						if (IsValidObj(pObj)) {
#endif

							if constexpr (std::is_same_v<void, ReturnType>) {
								consumer(reinterpret_cast<ObjSpec *>(pObj));
							} else {
								const bool stop = consumer(reinterpret_cast<ObjSpec *>(pObj));
								if (stop) return;
							}

#ifdef __WORLDSERVER
					pObj = pObj->GetNextNode();
#else
						} // IsValidObj

						pObj = pObj->GetNextNode();
					}
#endif
				}
			}
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
//// Legacy v15 FOR_LINKMAP / END_LINKMAP macros

#ifdef __WORLDSERVER	// _nRadiusLink
#ifdef __LAYER_1015
	#define FOR_LINKMAP( _pWorld, _vPos, _pObj, _nRange, _dwLinkType, _nLayer ) { \
			int _nLinkX = (int)( _vPos.x / _pWorld->m_iMPU );	\
			int _nLinkZ = (int)( _vPos.z / _pWorld->m_iMPU );	\
			int _nWidthLink, _nMaxWidth;	\
			int _nMaxHeight;	\
			int _nUnit;	\
			int _nLinkXMin, _nLinkZMin, _nLinkXMax, _nLinkZMax;	\
			int _nX, _nZ;	\
			int _d;	\
			int _nPos;	\
			for( int _i = 0; _i < (_pWorld)->m_linkMap.GetMaxLinkLevel( _dwLinkType, _nLayer ); _i++ ) {	\
				_nWidthLink		= (_pWorld)->m_linkMap.GetLinkWidth( _dwLinkType, _i, _nLayer );	\
				CObj** _pObjs	= (_pWorld)->m_linkMap.GetObj( _dwLinkType, _i, _nLayer );	\
				ASSERT( _pObjs );	\
				_nMaxWidth	= _nWidthLink * (_pWorld)->m_nLandWidth ;	\
				_nMaxHeight	= _nWidthLink * (_pWorld)->m_nLandHeight;	\
				_nUnit	=  ( MAP_SIZE * (_pWorld)->m_nLandWidth ) / _nMaxWidth;	\
				_nX		= ( _nLinkX / _nUnit ) * _nUnit * _pWorld->m_iMPU;	\
				_nZ		= ( _nLinkZ / _nUnit ) * _nUnit * _pWorld->m_iMPU;	\
				_d		= _nUnit * _pWorld->m_iMPU / 2;	\
				_nX		= ( (int)( _vPos.x ) - _nX > _d )? 1: 0;	\
				_nZ		= ( (int)( _vPos.z ) - _nZ > _d )? 1: 0;	\
				_nLinkXMin = ( ( _nLinkX - _nRange ) / _nUnit ) + ( _nX - 1 );	if( _nLinkXMin < 0 ) _nLinkXMin = 0;	\
				_nLinkZMin = ( ( _nLinkZ - _nRange ) / _nUnit ) + ( _nZ - 1 );	if( _nLinkZMin < 0 ) _nLinkZMin = 0;	\
				_nLinkXMax = ( ( _nLinkX + _nRange ) / _nUnit ) + _nX;	if( _nLinkXMax >= _nMaxWidth  ) _nLinkXMax = _nMaxWidth - 1;	\
				_nLinkZMax = ( ( _nLinkZ + _nRange ) / _nUnit ) + _nZ;	if( _nLinkZMax >= _nMaxHeight ) _nLinkZMax = _nMaxHeight - 1;	\
				for( int _j = _nLinkZMin; _j <= _nLinkZMax; _j++ )	\
				{	\
					for( int _k = _nLinkXMin; _k <= _nLinkXMax; _k++ ) \
					{	\
						_nPos = _j * _nMaxWidth + _k;	\
						_pObj = _pObjs[ _nPos ];	\
						int __limit = 1000;  \
						while( _pObj && __limit-- ) { 
#else	// __LAYER_1015
	#define FOR_LINKMAP( _pWorld, _vPos, _pObj, _nRange, _dwLinkType, _nLayer ) { \
			int _nLinkX = (int)( _vPos.x / _pWorld->m_iMPU );	\
			int _nLinkZ = (int)( _vPos.z / _pWorld->m_iMPU );	\
			int _nWidthLink, _nMaxWidth;	\
			int _nMaxHeight;	\
			int _nUnit;	\
			int _nLinkXMin, _nLinkZMin, _nLinkXMax, _nLinkZMax;	\
			int _nX, _nZ;	\
			int _d;	\
			int _nPos;	\
			for( int _i = 0; _i < (_pWorld)->m_linkMap.GetMaxLinkLevel( _dwLinkType ); _i++ ) {	\
				_nWidthLink		= (_pWorld)->m_linkMap.GetLinkWidth( _dwLinkType, _i);	\
				CObj** _pObjs	= (_pWorld)->m_linkMap.GetObj( _dwLinkType, _i );	\
				ASSERT( _pObjs );	\
				_nMaxWidth	= _nWidthLink * (_pWorld)->m_nLandWidth ;	\
				_nMaxHeight	= _nWidthLink * (_pWorld)->m_nLandHeight;	\
				_nUnit	=  ( MAP_SIZE * (_pWorld)->m_nLandWidth ) / _nMaxWidth;	\
				_nX		= ( _nLinkX / _nUnit ) * _nUnit * _pWorld->m_iMPU;	\
				_nZ		= ( _nLinkZ / _nUnit ) * _nUnit * _pWorld->m_iMPU;	\
				_d		= _nUnit * _pWorld->m_iMPU / 2;	\
				_nX		= ( (int)( _vPos.x ) - _nX > _d )? 1: 0;	\
				_nZ		= ( (int)( _vPos.z ) - _nZ > _d )? 1: 0;	\
				_nLinkXMin = ( ( _nLinkX - _nRange ) / _nUnit ) + ( _nX - 1 );	if( _nLinkXMin < 0 ) _nLinkXMin = 0;	\
				_nLinkZMin = ( ( _nLinkZ - _nRange ) / _nUnit ) + ( _nZ - 1 );	if( _nLinkZMin < 0 ) _nLinkZMin = 0;	\
				_nLinkXMax = ( ( _nLinkX + _nRange ) / _nUnit ) + _nX;	if( _nLinkXMax >= _nMaxWidth  ) _nLinkXMax = _nMaxWidth - 1;	\
				_nLinkZMax = ( ( _nLinkZ + _nRange ) / _nUnit ) + _nZ;	if( _nLinkZMax >= _nMaxHeight ) _nLinkZMax = _nMaxHeight - 1;	\
				for( int _j = _nLinkZMin; _j <= _nLinkZMax; _j++ )	\
				{	\
					for( int _k = _nLinkXMin; _k <= _nLinkXMax; _k++ ) \
					{	\
						_nPos = _j * _nMaxWidth + _k;	\
						_pObj = _pObjs[ _nPos ];	\
						int __limit = 1000;  \
						while( _pObj && __limit-- ) { 
#endif	// __LAYER_1015
	#define END_LINKMAP	\
		pObj = pObj->GetNextNode(); } } } } }

#else	// __WORLDSERVER
	#define FOR_LINKMAP( _pWorld, _vPos, _pObj, _nRange, _dwLinkType, _nLayer ) { \
		int _nLinkX = (int)( _vPos.x / _pWorld->m_iMPU );	\
		int _nLinkZ = (int)( _vPos.z / _pWorld->m_iMPU );	\
		int _nWidthLink;	\
		int _nMaxWidth, _nMaxHeight;	\
		int _nUnit;		\
		int _nLinkXMin, _nLinkZMin, _nLinkXMax, _nLinkZMax;	\
		int _nX, _nZ;	\
		int _d;		\
		int _nPos;	\
		for( int _i = 0; _i < MAX_LINKLEVEL; _i++ ) {	\
			_nWidthLink	= CLandscape::m_nWidthLinkMap[ _i ];	\
			_nMaxWidth	= _nWidthLink * (_pWorld)->m_nLandWidth;	\
			_nMaxHeight		= _nWidthLink * (_pWorld)->m_nLandHeight;	\
			_nUnit	= ( MAP_SIZE * (_pWorld)->m_nLandWidth ) / _nMaxWidth;	\
			_nX		= ( _nLinkX / _nUnit ) * _nUnit * _pWorld->m_iMPU;	\
			_nZ		= ( _nLinkZ / _nUnit ) * _nUnit * _pWorld->m_iMPU;	\
			_d		= _nUnit * _pWorld->m_iMPU / 2;		\
			_nX		= ( (int)( _vPos.x ) - _nX > _d )? 1: 0;		\
			_nZ		= ( (int)( _vPos.z ) - _nZ > _d )? 1: 0;		\
			_nLinkXMin = ( ( _nLinkX - _nRange ) / _nUnit ) + ( _nX - 1 );	if( _nLinkXMin < 0 ) _nLinkXMin = 0;	\
			_nLinkZMin = ( ( _nLinkZ - _nRange ) / _nUnit ) + ( _nZ - 1 );	if( _nLinkZMin < 0 ) _nLinkZMin = 0;	\
			_nLinkXMax = ( ( _nLinkX + _nRange ) / _nUnit ) + _nX;	if( _nLinkXMax >= _nMaxWidth  ) _nLinkXMax = _nMaxWidth - 1;	\
			_nLinkZMax = ( ( _nLinkZ + _nRange ) / _nUnit ) + _nZ;	if( _nLinkZMax >= _nMaxHeight ) _nLinkZMax = _nMaxHeight - 1;	\
			for( int _j = _nLinkZMin; _j <= _nLinkZMax; _j++ ) \
			{ \
				for( int _k = _nLinkXMin; _k <= _nLinkXMax; _k++ ) \
				{ \
					CLandscape* _pLand \
						= (_pWorld)->m_apLand[ ( _j / _nWidthLink ) * (_pWorld)->m_nLandWidth + ( _k / _nWidthLink ) ]; \
					if( _pLand ) \
					{ \
						_nPos = ( _j % _nWidthLink ) * _nWidthLink + ( _k % _nWidthLink ); \
	 					CObj** _pObjs = _pLand->GetObjLink( _dwLinkType, _i );	\
						ASSERT( _pObjs ); \
						_pObj = _pObjs[ _nPos ]; \
						while( _pObj ) { \
							if( IsValidObj( pObj ) )  

	#define END_LINKMAP pObj = pObj->GetNextNode(); } } } } } }


#endif	// __WORLDSERVER


