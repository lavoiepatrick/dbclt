/*
MIT License

Copyright (c) 2021 Patrick Lavoie

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <catch2/catch.hpp>

#include <chrono>
#include <date/date.h>
#include <iostream>

#include <dbclt/postgres.h>

#include <structs/to_tuple.h>

static std::ostream& operator<<( std::ostream& os, const dbclt::datetime& dt )
{
	os << date::format( "%FT%T", dt );
	return os;
}

static std::string env( const char* var, const char* defValue )
{
	const char* value = getenv( var );
	if( value == nullptr )
		return defValue;
	return value;
}

static const std::string connInfo( env( "DBXX_TEST_POSTGRES_CONNSTRING",
										"hostaddr=localhost port=5432 sslmode=prefer "
										"dbname=postgres user=postgres password=secret" ) );

struct MyRecord
{
	std::optional< std::string > t;
	std::optional< int32_t > i;
	std::optional< int64_t > bi;
	std::optional< double > f;
	std::optional< bool > b;
	std::optional< std::string > tt;
	std::optional< dbclt::datetime > dt;

	bool operator==( const MyRecord& rhs ) const
	{
		return t == rhs.t;
	}

	friend std::ostream& operator<<( std::ostream& os, const MyRecord& v );
};

template< typename T >
inline std::string toString( const std::optional< T >& v )
{
	if( v )
		return std::to_string( *v );
	return "null";
}

inline std::string toString( const std::optional< std::string >& v )
{
	if( v )
		return *v;
	return "null";
}

inline std::ostream& operator<<( std::ostream& os, const MyRecord& v )
{
	os << toString( v.t ) << "; " << toString( v.i );
	return os;
}

#if 0
template< typename BE >
void dbclt::convert( const dbclt::record< BE >& from, MyRecord& to )
{
	to.t = from.get_nullable_string( 0 );
	to.i = from.get_nullable_int32( 1 );
	to.bi = from.get_nullable_int64( 2 );
	to.f = from.get_nullable_double( 3 );
	to.b = from.get_nullable_bool( 4 );
	to.tt = from.get_nullable_string( 5 );
	to.dt = from.get_nullable_datetime( 6 );
}
#else
template<>
struct structs::to_tuple_size< MyRecord > : std::integral_constant< std::size_t, 7 >
{
};

namespace dbclt
{
	#if 1
template< typename BE, typename T >
struct get_value
{
	T operator( )( const dbclt::record< BE >& from, size_t ndx );
};

template< typename BE >
struct get_value< BE, std::string >
{
	std::string operator( )( const dbclt::record< BE >& from, size_t ndx )
	{
		return from.get_string( ndx );
	}
};

template< typename BE >
struct get_value< BE, std::optional< std::string > >
{
	std::optional< std::string > operator( )( const dbclt::record< BE >& from, size_t ndx )
	{
		return from.get_nullable_string( ndx );
	}
};

template< typename BE >
struct get_value< BE, std::optional< int32_t > >
{
	std::optional< int32_t > operator( )( const dbclt::record< BE >& from, size_t ndx )
	{
		return from.get_nullable_int32( ndx );
	}
};

template< typename BE >
struct get_value< BE, std::optional< int64_t > >
{
	std::optional< int64_t > operator( )( const dbclt::record< BE >& from, size_t ndx )
	{
		return from.get_nullable_int64( ndx );
	}
};

template< typename BE >
struct get_value< BE, std::optional< double > >
{
	std::optional< double > operator( )( const dbclt::record< BE >& from, size_t ndx )
	{
		return from.get_nullable_double( ndx );
	}
};

template< typename BE >
struct get_value< BE, std::optional< bool > >
{
	std::optional< bool > operator( )( const dbclt::record< BE >& from, size_t ndx )
	{
		return from.get_nullable_bool( ndx );
	}
};

template< typename BE >
struct get_value< BE, std::optional< dbclt::datetime > >
{
	std::optional< dbclt::datetime > operator( )( const dbclt::record< BE >& from, size_t ndx )
	{
		return from.get_nullable_datetime( ndx );
	}
};
	#elif 0
template< typename BE, typename T >
struct get_value
{
	T operator( )( const dbclt::record< BE >& from, size_t ndx );
};

template< typename BE >
struct get_value< BE, std::string >
{
	std::string operator( )( const dbclt::record< BE >& from, size_t ndx )
	{
		return from.get_string( ndx );
	}
};

template< typename BE >
struct get_value< BE, std::optional< std::string > >
{
	std::optional< std::string > operator( )( const dbclt::record< BE >& from, size_t ndx )
	{
		return from.get_nullable_string( ndx );
	}
};

template< typename BE >
struct get_value< BE, std::optional< int32_t > >
{
	std::optional< int32_t > operator( )( const dbclt::record< BE >& from, size_t ndx )
	{
		return from.get_nullable_int32( ndx );
	}
};

template< typename BE >
struct get_value< BE, std::optional< int64_t > >
{
	std::optional< int64_t > operator( )( const dbclt::record< BE >& from, size_t ndx )
	{
		return from.get_nullable_int64( ndx );
	}
};

template< typename BE >
struct get_value< BE, std::optional< double > >
{
	std::optional< double > operator( )( const dbclt::record< BE >& from, size_t ndx )
	{
		return from.get_nullable_double( ndx );
	}
};

template< typename BE >
struct get_value< BE, std::optional< bool > >
{
	std::optional< bool > operator( )( const dbclt::record< BE >& from, size_t ndx )
	{
		return from.get_nullable_bool( ndx );
	}
};

template< typename BE >
struct get_value< BE, std::optional< dbclt::datetime > >
{
	std::optional< dbclt::datetime > operator( )( const dbclt::record< BE >& from, size_t ndx )
	{
		return from.get_nullable_datetime( ndx );
	}
};
	#elif 1
template< typename BE >
struct value
{
	template< typename T >
	T get( const dbclt::record< BE >& from, size_t ndx );
};
template< typename BE >
std::string value< BE >::get< std::string >( const dbclt::record< BE >& from, size_t ndx )
{
	return from.get_string( ndx );
}

template<>
std::optional< std::string > get< std::optional< std::string > >( const dbclt::record< BE >& from,
																  size_t ndx )
{
	return from.get_nullable_string( ndx );
}

template<>
std::optional< int32_t > get< std::optional< int32_t > >( const dbclt::record< BE >& from,
														  size_t ndx )
{
	return from.get_nullable_int32( ndx );
}

template<>
std::optional< int64_t > get< std::optional< int64_t > >( const dbclt::record< BE >& from,
														  size_t ndx )
{
	return from.get_nullable_int64( ndx );
}

template<>
std::optional< double > get< std::optional< double > >( const dbclt::record< BE >& from,
														size_t ndx )
{
	return from.get_nullable_double( ndx );
}

template<>
std::optional< bool > get< std::optional< bool > >( const dbclt::record< BE >& from, size_t ndx )
{
	return from.get_nullable_bool( ndx );
}

template<>
std::optional< dbclt::datetime >
get< std::optional< dbclt::datetime > >( const dbclt::record< BE >& from, size_t ndx )
{
	return from.get_nullable_datetime( ndx );
}
};
	#else
template< typename BE, typename T >
T get_value( const dbclt::record< BE >& from, size_t ndx );

template< typename BE >
std::string get_value< BE, std::string >( const dbclt::record< BE >& from, size_t ndx )
{
	return from.get_string( ndx );
}

template< typename BE >
std::optional< std::string >
get_value< BE, std::optional< std::string > >( const dbclt::record< BE >& from, size_t ndx )
{
	return from.get_nullable_string( ndx );
}

template< typename BE >
std::optional< int32_t > get_value< BE, std::optional< int32_t > >( const dbclt::record< BE >& from,
																	size_t ndx )
{
	return from.get_nullable_int32( ndx );
}

template< typename BE >
std::optional< int64_t > get_value< BE, std::optional< int64_t > >( const dbclt::record< BE >& from,
																	size_t ndx )
{
	return from.get_nullable_int64( ndx );
}

template< typename BE >
std::optional< double > get_value< BE, std::optional< double > >( const dbclt::record< BE >& from,
																  size_t ndx )
{
	return from.get_nullable_double( ndx );
}

template< typename BE >
std::optional< bool > get_value< BE, std::optional< bool > >( const dbclt::record< BE >& from,
															  size_t ndx )
{
	return from.get_nullable_bool( ndx );
}

template< typename BE >
std::optional< dbclt::datetime >
get_value< BE, std::optional< dbclt::datetime > >( const dbclt::record< BE >& from, size_t ndx )
{
	return from.get_nullable_datetime( ndx );
}
	#endif

template< typename BE, typename T, typename Tup, std::size_t... I >
T _convert( const dbclt::record< BE >& from, std::index_sequence< I... > )
{
	#if 1
	return T { get_value< BE, typename std::tuple_element< I, Tup >::type >( )( from, I )... };
	#elif 1
	return T { value< BE >::get< typename std::tuple_element< I, Tup >::type >( )( from, I )... };
	#else
	return T { get_value< BE, typename std::tuple_element< I, Tup >::type >( )( from, I )... };
	#endif
}

}  // namespace dbclt

	#if 0
template< typename BE >
void dbclt::convert( const dbclt::record< BE >& from, MyRecord& to )
{
	using Tup = decltype( stdext::to_tuple( to ) );
		#if 1
	to = _convert< BE, MyRecord, Tup >( from,
										std::make_index_sequence< std::tuple_size_v< Tup > > { } );
		#else
	to.t = from.get_nullable_string( 0 );
	to.i = from.get_nullable_int32( 1 );
	to.bi = from.get_nullable_int64( 2 );
	to.f = from.get_nullable_double( 3 );
	to.b = from.get_nullable_bool( 4 );
	to.tt = from.get_nullable_string( 5 );
	to.dt = from.get_nullable_datetime( 6 );
		#endif
}
	#endif
#endif

TEST_CASE( "postgres" )
{
	SECTION( "NotInitialized" )
	{
		REQUIRE_THROWS_AS(
			[]( ) {
				dbclt::postgres::session session;
				session.execute( "select * from tmp" );
			}( ),
			dbclt::data_exception );

		REQUIRE_THROWS_AS(
			[]( ) {
				dbclt::postgres::session session;
				dbclt::postgres::statement stmt( session );
				stmt.execute( "select * from tmp" );
			}( ),
			dbclt::data_exception );
	}

	SECTION( "Connect" )
	{
		dbclt::postgres::session session;
		REQUIRE( !session.connected( ) );
		session.connect( connInfo );
		REQUIRE( session.connected( ) );
	}

	SECTION( "Empty" )
	{
		dbclt::postgres::session session;
		session.connect( connInfo );
		REQUIRE( session.connected( ) );

		dbclt::postgres::recordset rs = session.query( "select 1 where 1=0" );
		REQUIRE( rs.empty( ) );
		REQUIRE( rs.record_count( ) == 0 );

		rs = session.query( "select 1 where 1=1" );
		REQUIRE( !rs.empty( ) );
		REQUIRE( rs.record_count( ) == 1 );
	}

	SECTION( "Simple" )
	{
		dbclt::postgres::session session;
		session.connect( connInfo );
		dbclt::postgres::result res = session.execute(
			"create temp table tmp (t varchar(100) null, i integer null, bi bigint null, f float "
			"null, b boolean null, tt text null, dt timestamp null)" );
		REQUIRE( res.affected_count( ) == 0 );
		res = session.execute( "insert into tmp values ('t1', 1, 1234567890123456789, 1.1, true, "
							   "'long text', '2019-12-09')" );
		REQUIRE( res.affected_count( ) == 1 );

		dbclt::postgres::recordset rs = session.query( "select * from tmp" );
		REQUIRE( rs.record_count( ) == 1 );
		size_t processed = 0;
		for( const dbclt::postgres::record& rec: rs )
		{
			REQUIRE( rec.get_string( 0 ) == "t1" );
			REQUIRE( rec.get_int32( 1 ) == 1 );
			REQUIRE( rec.get_int64( 2 ) == 1234567890123456789 );
			REQUIRE( rec.get_double( 3 ) == Approx( 1.1 ).epsilon( 0.0000001 ) );
			REQUIRE( rec.get_bool( 4 ) );
			REQUIRE( rec.get_string( 5 ) == "long text" );
			std::cout << date::format( "%FT%T", rec.get_datetime( 6 ) ) << std::endl;
			REQUIRE( rec.get_datetime( 6 ) ==
					 dbclt::datetime( date::sys_days( date::year( 2019 ) / 12 / 9 ) ) );
			processed++;
		}
		REQUIRE( processed == 1 );
	}

	SECTION( "Convert" )
	{
		dbclt::postgres::session session;
		session.connect( connInfo );
		session.execute( "create temp table tmp (t varchar(100) null, i integer null, bi bigint "
						 "null, f float null, b boolean null, tt text null, dt timestamp null)" );
		dbclt::postgres::result res =
			session.execute( "insert into tmp values ('t1', 1, 1234567890123456789, 1.1, true, "
							 "'long text', '2019-12-09')" );
		REQUIRE( res.affected_count( ) == 1 );

		dbclt::postgres::recordset rs = session.query( "select * from tmp" );
		REQUIRE( rs.record_count( ) == 1 );
		size_t processed = 0;
		for( const dbclt::postgres::record& dbRec: rs )
		{
			MyRecord rec;
			dbclt::convert( dbRec, rec );

			REQUIRE( *rec.t == "t1" );
			REQUIRE( *rec.i == 1 );
			REQUIRE( *rec.bi == 1234567890123456789 );
			REQUIRE( *rec.f == Approx( 1.1 ).epsilon( 0.00001 ) );
			REQUIRE( *rec.b );
			REQUIRE( *rec.tt == "long text" );
			REQUIRE( *rec.dt == dbclt::datetime( date::sys_days( date::year( 2019 ) / 12 / 9 ) ) );
			processed++;
		}
		REQUIRE( processed == 1 );
	}

	SECTION( "Into" )
	{
		dbclt::postgres::session session;
		session.connect( connInfo );
		session.execute( "create temp table tmp (t varchar(100) null, i integer null, bi bigint "
						 "null, f float null, b boolean null, tt text null, dt timestamp null)" );
		session.execute( "insert into tmp values ('t1', 1, 1234567890123456789, 1.1, true, 'long "
						 "text', '2019-12-09')" );

		{
			dbclt::postgres::recordset rs = session.query( "select * from tmp" );
			REQUIRE( rs.record_count( ) == 1 );

			std::vector< MyRecord > records;
			rs.into( records );
			REQUIRE( records.size( ) == 1 );

			size_t processed = 0;
			for( const MyRecord& rec: records )
			{
				REQUIRE( *rec.t == "t1" );
				REQUIRE( *rec.i == 1 );
				REQUIRE( *rec.bi == 1234567890123456789 );
				REQUIRE( *rec.f == Approx( 1.1 ).epsilon( 0.00001 ) );
				REQUIRE( *rec.b );
				REQUIRE( *rec.tt == "long text" );
				REQUIRE( *rec.dt ==
						 dbclt::datetime( date::sys_days( date::year( 2019 ) / 12 / 9 ) ) );
				processed++;
			}
			REQUIRE( processed == 1 );
		}

		{
			std::vector< MyRecord > records;
			session.query( "select * from tmp" ).into( records );
			REQUIRE( records.size( ) == 1 );

			size_t processed = 0;
			for( const MyRecord& rec: records )
			{
				REQUIRE( *rec.t == "t1" );
				REQUIRE( *rec.i == 1 );
				REQUIRE( *rec.bi == 1234567890123456789 );
				REQUIRE( *rec.f == Approx( 1.1 ).epsilon( 0.00001 ) );
				REQUIRE( *rec.b );
				REQUIRE( *rec.tt == "long text" );
				REQUIRE( *rec.dt ==
						 dbclt::datetime( date::sys_days( date::year( 2019 ) / 12 / 9 ) ) );
				processed++;
			}
			REQUIRE( processed == 1 );
		}
	}

	SECTION( "ParamBinding" )
	{
		dbclt::postgres::session session;
		session.connect( connInfo );
		{
			dbclt::postgres::result res = session.execute(
				"create temp table tmp (t varchar(100) null, i integer null, bi bigint null, f "
				"float null, b boolean null, tt text null, dt timestamp null)" );
			REQUIRE( res.affected_count( ) == 0 );
			// res = session.execute( "insert into tmp values ('t1', 1, 1234567890123456789, 1.1,
			// true, 'long text', '2019-12-09')" );
			const std::string s1 = "t1";
			const int32_t i1 = 1;
			const int64_t i2 = 1234567890123456789;
			const float f = 1.1;
			const bool b = true;
			const std::string tt = "long text";
			const dbclt::datetime dt( date::sys_days( date::year( 2019 ) / 12 / 9 ) );

			dbclt::postgres::statement stmt( session );
			res = stmt.execute_params( "insert into tmp values ($1,$2,$3,$4,$5,$6,$7)",
									   s1,
									   i1,
									   i2,
									   f,
									   b,
									   tt,
									   dt );
			res = stmt.execute_params(
				"insert into tmp values ($1,$2,$3,$4,$5,$6,$7)",
				std::string( "t1" ),
				( int32_t )1,
				( int64_t )1234567890123456789,
				1.1f,
				true,
				std::string( "long text" ),
				dbclt::datetime( date::sys_days( date::year( 2019 ) / 12 / 9 ) ) );
			REQUIRE( res.affected_count( ) == 1 );

			dbclt::postgres::recordset rs = session.query( "select * from tmp" );
			REQUIRE( rs.record_count( ) == 2 );
			size_t processed = 0;
			for( const dbclt::postgres::record& rec: rs )
			{
				REQUIRE( rec.get_string( 0 ) == "t1" );
				REQUIRE( rec.get_int32( 1 ) == 1 );
				REQUIRE( rec.get_int64( 2 ) == 1234567890123456789 );
				REQUIRE( rec.get_float( 3 ) == 1.1f );
				REQUIRE( rec.get_double( 3 ) == 1.1f );
				REQUIRE( rec.get_bool( 4 ) == true );
				REQUIRE( rec.get_string( 5 ) == "long text" );
				REQUIRE( rec.get_datetime( 6 ) ==
						 dbclt::datetime( date::sys_days( date::year( 2019 ) / 12 / 9 ) ) );
				processed++;
			}
			REQUIRE( processed == 2 );
		}
#if 0
		{
			dbclt::postgres::result res = session.execute( R"(create or replace function swap(
inout x int,
inout y int)
language plpgsql
as $$
begin
   select x,y into y,x;
end; $$)" );
			REQUIRE( res.affected_count( ) == 0 );

			const int32_t i1 = 20;
			const int32_t i2 = 10;
			dbclt::postgres::statement stmt( session );
			dbclt::postgres::recordset rs =
				stmt.query_params( "select * from swap($1,$2)", i1, i2 );
			REQUIRE( rs.record_count( ) == 1 );
			const dbclt::postgres::record& rec = *rs.begin( );
			REQUIRE( rec.get_int32( 0 ) == 10 );
			REQUIRE( rec.get_int32( 1 ) == 20 );
			REQUIRE( i1 == 10 );
			REQUIRE( i2 == 20 );
		}
#endif
	}

#if 0
	SECTION( "BulkInsert" )
	{
		dbclt::postgres::session session;
		session.connect( connInfo );
		session.execute( "create temp table tmp (t varchar(100) null, i integer null, bi bigint "
						 "null, f float null, b boolean null, tt text null, dt timestamp null)" );

		std::vector< MyRecord > toInsert = {
			{ std::string( "t1" ),
			  1,
			  1234567890123456789,
			  1.1,
			  true,
			  std::string( "tt1" ),
			  dbclt::datetime( date::sys_days( date::year( 2019 ) / 12 / 9 ) ) },
			{ std::string( "t2" ),
			  2,
			  2345678901234567891,
			  2.2,
			  false,
			  std::string( "tt2" ),
			  dbclt::datetime( date::sys_days( date::year( 2019 ) / 12 / 10 ) ) },
			{ std::string( "t3" ),
			  3,
			  3456789012345678912,
			  3.3,
			  true,
			  std::string( "tt3" ),
			  dbclt::datetime( date::sys_days( date::year( 2019 ) / 12 / 11 ) ) }
		};

		session.bulk_insert( "tmp", toInsert );

		std::vector< MyRecord > records;
		session.query( "select * from tmp" ).into( records );
		REQUIRE( records.size( ) == 3 );

		for( size_t i = 0; i < records.size( ); ++i )
			REQUIRE( records[ i ] == toInsert[ i ] );
	}
#endif

	SECTION( "Transaction" )
	{
		dbclt::postgres::session session;
		session.connect( connInfo );
		session.execute( "create temp table tmp (t varchar(100) null, i integer null, bi bigint "
						 "null, f float null, b boolean null, tt text null, dt timestamp null)" );

		{
			dbclt::postgres::transaction txn( session );
			session.execute( "insert into tmp values ('t1', 1, 1234567890123456789, 1.1, true, "
							 "'long text', '2019-12-09')" );
			txn.rollback( );

			dbclt::postgres::recordset rs = session.query( "select * from tmp" );
			REQUIRE( rs.record_count( ) == 0 );
		}

		{
			{
				dbclt::postgres::transaction txn( session );
				session.execute( "insert into tmp values ('t1', 1, 1234567890123456789, 1.1, true, "
								 "'long text', '2019-12-09')" );
			}

			dbclt::postgres::recordset rs = session.query( "select * from tmp" );
			REQUIRE( rs.record_count( ) == 0 );
		}

		{
			dbclt::postgres::transaction txn( session );
			session.execute( "insert into tmp values ('t1', 1, 1234567890123456789, 1.1, true, "
							 "'long text', '2019-12-09')" );
			txn.commit( );

			dbclt::postgres::recordset rs = session.query( "select * from tmp" );
			REQUIRE( rs.record_count( ) == 1 );
		}
	}
}
