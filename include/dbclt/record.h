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

#include "datetime.h"

#include <optional>

namespace dbclt
{
template< typename BE >
class record
{
public:
	using backend_type = BE;
	using record_ptr = std::shared_ptr< backend_type >;

public:
	record( ) = default;
	record( record_ptr impl );

	const fields_type& fields( ) const;
	size_t field_index( const std::string& nameField ) const;

	bool is_null( size_t ndxField ) const;
	bool is_null( const std::string& nameField ) const;

	std::string get_as_string( size_t ndxField ) const;
	std::string get_as_string( const std::string& nameField ) const;

	std::string get_string( size_t ndxField ) const;
	std::string get_string( const std::string& nameField ) const;

	bool get_bool( size_t ndxField ) const;
	bool get_bool( const std::string& nameField ) const;

	int8_t get_int8( size_t ndxField ) const;
	int8_t get_int8( const std::string& nameField ) const;

	int16_t get_int16( size_t ndxField ) const;
	int16_t get_int16( const std::string& nameField ) const;

	int32_t get_int32( size_t ndxField ) const;
	int32_t get_int32( const std::string& nameField ) const;

	int64_t get_int64( size_t ndxField ) const;
	int64_t get_int64( const std::string& nameField ) const;

	float get_float( size_t ndxField ) const;
	float get_float( const std::string& nameField ) const;

	double get_double( size_t ndxField ) const;
	double get_double( const std::string& nameField ) const;

	datetime get_date( size_t ndxField ) const;
	datetime get_date( const std::string& nameField ) const;

	datetime get_datetime( size_t ndxField ) const;
	datetime get_datetime( const std::string& nameField ) const;

	std::optional< std::string > get_nullable_as_string( size_t ndxField ) const;
	std::optional< std::string > get_nullable_as_string( const std::string& nameField ) const;

	std::optional< std::string > get_nullable_string( size_t ndxField ) const;
	std::optional< std::string > get_nullable_string( const std::string& nameField ) const;

	std::optional< bool > get_nullable_bool( size_t ndxField ) const;
	std::optional< bool > get_nullable_bool( const std::string& nameField ) const;

	std::optional< int8_t > get_nullable_int8( size_t ndxField ) const;
	std::optional< int8_t > get_nullable_int8( const std::string& nameField ) const;

	std::optional< int16_t > get_nullable_int16( size_t ndxField ) const;
	std::optional< int16_t > get_nullable_int16( const std::string& nameField ) const;

	std::optional< int32_t > get_nullable_int32( size_t ndxField ) const;
	std::optional< int32_t > get_nullable_int32( const std::string& nameField ) const;

	std::optional< int64_t > get_nullable_int64( size_t ndxField ) const;
	std::optional< int64_t > get_nullable_int64( const std::string& nameField ) const;

	std::optional< float > get_nullable_float( size_t ndxField ) const;
	std::optional< float > get_nullable_float( const std::string& nameField ) const;

	std::optional< double > get_nullable_double( size_t ndxField ) const;
	std::optional< double > get_nullable_double( const std::string& nameField ) const;

	std::optional< datetime > get_nullable_date( size_t ndxField ) const;
	std::optional< datetime > get_nullable_date( const std::string& nameField ) const;

