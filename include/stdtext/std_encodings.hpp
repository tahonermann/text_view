#ifndef STDTEXT_STD_ENCODINGS_HPP // {
#define STDTEXT_STD_ENCODINGS_HPP


#include <stdtext/traits.hpp>
#include <stdtext/encodings/basic_encodings.hpp>
#include <stdtext/encodings/unicode_encodings.hpp>


#if !defined(STDTEXT_EXECUTION_CHARACTER_ENCODING)
#define STDTEXT_EXECUTION_CHARACTER_ENCODING \
        ::std::experimental::text::basic_execution_character_encoding
#endif
#if !defined(STDTEXT_EXECUTION_WIDE_CHARACTER_ENCODING)
#define STDTEXT_EXECUTION_WIDE_CHARACTER_ENCODING \
        ::std::experimental::text::basic_execution_wide_character_encoding
#endif
#if !defined(STDTEXT_CHAR8_CHARACTER_ENCODING)
#define STDTEXT_CHAR8_CHARACTER_ENCODING \
        ::std::experimental::text::utf8_encoding
#endif
#if !defined(STDTEXT_CHAR16_CHARACTER_ENCODING)
#define STDTEXT_CHAR16_CHARACTER_ENCODING \
        ::std::experimental::text::utf16_encoding
#endif
#if !defined(STDTEXT_CHAR32_CHARACTER_ENCODING)
#define STDTEXT_CHAR32_CHARACTER_ENCODING \
        ::std::experimental::text::utf32_encoding
#endif


namespace std {
namespace experimental {
namespace text {


/*
 * C++ execution character encoding
 * ISO/IEC 14882:2011(E) 2.14.5 [lex.string]
 */
using execution_character_encoding = STDTEXT_EXECUTION_CHARACTER_ENCODING;


/*
 * C++ execution wide character encoding
 * ISO/IEC 14882:2011(E) 2.14.5 [lex.string]
 */
using execution_wide_character_encoding = STDTEXT_EXECUTION_WIDE_CHARACTER_ENCODING;


/*
 * C++ char (u8) character encoding
 * ISO/IEC 14882:2011(E) 2.14.5 [lex.string]
 */
using char8_character_encoding = STDTEXT_CHAR8_CHARACTER_ENCODING;


/*
 * C++ char16_t (u) character encoding
 * ISO/IEC 14882:2011(E) 2.14.5 [lex.string]
 */
using char16_character_encoding = STDTEXT_CHAR16_CHARACTER_ENCODING;


/*
 * C++ char32_t (U) character encoding
 * ISO/IEC 14882:2011(E) 2.14.5 [lex.string]
 */
using char32_character_encoding = STDTEXT_CHAR32_CHARACTER_ENCODING;


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


#endif // } STDTEXT_STD_ENCODINGS_HPP
