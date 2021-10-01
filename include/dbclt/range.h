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
template< typename IT >
class range
{
public:
	using iterator = IT;

public:
	range( ) = default;
	range( const iterator& begin, const iterator& end );

	iterator begin( );
	iterator end( );

private:
	iterator m_begin;
	iterator m_end;
};

template< typename IT >
inline range< IT >::range( const iterator& begin, const iterator& end )
	: m_begin( begin ),
	  m_end( end )
{
}

template< typename IT >
inline typename range< IT >::iterator range< IT >::begin( )
{
	return m_begin;
}

template< typename IT >
inline typename range< IT >::iterator range< IT >::end( )
{
	return m_end;
}

}  // namespace dbclt
