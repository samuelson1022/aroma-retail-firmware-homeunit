
#ifndef rtc_h
#define rtc_h

#include "ESP32Time.h"

ESP32Time rtc;  // offset in seconds GMT+1

typedef struct
{
  uint8_t   Hours;
  uint8_t   Minutes;
  uint8_t   Seconds;
  uint8_t   TimeFormat;
  uint32_t  SubSeconds;
  uint32_t  SecondFraction;
  uint32_t  DayLightSaving;
  uint32_t  StoreOperation;
}
RTCTimeStructType;

typedef struct
{
  uint8_t   WeekDay;
  uint8_t   Month;
  uint8_t   Date;
  int   Year;
}
RTCDateStructType;

typedef enum
{
	eTimeSetDisabled,
	eTimeSetHours,
	eTimeSetMinutes,
	eTimeSetAMPM,
	eTimeSetDayOfWeek
}
eTimeSetState;
typedef struct
{
	eTimeSetState	TimeSetState;
	int8_t			TimeSetEnableDOW;
	int8_t			TimeSetRow;
	int8_t			TimeSetCol;
	int8_t			TimeSetFlash;
	int8_t*			pTimeSetHours;
	int8_t*			pTimeSetMinutes;
	int8_t*			pDayOfWeek;
	char			szLabel[6];
}
TimeSetStructType;

RTCTimeStructType SetTime;
RTCDateStructType SetDate;

RTCTimeStructType GetTime;
RTCDateStructType GetDate;

RTCTimeStructType CurrentTime;
RTCDateStructType CurrentDate;

int init_rtc(void);
int rtc_set(int hr, int min, int sec, int day, int mon, int year);
int rtc_get(RTCTimeStructType *SetTime, RTCDateStructType *SetDate);

#endif
