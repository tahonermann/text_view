#ifndef STDTEXT_TEXT_ITERATOR_HPP // {
#define STDTEXT_TEXT_ITERATOR_HPP


#include <stdtext/adl_customization.hpp>
#include <stdtext/concepts.hpp>
#include <iterator>
#include <origin/core/traits.hpp>


namespace std {
namespace experimental {
namespace text {


namespace detail {
/*
 * Codec concept       Code unit iterator concept Text iterator category
 * ------------------- -------------------------- ---------------------------
 * Codec               Output_iterator            output_iterator_tag
 * Codec               Input_iterator             input_iterator_tag
 * Codec               Forward_iterator           forward_iterator_tag
 * Codec               Bidirectional_iterator     forward_iterator_tag
 * Codec               Random_access_iterator     forward_iterator_tag
 * Bidirectional_codec Output_iterator            output_iterator_tag
 * Bidirectional_codec Input_iterator             input_iterator_tag
 * Bidirectional_codec Forward_iterator           forward_iterator_tag
 * Bidirectional_codec Bidirectional_iterator     bidirectional_iterator_tag
 * Bidirectional_codec Random_access_iterator     bidirectional_iterator_tag
 * Random_access_codec Output_iterator            output_iterator_tag
 * Random_access_codec Input_iterator             input_iterator_tag
 * Random_access_codec Forward_iterator           forward_iterator_tag
 * Random_access_codec Bidirectional_iterator     bidirectional_iterator_tag
 * Random_access_codec Random_access_iterator     random_access_iterator_tag
 */
template<typename Codec, typename Iterator>
struct text_iterator_category_selector;

template<Codec C, Code_unit_iterator CUIT>
struct text_iterator_category_selector<C, CUIT> {
    using type = origin::Iterator_category<CUIT>;
};
template<Codec C, Code_unit_iterator CUIT>
requires origin::Bidirectional_iterator<CUIT>() // or Random_access_iterator
      && ! Bidirectional_codec<C>()             // and ! Random_access_codec
struct text_iterator_category_selector<C, CUIT> {
    using type = std::forward_iterator_tag;
};
template<Bidirectional_codec C, Code_unit_iterator CUIT>
requires origin::Random_access_iterator<CUIT>()
      && ! Random_access_codec<C>()
struct text_iterator_category_selector<C, CUIT> {
    using type = std::bidirectional_iterator_tag;
};
} // namespace detail


template<typename ET, typename RT>
struct itext_iterator;

template<Encoding ET, origin::Input_range RT>
requires origin::Input_iterator<origin::Iterator_type<RT>>()
struct itext_iterator<ET, RT>
    : public std::iterator<
                 typename detail::text_iterator_category_selector<
                     typename ET::codec_type,
                     origin::Iterator_type<const RT>>::type,
                 typename ET::codec_type::character_type,
                 origin::Difference_type<origin::Iterator_type<const RT>>,
                 const typename ET::codec_type::character_type*,
                 const typename ET::codec_type::character_type&>
    , private ET::codec_type::state_type
{
    using encoding_type = ET;
    using range_type = origin::Remove_reference<RT>;
    using state_type = typename encoding_type::codec_type::state_type;
    using iterator = origin::Iterator_type<const RT>;
    using iterator_category = typename itext_iterator::iterator_category;
    using value_type = typename itext_iterator::value_type;
    using reference = typename itext_iterator::reference;
    using pointer = typename itext_iterator::pointer;
    using difference_type = typename itext_iterator::difference_type;

    itext_iterator()
        requires origin::Default_constructible<state_type>()
              && origin::Default_constructible<iterator>()
        : range{}, current{}, value{} {}

    itext_iterator(
        const state_type &state,
        const range_type *range,
        iterator current)
    :
        // CWG DR1467.  List-initialization doesn't consider copy constructors
        // for aggregates.  The state_type base class must be initialized with
        // an expression-list.
        state_type(state),
        range{range},
        current{current}
    {
        ++*this;
    }

    const state_type& state() const {
        return *this;
    }
    state_type& state() {
        return *this;
    }

    iterator base() const {
        return current;
    }

    reference operator*() const {
        return value;
    }
    pointer operator->() const {
        return &value;
    }

    bool operator==(const itext_iterator& other) const {
        return current == other.current;
    }
    bool operator!=(const itext_iterator& other) const {
        return !(*this == other);
    }

    itext_iterator& operator++() {
        using codec_type = typename encoding_type::codec_type;

        iterator tmp_iterator{current};
        auto end(detail::adl_end(*range));
        while (tmp_iterator != end) {
            value_type tmp_value;
            int decoded_code_units = 0;
            bool decoded_code_point = codec_type::decode(
                state(),
                tmp_iterator,
                end,
                tmp_value,
                decoded_code_units);
            current = tmp_iterator;
            if (decoded_code_point) {
                value = tmp_value;
                break;
            }
        }
        return *this;
    }
    itext_iterator operator++(int) {
        itext_iterator it{*this};
        ++*this;
        return it;
    }

private:
    const range_type *range;
    iterator current;
    value_type value;
};

template<Encoding ET, origin::Input_range RT>
requires origin::Forward_iterator<origin::Iterator_type<RT>>()
struct itext_iterator<ET, RT>
    : public std::iterator<
                 typename detail::text_iterator_category_selector<
                     typename ET::codec_type,
                     origin::Iterator_type<const RT>>::type,
                 typename ET::codec_type::character_type,
                 origin::Difference_type<origin::Iterator_type<const RT>>,
                 const typename ET::codec_type::character_type*,
                 const typename ET::codec_type::character_type&>
    , private ET::codec_type::state_type
{
    using encoding_type = ET;
    using range_type = origin::Remove_reference<RT>;
    using state_type = typename ET::codec_type::state_type;
    using iterator = origin::Iterator_type<const RT>;
    using iterator_category = typename itext_iterator::iterator_category;
    using value_type = typename itext_iterator::value_type;
    using reference = typename itext_iterator::reference;
    using pointer = typename itext_iterator::pointer;
    using difference_type = typename itext_iterator::difference_type;

    struct current_range_type {
        current_range_type()
            : first{}, last{} {}
        current_range_type(iterator first, iterator last)
            : first{first}, last{last} {}

        iterator begin() const { return first; }
        iterator end() const { return last; }

        iterator first;
        iterator last;
    };

    itext_iterator()
        requires origin::Default_constructible<state_type>()
              && origin::Default_constructible<iterator>()
        : range{}, current_range{}, value{} {}

    itext_iterator(
        const state_type &state,
        const range_type *range,
        iterator first)
    :
        // CWG DR1467.  List-initialization doesn't consider copy constructors
        // for aggregates.  The state_type base class must be initialized with
        // an expression-list.
        state_type(state),
        range{range},
        current_range{first, first}
    {
        ++*this;
    }

    const state_type& state() const {
        return *this;
    }
    state_type& state() {
        return *this;
    }

    iterator base() const {
        return current_range.first;
    }

    const current_range_type& base_range() const {
        return current_range;
    }

    reference operator*() const {
        return value;
    }
    pointer operator->() const {
        return &value;
    }

    bool operator==(const itext_iterator& other) const {
        return current_range.first == other.current_range.first;
    }
    bool operator!=(const itext_iterator& other) const {
        return !(*this == other);
    }

    bool operator<(const itext_iterator& other) const
        requires origin::Derived<
                     iterator_category,
                     std::random_access_iterator_tag>()
    {
        return (other - *this) > 0;
    }
    bool operator>(const itext_iterator& other) const
        requires origin::Derived<
                     iterator_category,
                     std::random_access_iterator_tag>()
    {
        return other < *this;
    }
    bool operator<=(const itext_iterator& other) const
        requires origin::Derived<
                     iterator_category,
                     std::random_access_iterator_tag>()
    {
        return !(*this > other);
    }
    bool operator>=(const itext_iterator& other) const
        requires origin::Derived<
                     iterator_category,
                     std::random_access_iterator_tag>()
    {
        return !(*this < other);
    }

    itext_iterator& operator++() {
        using codec_type = typename encoding_type::codec_type;

        current_range.first = current_range.last;
        iterator tmp_iterator{current_range.first};
        auto end(detail::adl_end(*range));
        while (tmp_iterator != end) {
            value_type tmp_value;
            int decoded_code_units = 0;
            bool decoded_code_point = codec_type::decode(
                state(),
                tmp_iterator,
                end,
                tmp_value,
                decoded_code_units);
            current_range.last = tmp_iterator;
            if (decoded_code_point) {
                value = tmp_value;
                break;
            }
            current_range.first = current_range.last;
        }
        return *this;
    }
    itext_iterator operator++(int) {
        itext_iterator it{*this};
        ++*this;
        return it;
    }

    itext_iterator& operator--()
        requires origin::Derived<
                     iterator_category,
                     std::bidirectional_iterator_tag>()
    {
        using codec_type = typename encoding_type::codec_type;

        current_range.last = current_range.first;
        std::reverse_iterator<iterator> rcurrent{current_range.last};
        std::reverse_iterator<iterator> rend{detail::adl_begin(*range)};
        while (rcurrent != rend) {
            value_type tmp_value;
            int decoded_code_units = 0;
            bool decoded_code_point = codec_type::rdecode(
                state(),
                rcurrent,
                rend,
                tmp_value,
                decoded_code_units);
            current_range.first = rcurrent.base();
            if (decoded_code_point) {
                value = tmp_value;
                break;
            }
            current_range.last = current_range.first;
        }
        return *this;
    }
    itext_iterator operator--(int)
        requires origin::Derived<
                     iterator_category,
                     std::bidirectional_iterator_tag>()
    {
        itext_iterator it{*this};
        --*this;
        return it;
    }

    itext_iterator& operator+=(difference_type n)
        requires origin::Derived<
                     iterator_category,
                     std::random_access_iterator_tag>()
    {
        if (n < 0) {
            current_range.first +=
                ((n+1) * encoding_type::codec_type::max_code_units);
            --*this;
        } else if (n > 0) {
            current_range.last +=
                ((n-1) * encoding_type::codec_type::max_code_units);
            ++*this;
        }
        return *this;
    }

    itext_iterator operator+(difference_type n) const
        requires origin::Derived<
                     iterator_category,
                     std::random_access_iterator_tag>()
    {
        itext_iterator it{*this};
        return it += n;
    }

    itext_iterator& operator-=(difference_type n)
        requires origin::Derived<
                     iterator_category,
                     std::random_access_iterator_tag>()
    {
        return *this += -n;
    }

    itext_iterator operator-(difference_type n) const
        requires origin::Derived<
                     iterator_category,
                     std::random_access_iterator_tag>()
    {
        itext_iterator it{*this};
        return it -= n;
    }

    difference_type operator-(itext_iterator it) const
        requires origin::Derived<
                     iterator_category,
                     std::random_access_iterator_tag>()
    {
        return (current_range.first - it.current_range.first) /
               encoding_type::codec_type::max_code_units;
    }

    // Random access iterator requirements state that operator[] must return
    // a reference.  That isn't possible here since the reference would be to
    // a value stored in an object (The itext_iterator instance produced for
    // '*this + n') that is destroyed before the function returns.
    value_type operator[](difference_type n) const
        requires origin::Derived<
                     iterator_category,
                     std::random_access_iterator_tag>()
    {
        return *(*this + n);
    }

private:
    const range_type *range;
    current_range_type current_range;
    value_type value;
};

template<Encoding ET, origin::Input_range RT>
requires origin::Random_access_iterator<origin::Iterator_type<RT>>()
itext_iterator<ET, RT> operator+(
    origin::Difference_type<origin::Iterator_type<RT>> n,
    itext_iterator<ET, RT> it)
{
    return it += n;
}


template<Encoding ET, origin::Input_range RT>
struct itext_sentinel {
    using range_type = origin::Remove_reference<RT>;
    using sentinel = origin::Sentinel_type<RT>;

