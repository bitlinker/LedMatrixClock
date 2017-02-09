#pragma once
#include <Arduino.h>

class Screen;
class ThermoView
{
  public:
    ThermoView();
    void draw(Screen& screen, uint8_t page); // TODO: virtual, base view?
    
    void setTemp(uint8_t temp); // deg. C, fixed point (*10)
    void setDirty() { mDirty = true; }
    
  private:
    uint8_t mTemp;
    bool mDirty;
};

