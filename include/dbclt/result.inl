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
inline result< backend >::result( result_ptr impl ) : m_impl( impl )
{
}

template< typename backend >
inline size_t result< backend >::affected_count( ) const
{
	return m_impl->affected_count( );
}

template< typename backend >
inline int result< backend >::return_value( ) const
{
	return m_impl->return_value( );
}

template< typename backend >
inline typename result< backend >::recordsets_type result< backend >::recordsets( )
{
	if( !m_recordsets )
	{
		m_recordset = m_impl->create_recordset( );
		m_recordsets = m_impl->create_recordsets( *m_recordset );
	}

	return *m_recordsets;
}

template< typename backend >
inline backend& result< backend >::backend( )
{
	return *m_impl;
}

}  // namespace dbclt
