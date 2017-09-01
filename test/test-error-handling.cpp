// Copyright (c) 2017, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

// Ensure assert is enabled regardless of build type.
#if defined(NDEBUG)
#undef NDEBUG
#endif

#include <algorithm>
#include <cassert>
#include <cctype>
#include <iterator>
#include <iomanip>
#include <iostream>
#include <limits>
#include <typeinfo>
#include <utility>
#include <vector>
#include <experimental/text_view>
#include <text_view_detail/ios_format_preserver.hpp>


using namespace std;
using namespace std::experimental;


// Tracks the total number of errors that occurred.
int error_count = 0;


enum error_policy {
    strict,
    permissive
};
constexpr int num_error_policies = 2;

template<TextErrorPolicy TEP>
error_policy tep_index;
template<>
constexpr error_policy tep_index<text_strict_error_policy> = strict;
template<>
constexpr error_policy tep_index<text_permissive_error_policy> = permissive;


template<TextEncoding ET>
class character_or_state_transition {
public:
    using character_type = character_type_t<ET>;
    using state_transition_type = typename ET::state_transition_type;

    character_or_state_transition(character_type c) noexcept
        : c(c), have_character{true} {}
    character_or_state_transition(state_transition_type st) noexcept
        : st(st), have_character{false} {}

    bool has_character() const noexcept {
        return have_character;
    }
    bool has_state_transition() const noexcept {
        return ! have_character;
    }

    const character_type& get_character() const noexcept {
        assert(has_character());
        return c;
    }
    const state_transition_type& get_state_transition() const noexcept {
        assert(has_state_transition());
        return st;
    }

private:
    union {
        state_transition_type st;
        character_type c;
    };
    bool have_character;
};

template<TextEncoding ET>
struct expected_code_unit_map {
    using state_transition_type = typename ET::state_transition_type;
    using code_unit_type = code_unit_type_t<ET>;
    using character_type = character_type_t<ET>;

    // The character or state transition sequence to encode.
    character_or_state_transition<ET> c_or_st;

    // The code unit sequence expected from encoding the character or state
    // transition.  The array dimensions correspond to error policies and are
    // indexed by the enumerators of 'error_policy'.
    // FIXME: std::optional or std::expected would be a better choice than
    // FIXME: vector for specifying the absence of a code unit sequence due
    // FIXME: to an encoding error.  The intent is that an empty outer vector
    // FIXME: signify an error condition.
    vector<vector<code_unit_type>> expected_code_units[num_error_policies];
};

template<TextEncoding ET>
using expected_code_unit_map_sequence = vector<expected_code_unit_map<ET>>;

template<TextEncoding ET>
struct expected_character_map {
    using state_transition_type = typename ET::state_transition_type;
    using code_unit_type = code_unit_type_t<ET>;
    using character_type = character_type_t<ET>;

    // The code unit sequence to decode.
    vector<code_unit_type> code_units;

    // The character expected from decoding the code unit sequence.  The
    // array dimensions correspond to error policies and are indexed by the
    // enumerators of 'error_policy'.
    // FIXME: std::optional or std::expected would be a better choice than
    // FIXME: vector for specifying the absence of a character due to a
    // FIXME: decoding error.  The intent is that an empty vector signify
    // FIXME: an error condition.
    vector<character_type> expected_character[num_error_policies];
};

template<TextEncoding ET>
using expected_character_map_sequence = vector<expected_character_map<ET>>;


template<TextEncoding ET>
void display_expected_code_unit_map_sequence(
    const expected_code_unit_map_sequence<ET> &ecums,
    error_policy ep)
{
    using code_unit_type = code_unit_type_t<ET>;
    using character_type = character_type_t<ET>;

    text_detail::ios_format_preserver ifp{cerr};

    cerr << "For encoding: " << typeid(ET).name() << endl;
    cerr << "With error policy: "
         << (ep == strict ? "strict" : "permissive") << endl;
    cerr << "For expected code unit map sequence:" << endl;
    int cu_index = 0;
    for (size_t i = 0; i < ecums.size(); ++i) {
        cerr << "  [" << dec << setw(2) << setfill(' ') << i << "]:" << endl;
        if (ecums[i].c_or_st.has_character()) {
            const auto &c = ecums[i].c_or_st.get_character();
            make_unsigned_t<code_point_type_t<character_type>> cp =
                c.get_code_point();
            cerr << "    character:" << endl;
            cerr << "      0x" << hex << setw(8) << setfill('0')
                 << (uint_least32_t)cp;
            if ((int_least32_t)cp >=
                     (int_least32_t)numeric_limits<char>::min() &&
                (int_least32_t)cp <=
                     (int_least32_t)numeric_limits<char>::max() &&
                isprint(cp))
            {
                cerr << " '" << (char)cp << "'";
            }
            cerr << " ("
                 << character_set_type_t<character_type>::get_name()
                 << ")" << endl;
        } else {
          assert(ecums[i].c_or_st.has_state_transition());
            cerr << "    state transition:" << endl;
        }
        const auto &expected_code_units = ecums[i].expected_code_units[ep];
        cerr << "    code units:" << endl;
        if (! expected_code_units.empty()) {
            assert(expected_code_units.size() == 1);
            for (make_unsigned_t<code_unit_type> cu : expected_code_units[0]) {
                cerr << "      ["
                     << dec << setw(2) << setfill(' ') << cu_index++
                     << "]:";
                cerr << " 0x" << hex << setw(8) << setfill('0')
                     << (uint_least32_t)cu;
                if ((int_least32_t)cu >=
                         (int_least32_t)numeric_limits<char>::min() &&
                    (int_least32_t)cu <=
                         (int_least32_t)numeric_limits<char>::max() &&
                    isprint(cu))
                {
                    cerr << " '" << (char)cu << "'";
                }
                cerr << endl;
            }
        } else {
            cerr << "      <error>" << endl;
        }
    }
}

