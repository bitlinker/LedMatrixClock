#include "Screen.h"
#include "Rtc.h"

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

volatile boolean mNeedScreenUpdate = true;
volatile boolean mNeedRtcUpdate = true;
volatile boolean mNeedControlsUpdate = true;
volatile uint16_t mClockCycles = 0; // Clock interrupt is called on 1khz. Here we are counting cycles to handle some events
void onClockInterrupt() 
{
  ++mClockCycles;
  if (mClockCycles == 1000) mClockCycles = 0;
  
  if (mClockCycles % 250 == 0) mNeedControlsUpdate = true; // Update controls
  if (mClockCycles % 750 == 0) mNeedScreenUpdate = true; // Update screen (~20 fps)
  if (mClockCycles == 0) mNeedRtcUpdate = true; // Update RTC
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

  // Enable clock interrupt
  pinMode(PIN_CLOCK, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_CLOCK), onClockInterrupt, CHANGE);

  // Buzz some melody...
  tone(PIN_BUZZER, 3500); // Send 1KHz sound signal...
  delay(40);
  noTone(PIN_BUZZER);

  // Init RTC
  mRtc.init();
}

byte mBtn1 = 0;
byte mBtn2 = 0;
byte mBtnDown = 0;
byte mBtnUp = 0;
void readButtons()
{
  mBtn1 = digitalRead(PIN_BTN_1);
  mBtn2 = digitalRead(PIN_BTN_2);
  mBtnUp = digitalRead(PIN_BTN_UP);
  mBtnDown = digitalRead(PIN_BTN_DOWN);
}

enum SetupMode
{
  SetupNone,
  SetupHours,
  SetupMinutes,
};
SetupMode mSetupMode = SetupNone;


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

RtcDateTime mCurTime;

void loop()
{ 
  if (mNeedControlsUpdate) {
    readButtons();
    if (mBtn1 == LOW) { 
      Serial.println("Btn1");
      //playMelody(PIN_BUZZER);
    }
    if (mBtn2 == LOW) Serial.println("Btn2");
    if (mBtnUp == LOW) Serial.println("Up");
    if (mBtnDown == LOW) Serial.println("Down");
    mNeedControlsUpdate = false;
  }
  
  if (mNeedRtcUpdate) {
    mRtc.getTime();
    // TODO
    //mCurTime = Rtc.GetDateTime();
    //printDateTime(mCurTime);
    //Serial.println();
    
    // TODO: temp
    mRtc.getTemp();

    updateBrightness();
    
    mNeedRtcUpdate = false;
  }

  if (mNeedScreenUpdate) { // TODO: in interrupt? analogread is slow...
    mScreen.updateScreen(mCurTime.Hour(), mCurTime.Minute(), mCurTime.Second());

    mNeedScreenUpdate = false;
  }
}


