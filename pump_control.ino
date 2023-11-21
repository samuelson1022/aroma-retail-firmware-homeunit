#include "pump_control.h"

stPumpControl PumpControl = { 0 };

void Handle_Pump_State(void) {
  //char szControl[VLCDCOLMAX];
  int iEvent = 0;
  int iDOW = 0;

  // If the pump is engaged, process the counters
  // If not, check event programming to enable the pump
  // This handler is not called during a vacation hold
  if (PumpControl.EnabledFlag) {
    Marquee_State = MarqueeState_Event_Running;
    switch (PumpControl.PumpState) {
//        Serial.println("PumpControl.PumpState = " + String(PumpControl.PumpState));
      case ePumpStateBegin:
        PumpControl.PumpState = ePumpStateWorking;
        Seconds_Of_Use += PumpControl.WorkSecsRemaining;
        break;
      case ePumpStateWorking:

        // Serial.println("PumpControl.WorkSecsRemaining = " + String(PumpControl.WorkSecsRemaining));
        // Check for air flow
        //if (GetAirFlowGood())
        //{
        //SetBlowerSpeed(PumpControl.FanSpeed);
        PumpActuate(1);
        if (--PumpControl.WorkSecsRemaining <= 0) {
          //SetBlowerSpeed(eSpeed_Off);
          PumpActuate(0);
          PumpControl.PumpState = ePumpStatePausing;
        }
        //}
        //else
        //{
        //PumpControl.WorkSecsRemaining = EventControl.Work;
        //PumpControl.PauseSecsRemaining = EventControl.Pause;
        //SetBlowerSpeed(eSpeed_Off);
        //PumpActuate(0);
        //}
        break;
      case ePumpStatePausing:
        //bEnableTimeUpdate = 0;
        //if (GetAirFlowGood())
        //{
        if (--PumpControl.PauseSecsRemaining <= 0) {
          // Control for this segment is completed
          PumpControl.EnabledFlag = 0;
          //bEnableTimeUpdate = 1;
        }
        //}
        //else
        //{
        // PumpControl.WorkSecsRemaining = EventControl.Work;
        // PumpControl.PumpState = ePumpStateWorking;
        //}
        break;
    }  // end switch
  }
  else {
    Marquee_State = MarqueeState_No_Event;
    // Serial.println("else condition, PumpControl.EnabledFlag");
    rtc_get(&CurrentTime, &CurrentDate);
    // Check the Override control block to see if an Override is pending
    switch (OverrideControl.State) {
      case eControl_None:

        // Check the Event control block to see if an Event is pending
        if (HasEventPending(&iEvent, &iDOW)) {
          Schedule_Event();
        }
        break;
      case eControl_Timer:
        // Check Override times to schedule pump control
        Schedule_Override(0);  // no hold
        break;
      case eControl_Hold:
        Schedule_Override(1);  // hold
        break;
    }
  }
}

void PumpActuate(int bOn) {
  int fanDuty = 0;
  digitalWrite(Pump_CTRL_Pin, bOn ? HIGH : LOW);
  if (bOn) {
    switch (PumpControl.FanSpeed) {
      case   eSpeed_Off:
        fanDuty = 0;
        break;
      case eSpeed_Low:
        fanDuty = 90;
        break;
      case eSpeed_Med:
        fanDuty = 180;
        break;
      case eSpeed_Hi:
        fanDuty = 240;
        break;
    }
  } else {
    fanDuty = 0;
  }
  ledcWrite(0, fanDuty);
//  Serial.print("PumpActuate: ");
//  Serial.println(fanDuty);
}

void Init_Pump_State() {
//  Serial.println("Init_Pump_State");
  PumpControl.EnabledFlag = 0;
}

void Start_Pump_Control(stEventControl* pControl) {
//  Serial.println("Start_Pump_Control");
  PumpControl.PumpState = ePumpStateBegin;
  PumpControl.WorkSecsRemaining = pControl->Work;
  PumpControl.PauseSecsRemaining = pControl->Pause;
  PumpControl.FanSpeed = (eSpeedType)pControl->Fan_Speed;
  PumpControl.EnabledFlag = 1;
  pControlRec = pControl;
}

void End_Pump_Control() {
//  Serial.println("End_Pump_Control");
  PumpControl.EnabledFlag = 0;
  pControlRec->State = eControl_None;
  PumpControl.PumpState = ePumpStateBegin;
  OverrideControl.Pending = 0;
  OverrideControl.State = eControl_None;
  //SetBlowerSpeed(eSpeed_Off);
  PumpActuate(0);
  //bEnableTimeUpdate = 1;
}


void Schedule_Override(int bHoldFlag) {
  int StartSeconds;
  int EndSeconds;
  int CurrentSeconds;

//  Serial.println("Schedule_Override");

  CurrentSeconds = (CurrentTime.Hours * 60 * 60) + (CurrentTime.Minutes * 60) + CurrentTime.Seconds;
  StartSeconds = (OverrideControl.Start_Hour * 60 * 60) + (OverrideControl.Start_Minute * 60);
  EndSeconds = (OverrideControl.End_Hour * 60 * 60) + (OverrideControl.End_Minute * 60);
  if (OverrideControl.HoldFlag) {
    printf("Ovr: Hold\r\n");
  } else {
    printf("Ovr: Current=%i, Start=%i, End=%i\r\n", CurrentSeconds, StartSeconds, EndSeconds);
  }
  // Are we outside the override control band?  Handle the "24 hour" wrap.
  // Or are we in HOLD mode
  if (CheckBetweenTimes(CurrentSeconds, StartSeconds, EndSeconds) || OverrideControl.HoldFlag) {
    Start_Pump_Control(&OverrideControl);
    OverrideControl.Pending = 0;
  } else {
    // We are outside the control band
    // Set state to "none" if we are not waiting for a pending override
    if (!OverrideControl.Pending) {
      OverrideControl.State = eControl_None;
    }
  }
}
