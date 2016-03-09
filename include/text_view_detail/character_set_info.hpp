// Copyright (c) 2016, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef TEXT_VIEW_CHARACTER_SET_INFO_HPP // {
#define TEXT_VIEW_CHARACTER_SET_INFO_HPP


#include <unordered_map>
#include <text_view_detail/character_set_id.hpp>
#include <text_view_detail/concepts.hpp>


namespace std {
namespace experimental {
inline namespace text {


/*
 * Character set info
 * character_set_info is modeled after std::locale::id.  All values of this class
 * type are constructed by get_character_set_info().
 */
class character_set_info {
public:
    character_set_info() = delete;

    character_set_id get_id() const noexcept {
        return id;
    }

    const char* get_name() const noexcept {
        return name;
    }

private:
    template<typename T>
    friend const character_set_info& get_character_set_info();

    character_set_info(
        character_set_id id,
        const char *name)
    :
        id(id),
        name(name)
    {}

    character_set_id id;
    const char *name;
};


namespace text_detail {

inline int
get_next_character_set_id() noexcept {
    static int next_id = 0;
    return ++next_id;
}

inline character_set_info*&
get_emplaced_character_set_info(
    int id)
{
    static std::unordered_map<int, character_set_info*> csi_map;
    return csi_map.emplace(id, nullptr).first->second;
}

} // namespace text_detail


/*
 * Character set info retrieval functions.
 */
template<CharacterSet CST>
inline
const character_set_info&
get_character_set_info() {
    static character_set_info csi{
               character_set_id{text_detail::get_next_character_set_id()},
               CST::get_name()};
    static character_set_info *csi_ptr =
           text_detail::get_emplaced_character_set_info(csi.id.id) =
               &csi;
    return *csi_ptr;
}

const character_set_info&
get_character_set_info(
    character_set_id id)
{
    return *text_detail::get_emplaced_character_set_info(id.id);
}

template<CharacterSet CST>
inline
character_set_id
get_character_set_id() {
    return get_character_set_info<CST>().get_id();
}


} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_CHARACTER_SET_INFO_HPP
