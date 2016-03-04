// Copyright (c) 2016, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

// This code demonstrates enumeration of code points in a UTF-8 encoded string.

#include <cassert>
#include <algorithm>
#include <experimental/text_view>
#include <text_view_detail/range_based_for.hpp>

using namespace std;
using namespace std::experimental;

int main() {
    // This UTF-8 encoded string contains a LATIN SMALL LETTER O WITH STROKE
    // specified as "\u00F8" and encoded as "\xC3\xB8" within the name "Jøerg".
    static auto &a_utf8_string = u8"J\u00F8erg is my friend";

    // Construct a text_view for the UTF-8 encoded string.
    auto a_utf8_tv = make_text_view<utf8_encoding>(a_utf8_string);

    // Declare a type alias for the UTF-8 character type for convenience.
    using CT = character_type_t<utf8_encoding>;

    // Declare an array of the expected code points encountered during
    // enumeration for validation purposes.
    static CT expected_code_points[] = {
        CT{0x004A}, // J
        CT{0x00F8}, // ø
        CT{0x0065}, // e
        CT{0x0072}, // r
        CT{0x0067}, // g
        CT{0x0020}, // <space>
        CT{0x0069}, // i
        CT{0x0073}, // s
        CT{0x0020}, // <space>
        CT{0x006D}, // m
        CT{0x0079}, // y
        CT{0x0020}, // <space>
        CT{0x0066}, // f
        CT{0x0072}, // r
        CT{0x0069}, // i
        CT{0x0065}, // e
        CT{0x006E}, // n
        CT{0x0064}, // d
        CT{0x0000}  // <nul>
    };

    // FIXME: The C++11 range-based-for requires that the begin and end types
    // FIXME: be identical.  The RANGE_BASED_FOR macro is used to work around
    // FIXME: this limitation.  This limitation will be removed if P0184R0 is
    // FIXME: adopted.
    int code_point_index = 0;
    RANGE_BASED_FOR (const auto &ch, a_utf8_tv) {
        assert(ch == expected_code_points[code_point_index++]);
    }
    assert(code_point_index == 
           sizeof(expected_code_points)/sizeof(expected_code_points[0]));
}
