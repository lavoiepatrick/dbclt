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
inline session_impl::~session_impl( )
{
	disconnect( );
}

inline void session_impl::begin_transaction( )
{
	if( ++m_transaction == 1 )
		execute( "begin" );
	else
		execute( "savepoint txn" + std::to_string( m_transaction ) );
}

inline void session_impl::commit_transaction( )
{
	if( --m_transaction == 0 )
		execute( "commit" );
}

inline void session_impl::rollback_transaction( )
{
	if( m_transaction > 1 )
		execute( "rollback to savepoint txn" + std::to_string( m_transaction ) );
	else
		execute( "rollback" );

	m_transaction--;
}

inline bool session_impl::connected( ) const
{
	return m_conn.get( );
}

inline void session_impl::connect( const std::string& connInfo )
{
	conn_ptr conn( PQconnectdb( connInfo.c_str( ) ) );
	if( PQstatus( conn.get( ) ) != CONNECTION_OK )
		throw data_exception( "Connection to database failed: " + connInfo + " -- " +
							  PQerrorMessage( conn.get( ) ) );
	m_conn = conn;
}

inline void session_impl::disconnect( )
{
	m_conn.reset( );
}

inline session_impl::result_type session_impl::execute( const std::string& stmt )
{
	if( !m_conn )
		throw data_exception( "Connection is not opened" );

	result_ptr res( PQexec( m_conn.get( ), stmt.c_str( ) ) );
	if( PQresultStatus( res.get( ) ) != PGRES_COMMAND_OK )
		throw data_exception( "Command failed: " + stmt + " -- " +
							  PQerrorMessage( m_conn.get( ) ) );

	return result_type( result_impl::create( res ) );
}

#ifndef WIN32
inline listener session_impl::listen( const std::string& table )
{
	execute( "listen " + table );
	return listener( m_conn );
}
#endif
}  // namespace postgres
}  // namespace dbclt
