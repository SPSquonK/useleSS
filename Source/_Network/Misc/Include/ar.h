#pragma once

#include "FlyFFTypes.h"
#include "StaticString.h"
#include <boost/container/static_vector.hpp>
#include <variant>

namespace ArHelper {
	template<typename T>
	concept UnitTypes =
		std::same_as<T, BYTE>
		|| std::same_as<T, WORD>
		|| std::same_as<T, LONG>
		|| std::same_as<T, DWORD>
		|| std::same_as<T, float>
		|| std::same_as<T, double>
		|| std::same_as<T, int>
		|| std::same_as<T, short>
		|| std::same_as<T, char>
		|| std::same_as<T, unsigned int>
		|| std::same_as<T, __int64>
		|| std::same_as<T, bool>;
}


class CAr final {
public:
	CAr(void * lpBuf = nullptr, u_int nBufSize = 0);
	~CAr();

	CAr(const CAr &) = delete;
	CAr & operator=(const CAr &) = delete;

// Flag values
	enum class Mode { store = 0, load = 1 };
	enum class GoToOffsetAnswer { SamePlace, NotAllConsumed, TooFar };
	static constexpr size_t nGrowSize = 16 * 1024;

// Attributes
	BOOL	IsLoading()	const;
	BOOL	IsStoring()	const;

// Operations
	void	Read( void* lpBuf, u_int nSize );
	void	Write( const void* lpBuf, u_int nSize );
	void	CheckBuf( u_int nSize );
	void	Reserve( u_int nSize );
	void	Flush( void );
	void	ReelIn( u_int uOffset );

	void IsUnsafe() { /* TODO: throw if this is a CAr received from client */ }

	template<typename T> class PushBacker;
	template<typename T> PushBacker<T> PushBack(const T & value);

	// reading and writing strings


	LPBYTE	GetBuffer( int* pnBufSize );
	u_long	GetOffset( void );

	// -- Trivial Types
	template<ArHelper::UnitTypes Type> CAr & operator<<(Type value);
	template<ArHelper::UnitTypes Type> CAr & operator>>(Type & value);
	template<sqktd::Archivable Archivable> CAr & operator<<(const Archivable & archivable);
	template<sqktd::Archivable Archivable> CAr & operator>>(Archivable & archivable);

	// -- Strings
	void WriteString(LPCTSTR lpsz);
	LPTSTR ReadString(LPTSTR lpsz, int nBufSize);
	template<size_t N> LPTSTR ReadString(char(&buffer)[N]) { return ReadString(buffer, N); }

	template<size_t N> CAr & operator<<(const char(&buffer)[N]) requires (N >= 3);
	template<size_t N> CAr & operator>>(char(&buffer)[N]) requires (N >= 3);

	// -- Avoid implicit cast of pointers to an integer type when sending
	template<typename T> requires (std::is_pointer_v<T>) CAr & operator<<(T t) = delete;

	// -- Enums
	template <typename E> CAr & operator<<(E e) requires (std::is_enum_v<E>);
	template <typename E> CAr & operator>>(E & e) requires (std::is_enum_v<E>);

	// -- Containers
	template<typename T, size_t N> CAr & operator<<(const std::array<T, N> & values);
	template<typename T, size_t N> CAr & operator>>(std::array<T, N> & values);
	template<typename T, size_t N> CAr & operator<<(const boost::container::static_vector<T, N> & values);
	template<typename T, size_t N> CAr & operator>>(boost::container::static_vector<T, N> & values);

	template<typename ... Ts> friend CAr & operator<<(CAr & ar, const std::variant<Ts ...> & variant);
	template<typename ... Ts> friend CAr & operator>>(CAr & ar, std::variant<Ts ...> & variant);
	
	template<sqktd::EmptyArchivable EmptyArchivable> CAr & operator<<(const EmptyArchivable & e) { return *this; }
	template<sqktd::EmptyArchivable EmptyArchivable> CAr & operator>>(EmptyArchivable & e) { return *this; }

public:
	/** Push into the archiver each passed value */
	template<typename ... Ts> void Accumulate(const Ts & ...);
	/** Extract from the archiver one value of each specified value type */
	template<typename ... Ts> std::tuple<Ts ...> Extract();