template<TextEncoding ET>
void display_expected_character_map_sequence(
    const expected_character_map_sequence<ET> &ecms,
    error_policy ep)
{
    using code_unit_type = code_unit_type_t<ET>;
    using character_type = character_type_t<ET>;

    text_detail::ios_format_preserver ifp{cerr};

    cerr << "For encoding: " << typeid(ET).name() << endl;
    cerr << "With error policy: "
         << (ep == strict ? "strict" : "permissive") << endl;
    cerr << "For expected character map sequence:" << endl;
    int cu_index = 0;
    for (size_t i = 0; i < ecms.size(); ++i) {
        cerr << "  [" << dec << setw(2) << setfill(' ') << i << "]:" << endl;
        cerr << "    code units:" << endl;
        for (make_unsigned_t<code_unit_type> cu : ecms[i].code_units) {
            cerr << "      ["
                 << dec << setw(2) << setfill(' ') << cu_index++
                 << "]:"
                 << " 0x" << hex << setw(8) << setfill('0')
                 << (uint_least32_t)cu;
            if ((int_least32_t)cu >=
                     (int_least32_t)numeric_limits<char>::min() &&
                (int_least32_t)cu <=
                     (int_least32_t)numeric_limits<char>::max() &&
                isprint(cu))
            {
                cerr << " '" << (char)cu << "'";
            }
            cerr << endl;
        }
        const auto &expected_character = ecms[i].expected_character[ep];
        cerr << "    character:" << endl;
        if (! expected_character.empty()) {
            assert(expected_character.size() == 1);
            make_unsigned_t<code_point_type_t<character_type>> cp =
                expected_character[0].get_code_point();
            cerr << "      0x" << hex << setw(8) << setfill('0')
                 << (uint_least32_t)cp;
            if ((int_least32_t)cp >=
                     (int_least32_t)numeric_limits<char>::min() &&
                (int_least32_t)cp <=
                     (int_least32_t)numeric_limits<char>::max() &&
                isprint(cp))
            {
                cerr << " '" << (char)cp << "'";
            }
            cerr << " ("
                 << character_set_type_t<character_type>::get_name()
                 << ")" << endl;
        } else {
            cerr << "      <error>" << endl;
        }
    }
}


template<TextEncoding ET>
void report_expected_exception_not_thrown_while_encoding_character(
    const expected_code_unit_map_sequence<ET> &ecums,
    error_policy ep,
    int c_index)
{
    ++error_count;
    cerr << "error: Expected exception not thrown when encoding character"
         << " at index " << c_index << endl;
    display_expected_code_unit_map_sequence<ET>(ecums, ep);
}

template<TextEncoding ET>
void report_expected_exception_not_thrown_while_encoding_state_transition(
    const expected_code_unit_map_sequence<ET> &ecums,
    error_policy ep,
    int st_index)
{
    ++error_count;
    display_expected_code_unit_map_sequence<ET>(ecums, ep);
    cerr << "error: Expected exception not thrown when encoding state transition"
         << " at index " << st_index << endl;
}

template<TextEncoding ET>
void report_expected_exception_not_thrown_while_decoding_character(
    const expected_character_map_sequence<ET> &ecms,
    error_policy ep,
    int c_index)
{
    ++error_count;
    cerr << "error: Expected exception not thrown when decoding character"
         << " at index " << c_index << endl;
    display_expected_character_map_sequence<ET>(ecms, ep);
}

template<TextEncoding ET>
void report_unexpected_exception_thrown_while_encoding_character(
    const expected_code_unit_map_sequence<ET> &ecums,
    error_policy ep,
    int c_index)
{
    ++error_count;
    display_expected_code_unit_map_sequence<ET>(ecums, ep);
    cerr << "error: Unexpected exception thrown when encoding character"
         << " at index " << c_index << endl;
}

template<TextEncoding ET>
void report_unexpected_exception_thrown_while_encoding_state_transition(
    const expected_code_unit_map_sequence<ET> &ecums,
    error_policy ep,
    int st_index)
{
    ++error_count;
    display_expected_code_unit_map_sequence<ET>(ecums, ep);
    cerr << "error: Unexpected exception thrown when encoding state transition"
         << " at index " << st_index << endl;
}

template<TextEncoding ET>
void report_unexpected_exception_thrown_while_decoding_character(
    const expected_character_map_sequence<ET> &ecms,
    error_policy ep,
    int c_index)
{
    ++error_count;
    display_expected_character_map_sequence<ET>(ecms, ep);
    cerr << "error: Unexpected exception thrown when decoding character"
         << " at index " << c_index << endl;
}

template<
    TextEncoding ET,
    CodeUnit CUT = code_unit_type_t<ET>>
void report_unexpected_code_unit(
    const expected_code_unit_map_sequence<ET> &ecums,
    error_policy ep,
    int cu_index,
    CUT expected_cu,
    CUT actual_cu)
{
    text_detail::ios_format_preserver ifp{cerr};

    ++error_count;
    display_expected_code_unit_map_sequence<ET>(ecums, ep);
    make_unsigned_t<CUT> expected_unsigned_cu = expected_cu;
    make_unsigned_t<CUT> actual_unsigned_cu = actual_cu;
    cerr << "error: At code unit index " << cu_index << ":" << endl;
    cerr << "error: expected a code unit with value"
         << " 0x" << hex << setw(8) << setfill('0')
         << (uint_least32_t)expected_unsigned_cu << endl;
    cerr << "error: but found a code unit with value"
         << " 0x" << hex << setw(8) << setfill('0')
         << (uint_least32_t)actual_unsigned_cu << endl;
}

template<
    TextEncoding ET,
    CodeUnit CUT = code_unit_type_t<ET>>
void report_too_few_actual_code_units(
    const expected_code_unit_map_sequence<ET> &ecums,
    error_policy ep,
    int cu_index,
    CUT expected_cu)
{
    text_detail::ios_format_preserver ifp{cerr};

    make_unsigned_t<CUT> expected_unsigned_cu = expected_cu;

    ++error_count;
    display_expected_code_unit_map_sequence<ET>(ecums, ep);
    cerr << "error: At code unit index " << cu_index << "," << endl;
    cerr << "error: expected a code unit with value"
         << " 0x" << hex << setw(8) << setfill('0')
         << (uint_least32_t)expected_unsigned_cu << "," << endl;
    cerr << "error: but no actual code units remained" << endl;
}

