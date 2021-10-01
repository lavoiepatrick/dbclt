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
class session
{
public:
	using backend_type = BE;
	using statement_type = typename backend_type::statement_type;
	using result_type = typename backend_type::result_type;
	using recordset_type = typename result_type::recordset_type;

public:
	session( );

	bool connected( ) const;
	void connect( const std::string& connInfo );
	void reconnect( );
	void disconnect( );

	void begin_transaction( );
	void commit_transaction( );
	void rollback_transaction( );

	result_type execute( const std::string& stmt );

	template< typename... col_types >
	result_type execute_params( const std::string& stmt, col_types&&... values );

	recordset_type query( const std::string& stmt );

	template< typename... col_types >
	recordset_type query_params( const std::string& stmt, col_types&&... values );

	template< typename T >
	void bulk_insert( const std::string& table, const T& container );

	backend_type& backend( );

private:
	using session_ptr = std::shared_ptr< backend_type >;

private:
	session_ptr m_session;
	std::string m_connInfo;
};

}  // namespace dbclt