	GoToOffsetAnswer GoToOffset(const u_long expectedOffset);

private:
	template<size_t POS, typename TupleType> void TupleExtract(TupleType & tuple);

	template<size_t Index, typename ... Ts> void VariantPull(size_t index, std::variant<Ts ...> & variant);
	template<size_t Index, typename ... Ts> void VariantPush(const std::variant<Ts ...> & variant);

protected:
	Mode m_nMode;
	u_int	m_nBufSize;
	LPBYTE	m_lpBufCur;
	LPBYTE	m_lpBufMax;
	LPBYTE	m_lpBufStart;
	BYTE	m_lpBuf[nGrowSize] = { '\0' };
};

inline BOOL CAr::IsLoading() const { return m_nMode == Mode::load; }
inline BOOL CAr::IsStoring() const { return m_nMode == Mode::store; }

// -- Trivial Types

template<ArHelper::UnitTypes Type>
CAr & CAr::operator<<(const Type value) {
	CheckBuf(sizeof(Type));

	*reinterpret_cast<UNALIGNED Type *>(m_lpBufCur) = value;
	m_lpBufCur += sizeof(Type);
	return *this;
}

template<ArHelper::UnitTypes Type>
CAr & CAr::operator>>(Type & value) {
	if (m_lpBufCur + sizeof(Type) <= m_lpBufMax) {
		value = *reinterpret_cast<UNALIGNED Type *>(m_lpBufCur);
		m_lpBufCur += sizeof(Type);
	} else {
		value = Type(0);
		m_lpBufCur = m_lpBufMax;
	}
	return *this;
}

template<sqktd::Archivable Archivable>
CAr & CAr::operator<<(const Archivable & archivable) {
	Write(&archivable, sizeof(Archivable));
	return *this;
}

template<sqktd::Archivable Archivable>
CAr & CAr::operator>>(Archivable & archivable) {
	Read(&archivable, sizeof(Archivable));
	return *this;
}

#include <D3DX9Math.h>

inline CAr& operator<<(CAr & ar, D3DXVECTOR3 v)
	{	ar.Write( &v, sizeof(D3DXVECTOR3) );	return ar;	}

inline CAr& operator>>(CAr & ar, D3DXVECTOR3& v)
	{	ar.Read( &v, sizeof(D3DXVECTOR3) );		return ar;	}

inline CAr& operator<<(CAr & ar, RECT rect)
	{	ar.Write( &rect, sizeof(RECT) );	return ar;	}

inline CAr& operator>>(CAr & ar, RECT & rect)
	{	ar.Read( &rect, sizeof(RECT) );	return ar;	}

inline CAr& operator<<(CAr & ar, PLAY_ACCOUNT pa)
	{	ar.Write( &pa, sizeof(PLAY_ACCOUNT) );	return ar;	}

inline CAr& operator>>(CAr & ar, PLAY_ACCOUNT& pa)
	{	ar.Read( &pa, sizeof(PLAY_ACCOUNT) );	return ar;	}
			
inline u_long CAr::GetOffset( void )
{
	ASSERT( IsStoring() );
	return( m_lpBufCur - m_lpBufStart );
}

template<typename... Ts>
inline void CAr::Accumulate(const Ts & ... ts) {
	((*this << ts), ...);
}

template<>
inline void CAr::Accumulate() {
}

template<size_t POS, typename TupleType>
inline void CAr::TupleExtract(TupleType & tuple) {
	if constexpr (POS != std::tuple_size<TupleType>::value) {
		*this >> std::get<POS>(tuple);
		TupleExtract<POS + 1, TupleType>(tuple);
	}
}

template<typename ...Ts>
inline std::tuple<Ts...> CAr::Extract() {
	std::tuple<Ts...> tuples;
	TupleExtract<0, std::tuple<Ts...>>(tuples);
	return tuples;
}

template<size_t N>
inline CAr & operator<<(CAr & ar, const StaticString<N> & str) {
	ar.WriteString(str.GetRawStr());
	return ar;
}

template<size_t N>
inline CAr & operator>>(CAr & ar, StaticString<N> & str) {
	ar.ReadString(str.buffer.data(), N);
	return ar;
}


