// Copyright (c) 2015, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

// This code demonstrates use of the std::find() algorithm to search for a code
// point represented by multiple code units in a UTF-8 encoded string, and that
// the underlying code unit sequence is available for introspection.

#include <cassert>
#include <algorithm>
#include <text_view>

using namespace std;
using namespace std::experimental;

int main() {
    // This UTF-8 encoded string contains a LATIN SMALL LETTER O WITH STROKE
    // specified as "\u00F8" and encoded as "\xC3\xB8" within the name "Jøerg".
    static auto &a_utf8_string = u8"J\u00F8erg is my friend";

    // Construct a text_view for the UTF-8 encoded string.
    auto a_utf8_tv = make_text_view<utf8_encoding>(a_utf8_string);

    // Declare a character corresponding to the LATIN SMALL LETTER O WITH STROKE
    // in the Unicode character set.
    auto latin_small_letter_o_with_stroke =
        utf8_encoding::codec_type::character_type{0x00F8};

    // Search for the code point.  This returns a text iterator.
    auto ti = find(
        begin(a_utf8_tv),
        end(a_utf8_tv),
        latin_small_letter_o_with_stroke);
    
    // Validate that the code point was found.
    assert(ti != end(a_utf8_tv));

    // Validate that the code point has the expected value.
    assert(ti->get_code_point() == 0x00F8);
    
    // Validate that the underlying code unit range is as expected.  Note that
    // the comparison is against code unit values expressed as character
    // literals rather than as simple integer values.  This is necessary because
    // the underlying code unit type of UTF-8 string literals is 'char' and
    // 'char' may be a signed 8-bit type that is unable to represent the full
    // unsigned range of UTF-8 code unit values.
    // FIXME: gcc 6.0.0 does not yet implement N4197.  The character literals
    // FIXME: below should be written with a u8 prefix for consistency with the
    // FIXME: UTF-8 encoded string written above.
    assert(*(ti.base_range().begin()+0) == '\xC3');
    assert(*(ti.base_range().begin()+1) == '\xB8');
    assert((ti.base_range().begin()+2) == ti.base_range().end());
}
