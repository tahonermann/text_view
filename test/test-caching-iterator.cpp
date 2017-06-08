// Copyright (c) 2017, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

// Ensure assert is enabled regardless of build type
#if defined(NDEBUG)
#undef NDEBUG
#endif

#include <cassert>
#include <experimental/ranges/iterator>
#include <text_view_detail/adl_customization.hpp>
#include <text_view_detail/caching_iterator.hpp>
#include "test_adapters.hpp"

using namespace std::experimental;
using namespace std::experimental::text_detail;

int main() {
    int seq[] = { 1, 2, 3, 4 };

    using IIA = input_iterator_adapter<int*>;
    using CI = caching_iterator<IIA>;
    using CIS = caching_iterator_sentinel<IIA>;

    static_assert(ranges::ForwardIterator<CI>);
    static_assert(!ranges::BidirectionalIterator<CI>);
    static_assert(ranges::Sentinel<CIS, CI>);

    CI ci = IIA{adl_begin(seq)};
    CIS cis = IIA{adl_end(seq)};
    CI ci2 = ci;
    assert(ci == ci2);
    assert(ranges::size(ci.cached_range()) == 0);
    assert(ranges::size(ci.look_ahead_range()) == 0);
    assert(ranges::size(ci2.cached_range()) == 0);
    assert(ranges::size(ci2.look_ahead_range()) == 0);
    assert(*ci == 1);
    assert(ci == ci2);
    assert(ranges::size(ci.cached_range()) == 0);
    assert(ranges::size(ci.look_ahead_range()) == 1);
    assert(ranges::size(ci2.cached_range()) == 0);
    assert(ranges::size(ci2.look_ahead_range()) == 1);
    assert(*ci == 1); // Read again without advancing.
    assert(ranges::size(ci.cached_range()) == 0);
    assert(ranges::size(ci.look_ahead_range()) == 1);
    assert(ranges::size(ci2.cached_range()) == 0);
    assert(ranges::size(ci2.look_ahead_range()) == 1);
    ++ci;
    assert(ci != ci2);
    assert(ranges::size(ci.cached_range()) == 1);
    assert(ranges::size(ci.look_ahead_range()) == 0);
    assert(ranges::size(ci2.cached_range()) == 0);
    assert(ranges::size(ci2.look_ahead_range()) == 1);
    assert(*ci2 == 1);
    assert(ranges::size(ci.cached_range()) == 1);
    assert(ranges::size(ci.look_ahead_range()) == 0);
    assert(ranges::size(ci2.cached_range()) == 0);
    assert(ranges::size(ci2.look_ahead_range()) == 1);
    assert(*ci == 2);
    assert(ranges::size(ci.cached_range()) == 1);
    assert(ranges::size(ci.look_ahead_range()) == 1);
    assert(ranges::size(ci2.cached_range()) == 0);
    assert(ranges::size(ci2.look_ahead_range()) == 2);
    ++ci2;
    assert(ci == ci2);
    assert(ranges::size(ci.cached_range()) == 1);
    assert(ranges::size(ci.look_ahead_range()) == 1);
    assert(ranges::size(ci2.cached_range()) == 1);
    assert(ranges::size(ci2.look_ahead_range()) == 1);
    ++ci;
    assert(ci != ci2);
    assert(ranges::size(ci.cached_range()) == 2);
    assert(ranges::size(ci.look_ahead_range()) == 0);
    assert(ranges::size(ci2.cached_range()) == 1);
    assert(ranges::size(ci2.look_ahead_range()) == 1);
    ++ci; // Advance without reading.
    assert(ci != ci2);
    assert(ranges::size(ci.cached_range()) == 3);
    assert(ranges::size(ci.look_ahead_range()) == 0);
    assert(ranges::size(ci2.cached_range()) == 1);
    assert(ranges::size(ci2.look_ahead_range()) == 2);
    assert(*ci2 == 2);
    assert(ranges::size(ci.cached_range()) == 3);
    assert(ranges::size(ci.look_ahead_range()) == 0);
    assert(ranges::size(ci2.cached_range()) == 1);
    assert(ranges::size(ci2.look_ahead_range()) == 2);
    assert(*++ci2 == 3);
    assert(ci != ci2);
    assert(ranges::size(ci.cached_range()) == 3);
    assert(ranges::size(ci.look_ahead_range()) == 0);
    assert(ranges::size(ci2.cached_range()) == 2);
    assert(ranges::size(ci2.look_ahead_range()) == 1);
    assert(*++ci2 == 4);
    assert(ci == ci2);
    assert(ranges::size(ci.cached_range()) == 3);
    assert(ranges::size(ci.look_ahead_range()) == 1);
    assert(ranges::size(ci2.cached_range()) == 3);
    assert(ranges::size(ci2.look_ahead_range()) == 1);
    ci.clear_cache(); // Invalidates ci2!
    ci2 = ci;
    assert(ranges::size(ci.cached_range()) == 0);
    assert(ranges::size(ci.look_ahead_range()) == 1);
    assert(ranges::size(ci2.cached_range()) == 0);
    assert(ranges::size(ci2.look_ahead_range()) == 1);
    assert(*ci == 4);
    assert(ranges::size(ci.cached_range()) == 0);
    assert(ranges::size(ci.look_ahead_range()) == 1);
    assert(ranges::size(ci2.cached_range()) == 0);
    assert(ranges::size(ci2.look_ahead_range()) == 1);
    assert(ci != cis);
    ++ci; // Now points past the end.
    assert(ci == cis);
    assert(ranges::size(ci.cached_range()) == 1);
    assert(ranges::size(ci.look_ahead_range()) == 0);
    assert(ranges::size(ci2.cached_range()) == 0);
    assert(ranges::size(ci2.look_ahead_range()) == 1);
    ci.clear_cache(); // Invalidates ci2!
    assert(ranges::size(ci.cached_range()) == 0);
    assert(ranges::size(ci.look_ahead_range()) == 0);

    return 0;
}
