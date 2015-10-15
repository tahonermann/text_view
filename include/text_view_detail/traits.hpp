// Copyright (c) 2015, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef TEXT_VIEW_TRAITS_HPP // {
#define TEXT_VIEW_TRAITS_HPP


#include <unordered_map>
#include <origin/core/traits.hpp>
#include <origin/sequence/concepts.hpp>
#include <origin/sequence/range.hpp>


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
    get_character_set_info() noexcept;

    friend const character_set_info&
    get_character_set_info(character_set_id id);

    character_set_id(int id) noexcept : id{id} {}

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
    friend const character_set_info& get_character_set_info() noexcept;

    character_set_info(
        character_set_id id,
        const char *name) noexcept
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
get_character_set_info() noexcept {
    static int id = text_detail::get_next_character_set_id();
    static character_set_info csi{
               character_set_id{id},
               CST::get_name()};
    static character_set_info *&csi_ptr_ref =
            text_detail::get_emplaced_character_set_info(id);
    static character_set_info *csi_ptr = csi_ptr_ref = &csi;
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
 * Associated code point type helper
 */
namespace text_detail {
template<typename T>
struct get_code_point_type_of;

template<typename T>
requires requires () { typename T::code_point_type; }
struct get_code_point_type_of<T> {
    using type = typename T::code_point_type;
};

template<typename T>
using code_point_type_of =
          typename get_code_point_type_of<origin::Strip<T>>::type;
} // namespace text_detail

template<typename T>
using code_point_type_of = text_detail::code_point_type_of<T>;


/*
 * Associated character set type helper
 */
namespace text_detail {
template<typename T>
struct get_character_set_type_of;

template<typename T>
requires requires () { typename T::character_set_type; }
struct get_character_set_type_of<T> {
    using type = typename T::character_set_type;
};

template<typename T>
using character_set_type_of =
          typename get_character_set_type_of<origin::Strip<T>>::type;
} // namespace text_detail

template<typename T>
using character_set_type_of = text_detail::character_set_type_of<T>;


/*
 * Associated encoding type helper
 */
namespace text_detail {
template<typename T>
struct get_encoding_type_of;

// String and character literals have statically known execution character
// encodings (though the literal code units need not form validly encoded
// text).  The underlying types of string and character literals
// (const char[N], char, wchar_t, int (for multicharacter literals), etc...)
// do not unambiguously imply a particular encoding since these types can be
// used as the underlying storage for arbitrary data).  It is not currently
// possible to differentiate between a string or character literal and
// variables declared using the underlying type.  If it were, then it would
// be possible to infer an encoding for string and character literals, but
// not for variables of the underlying types.  Since it isn't, a choice
// must be made to either infer encodings based on these underlying types
// (allowing the user to override the inference), or require the user to
// always specify an encoding.  For convenience, and since the underlying
// types are frequently used for storing text that does use the same
// encodings as string literals, types corresponding to such string literals
// (including their decayed types) are assumed by encoding_type_of to have
// an associated encoding matching a string literal of that type.  Note
// that this assumption is not extended to the underlying types of
// character literals by character_set_type_of since values of these types
// may not be capable of storing all code points valid for the encoded
// character sets.
template<>
struct get_encoding_type_of<char*>;
template<>
struct get_encoding_type_of<wchar_t*>;
template<>
struct get_encoding_type_of<char16_t*>;
template<>
struct get_encoding_type_of<char32_t*>;

template<typename T>
struct get_encoding_type_of<const T*> {
    using type = typename get_encoding_type_of<T*>::type;
};

template<typename T>
struct get_encoding_type_of<T[]> {
    using type = typename get_encoding_type_of<T*>::type;
};

template<typename T, unsigned N>
struct get_encoding_type_of<T[N]> {
    using type = typename get_encoding_type_of<T*>::type;
};

template<typename T>
requires requires () { typename T::encoding_type; }
struct get_encoding_type_of<T> {
    using type = typename T::encoding_type;
};

template<typename T>
using encoding_type_of = typename get_encoding_type_of<origin::Strip<T>>::type;
} // namespace text_detail

template<typename T>
using encoding_type_of = text_detail::encoding_type_of<T>;


} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_TRAITS_HPP
