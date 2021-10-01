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
namespace odbc
{
template< typename Handle, SQLSMALLINT Type >
class sql_handle
{
public:
	sql_handle( ) : m_handle( SQL_NULL_HANDLE )
	{
	}

	sql_handle( Handle h ) : m_handle( h )
	{
	}

	~sql_handle( );

	Handle release( );
	Handle& handle( )
	{
		return m_handle;
	}
	operator Handle( )
	{
		return m_handle;
	}
	SQLSMALLINT type( ) const
	{
		return Type;
	}

private:
	Handle m_handle;
};

using env_ptr = std::shared_ptr< sql_handle< HENV, SQL_HANDLE_ENV > >;
using conn_ptr = std::shared_ptr< sql_handle< HDBC, SQL_HANDLE_DBC > >;
using statement_ptr = std::shared_ptr< sql_handle< HSTMT, SQL_HANDLE_STMT > >;

template< typename Handle, SQLSMALLINT Type >
inline sql_handle< Handle, Type >::~sql_handle( )
{
	if( m_handle != SQL_NULL_HANDLE )
		SQLFreeHandle( Type, m_handle );
}

template< typename Handle, SQLSMALLINT Type >
inline Handle sql_handle< Handle, Type >::release( )
{
	Handle tmp = m_handle;
	m_handle = SQL_NULL_HANDLE;
	return tmp;
}

}  // namespace odbc
}  // namespace dbclt
