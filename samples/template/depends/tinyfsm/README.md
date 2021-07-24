TinyFSM
=======

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

Key Features:

 - Entry/exit actions
 - Event actions
 - Transition functions
 - Transition conditions
 - Event payload (classes)
 - Inheritance of states and action functions

TinyFSM benefits from the C++11 template metaprogramming features like
variadic templates, and does not depend on RTTI, exceptions or any
external library.


Official home page: <https://digint.ch/tinyfsm>

Current version: `0.3.2`


Documentation
-------------

You can find the main documentation in the `doc/` directory of the
TinyFSM project. The latest version is also available
[online](https://digint.ch/tinyfsm/doc/introduction.html).


Installation
------------

TinyFSM is a header-only library, no special installation steps are
needed. Just point your compiler to the "include" directory.


Donate
------

So TinyFSM has proven useful for you?

I will definitively continue developing TinyFSM for free, but if you
want to support me you can do so:

[![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=QZQE9HY6QHDHS)


Development
-----------

The source code for TinyFSM is managed using Git:

    git clone https://dev.tty0.ch/tinyfsm.git

Mirror on GitHub:

    git clone https://github.com/digint/tinyfsm.git

If you would like to contribute or have found bugs, visit the [TinyFSM
project page on GitHub] and use the [issues tracker] there.

  [TinyFSM project page on GitHub]: http://github.com/digint/tinyfsm
  [issues tracker]: http://github.com/digint/tinyfsm/issues


Contact
-------

For questions and suggestions regarding TinyFSM, success or failure
stories, and any other kind of feedback, please feel free to contact
the author (the email address can be found in the sources).


License
-------

TinyFSM is [Open Source] software. It may be used for any purpose,
including commercial purposes, at absolutely no cost. It is
distributed under the terms of the [MIT license].

  [Open Source]: http://www.opensource.org/docs/definition.html
  [MIT license]: http://www.opensource.org/licenses/mit-license.html
