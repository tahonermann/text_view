#include <stdtext/adl_customization.hpp>
#include <algorithm>
#include <cassert>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <forward_list>
#include <list>
#include <vector>
#include <utility>
#include <string>
#include <text>


using namespace std;
using namespace std::experimental::text;


// Class to associate a character with its encoded code unit sequence.
template<Character CT, Code_unit CUT>
struct encoded_character {
    CT character;
    vector<CUT> code_units;
};


// Input iterator model.  The standard doesn't provide input iterator types that
// don't also satisfy forward iterator requirements or impose additional
// requirements.  istream_iterator, for example, requires a char_traits
// specialization for its character type.
template<origin::Input_iterator IT>
struct input_iterator
    : public iterator<
                 input_iterator_tag,
                 typename iterator_traits<IT>::value_type,
                 typename iterator_traits<IT>::difference_type,
                 typename iterator_traits<IT>::pointer,
                 typename iterator_traits<IT>::reference>
{
    input_iterator(IT it) : it(it) {}

    auto operator==(const input_iterator& o) const -> bool {
        return it == o.it;
    }
    auto operator!=(const input_iterator& o) const -> bool {
        return it != o.it;
    }

    auto operator*() const -> typename iterator_traits<IT>::reference {
        return *it;
    }

    auto operator++() -> input_iterator& {
        ++it;
        return *this;
    }
    auto operator++(int) -> input_iterator {
        input_iterator tmp{*this};
        ++*this;
        return tmp;
    }

private:
    IT it;
};


// Output iterator model.  The standard doesn't provide output iterator types
// that don't also satisfy forward iterator requirements or impose additional
// requirements.  ostream_iterator, for example, requires a char_traits
// specialization for its character type.
template<
    typename IT,
    typename T>
requires origin::Output_iterator<IT, T>()
struct output_iterator
    : public iterator<
                 output_iterator_tag,
                 typename iterator_traits<IT>::value_type,
                 typename iterator_traits<IT>::difference_type,
                 typename iterator_traits<IT>::pointer,
                 typename iterator_traits<IT>::reference>
{
    output_iterator(IT it) : it(it) {}

    auto operator==(const output_iterator& o) const -> bool {
        return it == o.it;
    }
    auto operator!=(const output_iterator& o) const -> bool {
        return it != o.it;
    }

    auto operator*() -> output_iterator& {
        return *this;
    }
    void operator=(const T& t) {
        *it = t;
    }

    auto operator++() -> output_iterator& {
        ++it;
        return *this;
    }
    auto operator++(int) -> output_iterator {
        output_iterator tmp{*this};
        ++*this;
        return tmp;
    }

private:
    IT it;
};


// Input range model.  The standard doesn't provide a container type with
// input iterators that aren't also forward, bidirectional, or random access
// iterators.  input_range_view is used to provide wrapped input iterators
// for another container type.
template<origin::Range RT>
struct input_range_view {
    input_range_view(RT &r) : r(r) {}

    auto begin() const
        -> input_iterator<decltype(detail::adl_begin(declval<const RT>()))>
    {
        return input_iterator<
            decltype(detail::adl_begin(declval<const RT>()))>{detail::adl_begin(r)};
    }
    auto end() const
        -> input_iterator<decltype(detail::adl_end(declval<const RT>()))>
    {
        return input_iterator<
            decltype(detail::adl_end(declval<const RT>()))>{detail::adl_end(r)};
    }

private:
    RT &r;
};


void test_code_unit_models() {
    // Archetypes
    static_assert(Code_unit<code_unit_archetype>(), "");
    // std
    static_assert(Code_unit<char>(), "");
    static_assert(Code_unit<wchar_t>(), "");
    static_assert(Code_unit<char16_t>(), "");
    static_assert(Code_unit<char32_t>(), "");
    static_assert(Code_unit<unsigned char>(), "");
    static_assert(Code_unit<unsigned short>(), "");
    static_assert(Code_unit<unsigned int>(), "");
    static_assert(Code_unit<unsigned long>(), "");
    static_assert(Code_unit<unsigned long long>(), "");
}

void test_code_point_models() {
    // Archetypes
    static_assert(Code_point<code_point_archetype>(), "");
    // std
    static_assert(Code_point<char>(), "");
    static_assert(Code_point<wchar_t>(), "");
    static_assert(Code_point<char16_t>(), "");
    static_assert(Code_point<char32_t>(), "");
    static_assert(Code_point<unsigned char>(), "");
    static_assert(Code_point<unsigned short>(), "");
    static_assert(Code_point<unsigned int>(), "");
    static_assert(Code_point<unsigned long>(), "");
    static_assert(Code_point<unsigned long long>(), "");
}

void test_character_set_models() {
    // Archetypes
    static_assert(Character_set<character_set_archetype>(), "");
    // std
    static_assert(Character_set<execution_character_set>(), "");
    static_assert(Character_set<execution_wide_character_set>(), "");
    static_assert(Character_set<universal_character_set>(), "");
    static_assert(Character_set<any_character_set>(), "");
}

void test_character_models() {
    // Archetypes
    static_assert(Character<character_archetype>(), "");
    // std
    static_assert(Character<character<execution_character_set>>(), "");
    static_assert(Character<character<execution_wide_character_set>>(), "");
    static_assert(Character<character<universal_character_set>>(), "");
    static_assert(Character<character<any_character_set>>(), "");
}

