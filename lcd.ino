#define TEXT "abc MWy 123 |"  // Text that will be printed on screen in any font

#include "lcd.h"
#include "SPI.h"
#include "TFT_eSPI.h"
#include "ArialNarrow27pt7b.h"
#include "TickingTimebombBB18pt7b.h"
#include "TickingTimebombBB46pt7b.h"

#define REF_X 15
#define REF_Y 35


TFT_eSPI tft = TFT_eSPI(170, 320);

// X, Y, width, height, yoffset
const int characterDayPos[7][5] = {
  { 258, 0, 24, 16, 9 },  // SUN
  { 80, 0, 28, 16, 9 },   // MON
  { 114, 0, 23, 16, 9 },  // TUE
  { 143, 0, 27, 16, 9 },  // WED
  { 176, 0, 23, 16, 9 },  // THU
  { 205, 0, 18, 16, 9 },  // FRI
  { 229, 0, 23, 16, 9 }   // SAT
};

String characterDay[7] = {
  "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"
};

const int characterWorkPos[5] = { 0, 21, 36, 14, 9 };
const int characterPausePos[5] = { 0, 35, 40, 14, 9 };
const int characterEventPos[5] = { 0, 49, 38, 14, 9 };
const int characterONPos[5] = { 0, 63, 17, 14, 9 };
const int characterOffPos[5] = { 0, 77, 23, 14, 9 };
const int characterHourPos[5] = { 63, 13, 86, 90, 58 };
const int characterColonPos[5] = { 149, 13, 16, 90, 43 };
const int characterMinutePos[5] = { 175, 13, 86, 90, 58 };
const int characterLevelPos[5] = { 35, 60, 17, 35, 22 };

void lcdInit() {
  // if the display has CS pin try with SPI_MODE0
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  tft.setTextDatum(TL_DATUM);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
}

uint8_t prevHourValue = 0;
void showHour(uint8_t hourValue, bool visible) {
  tft.setTextPadding(characterHourPos[3]);
  tft.setFreeFont(&TickingTimebombBB46pt7b);
  if (visible) {
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    if (hourValue < 10)
      tft.drawString(String("0") + String(hourValue), REF_X + characterHourPos[0], REF_Y + characterHourPos[1]);  // Print the string name of the font
    else
      tft.drawString(String(hourValue), REF_X + characterHourPos[0], REF_Y + characterHourPos[1]);  // Print the string name of the font
  } else {
    tft.setTextColor(TFT_BLACK, TFT_BLACK);
    if (prevHourValue < 10)
      tft.drawString(String("0") + String(prevHourValue), REF_X + characterHourPos[0], REF_Y + characterHourPos[1]);  // Print the string name of the font
    else
      tft.drawString(String(prevHourValue), REF_X + characterHourPos[0], REF_Y + characterHourPos[1]);  // Print the string name of the font
  }
  prevHourValue = hourValue;
}

uint8_t prevMinuteValue = 0;
void showMinute(uint8_t minuteValue, bool visible) {
  tft.setFreeFont(&TickingTimebombBB46pt7b);
  tft.setTextPadding(characterMinutePos[3]);

  if (visible) {
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    if (minuteValue < 10)
      tft.drawString(String("0") + String(minuteValue), REF_X + characterMinutePos[0], REF_Y + characterMinutePos[1]);  // Print the string name of the font
    else
      tft.drawString(String(minuteValue), REF_X + characterMinutePos[0], REF_Y + characterMinutePos[1]);  // Print the string name of the font
  } else {
    tft.setTextColor(TFT_BLACK, TFT_BLACK);
    if (prevMinuteValue < 10)
      tft.drawString(String("0") + String(prevMinuteValue), REF_X + characterMinutePos[0], REF_Y + characterMinutePos[1]);  // Print the string name of the font
    else
      tft.drawString(String(prevMinuteValue), REF_X + characterMinutePos[0], REF_Y + characterMinutePos[1]);  // Print the string name of the font
  }
  prevMinuteValue = minuteValue;
}

