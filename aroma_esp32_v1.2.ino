#include "WiFi.h"
#include "HTTPClient.h"
#include "API_config.h"
#include "flash.h"
#include "rtc.h"
#include "pump_control.h"
#include "ota.h"
#include "button.h"
#include "menu.h"
#include "lcd.h"

#define BUTTON_MODE 35
#define BUTTON_SET 32
#define BUTTON_UP 34
#define BUTTON_DOWN 33
#define Pump_CTRL_Pin 4
#define AIR_FLOW 19
#define FAN_PWM 26
#define STATUS_LED 25


char* wifi_network_ssid = "CU_F68b";        //Replace with Your own network SSID
char* wifi_network_password = "hk2yjznx1";  //Replace with Your own network PASSWORD

const char* soft_ap_ssid = "Aroma-100-230422"; /*Create a SSID for ESP32 Access Point*/
const char* soft_ap_password = "";             /*Create Password for ESP32 AP*/

WiFiServer Server(333);
WiFiClient RemoteClient;

unsigned long lastTime = 0;
unsigned long timerDelay = 25000;
unsigned long lastrtcTime = 0;
unsigned long rtcDelay = 300 * 1000;
unsigned long milliLastTime = 0;
unsigned long buttonLastTime = 0;
unsigned long lcdLastTime = 0;
unsigned long lastpumpTime = 0;
unsigned long pumpDelay = 1000;


unsigned long lastfwTime = 0;
unsigned long fwDelay = 99000;
int RTC_Adjust_Secs = 0;

#define RTCADJSECSLIMIT 3

int bVacation_Hold = 0;
String serverName = "http://3.101.17.195:8080/api/v1";

// ---------------------------------------- API -------------------------------------
TaskHandle_t lcdTask;
TaskHandle_t buttonTask;


void setup() {
  Serial.begin(115200);
  delay(1000);

  init_rtc();
  Init_SCB();
  lcdInit();
  init_gpio();

  // Load the flash data
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS Mount Failed");
    return;
  }
  SPIFFS.format();
  listDir(SPIFFS, "/", 0);

  //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(
    buttonOperation, /* Task function. */
    "buttonTask",    /* name of task. */
    10000,           /* Stack size of task */
    NULL,            /* parameter of the task */
    1,               /* priority of the task */
    &buttonTask,     /* Task handle to keep track of created task */
    1);              /* pin task to core 0 */
  delay(500);

  //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(
    lcdOperation, /* Task function. */
    "LCD Task",    /* name of task. */
    20000,           /* Stack size of task */
    NULL,            /* parameter of the task */
    1,               /* priority of the task */
    &lcdTask,     /* Task handle to keep track of created task */
    0);              /* pin task to core 0 */
  delay(500);

  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(soft_ap_ssid, soft_ap_password); /*Configuring ESP32 access point SSID and password*/
  Serial.println(WiFi.softAPIP());
  WiFi.begin(StateCB.szSSID, StateCB.szPW);    /*Connecting to Defined Access point*/

  int count = 0;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
    count++;
    if (count == 40) break;
  }
  Serial.println(WiFi.localIP()); /*Printing IP address of Connected network*/
}