    itext_sentinel(sentinel s)
        : s{s} {}

    // This constructor is provided in lieu of a specialization of
    // std::common_type in order to satisfy the common type requirement for
    // cross-type equality comparison.  See N4128, 10 "Appendix 2: Sentinels,
    // Iterators, and the Cross-Type EqualityComparable Concept" and N3351,
    // 3.3 "Foundational Concepts".
    itext_sentinel(const itext_iterator<ET, RT> &ti)
        : s{ti.base()} {}

    bool operator==(const itext_sentinel& other) const {
        // Sentinels always compare equal regardless of any internal state.
        // See N4128, 10.1 "Sentinel Equality".
        return true;
    }
    bool operator!=(const itext_sentinel& other) const {
        return !(*this == other);
    }

    friend bool operator==(
        const itext_iterator<ET, RT> &ti,
        const itext_sentinel &ts)
    {
        return ti.base() == ts.base();
    }
    friend bool operator!=(
        const itext_iterator<ET, RT> &ti,
        const itext_sentinel &ts)
    {
        return !(ti == ts);
    }
    friend bool operator==(
        const itext_sentinel &ts,
        const itext_iterator<ET, RT> &ti)
    {
        return ti == ts;
    }
    friend bool operator!=(
        const itext_sentinel &ts,
        const itext_iterator<ET, RT> &ti)
    {
        return !(ts == ti);
    }

