#include <stdtext/adl_customization.hpp>
#include <stdtext/advance_to.hpp>
#include <stdtext/riterator.hpp>
#include <algorithm>
#include <array>
#include <cassert>
#include <initializer_list>
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
    // FIXME: std::optional would be a better choice than vector here.  The
    // FIXME: intent is to allow associating zero or one characters with a code
    // FIXME: unit sequence.
    vector<CT> character;
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


// Iterable model.  The standard doesn't provide a container type that has
// different return types for begin() and end() as permitted by the N4128
// Iterable concept.  This template class is used to wrap a type that models
// the N4128 Range concept to produce a type that merely models the N4128
// Iterable concept.
template<origin::Range RT>
struct iterable_view {
    using range_type = origin::Remove_reference<RT>;
    using iterator = origin::Iterator_type<RT>;

    struct sentinel {
        sentinel(iterator i) : i{i} {}

        bool operator==(const sentinel& other) const {
            // Sentinels always compare equal regardless of any internal state.
            // See N4128, 10.1 "Sentinel Equality".
            return true;
        }
        bool operator!=(const sentinel& other) const {
            return !(*this == other);
        }

        friend bool operator==(
            const iterator &i,
            const sentinel &s)
        {
            return i == s.i;
        }
        friend bool operator!=(
            const iterator &i,
            const sentinel &s)
        {
            return !(i == s);
        }
        friend bool operator==(
            const sentinel &s,
            const iterator &i)
        {
            return i == s;
        }
        friend bool operator!=(
            const sentinel &s,
            const iterator &i)
        {
            return !(s == i);
        }

        bool operator<(const sentinel& other) const {
            // Sentinels always compare equal regardless of any internal state.
            // See N4128, 10.1 "Sentinel Equality".
            return false;
        }   
        bool operator>(const sentinel& other) const {
            return other < *this;
        }   
        bool operator<=(const sentinel& other) const {
            return !(*this > other);
        }   
        bool operator>=(const sentinel& other) const {
            return !(*this < other);
        }   

        friend bool operator<(
            const iterator &i,
            const sentinel &s)
        {   
            return i < s.i;
        }   
        friend bool operator>(
            const iterator &i,
            const sentinel &s)
        {   
            return i > s.i;
        }   
        friend bool operator<=(
            const iterator &i,
            const sentinel &s)
        {   
            return i <= s.i;
        }   
        friend bool operator>=(
            const iterator &i,
            const sentinel &s)
        {   
            return i >= s.i;
        }   

        friend bool operator<(
            const sentinel &s,
            const iterator &i)
        {
            return s.i < i;
        }
        friend bool operator>(
            const sentinel &s,
            const iterator &i)
        {
            return s.i > i;
        }
        friend bool operator<=(
            const sentinel &s,
            const iterator &i)
        {
            return s.i <= i;
        }
        friend bool operator>=(
            const sentinel &s,
            const iterator &i)
        {
            return s.i >= i;
        }

        iterator base() const {
            return i;
        }

    private:
        iterator i;
    };

    iterable_view(range_type &r) : r{r} {}

    iterator begin() const {
        return detail::adl_begin(r);
    }
    sentinel end() const {
        return sentinel(detail::adl_end(r));
    }

private:
    range_type &r;
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
    static_assert(Text_iterator<text_view_archetype::iterator>(), "");
    static_assert(Text_iterator<otext_iterator<encoding_archetype, code_unit_iterator_archetype>>(), "");
    // std input iterators
    static_assert(Text_iterator<itext_iterator<basic_execution_character_encoding, char(&)[5]>>(), "");
    static_assert(Text_iterator<itext_iterator<basic_execution_wide_character_encoding, wchar_t(&)[5]>>(), "");
#if defined(__STDC_ISO_10646__)
    static_assert(Text_iterator<itext_iterator<iso_10646_wide_character_encoding, wchar_t(&)[5]>>(), "");
#endif
    static_assert(Text_iterator<itext_iterator<utf8_encoding, char(&)[5]>>(), "");
    static_assert(Text_iterator<itext_iterator<utf16_encoding, char16_t(&)[5]>>(), "");
    static_assert(Text_iterator<itext_iterator<utf16be_encoding, uint_least8_t(&)[5]>>(), "");
    static_assert(Text_iterator<itext_iterator<utf16le_encoding, uint_least8_t(&)[5]>>(), "");
    static_assert(Text_iterator<itext_iterator<utf32_encoding, char32_t(&)[5]>>(), "");
    static_assert(Text_iterator<itext_iterator<utf32be_encoding, uint_least8_t(&)[5]>>(), "");
    static_assert(Text_iterator<itext_iterator<utf32le_encoding, uint_least8_t(&)[5]>>(), "");
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
}

void test_text_view_models() {
    // Archetypes
    static_assert(Text_view<text_view_archetype>(), "");
    // std
    static_assert(Text_view<ntext_view>(), "");
    static_assert(Text_view<wtext_view>(), "");
    static_assert(Text_view<u8text_view>(), "");
    static_assert(Text_view<u16text_view>(), "");
    static_assert(Text_view<u32text_view>(), "");
    static_assert(Text_view<text_view<execution_character_encoding, char(&)[5]>>(), "");
    static_assert(Text_view<text_view<execution_character_encoding, const char(&)[5]>>(), "");
    static_assert(Text_view<text_view<execution_wide_character_encoding, wchar_t(&)[5]>>(), "");
    static_assert(Text_view<text_view<char8_character_encoding, uint_least8_t(&)[5]>>(), "");
    static_assert(Text_view<text_view<char16_character_encoding, char16_t(&)[5]>>(), "");
    static_assert(Text_view<text_view<char32_character_encoding, char32_t(&)[5]>>(), "");
    static_assert(Text_view<text_view<execution_character_encoding, string&>>(), "");
    static_assert(Text_view<text_view<execution_character_encoding, array<char, 5>&>>(), "");
    static_assert(Text_view<text_view<execution_character_encoding, vector<char>&>>(), "");
    // FIXME: The following text_view instantiations should be rejected since
    // FIXME: the range type they are instantiated with owns the underlying
    // FIXME: storage.
    static_assert(Text_view<text_view<execution_character_encoding, char[5]>>(), "");
    static_assert(Text_view<text_view<execution_character_encoding, string>>(), "");
    static_assert(Text_view<text_view<execution_character_encoding, array<char, 5>>>(), "");
    static_assert(Text_view<text_view<execution_character_encoding, vector<char>>>(), "");
}

// Test any_character_set.
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

// Test forward encoding of the character sequence present in the
// 'encoded_characters' vector and ensure that the encoded code units match.
// 'it' is expected to be an output iterator for this test.  Characters are
// encoded via 'it' which must write the resulting code units to the container
// reflected by 'code_unit_range'.
template<
    Encoding ET,
    origin::Input_range RT,
    Text_iterator TIT>
