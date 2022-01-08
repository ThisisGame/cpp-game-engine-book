/**
Lightweight profiler library for c++
Copyright(C) 2016-2019  Sergey Yagovtsev, Victor Zarubkin

Licensed under either of
    * MIT license (LICENSE.MIT or http://opensource.org/licenses/MIT)
    * Apache License, Version 2.0, (LICENSE.APACHE or http://www.apache.org/licenses/LICENSE-2.0)
at your option.

The MIT License
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
    of the Software, and to permit persons to whom the Software is furnished
    to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
    INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
    PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
    LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
    USE OR OTHER DEALINGS IN THE SOFTWARE.


The Apache License, Version 2.0 (the "License");
    You may not use this file except in compliance with the License.
    You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

**/

#ifndef COMPLEXITY_CALCULATOR_H
#define COMPLEXITY_CALCULATOR_H

#include <cmath>
#include <functional>
#include <map>
#include <numeric>
#include <vector>

enum class ComplexityType : uint8_t
{
    Constant = 0, ///< O(1)
    Logarithmic,  ///< O(logN)
    Linear,       ///< O(N)
    Quasilinear,  ///< O(N*logN)
    Quadratic,    ///< O(N^2)
    Cubic,        ///< O(N^3)
    Exponential,  ///< O(2^N)
    Factorial,    ///< O(N!)
    Unknown,      ///< cannot estimate
};

template<class TValue>
TValue getAverage(const std::vector<TValue>& derivatives)
{
    TValue result = std::accumulate(derivatives.begin(), derivatives.end(), TValue(0.0), [](TValue a, TValue b)
    {
        if (std::isnormal(b))
            return a + b;
        return a;
    });

    return result / TValue(derivatives.size());
}

template<class TKey, class TValue>
std::vector<TValue> calculateDerivatives(const std::map<TKey, TValue>& input_array)
{
    std::vector<TValue> result;

    for (auto it = input_array.cbegin(), next_it = input_array.cbegin()++; next_it != input_array.cend(); it = next_it, ++next_it)
    {
        const auto x0 = it->first;
        const auto x1 = next_it->first;

        const auto y0 = it->second;
        const auto y1 = next_it->second;

        result.push_back((y1 - y0) / (x1 - x0));
    }

    return result;
}

template<class TKey, class TValue>
std::map<TValue, TValue> getLogarithmicChart(const std::map<TKey, std::vector<TValue> >& input)
{
    std::map<TValue, TValue> result;

    for (auto it : input)
    {
        result[static_cast<TValue>(std::log2(it.first))] = std::log2(getAverage(it.second));
    }

    return result;
}

template <class TKey, class TValue>
ComplexityType estimateComplexity(const std::map<TKey, std::vector<TValue> >& input)
{
    const auto average = getAverage(calculateDerivatives(getLogarithmicChart(input)));
    const double estimate_angle = std::atan(double(average)) * 57.3;

    if (estimate_angle < 1)
        return ComplexityType::Constant;

    if (estimate_angle < 10)
        return ComplexityType::Logarithmic;

    if (estimate_angle < 30)
        return ComplexityType::Linear;

    if (estimate_angle < 50)
        return ComplexityType::Quasilinear;

    if (estimate_angle < 65)
        return ComplexityType::Quadratic;

    if (estimate_angle < 70)
        return ComplexityType::Cubic;

    if (estimate_angle < 85)
        return ComplexityType::Exponential;

    return ComplexityType::Factorial;
}

#endif // COMPLEXITY_CALCULATOR_H
