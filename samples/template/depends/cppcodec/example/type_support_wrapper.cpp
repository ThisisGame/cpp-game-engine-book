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

#include <cppcodec/base64_rfc4648.hpp>
#include <iostream>


// This example shows how to wrap a type (here: std::string) in another
// result type in order to modify the standard behavior for that type.
// Use this when you're in a position to wrap the result variable.
//
// This is the more straightforward of two ways for modifying cppcodec's
// behavior for a given type, the other one being able to modify the
// default behavior but also being more complex/intricate.
//
// The overall approach is straightforward: Define a result type with
// push_back(char) and size() methods, implement template specializations
// for init() and finish() for the result type, and call encode()/decode()
// with an object of this type as result parameter.

class string_append_wrapper
{
public:
    string_append_wrapper(std::string& backing)
        : m_backing(backing)
        , m_offset(0)
        , m_orig_size(0)
    {
    }

    void init(size_t capacity)
    {
        m_orig_size = m_backing.size();
        m_offset = m_orig_size;
        m_backing.resize(m_orig_size + capacity);
    }
    void finish()
    {
        m_backing.resize(m_offset);
    }

    // Methods required for satisfying default result type requirements:
    CPPCODEC_ALWAYS_INLINE void push_back(char c) { m_backing[m_offset++] = c; }
    CPPCODEC_ALWAYS_INLINE size_t size() const { return m_offset - m_orig_size; }

    // Note that the above implementation of push_back() is not the fastest,
    // because operator[] for std::string (for C++11 and above) still includes
    // a check for whether the size of the string fits into its allocation-less
    // character array union.
    //
    // With C++17 and above, it's legitimate to get the character array as a
    // mutable (non-const) char pointer, so this check can be skipped.
    // This is implemented via template specialization in cppcodec's
    // default behavior for std::string, but omitted here for simplicity.
    // If you need that last bit of extra performance, see
    // direct_data_access_result_state in cppcodec/data/access.hpp
    // for an example of optimal C++17 string access.

private:
    std::string& m_backing;
    size_t m_offset;
    size_t m_orig_size;
};


// init() and finish() must be declared in the cppcodec::data namespace.
namespace cppcodec {
namespace data {

template <> inline void init<string_append_wrapper>(
        string_append_wrapper& result, empty_result_state&, size_t capacity)
{
    // init() is called to prepare the output buffer. cppcodec will call it
    // with the maximum output size, null termination not included.
    //
    // Any thrown exception will not be caught by cppcodec itself,
    // the caller of the encode/decode function is responsible for handling it.
    //
    // empty_result_state can be ignored in this case because the wrapper type
    // can carry all required state internally.
    //
    // In order to maximize performance, init() should generally try to
    // allocate or guarantee the entire output buffer at once, so that
    // subsequent calls to push_back() don't result in extra checks (slower)
    // or even re-allocations.

    result.init(capacity);
}

// Between init() and finish(), cppcodec will call result.push_back(char)
// repeatedly, once for each output character with no rewinding.
// While init() can ask for greater capacity than the final output length,
// cppcodec guarantees that push_back() will never be called too often.
//
// (If you know exactly how long your output is, you could theoretically
// overcommit on capacity while allocating only the exact expected length
// of the output buffer. This is of course dangerous, because you can
// hardly ever know for sure and everyone's often wrong, so don't try it
// unless you have a business-critical reason to reduce/avoid the allocation.)

template <> inline void finish<string_append_wrapper>(
        string_append_wrapper& result, empty_result_state&)
{
    // finish() is called after encoding/decoding is done.
    // Its main purpose is to reduce the size of the result type
    // from capacity to the actual (often slightly smaller) output length.
    //
    // After finish(), cppcodec will assert that result.size() does indeed
    // equal the number of times that push_back() has been called.

    result.finish();
}

} // namespace data
} // namespace cppcodec


int main() {
    using base64 = cppcodec::base64_rfc4648;

    std::string result = "Result: ";
    string_append_wrapper appender(result);
    base64::encode(appender, std::string("any carnal pleasure"));
    std::cout << result << std::endl; // "Result: YW55IGNhcm5hbCBwbGVhc3VyZQ=="
    return 0;
}
