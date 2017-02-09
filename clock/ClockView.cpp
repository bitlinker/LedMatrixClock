#include "ClockView.h"
#include "Screen.h"
#include "Bitmaps.h"

ClockView::ClockView()
  : mHours(0)
  , mMinutes(0)
  , mDot(false)
  , mDirty(true)
{
}

void ClockView::setDotTick(bool isSecond)
{
  if (isSecond == mDot) return;
  mDot = isSecond;
  mDirty = true;
//  tone(8, 4500);
//  delay(50);
//  noTone(8);
}

void ClockView::setTime(uint8_t hours, uint8_t minutes)
{
  // TODO: start animation
  if (hours == mHours && minutes == mMinutes) return;
  mHours = hours;
  mMinutes = minutes;
  mDirty = true;
}

void ClockView::draw(Screen& screen, uint8_t page)
{
  if (!mDirty) return;
  screen.clearPage(page);

  screen.drawBitmap(page, 0, 0, 8, 8, BMP_DIGITS[mHours / 10]);
  screen.drawBitmap(page, 8, 0, 8, 8, BMP_DIGITS[mHours % 10]);
  screen.drawBitmap(page, 16, 0, 8, 8, BMP_DIGITS[mMinutes / 10]);
  screen.drawBitmap(page, 24, 0, 8, 8, BMP_DIGITS[mMinutes % 10]);
  if (mDot)
  {
    screen.drawBitmap(page, 15, 0, 1, 8, BMP_CLOCK_DOTS);
  }
  screen.display();
  mDirty = false;
}

