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
class recordset
{
public:
	using backend_type = BE;
	using recordset_ptr = std::shared_ptr< BE >;
	using iterator = typename BE::recordset_iterator;
	using record_type = typename BE::recordset_value_type;

public:
	recordset( );

	bool empty( ) const;
	size_t record_count( ) const;

	template< typename T >
	T& into( T& container );

	const fields_type& fields( ) const;

	iterator begin( );
	iterator end( );

	template< typename T >
	T get( size_t field );

public:
	recordset( recordset_ptr impl );

private:
	using record_type_ptr = std::shared_ptr< record_type >;

private:
	recordset_ptr m_impl;
	record_type_ptr m_record;
	iterator m_begin;
	iterator m_end;
};

template< typename BE, typename T >
class recordsets_iterator
{
public:
	using backend_type = BE;
	using value_type = typename BE::recordsets_value_type;

public:
	recordsets_iterator( )
	{
	}

	recordsets_iterator( backend_type* impl, T data ) : m_impl( impl ), m_data( data )
	{
	}

	recordsets_iterator& operator++( )
	{
		m_impl->next_recordset( m_data );
		return *this;
	}

	bool operator!=( const recordsets_iterator& other ) const
	{
		return m_data != other.m_data;
	}

	bool operator==( const recordsets_iterator& other ) const
	{
		return m_data == other.m_data;
	}

	value_type& operator*( )
	{
		return m_impl->get_recordset( m_data );
	}

private:
	backend_type* m_impl { nullptr };
	T m_data;
};

template< typename BE, typename T >
class recordset_iterator
{
public:
	using recordset_type = recordset< BE >;
	using backend_type = BE;
	using value_type = typename BE::recordset_value_type;
	using value_type_ptr = std::shared_ptr< value_type >;

public:
	recordset_iterator( )
	{
	}

	recordset_iterator( backend_type* impl, T data ) : m_impl( impl ), m_data( data )
	{
	}

	recordset_iterator( backend_type* impl, value_type_ptr record, T data )
		: m_impl( impl ),
		  m_data( data ),
		  m_record( record )
	{
	}

	recordset_iterator& operator++( )
	{
		m_impl->next_record( m_data );
		return *this;
	}

	bool operator!=( const recordset_iterator& other ) const
	{
		return m_data != other.m_data;
	}

	bool operator==( const recordset_iterator& other ) const
	{
		return m_data == other.m_data;
	}

	value_type& operator*( )
	{
		return m_impl->get_record( *m_record, m_data );
	}

	value_type* operator->( )
	{
		return &m_impl->get_record( *m_record, m_data );
	}

private:
	backend_type* m_impl { nullptr };
	T m_data;
	value_type_ptr m_record;
};

}  // namespace dbclt
