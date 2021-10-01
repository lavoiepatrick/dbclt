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

#include "statement.h"

namespace dbclt
{
template< typename BE >
class record;

template< typename BE >
class transaction;

template< typename BE >
class session;

namespace odbc
{
class session_impl
{
public:
	using result_type = result< result_impl >;
	using recordset_type = result_impl::recordset_type;
	using statement_type = dbclt::statement< statement_impl >;
	using transaction_type = dbclt::transaction< session_impl >;

public:
	session_impl( ) = default;
	~session_impl( );

	bool connected( ) const;
	void connect( const std::string& connInfo );
	const std::string& connectionString( ) const;
	void disconnect( );

	void begin_transaction( );
	void commit_transaction( );
	void rollback_transaction( );

	result_type execute( const std::string& stmt );
	recordset_type query( const std::string& stmt );

	static std::string get_messages( SQLHANDLE h, SQLSMALLINT handleType );

private:
	template< typename HANDLE >
	void alloc_handle( HANDLE& handle, SQLSMALLINT handleType, SQLHANDLE input );

	static void check_error( SQLRETURN rc, SQLHANDLE h, SQLSMALLINT handleType );
	static void check_dbc_error( SQLRETURN rc, HDBC dbc );
	static void check_stmt_error( SQLRETURN rc, HSTMT stmt );
	void check_env_error( SQLRETURN rc );
	void check_dbc_error( SQLRETURN rc );

private:
	env_ptr m_env;
	conn_ptr m_dbc;
	bool m_transaction { false };
	std::string m_connectionString;

	friend result_impl;
	friend statement_impl;
};

using session = dbclt::session< session_impl >;
using transaction = session_impl::transaction_type;
using statement = session_impl::statement_type;
using result = session_impl::result_type;
using recordset = dbclt::recordset< result_impl >;
using record = dbclt::record< result_impl >;

}  // namespace odbc
}  // namespace dbclt
