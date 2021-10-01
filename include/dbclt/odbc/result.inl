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
namespace odbc
{
inline std::shared_ptr< result_impl > result_impl::create( statement_ptr stmt )
{
	std::shared_ptr< result_impl > ptr( std::make_shared< result_impl >( ) );
	ptr->init( ptr, stmt );
	return ptr;
}

inline void result_impl::check_stmt_error( SQLRETURN rc )
{
	session_impl::check_stmt_error( rc, *m_stmt );
}

inline void result_impl::init( std::shared_ptr< result_impl > ptr, statement_ptr stmt )
{
	m_stmt = stmt;

	SQLLEN rowCount;
	check_stmt_error( SQLRowCount( *m_stmt, &rowCount ) );

	m_eof = true;
	m_records = rowCount;
}

inline size_t result_impl::affected_count( ) const
{
	return m_records;
}

inline size_t result_impl::record_count( ) const
{
	return m_records;
}

inline std::string result_impl::get_messages( )
{
	return session_impl::get_messages( m_stmt->handle( ), m_stmt->type( ) );
}

inline const fields_type& result_impl::record_fields( ) const
{
	return m_fields;
}

inline result_impl::recordset_type result_impl::create_recordset( )
{
	init_record_data( );
	return recordset_type( shared_from_this( ) );
}

inline result_impl::recordset_value_type* result_impl::create_record( )
{
	return new recordset_value_type( shared_from_this( ) );
}

inline result_impl::recordset_iterator
result_impl::begin( std::shared_ptr< recordset_value_type > record )
{
	return recordset_iterator( this, record, !m_eof );
}

inline result_impl::recordset_iterator result_impl::end( )
{
	return recordset_iterator( this, false );
}

inline result_impl::recordsets_type result_impl::create_recordsets( recordset_type& firstRecordset )
{
	return recordsets_type( recordsets_iterator( this, &firstRecordset ),
							recordsets_iterator( this, nullptr ) );
}

inline void result_impl::next_recordset( recordsets_value_type*& data )
{
	data = nullptr;
}

inline result_impl::recordsets_value_type& result_impl::get_recordset( recordsets_value_type* data )
{
	return *data;
}

inline void result_impl::next_record( bool& data )
{
	SQLRETURN rc = SQLFetch( *m_stmt );
	if( rc == SQL_NO_DATA )
		data = false;
	else
	{
		check_stmt_error( rc );
		get_record_data( );
	}
}

inline result_impl::recordset_value_type& result_impl::get_record( recordset_value_type& record,
																   bool /*data*/ )
{
	return record;
}

inline bool result_impl::is_null( size_t field ) const
{
	if( field >= m_fields.size( ) )
		throw data_exception( "Invalid field index" );

	return m_recordData[ field ].dataSize == SQL_NULL_DATA;
}

inline std::string result_impl::get_string( size_t field ) const
{
	static const std::string empty;
	const std::vector< char >& binding =
		m_strBindings[ data( field, m_strBindings ).ndxValue ].second;
	return binding.empty( ) ? empty : std::string( &*binding.begin( ) );
}

inline bool result_impl::get_bool( size_t field ) const
{
	return m_int8Bindings[ data( field, m_int8Bindings ).ndxValue ] != 0;
}

inline int8_t result_impl::get_int8( size_t field ) const
{
	return m_int8Bindings[ data( field, m_int8Bindings ).ndxValue ];
}

inline int16_t result_impl::get_int16( size_t field ) const
{
	return m_int16Bindings[ data( field, m_int16Bindings ).ndxValue ];
}

inline int32_t result_impl::get_int32( size_t field ) const
{
	return m_int32Bindings[ data( field, m_int32Bindings ).ndxValue ];
}

inline int64_t result_impl::get_int64( size_t field ) const
{
	return m_int64Bindings[ data( field, m_int64Bindings ).ndxValue ];
}

inline float result_impl::get_float( size_t field ) const
{
	return ( float )m_dblBindings[ data( field, m_dblBindings ).ndxValue ];
}

inline double result_impl::get_double( size_t field ) const
{
	return m_dblBindings[ data( field, m_dblBindings ).ndxValue ];
}

inline datetime result_impl::get_datetime( size_t field ) const
{
	return m_tsBindings[ data( field, m_tsBindings ).ndxValue ].second.date;
}

inline void result_impl::init_record_data( )
{
	m_recordData.clear( );

	SQLSMALLINT fields;
	check_stmt_error( SQLNumResultCols( *m_stmt, &fields ) );

	std::vector< col_desc > colDescs( fields );

	m_fields.resize( fields );
	m_recordData.resize( fields );
	for( SQLSMALLINT i = 0; i < fields; ++i )
	{
		col_desc& cd = colDescs[ i ];
		cd.pos = i + 1;
		check_stmt_error( SQLDescribeCol( *m_stmt,
										  cd.pos,
										  cd.columnName,
										  sizeof( cd.columnName ),
										  &cd.nameLength,
										  &cd.dataType,
										  &cd.columnSize,
										  &cd.decimalDigits,
										  &cd.nullable ) );

		grow_typed_binding( cd.dataType );
	}

	for( SQLSMALLINT i = 0; i < fields; ++i )
	{
		col_desc& cd = colDescs[ i ];
		m_fields[ i ] = field( ( const char* )cd.columnName, map_db_type( cd.dataType ) );

		init_typed_binding( cd );

		if( m_recordData[ i ].bound )
		{
			check_stmt_error( SQLBindCol( *m_stmt,
										  cd.pos,
										  m_recordData[ i ].cDataType,
										  m_recordData[ i ].value,
										  m_recordData[ i ].dataSize,
										  ( SQLLEN* )&m_recordData[ i ].dataSize ) );
		}
	}

	SQLRETURN rc = SQLFetch( *m_stmt );
	if( rc == SQL_NO_DATA )
		m_eof = true;
	else
	{
		check_stmt_error( rc );
		m_eof = false;
		get_record_data( );
	}
}

inline void result_impl::get_record_data( )
{
	get_record_data_string( );
	get_record_data_binary( );
	get_record_data_timestamp( );
}

inline void result_impl::get_record_data_string( )
{
	constexpr size_t chunkSize = 64 * 1024;
	// constexpr size_t chunkSize = 5;
	constexpr size_t strChunkSize = chunkSize - 1;
	using buffer = std::array< char, chunkSize >;
	using buffers = std::list< buffer >;

	buffers bufs;
	for( std::pair< size_t, std::vector< char > >& binding: m_strBindings )
	{
		record_data& recordData = m_recordData[ binding.first ];
		if( !recordData.bound )
		{
			size_t totalSize = 0;

			bufs.clear( );
			SQLRETURN rc;
			do
			{
				recordData.dataSize = chunkSize;
				bufs.resize( bufs.size( ) + 1 );
				rc = SQLGetData( *m_stmt,
								 ( SQLUSMALLINT )binding.first + 1,
								 recordData.cDataType,
								 ( SQLPOINTER )bufs.back( ).data( ),
								 chunkSize,
								 ( SQLLEN* )&recordData.dataSize );
				check_stmt_error( rc );

				if( recordData.dataSize == SQL_NULL_DATA || rc == SQL_NO_DATA )
					break;

				if( recordData.dataSize > strChunkSize )
					totalSize += strChunkSize;
				else
					totalSize += recordData.dataSize;
			} while( rc != SQL_SUCCESS );

			if( recordData.dataSize != SQL_NULL_DATA )
			{
				binding.second.resize( totalSize + 1 );
				recordData.dataSize = totalSize;

				size_t offset = 0;
				for( const buffer& buf: bufs )
					if( totalSize - offset > 0 )
					{
						memcpy( &binding.second[ offset ],
								buf.data( ),
								std::min( strChunkSize, totalSize - offset ) );
						offset += strChunkSize;
					}

				binding.second[ totalSize ] = '\0';
			}
		}
	}
}

inline void result_impl::get_record_data_binary( )
{
	constexpr size_t chunkSize = 64 * 1024;
	using buffer = std::array< char, chunkSize >;
	using buffers = std::list< buffer >;

	buffers bufs;
	for( std::pair< size_t, std::vector< char > >& binding: m_binBindings )
	{
		record_data& recordData = m_recordData[ binding.first ];
		if( !recordData.bound )
		{
			size_t totalSize = 0;

			bufs.clear( );
			SQLRETURN rc = SQL_NULL_DATA;
			do
			{
				bufs.resize( bufs.size( ) + 1 );
				rc = SQLGetData( *m_stmt,
								 ( SQLUSMALLINT )binding.first + 1,
								 recordData.cDataType,
								 ( SQLPOINTER )bufs.back( ).data( ),
								 chunkSize,
								 ( SQLLEN* )&recordData.dataSize );
				check_stmt_error( rc );

				if( recordData.dataSize == SQL_NULL_DATA || recordData.dataSize == SQL_NO_DATA )
					break;

				totalSize += recordData.dataSize;
			} while( true );

			if( recordData.dataSize != SQL_NULL_DATA )
			{
				binding.second.resize( totalSize );

				size_t offset = 0;
				for( const buffer& buf: bufs )
				{
					memcpy( &binding.second[ offset ],
							buf.data( ),
							std::min( chunkSize, totalSize - offset ) );
					offset += chunkSize;
				}
			}
		}
	}
}

inline void result_impl::get_record_data_timestamp( )
{
	for( std::pair< size_t, timestamp_binding >& binding: m_tsBindings )
	{
		SQL_TIMESTAMP_STRUCT& ts = binding.second.ts;
		binding.second.date = datetime(
			date::sys_days( date::year( ts.year ) / ts.month / ts.day ) +
			std::chrono::hours( ( int )ts.hour ) + std::chrono::minutes( ( int )ts.minute ) +
			std::chrono::seconds( ( int )ts.second ) + std::chrono::microseconds( ts.fraction ) );
	}
}

template< typename binding_type >
inline const result_impl::record_data& result_impl::data( size_t field,
														  const binding_type& bindings ) const
{
	if( field >= m_fields.size( ) )
		throw data_exception( "Invalid field index: " + std::to_string( field ) );

	if( m_recordData[ field ].typedBindings != &bindings )
		throw data_exception( "Invalid field type for field: " + std::to_string( field ) );

	if( m_recordData[ field ].ndxValue >= bindings.size( ) )
		throw data_exception( "Out of bound field:" + std::to_string( field ) );

	return m_recordData[ field ];
}

inline SQLSMALLINT result_impl::target_type( SQLSMALLINT sqlType ) const
{
	switch( sqlType )
	{
	case SQL_CHAR:
	case SQL_VARCHAR:
	case SQL_LONGVARCHAR: return SQL_C_CHAR;
	case SQL_BIT: return SQL_C_BIT;
	case SQL_TINYINT: return SQL_C_STINYINT;
	case SQL_SMALLINT: return SQL_C_SSHORT;
	case SQL_INTEGER: return SQL_C_SLONG;
	case SQL_BIGINT: return SQL_C_SBIGINT;
	case SQL_REAL: return SQL_C_FLOAT;
	case SQL_DECIMAL:
	case SQL_NUMERIC:
	case SQL_FLOAT:
	case SQL_DOUBLE: return SQL_C_DOUBLE;
	case SQL_BINARY:
	case SQL_VARBINARY:
	case SQL_LONGVARBINARY: return SQL_C_BINARY;
	case SQL_TYPE_DATE: return SQL_C_TYPE_DATE;
	case SQL_TYPE_TIME: return SQL_C_TYPE_TIME;
	case SQL_TYPE_TIMESTAMP: return SQL_C_TYPE_TIMESTAMP;
	default: return SQL_C_CHAR;
	}
}

template< typename binding_type >
inline void result_impl::set_typed_binding( const col_desc& cd,
											bool bound,
											SQLSMALLINT cDataType,
											binding_type& bindings )
{
	bindings.emplace_back( );
	m_recordData[ cd.pos - 1 ] = { true,      cDataType,           cd.columnSize, &bindings.back( ),
								   &bindings, bindings.size( ) - 1 };
}

template<>
inline void
result_impl::set_typed_binding( const col_desc& cd,
								bool bound,
								SQLSMALLINT cDataType,
								std::vector< std::pair< size_t, std::vector< char > > >& bindings )
{
	bindings.emplace_back( cd.pos - 1, bound ? cd.columnSize : 0 );
	m_recordData[ cd.pos - 1 ] = {
		bound,         cDataType,
		cd.columnSize, bound ? &*bindings.back( ).second.begin( ) : nullptr,
		&bindings,     bindings.size( ) - 1
	};
}

template<>
inline void
result_impl::set_typed_binding( const col_desc& cd,
								bool bound,
								SQLSMALLINT cDataType,
								std::vector< std::pair< size_t, timestamp_binding > >& bindings )
{
	bindings.emplace_back( cd.pos - 1, timestamp_binding( ) );
	m_recordData[ cd.pos - 1 ] = { true,          cDataType,
								   cd.columnSize, &bindings.back( ).second.ts,
								   &bindings,     bindings.size( ) - 1 };
}

template< typename binding_type >
inline void result_impl::grow_typed_binding( binding_type& bindings )
{
	bindings.reserve( bindings.capacity( ) + 1 );
}

inline void result_impl::grow_typed_binding( SQLSMALLINT dataType )
{
	switch( dataType )
	{
	case SQL_CHAR:
	case SQL_VARCHAR:
	case SQL_LONGVARCHAR: grow_typed_binding( m_strBindings ); break;

	case SQL_BIT:
	case SQL_TINYINT: grow_typed_binding( m_int8Bindings ); break;

	case SQL_SMALLINT: grow_typed_binding( m_int16Bindings ); break;

	case SQL_INTEGER: grow_typed_binding( m_int32Bindings ); break;

	case SQL_BIGINT: grow_typed_binding( m_int64Bindings ); break;

	case SQL_REAL:
	case SQL_DECIMAL:
	case SQL_NUMERIC:
	case SQL_FLOAT:
	case SQL_DOUBLE: grow_typed_binding( m_dblBindings ); break;

	case SQL_BINARY:
	case SQL_VARBINARY:
	case SQL_LONGVARBINARY: grow_typed_binding( m_binBindings ); break;

	case SQL_TYPE_DATE:
	case SQL_TYPE_TIME:
	case SQL_TYPE_TIMESTAMP: grow_typed_binding( m_tsBindings ); break;

	default: grow_typed_binding( m_strBindings );
	}
}

inline void result_impl::init_typed_binding( col_desc& cd )
{
	switch( cd.dataType )
	{
	case SQL_CHAR:
	case SQL_VARCHAR:
		cd.columnSize++;
		set_typed_binding( cd, true, SQL_C_CHAR, m_strBindings );
		break;

	case SQL_LONGVARCHAR:
		cd.columnSize++;
		set_typed_binding( cd, false, SQL_C_CHAR, m_strBindings );
		break;

	case SQL_BIT: set_typed_binding( cd, true, SQL_C_BIT, m_int8Bindings ); break;

	case SQL_TINYINT: set_typed_binding( cd, true, SQL_C_STINYINT, m_int8Bindings ); break;

	case SQL_SMALLINT: set_typed_binding( cd, true, SQL_C_SSHORT, m_int16Bindings ); break;

	case SQL_INTEGER: set_typed_binding( cd, true, SQL_C_SLONG, m_int32Bindings ); break;

	case SQL_BIGINT: set_typed_binding( cd, true, SQL_C_SBIGINT, m_int64Bindings ); break;

	case SQL_REAL:
	case SQL_DECIMAL:
	case SQL_NUMERIC:
	case SQL_FLOAT:
	case SQL_DOUBLE: set_typed_binding( cd, true, SQL_C_DOUBLE, m_dblBindings ); break;

	case SQL_BINARY:
	case SQL_VARBINARY: set_typed_binding( cd, true, SQL_C_BINARY, m_binBindings ); break;

	case SQL_LONGVARBINARY: set_typed_binding( cd, false, SQL_C_BINARY, m_binBindings ); break;

	case SQL_TYPE_DATE:
	case SQL_TYPE_TIME:
	case SQL_TYPE_TIMESTAMP:
		set_typed_binding( cd, true, SQL_C_TYPE_TIMESTAMP, m_tsBindings );
		break;

	default: set_typed_binding( cd, true, SQL_C_CHAR, m_strBindings );
	}
}

inline field::type result_impl::map_db_type( SQLSMALLINT dataType )
{
	switch( dataType )
	{
	case SQL_CHAR:
	case SQL_VARCHAR:
	case SQL_LONGVARCHAR: return field::type::db_string;

	case SQL_BIT: return field::type::db_bool;

	case SQL_TINYINT: return field::type::db_int8;

	case SQL_SMALLINT: return field::type::db_int16;

	case SQL_INTEGER: return field::type::db_int32;

	case SQL_BIGINT: return field::type::db_int64;

	case SQL_REAL:
	case SQL_DECIMAL:
	case SQL_NUMERIC:
	case SQL_FLOAT:
	case SQL_DOUBLE: return field::type::db_double;

	case SQL_BINARY:
	case SQL_VARBINARY:
	case SQL_LONGVARBINARY: return field::type::db_binary;

	case SQL_TYPE_DATE:
	case SQL_TYPE_TIME:
	case SQL_TYPE_TIMESTAMP: return field::type::db_datetime;

	default: return field::type::db_string;
	}
}

}  // namespace odbc
}  // namespace dbclt
