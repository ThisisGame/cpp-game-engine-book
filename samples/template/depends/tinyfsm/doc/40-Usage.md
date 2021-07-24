Usage
=====

Refer to the [API examples](/examples/api/) provided with the TinyFSM
package for a quick overview. Recommended starting points:

 - [Simple Switch](/examples/api/simple_switch.cpp)
 - [Moore Machine](/examples/api/moore_machine.cpp)
 - [Elevator Project](/examples/elevator/)

For an example in an RTOS environment, see the [stm32f103stk-demo] of
the [OpenMPTL] project. Starting points:

 - [screen.hpp](https://github.com/digint/openmptl/tree/master/projects/stm32f103stk-demo/src/screen.hpp)
   : TinyFSM declarations.
 - [kernel.cpp](https://github.com/digint/openmptl/tree/master/projects/stm32f103stk-demo/src/kernel.cpp)
   : Poll input and trigger events.

  [OpenMPTL]: https://digint.ch/openmptl/
  [stm32f103stk-demo]: https://github.com/digint/openmptl/tree/master/projects/stm32f103stk-demo


The examples in the documentation below are mainly based on the
"Elevator Project" example.


###  1. Declare Events

Declare events that your state machine will listen to. Events are
classes derived from the tinyfsm::Event class.

Example:

    struct FloorEvent : tinyfsm::Event
    {
      int floor;
    };

    struct Call        : FloorEvent { };
    struct FloorSensor : FloorEvent { };
    struct Alarm       : tinyfsm::Event { };

In the example above, we declare three events. Note that events are
regular classes, which are passed as arguments to the react() members
of a state class. In this example, we use a member variable "floor",
which is used to specify the floor number on "Call" and "FloorSensors"
events.


###  2. Declare the State Machine Class

Declare your state machine class. State machines are classes derived
from the tinyfsm::Fsm template class, where T is the type name of the
state machine itself.

You need to declare the following public members:

 - react() function for each event
 - entry() and exit() functions

Example:

    class Elevator
    : public tinyfsm::Fsm<Elevator>
    {
    public:
      /* default reaction for unhandled events */
      void react(tinyfsm::Event const &) { };
    
      virtual void react(Call        const &);
      virtual void react(FloorSensor const &);
      void         react(Alarm       const &);
    
      virtual void entry(void) { };  /* entry actions in some states */
      void         exit(void)  { };  /* no exit actions */
    };


Note that you are free to declare the functions non-virtual if you
like. This has implications on the execution speed: In the example
above, the react(Alarm) function is declared non-virtual, as all states
share the same reaction for this event. This makes code execution
faster when dispatching the "Alarm" event, since no vtable lookup is
needed.


###  3. Declare the States

Declare the states of your state machine. States are classes derived
from the state machine class.

Note that state classes are *implicitly instantiated*. If you want to
reuse states in multiple state machines, you need to declare them as
templates (see `/examples/api/multiple_switch.cpp`).

Example:

    class Panic
    : public Elevator
    {
      void entry() override;
    };
    
    class Moving
    : public Elevator
    {
      void react(FloorSensor const &) override;
    };
    
    class Idle
    : public Elevator
    {
      void entry() override;
      void react(Call const & e) override;
    };


In this example, we declare three states. Note that the "elevator"
example source code does not declare the states separately, but rather
defines the code directly in the declaration.


###  4. Implement Actions and Event Reactions

In most cases, event reactions consist of one or more of the following
steps:

 - Change some local data
 - Send events to other state machines
 - Transit to different state

**Important**:
Make sure that the `transit<>()` function call is the last command
executed within a reaction function!

**Important**:
Don't use `transit<>()` in entry/exit actions!

Example:

    void Idle::entry() {
      send_event(MotorStop());
    }
    
    void Idle::react(Call const & e) {
      dest_floor = e.floor;
    
      if(dest_floor == current_floor)
        return;
    
      /* lambda function used for transition action */
      auto action = [] { 
        if(dest_floor > current_floor)
          send_event(MotorUp());
        else if(dest_floor < current_floor)
          send_event(MotorDown());
      };
    
      transit<Moving>(action);
    };


In this example, we use a lambda function as transition action. The
`transit<>()` function does the following:

 1. Call the exit() function of the current state
 2. Call the the transition action if provided
 3. Change the current state to the new state
 4. Call the entry() function of the new state

Note that you can also pass condition functions to the `transit<>()`
function.


###  5. Define the Initial State

Use the macro `FSM_INITIAL_STATE(fsm, state)` for defining the initial
state (or "start state") of your state machine:

Example:

    FSM_INITIAL_STATE(Elevator, Idle)

This sets the current state of the "Elevator" state machine to "Idle".
More specifially, it defines a template specialization for
`Fsm<Elevator>::set_initial_state()`, setting the current state to
Idle.


###  6. Define Custom Initialization

If you need to perform custom initialization, you can override the
reset() member function in your state machine class. If you are using
state variables, you can re-instantiate your states by calling
`tinyfsm::StateList<MyStates...>::reset()`.

Example:

    class Switch : public tinyfsm::Fsm<Switch>
    {
      public: static void reset(void) {
        tinyfsm::StateList<Off, On>::reset();  // reset all states
        myvar = 0;
        ...
      }
      ...
    }

Make sure to always set the current state, or you'll end up with a
null pointer dereference.


###  7. Use FsmList for Event Dispatching

You might have noticed some calls to a send_event() function in the
example above. This is NOT a function provided with TinyFSM. Since
event dispatching can be implemented in several ways, TinyFSM leaves
this open to you. The "elevator" example implements the send_event()
function as *direct event dispatching*, without using event
queues. This has the advantage that execution is much faster, since no
RTTI is needed and the decision which function to call for an event
class is made at compile-time. On the other hand, special care has to
be taken when designing the state machines, in order to avoid loops.

Code from "fsmlist.hpp":

    typedef tinyfsm::FsmList<Motor, Elevator> fsm_list;
    
    template<typename E>
    void send_event(E const & event)
    {
      fsm_list::template dispatch<E>(event);
    }

Here, send_event() dispatches events to all state machines in the
list. It is important to understand that this approach comes with no
performance penalties at all, as long as the default reaction is
defined empty within the state machine declaration.
