/**
 *  This is free and unencumbered software released into the public domain.
 *  Anyone is free to copy, modify, publish, use, compile, sell, or
 *  distribute this software, either in source code form or as a compiled
 *  binary, for any purpose, commercial or non-commercial, and by any
 *  means.
 *
 *  In jurisdictions that recognize copyright laws, the author or authors
 *  of this software dedicate any and all copyright interest in the
 *  software to the public domain. We make this dedication for the benefit
 *  of the public at large and to the detriment of our heirs and
 *  successors. We intend this dedication to be an overt act of
 *  relinquishment in perpetuity of all present and future rights to this
 *  software under copyright law.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 *  OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 *  ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *  OTHER DEALINGS IN THE SOFTWARE.
 *
 *  For more information, please refer to <http://unlicense.org/>
 */

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
