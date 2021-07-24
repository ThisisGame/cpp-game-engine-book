Introduction
============

TinyFSM is a simple finite state machine library for C++, designed for
optimal performance and low memory footprint. This makes it ideal for
real-time operating systems. The concept is very simple, allowing the
programmer to fully understand what is happening behind the scenes. It
provides a straightforward way of mapping your state machine charts
into source code.

TinyFSM basically wraps event dispatching into function calls, making
event dispatching equally fast to calling (or even inlining) a
function. Even in the worst case, dispatching leads to nothing more
than a single vtable lookup and function call!

Key Features
------------

- Entry/exit actions
- Event actions
- Transition functions
- Transition conditions
- Event payload (classes)
- Inheritance of states and action functions

TinyFSM benefits from the C++11 template metaprogramming features like
variadic templates, and does not depend on RTTI, exceptions or any
external library.
