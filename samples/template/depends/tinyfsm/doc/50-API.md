API Reference
=============

`#include <tinyfsm.hpp>`


Class Diagram
-------------
                                           .......
    +--------------------------------------:  T  :
    | tinyfsm::FsmList                     :.....:
    +-----------------------------------------|
    | [+] set_initial_state()     <<static>>  |
    | [+] reset()                 <<static>>  |
    | [+] enter()                 <<static>>  |
    | [+] start()                 <<static>>  |
    | [+] dispatch(Event)         <<static>>  |
    +-----------------------------------------+


                                           .......
    +--------------------------------------:  T  :
    | tinyfsm::Fsm                         :.....:
    +-----------------------------------------|
    | [+] state<S>()              <<static>>  |
    | [+] set_initial_state()     <<static>>  |
    | [+] reset()                 <<static>>  |
    | [+] enter()                 <<static>>  |
    | [+] start()                 <<static>>  |
    | [+] dispatch(Event)         <<static>>  |
    | [#] transit<S>()                        |
    | [#] transit<S>(Action)                  |
    | [#] transit<S>(Action, Condition)       |
    +-----------------------------------------+
                         #
                         |
                         |
              +---------------------+
              | MyFSM               |
              +---------------------+
              | [+] entry()         |
              | [+] exit()          |
              | [+] react(EventX)   |
              | [+] react(EventY)   |
              | ...                 |
              +---------------------+
                         #
                         |
           +-------------+-------------+
           |             |             |
      +---------+   +---------+   +---------+
      | State_A |   | State_B |   | ...     |
      +---------+   +---------+   +---------+


    [#]  protected
    [+]  public
    [-]  private


template< typename F > class Fsm
--------------------------------

### State Machine Functions

 * `template< typename S > static constexpr S & state(void)`

   Returns a reference to a (implicitly instantiated) state S. Allows
   low-level access to all states;


 * `static void set_initial_state(void)`

   Function prototype, must be defined (explicit template
   specialization) for every state machine class (e.g. by using the
   `FSM_INITIAL_STATE(fsm, state`) macro). Sets current state to
   initial (start) state.


 * `static void reset(void)`

   Empty function, can be overridden by state machine class in order to
   perform custom initialization (e.g. set static state machine
   variables, or reset states using `StateList<MyStates...>::reset()`)
   or directly via the `state<MyState>()` instance).

   Note that this function is NOT called on start().

   See example: `/examples/api/resetting_switch.cpp`

 * `static void enter(void)`

   Helper function, usually not needed to be used directly:
   calls entry() function of current state.


 * `static void start()`

   Sets the initial (start) state and calls its entry() function.


 * `template< typename E > static void dispatch(E const &)`

   Dispatch an event to the current state of this state machine.


### State Transition Functions

 * `template< typename S > void transit(void)`

   Transit to a new state:

   1. Call exit() function on current state
   2. Set new current state to S
   3. Call entry() function on new state


 * `template< typename S, typename ActionFunction > void transit(ActionFunction)`

   Transit to a new state, with action function:

   1. Call exit() function on current state
   2. Call ActionFunction
   3. Set new current state to S
   4. Call entry() function on new state


 * `template< typename S, typename ActionFunction, typename ConditionFunction > void transit(ActionFunction, ConditionFunction)`

   Transit to a new state only if ConditionFunction returns true.
   Shortcut for: `if(ConditionFunction()) transit<S>(ActionFunction);`.


### Derived Classes

#### template< typename F > class MooreMachine

Moore state machines have entry actions, but no exit actions:

 * `virtual void entry(void) { }`

   Entry action, not enforcing. Can be enforced by declaring pure
   virtual: `virtual void entry(void) = 0`

 * `void exit(void) { }`

   No exit actions.

See example: `/examples/api/more_machine.cpp`

#### template< typename F > class MealyMachine

Mealy state machines do not have entry/exit actions:

 * `void entry(void) { }`

   No entry actions.

 * `void exit(void) { }`

   No exit actions.

*Input actions* are modeled in react(), conditional dependent of event
type or payload and using `transit<>(ActionFunction)`.

See example: `/examples/api/mealy_machine.cpp`


template< typename... FF > struct FsmList
-----------------------------------------

 * `static void set_initial_state(void)`

   Calls set_initial_state() on all state machines in the list.


 * `static void reset()`

   Calls reset() on all state machines in the list.


 * `static void enter()`

   Calls enter() on all state machines in the list.


 * `static void start()`

   Sets the initial (start) state for all state machines in list, then
   call all entry() functions.


 * `template< typename E > static void dispatch(E const &)`

   Dispatch an event to the current state of all the state machines in
   the list.


template< typename... SS > struct StateList
-------------------------------------------

 * `static void reset(void)`

   Re-instantiate all states in the list, using copy-constructor.

   See example: `/examples/api/resetting_switch.cpp`
