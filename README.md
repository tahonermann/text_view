# [Text_view]
A [C++ Concepts][ISO/IEC 19217:2015]
based character encoding and code point enumeration library.

- [Overview](#overview)
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
- [References](#references)

# Overview
[Text_view] provides iterator and range based interfaces for encoding and
decoding strings in a variety of character encodings.  The interface is
intended to support all modern and legacy character encodings, though
implementations are not yet available for legacy encodings.

An example usage follows.  Note that `\u00F8` (LATIN SMALL LETTER O WITH STROKE)
is encoded using two code units (`\xC3\xB8`), but iterator based enumeration
sees just the single code point.

```C++
using CT = utf8_encoding::character_type;
auto tv = make_text_view<utf8_encoding>(u8"J\u00F8erg is my friend");
auto it = tv.begin();
assert(*it++ == CT{0x004A}); // 'J'
assert(*it++ == CT{0x00F8}); // 'ø'
assert(*it++ == CT{0x0065}); // 'e'
```

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
template<Text_encoding ET, origin::Input_range RT>
  requires Text_decoder<ET, origin::Iterator_type<const RT>>()
    class itext_iterator;

// itext_sentinel:
template<Text_encoding ET, origin::Input_range RT>
  class itext_sentinel;

// otext_iterator:
template<Text_encoding E, Code_unit_iterator CUIT>
  requires origin::Output_iterator<CUIT, typename E::code_unit_type>()
    class otext_iterator;

// basic_text_view:
template<Text_encoding ET, origin::Input_range RT>
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
template<Text_encoding ET, origin::Input_iterator IT, origin::Sentinel<IT> ST>
  auto make_text_view(typename ET::state_type state, IT first, ST last);
template<Text_encoding ET, origin::Input_iterator IT, origin::Sentinel<IT> ST>
  auto make_text_view(IT first, ST last);
template<Text_encoding ET, origin::Forward_iterator IT>
  auto make_text_view(typename ET::state_type state,
                      IT first,
                      origin::Make_unsigned<origin::Difference_type<IT>> n);
template<Text_encoding ET, origin::Forward_iterator IT>
  auto make_text_view(IT first,
                      origin::Make_unsigned<origin::Difference_type<IT>> n);
template<Text_encoding ET, origin::Input_range Iterable>
  auto make_text_view(typename ET::state_type state,
                      const Iterable &iterable);
template<Text_encoding ET, origin::Input_range Iterable>
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
The `Code_unit` concept specifies requirements for a type usable as the code
unit type of a string type.

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
The `Code_point` concept specifies requirements for a type usable as the code
point type of a character set type.

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
a character set.  Such a type has a member typedef-name declaration for a type
that satisfies `Code_point` and a static member function that returns a name
for the character set.

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
character as defined by an associated character set.  Non-static member
functions provide access to the code point value of the described character.
Types that satisfy `Character` are regular and copy assignable.

```C++
template<typename T> concept bool Character() {
  return origin::Regular<T>()
      && origin::Copy_assignable<T>()
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
  return origin::Iterator<T>()
      && Code_unit<origin::Value_type<T>>();
}
```

### Concept Text_encoding_state

```C++
template<typename T> concept bool Text_encoding_state();
```

### Concept Text_encoding_state_transition

```C++
template<typename T> concept bool Text_encoding_state_transition();
```

### Concept Text_encoding

```C++
template<typename T> concept bool Text_encoding();
```

### Concept Text_encoder

```C++
template<typename T, typename I> concept bool Text_encoder();
```

### Concept Text_decoder

```C++
template<typename T, typename I> concept bool Text_decoder();
```

### Concept Text_forward_decoder

```C++
template<typename T, typename I> concept bool Text_forward_decoder();
```

### Concept Text_bidirectional_decoder

```C++
template<typename T, typename I> concept bool Text_bidirectional_decoder();
```

### Concept Text_random_access_decoder

```C++
template<typename T, typename I> concept bool Text_random_access_decoder();
```

### Concept Text_iterator

```C++
template<typename T> concept bool Text_iterator();
```

### Concept Text_sentinel

```C++
template<typename T, typename I> concept bool Text_sentinel();
```

### Concept Text_view

```C++
template<typename T> concept bool Text_view();
```

# Supported Encodings
As of 2015-12-31, supported encodings include:

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
utf8_encoding | Unicode UTF-8 | stateless, variable width
utf8bom_encoding | Unicode UTF-8 with a byte order mark | stateful, variable width
utf16_encoding | Unicode UTF-16, native endian | stateless, variable width
utf16be_encoding | Unicode UTF-16, big endian | stateless, variable width
utf16le_encoding | Unicode UTF-16, little endian | stateless, variable width
utf16bom_encoding | Unicode UTF-16 with a byte order mark | stateful, variable width
utf32_encoding | Unicode UTF-32, native endian | stateless, fixed width
utf32be_encoding | Unicode UTF-16, big endian | stateless, fixed width
utf32le_encoding | Unicode UTF-16, little endian | stateless, fixed width
utf32bom_encoding | Unicode UTF-32 with a byte order mark | stateful, variable width

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
[ISO/IEC 19217:2015]:
http://www.iso.org/iso/home/store/catalogue_tc/catalogue_detail.htm?csnumber=64031
(ISO/IEC technical specification 19217:2015, C++ Extensions for concepts)
[gcc]:
https://gcc.gnu.org
(GCC, the GNU Compiler Collection)
