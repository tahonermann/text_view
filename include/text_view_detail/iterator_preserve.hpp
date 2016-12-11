// Copyright (c) 2016, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef TEXT_VIEW_ITERATOR_PRESERVE_HPP // {
#define TEXT_VIEW_ITERATOR_PRESERVE_HPP


#include <type_traits>
#include <experimental/ranges/concepts>


namespace std {
namespace experimental {
inline namespace text {
namespace text_detail {


// iterator_preserve provides facilities to help implement exception safety
// guarantees for code that mutates iterators.  Conceptually, it provides a
// temporary copy of an iterator to be used within a block of code that might
// throw and an interface to assign the copy back to the preserved iterator
// once the block has completed without throwing an exception.
//
// Consider the following code that attempts to meet the strong exception
// safety guarantee by ensuring that the passed in iterator remains unmutated
// unless the function completes successfully.  This code works if 'I' is at
// least a forward iterator, but will result in undefined behavior if 'I' is
// only an input iterator, an exception is thrown, and the caller performs any
// operations on the iterator it provided other than destroy it.  This occurs
// because copying an input iterator (or an output iterator) and mutating it
// invalidates the iterator it was copied from.
//   template<InputIterator I>
//   void f(I &it) {
//     auto copy_of_it = it;
//     auto x = *copy_of_it++;
//     function_that_might_throw(x);
//     it = copy_of_it;
//   }
//
// The strong exception safety gurantee cannot, in general, be provided for
// input and output iterators since reads or writes through such iterators
// cannot be undone.  However, iterator_preserve can be used to allow code
// like the above to be written generically such that it provides the basic
// exception guarantee for input (and output) iterators and the strong
// exception guarantee for forward or better iterators.  It accomplishes this
// by providing an abstract interface that grants access to the original
// iterator for input and output iterators, and to a copy of the iterator
// for forward or better iterators.  The code above could be rewritten as
// follows.
//   template<Iterator I>
//   void f(I &it) {
//     auto preserve = make_iterator_preserve(it);
//     auto x = *preserve.get()++;
//     function_that_might_throw(x);
//     preserve.update();
//  }


// The primary iterator_preserve class template provides functionality for
// input and output iterators.  It provides the iterator to be preserved as
// the active iterator and update() is a no-op.
template<ranges::Iterator I>
class iterator_preserve {
public:
    iterator_preserve(I &i) noexcept : preserved(i) {}

    I& get() noexcept {
        return preserved;
    }
    const I& get() const noexcept {
        return preserved;
    }

    void update() noexcept {}

private:
    I &preserved;
};

// This iterator_preserve class template specialization provides functionality
// for forward or better iterators.  It provides a copy of the iterator to be
// preserved as the active iterator and update() synchronizes the copy back to
// the preserved iterator.
template<ranges::ForwardIterator I>
class iterator_preserve<I> {
public:
    iterator_preserve(I &i)
        noexcept(std::is_nothrow_copy_constructible<I>::value)
    : preserved(i), active(i) {}

    I& get() noexcept {
        return active;
    }
    const I& get() const noexcept {
        return active;
    }

    void update()
        noexcept(std::is_nothrow_copy_assignable<I>::value)
    {
        preserved = active;
    }

private:
    I &preserved;
    I active;
};


template<ranges::Iterator I>
auto make_iterator_preserve(I &i) {
    return iterator_preserve<I>{i};
}


} // namespace text_detail
} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_ITERATOR_PRESERVE_HPP