void showColon(bool visible) {
  if (visible) {
    tft.setTextColor(TFT_WHITE);
  } else {
    tft.setTextColor(TFT_BLACK);
  }
  tft.setTextPadding(characterColonPos[3]);
  tft.setFreeFont(&TickingTimebombBB46pt7b);
  tft.drawString(":", REF_X + characterColonPos[0], REF_Y + characterColonPos[1]);  // Print the string name of the font
}

void showDayOfWeek(uint8_t dayOfWeekValue, bool visible) {
  tft.setFreeFont(&ArialNarrow27pt7b);
  tft.setTextPadding(characterDayPos[0][3]);
  if (visible) {
    tft.setTextColor(TFT_WHITE, TFT_BLACK);  // Clear screen
    for (int i = 0; i < 7; i++) {
      if ((dayOfWeekValue >> i) & 1 == 1) {
        tft.drawString(characterDay[i], REF_X + characterDayPos[i][0], REF_Y + characterDayPos[i][1]);
      }
    }
  } else {
    tft.setTextColor(TFT_BLACK, TFT_BLACK);  // Clear screen
    for (int i = 0; i < 7; i++) {
      tft.drawString(characterDay[i], REF_X + characterDayPos[i][0], REF_Y + characterDayPos[i][1]);
    }
  }
}

void showWorkString(bool visible) {
  if (visible) {
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
  } else {
    tft.setTextColor(TFT_BLACK, TFT_BLACK);
  }

  tft.setTextPadding(characterWorkPos[3]);
  tft.setFreeFont(&ArialNarrow27pt7b);
  tft.drawString("WORK", REF_X, REF_Y + characterWorkPos[1]);
}

void showPauseString(bool visible) {
  if (visible) {
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
  } else {
    tft.setTextColor(TFT_BLACK, TFT_BLACK);
  }

  tft.setTextPadding(characterPausePos[3]);
  tft.setFreeFont(&ArialNarrow27pt7b);
  tft.drawString("PAUSE", REF_X, REF_Y + characterPausePos[1]);
}

void showEventString(bool visible) {
  if (visible) {
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
  } else {
    tft.setTextColor(TFT_BLACK, TFT_BLACK);
  }

  tft.setTextPadding(characterEventPos[3]);
  tft.setFreeFont(&ArialNarrow27pt7b);
  tft.drawString("EVENT", REF_X, REF_Y + characterEventPos[1]);
}

void showONString(bool visible) {
  if (visible) {
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
  } else {
    tft.setTextColor(TFT_BLACK, TFT_BLACK);
  }

  tft.setTextPadding(characterONPos[3]);
  tft.setFreeFont(&ArialNarrow27pt7b);
  tft.drawString("ON", REF_X, REF_Y + characterONPos[1]);
}

void showOffString(bool visible) {
  if (visible) {
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
  } else {
    tft.setTextColor(TFT_BLACK, TFT_BLACK);
  }

  tft.setTextPadding(characterOffPos[3]);
  tft.setFreeFont(&ArialNarrow27pt7b);
  tft.drawString("OFF", REF_X, REF_Y + characterOffPos[1]);
}

uint8_t prevLevelValue = 60;
void showLevelString(uint8_t levelValue, bool visible) {
  tft.setTextPadding(characterLevelPos[3]);
  tft.setFreeFont(&TickingTimebombBB18pt7b);

  if (visible) {
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString(String(levelValue), REF_X + characterLevelPos[0], REF_Y + characterLevelPos[1]);
  } else {
    tft.setTextColor(TFT_BLACK, TFT_BLACK);
    tft.drawString(String(levelValue), REF_X + characterLevelPos[0], REF_Y + characterLevelPos[1]);
  }
  prevLevelValue = levelValue;
}