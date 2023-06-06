#include "menu.h"

int init_rtc(void) {
  rtc.setTime(30, 24, 15, 15, 5, 2023);  //dummy time, can be set to current day via code
  //SetTime.Date =
  delay(10);
  return 0;
}

int rtc_set(int hr, int min, int sec, int day, int mon, int year) {
  //rtc.setTime(30, 24, 15, 19, 4, 2023);
  rtc.setTime(sec, min, hr, day, mon, year);
  delay(10);
  return 0;
}

int rtc_get(RTCTimeStructType *SetTime, RTCDateStructType *SetDate) {
  struct tm timeinfo = rtc.getTimeStruct();
  //tm (int8_t wday, int16_t year, int8_t mon, int8_t mday, int8_t hour, int8_t min, int8_t sec)
  SetTime->Seconds = rtc.getSecond();
  SetTime->Minutes = rtc.getMinute();
  SetTime->Hours = rtc.getHour(true);
  SetDate->Date = rtc.getDay();
  SetDate->Month = rtc.getMonth();
  SetDate->Year = rtc.getYear();
  SetDate->WeekDay = rtc.getDayofWeek();
  delay(10);
  return 0;
}

void SaveToRTC() {
  rtc_get(&GetTime, &GetDate);  //get time and date
  int dowDiff = prevTimeState.dayOfWeek - GetDate.WeekDay;
  int iDate = GetDate.Date;
  if (dowDiff < 0) {
    if (iDate > 7) iDate = iDate + dowDiff;
    else iDate = iDate + 7 + dowDiff;
  } else if (dowDiff > 0) {
    if (iDate > 20) iDate = iDate - 7 + dowDiff;
    else iDate = iDate + dowDiff;
  }

  Serial.println(String(prevTimeState.dayOfWeek)+"," + String(GetDate.WeekDay) + "," + String(GetDate.Date)+"," + String(iDate));
  rtc_set(prevTimeState.hour, prevTimeState.minute, 0, iDate, GetDate.Month + 1, GetDate.Year);
}
