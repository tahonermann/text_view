// Copyright (c) 2017, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#include <string>
#include <vector>
#include <experimental/text_view>
#include <text_view_detail/archetypes.hpp>


using namespace std;
using namespace std::experimental;


// Negative concept checks for use in static assertions.
template<template<typename...> typename T, typename ...Ts>
concept bool InvalidSpecialization() {
    return ! requires { typename T<Ts...>; };
}


void test_error_policy_models() {
    static_assert(! TextErrorPolicy<text_error_policy>());
    static_assert(TextErrorPolicy<text_strict_error_policy>());
    static_assert(TextErrorPolicy<text_permissive_error_policy>());
    static_assert(TextErrorPolicy<text_default_error_policy>());
}

void test_code_unit_models() {
    // Archetypes
    static_assert(CodeUnit<code_unit_archetype>());
    // std
    static_assert(CodeUnit<char>());
    static_assert(CodeUnit<wchar_t>());
    static_assert(CodeUnit<char16_t>());
    static_assert(CodeUnit<char32_t>());
    static_assert(CodeUnit<unsigned char>());
    static_assert(CodeUnit<unsigned short>());
    static_assert(CodeUnit<unsigned int>());
    static_assert(CodeUnit<unsigned long>());
    static_assert(CodeUnit<unsigned long long>());
}

void test_code_point_models() {
    // Archetypes
    static_assert(CodePoint<code_point_archetype>());
    // std
    static_assert(CodePoint<char>());
    static_assert(CodePoint<wchar_t>());
    static_assert(CodePoint<char16_t>());
    static_assert(CodePoint<char32_t>());
    static_assert(CodePoint<unsigned char>());
    static_assert(CodePoint<unsigned short>());
    static_assert(CodePoint<unsigned int>());
    static_assert(CodePoint<unsigned long>());
    static_assert(CodePoint<unsigned long long>());
}

void test_character_set_models() {
    // Archetypes
    static_assert(CharacterSet<character_set_archetype>());
    // std
    static_assert(CharacterSet<execution_character_set>());
    static_assert(CharacterSet<execution_wide_character_set>());
    static_assert(CharacterSet<universal_character_set>());
    static_assert(CharacterSet<any_character_set>());
}

void test_character_models() {
    // Archetypes
    static_assert(Character<character_archetype>());
    // std
    static_assert(Character<character<execution_character_set>>());
    static_assert(Character<character<execution_wide_character_set>>());
    static_assert(Character<character<universal_character_set>>());
    static_assert(Character<character<any_character_set>>());
}

void test_code_unit_iterator_models() {
    // Archetypes
    static_assert(CodeUnitIterator<code_unit_iterator_archetype>());
    // std
    static_assert(CodeUnitIterator<char*>());
    static_assert(CodeUnitIterator<const char*>());
    static_assert(CodeUnitIterator<wchar_t*>());
    static_assert(CodeUnitIterator<char16_t*>());
    static_assert(CodeUnitIterator<char32_t*>());
}

void test_code_unit_output_iterator_models() {
    // Archetypes
    static_assert(CodeUnitOutputIterator<code_unit_output_iterator_archetype, code_unit_archetype>());
    // std
    static_assert(CodeUnitOutputIterator<char*, char>());
    static_assert(CodeUnitOutputIterator<wchar_t*, wchar_t>());
    static_assert(CodeUnitOutputIterator<char16_t*, char16_t>());
    static_assert(CodeUnitOutputIterator<char32_t*, char32_t>());
}

void test_text_encoding_state_models() {
    // Archetypes
    static_assert(TextEncodingState<text_encoding_state_archetype>());
    // std
    static_assert(TextEncodingState<trivial_encoding_state>());
    static_assert(TextEncodingState<utf8bom_encoding_state>());
    static_assert(TextEncodingState<utf16bom_encoding_state>());
    static_assert(TextEncodingState<utf32bom_encoding_state>());
}

void test_text_encoding_state_transition_models() {
    // Archetypes
    static_assert(TextEncodingStateTransition<text_encoding_state_transition_archetype>());
    // std
    static_assert(TextEncodingStateTransition<trivial_encoding_state_transition>());
    static_assert(TextEncodingStateTransition<utf8bom_encoding_state_transition>());
    static_assert(TextEncodingStateTransition<utf16bom_encoding_state_transition>());
    static_assert(TextEncodingStateTransition<utf32bom_encoding_state_transition>());
}

