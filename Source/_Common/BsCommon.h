#pragma once

	template< typename T >
	struct Deleter {
		void operator( ) (T & t) {
			if (t) {
				delete t;
				t = 0;
			}
		}
	};

	template< typename T >
	struct PairDeleter
	{
		void operator( ) ( T& t )
		{
			if( t.second )
			{
				delete t.second;
				t.second = 0;
			}
		}
	};


	template< typename T >
	struct SeqDeleter
	{
		void operator( ) ( T& t )
		{
			std::ranges::for_each( t, Deleter< T::value_type > ( ) );
			t.clear();
		}
	};

	template< typename T >
	struct AssDeleter
	{
		void operator( ) ( T& t )
		{
			std::ranges::for_each( t, PairDeleter< T::value_type > ( ) );
			t.clear();
		}
	};

