
#include "lcd.h"
#include <Adafruit_GFX.h>  // Core graphics library
#include <Adafruit_ST7789.h>
#include "ArialNarrow27pt7b.h"
#include "TickingTimebombBB18pt7b.h"
#include "TickingTimebombBB46pt7b.h"

#define REF_X 15
#define REF_Y 40

// ST7789 TFT module connections
#define TFT_CS 16   // define chip select pin
#define TFT_DC 17   // define data/command pin
#define TFT_RST 13  // define reset pin, or set to -1 and connect to Arduino RESET pin
#define TFT_DAT 23
#define TFT_CLK 18

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_DAT, TFT_CLK, TFT_RST);

// X, Y, width, height, yoffset
const int characterDayPos[7][5] = {
  { 258, 0, 24, 16, 9 },   // SUN
  { 80, 0, 28, 16, 9 },   // MON
  { 114, 0, 23, 16, 9 },  // TUE
  { 143, 0, 27, 16, 9 },  // WED
  { 176, 0, 23, 16, 9 },  // THU
  { 205, 0, 18, 16, 9 },  // FRI
  { 229, 0, 23, 16, 9 }  // SAT
};

String characterDay[7] = {
  "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"
};

const int characterWorkPos[5] = { 0, 21, 36, 14, 9 };
const int characterPausePos[5] = { 0, 35, 40, 14, 9 };
const int characterEventPos[5] = { 0, 49, 38, 14, 9 };
const int characterONPos[5] = { 0, 63, 17, 14, 9 };
const int characterOffPos[5] = { 0, 77, 23, 14, 9 };
const int characterHourPos[5] = { 63, 81, 86, 90, 58 };
const int characterColonPos[5] = { 149, 81, 16, 90, 43 };
const int characterMinutePos[5] = { 175, 81, 86, 90, 58 };
const int characterLevelPos[5] = { 35, 77, 17, 35, 22 };

void lcdInit() {
  // if the display has CS pin try with SPI_MODE0
  tft.init(170, 320, SPI_MODE2);  // Init ST7789 display 240x240 pixel
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);

  // tft.setCursor(REF_X + 35, REF_Y + 77);
  // tft.setFont(&TickingTimebombBB18pt7b);
  // tft.print("8");
}

/*
void showTime() {
  if (prevTimeState.preMarqueeState != Marquee_State) {
    switch (Marquee_State) {
      case MarqueeState_Event_Running:
        showPauseString(false);
        showOffString(false);
        showWorkString(true);
        showONString(true);
        break;
      case MarqueeState_No_Event:
        showWorkString(false);
        showONString(false);
        showPauseString(true);
        showOffString(true);
        break;
    }
    prevTimeState.preMarqueeState = Marquee_State;
  }

  rtc_get(&currentTime, &currentDate);

  if (prevTimeState.hour != currentTime.Hours) {
    showHour(currentTime.Hours);
    prevTimeState.hour = currentTime.Hours;
  }

  if (prevTimeState.colonFlag) {
    tft.fillRect(REF_X + characterColonPos[0], REF_Y + characterColonPos[1] - characterColonPos[4], characterColonPos[2], characterHourPos[3], ST77XX_BLACK);
    prevTimeState.colonFlag = false;
  } else {
    tft.setCursor(REF_X + characterColonPos[0], REF_Y + characterColonPos[1]);
    tft.setFont(&TickingTimebombBB46pt7b);
    tft.setTextColor(ST77XX_WHITE);
    tft.print(":");
    prevTimeState.colonFlag = true;
  }

  if (prevTimeState.minute != currentTime.Minutes) {
    showMinute(currentTime.Minutes);
    prevTimeState.minute = currentTime.Minutes;
  }

  if (prevTimeState.dayOfWeek != currentDate.WeekDay) {
    tft.fillRect(REF_X + characterDayPos[prevTimeState.dayOfWeek][0], REF_Y + characterDayPos[prevTimeState.dayOfWeek][1] - characterDayPos[prevTimeState.dayOfWeek][4], characterDayPos[prevTimeState.dayOfWeek][2], 90, ST77XX_BLACK);
    tft.setCursor(REF_X + characterDayPos[currentDate.WeekDay][0], REF_Y + characterDayPos[currentDate.WeekDay][1]);
    tft.setFont(&ArialNarrow27pt7b);
    tft.print(characterDay[currentDate.WeekDay]);
    prevTimeState.dayOfWeek = currentDate.WeekDay;
  }
}
*/

