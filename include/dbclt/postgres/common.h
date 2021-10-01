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

#include <libpq-fe.h>

#define POSTGRESQL_INTEGER_DATETIME 1

namespace dbclt
{
namespace postgres
{
class conn_ptr : public std::shared_ptr< PGconn >
{
public:
	conn_ptr( )
	{
	}
	conn_ptr( PGconn* conn ) : std::shared_ptr< PGconn >( conn, PQfinish )
	{
	}
};

class result_ptr : public std::shared_ptr< PGresult >
{
public:
	result_ptr( )
	{
	}
	result_ptr( PGresult* result ) : std::shared_ptr< PGresult >( result, PQclear )
	{
	}
};

class notify_ptr : public std::shared_ptr< PGnotify >
{
public:
	notify_ptr( )
	{
	}
	notify_ptr( PGnotify* notify ) : std::shared_ptr< PGnotify >( notify, PQfreemem )
	{
	}
};

}  // namespace postgres
}  // namespace dbclt
