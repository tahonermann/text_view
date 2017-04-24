// Copyright (c) 2017, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef TEXT_VIEW_ARCHETYPES_HPP // {
#define TEXT_VIEW_ARCHETYPES_HPP


#include <experimental/ranges/concepts>
#include <text_view_detail/concepts.hpp>


namespace std {
namespace experimental {
inline namespace text {


/*
 * Code unit archetype
 */
template<CodeUnit CUT>
using code_unit_archetype_template = CUT;
using code_unit_archetype = code_unit_archetype_template<char>;


/*
 * Code point archetype
 */
template<CodePoint CPT>
using code_point_archetype_template = CPT;
using code_point_archetype = code_point_archetype_template<char>;


/*
 * Character set archetype
 */
template<CodePoint CPT>
struct character_set_archetype_template
{
    using code_point_type = CPT;
    static const char* get_name() noexcept;
};
using character_set_archetype =
          character_set_archetype_template<code_point_archetype>;


/*
 * Character archetype
 */
template<CharacterSet CST>
class character_archetype_template
{
public:
    using character_set_type = CST;
    using code_point_type = code_point_type_t<CST>;

    character_archetype_template();
    character_archetype_template(code_point_type cp);

    friend bool operator==(
        const character_archetype_template &l,
        const character_archetype_template &r) noexcept
    {
        return true;
    }
    friend bool operator!=(
        const character_archetype_template &l,
        const character_archetype_template &r) noexcept
    {
        return !(l == r);
    }

    void set_code_point(code_point_type cp) noexcept;
    code_point_type get_code_point() const noexcept;

    static character_set_id get_character_set_id();
};
using character_archetype =
          character_archetype_template<character_set_archetype>;


/*
 * Code unit iterator archetype
 */
template<CodeUnit CUT>
using code_unit_iterator_archetype_template = CUT*;
using code_unit_iterator_archetype =
          code_unit_iterator_archetype_template<code_unit_archetype>;


/*
 * Code unit output iterator archetype
 */
template<CodeUnit CUT>
using code_unit_output_iterator_archetype_template = CUT*;
using code_unit_output_iterator_archetype =
          code_unit_output_iterator_archetype_template<code_unit_archetype>;


/*
 * Text encoding state archetype
 */
struct text_encoding_state_archetype {};


/*
 * Text encoding state transition archetype
 */
struct text_encoding_state_transition_archetype {};


/*
 * Text encoding archetype
 */
template<
    TextEncodingState CST,
    TextEncodingStateTransition CSTT,
    CodeUnit CUT,
    Character C,
    int MinCodeUnits = 1,
    int MaxCodeUnits = 1>
struct text_encoding_archetype_template
{
    using state_type = CST;
    using state_transition_type = CSTT;
    using code_unit_type = CUT;
    using character_type = C;

    static constexpr int min_code_units = MinCodeUnits;
    static constexpr int max_code_units = MaxCodeUnits;

    static const state_type& initial_state() noexcept;

    template<CodeUnitOutputIterator<code_unit_type> CUIT>
    static encode_status encode_state_transition(
        state_type &state,
        CUIT &out,
        const state_transition_type &stt,
        int &encoded_code_units);

    template<CodeUnitOutputIterator<code_unit_type> CUIT>
    static encode_status encode(
        state_type &state,
        CUIT &out,
        character_type c,
        int &encoded_code_units);

    template<CodeUnitIterator CUIT, typename CUST>
    requires ranges::InputIterator<CUIT>()
          && ranges::ConvertibleTo<ranges::value_type_t<CUIT>, code_unit_type>()
          && ranges::Sentinel<CUST, CUIT>()
    static decode_status decode(
        state_type &state,
        CUIT &in_next,
        CUST in_end,
        character_type &c,
        int &decoded_code_units);

    template<CodeUnitIterator CUIT, typename CUST>
    requires ranges::InputIterator<CUIT>()
          && ranges::ConvertibleTo<ranges::value_type_t<CUIT>, code_unit_type>()
          && ranges::Sentinel<CUST, CUIT>()
    static decode_status rdecode(
        state_type &state,
        CUIT &in_next,
        CUST in_end,
        character_type &c,
        int &decoded_code_units);
};
using text_encoding_archetype = text_encoding_archetype_template<
                                    text_encoding_state_archetype,
                                    text_encoding_state_transition_archetype,
                                    code_unit_archetype,
                                    character_archetype>;


/*
 * Text iterator archetype
 */
template<TextEncoding ET, CodeUnitIterator CUIT>
class text_iterator_archetype_template {
public:
    using encoding_type = ET;
    using state_type = typename ET::state_type;
    using iterator = CUIT;
    using iterator_category = ranges::iterator_category_t<iterator>;
    using value_type = character_type_t<ET>;
    using reference = character_type_t<ET>&;
    using pointer = character_type_t<ET>*;
    using difference_type = ranges::difference_type_t<iterator>;

    text_iterator_archetype_template();
    text_iterator_archetype_template(iterator, iterator);
    text_iterator_archetype_template(state_type, iterator, iterator);
    const state_type& state() const noexcept;
    state_type& state() noexcept;
    iterator base() const;
    iterator begin() const;
    iterator end() const;
    reference operator*() const noexcept;
    pointer operator->() const noexcept;
    friend bool operator==(
        const text_iterator_archetype_template &l,
        const text_iterator_archetype_template &r) noexcept
    {
        return false;
    }
    friend bool operator!=(
        const text_iterator_archetype_template &l,
        const text_iterator_archetype_template &r) noexcept
    {
        return !(l == r);
    }
    friend bool operator<(
        const text_iterator_archetype_template &l,
        const text_iterator_archetype_template &r) noexcept
    {
        return true;
    }
    friend bool operator>(
        const text_iterator_archetype_template &l,
        const text_iterator_archetype_template &r) noexcept
    {
        return r < l;
    }
    friend bool operator<=(
        const text_iterator_archetype_template &l,
        const text_iterator_archetype_template &r) noexcept
    {
        return !(r < l);
    }
    friend bool operator>=(
        const text_iterator_archetype_template &l,
        const text_iterator_archetype_template &r) noexcept
    {
        return !(l < r);
    }
    text_iterator_archetype_template& operator++();
    text_iterator_archetype_template operator++(int);
    text_iterator_archetype_template& operator--();
    text_iterator_archetype_template operator--(int);
    text_iterator_archetype_template& operator+=(difference_type n);
    friend text_iterator_archetype_template operator+(
        text_iterator_archetype_template l,
        difference_type n)
    {
        return l += n;
    }
    friend text_iterator_archetype_template operator+(
        difference_type n,
        text_iterator_archetype_template r)
    {
        return r += n;
    }
    text_iterator_archetype_template& operator-=(difference_type n);
    friend text_iterator_archetype_template operator-(
        text_iterator_archetype_template l,
        difference_type n)
    {
        return l -= n;
    }
    friend difference_type operator-(
        const text_iterator_archetype_template &l,
        const text_iterator_archetype_template &r)
    {
        return 0;
    }
    value_type operator[](difference_type n) const;
};
using text_iterator_archetype = text_iterator_archetype_template<
                                    text_encoding_archetype,
                                    code_unit_iterator_archetype>;


/*
 * Text output iterator archetype
 */
template<TextEncoding ET, CodeUnitOutputIterator<code_unit_type_t<ET>> CUIT>
class text_output_iterator_archetype_template {
public:
    using encoding_type = ET;
    using state_type = typename ET::state_type;
    using iterator = CUIT;
    using iterator_category = ranges::iterator_category_t<iterator>;
    using value_type = character_type_t<ET>;
    using reference = character_type_t<ET>&;
    using pointer = character_type_t<ET>*;
    using difference_type = ranges::difference_type_t<iterator>;

    text_output_iterator_archetype_template();
    text_output_iterator_archetype_template(iterator, iterator);
    text_output_iterator_archetype_template(state_type, iterator, iterator);
    const state_type& state() const noexcept;
    state_type& state() noexcept;
    iterator base() const;
    reference operator*() const noexcept;
    text_output_iterator_archetype_template& operator++();
    text_output_iterator_archetype_template operator++(int);
};
using text_output_iterator_archetype = text_output_iterator_archetype_template<
                                           text_encoding_archetype,
                                           code_unit_output_iterator_archetype>;


/*
 * Text view archetype
 */
template<TextEncoding ET, ranges::View VT>
class text_view_archetype_template {
public:
    using view_type = VT;
    using encoding_type = ET;
    using state_type = typename ET::state_type;
    using code_unit_iterator = ranges::iterator_t<VT>;
    using iterator = text_iterator_archetype_template<ET, code_unit_iterator>;

    text_view_archetype_template();
    text_view_archetype_template(const state_type &initial_state, VT r);
    const VT& base() const noexcept;
    VT& base() noexcept;
    const state_type& initial_state() const noexcept;
    state_type& initial_state() noexcept;
    iterator begin() const;
    iterator end() const;
};
using text_view_archetype = text_view_archetype_template<
                                text_encoding_archetype,
                                text_detail::basic_view<code_unit_iterator_archetype>>;


} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_ARCHETYPES_HPP
