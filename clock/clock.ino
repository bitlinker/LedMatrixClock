#include "Screen.h"
#include "Rtc.h"
#include "ClockView.h"
#include "ThermoView.h"

#define PIN_BTN_UP 7
#define PIN_BTN_DOWN 5
#define PIN_BTN_1 6
#define PIN_BTN_2 4

#define PIN_BUZZER 8

#define PIN_LED_CLK 10
#define PIN_LED_CS 11
#define PIN_LED_DIN 12

#define PIN_CLOCK 2

#define PIN_ANALOG_BRIGHTNESS 0 // A0

Screen mScreen(PIN_LED_CLK, PIN_LED_CS, PIN_LED_DIN);
Rtc mRtc;

// TODO: impl; move
class Controls
{
  public:
    Controls()
      : mBtn1(0)
      , mBtn2(0)
      , mBtnUp(0)
      , mBtnDown(0)
    {      
    }
    
    void update()
    {
      mBtn1 = digitalRead(PIN_BTN_1);
      mBtn2 = digitalRead(PIN_BTN_2);
      mBtnUp = digitalRead(PIN_BTN_UP);
      mBtnDown = digitalRead(PIN_BTN_DOWN);
      
      // TODO
      //      if (mBtn1 == LOW) { 
      //      Serial.println("Btn1");
      //      //playMelody(PIN_BUZZER);
      //    }
      //    if (mBtn2 == LOW) Serial.println("Btn2");
      //    if (mBtnUp == LOW) Serial.println("Up");
      //    if (mBtnDown == LOW) Serial.println("Down");
      // TODO: call some methods in viewcontroller
    }
    
  private:
    uint8_t mBtn1;
    uint8_t mBtn2;
    uint8_t mBtnDown;
    uint8_t mBtnUp;
};
Controls mControls;
ClockView mClockView;
ThermoView mThermoView;

volatile boolean mNeedScreenUpdate = true;
volatile boolean mNeedRtcUpdate = true;
volatile boolean mNeedControlsUpdate = true;
volatile uint16_t mClockCycles = 0; // Clock interrupt is called on 1khz. Here we are counting cycles to handle some events
void onClockInterrupt() 
{
  ++mClockCycles;
  if (mClockCycles == 1000) 
  {
    mClockCycles = 0;
    mNeedRtcUpdate = true; // Update RTC
  }
  
  if (mClockCycles % 250 == 0) mNeedControlsUpdate = true; // Update controls
  if (mClockCycles % 50 == 0) mNeedScreenUpdate = true; // Update screen (~20 fps)
  
  if (mClockCycles % 500 == 0) mClockView.setDotTick(mClockCycles == 0); // Dot tick
}

void setup() 
{
  Serial.begin(9600);  

  // Init pins
  pinMode(PIN_BTN_UP, INPUT_PULLUP);
  pinMode(PIN_BTN_DOWN, INPUT_PULLUP);
  pinMode(PIN_BTN_1, INPUT_PULLUP);
  pinMode(PIN_BTN_2, INPUT_PULLUP);

  pinMode(PIN_BUZZER, OUTPUT);

  mScreen.init();

  // TODO: in RTC class?
  // Enable clock interrupt
  pinMode(PIN_CLOCK, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_CLOCK), onClockInterrupt, RISING);

  // Buzz some melody...
  tone(PIN_BUZZER, 3500); // Send 1KHz sound signal...
  delay(40);
  noTone(PIN_BUZZER);

  // Init RTC
  mRtc.init();
}

// TODO: rm?
uint8_t oldBrightness = 0;
void updateBrightness()
{
  uint16_t brightness = analogRead(PIN_ANALOG_BRIGHTNESS); // 0 - 1023
  uint8_t newBrightness = brightness / 250;  // TODO: configure curve
  if (oldBrightness != newBrightness)
  {
    Serial.print("new intensity: ");  
    Serial.print(newBrightness);
    Serial.print(" from brightness: ");
    Serial.println(brightness);
    oldBrightness = newBrightness;
    mScreen.setIntensity(newBrightness);
  }
}


bool mClockMode = true;
uint8_t mModeSwitched = 16;
int8_t mScrollDir = 1;
int8_t mScroll = 0;

char incomingBytes[10];

void loop()
{ 
  if (Serial.available())
  {
    char c = Serial.read();
    if (c == 'T')
    {
      RtcDateTime curTime = mRtc.getTime();
      uint8_t hours = Serial.parseInt();
      uint8_t minutes = Serial.parseInt();
      mRtc.setTime(RtcDateTime(
        curTime.Year(),
        curTime.Month(),
        curTime.Day(),
        hours,
        minutes,
        0
      ));
      Serial.println("time set!");
    }    
  }
  
  if (mNeedControlsUpdate) {
    mControls.update();    
    mNeedControlsUpdate = false;
  }
  
  if (mNeedRtcUpdate) {
    RtcDateTime curTime = mRtc.getTime();
    mClockView.setTime(curTime.Hour(), curTime.Minute());
    // TODO: rm?
    //printDateTime(mCurTime);
    //Serial.println();
    
    // TODO: temp
    mThermoView.setTemp((uint8_t)(mRtc.getTemp()));
    if (curTime.Second() % 10 == 0)
    {
      mClockMode = !mClockMode; // TODO;
      mModeSwitched = 16;
      mScrollDir = mClockMode ? 1 : -1;
    }
    

    updateBrightness();
    
    mNeedRtcUpdate = false;
  }

  if (mNeedScreenUpdate) { // TODO: in interrupt? analogread is slow...
    if (mClockMode) 
    {
      if (mModeSwitched) 
      {
        mScroll += mScrollDir;
        mClockView.setDirty();
        mThermoView.setDirty();
        mScreen.scrollY(mScroll);
        --mModeSwitched;
      }      
      if (mModeSwitched < 8)
        mClockView.draw(mScreen, 0);
      else 
        mThermoView.draw(mScreen, 0);
    }
    else 
    {
      if (mModeSwitched)
      {
        mScroll += mScrollDir;
        mClockView.setDirty();
        mThermoView.setDirty();
        mScreen.scrollY(mScroll);
        --mModeSwitched;
      }
      if (mModeSwitched < 8)
        mThermoView.draw(mScreen, 0);
      else
        mClockView.draw(mScreen, 0);
    }

    mNeedScreenUpdate = false;
  }
}


