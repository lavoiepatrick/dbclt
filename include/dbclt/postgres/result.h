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
class result;

template< typename BE >
class recordsets;

template< typename BE, typename T >
class recordsets_iterator;

template< typename BE >
class recordset;

template< typename BE, typename T >
class recordset_iterator;

namespace postgres
{
class result_impl : public std::enable_shared_from_this< result_impl >
{
public:
	using recordsets_value_type = dbclt::recordset< result_impl >;
	using recordsets_iterator = dbclt::recordsets_iterator< result_impl, recordsets_value_type* >;
	using recordsets_type = dbclt::range< recordsets_iterator >;

	using recordset_value_type = dbclt::record< result_impl >;
	using recordset_iterator = dbclt::recordset_iterator< result_impl, size_t >;
	using recordset_type = dbclt::recordset< result_impl >;

public:
	result_impl( )
	{
	}

	static std::shared_ptr< result_impl > create( result_ptr result );

	size_t affected_count( ) const;
	int return_value( ) const;
	size_t record_count( ) const;

	recordsets_type create_recordsets( recordset_type& firstRecordset );
	void next_recordset( recordsets_value_type*& data );
	recordsets_value_type& get_recordset( recordsets_value_type* data );

	recordset_type create_recordset( );
	void next_record( size_t& data );
	recordset_value_type& get_record( recordset_value_type& record, size_t data );

	recordset_value_type* create_record( );
	recordset_iterator begin( std::shared_ptr< recordset_value_type > record );
	recordset_iterator end( );

	const fields_type& record_fields( ) const;

	bool is_null( size_t field ) const;

	std::string get_string( size_t ndxField ) const;
	bool get_bool( size_t ndxField ) const;
	int8_t get_int8( size_t ndxField ) const;
	int16_t get_int16( size_t ndxField ) const;
	int32_t get_int32( size_t ndxField ) const;
	int64_t get_int64( size_t ndxField ) const;
	float get_float( size_t ndxField ) const;
	double get_double( size_t ndxField ) const;
	datetime get_date( size_t ndxField ) const;
	datetime get_datetime( size_t ndxField ) const;

private:
	void init( std::shared_ptr< result_impl > ptr, result_ptr result );

	const void* data( size_t field ) const;

	static field::type map_db_type( Oid type );

private:
	result_ptr m_stmtResult;
	fields_type m_fields;
	size_t m_affectedRecords { 0 };
	size_t m_records { 0 };
	size_t m_current { 0 };
};

}  // namespace postgres
}  // namespace dbclt
