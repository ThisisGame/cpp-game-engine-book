//
// In this example, we want to use the DefectiveSwitch FSM multiple
// times. As TinyFSM is all about templates, we need to declare it as
// a template class.
//
// This is a bit cumbersome, as the C++ syntax is really ugly when it
// comes to derived template classes.
//
#include <tinyfsm.hpp>
#include <iostream>
#include <stdlib.h> /* rand */

template<int inum>
class Off; // forward declaration

static void DumpState(int inum, const char * state, int on_counter, int defect_level) {
  std::cout << "* Switch[" << inum << "] is " << state << " (on_counter=" << on_counter << ", defect_level=" << defect_level << ")" << std::endl;
}

// ----------------------------------------------------------------------------
// 1. Event Declarations
//
struct Toggle : tinyfsm::Event { };


// ----------------------------------------------------------------------------
// 2. State Machine Base Class Declaration
//
template<int inum>
class DefectiveSwitch
: public tinyfsm::Fsm< DefectiveSwitch<inum> >
{
public:
  static constexpr unsigned int defect_level = (inum * 2);

  static void reset(void) {
    on_counter = 0;
  }

  /* default reaction for unhandled events */
  void react(tinyfsm::Event const &) { };

  virtual void react(Toggle const &) { };
  virtual void entry(void) { };  /* entry actions in some states */
  void         exit(void)  { };  /* no exit actions */

protected:
  static unsigned int on_counter;
};

// state variable definitions
template<int inum>
unsigned int DefectiveSwitch<inum>::on_counter{0};


// ----------------------------------------------------------------------------
// 3. State Declarations
//
template<int inum>
class On
: public DefectiveSwitch<inum>
{
  // note: base class is not known in dependend template
  using base = DefectiveSwitch<inum>;
  void entry() override {
    base::on_counter++;
    DumpState(inum, "ON ", base::on_counter, base::defect_level);
  };
  void react(Toggle const &) override {
    base::template transit< Off<inum> >();
  };
};


template<int inum>
class Off
: public DefectiveSwitch<inum>
{
  using base = DefectiveSwitch<inum>;
  void entry() override {
    DumpState(inum, "OFF", base::on_counter, base::defect_level);
  };
  void react(Toggle const &) override {
    if((rand() % (base::defect_level + 1)) == 0)
      base::template transit< On<inum> >();
    else {
      std::cout << "* Kzzz kzzzzzz" << std::endl;
      base::template transit< Off<inum> >();
    }
  };
};

FSM_INITIAL_STATE(DefectiveSwitch<0>, Off<0> )
FSM_INITIAL_STATE(DefectiveSwitch<1>, Off<1> )
FSM_INITIAL_STATE(DefectiveSwitch<2>, Off<2> )


// ----------------------------------------------------------------------------
// 4. State Machine List Declaration
//

using fsm_handle = tinyfsm::FsmList<
  DefectiveSwitch<0>,
  DefectiveSwitch<1>,
  DefectiveSwitch<2>
  >;

template<int inum>
void ToggleSingle() {
  std::cout << "> Toggling switch " << inum << "..." << std::endl;
  DefectiveSwitch<inum>::dispatch(Toggle());
}


// ----------------------------------------------------------------------------
// Main
//
int main()
{
  fsm_handle::start();

  while(1)
  {
    char c;
    std::cout << std::endl << "0,1,2=Toggle single, a=Toggle all, r=Restart, q=Quit ? ";
    std::cin >> c;
    switch(c) {
    case '0': ToggleSingle<0>(); break;
    case '1': ToggleSingle<1>(); break;
    case '2': ToggleSingle<2>(); break;
    case 'a':
      std::cout << "> Toggling all switches..." << std::endl;
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
