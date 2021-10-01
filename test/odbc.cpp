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

#include <iostream>

#include <dbclt/odbc.h>

/*
sudo apt-get install odbc-postgresql
*/

/*
sybase smalldatetime -> 1900-01-01 - 2079-06-06  (minutes)
sybase datetime -> 1753-01-01 - 9999-12-31  (ms)
sybase bigdatetime -> 0001-01-01 - 9999-12-31  (ns)
sybase date -> 0001-01-01 - 9999-12-31
sybase time -> ms
sybase bigtime -> ns

std::tm -> no ms

date::year_month_day for date only
date::time_of_day for time only
std::chrono::time_point< std::chrono::system_clock, std::chrono::microseconds > for datetime
*/

using time_point_minutes =
	std::chrono::time_point< std::chrono::system_clock, std::chrono::minutes >;
using time_point_seconds =
	std::chrono::time_point< std::chrono::system_clock, std::chrono::seconds >;
using time_point_milliseconds =
	std::chrono::time_point< std::chrono::system_clock, std::chrono::milliseconds >;
using time_point_nanoseconds =
	std::chrono::time_point< std::chrono::system_clock, std::chrono::nanoseconds >;
using time_point_microseconds =
	std::chrono::time_point< std::chrono::system_clock, std::chrono::microseconds >;

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

static const std::string connInfo( env( "DBXX_TEST_ODBC_POSTGRES_CONNSTRING",
										"DRIVER=PostgreSQL "
										"ANSI;SERVER=localhost;PORT=5432;DATABASE=Vision;UID="
										"postgres;PWD=secret;BoolsAsChar=0" ) );

