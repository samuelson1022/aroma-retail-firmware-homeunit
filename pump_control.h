
#ifndef pump_h
#define pump_h

typedef enum
{
  eSpeed_Off,
  eSpeed_Low,
  eSpeed_Med,
  eSpeed_Hi
} eSpeedType;

typedef enum
{
  ePumpStateBegin,
  ePumpStateWorking,
  ePumpStatePausing
} ePumpControlState;

typedef struct
{
  uint8_t       EnabledFlag;
  ePumpControlState PumpState;
  int16_t       WorkSecsRemaining;
  int16_t       PauseSecsRemaining;
  eSpeedType      FanSpeed;
}
stPumpControl;

typedef struct
{
  uint8_t   HoldFlag;
  uint8_t   Pending;
  uint8_t   State;
  uint8_t   Start_Hour;
  uint8_t   Start_Minute;
  uint8_t   End_Hour;
  uint8_t   End_Minute;
  uint16_t  Work;
  uint16_t  Pause;
  uint8_t   Fan_Speed;
  char    szDesc[8];
}
stEventControl;

int Seconds_Of_Use = 0;
extern stPumpControl PumpControl;
stEventControl OverrideControl = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Ovr:" };
stEventControl EventControl = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Evt:" };
stEventControl* pControlRec = &EventControl;

void Handle_Pump_State(void );

#endif
