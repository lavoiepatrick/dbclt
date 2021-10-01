# dbclt
This is a include only database client library for C++. This library is still in an early stage and have a fairly good support for PostgreSQL but it still missing a few features for ODBC. It aims to implement more database clients such as SQLite, MySQL and FreeDTS (for Sybase and SQLServer).

The main reason why I started implementing this library was because I needed to access some databases from both Windows and Linux, I wanted to have minimal dependencies and didn't want to depend on a big framework only for database access.

This is a work in progress and intend on finishing it when I have some spare time :)

# Features
- Include only (no need to link except for the specific database client libs)
- Cross platform (tested on Linux and Windows)
- C++ like interface
- Common interface for all database clients
- Use of variadic templates for parameterized queries
- Conversion of records into struct without any custom conversion code.

# Database Support
- PostgreSQL (missing bulk operations and output parameters)
- ODBC (missing parameterized queries, bulk operations and output parameters)
- SQLite (not implemented yet)
- MySQL (not implemented yet)
- FreeDTS (not implemented yet)

# Dependencies
- C++ 17
- Catch2 for test suite (https://github.com/catchorg/Catch2)
- Boost (version 1.73 was used for testing)
- date (https://github.com/HowardHinnant/date)
- structs (https://github.com/lavoiepatrick/structs)

# Quick Start
More examples can be found in the test directory.
```cpp
#include <dbclt/postgres.h>

#include <structs/to_tuple.h>

using namespace std;

struct MyRecord
{
	string t;
	int32_t i;
	int64_t bi;
	double f;
	bool b;
	string tt;
	dbclt::datetime dt;
};

int main( )
{
    dbclt::postgres::session session;
    session.connect( "dbname=postgres user=postgres password=secret" );

    int32_t id = 123;
    dbclt::postgres::recordset rs =
        session.query_params( "select * from tmp where id=$1", id );
    for( const MyRecord& record : rs )
        cout << record.t << endl;

    vector< MyRecord > records;
    session.query( "select * from tmp" ).into( records );
}
```

# Licence
dbclt is an open source project licensed under MIT.