void test_code_unit_iterator_models() {
    // Archetypes
    static_assert(Code_unit_iterator<code_unit_iterator_archetype>(), "");
    // std
    static_assert(Code_unit_iterator<char*>(), "");
    static_assert(Code_unit_iterator<const char*>(), "");
    static_assert(Code_unit_iterator<wchar_t*>(), "");
    static_assert(Code_unit_iterator<char16_t*>(), "");
    static_assert(Code_unit_iterator<char32_t*>(), "");
}

void test_codec_state_models() {
    // Archetypes
    static_assert(Codec_state<codec_state_archetype>(), "");
    // std
    static_assert(Codec_state<trivial_codec_state>(), "");
}

void test_codec_models() {
    // Archetypes
    static_assert(Codec<codec_archetype>(), "");
    static_assert(Bidirectional_codec<codec_archetype>(), "");
    // std
    static_assert(Bidirectional_codec<trivial_codec<character_archetype, code_unit_archetype>>(), "");
    static_assert(Bidirectional_codec<utf8_encoding::codec_type>(), "");
    static_assert(Bidirectional_codec<utf16_encoding::codec_type>(), "");
    static_assert(Bidirectional_codec<utf16be_encoding::codec_type>(), "");
    static_assert(Bidirectional_codec<utf16le_encoding::codec_type>(), "");
    static_assert(Bidirectional_codec<utf32_encoding::codec_type>(), "");
    static_assert(Bidirectional_codec<utf32be_encoding::codec_type>(), "");
    static_assert(Bidirectional_codec<utf32le_encoding::codec_type>(), "");
    static_assert(Codec<basic_execution_character_encoding::codec_type>(), "");
    static_assert(Codec<basic_execution_wide_character_encoding::codec_type>(), "");
#if defined(__STDC_ISO_10646__)
    static_assert(Bidirectional_codec<iso_10646_wide_character_encoding::codec_type>(), "");
#endif // __STDC_ISO_10646__
    static_assert(Codec<execution_character_encoding::codec_type>(), "");
    static_assert(Codec<execution_wide_character_encoding::codec_type>(), "");
    static_assert(Bidirectional_codec<char8_character_encoding::codec_type>(), "");
    static_assert(Bidirectional_codec<char16_character_encoding::codec_type>(), "");
    static_assert(Bidirectional_codec<char32_character_encoding::codec_type>(), "");
}

void test_encoding_models() {
    // Archetypes
    static_assert(Encoding<encoding_archetype>(), "");
    // std
    static_assert(Encoding<encoding_type_of<char*>>(), "");
    static_assert(Encoding<encoding_type_of<const char*>>(), "");
    static_assert(Encoding<encoding_type_of<char* const>>(), "");
    static_assert(Encoding<encoding_type_of<const char* const>>(), "");
    static_assert(Encoding<encoding_type_of<char[]>>(), "");
    static_assert(Encoding<encoding_type_of<const char[]>>(), "");
    static_assert(Encoding<encoding_type_of<char[5]>>(), "");
    static_assert(Encoding<encoding_type_of<const char[5]>>(), "");
    static_assert(Encoding<encoding_type_of<char(&)[5]>>(), "");
    static_assert(Encoding<encoding_type_of<const char(&)[5]>>(), "");
    static_assert(Encoding<encoding_type_of<wchar_t*>>(), "");
    static_assert(Encoding<encoding_type_of<char16_t*>>(), "");
    static_assert(Encoding<encoding_type_of<char32_t*>>(), "");
    static_assert(Encoding<utf8_encoding>(), "");
    static_assert(Encoding<utf16_encoding>(), "");
    static_assert(Encoding<utf16be_encoding>(), "");
    static_assert(Encoding<utf16le_encoding>(), "");
    static_assert(Encoding<utf32_encoding>(), "");
    static_assert(Encoding<utf32be_encoding>(), "");
    static_assert(Encoding<utf32le_encoding>(), "");
    static_assert(Encoding<basic_execution_character_encoding>(), "");
    static_assert(Encoding<basic_execution_wide_character_encoding>(), "");
#if defined(__STDC_ISO_10646__)
    static_assert(Encoding<iso_10646_wide_character_encoding>(), "");
#endif
    static_assert(Encoding<execution_character_encoding>(), "");
    static_assert(Encoding<execution_wide_character_encoding>(), "");
    static_assert(Encoding<char8_character_encoding>(), "");
    static_assert(Encoding<char16_character_encoding>(), "");
    static_assert(Encoding<char32_character_encoding>(), "");
}

