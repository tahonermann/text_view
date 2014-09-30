#if !defined(STDTEXT_CODECS_COMPOSITE_CODEC_HPP) // {
#define STDTEXT_CODECS_COMPOSITE_CODEC_HPP


#include <stdtext/concepts.hpp>
#include <stdtext/exceptions.hpp>
#include <stdtext/character.hpp>
#include <stdtext/codecs/detail/composite_codec_iterator.hpp>


namespace std {
namespace experimental {
namespace text {


template<Codec CT1, Codec CT2>
struct composite_codec {
    using codec1_type = CT1;
    using codec2_type = CT2;

    struct codec1_state_subobject
        : public codec1_type::state_type {};
    struct codec2_state_subobject
        : public codec2_type::state_type {};
    struct state_type
        : public codec1_state_subobject, public codec2_state_subobject {};

    using character_type = typename codec1_type::character_type;
    using codec1_code_unit_type = typename codec1_type::code_unit_type;
    using codec2_character_type = typename codec2_type::character_type;
    using code_unit_type = typename codec2_type::code_unit_type;

    static constexpr int min_code_units =
        codec1_type::min_code_units * codec2_type::min_code_units;
    static constexpr int max_code_units =
        codec1_type::max_code_units * codec2_type::max_code_units;

    template<Code_unit_iterator CUIT>
    requires origin::Output_iterator<CUIT, code_unit_type>()
    static void encode(
        state_type &state,
        CUIT &out,
        character_type c,
        int &encoded_code_units)
    {
        encoded_code_units = 0;

        codec2_state_subobject &codec2_state =
            static_cast<codec2_state_subobject&>(state);
        detail::composite_codec_encode_iterator<
            codec2_type, CUIT, codec1_code_unit_type>
            codec2_encode_iterator(codec2_state, out, encoded_code_units);

        int codec1_encoded_code_units = 0;
        codec1_type::encode(
            static_cast<codec1_state_subobject&>(state),
            codec2_encode_iterator,
            c, codec1_encoded_code_units);
    }

    template<Code_unit_iterator CUIT>
    requires origin::Output_iterator<CUIT, code_unit_type>()
    static void rencode(
        state_type &state,
        CUIT &out,
        character_type c,
        int &encoded_code_units)
    {
        encoded_code_units = 0;

        codec2_state_subobject &codec2_state =
            static_cast<codec2_state_subobject&>(state);
        detail::composite_codec_rencode_iterator<
            codec2_type, CUIT, codec1_code_unit_type>
            codec2_rencode_iterator(codec2_state, out, encoded_code_units);

        int codec1_encoded_code_units = 0;
        codec1_type::rencode(
            static_cast<codec1_state_subobject&>(state),
            codec2_rencode_iterator,
            c, codec1_encoded_code_units);
    }

    template<Code_unit_iterator CUIT>
    requires origin::Input_iterator<CUIT>()
          && origin::Convertible<origin::Value_type<CUIT>, code_unit_type>()
    static void decode(
        state_type &state,
        CUIT &in_next,
        CUIT in_end,
        character_type &c,
        int &decoded_code_units)
    {
        decoded_code_units = 0;

        codec2_state_subobject &codec2_state =
            static_cast<codec2_state_subobject&>(state);
        detail::composite_codec_decode_iterator<
            codec2_type, CUIT, codec1_code_unit_type>
            codec2_decode_iterator_next(
                codec2_state, in_next, in_end, decoded_code_units);
        detail::composite_codec_decode_iterator<
            codec2_type, CUIT, codec1_code_unit_type>
            codec2_decode_iterator_end(
                codec2_state, in_end, in_end, decoded_code_units);

        int codec1_decoded_code_units = 0;
        codec1_type::decode(
            static_cast<codec1_state_subobject&>(state),
            codec2_decode_iterator_next,
            codec2_decode_iterator_end,
            c, codec1_decoded_code_units);
    }

    template<Code_unit_iterator CUIT>
    requires origin::Input_iterator<CUIT>()
          && origin::Convertible<origin::Value_type<CUIT>, code_unit_type>()
    static void rdecode(
        state_type &state,
        CUIT &in_next,
        CUIT in_end,
        character_type &c,
        int &decoded_code_units)
    {
        decoded_code_units = 0;

        codec2_state_subobject &codec2_state =
            static_cast<codec2_state_subobject&>(state);
        detail::composite_codec_rdecode_iterator<
            codec2_type, CUIT, codec1_code_unit_type>
            codec2_rdecode_iterator_next(
                codec2_state, in_next, in_end, decoded_code_units);
        detail::composite_codec_rdecode_iterator<
            codec2_type, CUIT, codec1_code_unit_type>
            codec2_rdecode_iterator_end(
                codec2_state, in_end, in_end, decoded_code_units);

        int codec1_decoded_code_units = 0;
        codec1_type::rdecode(
            static_cast<codec1_state_subobject&>(state),
            codec2_rdecode_iterator_next,
            codec2_rdecode_iterator_end,
            c, codec1_decoded_code_units);
    }
};


} // namespace text
} // namespace experimental
} // namespace std


#endif // } STDTEXT_CODECS_COMPOSITE_CODEC_HPP