template<
    TextEncoding ET,
    CodeUnit CUT = code_unit_type_t<ET>>
void report_too_many_actual_code_units(
    const expected_code_unit_map_sequence<ET> &ecums,
    error_policy ep,
    int cu_index,
    CUT actual_cu)
{
    text_detail::ios_format_preserver ifp{cerr};

    make_unsigned_t<CUT> actual_unsigned_cu = actual_cu;

    ++error_count;
    display_expected_code_unit_map_sequence<ET>(ecums, ep);
    cerr << "error: At code unit index " << cu_index << "," << endl;
    cerr << "error: expected the end of encoded code units," << endl;
    cerr << "error: but found a code unit with value"
         << " 0x" << hex << setw(8) << setfill('0')
         << (uint_least32_t)actual_unsigned_cu << endl;
}

template<
    TextEncoding ET,
    Character CT = character_type_t<ET>,
    CodePoint CPT = code_point_type_t<CT>>
void report_unexpected_character(
    const expected_character_map_sequence<ET> &ecms,
    error_policy ep,
    int c_index,
    CT expected_c,
    CT actual_c)
{
    text_detail::ios_format_preserver ifp{cerr};

    ++error_count;
    display_expected_character_map_sequence<ET>(ecms, ep);
    make_unsigned_t<CPT> expected_unsigned_cp = expected_c.get_code_point();
    make_unsigned_t<CPT> actual_unsigned_cp = actual_c.get_code_point();
    cerr << "error: At character index " << c_index << ":" << endl;
    cerr << "error: expected a character with code point value"
         << " 0x" << hex << setw(8) << setfill('0')
         << (uint_least32_t)expected_unsigned_cp << endl;
    cerr << "error: but found a character with code point value"
         << " 0x" << hex << setw(8) << setfill('0')
         << (uint_least32_t)actual_unsigned_cp << endl;
}

template<
    TextEncoding ET,
    Character CT = character_type_t<ET>,
    CodePoint CPT = code_point_type_t<CT>>
void report_too_few_expected_code_units(
    const expected_character_map_sequence<ET> &ecms,
    error_policy ep,
    int c_index)
{
    text_detail::ios_format_preserver ifp{cerr};

    ++error_count;
    display_expected_character_map_sequence<ET>(ecms, ep);
    cerr << "error: At character index " << c_index << "," << endl;
    cerr << "error: expected to decode a character," << endl;
    cerr << "error: but no expected code units remained" << endl;
}

template<
    TextEncoding ET,
    Character CT = character_type_t<ET>,
    CodePoint CPT = code_point_type_t<CT>>
void report_too_many_expected_code_units(
    const expected_character_map_sequence<ET> &ecms,
    error_policy ep,
    int c_index)
{
    text_detail::ios_format_preserver ifp{cerr};

    ++error_count;
    display_expected_character_map_sequence<ET>(ecms, ep);
    cerr << "error: At character index " << c_index << "," << endl;
    cerr << "error: expected no additional characters," << endl;
    cerr << "error: but expected code units remained" << endl;
}

template<
    TextEncoding ET,
    Character CT = character_type_t<ET>,
    CodePoint CPT = code_point_type_t<CT>>
void report_too_few_actual_characters(
    const expected_character_map_sequence<ET> &ecms,
    error_policy ep,
    int c_index,
    CT expected_c)
{
    text_detail::ios_format_preserver ifp{cerr};

    make_unsigned_t<CPT> expected_unsigned_cp = expected_c.get_code_point();

    ++error_count;
    display_expected_character_map_sequence<ET>(ecms, ep);
    cerr << "error: At character index " << c_index << "," << endl;
    cerr << "error: expected a character with code point value"
         << " 0x" << hex << setw(8) << setfill('0')
         << (uint_least32_t)expected_unsigned_cp << "," << endl;
    cerr << "error: but no actual characters remained" << endl;
}

template<
    TextEncoding ET,
    Character CT = character_type_t<ET>,
    CodePoint CPT = code_point_type_t<CT>>
void report_too_many_actual_characters(
    const expected_character_map_sequence<ET> &ecms,
    error_policy ep,
    int c_index,
    CT actual_c)
{
    text_detail::ios_format_preserver ifp{cerr};

    make_unsigned_t<CPT> actual_unsigned_cp = actual_c.get_code_point();

    ++error_count;
    display_expected_character_map_sequence<ET>(ecms, ep);
    cerr << "error: At character index " << c_index << "," << endl;
    cerr << "error: expected the end of decoded characters," << endl;
    cerr << "error: but found a character with code point value"
         << " 0x" << hex << setw(8) << setfill('0')
         << (uint_least32_t)actual_unsigned_cp << endl;
}


template<
    TextEncoding ET,
    CodeUnit CUT = code_unit_type_t<ET>,
    Character CT = character_type_t<ET>>
void compare_expected_and_actual_code_units(
    const expected_code_unit_map_sequence<ET> &ecums,
    error_policy ep,
    const vector<CUT> &actual_code_units)
{
    int cu_index = 0;
    auto actual_it = actual_code_units.begin();
    for (auto ecum : ecums) {
        if (ecum.expected_code_units[ep].empty()) {
            continue;
        }
        assert(ecum.expected_code_units[ep].size() == 1);
        for (auto expected_cu : ecum.expected_code_units[ep][0]) {
            if (actual_it == actual_code_units.end()) {
                report_too_few_actual_code_units<ET>(
                    ecums, ep, cu_index, expected_cu);
                return;
            }
            auto actual_cu = *actual_it++;
            if (actual_cu != expected_cu) {
                report_unexpected_code_unit<ET>(
                    ecums, ep, cu_index, expected_cu, actual_cu);
                return;
            }
            ++cu_index;
        }
    }

    if (actual_it != actual_code_units.end()) {
        auto actual_cu = *actual_it++;
        report_too_many_actual_code_units<ET>(
            ecums, ep, cu_index, actual_cu);
    }
}

