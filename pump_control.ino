#include "pump_control.h"

stPumpControl PumpControl = { 0 };

void Handle_Pump_State(void) {
  char szControl[96];
  int iEvent = 0;
  int iDOW = 0;

  // If the pump is engaged, process the counters
  // If not, check event programming to enable the pump
  // This handler is not called during a vacation hold
  if (PumpControl.EnabledFlag) {
    Marquee_State = MarqueeState_Event_Running;
    switch (PumpControl.PumpState) {
      Serial.println("PumpControl.PumpState = " + String(PumpControl.PumpState));
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
        sprintf(szControl, "Wrk%3is Paus%3is", PumpControl.WorkSecsRemaining - 1, pControlRec->Pause);
        //Print_VLCD(2, 1, 0, szControl);  
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
        sprintf(szControl, "Wrk%3is Paus%3is", pControlRec->Work, PumpControl.PauseSecsRemaining - 1);
        //Print_VLCD(2, 1, 0, szControl);
        break;
    }  // end switch
  } else {
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
  digitalWrite(Pump_CTRL_Pin, bOn ? HIGH : LOW);
  Serial.println("PumpActuate");
}

/*int GetAirFlowGood(void)
{

#ifdef SIMULATEFLOWSENSOR
  return GetManufShunt() ? 0 : 1; // SHUNT ON --> No Flow detected
  // SHUNT OFF --> Flow Good or not connected
#else
  //return HAL_GPIO_ReadPin(AirFlow_Good_GPIO_Port, AirFlow_Good_Pin) == GPIO_PIN_SET ? 1 : 0;
  return (digitalRead(AirFlow_Good_Pin));


}

int GetManufShunt()
{
  return (digitalRead(MFG_Test_Pin));
}

void Check_Flow_Condition()
{
  int oPumpPin = digitalRead(Pump_CTRL_Pin);
  if (oPumpPin == HIGH && !GetAirFlowGood())
  {
    PumpActuate(0);
    //printf("Pump disabled due to no air flow detected...\r\n");
  }
}*/

void Init_Pump_State() {
  Serial.println("Init_Pump_State");
  PumpControl.EnabledFlag = 0;
}

void Start_Pump_Control(stEventControl* pControl) {
  Serial.println("Start_Pump_Control");
  PumpControl.PumpState = ePumpStateBegin;
  PumpControl.WorkSecsRemaining = pControl->Work;
  PumpControl.PauseSecsRemaining = pControl->Pause;
  PumpControl.FanSpeed = (eSpeedType)pControl->Fan_Speed;
  PumpControl.EnabledFlag = 1;
  pControlRec = pControl;
}

void End_Pump_Control() {
  Serial.println("End_Pump_Control");
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

  Serial.println("Schedule_Override");

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