void test_text_iterator_models() {
    // Archetypes
    static_assert(Text_iterator<text_iterator_archetype>(), "");
    static_assert(Text_iterator<itext_iterator<encoding_archetype, code_unit_iterator_archetype>>(), "");
    static_assert(Text_iterator<otext_iterator<encoding_archetype, code_unit_iterator_archetype>>(), "");
    static_assert(Text_iterator<rotext_iterator<encoding_archetype, code_unit_iterator_archetype>>(), "");
    // std input iterators
    static_assert(Text_iterator<itext_iterator<basic_execution_character_encoding, char*>>(), "");
    static_assert(Text_iterator<itext_iterator<basic_execution_wide_character_encoding, wchar_t*>>(), "");
#if defined(__STDC_ISO_10646__)
    static_assert(Text_iterator<itext_iterator<iso_10646_wide_character_encoding, wchar_t*>>(), "");
#endif
    static_assert(Text_iterator<itext_iterator<utf8_encoding, uint_least8_t*>>(), "");
    static_assert(Text_iterator<itext_iterator<utf16_encoding, char16_t*>>(), "");
    static_assert(Text_iterator<itext_iterator<utf16be_encoding, uint_least8_t*>>(), "");
    static_assert(Text_iterator<itext_iterator<utf16le_encoding, uint_least8_t*>>(), "");
    static_assert(Text_iterator<itext_iterator<utf32_encoding, char32_t*>>(), "");
    static_assert(Text_iterator<itext_iterator<utf32be_encoding, uint_least8_t*>>(), "");
    static_assert(Text_iterator<itext_iterator<utf32le_encoding, uint_least8_t*>>(), "");
    // std output iterators
    static_assert(Text_iterator<otext_iterator<basic_execution_character_encoding, char*>>(), "");
    static_assert(Text_iterator<otext_iterator<basic_execution_wide_character_encoding, wchar_t*>>(), "");
#if defined(__STDC_ISO_10646__)
    static_assert(Text_iterator<otext_iterator<iso_10646_wide_character_encoding, wchar_t*>>(), "");
#endif
    static_assert(Text_iterator<otext_iterator<utf8_encoding, uint_least8_t*>>(), "");
    static_assert(Text_iterator<otext_iterator<utf16_encoding, char16_t*>>(), "");
    static_assert(Text_iterator<otext_iterator<utf16be_encoding, uint_least8_t*>>(), "");
    static_assert(Text_iterator<otext_iterator<utf16le_encoding, uint_least8_t*>>(), "");
    static_assert(Text_iterator<otext_iterator<utf32_encoding, char32_t*>>(), "");
    static_assert(Text_iterator<otext_iterator<utf32be_encoding, uint_least8_t*>>(), "");
    static_assert(Text_iterator<otext_iterator<utf32le_encoding, uint_least8_t*>>(), "");
    // std reverse output iterators
#if defined(__STDC_ISO_10646__)
    static_assert(Text_iterator<rotext_iterator<iso_10646_wide_character_encoding, wchar_t*>>(), "");
#endif
    static_assert(Text_iterator<rotext_iterator<utf8_encoding, uint_least8_t*>>(), "");
    static_assert(Text_iterator<rotext_iterator<utf16_encoding, char16_t*>>(), "");
    static_assert(Text_iterator<rotext_iterator<utf16be_encoding, char16_t*>>(), "");
    static_assert(Text_iterator<rotext_iterator<utf16le_encoding, char16_t*>>(), "");
    static_assert(Text_iterator<rotext_iterator<utf32_encoding, char32_t*>>(), "");
    static_assert(Text_iterator<rotext_iterator<utf32be_encoding, char32_t*>>(), "");
    static_assert(Text_iterator<rotext_iterator<utf32le_encoding, char32_t*>>(), "");
}

void test_text_view_models() {
    // Archetypes
    static_assert(Text_view<text_view_archetype>(), "");
    // std
    static_assert(Text_view<text_view<execution_character_encoding, char*>>(), "");
    static_assert(Text_view<text_view<execution_character_encoding, const char*>>(), "");
    static_assert(Text_view<text_view<execution_character_encoding, char[5]>>(), "");
    static_assert(Text_view<text_view<execution_character_encoding, const char[5]>>(), "");
    static_assert(Text_view<text_view<execution_character_encoding, char(&)[5]>>(), "");
    static_assert(Text_view<text_view<execution_character_encoding, const char(&)[5]>>(), "");
    static_assert(Text_view<text_view<execution_wide_character_encoding, wchar_t*>>(), "");
    static_assert(Text_view<text_view<char8_character_encoding, uint_least8_t*>>(), "");
    static_assert(Text_view<text_view<char16_character_encoding, char16_t*>>(), "");
    static_assert(Text_view<text_view<char32_character_encoding, char32_t*>>(), "");
}

// Test forward encoding of the character sequence present in the
// 'encoded_characters' vector and ensure that the encoded code units match.
// 'it' is expected to be an output iterator for this test.  Characters are
// encoded via 'it' which must write the resulting code units to the container
// reflected by 'code_unit_range'.
template<
    Encoding E,
    Character CT,
    Code_unit CUT,
    origin::Input_range RT,
    Text_iterator TIT>
requires origin::Output_iterator<TIT, typename E::codec_type::character_type>()
void test_forward_encode(
    const vector<encoded_character<CT, CUT>> &encoded_characters,
    const RT &code_unit_range,
    TIT it)
{
    for (const auto &ecp : encoded_characters) {
        *it++ = ecp.character;
    }
    auto code_unit_it = begin(code_unit_range);
    for (const auto &ecp : encoded_characters) {
        for (auto ecu : ecp.code_units) {
            auto rcu = *code_unit_it++;
            assert(ecu == rcu);
        }
    }
}

// Test forward encoding of the character sequence present in the
// 'encoded_characters' vector and ensure that the encoded code units match.
// 'it' is expected to be a forward, bidirectional, or random access iterator
// for this test.  Characters are encoded via 'it' which must write the
// resulting code units to the container reflected by 'code_unit_range'.
template<
    Encoding E,
    Character CT,
    Code_unit CUT,
    origin::Input_range RT,
    Text_iterator TIT>
requires origin::Forward_iterator<TIT>()
void test_forward_encode(
    const vector<encoded_character<CT, CUT>> &encoded_characters,
    const RT &code_unit_range,
    TIT it)
{
    for (const auto &ecp : encoded_characters) {
        *it++ = ecp.character;
    }
    assert(it.base() == end(code_unit_range));
    auto code_unit_it = begin(code_unit_range);
    for (const auto &ecp : encoded_characters) {
        for (auto ecu : ecp.code_units) {
            auto rcu = *code_unit_it++;
            assert(ecu == rcu);
        }
    }
    assert(code_unit_it == end(code_unit_range));
}

