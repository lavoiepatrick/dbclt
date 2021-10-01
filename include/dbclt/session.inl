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
inline session< BE >::session( ) : m_session( std::make_shared< BE >( ) )
{
}

template< typename BE >
bool session< BE >::connected( ) const
{
	return m_session->connected( );
}

template< typename BE >
void session< BE >::connect( const std::string& connInfo )
{
	m_connInfo = connInfo;
	m_session->connect( m_connInfo );
}

template< typename BE >
inline void session< BE >::reconnect( )
{
	m_session->disconnect( );
	m_session->connect( m_connInfo );
}

template< typename BE >
inline void session< BE >::disconnect( )
{
	m_session->disconnect( );
}

template< typename BE >
inline void session< BE >::begin_transaction( )
{
	m_session->begin_transaction( );
}

template< typename BE >
inline void session< BE >::commit_transaction( )
{
	m_session->commit_transaction( );
}

template< typename BE >
inline void session< BE >::rollback_transaction( )
{
	m_session->rollback_transaction( );
}

template< typename BE >
inline typename session< BE >::result_type session< BE >::execute( const std::string& stmt )
{
	statement_type st( std::make_shared< typename statement_type::backend_type >( *m_session ) );
	return st.execute( stmt );
}

template< typename BE >
template< typename... col_types >
inline typename session< BE >::result_type session< BE >::execute_params( const std::string& stmt,
																		  col_types&&... values )
{
	statement_type st( std::make_shared< typename statement_type::backend_type >( *m_session ) );
	return st.execute_params( stmt, std::forward< col_types >( values )... );
}

template< typename BE >
inline typename session< BE >::recordset_type session< BE >::query( const std::string& stmt )
{
	statement_type st( std::make_shared< typename statement_type::backend_type >( *m_session ) );
	return st.query( stmt );
}

template< typename BE >
template< typename... col_types >
inline typename session< BE >::recordset_type session< BE >::query_params( const std::string& stmt,
																		   col_types&&... values )
{
	statement_type st( std::make_shared< typename statement_type::backend_type >( *m_session ) );
	return st.query_params( stmt, std::forward< col_types >( values )... );
}

template< typename BE >
template< typename T >
inline void session< BE >::bulk_insert( const std::string& table, const T& container )
{
}

template< typename BE >
inline BE& session< BE >::backend( )
{
	return *m_session;
}

}  // namespace dbclt
