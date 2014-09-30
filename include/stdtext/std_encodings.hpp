#ifndef STDTEXT_STD_ENCODINGS_HPP // {
#define STDTEXT_STD_ENCODINGS_HPP


#include <stdtext/concepts.hpp>
#include <stdtext/character.hpp>


#if !defined(STDTEXT_EXECUTION_CHARACTER_ENCODING)
#define STDTEXT_EXECUTION_CHARACTER_ENCODING \
        basic_execution_character_encoding
#endif
#if !defined(STDTEXT_EXECUTION_WIDE_CHARACTER_ENCODING)
#define STDTEXT_EXECUTION_WIDE_CHARACTER_ENCODING \
        basic_execution_wide_character_encoding
#endif
#if !defined(STDTEXT_CHAR8_CHARACTER_ENCODING)
#define STDTEXT_CHAR8_CHARACTER_ENCODING \
        utf8_encoding
#endif
#if !defined(STDTEXT_CHAR16_CHARACTER_ENCODING)
#define STDTEXT_CHAR16_CHARACTER_ENCODING \
        utf16_encoding
#endif
#if !defined(STDTEXT_CHAR32_CHARACTER_ENCODING)
#define STDTEXT_CHAR32_CHARACTER_ENCODING \
        utf32_encoding
#endif


namespace std {
namespace experimental {
namespace text {


/*
 * C++ basic execution character encoding
 */
struct basic_execution_character_encoding {
    using codec_type = trivial_codec<
              character<basic_execution_character_set>,
              char>;
    static const typename codec_type::state_type initial_state;
};


/*
 * C++ basic execution wide character encoding
 */
struct basic_execution_wide_character_encoding {
    using codec_type = trivial_codec<
              character<basic_execution_wide_character_set>,
              wchar_t>;
    static const typename codec_type::state_type initial_state;
};


#if defined(__STDC_ISO_10646__)
/*
 * C++ ISO-10646 wide character encoding
 */
struct iso_10646_wide_character_encoding {
    using codec_type = trivial_codec<
              character<unicode_character_set_template<wchar_t>>,
              wchar_t>;
    static const typename codec_type::state_type initial_state;
};
#endif // __STDC_ISO_10646__


/*
 * Unicode UTF-8 character encoding
 */
template<Character CT, Code_unit CUT>
struct utf8_encoding_template {
    using codec_type = utf8_codec<CT, CUT>;
    static const typename codec_type::state_type initial_state;
};
using utf8_encoding = utf8_encoding_template<char32_t, unsigned char>;


/*
 * Unicode UTF-16 character encodings
 */
template<Character CT, Code_unit CUT>
struct utf16_encoding_template {
    using codec_type = utf16_codec<CT, CUT>;
    static const typename codec_type::state_type initial_state;
};
using utf16_encoding = utf16_encoding_template<char32_t, char16_t>;

template<Character CT, Character ICT, Code_unit CUT>
struct utf16be_encoding_template {
    using codec_type = composite_codec<
                           utf16_codec<CT, ICT>,
                           big_endian_codec<ICT, CUT>>;
    static const typename codec_type::state_type initial_state;
};
using utf16be_encoding =
          utf16be_encoding_template<char32_t, char16_t, unsigned char>;

template<Character CT, Character ICT, Code_unit CUT>
struct utf16le_encoding_template {
    using codec_type = composite_codec<
                           utf16_codec<CT, ICT>,
                           little_endian_codec<ICT, CUT>>;
    static const typename codec_type::state_type initial_state;
};
using utf16le_encoding =
          utf16le_encoding_template<char32_t, char16_t, unsigned char>;


/*
 * Unicode UTF-32 character encodings
 */
template<Character CT, Code_unit CUT>
struct utf32_encoding_template {
    using codec_type = trivial_codec<CT, CUT>;
    static const typename codec_type::state_type initial_state;
};
using utf32_encoding = utf32_encoding_template<char32_t, char32_t>;

template<Character CT, Code_unit CUT>
struct utf32be_encoding_template {
    using codec_type = big_endian_codec<CT, CUT>;
    static const typename codec_type::state_type initial_state;
};
using utf32be_encoding = utf32be_encoding_template<char32_t, unsigned char>;

template<Character CT, Code_unit CUT>
struct utf32le_encoding_template {
    using codec_type = little_endian_codec<CT, CUT>;
    static const typename codec_type::state_type initial_state;
};
using utf32le_encoding = utf32le_encoding_template<char32_t, unsigned char>;


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
