#include <tinyfsm.hpp>

#include "elevator.hpp"
#include "fsmlist.hpp"

#include <iostream>

class Idle; // forward declaration


// ----------------------------------------------------------------------------
// Transition functions
//

static void CallMaintenance() {
  std::cout << "*** calling maintenance ***" << std::endl;
}

static void CallFirefighters() {
  std::cout << "*** calling firefighters ***" << std::endl;
}


// ----------------------------------------------------------------------------
// State: Panic
//

class Panic
: public Elevator
{
  void entry() override {
    send_event(MotorStop());
  }
};


// ----------------------------------------------------------------------------
// State: Moving
//

class Moving
: public Elevator
{
  void react(FloorSensor const & e) override {
    int floor_expected = current_floor + Motor::getDirection();
    if(floor_expected != e.floor)
    {
      std::cout << "Floor sensor defect (expected " << floor_expected << ", got " << e.floor << ")" << std::endl;
      transit<Panic>(CallMaintenance);
    }
    else
    {
      std::cout << "Reached floor " << e.floor << std::endl;
      current_floor = e.floor;
      if(e.floor == dest_floor)
        transit<Idle>();
    }
  };
};


// ----------------------------------------------------------------------------
// State: Idle
//

class Idle
: public Elevator
{
  void entry() override {
    send_event(MotorStop());
  }

  void react(Call const & e) override {
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
};


// ----------------------------------------------------------------------------
// Base state: default implementations
//

void Elevator::react(Call const &) {
  std::cout << "Call event ignored" << std::endl;
}

void Elevator::react(FloorSensor const &) {
  std::cout << "FloorSensor event ignored" << std::endl;
}

void Elevator::react(Alarm const &) {
  transit<Panic>(CallFirefighters);
}

int Elevator::current_floor = Elevator::initial_floor;
int Elevator::dest_floor    = Elevator::initial_floor;


// ----------------------------------------------------------------------------
// Initial state definition
//
FSM_INITIAL_STATE(Elevator, Idle)
