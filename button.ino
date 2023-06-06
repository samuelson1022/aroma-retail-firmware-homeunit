
#include "rtc.h"
#include "button.h"
#include "menu.h"
// #include "lcd.h"

// unsigned long int ButtonDownMsec = 0;
eButtonType Button_Type = eButton_None;
ePressState Button_Press = ePress_None;
// int bRequest_Handle_Button_Press = 0;
extern TimePreviousState prevTimeState;
int eventDoWSetIndex = 0;

eMenuType MenuType;
void Menu_Display(void);

void Button_Scan() {
  switch (Button_Press) {
    case ePress_None:
      if (GetButtonsDown()) {
        // We have a button press
        ButtonDownMsec = 0;
        Button_Press = ePress_Pushed;
      }
      break;
    case ePress_Pushed:
      // Allow for one or more buttons to set up
      if (ButtonDownMsec >= ButtonDebounceMsec) {
        Button_Type = GetButtonsDown();
        Button_Press = (GetButtonsDown()) ? ePress_Down : ePress_None;
      }
      break;
    case ePress_Down:
      if (GetButtonsDown()) {
        // Check timer for repeat
        if (ButtonDownMsec >= ButtonRepeatMsec) {
          // Serial.println("Button Press Repeat!");
          Button_Press = ePress_Repeat;
        }
      } else {
        if (ButtonDownMsec >= ButtonShortMsec) {
          // Button was released for a short press
          // Serial.println("Button Press Short!");
          Button_Press = ePress_Short;
        } else {
          // Button "bounced"
          Button_Press = ePress_None;
        }
      }
      break;
    case ePress_Short:
      bRequest_Handle_Button_Press = 1;
      break;
    case ePress_Repeat:
      if (!GetButtonsDown()) {
        ButtonDownMsec = 0;
        Button_Press = ePress_None;
      }
      bRequest_Handle_Button_Press = 1;
      break;
  }
}

eButtonType GetButtonsDown() {
  eButtonType oButton = eButton_None;
  if (digitalRead(BUTTON_MODE) == LOW) {
    oButton = eButton_Mode;
  }

  if (digitalRead(BUTTON_SET) == LOW) {
    oButton = eButton_Set;
  }

  if (digitalRead(BUTTON_UP) == LOW) {
    oButton = eButton_Up;
  }

  if (digitalRead(BUTTON_DOWN) == LOW) {
    oButton = eButton_Down;
  }

  return oButton;
}

void Handle_Button_Press() {
  switch (Button_Type) {
    case eButton_None:
      break;
    case eButton_Up:
      switch (Button_Press) {
        case ePress_Short:
          Handle_Up_Short();
          Button_Press = ePress_None;
          break;
        case ePress_Repeat:
          Handle_Up_Repeat();
          break;
      }
      break;
    case eButton_Down:
      switch (Button_Press) {
        case ePress_Short:
          Handle_Down_Short();
          Button_Press = ePress_None;
          break;
        case ePress_Repeat:
          Handle_Down_Repeat();
          break;
      }
      break;

    case eButton_Set:
      switch (Button_Press) {
        case ePress_Short:
          Handle_Set_Short();
          Button_Press = ePress_None;
          break;
        case ePress_Repeat:
          break;
      }
      break;
    case eButton_Mode:
      switch (Button_Press) {
        case ePress_Short:
          Handle_Mode_Short();
          Button_Press = ePress_None;
          break;
        case ePress_Repeat:
          break;
      }
      break;
  }
}

