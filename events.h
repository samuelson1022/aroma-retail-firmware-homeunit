
#ifndef events_h
#define events_h

#define SECONDSINDAY  86400

typedef struct
{
  uint8_t   Hours;
  uint8_t   Minutes;
  uint8_t   Seconds;
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

int GetNextPendingEvent(int* piDOW);
int HasEventPending(int* piEvent, int* piDOW);
uint8_t CheckBetweenTimes(int RTCSeconds, int TimeA_Seconds, int TimeB_Seconds);
void Schedule_Event(void);
int HasEventActive(int*);
int8_t Get_DOW_Mask(int);



#endif
