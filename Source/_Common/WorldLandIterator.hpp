#pragma once

#if !(defined(__WORLDSERVER) || defined(__CLIENT))
static_assert(false, "WorldLandIterator.hpp may only be included in WorldServer and Neuz")
#endif

///////////////////////////////////////////////////////////////////////////////
//// FOR LAND RANGE

namespace useless_impl {
namespace world {
	template<LinkType dwLinkType, typename CObjSpec> class LandIterator;
	template<LinkType dwLinkType> class LinkLevelIterator;
	template<LinkType dwLinkType> class AreaIterator;
	template<LinkType dwLinkType> class ObjIterator;

	template<LinkType dwLinkType>
	class LinkLevelIterator {
	private:
		CWorld * m_pWorld;
		D3DXVECTOR3 m_vPos;
		int m_nRange;
		int m_nLayer;
		int m_nLinkX;
		int m_nLinkZ;
		int m_linkLevel;

	public:
		LinkLevelIterator(CWorld * pWorld, const D3DXVECTOR3 & vPos, int nRange, int nLayer);
		[[nodiscard]] bool operator==(CWorld::Iterators::Sentinel) const;
		LinkLevelIterator & operator++();

		friend ObjIterator<dwLinkType>;
		friend AreaIterator<dwLinkType>;
	};

	template<LinkType dwLinkType>
	class AreaIterator {
		int m_x, m_z;
		int m_nLinkXMin, m_nLinkXMax;
		int m_nLinkZMin, m_nLinkZMax;
		int m_nMaxWidth;
#ifdef __WORLDSERVER
		CObj ** m_pObjs;
#else
		int nWidthLink;
#endif

	public:
		AreaIterator(const LinkLevelIterator<dwLinkType> & link);
		[[nodiscard]] bool operator==(CWorld::Iterators::Sentinel) const;
		AreaIterator & operator++();

		friend class ObjIterator<dwLinkType>;
	};

	template<LinkType dwLinkType>
	class ObjIterator {
		CObj * m_pObj;
#ifdef __WORLDSERVER
		int limit = 1000;
#endif

	public:
		ObjIterator(
			const LinkLevelIterator<dwLinkType> & link,
			const AreaIterator<dwLinkType> & area
		);

		[[nodiscard]] bool operator==(CWorld::Iterators::Sentinel) const;
		ObjIterator & operator++();
		[[nodiscard]] CObj * operator*() const { return m_pObj; }
	};

	template<LinkType dwLinkType, typename CObjSpec>
	class LandIterator {
		LinkLevelIterator<dwLinkType> m_link;
		AreaIterator<dwLinkType> m_area;
		ObjIterator<dwLinkType> m_obj;

	public:
		LandIterator(CWorld * pWorld, const D3DXVECTOR3 & vPos, int nRange, int nLayer);
		[[nodiscard]] bool operator==(CWorld::Iterators::Sentinel sentinel) const;
		CObjSpec * operator*() { return reinterpret_cast<CObjSpec *>(*m_obj); }
		LandIterator & operator++();
	};

	template<LinkType dwLinkType, typename ObjSpec>
	inline auto GetLandRange_(
		CWorld * pWorld,
		const D3DXVECTOR3 & vPos,
		int nRange, int nLayer
	) {
		struct Range {
			LandIterator<dwLinkType, ObjSpec> begin_;

			explicit Range(LandIterator<dwLinkType, ObjSpec> begin_) : begin_(begin_) {}

			[[nodiscard]] auto begin() const { return begin_; }
			[[nodiscard]] CWorld::Iterators::Sentinel end() const { return CWorld::Iterators::Sentinel{}; }
		};

		return Range(LandIterator<dwLinkType, ObjSpec>(pWorld, vPos, nRange, nLayer));
	}

#pragma region LandIterator Implementation
	template<LinkType dwLinkType, typename CObjSpec>
	LandIterator<dwLinkType, CObjSpec>::LandIterator(
		CWorld * pWorld, const D3DXVECTOR3 & vPos, int nRange, int nLayer
	)
		: m_link(pWorld, vPos, nRange, nLayer)
		, m_area(m_link)
		, m_obj(m_link, m_area)
	{

	}