// Test reverse encoding of the character sequence present in the
// 'encoded_characters' vector and ensure that the encoded code units match.
// 'it' is expected to be an output iterator.  Characters are encoded via 'it'
// which must write the resulting code units to the container reflected by
// 'code_unit_range'.  Note that the characters are forward encoded, but the
// code unit sequence for each character is reverse encoded.
template<
    Encoding E,
    Character CT,
    Code_unit CUT,
    origin::Input_range RT,
    Text_iterator TIT>
requires Bidirectional_codec<typename E::codec_type>()
      && origin::Output_iterator<TIT, typename E::codec_type::character_type>()
void test_reverse_encode(
    const vector<encoded_character<CT, CUT>> &encoded_characters,
    const RT &code_unit_range,
    TIT it)
{
    for (const auto &ecp : encoded_characters) {
        *it++ = ecp.character;
    }
    auto code_unit_it = begin(code_unit_range);
    for (const auto &ecp : encoded_characters) {
        for (auto ecuit = end(ecp.code_units);
             ecuit != begin(ecp.code_units);
             )
        {
            assert(*--ecuit == *code_unit_it++);
        }
    }
}

// Test reverse encoding of the character sequence present in the
// 'encoded_characters' vector and ensure that the encoded code units match.
// 'it' is expected to be a forward, bidirectional, or random access iterator.
// Characters are encoded 'it' which must write the resulting code units to the
// container reflected by 'code_unit_range'.  Note that the characters are
// forward encoded, but the code unit sequence for each character is reverse
// encoded.
template<
    Encoding E,
    Character CT,
    Code_unit CUT,
    origin::Input_range RT,
    Text_iterator TIT>
requires Bidirectional_codec<typename E::codec_type>()
      && origin::Forward_iterator<TIT>()
void test_reverse_encode(
    const vector<encoded_character<CT, CUT>> &encoded_characters,
    const RT &code_unit_range,
    TIT it)
{
    for (const auto &ecp : encoded_characters) {
        *it++ = ecp.character;
    }
    assert(it.base() == end(code_unit_range));
    auto code_unit_it = begin(code_unit_range);
    for (const auto &ecp : encoded_characters) {
        for (auto ecuit = end(ecp.code_units);
             ecuit != begin(ecp.code_units);
             )
        {
            assert(*--ecuit == *code_unit_it++);
        }
    }
    assert(code_unit_it == end(code_unit_range));
}

// Test forward decoding of the code unit sequence present in the
// 'code_unit_range' range using the 'tv' text view of that range.  The
// 'encoded_characters' vector provides the characters and code unit sequences
// to compare against.  'tv' is expected to provide input iterators for this
// test.  Despite the text iterators being input iterators, this test presumes
// that text view iteration is restartable so that pre and post increment
// iteration and iterator equality comparisons can be tested.
template<
    Encoding E,
    Character CT,
    Code_unit CUT,
    origin::Input_range RT,
    Text_view TVT>
requires origin::Input_iterator<origin::Iterator_type<TVT>>()
void test_forward_decode(
    const vector<encoded_character<CT, CUT>> &encoded_characters,
    const RT &code_unit_range,
    TVT tv)
{
    // Validate pre-increment iteration.
    auto tvit = begin(tv);
    for (const auto &ecp : encoded_characters) {
        // Decode and advance.
        auto tvcp = *tvit;
        ++tvit;
        // Validate the decoded character.
        assert(tvcp == ecp.character);
    }
    // Validate base code unit iterators.
    assert(tvit.base() == end(code_unit_range));
    assert(tvit == end(tv));

    // Validate post-increment iteration.
    tvit = begin(tv);
    for (const auto &ecp : encoded_characters) {
        // Decode and advance.
        auto tvcp = *tvit++;
        // Validate the decoded character.
        assert(tvcp == ecp.character);
    }
    // Validate base code unit iterators.
    assert(tvit.base() == end(code_unit_range));
    assert(tvit == end(tv));

    // Validate iterator equality comparison.
    assert(begin(tv) == begin(tv));
    assert(begin(tv) != end(tv));
}

// Test forward decoding of the code unit sequence present in the
// 'code_unit_range' range using the 'tv' text view of that range.  The
// 'encoded_characters' vector provides the characters and code unit sequences
// to compare against.  'tv' is expected to provide forward, bidirectional, or
// random access iterators for this test.
template<
    Encoding E,
    Character CT,
    Code_unit CUT,
    origin::Input_range RT,
    Text_view TVT>
requires origin::Forward_iterator<origin::Iterator_type<TVT>>()
void test_forward_decode(
    const vector<encoded_character<CT, CUT>> &encoded_characters,
    const RT &code_unit_range,
    TVT tv)
{
    // Validate pre-increment iteration.
    auto tvit = begin(tv);
    auto cuit = begin(code_unit_range);
    for (const auto &ecp : encoded_characters) {
        // Validate base code unit iterators.
        assert(tvit.base() == cuit);
        assert(begin(tvit) == cuit);
        advance(cuit, ecp.code_units.size());
        assert(end(tvit) == cuit);
        // Validate the underlying code unit sequence.
        assert(equal(begin(tvit), end(tvit), begin(ecp.code_units)));
        // Decode and advance.
        auto tvcp = *tvit;
        ++tvit;
        // Validate the decoded character.
        assert(tvcp == ecp.character);
    }
    // Validate base code unit iterators.
    assert(tvit.base() == cuit);
    assert(begin(tvit) == cuit);
    assert(end(tvit) == cuit);
    assert(begin(tvit) == end(code_unit_range));
    assert(tvit == end(tv));

    // Validate post-increment iteration.
    tvit = begin(tv);
    cuit = begin(code_unit_range);
    for (const auto &ecp : encoded_characters) {
        // Validate base code unit iterators.
        assert(tvit.base() == cuit);
        assert(begin(tvit) == cuit);
        advance(cuit, ecp.code_units.size());
        assert(end(tvit) == cuit);
        // Validate the underlying code unit sequence.
        assert(equal(begin(tvit), end(tvit), begin(ecp.code_units)));
        // Decode and advance.
        auto tvcp = *tvit++;
        // Validate the decoded character.
        assert(tvcp == ecp.character);
    }
    // Validate base code unit iterators.
    assert(tvit.base() == cuit);
    assert(begin(tvit) == cuit);
    assert(end(tvit) == cuit);
    assert(begin(tvit) == end(code_unit_range));
    assert(tvit == end(tv));

    // Validate iterator equality comparison.
    assert(begin(tv) == begin(tv));
    assert(begin(tv) != end(tv));

    // Validate underlying iterator access.
    assert(begin(tv).base() == begin(code_unit_range));
    assert(end(tv).base() == end(code_unit_range));
}

