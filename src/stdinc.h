/* Frobby: Software for monomial ideal computations.
   Copyright (C) 2007 Bjarke Hammersholt Roune (www.broune.com)

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see http://www.gnu.org/licenses/.
*/
/** \file stdinc.h
 This header file includes common definitions and is included as the
 first line of code in every implementation file (.cpp). Thus it is
 never necessary to include it in headers, and anything defined here
 is defined everywhere throughout the code base.
*/

#ifdef _MSC_VER // For Microsoft Compiler in Visual Studio C++.
#pragma warning (push, 1) // Reduce warning level for GMP headers.
#endif

// Some versions of GMP do not define gmp_fprintf unless cstdio is
// included first, so we have to include it here.
#include <cstdio>

#include <gmp.h>
#include <gmpxx.h>

#ifdef _MSC_VER // For Microsoft Compiler in Visual Studio C++.
#pragma warning (pop) // Go back to previous warning level.
#pragma warning (disable: 4996) // std::copy is flagged as dangerous.
#pragma warning (disable: 4290) // VC++ ignores throw () specification.
#pragma warning (disable: 4127) // Warns about using "while (true)".
#pragma warning (disable: 4100) // Warns about unused parameters.
#pragma warning (disable: 4800) // Warns on int to bool conversion.

// This warning warns about using the this pointer in base member
// initializer lists. This is a pretty good warning as that can
// obviously easily go wrong, but it is pretty useful to do as well,
// so the warning is turned off.
#pragma warning (disable: 4355)

// MSC's map header has an issue where you get a syntax error if you
// define a macro for new like we do below. We work around this by including
// map before we define the macro.
#include <map>

#ifdef _DEBUG
#define DEBUG
#endif
#endif

#include <memory>
using namespace std;

#ifdef DEBUG
#include <iostream> // Useful for debugging.
#define PRINT
#define IF_DEBUG(X) X
#include <cassert>
#define ASSERT(X) assert(X);
void* operator new(size_t s, const char* file, size_t line)
  throw (std::bad_alloc);
void* operator new[](size_t s, const char* file, size_t line)
  throw (std::bad_alloc);
void operator delete(void* s, const char* file, size_t line);
void operator delete[](void* s, const char* file, size_t line);
#define new new (__FILE__, __LINE__)
#else
#define IF_DEBUG(X)
#define ASSERT(X)
#endif

typedef unsigned int Exponent;
typedef unsigned long Word;
static const size_t BitsPerWord = 8 * sizeof(Word);

/** @todo: move this elsewhere. */
namespace constants {
  extern const char* const version;
}
