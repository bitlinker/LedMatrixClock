#pragma once
#include <Arduino.h>

// Libraries:
// Rtc_by_Makuna for DS3231 RTC
// TODO: RM
#include "Wire.h"
#include "RtcDS3231.h"

class Rtc
{
  public:
    Rtc();

    void init();
    RtcDateTime getTime();
    void setTime(const RtcDateTime& tm);
    float getTemp();

    // TODO: alarms?

  private:
    RtcDS3231<TwoWire> mRtc;
};

