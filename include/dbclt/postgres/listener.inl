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

#ifndef WIN32

namespace dbclt
{
namespace postgres
{
inline listener::listener( conn_ptr conn ) : m_conn( conn )
{
}

inline bool listener::operator( )( )
{
	m_notify = PQnotifies( m_conn.get( ) );
	if( m_notify )
		return true;

	// wait for input
	int socket = PQsocket( m_conn.get( ) );
	if( socket < 0 )
		throw data_exception( "Cannot open listening socket" );

	fd_set inputMask;
	FD_ZERO( &inputMask );
	FD_SET( socket, &inputMask );

	if( select( socket + 1, &inputMask, nullptr, nullptr, nullptr ) < 0 )
		throw data_exception( std::string( "Cannot read on listening socket: " ) +
							  strerror( errno ) );

	PQconsumeInput( m_conn.get( ) );
	return true;
}

}  // namespace postgres
}  // namespace dbclt

#endif
