#ifndef TEXT_VIEW_ARCHETYPES_HPP // {
#define TEXT_VIEW_ARCHETYPES_HPP


#include <text_view_detail/concepts.hpp>
#include <origin/core/concepts.hpp>
#include <origin/sequence/concepts.hpp>
#include <origin/sequence/range.hpp>


namespace std {
namespace experimental {
inline namespace text {


/*
 * Code unit archetype
 */
template<Code_unit CUT>
using code_unit_archetype_template = CUT;
using code_unit_archetype = code_unit_archetype_template<char>;


/*
 * Code point archetype
 */
template<Code_point CPT>
using code_point_archetype_template = CPT;
using code_point_archetype = code_point_archetype_template<char>;


/*
 * Character set archetype
 */
template<Code_point CPT>
struct character_set_archetype_template
{
    using code_point_type = CPT;
};
using character_set_archetype =
          character_set_archetype_template<code_point_archetype>;


/*
 * Character archetype
 */
template<Character_set CST>
class character_archetype_template
{
public:
    using character_set_type = CST;
    using code_point_type = typename CST::code_point_type;

    character_archetype_template() : cp{} {}
    character_archetype_template(code_point_type cp) : cp(cp) {}

    bool operator==(const character_archetype_template& other) const noexcept;
    bool operator!=(const character_archetype_template& other) const noexcept;

    void set_code_point(code_point_type cp) noexcept;
    code_point_type get_code_point() const noexcept;

    static character_set_id get_character_set_id() noexcept;

private:
    code_point_type cp;
};
using character_archetype =
          character_archetype_template<character_set_archetype>;


/*
 * Code unit iterator archetype
 */
template<Code_unit CUT>
using code_unit_iterator_archetype_template = CUT*;
using code_unit_iterator_archetype =
          code_unit_iterator_archetype_template<code_unit_archetype>;


/*
 * Codec state archetype
 */
struct codec_state_archetype {};


/*
 * Codec state transition archetype
 */
struct codec_state_transition_archetype {};


/*
 * Codec archetype
 */
template <
    Codec_state CST,
    Codec_state_transition CSTT,
    Code_unit CUT,
    Character C,
    int MinCodeUnits = 1,
    int MaxCodeUnits = 1>
class codec_archetype_template {
public:
    using state_type = CST;
    using state_transition_type = CSTT;
    using code_unit_type = CUT;
    using character_type = C;
    static constexpr int min_code_units = MinCodeUnits;
    static constexpr int max_code_units = MaxCodeUnits;

    template<Code_unit_iterator CUIT>
    requires origin::Output_iterator<CUIT, code_unit_type>()
    static void encode_state_transition(
        state_type &state,
        CUIT &out,
        const state_transition_type &stt,
        int &encoded_code_units);

    template<Code_unit_iterator CUIT>
    requires origin::Output_iterator<CUIT, code_unit_type>()
    static void encode(
        state_type &state,
        CUIT &out,
        character_type c,
        int &encoded_code_units);

    template<Code_unit_iterator CUIT, typename CUST>
    requires origin::Input_iterator<CUIT>()
          && origin::Convertible<origin::Value_type<CUIT>, code_unit_type>()
          && origin::Sentinel<CUST, CUIT>()
    static bool decode(
        state_type &state,
        CUIT &in_next,
        CUST in_end,
        character_type &c,
        int &decoded_code_units);

    template<Code_unit_iterator CUIT, typename CUST>
    requires origin::Input_iterator<CUIT>()
          && origin::Convertible<origin::Value_type<CUIT>, code_unit_type>()
          && origin::Sentinel<CUST, CUIT>()
    static bool rdecode(
        state_type &state,
        CUIT &in_next,
        CUST in_end,
        character_type &c,
        int &decoded_code_units);
};
using codec_archetype = codec_archetype_template<
                            codec_state_archetype,
                            codec_state_transition_archetype,
                            code_unit_archetype,
                            character_archetype>;


/*
 * Encoding archetype
 */
template<Codec CT>
struct encoding_archetype_template
{
    using codec_type = CT;
    static const typename codec_type::state_type& initial_state() noexcept;
};
using encoding_archetype = encoding_archetype_template<codec_archetype>;


/*
 * Text iterator archetype
 */
template<Encoding ET, Code_unit_iterator CUIT>
class text_iterator_archetype_template {
public:
    using encoding_type = ET;
    using state_type = typename ET::codec_type::state_type;
    using iterator = CUIT;
    using iterator_category = origin::Iterator_category<iterator>;
    using value_type = typename ET::codec_type::character_type;
    using reference = typename ET::codec_type::character_type&;
    using pointer = typename ET::codec_type::character_type*;
    using difference_type = origin::Difference_type<iterator>;

    text_iterator_archetype_template();
    text_iterator_archetype_template(iterator, iterator);
    text_iterator_archetype_template(state_type, iterator, iterator);
    const state_type& state() const;
    state_type& state();
    iterator base() const;
    iterator begin() const;
    iterator end() const;
    reference operator*() const;
    pointer operator->() const;
    bool operator==(const text_iterator_archetype_template& other) const;
    bool operator!=(const text_iterator_archetype_template& other) const;
    bool operator<(const text_iterator_archetype_template& other) const;
    bool operator>(const text_iterator_archetype_template& other) const;
    bool operator<=(const text_iterator_archetype_template& other) const;
    bool operator>=(const text_iterator_archetype_template& other) const;
    text_iterator_archetype_template& operator++();
    text_iterator_archetype_template operator++(int);
    text_iterator_archetype_template& operator--();
    text_iterator_archetype_template operator--(int);
    text_iterator_archetype_template& operator+=(difference_type n);
    text_iterator_archetype_template operator+(difference_type n) const;
    text_iterator_archetype_template& operator-=(difference_type n);
    text_iterator_archetype_template operator-(difference_type n) const;
    difference_type operator-(text_iterator_archetype_template it) const;
    value_type operator[](difference_type n) const;
};
template<Encoding ET, Code_unit_iterator CUIT>
text_iterator_archetype_template<ET, CUIT> operator+(
    origin::Difference_type<CUIT> n,
    text_iterator_archetype_template<ET, CUIT> it);
using text_iterator_archetype = text_iterator_archetype_template<
                                    encoding_archetype,
                                    code_unit_iterator_archetype>;


/*
 * Text view archetype
 */
template<Encoding ET, origin::Input_range RT>
class text_view_archetype_template {
public:
    using range_type = RT;
    using encoding_type = ET;
    using state_type = typename ET::codec_type::state_type;
    using code_unit_iterator = origin::Iterator_type<RT>;
    using iterator = text_iterator_archetype_template<ET, code_unit_iterator>;

    text_view_archetype_template(const state_type &initial_state, RT r);
    const RT& base() const;
    RT& base();
    const state_type& initial_state() const;
    state_type& initial_state();
    iterator begin() const;
    iterator end() const;
};
using text_view_archetype = text_view_archetype_template<
                                encoding_archetype,
                                origin::bounded_range<code_unit_iterator_archetype>>;


} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_ARCHETYPES_HPP
