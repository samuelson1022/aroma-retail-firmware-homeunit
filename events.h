
#ifndef events_h
#define events_h

#define SECONDSINDAY  86400

int GetNextPendingEvent(int* piDOW);
int HasEventPending(int* piEvent, int* piDOW);
uint8_t CheckBetweenTimes(int RTCSeconds, int TimeA_Seconds, int TimeB_Seconds);
void Schedule_Event(void);
int HasEventActive(int*);
int8_t Get_DOW_Mask(int);



#endif
