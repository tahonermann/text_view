// Copyright (c) 2015, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef TEXT_VIEW_UNICODE_ENCODINGS_HPP // {
#define TEXT_VIEW_UNICODE_ENCODINGS_HPP


#include <text_view_detail/charsets/unicode_charsets.hpp>
#include <text_view_detail/character.hpp>
#include <text_view_detail/codecs/trivial_codec.hpp>
#include <text_view_detail/codecs/utf8_codec.hpp>
#include <text_view_detail/codecs/utf8bom_codec.hpp>
#include <text_view_detail/codecs/utf16_codec.hpp>
#include <text_view_detail/codecs/utf16be_codec.hpp>
#include <text_view_detail/codecs/utf16le_codec.hpp>
#include <text_view_detail/codecs/utf16bom_codec.hpp>
#include <text_view_detail/codecs/utf32be_codec.hpp>
#include <text_view_detail/codecs/utf32le_codec.hpp>
#include <text_view_detail/codecs/utf32bom_codec.hpp>
#include <cstdint>


namespace std {
namespace experimental {
inline namespace text {


#if defined(__STDC_ISO_10646__)
/*
 * C++ ISO-10646 wide character encoding
 */
struct iso_10646_wide_character_encoding {
    using codec_type = trivial_codec<
              character<unicode_character_set_template<wchar_t>>,
              wchar_t>;
    using state_type = codec_type::state_type;
    static const state_type& initial_state() {
        static const state_type state;
        return state;
    }
};
#endif // __STDC_ISO_10646__


/*
 * Unicode UTF-8 character encodings
 */
template<Character CT, Code_unit CUT>
struct utf8_encoding_template {
    using codec_type = utf8_codec<CT, CUT>;
    using state_type = typename codec_type::state_type;
    static const state_type& initial_state() {
        static const state_type state;
        return state;
    }
};
// FIXME: If N3398 were to be adopted, replace char with char8_t.
// FIXME: If CHAR_BIT is less than 8, then char isn't large enough for UTF-8
// FIXME: code units.  char is used anyway since the standard specifies that
// FIXME: u8 string literals are const arrays of char.  Use of char is also
// FIXME: problematic for implementations with a signed char.
using utf8_encoding = utf8_encoding_template<
              character<unicode_character_set_template<char32_t>>,
              char>;

template<Character CT, Code_unit CUT>
struct utf8bom_encoding_template {
    using codec_type = utf8bom_codec<CT, CUT>;
    using state_type = typename codec_type::state_type;
    static const state_type& initial_state() {
        static const state_type state{ false };
        return state;
    }
};
// FIXME: If N3398 were to be adopted, replace char with char8_t.
// FIXME: If CHAR_BIT is less than 8, then char isn't large enough for UTF-8
// FIXME: code units.  char is used anyway since the standard specifies that
// FIXME: u8 string literals are const arrays of char.  Use of char is also
// FIXME: problematic for implementations with a signed char.
using utf8bom_encoding = utf8bom_encoding_template<
              character<unicode_character_set_template<char32_t>>,
              char>;


/*
 * Unicode UTF-16 character encodings
 */
template<Character CT, Code_unit CUT>
struct utf16_encoding_template {
    using codec_type = utf16_codec<CT, CUT>;
    using state_type = typename codec_type::state_type;
    static const state_type& initial_state() {
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
    static const state_type& initial_state() {
        static const state_type state;
        return state;
    }
};
// FIXME: If CHAR_BIT is less than 8, then char isn't large enough for UTF-16be
// FIXME: code units.  uint_least8_t is used now, but char would be preferred.
// FIXME: Use of char would also be problematic for implementations with a
// FIXME: signed char.
using utf16be_encoding = utf16be_encoding_template<
              character<unicode_character_set_template<char32_t>>,
              uint_least8_t>;

template<Character CT, Code_unit CUT>
struct utf16le_encoding_template {
    using codec_type = utf16le_codec<CT, CUT>;
    using state_type = typename codec_type::state_type;
    static const state_type& initial_state() {
        static const state_type state;
        return state;
    }
};
// FIXME: If CHAR_BIT is less than 8, then char isn't large enough for UTF-16le
// FIXME: code units.  uint_least8_t is used now, but char would be preferred.
// FIXME: Use of char would also be problematic for implementations with a
// FIXME: signed char.
using utf16le_encoding = utf16le_encoding_template<
              character<unicode_character_set_template<char32_t>>,
              uint_least8_t>;

template<Character CT, Code_unit CUT>
struct utf16bom_encoding_template {
    using codec_type = utf16bom_codec<CT, CUT>;
    using state_type = typename codec_type::state_type;
    static const state_type& initial_state() {
        static const state_type state{ false, state_type::big_endian };
        return state;
    }
};
// FIXME: If CHAR_BIT is less than 8, then char isn't large enough for UTF-16
// FIXME: code units.  uint_least8_t is used now, but char would be preferred.
// FIXME: Use of char would also be problematic for implementations with a
// FIXME: signed char.
using utf16bom_encoding = utf16bom_encoding_template<
              character<unicode_character_set_template<char32_t>>,
              uint_least8_t>;


/*
 * Unicode UTF-32 character encodings
 */
template<Character CT, Code_unit CUT>
struct utf32_encoding_template {
    using codec_type = trivial_codec<CT, CUT>;
    using state_type = typename codec_type::state_type;
    static const state_type& initial_state() {
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
    static const state_type& initial_state() {
        static const state_type state;
        return state;
    }
};
// FIXME: If CHAR_BIT is less than 8, then char isn't large enough for UTF-32be
// FIXME: code units.  uint_least8_t is used now, but char would be preferred.
// FIXME: Use of char would also be problematic for implementations with a
// FIXME: signed char.
using utf32be_encoding = utf32be_encoding_template<
              character<unicode_character_set_template<char32_t>>,
              uint_least8_t>;

template<Character CT, Code_unit CUT>
struct utf32le_encoding_template {
    using codec_type = utf32le_codec<CT, CUT>;
    using state_type = typename codec_type::state_type;
    static const state_type& initial_state() {
        static const state_type state;
        return state;
    }
};
// FIXME: If CHAR_BIT is less than 8, then char isn't large enough for UTF-32le
// FIXME: code units.  uint_least8_t is used now, but char would be preferred.
// FIXME: Use of char would also be problematic for implementations with a
// FIXME: signed char.
using utf32le_encoding = utf32le_encoding_template<
              character<unicode_character_set_template<char32_t>>,
              uint_least8_t>;

template<Character CT, Code_unit CUT>
struct utf32bom_encoding_template {
    using codec_type = utf32bom_codec<CT, CUT>;
    using state_type = typename codec_type::state_type;
    static const state_type& initial_state() {
        static const state_type state{ false, state_type::big_endian };
        return state;
    }
};
// FIXME: If CHAR_BIT is less than 8, then char isn't large enough for UTF-32
// FIXME: code units.  uint_least8_t is used now, but char would be preferred.
// FIXME: Use of char would also be problematic for implementations with a
// FIXME: signed char.
using utf32bom_encoding = utf32bom_encoding_template<
              character<unicode_character_set_template<char32_t>>,
              uint_least8_t>;


} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_UNICODE_ENCODINGS_HPP