	template<LinkType dwLinkType, typename CObjSpec>
	bool LandIterator<dwLinkType, CObjSpec>::operator==(CWorld::Iterators::Sentinel sentinel) const {
		return m_obj == sentinel;
	}

	template<LinkType dwLinkType, typename CObjSpec>
	LandIterator<dwLinkType, CObjSpec> & LandIterator<dwLinkType, CObjSpec>::operator++() {
		++m_obj;

		while (m_obj == CWorld::Iterators::Sentinel{}) {
			// Go to next area
			++m_area;

			while (m_area == CWorld::Iterators::Sentinel{}) {
				// If area empty, go to next linklevel
				++m_link;

				if (m_link == CWorld::Iterators::Sentinel{}) {
					// No more link level
					return *this;
				}

				// On next link
				m_area = AreaIterator<dwLinkType>(m_link);
				// This link level may have no area, so we loop
			}

			// On next area
			m_obj = ObjIterator<dwLinkType>(m_link, m_area);
			// There may be no object in this area, so we loop
		}

		return *this;
	}
#pragma endregion


#pragma region LinkLevelIterator Implementation
	template<LinkType dwLinkType>
	LinkLevelIterator<dwLinkType>::LinkLevelIterator(
		CWorld * pWorld, const D3DXVECTOR3 & vPos, int nRange, int nLayer
	) {
		m_pWorld = pWorld;
		m_vPos = vPos;
		m_nRange = nRange;
		m_nLayer = nLayer;

		m_nLinkX = (int)(m_vPos.x / m_pWorld->m_iMPU);
		m_nLinkZ = (int)(m_vPos.z / m_pWorld->m_iMPU);

		m_linkLevel = 0;
	}

	template<LinkType dwLinkType>
	[[nodiscard]] bool LinkLevelIterator<dwLinkType>::operator==(CWorld::Iterators::Sentinel) const {
#ifdef __WORLDSERVER
		return m_linkLevel < m_pWorld->m_linkMap.GetMaxLinkLevel(dwLinkType, m_nLayer);
#else
		return m_linkLevel < MAX_LINKLEVEL;
#endif
	}

	template<LinkType dwLinkType>
	LinkLevelIterator<dwLinkType> & LinkLevelIterator<dwLinkType>::operator++() {
		++m_linkLevel;
		return *this;
	}
#pragma endregion

#pragma region AreaIterator Implementation
	template<LinkType dwLinkType>
	AreaIterator<dwLinkType>::AreaIterator(const LinkLevelIterator<dwLinkType> & link) {
		if (link == CWorld::Iterators::Sentinel{}) {
			m_nLinkXMin = m_nLinkXMax = m_nLinkZMin = m_nLinkZMax = 0;
			m_x = m_z = 1;
			m_nMaxWidth = 0;
#ifdef __WORLDSERVER
			m_pObjs = nullptr;
#endif
			return;
		}

		CWorld * pWorld = link.m_pWorld;

#ifdef __WORLDSERVER
		m_pObjs = pWorld->m_linkMap.GetObj(dwLinkType, link.m_linkLevel, link.m_nLayer);
#endif

#ifdef __WORLDSERVER
		const int nWidthLink = pWorld->m_linkMap.GetLinkWidth(dwLinkType, link.m_linkLevel, link.m_nLayer);
#else // __CLIENT
		nWidthLink = CLandscape::m_nWidthLinkMap[link.m_linkLevel];
#endif
		m_nMaxWidth = nWidthLink * pWorld->m_nLandWidth;
		const int nMaxHeight = nWidthLink * pWorld->m_nLandHeight;
		const int nUnit = (MAP_SIZE * pWorld->m_nLandWidth) / m_nMaxWidth;
		const int d = nUnit * pWorld->m_iMPU / 2;
		int nX = (link.m_nLinkX / nUnit) * nUnit * pWorld->m_iMPU;
		int nZ = (link.m_nLinkZ / nUnit) * nUnit * pWorld->m_iMPU;
		nX = ((int)(link.m_vPos.x) - nX > d) ? 1 : 0;
		nZ = ((int)(link.m_vPos.z) - nZ > d) ? 1 : 0;

		m_nLinkXMin = std::max(((link.m_nLinkX - link.m_nRange) / nUnit) + (nX - 1), 0);
		m_nLinkZMin = std::max(((link.m_nLinkZ - link.m_nRange) / nUnit) + (nZ - 1), 0);
		m_nLinkXMax = std::min(((link.m_nLinkX + link.m_nRange) / nUnit) + nX, m_nMaxWidth - 1);
		m_nLinkZMax = std::min(((link.m_nLinkZ + link.m_nRange) / nUnit) + nZ, nMaxHeight - 1);

		m_x = m_nLinkXMin;
		m_z = m_nLinkZMin;

		if (m_nLinkXMin > m_nLinkXMax) m_z = m_nLinkZMax + 1;
	}

