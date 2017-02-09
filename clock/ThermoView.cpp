#include "ThermoView.h"
#include "Screen.h"
#include "Bitmaps.h"

ThermoView::ThermoView()
  : mTemp(0)
  , mDirty(true)
{
}

void ThermoView::setTemp(uint8_t temp)
{
  if (temp == mTemp) return;
  mTemp = temp;
  mDirty = true;
}

void ThermoView::draw(Screen& screen, uint8_t page)
{
  if (!mDirty) return;
  screen.clearPage(page);

  uint8_t digit1 = mTemp / 100;
  uint8_t tmp = mTemp - digit1 * 100;
  uint8_t digit2 = tmp / 10;
  uint8_t digit3 = tmp - digit2 * 10;

  screen.drawBitmap(page, 0, 0, 8, 8, BMP_DIGITS[digit1]);
  screen.drawBitmap(page, 8, 0, 8, 8, BMP_DIGITS[digit2]);
  screen.setPixel(page, 15, 7, true);
  screen.drawBitmap(page, 16, 0, 8, 8, BMP_DIGITS[digit3]);
  screen.drawBitmap(page, 24, 0, 4, 4, BMP_DEG);
  screen.display();
  mDirty = false;
}

