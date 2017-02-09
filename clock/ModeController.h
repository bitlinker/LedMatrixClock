#pragma once
#include <Arduino.h>

class ModeController
{
protected:
  ModeController() {}
  virtual ~ModeController() {}
  
public:
  // Ticks with update screen framerate?
  void tick();

  // TODO: pass buttons here
  // TODO: how to change mode controller?
};


// TODO: needed?
class Director()
{
public:
private:
  ModeController* mModeController;  
};
