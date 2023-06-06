#include "ota.h"

FWUpdate_Struct_Type FWUpdate_Rec = { 0, 0, 0 };

eFWUpdate_State_Type FWUpdate_State = eFWUpdate_Idle;

void Boot_System()
{
//  Print_VLCD(1, 1, 0, szBlankLine);
//  Print_VLCD(2, 1, 0, szBlankLine);
//  printString(1, 1, szBlankLine);
//  printString(2, 1, szBlankLine);
  //Clear the WD refresh flag
  //This is the recommended method for soft reset
  //WD_Refresh_Enable = 0;
  /*NVIC_SystemReset(); */
}

//void FlashPacket(int iPacketIndex, char* pPacket, int iPigTail)
//{
//  // Decode a hex-encoded packet into sector-sized byte buffer
//  // When the end of the flash sector is filled, write it to the update region at the proper sector offset
//  int i;
//  int iSectorBaseAddress;
//  int iSectorOffset = (iPacketIndex * (PACKETSIZEBYTES / 2)) % SECTORSIZEBYTES;
//  char szByte[3] = { 0, 0, 0 };
//  for (i = 0; i < PACKETSIZEBYTES / 2; i++)
//  {
//    memcpy(szByte, pPacket + (2*i), 2);
//    SectorBuf[i + iSectorOffset] = strtol(szByte, NULL, 16);
//  }
//  if (iPigTail || ((iPacketIndex + 1) % (SECTORSIZEBYTES / (PACKETSIZEBYTES / 2))) == 0) 
//  {
//    // We have filled a sectorbuf or pigtail sector with data. 
//    // Now write it to the proper sector of flash.
//    iSectorBaseAddress = (iPacketIndex / (SECTORSIZEBYTES / (PACKETSIZEBYTES / 2))) * SECTORSIZEBYTES;
//    MY_FLASH_SetSectorAddrs(UPDATE_STAGING_ADDRESS + iSectorBaseAddress);
//    MY_FLASH_EraseSector();
//    MY_FLASH_WriteN(0, SectorBuf, SECTORSIZEBYTES / 8, eDATA_TYPE_64);
//
//    db_printf("FlashPacket: Writing sector to 0x%0X %i\r\n", UPDATE_STAGING_ADDRESS + iSectorBaseAddress, iPigTail);
//  }
//}

void FWUpdate_GetFWInfo()
{
  int iTimeout = API_Get_FWUpdateInfo(StateCB.szUnitSerial);
  if (iTimeout)
  {
    FWUpdate_State = eFWUpdate_Idle;
  }
  else
  {
    FWUpdate_Rec.PacketIndex = 0;
    FWUpdate_State = eFWUpdate_GetFWPackets;
  }
}

void FWUpdate_GetFWPackets()
{
  int iTimeout = API_Get_FWUpdatePackets(StateCB.szUnitSerial);
}

void listDir(fs::FS &fs, const char *dirname, uint8_t levels)
{
  Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if (!root)
  {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory())
  {
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file)
  {
    if (file.isDirectory())
    {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels)
      {
        listDir(fs, file.name(), levels - 1);
      }
    }
    else
    {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

void readFile(fs::FS &fs, const char *path)
{
  Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if (!file || file.isDirectory())
  {
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.print("Read from file: ");
  while (file.available())
  {
    Serial.write(file.read());
    delayMicroseconds(100);
  }
}

void writeFile(fs::FS &fs, const char *path, const char *message)
{
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file)
  {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message))
  {
    Serial.println("File written");
  }
  else
  {
    Serial.println("Write failed");
  }
}

void updateFromFS()
{
  File updateBin = SPIFFS.open("/update.bin");
  if (updateBin)
  {
    if (updateBin.isDirectory())
    {
      Serial.println("Directory error");
      updateBin.close();
      return;
    }

    size_t updateSize = updateBin.size();

    if (updateSize > 0)
    {
      Serial.println("Starting update");
      performUpdate(updateBin, updateSize);
    }
    else
    {
      Serial.println("Error, archivo vacío");
    }

    updateBin.close();

    // whe finished remove the binary from sd card to indicate end of the process
    //fs.remove("/update.bin");
  }
  else
  {
    Serial.println("no such binary");
  }
}

void performUpdate(Stream &updateSource, size_t updateSize)
{
 if  (Update.begin(updateSize))
  {
    size_t written = Update.writeStream(updateSource);
    if (written == updateSize)
    {
      Serial.println("Writes : " + String(written) + " successfully");
    }
    else
    {
      Serial.println("Written only : " + String(written) + "/" + String(updateSize) + ". Retry?");
    }
    if (Update.end())
    {
      Serial.println("OTA finished!");
      if (Update.isFinished())
      {
        Serial.println("Restart ESP device!");
        ESP.restart();
      }
      else
      {
        Serial.println("OTA not fiished");
      }
    }
    else
    {
      Serial.println("Error occured #: " + String(Update.getError()));
    }
  }
  else
  {
    Serial.println("Cannot beggin update");
  }
}
