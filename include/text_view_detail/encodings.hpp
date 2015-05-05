#ifndef TEXT_VIEW_ENCODINGS_HPP // {
#define TEXT_VIEW_ENCODINGS_HPP


#include <text_view_detail/traits.hpp>
#include <text_view_detail/encodings/basic_encodings.hpp>
#include <text_view_detail/encodings/unicode_encodings.hpp>
#include <text_view_detail/encodings/std_encodings.hpp>


namespace std {
namespace experimental {
namespace text {
namespace detail {

template<>
struct get_encoding_type_of<char*> {
    using type = execution_character_encoding;
};

template<>
struct get_encoding_type_of<wchar_t*> {
    using type = execution_wide_character_encoding;
};

template<>
struct get_encoding_type_of<char16_t*> {
    using type = char16_character_encoding;
};

template<>
struct get_encoding_type_of<char32_t*> {
    using type = char32_character_encoding;
};

} // namespace detail
} // namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_ENCODINGS_HPP
