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

#include "binder.h"

namespace dbclt
{
template< typename T >
struct type_traits
{
	using value_type = T;
	using bound_type = T;
};

template< typename BE >
class statement
{
public:
	using backend_type = BE;
	using statement_ptr = std::shared_ptr< backend_type >;
	using session_type = typename backend_type::session_type;
	using result_type = typename session_type::result_type;
	using recordset_type = typename session_type::recordset_type;

public:
	statement( session_type session );
	statement( statement_ptr impl );

	result_type execute( const std::string& stmt );

	template< typename T, typename... col_types >
	result_type execute_params( const std::string& stmt, T&& value, col_types&&... values );

	recordset_type query( const std::string& stmt );

	template< typename T, typename... col_types >
	recordset_type query_params( const std::string& stmt, T&& value, col_types&&... values );

private:
	template< typename T >
	auto get_binder( T& v )
	{
		auto t = m_impl->get_binder_traits( v );
		return binder_ref< typename decltype( t )::traits >( v );
	}

	template< typename T >
	auto get_binder( const T& v )
	{
		auto t = m_impl->get_binder_traits( v );
		return binder< typename decltype( t )::traits >( v );
	}

	binder_traits< std::string > get_binder( char* v )
	{
		const std::string str( v );
		return get_binder( v );
	}

	binder_traits< std::string > get_binder( const char* v )
	{
		const std::string str( v );
		return get_binder( v );
	}

private:
	result_type execute_params( const std::string& stmt );
	recordset_type query_params( const std::string& stmt );

private:
	statement_ptr m_impl;
};

}  // namespace dbclt