template<
    TextEncoding ET,
    Character CT = character_type_t<ET>,
    CodeUnit CUT = code_unit_type_t<ET>>
void compare_expected_and_actual_characters(
    const expected_character_map_sequence<ET> &ecms,
    error_policy ep,
    const vector<CT> &actual_characters)
{
    int c_index = 0;
    auto actual_it = actual_characters.begin();
    for (auto ecm : ecms) {
        if (ecm.expected_character[ep].empty()) {
            continue;
        }
        assert(ecm.expected_character[ep].size() == 1);
        auto expected_c = ecm.expected_character[ep][0];
        if (actual_it == actual_characters.end()) {
            report_too_few_actual_characters<ET>(
                ecms, ep, c_index, expected_c);
            return;
        }
        auto actual_c = *actual_it++;
        if (actual_c != expected_c) {
            report_unexpected_character<ET>(
                ecms, ep, c_index, expected_c, actual_c);
            return;
        }
        ++c_index;
    }

    if (actual_it != actual_characters.end()) {
        auto actual_c = *actual_it++;
        report_too_many_actual_characters<ET>(
            ecms, ep, c_index, actual_c);
    }
}


template<TextEncoding ET, TextErrorPolicy TEP>
void test_forward_encode(
    const expected_code_unit_map_sequence<ET> &ecums)
{
    using CUT = code_unit_type_t<ET>;
    constexpr error_policy ep = tep_index<TEP>;

    vector<CUT> actual_code_units;
    auto it = ranges::back_inserter(actual_code_units);
    auto tvit = make_otext_iterator<ET, TEP>(it);
    int c_or_st_index = 0;
    for (const auto &ecum : ecums) {
        bool expect_exception = ecum.expected_code_units[ep].empty();
        if (ecum.c_or_st.has_character()) {
            const auto &c = ecum.c_or_st.get_character();
            bool exception_caught = false;
            try {
                *tvit = c;
            } catch(text_encode_error &e) {
                exception_caught = true;
            }
            if (expect_exception && ! exception_caught) {
                report_expected_exception_not_thrown_while_encoding_character<ET>(
                    ecums, ep, c_or_st_index);
                return;
            }
            if (! expect_exception && exception_caught) {
                report_unexpected_exception_thrown_while_encoding_character<ET>(
                    ecums, ep, c_or_st_index);
                return;
            }
            ++tvit;
        } else {
            assert(ecum.c_or_st.has_state_transition());
            const auto &st = ecum.c_or_st.get_state_transition();
            bool exception_caught = false;
            try {
                *tvit = st;
            } catch(text_encode_error &e) {
                exception_caught = true;
            }
            if (expect_exception && ! exception_caught) {
                report_expected_exception_not_thrown_while_encoding_state_transition<ET>(
                    ecums, ep, c_or_st_index);
                return;
            }
            if (! expect_exception && exception_caught) {
                report_unexpected_exception_thrown_while_encoding_state_transition<ET>(
                    ecums, ep, c_or_st_index);
                return;
            }
            ++tvit;
        }
        ++c_or_st_index;
    }

    compare_expected_and_actual_code_units<ET>(ecums, ep, actual_code_units);
}


template<TextEncoding ET, TextErrorPolicy TEP>
void test_forward_decode(
    const expected_character_map_sequence<ET> &ecms)
{
    using CUT = code_unit_type_t<ET>;
    using CT = character_type_t<ET>;
    constexpr error_policy ep = tep_index<TEP>;

    vector<CUT> code_units;
    for (const auto &ecm : ecms) {
        for (auto cu : ecm.code_units) {
            code_units.push_back(cu);
        }
    }

    vector<CT> actual_characters;
    auto tv = make_text_view<ET, TEP>(code_units);
    auto tvit = tv.begin();
    int c_index = 0;
    for (const auto &ecm : ecms) {
        bool expect_exception = ecm.expected_character[ep].empty();
        if (tvit == tv.end()) {
            report_too_few_expected_code_units<ET>(
                ecms, ep, c_index);
            return;
        }
        bool exception_caught = false;
        try {
            actual_characters.push_back(*tvit);
        } catch(text_decode_error &e) {
            exception_caught = true;
        }
        if (expect_exception && ! exception_caught) {
            report_expected_exception_not_thrown_while_decoding_character<ET>(
                ecms, ep, c_index);
            return;
        }
        if (! expect_exception && exception_caught) {
            report_unexpected_exception_thrown_while_decoding_character<ET>(
                ecms, ep, c_index);
            return;
        }
        ++tvit;
        ++c_index;
    }

    if (tvit != tv.end()) {
        report_too_many_expected_code_units<ET>(
            ecms, ep, c_index);
        return;
    }

    compare_expected_and_actual_characters<ET>(ecms, ep, actual_characters);
}

template<TextEncoding ET, TextErrorPolicy TEP>
void test_reverse_decode(
    const expected_character_map_sequence<ET> &ecms)
{
    // FIXME: Exercise reverse decoding.
}

template<TextEncoding ET, TextErrorPolicy TEP>
void test_random_decode(
    const expected_character_map_sequence<ET> &ecms)
{
    // FIXME: Exercise random access decoding.
}


// Tests for encoding.
template<TextEncoding ET>
void test_forward_encoding(
    const expected_code_unit_map_sequence<ET> &ecums)
{
    test_forward_encode<ET, text_strict_error_policy>(ecums);
    test_forward_encode<ET, text_permissive_error_policy>(ecums);
}
template<TextEncoding ET>
void test_bidirectional_encoding(
    const expected_code_unit_map_sequence<ET> &ecums)
{
    test_forward_encoding<ET>(ecums);
}
template<TextEncoding ET>
void test_random_access_encoding(
    const expected_code_unit_map_sequence<ET> &ecums)
{
    test_bidirectional_encoding<ET>(ecums);
}


