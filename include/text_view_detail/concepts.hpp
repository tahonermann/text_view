// Copyright (c) 2017, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef TEXT_VIEW_CONCEPTS_HPP // {
#define TEXT_VIEW_CONCEPTS_HPP


#include <experimental/ranges/concepts>
#include <experimental/ranges/type_traits>
#include <text_view_detail/error_status.hpp>
#include <text_view_detail/traits.hpp>
#include <text_view_detail/character_set_id.hpp>


namespace std {
namespace experimental {
inline namespace text {


namespace text_detail {
template<typename T, T t1, T t2>
concept bool SameValue() {
    return t1 == t2;
}

// FIXME: gcc bug 67565, https://gcc.gnu.org/bugzilla/show_bug.cgi?id=67565
// FIXME: Compile time performance is negatively impacted by concept definitions
// FIXME: specified with disjunction constraints.  Performance is significantly
// FIXME: improved by rewriting such constraints in terms of constexpr function
// FIXME: or variable templates.  Rewriting the CodeUnit and CodePoint concepts
// FIXME: in terms of is_code_unit_v and is_code_point_v dropped build times
// FIXME: from 5m50s to 2m00s.  Once the performance issues in the compiler are
// FIXME: addressed, is_code_unit_v and is_code_point_v can be removed and
// FIXME: the original CodeUnit and CodePoint concept definitions restored.
template<typename T>
constexpr bool is_code_unit_v =
    std::is_integral<T>::value
    && (std::is_unsigned<T>::value
        || std::is_same<std::remove_cv_t<T>, char>::value
        || std::is_same<std::remove_cv_t<T>, wchar_t>::value);

template<typename T>
constexpr bool is_code_point_v =
    std::is_integral<T>::value
    && (std::is_unsigned<T>::value
        || std::is_same<std::remove_cv_t<T>, char>::value
        || std::is_same<std::remove_cv_t<T>, wchar_t>::value);
} // text_detail namespace


/*
 * Code unit concept
 */
template<typename T>
concept bool CodeUnit() {
    return text_detail::is_code_unit_v<T>;
}


/*
 * Code point concept
 */
template<typename T>
concept bool CodePoint() {
    return text_detail::is_code_point_v<T>;
}


/*
 * Character set concept
 */
template<typename T>
concept bool CharacterSet() {
    return requires () {
               { T::get_name() } noexcept -> const char *;
           }
        && CodePoint<code_point_type_t<T>>();
}


/*
 * Character concept
 */
template<typename T>
concept bool Character() {
    return CharacterSet<character_set_type_t<T>>()
        && ranges::Regular<T>()
        && requires (T t,
                     const T ct,
                     code_point_type_t<character_set_type_t<T>> cp)
           {
               t.set_code_point(cp);
               { ct.get_code_point() } noexcept
                   -> code_point_type_t<character_set_type_t<T>>;
               { ct.get_character_set_id() }
                   -> character_set_id;
           };
}


/*
 * Code unit iterator concept
 */
template<typename T>
concept bool CodeUnitIterator() {
    return ranges::Iterator<T>()
        && CodeUnit<ranges::value_type_t<T>>();
}


/*
 * Code unit output iterator concept
 */
template<typename T, typename V>
concept bool CodeUnitOutputIterator() {
    return ranges::OutputIterator<T, V>()
        && CodeUnit<V>();
}


/*
 * Text encoding state concept
 * These requirements are intended to match the char_traits<T>::state_type
 * requirements described in C++11 [char.traits.typedefs] 21.2.2p4.
 */
template<typename T>
concept bool TextEncodingState() {
    return ranges::Semiregular<T>();
}


/*
 * Text encoding state transition concept
 */
template<typename T>
concept bool TextEncodingStateTransition() {
    return ranges::Semiregular<T>();
}


/*
 * Text encoding concept
 */
template<typename T>
concept bool TextEncoding() {
    return requires () {
               { T::min_code_units } noexcept -> int;
               { T::max_code_units } noexcept -> int;
           }
        && TextEncodingState<typename T::state_type>()
        && TextEncodingStateTransition<typename T::state_transition_type>()
        && CodeUnit<code_unit_type_t<T>>()
        && Character<character_type_t<T>>()
        && requires () {
               { T::initial_state() } noexcept
                   -> const typename T::state_type&;
           };
}


/*
 * Text encoder concept
 */
template<typename T, typename CUIT>
concept bool TextEncoder() {
    return TextEncoding<T>()
        && CodeUnitOutputIterator<CUIT, code_unit_type_t<T>>()
        && requires (
               typename T::state_type &state,
               CUIT &out,
               typename T::state_transition_type stt,
               int &encoded_code_units)
           {
               { T::encode_state_transition(
                     state, out, stt, encoded_code_units) }
                 -> encode_status;
           }
        && requires (
               typename T::state_type &state,
               CUIT &out,
               character_type_t<T> c,
               int &encoded_code_units)
           {
               { T::encode(
                     state, out, c, encoded_code_units) }
                 -> encode_status;
           };
}


/*
 * Text decoder concept
 */
template<typename T, typename CUIT>
concept bool TextDecoder() {
    return TextEncoding<T>()
        && ranges::InputIterator<CUIT>()
        && ranges::ConvertibleTo<ranges::value_type_t<CUIT>,
                                 code_unit_type_t<T>>()
        && requires (
               typename T::state_type &state,
               CUIT &in_next,
               CUIT in_end,
               character_type_t<T> &c,
               int &decoded_code_units)
           {
               { T::decode(
                     state, in_next, in_end, c, decoded_code_units) }
                 -> decode_status;
           };
}


/*
 * Text forward decoder concept
 */
template<typename T, typename CUIT>
concept bool TextForwardDecoder() {
    return TextDecoder<T, CUIT>()
        && ranges::ForwardIterator<CUIT>();
}


/*
 * Text bidirectional decoder concept
 */
template<typename T, typename CUIT>
concept bool TextBidirectionalDecoder() {
    return TextForwardDecoder<T, CUIT>()
        && ranges::BidirectionalIterator<CUIT>()
        && requires (
               typename T::state_type &state,
               CUIT &in_next,
               CUIT in_end,
               character_type_t<T> &c,
               int &decoded_code_units)
           {
               { T::rdecode(
                     state, in_next, in_end, c, decoded_code_units) }
                 -> decode_status;
           };
}


/*
 * Text random access decoder concept
 */
template<typename T, typename CUIT>
concept bool TextRandomAccessDecoder() {
    return TextBidirectionalDecoder<T, CUIT>()
        && ranges::RandomAccessIterator<CUIT>()
        && text_detail::SameValue<int, T::min_code_units, T::max_code_units>()
        && std::is_empty<typename T::state_type>::value;
}


/*
 * Text iterator concept
 */
template<typename T>
concept bool TextIterator() {
    return ranges::Iterator<T>()
        && TextEncoding<encoding_type_t<T>>()
        && TextEncodingState<typename T::state_type>()
        && requires (const T ct) {
               { ct.state() } noexcept
                   -> const typename encoding_type_t<T>::state_type&;
           };
}


/*
 * Text sentinel concept
 */
template<typename T, typename I>
concept bool TextSentinel() {
    return ranges::Sentinel<T, I>()
        && TextIterator<I>();
}


/*
 * Text output iterator concept
 */
template<typename T>
concept bool TextOutputIterator() {
    return TextIterator<T>()
        && ranges::OutputIterator<T, character_type_t<encoding_type_t<T>>>();
}


/*
 * Text input iterator concept
 */
template<typename T>
concept bool TextInputIterator() {
    return TextIterator<T>()
        && ranges::InputIterator<T>()
        && Character<ranges::value_type_t<T>>();
}


/*
 * Text forward iterator concept
 */
template<typename T>
concept bool TextForwardIterator() {
    return TextInputIterator<T>()
        && ranges::ForwardIterator<T>();
}


/*
 * Text bidirectional iterator concept
 */
template<typename T>
concept bool TextBidirectionalIterator() {
    return TextForwardIterator<T>()
        && ranges::BidirectionalIterator<T>();
}


/*
 * Text random access iterator concept
 */
template<typename T>
concept bool TextRandomAccessIterator() {
    return TextBidirectionalIterator<T>()
        && ranges::RandomAccessIterator<T>();
}


/*
 * Text view concept
 */
template<typename T>
concept bool TextView() {
    return ranges::View<T>()
        && TextIterator<ranges::iterator_t<T>>()
        && TextEncoding<encoding_type_t<T>>()
        && ranges::View<typename T::view_type>()
        && TextEncodingState<typename T::state_type>()
        && CodeUnitIterator<typename T::code_unit_iterator>()
        && requires (T t, const T ct) {
               { t.base() } noexcept
                   -> typename T::view_type&;
               { ct.base() } noexcept
                   -> const typename T::view_type&;
               { ct.initial_state() } noexcept
                   -> const typename T::state_type&;
           };
}


/*
 * Text input view concept
 */
template<typename T>
concept bool TextInputView() {
    return TextView<T>()
        && TextInputIterator<ranges::iterator_t<T>>();
}


/*
 * Text forward view concept
 */
template<typename T>
concept bool TextForwardView() {
    return TextInputView<T>()
        && TextForwardIterator<ranges::iterator_t<T>>();
}


/*
 * Text bidirectional view concept
 */
template<typename T>
concept bool TextBidirectionalView() {
    return TextForwardView<T>()
        && TextBidirectionalIterator<ranges::iterator_t<T>>();
}


/*
 * Text random access view concept
 */
template<typename T>
concept bool TextRandomAccessView() {
    return TextBidirectionalView<T>()
        && TextRandomAccessIterator<ranges::iterator_t<T>>();
}


} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_CONCEPTS_HPP
