#ifndef TEXT_VIEW_CONCEPTS_HPP // {
#define TEXT_VIEW_CONCEPTS_HPP


#include <origin/core/traits.hpp>
#include <origin/sequence/concepts.hpp>
#include <origin/sequence/range.hpp>
#include <text_view_detail/traits.hpp>


namespace std {
namespace experimental {
namespace text_view {


namespace detail {
template<typename T, T t1, T t2>
concept bool Same_value() {
    return t1 == t2;
}
} // detail namespace


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
 * Codec state concept
 * These requirements are intended to match the char_traits<T>::state_type
 * requirements described in C++11 [char.traits.typedefs] 21.2.2p4.
 */
template<typename T>
concept bool Codec_state() {
    return origin::Default_constructible<T>()
        && origin::Copy_constructible<T>()
        && origin::Copy_assignable<T>();
}


/*
 * Codec state transition concept
 */
template<typename T>
concept bool Codec_state_transition() {
    return origin::Default_constructible<T>()
        && origin::Copy_constructible<T>()
        && origin::Copy_assignable<T>();
}


/*
 * Codec concept
 */
template<typename T>
concept bool Codec() {
    return requires () {
               typename T::state_type;
               typename T::state_transition_type;
               typename T::code_unit_type;
               typename T::character_type;
               { T::min_code_units } noexcept -> int;
               { T::max_code_units } noexcept -> int;
           }
        && Codec_state<typename T::state_type>()
        && Codec_state_transition<typename T::state_transition_type>()
        && Code_unit<typename T::code_unit_type>()
        && Character<typename T::character_type>();
}


/*
 * Encoder concept
 */
template<typename T, typename CUIT>
concept bool Encoder() {
    return Codec<T>()
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
 * Decoder concept
 */
template<typename T, typename CUIT>
concept bool Decoder() {
    return Codec<T>()
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
 * Forward decoder concept
 */
template<typename T, typename CUIT>
concept bool Forward_decoder() {
    return Decoder<T, CUIT>()
        && origin::Forward_iterator<CUIT>();
}


/*
 * Bidirectional decoder concept
 */
template<typename T, typename CUIT>
concept bool Bidirectional_decoder() {
    return Forward_decoder<T, CUIT>()
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
 * Random access decoder concept
 */
template<typename T, typename CUIT>
concept bool Random_access_decoder() {
    return Bidirectional_decoder<T, CUIT>()
        && origin::Random_access_iterator<CUIT>()
        && detail::Same_value<int, T::min_code_units, T::max_code_units>()
        && origin::Empty_type<typename T::state_type>();
}


/*
 * Forward codec concept
 */
template<typename T, typename CUIT>
concept bool Forward_codec() {
    return origin::Forward_iterator<CUIT>()
        && Encoder<T, CUIT>()
        && Decoder<T, CUIT>();
}


/*
 * Bidirectional codec concept
 */
template<typename T, typename CUIT>
concept bool Bidirectional_codec() {
    return Encoder<T, CUIT>()
        && Bidirectional_decoder<T, CUIT>();
}


/*
 * Random access codec concept
 */
template<typename T, typename CUIT>
concept bool Random_access_codec() {
    return Encoder<T, CUIT>()
        && Random_access_decoder<T, CUIT>();
}


/*
 * Encoding concept
 */
template<typename T>
concept bool Encoding() {
    return requires () {
               typename T::codec_type;
           }
        && Codec<typename T::codec_type>()
        && requires () {
               { T::initial_state() } noexcept
                   -> const typename T::codec_type::state_type&;
           };
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
        && Encoding<typename T::encoding_type>()
        && Codec_state<typename T::state_type>()
        && origin::Iterator<T>()
        && Character<origin::Value_type<T>>()
        && requires (T t, const T ct) {
               { t.state() }
                   -> typename encoding_type_of<T>::codec_type::state_type&;
               { ct.state() }
                   -> const typename encoding_type_of<T>::codec_type::state_type&;
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
        && Encoding<typename T::encoding_type>()
        && origin::Input_range<typename T::range_type>()
        && Codec_state<typename T::state_type>()
        && origin::Iterator<typename T::code_unit_iterator>()
        && origin::Input_range<T>()
        && Text_iterator<origin::Iterator_type<T>>()
        && requires (T t, const T ct) {
               { t.base() }
                   -> typename T::range_type&;
               { ct.base() }
                   -> const typename T::range_type&;
               { t.initial_state() }
                   -> typename T::state_type&;
               { ct.initial_state() }
                   -> const typename T::state_type&;
           };
}


} // namespace text_view
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_CONCEPTS_HPP
