// Copyright (c) 2017, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef TEXT_VIEW_ANY_CHARSET_HPP // {
#define TEXT_VIEW_ANY_CHARSET_HPP


#include <cstdint>


namespace std {
namespace experimental {
inline namespace text {


/*
 * Any character set
 * This character set is associated with characters that do not have a
 * statically known character set.
 */
class any_character_set {
public:
    using code_point_type = uint_least32_t;

    static const char* get_name() noexcept {
        return "any_character_set";
    }

    static constexpr code_point_type get_substitution_code_point() noexcept {
        return code_point_type(-1);
    }
};


} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_ANY_CHARSET_HPP
