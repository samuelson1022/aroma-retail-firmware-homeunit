
#include "rtc.h"
#include "lcd.h"

RTCTimeStructType currentTime;
RTCDateStructType currentDate;
eMarqueeState Marquee_State;
TimePreviousState prevTimeState;

int levelTest = 1;
bool showEventDisplayOnce = false;

void Menu_Display() {
  switch (MenuType) {
    case eMenu_Aroma_Home:
      showLevelString(0, false);
      showEventString(false);
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

      if (!prevTimeState.dayOfWeekFlag) {
        showDayOfWeek(1 << currentDate.WeekDay, true);
        prevTimeState.dayOfWeekFlag = true;
      } else {
        if (prevTimeState.dayOfWeek != currentDate.WeekDay) {
          showDayOfWeek(1 << currentDate.WeekDay, false);
          showDayOfWeek(1 << currentDate.WeekDay, true);
          prevTimeState.dayOfWeek = currentDate.WeekDay;
        }
      }

      if (prevTimeState.hour != currentTime.Hours) {
        showHour(currentTime.Hours, false);
        showHour(currentTime.Hours, true);
        prevTimeState.hour = currentTime.Hours;
      }

      prevTimeState.colonFlag = !(prevTimeState.colonFlag);
      showColon(prevTimeState.colonFlag);

      if (prevTimeState.minute != currentTime.Minutes) {
        showMinute(currentTime.Minutes, false);
        showMinute(currentTime.Minutes, true);
        prevTimeState.minute = currentTime.Minutes;
      }
      break;
    case eMenu_Aroma_Set_Events:
      if (!showEventDisplayOnce) {
        showPauseString(false);
        showOffString(false);
        showWorkString(false);
        showEventString(true);
        showONString(true);
        showLevelString(0, false);
        showLevelString(prevTimeState.eventLevel, true);

        // Event stored day of week - bit value
        showDayOfWeek(0, false);
        showDayOfWeek(prevTimeState.dayOfWeek, true);

        Serial.println(prevTimeState.hour);
        Serial.println(prevTimeState.minute);
        showHour(0, false);
        showHour(prevTimeState.hour, true);
        showColon(true);
        showMinute(0, false);
        showMinute(prevTimeState.minute, true);

        showEventDisplayOnce = true;
      }
      break;
    case eMenu_Aroma_Set_Hour:
      showPauseString(false);
      showOffString(false);
      showWorkString(false);
      showLevelString(0, false);
      showONString(false);
      setHour();
      showColon(true);
      break;
    case eMenu_Aroma_Set_Event_Start_Hour:
      showDayOfWeek(prevTimeState.dayOfWeek, true);
      setHour();
      break;
    case eMenu_Aroma_Set_Event_End:
      if (!showEventDisplayOnce) {
        showOffString(true);
        showONString(false);
        showHour(0, false);
        showHour(prevTimeState.hour, true);
        showMinute(0, false);
        showMinute(prevTimeState.minute, true);
        // Event stored day of week - bit value
        showDayOfWeek(0, false);
        showDayOfWeek(prevTimeState.dayOfWeek, true);

        showEventDisplayOnce = true;
      }
      break;
    case eMenu_Aroma_Set_Event_End_Hour:
      setHour();
      break;
    case eMenu_Aroma_Set_Minutes:
    case eMenu_Aroma_Set_Event_Start_Minute:
    case eMenu_Aroma_Set_Event_End_Minute:
      showHour(prevTimeState.hour, true);
      prevTimeState.minuteFlag = !(prevTimeState.minuteFlag);
      showMinute(prevTimeState.minute, prevTimeState.minuteFlag);
      break;
    case eMenu_Aroma_Set_Day_Of_Week:
      showMinute(prevTimeState.minute, true);
      prevTimeState.dayOfWeekFlag = !(prevTimeState.dayOfWeekFlag);
      showDayOfWeek(1 << prevTimeState.dayOfWeek, prevTimeState.dayOfWeekFlag);
      break;
    case eMenu_Aroma_Set_Event_Start_Day_Of_Week:
      showMinute(prevTimeState.minute, true);
      prevTimeState.dayOfWeekFlag = !(prevTimeState.dayOfWeekFlag);
      showDayOfWeek(prevTimeState.dayOfWeek, prevTimeState.dayOfWeekFlag);
      break;
    case eMenu_Aroma_Set_Event_Work_Level:
      showOffString(false);
      showHour(0, false);
      showColon(false);
      showMinute(0, false);
      showDayOfWeek(0, false);
      showMinute(prevTimeState.workLevel * 10, true);
      break;
  }
}

void setHour() {
  prevTimeState.hourFlag = !(prevTimeState.hourFlag);
  showHour(prevTimeState.hour, prevTimeState.hourFlag);
}
