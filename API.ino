//#include "rtc.h"
#include <HTTPUpdate.h>

int API_Get_Status(char* szSerial, int bUpdateRTCFlag, int* piAction)
{
  char szResponse[128];
  char szAPI[128];
  int iNextTimeSecs = 0;
  char* token;
  int iAction;
  int iHours, iMinutes, iSeconds, iDOW;

//  Serial.println("API_Get_Status");

  // Get status from API
  sprintf(szAPI, "%s/%s/%s", SERVER, API_STATUS, szSerial);
//  Serial.println(szAPI);
  http.begin(szAPI);
  *piAction = 0;

  // http.begin(serverPath.c_str());
  int httpCode = http.GET();

  if (httpCode > 0) {
    String payload = http.getString();
    payload.remove(0, 9);
    payload.toCharArray(szResponse, 128);
//    Serial.println("HTTP code for GET Status = " + String(httpCode));
//    Serial.println(payload);
  } else {
//    Serial.println("Error on HTTP ACK request");
  }

  if (httpCode == 200)
  {
    // Get RTC clock contents
    token = strtok(szResponse, "|"); if (token == NULL) return 0;
//    Serial.println(token);
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

//    Serial.print("iAction = " + String(iAction));
//    Serial.print("iHours = " + String(iHours));


    if (iAction > 0) printf("Status API Action = %i\r\n", iAction);
    // Got the full payload, set clock if requested
    if (bUpdateRTCFlag)
    {
      myRTC.setHour(iHours);
      myRTC.setMinute(iMinutes);
      myRTC.setSecond(iSeconds);
      myRTC.setDoW(iDOW);      
    }
    // All good, pass the action code through
    *piAction = iAction;
  }
  else
  {
//    Serial.println("Status request timed out\r\n");
//    Serial.println(httpCode);
  }
  return httpCode;
}


int API_Get_Events(char* szSerial)
{
  char szResponse[350];
  char szAPI[128];
  int i;
  char* token;
  uint8_t doWTemp, count;

  // Events
  //STHR|STMIN|EHR|EMIN|DOWBM|W|P|F|..(4 more sets)

  // Get status from API
  
  sprintf(szAPI, "%s/%s/%s", SERVER, API_EVENTS, szSerial);
//  Serial.println(szAPI);
  http.begin(szAPI);

  int httpCode = http.GET();

  if (httpCode > 0) {
    String payload = http.getString();    
    payload.remove(0, 9);
    payload.toCharArray(szResponse, 350);
//    Serial.println("HTTP code for GET Events = "+ String(httpCode));
//    Serial.println(payload);
  } else {
//    Serial.println("Error on HTTP ACK request");
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
      doWTemp = atoi(token);
      token = strtok(NULL, "|"); if (token == NULL) return 0;
      StateCB.Events[i].WorkSeconds = atoi(token);
      token = strtok(NULL, "|"); if (token == NULL) return 0;
      StateCB.Events[i].PauseSeconds = atoi(token);
      token = strtok(NULL, "|"); if (token == NULL) return 0;
      StateCB.Events[i].FanSpeed = atoi(token);

      StateCB.Events[i].DayOfWeek = 0x00;
      for (count = 0; count < 7; count ++) {
        StateCB.Events[i].DayOfWeek = StateCB.Events[i].DayOfWeek | (((doWTemp >> count) & 0x01) << (6 - count));
      }
      Serial.println(StateCB.Events[i].DayOfWeek);
    }
    // Store the SCB
    Update_SCB();

    // Got the full payload, acknowledge it
//    Serial.println(szSerial);
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
//  Serial.println(szAPI);

  http.begin(szAPI);
  int httpCode = http.GET();
  String payload = http.getString();
  payload.toCharArray(szResponse, 256);
//  Serial.println("HTTP code for GET Ack = "+ String(httpCode));
//  Serial.println(payload);

  if (httpCode > 0) {
  } else {
//    Serial.println("Error on HTTP ACK request");
  }

  return httpCode;
}


int API_Get_FriendlyName(char* szSerial)
{
  //arapi.aromaretail.com/api/v1/ar_FriendlyName/Aroma-003-210825
  char szResponse[256];
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
//    Serial.println("HTTP code for GET FriendlyName = "+ String(httpCode));
//    Serial.println(payload);
    int iTransfer = strlen(szResponse) > (sizeof(StateCB.szUnitName) - 1) ? sizeof(StateCB.szUnitName) - 1 : strlen(szResponse);
    strncpy(StateCB.szUnitName, szResponse, iTransfer);
    StateCB.szUnitName[iTransfer] = 0x0;
    Update_SCB();

    // Got the full payload, acknowledge it
    API_Get_ACK(szSerial, API_FRIENDLYNAME);
  } else {
//    Serial.println("Error on HTTP API_Get_FriendlyName request");
  }
  return httpCode;
}


int API_Get_Override(char* szSerial)
{
  char szResponse[128];
  char szMesg[64];
  char szAPI[128];
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
//    Serial.println("HTTP code for GET Override = "+ String(httpCode));
//    Serial.println(payload);
  } else {
//    Serial.println("Error on HTTP ACK request");
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

int API_Get_FWUpdateInfo(char* szSerial)
{
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    httpUpdate.rebootOnUpdate(false); // remove automatic update

    Serial.println(F("Update start now!"));

    t_httpUpdate_return ret = httpUpdate.update(client, "3.26.104.241", 3000, "/firmware/httpUpdateNew.bin");

    switch (ret) {
      case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
        Serial.println(F("Retry in 10secs!"));
        delay(10000); // Wait 10secs
        break;

      case HTTP_UPDATE_NO_UPDATES:
        Serial.println("HTTP_UPDATE_NO_UPDATES");
        break;

      case HTTP_UPDATE_OK:
        Serial.println("HTTP_UPDATE_OK");
        delay(1000); // Wait a second and restart
        API_Get_ACK(StateCB.szUnitSerial, API_FWPACKET);
        ESP.restart();
        break;
    }
  }
}
