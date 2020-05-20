#pragma once
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "SdFat.h"
#include <Arduino.h>
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern SdFatSdio SD_CARD;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#pragma pack(push,1)
typedef struct
{
  bool testSucceeded;
  uint32_t numPasses;
  uint32_t writeSpeed;
  uint32_t readSpeed;

  uint32_t maxReadLatency;
  uint32_t minReadLatency;
  uint32_t avgReadLatency;  

  uint32_t maxWriteLatency;
  uint32_t minWriteLatency;
  uint32_t avgWriteLatency;  
  
} SDSpeedResults;
#pragma pack(pop)
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
class SDInit
{
  public:
    static bool sdInitFlag;
    static bool sdInitResult;
    static bool InitSD();
    static SDSpeedResults MeasureSpeed(Stream* intermediateResultsOutStream);
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