// Tests for decoding.
template<TextEncoding ET>
void test_forward_encoding(
    const expected_character_map_sequence<ET> &ecms)
{
    test_forward_decode<ET, text_strict_error_policy>(ecms);
    test_forward_decode<ET, text_permissive_error_policy>(ecms);
}
template<TextEncoding ET>
void test_bidirectional_encoding(
    const expected_character_map_sequence<ET> &ecms)
{
    test_forward_encoding<ET>(ecms);

    test_reverse_decode<ET, text_strict_error_policy>(ecms);
    test_reverse_decode<ET, text_permissive_error_policy>(ecms);
}
template<TextEncoding ET>
void test_random_access_encoding(
    const expected_character_map_sequence<ET> &ecms)
{
    test_bidirectional_encoding<ET>(ecms);

    test_random_decode<ET, text_strict_error_policy>(ecms);
    test_random_decode<ET, text_permissive_error_policy>(ecms);
}


void test_basic_execution_character_encoding() {
    // using ET = basic_execution_character_encoding;

    // FIXME: test the basic execution character encoding.
}

void test_basic_execution_wide_character_encoding() {
    // using ET = basic_execution_wide_character_encoding;

    // FIXME: test the basic execution wide character encoding.
}

#if defined(__STDC_ISO_10646__)
void test_iso_10646_wide_character_encoding() {
    // using ET = iso_10646_wide_character_encoding;

    // FIXME: test the ISO-10646 wide character encoding.
}
#endif // __STDC_ISO_10646__

