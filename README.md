# Text view
A [C++ Concepts](http://www.iso.org/iso/home/store/catalogue_tc/catalogue_detail.htm?csnumber=64031)
based character encoding and code point enumeration library.

# Overview
`text_view` provides iterator and range based interfaces for encoding and decoding
strings in a variety of character encodings.  The interface is intended to support
all modern and legacy character encodings, though implementations are not yet
available for legacy encodings.

An example usage follows.  Note that `\u00F8` (LATIN SMALL LETTER O WITH STROKE)
is encoded using two code units (`\xC3\xB8`), but iterator based enumeration sees
just the single code point.

```C++
using CT = utf8_encoding::codec_type::character_type;
auto tv = make_text_view<utf8_encoding>(u8"J\u00F8erg is my friend");
auto it = tv.begin();
assert(*it++ == CT{0x004A}); // 'J'
assert(*it++ == CT{0x00F8}); // 'ø'
assert(*it++ == CT{0x0065}); // 'e'
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

# Requirements
`text_view` requires a C++ compiler that implements [ISO/IEC technical specification
19217:2015, **C++ Extensions for concepts**]
(http://www.iso.org/iso/home/store/catalogue_tc/catalogue_detail.htm?csnumber=64031).
As of 2015-12-31, this specification is only supported by the current in-development
release of [gcc](https://gcc.gnu.org) that currently self-identifies itself as gcc
version 6.0.0.  Additionally, `text_view` depends on
[Andrew Sutton](https://github.com/asutton)'s [Origin](http://asutton.github.io/origin/)
library for concept definitions.

## Building and installing [gcc](https://gcc.gnu.org)
`text_view` is known to build successfully with gcc svn revision 232017; the top of
trunk as of 2015-12-31.  The following commands can be used to perform a suitable build
of gcc on Linux that is known to successfully compile `text_view`.

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

When complete, the new gcc build will be present in the `gcc-trunk-install` directory.

## Building and installing [Origin](http://asutton.github.io/origin/)
`text_view` does not currently build successfully with the top of trunk version of the
[Origin](http://asutton.github.io/origin/) library for two reasons:

1. Changes to `text_view` are needed to accommodate header file reorganization.  These
  changes are simple, but...
2. Compiling `text_view` with the current top of trunk version of the
   [Origin](http://asutton.github.io/origin/) library (with changes needed for the above
   mentioned header file reorganization) results in gcc entering a seemingly infinite
   loop that eventually crashes the process when memory is exhausted.  This issue has
   not yet been investigated or reported.

As of 2015-12-31, `text_view` is known to build successfully with commit
asutton/origin@4066c128becb83a9eb40eff0562d7dadd16e5ba9.  The following commands can be
used to perform a suitable build of Origin on Linux that is known to successfully work
with `text_view`.

```sh
$ git clone https://github.com/asutton/origin.git origin-trunk-src
$ cd origin-trunk-src
$ git reset --hard 4066c128becb83a9eb40eff0562d7dadd16e5ba9
```

With this version of the [Origin](http://asutton.github.io/origin/) library, it is not
necessary to perform a build or installation of it.  `text_view` only needs the header
files.

# Building and installing `text_view`
`text_view` is currently a header-only library, so no build or installation is required.
It is unlikely that `text_view` will remain a header-only library in the future, so at
some point, a build and installation step will be required.

`text_view` currently has an exceedingly simple build system that is hard-coded to look
for gcc and Origin at specific locations.  This will change at some point in the future,
but given the present gcc and Origin requirements, hasn't been a burden.  The build
system is currently used to build and run a few test programs.

The following commands suffice to build and run the test programs:

```sh
$ vi setenv-gcc-trunk.sh  # Update GCC_INSTALL_PATH and ORIGIN_INSTALL_PATH.
$ . ./setenv-gcc-trunk.sh
$ make
```

If the build succeeds, a few test and utility programs will be present in the `bin`
directory.

# Usage
`text_view` is currently a header-only library.  To use it in your own code, add include
paths for the `text_view/include` and Origin installation locations, and include the
`text_view` header file:

```C++
#include <text_view>
```

All interfaces intended for public use are declared in the `std::experimental::text`
namespace.  The `text` namespace is an inline namespace, so all entities are available
from the `std::experimental` namespace itself.
