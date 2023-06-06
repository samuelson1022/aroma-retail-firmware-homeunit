
#ifndef flash_h
#define flash_h

#include <EEPROM.h>

#define EEPROM_SIZE sizeof(SCBStructType)

#define SCBHEADERID            0x7388

typedef enum
{
  eSCB_CM_Validate,
  eSCB_OnBoarding,
  eSCB_Running
} eSCBStateType;

typedef enum
{
  eControl_None,
  eControl_Timer,
  eControl_Hold
} eControlStateType;

void Init_SCB();
void Set_Factory_Defaults();
void Update_SCB();

#endif
