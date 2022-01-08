Concepts
========

Keep it Simple
--------------

By design, TinyFSM implements only the very basics needed for
designing state machines. For many people, it is important to know
what a library is doing when making a decision for a specific library.


State Definition
----------------

States are derived classes from a base FSM state, providing react()
functions for every event, as well as entry() and exit() functions.


Event Dispatching
-----------------

TinyFSM does not hold state/event function tables like most other
state machine processors do. Instead, it keeps a pointer to the
current state (having the type of the state machine base
class). Dispatching an event simply calls the react() function of the
current state, with the event class as argument. This results in a
single vtable lookup and a function call, which is very efficient!

Event dispatching on an FsmList<> are simply dispatch() calls to all
state machines in the list.


Header-Only Library
-------------------

The TinyFSM library consist entirely of header files containing
templates, and requires no separately-compiled library binaries or
special treatment when linking.
