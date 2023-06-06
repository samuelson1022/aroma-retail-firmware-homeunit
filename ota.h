
#ifndef ota_h
#define ota_h

#include "FS.h"
#include "SPIFFS.h"
#include <Update.h>

#define BOOTLOADER_ADDRESS        0x08000000
#define APPLICATION_FIRMWARE_ADDRESS  0x08002000
#define UPDATE_STAGING_ADDRESS      0x08020800
#define SCBSTARTADDRESS         0x0803F800
#define SCBMAXSIZEBYTES         0x800
#define SCBHEADERID           0x7388
#define EVENTLISTSIZE         5
#define LEVELLISTSIZE         10
#define PACKETSIZEBYTES         128
#define SECTORSIZEBYTES         2048

typedef struct
{
  int     ByteCount;
  uint16_t uCRC;
  int     PacketIndex;
} FWUpdate_Struct_Type;

typedef enum
{
  eFWUpdate_Idle,
  eFWUpdate_GetFWInfo,
  eFWUpdate_GetFWPackets
} eFWUpdate_State_Type;

extern eFWUpdate_State_Type FWUpdate_State;
extern FWUpdate_Struct_Type FWUpdate_Rec;
void listDir(fs::FS &fs, const char *dirname, uint8_t levels);
void updateFromFS();
void performUpdate(Stream &updateSource, size_t updateSize);
void FWUpdate_GetFWInfo();
void FWUpdate_GetFWPackets();

#endif
