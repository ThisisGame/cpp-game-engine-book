# Tweeny
<a href="https://repology.org/project/tweeny/versions">
    <img src="https://repology.org/badge/vertical-allrepos/tweeny.svg" alt="Packaging status" align="right" style="padding-left: 20px">
</a>

Tweeny is an inbetweening library designed for the creation of complex animations for games and other beautiful interactive software. It leverages features of modern C++ to empower developers with an intuitive API for declaring tweenings of any type of value, as long as they support arithmetic operations.

The goal of Tweeny is to provide means to create fluid interpolations when animating position, scale, rotation, frames or other values of screen objects, by setting their values as the tween starting point and then, after each tween step, plugging back the result.

**It features**:

- A descriptive and (hopefully) intuitive API,
- 30+ easing functions,
- Allows custom easing functions,
- Multi-point tweening,
- Simultaneous tween of heterogeneous value sets,
- Timeline-like usage (allows seeking to any point),
- Header-only
- Zero external dependencies
- Steps forwards or backwards :)
- Accepts lambdas, functors and functions as step and seek callbacks

**Obligatory hello world example**:

Linearly interpolate character by character from the word *hello* to *world* in `50` steps:

```cpp
auto helloworld = tweeny::from('h','e','l','l','o').to('w','o','r','l','d').during(50);
for (int i = 0; i < 50; i++) {
    for (char c : helloworld.step(1)) { printf("%c", c); }
    printf("\n");
}
```

Relevant code:

- **1**: create the tween instance starting with characters of the `hello` word, adds a tween target with the chars of the `world` word and specify it should reach it in `50` steps.
- **3**: move the tween forward by one step. Use the return value of it (which ill be a `std::array<char, 5>` in this case) to set up a for loop iterating in each char, printing it.

## Installation methods:

**Using your package manager**

There are some packages for tweeny made by some great people. Repology has a list of them and their versions [here](https://repology.org/metapackage/tweeny/versions). Thanks, great people!

**Not installing it**

You just need to adjust your include path to point to the `include/` folder after you've cloned this repository.

**Copying the `include` folder:**

Tweeny itself is a header only library. You can copy the `include/` folder into your project folder and then include from it: `#include "tweeny/tweeny.h"`

**Copying the `tweeny-<version>.h` header**

Since version 3.1.1 tweeny releases include a single-header file with all the necessary code glued together. Simply drop it on your project and/or adjust the include path and then `#include "tweeny-3.1.1.h"`.

**CMake subproject**

This is useful if you are using CMake already. Copy the whole tweeny project and include it in a top-level `CMakeLists.txt` file and then use `target_link_libraries` to add it to your target:

```
add_subdirectory(tweeny)
target_link_libraries(yourtarget tweeny)
```
This will add the `include/` folder to your search path, and you can `#include "tweeny.h"`.

## Doxygen documentation

This library is documented using Doxygen. If you intend to generate docs, specify the flag `TWEENY_BUILD_DOCUMENTATION` when generating CMake build files (e.g: `cmake .. -DTWEENY_BUILD_DOCUMENTATION=1`). You will need doxygen installed.

## Contributing

Tweeny is open-source, meaning that it is open to modifications and contrubutions from the community (you are very much encouraged to do so!). However, we'd appreciate if you follow these guidelines:

- Don't use `PascalCase` nor `snake_case` in names
- Use `camelCase`, but try to avoid multi word names as hard as possible
- Document code using Doxygen
- Implementation details should go inside `tweeny::detail` namespace.
- Template implementations should go into a `.tcc` file

## Examples:

Demo code showcasing some of Tweeny features can be seen in the [tweeny-demo](https://github.com/mobius3/tweeny-demos) repository. This repository also has instructions on how to build them.