void Handle_Set_Short() {
  switch (MenuType) {
    case eMenu_Aroma_Home:
      MenuType = eMenu_Aroma_Set_Hour;
      break;
    case eMenu_Aroma_Set_Hour:
      // TimeSetBuf.TimeSetState = eTimeSetMinutes;
      MenuType = eMenu_Aroma_Set_Minutes;
      break;
    case eMenu_Aroma_Set_Minutes:
      // TimeSetBuf.TimeSetState = eTimeSetDayOfWeek;
      MenuType = eMenu_Aroma_Set_Day_Of_Week;
      break;
    case eMenu_Aroma_Set_Day_Of_Week:
      SaveToRTC();
      prevTimeState.preMarqueeState = MarqueeState_Updating_FW;
      MenuType = eMenu_Aroma_Home;
      break;
    case eMenu_Aroma_Set_Events:
      MenuType = eMenu_Aroma_Set_Event_Start_Hour;
      break;
    case eMenu_Aroma_Set_Event_Start_Hour:
      MenuType = eMenu_Aroma_Set_Event_Start_Minute;
      break;
    case eMenu_Aroma_Set_Event_Start_Minute:
      MenuType = eMenu_Aroma_Set_Event_Start_Day_Of_Week;
      break;
    case eMenu_Aroma_Set_Event_Start_Day_Of_Week:
      MenuType = eMenu_Aroma_Set_Event_Start_Hour;
      break;
    case eMenu_Aroma_Set_Event_End:
      MenuType = eMenu_Aroma_Set_Event_End_Hour;
      break;
    case eMenu_Aroma_Set_Event_End_Hour:
      MenuType = eMenu_Aroma_Set_Event_End_Minute;
      break;
    case eMenu_Aroma_Set_Event_End_Minute:
      MenuType = eMenu_Aroma_Set_Event_End_Hour;
      break;
    case eMenu_Aroma_Set_Event_End_Day_Of_Week:
      // Update_SCB();
      // TimeSetBuf.TimeSetState = eTimeSetDisabled;
      MenuType = eMenu_Aroma_Set_Event_End_Hour;
      break;
  }
}
void Handle_Mode_Short() {
  switch (MenuType) {
    case eMenu_Aroma_Home:
      prevTimeState.eventLevel = 1;
      prevTimeState.hour = StateCB.Events[prevTimeState.eventLevel - 1].StartHour;
      prevTimeState.minute = StateCB.Events[prevTimeState.eventLevel - 1].StartMinute;
      prevTimeState.dayOfWeek = StateCB.Events[prevTimeState.eventLevel - 1].DayOfWeek;
      prevTimeState.workLevel = StateCB.Events[prevTimeState.eventLevel - 1].Level;

      showEventDisplayOnce = false;
      MenuType = eMenu_Aroma_Set_Events;
      break;
    case eMenu_Aroma_Set_Events:
    case eMenu_Aroma_Set_Event_Start_Hour:
    case eMenu_Aroma_Set_Event_Start_Minute:
    case eMenu_Aroma_Set_Event_Start_Day_Of_Week:
      StateCB.Events[prevTimeState.eventLevel - 1].StartHour = prevTimeState.hour;
      StateCB.Events[prevTimeState.eventLevel - 1].StartMinute = prevTimeState.minute;
      StateCB.Events[prevTimeState.eventLevel - 1].DayOfWeek = prevTimeState.dayOfWeek;
      Update_SCB();
      prevTimeState.hour = StateCB.Events[prevTimeState.eventLevel - 1].EndHour;
      prevTimeState.minute = StateCB.Events[prevTimeState.eventLevel - 1].EndMinute;

      showEventDisplayOnce = false;
      MenuType = eMenu_Aroma_Set_Event_End;
      break;
    case eMenu_Aroma_Set_Event_End:
    case eMenu_Aroma_Set_Event_End_Hour:
    case eMenu_Aroma_Set_Event_End_Minute:
      StateCB.Events[prevTimeState.eventLevel - 1].EndHour = prevTimeState.hour;
      StateCB.Events[prevTimeState.eventLevel - 1].EndMinute = prevTimeState.minute;
      Update_SCB();
      MenuType = eMenu_Aroma_Set_Event_Work_Level;
      break;
    case eMenu_Aroma_Set_Event_Work_Level:
      StateCB.Events[prevTimeState.eventLevel - 1].Level = prevTimeState.workLevel;
      Update_SCB();
      prevTimeState.eventLevel++;
      if (prevTimeState.eventLevel > EVENTLISTSIZE) {
        prevTimeState.preMarqueeState = MarqueeState_Updating_FW;
        MenuType = eMenu_Aroma_Home;
        prevTimeState.dayOfWeek = 7;  // Random value to show the current DoW
      } else {
        prevTimeState.hour = StateCB.Events[prevTimeState.eventLevel - 1].StartHour;
        prevTimeState.minute = StateCB.Events[prevTimeState.eventLevel - 1].StartMinute;
        prevTimeState.dayOfWeek = StateCB.Events[prevTimeState.eventLevel - 1].DayOfWeek;
        prevTimeState.workLevel = StateCB.Events[prevTimeState.eventLevel - 1].Level;
        showEventDisplayOnce = false;
        MenuType = eMenu_Aroma_Set_Events;
      }
      break;
  }
}
void Handle_Up_Short() {
  switch (MenuType) {
    case eMenu_Aroma_Set_Hour:
    case eMenu_Aroma_Set_Event_Start_Hour:
    case eMenu_Aroma_Set_Event_End_Hour:
      prevTimeState.hour = (prevTimeState.hour + 1) % 24;
      break;
    case eMenu_Aroma_Set_Minutes:
    case eMenu_Aroma_Set_Event_Start_Minute:
    case eMenu_Aroma_Set_Event_End_Minute:
      prevTimeState.minute = (prevTimeState.minute + 1) % 60;
      break;
    case eMenu_Aroma_Set_Day_Of_Week:
      prevTimeState.dayOfWeek = (prevTimeState.dayOfWeek + 1) % 7;
      break;
    case eMenu_Aroma_Set_Event_Start_Day_Of_Week:
    case eMenu_Aroma_Set_Event_End_Day_Of_Week:
      eventDoWSetIndex = (eventDoWSetIndex + 1) % 15;
      prevTimeState.dayOfWeek = eventDoWSet[eventDoWSetIndex];
      break;
    case eMenu_Aroma_Set_Event_Work_Level:
      prevTimeState.workLevel = (prevTimeState.workLevel + 1) % 10;
      break;
  }
}

