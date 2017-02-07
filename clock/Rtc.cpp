#include "Rtc.h"

void printDateTime(const RtcDateTime& dt)
{
  char datestring[20];
  snprintf_P(datestring, 20,
  PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
  dt.Month(),
  dt.Day(),
  dt.Year(),
  dt.Hour(),
  dt.Minute(),
  dt.Second() );
  Serial.print(datestring);
}

Rtc::Rtc()
: mRtc(Wire)
{
}

void Rtc::init()
{
  mRtc.Begin();
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  printDateTime(compiled);
  Serial.println();
  if (!mRtc.IsDateTimeValid()) 
  {
    Serial.println("RTC lost confidence in the DateTime!");
    mRtc.SetDateTime(compiled);
  }

  if (!mRtc.GetIsRunning())
  {
    Serial.println("RTC was not actively running, starting now");
    mRtc.SetIsRunning(true);
  }

  // Enable RTC interrupt pin at 1kHz
  mRtc.Enable32kHzPin(false);
  mRtc.SetSquareWavePin(DS3231SquareWavePin_ModeClock);
  mRtc.SetSquareWavePinClockFrequency(DS3231SquareWaveClock_1kHz);
}

void Rtc::getTime()
{
  //mCurTime = mRtc.GetDateTime();
}

void Rtc::getTemp()
{
  RtcTemperature temp = mRtc.GetTemperature();
  Serial.print(temp.AsFloat());
  Serial.println("C");
}

