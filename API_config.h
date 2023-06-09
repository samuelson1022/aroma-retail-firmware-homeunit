
#ifndef API_config_h
#define API_config_h

#define EVENTLISTSIZE          5
#define LEVELLISTSIZE         10

int RequestStatusMOD = 30000;
int API_Get_Status(String serverPath, int bUpdateRTCFlag, int* piAction);
int API_Get_Events(char* szSerial);
int API_Get_ACK(char* szSerial, char* ACK_Type);
//int bVacation_Hold = 0;
char szUnitSerial[] = "Aroma-005-220221";

typedef enum
{
  APIActionCode_Idle,
  APIActionCode_Override,
  APIActionCode_OverrideCancel,
  APIActionCode_GetEvents,
  APIActionCode_GetFWUpdate,
  APIActionCode_VacationHold,
  APIActionCode_FriendlyName
} eAPIActionCode;

typedef struct
{
  uint8_t StartHour;              // 24 hour clock
  uint8_t StartMinute;            //
  uint8_t EndHour;                // 24 hour clock
  uint8_t EndMinute;              //
  uint8_t DayOfWeek;              //  MO TU WE TH FR SA SU  (0=OFF / 1=ON) - 1111100  (off for sat and sunday and on for MO-FR)
  uint8_t FanSpeed;               //
  uint16_t Level;
  uint16_t WorkSeconds;            //
  uint16_t PauseSeconds;           //
}
EventStructType;

typedef struct
{
  uint16_t  WorkSeconds;
  uint16_t  PauseSeconds;
} 
LevelStructType;

typedef struct
{
  uint16_t  uHeaderID; // set to SCBHEADERID when initialized
  uint16_t  SCB_State;
  uint8_t   MACAddress[6];
  char    szUnitSerial[32];
  char    szPCBSerial[16];
  char    szUnitName[16];
  char    szSSID[32];
  char    szPW[64];
  uint16_t  Secs_Per_Bottle;
  EventStructType   Events[EVENTLISTSIZE];
  LevelStructType   Levels[LEVELLISTSIZE];
} SCBStructType;

SCBStructType StateCB;

#define SERVER          "http://3.101.17.195:8081/api/v1"
#define API_EVENTS      "ar_GetEvents"
#define API_FWINFO      "ar_GetFWUpdateInfo"
#define API_FWPACKET    "ar_GetFWUpdatePacket"
#define API_OVERRIDE    "ar_Override"
#define API_OVERRIDECANCEL  "ar_OverrideCancel"
#define API_ACK       "ar_ACK"
#define API_METRICS     "ar_Metrics"
#define API_SETEVENT    "ar_SetEvent"
#define API_FRIENDLYNAME  "ar_FriendlyName"
#define API_STATUS      "ar_Status"

#endif