	template<LinkType dwLinkType>
	[[nodiscard]] bool AreaIterator<dwLinkType>::operator==(CWorld::Iterators::Sentinel) const {
		return m_z > m_nLinkZMax;
	}

	template<LinkType dwLinkType>
	AreaIterator<dwLinkType> & AreaIterator<dwLinkType>::operator++() {
		if (m_x < m_nLinkXMax) {
			++m_x;
		} else {
			m_x = m_nLinkXMin;
			++m_z;
		}
		return *this;
	}


#pragma endregion

#pragma region ObjIterator Implementation
	template<LinkType dwLinkType>
	ObjIterator<dwLinkType>::ObjIterator(
		const LinkLevelIterator<dwLinkType> & link,
		const AreaIterator<dwLinkType> & area
	) {
		if (area == CWorld::Iterators::Sentinel{}) {
			m_pObj = nullptr;
			return;
		}

#ifdef __WORLDSERVER
		const int nPos = area.m_z * area.m_nMaxWidth + area.m_x;
		CObj * pObj = area.m_pObjs[nPos];
#else
		CLandscape * pLand = link.m_pWorld->m_apLand[(area.m_z / area.nWidthLink) * link.m_pWorld->m_nLandWidth + (area.m_x / area.nWidthLink)];
		if (pLand) {
			CObj ** pObjs = pLand->GetObjLink(dwLinkType, link.m_linkLevel);
			ASSERT(_pObjs);
			const int nPos = (area.m_z % area.nWidthLink) * area.nWidthLink + (area.m_x % area.nWidthLink);
			m_pObj = pObjs[nPos];

			while (!IsValidObj(m_pObj)) {
				m_pObj = m_pObj->GetNextNode();
			}
		} else {
			m_pObj = nullptr;
		}
#endif
	}

	template<LinkType dwLinkType>
	[[nodiscard]] bool ObjIterator<dwLinkType>::operator==(CWorld::Iterators::Sentinel) const {
		return !m_pObj;
	}

	template<LinkType dwLinkType>
	ObjIterator<dwLinkType> & ObjIterator<dwLinkType>::operator++() {
		m_pObj = m_pObj->GetNextNode();

#ifdef __WORLDSERVER
		--limit;
		if (limit == 0) m_pObj = nullptr;
#endif

#ifdef __CLIENT
		while (m_pObj && !IsValidObj(m_pObj)) {
			m_pObj = m_pObj->GetNextNode();
		}
#endif

		return *this;
	}
#pragma endregion
}
}



#define GetLandRangeMostSpecializedKnownType(dwLinkType) \
	std::conditional_t< \
		std::is_convertible_v<CObjSpecialization<dwLinkType> *, CObj *>, \
		CObjSpecialization<dwLinkType>, \
		CObj \
	>

#ifdef __WORLDSERVER
#define GetLandRange(pWorld, vPos, nRange, dwLinkType, nLayer) useless_impl::world::GetLandRange_<dwLinkType, GetLandRangeMostSpecializedKnownType(dwLinkType)>(pWorld, vPos, nRange, nLayer)
#else
#define GetLandRange(pWorld, vPos, nRange, dwLinkType) useless_impl::world::GetLandRange_<dwLinkType, GetLandRangeMostSpecializedKnownType(dwLinkType)>(pWorld, vPos, nRange, 0)
#define GetLandRange(pWorld, vPos, nRange, dwLinkType, nLayer) useless_impl::world::GetLandRange_<dwLinkType, GetLandRangeMostSpecializedKnownType(dwLinkType)>(pWorld, vPos, nRange, 0)
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


