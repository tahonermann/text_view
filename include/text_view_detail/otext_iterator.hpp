// Copyright (c) 2017, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef TEXT_VIEW_OTEXT_ITERATOR_HPP // {
#define TEXT_VIEW_OTEXT_ITERATOR_HPP


#include <experimental/ranges/iterator>
#include <text_view_detail/concepts.hpp>
#include <text_view_detail/default_encoding.hpp>
#include <text_view_detail/error_policy.hpp>
#include <text_view_detail/iterator_preserve.hpp>
#include <text_view_detail/subobject.hpp>


namespace std {
namespace experimental {
inline namespace text {


namespace text_detail {

template<
    TextEncoding ET,
    CodeUnitOutputIterator<code_unit_type_t<ET>> CUIT,
    TextErrorPolicy TEP = text_default_error_policy>
class otext_cursor
    : private subobject<typename ET::state_type>
{
    using base_type = subobject<typename ET::state_type>;
    using encoding_type = ET;
    using iterator_type = CUIT;
    using error_policy = TEP;
    using state_type = typename ET::state_type;
    using state_transition_type = typename ET::state_transition_type;

    class post_increment_proxy {
        friend class otext_cursor;
    public:
        post_increment_proxy(otext_cursor& self) noexcept
            : self(self)
        {}

        post_increment_proxy& operator*() noexcept {
            return *this;
        }

        post_increment_proxy& operator=(
            const state_transition_type &stt)
        {
            self.write(stt);
            return *this;
        }

        post_increment_proxy& operator=(
            const character_type_t<encoding_type> &value)
        {
            self.write(value);
            return *this;
        }

    private:
        otext_cursor& self;
    };

public:
    using difference_type = ranges::difference_type_t<iterator_type>;

    class mixin
        : protected ranges::basic_mixin<otext_cursor>
    {
        using base_type = ranges::basic_mixin<otext_cursor>;
    public:
        using encoding_type = typename otext_cursor::encoding_type;
        using error_policy = typename otext_cursor::error_policy;
        using state_type = typename otext_cursor::state_type;
        using state_transition_type = typename otext_cursor::state_transition_type;

        mixin() = default;

        mixin(
            state_type state,
            iterator_type current)
        :
            base_type{otext_cursor{std::move(state), std::move(current)}}
        {}

        mixin(
            const post_increment_proxy &p)
        :
            base_type{p.self}
        {}

        using base_type::base_type;

        const state_type& state() const noexcept {
            return this->get().state();
        }

        const iterator_type& base() const noexcept {
            return this->get().current;
        }

        bool error_occurred() const noexcept {
            return this->get().error_occurred();
        }

        encode_status get_error() const noexcept {
            return this->get().get_error();
        }
    };

    otext_cursor() = default;

    otext_cursor(
        state_type state,
        iterator_type current)
    :
        base_type{std::move(state)},
        current(std::move(current)),
        es{encode_status::no_error}
    {}

    const state_type& state() const noexcept {
        return base_type::get();
    }
    state_type& state() noexcept {
        return base_type::get();
    }

    bool error_occurred() const noexcept {
        return text::error_occurred(es);
    }

    encode_status get_error() const noexcept {
        return es;
    }

    void write(const state_transition_type &stt) {
        // Preserve current so that it remains unmutated in the event that
        // encode_state_transition() throws an exception (unless it is an
        // actual output iterator).
        auto preserved_current = make_iterator_preserve(current);
        int encoded_code_units = 0;
        es = encoding_type::encode_state_transition(
            state(), preserved_current.get(), stt, encoded_code_units);
        preserved_current.update();
        if (error_occurred()) {
            if (std::is_base_of<
                    text_permissive_error_policy,
                    error_policy
                >::value)
            {
                // Permissive error policy: ignore the error as there is no
                // reasonable substitute state transition to encode.
            } else {
                // Strict error policy: throw an exception.
                throw text_encode_error{es};
            }
        }
    }

    void write(const character_type_t<encoding_type> &value) {
        // Preserve current so that it remains unmutated in the event that
        // encode_state_transition() throws an exception (unless it is an
        // actual output iterator).
        auto preserved_current = make_iterator_preserve(current);
        int encoded_code_units = 0;
        es = encoding_type::encode(
            state(), preserved_current.get(), value, encoded_code_units);
        preserved_current.update();
        if (error_occurred()) {
            if (std::is_base_of<
                    text_permissive_error_policy,
                    error_policy
                >::value)
            {
                // Permissive error policy: attempt to encode the substitution
                // character.
                using CT = character_type_t<encoding_type>;
                using CST = character_set_type_t<CT>;
                CT c;
                c.set_code_point(CST::get_substitution_code_point());
                es = encoding_type::encode(
                    state(), preserved_current.get(), c, encoded_code_units);
                preserved_current.update();
            } else {
                // Strict error policy: throw an exception.
                throw text_encode_error{es};
            }
        }
    }

    void next() noexcept
    {}

    auto post_increment() noexcept {
        return post_increment_proxy{*this};
    }

private:
    iterator_type current;
    encode_status es = encode_status::no_error;
};

} // namespace text_detail


/*
 * otext_iterator
 */
template<
    TextEncoding ET,
    CodeUnitOutputIterator<code_unit_type_t<ET>> CUIT,
    TextErrorPolicy TEP = text_default_error_policy>
using otext_iterator =
    ranges::basic_iterator<text_detail::otext_cursor<ET, CUIT, TEP>>;


/*
 * make_otext_iterator
 */
// Overload to construct an output text iterator for an explicitly specified
// encoding type and error policy from an output iterator and an explicitly
// specified initial encoding state.
template<
    TextEncoding ET,
    TextErrorPolicy TEP,
    CodeUnitOutputIterator<code_unit_type_t<ET>> IT>
auto make_otext_iterator(
    typename ET::state_type state,
    IT out)
{
    return otext_iterator<ET, IT, TEP>{std::move(state), std::move(out)};
}

// Overload to construct an output text iterator for an explicitly specified
// encoding type and an implicitly assumed error policy from an output iterator
// and an explicitly specified initial encoding state.
template<
    TextEncoding ET,
    CodeUnitOutputIterator<code_unit_type_t<ET>> IT>
auto make_otext_iterator(
    typename ET::state_type state,
    IT out)
{
    return otext_iterator<ET, IT>{std::move(state), std::move(out)};
}

// Overload to construct an output text iterator for an explicitly specified
// encoding type and error policy from an output iterator and an implicit
// initial encoding state.
template<
    TextEncoding ET,
    TextErrorPolicy TEP,
    CodeUnitOutputIterator<code_unit_type_t<ET>> IT>
auto make_otext_iterator(
    IT out)
{
    return otext_iterator<ET, IT, TEP>{ET::initial_state(), std::move(out)};
}

// Overload to construct an output text iterator for an explicitly specified
// encoding type and an implicitly assumed error policy from an output iterator
// and an implicit initial encoding state.
template<
    TextEncoding ET,
    CodeUnitOutputIterator<code_unit_type_t<ET>> IT>
auto make_otext_iterator(
    IT out)
{
    return otext_iterator<ET, IT>{ET::initial_state(), std::move(out)};
}


} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_OTEXT_ITERATOR_HPP