String data;
HTTPClient http;
void loop() {
  if (Server.hasClient()) {
    if (RemoteClient.connected()) {
      Serial.println("Connection rejected");
      Server.available().stop();
    } else {
      Serial.println("Connection accepted");
      RemoteClient = Server.available();
    }

    int maxloops = 0;

    //wait for the server's reply to become available
    while (!RemoteClient.available() && maxloops < 1000) {
      maxloops++;
      delay(1);  //delay 1 msec
    }

    if (RemoteClient.available() > 0) {
      //read back one line from the server
      String line = RemoteClient.readStringUntil('\r');
      Serial.println(line);
      if (line.indexOf("WHOIS") != -1) {
        char szSerialStr[128];
        sprintf(szSerialStr, "1.36 | %s\r\nOK\r\n", StateCB.szUnitSerial);
        // RemoteClient.print("1.36 | Aroma-100-230420\r\nOK\r\n");
        RemoteClient.print(szSerialStr);
      }

      if (line.indexOf("getssids") != -1) {
        int n = WiFi.scanNetworks();
        String ssidlist = "";
        for (int i = 0; i < n; ++i) {
          ssidlist += "+CWLAP:(";
          ssidlist += String(WiFi.encryptionType(i)) + ",";
          ssidlist += "\"" + WiFi.SSID(i) + "\",";
          ssidlist += String(WiFi.RSSI(i)) + ",";
          ssidlist += "\"" + String(WiFi.softAPmacAddress()) + "\",";
          ssidlist += String(WiFi.channel(i)) + ")\r\n";
          if (i > 6) break;
        }
        ssidlist += "OK\r\n";
        Serial.println(ssidlist);
        RemoteClient.print(ssidlist);
        WiFi.scanDelete();
      }

      if (line.indexOf("setssid") != -1) {
        String temp1 = line.substring(line.indexOf('"') + 1);
        String ssid = temp1.substring(0, temp1.indexOf('"'));
        String temp2 = line.substring(line.indexOf(",\"") + 2);
        String password = temp2.substring(0, temp2.indexOf('"'));


        ssid.toCharArray(StateCB.szSSID, ssid.length() + 1);
        password.toCharArray(StateCB.szPW, password.length() + 1);
        Serial.println("SSID: " + String(StateCB.szSSID));
        Serial.println("pass: " + String(StateCB.szPW));
        WiFi.begin(StateCB.szSSID, StateCB.szPW);
        int count = 0;
        while (WiFi.status() != WL_CONNECTED) {
          Serial.print(".");
          delay(100);
          count++;
          if (count == 40) break;
        }
        if (WiFi.status() == WL_CONNECTED)
          Serial.println("WiFi Connected!");
        else
          Serial.println("Wrong credential");

        Update_SCB();
      }
    }
  }


  if ((millis() - lastpumpTime) > pumpDelay) {
    if (!bVacation_Hold) {
      Handle_Pump_State();
    }
    lastpumpTime = millis();
  }

  // if ((millis() - lastfwTime) > fwDelay) {
  //   switch (FWUpdate_State) {
  //     case eFWUpdate_Idle:
  //       Serial.println("eFWUpdate_Idle");
  //       break;
  //     case eFWUpdate_GetFWInfo:
  //       FWUpdate_GetFWInfo();
  //       break;
  //     case eFWUpdate_GetFWPackets:
  //       FWUpdate_GetFWPackets();
  //       break;
  //   }
  //   lastfwTime = millis();
  // }

  if ((millis() - lastTime) > timerDelay) {
    if (WiFi.status() == WL_CONNECTED) {


      // Ack
      int iTimeout;
      int iAction = 0;

      RTC_Adjust_Secs++;
      if (RTC_Adjust_Secs >= RTCADJSECSLIMIT) {
        // Request clock change
        iTimeout = API_Get_Status(StateCB.szUnitSerial, 1, &iAction);
        RTC_Adjust_Secs = 0;
      } else {
        // Get status, no clock change
        iTimeout = API_Get_Status(StateCB.szUnitSerial, 0, &iAction);
      }

      //iTimeout = API_Get_Status(serverName, 0, &iAction);
      if (iTimeout == 200) {
        Serial.println("-----------OK--------------");
      } else Serial.println("Not OK");

      //checking action
      switch (iAction) {
        case APIActionCode_Idle:
          Serial.println("APIActionCode_Idle");
          break;

        case APIActionCode_GetEvents:
          Serial.println("APIActionCode_GetEvents");
          bVacation_Hold = 0;
          End_Pump_Control();
          API_Get_Events(StateCB.szUnitSerial);
          break;

        case APIActionCode_FriendlyName:
          Serial.println("APIActionCode_FriendlyName");
          API_Get_FriendlyName(StateCB.szUnitSerial);
          break;

        case APIActionCode_Override:
          Serial.println("APIActionCode_Override");
          bVacation_Hold = 0;
          API_Get_Override(StateCB.szUnitSerial);
          break;

        case APIActionCode_OverrideCancel:
          Serial.println("APIActionCode_OverrideCancel");
          bVacation_Hold = 0;
          API_Get_OverrideCancel(StateCB.szUnitSerial);
          break;

        case APIActionCode_GetFWUpdate:
          if (FWUpdate_State == eFWUpdate_Idle) {
            FWUpdate_State = eFWUpdate_GetFWInfo;
          }
          break;

        default:
          Serial.println("default case");
          iAction = APIActionCode_Idle;
          break;
      }

      // Free resources
      http.end();
    } else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}

void init_gpio() {
  pinMode(STATUS_LED, OUTPUT);
  digitalWrite(STATUS_LED, LOW);

  pinMode(BUTTON_MODE, INPUT);
  pinMode(BUTTON_SET, INPUT);
  pinMode(BUTTON_UP, INPUT);
  pinMode(BUTTON_DOWN, INPUT);
  pinMode(Pump_CTRL_Pin, OUTPUT);
  pinMode(AIR_FLOW, INPUT);
  pinMode(FAN_PWM, OUTPUT);

  digitalWrite(Pump_CTRL_Pin, LOW);
  digitalWrite(FAN_PWM, LOW);
}

void buttonOperation(void* pvParameters) {
  for (;;) {
    if (millis() != milliLastTime) {
      milliLastTime = millis();
      ButtonDownMsec++;
    }

    if ((millis() - buttonLastTime) > 10) {
      buttonLastTime = millis();
      Button_Scan();
    }
    if (bRequest_Handle_Button_Press) {
      bRequest_Handle_Button_Press = 0;
      Handle_Button_Press();
    }
  }
}

void lcdOperation(void* pvParameters) {
  for (;;) {
    // if ((millis() - lcdLastTime) > 500) {
    //   lcdLastTime = millis();
      Menu_Display();
      delay(500);
    // }
  }
}