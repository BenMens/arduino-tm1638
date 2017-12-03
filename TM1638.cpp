#include "TM1638.h"

const uint8_t digitToPattern[] PROGMEM = {
  0b00111111, // 0
  0b00000110, // 1
  0b01011011, // 2
  0b01001111, // 3
  0b01100110, // 4
  0b01101101, // 5
  0b01111101, // 6
  0b00000111, // 7
  0b01111111, // 8
  0b01101111, // 9
  0b01110111, // A
  0b01111100, // B
  0b00111001, // C
  0b01011110, // D
  0b01111001, // E
  0b01110001, // F
};


TM1638::TM1638(uint8_t dio,uint8_t clk,uint8_t stb) {
  this->dio = dio;
  this->clk = clk;
  this->stb = stb;

  this->keyboardCallback = NULL;
  this->debounceTime = millis();
  this->stableCount  = 0;
}

void TM1638::begin() {
  pinMode(this->clk,OUTPUT);
  pinMode(this->stb,OUTPUT);
  pinMode(this->dio,OUTPUT);

  digitalWrite(this->clk,LOW);
  digitalWrite(this->stb,HIGH);
  digitalWrite(this->dio,HIGH);

  setDigit(0xff,0xff);
  setBrightness(0,true);
  update();
}

void TM1638::write(uint8_t command) {
  digitalWrite(this->stb,LOW);
  delayMicroseconds(1);

  shiftOut(this->dio, this->clk, LSBFIRST, command);

  delayMicroseconds(1);
  digitalWrite(this->stb,HIGH);
}

void TM1638::write(uint8_t command,uint8_t *data,uint8_t len,boolean oddZero) {
  digitalWrite(this->stb,LOW);
  delayMicroseconds(1);

  shiftOut(this->dio, this->clk, LSBFIRST, command);

  delayMicroseconds(1);

  while(len--) {
    shiftOut(this->dio, this->clk, LSBFIRST, *data++);
    if (oddZero) {
      shiftOut(this->dio, this->clk, LSBFIRST, 0);
    }
  }

  delayMicroseconds(1);
  digitalWrite(this->stb,HIGH);
}

void TM1638::update() {
  write(0b01000000,this->displayBuffer,sizeof(this->displayBuffer),true);
}

void TM1638::setDigit(uint8_t mask,uint8_t pattern) {
  uint8_t *data = this->displayBuffer;

  for (int8_t i=8;i>0;i--) {
    if (pattern & 1) {
      *data++ |= mask;
    }
    else {
      *data++ &= ~mask;
    }
    pattern >>= 1;
  }
}

void TM1638::scrollLeft() {
  uint8_t *data = this->displayBuffer;

  for (int8_t i=8;i>0;i--) {
      *data = *data<<1;
      data++;
  }
}

void TM1638::scrollRight() {
  uint8_t *data = this->displayBuffer;

  for (int8_t i=8;i>0;i--) {
      *data = *data>>1;
      data++;
  }
}

void TM1638::setBrightness(uint8_t brightness,bool on) {
  if (on) {
    write(0b10001000 | (brightness & 0b00000111));
  }
  else {
    write(0b10000000 | (brightness & 0b00000111));
  }
}


void TM1638::scanKeyboard() {

  digitalWrite(this->stb,LOW);
  delayMicroseconds(1);

  shiftOut(this->dio, this->clk, LSBFIRST, 0b01000010);

  pinMode(this->dio,INPUT);

  uint8_t *buffer = &this->keyboardBuffer[0][0];

  for (uint8_t i=4;i>0;i--) {
    uint8_t k = shiftIn(this->dio, this->clk, LSBFIRST);
    if (*buffer != k) {
      this->debounceTime = millis();
      this->stableCount = 0;
    }
    *buffer = k;
    buffer++;
  }
  pinMode(this->dio,OUTPUT);

  delayMicroseconds(1);
  digitalWrite(this->stb,HIGH);
  delayMicroseconds(1);

  if (this->stableCount>=10) { // Need minimum of 10 stable readings
    if (millis() - this->debounceTime > 50 && this->stableCount==10) {

      bool changed=false;
      for (int8_t i=0;i<4;i++) {
        if (this->keyboardBuffer[1][i] != this->keyboardBuffer[0][i]) {
          changed=true;
          this->keyboardBuffer[1][i] = this->keyboardBuffer[0][i];
        }
      }
      if (changed && this->keyboardCallback != NULL) {
        this->keyboardCallback(this->keyboardBuffer[0]);
      }

      this->stableCount=11; // Signal change processed
    }
  }
  else {
    this->stableCount++;
  }
}

void TM1638::setKeyboardCallback(void (*keyboardCallback)(uint8_t (&)[4])) {
  this->keyboardCallback = keyboardCallback;
}
