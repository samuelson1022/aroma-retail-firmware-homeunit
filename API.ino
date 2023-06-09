//#include "rtc.h"

int API_Get_Status(char* szSerial, int bUpdateRTCFlag, int* piAction)
{
  char szResponse[128];
  char szAPI[64];
  char szTicks[16];
  int iNextTimeSecs = 0;
  char* token;
  int iAction;
  int iHours, iMinutes, iSeconds, iDOW;

  Serial.println("API_Get_Status");

  // Get status from API
  sprintf(szAPI, "%s/%s/%s", SERVER, API_STATUS, szSerial);
  Serial.println(szAPI);
  http.begin(szAPI);
  // String serverPath = serverName + "/ar_Status/Aroma-100-230420";
  *piAction = 0;

  // http.begin(serverPath.c_str());
  int httpCode = http.GET();

  if (httpCode > 0) {
    String payload = http.getString();
    payload.remove(0, 9);
    payload.toCharArray(szResponse, 128);
    Serial.println("HTTP code for GET Status = " + String(httpCode));
    Serial.println(payload);
  } else {
    Serial.println("Error on HTTP ACK request");
  }

  if (httpCode == 200)
  {
    // Get RTC clock contents
    token = strtok(szResponse, "|"); if (token == NULL) return 0;
    Serial.println(token);
    iAction = atoi(token);
    token = strtok(NULL, "|"); if (token == NULL) return 0;
    iHours = atoi(token);
    token = strtok(NULL, "|"); if (token == NULL) return 0;
    iMinutes = atoi(token);
    token = strtok(NULL, "|"); if (token == NULL) return 0;
    iSeconds = atoi(token);
    token = strtok(NULL, "|"); if (token == NULL) return 0;
    iDOW = atoi(token);
    iDOW = iDOW < 1 ? 1 : iDOW;    //Must be between 1 and 7, 1=Mon
    token = strtok(NULL, "|"); if (token == NULL) return 0;
    iNextTimeSecs = atoi(token);
    iNextTimeSecs = iNextTimeSecs < 15 ? 15 : iNextTimeSecs;   // minimum 15 seconds
    RequestStatusMOD = iNextTimeSecs * 1000;   // convert to msec ticks

    Serial.print("iAction = " + String(iAction));
    Serial.print("iHours = " + String(iHours));


    if (iAction > 0) printf("Status API Action = %i\r\n", iAction);
    // Got the full payload, set clock if requested
    if (bUpdateRTCFlag)
    {
      rtc_get(&GetTime, &GetDate); //get time and date
      int dowDiff = iDOW - GetDate.WeekDay;
      int iDate = GetDate.Date;
      if(dowDiff < 0) {
        if(iDate > 7) iDate = iDate + dowDiff;
        else iDate = iDate + 7 + dowDiff;
      } else if (dowDiff > 0) {
        if(iDate > 20) iDate -7 + dowDiff;
        else iDate = iDate + dowDiff;
      }
      rtc_set(iHours, iMinutes, iSeconds, iDate, GetDate.Month + 1, GetDate.Year);
      
    }
    // All good, pass the action code through
    *piAction = iAction;
  }
  else
  {
    Serial.println("Status request timed out\r\n");
  }
  return httpCode;
}


int API_Get_Events(char* szSerial)
{
  char szResponse[256];
  char szMesg[64];
  char szAPI[64];
  int i;
  char* token;

  // Events
  //STHR|STMIN|EHR|EMIN|DOWBM|W|P|F|..(4 more sets)

  // Get status from API
  
  sprintf(szAPI, "%s/%s/%s", SERVER, API_EVENTS, szSerial);
  Serial.println(szAPI);
  http.begin(szAPI);

  // String serverPath = serverName + "/ar_GetEvents/Aroma-100-230420";

  // http.begin(serverPath.c_str());
  int httpCode = http.GET();

  if (httpCode > 0) {
    String payload = http.getString();    
    payload.remove(0, 9);
    payload.toCharArray(szResponse, 256);
    Serial.println("HTTP code for GET Events = "+ String(httpCode));
    Serial.println(payload);
  } else {
    Serial.println("Error on HTTP ACK request");
  }

  if (httpCode == 200)
  {
    printf("Event response: %s\r\n", szResponse);
    // Parse event parameters
    //STHR|STMIN|EHR|EMIN|DOWBM|W|P|F|..(4 more sets)
    for (i = 0 ; i < 5 ; i++)
    {
      if (i == 0)
      {
        // 1st pass
        token = strtok(szResponse, "|"); if (token == NULL) break;
      }
      else
      {
        // continuing to parse
        token = strtok(NULL, "|"); if (token == NULL) break;
      }
      StateCB.Events[i].StartHour = atoi(token);
      token = strtok(NULL, "|"); if (token == NULL) break;
      StateCB.Events[i].StartMinute = atoi(token);
      token = strtok(NULL, "|"); if (token == NULL) return 0;
      StateCB.Events[i].EndHour = atoi(token);
      token = strtok(NULL, "|"); if (token == NULL) return 0;
      StateCB.Events[i].EndMinute = atoi(token);
      token = strtok(NULL, "|"); if (token == NULL) return 0;
      StateCB.Events[i].DayOfWeek = atoi(token);
      token = strtok(NULL, "|"); if (token == NULL) return 0;
      StateCB.Events[i].WorkSeconds = atoi(token);
      token = strtok(NULL, "|"); if (token == NULL) return 0;
      StateCB.Events[i].PauseSeconds = atoi(token);
      token = strtok(NULL, "|"); if (token == NULL) return 0;
      StateCB.Events[i].FanSpeed = atoi(token);
    }
    // Store the SCB
    Update_SCB();

    // Got the full payload, acknowledge it
    Serial.println(szSerial);
    API_Get_ACK(szSerial, API_EVENTS);
  }
  return httpCode;
}


