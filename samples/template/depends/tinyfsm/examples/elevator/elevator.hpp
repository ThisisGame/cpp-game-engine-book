#ifndef ELEVATOR_HPP_INCLUDED
#define ELEVATOR_HPP_INCLUDED

#include <tinyfsm.hpp>


// ----------------------------------------------------------------------------
// Event declarations
//

struct FloorEvent : tinyfsm::Event
{
  int floor;
};

struct Call        : FloorEvent { };
struct FloorSensor : FloorEvent { };
struct Alarm       : tinyfsm::Event { };



// ----------------------------------------------------------------------------
// Elevator (FSM base class) declaration
//

class Elevator
: public tinyfsm::Fsm<Elevator>
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

  virtual void react(Call        const &);
  virtual void react(FloorSensor const &);
  void         react(Alarm       const &);

  virtual void entry(void) { };  /* entry actions in some states */
  void         exit(void)  { };  /* no exit actions at all */

protected:

  static constexpr int initial_floor = 0;
  static int current_floor;
  static int dest_floor;
};


#endif
