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
class result;

template< typename BE >
class statement;

namespace postgres
{
class session_impl;

class statement_impl
{
public:
	using session_type = dbclt::session< session_impl >;
	using result_type = result< result_impl >;
	using recordset_type = typename result_type::recordset_type;

public:
	statement_impl( session_impl& session );

	result_type execute( const std::string& stmt );
	result_type execute_params( const std::string& stmt );
	recordset_type query( const std::string& stmt );
	recordset_type query_params( const std::string& stmt );

	template< typename T1, typename T2 >
	void bind_parameter( const T1& value, T2& bound );

	template< typename T >
	auto get_binder_traits( const T& )
	{
		return binder_traits< T >( );
	}

private:
	using paramTypes = std::vector< Oid >;
	using paramValues = std::vector< const char* >;
	using paramLengths = std::vector< int >;
	using paramFormats = std::vector< int >;

private:
	conn_ptr m_conn;
	result_ptr m_result;
	paramTypes m_paramTypes;
	paramValues m_paramValues;
	paramLengths m_paramLengths;
	paramFormats m_paramFormats;
};

using statement = dbclt::statement< statement_impl >;

}  // namespace postgres
}  // namespace dbclt
