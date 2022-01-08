#include <tinyfsm.hpp>
#include <iostream>
#include <cassert>

struct Off; // forward declaration


// ----------------------------------------------------------------------------
// Event Declarations
//
struct Toggle : tinyfsm::Event { }; // Event Declarations


// ----------------------------------------------------------------------------
// State Machine Declaration
//
struct Switch
: tinyfsm::Fsm<Switch>
{
  static void reset(void);

  // NOTE: on reset: "tinyfsm::StateList<Off, On>::reset()", copy
  // constructor is used by default, so "this" points to neither
  // "Off" nor "On" (see operator=() below).
  Switch() : counter(0) {
    std::cout << "* Switch()" << std::endl
              << "  this          = " << this << std::endl;
  }

  ~Switch() {
    std::cout << "* ~Switch()" << std::endl
              << "  this          = " << this << std::endl;
  }

  Switch & operator=(const Switch & other) {
    std::cout << "* operator=()" << std::endl
              << "  this          = " << this << std::endl
              << "  other         = " << &other << std::endl;
    counter = other.counter;
    return *this;
  }

  virtual void react(Toggle const &) { };
  void entry(void);
  void exit(void);

  int counter;
};

struct On : Switch {
  void react(Toggle const &) override { transit<Off>(); };
};

struct Off : Switch {
  void react(Toggle const &) override { transit<On>(); };
};

FSM_INITIAL_STATE(Switch, Off)


// ----------------------------------------------------------------------------
// State Machine Definitions
//
void Switch::reset() {
  tinyfsm::StateList<Off, On>::reset();
}

void Switch::entry() {
  counter++;

  // debugging only. properly designed state machines don't need this:
  if(is_in_state<On>())       { std::cout << "* On::entry()"  << std::endl; }
  else if(is_in_state<Off>()) { std::cout << "* Off::entry()" << std::endl; }
  else assert(true);

  assert(current_state_ptr == this);
  std::cout << "  this (cur)    = " << this << std::endl
            << "  state<On>     = " << &state<On>()  << std::endl
            << "  state<Off>    = " << &state<Off>() << std::endl;
}

void Switch::exit() {
  assert(current_state_ptr == this);
  std::cout << "* exit()" << std::endl
            << "  this (cur)    = " << this << std::endl
            << "  state<On>     = " << &state<On>()  << std::endl
            << "  state<Off>    = " << &state<Off>() << std::endl;
}


// ----------------------------------------------------------------------------
// Main
//
int main()
{
  Switch::start();

  while(1)
  {
    char c;
    std::cout << "* main()" << std::endl
              << "  cur_counter   = " << Switch::current_state_ptr->counter  << std::endl
              << "  on_counter    = " << Switch::state<On>().counter  << std::endl
              << "  off_counter   = " << Switch::state<Off>().counter << std::endl;

    std::cout << std::endl << "t=Toggle, r=Restart, q=Quit ? ";
    std::cin >> c;
    switch(c) {
    case 't':
      Switch::dispatch(Toggle());
      break;
    case 'r':
      Switch::reset();
      Switch::start();
      break;
    case 'q':
      return 0;
    default:
      std::cout << "> Invalid input" << std::endl;
    };
  }
}