void test_text_encoding_models() {
    // Archetypes
    static_assert(TextEncoding<text_encoding_archetype>());
    // std
    static_assert(TextEncoding<utf8_encoding>());
    static_assert(TextEncoding<utf16_encoding>());
    static_assert(TextEncoding<utf16be_encoding>());
    static_assert(TextEncoding<utf16le_encoding>());
    static_assert(TextEncoding<utf32_encoding>());
    static_assert(TextEncoding<utf32be_encoding>());
    static_assert(TextEncoding<utf32le_encoding>());
    static_assert(TextEncoding<basic_execution_character_encoding>());
    static_assert(TextEncoding<basic_execution_wide_character_encoding>());
#if defined(__STDC_ISO_10646__)
    static_assert(TextEncoding<iso_10646_wide_character_encoding>());
#endif
    static_assert(TextEncoding<execution_character_encoding>());
    static_assert(TextEncoding<execution_wide_character_encoding>());
    static_assert(TextEncoding<char8_character_encoding>());
    static_assert(TextEncoding<char16_character_encoding>());
    static_assert(TextEncoding<char32_character_encoding>());
}

void test_text_encoder_models() {
    // Archetypes
    static_assert(TextEncoder<
                      text_encoding_archetype,
                      code_unit_iterator_archetype>());
    // std
    static_assert(TextEncoder<
                      execution_character_encoding,
                      char*>());
    static_assert(TextEncoder<
                      execution_wide_character_encoding,
                      wchar_t*>());
    // FIXME: If P0482 were to be adopted, replace char with char8_t.
    static_assert(TextEncoder<
                      char8_character_encoding,
                      char*>());
    static_assert(TextEncoder<
                      char16_character_encoding,
                      char16_t*>());
    static_assert(TextEncoder<
                      char32_character_encoding,
                      char32_t*>());
    static_assert(TextEncoder<
                      basic_execution_character_encoding,
                      char*>());
    static_assert(TextEncoder<
                      basic_execution_wide_character_encoding,
                      wchar_t*>());
#if defined(__STDC_ISO_10646__)
    static_assert(TextEncoder<
                      iso_10646_wide_character_encoding,
                      wchar_t*>());
#endif // __STDC_ISO_10646__
    static_assert(TextEncoder<
                      utf8_encoding,
                      char*>());
    static_assert(TextEncoder<
                      utf8bom_encoding,
                      char*>());
    static_assert(TextEncoder<
                      utf16_encoding,
                      char16_t*>());
    static_assert(TextEncoder<
                      utf16be_encoding,
                      char*>());
    static_assert(TextEncoder<
                      utf16le_encoding,
                      char*>());
    static_assert(TextEncoder<
                      utf16bom_encoding,
                      char*>());
    static_assert(TextEncoder<
                      utf32_encoding,
                      char32_t*>());
    static_assert(TextEncoder<
                      utf32be_encoding,
                      char*>());
    static_assert(TextEncoder<
                      utf32le_encoding,
                      char*>());
    static_assert(TextEncoder<
                      utf32bom_encoding,
                      char*>());
}

void test_text_decoder_models() {
    // Archetypes
    static_assert(TextRandomAccessDecoder<
                      text_encoding_archetype,
                      code_unit_iterator_archetype>());
    // std
    static_assert(TextForwardDecoder<
                      execution_character_encoding,
                      char*>());
    static_assert(TextForwardDecoder<
                      execution_wide_character_encoding,
                      wchar_t*>());
    // FIXME: If P0482 were to be adopted, replace char with char8_t.
    static_assert(TextBidirectionalDecoder<
                      char8_character_encoding,
                      char*>());
    static_assert(TextBidirectionalDecoder<
                      char16_character_encoding,
                      char16_t*>());
    static_assert(TextRandomAccessDecoder<
                      char32_character_encoding,
                      char32_t*>());
    static_assert(TextRandomAccessDecoder<
                      basic_execution_character_encoding,
                      char*>());
    static_assert(TextRandomAccessDecoder<
                      basic_execution_wide_character_encoding,
                      wchar_t*>());
#if defined(__STDC_ISO_10646__)
    static_assert(TextRandomAccessDecoder<
                      iso_10646_wide_character_encoding,
                      wchar_t*>());
#endif // __STDC_ISO_10646__
    static_assert(TextBidirectionalDecoder<
                      utf8_encoding,
                      char*>());
    static_assert(TextBidirectionalDecoder<
                      utf8bom_encoding,
                      char*>());
    static_assert(TextBidirectionalDecoder<
                      utf16_encoding,
                      char16_t*>());
    static_assert(TextBidirectionalDecoder<
                      utf16be_encoding,
                      char*>());
    static_assert(TextBidirectionalDecoder<
                      utf16le_encoding,
                      char*>());
    static_assert(TextBidirectionalDecoder<
                      utf16bom_encoding,
                      char*>());
    static_assert(TextRandomAccessDecoder<
                      utf32_encoding,
                      char32_t*>());
    static_assert(TextRandomAccessDecoder<
                      utf32be_encoding,
                      char*>());
    static_assert(TextRandomAccessDecoder<
                      utf32le_encoding,
                      char*>());
    static_assert(TextBidirectionalDecoder<
                      utf32bom_encoding,
                      char*>());

    // Expected model failures.
    static_assert(! TextRandomAccessDecoder<
                      utf8_encoding,
                      char*>());
    static_assert(! TextRandomAccessDecoder<
                      utf8bom_encoding,
                      char*>());
    static_assert(! TextRandomAccessDecoder<
                      utf16_encoding,
                      char16_t*>());
    static_assert(! TextRandomAccessDecoder<
                      utf16be_encoding,
                      char*>());
    static_assert(! TextRandomAccessDecoder<
                      utf16le_encoding,
                      char*>());
    static_assert(! TextRandomAccessDecoder<
                      utf16bom_encoding,
                      char*>());
    static_assert(! TextRandomAccessDecoder<
                      utf32bom_encoding,
                      char*>());
}

