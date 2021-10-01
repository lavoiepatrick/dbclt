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
class field
{
public:
	enum class type
	{
		db_bool,
		db_string,
		db_int8,
		db_int16,
		db_int32,
		db_int64,
		db_float,
		db_double,
		db_binary,
		db_date,
		db_datetime
	};

public:
	field( );
	field( const std::string& name, type type );

	const std::string& name( ) const;
	type db_type( ) const;

private:
	std::string m_name;
	type m_type;
};

using fields_type = std::vector< field >;

inline field::field( )
{
}

inline field::field( const std::string& name, type type ) : m_name( name ), m_type( type )
{
}

inline const std::string& field::name( ) const
{
	return m_name;
}

inline field::type field::db_type( ) const
{
	return m_type;
}

}  // namespace dbclt