    bool operator<(const itext_sentinel& other) const {
        // Sentinels always compare equal regardless of any internal state.
        // See N4128, 10.1 "Sentinel Equality".
        return false;
    }
    bool operator>(const itext_sentinel& other) const {
        return other < *this;
    }
    bool operator<=(const itext_sentinel& other) const {
        return !(*this > other);
    }
    bool operator>=(const itext_sentinel& other) const {
        return !(*this < other);
    }

    friend bool operator<(
        const itext_iterator<ET, RT> &ti,
        const itext_sentinel &ts)
    requires origin::Weakly_ordered<
                 typename itext_iterator<ET, RT>::iterator,
                 sentinel>()
    {
        return ti.base() < ts.base();
    }
    friend bool operator>(
        const itext_iterator<ET, RT> &ti,
        const itext_sentinel &ts)
    requires origin::Weakly_ordered<
                 typename itext_iterator<ET, RT>::iterator,
                 sentinel>()
    {
        return ti.base() > ts.base();
    }
    friend bool operator<=(
        const itext_iterator<ET, RT> &ti,
        const itext_sentinel &ts)
    requires origin::Weakly_ordered<
                 typename itext_iterator<ET, RT>::iterator,
                 sentinel>()
    {
        return ti.base() <= ts.base();
    }
    friend bool operator>=(
        const itext_iterator<ET, RT> &ti,
        const itext_sentinel &ts)
    requires origin::Weakly_ordered<
                 typename itext_iterator<ET, RT>::iterator,
                 sentinel>()
    {
        return ti.base() >= ts.base();
    }

