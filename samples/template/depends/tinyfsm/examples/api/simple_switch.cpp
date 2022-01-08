#include <tinyfsm.hpp>
#include <iostream>

struct Off; // forward declaration


// ----------------------------------------------------------------------------
// 1. Event Declarations
//
struct Toggle : tinyfsm::Event { };


// ----------------------------------------------------------------------------
// 2. State Machine Base Class Declaration
//
struct Switch : tinyfsm::Fsm<Switch>
{
  virtual void react(Toggle const &) { };

  // alternative: enforce handling of Toggle in all states (pure virtual)
  //virtual void react(Toggle const &) = 0;

  virtual void entry(void) { };  /* entry actions in some states */
  void         exit(void)  { };  /* no exit actions */

  // alternative: enforce entry actions in all states (pure virtual)
  //virtual void entry(void) = 0;
};


// ----------------------------------------------------------------------------
// 3. State Declarations
//
struct On : Switch
{
  void entry() override { std::cout << "* Switch is ON" << std::endl; };
  void react(Toggle const &) override { transit<Off>(); };
};

struct Off : Switch
{
  void entry() override { std::cout << "* Switch is OFF" << std::endl; };
  void react(Toggle const &) override { transit<On>(); };
};

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
  // instantiate events
  Toggle toggle;

  fsm_handle::start();

  while(1)
  {
    char c;
    std::cout << std::endl << "t=Toggle, q=Quit ? ";
    std::cin >> c;
    switch(c) {
    case 't':
      std::cout << "> Toggling switch..." << std::endl;
      fsm_handle::dispatch(toggle);
      // alternative: instantiating causes no overhead (empty declaration)
      //fsm_handle::dispatch(Toggle());
      break;
    case 'q':
      return 0;
    default:
      std::cout << "> Invalid input" << std::endl;
    };
  }
}
