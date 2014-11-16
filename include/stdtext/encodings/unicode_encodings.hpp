#ifndef STDTEXT_UNICODE_ENCODINGS_HPP // {
#define STDTEXT_UNICODE_ENCODINGS_HPP


#include <stdtext/charsets/unicode_charsets.hpp>
#include <stdtext/character.hpp>
#include <stdtext/codecs/trivial_codec.hpp>
#include <stdtext/codecs/utf8_codec.hpp>
#include <stdtext/codecs/utf16_codec.hpp>
#include <stdtext/codecs/utf16be_codec.hpp>
#include <stdtext/codecs/utf16le_codec.hpp>
#include <stdtext/codecs/utf32be_codec.hpp>
#include <stdtext/codecs/utf32le_codec.hpp>
#include <cstdint>


namespace std {
namespace experimental {
namespace text {


#if defined(__STDC_ISO_10646__)
/*
 * C++ ISO-10646 wide character encoding
 */
struct iso_10646_wide_character_encoding {
    using codec_type = trivial_codec<
              character<unicode_character_set_template<wchar_t>>,
              wchar_t>;
    using state_type = codec_type::state_type;
    static const state_type& initial_state() noexcept {
        static const state_type state;
        return state;
    }
};
#endif // __STDC_ISO_10646__


/*
 * Unicode UTF-8 character encoding
 */
template<Character CT, Code_unit CUT>
struct utf8_encoding_template {
    using codec_type = utf8_codec<CT, CUT>;
    using state_type = typename codec_type::state_type;
    static const state_type& initial_state() noexcept {
        static const state_type state;
        return state;
    }
};
using utf8_encoding = utf8_encoding_template<
              character<unicode_character_set_template<char32_t>>,
              uint_least8_t>;


/*
 * Unicode UTF-16 character encodings
 */
template<Character CT, Code_unit CUT>
struct utf16_encoding_template {
    using codec_type = utf16_codec<CT, CUT>;
    using state_type = typename codec_type::state_type;
    static const state_type& initial_state() noexcept {
        static const state_type state;
        return state;
    }
};
using utf16_encoding = utf16_encoding_template<
              character<unicode_character_set_template<char32_t>>,
              char16_t>;

template<Character CT, Code_unit CUT>
struct utf16be_encoding_template {
    using codec_type = utf16be_codec<CT, CUT>;
    using state_type = typename codec_type::state_type;
    static const state_type& initial_state() noexcept {
        static const state_type state;
        return state;
    }
};
using utf16be_encoding = utf16be_encoding_template<
              character<unicode_character_set_template<char32_t>>,
              uint_least8_t>;

template<Character CT, Code_unit CUT>
struct utf16le_encoding_template {
    using codec_type = utf16le_codec<CT, CUT>;
    using state_type = typename codec_type::state_type;
    static const state_type& initial_state() noexcept {
        static const state_type state;
        return state;
    }
};
using utf16le_encoding = utf16le_encoding_template<
              character<unicode_character_set_template<char32_t>>,
              uint_least8_t>;


/*
 * Unicode UTF-32 character encodings
 */
template<Character CT, Code_unit CUT>
struct utf32_encoding_template {
    using codec_type = trivial_codec<CT, CUT>;
    using state_type = typename codec_type::state_type;
    static const state_type& initial_state() noexcept {
        static const state_type state;
        return state;
    }
};
using utf32_encoding = utf32_encoding_template<
              character<unicode_character_set_template<char32_t>>,
              char32_t>;

template<Character CT, Code_unit CUT>
struct utf32be_encoding_template {
    using codec_type = utf32be_codec<CT, CUT>;
    using state_type = typename codec_type::state_type;
    static const state_type& initial_state() noexcept {
        static const state_type state;
        return state;
    }
};
using utf32be_encoding = utf32be_encoding_template<
              character<unicode_character_set_template<char32_t>>,
              uint_least8_t>;

template<Character CT, Code_unit CUT>
struct utf32le_encoding_template {
    using codec_type = utf32le_codec<CT, CUT>;
    using state_type = typename codec_type::state_type;
    static const state_type& initial_state() noexcept {
        static const state_type state;
        return state;
    }
};
using utf32le_encoding = utf32le_encoding_template<
              character<unicode_character_set_template<char32_t>>,
              uint_least8_t>;


} // namespace text
} // namespace experimental
} // namespace std


#endif // } STDTEXT_UNICODE_ENCODINGS_HPP