// Test reverse decoding of the code unit sequence present in the
// 'code_unit_range' range using the 'tv' text view of that range.  The
// 'encoded_characters' vector provides the characters and code unit sequences
// to compare against.  'tv' is expected to provide bidirectional or random
// access iterators for this test.
template<
    Encoding E,
    Character CT,
    Code_unit CUT,
    origin::Input_range RT,
    Text_view TVT>
requires origin::Bidirectional_iterator<origin::Iterator_type<TVT>>()
void test_reverse_decode(
    const vector<encoded_character<CT, CUT>> &encoded_characters,
    const RT &code_unit_range,
    TVT tv)
{
    // Validate pre-decrement.
    auto tvit = end(tv);
    auto cuit = end(code_unit_range);
    for (auto ecpit = end(encoded_characters);
         ecpit != begin(encoded_characters);
         )
    {
        // Decode and decrement.
        const auto &ecp = *--ecpit;
        auto tvcp = *--tvit;
        // Validate base code unit iterators.
        assert(end(tvit) == cuit);
        advance(cuit, -ecp.code_units.size());
        assert(begin(tvit) == cuit);
        assert(tvit.base() == cuit);
        // Validate the underlying code unit sequence.
        assert(equal(begin(tvit), end(tvit), begin(ecp.code_units)));
        // Validate the decoded character.
        assert(tvcp == ecp.character);
    }
    // Validate base code unit iterators.
    assert(tvit.base() == cuit);
    assert(begin(tvit) == cuit);
    assert(begin(tvit) == begin(code_unit_range));
    assert(tvit == begin(tv));

    // Validate post-decrement.
    tvit = end(tv);
    cuit = end(code_unit_range);
    for (auto ecpit = end(encoded_characters);
         ecpit != begin(encoded_characters);
         )
    {
        // Decode and decrement.
        const auto &ecp = *--ecpit;
        tvit--;
        auto tvcp = *tvit;
        // Validate base code unit iterators.
        assert(end(tvit) == cuit);
        advance(cuit, -ecp.code_units.size());
        assert(begin(tvit) == cuit);
        assert(tvit.base() == cuit);
        // Validate the underlying code unit sequence.
        assert(equal(begin(tvit), end(tvit), begin(ecp.code_units)));
        // Validate the decoded character.
        assert(tvcp == ecp.character);
    }
    // Validate base code unit iterators.
    assert(tvit.base() == cuit);
    assert(begin(tvit) == cuit);
    assert(begin(tvit) == begin(code_unit_range));
    assert(tvit == begin(tv));
}

// Test random access decoding of the code unit sequence present in the
// 'code_unit_range' range using the 'tv' text view of that range.  The
// 'encoded_characters' vector provides the characters and code unit sequences
// to compare against.  'tv' is expected to provide random access iterators for
// this test.
template<
    Encoding E,
    Character CT,
    Code_unit CUT,
    origin::Input_range RT,
    Text_view TVT>
requires origin::Random_access_iterator<origin::Iterator_type<TVT>>()
void test_random_decode(
    const vector<encoded_character<CT, CUT>> &encoded_characters,
    const RT &code_unit_range,
    TVT tv)
{
    // Validate random access.
    int i = 0;
    auto tvit = begin(tv);
    auto cuit = begin(code_unit_range);
    for (const auto &ecp : encoded_characters) {
        // Validate base code unit iterators.
        assert((tvit+i).base() == cuit);
        assert(begin(tvit+i) == cuit);
        advance(cuit, ecp.code_units.size());
        assert(end(tvit+i) == cuit);
        // Validate the underlying code unit sequence.
        assert(equal(begin(tvit+i), end(tvit+i), begin(ecp.code_units)));
        // Validate the decoded character.
        assert(tvit[i] == ecp.character);
        // Advance.
        ++i;
    }
    // Validate base code unit iterators.
    assert(end(tv).base() == cuit);
    assert(begin(end(tv)) == cuit);
    assert(end(end(tv)) == cuit);

    // Validate random access iterator requirements.
    auto num_characters = encoded_characters.size();
    assert(num_characters > 0);
    assert(begin(tv) <  end(tv));
    assert(begin(tv) <= end(tv));
    assert(end(tv)   >  begin(tv));
    assert(end(tv)   >= begin(tv));
    assert(static_cast<decltype(num_characters)>(end(tv) - begin(tv)) ==
           num_characters);
    assert(begin(tv) + num_characters == end(tv));
    assert(num_characters + begin(tv) == end(tv));
    assert(end(tv) - num_characters == begin(tv));
    assert(-num_characters + end(tv) == begin(tv));
    assert(end(tv)[-num_characters] == *begin(tv));
    tvit = begin(tv);
    tvit += num_characters;
    assert(tvit == end(tv));
    tvit = end(tv);
    tvit -= num_characters;
    assert(tvit == begin(tv));
    tvit = end(tv);
    tvit += -num_characters;
    assert(tvit == begin(tv));
    tvit = begin(tv);
    tvit -= -num_characters;
    assert(tvit == end(tv));
}

