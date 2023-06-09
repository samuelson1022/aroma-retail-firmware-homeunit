#include "events.h"
#include "pump_control.h"


int GetNextPendingEvent(int* piDOW)
{
  // Scan active events for next one
  // Find Min(Seconds) to those events
  long MinSeconds = SECONDSINDAY * 7;
  long SecondsToStart;
  long CurrentSeconds = (CurrentTime.Hours * 60 * 60) + (CurrentTime.Minutes * 60) + CurrentTime.Seconds;
  int i, j;
  int iMinIndex = -1;
  int iDayOfWeek = 0;
  int DOWMask; 
  int StartSecs;
  
  for (i = 0; i < EVENTLISTSIZE; i++)
  {
    DOWMask = Get_DOW_Mask(CurrentDate.WeekDay);
    StartSecs = (StateCB.Events[i].StartHour * 60 * 60) + (StateCB.Events[i].StartMinute * 60);
    for (j = 0; j < 7; j++)
    {
      if ((StateCB.Events[i].DayOfWeek & DOWMask) > 0)
      {
        SecondsToStart = (j * SECONDSINDAY) + ((StateCB.Events[i].StartHour * 60 * 60) + (StateCB.Events[i].StartMinute * 60)) - CurrentSeconds;
        SecondsToStart = SecondsToStart < 0 ? SecondsToStart + SECONDSINDAY : SecondsToStart;
        // If event has passed on the start day then ignore
        if (j == 0 && CurrentSeconds > StartSecs) 
        {
          // Event has already passed on current day
          DOWMask = DOWMask == 0x1 ? 0x40 : DOWMask >> 1;
          continue;
        }
        if (SecondsToStart > 0)
        {
          // This event starts later
          if (SecondsToStart < MinSeconds)
          {
            // Track smallest SecondsToStart
            MinSeconds = SecondsToStart;
            iMinIndex = i;
            iDayOfWeek = (((CurrentDate.WeekDay - 1) + j) % 7) + 1;
          }
        }
      }
      // Next day
      DOWMask = DOWMask == 0x1 ? 0x40 : DOWMask >> 1;
    }
  }
  *piDOW = iDayOfWeek;
  return iMinIndex;
}


int HasEventPending(int* piEvent, int* piDOW)
{
  *piEvent = GetNextPendingEvent(piDOW);
  return *piEvent < 0 ? 0 : 1;
}


uint8_t CheckBetweenTimes(int RTCSeconds, int TimeA_Seconds, int TimeB_Seconds)
{
  // Checks to see if the current time is "between" a start and end time
  // Takes midnight rollover into consideration
  //00:00 ----------------------- AXXXXXXXXXXXXXXXXXXXXXXXB ------- 23:59
  if(TimeB_Seconds >= TimeA_Seconds) 
  {
    if (RTCSeconds >= TimeA_Seconds && RTCSeconds < TimeB_Seconds)
    {
      return 1;
    }
  }   
  //00:00 XXXxxxxXXXB --------------------------------AXXXXXXXXXXXX 23:59
  else              
  { 
    if (
      (RTCSeconds < SECONDSINDAY && RTCSeconds >= TimeA_Seconds) ||
      (RTCSeconds < TimeB_Seconds))
    {
      return 1;
    }
  }
  return 0;
}

int8_t Get_DOW_Mask(int iDayOfWeek)
{
  return 0x01 << iDayOfWeek;
}

int HasEventActive(int* piEvent)
{
  int StartSeconds;
  int EndSeconds;
  int CurrentSeconds;
  int i;
  
  CurrentSeconds = (CurrentTime.Hours * 60 * 60) + (CurrentTime.Minutes * 60) + CurrentTime.Seconds;
  for (i = 0; i < EVENTLISTSIZE; i++)
  {
    if ((StateCB.Events[i].DayOfWeek & Get_DOW_Mask(CurrentDate.WeekDay)) > 0)
    {
      StartSeconds = (StateCB.Events[i].StartHour * 60 * 60) + (StateCB.Events[i].StartMinute * 60);
      EndSeconds = (StateCB.Events[i].EndHour * 60 * 60) + (StateCB.Events[i].EndMinute * 60);
      
      if(CheckBetweenTimes(CurrentSeconds, StartSeconds, EndSeconds))
      {
        // First event wins
        *piEvent = i;
        return 1;
      }
    }
  }
  return 0;
}

void Schedule_Event()
{
  int iEvent;
  Serial.println("Schedule_Event()");
  if (HasEventActive(&iEvent))  
  {
    EventControl.Start_Hour = StateCB.Events[iEvent].StartHour;
    EventControl.Start_Minute = StateCB.Events[iEvent].StartMinute;
    EventControl.End_Hour = StateCB.Events[iEvent].EndHour;
    EventControl.End_Minute = StateCB.Events[iEvent].EndMinute;
    EventControl.Work = StateCB.Events[iEvent].WorkSeconds;
    EventControl.Pause = StateCB.Events[iEvent].PauseSeconds;
    EventControl.Fan_Speed = StateCB.Events[iEvent].FanSpeed;
    EventControl.Pending = 0;
    Start_Pump_Control(&EventControl);
  }
}
