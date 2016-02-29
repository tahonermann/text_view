// Copyright (c) 2016, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef TEXT_VIEW_TEXT_VIEW_HPP // {
#define TEXT_VIEW_TEXT_VIEW_HPP


#include <string>
#include <experimental/ranges/concepts>
#include <experimental/ranges/iterator>
#include <experimental/ranges/utility>
#include <text_view_detail/adl_customization.hpp>
#include <text_view_detail/advance_to.hpp>
#include <text_view_detail/basic_view.hpp>
#include <text_view_detail/concepts.hpp>
#include <text_view_detail/text_iterator.hpp>


namespace std {
namespace experimental {
inline namespace text {


// FIXME: In the language of N4382, types that model the Iterable concept
// FIXME: (24.10.2.2) may own their elements, but types that model Range
// FIXME: (24.10.2.4) explicitly do not.  The Range concept subsumes Iterable.
// FIXME: Types that model Range may be copied without incurring element copy
// FIXME: costs.  Each such copy will contain references to the same underlying
// FIXME: sequence of elements.  Iterators obtained from an object that models
// FIXME: Range may be invalidated if the object from which they were obtained
// FIXME: is destroyed (the iterators may hold references to their originating
// FIXME: object).
// FIXME:
// FIXME: For convenience, make_text_view() will assume that provided range
// FIXME: objects model the N4382 Iterable concept and may therefore own their
// FIXME: elements and will therefore construct a type that models the N4382
// FIXME: Range concept for which to instantiate basic_text_view.  This allows
// FIXME: make_text_view() to be safely passed references to standard containers
// FIXME: (array, string, vector, etc...) that model N4382's Iterable as well as
// FIXME: types that model N4382's Range (such types will be "unwrapped" and
// FIXME: their iterator types wrapped by another type that models Range).  Note
// FIXME: that this means the range type for which basic_text_view is
// FIXME: instantiated may not match the range type that was passed to
// FIXME: make_text_view().
// FIXME: 
// FIXME: Each text view object holds an encoding state subobject.  For stateful
// FIXME: encodings, copying text view objects could carry non-negligible cost.
// FIXME: 
// FIXME: N4382 specifies a range_base class that is used as a base class to
// FIXME: explicitly specify that a class models the N4382 Range concept.
// FIXME: basic_text_view models the N4382 Range concept, and so could (should?)
// FIXME: derive from range_base to make this explicit.
template<TextEncoding ET, ranges::View RT>
class basic_text_view
    : private ET::state_type
{
public:
    using encoding_type = ET;
    using range_type = RT;
    using state_type = typename ET::state_type;
    using code_unit_iterator = ranges::iterator_t<std::add_const_t<range_type>>;
    using code_unit_sentinel = ranges::sentinel_t<std::add_const_t<range_type>>;
    using iterator = itext_iterator<ET, RT>;
    using sentinel = itext_sentinel<ET, RT>;

    // The default constructor produces a text view with a singular range.  An
    // object produced with this constructor may only be assigned to or
    // destroyed.  Note that the default constructor will be defined as deleted
    // if either state_type or range_type is not default constructible.
    basic_text_view() = default;

    // Overload to initialize a text view from an N4382 Range and an explicitly
    // specified initial encoding state.  This overload requires that range_type
    // be copy constructible.
    basic_text_view(
        state_type state,
        range_type r)
    requires ranges::CopyConstructible<range_type>()
    :
        // CWG DR1467.  List-initialization doesn't consider copy constructors
        // for aggregates.  The state_type base class must be initialized with
        // an expression-list.
        state_type(state),
        r{r}
    {}

    // Overload to initialize a text view from an N4382 Range and an implicit
    // initial encoding state.  This overload requires that range_type be copy
    // constructible.
    basic_text_view(
        range_type r)
    requires ranges::CopyConstructible<range_type>()
    : basic_text_view{encoding_type::initial_state(), r} {}

    // Overload to initialize a text view from an N4382 InputIterator, Sentinel,
    // and an explicitly specified initial encoding state.  This overload
    // requires that range_type be constructible from a code_unit_iterator pair.
    basic_text_view(
        state_type state,
        code_unit_iterator first,
        code_unit_sentinel last)
    requires ranges::Constructible<
                 range_type, code_unit_iterator, code_unit_sentinel>()
    :
        // CWG DR1467.  List-initialization doesn't consider copy constructors
        // for aggregates.  The state_type base class must be initialized with
        // an expression-list.
        state_type(state),
        r{first, last}
    {}

    // Overload to initialize a text view from an N4382 InputIterator, Sentinel,
    // and an implicit initial encoding state.  This overload requires that
    // range_type be constructible from a code_unit_iterator pair.
    basic_text_view(
        code_unit_iterator first,
        code_unit_sentinel last)
    requires ranges::Constructible<
                 range_type, code_unit_iterator, code_unit_sentinel>()
    : basic_text_view{encoding_type::initial_state(), first, last} {}

    // Overload to initialize a text view from an N4382 InputIterator, a count,
    // and an explicitly specified initial encoding state.  This overload
    // requires that range_type be constructible from a code_unit_iterator pair.
    basic_text_view(
        state_type state,
        code_unit_iterator first,
        ranges::difference_type_t<code_unit_iterator> n)
    requires ranges::Constructible<
                 range_type, code_unit_iterator, code_unit_iterator>()
    : basic_text_view{state, first, std::next(first, n)} {}

    // Overload to initialize a text view from an N4382 InputIterator, a count,
    // and an implicit initial encoding state.  This overload requires that
    // range_type be constructible from a code_unit_iterator pair.
    basic_text_view(
        code_unit_iterator first,
        ranges::difference_type_t<code_unit_iterator> n)
    requires ranges::Constructible<
                 range_type, code_unit_iterator, code_unit_iterator>()
    : basic_text_view{first, std::next(first, n)} {}

    // Overload to initialize a text view with an explicitly specified initial
    // encoding state and a basic_string specialization.  This overload requires
    // that range_type be constructible from a code_unit_iterator pair, that
    // code_unit_iterator be constructible from a const code unit pointer, and
    // that the difference type (likely ptrdiff_t) for code_unit_iterator be
    // constructible from the basic_string size_type.  This overload enables
    // construction with basic_string specializations when range_type requires
    // pointers to contiguous memory (as is the case for text_view, wtext_view,
    // etc...).  When this overload is not enabled, construction with a
    // basic_string specialization will be handled by the constructor below
    // and the basic_string specialization will be treated as any other type
    // that models N4382 Iterable (in which case, code_unit_iterator must be
    // constructible from the basic_string const_iterator type).  Since
    // iterators into the provided basic_string object will be held, the
    // basic_string reference must reference an object with a lifetime that
    // exceeds the basic_text_view object being constructed.
    template<typename charT, typename traits, typename Allocator>
    basic_text_view(
        state_type state,
        const basic_string<charT, traits, Allocator> &str)
    requires ranges::Constructible<code_unit_iterator, const charT *>()
          && ranges::ConvertibleTo< // Allow narrowing conversions.
                 ranges::difference_type_t<code_unit_iterator>,
                 typename basic_string<charT, traits, Allocator>::size_type>()
          && ranges::Constructible<
                 range_type, code_unit_iterator, code_unit_sentinel>()
    : basic_text_view{state,
                      str.c_str(),
                      ranges::difference_type_t<code_unit_iterator>(str.size())} {}

    // Overload to initialize a text view with an implicitly specified initial
    // encoding state and a basic_string specialization.  See the above comments
    // for the similar overload that accepts an explicitly specified initial
    // state.
    template<typename charT, typename traits, typename Allocator>
    basic_text_view(
        const basic_string<charT, traits, Allocator> &str)
    requires ranges::Constructible<code_unit_iterator, const charT *>()
          && ranges::ConvertibleTo< // Allow narrowing conversions.
                 ranges::difference_type_t<code_unit_iterator>,
                 typename basic_string<charT, traits, Allocator>::size_type>()
          && ranges::Constructible<
                 range_type, code_unit_iterator, code_unit_sentinel>()
    : basic_text_view{str.c_str(),
                      ranges::difference_type_t<code_unit_iterator>(str.size())} {}

    // Overload to initialize a text view with an explicitly specified initial
    // encoding state and an N4382 InputIterator and Sentinel extracted from a
    // supplied N4382 Iterable.  The underlying range type must be constructible
    // from the iterator type of the Iterable type.  This overload may be used
    // to initialize a basic_text_view (that holds a range modeling the N4382
    // Range concept) with a type that models the N4382 Iterable concept.  Since
    // iterators into the provided Iterable object will be held, the specified
    // range must be a reference to an object with a lifetime that exceeds the
    // basic_text_view object being constructed.
    template<ranges::InputRange Iterable>
    basic_text_view(
        state_type state,
        const Iterable &iterable)
    requires ranges::Constructible<
                 code_unit_iterator, ranges::iterator_t<const Iterable>>()
          && ranges::Constructible<
                 range_type, code_unit_iterator, code_unit_sentinel>()
    : basic_text_view(state,
                      text_detail::adl_begin(iterable),
                      text_detail::adl_end(iterable))
    {}

    // Overload to initialize a text view with an implicitly specified initial
    // encoding state and an N4382 InputIterator and Sentinel extracted from a
    // supplied N4382 Iterable.  See the above comments for the similar overload
    // that accepts an explicitly specified initial state.
    template<ranges::InputRange Iterable>
    basic_text_view(
        const Iterable &iterable)
    requires ranges::Constructible<
                 code_unit_iterator, ranges::iterator_t<const Iterable>>()
          && ranges::Constructible<
                 range_type, code_unit_iterator, code_unit_sentinel>()
    : basic_text_view(text_detail::adl_begin(iterable), text_detail::adl_end(iterable)) {}

    // Overload to initialize a text view from a text iterator pair.  The
    // initial encoding state is inferred from the first iterator.
    basic_text_view(
        iterator first,
        sentinel last)
    requires ranges::Constructible<
                 code_unit_iterator,
                 decltype(std::declval<iterator>().base())>()
          && ranges::Constructible<
                 range_type, code_unit_iterator, code_unit_sentinel>()
    : basic_text_view{first.state(), first.base(), last.base()} {}

    const range_type& base() const noexcept {
        return r;
    }
    range_type& base() noexcept {
        return r;
    }

    const state_type& initial_state() const noexcept {
        return *this;
    }
    state_type& initial_state() noexcept {
        return *this;
    }

    iterator begin() const {
        return iterator{initial_state(), &r, text_detail::adl_begin(r)};
    }
    iterator end() const
    requires std::is_empty<state_type>::value
          && ranges::Iterator<code_unit_sentinel>()
    {
        // Use the (empty) initial state to construct the end iterator.
        return iterator{ET::initial_state(), &r, text_detail::adl_end(r)};
    }
    sentinel end() const
    requires !std::is_empty<state_type>::value
          || !ranges::Iterator<code_unit_sentinel>()
    {
        return sentinel{text_detail::adl_end(r)};
    }

private:
    range_type r;
};


using text_view = basic_text_view<
          execution_character_encoding,
          text_detail::basic_view<const char*>>;
using wtext_view = basic_text_view<
          execution_wide_character_encoding,
          text_detail::basic_view<const wchar_t*>>;
using u8text_view = basic_text_view<
          char8_character_encoding,
          text_detail::basic_view<const char*>>;
using u16text_view = basic_text_view<
          char16_character_encoding,
          text_detail::basic_view<const char16_t*>>;
using u32text_view = basic_text_view<
          char32_character_encoding,
          text_detail::basic_view<const char32_t*>>;


/*
 * make_text_view
 */
// Overload to construct a text view from an N4382 InputIterator, Sentinel,
// and an explicitly specified initial encoding state.
template<TextEncoding ET, ranges::InputIterator IT, ranges::Sentinel<IT> ST>
auto make_text_view(
    typename ET::state_type state,
    IT first,
    ST last)
{
    using range_type = text_detail::basic_view<IT, ST>;
    return basic_text_view<ET, range_type>{state, first, last};
}

// Overload to construct a text view from an N4382 InputIterator, Sentinel,
// and an implicit initial encoding state.
template<TextEncoding ET, ranges::InputIterator IT, ranges::Sentinel<IT> ST>
auto make_text_view(
    IT first,
    ST last)
{
    return make_text_view<ET>(
               ET::initial_state(),
               first,
               last);
}

// Overload to construct a text view from an N4382 ForwardIterator, a count, and
// an explicitly specified initial encoding state.
template<TextEncoding ET, ranges::ForwardIterator IT>
auto make_text_view(
    typename ET::state_type state,
    IT first,
    ranges::difference_type_t<IT> n)
{
    return make_text_view<ET>(
               state,
               first,
               std::next(first, n));
}

// Overload to construct a text view from an N4382 ForwardIterator, a count, and
// an implicit initial encoding state.
template<TextEncoding ET, ranges::ForwardIterator IT>
auto make_text_view(
    IT first,
    ranges::difference_type_t<IT> n)
{
    return make_text_view<ET>(
               first,
               std::next(first, n));
}

// Overload to construct a text view from an N4382 Iterable const reference
// and an explicitly specified initial encoding state.
template<TextEncoding ET, ranges::InputRange Iterable>
auto make_text_view(
    typename ET::state_type state,
    const Iterable &iterable)
{
    return make_text_view<ET>(
               state,
               text_detail::adl_begin(iterable),
               text_detail::adl_end(iterable));
}

// Overload to construct a text view from an N4382 Iterable const reference
// and an implicit initial encoding state.
template<TextEncoding ET, ranges::InputRange Iterable>
auto make_text_view(
    const Iterable &iterable)
{
    return make_text_view<ET>(
               text_detail::adl_begin(iterable),
               text_detail::adl_end(iterable));
}

// Overload to construct a text view from a text iterator/sentinel pair.  The
// initial encoding state is inferred from the first iterator.
template<TextIterator TIT, TextSentinel<TIT> TST>
auto make_text_view(
    TIT first,
    TST last)
{
    using ET = encoding_type_t<TIT>;
    return make_text_view<ET>(
               first.state(),
               first.base(),
               last.base());
}

// Overload to construct a text view from an existing text view.
template<TextView TVT>
auto make_text_view(
    TVT tv)
{
    return tv;
}


} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_TEXT_VIEW_HPP
