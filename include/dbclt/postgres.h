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

#include <chrono>
#include <date/date.h>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <string.h>
#include <vector>

#include "data_exception.h"
#include "field.h"
#include "range.h"
#include "util.h"

#include "record.h"
#include "recordset.h"
#include "result.h"

#include "session.h"
#include "statement.h"
#include "transaction.h"

#include "postgres/common.h"
#include "postgres/listener.h"
#include "postgres/result.h"
#include "postgres/session.h"
#include "postgres/statement.h"

#include "postgres/listener.inl"
#include "postgres/result.inl"
#include "postgres/session.inl"
#include "postgres/statement.inl"

#include "recordset.inl"
#include "result.inl"
#include "session.inl"
#include "statement.inl"
#include "transaction.inl"
