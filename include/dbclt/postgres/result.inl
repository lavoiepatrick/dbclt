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

namespace dbclt
{
namespace postgres
{
inline std::shared_ptr< result_impl > result_impl::create( result_ptr result )
{
	std::shared_ptr< result_impl > ptr( std::make_shared< result_impl >( ) );
	ptr->init( ptr, result );
	return ptr;
}

inline void result_impl::init( std::shared_ptr< result_impl > ptr, result_ptr result )
{
	const size_t fields = PQnfields( result.get( ) );
	m_fields.resize( fields );
	for( size_t i = 0; i < fields; ++i )
		m_fields[ i ] =
			field( PQfname( result.get( ), i ), map_db_type( PQftype( result.get( ), i ) ) );

	m_affectedRecords = atoi( PQcmdTuples( result.get( ) ) );
	m_records = PQntuples( result.get( ) );
	m_stmtResult = result;
}

inline size_t result_impl::affected_count( ) const
{
	return m_affectedRecords;
}

inline size_t result_impl::record_count( ) const
{
	return m_records;
}

inline const fields_type& result_impl::record_fields( ) const
{
	return m_fields;
}

inline result_impl::recordset_value_type* result_impl::create_record( )
{
	return new recordset_value_type( shared_from_this( ) );
}

inline result_impl::recordset_iterator
result_impl::begin( std::shared_ptr< recordset_value_type > record )
{
	return recordset_iterator( this, record, 0 );
}

inline result_impl::recordset_iterator result_impl::end( )
{
	return recordset_iterator( this, m_records );
}

inline result_impl::recordset_type result_impl::create_recordset( )
{
	return recordset_type( shared_from_this( ) );
}

inline result_impl::recordsets_type result_impl::create_recordsets( recordset_type& firstRecordset )
{
	return recordsets_type( recordsets_iterator( this, &firstRecordset ),
							recordsets_iterator( this, nullptr ) );
}

inline void result_impl::next_recordset( recordsets_value_type*& data )
{
	data = nullptr;
}

inline result_impl::recordsets_value_type& result_impl::get_recordset( recordsets_value_type* data )
{
	return *data;
}

inline void result_impl::next_record( size_t& data )
{
	data = ++m_current;
}

inline result_impl::recordset_value_type& result_impl::get_record( recordset_value_type& record,
																   size_t data )
{
	return record;
}

inline bool result_impl::is_null( size_t field ) const
{
	return PQgetisnull( m_stmtResult.get( ), m_current, field );
}

inline std::string result_impl::get_string( size_t field ) const
{
	return std::string( ( const char* )data( field ) );
}

inline bool result_impl::get_bool( size_t field ) const
{
	return *( bool* )data( field );
}

inline int8_t result_impl::get_int8( size_t field ) const
{
	return get_int32( field );
}

inline int16_t result_impl::get_int16( size_t field ) const
{
	return get_int32( field );
}

inline int32_t result_impl::get_int32( size_t field ) const
{
	return util::big_to_native32( *( int32_t* )data( field ) );
}

inline int64_t result_impl::get_int64( size_t field ) const
{
	return util::big_to_native64( *( int64_t* )data( field ) );
}

inline float result_impl::get_float( size_t field ) const
{
	return get_double( field );
}

inline double result_impl::get_double( size_t field ) const
{
	return util::big_to_native64( *( double* )data( field ) );
}

inline datetime result_impl::get_date( size_t field ) const
{
	static const date::sys_days pgEpoch( date::year( 2000 ) / 1 / 1 );
#if POSTGRESQL_INTEGER_DATETIME
	// for servers built with the integer_datetime option
	const int32_t ts = get_int32( field );
	return datetime( pgEpoch + std::chrono::hours( ts ) * 24 );
#else
	// for servers built without the integer_datetime option
	const double ts = get_double( field );
	const int64_t secs = ( int64_t )ts;
	const int64_t usecs = ( int64_t )( ( ts - secs ) * 1000000 );
	return datetime( pgEpoch + std::chrono::seconds( secs ) + std::chrono::microseconds( usecs ) );
#endif
}

inline datetime result_impl::get_datetime( size_t field ) const
{
	static const date::sys_days pgEpoch( date::year( 2000 ) / 1 / 1 );
#if POSTGRESQL_INTEGER_DATETIME
	// for servers built with the integer_datetime option
	const int64_t ts = get_int64( field );
	return datetime( pgEpoch + std::chrono::microseconds( ts ) );
#else
	// for servers built without the integer_datetime option
	const double ts = get_double( field );
	const int64_t secs = ( int64_t )ts;
	const int64_t usecs = ( int64_t )( ( ts - secs ) * 1000000 );
	return datetime( pgEpoch + std::chrono::seconds( secs ) + std::chrono::microseconds( usecs ) );
#endif
}

inline const void* result_impl::data( size_t field ) const
{
	if( field >= m_fields.size( ) )
		throw data_exception( "Invalid field index" );
	return PQgetvalue( m_stmtResult.get( ), m_current, field );
}

inline field::type result_impl::map_db_type( Oid type )
{
	switch( type )
	{
	case 25:    // text
	case 1043:  // varchar
	case 2275:  // cstring
	case 18:    // char
	case 1042:  // bpchar
	case 142:   // xml
	case 114:   // json
		return field::type::db_string;

	case 17:  // bytea
		return field::type::db_binary;

	case 1082:  // date
		return field::type::db_date;

	case 702:   // abstime
	case 703:   // reltime
	case 1083:  // time
	case 1114:  // timestamp
	case 1184:  // timestamptz
	case 1266:  // timetz
		return field::type::db_datetime;

	case 700:   // float4
	case 701:   // float8
	case 1700:  // numeric
		return field::type::db_double;

	case 16:  // bool
		return field::type::db_bool;

	case 21:  // int2
		return field::type::db_int16;

	case 23:  // int4
	case 26:  // oid
		return field::type::db_int32;

	case 20:  // int8
		return field::type::db_int64;

	default: return field::type::db_string;
	}
}

}  // namespace postgres
}  // namespace dbclt
