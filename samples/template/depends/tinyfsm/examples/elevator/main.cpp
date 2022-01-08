#include "fsmlist.hpp"

#include <iostream>


int main()
{
  fsm_list::start();

  Call call;
  FloorSensor sensor;

  while(1)
  {
    char c;

    std::cout << "c=Call, f=FloorSensor, a=Alarm, q=Quit ? ";
    std::cin >> c;
    switch(c) {
    case 'c':
      std::cout << "Floor ? ";
      std::cin >> call.floor;
      send_event(call);
      break;
    case 'f':
      std::cout << "Floor ? ";
      std::cin >> sensor.floor;
      send_event(sensor);
      break;
    case 'a':
      send_event(Alarm());
      break;
    case 'q':
      std::cout << "Thanks for playing!" << std::endl;
      return 0;
    default:
      std::cout << "Invalid input" << std::endl;
    };
  }
}