// std::variant send / receive
// - Original author: @SPSquonK 2022-06, released under the Boost License

template<size_t Index, typename ... Ts>
void CAr::VariantPush(const std::variant<Ts ...> & variant) {
	if constexpr (sizeof...(Ts) == Index) {
		*this << std::variant_npos;
		// end
	} else {
		if (const auto * const ptr = std::get_if<Index>(&variant)) {
			*this << Index << *ptr;
		} else {
			VariantPush<Index + 1, Ts ...>(variant);
		}
	}
}

template<size_t Index, typename ... Ts>
void CAr::VariantPull(size_t index, std::variant<Ts ...> & variant) {
	if constexpr (sizeof...(Ts) == Index) {
		// We have no really good option here

		// - You may consider throwing:
		// throw std::exception("Bad variant received in CAr");
		// - Or we can do some hacky thing to try to initialize the struct
		// to valueless

		using VariantType = std::variant<Ts ...>;
		// Destroy currently stored data
		variant.~VariantType();
		// Variant is now in "valueless_by_exception" state
		memset(&variant, 0xFFFFFFFF, sizeof(variant));
	} else {
		if (index == Index) {
			variant.emplace<Index>();
			*this >> std::get<Index>(variant);
		} else {
			VariantPull<Index + 1, Ts ...>(index, variant);
		}
	}
}

template<typename ... Ts>
CAr & operator<<(CAr & ar, const std::variant<Ts ...> & variant) {
	ar.VariantPush<0, Ts ...>(variant);
	return ar;
}

template<typename ... Ts>
CAr & operator>>(CAr & ar, std::variant<Ts ...> & variant) {
	size_t index; ar >> index;
	ar.VariantPull<0, Ts ...>(index, variant);
	return ar;
}

template<typename T> class CAr::PushBacker {
private:
	CAr * m_self;
	u_long offset;

public:
	explicit(false) PushBacker(CAr & self) : m_self(&self), offset(self.GetOffset()) {}

	[[nodiscard]] T & operator*() const {
		return *reinterpret_cast<T *>(m_self->m_lpBufStart + offset);
	}

	[[nodiscard]] T * operator->() const {
		return reinterpret_cast<T *>(m_self->m_lpBufStart + offset);
	}
};



template<typename T>
CAr::PushBacker<T> CAr::PushBack(const T & value) {
	PushBacker<T> lookBack = *this;
	*this << value;
	return lookBack;
}

// -- Strings

template<size_t N>
CAr & CAr::operator<<(const char(&buffer)[N]) requires (N >= 3) {
	WriteString(buffer);
	return *this;
}

template<size_t N>
CAr & CAr::operator>>(char(&buffer)[N]) requires (N >= 3) {
	ReadString(buffer, N);
	return *this;
}


// -- Enums

template <typename E>
CAr & CAr::operator<<(E e) requires (std::is_enum_v<E>) {
	return *this << std::to_underlying(e);
}

template <typename E>
CAr & CAr::operator>>(E & e) requires (std::is_enum_v<E>) {
	std::underlying_type_t<E> v;
	*this >> v;
	e = static_cast<E>(v);
	return *this;
}


#pragma region Containers

template<typename T, size_t N>
CAr & CAr::operator<<(const std::array<T, N> & values) {
	for (size_t i = 0; i != N; ++i) {
		*this << values[i];
	}
	return *this;
}

template<typename T, size_t N>
CAr & CAr::operator>>(std::array<T, N> & values) {
	for (size_t i = 0; i != N; ++i) {
		*this >> values[i];
	}
	return *this;
}

template<typename T, size_t N>
CAr & CAr::operator<<(const boost::container::static_vector<T, N> & values) {
	*this << values.size();
	for (const auto & value : values) {
		*this << value;
	}
	return *this;
}

template<typename T, size_t N>
CAr & CAr::operator>>(boost::container::static_vector<T, N> & values) {
	values.clear();

	size_t size;
	*this >> size;
	if (size > N) {
		size = 0;
	}
	T value{};
	for (size_t i = 0; i != size; ++i) {
		*this >> value;
		values.push_back(value);
	}
	return *this;
}

#pragma endregion
