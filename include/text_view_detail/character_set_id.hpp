// Copyright (c) 2016, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef TEXT_VIEW_CHARACTER_SET_ID_HPP // {
#define TEXT_VIEW_CHARACTER_SET_ID_HPP


namespace std {
namespace experimental {
inline namespace text {


class character_set_info;

/*
 * Character set ID
 * character_set_id is modeled after std::locale::id.  All values of this class
 * type are constructed by get_character_set_info().
 */
class character_set_id {
public:
    character_set_id() = delete;

    friend bool operator==(
        character_set_id l,
        character_set_id r) noexcept
    {
        return l.id == r.id;
    }
    friend bool operator!=(
        character_set_id l,
        character_set_id r) noexcept
    {
        return !(l == r);
    }

    friend bool operator<(
        character_set_id l,
        character_set_id r) noexcept
    {
        return l.id < r.id;
    }
    friend bool operator>(
        character_set_id l,
        character_set_id r) noexcept
    {
        return r < l;
    }
    friend bool operator<=(
        character_set_id l,
        character_set_id r) noexcept
    {
        return !(r < l);
    }
    friend bool operator>=(
        character_set_id l,
        character_set_id r) noexcept
    {
        return !(l < r);
    }

private:
    template<typename T>
    friend const character_set_info&
    get_character_set_info();

    friend const character_set_info&
    get_character_set_info(character_set_id id);

    character_set_id(int id) : id{id} {}

    int id;
};


} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_CHARACTER_SET_ID_HPP