int API_Get_ACK(char* szSerial, char* ACK_Type)
{
  char szResponse[128];
  char szAPI[128];
  sprintf(szAPI, "%s/%s/%s/%s", SERVER, API_ACK, szSerial, ACK_Type);

  //String serverPath = serverName + "/ar_ACK/Aroma-005-220221/ar_GetEvents";
  Serial.println(szAPI);

  http.begin(szAPI);
  int httpCode = http.GET();
  String payload = http.getString();
  payload.toCharArray(szResponse, 256);
  Serial.println("HTTP code for GET Ack = "+ String(httpCode));
  Serial.println(payload);

  if (httpCode > 0) {
  } else {
    Serial.println("Error on HTTP ACK request");
  }

  return httpCode;
}


int API_Get_FriendlyName(char* szSerial)
{
  //arapi.aromaretail.com/api/v1/ar_FriendlyName/Aroma-003-210825
  char szResponse[256];
  char szMesg[64];
  char szAPI[128];
  int i;
  char* token;

  sprintf(szAPI, "%s/%s/%s", SERVER, API_FRIENDLYNAME, szSerial);
  //int iTimeout = API_Get_Request(szAPI, szResponse);

  http.begin(szAPI);
  int httpCode = http.GET();

  if (httpCode > 0) {
    String payload = http.getString();
    payload.remove(0, 9);
    payload.toCharArray(szResponse, 256);
    Serial.println("HTTP code for GET FriendlyName = "+ String(httpCode));
    Serial.println(payload);
    int iTransfer = strlen(szResponse) > (sizeof(StateCB.szUnitName) - 1) ? sizeof(StateCB.szUnitName) - 1 : strlen(szResponse);
    strncpy(StateCB.szUnitName, szResponse, iTransfer);
    StateCB.szUnitName[iTransfer] = 0x0;
    Update_SCB();

    // Got the full payload, acknowledge it
    API_Get_ACK(szSerial, API_FRIENDLYNAME);
  } else {
    Serial.println("Error on HTTP API_Get_FriendlyName request");
  }
  return httpCode;
}


int API_Get_Override(char* szSerial)
{
  char szResponse[128];
  char szMesg[64];
  char szAPI[128];
  char szTicks[16];
  int iAction = 0;
  int iNextTimeSecs = 0;
  char* token;

  sprintf(szAPI, "%s/%s/%s", SERVER, API_OVERRIDE, szSerial);
  //int iTimeout = API_Get_Request(szAPI, szResponse);

  http.begin(szAPI);
  int httpCode = http.GET();

  if (httpCode > 0) {
    String payload = http.getString();
    payload.remove(0, 9);
    payload.toCharArray(szResponse, 128);
    Serial.println("HTTP code for GET Override = "+ String(httpCode));
    Serial.println(payload);
  } else {
    Serial.println("Error on HTTP ACK request");
  }
  
  //printf("Override requested, Timeout = %i\r\n", httpCode);
  if (httpCode == 200)
  {
    // Parse override parameters
    token = strtok(szResponse, "|"); if (token == NULL) return 0;
    OverrideControl.Start_Hour = atoi(token);
    token = strtok(NULL, "|"); if (token == NULL) return 0;
    OverrideControl.Start_Minute = atoi(token);
    token = strtok(NULL, "|"); if (token == NULL) return 0;
    OverrideControl.End_Hour = atoi(token);
    token = strtok(NULL, "|"); if (token == NULL) return 0;
    OverrideControl.End_Minute = atoi(token);
    token = strtok(NULL, "|"); if (token == NULL) return 0;
    OverrideControl.Work = atoi(token);
    token = strtok(NULL, "|"); if (token == NULL) return 0;
    OverrideControl.Pause = atoi(token);
    token = strtok(NULL, "|"); if (token == NULL) return 0;
    OverrideControl.Fan_Speed = atoi(token);
    token = strtok(NULL, "|");
    OverrideControl.HoldFlag = (token == NULL) ? 0 : atoi(token);

    // Got the full payload, acknowledge it
    API_Get_ACK(szSerial, API_OVERRIDE);

    // On successful acknowledge, start the override
    Init_Pump_State();  //Forces new program parameters to load
    OverrideControl.State = (OverrideControl.HoldFlag) ? eControl_Hold : eControl_Timer;
    OverrideControl.Pending = 1;
    if (OverrideControl.HoldFlag)
    {
      printf("Override enabled in HOLD mode\r\n");
    }
    else
    {
      sprintf(szMesg,
              "SCHEDULED %i:%i %i secs on %i secs off ending %i:%i\r",
              OverrideControl.Start_Hour,
              OverrideControl.Start_Minute,
              OverrideControl.Work,
              OverrideControl.Pause,
              OverrideControl.End_Hour,
              OverrideControl.End_Minute);
      printf("%s\r\n", szMesg);
    }
  }
  return httpCode;
}


int API_Get_OverrideCancel(char* szSerial)
{
  int httpCode;
    
  // Cancel override
  printf("Override canceled\r\n");
  End_Pump_Control();
  httpCode = API_Get_ACK(szSerial, API_OVERRIDECANCEL);
  return httpCode;
}
