#ifndef STDTEXT_TEXT_ITERATOR_HPP // {
#define STDTEXT_TEXT_ITERATOR_HPP


#include <stdtext/concepts.hpp>
#include <iterator>


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


template<typename E, typename CUIT>
struct itext_iterator;

template<Encoding E, Code_unit_iterator CUIT>
requires origin::Input_iterator<CUIT>()
struct itext_iterator<E, CUIT>
    : public std::iterator<
                 typename detail::text_iterator_category_selector<
                     typename E::codec_type,
                     CUIT>::type,
                 typename E::codec_type::character_type,
                 origin::Difference_type<CUIT>,
                 const typename E::codec_type::character_type*,
                 const typename E::codec_type::character_type&>
    , public E::codec_type::state_type
{
    using encoding_type = E;
    using iterator = CUIT;
    using iterator_category = typename itext_iterator::iterator_category;
    using value_type = typename itext_iterator::value_type;
    using reference = typename itext_iterator::reference;
    using pointer = typename itext_iterator::pointer;
    using difference_type = typename itext_iterator::difference_type;

    itext_iterator() = default
        requires origin::Default_constructible<
                     typename encoding_type::codec_type::state_type>();

    itext_iterator(iterator first, iterator last)
        requires origin::Default_constructible<
                     typename encoding_type::codec_type::state_type>()
        : current{first}, end{last}
    {
        ++*this;
    }

    itext_iterator(
        typename encoding_type::codec_type::state_type state,
        iterator first, iterator last)
    :
        encoding_type::codec_type::state_type{state},
        current{first}, end{last}
    {
        ++*this;
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
        return ok == other.ok && (!ok || current == other.current);
    }
    bool operator!=(const itext_iterator& other) const {
        return !(*this == other);
    }

    itext_iterator& operator++() {
        ok = (current != end);
        if (ok) {
            using codec_type = typename encoding_type::codec_type;
            iterator tmp_iterator{current};
            value_type tmp_value;
            int decoded_code_units = 0;
            codec_type::decode(
                *this,
                tmp_iterator,
                end,
                tmp_value,
                decoded_code_units);
            current = tmp_iterator;
            value = tmp_value;
        }
        return *this;
    }
    itext_iterator operator++(int) {
        itext_iterator it{*this};
        ++*this;
        return it;
    }

protected:
    iterator current, end;
    value_type value;
    bool ok;
};

