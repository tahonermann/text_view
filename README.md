# [Text_view]
A [C++ Concepts][ISO/IEC 19217:2015]
based character encoding and code point enumeration library.

- [Overview](#overview)
- [Current features and limitations](#current-features-and-limitations)
- [Requirements](#requirements)
- [Build and installation](#build-and-installation)
  - [Building and installing gcc]
    (#building-and-installing-gcc)
  - [Building and installing Origin-text_view]
    (#building-and-installing-origin-text_view)
  - [Building and installing Text_view]
    (#building-and-installing-text_view)
- [Usage](#usage)
  - [Header &lt;text_view&gt; synopsis](#header-text_view-synopsis)
  - [Concept definitions](#concept-definitions)
    - [Concept Code_unit](#concept-code_unit)
    - [Concept Code_point](#concept-code_point)
    - [Concept Character_set](#concept-character_set)
    - [Concept Character](#concept-character)
    - [Concept Code_unit_iterator](#concept-code_unit_iterator)
    - [Concept Text_encoding_state](#concept-text_encoding_state)
    - [Concept Text_encoding_state_transition]
      (#concept-text_encoding_state_transition)
    - [Concept Text_encoding](#concept-text_encoding)
    - [Concept Text_encoder](#concept-text_encoder)
    - [Concept Text_decoder](#concept-text_decoder)
    - [Concept Text_forward_decoder](#concept-text_forward_decoder)
    - [Concept Text_bidirectional_decoder](#concept-text_bidirectional_decoder)
    - [Concept Text_random_access_decoder](#concept-text_random_access_decoder)
    - [Concept Text_iterator](#concept-text_iterator)
    - [Concept Text_sentinel](#concept-text_sentinel)
    - [Concept Text_view](#concept-text_view)
- [Supported Encodings](#supported-encodings)
- [Terminology](#terminology)
  - [Code Unit](#code-unit)
  - [Code Point](#code-point)
  - [Character Set](#character-set)
  - [Character](#character)
  - [Encoding](#encoding)
- [References](#references)

# Overview
[C++11][ISO/IEC 14882:2011] added support for new character types ([N2249]) and
[Unicode] string literals ([N2442]), but neither [C++11][ISO/IEC 14882:2011],
nor more recent standards have provided means of efficiently and conveniently
enumerating [code points](#code-point) in [Unicode] or legacy encodings.  While
it is possible to implement such enumeration using interfaces provided in the
standard `<codecvt>` library, doing to is awkward, requires that text be
provided as pointers to contiguous memory, and ineffient due to virtual function
call overhead (__examples and data required to back up these assertions__).

[Text_view] provides iterator and range based interfaces for encoding and
decoding strings in a variety of [character encodings](#encoding).  The
interface is intended to support all modern and legacy
[character encodings](#encoding), though this library does not yet provide
implementations for legacy [encodings](#encoding).

An example usage follows.  Note that `\u00F8` (LATIN SMALL LETTER O WITH STROKE)
is encoded as UTF-8 using two [code units](#code-unit) (`\xC3\xB8`), but
iterator based enumeration sees just the single [code point](#code-point).

```C++
using CT = utf8_encoding::character_type;
auto tv = make_text_view<utf8_encoding>(u8"J\u00F8erg is my friend");
auto it = tv.begin();
assert(*it++ == CT{0x004A}); // 'J'
assert(*it++ == CT{0x00F8}); // 'ø'
assert(*it++ == CT{0x0065}); // 'e'
```

The iterators and ranges that [Text_view] provides are compatible with the
non-modifying sequence utilities provided by the standard C++ `<algorithm>`
library.  This enables use of standard algorithms to search encoded text.

```C++
it = std::find(tv.begin(), tv.end(), CT{0x00F8});
assert(it != tv.end());
```

The iterators provided by [Text_view] also provide access to the underlying
[code unit](#code-unit) sequence.

```C++
auto base_it = it.base_range().begin();
assert(*base_it++ == '\xC3');
assert(*base_it++ == '\xB8');
```

[Text_view] ranges satisfy the requirements for use in range-based for
statements.

```C++
for (const auto &ch : tv) {
  ...
}
```

# Current features and limitations
[Text_view] provides interfaces for the following:
- Encoding and decoding of text for the [encodings](#encoding) listed in
  [supported encodings](#supported-encodings).
- Encoding text using [C++11][ISO/IEC 14882:2011] compliant output iterators.
- Decoding text using mostly [C++11][ISO/IEC 14882:2011] compliant input,
  forward, bidirectional, and random access iterators.  These iterators lack
  full compliance for the forward, bidirectional, and random access iterator
  requirements for issues that are addressed by [P0022R1].
- Constructing view adapters for encoded text stored in arrays, containers,
  or std::basic_string, or referenced by another range or view.  These view
  adapters meet the requirements for views in the [ranges proposal][N4560].

[Text_view] does **not** currently provide interfaces for the following:
- Transcoding of code points from one [character set](#character-set) to
  another.
- Collation.
- Localization.
- Internationalization.
- [Unicode] code point properties.
- [Unicode] normalization.

# Requirements
[Text_view] requires a C++ compiler that implements [ISO/IEC technical
specification 19217:2015, **C++ Extensions for concepts**][ISO/IEC 19217:2015]
As of 2015-12-31, this specification is only supported by the current
in-development release of [gcc] that currently self-identifies itself as [gcc]
version 6.0.0.  Additionally, [Text_view] depends on the [Origin-text_view] fork
of [Andrew Sutton]'s [Origin] library for concept definitions.  The fork is
necessary to work around the following [gcc] defects and [Origin] issues that
are surfaced by [Text_view].

1. https://github.com/asutton/origin/issues/5
2. https://gcc.gnu.org/bugzilla/show_bug.cgi?id=69235
3. https://gcc.gnu.org/bugzilla/show_bug.cgi?id=67565

# Build and installation
This section provides instructions for building [Text_view] and suitable
versions of its dependencies.

## Building and installing [gcc]
[Text_view] is known to build successfully with [gcc] svn revision 232017; the
top of trunk as of 2015-12-31.  The following commands can be used to perform a
suitable build of that [gcc] revision on Linux.

```sh
$ svn co svn://gcc.gnu.org/svn/gcc/trunk gcc-trunk-src
$ curl -O ftp://ftp.gnu.org/gnu/gmp/gmp-5.1.1.tar.bz2
$ curl -O ftp://ftp.gnu.org/gnu/mpfr/mpfr-3.1.2.tar.bz2
$ curl -O ftp://ftp.gnu.org/gnu/mpc/mpc-1.0.1.tar.gz
$ cd gcc-trunk-src
$ svn update 32017  # Optional command to select a known good gcc version
$ bzip2 -d -c ../gmp-5.1.1.tar.bz2 | tar -xvf -
$ mv gmp-5.1.1 gmp
$ bzip2 -d -c ../mpfr-3.1.2.tar.bz2 | tar -xvf -
$ mv mpfr-3.1.2 mpfr
$ tar -zxvf ../mpc-1.0.1.tar.gz
$ mv mpc-1.0.1 mpc
$ cd ..
$ mkdir gcc-trunk-build
$ cd gcc-trunk-build
$ LIBRARY_PATH=/usr/lib/$(gcc -print-multiarch); export LIBRARY_PATH
$ CPATH=/usr/include/$(gcc -print-multiarch); export CPATH
$ ../gcc-trunk-src/configure \
  CC=gcc \
  CXX=g++ \
  --prefix $(pwd)/../gcc-trunk-install \
  --disable-multilib \
  --disable-bootstrap \
  --enable-languages=c,c++
$ make -j 4
$ make install
$ cd ..
```

When complete, the new [gcc] build will be present in the `gcc-trunk-install`
directory.

## Building and installing [Origin-text_view]
[Text_view] does not currently build successfully with the top of trunk version
of the [Origin] library.  The [Origin-text_view] fork of [Origin] was created
to work around the issues.  The following commands can be used to perform a
suitable build of [Origin] on Linux that will work with [Text_view].  Note that
these commands assume that an appropriate build of [gcc] was performed and
installed as per the previous section.

```sh
$ PATH=$(pwd)/gcc-trunk-install/bin; export PATH
$ git clone https://github.com/tahonermann/origin.git origin-text_view-src
$ mkdir origin-text_view-build
$ cd origin-text_view-build
$ cmake ../origin-text_view-src \
  -DCMAKE_INSTALL_PREFIX:PATH=$(pwd)/../origin-text_view-install
$ make install
$ cd ..
```

## Building and installing [Text_view]
[Text_view] is currently a header-only library, so no build or installation is
currently required to use it.  It is unlikely that [Text_view] will remain a
header-only library in the future, so at some point, a build and installation
step will be required.

[Text_view] currently has a simple build system that is hard-coded to look for
[gcc] and [Origin] at specific locations.  This will change at some point in
the future, but given the present [gcc] and [Origin] requirements, hasn't been
a burden.  The build system is only used to build and run a few test programs.

The following commands suffice to build and run the test programs.  Note that it
may take several minutes to build the test-text_view program.

```sh
$ vi setenv-gcc-trunk.sh  # Update GCC_INSTALL_PATH and ORIGIN_INSTALL_PATH.
$ . ./setenv-gcc-trunk.sh
$ make
```

If the build succeeds, a few test and utility programs will be present in the
`bin` directory.

# Usage
[Text_view] is currently a header-only library.  To use it in your own code,
add include paths for the `text_view/include` and [Origin] installation
locations, and include the `text_view` header file:

```C++
#include <text_view>
```

All interfaces intended for public use are declared in the
`std::experimental::text` namespace.  The `text` namespace is an inline
namespace, so all entities are available from the `std::experimental` namespace
itself.

The interface descriptions in the sections that follow use the concept names
from the [ranges proposal][N4560].  However, the actual [Text_view]
implementation uses the [Origin] library for concept definitions and those
definitions do not currently match those in the [ranges proposal][N4560].  As a
result, the definitions that follow do not exactly match the code in the
[Text_view] library.  The following definitions are intended to be used as
specification and should be considereed authoritative.  Any differences in
behavior as defined by these definitions as compared to the [Text_view]
implementation is unintentional and should be considered indicatative of a
defect.

Once an implementation of the [ranges proposal][N4560] becomes available, it is
expected that [Text_view] will be ported to it.

## Header &lt;text_view&gt; synopsis

```C++
namespace std {
namespace experimental {
inline namespace text {

// concepts:
template<typename T> concept bool Code_unit();
template<typename T> concept bool Code_point();
template<typename T> concept bool Character_set();
template<typename T> concept bool Character();
template<typename T> concept bool Code_unit_iterator();
template<typename T> concept bool Text_encoding_state();
template<typename T> concept bool Text_encoding_state_transition();
template<typename T> concept bool Text_encoding();
template<typename T, typename I> concept bool Text_encoder();
template<typename T, typename I> concept bool Text_decoder();
template<typename T, typename I> concept bool Text_forward_decoder();
template<typename T, typename I> concept bool Text_bidirectional_decoder();
template<typename T, typename I> concept bool Text_random_access_decoder();
template<typename T> concept bool Text_iterator();
template<typename T, typename I> concept bool Text_sentinel();
template<typename T> concept bool Text_view();

// character sets:
class any_character_set;
class basic_execution_character_set;
class basic_execution_wide_character_set;
class unicode_character_set;

using execution_character_set = /* implementation-defined */ ;
using execution_wide_character_set = /* implementation-defined */ ;
using universal_character_set = /* implementation-defined */ ;

class character_set_info;
class character_set_id;

template<typename CST>
  inline const character_set_info& get_character_set_info();
const character_set_info& get_character_set_info(character_set_id id);
template<typename CST>
  inline character_set_id get_character_set_id();

template<typename T>
  using code_point_type_of = /* implementation-defined */ ;
template<typename T>
  using character_set_type_of = /* implementation-defined */ ;
template<typename T>
  using encoding_type_of = /* implementation-defined */ ;

// characters:
template<Character_set CST> class character;
template <> class character<any_character_set>;

template<Character_set CST>
  bool operator==(const character<any_character_set> &c1,
                  const character<CST> &c2);
template<Character_set CST>
  bool operator==(const character<CST> &c1,
                  const character<any_character_set> &c2);
template<Character_set CST>
  bool operator!=(const character<any_character_set> &c1,
                  const character<CST> &c2);
template<Character_set CST>
  bool operator!=(const character<CST> &c1,
                  const character<any_character_set> &c2);

// encodings:
class basic_execution_character_encoding;
class basic_execution_wide_character_encoding;
#if defined(__STDC_ISO_10646__)
class iso_10646_wide_character_encoding;
#endif // __STDC_ISO_10646__
class utf8_encoding;
class utf8bom_encoding;
class utf16_encoding;
class utf16be_encoding;
class utf16le_encoding;
class utf16bom_encoding;
class utf32_encoding;
class utf32be_encoding;
class utf32le_encoding;
class utf32bom_encoding;

using execution_character_encoding = /* implementation-defined */ ;
using execution_wide_character_encoding = /* implementation-defined */ ;
using char8_character_encoding = /* implementation-defined */ ;
using char16_character_encoding = /* implementation-defined */ ;
using char32_character_encoding = /* implementation-defined */ ;

// itext_iterator:
template<Text_encoding ET, ranges::InputRange RT>
  requires Text_decoder<ET, ranges::iterator_t<const RT>>()
    class itext_iterator;

// itext_sentinel:
template<Text_encoding ET, ranges::InputRange RT>
  class itext_sentinel;

// otext_iterator:
template<Text_encoding E, Code_unit_iterator CUIT>
  requires ranges::OutputIterator<CUIT, typename E::code_unit_type>()
    class otext_iterator;

// basic_text_view:
template<Text_encoding ET, ranges::InputRange RT>
  class basic_text_view;

// basic_text_view synonyms:
using text_view = basic_text_view<execution_character_encoding,
                                  /* implementation-defined */ >;
using wtext_view = basic_text_view<execution_wide_character_encoding,
                                   /* implementation-defined */ >;
using u8text_view = basic_text_view<char8_character_encoding,
                                    /* implementation-defined */ >;
using u16text_view = basic_text_view<char16_character_encoding,
                                     /* implementation-defined */ >;
using u32text_view = basic_text_view<char32_character_encoding,
                                     /* implementation-defined */ >;

// basic_text_view factory functions:
template<Text_encoding ET, ranges::InputIterator IT, ranges::Sentinel<IT> ST>
  auto make_text_view(typename ET::state_type state, IT first, ST last);
template<Text_encoding ET, ranges::InputIterator IT, ranges::Sentinel<IT> ST>
  auto make_text_view(IT first, ST last);
template<Text_encoding ET, ranges::ForwardIterator IT>
  auto make_text_view(typename ET::state_type state,
                      IT first,
                      typename std::make_unsigned<ranges::difference_type_t<IT>>::type n);
template<Text_encoding ET, ranges::ForwardIterator IT>
  auto make_text_view(IT first,
                      typename std::make_unsigned<ranges::difference_type_t<IT>>::type n);
template<Text_encoding ET, ranges::InputRange Iterable>
  auto make_text_view(typename ET::state_type state,
                      const Iterable &iterable);
template<Text_encoding ET, ranges::InputRange Iterable>
  auto make_text_view(const Iterable &iterable);
template<Text_iterator TIT, Text_sentinel<TIT> TST>
  auto make_text_view(TIT first, TST last);
template<Text_view TVT>
auto make_text_view(TVT tv);

// make_cstr_range:
template<Code_unit CUT, std::size_t N>
  auto make_cstr_range(const CUT (&cstr)[N]);

} // inline namespace text
} // namespace experimental
} // namespace std
```

## Concept definitions

### Concept Code_unit
The `Code_unit` concept specifies requirements for a type usable as the
[code unit](#code-unit) type of a string type.

```C++
template<typename T> concept bool Code_unit() {
    return /* implementation-defined */ ;
}
```

`Code_unit<T>()` is satisfied if and only if
`std::is_integral<T>::value` is true and at least one of
`std::is_unsigned<T>::value` is true,
`std::is_same<std::remove_cv<T>::type, char>::value` is true, or
`std::is_same<std::remove_cv<T>::type, wchar_t>::value` is true.

### Concept Code_point
The `Code_point` concept specifies requirements for a type usable as the
[code point](#code-point) type of a [character set](#character-set) type.

```C++
template<typename T> concept bool Code_point() {
    return /* implementation-defined */ ;
}
```

`Code_point<T>()` is satisfied if and only if
`std::is_integral<T>::value` is true and at least one of
`std::is_unsigned<T>::value` is true,
`std::is_same<std::remove_cv<T>::type, char>::value` is true, or
`std::is_same<std::remove_cv<T>::type, wchar_t>::value` is true.

### Concept Character_set
The `Character_set` concept specifies requirements for a type that describes
a [character set](#character-set).  Such a type has a member typedef-name
declaration for a type that satisfies `Code_point` and a static member function
that returns a name for the [character set](#character-set).

```C++
template<typename T> concept bool Character_set() {
  return Code_point<typename T::code_point_type>()
      && requires () {
           { T::get_name() } noexcept -> const char *;
         };
}
```

### Concept Character
The `Character` concept specifies requirements for a type that describes a
[character](#character) as defined by an associated
[character set](#character-set).  Non-static member functions provide access to
the [code point](#code-point) value of the described [character](#character).
Types that satisfy `Character` are regular and copyable.

```C++
template<typename T> concept bool Character() {
  return ranges::Regular<T>()
      && ranges::Copyable<T>()
      && Character_set<typename T::character_set_type>()
      && requires (T t, typename T::character_set_type::code_point_type cp) {
           t.set_code_point(cp);
           { t.get_code_point() } -> typename T::character_set_type::code_point_type;
           { t.get_character_set_id() } -> character_set_id;
         };
}
```

### Concept Code_unit_iterator
The `Code_unit_iterator` concept specifies requirements of an iterator that
has a value type that satisfies `Code_unit`.

```C++
template<typename T> concept bool Code_unit_iterator() {
  return ranges::Iterator<T>()
      && Code_unit<ranges::value_type_t<T>>();
}
```

### Concept Text_encoding_state
The `Text_encoding_state` concept specifies requirements of types that hold
[encoding](#encoding) state.  Such types are default constructible and copyable.

```C++
template<typename T> concept bool Text_encoding_state() {
  return ranges::DefaultConstructible<T>()
      && ranges::Copyable<T>();
}
```

### Concept Text_encoding_state_transition
The `Text_encoding_state_transition` concept specifies requirements of types
that hold [encoding](#encoding) state transitions.  Such types are default
constructible and copyable.

```C++
template<typename T> concept bool Text_encoding_state_transition() {
  return ranges::DefaultConstructible<T>()
      && ranges::Copyable<T>();
}
```

### Concept Text_encoding
The `Text_encoding` concept specifies requirements of types that define an
[encoding](#encoding).  Such types define member types that identify the
[code unit](#code-unit), [character](#character), encoding state, and encoding
state transition types, a static member function that returns an initial
encoding state object that defines the encoding state at the beginning of a
sequence of encoded characters, and static data members that specify the
minimum and maximum number of [code units](#code-units) used to encode any
single character.

```C++
template<typename T> concept bool Text_encoding() {
  return requires () {
           { T::min_code_units } noexcept -> int;
           { T::max_code_units } noexcept -> int;
         }
      && Text_encoding_state<typename T::state_type>()
      && Text_encoding_state_transition<typename T::state_transition_type>()
      && Code_unit<typename T::code_unit_type>()
      && Character<typename T::character_type>()
      && requires () {
           { T::initial_state() }
               -> const typename T::state_type&;
         };
}
```

### Concept Text_encoder
The `Text_encoder` concept specifies requirements of types that are used to
encode [characters](#character) using a particular [code unit](#code-unit)
iterator that satisfies `OutputIterator`.  Such a type satisifies
`Text_encoding` and defines static member functions used to encode state
transitions and [characters](#character).

```C++
template<typename T, typename I> concept bool Text_encoder() {
  return Text_encoding<T>()
      && ranges::OutputIterator<CUIT, typename T::code_unit_type>()
      && requires (
           typename T::state_type &state,
           CUIT &out,
           typename T::state_transition_type stt,
           int &encoded_code_units)
         {
           T::encode_state_transition(state, out, stt, encoded_code_units);
         }
      && requires (
           typename T::state_type &state,
           CUIT &out,
           typename T::character_type c,
           int &encoded_code_units)
         {
           T::encode(state, out, c, encoded_code_units);
         };
}
```

### Concept Text_decoder
The `Text_decoder` concept specifies requirements of types that are used to
decode [characters](#character) using a particular [code unit](#code-unit)
iterator that satisifies `InputIterator`.  Such a type satisfies
`Text_encoding` and defines a static member function used to decode state
transitions and [characters](#character).

```C++
template<typename T, typename I> concept bool Text_decoder() {
  return Text_encoding<T>()
      && ranges::InputIterator<CUIT>()
      && ranges::ConvertibleTo<ranges::value_type_t<CUIT>,
                               typename T::code_unit_type>()
      && requires (
           typename T::state_type &state,
           CUIT &in_next,
           CUIT in_end,
           typename T::character_type &c,
           int &decoded_code_units)
         {
           { T::decode(state, in_next, in_end, c, decoded_code_units) } -> bool;
         };
}
```

### Concept Text_forward_decoder
The `Text_forward_decoder` concept specifies requirements of types that are
used to decode [characters](#character) using a particular
[code unit](#code-unit) iterator that satisifies `ForwardIterator`.  Such a
type satisfies `Text_decoder`.

```C++
template<typename T, typename I> concept bool Text_forward_decoder() {
  return Text_decoder<T, CUIT>()
      && ranges::ForwardIterator<CUIT>();
}
```

### Concept Text_bidirectional_decoder
The `Text_bidirectional_decoder` concept specifies requirements of types that
are used to decode [characters](#character) using a particular
[code unit](#code-unit) iterator that satisifies `BidirectionalIterator`.  Such
a type satisfies `Text_forward_decoder` and defines a static member function
used to decode state transitions and [characters](#character) in the reverse
order of their encoding.

```C++
template<typename T, typename I> concept bool Text_bidirectional_decoder() {
  return Text_forward_decoder<T, CUIT>()
      && ranges::BidirectionalIterator<CUIT>()
      && requires (
           typename T::state_type &state,
           CUIT &in_next,
           CUIT in_end,
           typename T::character_type &c,
           int &decoded_code_units)
         {
           { T::rdecode(state, in_next, in_end, c, decoded_code_units) } -> bool;
         };
}
```

### Concept Text_random_access_decoder
The `Text_random_access_decoder` concept specifies requirements of types that
are used to decode [characters](#character) using a particular
[code unit](#code-unit) iterator that satisifies `RandomAccessIterator`.  Such a
type satisfies `Text_bidirectional_decoder`, requires that the minimum and
maximum number of [code units](#code-unit) used to encode any character have
the same value, and that the encoding state be an empty type.

```C++
template<typename T, typename I> concept bool Text_random_access_decoder() {
  return Text_bidirectional_decoder<T, CUIT>()
      && ranges::RandomAccessIterator<CUIT>()
      && T::min_code_units == T::max_code_units
      && std::is_empty<typename T::state_type>::value;
}
```

### Concept Text_iterator
The `Text_iterator` concept specifies requirements of types that are used to
iterator over [characters](#character) in an [encoded](#encoding) sequence of
[code units](#code-unit).  [Encoding](#encoding) state is held in each iterator
instance as needed to decode the [code unit](#code-unit) sequence and is made
accessible via non-static member functions.  The value type of a
`Text_iterator` satisfies `Character`.

```C++
template<typename T> concept bool Text_iterator() {
  return ranges::Iterator<T>()
      && Character<ranges::value_type_t<T>>()
      && Text_encoding<typename T::encoding_type>()
      && Text_encoding_state<typename T::state_type>()
      && requires (T t, const T ct) {
           { t.state() } noexcept
               -> typename T::encoding_type::state_type&;
           { ct.state() } noexcept
               -> const typename T::encoding_type::state_type&;
         };
}
```

### Concept Text_sentinel
The `Text_sentinel` concept specifies requirements of types that are used to
mark the end of a range of encoded [characters](#character).  A type T that
satisfies `Text_iterator` also satisfies `Text_sentinel<T>` there by enabling
`Text_iterator` types to be used as sentinels.

```C++
template<typename T, typename I> concept bool Text_sentinel() {
  return ranges::Sentinel<T, I>()
      && Text_iterator<I>();
}
```

### Concept Text_view
The `Text_view` concept specifies requirements of types that provide view access
to an underlying [code unit](#code-unit) range.  Such types satisy
`ranges::View`, provide iterators that satisfy `Text_iterator`, define member
types that identify the [encoding](#encoding), encoding state, and underlying
[code unit](#code-unit) range and iterator types.  Non-static member functions
are provided to access the underlying [code unit](#code-unit) range and initial
[encoding](#encoding) state.

Types that satisfy `Text_view` do not own the underlying [code unit](#code-unit)
range and are copyable in constant time.  The lifetime of the underlying range
must exceed the lifetime of referencing `Text_view` objects.

```C++
template<typename T> concept bool Text_view() {
  return ranges::View<T>()
      R& Text_iterator<ranges::iterator_t<T>>()
      && Text_encoding<typename T::encoding_type>()
      && ranges::InputRange<typename T::range_type>()
      && Text_encoding_state<typename T::state_type>()
      && Code_unit_iterator<typename T::code_unit_iterator>()
      R& requires (T t, const T ct) {
           { t.base() } noexcept
               -> typename T::range_type&;
           { ct.base() } noexcept
               -> const typename T::range_type&;
           { t.initial_state() } noexcept
               -> typename T::state_type&;
           { ct.initial_state() } noexcept
               -> const typename T::state_type&;
         };
}
```

# Supported Encodings
As of 2015-12-31, supported [encodings](#encoding) include:

Encoding name | Description | Encoding type
------------- | ----------- | -------------
execution_character_encoding | Type alias for the encoding of ordinary string literals (C++11 2.3p3) | implementation defined
execution_wide_character_encoding | Type alias for the encoding of wide string literals (C++11 2.3p3) | implementation defined
char8_character_encoding | Type alias for the encoding of UTF-8 string literals (C++11 2.14.5p7) | implementation defined
char16_character_encoding | Type alias for the encoding of char16_t string literals (C++11 2.14.5p9) | implementation defined
char32_character_encoding | Type alias for the encoding of char32_t string literals (C++11 2.14.5p10) | implementation defined
basic_execution_character_encoding | An encoding that meets the minimum requirements of C++11 2.3p3 | trivial
basic_execution_wide_character_encoding | An encoding that meets the minimum requirements of C++11 2.3p3 | trivial
iso_10646_wide_character_encoding | An ISO 10646 encoding.  Only defined if __STDC_ISO_10646__ is defined | trivial
utf8_encoding | [Unicode] UTF-8 | stateless, variable width
utf8bom_encoding | [Unicode] UTF-8 with a byte order mark | stateful, variable width
utf16_encoding | [Unicode] UTF-16, native endian | stateless, variable width
utf16be_encoding | [Unicode] UTF-16, big endian | stateless, variable width
utf16le_encoding | [Unicode] UTF-16, little endian | stateless, variable width
utf16bom_encoding | [Unicode] UTF-16 with a byte order mark | stateful, variable width
utf32_encoding | [Unicode] UTF-32, native endian | stateless, fixed width
utf32be_encoding | [Unicode] UTF-16, big endian | stateless, fixed width
utf32le_encoding | [Unicode] UTF-16, little endian | stateless, fixed width
utf32bom_encoding | [Unicode] UTF-32 with a byte order mark | stateful, variable width

# Terminology
The terminology used in this document and in the [Text_view] library has been
chosen to be consistent with industry standards and, in particular, the
[Unicode standard].  Any inconsistencies in the use of this terminology and that
in the [Unicode standard] is unintentional.  The terms described in this
document and used within the [Text_view] library comprise a subset of the
terminology used within the [Unicode standard]; only those terms necessary to
specify functionality exhibited by this library are included here.  Those who
would like to learn more about general text processing terminology in computer
systems are encouraged to read chatper 2, "General Structure" of the
[Unicode standard].

## Code Unit
A single, indivisible, integral element of an encoded sequence of characters.  A
sequence of one or more code units specifies a code point or encoding state
transition as defined by a character encoding.  A code unit does not, by itself,
identify any particular character or code point; the meaning ascribed to a
particular code unit value is derived from a character encoding definition.

The [C++11][ISO/IEC 14882:2011] `char`, `wchar_t`, `char16_t`, and `char32_t`
types are most commonly used as code unit types.

The string literal `u8"J\u00F8erg"` contains 7 code units and 6 code unit
sequences; "\u00F8" is encoded using two code units and string literals contain
a trailing NUL code unit.

The string literal `"J\u00F8erg"` contains an implementation defined number of
code units.  The standard does not specify the encoding of ordinary and wide
string literals, so the number of code units encoded by "\u00F8" depends on the
implementation defined encoding used for ordinary string literals.

## Code Point
An integral value denoting an abstract character as defined by a character set.
A code point does not, by itself, identify any particular character; the
meaning ascribed to a particular code point value is derived from a character
set definition.

The [C++11][ISO/IEC 14882:2011] `char`, `wchar_t`, `char16_t`, and `char32_t`
types are most commonly used as code point types.

The string literal `u8"J\u00F8erg"` describes a sequence of 6 code point values;
string literals implicitly specify a trailing NUL code point.

The string literal `"J\u00F8erg"` describes a sequence of an implementation
defined number of code point values.  The standard does not specify the encoding
of ordinary and wide string literals, so the number of code points encoded by
"\u00F8" depends on the implementation defined encoding used for ordinary string
literals.  Implementations are permitted to translate a single code point in the
source or Unicode character sets to multiple code points in the execution
encoding.

## Character Set
A mapping of code point values to abstract characters.  A character set need not
provide a mapping for every possible code point value representable by the code
point type.

[C++11][ISO/IEC 14882:2011] does not specify the use of any particular character
set or encoding for ordinary and wide character and string literals, though it
does place some restrictions on them.  Unicode character and string literals are
governed by the [Unicode] standard.

Common character sets include [ASCII], [Unicode][Unicode code charts], and
[Windows code page 1252].

## Character
An element of written language, for example, a letter, number, or symbol.  A
character is identified by the combination of a character set and a code point
value.

## Encoding
A method of representing a sequence of characters as a sequence of code unit
sequences.

An encoding may be stateless or stateful.  In stateless encodings, characters
may be encoded or decoded starting from the beginning of any code unit sequence.
In stateful encodings, it may be necessary to record certain affects of
previously encoded characters in order to correctly encode additional
characters, or to decode preceding code unit sequences in order to correctly
decode following code unit sequences.

An encoding may be fixed width or variable width.  In fixed width encodings,
all characters are encoded using a single code unit sequence and all code unit
sequences have the same length.  In variable width encodings, different
characters may require multiple code unit sequences, or code unit sequences of
varying length.

An encoding may support bidirectional or random access decoding of code unit
sequences.  In bidirectional encodings, characters may be decoded by traversing
code unit sequences in reverse order.  Such encodings must support a method to
identify the start of a preceding code unit sequence.  In random access
encodings, characters may be decoded from any code unit sequence within the
sequence of code unit sequences, in constant time, without having to decode any
other code unit sequence.  Random access encodings are necessarily stateless
and fixed length.  An encoding that is neither bidirectional, nor random
access, may only be decoded by traversing code unit sequences in forward order.

An encoding may support encoding characters from multiple character sets.  Such
an encoding is either stateful and defines code unit sequences that switch the
active character set, or defines code unit sequences that implicitly identify
a character set, or both.

A trivial encoding is one in which all encoded characters correspond to a single
character set and where each code unit encodes exactly one character using the
same value as the code point for that character.  Such an encoding is stateless,
fixed width, and supports random access decoding.

Common encodings include the [Unicode] UTF-8, UTF-16, and UTF-32 encodings, the
ISO/IEC 8859 series of encodings including [ISO/IEC 8859-1], and many trivial
encodings such as [Windows code page 1252].

# References
- [Text_view]  
  https://github.com/tahonermann/text_view
- [Origin]  
  http://asutton.github.io/origin
- [Origin-text_view]  
  https://github.com/tahonermann/origin
- [ISO/IEC technical specification 19217:2015, C++ Extensions for concepts][ISO/IEC 19217:2015]  
  http://www.iso.org/iso/home/store/catalogue_tc/catalogue_detail.htm?csnumber=64031

[Text_view]:
https://github.com/tahonermann/text_view
(Text_view library)
[Andrew Sutton]:
https://github.com/asutton
(Andrew Sutton)
[Origin]:
http://asutton.github.io/origin
(Origin libraries)
[Origin-text_view]:
https://github.com/tahonermann/origin
(Origin libraries for text_view)
[Unicode]:
http://unicode.org
(The Unicode Consortium)
[Unicode standard]:
http://www.unicode.org/standard/standard.html
(The Unicode Standard)
[Unicode code charts]:
http://www.unicode.org/charts
(Unicode Character Code Charts)
[ASCII]:
http://webstore.ansi.org/RecordDetail.aspx?sku=INCITS+4-1986[R2012]
(INCITS 4-1986[R2012])
[Windows code page 1252]:
https://msdn.microsoft.com/en-us/library/cc195054.aspx
(Code Page 1252 Windows Latin 1)
[ISO/IEC 8859-1]:
http://webstore.ansi.org/RecordDetail.aspx?sku=ISO%2fIEC+8859-1%3a1998
(ISO/IEC 8859-1:1998 Information technology - 8-bit single-byte coded graphic character sets - Part 1: Latin alphabet No. 1)
[ISO/IEC 14882:2011]:
http://www.iso.org/iso/home/store/catalogue_ics/catalogue_detail_ics.htm?csnumber=50372
(ISO/IEC 14882:2011 Information technology -- Programming languages -- C++)
[ISO/IEC 19217:2015]:
http://www.iso.org/iso/home/store/catalogue_tc/catalogue_detail.htm?csnumber=64031
(ISO/IEC technical specification 19217:2015, C++ Extensions for concepts)
[N2249]:
http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2249.html
(N2249: New Character Types in C++)
[N2442]:
http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2442.htm
(N2442: Raw and Unicode String Literals; Unified Proposal (Rev. 2))
[N4560]:
http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4560.pdf
(N4560: Working Draft, C++ Extensions for Ranges)
[P0022R1]:
http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0022r1.html
(P0022R1: Proxy Iterators for the Ranges Extensions)
[gcc]:
https://gcc.gnu.org
(GCC, the GNU Compiler Collection)
