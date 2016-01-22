// Copyright (c) 2016, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef TEXT_VIEW_CONCEPTS_HPP // {
#define TEXT_VIEW_CONCEPTS_HPP


#include <origin/core/traits.hpp>
#include <origin/algorithm/concepts.hpp>
#include <origin/range/range.hpp>
#include <text_view_detail/traits.hpp>


namespace std {
namespace experimental {
inline namespace text {


namespace text_detail {
template<typename T, T t1, T t2>
concept bool Same_value() {
    return t1 == t2;
}
} // text_detail namespace


/*
 * Code unit concept
 */
template<typename T>
concept bool Code_unit() {
    return origin::Integral_type<T>()
        && (origin::Unsigned_type<T>()
            || origin::Same<T, origin::Remove_cv<char>>()
            || origin::Same<T, origin::Remove_cv<wchar_t>>());
}


/*
 * Code point concept
 */
template<typename T>
concept bool Code_point() {
    return origin::Integral_type<T>()
        && (origin::Unsigned_type<T>()
            || origin::Same<T, origin::Remove_cv<char>>()
            || origin::Same<T, origin::Remove_cv<wchar_t>>());
}


/*
 * Character set concept
 */
template<typename T>
concept bool Character_set() {
    return requires () {
               typename T::code_point_type;
               { T::get_name() } noexcept -> const char *;
           }
        && Code_point<typename T::code_point_type>();
}


/*
 * Character concept
 */
template<typename T>
concept bool Character() {
    return Character_set<character_set_type_of<T>>()
        && origin::Regular<T>()
        && origin::Copy_assignable<T>()
        && requires (T t, code_point_type_of<character_set_type_of<T>> cp) {
               t.set_code_point(cp);
               { t.get_code_point() } -> code_point_type_of<character_set_type_of<T>>;
               { t.get_character_set_id() } -> character_set_id;
           };
}


/*
 * Code unit iterator concept
 */
template<typename T>
concept bool Code_unit_iterator() {
    return origin::Iterator<T>()
        && Code_unit<origin::Value_type<T>>();
}


/*
 * Text encoding state concept
 * These requirements are intended to match the char_traits<T>::state_type
 * requirements described in C++11 [char.traits.typedefs] 21.2.2p4.
 */
template<typename T>
concept bool Text_encoding_state() {
    return origin::Default_constructible<T>()
        && origin::Copy_constructible<T>()
        && origin::Copy_assignable<T>();
}


/*
 * Text encoding state transition concept
 */
template<typename T>
concept bool Text_encoding_state_transition() {
    return origin::Default_constructible<T>()
        && origin::Copy_constructible<T>()
        && origin::Copy_assignable<T>();
}


/*
 * Text encoding concept
 */
template<typename T>
concept bool Text_encoding() {
    return requires () {
               typename T::state_type;
               typename T::state_transition_type;
               typename T::code_unit_type;
               typename T::character_type;
               { T::min_code_units } noexcept -> int;
               { T::max_code_units } noexcept -> int;
           }
        && Text_encoding_state<typename T::state_type>()
        && Text_encoding_state_transition<typename T::state_transition_type>()
        && Code_unit<typename T::code_unit_type>()
        && Character<typename T::character_type>()
        && requires () {
               { T::initial_state() }
                   -> const typename T::state_type&;
           };
}


/*
 * Text encoder concept
 */
template<typename T, typename CUIT>
concept bool Text_encoder() {
    return Text_encoding<T>()
        && origin::Output_iterator<CUIT, typename T::code_unit_type>()
        && requires (
               typename T::state_type &state,
               CUIT &out,
               typename T::state_transition_type stt,
               int &encoded_code_units)
           {
               T::encode_state_transition(state, out, stt, encoded_code_units);
           }
        && requires (
               typename T::state_type &state,
               CUIT &out,
               typename T::character_type c,
               int &encoded_code_units)
           {
               T::encode(state, out, c, encoded_code_units);
           };
}


/*
 * Text decoder concept
 */
template<typename T, typename CUIT>
concept bool Text_decoder() {
    return Text_encoding<T>()
        && origin::Input_iterator<CUIT>()
        && origin::Convertible<origin::Value_type<CUIT>,
                               typename T::code_unit_type>()
        && requires (
               typename T::state_type &state,
               CUIT &in_next,
               CUIT in_end,
               typename T::character_type &c,
               int &decoded_code_units)
           {
               { T::decode(state, in_next, in_end, c, decoded_code_units) } -> bool;
           };
}


/*
 * Text forward decoder concept
 */
template<typename T, typename CUIT>
concept bool Text_forward_decoder() {
    return Text_decoder<T, CUIT>()
        && origin::Forward_iterator<CUIT>();
}


/*
 * Text bidirectional decoder concept
 */
template<typename T, typename CUIT>
concept bool Text_bidirectional_decoder() {
    return Text_forward_decoder<T, CUIT>()
        && origin::Bidirectional_iterator<CUIT>()
        && requires (
               typename T::state_type &state,
               CUIT &in_next,
               CUIT in_end,
               typename T::character_type &c,
               int &decoded_code_units)
           {
               { T::rdecode(state, in_next, in_end, c, decoded_code_units) } -> bool;
           };
}


/*
 * Text random access decoder concept
 */
template<typename T, typename CUIT>
concept bool Text_random_access_decoder() {
    return Text_bidirectional_decoder<T, CUIT>()
        && origin::Random_access_iterator<CUIT>()
        && text_detail::Same_value<int, T::min_code_units, T::max_code_units>()
        && origin::Empty_type<typename T::state_type>();
}


/*
 * Text iterator concept
 */
template<typename T>
concept bool Text_iterator() {
    return requires () {
               typename T::encoding_type;
               typename T::state_type;
           }
        && Text_encoding<typename T::encoding_type>()
        && Text_encoding_state<typename T::state_type>()
        && origin::Iterator<T>()
        && Character<origin::Value_type<T>>()
        && requires (T t, const T ct) {
               { t.state() } noexcept
                   -> typename T::encoding_type::state_type&;
               { ct.state() } noexcept
                   -> const typename T::encoding_type::state_type&;
           };
}


/*
 * Text sentinel concept
 */
template<typename T, typename I>
concept bool Text_sentinel() {
    return origin::Sentinel<T, I>()
        && Text_iterator<I>();
}


/*
 * Text view concept
 */
template<typename T>
concept bool Text_view() {
    return requires () {
               typename T::encoding_type;
               typename T::range_type;
               typename T::state_type;
               typename T::code_unit_iterator;
           }
        && Text_encoding<typename T::encoding_type>()
        && origin::Input_range<typename T::range_type>()
        && Text_encoding_state<typename T::state_type>()
        && origin::Iterator<typename T::code_unit_iterator>()
        && origin::Input_range<T>()
        && Text_iterator<origin::Iterator_type<T>>()
        && requires (T t, const T ct) {
               { t.base() } noexcept
                   -> typename T::range_type&;
               { ct.base() } noexcept
                   -> const typename T::range_type&;
               { t.initial_state() } noexcept
                   -> typename T::state_type&;
               { ct.initial_state() } noexcept
                   -> const typename T::state_type&;
           };
}


} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_CONCEPTS_HPP
