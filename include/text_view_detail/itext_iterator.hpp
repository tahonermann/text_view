// Copyright (c) 2017, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef TEXT_VIEW_ITEXT_ITERATOR_HPP // {
#define TEXT_VIEW_ITEXT_ITERATOR_HPP


#include <cassert>
#include <experimental/ranges/iterator>
#include <text_view_detail/adl_customization.hpp>
#include <text_view_detail/caching_iterator.hpp>
#include <text_view_detail/concepts.hpp>
#include <text_view_detail/error_policy.hpp>
#include <text_view_detail/exceptions.hpp>
#include <text_view_detail/subobject.hpp>


namespace std {
namespace experimental {
inline namespace text {


namespace text_detail {

template<TextEncoding ET>
class character_or_error {
    using character_type = character_type_t<ET>;

public:
    bool has_character() const noexcept {
        return have_character;
    }

    void set_character(character_type c) noexcept {
        u.c = c;
        have_character = true;
    }
    character_type& get_character() {
        assert(have_character);
        return u.c;
    }
    const character_type& get_character() const {
        return const_cast<character_or_error*>(this)->get_character();
    }

    void set_error(decode_status ds) noexcept {
        u.ds = ds;
        have_character = false;
    }
    decode_status get_error() const noexcept {
        return have_character ? decode_status::no_error : u.ds;
    }

private:
    union {
        decode_status ds;
        character_type c;
    } u = { decode_status::no_error };
    bool have_character = false;
};


template<ranges::InputIterator I>
struct itext_current_iterator_type {
    using type = caching_iterator<I>;
};
template<ranges::ForwardIterator I>
struct itext_current_iterator_type<I> {
    using type = I;
};

template<ranges::InputIterator I>
using itext_current_iterator_type_t =
    typename itext_current_iterator_type<I>::type;


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
        base_type{std::move(state)},
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
requires ranges::InputIterator<ranges::iterator_t<VT>>
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
        itext_cursor_base<ET, VT>{std::move(state), view},
        current(std::move(current))
    {}

    const iterator_type& base() const noexcept {
        return current.base();
    }

    decltype(auto) base_range() const noexcept {
        return current.cached_range();
    }

    decltype(auto) look_ahead_range() const noexcept {
        return current.look_ahead_range();
    }

protected:
    caching_iterator<iterator_type> current;
};

template<TextEncoding ET, ranges::View VT>
requires ranges::ForwardIterator<ranges::iterator_t<VT>>
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
        itext_cursor_base<ET, VT>{std::move(state), view},
        current_view{first, first}
    {}

    const iterator_type& base() const noexcept {
        return current_view.first;
    }

    const current_view_type& base_range() const noexcept {
        return current_view;
    }

protected:
    current_view_type current_view;
};

template<
    TextEncoding ET,
    ranges::View VT,
    TextErrorPolicy TEP>
class itext_cursor
    : public itext_cursor_data<ET, VT>
{
    using encoding_type = typename itext_cursor::encoding_type;
    using view_type = typename itext_cursor::view_type;
    using error_policy = TEP;
    using state_type = typename itext_cursor::state_type;
    using iterator_type = typename itext_cursor::iterator_type;
    using value_type = character_type_t<encoding_type>;
    using reference = value_type;
    using pointer = const value_type*;
    using difference_type = ranges::difference_type_t<iterator_type>;

    class post_increment_proxy {
        friend class itext_cursor;
    public:
        post_increment_proxy(itext_cursor& self) noexcept
            : self(self), value(self.value)
        {}

        reference operator*() const {
            return self.dereference(value);
        }
        pointer operator->() const {
            return &self.dereference(value);
        }

    private:
        itext_cursor& self;
        character_or_error<encoding_type> value;
    };

public:
    using single_pass =
        std::integral_constant<bool, !ranges::ForwardIterator<iterator_type>>;

    class mixin
        : protected ranges::basic_mixin<itext_cursor>
    {
        using base_type = ranges::basic_mixin<itext_cursor>;
    public:
        using encoding_type = typename itext_cursor::encoding_type;
        using view_type = typename itext_cursor::view_type;
        using error_policy = typename itext_cursor::error_policy;
        using state_type = typename itext_cursor::state_type;

        mixin() = default;

        mixin(
            state_type state,
            const view_type *view,
            iterator_type first)
        :
            base_type{itext_cursor{std::move(state), view, std::move(first)}}
        {}

        mixin(
            const post_increment_proxy &p)
        :
            base_type{p.self}
        {
            this->value = p.value;
        }

        using base_type::base_type;

        const state_type& state() const noexcept {
            return this->get().state();
        }

        const iterator_type& base() const noexcept {
            return this->get().base();
        }

        decltype(auto) base_range() const noexcept {
            return this->get().base_range();
        }

        decltype(auto) look_ahead_range() const noexcept
            requires ! ranges::ForwardIterator<iterator_type>
        {
            return this->get().look_ahead_range();
        }

        bool error_occurred() const noexcept {
            return this->get().error_occurred();
        }

        decode_status get_error() const noexcept {
            return this->get().get_error();
        }

        // Iterators that are not ok include:
        // - Singular iterators.
        // - Past the end iterators.
        bool is_ok() const noexcept {
            return this->get().is_ok();
        }
    };

    itext_cursor() = default;

    itext_cursor(
        state_type state,
        const view_type *view,
        iterator_type first)
    :
        itext_cursor_data<ET, VT>{std::move(state), view, std::move(first)}
    {
        next();
    }

    bool error_occurred() const noexcept {
        return text::error_occurred(value.get_error());
    }

    decode_status get_error() const noexcept {
        return value.get_error();
    }

    bool is_ok() const noexcept {
        return ok;
    }

    reference read() const {
        return dereference(value);
    }

    pointer arrow() const {
        return &dereference(value);
    }

    void next() {
        ok = false;
        this->current.clear_cache();
        auto end = make_caching_iterator_sentinel(
                       text_detail::adl_end(*this->underlying_view()));
        while (this->current != end) {
            value_type tmp_value;
            int decoded_code_units = 0;
            decode_status ds = encoding_type::decode(
                this->state(),
                this->current,
                end,
                tmp_value,
                decoded_code_units);
            if (text::error_occurred(ds)) {
                value.set_error(ds);
                ok = true;
                break;
            }
            else if (ds == decode_status::no_error) {
                value.set_character(tmp_value);
                ok = true;
                break;
            }
            // Clear the cache prior to looping to maintain consistency with
            // forward iterators; non-character encoding sequences are not
            // reflected in base_range().
            this->current.clear_cache();
        }
    }

    void next()
        requires TextForwardDecoder<encoding_type, iterator_type>()
    {
        ok = false;
        this->current_view.first = this->current_view.last;
        iterator_type tmp_iterator{this->current_view.first};
        auto end(text_detail::adl_end(*this->underlying_view()));
        while (tmp_iterator != end) {
            value_type tmp_value;
            int decoded_code_units = 0;
            decode_status ds = encoding_type::decode(
                this->state(),
                tmp_iterator,
                end,
                tmp_value,
                decoded_code_units);
            this->current_view.last = tmp_iterator;
            if (text::error_occurred(ds)) {
                value.set_error(ds);
                ok = true;
                break;
            }
            else if (ds == decode_status::no_error) {
                value.set_character(tmp_value);
                ok = true;
                break;
            }
            this->current_view.first = this->current_view.last;
        }
    }

    // For input iterators, a proxy is returned for post increment operations
    // that limits further operations to dereferences and explicit conversions
    // to a copy of the iterator.  This is done to prevent possibly unintended
    // usage of a copy of the stored input iterator that would otherwise
    // invalidate the stored iterator.
    auto post_increment()
        requires ! TextForwardDecoder<encoding_type, iterator_type>()
    {
        post_increment_proxy proxy{*this};
        next();
        return proxy;
    }

    void prev()
        requires TextBidirectionalDecoder<encoding_type, iterator_type>()
    {
        ok = false;
        this->current_view.last = this->current_view.first;
        std::reverse_iterator<iterator_type> rcurrent{this->current_view.last};
        std::reverse_iterator<iterator_type> rend{
            text_detail::adl_begin(*this->underlying_view())};
        while (rcurrent != rend) {
            value_type tmp_value;
            int decoded_code_units = 0;
            decode_status ds = encoding_type::rdecode(
                this->state(),
                rcurrent,
                rend,
                tmp_value,
                decoded_code_units);
            this->current_view.first = rcurrent.base();
            if (text::error_occurred(ds)) {
                value.set_error(ds);
                ok = true;
                break;
            }
            else if (ds == decode_status::no_error) {
                value.set_character(tmp_value);
                ok = true;
                break;
            }
            this->current_view.last = this->current_view.first;
        }
    }

    void advance(difference_type n)
        requires TextRandomAccessDecoder<encoding_type, iterator_type>()
    {
        if (n < 0) {
            this->current_view.first +=
                ((n+1) * encoding_type::max_code_units);
            prev();
        } else if (n > 0) {
            this->current_view.last +=
                ((n-1) * encoding_type::max_code_units);
            next();
        }
    }

    difference_type distance_to(const itext_cursor &other) const
        requires TextRandomAccessDecoder<encoding_type, iterator_type>()
    {
        return (other.current_view.first - this->current_view.first) /
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
        requires ranges::ForwardIterator<iterator_type>
    {
        return this->base() == other.base();
    }

private:
    static const value_type& dereference(
        const character_or_error<encoding_type> &coe)
    {
        if (coe.has_character()) {
            return coe.get_character();
        }
        if (std::is_base_of<
                text_permissive_error_policy,
                error_policy
            >::value)
        {
            // Permissive error policy: return the substitution
            // character.
            using CT = character_type_t<encoding_type>;
            using CST = character_set_type_t<CT>;
            static CT c{CST::get_substitution_code_point()};
            return c;
        } else {
            // Strict error policy: throw an exception.
            throw text_decode_error{coe.get_error()};
        }
    }

    character_or_error<encoding_type> value;
    bool ok = false;
};

} // namespace text_detail


/*
 * itext_iterator
 */
template<
    TextEncoding ET,
    ranges::View VT,
    TextErrorPolicy TEP = text_default_error_policy>
requires TextForwardDecoder<
             ET,
             text_detail::itext_current_iterator_type_t<
                 ranges::iterator_t<std::add_const_t<VT>>>>()
using itext_iterator =
    ranges::basic_iterator<text_detail::itext_cursor<ET, VT, TEP>>;


} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_ITEXT_ITERATOR_HPP
