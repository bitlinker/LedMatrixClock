// Libraries: 
// LedControl for Max7219 display
// Rtc_by_Makuna for DS3231 RTC
#include "Wire.h"
#include "LedControl.h"
#include "RtcDS3231.h"

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

LedControl mLed = LedControl(PIN_LED_DIN, PIN_LED_CLK, PIN_LED_CS, 4); 
RtcDS3231<TwoWire> Rtc(Wire);


const byte IMAGES[][8] = {
{
  B00111000,
  B01000100,
  B01000100,
  B01000100,
  B01000100,
  B01000100,
  B01000100,
  B00111000
},{
  B00010000,
  B00110000,
  B00010000,
  B00010000,
  B00010000,
  B00010000,
  B00010000,
  B00111000
},{
  B00111000,
  B01000100,
  B00000100,
  B00000100,
  B00001000,
  B00010000,
  B00100000,
  B01111100
},{
  B00111000,
  B01000100,
  B00000100,
  B00011000,
  B00000100,
  B00000100,
  B01000100,
  B00111000
},{
  B00000100,
  B00001100,
  B00010100,
  B00100100,
  B01000100,
  B01111100,
  B00000100,
  B00000100
},{
  B01111100,
  B01000000,
  B01000000,
  B01111000,
  B00000100,
  B00000100,
  B01000100,
  B00111000
},{
  B00111000,
  B01000100,
  B01000000,
  B01111000,
  B01000100,
  B01000100,
  B01000100,
  B00111000
},{
  B01111100,
  B00000100,
  B00000100,
  B00001000,
  B00010000,
  B00100000,
  B00100000,
  B00100000
},{
  B00111000,
  B01000100,
  B01000100,
  B00111000,
  B01000100,
  B01000100,
  B01000100,
  B00111000
},{
  B00111000,
  B01000100,
  B01000100,
  B01000100,
  B00111100,
  B00000100,
  B01000100,
  B00111000
}};
const int IMAGES_LEN = sizeof(IMAGES)/8;

volatile boolean mUpdateNeeded = true;
void onClockInterrupt() 
{
  mUpdateNeeded = true;
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

  pinMode(PIN_LED_CLK, OUTPUT);
  pinMode(PIN_LED_CS, OUTPUT);
  pinMode(PIN_LED_DIN, OUTPUT);

  // Enable clock interrupt
  pinMode(PIN_CLOCK, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_CLOCK), onClockInterrupt, CHANGE);

  // Buzz some melody...
  tone(PIN_BUZZER, 500); // Send 1KHz sound signal...
  delay(100);
  noTone(PIN_BUZZER);

  for (byte index = 0; index < mLed.getDeviceCount(); ++index)
  {    
      mLed.shutdown(index, false);
      //mLed.setIntensity(index, 4); // Brightness
      mLed.clearDisplay(index);
      mLed.setLed(index, 0, 0, true);
  }

  Rtc.Begin();
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  printDateTime(compiled);
  Serial.println();
  if (!Rtc.IsDateTimeValid()) 
  {
      // Common Cuases:
      //    1) first time you ran and the device wasn't running yet
      //    2) the battery on the device is low or even missing

      Serial.println("RTC lost confidence in the DateTime!");

      // following line sets the RTC to the date & time this sketch was compiled
      // it will also reset the valid flag internally unless the Rtc device is
      // having an issue

      Rtc.SetDateTime(compiled);
  }

  if (!Rtc.GetIsRunning())
  {
      Serial.println("RTC was not actively running, starting now");
      Rtc.SetIsRunning(true);
  }

  Rtc.Enable32kHzPin(false);
  // Enable interrupt pin TODO
  Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeClock);
  Rtc.SetSquareWavePinClockFrequency(DS3231SquareWaveClock_1Hz);
}

void drawDigit(byte device, byte digit)
{
  for (byte r = 0; r < 8; ++r)
  {
    mLed.setRow(device, r, IMAGES[digit][r]);
  }
}

byte btn1 = 0;
byte btn2 = 0;
byte btnDown = 0;
byte btnUp = 0;
int brightness = 0;

boolean dot = false;
byte oldBrightness = 0;
void loop()
{
  while(!mUpdateNeeded)
    ;
  mUpdateNeeded = false;

  // Read buttons
  btn1 = digitalRead(PIN_BTN_1);
  btn2 = digitalRead(PIN_BTN_2);
  btnUp = digitalRead(PIN_BTN_UP);
  btnDown = digitalRead(PIN_BTN_DOWN);
  brightness = analogRead(PIN_ANALOG_BRIGHTNESS); // 0 - 1023
  
  if (btn1 == LOW) { 
    Serial.println("Btn1");
    playMelody(PIN_BUZZER);
  }
  if (btn2 == LOW) Serial.println("Btn2");
  if (btnUp == LOW) Serial.println("Up");
  if (btnDown == LOW) Serial.println("Down");
  Serial.print("Brightness: ");  Serial.println(brightness);

  RtcDateTime now = Rtc.GetDateTime();
  printDateTime(now);
  Serial.println();

  byte hour = now.Hour();
  byte minute = now.Minute();
  drawDigit(3, hour / 10);
  drawDigit(2, hour % 10);
  drawDigit(1, minute / 10);
  drawDigit(0, minute % 10);
  
  //mLed.setLed(1, 2, 0, dot);
  //mLed.setLed(1, 5, 0, dot);
  mLed.setLed(2, 2, 7, dot);
  mLed.setLed(2, 5, 7, dot);
  dot = !dot;

  byte newBrightness = brightness / 250;  // TODO: configure curve
  if (oldBrightness != newBrightness)
  {
    oldBrightness = newBrightness;
    for (byte index = 0; index < mLed.getDeviceCount(); ++index)
    {    
      Serial.print("new intensity: ");  Serial.println(newBrightness);
      mLed.setIntensity(index, newBrightness);
    }
  }  

  RtcTemperature temp = Rtc.GetTemperature();
  Serial.print(temp.AsFloat());
  Serial.println("C");
}


#define countof(a) (sizeof(a) / sizeof(a[0]))

void printDateTime(const RtcDateTime& dt)
{
    char datestring[20];

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
            dt.Month(),
            dt.Day(),
            dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
    Serial.print(datestring);
}

