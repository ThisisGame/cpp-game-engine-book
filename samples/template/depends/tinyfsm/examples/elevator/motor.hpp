#ifndef MOTOR_HPP_INCLUDED
#define MOTOR_HPP_INCLUDED

#include <tinyfsm.hpp>


// ----------------------------------------------------------------------------
// Event declarations
//

struct MotorUp   : tinyfsm::Event { };
struct MotorDown : tinyfsm::Event { };
struct MotorStop : tinyfsm::Event { };


// ----------------------------------------------------------------------------
// Motor (FSM base class) declaration
//
class Motor
: public tinyfsm::Fsm<Motor>
{
  /* NOTE: react(), entry() and exit() functions need to be accessible
   * from tinyfsm::Fsm class. You might as well declare friendship to
   * tinyfsm::Fsm, and make these functions private:
   *
   * friend class Fsm;
   */
public:

  /* default reaction for unhandled events */
  void react(tinyfsm::Event const &) { };

  /* non-virtual declaration: reactions are the same for all states */
  void react(MotorUp   const &);
  void react(MotorDown const &);
  void react(MotorStop const &);

  virtual void entry(void) = 0;  /* pure virtual: enforce implementation in all states */
  void exit(void)  { };          /* no exit actions at all */

protected:

  static int direction;

public:
  static int getDirection() { return direction; }
};


#endif
