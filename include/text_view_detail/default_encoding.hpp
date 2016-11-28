// Copyright (c) 2016, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef TEXT_VIEW_DEFAULT_ENCODING_HPP // {
#define TEXT_VIEW_DEFAULT_ENCODING_HPP


#include <text_view_detail/encodings/std_encodings.hpp>


namespace std {
namespace experimental {
inline namespace text {

namespace text_detail {
template<typename T>
struct get_default_encoding_type_t;

template<typename T>
requires requires () { typename T::encoding_type; }
struct get_default_encoding_type_t<T> {
    using type = typename T::default_encoding_type;
};

template<>
struct get_default_encoding_type_t<char> {
    using type = execution_character_encoding;
};

template<>
struct get_default_encoding_type_t<wchar_t> {
    using type = execution_wide_character_encoding;
};

#if 0
// FIXME: Enable default_encoding<char8_t> if P0482 were to be adopted.
template<>
struct get_default_encoding_type_t<char8_t> {
    using type = char8_character_encoding;
};
#endif

template<>
struct get_default_encoding_type_t<char16_t> {
    using type = char16_character_encoding;
};

template<>
struct get_default_encoding_type_t<char32_t> {
    using type = char32_character_encoding;
};

template<typename T>
using default_encoding_type_t =
    typename get_default_encoding_type_t<
        std::remove_cv_t<
        std::remove_reference_t<T>>>::type;
} // namespace text_detail


template<typename T>
using default_encoding_type_t = text_detail::default_encoding_type_t<T>;


} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_DEFAULT_ENCODING_HPP
