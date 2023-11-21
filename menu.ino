#include "events.h"
#include "lcd.h"

eMarqueeState Marquee_State;
TimePreviousState prevTimeState;

int countVariable = 1;
bool showEventDisplayOnce = false;
int windIndex = 0;

unsigned long lastBLTime = millis();

void Menu_Display() {
  switch (MenuType) {
    case eMenu_Aroma_Home:
      lcdRefreshTime = 500;
      showLevelString(5, false);
      showEventString(false);
      showPercent(false);
      //      if (prevTimeState.preMarqueeState != Marquee_State) {
      switch (Marquee_State) {
        case MarqueeState_Event_Running:
          showPauseString(false);
          showOffString(false);
          showWorkString(true);
          showONString(true);
          showWind(windIndex);
          windIndex ++;
          if (windIndex == 4) windIndex = 1;
          break;
        case MarqueeState_No_Event:
          showWorkString(false);
          showONString(false);
          showPauseString(true);
          showOffString(true);
          showWind(0);
          break;
      }
      prevTimeState.preMarqueeState = Marquee_State;
      //      }

      rtc_get(&CurrentTime, &CurrentDate);
      prevTimeState.dayOfWeek = 1 << CurrentDate.WeekDay;
//      prevTimeState.dayOfWeek = StateCB.Events[0].DayOfWeek;
//      for (countVariable = 0; countVariable < 5; countVariable ++) {
//        if (((1 << (CurrentDate.WeekDay - 1)) & StateCB.Events[countVariable].DayOfWeek) != 0) {
//          prevTimeState.dayOfWeek = StateCB.Events[countVariable].DayOfWeek;
//          break;
//        }
//      }
      showDayOfWeek(prevTimeState.dayOfWeek, false);
      showDayOfWeek(prevTimeState.dayOfWeek, true);

      if (prevTimeState.hour != CurrentTime.Hours) {
        // showHour(CurrentTime.Hours, false);
        showHour(CurrentTime.Hours, true);
        prevTimeState.hour = CurrentTime.Hours;
      }

      prevTimeState.colonFlag = !(prevTimeState.colonFlag);
      showColon(prevTimeState.colonFlag);

      if (prevTimeState.minute != CurrentTime.Minutes) {
        // showMinute(CurrentTime.Minutes, false);
        showMinute(CurrentTime.Minutes, true);
        prevTimeState.minute = CurrentTime.Minutes;
      }
      break;
    case eMenu_Aroma_Set_Events:
      showWind(0);
      showPercent(false);
      if (!showEventDisplayOnce) {
        // Event stored day of week - bit value
        showDayOfWeek(0, false);
        showDayOfWeek(prevTimeState.dayOfWeek, true);

        //        Serial.println(prevTimeState.hour);
        //        Serial.println(prevTimeState.minute);
        showHour(0, false);
        showHour(prevTimeState.hour, true);
        showColon(true);
        showMinute(0, false);
        showMinute(prevTimeState.minute, true);

        showPauseString(false);
        showOffString(false);
        showWorkString(false);
        showEventString(true);
        showONString(true);
        showLevelString(0, false);
        showLevelString(prevTimeState.eventLevel, true);

        showEventDisplayOnce = true;
      }
      break;
    case eMenu_Aroma_Set_Hour:
      showWind(0);
      showPauseString(false);
      showOffString(false);
      showWorkString(false);
      showLevelString(0, false);
      showONString(false);
      setHour();
      showColon(true);
      rtc_get(&CurrentTime, &CurrentDate);
      prevTimeState.dayOfWeek = CurrentDate.WeekDay;

      showDayOfWeek(0, false);
      showDayOfWeek(1 << (prevTimeState.dayOfWeek - 1), true);
      break;
    case eMenu_Aroma_Set_Event_Start_Hour:
      lcdRefreshTime = 500;
      showDayOfWeek(prevTimeState.dayOfWeek, true);
      setHour();
      showMinute(prevTimeState.minute, true);
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
      showMinute(prevTimeState.minute, true);
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
      //      iDayOfWeek = (((CurrentDate.WeekDay - 1) + j) % 7) + 1;
      showDayOfWeek(1 << prevTimeState.dayOfWeek, prevTimeState.dayOfWeekFlag);
      lastBLTime = millis();
      break;
    case eMenu_Aroma_Set_Event_Start_Day_Of_Week:
      showMinute(prevTimeState.minute, true);
      prevTimeState.dayOfWeekFlag = !(prevTimeState.dayOfWeekFlag);
      if (prevTimeState.dayOfWeekFlag)
        showDayOfWeek(prevTimeState.dayOfWeek, true);
      else {
        if ((prevTimeState.dayOfWeek & (1 << eventDoWSetIndex)) == (1 << eventDoWSetIndex)) {
          lcdRefreshTime = 70;
          showDayOfWeek(prevTimeState.dayOfWeek & (~(1 << eventDoWSetIndex)), true);
        }
        else {
          lcdRefreshTime = 500;
          showDayOfWeek(prevTimeState.dayOfWeek | (1 << eventDoWSetIndex), true);
        }
      }
      break;
    case eMenu_Aroma_Set_Event_Work_Level:
      lcdRefreshTime = 500;
      showOffString(false);
      showHour(0, false);
      showColon(false);
      showMinute(0, false);
      showDayOfWeek(0, false);
      prevTimeState.minuteFlag = !(prevTimeState.minuteFlag);
      showMinute(prevTimeState.workLevel * 10, prevTimeState.minuteFlag);
      showPercent(true);
      lastBLTime = millis();
      break;
  }
}

void setHour() {
  prevTimeState.hourFlag = !(prevTimeState.hourFlag);
  showHour(prevTimeState.hour, prevTimeState.hourFlag);
}
