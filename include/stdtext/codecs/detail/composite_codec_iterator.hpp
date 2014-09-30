#if !defined(STDTEXT_CODECS_COMPOSITE_CODEC_ITERATOR_HPP) // {
#define STDTEXT_CODECS_COMPOSITE_CODEC_ITERATOR_HPP


#include <stdtext/concepts.hpp>
#include <iterator>


namespace std {
namespace experimental {
namespace text {
namespace detail {


template<typename C, typename CUIT, typename CUT>
struct composite_codec_encode_iterator;

template<Codec C, Code_unit_iterator CUIT, Code_unit CUT>
requires origin::Output_iterator<CUIT, typename C::code_unit_type>()
struct composite_codec_encode_iterator<C, CUIT, CUT>
    : public std::iterator<
                 std::output_iterator_tag,
                 CUT,
                 origin::Difference_type<CUIT>,
                 CUT*,
                 CUT&>
{
    using codec_type = C;
    using iterator = CUIT;
    using iterator_category = typename composite_codec_encode_iterator::iterator_category;
    using value_type = typename composite_codec_encode_iterator::value_type;
    using reference = typename composite_codec_encode_iterator::reference;
    using pointer = typename composite_codec_encode_iterator::pointer;
    using difference_type = typename composite_codec_encode_iterator::difference_type;

    composite_codec_encode_iterator(
        typename codec_type::state_type &state,
        iterator &current,
        int &total_encoded_code_units)
    :
        state(state),
        current{current},
        total_encoded_code_units{total_encoded_code_units}
    {}

    iterator base() const {
        return current;
    }

    composite_codec_encode_iterator& operator*() {
        return *this;
    }

    bool operator==(const composite_codec_encode_iterator& other) const {
        return current == other.current;
    }
    bool operator!=(const composite_codec_encode_iterator& other) const {
        return current != other.current;
    }

    composite_codec_encode_iterator& operator++() {
        return *this;
    }
    composite_codec_encode_iterator& operator++(int) {
        return *this;
    }

    composite_codec_encode_iterator& operator=(const value_type &value) {
        const typename codec_type::character_type c{value};
        int encoded_code_units = 0;
        try {
            codec_type::encode(state, current, c, encoded_code_units);
        } catch(...) {
            total_encoded_code_units += encoded_code_units;
            throw;
        }
        return *this;
    }

protected:
    typename codec_type::state_type &state;
    iterator &current;
    int &total_encoded_code_units;
};


template<typename C, typename CUIT, typename CUT>
struct composite_codec_rencode_iterator;

template<Codec C, Code_unit_iterator CUIT, typename CUT>
requires origin::Output_iterator<CUIT, typename C::code_unit_type>()
struct composite_codec_rencode_iterator<C, CUIT, CUT>
    : public std::iterator<
                 std::output_iterator_tag,
                 CUT,
                 origin::Difference_type<CUIT>,
                 CUT*,
                 CUT&>
{
    using codec_type = C;
    using iterator = CUIT;
    using iterator_category = typename composite_codec_rencode_iterator::iterator_category;
    using value_type = typename composite_codec_rencode_iterator::value_type;
    using reference = typename composite_codec_rencode_iterator::reference;
    using pointer = typename composite_codec_rencode_iterator::pointer;
    using difference_type = typename composite_codec_rencode_iterator::difference_type;

    composite_codec_rencode_iterator(
        typename codec_type::state_type &state,
        iterator &current,
        int &total_rencoded_code_units)
    :
        state(state),
        current{current},
        total_encoded_code_units{total_encoded_code_units}
    {}

    iterator base() const {
        return current;
    }

    composite_codec_rencode_iterator& operator*() {
        return *this;
    }

    bool operator==(const composite_codec_rencode_iterator& other) const {
        return current == other.current;
    }
    bool operator!=(const composite_codec_rencode_iterator& other) const {
        return current != other.current;
    }

    composite_codec_rencode_iterator& operator++() {
        return *this;
    }
    composite_codec_rencode_iterator& operator++(int) {
        return *this;
    }

    composite_codec_rencode_iterator& operator=(const value_type &value) {
        const typename codec_type::character_type c{value};
        int encoded_code_units = 0;
        try {
            codec_type::rencode(state, current, value, encoded_code_units);
        } catch(...) {
            total_encoded_code_units += encoded_code_units;
            throw;
        }
        return *this;
    }

protected:
    typename codec_type::state_type &state;
    iterator &current;
    int &total_encoded_code_units;
};

template<typename C, typename CUIT, typename CUT>
struct composite_codec_decode_iterator;

template<Codec C, Code_unit_iterator CUIT, Code_unit CUT>
requires origin::Input_iterator<CUIT>()
struct composite_codec_decode_iterator<C, CUIT, CUT>
    : public std::iterator<
                 std::input_iterator_tag,
                 CUT,
                 origin::Difference_type<CUIT>,
                 const CUT*,
                 const CUT&>
{
    using codec_type = C;
    using iterator = CUIT;
    using iterator_category = typename composite_codec_decode_iterator::iterator_category;
    using value_type = typename composite_codec_decode_iterator::value_type;
    using reference = typename composite_codec_decode_iterator::reference;
    using pointer = typename composite_codec_decode_iterator::pointer;
    using difference_type = typename composite_codec_decode_iterator::difference_type;

    composite_codec_decode_iterator(
        typename codec_type::state_type &state,
        iterator &current, iterator end,
        int &total_decoded_code_units)
    :   
        state(state),
        current{current}, end{end},
        total_decoded_code_units{total_decoded_code_units}
    {}   

    iterator base() const {
        return current;
    }

    reference operator*() const {
        typename codec_type::character_type c;
        int decoded_code_units = 0;
        try {
            codec_type::decode(state, current, end, c, decoded_code_units);
        } catch(...) {
            total_decoded_code_units += decoded_code_units;
            throw;
        }
        value = c;
        return value;
    }

    bool operator==(const composite_codec_decode_iterator& other) const {
        return current == other.current;
    }
    bool operator!=(const composite_codec_decode_iterator& other) const {
        return !(*this == other);
    }

    composite_codec_decode_iterator& operator++() {
        // Nothing to do, increment occurs during dereference.
        return *this;
    }
    composite_codec_decode_iterator operator++(int) {
        composite_codec_decode_iterator it{*this};
        ++*this;
        return it;
    }

protected:
    typename codec_type::state_type &state;
    iterator &current, end;
    int &total_decoded_code_units;
    mutable value_type value;
};

template<typename C, typename CUIT, typename CUT>
struct composite_codec_rdecode_iterator;

template<Codec C, Code_unit_iterator CUIT, Code_unit CUT>
requires origin::Input_iterator<CUIT>()
struct composite_codec_rdecode_iterator<C, CUIT, CUT>
    : public std::iterator<
                 std::input_iterator_tag,
                 CUT,
                 origin::Difference_type<CUIT>,
                 const CUT*,
                 const CUT&>
{
    using codec_type = C;
    using iterator = CUIT;
    using iterator_category = typename composite_codec_rdecode_iterator::iterator_category;
    using value_type = typename composite_codec_rdecode_iterator::value_type;
    using reference = typename composite_codec_rdecode_iterator::reference;
    using pointer = typename composite_codec_rdecode_iterator::pointer;
    using difference_type = typename composite_codec_rdecode_iterator::difference_type;

    composite_codec_rdecode_iterator(
        typename codec_type::state_type &state,
        iterator &current, iterator end,
        int &total_decoded_code_units)
    :   
        state(state),
        current{current}, end{end},
        total_decoded_code_units{total_decoded_code_units}
    {}   

    iterator base() const {
        return current;
    }

    reference operator*() const {
        typename codec_type::character_type c;
        int decoded_code_units = 0;
        try {
            codec_type::rdecode(state, current, end, c, decoded_code_units);
        } catch(...) {
            total_decoded_code_units += decoded_code_units;
            throw;
        }
        value = c;
        return value;
    }

    bool operator==(const composite_codec_rdecode_iterator& other) const {
        return current == other.current;
    }
    bool operator!=(const composite_codec_rdecode_iterator& other) const {
        return !(*this == other);
    }

    composite_codec_rdecode_iterator& operator++() {
        // Nothing to do, increment occurs during dereference.
        return *this;
    }
    composite_codec_rdecode_iterator operator++(int) {
        composite_codec_rdecode_iterator it{*this};
        ++*this;
        return it;
    }

protected:
    typename codec_type::state_type &state;
    iterator &current, end;
    int &total_decoded_code_units;
    mutable value_type value;
};


} // namespace detail
} // namespace text
} // namespace experimental
} // namespace std


#endif // } STDTEXT_CODECS_COMPOSITE_CODEC_ITERATOR_HPP
