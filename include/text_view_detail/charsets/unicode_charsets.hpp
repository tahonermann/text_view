// Copyright (c) 2015, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef TEXT_VIEW_UNICODE_CHARSETS_HPP // {
#define TEXT_VIEW_UNICODE_CHARSETS_HPP


namespace std {
namespace experimental {
inline namespace text {


/*
 * C++ universal character set
 * ISO/IEC 14882:2011(E) 2.3
 */
class unicode_character_set {
public:
    using code_point_type = char32_t;

    static const char* get_name() noexcept {
        return "unicode_character_set";
    }
};


} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_UNICODE_CHARSETS_HPP
