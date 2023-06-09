
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

RTCTimeStructType SetTime;
RTCDateStructType SetDate;

RTCTimeStructType GetTime;
RTCDateStructType GetDate;

RTCTimeStructType CurrentTime;
RTCDateStructType CurrentDate;

int init_rtc(void);
int rtc_set(int hr, int min, int sec, int wday);
int rtc_get(RTCTimeStructType *SetTime, RTCDateStructType *SetDate);

#endif