void test_utf8_encoding() {
    using ET = utf8_encoding;
    using CT = character_type_t<ET>;
    using CUT = code_unit_type_t<ET>;

    // FIXME: code_unit_type for UTF-8 is char, but the values below require
    // FIXME: an unsigned (8-bit) char.  An initializer that allows narrowing
    // FIXME: conversions is used to support implementations with a signed
    // FIXME: 8-bit char.

    expected_code_unit_map_sequence<ET> encode_tests[] = {
        // Test attempting to encode lone UTF-16 high-surrogates.
        { {
            CT{0x0000D800}, {
                /*strict*/     {},
                /*permissive*/ { { CUT(0xEF), CUT(0xBF), CUT(0xBD) } }
            }
        } },
        { {
            CT{0x0000DBFF}, {
                /*strict*/     {},
                /*permissive*/ { { CUT(0xEF), CUT(0xBF), CUT(0xBD) } }
            }
        } },
        { {
            CT{U'A'}, {
                /*strict*/     { { CUT(0x41) } },
                /*permissive*/ { { CUT(0x41) } }
            }
          }, {
            CT{0x0000D800}, {
                /*strict*/     {},
                /*permissive*/ { { CUT(0xEF), CUT(0xBF), CUT(0xBD) } }
            }
          }, {
            CT{U'C'}, {
                /*strict*/     { { CUT(0x43) } },
                /*permissive*/ { { CUT(0x43) } }
            }
        } },
        { {
            CT{U'A'}, {
                /*strict*/     { { CUT(0x41) } },
                /*permissive*/ { { CUT(0x41) } }
            }
          }, {
            CT{0x0000DBFF}, {
                /*strict*/     {},
                /*permissive*/ { { CUT(0xEF), CUT(0xBF), CUT(0xBD) } }
            }
          }, {
            CT{U'C'}, {
                /*strict*/     { { CUT(0x43) } },
                /*permissive*/ { { CUT(0x43) } }
            }
        } },
        // Test attempting to encode lone UTF-16 low-surrogates.
        { {
            CT{0x0000DC00}, {
                /*strict*/     {},
                /*permissive*/ { { CUT(0xEF), CUT(0xBF), CUT(0xBD) } }
            }
        } },
        { {
            CT{0x0000DFFF}, {
                /*strict*/     {},
                /*permissive*/ { { CUT(0xEF), CUT(0xBF), CUT(0xBD) } }
            }
        } },
        { {
            CT{U'A'}, {
                /*strict*/     { { CUT(0x41) } },
                /*permissive*/ { { CUT(0x41) } }
            }
          }, {
            CT{0x0000DC00}, {
                /*strict*/     {},
                /*permissive*/ { { CUT(0xEF), CUT(0xBF), CUT(0xBD) } }
            }
          }, {
            CT{U'C'}, {
                /*strict*/     { { CUT(0x43) } },
                /*permissive*/ { { CUT(0x43) } }
            }
        } },
        { {
            CT{U'A'}, {
                /*strict*/     { { CUT(0x41) } },
                /*permissive*/ { { CUT(0x41) } }
            }
          }, {
            CT{0x0000DFFF}, {
                /*strict*/     {},
                /*permissive*/ { { CUT(0xEF), CUT(0xBF), CUT(0xBD) } }
            }
          }, {
            CT{U'C'}, {
                /*strict*/     { { CUT(0x43) } },
                /*permissive*/ { { CUT(0x43) } }
            }
        } },
        // Test attempting to encode a code point value outside the Unicode range.
        { {
            CT{U'A'}, {
                /*strict*/     { { CUT(0x41) } },
                /*permissive*/ { { CUT(0x41) } }
            }
          }, {
            CT{0x00110000}, {
                /*strict*/     {},
                /*permissive*/ { { CUT(0xEF), CUT(0xBF), CUT(0xBD) } }
            }
          }, {
            CT{U'C'}, {
                /*strict*/     { { CUT(0x43) } },
                /*permissive*/ { { CUT(0x43) } }
            }
        } }
    };

    expected_character_map_sequence<ET> decode_tests[] = {
        // Test attempting to decode missing trailing code units.
        { {
            { CUT(0xC2) }, {
                /*strict*/     {},
                /*permissive*/ { CT{0x0000FFFD} }
            }
        } },
        { {
            { CUT(0x41) }, {
                /*strict*/     { CT{U'A'} },
                /*permissive*/ { CT{U'A'} }
            }
          }, {
            { CUT(0xC2) }, {
                /*strict*/     {},
                /*permissive*/ { CT{0x0000FFFD} }
            }
          }, {
            { CUT(0x43) }, {
                /*strict*/     { CT{U'C'} },
                /*permissive*/ { CT{U'C'} }
            }
        } },
        { {
            { CUT(0xC2) }, {
                /*strict*/     {},
                /*permissive*/ { CT{0x0000FFFD} }
            }
          }, {
            { CUT(0xC2), CUT(0x80) }, {
                /*strict*/     { CT{U'\u0080'} },
                /*permissive*/ { CT{U'\u0080'} }
            }
        } },
        { {
            { CUT(0xE0) }, {
                /*strict*/     {},
                /*permissive*/ { CT{0x0000FFFD} }
            }
        } },
        { {
            { CUT(0x41) }, {
                /*strict*/     { CT{U'A'} },
                /*permissive*/ { CT{U'A'} }
            }
          }, {
            { CUT(0xE0) }, {
                /*strict*/     {},
                /*permissive*/ { CT{0x0000FFFD} }
            }
          }, {
            { CUT(0x43) }, {
                /*strict*/     { CT{U'C'} },
                /*permissive*/ { CT{U'C'} }
            }
        } },
        { {
            { CUT(0xE0), CUT(0x80) }, {
                /*strict*/     {},
                /*permissive*/ { CT{0x0000FFFD} }
            }
        } },
        { {
            { CUT(0x41) }, {
                /*strict*/     { CT{U'A'} },
                /*permissive*/ { CT{U'A'} }
            }
          }, {
            { CUT(0xE0), CUT(0x80) }, {
                /*strict*/     {},
                /*permissive*/ { CT{0x0000FFFD} }
            }
          }, {
            { CUT(0x43) }, {
                /*strict*/     { CT{U'C'} },
                /*permissive*/ { CT{U'C'} }
            }
        } },
        { {
            { CUT(0xF0) }, {
                /*strict*/     {},
                /*permissive*/ { CT{0x0000FFFD} }
            }
        } },
        { {
            { CUT(0x41) }, {
                /*strict*/     { CT{U'A'} },
                /*permissive*/ { CT{U'A'} }
            }
          }, {
            { CUT(0xF0) }, {
                /*strict*/     {},
                /*permissive*/ { CT{0x0000FFFD} }
            }
          }, {
            { CUT(0x43) }, {
                /*strict*/     { CT{U'C'} },
                /*permissive*/ { CT{U'C'} }
            }
        } },
        { {
            { CUT(0xF0), CUT(0x80) }, {
                /*strict*/     {},
                /*permissive*/ { CT{0x0000FFFD} }
            }
        } },
        { {
            { CUT(0x41) }, {
                /*strict*/     { CT{U'A'} },
                /*permissive*/ { CT{U'A'} }
            }
          }, {
            { CUT(0xF0), CUT(0x80) }, {
                /*strict*/     {},
                /*permissive*/ { CT{0x0000FFFD} }
            }
          }, {
            { CUT(0x43) }, {
                /*strict*/     { CT{U'C'} },
                /*permissive*/ { CT{U'C'} }
            }
        } },
        { {
            { CUT(0xF0), CUT(0x80), CUT(0x80) }, {
                /*strict*/     {},
                /*permissive*/ { CT{0x0000FFFD} }
            }
        } },
        { {
            { CUT(0x41) }, {
                /*strict*/     { CT{U'A'} },
                /*permissive*/ { CT{U'A'} }
            }
          }, {
            { CUT(0xF0), CUT(0x80), CUT(0x80) }, {
                /*strict*/     {},
                /*permissive*/ { CT{0x0000FFFD} }
            }
          }, {
            { CUT(0x43) }, {
                /*strict*/     { CT{U'C'} },
                /*permissive*/ { CT{U'C'} }
            }
        } },
        // Test attempting to decode missing leading code units.
        { {
            { CUT(0x80) }, {
                /*strict*/     {},
                /*permissive*/ { CT{0x0000FFFD} }
            }
        } },
        { {
            { CUT(0x41) }, {
                /*strict*/     { CT{U'A'} },
                /*permissive*/ { CT{U'A'} }
            }
          }, {
            { CUT(0x80) }, {
                /*strict*/     {},
                /*permissive*/ { CT{0x0000FFFD} }
            }
          }, {
            { CUT(0x43) }, {
                /*strict*/     { CT{U'C'} },
                /*permissive*/ { CT{U'C'} }
            }
        } },
        { {
            { CUT(0x80), CUT(0x80) }, {
                /*strict*/     {},
                /*permissive*/ { CT{0x0000FFFD} }
            }
        } },
        { {
            { CUT(0x41) }, {
                /*strict*/     { CT{U'A'} },
                /*permissive*/ { CT{U'A'} }
            }
          }, {
            { CUT(0x80), CUT(0x80) }, {
                /*strict*/     {},
                /*permissive*/ { CT{0x0000FFFD} }
            }
          }, {
            { CUT(0x43) }, {
                /*strict*/     { CT{U'C'} },
                /*permissive*/ { CT{U'C'} }
            }
        } },
        // Test outside the boundaries of well-formed code unit sequences.
        { {
            { CUT(0x80) }, {
                /*strict*/     {},
                /*permissive*/ { CT{0x0000FFFD} }
            }
          }, {
            { CUT(0x43) }, {
                /*strict*/     { CT{U'C'} },
                /*permissive*/ { CT{U'C'} }
            }
        } },
        { {
            { CUT(0xC0), CUT(0x80) }, {
                /*strict*/     {},
                /*permissive*/ { CT{0x0000FFFD} }
            },
          }, {
            { CUT(0x43) }, {
                /*strict*/     { CT{U'C'} },
                /*permissive*/ { CT{U'C'} }
            }
        } },
        { {
            { CUT(0xC2) }, {
                /*strict*/     {},
                /*permissive*/ { CT{0x0000FFFD} }
            },
          }, {
            { CUT(0x7F) }, {
                /*strict*/     { CT{U'\u007F'} },
                /*permissive*/ { CT{U'\u007F'} }
            }
          }, {
            { CUT(0x43) }, {
                /*strict*/     { CT{U'C'} },
                /*permissive*/ { CT{U'C'} }
            }
        } },
        { {
            { CUT(0xDF), CUT(0xC0) }, {
                /*strict*/     {},
                /*permissive*/ { CT{0x0000FFFD} }
            }
          }, {
            { CUT(0x43) }, {
                /*strict*/     { CT{U'C'} },
                /*permissive*/ { CT{U'C'} }
            }
        } },
        { {
            { CUT(0xE0), CUT(0x9F), CUT(0xBF) }, {
                /*strict*/     {},
                /*permissive*/ { CT{0x0000FFFD} }
            }
          }, {
            { CUT(0x43) }, {
                /*strict*/     { CT{U'C'} },
                /*permissive*/ { CT{U'C'} }
            }
        } },
        { {
            { CUT(0xED), CUT(0xA0), CUT(0x80) }, {
                /*strict*/     {},
                /*permissive*/ { CT{0x0000FFFD} }
            }
          }, {
            { CUT(0x43) }, {
                /*strict*/     { CT{U'C'} },
                /*permissive*/ { CT{U'C'} }
            }
        } },
        { {
            { CUT(0xF0), CUT(0x8F), CUT(0xBF), CUT(0xBF) }, {
                /*strict*/     {},
                /*permissive*/ { CT{0x0000FFFD} }
            }
          }, {
            { CUT(0x43) }, {
                /*strict*/     { CT{U'C'} },
                /*permissive*/ { CT{U'C'} }
            }
        } },
        { {
            { CUT(0xF4), CUT(0x90), CUT(0x80), CUT(0x80) }, {
                /*strict*/     {},
                /*permissive*/ { CT{0x0000FFFD} }
            }
          }, {
            { CUT(0x43) }, {
                /*strict*/     { CT{U'C'} },
                /*permissive*/ { CT{U'C'} }
            }
        } },
        { {
            { CUT(0xF5), CUT(0x80), CUT(0x80), CUT(0x80) }, {
                /*strict*/     {},
                /*permissive*/ { CT{0x0000FFFD} }
            }
          }, {
            { CUT(0x43) }, {
                /*strict*/     { CT{U'C'} },
                /*permissive*/ { CT{U'C'} }
            }
        } }
    };

    for (const auto &ecums : encode_tests) {
        test_bidirectional_encoding<ET>(ecums);
    }

    for (const auto &ecms : decode_tests) {
        test_bidirectional_encoding<ET>(ecms);
    }
}

