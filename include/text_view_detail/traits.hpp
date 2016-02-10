// Copyright (c) 2015, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef TEXT_VIEW_TRAITS_HPP // {
#define TEXT_VIEW_TRAITS_HPP


#include <unordered_map>
#include <origin/core/traits.hpp>
#include <origin/algorithm/concepts.hpp>
#include <origin/range/range.hpp>


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
        character_set_id r)
    {
        return l.id == r.id;
    }
    friend bool operator!=(
        character_set_id l,
        character_set_id r)
    {
        return !(l == r);
    }

    friend bool operator<(
        character_set_id l,
        character_set_id r)
    {
        return l.id < r.id;
    }
    friend bool operator>(
        character_set_id l,
        character_set_id r)
    {
        return r < l;
    }
    friend bool operator<=(
        character_set_id l,
        character_set_id r)
    {
        return !(r < l);
    }
    friend bool operator>=(
        character_set_id l,
        character_set_id r)
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

namespace text_detail {
inline int
get_next_character_set_id() noexcept {
    static int next_id = 0;
    return ++next_id;
}
} // namespace text_detail


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
template<typename CST>
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

template<typename CST>
inline
character_set_id
get_character_set_id() {
    return get_character_set_info<CST>().get_id();
}


/*
 * Associated code unit type helper
 */
namespace text_detail {
template<typename T>
struct get_code_unit_type_t;

template<typename T>
requires requires () { typename T::code_unit_type; }
struct get_code_unit_type_t<T> {
    using type = typename T::code_unit_type;
};

template<typename T>
using code_unit_type_t =
          typename get_code_unit_type_t<origin::Strip<T>>::type;
} // namespace text_detail

template<typename T>
using code_unit_type_t = text_detail::code_unit_type_t<T>;


/*
 * Associated code point type helper
 */
namespace text_detail {
template<typename T>
struct get_code_point_type_t;

template<typename T>
requires requires () { typename T::code_point_type; }
struct get_code_point_type_t<T> {
    using type = typename T::code_point_type;
};

template<typename T>
using code_point_type_t =
          typename get_code_point_type_t<origin::Strip<T>>::type;
} // namespace text_detail

template<typename T>
using code_point_type_t = text_detail::code_point_type_t<T>;


/*
 * Associated character set type helper
 */
namespace text_detail {
template<typename T>
struct get_character_set_type_t;

template<typename T>
requires requires () { typename T::character_set_type; }
struct get_character_set_type_t<T> {
    using type = typename T::character_set_type;
};

template<typename T>
using character_set_type_t =
          typename get_character_set_type_t<origin::Strip<T>>::type;
} // namespace text_detail

template<typename T>
using character_set_type_t = text_detail::character_set_type_t<T>;


/*
 * Associated character type helper
 */
namespace text_detail {
template<typename T>
struct get_character_type_t;

template<typename T>
requires requires () { typename T::character_type; }
struct get_character_type_t<T> {
    using type = typename T::character_type;
};

template<typename T>
using character_type_t =
          typename get_character_type_t<origin::Strip<T>>::type;
} // namespace text_detail

template<typename T>
using character_type_t = text_detail::character_type_t<T>;


/*
 * Associated encoding type helper
 */
namespace text_detail {
template<typename T>
struct get_encoding_type_t;

template<typename T>
requires requires () { typename T::encoding_type; }
struct get_encoding_type_t<T> {
    using type = typename T::encoding_type;
};

template<typename T>
using encoding_type_t = typename get_encoding_type_t<origin::Strip<T>>::type;
} // namespace text_detail

template<typename T>
using encoding_type_t = text_detail::encoding_type_t<T>;


} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_TRAITS_HPP
