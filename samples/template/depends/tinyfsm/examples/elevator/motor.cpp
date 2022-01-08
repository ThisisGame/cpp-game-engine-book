#include <tinyfsm.hpp>
#include "motor.hpp"
#include <iostream>


// ----------------------------------------------------------------------------
// Motor states
//

class Stopped
: public Motor
{
  void entry() override {
    std::cout << "Motor: stopped" << std::endl;
    direction = 0;
  };
};

class Up
: public Motor
{
  void entry() override {
    std::cout << "Motor: moving up" << std::endl;
    direction = 1;
  };
};

class Down
: public Motor
{
  void entry() override {
    std::cout << "Motor: moving down" << std::endl;
    direction = -1;
  };
};


// ----------------------------------------------------------------------------
// Base State: default implementations
//

void Motor::react(MotorStop const &) {
  transit<Stopped>();
}

void Motor::react(MotorUp const &) {
  transit<Up>();
}

void Motor::react(MotorDown const &) {
  transit<Down>();
}

int Motor::direction{0};


// ----------------------------------------------------------------------------
// Initial state definition
//
FSM_INITIAL_STATE(Motor, Stopped)
