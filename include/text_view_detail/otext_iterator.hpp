// Copyright (c) 2016, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef TEXT_VIEW_TEXT_OITERATOR_HPP // {
#define TEXT_VIEW_TEXT_OITERATOR_HPP


#include <experimental/ranges/iterator>
#include <text_view_detail/concepts.hpp>


namespace std {
namespace experimental {
inline namespace text {


template<TextEncoding ET, CodeUnitOutputIterator<code_unit_type_t<ET>> CUIT>
class otext_iterator
    : private ET::state_type
{
public:
    using encoding_type = ET;
    using state_type = typename ET::state_type;
    using state_transition_type = typename ET::state_transition_type;
    using iterator = CUIT;
    using iterator_category = std::output_iterator_tag;
    using value_type = character_type_t<encoding_type>;
    using reference = value_type&;
    using pointer = value_type*;
    using difference_type = ranges::difference_type_t<iterator>;

    otext_iterator() = default;

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

    const state_type& state() const noexcept {
        return *this;
    }
    state_type& state() noexcept {
        return *this;
    }

    iterator base() const {
        return current;
    }

    otext_iterator& operator*() noexcept {
        return *this;
    }

    otext_iterator& operator++() noexcept {
        return *this;
    }
    otext_iterator& operator++(int) noexcept {
        return *this;
    }

    otext_iterator& operator=(
        const state_transition_type &stt)
    {
        iterator tmp{current};
        int encoded_code_units = 0;
        encoding_type::encode_state_transition(state(), tmp, stt,
                                               encoded_code_units);
        current = tmp;
        return *this;
    }

    otext_iterator& operator=(
        const character_type_t<encoding_type> &value)
    {
        iterator tmp{current};
        int encoded_code_units = 0;
        encoding_type::encode(state(), tmp, value, encoded_code_units);
        current = tmp;
        return *this;
    }

protected:
    iterator current;
};


/*
 * make_otext_iterator
 */
// Overload to construct an output text iterator from an output iterator and
// and an explicitly specified initial encoding state.
template<TextEncoding ET, CodeUnitOutputIterator<code_unit_type_t<ET>> IT>
auto make_otext_iterator(
    typename ET::state_type state,
    IT out)
{
    return otext_iterator<ET, IT>{state, out};
}

// Overload to construct an output text iterator from an output iterator and
// and an implicit initial encoding state.
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
