#include "Screen.h"

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
  }
  ,{
    B00010000,
    B00110000,
    B00010000,
    B00010000,
    B00010000,
    B00010000,
    B00010000,
    B00111000
  }
  ,{
    B00111000,
    B01000100,
    B00000100,
    B00000100,
    B00001000,
    B00010000,
    B00100000,
    B01111100
  }
  ,{
    B00111000,
    B01000100,
    B00000100,
    B00011000,
    B00000100,
    B00000100,
    B01000100,
    B00111000
  }
  ,{
    B00000100,
    B00001100,
    B00010100,
    B00100100,
    B01000100,
    B01111100,
    B00000100,
    B00000100
  }
  ,{
    B01111100,
    B01000000,
    B01000000,
    B01111000,
    B00000100,
    B00000100,
    B01000100,
    B00111000
  }
  ,{
    B00111000,
    B01000100,
    B01000000,
    B01111000,
    B01000100,
    B01000100,
    B01000100,
    B00111000
  }
  ,{
    B01111100,
    B00000100,
    B00000100,
    B00001000,
    B00010000,
    B00100000,
    B00100000,
    B00100000
  }
  ,{
    B00111000,
    B01000100,
    B01000100,
    B00111000,
    B01000100,
    B01000100,
    B01000100,
    B00111000
  }
  ,{
    B00111000,
    B01000100,
    B01000100,
    B01000100,
    B00111100,
    B00000100,
    B01000100,
    B00111000
  }
};

#define OP_NOOP   0
#define OP_DIGIT0 1
#define OP_DIGIT1 2
#define OP_DIGIT2 3
#define OP_DIGIT3 4
#define OP_DIGIT4 5
#define OP_DIGIT5 6
#define OP_DIGIT6 7
#define OP_DIGIT7 8
#define OP_DECODEMODE  9
#define OP_INTENSITY   10
#define OP_SCANLIMIT   11
#define OP_SHUTDOWN    12
#define OP_DISPLAYTEST 15

void Screen::drawDigit(byte device, byte digit, boolean isDimmed)
{
  // TODO
//  for (byte r = 0; r < 8; ++r)
//  {
//    mLed.setRow(device, r, isDimmed ? 0 : IMAGES[digit][r]);
//  }
}

inline uint8_t* Screen::getPagePtrMirrored(int8_t pageX, int8_t pageY)
{
  uint8_t pageIndex = (pageX + pageY) & 1;
  return getPagePtr(pageIndex);
}

inline uint8_t* Screen::getPagePtr(uint8_t pageNum)
{
  return &mFramebuf[PAGE_SIZE * pageNum];
}

// TODO: move
boolean dot = false;
int8_t scrlx = 0;
int8_t scrly = 0;
int8_t scrlDir = 1;
void Screen::updateScreen(uint8_t hour, uint8_t minute, uint8_t second)
{
  //scrly += scrlDir;
  scrlx += scrlDir;
  //if (scrly >= 32 || scrly <= -32) scrlDir = -scrlDir;
  if (scrlx >= 64 || scrlx <= -64) scrlDir = -scrlDir;

  scrollX(scrlx);
  //scrollY(scrly);
  
//  drawBitmap(0, 3, hour / 10, 8, 8, IMAGES[hour / 10]);
//  drawDigit(3, hour / 10, dot);
//  drawDigit(2, hour % 10, dot);
//  drawDigit(1, minute / 10, false);
//  drawDigit(0, minute % 10, false);

  boolean dt = second % 2;

  setPixel(0, 0, 0, true);
  setPixel(0, 1, 0, true);
  setPixel(0, 7, 0, true);
  setPixel(0, 7, 1, true);
  setPixel(0, 31, 0, true);
  setPixel(0, 31, 7, true);
  setPixel(0, 0, 7, true);
  
  setPixel(1, 0, 0, true);
  setPixel(1, 31, 0, true);
  setPixel(1, 31, 7, true);
  setPixel(1, 0, 7, true);
  setPixel(1, 2, 0, true);
  
//  setPixel(0, 15, 2, dt);
//  setPixel(0, 15, 5, dt);
//  setPixel(1, 14, 2, true);
//  setPixel(1, 14, 5, true);
  dot = !dot;
  display();
}

Screen::Screen(int pinClk, int pinCs, int pinDin)
  : mPinClk(pinClk)
  , mPinCs(pinCs)
  , mPinDin(pinDin)
  , mShiftX(0)
  , mShiftY(0)
{ 
}

