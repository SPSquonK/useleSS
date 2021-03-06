#pragma once

#include "HeapMng.h"
#include "FlyFFTypes.h"
#include "StaticString.h"
#include <boost/container/static_vector.hpp>
#include <variant>

class CAr final {
public:
	CAr(void *lpBuf = NULL, u_int nBufSize = 0);
	~CAr();

	CAr(const CAr &) = delete;
	CAr & operator=(const CAr &) = delete;

// Flag values
	enum	{ store = 0, load = 1 };
	enum	{ nGrowSize = 16384 };

	enum class GoToOffsetAnswer {
		SamePlace,
		NotAllConsumed,
		TooFar
	};

	static CHeapMng*	m_pHeapMng;


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

	void IsUnsafe() {
		// TODO: throw if this is a CAr received from client
	}


	template<typename T> friend class PushBacker;

	template<typename T> class PushBacker {
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
	PushBacker<T> PushBack(const T & value) {
		PushBacker<T> lookBack = *this;
		*this << value;
		return lookBack;
	}

	// reading and writing strings
	void WriteString(LPCTSTR lpsz);
	LPTSTR ReadString( LPTSTR lpsz, int nBufSize );

	template<size_t N>
	LPTSTR ReadString(char (&buffer)[N]) { return ReadString(buffer, N); }

	LPBYTE	GetBuffer( int* pnBufSize );
	u_long	GetOffset( void );
/*
	void	Copy( CAr & ar );
	CAr& operator = ( CAr & ar );
*/
	// insertion operations
	CAr& operator<<(BYTE by);
	CAr& operator<<(WORD w);
	CAr& operator<<(LONG l);
	CAr& operator<<(DWORD dw);
	CAr& operator<<(float f);
	CAr& operator<<(double d);

	CAr& operator<<(int i);
	CAr& operator<<(short w);
	CAr& operator<<(char ch);
	CAr& operator<<(unsigned u);

	template<size_t N>
	CAr & operator<<(const char(&buffer)[N]) requires (N >= 3) {
		WriteString(buffer);
		return *this;
	}

	// Avoid implicit cast of pointers to an integer type when sending
	template<typename T> requires (std::is_pointer_v<T>)
	CAr & operator<<(T t) = delete;

	// extraction operations
	CAr& operator>>(BYTE& by);
	CAr& operator>>(WORD& w);
	CAr& operator>>(DWORD& dw);
	CAr& operator>>(LONG& l);
	CAr& operator>>(float& f);
	CAr& operator>>(double& d);

	CAr& operator>>(int& i);
	CAr& operator>>(short& w);
	CAr& operator>>(char& ch);
	CAr& operator>>(unsigned& u);

	CAr & operator<<(bool b) { return *this << static_cast<BYTE>(b ? 1 : 0); }
	CAr & operator>>(bool & b) { BYTE bb; *this >> bb; b = bb != 0; return *this; }

	template <typename E>
	CAr & operator<<(E e) requires (std::is_scoped_enum_v<E>) {
		return *this << std::to_underlying(e);
	}

	template <typename E>
	CAr & operator>>(E & e) requires (std::is_scoped_enum_v<E>) {
		std::underlying_type_t<E> v;
		*this >> v;
		e = static_cast<E>(v);
		return *this;
	}

	template<size_t N>
	CAr & operator>>(char(&buffer)[N]) requires (N >= 3) {
		ReadString(buffer, N);
		return *this;
	}

#ifdef __CLIENT
#ifdef _DEBUG
static	DWORD	s_dwHdrPrev;
static	DWORD	s_dwHdrCur;
#endif	// _DEBUG
#endif	// __CLIENT

	/** Push into the archiver each passed value */
	template<typename ... Ts> void Accumulate(const Ts & ...);
	/** Extract from the archiver one value of each specified value type */
	template<typename ... Ts> std::tuple<Ts ...> Extract();

	template<sqktd::Archivable Archivable>
	CAr & operator<<(const Archivable & archivable) {
		Write(&archivable, sizeof(Archivable));
		return *this;
	}

	template<sqktd::Archivable Archivable>
	CAr & operator>>(Archivable & archivable) {
		Read(&archivable, sizeof(Archivable));
		return *this;
	}

	template<sqktd::EmptyArchivable EmptyArchivable>
	CAr & operator<<(const EmptyArchivable & e) { return *this; }

	template<sqktd::EmptyArchivable EmptyArchivable>
	CAr & operator>>(EmptyArchivable & e) { return *this; }

	template<typename T, size_t N>
	CAr & operator<<(const std::array<T, N> & values) {
		for (size_t i = 0; i != N; ++i) {
			*this << values[i];
		}
		return *this;
	}

	template<typename T, size_t N>
	CAr & operator>>(std::array<T, N> & values) {
		for (size_t i = 0; i != N; ++i) {
			*this >> values[i];
		}
		return *this;
	}

	template<typename T, size_t N>
	CAr & operator<<(const boost::container::static_vector<T, N> & values) {
		*this << values.size();
		for (const auto & value : values) {
			*this << value;
		}
		return *this;
	}

	template<typename T, size_t N>
	CAr & operator>>(boost::container::static_vector<T, N> & values) {
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


	template<typename ... Ts> friend CAr & operator<<(CAr & ar, const std::variant<Ts ...> & variant);

	template<typename ... Ts> friend CAr & operator>>(CAr & ar, std::variant<Ts ...> & variant);


	GoToOffsetAnswer GoToOffset(const u_long expectedOffset) {
		BYTE * target = m_lpBufStart + expectedOffset;

		if (target == m_lpBufCur) {
			return GoToOffsetAnswer::SamePlace;
		}

		const GoToOffsetAnswer answer = target > m_lpBufCur ?
			GoToOffsetAnswer::NotAllConsumed :
			GoToOffsetAnswer::TooFar;
		
		m_lpBufCur = target;

		if (target > m_lpBufMax) [[unlikely]] {
			Error("CAr::GoToOffset - Target is out of bound");
			m_lpBufCur = m_lpBufMax;
		}

		return answer;
	}

private:
	template<size_t POS, typename TupleType> void TupleExtract(TupleType & tuple);

	template<size_t Index, typename ... Ts>
	void VariantPull(size_t index, std::variant<Ts ...> & variant);
	
	template<size_t Index, typename ... Ts>
	void VariantPush(const std::variant<Ts ...> & variant);

protected:
	BYTE	m_nMode;	// read or write
	u_int	m_nBufSize;
	LPBYTE	m_lpBufCur;
	LPBYTE	m_lpBufMax;
	LPBYTE	m_lpBufStart;
	BYTE	m_lpBuf[nGrowSize];
};
/*
inline void CAr::Copy( CAr & ar )
	{	ASSERT( IsLoading() );	ASSERT( ar.IsStoring() );	ar.Write( (void*)m_lpBufStart, (u_int)( m_lpBufMax - m_lpBufStart ) );	}
inline CAr& CAr::operator = ( CAr & ar )
	{	ar.Copy( *this );	return *this;	}
*/
inline BOOL CAr::IsLoading() const
	{ return (m_nMode & CAr::load) != 0; }
inline BOOL CAr::IsStoring() const
	{ return (m_nMode & CAr::load) == 0; }

inline CAr& CAr::operator<<(int i)
	{ return CAr::operator<<((LONG)i); }
inline CAr& CAr::operator<<(unsigned u)
	{ return CAr::operator<<((LONG)u); }
inline CAr& CAr::operator<<(short w)
	{ return CAr::operator<<((WORD)w); }
inline CAr& CAr::operator<<(char ch)
	{ return CAr::operator<<((BYTE)ch); }
inline CAr& CAr::operator<<(BYTE by)
	{ CheckBuf( sizeof(BYTE) );
		*(UNALIGNED BYTE*)m_lpBufCur = by; m_lpBufCur += sizeof(BYTE); return *this; }
inline CAr& CAr::operator<<(WORD w)
	{ CheckBuf( sizeof( WORD ) );
		*(UNALIGNED WORD*)m_lpBufCur = w; m_lpBufCur += sizeof(WORD); return *this; }
inline CAr& CAr::operator<<(LONG l)
	{ CheckBuf( sizeof(LONG) );
		*(UNALIGNED LONG*)m_lpBufCur = l; m_lpBufCur += sizeof(LONG); return *this; }
inline CAr& CAr::operator<<(DWORD dw)
	{ CheckBuf( sizeof(DWORD) );
		*(UNALIGNED DWORD*)m_lpBufCur = dw; m_lpBufCur += sizeof(DWORD); return *this; }
inline CAr& CAr::operator<<(float f)
	{ CheckBuf( sizeof(float) );
		*(UNALIGNED FLOAT*)m_lpBufCur = *(FLOAT*)&f; m_lpBufCur += sizeof(float); return *this; }
inline CAr& CAr::operator<<(double d)
	{ CheckBuf( sizeof(double) );
		*(UNALIGNED double*)m_lpBufCur = *(double*)&d; m_lpBufCur += sizeof(double); return *this; }

inline CAr& CAr::operator>>(int& i)
	{ return CAr::operator>>((LONG&)i); }
inline CAr& CAr::operator>>(unsigned& u)
	{ return CAr::operator>>((LONG&)u); }
inline CAr& CAr::operator>>(short& w)
	{ return CAr::operator>>((WORD&)w); }
inline CAr& CAr::operator>>(char& ch)
	{ return CAr::operator>>((BYTE&)ch); }

#define	CAR_SAFE_READ( type, value )	\
	if( m_lpBufCur + sizeof(type) <= m_lpBufMax )	\
		{	value	= *(UNALIGNED type*)m_lpBufCur;	m_lpBufCur += sizeof(type);	}	\
	else	\
		{	value	= (type)0;	m_lpBufCur	= m_lpBufMax;	}	\
	return *this

inline CAr& CAr::operator>>(BYTE& by)
	{	CAR_SAFE_READ( BYTE, by );	}
inline CAr& CAr::operator>>(WORD& w)
	{	CAR_SAFE_READ( WORD, w );	}
inline CAr& CAr::operator>>(DWORD& dw)
	{	CAR_SAFE_READ( DWORD, dw );	}
inline CAr& CAr::operator>>(float& f)
	{	CAR_SAFE_READ( float, f );	}
inline CAr& CAr::operator>>(double& d)
	{	CAR_SAFE_READ( double, d );	}
inline CAr& CAr::operator>>(LONG& l)
	{	CAR_SAFE_READ( LONG, l );	}

#include <D3DX9Math.h>

inline CAr& operator<<(CAr & ar, D3DXVECTOR3 v)
	{	ar.Write( &v, sizeof(D3DXVECTOR3) );	return ar;	}

inline CAr& operator>>(CAr & ar, D3DXVECTOR3& v)
	{	ar.Read( &v, sizeof(D3DXVECTOR3) );		return ar;	}

inline CAr& operator<<(CAr & ar, __int64 i)
	{	ar.Write( &i, sizeof(__int64) );	return ar;	}

inline CAr& operator>>(CAr & ar, __int64& i)
	{	ar.Read( &i, sizeof(__int64) );	return ar;	}

/*
inline CAr& operator<<(CAr & ar, CRect rect)
	{	ar.Write( &rect, sizeof(CRect) );	return ar;	}

inline CAr& operator>>(CAr & ar, CRect & rect)
	{	ar.Read( &rect, sizeof(CRect) );	return ar;	}
*/

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
