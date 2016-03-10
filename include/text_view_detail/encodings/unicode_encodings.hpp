// Copyright (c) 2016, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef TEXT_VIEW_UNICODE_ENCODINGS_HPP // {
#define TEXT_VIEW_UNICODE_ENCODINGS_HPP


#include <cstdint>
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


namespace std {
namespace experimental {
inline namespace text {


#if defined(__STDC_ISO_10646__)
/*
 * C++ ISO-10646 wide character encoding
 */
struct iso_10646_wide_character_encoding
    : public text_detail::trivial_codec<
                 character<unicode_character_set>,
                 wchar_t>
{
    static const state_type& initial_state() noexcept {
        static const state_type state;
        return state;
    }
};
#endif // __STDC_ISO_10646__


/*
 * Unicode UTF-8 character encodings
 */
// FIXME: If N3398 were to be adopted, replace char with char8_t.
struct utf8_encoding
    : public text_detail::utf8_codec<
                 character<unicode_character_set>,
                 char>
{
    static const state_type& initial_state() noexcept {
        static const state_type state;
        return state;
    }
};

// FIXME: If N3398 were to be adopted, replace char with char8_t.
struct utf8bom_encoding
    : public text_detail::utf8bom_codec<
                 character<unicode_character_set>,
                 char>
{
    static const state_type& initial_state() noexcept {
        static const state_type state{ false };
        return state;
    }
};


/*
 * Unicode UTF-16 character encodings
 */
struct utf16_encoding
    : public text_detail::utf16_codec<
                 character<unicode_character_set>,
                 char16_t>
{
    static const state_type& initial_state() noexcept {
        static const state_type state;
        return state;
    }
};

struct utf16be_encoding
    : public text_detail::utf16be_codec<
                 character<unicode_character_set>,
                 char>
{
    static const state_type& initial_state() noexcept {
        static const state_type state;
        return state;
    }
};

struct utf16le_encoding
    : public text_detail::utf16le_codec<
                 character<unicode_character_set>,
                 char>
{
    static const state_type& initial_state() noexcept {
        static const state_type state;
        return state;
    }
};

struct utf16bom_encoding
    : public text_detail::utf16bom_codec<
                 character<unicode_character_set>,
                 char>
{
    static const state_type& initial_state() noexcept {
        static const state_type state{ false, state_type::big_endian };
        return state;
    }
};


/*
 * Unicode UTF-32 character encodings
 */
struct utf32_encoding
    : public text_detail::trivial_codec<
                 character<unicode_character_set>,
                 char32_t>
{
    static const state_type& initial_state() noexcept {
        static const state_type state;
        return state;
    }
};

struct utf32be_encoding
    : public text_detail::utf32be_codec<
                 character<unicode_character_set>,
                 char>
{
    static const state_type& initial_state() noexcept {
        static const state_type state;
        return state;
    }
};

struct utf32le_encoding
    : public text_detail::utf32le_codec<
                 character<unicode_character_set>,
                 char>
{
    static const state_type& initial_state() noexcept {
        static const state_type state;
        return state;
    }
};

struct utf32bom_encoding
    : public text_detail::utf32bom_codec<
                 character<unicode_character_set>,
                 char>
{
    static const state_type& initial_state() noexcept {
        static const state_type state{ false, state_type::big_endian };
        return state;
    }
};


} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_UNICODE_ENCODINGS_HPP
