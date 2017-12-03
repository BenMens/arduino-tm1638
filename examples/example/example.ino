#include <TM1638.h>

#define KEY_00 0x0800
#define KEY_01 0x8000
#define KEY_02 0x0200
#define KEY_03 0x2000
#define KEY_04 0x0008
#define KEY_05 0x0080
#define KEY_06 0x0002
#define KEY_07 0x0020
#define KEY_08 0x0400
#define KEY_09 0x4000
#define KEY_10 0x0100
#define KEY_11 0x1000
#define KEY_12 0x0004
#define KEY_13 0x0040
#define KEY_14 0x0001
#define KEY_15 0x0010


TM1638 tm1638(4,3,2);

unsigned long nextUpdateTime;
int8_t i=0;
uint16_t keyMap[] = {
  KEY_00,KEY_01,KEY_02,KEY_03,
  KEY_04,KEY_05,KEY_06,KEY_07,
  KEY_08,KEY_09,KEY_10,KEY_11,
  KEY_12,KEY_13,KEY_14,KEY_15
};


void handleKeyboardChange(uint8_t (&keys)[4]) {
  uint16_t keyAsBits =
          ((uint16_t)keys[0] & 0b01100110) << 9 |
          ((uint16_t)keys[1] & 0b01100110) << 7 |
          (keys[2] & 0b01100110) << 1 |
          (keys[3] & 0b01100110) >> 1;

  for(int8_t i=0;i<16;i++) {
    if (keyAsBits==keyMap[i]) {
      tm1638.scrollLeft();
      tm1638.setDigit(0x01,DIGIT_PATTERN(i));
      tm1638.update();
    }
  }
}



void setup() {
  tm1638.begin();
  tm1638.setDigit(0xFF,0x00);
  tm1638.update();
  tm1638.setKeyboardCallback(handleKeyboardChange);

  Serial.begin(115200);

  nextUpdateTime = millis()+500;
}


void loop() {
  unsigned long time = millis();

  if (time>nextUpdateTime) {
    // tm1638.scrollLeft();
    // tm1638.setDigit(0x01,DIGIT_PATTERN(i));
    // tm1638.update();

    i = (i+1) % 16;
    nextUpdateTime += 500;
  }

  tm1638.scan();
}
