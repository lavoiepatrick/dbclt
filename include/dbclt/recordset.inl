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

#include <structs/to_tuple.h>

namespace dbclt
{
namespace detail
{
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
struct get_value< BE, int8_t >
{
	int8_t operator( )( const dbclt::record< BE >& from, size_t ndx )
	{
		return from.get_int8( ndx );
	}
};

template< typename BE >
struct get_value< BE, uint8_t >
{
	uint8_t operator( )( const dbclt::record< BE >& from, size_t ndx )
	{
		return from.get_int8( ndx );
	}
};

template< typename BE >
struct get_value< BE, int16_t >
{
	int16_t operator( )( const dbclt::record< BE >& from, size_t ndx )
	{
		return from.get_int16( ndx );
	}
};

template< typename BE >
struct get_value< BE, uint16_t >
{
	uint16_t operator( )( const dbclt::record< BE >& from, size_t ndx )
	{
		return from.get_int16( ndx );
	}
};

template< typename BE >
struct get_value< BE, int32_t >
{
	int32_t operator( )( const dbclt::record< BE >& from, size_t ndx )
	{
		return from.get_int32( ndx );
	}
};

template< typename BE >
struct get_value< BE, uint32_t >
{
	uint32_t operator( )( const dbclt::record< BE >& from, size_t ndx )
	{
		return from.get_int32( ndx );
	}
};

template< typename BE >
struct get_value< BE, int64_t >
{
	int64_t operator( )( const dbclt::record< BE >& from, size_t ndx )
	{
		return from.get_int64( ndx );
	}
};

template< typename BE >
struct get_value< BE, uint64_t >
{
	uint64_t operator( )( const dbclt::record< BE >& from, size_t ndx )
	{
		return from.get_int64( ndx );
	}
};

template< typename BE >
struct get_value< BE, double >
{
	double operator( )( const dbclt::record< BE >& from, size_t ndx )
	{
		return from.get_double( ndx );
	}
};

template< typename BE >
struct get_value< BE, bool >
{
	bool operator( )( const dbclt::record< BE >& from, size_t ndx )
	{
		return from.get_bool( ndx );
	}
};

template< typename BE >
struct get_value< BE, dbclt::datetime >
{
	dbclt::datetime operator( )( const dbclt::record< BE >& from, size_t ndx )
	{
		return from.get_datetime( ndx );
	}
};

template< typename BE, typename T >
struct get_value< BE, std::optional< T > >
{
	std::optional< T > operator( )( const dbclt::record< BE >& from, size_t ndx )
	{
		if( from.is_null( ndx ) )
			return std::optional< T >( );
		return get_value< BE, T >( )( from, ndx );
	}
};

template< typename BE, typename T, typename Tup, std::size_t... I >
T convert( const dbclt::record< BE >& from, std::index_sequence< I... > )
{
	return T { get_value< BE, typename std::tuple_element< I, Tup >::type >( )( from, I )... };
}
}  // namespace detail

template< typename BE, typename T >
void dbclt::convert( const dbclt::record< BE >& from, T& to )
{
	using Tup = decltype( structs::to_tuple( to ) );
	to = detail::convert< BE, T, Tup >( from,
										std::make_index_sequence< std::tuple_size_v< Tup > > { } );
}

template< typename BE >
inline recordset< BE >::recordset( )
{
}

template< typename BE >
inline recordset< BE >::recordset( recordset_ptr impl )
	: m_impl( impl ),
	  m_record( impl->create_record( ) ),
	  m_begin( impl->begin( m_record ) ),
	  m_end( impl->end( ) )
{
}

template< typename BE >
inline size_t recordset< BE >::record_count( ) const
{
	return m_impl->record_count( );
}

template< typename BE >
inline bool recordset< BE >::empty( ) const
{
	return m_begin == m_end;
}

template< typename BE >
template< typename T >
inline T& recordset< BE >::into( T& container )
{
	for( const typename BE::recordset_value_type& rec: *this )
	{
		typename T::value_type converted;
		convert( rec, converted );
		container.emplace_back( std::move( converted ) );
	}

	return container;
}

template< typename BE >
inline const fields_type& recordset< BE >::fields( ) const
{
	return m_impl->record_fields( );
}

template< typename BE >
inline typename recordset< BE >::iterator recordset< BE >::begin( )
{
	return m_begin;
}

template< typename BE >
inline typename recordset< BE >::iterator recordset< BE >::end( )
{
	return m_end;
}

}  // namespace dbclt
