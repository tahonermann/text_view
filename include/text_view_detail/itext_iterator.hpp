// Copyright (c) 2016, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef TEXT_VIEW_ITEXT_ITERATOR_HPP // {
#define TEXT_VIEW_ITEXT_ITERATOR_HPP


#include <experimental/ranges/iterator>
#include <text_view_detail/adl_customization.hpp>
#include <text_view_detail/concepts.hpp>
#include <text_view_detail/subobject.hpp>


namespace std {
namespace experimental {
inline namespace text {


namespace text_detail {


template<TextEncoding ET, ranges::View VT>
class itext_cursor_base
    : private subobject<typename ET::state_type>
{
    using base_type = subobject<typename ET::state_type>;
    using encoding_type = ET;
    using view_type = VT;
    using iterator_type = ranges::iterator_t<std::add_const_t<view_type>>;
    using state_type = typename encoding_type::state_type;

public:
    itext_cursor_base() = default;

    itext_cursor_base(
        state_type state,
        const view_type *view)
    :
        base_type{state},
        view(view)
    {}

    const state_type& state() const noexcept {
        return base_type::get();
    }
    state_type& state() noexcept {
        return base_type::get();
    }

    const view_type* underlying_view() const noexcept {
        return view;
    }

private:
    const view_type *view;
};

template<TextEncoding ET, ranges::View VT>
requires TextDecoder<ET, ranges::iterator_t<std::add_const_t<VT>>>()
class itext_cursor_data
    : public itext_cursor_base<ET, VT>
{
    using encoding_type = typename itext_cursor_data::encoding_type;
    using view_type = typename itext_cursor_data::view_type;
    using state_type = typename itext_cursor_data::state_type;
    using iterator_type = typename itext_cursor_data::iterator_type;

public:
    itext_cursor_data() = default;

    itext_cursor_data(
        state_type state,
        const view_type *view,
        iterator_type current)
    :
        itext_cursor_base<ET, VT>{state, view},
        current(current)
    {}

    const iterator_type& base() const noexcept {
        return current;
    }
    iterator_type& base() noexcept {
        return current;
    }

private:
    iterator_type current;
};

template<TextEncoding ET, ranges::View VT>
requires TextForwardDecoder<ET, ranges::iterator_t<std::add_const_t<VT>>>()
class itext_cursor_data<ET, VT>
    : public itext_cursor_base<ET, VT>
{
    using encoding_type = typename itext_cursor_data::encoding_type;
    using view_type = typename itext_cursor_data::view_type;
    using state_type = typename itext_cursor_data::state_type;
    using iterator_type = typename itext_cursor_data::iterator_type;

    struct current_view_type {
        current_view_type()
            : first{}, last{} {}
        current_view_type(iterator_type first, iterator_type last)
            : first{first}, last{last} {}

        iterator_type begin() const { return first; }
        iterator_type end() const { return last; }

        iterator_type first;
        iterator_type last;
    };

public:
    itext_cursor_data() = default;

    itext_cursor_data(
        state_type state,
        const view_type *view,
        iterator_type first)
    :
        itext_cursor_base<ET, VT>{state, view},
        current_view{first, first}
    {}

    const iterator_type& base() const noexcept {
        return current_view.first;
    }
    iterator_type& base() noexcept {
        return current_view.first;
    }

    const current_view_type& base_range() const noexcept {
        return current_view;
    }
    current_view_type& base_range() noexcept {
        return current_view;
    }

private:
    current_view_type current_view;
};

template<TextEncoding ET, ranges::View VT>
class itext_iterator_mixin;

template<TextEncoding ET, ranges::View VT>
class itext_cursor
    : public itext_cursor_data<ET, VT>
{
    using encoding_type = typename itext_cursor::encoding_type;
    using view_type = typename itext_cursor::view_type;
    using state_type = typename itext_cursor::state_type;
    using iterator_type = typename itext_cursor::iterator_type;
    using value_type = character_type_t<encoding_type>;
    using reference = value_type;
    using pointer = const value_type*;
    using difference_type = ranges::difference_type_t<iterator_type>;

public:
    using mixin = itext_iterator_mixin<ET, VT>;
    using single_pass =
        std::integral_constant<bool, !ranges::ForwardIterator<iterator_type>()>;

    itext_cursor() = default;

    itext_cursor(
        state_type state,
        const view_type *view,
        iterator_type first)
    :
        itext_cursor_data<ET, VT>{state, view, first}
    {
        next();
    }

    bool is_ok() const noexcept {
        return ok;
    }

    reference read() const noexcept {
        return value;
    }

    pointer arrow() const noexcept {
        return &value;
    }

    void next() {
        ok = false;
        iterator_type tmp_iterator{this->base()};
        auto end(text_detail::adl_end(*this->underlying_view()));
        while (tmp_iterator != end) {
            value_type tmp_value;
            int decoded_code_units = 0;
            bool decoded_code_point = encoding_type::decode(
                this->state(),
                tmp_iterator,
                end,
                tmp_value,
                decoded_code_units);
            this->base() = tmp_iterator;
            if (decoded_code_point) {
                value = tmp_value;
                ok = true;
                break;
            }
        }
    }

    void next()
        requires TextForwardDecoder<encoding_type, iterator_type>()
    {
        ok = false;
        this->base_range().first = this->base_range().last;
        iterator_type tmp_iterator{this->base_range().first};
        auto end(text_detail::adl_end(*this->underlying_view()));
        while (tmp_iterator != end) {
            value_type tmp_value;
            int decoded_code_units = 0;
            bool decoded_code_point = encoding_type::decode(
                this->state(),
                tmp_iterator,
                end,
                tmp_value,
                decoded_code_units);
            this->base_range().last = tmp_iterator;
            if (decoded_code_point) {
                value = tmp_value;
                ok = true;
                break;
            }
            this->base_range().first = this->base_range().last;
        }
    }

    itext_cursor post_increment()
        requires ! TextForwardDecoder<encoding_type, iterator_type>()
    {
        itext_cursor tmp = *this;
        next();
        return tmp;
    }

    void prev()
        requires TextBidirectionalDecoder<encoding_type, iterator_type>()
    {
        ok = false;
        this->base_range().last = this->base_range().first;
        std::reverse_iterator<iterator_type> rcurrent{this->base_range().last};
        std::reverse_iterator<iterator_type> rend{
            text_detail::adl_begin(*this->underlying_view())};
        while (rcurrent != rend) {
            value_type tmp_value;
            int decoded_code_units = 0;
            bool decoded_code_point = encoding_type::rdecode(
                this->state(),
                rcurrent,
                rend,
                tmp_value,
                decoded_code_units);
            this->base_range().first = rcurrent.base();
            if (decoded_code_point) {
                value = tmp_value;
                ok = true;
                break;
            }
            this->base_range().last = this->base_range().first;
        }
    }

    void advance(difference_type n)
        requires TextRandomAccessDecoder<encoding_type, iterator_type>()
    {
        if (n < 0) {
            this->base_range().first +=
                ((n+1) * encoding_type::max_code_units);
            prev();
        } else if (n > 0) {
            this->base_range().last +=
                ((n-1) * encoding_type::max_code_units);
            next();
        }
    }

    difference_type distance_to(const itext_cursor &other) const
        requires TextRandomAccessDecoder<encoding_type, iterator_type>()
    {
        return (other.base_range().first - this->base_range().first) /
               encoding_type::max_code_units;
    }

    bool equal(const itext_cursor &other) const {
        // For input iterators, the base iterator corresponds to the next input
        // to be decoded.  Naively checking for base comparison only therefore
        // results in premature matches when the last code point in the input
        // is consumed.  For this reason, base equality is only considered a
        // match if the iterator is not ok.  It is assumed that the reason the
        // base iterator is not ok is that an attempt was made to decode a
        // code point after the last one in the input.  This heuristic can be
        // defeated when an iterator is not ok for other reasons.
        return ok == other.ok
            && (!ok || this->base() == other.base());
    }
    bool equal(const itext_cursor &other) const
        requires ranges::ForwardIterator<iterator_type>()
    {
        return this->base() == other.base();
    }

private:
    value_type value = {};
    bool ok = false;
};

template<TextEncoding ET, ranges::View VT>
class itext_iterator_mixin
    : protected ranges::basic_mixin<itext_cursor<ET, VT>>
{
    using iterator_type = typename itext_iterator_mixin::iterator_type;
    using cursor_type = itext_cursor<ET, VT>;
    using base_type = ranges::basic_mixin<cursor_type>;

public:
    using encoding_type = ET;
    using view_type = VT;
    using state_type = typename ET::state_type;

    itext_iterator_mixin() = default;

    itext_iterator_mixin(
        state_type state,
        const view_type *view,
        iterator_type first)
    :
        base_type{cursor_type{state, view, first}}
    {}

    using base_type::base_type;

    const state_type& state() const noexcept {
        return this->get().state();
    }

    iterator_type base() const noexcept {
        return this->get().base();
    }

    auto base_range() const noexcept
    requires TextDecoder<encoding_type, iterator_type>()
          && ranges::ForwardIterator<iterator_type>()
    {
        return this->get().base_range();
    }

    // Iterators that are not ok include:
    // - Singular iterators.
    // - Past the end iterators.
    // - Iterators for which a decoding error occurred during increment or
    //   decrement operations.
    bool is_ok() const noexcept {
        return this->get().is_ok();
    }
};

} // namespace text_detail


/*
 * itext_iterator
 */
template<TextEncoding ET, ranges::View VT>
requires TextDecoder<ET, ranges::iterator_t<std::add_const_t<VT>>>()
using itext_iterator =
    ranges::basic_iterator<text_detail::itext_cursor<ET, VT>>;


} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_ITEXT_ITERATOR_HPP
