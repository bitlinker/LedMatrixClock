#pragma once
#include <Arduino.h>

class Screen
{
  public:
    Screen(int pinClk, int pinCs, int pinDin);
    
    void init();

    // 0 - 16
    void setIntensity(uint8_t value);
    void shutdown(bool value);
    void clearAll();
    void clearPage(uint8_t page);
    void setPixel(uint8_t page, uint8_t x, uint8_t y, boolean state);
    void drawBitmap(uint8_t page, int8_t x, int8_t y, uint8_t width, uint8_t height, const uint8_t* data);
    void scrollX(int8_t value) { mShiftX = value;}
    void scrollY(int8_t value) { mShiftY = value;}
   
    void display();

    void updateScreen(uint8_t hour, uint8_t minute, uint8_t second); 
    void drawDigit(byte device, byte digit, boolean isDimmed);
  

  private:
    inline uint8_t* getPagePtrMirrored(int8_t pageX, int8_t pageY); // By page offsets
    inline uint8_t* getPagePtr(uint8_t pageNum); // By page number
    void spiTransferToAll(uint8_t opcode, uint8_t data);    
  
  private:
    int mPinClk;
    int mPinCs;
    int mPinDin;
    static const uint8_t NUM_LED_DEVICES = 4;
    static const uint8_t PAGE_STRIDE = NUM_LED_DEVICES;
    static const uint8_t PAGE_WIDTH = PAGE_STRIDE * 8;
    static const uint8_t PAGE_HEIGHT = 8;
    static const uint8_t PAGE_SIZE = PAGE_HEIGHT * PAGE_STRIDE; // In bytes
    static const uint8_t NUM_PAGES = 2;
    uint8_t mFramebuf[PAGE_SIZE * NUM_PAGES]; // Pages of video memory, sequential
    int8_t mShiftX; // + scrolls all image right
    int8_t mShiftY; // + scrolls all image down
};

