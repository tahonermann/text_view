// Copyright (c) 2016, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef TEXT_VIEW_SUBOBJECT_HPP // {
#define TEXT_VIEW_SUBOBJECT_HPP


#include <experimental/ranges/concepts>
#include <type_traits>


namespace std {
namespace experimental {
inline namespace text {
namespace text_detail {


template<ranges::Semiregular T>
class subobject {
public:
    constexpr subobject() = default;

    constexpr subobject(const T& t)
        noexcept(std::is_nothrow_copy_constructible<T>::value)
    : t(t) {}

    constexpr subobject(T&& t)
        noexcept(std::is_nothrow_move_constructible<T>::value)
    : t(std::move(t)) {}

    constexpr T& get() noexcept {
        return t;
    }
    constexpr const T& get() const noexcept {
        return t;
    }

private:
    T t;
};

template<ranges::Semiregular T>
requires std::is_class<T>::value
      && ! std::is_final<T>::value
class subobject<T>
    : public T
{
public:
    constexpr subobject() = default;

    constexpr subobject(const T& t)
        noexcept(std::is_nothrow_copy_constructible<T>::value)
    : T(t) {}

    constexpr subobject(T&& t)
        noexcept(std::is_nothrow_move_constructible<T>::value)
    : T(std::move(t)) {}

    constexpr T& get() noexcept {
        return *this;
    }
    constexpr const T& get() const noexcept {
        return *this;
    }
};


} // namespace text_detail
} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_SUBOBJECT_HPP
