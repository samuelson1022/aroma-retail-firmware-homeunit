#include "HT1621.h"

#define LCD_CS_PIN 23
#define LCD_WR_PIN 13
#define LCD_DATA_PIN 16
#define BACKLIGHT_PIN 17


uint8_t section[32];

void lcdInit() {
  // if the display has CS pin try with SPI_MODE0
  lcd.begin(LCD_CS_PIN, LCD_WR_PIN, LCD_DATA_PIN, BACKLIGHT_PIN);
  lcd.backlight();
  lcd.clear();
}

void showHour(uint8_t hourValue, bool visible) {
  section[6] = section[6] & 0x80;
  section[8] = section[8] & 0x80;
  if (visible) {
    if (hourValue < 10) {
      section[6] |= lcd.charToSegBits(0) << 4;
      section[8] |= lcd.charToSegBits(hourValue) << 4;
      lcd.wrone(0x05, lcd.charToSegBits(0));
      lcd.wrone(0x06, section[6]);
      lcd.wrone(0x07, lcd.charToSegBits(hourValue));
      lcd.wrone(0x08, section[8]);
    }
    else {
      if (hourValue > 24) {
        section[6] |= lcd.charToSegBits(10) << 4;
        section[8] |= lcd.charToSegBits(10) << 4;
        lcd.wrone(0x05, lcd.charToSegBits(10));
        lcd.wrone(0x06, section[6]);
        lcd.wrone(0x07, lcd.charToSegBits(10));
        lcd.wrone(0x08, section[8]);
      }
      else {
        section[6] |= lcd.charToSegBits(hourValue / 10) << 4;
        section[8] |= lcd.charToSegBits(hourValue % 10) << 4;
        lcd.wrone(0x05, lcd.charToSegBits(hourValue / 10));
        lcd.wrone(0x06, section[6]);
        lcd.wrone(0x07, lcd.charToSegBits(hourValue % 10));
        lcd.wrone(0x08, section[8]);
      }
    }
  } else {
    lcd.wrone(0x05, 0);
    lcd.wrone(0x06, section[6]);
    lcd.wrone(0x07, 0);
    lcd.wrone(0x08, section[8]);
  }
}

uint8_t convertSegData(uint8_t data)
{
  uint8_t segData = 0;
  segData = data << 3;
  return segData;
}

void showMinute(uint8_t minuteValue, bool visible) {
  section[23] = section[23] & 0x80;
  section[25] = section[25] & 0x80;
  if (visible) {
    if (minuteValue < 10) {
      section[23] |= lcd.charToSegBits(0) << 4;
      section[25] |= lcd.charToSegBits(minuteValue) << 4;
      lcd.wrone(9, lcd.charToSegBits(0));
      lcd.wrone(23, section[23]);
      lcd.wrone(24, lcd.charToSegBits(minuteValue));
      lcd.wrone(25, section[25]);
    }
    else {
      if (MenuType != eMenu_Aroma_Set_Event_Work_Level && minuteValue > 60) {
        section[23] |= lcd.charToSegBits(10) << 4;
        section[25] |= lcd.charToSegBits(10) << 4;
        lcd.wrone(9, lcd.charToSegBits(10));
        lcd.wrone(23, section[23]);
        lcd.wrone(24, lcd.charToSegBits(10));
        lcd.wrone(25, section[25]);
      }
      else {
        section[23] |= lcd.charToSegBits(minuteValue / 10) << 4;
        section[25] |= lcd.charToSegBits(minuteValue % 10) << 4;
        lcd.wrone(9, lcd.charToSegBits(minuteValue / 10));
        lcd.wrone(23, section[23]);
        lcd.wrone(24, lcd.charToSegBits(minuteValue % 10));
        lcd.wrone(25, section[25]);
      }
    }
  } else {
    lcd.wrone(9, 0);
    lcd.wrone(23, section[23]);
    lcd.wrone(24, 0);
    lcd.wrone(25, section[25]);
  }
}

void showColon(bool visible) {
  section[8] &= 0x70;
  section[8] |= visible << 7;
  lcd.wrone(8, section[8]);
}

void showDayOfWeek(uint8_t dayOfWeekValue, bool visible) {
  section[31] &= 0x70;
  section[28] &= 0b10000000;
  section[26] &= 0b10000000;

  if (visible) {

    // Mon
    section[31] |= (dayOfWeekValue & 1) << 7;

    // Tue
    dayOfWeekValue >>= 1;
    section[28] |= (dayOfWeekValue & 1) << 6;

    // Wed
    dayOfWeekValue >>= 1;
    section[28] |= (dayOfWeekValue & 1) << 5;

    // Thurs
    dayOfWeekValue >>= 1;
    section[28] |= (dayOfWeekValue & 1) << 4;

    // Fri
    dayOfWeekValue >>= 1;
    section[26] |= (dayOfWeekValue & 1) << 4;

    // Sat
    dayOfWeekValue >>= 1;
    section[26] |= (dayOfWeekValue & 1) << 5;
    
    // SUN
    dayOfWeekValue >>= 1;
    section[26] |= (dayOfWeekValue & 1) << 6;
  }

  lcd.wrone(26, section[26]);
  lcd.wrone(28, section[28]);
  lcd.wrone(31, section[31]);
}

void showPercent(bool visible) {
  section[26] &= 0b01110000;
  section[26] |= visible << 7;
  lcd.wrone(26, section[26]);
}

void showWind(uint8_t index) {
  section[6] &= 0x70;
  section[23] &= 0x70;
  section[28] &= 0x70;

  switch (index) {
    case 1:
      section[28] |= 1 << 7;
      break;
    case 2:
      section[23] |= 1 << 7;
      break;
    case 3:
      section[6] |= 1 << 7;
      break;
    case 0:
    default:
      break;
  }
  lcd.wrone(6, section[6]);
  lcd.wrone(23, section[23]);
  lcd.wrone(28, section[28]);
}

void showWorkString(bool visible) {
  section[27] &= 0b11010000;
  section[27] |= visible << 5;
  lcd.wrone(27, section[27]);
}

void showPauseString(bool visible) {
  section[27] &= 0b10110000;
  section[27] |= visible << 6;
  lcd.wrone(27, section[27]);
}

void showEventString(bool visible) {
  section[27] &= 0b01110000;
  section[27] |= visible << 7;
  lcd.wrone(27, section[27]);
}

void showONString(bool visible) {
  section[29] &= 0b00010000;
  section[29] |= visible << 5;
  lcd.wrone(29, section[29]);
}

void showOffString(bool visible) {
  section[29] &= 0b00100000;
  section[29] |= visible << 4;
  lcd.wrone(29, section[29]);
}


void showLevelString(uint8_t levelValue, bool visible) {
  section[31] &= 0x80;
  if (visible) {
    section[31] |= lcd.charToSegBits(levelValue) << 4;
    lcd.wrone(30, lcd.charToSegBits(levelValue));
    lcd.wrone(31, section[31]);
  } else {
    lcd.wrone(30, 0);
    lcd.wrone(31, section[31]);
  }
}