requires origin::Output_iterator<TIT, typename ET::codec_type::character_type>()
void test_forward_encode(
    const vector<encoded_character<
        typename ET::codec_type::character_type,
        typename ET::codec_type::code_unit_type>> &encoded_characters,
    const RT &code_unit_range,
    TIT it)
{
    for (const auto &ecs : encoded_characters) {
        for (auto ec : ecs.character) {
            *it++ = ec;
        }
    }
    auto code_unit_it = begin(code_unit_range);
    for (const auto &ecs : encoded_characters) {
        for (auto ecu : ecs.code_units) {
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
    Encoding ET,
    origin::Input_range RT,
    Text_iterator TIT>
requires origin::Forward_iterator<TIT>()
void test_forward_encode(
    const vector<encoded_character<
        typename ET::codec_type::character_type,
        typename ET::codec_type::code_unit_type>> &encoded_characters,
    const RT &code_unit_range,
    TIT it)
{
    for (const auto &ecs : encoded_characters) {
        for (auto ec : ecs.character) {
            *it++ = ec;
        }
    }
    assert(it.base() == end(code_unit_range));
    auto code_unit_it = begin(code_unit_range);
    for (const auto &ecs : encoded_characters) {
        for (auto ecu : ecs.code_units) {
            auto rcu = *code_unit_it++;
            assert(ecu == rcu);
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
    origin::Input_range RT,
    Text_view TVT>
requires origin::Input_iterator<origin::Iterator_type<TVT>>()
void test_forward_decode(
    const vector<encoded_character<
        typename encoding_type_of<TVT>::codec_type::character_type,
        typename encoding_type_of<TVT>::codec_type::code_unit_type>> &encoded_characters,
    const RT &code_unit_range,
    TVT tv)
{
    // Validate pre-increment iteration.
    auto tvit = begin(tv);
    for (const auto &ecs : encoded_characters) {
        for (auto ec : ecs.character) {
            // Decode and advance.
            auto tvcp = *tvit;
            ++tvit;
            // Validate the decoded character.
            assert(tvcp == ec);
        }
    }
    // Validate base code unit iterators.
    assert(tvit.base() == end(code_unit_range));
    assert(tvit == end(tv));

    // Validate post-increment iteration.
    tvit = begin(tv);
    for (const auto &ecs : encoded_characters) {
        for (auto ec : ecs.character) {
            // Decode and advance.
            auto tvcp = *tvit++;
            // Validate the decoded character.
            assert(tvcp == ec);
        }
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
    origin::Forward_range RT,
    Text_view TVT>
requires origin::Forward_iterator<origin::Iterator_type<TVT>>()
void test_forward_decode(
    const vector<encoded_character<
        typename encoding_type_of<TVT>::codec_type::character_type,
        typename encoding_type_of<TVT>::codec_type::code_unit_type>> &encoded_characters,
    const RT &code_unit_range,
    TVT tv)
{
    // Validate pre-increment iteration.
    auto tvit = begin(tv);
    auto cuit = begin(code_unit_range);
    for (const auto &ecs : encoded_characters) {
        // Validate the code unit sequence.
        assert(distance(cuit, detail::advance_to(cuit, end(code_unit_range)))
               >= (int)ecs.code_units.size());
        assert(equal(
            begin(ecs.code_units),
            end(ecs.code_units),
            cuit));

        if (ecs.character.empty()) {
            advance(cuit, ecs.code_units.size());
        } else for (auto ec : ecs.character) {
            // Validate base code unit iterators.
            assert(tvit.base() == cuit);
            assert(begin(tvit.base_range()) == cuit);
            advance(cuit, ecs.code_units.size());
            assert(end(tvit.base_range()) == cuit);
            // Validate the underlying code unit sequence.
            assert(equal(
                begin(tvit.base_range()),
                end(tvit.base_range()),
                begin(ecs.code_units)));
            // Decode and advance.
            auto tvcp = *tvit;
            ++tvit;
            // Validate the decoded character.
            assert(tvcp == ec);
        }
    }
    // Validate base code unit iterators.
    assert(tvit.base() == cuit);
    assert(begin(tvit.base_range()) == cuit);
    assert(end(tvit.base_range()) == cuit);
    assert(begin(tvit.base_range()) == end(code_unit_range));
    assert(tvit == end(tv));

    // Validate post-increment iteration.
    tvit = begin(tv);
    cuit = begin(code_unit_range);
    for (const auto &ecs : encoded_characters) {
        // Validate the underlying code unit sequence.
        assert(distance(cuit, detail::advance_to(cuit, end(code_unit_range)))
               >= (int)ecs.code_units.size());
        assert(equal(
            begin(ecs.code_units),
            end(ecs.code_units),
            cuit));

        if (ecs.character.empty()) {
            advance(cuit, ecs.code_units.size());
        } else for (auto ec : ecs.character) {
            // Validate base code unit iterators.
            assert(tvit.base() == cuit);
            assert(begin(tvit.base_range()) == cuit);
            advance(cuit, ecs.code_units.size());
            assert(end(tvit.base_range()) == cuit);
            // Validate the underlying code unit sequence.
            assert(equal(
                begin(tvit.base_range()),
                end(tvit.base_range()),
                begin(ecs.code_units)));
            // Decode and advance.
            auto tvcp = *tvit++;
            // Validate the decoded character.
            assert(tvcp == ec);
        }
    }
    // Validate base code unit iterators.
    assert(tvit.base() == cuit);
    assert(begin(tvit.base_range()) == cuit);
    assert(end(tvit.base_range()) == cuit);
    assert(begin(tvit.base_range()) == end(code_unit_range));
    assert(tvit == end(tv));

    // Validate iterator equality comparison.
    assert(begin(tv) == begin(tv));
    if (distance(
            begin(code_unit_range),
            detail::advance_to(begin(code_unit_range), end(code_unit_range)))
        > 0)
    {
        assert(begin(tv) != end(tv));
    }

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
    origin::Bidirectional_range RT,
    Text_view TVT>
requires origin::Bidirectional_iterator<origin::Iterator_type<TVT>>()
void test_reverse_decode(
    const vector<encoded_character<
        typename encoding_type_of<TVT>::codec_type::character_type,
        typename encoding_type_of<TVT>::codec_type::code_unit_type>> &encoded_characters,
    const RT &code_unit_range,
    TVT tv)
{
    // Validate pre-decrement.
    auto tvit = detail::advance_to(begin(tv), end(tv));
    auto rcuit = detail::rbegin(code_unit_range);
    for (auto ecsit = detail::rbegin(encoded_characters);
         ecsit != detail::rend(encoded_characters);
         ++ecsit)
    {
        auto& ecs = *ecsit;

        // Validate the code unit sequence.
        assert(distance(rcuit, detail::rend(code_unit_range))
               >= (int)ecs.code_units.size());
        assert(equal(
            detail::rbegin(ecs.code_units),
            detail::rend(ecs.code_units),
            rcuit));

        if (ecs.character.empty()) {
            advance(rcuit, ecs.code_units.size());
        } else for (auto ec : ecs.character) {
            // Decode and decrement.
            auto tvcp = *--tvit;
            // Validate base code unit iterators.
            assert(end(tvit.base_range()) == rcuit.base());
            advance(rcuit, ecs.code_units.size());
            assert(begin(tvit.base_range()) == rcuit.base());
            assert(tvit.base() == rcuit.base());
            // Validate the underlying code unit sequence.
            assert(equal(
                begin(tvit.base_range()),
                end(tvit.base_range()),
                begin(ecs.code_units)));
            // Validate the decoded character.
            assert(tvcp == ec);
        }
    }
    // Validate base code unit iterators.
    assert(tvit.base() == rcuit.base());
    assert(begin(tvit.base_range()) == rcuit.base());
    assert(begin(tvit.base_range()) == begin(code_unit_range));
    assert(tvit == begin(tv));

    // Validate post-decrement.
    tvit = detail::advance_to(begin(tv), end(tv));
    rcuit = detail::rbegin(code_unit_range);
    for (auto ecsit = detail::rbegin(encoded_characters);
         ecsit != detail::rend(encoded_characters);
         ++ecsit)
    {
        auto& ecs = *ecsit;

        // Validate the code unit sequence.
        assert(distance(rcuit, detail::rend(code_unit_range))
               >= (int)ecs.code_units.size());
        assert(equal(
            detail::rbegin(ecs.code_units),
            detail::rend(ecs.code_units),
            rcuit));

        if (ecs.character.empty()) {
            advance(rcuit, ecs.code_units.size());
        } else for (auto ec : ecs.character) {
            // Decode and decrement.
            tvit--;
            auto tvcp = *tvit;
            // Validate base code unit iterators.
            assert(end(tvit.base_range()) == rcuit.base());
            advance(rcuit, ecs.code_units.size());
            assert(begin(tvit.base_range()) == rcuit.base());
            assert(tvit.base() == rcuit.base());
            // Validate the underlying code unit sequence.
            assert(equal(
                begin(tvit.base_range()),
                end(tvit.base_range()),
                begin(ecs.code_units)));
            // Validate the decoded character.
            assert(tvcp == ec);
        }
    }
    // Validate base code unit iterators.
    assert(tvit.base() == rcuit.base());
    assert(begin(tvit.base_range()) == rcuit.base());
    assert(begin(tvit.base_range()) == begin(code_unit_range));
    assert(tvit == begin(tv));
}

// Test random access decoding of the code unit sequence present in the
// 'code_unit_range' range using the 'tv' text view of that range.  The
// 'encoded_characters' vector provides the characters and code unit sequences
// to compare against.  'tv' is expected to provide random access iterators for
// this test.
template<
    origin::Random_access_range RT,
    Text_view TVT>
requires origin::Random_access_iterator<origin::Iterator_type<TVT>>()
void test_random_decode(
    const vector<encoded_character<
        typename encoding_type_of<TVT>::codec_type::character_type,
        typename encoding_type_of<TVT>::codec_type::code_unit_type>> &encoded_characters,
    const RT &code_unit_range,
    TVT tv)
{
    // Validate random access.
    int i = 0;
    auto tvit = begin(tv);
    auto cuit = begin(code_unit_range);
    for (const auto &ecs : encoded_characters) {
        // Validate the underlying code unit sequence.
        assert(distance(cuit, detail::advance_to(cuit, end(code_unit_range)))
               >= (int)ecs.code_units.size());
        assert(equal(
            begin(ecs.code_units),
            end(ecs.code_units),
            cuit));

        if (ecs.character.empty()) {
            advance(cuit, ecs.code_units.size());
        } else for (auto ec : ecs.character) {
            // Validate base code unit iterators.
            assert((tvit+i).base() == cuit);
            assert(begin((tvit+i).base_range()) == cuit);
            advance(cuit, ecs.code_units.size());
            assert(end((tvit+i).base_range()) == cuit);
            // Validate the underlying code unit sequence.
            assert(equal(
                begin((tvit+i).base_range()),
                end((tvit+i).base_range()),
                begin(ecs.code_units)));
            // Validate the decoded character.
            assert(tvit[i] == ec);
        }
        // Advance.
        ++i;
    }
    // Validate base code unit iterators.
    assert(end(tv).base() == cuit);
    assert(begin(detail::advance_to(begin(tv), end(tv)).base_range()) == cuit);
    assert(end(detail::advance_to(begin(tv), end(tv)).base_range()) == cuit);

    // Validate random access iterator requirements.
    auto num_characters = encoded_characters.size();
    assert(num_characters > 0);
    assert(begin(tv) <  end(tv));
    assert(begin(tv) <= end(tv));
    assert(end(tv)   >  begin(tv));
    assert(end(tv)   >= begin(tv));
    assert(static_cast<decltype(num_characters)>(
               detail::advance_to(begin(tv), end(tv)) - begin(tv))
               == num_characters);
    assert(begin(tv) + num_characters == end(tv));
    assert(num_characters + begin(tv) == end(tv));
    assert(detail::advance_to(begin(tv), end(tv)) - num_characters
           == begin(tv));
    assert(-num_characters + detail::advance_to(begin(tv), end(tv))
           == begin(tv));
    assert(detail::advance_to(begin(tv), end(tv))[-num_characters]
           == *begin(tv));
    tvit = begin(tv);
    tvit += num_characters;
    assert(tvit == end(tv));
    tvit = detail::advance_to(begin(tv), end(tv));
    tvit -= num_characters;
    assert(tvit == begin(tv));
    tvit = detail::advance_to(begin(tv), end(tv));
    tvit += -num_characters;
    assert(tvit == begin(tv));
    tvit = begin(tv);
    tvit -= -num_characters;
    assert(tvit == end(tv));
}

// Test forward encoding and decoding of the character sequence and code unit
// sequences present in the 'encoded_characters' vector for the character
// encoding specified by 'ET'.  This text exercises encoding and decoding using
// input and output text iterators with underlying input, output, forward,
// bidirectional, and random access iterators.
template<Encoding ET>
void test_forward_encoding(
    const vector<encoded_character<
        typename ET::codec_type::character_type,
        typename ET::codec_type::code_unit_type>> &encoded_characters)
{
    using codec_type = typename ET::codec_type;
    using code_unit_type = typename codec_type::code_unit_type;

    int num_code_units = 0;
    for (const auto &ecs : encoded_characters) {
        num_code_units += ecs.code_units.size();
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
    otext_iterator<ET, base_iterator_type> it(base_iterator);
    test_forward_encode<ET>(encoded_characters, container, it);
    }

    // Test otext_iterator with an underlying forward iterator.
    {
    forward_list<code_unit_type> container;
    fill_n(
        front_insert_iterator<decltype(container)>{container},
        num_code_units,
        code_unit_type{});
    otext_iterator<ET, decltype(begin(container))> it(begin(container));
    test_forward_encode<ET>(encoded_characters, container, it);
    }

    // Test otext_iterator with an underlying bidirectional iterator.
    {
    list<code_unit_type> container;
    fill_n(
        front_insert_iterator<decltype(container)>{container},
        num_code_units,
        code_unit_type{});
    otext_iterator<ET, decltype(begin(container))> it(begin(container));
    test_forward_encode<ET>(encoded_characters, container, it);
    }

    // Test otext_iterator with an underlying random access iterator.
    {
    vector<code_unit_type> container(num_code_units);
    otext_iterator<ET, decltype(begin(container))> it(begin(container));
    test_forward_encode<ET>(encoded_characters, container, it);
    }


    // Test itext_iterator with an underlying input iterator.
    {
    forward_list<code_unit_type> container;
    auto insert_it = container.before_begin();
    for (const auto &ecs : encoded_characters) {
        for (const auto &ecu : ecs.code_units) {
            insert_it = container.insert_after(insert_it, ecu);
        }
    }
    auto input_container =
        input_range_view<forward_list<code_unit_type>>{container};
    auto tv = make_text_view<ET>(input_container);
    test_forward_decode(encoded_characters, input_container, tv);
    }

    // Test itext_iterator with an underlying forward iterator.
    {
    forward_list<code_unit_type> container;
    auto insert_it = container.before_begin();
    for (const auto &ecs : encoded_characters) {
        for (const auto &ecu : ecs.code_units) {
            insert_it = container.insert_after(insert_it, ecu);
        }
    }
    auto tv = make_text_view<ET>(container);
    test_forward_decode(encoded_characters, container, tv);
    }

    // Test itext_iterator with an underlying bidirectional iterator.
    {
    list<code_unit_type> container;
    for (const auto &ecs : encoded_characters) {
        for (const auto &ecu : ecs.code_units) {
            container.push_back(ecu);
        }
    }
    auto tv = make_text_view<ET>(container);
    test_forward_decode(encoded_characters, container, tv);
    }

    // Test itext_iterator with an underlying random access iterator.
    {
    vector<code_unit_type> container;
    for (const auto &ecs : encoded_characters) {
        for (const auto &ecu : ecs.code_units) {
            container.push_back(ecu);
        }
    }
    auto tv = make_text_view<ET>(container);
    test_forward_decode(encoded_characters, container, tv);
    }

    // Test itext_iterator with an underlying N4128 Iterable.
    {
    vector<code_unit_type> container;
    for (const auto &ecs : encoded_characters) {
        for (const auto &ecu : ecs.code_units) {
            container.push_back(ecu);
        }
    }
    iterable_view<decltype(container)> iv_container{container};
    auto tv = make_text_view<ET>(iv_container);
    test_forward_decode(encoded_characters, iv_container, tv);
    }
}

// Test bidirectional encoding and decoding of the character sequence and code
// unit sequences present in the 'encoded_characters' vector for the character
// encoding specified by 'ET'.  This text exercises encoding and decoding using
// input text iterators with underlying bidirectional, and random access
// iterators and reverse output text iterators with underlying output, forward,
// bidirectional, and random access iterators.
template<Encoding ET>
requires Bidirectional_codec<typename ET::codec_type>()
void test_bidirectional_encoding(
    const vector<encoded_character<
        typename ET::codec_type::character_type,
        typename ET::codec_type::code_unit_type>> &encoded_characters)
{
    test_forward_encoding<ET>(encoded_characters);

    using codec_type = typename ET::codec_type;
    using code_unit_type = typename codec_type::code_unit_type;

    // Test itext_iterator with an underlying bidirectional iterator.
    {
    list<code_unit_type> container;
    for (const auto &ecs : encoded_characters) {
        for (const auto &ecu : ecs.code_units) {
            container.push_back(ecu);
        }
    }
    auto tv = make_text_view<ET>(container);
    test_reverse_decode(encoded_characters, container, tv);
    }

    // Test itext_iterator with an underlying random access iterator.
    {
    vector<code_unit_type> container;
    for (const auto &ecs : encoded_characters) {
        for (const auto &ecu : ecs.code_units) {
            container.push_back(ecu);
        }
    }
    auto tv = make_text_view<ET>(container);
    test_reverse_decode(encoded_characters, container, tv);
    }

    // Test itext_iterator with an underlying N4128 Iterable.
    {
    vector<code_unit_type> container;
    for (const auto &ecs : encoded_characters) {
        for (const auto &ecu : ecs.code_units) {
            container.push_back(ecu);
        }
    }
    iterable_view<decltype(container)> iv_container{container};
    auto tv = make_text_view<ET>(iv_container);
    test_reverse_decode(encoded_characters, iv_container, tv);
    }
}

// Test random access encoding and decoding of the character sequence and code
// unit sequences present in the 'encoded_characters' vector for the character
// encoding specified by 'ET'.  This text exercises decoding using input text
// iterators with underlying random access iterators.
template<Encoding ET>
requires Random_access_codec<typename ET::codec_type>()
void test_random_access_encoding(
    const vector<encoded_character<
        typename ET::codec_type::character_type,
        typename ET::codec_type::code_unit_type>> &encoded_characters)
{
    test_bidirectional_encoding<ET>(encoded_characters);

    using codec_type = typename ET::codec_type;
    using code_unit_type = typename codec_type::code_unit_type;

    // Test itext_iterator with an underlying random access iterator.
    {
    vector<code_unit_type> container;
    for (const auto &ecs : encoded_characters) {
        for (const auto &ecu : ecs.code_units) {
            container.push_back(ecu);
        }
    }
    auto tv = make_text_view<ET>(container);
    test_random_decode(encoded_characters, container, tv);
    }

    // Test itext_iterator with an underlying N4128 Iterable.
    {
    vector<code_unit_type> container;
    for (const auto &ecs : encoded_characters) {
        for (const auto &ecu : ecs.code_units) {
            container.push_back(ecu);
        }
    }
    iterable_view<decltype(container)> iv_container{container};
    auto tv = make_text_view<ET>(iv_container);
    test_random_decode(encoded_characters, iv_container, tv);
    }
}

template<
    Text_view TVT,
    origin::Input_range RT>
void test_text_view(
    const vector<encoded_character<
        typename encoding_type_of<TVT>::codec_type::character_type,
        typename encoding_type_of<TVT>::codec_type::code_unit_type>> &encoded_characters,
    const RT &code_unit_range,
    TVT tv)
{
    test_forward_decode(encoded_characters, code_unit_range, tv);
}

template<
    Text_view TVT,
    size_t cstr_length,
    size_t ary_length,
    typename String>
void test_construct_text_view(
    const vector<encoded_character<
        typename encoding_type_of<TVT>::codec_type::character_type,
        typename encoding_type_of<TVT>::codec_type::code_unit_type>> &characters_with_terminator,
    const vector<encoded_character<
        typename encoding_type_of<TVT>::codec_type::character_type,
        typename encoding_type_of<TVT>::codec_type::code_unit_type>> &characters_without_terminator,
    const typename encoding_type_of<TVT>::codec_type::code_unit_type (&cstr)[cstr_length],
    const array<typename encoding_type_of<TVT>::codec_type::code_unit_type, ary_length> &ary,
    const String &str,
    const initializer_list<const typename encoding_type_of<TVT>::codec_type::code_unit_type> &il)
{
    using ET = encoding_type_of<TVT>;
    using RT = typename TVT::range_type;
    using ECV = vector<encoded_character<
                    typename encoding_type_of<TVT>::codec_type::character_type,
                    typename encoding_type_of<TVT>::codec_type::code_unit_type>>;

    // Note: copy-initialization is used in these tests to ensure these forms
    // are valid for function arguments.  list-initialization is used to ensure
    // no interference from initializer-list constructors.

    // Test initialization via the default constructor.
    TVT tv1 = {};
    test_text_view<TVT>(ECV{}, RT{}, tv1);

    // Test initialization with an explicit initial state and underlying range.
    TVT tv2 = {ET::initial_state(), RT{&cstr[0], &cstr[cstr_length]}};
    test_text_view<TVT>(characters_with_terminator,
                        RT{&cstr[0], &cstr[cstr_length]},
                        tv2);

    // Test initialization with an implicit initial state and underlying range.
    TVT tv3 = {RT{&cstr[0], &cstr[cstr_length]}};
    test_text_view<TVT>(characters_with_terminator,
                        RT{&cstr[0], &cstr[cstr_length]},
                        tv3);

    // Test initialization with an explicit initial state and an iterator range.
    TVT tv4 = {ET::initial_state(), &cstr[0], &cstr[cstr_length]};
    test_text_view<TVT>(characters_with_terminator,
                        RT{&cstr[0], &cstr[cstr_length]},
                        tv4);

    // Test initialization with an implicit initial state and an iterator range.
    TVT tv5 = {&cstr[0], &cstr[cstr_length]};
    test_text_view<TVT>(characters_with_terminator,
                        RT{&cstr[0], &cstr[cstr_length]},
                        tv5);

    // Test initialization with an explicit initial state and a sized iterator range.
    TVT tv6 = {ET::initial_state(), &cstr[0], cstr_length};
    test_text_view<TVT>(characters_with_terminator,
                        RT{&cstr[0], &cstr[cstr_length]},
                        tv6);

    // Test initialization with an implicit initial state and a sized iterator range.
    TVT tv7 = {&cstr[0], cstr_length};
    test_text_view<TVT>(characters_with_terminator,
                        RT{&cstr[0], &cstr[cstr_length]},
                        tv7);

    // Test initialization with an explicit initial state and a std::string.
    TVT tv8 = {ET::initial_state(), str};
    test_text_view<TVT>(characters_without_terminator,
                        RT{str.c_str(), str.c_str() + str.size()},
                        tv8);

    // Test initialization with an implicit initial state and a std::string.
    TVT tv9 = {str};
    test_text_view<TVT>(characters_without_terminator,
                        RT{str.c_str(), str.c_str() + str.size()},
                        tv9);

    // Test initialization with an explicit initial state and an iterable (C array).
    TVT tv10 = {ET::initial_state(), cstr};
    test_text_view<TVT>(characters_with_terminator,
                        RT{&cstr[0], &cstr[cstr_length]},
                        tv10);

    // Test initialization with an implicit initial state and an iterable (C array).
    TVT tv11 = {cstr};
    test_text_view<TVT>(characters_with_terminator,
                        RT{&cstr[0], &cstr[cstr_length]},
                        tv11);

    // Test initialization with an explicit initial state and an iterable (std::array).
    TVT tv12 = {ET::initial_state(), ary};
    test_text_view<TVT>(characters_without_terminator,
                        ary,
                        tv12);

    // Test initialization with an implicit initial state and an iterable (std::array).
    TVT tv13 = {ary};
    test_text_view<TVT>(characters_without_terminator,
                        ary,
                        tv13);

    // Test initialization with an explicit initial state and an iterable (std::initializer_list).
    TVT tv14 = {ET::initial_state(), il};
    test_text_view<TVT>(characters_without_terminator,
                        il,
                        tv14);

    // Test initialization with an implicit initial state and an iterable (std::initializer_list).
    TVT tv15 = {il};
    test_text_view<TVT>(characters_without_terminator,
                        il,
                        tv15);

    // Test initialization with an explicit initial state and an iterable (make_cstr_range).
    TVT tv16 = {ET::initial_state(), make_cstr_range(cstr)};
    test_text_view<TVT>(characters_without_terminator,
                        make_cstr_range(cstr),
                        tv16);
                             
    // Test initialization with an implicit initial state and an iterable (make_cstr_range).
    TVT tv17 = {make_cstr_range(cstr)};
    test_text_view<TVT>(characters_without_terminator,
                        make_cstr_range(cstr),
                        tv17);

    // Test initialization with a text iterator pair.
    TVT tv18 = {begin(tv17), end(tv17)};
    test_text_view<TVT>(characters_without_terminator,
                        RT{begin(begin(tv17).base_range()), end(end(tv17).base_range())},
                        tv18);
                             
    // Test initialization via the copy constructor.
    TVT tv19 = {tv2}; // Note: used to test move construction below.
    test_text_view<TVT>(characters_with_terminator,
                        RT{&cstr[0], &cstr[cstr_length]},
                        tv19);
                             
    // Test initialization via the move constructor.
    TVT tv20 = {move(tv19)};
    test_text_view<TVT>(characters_with_terminator,
                        RT{&cstr[0], &cstr[cstr_length]},
                        tv20);

    // Test copy assignment.
    TVT tv21; // Note: used to test move assignment below.
    tv21 = {tv2};
    test_text_view<TVT>(characters_with_terminator,
                        RT{&cstr[0], &cstr[cstr_length]},
                        tv21);

    // Test move assignment.
    TVT tv22;
    tv22 = {move(tv21)};
    test_text_view<TVT>(characters_with_terminator,
                        RT{&cstr[0], &cstr[cstr_length]},
                        tv22);
}

template<
    Encoding ET,
    size_t cstr_length,
    size_t ary_length,
    typename String>
void test_make_text_view(
    const vector<encoded_character<
        typename ET::codec_type::character_type,
        typename ET::codec_type::code_unit_type>> &characters_with_terminator,
    const vector<encoded_character<
        typename ET::codec_type::character_type,
        typename ET::codec_type::code_unit_type>> &characters_without_terminator,
    const typename ET::codec_type::code_unit_type (&cstr)[cstr_length],
    const array<typename ET::codec_type::code_unit_type, ary_length> &ary,
    const String &str,
    const initializer_list<const typename ET::codec_type::code_unit_type> &il)
{
    // Test construction with an explicit initial state and an iterator range.
    test_text_view(characters_without_terminator,
                   ary,
                   make_text_view<ET>(ET::initial_state(), begin(ary), end(ary)));

    // Test construction with an implicit initial state and an iterator range.
    test_text_view(characters_without_terminator,
                   ary,
                   make_text_view<ET>(begin(ary), end(ary)));

    // Test construction with an explicit initial state and a sized iterator range.
    test_text_view(characters_without_terminator,
                   ary,
                   make_text_view<ET>(ET::initial_state(), begin(ary), ary.size()));

    // Test construction with an implicit initial state and a sized iterator range.
    test_text_view(characters_without_terminator,
                   ary,
                   make_text_view<ET>(begin(ary), ary.size()));

    // Test construction with an explicit initial state and an iterable (C array).
    test_text_view(characters_with_terminator,
                   cstr,
                   make_text_view<ET>(ET::initial_state(), cstr));

    // Test construction with an implicit initial state and an iterable (C array).
    test_text_view(characters_with_terminator,
                   cstr,
                   make_text_view<ET>(cstr));

    // Test construction with an explicit initial state and an iterable (std::array).
    test_text_view(characters_without_terminator,
                   ary,
                   make_text_view<ET>(ET::initial_state(), ary));

    // Test construction with an implicit initial state and an iterable (std::array).
    test_text_view(characters_without_terminator,
                   ary,
                   make_text_view<ET>(ary));

    // Test construction with an explicit initial state and an iterable (std::string).
    test_text_view(characters_without_terminator,
                   str,
                   make_text_view<ET>(ET::initial_state(), str));

    // Test construction with an implicit initial state and an iterable (std::string).
    test_text_view(characters_without_terminator,
                   str,
                   make_text_view<ET>(str));

    // Test construction with an explicit initial state and an iterable (std::initializer_list).
    test_text_view(characters_without_terminator,
                   il,
                   make_text_view<ET>(ET::initial_state(), il));

    // Test construction with an implicit initial state and an iterable (std::initializer_list).
    test_text_view(characters_without_terminator,
                   il,
                   make_text_view<ET>(il));

    // Test construction with an explicit initial state and an iterable (make_cstr_range).
    test_text_view(characters_without_terminator,
                   make_cstr_range(cstr),
                   make_text_view<ET>(ET::initial_state(), make_cstr_range(cstr)));

    // Test construction with an implicit initial state and an iterable (make_cstr_range).
    test_text_view(characters_without_terminator,
                   make_cstr_range(cstr),
                   make_text_view<ET>(make_cstr_range(cstr)));

    // Test construction via the copy constructor.  Note that an explicit
    // encoding is not specified in the make_text_view call.
    auto tv1 = make_text_view<ET>(ary);
    test_text_view(characters_without_terminator,
                   ary,
                   make_text_view(tv1));

    // Test construction via the move constructor.  Note that an explicit
    // encoding is not specified in the make_text_view call.
    auto tv2 = tv1;
    test_text_view(characters_without_terminator,
                   ary,
                   make_text_view(move(tv2)));

    // Test construction with a text iterator pair.  Note that an explicit
    // encoding is not specified in the make_text_view call.
    test_text_view(characters_without_terminator,
                   ary,
                   make_text_view(begin(tv1), end(tv1)));
}

void test_ntext_view() {
    using TVT = ntext_view;
    using ET = encoding_type_of<TVT>;
    using CT = ET::codec_type::character_type;
    using CUT = ET::codec_type::code_unit_type;
    using ECV = vector<encoded_character<CT, CUT>>;

    ECV characters_with_terminator{
        { { CT{'t'} },  { 't'  } },
        { { CT{'e'} },  { 'e'  } },
        { { CT{'x'} },  { 'x'  } },
        { { CT{'t'} },  { 't'  } },
        { { CT{'\0'} }, { '\0' } } };
    ECV characters_without_terminator{
        { { CT{'t'} },  { 't'  } },
        { { CT{'e'} },  { 'e'  } },
        { { CT{'x'} },  { 'x'  } },
        { { CT{'t'} },  { 't'  } } };

    // Underlying code unit containers.
    static const char cstr[] = "text";
    static const array<char, 4> ary{ 't', 'e', 'x', 't' };
    static const string str{"text"};
    static const auto il = { 't', 'e', 'x', 't' }; // std::initializer_list<char>.

    test_construct_text_view<TVT>(
        characters_with_terminator,
        characters_without_terminator,
        cstr,
        ary,
        str,
        il);

    test_make_text_view<ET>(
        characters_with_terminator,
        characters_without_terminator,
        cstr,
        ary,
        str,
        il);
}

void test_wtext_view() {
    using TVT = wtext_view;
    using ET = encoding_type_of<TVT>;
    using CT = ET::codec_type::character_type;
    using CUT = ET::codec_type::code_unit_type;
    using ECV = vector<encoded_character<CT, CUT>>;

    ECV characters_with_terminator{
        { { CT{L't'} },  { L't'  } },
        { { CT{L'e'} },  { L'e'  } },
        { { CT{L'x'} },  { L'x'  } },
        { { CT{L't'} },  { L't'  } },
        { { CT{L'\0'} }, { L'\0' } } };
    ECV characters_without_terminator{
        { { CT{L't'} },  { L't'  } },
        { { CT{L'e'} },  { L'e'  } },
        { { CT{L'x'} },  { L'x'  } },
        { { CT{L't'} },  { L't'  } } };

    // Underlying code unit containers.
    static const wchar_t cstr[] = L"text";
    static const array<wchar_t, 4> ary{ L't', L'e', L'x', L't' };
    static const wstring str{L"text"};
    static const auto il = { L't', L'e', L'x', L't' }; // std::initializer_list<wchar_t>.

    test_construct_text_view<TVT>(
        characters_with_terminator,
        characters_without_terminator,
        cstr,
        ary,
        str,
        il);

    test_make_text_view<ET>(
        characters_with_terminator,
        characters_without_terminator,
        cstr,
        ary,
        str,
        il);
}

void test_u8text_view() {
    using TVT = u8text_view;
    using ET = encoding_type_of<TVT>;
    using CT = ET::codec_type::character_type;
    using CUT = ET::codec_type::code_unit_type;
    using ECV = vector<encoded_character<CT, CUT>>;

    // FIXME: Once N4267 is implemented, replace these character literals with
    // FIXME: u8 prefixed ones.
    ECV characters_with_terminator{
        { { CT{U't'} },  { 't'  } },
        { { CT{U'e'} },  { 'e'  } },
        { { CT{U'x'} },  { 'x'  } },
        { { CT{U't'} },  { 't'  } },
        { { CT{U'\0'} }, { '\0' } } };
    ECV characters_without_terminator{
        { { CT{U't'} },  { 't'  } },
        { { CT{U'e'} },  { 'e'  } },
        { { CT{U'x'} },  { 'x'  } },
        { { CT{U't'} },  { 't'  } } };

    // Underlying code unit containers.
    // FIXME: If N3398 were to be adopted, replace char with char8_t.
    static const char cstr[] = u8"text";
    // FIXME: If N3398 were to be adopted, replace char with char8_t.
    // FIXME: Once N4267 is implemented, replace these character literals with
    // FIXME: u8 prefixed ones.
    static const array<char, 4> ary{ 't', 'e', 'x', 't' };
    static const string str{u8"text"};
    // FIXME: If N3398 were to be adopted, replace char with char8_t.
    // FIXME: Once N4267 is implemented, replace these character literals with
    // FIXME: u8 prefixed ones.
    static const auto il = { 't', 'e', 'x', 't' }; // std::initializer_list<char>.

    test_construct_text_view<TVT>(
        characters_with_terminator,
        characters_without_terminator,
        cstr,
        ary,
        str,
        il);

    test_make_text_view<ET>(
        characters_with_terminator,
        characters_without_terminator,
        cstr,
        ary,
        str,
        il);
}

void test_u16text_view() {
    using TVT = u16text_view;
    using ET = encoding_type_of<TVT>;
    using CT = ET::codec_type::character_type;
    using CUT = ET::codec_type::code_unit_type;
    using ECV = vector<encoded_character<CT, CUT>>;

    ECV characters_with_terminator{
        { { CT{U't'} },  { u't'  } },
        { { CT{U'e'} },  { u'e'  } },
        { { CT{U'x'} },  { u'x'  } },
        { { CT{U't'} },  { u't'  } },
        { { CT{U'\0'} }, { u'\0' } } };
    ECV characters_without_terminator{
        { { CT{U't'} },  { u't'  } },
        { { CT{U'e'} },  { u'e'  } },
        { { CT{U'x'} },  { u'x'  } },
        { { CT{U't'} },  { u't'  } } };

    // Underlying code unit containers.
    static const char16_t cstr[] = u"text";
    static const array<char16_t, 4> ary{ u't', u'e', u'x', u't' };
    static const u16string str{u"text"};
    static const auto il = { u't', u'e', u'x', u't' }; // std::initializer_list<char16_t>.

    test_construct_text_view<TVT>(
        characters_with_terminator,
        characters_without_terminator,
        cstr,
        ary,
        str,
        il);

    test_make_text_view<ET>(
        characters_with_terminator,
        characters_without_terminator,
        cstr,
        ary,
        str,
        il);
}

void test_u32text_view() {
    using TVT = u32text_view;
    using ET = encoding_type_of<TVT>;
    using CT = ET::codec_type::character_type;
    using CUT = ET::codec_type::code_unit_type;
    using ECV = vector<encoded_character<CT, CUT>>;

    ECV characters_with_terminator{
        { { CT{U't'} },  { U't'  } },
        { { CT{U'e'} },  { U'e'  } },
        { { CT{U'x'} },  { U'x'  } },
        { { CT{U't'} },  { U't'  } },
        { { CT{U'\0'} }, { U'\0' } } };
    ECV characters_without_terminator{
        { { CT{U't'} },  { U't'  } },
        { { CT{U'e'} },  { U'e'  } },
        { { CT{U'x'} },  { U'x'  } },
        { { CT{U't'} },  { U't'  } } };

    // Underlying code unit containers.
    static const char32_t cstr[] = U"text";
    static const array<char32_t, 4> ary{ U't', U'e', U'x', U't' };
    static const u32string str{U"text"};
    static const auto il = { U't', U'e', U'x', U't' }; // std::initializer_list<char32_t>.

    test_construct_text_view<TVT>(
        characters_with_terminator,
        characters_without_terminator,
        cstr,
        ary,
        str,
        il);

    test_make_text_view<ET>(
        characters_with_terminator,
        characters_without_terminator,
        cstr,
        ary,
        str,
        il);
}

void test_utf8_encoding() {
    using ET = utf8_encoding;
    using CT = ET::codec_type::character_type;
    using CUT = ET::codec_type::code_unit_type;
    // FIXME: code_unit_type for UTF-8 is char, but the values below require
    // FIXME: an unsigned (8-bit) char.  An initializer that allows narrowing
    // FIXME: conversions is used to support implementations with a signed
    // FIXME: 8-bit char.
    vector<encoded_character<CT, CUT>> encoded_characters{
        { { CT{U'\U00000041'} }, { CUT(0x41) } },
        { { CT{U'\U00000141'} }, { CUT(0xC5), CUT(0x81) } },
        { { CT{U'\U00001141'} }, { CUT(0xE1), CUT(0x85), CUT(0x81) } },
        { { CT{U'\U00011141'} }, { CUT(0xF0), CUT(0x91), CUT(0x85), CUT(0x81) } },
        { { CT{U'\0'} },         { CUT(0x00) } } };

    test_bidirectional_encoding<ET>(encoded_characters);

    string encoded_string(u8"a\U00011141z");
    auto tv = make_text_view<ET>(encoded_string);
    static_assert(origin::Iterator<decltype(end(tv))>(), "");
    auto tvit = find(begin(tv), end(tv), CT{U'\U00011141'});
    assert(begin(tvit.base_range()) == begin(encoded_string) + 1);
    assert(end(tvit.base_range()) == begin(encoded_string) + 5);
}

void test_utf16_encoding() {
    using ET = utf16_encoding;
    using CT = ET::codec_type::character_type;
    using CUT = ET::codec_type::code_unit_type;
    vector<encoded_character<CT, CUT>> encoded_characters{
        { { CT{U'\U00000041'} }, { 0x0041 } },
        { { CT{U'\U00000141'} }, { 0x0141 } },
        { { CT{U'\U00001141'} }, { 0x1141 } },
        { { CT{U'\U00011141'} }, { 0xD804, 0xDD41 } },
        { { CT{U'\0'} },         { 0x0000 } } };

    test_bidirectional_encoding<ET>(encoded_characters);

    u16string encoded_string(u"a\U00011141z");
    auto tv = make_text_view<ET>(encoded_string);
    static_assert(origin::Iterator<decltype(end(tv))>(), "");
    auto tvit = find(begin(tv), end(tv), CT{U'\U00011141'});
    assert(begin(tvit.base_range()) == begin(encoded_string) + 1);
    assert(end(tvit.base_range()) == begin(encoded_string) + 3);
}

void test_utf16be_encoding() {
    using ET = utf16be_encoding;
    using CT = ET::codec_type::character_type;
    using CUT = ET::codec_type::code_unit_type;
    vector<encoded_character<CT, CUT>> encoded_characters{
        { { CT{U'\U00000041'} }, { 0x00, 0x41 } },
        { { CT{U'\U00000141'} }, { 0x01, 0x41 } },
        { { CT{U'\U00001141'} }, { 0x11, 0x41 } },
        { { CT{U'\U00011141'} }, { 0xD8, 0x04, 0xDD, 0x41 } },
        { { CT{U'\0'} },         { 0x00, 0x00 } } };

    test_bidirectional_encoding<ET>(encoded_characters);

    string encoded_string("\x00\x61\xD8\x04\xDD\x41\x00\x7A", 8);
    auto tv = make_text_view<ET>(encoded_string);
    static_assert(origin::Iterator<decltype(end(tv))>(), "");
    auto tvit = find(begin(tv), end(tv), CT{U'\U00011141'});
    assert(begin(tvit.base_range()) == begin(encoded_string) + 2);
    assert(end(tvit.base_range()) == begin(encoded_string) + 6);
}

void test_utf16le_encoding() {
    using ET = utf16le_encoding;
    using CT = ET::codec_type::character_type;
    using CUT = ET::codec_type::code_unit_type;
    vector<encoded_character<CT, CUT>> encoded_characters{
        { { CT{U'\U00000041'} }, { 0x41, 0x00 } },
        { { CT{U'\U00000141'} }, { 0x41, 0x01 } },
        { { CT{U'\U00001141'} }, { 0x41, 0x11 } },
        { { CT{U'\U00011141'} }, { 0x04, 0xD8, 0x41, 0xDD } },
        { { CT{U'\0'} },         { 0x00, 0x00 } } };

    test_bidirectional_encoding<ET>(encoded_characters);

    string encoded_string("\x61\x00\x04\xD8\x41\xDD\x7A\x00", 8);
    auto tv = make_text_view<ET>(encoded_string);
    static_assert(origin::Iterator<decltype(end(tv))>(), "");
    auto tvit = find(begin(tv), end(tv), CT{U'\U00011141'});
    assert(begin(tvit.base_range()) == begin(encoded_string) + 2);
    assert(end(tvit.base_range()) == begin(encoded_string) + 6);
}

void test_utf32_encoding() {
    using ET = utf32_encoding;
    using CT = ET::codec_type::character_type;
    using CUT = ET::codec_type::code_unit_type;
    vector<encoded_character<CT, CUT>> encoded_characters{
        { { CT{U'\U00000041'} }, { 0x00000041 } },
        { { CT{U'\U00000141'} }, { 0x00000141 } },
        { { CT{U'\U00001141'} }, { 0x00001141 } },
        { { CT{U'\U00011141'} }, { 0x00011141 } },
        { { CT{U'\0'} },         { 0x00000000 } } };

    test_random_access_encoding<ET>(encoded_characters);

    u32string encoded_string(U"a\U00011141z");
    auto tv = make_text_view<ET>(encoded_string);
    static_assert(origin::Iterator<decltype(end(tv))>(), "");
    auto tvit = find(begin(tv), end(tv), CT{U'\U00011141'});
    assert(begin(tvit.base_range()) == begin(encoded_string) + 1);
    assert(end(tvit.base_range()) == begin(encoded_string) + 2);
}

void test_utf32be_encoding() {
    using ET = utf32be_encoding;
    using CT = ET::codec_type::character_type;
    using CUT = ET::codec_type::code_unit_type;
    vector<encoded_character<CT, CUT>> encoded_characters{
        { { CT{U'\U00000041'} }, { 0x00, 0x00, 0x00, 0x41 } },
        { { CT{U'\U00000141'} }, { 0x00, 0x00, 0x01, 0x41 } },
        { { CT{U'\U00001141'} }, { 0x00, 0x00, 0x11, 0x41 } },
        { { CT{U'\U00011141'} }, { 0x00, 0x01, 0x11, 0x41 } },
        { { CT{U'\0'} },         { 0x00, 0x00, 0x00, 0x00 } } };

    test_random_access_encoding<ET>(encoded_characters);

    string encoded_string("\x00\x00\x00\x61\x00\x01\x11\x41\x00\x00\x00\x7A", 12);
    auto tv = make_text_view<ET>(encoded_string);
    static_assert(origin::Iterator<decltype(end(tv))>(), "");
    auto tvit = find(begin(tv), end(tv), CT{U'\U00011141'});
    assert(begin(tvit.base_range()) == begin(encoded_string) + 4);
    assert(end(tvit.base_range()) == begin(encoded_string) + 8);
}

void test_utf32le_encoding() {
    using ET = utf32le_encoding;
    using CT = ET::codec_type::character_type;
    using CUT = ET::codec_type::code_unit_type;
    vector<encoded_character<CT, CUT>> encoded_characters{
        { { CT{U'\U00000041'} }, { 0x41, 0x00, 0x00, 0x00 } },
        { { CT{U'\U00000141'} }, { 0x41, 0x01, 0x00, 0x00 } },
        { { CT{U'\U00001141'} }, { 0x41, 0x11, 0x00, 0x00 } },
        { { CT{U'\U00011141'} }, { 0x41, 0x11, 0x01, 0x00 } },
        { { CT{U'\0'} },         { 0x00, 0x00, 0x00, 0x00 } } };

    test_random_access_encoding<ET>(encoded_characters);

    string encoded_string("\x61\x00\x00\x00\x41\x11\x01\x00\x7A\x00\x00\x00", 12);
    auto tv = make_text_view<ET>(encoded_string);
    static_assert(origin::Iterator<decltype(end(tv))>(), "");
    auto tvit = find(begin(tv), end(tv), CT{U'\U00011141'});
    assert(begin(tvit.base_range()) == begin(encoded_string) + 4);
    assert(end(tvit.base_range()) == begin(encoded_string) + 8);
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

    test_ntext_view();
    test_wtext_view();
    test_u8text_view();
    test_u16text_view();
    test_u32text_view();

    test_utf8_encoding();
    test_utf16_encoding();
    test_utf16be_encoding();
    test_utf16le_encoding();
    test_utf32_encoding();
    test_utf32be_encoding();
    test_utf32le_encoding();

    return 0;
}
