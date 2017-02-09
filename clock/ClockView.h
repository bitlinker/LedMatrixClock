#pragma once
#include <Arduino.h>

class Screen;
class ClockView
{
  public:
    ClockView();
    void draw(Screen& screen, uint8_t page); // TODO: virtual, base view?

    // TODO: dot signal - subscribe on interrupt?
    void setDotTick(bool isSecond);
    void setTime(uint8_t hours, uint8_t minutes);
    void setDirty() { mDirty = true; }
    
  private:
    uint8_t mHours;
    uint8_t mMinutes;
    bool mDot;
    bool mDirty;
};

