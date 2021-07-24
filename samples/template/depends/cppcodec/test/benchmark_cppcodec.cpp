/**
 *  Copyright (C) 2018 Jakob Petsovits
 *  All rights reserved.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 *  deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 *  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 *  IN THE SOFTWARE.
 */

#include <cppcodec/hex_lower.hpp>
#include <cppcodec/base32_rfc4648.hpp>
#include <cppcodec/base64_rfc4648.hpp>

#include <chrono>
#include <iostream>
#include <iomanip>
#include <random>
#include <stdint.h>
#include <string>
#include <vector>

#define BENCHMARK_ENCODING_STR true
#define BENCHMARK_DECODING_STR true
#define BENCHMARK_DECODING_VEC_U8 true

const size_t max_iterations = 1000000; // 1m iterations ought to be enough for anybody
const size_t iteration_max_ms = 500; // half a second

uint8_t random_uint8()
{
    static std::random_device rd;
    static std::mt19937 pseudo_random(rd());
    static std::uniform_int_distribution<int> dist(0, 255);
    return static_cast<uint8_t>(dist(pseudo_random));
}

template <typename Codec>
void benchmark(std::ostream& stream, const std::vector<size_t>& decoded_sizes)
{
    using clock = std::chrono::high_resolution_clock;

    // Measure decoding into both uint8_t and string.
    std::vector<double> time_encoding_str(decoded_sizes.size());
    std::vector<double> time_decoding_vec_u8(decoded_sizes.size());
    std::vector<double> time_decoding_str(decoded_sizes.size());
    std::vector<std::vector<uint8_t>> decoded_vec_u8(decoded_sizes.size());
    std::vector<std::string> decoded_str(decoded_sizes.size());
    std::vector<std::string> encoded_str(decoded_sizes.size());

    for (size_t i = 0; i < decoded_sizes.size(); ++i) {
        decoded_vec_u8[i].resize(decoded_sizes[i]);
        for (size_t j = 0; j < decoded_sizes[i]; ++j) {
            decoded_vec_u8[i][j] = random_uint8();
        }
    }

    auto flags = stream.flags();
    auto precision = stream.precision();
    stream << std::fixed << std::setprecision(4);

#if BENCHMARK_ENCODING_STR
    stream << "Encoding:\n";

    for (size_t i = 0; i < decoded_sizes.size(); ++i) {
        encoded_str[i] = Codec::encode(decoded_vec_u8[i]);

        clock::time_point start = clock::now();
        clock::time_point end = start + std::chrono::milliseconds(iteration_max_ms);
        size_t j = 0;
        for (; j < max_iterations; ++j) {
            if (clock::now() > end) {
                break;
            }
            encoded_str[i] = Codec::encode(decoded_vec_u8[i]);
        }
        time_encoding_str[i] = std::chrono::duration_cast<std::chrono::microseconds>(
                clock::now() - start).count() / static_cast<double>(j);

        stream << (i == 0 ? "" : "\t") << decoded_sizes[i] << ": "
                << time_encoding_str[i] << std::flush;
    }
    stream << "\n";
#else
    // Even if we're not benchmarking encoding, we still need the encoded strings.
    for (size_t i = 0; i < decoded_sizes.size(); ++i) {
        encoded_str[i] = Codec::encode(decoded_vec_u8[i]);
    }
#endif // BENCHMARK_ENCODING_STR

#if BENCHMARK_DECODING_STR
    stream << "Decoding to string:\n";

    for (size_t i = 0; i < decoded_sizes.size(); ++i) {
        decoded_str[i] = std::string();
        clock::time_point start = clock::now();
        clock::time_point end = start + std::chrono::milliseconds(iteration_max_ms);
        size_t j = 0;
        for (; j < max_iterations; ++j) {
            if (clock::now() > end) {
                break;
            }
            decoded_str[i] = Codec::template decode<std::string>(encoded_str[i]);
        }
        time_decoding_str[i] = std::chrono::duration_cast<std::chrono::microseconds>(
                clock::now() - start).count() / static_cast<double>(j);

        stream << (i == 0 ? "" : "\t") << decoded_sizes[i] << ": "
                << time_decoding_str[i] << std::flush;
    }

    stream << "\n";
#endif // BENCHMARK_DECODING_STR

#if BENCHMARK_DECODING_VEC_U8
    stream << "Decoding to vector<uint8_t>:\n";

    for (size_t i = 0; i < decoded_sizes.size(); ++i) {
        decoded_vec_u8[i] = std::vector<uint8_t>();
        clock::time_point start = clock::now();
        clock::time_point end = start + std::chrono::milliseconds(iteration_max_ms);
        size_t j = 0;
        for (; j < max_iterations; ++j) {
            if (clock::now() > end) {
                break;
            }
            decoded_vec_u8[i] = Codec::decode(encoded_str[i]);
        }
        time_decoding_vec_u8[i] = std::chrono::duration_cast<std::chrono::microseconds>(
                clock::now() - start).count() / static_cast<double>(j);

        stream << (i == 0 ? "" : "\t") << decoded_sizes[i] << ": "
                << time_decoding_vec_u8[i] << std::flush;
    }

    stream << "\n";
#endif // BENCHMARK_DECODING_VEC_U8

    stream << std::setprecision(precision) << "\n";
    stream.flags(flags);
}

int main()
{
    std::vector<size_t> decoded_sizes = {
        1, 4, 8, 16, 32, 64, 128, 256, 2048, 4096, 32768
    };
    std::cout << "base64_rfc4648: [decoded size: microseconds]\n";
    benchmark<cppcodec::base64_rfc4648>(std::cout, decoded_sizes);
    return 0;
}
