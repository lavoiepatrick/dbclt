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
template< typename T >
struct binder_traits
{
	using user_type = T;
	using bound_type = T;
	using traits = binder_traits< T >;

	static bound_type transform( user_type from )
	{
		return from;
	}
};

template< typename BT >
class binder
{
public:
	using user_type = typename BT::user_type;
	using bound_type = typename BT::bound_type;

public:
	binder( const user_type& value ) : m_bound( BT::transform( value ) )
	{
	}

	~binder( )
	{
	}

	bound_type& value( )
	{
		return m_bound;
	}

private:
	bound_type m_bound;
};

template< typename BT >
class binder_ref
{
public:
	using user_type = typename BT::user_type;
	using bound_type = typename BT::bound_type;

public:
	binder_ref( user_type& value ) : m_value( &value ), m_bound( BT::transform( value ) )
	{
	}

	~binder_ref( )
	{
		*m_value = BT::transform( m_bound );
	}

	bound_type& value( )
	{
		return m_bound;
	}

private:
	user_type* m_value;
	bound_type m_bound;
};

}  // namespace dbclt
