#pragma once
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include <SdFat.h>
#include <Arduino.h>
#include "CONFIG.h"
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifdef USE_SDFAT
  extern SdFat SD_CARD;
#else
  extern SdFatSdio SD_CARD;
#endif
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#pragma pack(push,1)
typedef struct
{
  bool testSucceeded;
  uint32_t numPasses;
  uint32_t writeSpeed;
  uint32_t readSpeed;
/*
  uint32_t maxReadLatency;
  uint32_t minReadLatency;
  uint32_t avgReadLatency;  

  uint32_t maxWriteLatency;
  uint32_t minWriteLatency;
  uint32_t avgWriteLatency;  
*/  
} SDSpeedResults;
#pragma pack(pop)
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern void showSDStats(const SDSpeedResults& info, Stream* showIn);
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class SDInit
{
  public:
    static bool sdInitFlag;
    static bool sdInitResult;
    static bool InitSD();
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class FileUtils
{
  public:

    static int CountFiles(const String& dirName);
    static void SendToStream(Stream* s, const String& fileName);
    static void SendToStream(Stream& s, const String& fileName);
    static void printFilesNames(const String& dirName, bool recursive, Stream* outStream);
    static String getFileName(SdFile &f);
    static void deleteFile(const String& fileName);
    static uint32_t getFileSize(const String& fileName);
  
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern SDSpeedResults sdSpeed;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