// Test forward encoding and decoding of the character sequence and code unit
// sequences present in the 'encoded_characters' vector for the character
// encoding specified by 'E'.  This text exercises encoding and decoding using
// input and output text iterators with underlying input, output, forward,
// bidirectional, and random access iterators.
template<
    Encoding E,
    Character CT,
    Code_unit CUT>
void test_forward_encoding(
    const vector<encoded_character<CT, CUT>> &encoded_characters)
{
    using codec_type = typename E::codec_type;
    using code_unit_type = typename codec_type::code_unit_type;

    int num_code_units = 0;
    for (const auto &ecp : encoded_characters) {
        num_code_units += ecp.code_units.size();
    }

    // Test otext_iterator with an underlying output iterator.
    {
    forward_list<code_unit_type> container;
    fill_n(
        front_insert_iterator<decltype(container)>{container},
        num_code_units,
        code_unit_type{});
    using base_iterator_type =
        output_iterator<decltype(begin(container)), code_unit_type>;
    base_iterator_type base_iterator(begin(container));
    otext_iterator<E, base_iterator_type> it(base_iterator);
    test_forward_encode<E>(encoded_characters, container, it);
    }

    // Test otext_iterator with an underlying forward iterator.
    {
    forward_list<code_unit_type> container;
    fill_n(
        front_insert_iterator<decltype(container)>{container},
        num_code_units,
        code_unit_type{});
    otext_iterator<E, decltype(begin(container))> it(begin(container));
    test_forward_encode<E>(encoded_characters, container, it);
    }

    // Test otext_iterator with an underlying bidirectional iterator.
    {
    list<code_unit_type> container;
    fill_n(
        front_insert_iterator<decltype(container)>{container},
        num_code_units,
        code_unit_type{});
    otext_iterator<E, decltype(begin(container))> it(begin(container));
    test_forward_encode<E>(encoded_characters, container, it);
    }

    // Test otext_iterator with an underlying random access iterator.
    {
    vector<code_unit_type> container(num_code_units);
    otext_iterator<E, decltype(begin(container))> it(begin(container));
    test_forward_encode<E>(encoded_characters, container, it);
    }


    // Test itext_iterator with an underlying input iterator.
    {
    forward_list<code_unit_type> container;
    auto insert_it = container.before_begin();
    for (const auto &ecp : encoded_characters) {
        for (const auto &ecu : ecp.code_units) {
            insert_it = container.insert_after(insert_it, ecu);
        }
    }
    auto input_container =
        input_range_view<forward_list<code_unit_type>>{container};
    auto tv = make_text_view<E>(input_container);
    test_forward_decode<E>(encoded_characters, input_container, tv);
    }

    // Test itext_iterator with an underlying forward iterator.
    {
    forward_list<code_unit_type> container;
    auto insert_it = container.before_begin();
    for (const auto &ecp : encoded_characters) {
        for (const auto &ecu : ecp.code_units) {
            insert_it = container.insert_after(insert_it, ecu);
        }
    }
    auto tv = make_text_view<E>(container);
    test_forward_decode<E>(encoded_characters, container, tv);
    }

    // Test itext_iterator with an underlying bidirectional iterator.
    {
    list<code_unit_type> container;
    for (const auto &ecp : encoded_characters) {
        for (const auto &ecu : ecp.code_units) {
            container.push_back(ecu);
        }
    }
    auto tv = make_text_view<E>(container);
    test_forward_decode<E>(encoded_characters, container, tv);
    }

    // Test itext_iterator with an underlying random access iterator.
    {
    vector<code_unit_type> container;
    for (const auto &ecp : encoded_characters) {
        for (const auto &ecu : ecp.code_units) {
            container.push_back(ecu);
        }
    }
    auto tv = make_text_view<E>(container);
    test_forward_decode<E>(encoded_characters, container, tv);
    }
}

// Test bidirectional encoding and decoding of the character sequence and code
// unit sequences present in the 'encoded_characters' vector for the character
// encoding specified by 'E'.  This text exercises encoding and decoding using
// input text iterators with underlying bidirectional, and random access
// iterators and reverse output text iterators with underlying output, forward,
// bidirectional, and random access iterators.
template<
    Encoding E,
    Character CT,
    Code_unit CUT>