void Screen::init()
{
  pinMode(mPinClk, OUTPUT);
  pinMode(mPinCs, OUTPUT);
  pinMode(mPinDin, OUTPUT);
  digitalWrite(mPinCs, HIGH);

  spiTransferToAll(OP_DISPLAYTEST, 0);
  spiTransferToAll(OP_SCANLIMIT, 7);
  spiTransferToAll(OP_DECODEMODE, 0);
  clearAll();
  display();
  shutdown(false);
}

void Screen::setIntensity(uint8_t value)
{
  spiTransferToAll(OP_INTENSITY, value);
}

void Screen::shutdown(bool value) 
{
  spiTransferToAll(OP_SHUTDOWN, value ? 0 : 1);
}

void Screen::clearAll()
{
  for (uint8_t i = 0; i < NUM_PAGES; ++i)
  {
    clearPage(i);
  }
}

void Screen::clearPage(uint8_t page)
{
  memset(getPagePtr(page), 0, PAGE_SIZE);
}

void Screen::setPixel(uint8_t page, uint8_t x, uint8_t y, boolean state)
{
  uint8_t* ptr = getPagePtr(page);
  uint8_t val = B10000000 >> (x % 8);
  uint8_t offset = (x >> 3) + y * PAGE_STRIDE;
  if(state)
    ptr[offset] |= val;
  else
    ptr[offset] &= ~val;
}

// x is mul 8 and width is 8 now...
void Screen::drawBitmap(uint8_t page, uint8_t x, uint8_t y, uint8_t width, uint8_t height, const uint8_t* data)
{  
  for (int j = 0; j < height; ++j)
  {

//    
//    int8_t colByte = pageXX / 8;
//    int8_t colBit = pageXX % 8; // TODO: modulo
//
//    for (int x = 0; x < PAGE_WIDTH; ++x)
//    {
//      // TODO
//    }
//    
    //mFramebuffer[(j + y) * 8  + x] = data[j];
//    for (int i = 0; i < width; ++i)
//    {    
//      char* dstByte = mFramebuffer[j + y + x]; // TODO: check                 
//    }
  }
}

static void Swap32(uint32_t& val)
{
  val =   ((val >> 24) & 0xff) |
          ((val << 8) & 0xff0000) |
          ((val >> 8) & 0xff00) |
          ((val << 24) & 0xff000000);
}

void Screen::display()
{  
  // Column calculations
  int8_t col = -mShiftX; 
  int8_t pageX = col / PAGE_WIDTH;
  if (col < 0) --pageX;
  int8_t pageXX = col - pageX * PAGE_WIDTH; // Offset from start of the page, columns (bits)
    
  for (int8_t y = 0; y < PAGE_HEIGHT; ++y)
  {
    // Row calculations
    int8_t row = y - mShiftY; // Visible row
    int8_t pageY; // page index
    if (row >= 0)
      pageY = row / PAGE_HEIGHT;
    else
      pageY = (row + 1) / PAGE_HEIGHT - 1;
    int8_t pageYY = row - pageY * PAGE_HEIGHT; // Offset from start of the page, rows
    int8_t rowOffset = pageYY * PAGE_STRIDE; // Offset from start of the page, bytes

    // Assumption: This calculations is based on that page width is 32!
    uint32_t rowBegin = *(uint32_t*)(getPagePtrMirrored(pageX, pageY) + rowOffset);
    uint32_t rowEnd = *(uint32_t*)(getPagePtrMirrored(pageX + 1, pageY) + rowOffset);
    Swap32(rowBegin);
    Swap32(rowEnd);
    uint32_t rowData = (rowBegin << pageXX) | (rowEnd >> (32 - pageXX));
    Swap32(rowData);
    const uint8_t* rowPtr = (uint8_t*)&rowData;
    
    digitalWrite(mPinCs, LOW);
    for (uint8_t dev = 0; dev < NUM_LED_DEVICES; ++dev)   
    {
      shiftOut(mPinDin, mPinClk, MSBFIRST, y + 1);
      shiftOut(mPinDin, mPinClk, MSBFIRST, *rowPtr++);
    }
    digitalWrite(mPinCs, HIGH);
  }   
}

void Screen::spiTransferToAll(uint8_t opcode, uint8_t data)
{
  digitalWrite(mPinCs, LOW);
  for (uint8_t i = 0; i < NUM_LED_DEVICES; ++i)
  {
    shiftOut(mPinDin, mPinClk, MSBFIRST, opcode);
    shiftOut(mPinDin, mPinClk, MSBFIRST, data);
  }
  digitalWrite(mPinCs, HIGH);
}
