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

namespace odbc
{
class result_impl : public std::enable_shared_from_this< result_impl >
{
public:
	using recordsets_value_type = dbclt::recordset< result_impl >;
	using recordsets_iterator = dbclt::recordsets_iterator< result_impl, recordsets_value_type* >;
	using recordsets_type = dbclt::range< recordsets_iterator >;

	using recordset_value_type = dbclt::record< result_impl >;
	using recordset_iterator = dbclt::recordset_iterator< result_impl, bool >;
	using recordset_type = dbclt::recordset< result_impl >;

public:
	result_impl( ) = default;
	static std::shared_ptr< result_impl > create( statement_ptr stmt );

	size_t affected_count( ) const;
	int return_value( ) const;
	size_t record_count( ) const;

	recordsets_type create_recordsets( recordset_type& firstRecordset );
	void next_recordset( recordsets_value_type*& data );
	recordsets_value_type& get_recordset( recordsets_value_type* data );

	recordset_type create_recordset( );
	void next_record( bool& data );
	recordset_value_type& get_record( recordset_value_type& record, bool data );

	recordset_value_type* create_record( );
	recordset_iterator begin( std::shared_ptr< recordset_value_type > record );
	recordset_iterator end( );

	std::string get_messages( );

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
	datetime get_datetime( size_t ndxField ) const;

private:
	struct col_desc
	{
		SQLUSMALLINT pos;
		SQLCHAR columnName[ 2048 ];
		SQLSMALLINT nameLength;
		SQLSMALLINT dataType;
		SQLULEN columnSize;
		SQLSMALLINT decimalDigits;
		SQLSMALLINT nullable;
	};

	struct record_data
	{
		bool bound;
		SQLSMALLINT cDataType;
		SQLULEN dataSize;
		void* value;
		void* typedBindings;
		size_t ndxValue;
	};
	std::vector< int8_t > m_int8Bindings;
	std::vector< int16_t > m_int16Bindings;
	std::vector< int32_t > m_int32Bindings;
	std::vector< int64_t > m_int64Bindings;
	std::vector< double > m_dblBindings;
	struct timestamp_binding
	{
		SQL_TIMESTAMP_STRUCT ts;
		datetime date;
	};
	std::vector< std::pair< size_t, timestamp_binding > > m_tsBindings;

	std::vector< std::pair< size_t, std::vector< char > > > m_strBindings;
	std::vector< std::pair< size_t, std::vector< char > > > m_binBindings;

private:
	void check_stmt_error( SQLRETURN rc );
	void init( std::shared_ptr< result_impl > ptr, statement_ptr stmt );

	void init_record_data( );
	void get_record_data( );
	void get_record_data_string( );
	void get_record_data_binary( );
	void get_record_data_timestamp( );

	template< typename binding_type >
	const record_data& data( size_t field, const binding_type& bindings ) const;

	SQLSMALLINT target_type( SQLSMALLINT sqlType ) const;

	template< typename binding_type >
	void grow_typed_binding( binding_type& bindings );
	void grow_typed_binding( SQLSMALLINT sqlType );
	void init_typed_binding( col_desc& cd );

	static field::type map_db_type( SQLSMALLINT dataType );

	template< typename binding_type >
	void set_typed_binding( const col_desc& cd,
							bool bound,
							SQLSMALLINT cDataType,
							binding_type& bindings );

private:
	statement_ptr m_stmt;
	fields_type m_fields;
	std::vector< record_data > m_recordData;

	size_t m_records { 0 };
	bool m_eof { false };
};

}  // namespace odbc
}  // namespace dbclt
