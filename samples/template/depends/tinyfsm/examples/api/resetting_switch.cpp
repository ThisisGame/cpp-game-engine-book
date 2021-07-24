#include <tinyfsm.hpp>
#include <iostream>

class Off; // forward declaration


// ----------------------------------------------------------------------------
// 1. Event Declarations
//
struct Toggle : tinyfsm::Event { };


// ----------------------------------------------------------------------------
// 2. State Machine Base Class Declaration
//
class Switch
: public tinyfsm::Fsm<Switch>
{
  // entry(), exit() and react() are called from Fsm<Switch>::transit()
  // in derived states, make friends:
  friend class tinyfsm::Fsm<Switch>;

  /* default reaction for unhandled events */
  void react(tinyfsm::Event const &) { };

  virtual void react(Toggle const &) { };
  virtual void entry(void) { };  /* entry actions in some states */
  void         exit(void)  { };  /* no exit actions */

public:
  static void reset(void);   /* implemented below */
};


// ----------------------------------------------------------------------------
// 3. State Declarations
//
class On
: public Switch
{
  void entry() override { counter++; std::cout << "* Switch is ON, counter=" << counter << std::endl; };
  void react(Toggle const &) override { transit<Off>(); };
  int counter;

public:
  On() : counter(0) { std::cout << "** RESET State=On" << std::endl; }
};

class Off
: public Switch
{
  void entry() override { counter++; std::cout << "* Switch is OFF, counter=" << counter << std::endl; };
  void react(Toggle const &) override { transit<On>(); };
  int counter;

public:
  Off() : counter(0) { std::cout << "** RESET State=Off" << std::endl; }
};


void Switch::reset() {
  std::cout << "** RESET Switch" << std::endl;
  // Reset all states (calls constructor on all states in list)
  tinyfsm::StateList<Off, On>::reset();

  // Alternatively, make counter public above and reset the values
  // here instead of using a copy-constructor with StateList<>:
  //state<On>().counter = 0;
  //state<Off>().counter = 0;
}

FSM_INITIAL_STATE(Switch, Off)


// ----------------------------------------------------------------------------
// 4. State Machine List Declaration (dispatches events to multiple FSM's)
//
// In this example, we only have a single state machine, no need to use FsmList<>:
//using fsm_handle = tinyfsm::FsmList< Switch >;
using fsm_handle = Switch;


// ----------------------------------------------------------------------------
// Main
//
int main()
{
  fsm_handle::start();

  while(1)
  {
    char c;
    std::cout << std::endl << "t=Toggle, r=Restart, q=Quit ? ";
    std::cin >> c;
    switch(c) {
    case 't':
      std::cout << "> Toggling switch..." << std::endl;
      fsm_handle::dispatch(Toggle());
      break;
    case 'r':
      fsm_handle::reset();
      fsm_handle::start();
      break;
    case 'q':
      return 0;
    default:
      std::cout << "> Invalid input" << std::endl;
    };
  }
}