	std::optional< datetime > get_nullable_datetime( size_t ndxField ) const;
	std::optional< datetime > get_nullable_datetime( const std::string& nameField ) const;

private:
	record_ptr m_impl;
};

template< typename BE, typename T >
void convert( const record< BE >& from, T& to );

template< typename T, typename BE >
inline T convert( const record< BE >& from )
{
	T to;
	convert( from, to );
	return to;
}

template< typename BE >
inline record< BE >::record( record_ptr impl ) : m_impl( impl )
{
}

template< typename BE >
inline const fields_type& record< BE >::fields( ) const
{
	return m_impl->record_fields( );
}

template< typename BE >
inline size_t record< BE >::field_index( const std::string& nameField ) const
{
	size_t ndx = 0;
	for( const field& fld: m_impl->record_fields( ) )
	{
		if( fld.name( ) == nameField )
			return ndx;
		++ndx;
	}

	throw data_exception( "Invalid field name: " + nameField );
}

template< typename BE >
inline bool record< BE >::is_null( size_t ndxField ) const
{
	return m_impl->is_null( ndxField );
}

template< typename BE >
inline bool record< BE >::is_null( const std::string& nameField ) const
{
	return is_null( field_index( nameField ) );
}

template< typename BE >
inline std::string record< BE >::get_as_string( size_t ndxField ) const
{
	if( fields( )[ ndxField ].db_type( ) == field::type::db_bool )
		return std::to_string( m_impl->get_bool( ndxField ) );
	else if( fields( )[ ndxField ].db_type( ) == field::type::db_int8 )
		return std::to_string( m_impl->get_int8( ndxField ) );
	else if( fields( )[ ndxField ].db_type( ) == field::type::db_int16 )
		return std::to_string( m_impl->get_int16( ndxField ) );
	else if( fields( )[ ndxField ].db_type( ) == field::type::db_int32 )
		return std::to_string( m_impl->get_int32( ndxField ) );
	else if( fields( )[ ndxField ].db_type( ) == field::type::db_int64 )
		return std::to_string( m_impl->get_int64( ndxField ) );
	else if( fields( )[ ndxField ].db_type( ) == field::type::db_float )
		return std::to_string( m_impl->get_float( ndxField ) );
	else if( fields( )[ ndxField ].db_type( ) == field::type::db_double )
		return std::to_string( m_impl->get_double( ndxField ) );
	else if( fields( )[ ndxField ].db_type( ) == field::type::db_date )
		return date::format( "%F", m_impl->get_date( ndxField ) );
	else if( fields( )[ ndxField ].db_type( ) == field::type::db_datetime )
		return date::format( "%FT%T", m_impl->get_datetime( ndxField ) );
	return m_impl->get_string( ndxField );
}

template< typename BE >
inline std::string record< BE >::get_as_string( const std::string& nameField ) const
{
	return get_as_string( field_index( nameField ) );
}

template< typename BE >
inline std::string record< BE >::get_string( size_t ndxField ) const
{
	return m_impl->get_string( ndxField );
}

template< typename BE >
inline std::string record< BE >::get_string( const std::string& nameField ) const
{
	return get_string( field_index( nameField ) );
}

template< typename BE >
inline bool record< BE >::get_bool( size_t ndxField ) const
{
	return m_impl->get_bool( ndxField );
}

template< typename BE >
inline bool record< BE >::get_bool( const std::string& nameField ) const
{
	return get_bool( field_index( nameField ) );
}

template< typename BE >
inline int8_t record< BE >::get_int8( size_t ndxField ) const
{
	return m_impl->get_int8( ndxField );
}

template< typename BE >
inline int8_t record< BE >::get_int8( const std::string& nameField ) const
{
	return get_int8( field_index( nameField ) );
}

template< typename BE >
inline int16_t record< BE >::get_int16( size_t ndxField ) const
{
	return m_impl->get_int16( ndxField );
}

template< typename BE >
inline int16_t record< BE >::get_int16( const std::string& nameField ) const
{
	return get_int16( field_index( nameField ) );
}

template< typename BE >
inline int32_t record< BE >::get_int32( size_t ndxField ) const
{
	return m_impl->get_int32( ndxField );
}

template< typename BE >
inline int32_t record< BE >::get_int32( const std::string& nameField ) const
{
	return get_int32( field_index( nameField ) );
}

template< typename BE >
inline int64_t record< BE >::get_int64( size_t ndxField ) const
{
	return m_impl->get_int64( ndxField );
}

template< typename BE >
inline int64_t record< BE >::get_int64( const std::string& nameField ) const
{
	return get_int64( field_index( nameField ) );
}

template< typename BE >
inline float record< BE >::get_float( size_t ndxField ) const
{
	return m_impl->get_float( ndxField );
}

template< typename BE >
inline float record< BE >::get_float( const std::string& nameField ) const
{
	return get_float( field_index( nameField ) );
}

template< typename BE >
inline double record< BE >::get_double( size_t ndxField ) const
{
	return m_impl->get_double( ndxField );
}

template< typename BE >
inline double record< BE >::get_double( const std::string& nameField ) const
{
	return get_double( field_index( nameField ) );
}

template< typename BE >
inline datetime record< BE >::get_date( size_t ndxField ) const
{
	return m_impl->get_date( ndxField );
}

template< typename BE >
inline datetime record< BE >::get_date( const std::string& nameField ) const
{
	return get_date( field_index( nameField ) );
}

template< typename BE >
inline datetime record< BE >::get_datetime( size_t ndxField ) const
{
	return m_impl->get_datetime( ndxField );
}

template< typename BE >
inline datetime record< BE >::get_datetime( const std::string& nameField ) const
{
	return get_datetime( field_index( nameField ) );
}

template< typename BE >
inline std::optional< std::string > record< BE >::get_nullable_as_string( size_t ndxField ) const
{
	if( is_null( ndxField ) )
		return std::optional< std::string >( );
	return get_as_string( ndxField );
}

template< typename BE >
inline std::optional< std::string >
record< BE >::get_nullable_as_string( const std::string& nameField ) const
{
	return get_nullable_as_string( field_index( nameField ) );
}

template< typename BE >
inline std::optional< std::string > record< BE >::get_nullable_string( size_t ndxField ) const
{
	if( is_null( ndxField ) )
		return std::optional< std::string >( );
	return get_string( ndxField );
}

template< typename BE >
inline std::optional< std::string >
record< BE >::get_nullable_string( const std::string& nameField ) const
{
	return get_nullable_string( field_index( nameField ) );
}

template< typename BE >
inline std::optional< bool > record< BE >::get_nullable_bool( size_t ndxField ) const
{
	if( is_null( ndxField ) )
		return std::optional< bool >( );
	return get_bool( ndxField );
}

template< typename BE >
inline std::optional< bool > record< BE >::get_nullable_bool( const std::string& nameField ) const
{
	return get_nullable_bool( field_index( nameField ) );
}

template< typename BE >
inline std::optional< int8_t > record< BE >::get_nullable_int8( size_t ndxField ) const
{
	if( is_null( ndxField ) )
		return std::optional< int8_t >( );
	return get_int8( ndxField );
}

template< typename BE >
inline std::optional< int8_t > record< BE >::get_nullable_int8( const std::string& nameField ) const
{
	return get_nullable_int8( field_index( nameField ) );
}

template< typename BE >
inline std::optional< int16_t > record< BE >::get_nullable_int16( size_t ndxField ) const
{
	if( is_null( ndxField ) )
		return std::optional< int16_t >( );
	return get_int16( ndxField );
}

template< typename BE >
inline std::optional< int16_t >
record< BE >::get_nullable_int16( const std::string& nameField ) const
{
	return get_nullable_int16( field_index( nameField ) );
}

template< typename BE >
inline std::optional< int32_t > record< BE >::get_nullable_int32( size_t ndxField ) const
{
	if( is_null( ndxField ) )
		return std::optional< int32_t >( );
	return get_int32( ndxField );
}

template< typename BE >
inline std::optional< int32_t >
record< BE >::get_nullable_int32( const std::string& nameField ) const
{
	return get_nullable_int32( field_index( nameField ) );
}

template< typename BE >
inline std::optional< int64_t > record< BE >::get_nullable_int64( size_t ndxField ) const
{
	if( is_null( ndxField ) )
		return std::optional< int64_t >( );
	return get_int64( ndxField );
}

template< typename BE >
inline std::optional< int64_t >
record< BE >::get_nullable_int64( const std::string& nameField ) const
{
	return get_nullable_int64( field_index( nameField ) );
}

template< typename BE >
inline std::optional< float > record< BE >::get_nullable_float( size_t ndxField ) const
{
	if( is_null( ndxField ) )
		return std::optional< float >( );
	return get_float( ndxField );
}

template< typename BE >
inline std::optional< float > record< BE >::get_nullable_float( const std::string& nameField ) const
{
	return get_nullable_float( field_index( nameField ) );
}

template< typename BE >
inline std::optional< double > record< BE >::get_nullable_double( size_t ndxField ) const
{
	if( is_null( ndxField ) )
		return std::optional< double >( );
	return get_double( ndxField );
}

template< typename BE >
inline std::optional< double >
record< BE >::get_nullable_double( const std::string& nameField ) const
{
	return get_nullable_double( field_index( nameField ) );
}

template< typename BE >
inline std::optional< datetime > record< BE >::get_nullable_date( size_t ndxField ) const
{
	if( is_null( ndxField ) )
		return std::optional< datetime >( );
	return get_date( ndxField );
}

template< typename BE >
inline std::optional< datetime >
record< BE >::get_nullable_date( const std::string& nameField ) const
{
	return get_nullable_date( field_index( nameField ) );
}

template< typename BE >
inline std::optional< datetime > record< BE >::get_nullable_datetime( size_t ndxField ) const
{
	if( is_null( ndxField ) )
		return std::optional< datetime >( );
	return get_datetime( ndxField );
}

template< typename BE >
inline std::optional< datetime >
record< BE >::get_nullable_datetime( const std::string& nameField ) const
{
	return get_nullable_datetime( field_index( nameField ) );
}

}  // namespace dbclt