void test_utf8bom_encoding() {
    // using ET = utf8bom_encoding;

    // FIXME: test the UTF-8-BOM encoding.
}

void test_utf16_encoding() {
    using ET = utf16_encoding;
    using CT = character_type_t<ET>;
    using CUT = code_unit_type_t<ET>;

    expected_code_unit_map_sequence<ET> encode_tests[] = {
        // Test attempting to encode lone UTF-16 high-surrogates.
        { {
            CT{0x0000D800}, {
                /*strict*/     {},
                /*permissive*/ { { CUT{0xFFFD} } }
            }
        } },
        { {
            CT{0x0000DBFF}, {
                /*strict*/     {},
                /*permissive*/ { { CUT{0xFFFD} } }
            }
        } },
        { {
            CT{U'A'}, {
                /*strict*/     { { CUT{0x0041} } },
                /*permissive*/ { { CUT{0x0041} } }
            }
          }, {
            CT{0x0000D800}, {
                /*strict*/     {},
                /*permissive*/ { { CUT{0xFFFD} } }
            }
          }, {
            CT{U'C'}, {
                /*strict*/     { { CUT{0x0043} } },
                /*permissive*/ { { CUT{0x0043} } }
            }
        } },
        { {
            CT{U'A'}, {
                /*strict*/     { { CUT{0x0041} } },
                /*permissive*/ { { CUT{0x0041} } }
            }
          }, {
            CT{0x0000DBFF}, {
                /*strict*/     {},
                /*permissive*/ { { CUT{0xFFFD} } }
            }
          }, {
            CT{U'C'}, {
                /*strict*/     { { CUT{0x0043} } },
                /*permissive*/ { { CUT{0x0043} } }
            }
        } },
        // Test attempting to encode lone UTF-16 low-surrogates.
        { {
            CT{0x0000DC00}, {
                /*strict*/     {},
                /*permissive*/ { { CUT{0xFFFD} } }
            }
        } },
        { {
            CT{0x0000DFFF}, {
                /*strict*/     {},
                /*permissive*/ { { CUT{0xFFFD} } }
            }
        } },
        { {
            CT{U'A'}, {
                /*strict*/     { { CUT{0x0041} } },
                /*permissive*/ { { CUT{0x0041} } }
            }
          }, {
            CT{0x0000DC00}, {
                /*strict*/     {},
                /*permissive*/ { { CUT{0xFFFD} } }
            }
          }, {
            CT{U'C'}, {
                /*strict*/     { { CUT{0x0043} } },
                /*permissive*/ { { CUT{0x0043} } }
            }
        } },
        { {
            CT{U'A'}, {
                /*strict*/     { { CUT{0x0041} } },
                /*permissive*/ { { CUT{0x0041} } }
            }
          }, {
            CT{0x0000DFFF}, {
                /*strict*/     {},
                /*permissive*/ { { CUT{0xFFFD} } }
            }
          }, {
            CT{U'C'}, {
                /*strict*/     { { CUT{0x0043} } },
                /*permissive*/ { { CUT{0x0043} } }
            }
        } },
        // Test attempting to encode UTF-16 surrogates in the wrong order.
        { {
            CT{0x0000DC00}, {
                /*strict*/     {},
                /*permissive*/ { { CUT{0xFFFD} } }
            }
          }, {
            CT{0x0000D800}, {
                /*strict*/     {},
                /*permissive*/ { { CUT{0xFFFD} } }
            }
        } },
        { {
            CT{U'A'}, {
                /*strict*/     { { CUT{0x0041} } },
                /*permissive*/ { { CUT{0x0041} } }
            }
          }, {
            CT{0x0000DC00}, {
                /*strict*/     {},
                /*permissive*/ { { CUT{0xFFFD} } }
            }
          }, {
            CT{0x0000D800}, {
                /*strict*/     {},
                /*permissive*/ { { CUT{0xFFFD} } }
            }
          }, {
            CT{U'C'}, {
                /*strict*/     { { CUT{0x0043} } },
                /*permissive*/ { { CUT{0x0043} } }
            }
        } },
        // Test attempting to encode a code point value outside the Unicode range.
        { {
            CT{U'A'}, {
                /*strict*/     { { CUT{0x0041} } },
                /*permissive*/ { { CUT{0x0041} } }
            }
          }, {
            CT{0x00110000}, {
                /*strict*/     {},
                /*permissive*/ { { CUT{0xFFFD} } }
            }
          }, {
            CT{U'C'}, {
                /*strict*/     { { CUT{0x0043} } },
                /*permissive*/ { { CUT{0x0043} } }
            }
        } }
    };

    expected_character_map_sequence<ET> decode_tests[] = {
        // Test attempting to decode lone UTF-16 high-surrogates.
        { {
            { CUT{0xD800} }, {
                /*strict*/     {},
                /*permissive*/ { CT{0x0000FFFD} }
            }
        } },
        { {
            { CUT{0xDBFF} }, {
                /*strict*/     {},
                /*permissive*/ { CT{0x0000FFFD} }
            }
        } },
        { {
            { CUT{0x0041} }, {
                /*strict*/     { CT{U'A'} },
                /*permissive*/ { CT{U'A'} }
            }
          }, {
            { CUT{0xD800} }, {
                /*strict*/     {},
                /*permissive*/ { CT{0x0000FFFD} }
            }
          }, {
            { CUT{0x0043} }, {
                /*strict*/     { CT{U'C'} },
                /*permissive*/ { CT{U'C'} }
            }
        } },
        { {
            { CUT{0x0041} }, {
                /*strict*/     { CT{U'A'} },
                /*permissive*/ { CT{U'A'} }
            }
          }, {
            { CUT{0xDBFF} }, {
                /*strict*/     {},
                /*permissive*/ { CT{0x0000FFFD} }
            }
          }, {
            { CUT{0x0043} }, {
                /*strict*/     { CT{U'C'} },
                /*permissive*/ { CT{U'C'} }
            }
        } },
        // Test attempting to decode lone UTF-16 low-surrogates.
        { {
            { CUT{0xDC00} }, {
                /*strict*/     {},
                /*permissive*/ { CT{0x0000FFFD} }
            }
        } },
        { {
            { CUT{0xDFFF} }, {
                /*strict*/     {},
                /*permissive*/ { CT{0x0000FFFD} }
            }
        } },
        { {
            { CUT{0x0041} }, {
                /*strict*/     { CT{U'A'} },
                /*permissive*/ { CT{U'A'} }
            }
          }, {
            { CUT{0xDC00} }, {
                /*strict*/     {},
                /*permissive*/ { CT{0x0000FFFD} }
            }
          }, {
            { CUT{0x0043} }, {
                /*strict*/     { CT{U'C'} },
                /*permissive*/ { CT{U'C'} }
            }
        } },
        { {
            { CUT{0x0041} }, {
                /*strict*/     { CT{U'A'} },
                /*permissive*/ { CT{U'A'} }
            }
          }, {
            { CUT{0xDFFF} }, {
                /*strict*/     {},
                /*permissive*/ { CT{0x0000FFFD} }
            }
          }, {
            { CUT{0x0043} }, {
                /*strict*/     { CT{U'C'} },
                /*permissive*/ { CT{U'C'} }
            }
        } },
        // Test attempting to decode UTF-16 surrogates in the wrong order.
        { {
            { CUT{0xDC00} }, {
                /*strict*/     {},
                /*permissive*/ { CT{0x0000FFFD} }
            }
          }, {
            { CUT{0xD800} }, {
                /*strict*/     {},
                /*permissive*/ { CT{0x0000FFFD} }
            }
        } },
        { {
            { CUT{0x0041} }, {
                /*strict*/     { CT{U'A'} },
                /*permissive*/ { CT{U'A'} }
            }
          }, {
            { CUT{0xDC00} }, {
                /*strict*/     {},
                /*permissive*/ { CT{0x0000FFFD} }
            }
          }, {
            { CUT{0xD800} }, {
                /*strict*/     {},
                /*permissive*/ { CT{0x0000FFFD} }
            }
          }, {
            { CUT{0x0043} }, {
                /*strict*/     { CT{U'C'} },
                /*permissive*/ { CT{U'C'} }
            }
        } }
    };

    for (const auto &ecums : encode_tests) {
        test_bidirectional_encoding<ET>(ecums);
    }

    for (const auto &ecms : decode_tests) {
        test_bidirectional_encoding<ET>(ecms);
    }
}

