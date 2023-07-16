#pragma once

#ifdef __CLIENT
static constexpr int nDefaultLayer = 0;
#endif

enum class LinkType : std::uint8_t {
	/* CObj  */ Static = 0,
	/* CCtrl */ Dynamic = 1,
	/* CUser */ Player = 2,
	/* CShip */ AirShip = 3
};
static constexpr std::underlying_type_t<LinkType> MAX_LINKTYPE = 4;

class CObj;
class CMover;
#ifdef __WORLDSERVER
class CUser;
#else if defined(__CLIENT)
using CUser = CMover;
#endif
class CShip;
class CCtrl;

namespace useless_impl {
	template<LinkType linkType>
	struct CObjSpecialization {
		using type = CObj;
	};

	template<>
	struct CObjSpecialization<LinkType::Player> {
		using type = CUser;
	};

	template<>
	struct CObjSpecialization<LinkType::Dynamic> {
		// CMover or CCommonCtrl or CSfx or CItem
		using type = CCtrl;
	};

	template<>
	struct CObjSpecialization<LinkType::AirShip> {
		using type = CShip;
	};
}

/// The most specialized CObj type shared by all objs with the given linkmap
template<LinkType linkType>
using CObjSpecialization = useless_impl::CObjSpecialization<linkType>::type;

