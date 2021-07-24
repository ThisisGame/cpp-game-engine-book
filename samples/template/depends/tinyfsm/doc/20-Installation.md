Installation
============

TinyFSM is an header-only library, no special installation steps are
needed. Just point your compiler to the "include" directory, and in
your source files:

    #include <tinyfsm.hpp>


Prerequisites
-------------

TinyFSM requires a compiler supporting the C++11 language standard
("-std=c++11" in gcc).

TinyFSM does not depend on RTTI, exceptions or any external library.
If you need to compile without standard libraries (e.g. in conjunction
with `-nostdlib` linker option), add `-DTINYFSM_NOSTDLIB` to the
compiler options: this removes all dependencies on the standard
library by disabling some compile-time type checks.


Building the Elevator Example
-----------------------------

Change to the elevator example directory and compile the sources:

    $ cd examples/elevator
    $ make

Our elevator has call buttons on every floor, sensors reporting the
current position, and an alarm button for emergency. These actors can
be triggered via a simple command interface:

    $ ./elevator
    Motor: stopped
    Motor: stopped
    c=Call, f=FloorSensor, a=Alarm, q=Quit ?

Let's call the elevator to floor 2:

    c=Call, f=FloorSensor, a=Alarm, q=Quit ? c
    Floor ? 2
    Motor: moving up
    c=Call, f=FloorSensor, a=Alarm, q=Quit ?

Now the elevator is moving up, and we need to trigger the floor sensor:

    c=Call, f=FloorSensor, a=Alarm, q=Quit ? f
    Floor ? 1
    Reached floor 1
    c=Call, f=FloorSensor, a=Alarm, q=Quit ? f
    Floor ? 2
    Reached floor 2
    Motor: stopped
    c=Call, f=FloorSensor, a=Alarm, q=Quit ?

Now we simulate a sensor defect:

    c=Call, f=FloorSensor, a=Alarm, q=Quit ? c
    Floor ? 1
    Motor: moving down
    c=Call, f=FloorSensor, a=Alarm, q=Quit ? f
    Floor ? 2
    Floor sensor defect (expected 1, got 2)
    *** calling maintenance ***
    Motor: stopped