void test_utf16be_encoding() {
    // using ET = utf16be_encoding;

    // FIXME: test the UTF-16BE encoding.
}

void test_utf16le_encoding() {
    // using ET = utf16le_encoding;

    // FIXME: test the UTF-16LE encoding.
}

void test_utf16bom_encoding() {
    // using ET = utf16bom_encoding;

    // FIXME: test the UTF-16-BOM encoding.
}

void test_utf32_encoding() {
    // using ET = utf32_encoding;

    // FIXME: test the UTF-32 encoding.
}

void test_utf32be_encoding() {
    // using ET = utf32be_encoding;

    // FIXME: test the UTF-32BE encoding.
}

void test_utf32le_encoding() {
    // using ET = utf32le_encoding;

    // FIXME: test the UTF-32LE encoding.
}

void test_utf32bom_encoding() {
    // using ET = utf32bom_encoding;

    // FIXME: test the UTF-32-BOM encoding.
}

int main() {
    test_basic_execution_character_encoding();
    test_basic_execution_wide_character_encoding();
#if defined(__STDC_ISO_10646__)
    test_iso_10646_wide_character_encoding();
#endif // __STDC_ISO_10646__
    test_utf8_encoding();
    test_utf8bom_encoding();
    test_utf16_encoding();
    test_utf16be_encoding();
    test_utf16le_encoding();
    test_utf16bom_encoding();
    test_utf32_encoding();
    test_utf32be_encoding();
    test_utf32le_encoding();
    test_utf32bom_encoding();

    if (error_count != 0) {
        cerr << "Total error count: " << error_count << endl;
        return 1;
    }

    return 0;
}
