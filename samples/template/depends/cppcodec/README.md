# cppcodec

[![Build Status](https://travis-ci.org/tplgy/cppcodec.png)](https://travis-ci.org/tplgy/cppcodec) [![Build status](https://ci.appveyor.com/api/projects/status/github/tplgy/cppcodec?branch=master&svg=true)](https://ci.appveyor.com/project/efidler/cppcodec)

Header-only C++11 library to encode/decode base64, base64url, base32, base32hex
and hex (a.k.a. base16) as specified in RFC 4648, plus Crockford's base32.

MIT licensed with consistent, flexible API. Supports raw pointers,
`std::string` and (templated) character vectors without unnecessary allocations.
Cross-platform with measured decent performance and without compiler warnings.

## Contents
- ### [Usage](https://github.com/tplgy/cppcodec/blob/master/README.md#usage)
- ### [Variants](https://github.com/tplgy/cppcodec/blob/master/README.md#variants)
  - #### [base64](https://github.com/tplgy/cppcodec/blob/master/README.md#base64)
  - #### [base32](https://github.com/tplgy/cppcodec/blob/master/README.md#base32)
  - #### [hex](https://github.com/tplgy/cppcodec/blob/master/README.md#hex)
- ### [Philosophy and trade-offs](https://github.com/tplgy/cppcodec/blob/master/README.md#philosophy-and-trade-offs)
- ### [API](https://github.com/tplgy/cppcodec/blob/master/README.md#api)
  - #### [Encoding](https://github.com/tplgy/cppcodec/blob/master/README.md#encoding)
  - #### [Decoding](https://github.com/tplgy/cppcodec/blob/master/README.md#decoding)

****

# Usage

1. Import cppcodec into your project (copy, git submodule, etc.)
2. Add the cppcodec root directory to your build system's list of include directories
3. Include headers and start using the API.

Since cppcodec is a header-only library, no extra build step is needed.
Alternatively, you can install the headers and build extra tools/tests with CMake.



# Variants

A number of codec variants exist for base64 and base32, defining different alphabets
or specifying the use of padding and line breaks in different ways. cppcodec is designed
to let you make a conscious choice about which one you're using, see below for a list of variants.

cppcodec's approach is to implement encoding/decoding algorithms in different classes for namespacing (e.g. `cppcodec::base64_rfc4648`), with classes and their associated header files named verbatim after the codec variants.

Here is an expected standard use of cppcodec:

```C++
#include <cppcodec/base32_crockford.hpp>
#include <cppcodec/base64_rfc4648.hpp>
#include <iostream>

int main() {
  using base32 = cppcodec::base32_crockford;
  using base64 = cppcodec::base64_rfc4648;

  std::vector<uint8_t> decoded = base64::decode("YW55IGNhcm5hbCBwbGVhc3VyZQ==");
  std::cout << "decoded size (\"any carnal pleasure\"): " << decoded.size() << '\n';
  std::cout << base32::encode(decoded) << std::endl; // "C5Q7J833C5S6WRBC41R6RSB1EDTQ4S8"
  return 0;
}
```

(The prior example included "baseXX_default_*.h" includes, these are not recommended anymore and may eventually get deprecated.)

Currently supported codec variants are:

### base64

* `base64_rfc4648` uses the PEM/MIME/UTF-7 alphabet, that is (in order)
  A-Z, a-z, 0-9 plus characters '+' and '/'. This is what's usually considered
  the "standard base64" that you see everywhere and requires padding ('=') but
  no line breaks. Whitespace and other out-of-alphabet symbols are regarded as
  a parse error.
* `base64_url` is the same as `base64_rfc4648` (and defined in the same RFC)
  but uses '-' (minus) and '_' (underscore) as special characters instead of
  '+' and '/'. This is safe to use for URLs and file names. Padding with '=' is
  required, it will be generated when encoding to a string and regarded as a
  parse error if it's not present when decoding.
* `base64_url_unpadded` variant is the same as `base64_url`, but '=' padding
  characters are optional. When encoding, no padding will be appended to the
  resulting string. Decoding accepts either padded or unpadded strings.

### base32

All base32 variants encode 5 bits as one (8-bit) character, which results in
an encoded length of roughly 160% (= 8/5). Their selling point is mainly
case-insensitive decoding, no special characters and alphabets that can be
communicated via phone.

* `base32_rfc4648` implements the popular, standardized variant defined in
  RFC 4648. It uses the full upper-case alphabet A-Z for the first 26 values
  and the digit characters 2-7 for the last ten. Padding with '=' is required
  and makes the encoded string a multiple of 8 characters. The codec accepts
  no invalid symbols, so if you want to let the user enter base32 data then
  consider replacing numbers '0', '1' and '8' with 'O', 'I' and 'B' on input.
* `base32_crockford` implements [Crockford base32](http://www.crockford.com/wrmg/base32.html).
  It's less widely used than the RFC 4648 alphabet, but offers a more carefully
  picked alphabet and also defines decoding similar characters 'I', 'i', 'L'
  'l' as '1' plus 'O' and 'o' as '0' so no care is required for user input.
  Crockford base32 does not use '=' padding. Checksums are not implemented.
  Note that the specification is ambiguous about whether to pad bit quintets to
  the left or to the right, i.e. whether the codec is a place-based single number
  encoding system or a concatenative iterative stream encoder. This codec variant
  picks the streaming interpretation and thus zero-pads on the right. (See
  http://merrigrove.blogspot.ca/2014/04/what-heck-is-base64-encoding-really.html
  for a detailed discussion of the issue.)
* `base32_hex` is the logical extension of the hexadecimal alphabet, and also
  specified in RFC 4648. It uses the digit characters 0-9 for the first 10 values
  and the upper-case letters A-V for the remaining ones. The alphabet is
  conceptually simple, but contains all of the ambiguous number/letter pairs that
  the other variants try to avoid. It is also less suitable for verbal
  transmission. Padding with '=' is required and makes the encoded string a
  multiple of 8 characters.

### hex

* `hex_upper` outputs upper-case letters and accepts lower-case as well.
  This is an octet-streaming codec variant and for decoding, requires an even
  number of input symbols. In other words, don't try to decode (0x)"F",
  (0x)"10F" etc. with this variant, use a place-based single number codec
  instead if you want to do this. Also, you are expected to prepend and remove
  a "0x" prefix externally as it won't be generated when encoding / will be
  rejected when decoding.
* `hex_lower` outputs lower-case letters and accepts upper-case as well.
  Similar to `hex_upper`, it's stream-based (no odd symbol lengths) and does
  not deal with "0x" prefixes.



# Philosophy and trade-offs

cppcodec aims to support a range of codecs using a shared template-based implementation.
The focus is on a high-quality API that encourages correct use, includes error handling,
and is easy to adopt into other codebases. As a header-only library, cppcodec can
ship implementations of several codecs and variants while only compiling the ones
that you actually use.

Good performance is a goal, but not the topmost priority. In theory, templates allows
to write generic code that is optimized for each specialization individually; however,
in practice compilers still struggle to produce code that's as simple as a
hand-written specialized function. On release builds, depending on the C++ compiler,
cppcodec runs in between (approx.) 100% and 300% of time compared to "regular" optimized
base64 implementations. Both are beat by highly optimized implementations that use
vector instructions (such as [this](https://github.com/aklomp/base64)) or buy better
performance with larger pre-computed tables (such as Chrome's base64 implementation).
Debug builds of cppcodec are slower by an order of magnitude due to the use of templates
and abstractions; make sure you use release or minimum-size builds in production.



# API

All codecs expose the same API. In the below documentation, replace `<codec>` with a
default alias such as `base64`, `base32` or `hex`, or with the full namespace such as
`cppcodec::base64_rfc4648` or `cppcodec::base32_crockford`.

For templated parameters `T` and `Result`, you can use e.g. `std::vector<uint8_t>`,
`std::string` or anything that supports:
* `.data()` and `.size()` for `T` (read-only) template parameters,
* for `Result` template parameters, also `.reserve(size_t)`, `.resize(size_t)`
  and `.push_back([uint8_t|char])`.

It's possible to support types lacking these functions, consult the code directly if you need this.


### Encoding

```C++
// Convenient version, returns an std::string.
std::string <codec>::encode(const [uint8_t|char]* binary, size_t binary_size);
std::string <codec>::encode(const T& binary);

// Convenient version with templated result type.
Result <codec>::encode<Result>(const [uint8_t|char]* binary, size_t binary_size);
Result <codec>::encode<Result>(const T& binary);

// Reused result container version. Resizes encoded_result before writing to it.
void <codec>::encode(Result& encoded_result, const [uint8_t|char]* binary, size_t binary_size);
void <codec>::encode(Result& encoded_result, const T& binary);
```

Encode binary data into an encoded (base64/base32/hex) string.
Won't throw by itself, but the result type might throw on `.resize()`.

```C++
size_t <codec>::encode(char* encoded_result, size_t encoded_buffer_size, const [uint8_t|char]* binary, size_t binary_size) noexcept;
size_t <codec>::encode(char* encoded_result, size_t encoded_buffer_size, const T& binary) noexcept;
```

Encode binary data into pre-allocated memory with a buffer size of
`<codec>::encoded_size(binary_size)` or larger.

Returns the byte size of the encoded string excluding null termination,
which is equal to `<codec>::encoded_size(binary_size)`.

If `encoded_buffer_size` is larger than required, a single null termination character (`'\0'`)
is written after the last encoded character. The `encoded_size()` function ensures that the required
buffer size is large enough to hold the padding required for the respective codec variant.
Provide a buffer of size `encoded_size() + 1` to make it a null-terminated C string.

Calls abort() if `encoded_buffer_size` is insufficient. (That way, the function can remain `noexcept`
rather than throwing on an entirely avoidable error condition.)

```C++
size_t <codec>::encoded_size(size_t binary_size) noexcept;
```

Calculate the (exact) length of the encoded string based on binary size,
excluding null termination but including padding (if specified by the codec variant).


### Decoding

```C++
// Convenient version, returns an std::vector<uint8_t>.
std::vector<uint8_t> <codec>::decode(const char* encoded, size_t encoded_size);
std::vector<uint8_t> <codec>::decode(const T& encoded);

// Convenient version with templated result type.
Result <codec>::decode<Result>(const char* encoded, size_t encoded_size);
Result <codec>::decode<Result>(const T& encoded);

// Reused result container version. Resizes binary_result before writing to it.
void <codec>::decode(Result& binary_result, const char* encoded, size_t encoded_size);
void <codec>::decode(Result& binary_result, const T& encoded);
```

Decode an encoded (base64/base32/hex) string into a binary buffer.

Throws a cppcodec::parse_error exception (inheriting from std::domain_error)
if the input data does not conform to the codec variant specification.
Also, the result type might throw on `.resize()`.

```C++
size_t <codec>::decode([uint8_t|char]* binary_result, size_t binary_buffer_size, const char* encoded, size_t encoded_size);
size_t <codec>::decode([uint8_t|char]* binary_result, size_t binary_buffer_size, const T& encoded);
```

Decode an encoded string into pre-allocated memory with a buffer size of
`<codec>::decoded_max_size(encoded_size)` or larger.

Returns the byte size of the decoded binary data, which is less or equal to
`<codec>::decoded_max_size(encoded_size)`.

Calls abort() if `binary_buffer_size` is insufficient, for consistency with encode().
Throws a cppcodec::parse_error exception (inheriting from std::domain_error)
if the input data does not conform to the codec variant specification.

```C++
size_t <codec>::decoded_max_size(size_t encoded_size) noexcept;
```

Calculate the maximum size of the decoded binary buffer based on the encoded string length.

If the codec variant does not allow padding or whitespace / line breaks,
the maximum decoded size will be the exact decoded size.

If the codec variant allows padding or whitespace / line breaks, the actual decoded size
might be smaller. If you're using the pre-allocated memory result call, make sure to take
its return value (the actual decoded size) into account.