    friend bool operator<(
        const itext_sentinel &ts,
        const itext_iterator<ET, RT> &ti)
    requires origin::Weakly_ordered<
                 typename itext_iterator<ET, RT>::iterator,
                 sentinel>()
    {
        return ts.base() < ti.base();
    }
    friend bool operator>(
        const itext_sentinel &ts,
        const itext_iterator<ET, RT> &ti)
    requires origin::Weakly_ordered<
                 typename itext_iterator<ET, RT>::iterator,
                 sentinel>()
    {
        return ts.base() > ti.base();
    }
    friend bool operator<=(
        const itext_sentinel &ts,
        const itext_iterator<ET, RT> &ti)
    requires origin::Weakly_ordered<
                 typename itext_iterator<ET, RT>::iterator,
                 sentinel>()
    {
        return ts.base() <= ti.base();
    }
    friend bool operator>=(
        const itext_sentinel &ts,
        const itext_iterator<ET, RT> &ti)
    requires origin::Weakly_ordered<
                 typename itext_iterator<ET, RT>::iterator,
                 sentinel>()
    {
        return ts.base() >= ti.base();
    }

    sentinel base() const {
        return s;
    }

private:
    sentinel s;
};


template<typename E, typename CUIT>
struct otext_iterator;

template<Encoding E, Code_unit_iterator CUIT>
requires origin::Output_iterator<CUIT, typename E::codec_type::code_unit_type>()
struct otext_iterator<E, CUIT>
    : public std::iterator<
                 std::output_iterator_tag,
                 typename E::codec_type::character_type,
                 origin::Difference_type<CUIT>,
                 typename E::codec_type::character_type*,
                 typename E::codec_type::character_type&>
    , private E::codec_type::state_type
{
    using encoding_type = E;
    using state_type = typename E::codec_type::state_type;
    using state_transition_type = typename E::codec_type::state_transition_type;
    using iterator = CUIT;
    using iterator_category = typename otext_iterator::iterator_category;
    using value_type = typename otext_iterator::value_type;
    using reference = typename otext_iterator::reference;
    using pointer = typename otext_iterator::pointer;
    using difference_type = typename otext_iterator::difference_type;

    otext_iterator()
        requires origin::Default_constructible<state_type>()
        = default;

    otext_iterator(
        state_type state,
        iterator current)
    :
        // CWG DR1467.  List-initialization doesn't consider copy constructors
        // for aggregates.  The state_type base class must be initialized with
        // an expression-list.
        state_type(state),
        current{current}
    {}

    const state_type& state() const {
        return *this;
    }
    state_type& state() {
        return *this;
    }

    iterator base() const {
        return current;
    }

    otext_iterator& operator*() {
        return *this;
    }

    bool operator==(const otext_iterator& other) const {
        return current == other.current;
    }
    bool operator!=(const otext_iterator& other) const {
        return current != other.current;
    }

    otext_iterator& operator++() {
        return *this;
    }
    otext_iterator& operator++(int) {
        return *this;
    }

    otext_iterator& operator=(
        const state_transition_type &stt)
    {
        using codec_type = typename encoding_type::codec_type;
        iterator tmp{current};
        int encoded_code_units = 0;
        codec_type::encode_state_transition(state(), tmp, stt,
                                            encoded_code_units);
        current = tmp;
        return *this;
    }

    otext_iterator& operator=(
        const typename encoding_type::codec_type::character_type &value)
    {
        using codec_type = typename encoding_type::codec_type;
        iterator tmp{current};
        int encoded_code_units = 0;
        codec_type::encode(state(), tmp, value, encoded_code_units);
        current = tmp;
        return *this;
    }

protected:
    iterator current;
};


} // namespace text
} // namespace experimental
} // namespace std


#endif // } STDTEXT_TEXT_ITERATOR_HPP