requires Bidirectional_codec<typename E::codec_type>()
void test_bidirectional_encoding(
    const vector<encoded_character<CT, CUT>> &encoded_characters)
{
    test_forward_encoding<E>(encoded_characters);

    using codec_type = typename E::codec_type;
    using code_unit_type = typename codec_type::code_unit_type;

    int num_code_units = 0;
    for (const auto &ecp : encoded_characters) {
        num_code_units += ecp.code_units.size();
    }

    // Test rotext_iterator with an underlying output iterator.
    {
    forward_list<code_unit_type> container;
    fill_n(
        front_insert_iterator<decltype(container)>{container},
        num_code_units,
        code_unit_type{});
    using base_iterator_type =
        output_iterator<decltype(begin(container)), code_unit_type>;
    base_iterator_type base_iterator(begin(container));
    rotext_iterator<E, base_iterator_type> it(base_iterator);
    test_reverse_encode<E>(encoded_characters, container, it);
    }

    // Test rotext_iterator with an underlying forward iterator.
    {
    forward_list<code_unit_type> container;
    fill_n(
        front_insert_iterator<decltype(container)>{container},
        num_code_units,
        code_unit_type{});
    rotext_iterator<E, decltype(begin(container))> it(begin(container));
    test_reverse_encode<E>(encoded_characters, container, it);
    }

    // Test rotext_iterator with an underlying bidirectional iterator.
    {
    list<code_unit_type> container;
    fill_n(
        front_insert_iterator<decltype(container)>{container},
        num_code_units,
        code_unit_type{});
    rotext_iterator<E, decltype(begin(container))> it(begin(container));
    test_reverse_encode<E>(encoded_characters, container, it);
    }

    // Test rotext_iterator with an underlying random access iterator.
    {
    vector<code_unit_type> container(num_code_units);
    rotext_iterator<E, decltype(begin(container))> it(begin(container));
    test_reverse_encode<E>(encoded_characters, container, it);
    }


    // Test itext_iterator with an underlying bidirectional iterator.
    {
    list<code_unit_type> container;
    for (const auto &ecp : encoded_characters) {
        for (const auto &ecu : ecp.code_units) {
            container.push_back(ecu);
        }
    }
    auto tv = make_text_view<E>(container);
    test_reverse_decode<E>(encoded_characters, container, tv);
    }

    // Test itext_iterator with an underlying random access iterator.
    {
    vector<code_unit_type> container;
    for (const auto &ecp : encoded_characters) {
        for (const auto &ecu : ecp.code_units) {
            container.push_back(ecu);
        }
    }
    auto tv = make_text_view<E>(container);
    test_reverse_decode<E>(encoded_characters, container, tv);
    }
}

// Test random access encoding and decoding of the character sequence and code
// unit sequences present in the 'encoded_characters' vector for the character
// encoding specified by 'E'.  This text exercises decoding using input text
// iterators with underlying random access iterators.
template<
    Encoding E,
    Character CT,
    Code_unit CUT>
requires Random_access_codec<typename E::codec_type>()
void test_random_access_encoding(
    const vector<encoded_character<CT, CUT>> &encoded_characters)
{
    test_bidirectional_encoding<E>(encoded_characters);

    using codec_type = typename E::codec_type;
    using code_unit_type = typename codec_type::code_unit_type;

    // Test itext_iterator with an underlying random access iterator.
    {
    vector<code_unit_type> container;
    for (const auto &ecp : encoded_characters) {
        for (const auto &ecu : ecp.code_units) {
            container.push_back(ecu);
        }
    }
    auto tv = make_text_view<E>(container);
    test_random_decode<E>(encoded_characters, container, tv);
    }
}

void test_any_character_set() {
    character<any_character_set> c1(U'\U00011141');
    character<any_character_set> c2(get_character_set_id<unicode_character_set>(), U'\U00011141');
    character<unicode_character_set> c3(U'\U00011141');
    assert(c1 == c1);
    assert(c2 == c2);
    assert(c3 == c3);
    assert(c1 != c2);
    assert(c2 != c1);
    assert(c1.get_character_set_id() != c2.get_character_set_id());
    assert(c1.get_code_point() == c2.get_code_point());
    assert(c2 == c3);
    assert(c3 == c2);
    assert(c2.get_character_set_id() == c3.get_character_set_id());
    assert(c2.get_code_point() == c3.get_code_point());
    c2.set_character_set_id(get_character_set_id<any_character_set>());
    assert(c2 != c3);
    assert(c3 != c2);
    assert(c2.get_character_set_id() != c3.get_character_set_id());
    assert(c2.get_code_point() == c3.get_code_point());
}

void test_utf8_encoding() {
    using CT = character<unicode_character_set>;
    vector<encoded_character<CT, uint_least8_t>> encoded_characters{
        { CT{U'\U00000041'}, { 0x41 } },
        { CT{U'\U00000141'}, { 0xC5, 0x81 } },
        { CT{U'\U00001141'}, { 0xE1, 0x85, 0x81 } },
        { CT{U'\U00011141'}, { 0xF0, 0x91, 0x85, 0x81 } },
        { CT{U'\0'}        , { 0x00 } } };

    test_bidirectional_encoding<utf8_encoding>(encoded_characters);

    string encoded_string(u8"a\U00011141z");
    auto tv = make_text_view<utf8_encoding>(encoded_string);
    auto tit = find(begin(tv), end(tv), CT{U'\U00011141'});
    assert(tit.begin() == begin(encoded_string) + 1);
    assert(tit.end() == begin(encoded_string) + 5);
}

void test_utf16_encoding() {
    using CT = character<unicode_character_set>;
    vector<encoded_character<CT, char16_t>> encoded_characters{
        { CT{U'\U00000041'}, { 0x0041 } },
        { CT{U'\U00000141'}, { 0x0141 } },
        { CT{U'\U00001141'}, { 0x1141 } },
        { CT{U'\U00011141'}, { 0xD804, 0xDD41 } },
        { CT{U'\0'}        , { 0x0000 } } };

    test_bidirectional_encoding<utf16_encoding>(encoded_characters);

    u16string encoded_string(u"a\U00011141z");
    auto tv = make_text_view<utf16_encoding>(encoded_string);
    auto tit = find(begin(tv), end(tv), CT{U'\U00011141'});
    assert(tit.begin() == begin(encoded_string) + 1);
    assert(tit.end() == begin(encoded_string) + 3);
}

