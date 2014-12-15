#ifndef STDTEXT_TEXT_VIEW_HPP // {
#define STDTEXT_TEXT_VIEW_HPP


#include <stdtext/adl_customization.hpp>
#include <stdtext/concepts.hpp>
#include <stdtext/text_iterator.hpp>
#include <iterator>


namespace std {
namespace experimental {
namespace text {


template<typename Encoding, typename T>
struct text_view;

template<Encoding E, Code_unit_iterator CUIT>
struct text_view<E, CUIT> {
    using encoding_type = E;
    using code_unit_iterator = CUIT;
    using iterator = itext_iterator<encoding_type, code_unit_iterator>;

    text_view(code_unit_iterator first, code_unit_iterator last)
        : first{first}, last{last} {}

    iterator begin() const
        requires ! origin::Forward_iterator<iterator>() // Input_iterator only
    {
        return iterator{first, last};
    }
    iterator end() const
        requires ! origin::Forward_iterator<iterator>() // Input_iterator only
    {
        return iterator{last, last};
    }

    iterator begin() const
        requires origin::Forward_iterator<iterator>()
    {
        return iterator{first, first, last};
    }
    iterator end() const
        requires origin::Forward_iterator<iterator>()
    {
        return iterator{first, last, last};
    }

private:
    code_unit_iterator first, last;
};

// FIXME: Per D4128 (Ranges for the Standard Library), an Iterable concept is
// FIXME: defined with less constraints than Range (or Input_range).  Does
// FIXME: text_view require the stronger Input_range constraints?
template<Encoding E, origin::Input_range R>
struct text_view<E, R> {
    using encoding_type = E;
    using code_unit_iterator = origin::Iterator_type<R>;
    using iterator = itext_iterator<encoding_type, code_unit_iterator>;

    text_view(const R &range) : range{range} {}

    iterator begin() const
        requires ! origin::Forward_iterator<iterator>() // Input_iterator only
    {
        return iterator{detail::adl_begin(range), detail::adl_end(range)};
    }
    iterator end() const
        requires ! origin::Forward_iterator<iterator>() // Input_iterator only
    {
        return iterator{detail::adl_end(range), detail::adl_end(range)};
    }

    iterator begin() const
        requires origin::Forward_iterator<iterator>()
    {
        return iterator{
                   detail::adl_begin(range),
                   detail::adl_begin(range),
                   detail::adl_end(range)};
    }
    iterator end() const
        requires origin::Forward_iterator<iterator>()
    {
        return iterator{
                   detail::adl_begin(range),
                   detail::adl_end(range),
                   detail::adl_end(range)};
    }

private:
    const R &range;
};


/*
 * Make text view
 */
template<Encoding E, origin::Input_range R>
auto make_text_view(const R &range) {
    return text_view<E, const R>{range};
}

template<Encoding E, origin::Iterator I>
auto make_text_view(I first, I last) {
    return text_view<E, I>{first, last};
}


} // namespace text
} // namespace experimental
} // namespace std


#endif // } STDTEXT_TEXT_VIEW_HPP
