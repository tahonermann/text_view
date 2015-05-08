#ifndef TEXT_VIEW_TEXT_VIEW_HPP // {
#define TEXT_VIEW_TEXT_VIEW_HPP


#include <text_view_detail/adl_customization.hpp>
#include <text_view_detail/advance_to.hpp>
#include <text_view_detail/bounded_iterable.hpp>
#include <text_view_detail/concepts.hpp>
#include <text_view_detail/text_iterator.hpp>
#include <iterator>
#include <string>
#include <utility>
#include <origin/core/traits.hpp>
#include <origin/sequence/concepts.hpp>
#include <origin/sequence/range.hpp>


namespace std {
namespace experimental {
namespace text_view {


// FIXME: In the language of N4128, types that model the Iterable concept may
// FIXME: own their elements, but types that model Range explicitly do not.
// FIXME: The Range concept subsumes Iterable.  Types that model Range may be
// FIXME: copied without incurring element copy costs.  Each such copy will
// FIXME: contain references to the same underlying sequence of elements.
// FIXME: Iterators obtained from an object that models Range may be
// FIXME: invalidated if the object from which they were obtained is destroyed
// FIXME: (The iterators may hold references to their originating object).
// FIXME:
// FIXME: Origin does not currently declare a concept that exactly matches the
// FIXME: N4128 Iterable concept.  However, its Range concept closely matches.
// FIXME: Origin does not declare a concept that matches N4128's Range concept.
// FIXME: 
// FIXME: basic_text_view is expected to be instantiated with a type that models
// FIXME: the N4128 Range concept as described above.  In particular, this means
// FIXME: that the underlying range type should be one that does not own its
// FIXME: elements.  This is not currently enforced since Origin lacks the
// FIXME: concepts required to enforce it.  It is assumed that the range type
// FIXME: for which basic_text_view is instantiated models the N4128 Range
// FIXME: concept.
// FIXME: 
// FIXME: For convenience, make_text_view() will assume that provided range
// FIXME: objects model the N4128 Iterable concept and may therefore own their
// FIXME: elements and will therefore construct a type that models the N4128
// FIXME: Range concept for which to instantiate basic_text_view.  This allows
// FIXME: make_text_view() to be safely passed references to standard containers
// FIXME: (array, string, vector, etc...) that model N4128 Iterable as well as
// FIXME: types that model N4128 Range (such types will be "unwrapped" and their
// FIXME: iterator types wrapped by another type that models Range).  Note that
// FIXME: this means the range type for which basic_text_view is instantiated
// FIXME: may not match the range type that was passed to make_text_view().
// FIXME: 
// FIXME: Each text view object holds a codec state subobject.  For stateful
// FIXME: encodings, copying text view objects could carry non-negligible cost.
// FIXME: 
// FIXME: N4128 specifies a range_base class that is used as a base class to
// FIXME: explicitly specify that a class models the N4128 Range concept.
// FIXME: basic_text_view models the N4128 Range concept, and so could (should?)
// FIXME: derive from range_base to make this explicit.  Origin does define a
// FIXME: range_base template class that differs from what N4128 specifies.
template<Encoding ET, origin::Input_range RT>
struct basic_text_view
    : private ET::codec_type::state_type
{
    using encoding_type = ET;
    using range_type = RT;
    using state_type = typename ET::codec_type::state_type;
    using code_unit_iterator = origin::Iterator_type<const range_type>;
    using code_unit_sentinel = origin::Sentinel_type<const range_type>;
    using iterator = itext_iterator<ET, RT>;
    using sentinel = itext_sentinel<ET, RT>;

    // Overload to initialize a text view with an empty range.  Note that this
    // constructor will be defined as deleted if either state_type or range_type
    // is not default constructible.
    basic_text_view() = default;

    // Overload to initialize a text view from an N4128 Range and an explicitly
    // specified initial codec state.  This overload requires that range_type
    // be copy constructible.
    basic_text_view(
        state_type state,
        range_type r)
    requires origin::Copy_constructible<range_type>()
    :
        // CWG DR1467.  List-initialization doesn't consider copy constructors
        // for aggregates.  The state_type base class must be initialized with
        // an expression-list.
        state_type(state),
        r{r}
    {}


    // Overload to initialize a text view from an N4128 Range and an implicit
    // initial codec state.  This overload requires that range_type be copy
    // constructible.
    basic_text_view(
        range_type r)
    requires origin::Copy_constructible<range_type>()
    : basic_text_view{encoding_type::initial_state(), r} {}

    // Overload to initialize a text view from an N4128 IteratorRange and an
    // explicitly specified initial codec state.  This overload requires that
    // range_type be constructible from a code_unit_iterator pair.
    basic_text_view(
        state_type state,
        code_unit_iterator first,
        code_unit_sentinel last)
    requires origin::Constructible<
                 range_type, code_unit_iterator, code_unit_sentinel>()
    :
        // CWG DR1467.  List-initialization doesn't consider copy constructors
        // for aggregates.  The state_type base class must be initialized with
        // an expression-list.
        state_type(state),
        r{first, last}
    {}

    // Overload to initialize a text view from an N4128 IteratorRange and an
    // implicit initial codec state.  This overload requires that range_type be
    // constructible from a code_unit_iterator pair.
    basic_text_view(
        code_unit_iterator first,
        code_unit_sentinel last)
    requires origin::Constructible<
                 range_type, code_unit_iterator, code_unit_sentinel>()
    : basic_text_view{encoding_type::initial_state(), first, last} {}

    // Overload to initialize a text view from an N4128 SizedIteratorRange and
    // an explicitly specified initial codec state.  This overload requires that
    // range_type be constructible from a code_unit_iterator pair.
    basic_text_view(
        state_type state,
        code_unit_iterator first,
        origin::Make_unsigned<origin::Difference_type<code_unit_iterator>> n)
    requires origin::Constructible<
                 range_type, code_unit_iterator, code_unit_sentinel>()
    : basic_text_view{state, first, std::next(first, n)} {}

    // Overload to initialize a text view from an N4128 SizedIteratorRange and
    // an implicit initial codec state.  This overload requires that range_type
    // be constructible from a code_unit_iterator pair.
    basic_text_view(
        code_unit_iterator first,
        origin::Make_unsigned<origin::Difference_type<code_unit_iterator>> n)
    requires origin::Constructible<
                 range_type, code_unit_iterator, code_unit_sentinel>()
    : basic_text_view{first, std::next(first, n)} {}

    // Overload to initialize a text view with an explicitly specified initial
    // codec state and a basic_string specialization.  This overload requires
    // that range_type be constructible from a code_unit_iterator pair, that
    // code_unit_iterator be constructible from a const code unit pointer, and
    // that the difference type (likely ptrdiff_t) for code_unit_iterator be
    // constructible from the basic_string size_type.  This overload enables
    // construction with basic_string specializations when range_type requires
    // pointers to contiguous memory (as is the case for text_view, wtext_view,
    // etc...).  When this overload is not enabled, construction with a
    // basic_string specialization will be handled by the constructor below
    // and the basic_string specialization will be treated as any other type
    // that models N4128 Iterable (in which case, code_unit_iterator must be
    // constructible from the basic_string const_iterator type).  Since
    // iterators into the provided basic_string object will be held, the
    // basic_string reference must reference an object with a lifetime that
    // exceeds the text_view object being constructed.
    template<typename charT, typename traits, typename Allocator>
    basic_text_view(
        state_type state,
        const basic_string<charT, traits, Allocator> &str)
    requires origin::Constructible<code_unit_iterator, const charT *>()
          && origin::Constructible<
                 origin::Make_unsigned<origin::Difference_type<code_unit_iterator>>,
                 typename basic_string<charT, traits, Allocator>::size_type>()
          && origin::Constructible<
                 range_type, code_unit_iterator, code_unit_sentinel>()
    : basic_text_view{state, str.c_str(), str.size()} {}

    // Overload to initialize a text view with an implicitly specified initial
    // codec state and a basic_string specialization.  See the above comments
    // for the similar overload that accepts an explicitly specified initial
    // state.
    template<typename charT, typename traits, typename Allocator>
    basic_text_view(
        const basic_string<charT, traits, Allocator> &str)
    requires origin::Constructible<code_unit_iterator, const charT *>()
          && origin::Constructible<
                 origin::Make_unsigned<origin::Difference_type<code_unit_iterator>>,
                 typename basic_string<charT, traits, Allocator>::size_type>()
          && origin::Constructible<
                 range_type, code_unit_iterator, code_unit_sentinel>()
    : basic_text_view{str.c_str(), str.size()} {}

    // Overload to initialize a text view with an explicitly specified initial
    // codec state and an N4128 IteratorRange extracted from a supplied N4128
    // Iterable.  The underlying range type must be constructible from the
    // iterator type of the Iterable type.  This overload may be used to
    // initialize a basic_text_view (that holds a range modeling the N4128 Range
    // concept) with a type that models the N4128 Iterable concept.  Since
    // iterators into the provided Iterable object will be held, the specified
    // range must be a reference to an object with a lifetime that exceeds the
    // basic_text_view object being constructed.
    template<origin::Input_range Iterable>
    basic_text_view(
        state_type state,
        const Iterable &iterable)
    requires origin::Constructible<
                 code_unit_iterator, origin::Iterator_type<const Iterable>>()
          && origin::Constructible<
                 range_type, code_unit_iterator, code_unit_sentinel>()
    : basic_text_view(state,
                      detail::adl_begin(iterable),
                      detail::adl_end(iterable))
    {}

    // Overload to initialize a text view with an implicitly specified initial
    // codec state and an N4128 IteratorRange extracted from a supplied N4128
    // Iterable.  See the above comments for the similar overload that accepts
    // an explicitly specified initial state.
    template<origin::Input_range Iterable>
    basic_text_view(
        const Iterable &iterable)
    requires origin::Constructible<
                 code_unit_iterator, origin::Iterator_type<const Iterable>>()
          && origin::Constructible<
                 range_type, code_unit_iterator, code_unit_sentinel>()
    : basic_text_view(detail::adl_begin(iterable), detail::adl_end(iterable)) {}

    // Overload to initialize a text view from a text iterator pair.  The
    // initial codec state is inferred from the first iterator.
    basic_text_view(
        iterator first,
        sentinel last)
    requires origin::Constructible<
                 code_unit_iterator,
                 decltype(std::declval<iterator>().base())>()
          && origin::Constructible<
                 range_type, code_unit_iterator, code_unit_sentinel>()
    : basic_text_view{first.state(), first.base(), last.base()} {}

    const range_type& base() const {
        return r;
    }
    range_type& base() {
        return r;
    }

    const state_type& initial_state() const {
        return *this;
    }
    state_type& initial_state() {
        return *this;
    }

    iterator begin() const {
        return iterator{initial_state(), &r, detail::adl_begin(r)};
    }
    iterator end() const
    requires origin::Empty_type<state_type>()
          && origin::Iterator<code_unit_sentinel>()
    {
        // Use the (empty) initial state to construct the end iterator.
        return iterator{ET::initial_state(), &r, detail::adl_end(r)};
    }
    sentinel end() const
    requires !origin::Empty_type<state_type>()
          || !origin::Iterator<code_unit_sentinel>()
    {
        return sentinel{detail::adl_end(r)};
    }

private:
    range_type r;
};


using text_view = basic_text_view<
          execution_character_encoding,
          origin::bounded_range<const char*>>;
using wtext_view = basic_text_view<
          execution_wide_character_encoding,
          origin::bounded_range<const wchar_t*>>;
using u8text_view = basic_text_view<
          char8_character_encoding,
          origin::bounded_range<const char*>>;
using u16text_view = basic_text_view<
          char16_character_encoding,
          origin::bounded_range<const char16_t*>>;
using u32text_view = basic_text_view<
          char32_character_encoding,
          origin::bounded_range<const char32_t*>>;


/*
 * make_text_view
 */
// Overload to construct a text view from an N4128 IteratorRange and an
// explicitly specified initial codec state.
template<Encoding ET, origin::Iterator IT, origin::Sentinel<IT> ST>
auto make_text_view(
    typename ET::codec_type::state_type state,
    IT first,
    ST last)
{
    using range_type = detail::bounded_iterable<IT, ST>;
    return basic_text_view<ET, range_type>{state, first, last};
}

// Overload to construct a text view from an N4128 IteratorRange and an
// implicit initial codec state.
template<Encoding ET, origin::Iterator IT, origin::Sentinel<IT> ST>
auto make_text_view(
    IT first,
    ST last)
{
    return make_text_view<ET>(
               ET::initial_state(),
               first,
               last);
}

// Overload to construct a text view from an N4128 SizedIteratorRange and an
// explicitly specified initial codec state.
template<Encoding ET, origin::Forward_iterator IT>
auto make_text_view(
    typename ET::codec_type::state_type state,
    IT first,
    origin::Make_unsigned<origin::Difference_type<IT>> n)
{
    return make_text_view<ET>(
               state,
               first,
               std::next(first, n));
}

// Overload to construct a text view from an N4128 SizedIteratorRange and an
// implicit initial codec state.
template<Encoding ET, origin::Forward_iterator IT>
auto make_text_view(
    IT first,
    origin::Make_unsigned<origin::Difference_type<IT>> n)
{
    return make_text_view<ET>(
               first,
               std::next(first, n));
}

// Overload to construct a text view from an N4128 Iterable const reference
// and an explicitly specified initial codec state.
template<Encoding ET, origin::Input_range Iterable>
auto make_text_view(
    typename ET::codec_type::state_type state,
    const Iterable &iterable)
{
    return make_text_view<ET>(
               state,
               detail::adl_begin(iterable),
               detail::adl_end(iterable));
}

// Overload to construct a text view from an N4128 Iterable const reference
// and an implicit initial codec state.
template<Encoding ET, origin::Input_range Iterable>
auto make_text_view(
    const Iterable &iterable)
{
    return make_text_view<ET>(
               detail::adl_begin(iterable),
               detail::adl_end(iterable));
}

// Overload to construct a text view from a text iterator/sentinel pair.  The
// initial codec state is inferred from the first iterator.
template<Text_iterator TIT, Text_sentinel<TIT> TST>
auto make_text_view(
    TIT first,
    TST last)
{
    using ET = encoding_type_of<TIT>;
    return make_text_view<ET>(
               first.state(),
               first.base(),
               last.base());
}

// Overload to construct a text view from an existing text view.
template<Text_view TVT>
auto make_text_view(
    TVT tv)
{
    return tv;
}


/*
 * make_cstr_range
 */
// Constructs a range from an array reference (that may correspond to a string
// literal) that holds a C style string with a null character terminator.  The
// constructed range excludes the string terminator (that is presumed, but not
// verified to be present).
template<Code_unit CUT, std::size_t N>
auto make_cstr_range(const CUT (&cstr)[N]) {
    using range_type = origin::bounded_range<const CUT*>;
    return range_type{cstr, cstr + (N - 1)};
}


} // namespace text_view
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_TEXT_VIEW_HPP