void test_text_iterator_models() {
    // Archetypes
    static_assert(TextIterator<text_iterator_archetype>());
    static_assert(TextIterator<text_view_archetype::iterator>());
    // std input iterators
    static_assert(TextIterator<itext_iterator<basic_execution_character_encoding, text_detail::basic_view<char*>>>());
    static_assert(TextIterator<itext_iterator<basic_execution_wide_character_encoding, text_detail::basic_view<wchar_t*>>>());
#if defined(__STDC_ISO_10646__)
    static_assert(TextIterator<itext_iterator<iso_10646_wide_character_encoding, text_detail::basic_view<wchar_t*>>>());
#endif
    // FIXME: If P0482 were to be adopted, replace char with char8_t.
    static_assert(TextIterator<itext_iterator<utf8_encoding, text_detail::basic_view<char*>>>());
    static_assert(TextIterator<itext_iterator<utf8bom_encoding, text_detail::basic_view<char*>>>());
    static_assert(TextIterator<itext_iterator<utf16_encoding, text_detail::basic_view<char16_t*>>>());
    static_assert(TextIterator<itext_iterator<utf16be_encoding, text_detail::basic_view<char*>>>());
    static_assert(TextIterator<itext_iterator<utf16le_encoding, text_detail::basic_view<char*>>>());
    static_assert(TextIterator<itext_iterator<utf16bom_encoding, text_detail::basic_view<char*>>>());
    static_assert(TextIterator<itext_iterator<utf32_encoding, text_detail::basic_view<char32_t*>>>());
    static_assert(TextIterator<itext_iterator<utf32be_encoding, text_detail::basic_view<char*>>>());
    static_assert(TextIterator<itext_iterator<utf32le_encoding, text_detail::basic_view<char*>>>());
    static_assert(TextIterator<itext_iterator<utf32bom_encoding, text_detail::basic_view<char*>>>());
}

void test_text_sentinel_models() {
    // Archetypes
    static_assert(TextSentinel<text_iterator_archetype, text_iterator_archetype>());
    static_assert(TextSentinel<
                      ranges::sentinel_t<text_view_archetype>,
                      ranges::iterator_t<text_view_archetype>>());
    // std sentinels
    static_assert(TextSentinel<
                      itext_sentinel<basic_execution_character_encoding, text_detail::basic_view<char*>>,
                      itext_iterator<basic_execution_character_encoding, text_detail::basic_view<char*>>>());
    static_assert(TextSentinel<
                      itext_sentinel<basic_execution_wide_character_encoding, text_detail::basic_view<wchar_t*>>,
                      itext_iterator<basic_execution_wide_character_encoding, text_detail::basic_view<wchar_t*>>>());
#if defined(__STDC_ISO_10646__)
    static_assert(TextSentinel<
                      itext_sentinel<iso_10646_wide_character_encoding, text_detail::basic_view<wchar_t*>>,
                      itext_iterator<iso_10646_wide_character_encoding, text_detail::basic_view<wchar_t*>>>());
#endif
    // FIXME: If P0482 were to be adopted, replace char with char8_t.
    static_assert(TextSentinel<
                      itext_sentinel<utf8_encoding, text_detail::basic_view<char*>>,
                      itext_iterator<utf8_encoding, text_detail::basic_view<char*>>>());
    static_assert(TextSentinel<
                      itext_sentinel<utf8bom_encoding, text_detail::basic_view<char*>>,
                      itext_iterator<utf8bom_encoding, text_detail::basic_view<char*>>>());
    static_assert(TextSentinel<
                      itext_sentinel<utf16_encoding, text_detail::basic_view<char16_t*>>,
                      itext_iterator<utf16_encoding, text_detail::basic_view<char16_t*>>>());
    static_assert(TextSentinel<
                      itext_sentinel<utf16be_encoding, text_detail::basic_view<char*>>,
                      itext_iterator<utf16be_encoding, text_detail::basic_view<char*>>>());
    static_assert(TextSentinel<
                      itext_sentinel<utf16le_encoding, text_detail::basic_view<char*>>,
                      itext_iterator<utf16le_encoding, text_detail::basic_view<char*>>>());
    static_assert(TextSentinel<
                      itext_sentinel<utf16bom_encoding, text_detail::basic_view<char*>>,
                      itext_iterator<utf16bom_encoding, text_detail::basic_view<char*>>>());
    static_assert(TextSentinel<
                      itext_sentinel<utf32_encoding, text_detail::basic_view<char32_t*>>,
                      itext_iterator<utf32_encoding, text_detail::basic_view<char32_t*>>>());
    static_assert(TextSentinel<
                      itext_sentinel<utf32be_encoding, text_detail::basic_view<char*>>,
                      itext_iterator<utf32be_encoding, text_detail::basic_view<char*>>>());
    static_assert(TextSentinel<
                      itext_sentinel<utf32le_encoding, text_detail::basic_view<char*>>,
                      itext_iterator<utf32le_encoding, text_detail::basic_view<char*>>>());
    static_assert(TextSentinel<
                      itext_sentinel<utf32bom_encoding, text_detail::basic_view<char*>>,
                      itext_iterator<utf32bom_encoding, text_detail::basic_view<char*>>>());
}

