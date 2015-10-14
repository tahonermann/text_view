// Copyright (c) 2015, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef TEXT_VIEW_CHARACTER_HPP // {
#define TEXT_VIEW_CHARACTER_HPP


#include <text_view_detail/concepts.hpp>
#include <text_view_detail/charsets/any_charset.hpp>


namespace std {
namespace experimental {
inline namespace text {


template<Character_set CST>
class character {
public:
    using character_set_type = CST;
    using code_point_type = typename character_set_type::code_point_type;

    character() noexcept = default;
    explicit character(code_point_type code_point) noexcept
        : code_point{code_point} {}

    friend bool operator==(
        const character &l,
        const character &r) noexcept
    {
        return l.code_point == r.code_point;
    }
    friend bool operator!=(
        const character &l,
        const character &r) noexcept
    {
        return !(l == r);
    }

    void set_code_point(code_point_type code_point) noexcept {
        this->code_point = code_point;
    }
    code_point_type get_code_point() const noexcept {
        return code_point;
    }

    static character_set_id get_character_set_id() noexcept {
        return std::experimental::text::get_character_set_id<CST>();
    }

private:
    code_point_type code_point = {};
};


/*
 * character specialization for the any_character_set character set.
 */
template<>
class character<any_character_set> {
public:
    using character_set_type = any_character_set;
    using code_point_type = typename character_set_type::code_point_type;

    character() noexcept = default;
    explicit character(code_point_type code_point) noexcept
        : cs_id{std::experimental::text::get_character_set_id<any_character_set>()}, code_point{code_point} {}
    character(character_set_id cs_id, code_point_type code_point) noexcept
        : cs_id{cs_id}, code_point{code_point} {}

    friend bool operator==(
        const character &l,
        const character &r) noexcept
    {
        return l.cs_id == r.cs_id
            && l.code_point == r.code_point;
    }
    friend bool operator!=(
        const character &l,
        const character &r) noexcept
    {
        return !(l == r);
    }

    void set_code_point(code_point_type code_point) noexcept {
        this->code_point = code_point;
    }
    code_point_type get_code_point() const noexcept {
        return code_point;
    }

    void set_character_set_id(character_set_id new_cs_id) noexcept {
        cs_id = new_cs_id;
    }
    character_set_id get_character_set_id() const noexcept {
        return cs_id;
    }

private:
    character_set_id cs_id = std::experimental::text::get_character_set_id<any_character_set>();
    code_point_type code_point = {};
};

template<Character_set CST>
bool operator==(
    const character<any_character_set> &c1,
    const character<CST> &c2)
{
    return c1.get_character_set_id() == c2.get_character_set_id()
        && c1.get_code_point() == c2.get_code_point();
}

template<Character_set CST>
bool operator==(
    const character<CST> &c1,
    const character<any_character_set> &c2)
{
    return c1.get_character_set_id() == c2.get_character_set_id()
        && c1.get_code_point() == c2.get_code_point();
}

template<Character_set CST>
bool operator!=(
    const character<any_character_set> &c1,
    const character<CST> &c2)
{
    return !(c1 == c2);
}

template<Character_set CST>
bool operator!=(
    const character<CST> &c1,
    const character<any_character_set> &c2)
{
    return !(c1 == c2);
}


} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_CHARACTER_HPP
