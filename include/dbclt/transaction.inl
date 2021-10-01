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
template< typename backend >
inline transaction< backend >::transaction( session_type& session ) : m_session( session )
{
	m_session.begin_transaction( );
	m_inTransaction = true;
}

template< typename backend >
inline transaction< backend >::~transaction( )
{
	if( m_inTransaction )
		rollback( );
}

template< typename backend >
inline void transaction< backend >::commit( )
{
	if( !m_inTransaction )
		throw data_exception( "Cannot commit transaction, already handled." );
	m_session.commit_transaction( );
	m_inTransaction = false;
}

template< typename backend >
inline void transaction< backend >::rollback( )
{
	if( !m_inTransaction )
		throw data_exception( "Cannot rollback transaction, already handled." );
	m_session.rollback_transaction( );
	m_inTransaction = false;
}

}  // namespace dbclt
