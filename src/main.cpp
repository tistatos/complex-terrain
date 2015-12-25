/**
* @file main.cpp
* @author Erik Sandrén
* @date 12-12-2015
* @brief [Description Goes Here]
*/

#include <iostream>

#include "engine.h"
#include "gameTimer.h"

int main(int argc, char* argv[])
{
  CTEngine engine("Complex Terrain");
  GameTimer timer;
  if(!engine.initialize()) return -1;
  double dt;
  double fps;

  while (engine.running())
  {
    timer.tick();
    dt = timer.getDeltaTime();
    engine.update(dt);
    engine.render(dt);
  }

  engine.shutdown();
  return 0;
}