void test_text_output_iterator_models() {
    // Archetypes
    static_assert(TextOutputIterator<text_output_iterator_archetype>());
    static_assert(TextOutputIterator<otext_iterator<text_encoding_archetype, code_unit_iterator_archetype>>());
    // std output iterators
    static_assert(TextOutputIterator<otext_iterator<basic_execution_character_encoding, char*>>());
    static_assert(TextOutputIterator<otext_iterator<basic_execution_wide_character_encoding, wchar_t*>>());
#if defined(__STDC_ISO_10646__)
    static_assert(TextOutputIterator<otext_iterator<iso_10646_wide_character_encoding, wchar_t*>>());
#endif
    // FIXME: If P0482 were to be adopted, replace char with char8_t.
    static_assert(TextOutputIterator<otext_iterator<utf8_encoding, char*>>());
    static_assert(TextOutputIterator<otext_iterator<utf8bom_encoding, char*>>());
    static_assert(TextOutputIterator<otext_iterator<utf16_encoding, char16_t*>>());
    static_assert(TextOutputIterator<otext_iterator<utf16be_encoding, char*>>());
    static_assert(TextOutputIterator<otext_iterator<utf16le_encoding, char*>>());
    static_assert(TextOutputIterator<otext_iterator<utf16bom_encoding, char*>>());
    static_assert(TextOutputIterator<otext_iterator<utf32_encoding, char32_t*>>());
    static_assert(TextOutputIterator<otext_iterator<utf32be_encoding, char*>>());
    static_assert(TextOutputIterator<otext_iterator<utf32le_encoding, char*>>());
    static_assert(TextOutputIterator<otext_iterator<utf32bom_encoding, char*>>());
}

void test_text_view_models() {
    // Archetypes
    static_assert(TextView<text_view_archetype>());
    // std
    static_assert(TextView<text_view>());
    static_assert(TextView<wtext_view>());
    static_assert(TextView<u8text_view>());
    static_assert(TextView<u16text_view>());
    static_assert(TextView<u32text_view>());
    static_assert(TextView<basic_text_view<execution_character_encoding, text_detail::basic_view<char*>>>());
    static_assert(TextView<basic_text_view<execution_wide_character_encoding, text_detail::basic_view<wchar_t*>>>());
    // FIXME: If P0482 were to be adopted, replace char with char8_t.
    static_assert(TextView<basic_text_view<char8_character_encoding, text_detail::basic_view<char*>>>());
    static_assert(TextView<basic_text_view<char16_character_encoding, text_detail::basic_view<char16_t*>>>());
    static_assert(TextView<basic_text_view<char32_character_encoding, text_detail::basic_view<char32_t*>>>());

    // Validate that basic_text_view instantiation fails with range types that
    // are not views.
    static_assert(InvalidSpecialization<basic_text_view, execution_character_encoding, char(&)[1]>());
    static_assert(InvalidSpecialization<basic_text_view, execution_character_encoding, string>());
    static_assert(InvalidSpecialization<basic_text_view, execution_character_encoding, vector<char>>());
}

int main() {
    test_error_policy_models();
    test_code_unit_models();
    test_code_point_models();
    test_character_set_models();
    test_character_models();
    test_code_unit_iterator_models();
    test_code_unit_output_iterator_models();
    test_text_encoding_state_models();
    test_text_encoding_state_transition_models();
    test_text_encoding_models();
    test_text_encoder_models();
    test_text_decoder_models();
    test_text_iterator_models();
    test_text_sentinel_models();
    test_text_output_iterator_models();
    test_text_view_models();

    return 0;
}
