// Copyright (c) 2016, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef TEXT_VIEW_TEXT_OITERATOR_HPP // {
#define TEXT_VIEW_TEXT_OITERATOR_HPP


#include <experimental/ranges/iterator>
#include <text_view_detail/concepts.hpp>
#include <text_view_detail/default_encoding.hpp>
#include <text_view_detail/subobject.hpp>


namespace std {
namespace experimental {
inline namespace text {


namespace text_detail {

template<TextEncoding ET, CodeUnitOutputIterator<code_unit_type_t<ET>> CUIT>
class otext_iterator_mixin;

template<TextEncoding ET, CodeUnitOutputIterator<code_unit_type_t<ET>> CUIT>
class otext_cursor
    : private subobject<typename ET::state_type>
{
    using base_type = subobject<typename ET::state_type>;
    using encoding_type = ET;
    using iterator_type = CUIT;
    using state_type = typename ET::state_type;
    using state_transition_type = typename ET::state_transition_type;

public:
    using mixin = otext_iterator_mixin<ET, CUIT>;

    otext_cursor() = default;

    otext_cursor(
        state_type state,
        iterator_type current)
    :
        base_type{state},
        current(current)
    {}

    const state_type& state() const noexcept {
        return base_type::get();
    }
    state_type& state() noexcept {
        return base_type::get();
    }

    const iterator_type& base() const noexcept {
        return current;
    }
    iterator_type& base() noexcept {
        return current;
    }

    void write(const state_transition_type &stt) {
        iterator_type tmp{current};
        int encoded_code_units = 0;
        encoding_type::encode_state_transition(state(), tmp, stt,
                                               encoded_code_units);
        current = tmp;
    }

    void write(const character_type_t<encoding_type> &value) {
        iterator_type tmp{current};
        int encoded_code_units = 0;
        encoding_type::encode(state(), tmp, value, encoded_code_units);
        current = tmp;
    }

private:
    iterator_type current;
};


template<TextEncoding ET, CodeUnitOutputIterator<code_unit_type_t<ET>> CUIT>
class otext_iterator_mixin
    : protected ranges::basic_mixin<otext_cursor<ET, CUIT>>
{
    using iterator_type = CUIT;
    using cursor_type = otext_cursor<ET, CUIT>;
    using base_type = ranges::basic_mixin<cursor_type>;

public:
    using encoding_type = ET;
    using state_type = typename ET::state_type;
    using state_transition_type = typename ET::state_transition_type;

    otext_iterator_mixin() = default;

    otext_iterator_mixin(
        state_type state,
        iterator_type current)
    :
        base_type{cursor_type{state, current}}
    {}

    const state_type& state() const noexcept {
        return this->get().state();
    }

    const iterator_type& base() const noexcept {
        return this->get().base();
    }
};

} // namespace text_detail


/*
 * otext_iterator
 */
template<TextEncoding ET, CodeUnitOutputIterator<code_unit_type_t<ET>> CUIT>
using otext_iterator =
    ranges::basic_iterator<text_detail::otext_cursor<ET, CUIT>>;


/*
 * make_otext_iterator
 */
// Overload to construct an output text iterator for an explicitly specified
// encoding from an output iterator and and an explicitly specified initial
// encoding state.
template<TextEncoding ET, CodeUnitOutputIterator<code_unit_type_t<ET>> IT>
auto make_otext_iterator(
    typename ET::state_type state,
    IT out)
{
    return otext_iterator<ET, IT>{state, out};
}

// Overload to construct an output text iterator for an explicitly specified
// encoding from an output iterator and and an implicit initial encoding state.
template<TextEncoding ET, CodeUnitOutputIterator<code_unit_type_t<ET>> IT>
auto make_otext_iterator(
    IT out)
{
    return otext_iterator<ET, IT>{ET::initial_state(), out};
}


} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_OTEXT_ITERATOR_HPP