void showHour(uint8_t hourValue, bool visible) {
  if (visible) {
    tft.setCursor(REF_X + characterHourPos[0], REF_Y + characterHourPos[1]);
    tft.setFont(&TickingTimebombBB46pt7b);
    if (hourValue < 10)
      tft.print(String("0") + String(hourValue));
    else
      tft.print(String(hourValue));
  } else
    tft.fillRect(REF_X + characterHourPos[0], REF_Y + characterHourPos[1] - characterHourPos[4], characterHourPos[2], characterHourPos[3], ST77XX_BLACK);
}

void showMinute(uint8_t minuteValue, bool visible) {
  if (visible) {
    tft.setCursor(REF_X + characterMinutePos[0], REF_Y + characterMinutePos[1]);
    tft.setFont(&TickingTimebombBB46pt7b);
    if (minuteValue < 10)
      tft.print(String("0") + String(minuteValue));
    else
      tft.print(String(minuteValue));
  } else {
    tft.fillRect(REF_X + characterMinutePos[0], REF_Y + characterMinutePos[1] - characterMinutePos[4], characterMinutePos[2], characterHourPos[3], ST77XX_BLACK);
  }
}

void showColon(bool visible) {
  if (visible) {
    tft.setCursor(REF_X + characterColonPos[0], REF_Y + characterColonPos[1]);
    tft.setFont(&TickingTimebombBB46pt7b);
    tft.setTextColor(ST77XX_WHITE);
    tft.print(":");
  } else {
    tft.fillRect(REF_X + characterColonPos[0], REF_Y + characterColonPos[1] - characterColonPos[4], characterColonPos[2], characterHourPos[3], ST77XX_BLACK);
  }
}

void showDayOfWeek(uint8_t dayOfWeekValue, bool visible) {
  if (visible) {
    tft.setFont(&ArialNarrow27pt7b);
    for (int i = 0; i < 7; i++) {
      if ((dayOfWeekValue >> i) & 1 == 1) {
        tft.setCursor(REF_X + characterDayPos[i][0], REF_Y + characterDayPos[i][1]);
        tft.setFont(&ArialNarrow27pt7b);
        tft.print(characterDay[i]);
      }
    }
  } else {
    tft.fillRect(REF_X + characterDayPos[1][0], REF_Y + characterDayPos[1][1] - characterDayPos[1][4], 202, characterDayPos[1][3], ST77XX_BLACK);
  }
}

void showWorkString(bool visible) {
  if (visible) {
    tft.setCursor(REF_X, REF_Y + characterWorkPos[1]);
    tft.setFont(&ArialNarrow27pt7b);
    tft.print("WORK");
  } else {
    tft.fillRect(REF_X, REF_Y + characterWorkPos[1] - characterWorkPos[4], characterWorkPos[2], characterWorkPos[3], ST77XX_BLACK);
  }
}

void showPauseString(bool visible) {
  if (visible) {
    tft.setCursor(REF_X, REF_Y + characterPausePos[1]);
    tft.setFont(&ArialNarrow27pt7b);
    tft.print("PAUSE");
  } else {

    tft.fillRect(REF_X, REF_Y + characterPausePos[1] - characterPausePos[4], characterPausePos[2], characterPausePos[3], ST77XX_BLACK);
  }
}

void showEventString(bool visible) {
  if (visible) {
    tft.setCursor(REF_X, REF_Y + characterEventPos[1]);
    tft.setFont(&ArialNarrow27pt7b);
    tft.print("EVENT");
  } else {
    tft.fillRect(REF_X, REF_Y + characterEventPos[1] - characterEventPos[4], characterEventPos[2], characterEventPos[3], ST77XX_BLACK);
  }
}

void showONString(bool visible) {
  if (visible) {
    tft.setCursor(REF_X, REF_Y + characterONPos[1]);
    tft.setFont(&ArialNarrow27pt7b);
    tft.print("ON");
  } else {
    tft.fillRect(REF_X, REF_Y + characterONPos[1] - characterONPos[4], characterONPos[2], characterONPos[3], ST77XX_BLACK);
  }
}

void showOffString(bool visible) {
  if (visible) {
    tft.setCursor(REF_X, REF_Y + characterOffPos[1]);
    tft.setFont(&ArialNarrow27pt7b);
    tft.print("OFF");
  } else {
    tft.fillRect(REF_X, REF_Y + characterOffPos[1] - characterOffPos[4], characterOffPos[2], characterOffPos[3], ST77XX_BLACK);
  }
}

void showLevelString(uint8_t levelValue, bool visible) {
  if (visible) {
    tft.setCursor(REF_X + characterLevelPos[0], REF_Y + characterLevelPos[1]);
    tft.setFont(&TickingTimebombBB18pt7b);
    tft.print(String(levelValue));
  } else {
    tft.fillRect(REF_X + characterLevelPos[0], REF_Y + characterLevelPos[1] - characterLevelPos[4], characterLevelPos[2], characterLevelPos[3], ST77XX_BLACK);
  }
}