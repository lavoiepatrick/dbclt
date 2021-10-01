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
inline session_impl::~session_impl( )
{
	disconnect( );
}

inline void session_impl::begin_transaction( )
{
	if( m_transaction )
		throw data_exception( "Transaction already started" );

	check_dbc_error( SQLSetConnectAttr( *m_dbc, SQL_ATTR_AUTOCOMMIT, ( SQLPOINTER ) false, 0 ) );
	m_transaction = true;
}

inline void session_impl::commit_transaction( )
{
	if( m_transaction )
	{
		check_dbc_error( SQLEndTran( SQL_HANDLE_DBC, *m_dbc, SQL_COMMIT ) );
		check_dbc_error( SQLSetConnectAttr( *m_dbc, SQL_ATTR_AUTOCOMMIT, ( SQLPOINTER ) true, 0 ) );
	}

	m_transaction = false;
}

inline void session_impl::rollback_transaction( )
{
	if( m_transaction )
	{
		check_dbc_error( SQLEndTran( SQL_HANDLE_DBC, *m_dbc, SQL_ROLLBACK ) );
		check_dbc_error( SQLSetConnectAttr( *m_dbc, SQL_ATTR_AUTOCOMMIT, ( SQLPOINTER ) true, 0 ) );
	}

	m_transaction = false;
}

template< typename HANDLE >
inline void session_impl::alloc_handle( HANDLE& handle, SQLSMALLINT handleType, SQLHANDLE input )
{
	typename HANDLE::element_type h;
	const SQLRETURN rc = SQLAllocHandle( handleType, input, &h.handle( ) );
	handle.reset( new typename HANDLE::element_type( h.release( ) ) );

	if( !handle || handleType == SQL_HANDLE_ENV )
		check_error( rc, *handle, handleType );
	else if( handleType == SQL_HANDLE_STMT )
		check_error( rc, input, SQL_HANDLE_DBC );
	else if( handleType == SQL_HANDLE_DBC )
		check_error( rc, input, SQL_HANDLE_ENV );
}

inline std::string session_impl::get_messages( SQLHANDLE h, SQLSMALLINT handleType )
{
	std::string msg;

	SQLRETURN ret;
	do
	{
		SQLSMALLINT i = 0;
		SQLINTEGER native;
		SQLCHAR state[ 10 ];
		SQLCHAR text[ 2048 ];
		SQLSMALLINT len;
		ret = SQLGetDiagRec( handleType, h, ++i, state, &native, text, sizeof( text ), &len );
		;
		while( SQL_SUCCEEDED( ret ) )
		{
			if( !msg.empty( ) )
				msg += "\n";
			msg += std::string( "[" ) + ( const char* )state + "] " + std::to_string( native ) +
				   " - " + ( const char* )text;
			ret = SQLGetDiagRec( handleType, h, ++i, state, &native, text, sizeof( text ), &len );
		}
		if( handleType == SQL_HANDLE_STMT )
			ret = SQLMoreResults( h );
	} while( SQL_SUCCEEDED( ret ) );

	return msg;
}

inline void session_impl::check_error( SQLRETURN rc, SQLHANDLE h, SQLSMALLINT handleType )
{
	if( rc == SQL_INVALID_HANDLE )
		throw data_exception( "Invalid handle" );

	if( rc == SQL_ERROR )
	{
		std::string msg;

		SQLSMALLINT i = 0;
		SQLINTEGER native;
		SQLCHAR state[ 10 ];
		SQLCHAR text[ 2048 ];
		SQLSMALLINT len;
		SQLRETURN ret;
		do
		{
			ret = SQLGetDiagRec( handleType, h, ++i, state, &native, text, sizeof( text ), &len );
			if( SQL_SUCCEEDED( ret ) )
			{
				if( i > 1 )
					msg += "\n";
				msg += std::string( "[" ) + ( const char* )state + "] " + std::to_string( native ) +
					   " - " + ( const char* )text;
			}
		} while( ret == SQL_SUCCESS );
		throw data_exception( msg );
	}
}

inline void session_impl::check_dbc_error( SQLRETURN rc, HDBC dbc )
{
	check_error( rc, dbc, SQL_HANDLE_DBC );
}

inline void session_impl::check_stmt_error( SQLRETURN rc, HSTMT stmt )
{
	check_error( rc, stmt, SQL_HANDLE_STMT );
}

inline void session_impl::check_env_error( SQLRETURN rc )
{
	check_error( rc, *m_env, SQL_HANDLE_ENV );
}

inline void session_impl::check_dbc_error( SQLRETURN rc )
{
	check_error( rc, *m_dbc, SQL_HANDLE_DBC );
}

inline bool session_impl::connected( ) const
{
	return m_dbc && *m_dbc != SQL_NULL_HANDLE;
}

inline void session_impl::connect( const std::string& connInfo )
{
	try
	{
		alloc_handle( m_env, SQL_HANDLE_ENV, SQL_NULL_HANDLE );
		check_env_error(
			SQLSetEnvAttr( *m_env, SQL_ATTR_ODBC_VERSION, ( SQLPOINTER* )SQL_OV_ODBC3, 0 ) );

		alloc_handle( m_dbc, SQL_HANDLE_DBC, *m_env );
		check_dbc_error( SQLSetConnectAttr( *m_dbc, SQL_LOGIN_TIMEOUT, ( SQLPOINTER )5, 0 ) );
		check_dbc_error( SQLSetConnectAttr( *m_dbc, SQL_ATTR_AUTOCOMMIT, ( SQLPOINTER ) true, 0 ) );

		SQLCHAR co[ 16000 ] = { 0 };
		SQLSMALLINT r;
		check_dbc_error( SQLDriverConnectA( *m_dbc,
											nullptr,
											( SQLCHAR* )connInfo.c_str( ),
											SQL_NTS,
											co,
											sizeof( co ),
											&r,
											SQL_DRIVER_NOPROMPT ) );
		m_connectionString = ( const char* )co;
	}
	catch( const std::exception& )
	{
		m_env.reset( );
		m_dbc.reset( );
		throw;
	}
}

inline const std::string& session_impl::connectionString( ) const
{
	return m_connectionString;
}

inline void session_impl::disconnect( )
{
	if( m_dbc && *m_dbc != SQL_NULL_HANDLE )
	{
		check_dbc_error( SQLDisconnect( *m_dbc ) );
		m_dbc.reset( );
		m_env.reset( );
	}
}

inline session_impl::result_type session_impl::execute( const std::string& stmtText )
{
	if( !connected( ) )
		throw data_exception( "Connection is not opened" );

	statement_ptr stmt;
	alloc_handle( stmt, SQL_HANDLE_STMT, *m_dbc );
	check_stmt_error( SQLExecDirect( *stmt, ( SQLCHAR* )stmtText.c_str( ), SQL_NTS ), *stmt );

	return result_type( result_impl::create( stmt ) );
}

inline session_impl::recordset_type session_impl::query( const std::string& stmtText )
{
	if( !connected( ) )
		throw data_exception( "Connection is not opened" );

	statement_ptr stmt;
	alloc_handle( stmt, SQL_HANDLE_STMT, *m_dbc );
	check_stmt_error( SQLExecDirect( *stmt, ( SQLCHAR* )stmtText.c_str( ), SQL_NTS ), *stmt );

	result_type result( result_impl::create( stmt ) );
	return *result.recordsets( ).begin( );
}

}  // namespace odbc
}  // namespace dbclt
