#include "flash.h"

#define StateCB_addr  (0)

char* Get_MAC_Address_Str(char* szAddress)
{
  sprintf(szAddress, "%s", StateCB.MACAddress);
  return szAddress;
}

int read_struct()
{
  EEPROM.get(StateCB_addr, StateCB);
  return 0;
}

void Init_SCB()
{
  char szMesg[20];

  // Init FLASH and read the State Control Block
  EEPROM.begin(sizeof(SCBStructType));

  // Read serial block from FLASH and overlay on SCB structure
  read_struct();

  // If the HeaderID is not defined, initialize SCB and write back to FLASH
  if (StateCB.uHeaderID != SCBHEADERID)
  {
    printf("Initializing SCB\r\n");
    StateCB.uHeaderID = SCBHEADERID;
    StateCB.SCB_State = eSCB_CM_Validate;
    strcpy(StateCB.szUnitSerial, "Aroma-001-201111");
    strcpy(StateCB.szUnitName, "Aroma Dispenser");
    strcpy(StateCB.szPCBSerial, "?");
    strcpy(StateCB.szSSID, "");
    strcpy(StateCB.szPW, "");
    strcpy(StateCB.MACAddress, "FC:CD:2F:90:03:FF");
    StateCB.Levels[0].WorkSeconds =  3; StateCB.Levels[0].PauseSeconds = 160;
    StateCB.Levels[1].WorkSeconds = 10; StateCB.Levels[1].PauseSeconds =  90;
    StateCB.Levels[2].WorkSeconds = 15; StateCB.Levels[2].PauseSeconds =  60;
    StateCB.Levels[3].WorkSeconds = 30; StateCB.Levels[3].PauseSeconds =  70;
    StateCB.Levels[4].WorkSeconds = 40; StateCB.Levels[4].PauseSeconds =  60;
    StateCB.Levels[5].WorkSeconds = 60; StateCB.Levels[5].PauseSeconds =  60;
    StateCB.Levels[6].WorkSeconds = 60; StateCB.Levels[6].PauseSeconds =  40;
    StateCB.Levels[7].WorkSeconds = 70; StateCB.Levels[7].PauseSeconds =  30;
    StateCB.Levels[8].WorkSeconds = 80; StateCB.Levels[8].PauseSeconds =  20;
    StateCB.Levels[9].WorkSeconds =  0; StateCB.Levels[9].PauseSeconds =  0;

    Set_Factory_Defaults();
    Update_SCB();
  }
  else
  {
    printf("Reading SCB\r\n");
    StateCB.SCB_State &= 0xFF; // Mask the update bit
  }
//  printf("SCB Size %i Bytes\r\n", sizeof(StateCB));
//  printf("SCB State: %i\r\n", StateCB.SCB_State);
//  printf("UNIT S/N:  %s\r\n", StateCB.szUnitSerial);
//  printf("PCB  S/N:  %s\r\n", StateCB.szPCBSerial);
//  printf("Name: %s\r\n", StateCB.szUnitName);
//  printf("MAC:  %s\r\n", Get_MAC_Address_Str(szMesg));
//  printf("SSID: %s\r\n", StateCB.szSSID);
//  printf("PW:   %s\r\n", StateCB.szPW);
}
void Set_Factory_Defaults()
{
  int i;
  memset(StateCB.Events, 0, sizeof(EventStructType) * EVENTLISTSIZE);
  //Default events
  for (i = 0; i < EVENTLISTSIZE; i++)
  {
    // Defaults for all events
    StateCB.Events[i].StartHour = 0;
    StateCB.Events[i].StartMinute = 0;
    StateCB.Events[i].EndHour = 0;
    StateCB.Events[i].EndMinute = 0;
    StateCB.Events[i].WorkSeconds = 15;
    StateCB.Events[i].PauseSeconds = 60;
    StateCB.Events[i].DayOfWeek = 0b00000000; // all days for event 1 only
    StateCB.Events[i].Level = 2; // "Level 3"
    StateCB.Events[i].FanSpeed = 1; // "Low"
  }
  // Defaults for all events
  StateCB.Events[0].StartHour = 8;
  StateCB.Events[0].StartMinute = 30;
  StateCB.Events[0].EndHour = 20;
  StateCB.Events[0].EndMinute = 30;
  StateCB.Events[0].DayOfWeek = 0b01111111; // all days for event 1 only
  Update_SCB();
}
void Update_SCB()
{
  EEPROM.put(StateCB_addr, StateCB);
  EEPROM.commit();
//  Serial.println("updated");
  //Init_Pump_State(); //Forces new program parameters to load
}
