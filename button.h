#pragma once

#define ButtonDebounceMsec 50
#define ButtonShortMsec 100
#define ButtonRepeatMsec 1000

typedef enum {
  eButton_None = 0x00,
  eButton_Up = 0x01,
  eButton_Down = 0x02,
  eButton_Set = 0x04,
  eButton_Mode = 0x08,
} eButtonType;

uint8_t eventDoWSet[15] = {
  0x7F,         // 01111111 - Mon ~ Sun
  0x02,         // 00000010 - Mon
  0x04,         // 00000100 - Tues
  0x08,         // 00001000 - Wed
  0x10,         // 00010000 - Thu
  0x20,         // 00100000 - Fri
  0x40,         // 01000000 - Sat
  0x01,         // 00000001 - Sun
  0x2A,         // 00101010 - Mon, Wed, Fri
  0x54,         // 01010100 - Tue, Thu, Sat
  0x0E,         // 00001110 - Mon, Tue, Wed
  0x1C,         // 00011100 - Tue, Wed, Thu
  0x3E,         // 00111110 - Mon~Fri
  0x71,         // 01000001 - Sat, Sun
  0x7E          // 01111110 - Mon~Sat
};

typedef enum {
  ePress_Pushed,
  ePress_Down,
  ePress_Short,
  ePress_Repeat,
  ePress_10s,
  ePress_None
} ePressState;

unsigned long int ButtonDownMsec = 0;
int bRequest_Handle_Button_Press = 0;
eButtonType GetButtonsDown();