void Handle_Up_Repeat() {}

int iDiff = 0;

void Handle_Down_Short() {
  switch (MenuType) {
    case eMenu_Aroma_Set_Hour:
    case eMenu_Aroma_Set_Event_Start_Hour:
    case eMenu_Aroma_Set_Event_End_Hour:
      iDiff = prevTimeState.hour - 1;
      iDiff = iDiff < 0 ? 24 + iDiff : iDiff;
      prevTimeState.hour = iDiff;
      break;
    case eMenu_Aroma_Set_Minutes:
    case eMenu_Aroma_Set_Event_Start_Minute:
    case eMenu_Aroma_Set_Event_End_Minute:
      iDiff = prevTimeState.minute - 1;
      iDiff = iDiff < 0 ? 60 + iDiff : iDiff;
      prevTimeState.minute = iDiff;
      break;
    case eMenu_Aroma_Set_Day_Of_Week:
      prevTimeState.dayOfWeek = (prevTimeState.dayOfWeek + 6) % 7;
      break;
    case eMenu_Aroma_Set_Event_Start_Day_Of_Week:
    case eMenu_Aroma_Set_Event_End_Day_Of_Week:
      eventDoWSetIndex = (eventDoWSetIndex + 14) % 15;
      prevTimeState.dayOfWeek = eventDoWSet[eventDoWSetIndex];
      break;
    case eMenu_Aroma_Set_Event_Work_Level:
      iDiff = prevTimeState.workLevel - 1;
      iDiff = iDiff < 0 ? 10 + iDiff : iDiff;
      prevTimeState.workLevel = iDiff;
      break;
  }
}

void Handle_Down_Repeat() {}
