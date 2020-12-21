#pragma once
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#include <SdFat.h>
#include <Arduino.h>
#include "CONFIG.h"
#include "TinyVector.h"
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

    static int CountFiles(const String& dirName); // кол-во файлов в папке
    static void SendToStream(Stream* s, const String& fileName); // вывод файла в поток
    static void SendToStream(Stream& s, const String& fileName); // вывод файла в поток
    static void printFilesNames(const String& dirName, bool recursive, Stream* outStream); // вывод имён файлов в поток
    static String getFileName(SdFile &f); // получение имени файла
    static void deleteFile(const String& fileName); // удаление файла
    static uint32_t getFileSize(const String& fileName); // получение размера файла
    static void saveEthalon(uint8_t channel, bool isUpMove, Vector<uint32_t>& list); // сохранение эталона в файл
    static bool isEthalonExists(uint8_t channel, bool isUpMove); // проверка на существование эталона
  
};
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern SDSpeedResults sdSpeed;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------
