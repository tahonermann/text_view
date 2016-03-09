// Copyright (c) 2016, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef TEXT_VIEW_TRAITS_HPP // {
#define TEXT_VIEW_TRAITS_HPP


#include <type_traits>


namespace std {
namespace experimental {
inline namespace text {


/*
 * Associated code unit type helper
 */
namespace text_detail {
template<typename T>
struct get_code_unit_type_t;

template<typename T>
requires requires () { typename T::code_unit_type; }
struct get_code_unit_type_t<T> {
    using type = typename T::code_unit_type;
};

template<typename T>
using code_unit_type_t =
          typename get_code_unit_type_t<std::remove_reference_t<T>>::type;
} // namespace text_detail

template<typename T>
using code_unit_type_t = text_detail::code_unit_type_t<T>;


/*
 * Associated code point type helper
 */
namespace text_detail {
template<typename T>
struct get_code_point_type_t;

template<typename T>
requires requires () { typename T::code_point_type; }
struct get_code_point_type_t<T> {
    using type = typename T::code_point_type;
};

template<typename T>
using code_point_type_t =
          typename get_code_point_type_t<std::remove_reference_t<T>>::type;
} // namespace text_detail

template<typename T>
using code_point_type_t = text_detail::code_point_type_t<T>;


/*
 * Associated character set type helper
 */
namespace text_detail {
template<typename T>
struct get_character_set_type_t;

template<typename T>
requires requires () { typename T::character_set_type; }
struct get_character_set_type_t<T> {
    using type = typename T::character_set_type;
};

template<typename T>
using character_set_type_t =
          typename get_character_set_type_t<std::remove_reference_t<T>>::type;
} // namespace text_detail

template<typename T>
using character_set_type_t = text_detail::character_set_type_t<T>;


/*
 * Associated character type helper
 */
namespace text_detail {
template<typename T>
struct get_character_type_t;

template<typename T>
requires requires () { typename T::character_type; }
struct get_character_type_t<T> {
    using type = typename T::character_type;
};

template<typename T>
using character_type_t =
          typename get_character_type_t<std::remove_reference_t<T>>::type;
} // namespace text_detail

template<typename T>
using character_type_t = text_detail::character_type_t<T>;


/*
 * Associated encoding type helper
 */
namespace text_detail {
template<typename T>
struct get_encoding_type_t;

template<typename T>
requires requires () { typename T::encoding_type; }
struct get_encoding_type_t<T> {
    using type = typename T::encoding_type;
};

template<typename T>
using encoding_type_t = typename get_encoding_type_t<std::remove_reference_t<T>>::type;
} // namespace text_detail

template<typename T>
using encoding_type_t = text_detail::encoding_type_t<T>;


} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_TRAITS_HPP
