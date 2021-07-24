/*
 This file is part of the Tweeny library.

 Copyright (c) 2016-2021 Leonardo Guilherme Lucena de Freitas
 Copyright (c) 2016 Guilherme R. Costa

 Permission is hereby granted, free of charge, to any person obtaining a copy of
 this software and associated documentation files (the "Software"), to deal in
 the Software without restriction, including without limitation the rights to
 use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 the Software, and to permit persons to whom the Software is furnished to do so,
 subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/**
 * @file tweeny.h
 * This file is the main header file for Tweeny. You should not need to include anything else.
 */

/**
 * @mainpage Tweeny
 *
 * Tweeny is an inbetweening library designed for the creation of complex animations for games and other beautiful
 * interactive software. It leverages features of modern C++ to empower developers with an intuitive API for
 * declaring tweenings of any type of value, as long as they support arithmetic operations.
 *
 * This document contains Tweeny's API reference. The most interesting parts are:
 *
 * * The Fine @ref manual
 * * The tweeny::from global function, to start a new tween.
 * * The tweeny::tween class itself, that has all the interesting methods for a tween.
 * * The <a href="modules.html">modules page</a> has a list of type of easings.
 *
 * This is how the API looks like:
 *
 * @code
 *
 * #include "tweeny.h"
 *
 * using tweeny::easing;
 *
 * int main() {
 *  // steps 1% each iteration
 *  auto tween = tweeny::from(0).to(100).during(100).via(easing::linear);
 *  while (tween.progress() < 1.0f) tween.step(0.01f);
 *
 *  // a tween with multiple values
 *  auto tween2 = tweeny::from(0, 1.0f).to(1200, 7.0f).during(1000).via(easing::backInOut, easing::linear);
 *
 *  // a tween with multiple points, different easings and durations
 *  auto tween3 = tweeny::from(0, 0)
 *                  .to(100, 100).during(100).via(easing::backOut, easing::backOut)
 *                  .to(200, 200).during(500).via(easing::linear);
 *  return 0;
 * }
 *
 * @endcode
 *
 * **Examples**
 *
 * * Check <a href="http://github.com/mobius3/tweeny-demos">tweeny-demos</a> repository to see demonstration code
 *
 * **Useful links and references**
 * * <a href="http://www.timotheegroleau.com/Flash/experiments/easing_function_generator.htm">Tim Groleau's easing function generator (requires flash)</a>
 * * <a href="http://easings.net/">Easing cheat sheet (contains graphics!)</a>
 */

#ifndef TWEENY_H
#define TWEENY_H

#include "tween.h"
#include "easing.h"

/**
 * @brief The tweeny namespace contains all symbols and names for the Tweeny library.
 */
namespace tweeny {
  /**
   * @brief Creates a tween starting from the values defined in the arguments.
   *
   * Starting values can have heterogeneous types, even user-defined types, provided they implement the
   * four arithmetic operators (+, -, * and /). The types used will also define the type of each next step, the type
   * of the callback and the type of arguments the passed easing functions must have.
   *
   * @sa tweeny::tween
   */
    template<typename... Ts> tween<Ts...> from(Ts... vs);
}

#include "tweeny.tcc"

#endif //TWEENY_TWEENY_H
