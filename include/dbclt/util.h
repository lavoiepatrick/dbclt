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
namespace util
{
void swap16( void* value );
void swap32( void* value );
void swap64( void* value );

template< typename T >
T big_to_native16( T value );

template< typename T >
T big_to_native32( T value );

template< typename T >
T big_to_native64( T value );

inline void swap16( void* value )
{
	union
	{
		uint16_t v;
		unsigned char bytes[ 2 ];
	} src, dest;

	src.v = *( uint16_t* )value;
	dest.bytes[ 0 ] = src.bytes[ 1 ];
	dest.bytes[ 1 ] = src.bytes[ 0 ];
	*( uint16_t* )value = dest.v;
}

inline void swap32( void* value )
{
	union
	{
		uint32_t v;
		unsigned char bytes[ 4 ];
	} src, dest;

	src.v = *( uint32_t* )value;
	dest.bytes[ 0 ] = src.bytes[ 3 ];
	dest.bytes[ 1 ] = src.bytes[ 2 ];
	dest.bytes[ 2 ] = src.bytes[ 1 ];
	dest.bytes[ 3 ] = src.bytes[ 0 ];
	*( uint32_t* )value = dest.v;
}

inline void swap64( void* value )
{
	union
	{
		uint64_t v;
		unsigned char bytes[ 8 ];
	} src, dest;

	src.v = *( uint64_t* )value;
	dest.bytes[ 0 ] = src.bytes[ 7 ];
	dest.bytes[ 1 ] = src.bytes[ 6 ];
	dest.bytes[ 2 ] = src.bytes[ 5 ];
	dest.bytes[ 3 ] = src.bytes[ 4 ];
	dest.bytes[ 4 ] = src.bytes[ 3 ];
	dest.bytes[ 5 ] = src.bytes[ 2 ];
	dest.bytes[ 6 ] = src.bytes[ 1 ];
	dest.bytes[ 7 ] = src.bytes[ 0 ];
	*( uint64_t* )value = dest.v;
}

template< typename T >
inline T big_to_native16( T value )
{
	swap16( &value );
	return value;
}

template< typename T >
inline T big_to_native32( T value )
{
	swap32( &value );
	return value;
}

template< typename T >
inline T big_to_native64( T value )
{
	swap64( &value );
	return value;
}

}  // namespace util
}  // namespace dbclt
