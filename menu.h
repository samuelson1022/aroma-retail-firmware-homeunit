// menu.h
#ifndef menu_h
#define menu_h

typedef enum
{
	eMenu_Aroma_Home,
	eMenu_Aroma_Set_Time,
	eMenu_Aroma_Set_Hour,
	eMenu_Aroma_Set_Minutes,
	eMenu_Aroma_Set_AM_PM,
	eMenu_Aroma_Set_Day_Of_Week,
	eMenu_Aroma_Set_Events,
	eMenu_Aroma_Set_Event_Item,
	eMenu_Aroma_Set_Event_Start,
	eMenu_Aroma_Set_Event_Start_Hour,
	eMenu_Aroma_Set_Event_Start_Minute,
	eMenu_Aroma_Set_Event_Start_Day_Of_Week,
	eMenu_Aroma_Set_Event_Start_AM_PM,
	eMenu_Aroma_Set_Event_End,
	eMenu_Aroma_Set_Event_End_Hour,
	eMenu_Aroma_Set_Event_End_Minute,
	eMenu_Aroma_Set_Event_End_Day_Of_Week,
	eMenu_Aroma_Set_Event_End_AM_PM,
	eMenu_Aroma_Set_Event_Work_Level,
	eMenu_Aroma_Set_Event_Days_Prompt,
	eMenu_Aroma_Set_Event_Days,
	eMenu_Aroma_Set_Event_Level,
	eMenu_Aroma_Set_Event_Fan,
	eMenu_Aroma_Machine_Info,
	eMenu_Aroma_Machine_Info_Name,
	eMenu_Aroma_Machine_Info_FWRev,
	eMenu_Aroma_Machine_Info_Unit_SN,
	eMenu_Aroma_Machine_Info_PCB_SN,
	eMenu_Aroma_Machine_Info_MAC,
	eMenu_Aroma_Machine_Info_SSID,
	eMenu_Aroma_Reset_Events,
	eMenu_Aroma_Reset_Events_Confirm,
	eMenu_Aroma_WiFi_Reset,
	eMenu_Aroma_WiFi_Reset_Confirm,
	eMenu_Aroma_Override_Cancel,
	eMenu_Aroma_Override_Cancel_Confirm
} eMenuType;

typedef enum
{
	MarqueeState_Aroma,
	MarqueeState_AdHoc,
	MarqueeState_Override,
	MarqueeState_Event_Running,
	MarqueeState_Next_Event,
	MarqueeState_No_Event,
	MarqueeState_Updating_FW,
	MarqueeState_Vacation_Hold
} eMarqueeState;

typedef struct
{
  int hour;
  int minute;
  int dayOfWeek = 7;
  int workLevel;
  int eventLevel;
  bool colonFlag;
  bool hourFlag;
  bool minuteFlag;
  bool dayOfWeekFlag;
  eMarqueeState preMarqueeState = MarqueeState_Aroma;
} TimePreviousState;


void Menu_Display();

extern TimePreviousState prevTimeState;
extern bool showEventDisplayOnce;

#endif menu_h