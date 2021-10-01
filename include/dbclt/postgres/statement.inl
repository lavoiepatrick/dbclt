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

#pragma once

#include "../util.h"

namespace dbclt
{
template<>
struct type_traits< datetime >
{
	using value_type = datetime;
	using bound_type = int64_t;
};

namespace postgres
{
inline statement_impl::statement_impl( session_impl& session ) : m_conn( session.m_conn )
{
}

inline statement_impl::result_type statement_impl::execute( const std::string& stmt )
{
	if( !m_conn )
		throw data_exception( "Connection is not opened" );

	result_ptr res( PQexec( m_conn.get( ), stmt.c_str( ) ) );
	ExecStatusType status = PQresultStatus( res.get( ) );
	if( status != PGRES_COMMAND_OK )
		throw data_exception( "Command failed: " + stmt + " -- " + PQresStatus( status ) + " -- " +
							  PQerrorMessage( m_conn.get( ) ) );

	return result_type( result_impl::create( res ) );
}

inline statement_impl::result_type statement_impl::execute_params( const std::string& stmt )
{
	if( !m_conn )
		throw data_exception( "Connection is not opened" );

	result_ptr res( PQexecParams( m_conn.get( ),
								  stmt.c_str( ),
								  m_paramTypes.size( ),
								  &*m_paramTypes.begin( ),
								  &*m_paramValues.begin( ),
								  &*m_paramLengths.begin( ),
								  &*m_paramFormats.begin( ),
								  0 ) );
	ExecStatusType status = PQresultStatus( res.get( ) );
	if( status != PGRES_COMMAND_OK )
		throw data_exception( "Command failed: " + stmt + " -- " + PQresStatus( status ) + " -- " +
							  PQerrorMessage( m_conn.get( ) ) );

	return result_type( result_impl::create( res ) );
}

inline statement_impl::recordset_type statement_impl::query( const std::string& stmt )
{
	if( !m_conn )
		throw data_exception( "Connection is not opened" );

	result_ptr res(
		PQexecParams( m_conn.get( ), stmt.c_str( ), 0, nullptr, nullptr, nullptr, nullptr, 1 ) );
	ExecStatusType status = PQresultStatus( res.get( ) );
	if( status != PGRES_TUPLES_OK )
		throw data_exception( "Command failed: " + stmt + " -- " + PQresStatus( status ) + " -- " +
							  PQerrorMessage( m_conn.get( ) ) );

	result_type result( result_impl::create( res ) );
	return *result.recordsets( ).begin( );
}

inline statement_impl::recordset_type statement_impl::query_params( const std::string& stmt )
{
	if( !m_conn )
		throw data_exception( "Connection is not opened" );

	result_ptr res( PQexecParams( m_conn.get( ),
								  stmt.c_str( ),
								  m_paramTypes.size( ),
								  &*m_paramTypes.begin( ),
								  &*m_paramValues.begin( ),
								  &*m_paramLengths.begin( ),
								  &*m_paramFormats.begin( ),
								  1 ) );
	ExecStatusType status = PQresultStatus( res.get( ) );
	if( status != PGRES_TUPLES_OK )
		throw data_exception( "Command failed: " + stmt + " -- " + PQresStatus( status ) + " -- " +
							  PQerrorMessage( m_conn.get( ) ) );

	result_type result( result_impl::create( res ) );
	return *result.recordsets( ).begin( );
}

template<>
inline void statement_impl::bind_parameter( const std::string& value, std::string& bound )
{
	m_paramTypes.emplace_back( 1043 );
	m_paramValues.emplace_back( bound.c_str( ) );
	m_paramLengths.emplace_back( ( int )bound.length( ) );
	m_paramFormats.emplace_back( 0 );
}

template<>
inline void statement_impl::bind_parameter( const std::string_view& value, std::string_view& bound )
{
	m_paramTypes.emplace_back( 1043 );
	m_paramValues.emplace_back( bound.data( ) );
	m_paramLengths.emplace_back( ( int )bound.length( ) );
	m_paramFormats.emplace_back( 0 );
}

template<>
inline void statement_impl::bind_parameter( const bool& value, bool& bound )
{
	m_paramTypes.emplace_back( 16 );
	m_paramValues.emplace_back( ( const char* )&bound );
	m_paramLengths.emplace_back( ( int )sizeof( bound ) );
	m_paramFormats.emplace_back( 1 );
}

template<>
inline void statement_impl::bind_parameter( const int8_t& value, int8_t& bound )
{
	m_paramTypes.emplace_back( 20 );
	m_paramValues.emplace_back( ( const char* )&bound );
	m_paramLengths.emplace_back( ( int )sizeof( bound ) );
	m_paramFormats.emplace_back( 1 );
}

template<>
inline void statement_impl::bind_parameter( const int16_t& value, int16_t& bound )
{
	m_paramTypes.emplace_back( 21 );
	m_paramValues.emplace_back( ( const char* )( const void* )&bound );
	m_paramLengths.emplace_back( ( int )sizeof( bound ) );
	m_paramFormats.emplace_back( 1 );
}

template<>
inline void statement_impl::bind_parameter( const int32_t& value, int32_t& bound )
{
	m_paramTypes.emplace_back( 23 );
	m_paramValues.emplace_back( ( const char* )( const void* )&bound );
	m_paramLengths.emplace_back( ( int )sizeof( bound ) );
	m_paramFormats.emplace_back( 1 );
}

template<>
inline void statement_impl::bind_parameter( const int64_t& value, int64_t& bound )
{
	m_paramTypes.emplace_back( 20 );
	m_paramValues.emplace_back( ( const char* )( const void* )&bound );
	m_paramLengths.emplace_back( ( int )sizeof( bound ) );
	m_paramFormats.emplace_back( 1 );
}

template<>
inline void statement_impl::bind_parameter( const float& value, float& bound )
{
	m_paramTypes.emplace_back( 700 );
	m_paramValues.emplace_back( ( const char* )( const void* )&bound );
	m_paramLengths.emplace_back( ( int )sizeof( bound ) );
	m_paramFormats.emplace_back( 1 );
}

template<>
inline void statement_impl::bind_parameter( const double& value, double& bound )
{
	m_paramTypes.emplace_back( 701 );
	m_paramValues.emplace_back( ( const char* )( const void* )&bound );
	m_paramLengths.emplace_back( ( int )sizeof( bound ) );
	m_paramFormats.emplace_back( 1 );
}

template<>
inline void statement_impl::bind_parameter( const datetime& value, int64_t& bound )
{
	m_paramTypes.emplace_back( 1114 );
	m_paramValues.emplace_back( ( const char* )( const void* )&bound );
	m_paramLengths.emplace_back( ( int )sizeof( bound ) );
	m_paramFormats.emplace_back( 1 );
}

template<>
inline auto statement_impl::get_binder_traits( const int32_t& )
{
	struct binder_traits
	{
		using user_type = int32_t;
		using bound_type = int32_t;
		using traits = binder_traits;

		static int32_t transform( int32_t from )
		{
			return util::big_to_native32( from );
		}
	};

	return binder_traits( );
}

template<>
inline auto statement_impl::get_binder_traits( const int64_t& )
{
	struct binder_traits
	{
		using user_type = int64_t;
		using bound_type = int64_t;
		using traits = binder_traits;

		static int64_t transform( int64_t from )
		{
			return util::big_to_native64( from );
		}
	};

	return binder_traits( );
}

template<>
inline auto statement_impl::get_binder_traits( const float& )
{
	struct binder_traits
	{
		using user_type = float;
		using bound_type = float;
		using traits = binder_traits;

		static float transform( float from )
		{
			return util::big_to_native32( from );
		}
	};

	return binder_traits( );
}

template<>
inline auto statement_impl::get_binder_traits( const double& )
{
	struct binder_traits
	{
		using user_type = double;
		using bound_type = double;
		using traits = binder_traits;

		static double transform( double from )
		{
			return util::big_to_native64( from );
		}
	};

	return binder_traits( );
}

template<>
inline auto statement_impl::get_binder_traits( const datetime& )
{
	struct binder_traits
	{
		using user_type = datetime;
		using bound_type = int64_t;
		using traits = binder_traits;

		static int64_t transform( datetime from )
		{
			static const date::sys_days pgEpoch( date::year( 2000 ) / 1 / 1 );
			return util::big_to_native64( ( from - pgEpoch ).count( ) );
		}
		static datetime transform( int64_t from )
		{
			static const date::sys_days pgEpoch( date::year( 2000 ) / 1 / 1 );
			return pgEpoch + std::chrono::microseconds( util::big_to_native64( from ) );
		}
	};

	return binder_traits( );
}

}  // namespace postgres
}  // namespace dbclt
