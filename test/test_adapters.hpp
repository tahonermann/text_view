// Copyright (c) 2017, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef TEXT_VIEW_TEST_ADAPTERS_HPP // {
#define TEXT_VIEW_TEST_ADAPTERS_HPP


#include <iterator>
#include <experimental/ranges/iterator>
#include <text_view_detail/adl_customization.hpp>


// Input iterator adapter.  The standard doesn't provide input iterator
// types that don't also satisfy forward iterator requirements or impose
// additional requirements.  istream_iterator, for example, requires a
// char_traits specialization for its character type.
template<std::experimental::ranges::InputIterator IT>
class input_iterator_adapter {
public:
    using iterator_category = std::input_iterator_tag;
    using value_type = typename std::iterator_traits<IT>::value_type;
    using difference_type = typename std::iterator_traits<IT>::difference_type;
    using pointer = typename std::iterator_traits<IT>::pointer;
    using reference = typename std::iterator_traits<IT>::reference;

    input_iterator_adapter() = default;
    input_iterator_adapter(IT it) : it(it) {}

    friend bool operator==(
        const input_iterator_adapter &l,
        const input_iterator_adapter &r)
    {
        return l.it == r.it;
    }
    friend bool operator!=(
        const input_iterator_adapter &l,
        const input_iterator_adapter &r)
    {
        return !(l == r);
    }

    auto operator*() const -> typename std::iterator_traits<IT>::reference {
        return *it;
    }

    auto operator++() -> input_iterator_adapter& {
        ++it;
        return *this;
    }
    auto operator++(int) -> input_iterator_adapter {
        input_iterator_adapter tmp{*this};
        ++*this;
        return tmp;
    }

private:
    IT it;
};


// Output iterator adapter.  The standard doesn't provide output iterator
// types that don't also satisfy forward iterator requirements or impose
// additional requirements.  ostream_iterator, for example, requires a
// char_traits specialization for its character type.
template<
    typename IT,
    typename T>
requires std::experimental::ranges::OutputIterator<IT, T>()
class output_iterator_adapter {
public:
    using iterator_category = std::output_iterator_tag;
    using value_type = typename std::iterator_traits<IT>::value_type;
    using difference_type = typename std::iterator_traits<IT>::difference_type;
    using pointer = typename std::iterator_traits<IT>::pointer;
    using reference = typename std::iterator_traits<IT>::reference;

    output_iterator_adapter() = default;
    output_iterator_adapter(IT it) : it(it) {}

    auto operator*() -> output_iterator_adapter& {
        return *this;
    }
    void operator=(const T& t) {
        *it = t;
    }

    auto operator++() -> output_iterator_adapter& {
        ++it;
        return *this;
    }
    auto operator++(int) -> output_iterator_adapter {
        output_iterator_adapter tmp{*this};
        ++*this;
        return tmp;
    }

private:
    IT it;
};


// Input view adapter.  The standard doesn't provide a container type with
// input iterators that aren't also forward, bidirectional, or random access
// iterators.
template<std::experimental::ranges::InputRange RT>
class input_range_view_adapter {
public:
    input_range_view_adapter() = default;
    input_range_view_adapter(RT &r) : r(&r) {}

    auto begin() const
        -> input_iterator_adapter<
               decltype(std::experimental::text_detail::adl_begin(
                   std::declval<const RT>()))>
    {
        return input_iterator_adapter<
            decltype(std::experimental::text_detail::adl_begin(
                std::declval<const RT>()))>{
                    std::experimental::text_detail::adl_begin(*r)};
    }
    auto end() const
        -> input_iterator_adapter<
               decltype(std::experimental::text_detail::adl_end(
                   std::declval<const RT>()))>
    {
        return input_iterator_adapter<
            decltype(std::experimental::text_detail::adl_end(
                std::declval<const RT>()))>{
                    std::experimental::text_detail::adl_end(*r)};
    }

private:
    RT *r;
};


// Iterable view adapter.  This class provides an input range with different
// return types for begin() and end().
template<std::experimental::ranges::InputRange RT>
class iterable_view_adapter {
public:
    using range_type = RT;
    using iterator = std::experimental::ranges::iterator_t<std::add_const_t<RT>>;

    class sentinel {
    public:
        sentinel() = default;
        explicit sentinel(iterator i) : i{i} {}

        friend bool operator==(
            const sentinel &l,
            const sentinel &r)
        {
            // Sentinels always compare equal regardless of any internal state.
            // See N4128, 10.1 "Sentinel Equality".
            return true;
        }
        friend bool operator!=(
            const sentinel &l,
            const sentinel &r)
        {
            return !(l == r);
        }

        friend bool operator==(
            const iterator &i,
            const sentinel &s)
        {
            return i == s.i;
        }
        friend bool operator!=(
            const iterator &i,
            const sentinel &s)
        {
            return !(i == s);
        }
        friend bool operator==(
            const sentinel &s,
            const iterator &i)
        {
            return i == s;
        }
        friend bool operator!=(
            const sentinel &s,
            const iterator &i)
        {
            return !(s == i);
        }

        iterator base() const {
            return i;
        }

    private:
        iterator i;
    };

    iterable_view_adapter(range_type &r) : r{r} {}

    iterator begin() const {
        return std::experimental::text_detail::adl_begin(r);
    }
    sentinel end() const {
        return sentinel{std::experimental::text_detail::adl_end(r)};
    }

private:
    range_type &r;
};


#endif // } TEXT_VIEW_TEST_ADAPTERS_HPP