template<Encoding E, Code_unit_iterator CUIT>
requires origin::Forward_iterator<CUIT>()
struct itext_iterator<E, CUIT>
    : public std::iterator<
                 typename detail::text_iterator_category_selector<
                     typename E::codec_type,
                     CUIT>::type,
                 typename E::codec_type::character_type,
                 origin::Difference_type<CUIT>,
                 const typename E::codec_type::character_type*,
                 const typename E::codec_type::character_type&>
    , public E::codec_type::state_type
{
    using encoding_type = E;
    using iterator = CUIT;
    using iterator_category = typename itext_iterator::iterator_category;
    using value_type = typename itext_iterator::value_type;
    using reference = typename itext_iterator::reference;
    using pointer = typename itext_iterator::pointer;
    using difference_type = typename itext_iterator::difference_type;

    itext_iterator() = default
        requires origin::Default_constructible<
                     typename encoding_type::codec_type::state_type>();

    itext_iterator(iterator start, iterator first, iterator finish)
        requires origin::Default_constructible<
                     typename encoding_type::codec_type::state_type>()
    :
        start{start}, first{first}, last{first}, finish{finish}
    {
        ++*this;
    }

    itext_iterator(
        typename encoding_type::codec_type::state_type state,
        iterator start, iterator first, iterator finish)
    :
        encoding_type::codec_type::state_type{state},
        start{start}, first{first}, last{first}, finish{finish}
    {
        ++*this;
    }

    iterator base() const {
        return first;
    }

    iterator begin() const {
        return first;
    }
    iterator end() const {
        return last;
    }

    reference operator*() const {
        return value;
    }
    pointer operator->() const {
        return &value;
    }

    bool operator==(const itext_iterator& other) const {
        return first == other.first;
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
        first = last;
        if (first != finish) {
            using codec_type = typename encoding_type::codec_type;
            iterator tmp_iterator{first};
            value_type tmp_value;
            int decoded_code_units = 0;
            codec_type::decode(
                *this,
                tmp_iterator,
                finish,
                tmp_value,
                decoded_code_units);
            last = tmp_iterator;
            value = tmp_value;
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
        last = first;
        using codec_type = typename encoding_type::codec_type;
        std::reverse_iterator<iterator> rcurrent{last};
        std::reverse_iterator<iterator> rfinish{start};
        value_type tmp_value;
        int decoded_code_units = 0;
        codec_type::rdecode(
            *this,
            rcurrent,
            rfinish,
            tmp_value,
            decoded_code_units);
        first = rcurrent.base();
        value = tmp_value;
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
            first += ((n+1) * encoding_type::codec_type::max_code_units);
            --*this;
        } else if (n > 0) {
            last += ((n-1) * encoding_type::codec_type::max_code_units);
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
        return (first - it.first) / encoding_type::codec_type::max_code_units;
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

protected:
    iterator start, first, last, finish;
    value_type value;
};

template<Encoding E, Code_unit_iterator CUIT>
requires origin::Random_access_iterator<CUIT>()
itext_iterator<E, CUIT> operator+(
    origin::Difference_type<CUIT> n,
    itext_iterator<E, CUIT> it)
{
    return it += n;
}


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
    , public E::codec_type::state_type
{
    using encoding_type = E;
    using iterator = CUIT;
    using iterator_category = typename otext_iterator::iterator_category;
    using value_type = typename otext_iterator::value_type;
    using reference = typename otext_iterator::reference;
    using pointer = typename otext_iterator::pointer;
    using difference_type = typename otext_iterator::difference_type;

    otext_iterator() = default
        requires origin::Default_constructible<
                     typename encoding_type::codec_type::state_type>();

    otext_iterator(iterator current)
        requires origin::Default_constructible<
                     typename encoding_type::codec_type::state_type>()
        : current{current} {}

    otext_iterator(
        typename encoding_type::codec_type::state_type state,
        iterator current)
    :
        encoding_type::codec_type::state_type{state}, current{current}
    {}

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
        const typename encoding_type::codec_type::character_type &value)
    {
        using codec_type = typename encoding_type::codec_type;
        iterator tmp{current};
        int encoded_code_units = 0;
        codec_type::encode(*this, tmp, value, encoded_code_units);
        current = tmp;
        return *this;
    }

protected:
    iterator current;
};


template<typename E, typename CUIT>
struct rotext_iterator;

template<Encoding E, Code_unit_iterator CUIT>
requires origin::Output_iterator<CUIT, typename E::codec_type::code_unit_type>()
struct rotext_iterator<E, CUIT>
    : public std::iterator<
                 std::output_iterator_tag,
                 typename E::codec_type::character_type,
                 origin::Difference_type<CUIT>,
                 typename E::codec_type::character_type*,
                 typename E::codec_type::character_type&>
    , public E::codec_type::state_type
{
    using encoding_type = E;
    using iterator = CUIT;
    using iterator_category = typename rotext_iterator::iterator_category;
    using value_type = typename rotext_iterator::value_type;
    using reference = typename rotext_iterator::reference;
    using pointer = typename rotext_iterator::pointer;
    using difference_type = typename rotext_iterator::difference_type;

    rotext_iterator() = default
        requires origin::Default_constructible<
                     typename encoding_type::codec_type::state_type>();

    rotext_iterator(iterator current)
        requires origin::Default_constructible<
                     typename encoding_type::codec_type::state_type>()
        : current{current} {}

    rotext_iterator(
        typename encoding_type::codec_type::state_type state,
        iterator current)
    :
        encoding_type::codec_type::state_type{state}, current{current}
    {}

    iterator base() const {
        return current;
    }

    rotext_iterator& operator*() {
        return *this;
    }

    bool operator==(const rotext_iterator& other) const {
        return current == other.current;
    }
    bool operator!=(const rotext_iterator& other) const {
        return current != other.current;
    }

    rotext_iterator& operator++() {
        return *this;
    }
    rotext_iterator& operator++(int) {
        return *this;
    }

    rotext_iterator& operator=(
        const typename encoding_type::codec_type::character_type &value)
    {
        using codec_type = typename encoding_type::codec_type;
        iterator tmp{current};
        int encoded_code_units = 0;
        codec_type::rencode(*this, tmp, value, encoded_code_units);
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