struct MyRecord
{
	MyRecord( )
	{
	}
	MyRecord( std::optional< std::string > t,
			  std::optional< int32_t > i,
			  std::optional< int64_t > bi,
			  std::optional< double > f,
			  std::optional< bool > b,
			  std::optional< std::string > tt,
			  std::optional< dbclt::datetime > dt )
	{
	}

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

namespace dbclt
{
template< typename BE >
void convert( const dbclt::record< BE >& from, MyRecord& to )
{
	to.t = from.get_nullable_string( 0 );
	to.i = from.get_nullable_int32( 1 );
	to.bi = from.get_nullable_int64( 2 );
	to.f = from.get_nullable_double( 3 );
	to.b = from.get_nullable_bool( 4 );
	to.tt = from.get_nullable_string( 5 );
	to.dt = from.get_nullable_datetime( 6 );
}
}  // namespace dbclt

TEST_CASE( "odbc" )
{
	SECTION( "NotInitialized" )
	{
		REQUIRE_THROWS_AS(
			[]( ) {
				dbclt::odbc::session session;
				session.execute( "select * from tmp" );
			}( ),
			dbclt::data_exception );

		REQUIRE_THROWS_AS(
			[]( ) {
				dbclt::odbc::session session;
				dbclt::odbc::statement stmt( session );
				stmt.execute( "select * from tmp" );
			}( ),
			dbclt::data_exception );
	}

	SECTION( "Connect" )
	{
		dbclt::odbc::session session;
		session.connect( connInfo );
		std::cout << session.backend( ).connectionString( ) << std::endl;
		REQUIRE( session.connected( ) );

		dbclt::odbc::recordset rs = session.query( "select 1 where 1=0" );
		REQUIRE( rs.empty( ) );
		REQUIRE( rs.record_count( ) == 0 );
	}

	SECTION( "Empty" )
	{
		dbclt::odbc::session session;
		REQUIRE( !session.connected( ) );
		session.connect( connInfo );
		REQUIRE( session.connected( ) );

		dbclt::odbc::recordset rs = session.query( "select 1 where 1=0" );
		REQUIRE( rs.empty( ) );
		REQUIRE( rs.record_count( ) == 0 );

		rs = session.query( "select 1 where 1=1" );
		REQUIRE( !rs.empty( ) );
		REQUIRE( rs.record_count( ) == 1 );
	}

	SECTION( "Simple" )
	{
		dbclt::odbc::session session;
		session.connect( connInfo );
		session.execute( "create temp table tmp (t varchar(100) null, i integer null, bi bigint "
						 "null, f float null, b boolean null, tt text null, dt timestamp null)" );
		session.execute( "insert into tmp values ('t1', 1, 1234567890123456789, 1.1, true, 'long "
						 "text', '2019-12-09')" );

		dbclt::odbc::recordset rs = session.query( "select * from tmp" );
		REQUIRE( rs.record_count( ) == 1 );
		size_t processed = 0;
		for( const dbclt::odbc::record& rec: rs )
		{
			REQUIRE( rec.get_string( 0 ) == "t1" );
			REQUIRE( rec.get_int32( 1 ) == 1 );
			REQUIRE( rec.get_int64( 2 ) == 1234567890123456789 );
			REQUIRE( rec.get_double( 3 ) == Approx( 1.1 ).epsilon( 0.0000001 ) );
			REQUIRE( rec.get_bool( 4 ) );
			REQUIRE( rec.get_string( 5 ) == "long text" );
			REQUIRE( rec.get_datetime( 6 ) ==
					 dbclt::datetime( date::sys_days( date::year( 2019 ) / 12 / 9 ) ) );
			processed++;
		}
		REQUIRE( processed == 1 );
	}

	SECTION( "Convert" )
	{
		dbclt::odbc::session session;
		session.connect( connInfo );
		session.execute( "create temp table tmp (t varchar(100) null, i integer null, bi bigint "
						 "null, f float null, b boolean null, tt text null, dt timestamp null)" );
		session.execute( "insert into tmp values ('t1', 1, 1234567890123456789, 1.1, true, 'long "
						 "text', '2019-12-09')" );

		dbclt::odbc::recordset rs = session.query( "select * from tmp" );
		REQUIRE( rs.record_count( ) == 1 );
		size_t processed = 0;
		for( const dbclt::odbc::record& dbRec: rs )
		{
			MyRecord rec;
		dbclt:
			convert( dbRec, rec );

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
		dbclt::odbc::session session;
		session.connect( connInfo );
		session.execute( "create temp table tmp (t varchar(100) null, i integer null, bi bigint "
						 "null, f float null, b boolean null, tt text null, dt timestamp null)" );
		session.execute( "insert into tmp values ('t1', 1, 1234567890123456789, 1.1, true, 'long "
						 "text', '2019-12-09')" );

		{
			dbclt::odbc::recordset rs = session.query( "select * from tmp" );
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

	SECTION( "BulkInsert" )
	{
		dbclt::odbc::session session;
		session.connect( connInfo );
		session.execute( "create temp table tmp (t varchar(100) null, i integer null, bi bigint "
						 "null, f float null, b boolean null, tt text null, dt timestamp null)" );

#if 0
		std::vector< MyRecord > toInsert =
		{
			MyRecord( std::string( "t1" ), 1, 1234567890123456789, 1.1, true, std::string( "tt1" ), dbclt::datetime( date::sys_days( date::year( 2019 ) / 12 / 9 ) ) ),
			MyRecord( std::string( "t2" ), 2, 2345678901234567891, 2.2, false, std::string( "tt2" ), dbclt::datetime( date::sys_days( date::year( 2019 ) / 12 / 10 ) ) ),
			MyRecord( std::string( "t3" ), 3, 3456789012345678912, 3.3, true, std::string( "tt3" ), dbclt::datetime( date::sys_days( date::year( 2019 ) / 12 / 11 ) ) )
		};

		session.bulk_insert( "tmp", toInsert );

		std::vector< MyRecord > records;
		session.query( "select * from tmp" ).into( records );
		REQUIRE( records.size( ) == 3 );

		for( size_t i = 0; i < records.size( ); ++i )
			REQUIRE( records[ i ] == toInsert[ i ] );
#endif
	}

	SECTION( "Transaction" )
	{
		dbclt::odbc::session session;
		session.connect( connInfo );
		session.execute( "create temp table tmp (t varchar(100) null, i integer null, bi bigint "
						 "null, f float null, b boolean null, tt text null, dt timestamp null)" );

		{
			dbclt::odbc::transaction txn( session );
			session.execute( "insert into tmp values ('t1', 1, 1234567890123456789, 1.1, true, "
							 "'long text', '2019-12-09')" );
			txn.rollback( );

			dbclt::odbc::recordset rs = session.query( "select * from tmp" );
			REQUIRE( rs.record_count( ) == 0 );
		}

		{
			{
				dbclt::odbc::transaction txn( session );
				session.execute( "insert into tmp values ('t1', 1, 1234567890123456789, 1.1, true, "
								 "'long text', '2019-12-09')" );
			}

			dbclt::odbc::recordset rs = session.query( "select * from tmp" );
			REQUIRE( rs.record_count( ) == 0 );
		}

		{
			dbclt::odbc::transaction txn( session );
			session.execute( "insert into tmp values ('t1', 1, 1234567890123456789, 1.1, true, "
							 "'long text', '2019-12-09')" );
			txn.commit( );

			dbclt::odbc::recordset rs = session.query( "select * from tmp" );
			REQUIRE( rs.record_count( ) == 1 );
		}
	}
}
