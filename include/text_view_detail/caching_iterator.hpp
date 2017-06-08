// Copyright (c) 2017, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef TEXT_VIEW_CACHING_ITERATOR_HPP // {
#define TEXT_VIEW_CACHING_ITERATOR_HPP


#include <deque>
#include <memory>
#include <experimental/ranges/iterator>
#include <text_view_detail/basic_view.hpp>


namespace std {
namespace experimental {
inline namespace text {
namespace text_detail {


// caching_iterator is an iterator adapter that models forward iterator given
// an input iterator by caching read values.  Multiple instances of
// caching_iterator that adapt the same input iterator are supported (as
// required for forward iterators) with the restriction that each instance
// other than the first be either copy constructed or assigned from an existing
// caching_iterator; this is necessary so that the copies share the same data
// structures and reference the same copy of the adapted input iterator.
//
// The shared cache held by a set of caching_iterators grows indefinitely as
// the underlying input iterator is dereferenced and advanced.  Calling
// clear_cache() on a given caching_iterator discards values not reachable from
// that iterator and invalidates all other iterators that adapt the same input
// iterator.
//
// The cached_range() and look_ahead_range() member functions provide access to
// the cached values read from the underlying input iterator.  The values held
// in the look_ahead_range() correspond to values that will be read as the
// current iterator is dereferenced and advanced.  look_ahead_range() is useful
// to retrieve values that were read by algorithms, such as parsers, that
// require some degree of look ahead, but that still require further processing.

template<ranges::InputIterator I>
requires ! ranges::ForwardIterator<I>
class caching_cursor
{
    using iterator_type = I;
    using value_type = ranges::value_type_t<I>;
    using reference = value_type;
    using pointer = const value_type*;
    using difference_type = ranges::difference_type_t<I>;

    class shared_data {
    public:
        shared_data(I current) : current(current) {}

        iterator_type current;
        std::deque<value_type> cache;
    };

public:
    class mixin
        : protected ranges::basic_mixin<caching_cursor>
    {
        using base_type = ranges::basic_mixin<caching_cursor>;

        template<typename> friend class caching_iterator_sentinel;
    public:
        mixin() = default;

        mixin(iterator_type current)
        :
            base_type{caching_cursor{std::move(current)}}
        {}

        using base_type::base_type;

        const iterator_type& base() const noexcept {
            return this->get().base();
        }

        decltype(auto) cached_range() const noexcept {
            return this->get().cached_range();
        }

        decltype(auto) look_ahead_range() const noexcept {
            return this->get().look_ahead_range();
        }

        void clear_cache() const {
            return this->get().clear_cache();
        }

    private:
        std::size_t position() const {
            return this->get().position;
        }
        std::size_t cache_size() const {
            return this->get().current_data->cache.size();
        }
    };

    caching_cursor() = default;

    caching_cursor(iterator_type current)
    :
        current_data(make_shared<shared_data>(std::move(current))),
        position{0}
    {}

    reference read() const {
        return dereference();
    }

    pointer arrow() const {
        return &dereference();
    }

    void next() {
        // Ensure the current position has been read.
        dereference();
        ++position;
    }

    bool equal(const caching_cursor &other) const {
        return current_data == other.current_data
            && position == other.position;
    }

    const iterator_type& base() const noexcept {
        return current_data->current;
    }

    auto cached_range() const noexcept {
        return make_basic_view(
                   current_data->cache.begin(),
                   current_data->cache.begin()+position);
    }

    auto look_ahead_range() const noexcept {
        return make_basic_view(
                   current_data->cache.begin()+position,
                   current_data->cache.end());
    }

    // clear_cache() invalidates all other iterators.
    void clear_cache() const {
        current_data->cache.erase(
            current_data->cache.begin(),
            current_data->cache.begin()+position);
        position = 0;
    }

private:
    const value_type& dereference() const {
        if (position >= current_data->cache.size()) {
            current_data->cache.push_back(*current_data->current);
            ++current_data->current;
        }
        return current_data->cache[position];
    }

    std::shared_ptr<shared_data> current_data;
    mutable std::size_t position;
};


/*
 * caching_iterator
 */
template<ranges::InputIterator I>
requires ! ranges::ForwardIterator<I>
using caching_iterator =
    ranges::basic_iterator<caching_cursor<I>>;


/*
 * make_caching_iterator
 */
template<ranges::InputIterator I>
requires ! ranges::ForwardIterator<I>
caching_iterator<I>
make_caching_iterator(I i) {
    return { i };
}


/*
 * caching_iterator_sentinel
 */
template<typename S>
class caching_iterator_sentinel
{
public:
    using sentinel = S;

    caching_iterator_sentinel() = default;

    caching_iterator_sentinel(sentinel s)
        : s(std::move(s)) {}

    template<ranges::InputIterator I>
    requires ranges::Sentinel<S, I>
    friend bool operator==(
        const caching_iterator<I> &ci,
        const caching_iterator_sentinel &cis)
    {
        return cis.equal(ci);
    }
    template<ranges::InputIterator I>
    requires ranges::Sentinel<S, I>
    friend bool operator!=(
        const caching_iterator<I> &ci,
        const caching_iterator_sentinel &cis)
    {
        return !(ci == cis);
    }
    template<ranges::InputIterator I>
    requires ranges::Sentinel<S, I>
    friend bool operator==(
        const caching_iterator_sentinel &cis,
        const caching_iterator<I> &ci)
    {
        return ci == cis;
    }
    template<ranges::InputIterator I>
    requires ranges::Sentinel<S, I>
    friend bool operator!=(
        const caching_iterator_sentinel &cis,
        const caching_iterator<I> &ci)
    {
        return !(cis == ci);
    }

    const sentinel& base() const noexcept {
        return s;
    }

private:
    template<ranges::InputIterator I>
    requires ranges::Sentinel<S, I>
    bool equal(const caching_iterator<I> &ci) const {
        return ci.position() == ci.cache_size()
            && ci.base() == s;
    }

    sentinel s;
};


/*
 * make_caching_iterator_sentinel
 */
template<typename S>
caching_iterator_sentinel<S>
make_caching_iterator_sentinel(S s) {
    return { s };
}


} // namespace text_detail
} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_CACHING_ITERATOR_HPP