void test_utf16be_encoding() {
    using CT = character<unicode_character_set>;
    vector<encoded_character<CT, uint_least8_t>> encoded_characters{
        { CT{U'\U00000041'}, { 0x00, 0x41 } },
        { CT{U'\U00000141'}, { 0x01, 0x41 } },
        { CT{U'\U00001141'}, { 0x11, 0x41 } },
        { CT{U'\U00011141'}, { 0xD8, 0x04, 0xDD, 0x41 } },
        { CT{U'\0'}        , { 0x00, 0x00 } } };

    test_bidirectional_encoding<utf16be_encoding>(encoded_characters);

    string encoded_string("\x00\x61\xD8\x04\xDD\x41\x00\x7A", 8);
    auto tv = make_text_view<utf16be_encoding>(encoded_string);
    auto tit = find(begin(tv), end(tv), CT{U'\U00011141'});
    assert(tit.begin() == begin(encoded_string) + 2);
    assert(tit.end() == begin(encoded_string) + 6);
}

void test_utf16le_encoding() {
    using CT = character<unicode_character_set>;
    vector<encoded_character<CT, uint_least8_t>> encoded_characters{
        { CT{U'\U00000041'}, { 0x41, 0x00 } },
        { CT{U'\U00000141'}, { 0x41, 0x01 } },
        { CT{U'\U00001141'}, { 0x41, 0x11 } },
        { CT{U'\U00011141'}, { 0x04, 0xD8, 0x41, 0xDD } },
        { CT{U'\0'}        , { 0x00, 0x00 } } };

    test_bidirectional_encoding<utf16le_encoding>(encoded_characters);

    string encoded_string("\x61\x00\x04\xD8\x41\xDD\x7A\x00", 8);
    auto tv = make_text_view<utf16le_encoding>(encoded_string);
    auto tit = find(begin(tv), end(tv), CT{U'\U00011141'});
    assert(tit.begin() == begin(encoded_string) + 2);
    assert(tit.end() == begin(encoded_string) + 6);
}

void test_utf32_encoding() {
    using CT = character<unicode_character_set>;
    vector<encoded_character<CT, char32_t>> encoded_characters{
        { CT{U'\U00000041'}, { 0x00000041 } },
        { CT{U'\U00000141'}, { 0x00000141 } },
        { CT{U'\U00001141'}, { 0x00001141 } },
        { CT{U'\U00011141'}, { 0x00011141 } },
        { CT{U'\0'}        , { 0x00000000 } } };

    test_random_access_encoding<utf32_encoding>(encoded_characters);

    u32string encoded_string(U"a\U00011141z");
    auto tv = make_text_view<utf32_encoding>(encoded_string);
    auto tit = find(begin(tv), end(tv), CT{U'\U00011141'});
    assert(tit.begin() == begin(encoded_string) + 1);
    assert(tit.end() == begin(encoded_string) + 2);
}

void test_utf32be_encoding() {
    using CT = character<unicode_character_set>;
    vector<encoded_character<CT, uint_least8_t>> encoded_characters{
        { CT{U'\U00000041'}, { 0x00, 0x00, 0x00, 0x41 } },
        { CT{U'\U00000141'}, { 0x00, 0x00, 0x01, 0x41 } },
        { CT{U'\U00001141'}, { 0x00, 0x00, 0x11, 0x41 } },
        { CT{U'\U00011141'}, { 0x00, 0x01, 0x11, 0x41 } },
        { CT{U'\0'}        , { 0x00, 0x00, 0x00, 0x00 } } };

    test_random_access_encoding<utf32be_encoding>(encoded_characters);

    string encoded_string("\x00\x00\x00\x61\x00\x01\x11\x41\x00\x00\x00\x7A", 12);
    auto tv = make_text_view<utf32be_encoding>(encoded_string);
    auto tit = find(begin(tv), end(tv), CT{U'\U00011141'});
    assert(tit.begin() == begin(encoded_string) + 4);
    assert(tit.end() == begin(encoded_string) + 8);
}

void test_utf32le_encoding() {
    using CT = character<unicode_character_set>;
    vector<encoded_character<CT, uint_least8_t>> encoded_characters{
        { CT{U'\U00000041'}, { 0x41, 0x00, 0x00, 0x00 } },
        { CT{U'\U00000141'}, { 0x41, 0x01, 0x00, 0x00 } },
        { CT{U'\U00001141'}, { 0x41, 0x11, 0x00, 0x00 } },
        { CT{U'\U00011141'}, { 0x41, 0x11, 0x01, 0x00 } },
        { CT{U'\0'}        , { 0x00, 0x00, 0x00, 0x00 } } };

    test_random_access_encoding<utf32le_encoding>(encoded_characters);

    string encoded_string("\x61\x00\x00\x00\x41\x11\x01\x00\x7A\x00\x00\x00", 12);
    auto tv = make_text_view<utf32le_encoding>(encoded_string);
    auto tit = find(begin(tv), end(tv), CT{U'\U00011141'});
    assert(tit.begin() == begin(encoded_string) + 4);
    assert(tit.end() == begin(encoded_string) + 8);
}

int main() {
    test_code_unit_models();
    test_code_point_models();
    test_character_set_models();
    test_character_models();
    test_code_unit_iterator_models();
    test_codec_state_models();
    test_codec_models();
    test_encoding_models();
    test_text_iterator_models();
    test_text_view_models();

    test_any_character_set();

    test_utf8_encoding();
    test_utf16_encoding();
    test_utf16be_encoding();
    test_utf16le_encoding();
    test_utf32_encoding();
    test_utf32be_encoding();
    test_utf32le_encoding();

    return 0;
}
