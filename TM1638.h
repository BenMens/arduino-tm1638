#ifndef TM1638_h
#define TM1638_h

#include <arduino.h>

#define DIGIT_PATTERN(x) digitToPattern[x]

extern uint8_t digitToPattern[];

typedef uint8_t TM1638DisplayBuffer[8];

class TM1638
{
  public:

    TM1638(uint8_t dio,uint8_t clk,uint8_t stb);
    void begin();

    void write(uint8_t command);
    void write(uint8_t command,uint8_t *data,uint8_t len,boolean oddZero);
    void update();
    void setDigit(uint8_t mask,uint8_t pattern);
    void scrollLeft();
    void scrollRight();
    void scan();
    void setKeyboardCallback(void(*keyboardCallback)(uint8_t (&)[4]));

  private:
    TM1638DisplayBuffer displayBuffer;
    void (*keyboardCallback)(uint8_t (&)[4]);
    uint8_t keyboardBuffer[2][4];
    uint8_t dio,clk,stb;
    unsigned long debounceTime;
    uint8_t stableCount;
};

#endif
