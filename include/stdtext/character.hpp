#ifndef STDTEXT_CHARACTER_HPP // {
#define STDTEXT_CHARACTER_HPP


#include <stdtext/concepts.hpp>


namespace std {
namespace experimental {
namespace text {


template<Character_set CST>
struct character {
    using character_set_type = CST;
    using code_point_type = typename character_set_type::code_point_type;

    character() noexcept {}
    explicit character(code_point_type code_point) noexcept
        : code_point{code_point} {}

    character& operator=(code_point_type code_point) noexcept {
        this->code_point = code_point;
        return *this;
    }

    operator code_point_type() const noexcept {
        return code_point;
    }

private:
    code_point_type code_point;
};

template<Character_set CST>
bool operator==(
    const character<CST> &c1,
    const character<CST> &c2)
{
    typename CST::code_point_type cp1{c1};
    typename CST::code_point_type cp2{c2};
    return cp1 == cp2;
}

template<Character_set CST>
bool operator!=(
    const character<CST> &c1,
    const character<CST> &c2)
{
    typename CST::code_point_type cp1{c1};
    typename CST::code_point_type cp2{c2};
    return cp1 != cp2;
}


} // namespace text
} // namespace experimental
} // namespace std


#endif // } STDTEXT_CHARACTER_HPP
