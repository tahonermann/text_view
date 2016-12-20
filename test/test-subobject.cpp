// Copyright (c) 2016, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

// Ensure assert is enabled regardless of build type
#if defined(NDEBUG)
#undef NDEBUG
#endif

#include <text_view_detail/subobject.hpp>

using namespace std::experimental::text_detail;

class non_empty_class { int i; };
class empty_class {};
class empty_final_class final {};

struct with_non_empty : subobject<non_empty_class> { int i; };

struct with_empty : subobject<empty_class> { int i; };
// Embedding an empty class should use the empty base class optimization.
static_assert(sizeof(with_empty) == sizeof(int));

struct with_final_empty : subobject<empty_final_class> { int i; };
// Embedding a final empty class cannot use the empty base class optimization.
static_assert(sizeof(with_final_empty) != sizeof(int));

struct with_int : subobject<int> { int i; };

int main() {
}
