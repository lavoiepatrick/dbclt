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
template< typename BE >
inline statement< BE >::statement( session_type session )
	: m_impl( std::make_shared< backend_type >( session.backend( ) ) )
{
}

template< typename BE >
inline statement< BE >::statement( statement_ptr impl ) : m_impl( impl )
{
}

template< typename BE >
inline typename statement< BE >::result_type statement< BE >::execute( const std::string& stmt )
{
	return m_impl->execute( stmt );
}

template< typename BE >
template< typename T, typename... col_types >
inline typename statement< BE >::result_type
statement< BE >::execute_params( const std::string& stmt, T&& value, col_types&&... values )
{
	auto binder = get_binder< T >( std::forward< T >( value ) );
	m_impl->bind_parameter( value, binder.value( ) );
	return execute_params( stmt, values... );
}

template< typename BE >
inline typename statement< BE >::result_type
statement< BE >::execute_params( const std::string& stmt )
{
	return m_impl->execute_params( stmt );
}

template< typename BE >
inline typename statement< BE >::recordset_type statement< BE >::query( const std::string& stmt )
{
	return m_impl->query( stmt );
}

template< typename BE >
template< typename T, typename... col_types >
inline typename statement< BE >::recordset_type
statement< BE >::query_params( const std::string& stmt, T&& value, col_types&&... values )
{
	auto binder = get_binder( std::forward< T >( value ) );
	m_impl->bind_parameter( value, binder.value( ) );
	return query_params( stmt, std::forward< col_types >( values )... );
}

template< typename BE >
inline typename statement< BE >::recordset_type
statement< BE >::query_params( const std::string& stmt )
{
	return m_impl->query_params( stmt );
}

}  // namespace dbclt
