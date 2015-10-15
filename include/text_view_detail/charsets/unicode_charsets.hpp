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
template<Code_point CPT>
class unicode_character_set_template {
public:
    using code_point_type = CPT;

    static const char* get_name() noexcept {
        return "unicode_character_set";
    }
};
using unicode_character_set = unicode_character_set_template<char32_t>;


} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_UNICODE_CHARSETS_HPP
