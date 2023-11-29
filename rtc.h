#ifndef rtc_h
#define rtc_h

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
  uint32_t   Year;
}
RTCDateStructType;

RTCTimeStructType GetTime;
RTCDateStructType GetDate;
RTCTimeStructType CurrentTime;
RTCDateStructType CurrentDate;

int rtc_get();
void SaveToRTC();

#endif
